//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       C_Can implementation

   Experimental CAN driver for MPC5200 Linux SocketCAN.

   Only for demonstration purposes. Not fully tested and implemented. Not really portable.

   \copyright   Copyright 2012 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <cstdio>
#include <iostream>
#include <iomanip>
#include <errno.h>
#include <cstring>
#include <unistd.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <net/if.h>
#include <linux/can.h>
#include <linux/can/raw.h>
#include <poll.h>

#include "stwerrors.hpp"
#include "stwtypes.hpp"
#include "C_SclString.hpp"
#include "TglTime.hpp"
#include "C_Can.hpp"

//----------------------------------------------------------------------------------------------------------------------

using namespace stw::errors;
using namespace stw::scl;
using namespace stw::can;
using namespace stw::tgl;

/* -- Defines ------------------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

/* -- Global Variables ---------------------------------------------------------------------------------------------- */

/* -- Function Prototypes ------------------------------------------------------------------------------------------- */

/* -- Implementation ------------------------------------------------------------------------------------------------ */

//----------------------------------------------------------------------------------------------------------------------

void C_Can::m_InitClass(const uint8_t ou8_CommChannel)
{
   ms32_Socket = -1;
   ms32_RxTimeout = 0;
   mu32_RxId = CAN_RX_ID_INVALID;
   mc_CanIfName.PrintFormatted("can%d", ou8_CommChannel);
}

//----------------------------------------------------------------------------------------------------------------------

C_Can::C_Can(const uint8_t ou8_CommChannel) :
   C_CanDispatcher(ou8_CommChannel)
{
   m_InitClass(ou8_CommChannel);
}

//----------------------------------------------------------------------------------------------------------------------

C_Can::C_Can(void) :
   C_CanDispatcher()
{
   m_InitClass(0);
}

//----------------------------------------------------------------------------------------------------------------------

C_Can::~C_Can(void)
{
   if (ms32_Socket >= 0)
   {
      (void)this->C_Can::CAN_Exit();
   }
}

//----------------------------------------------------------------------------------------------------------------------

int32_t C_Can::CAN_Exit(void)
{
   int32_t s32_Return = C_NO_ERR;

   // check if the socket is correct bind
   if (ms32_Socket >= 0)
   {
      // close previously initialized socket
      (void) close(ms32_Socket);
   }
   else
   {
      s32_Return = C_CONFIG;
   }
   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------

int32_t C_Can::CAN_Reset(void)
{
   (void)CAN_Exit();
   return CAN_Init();
}

//----------------------------------------------------------------------------------------------------------------------

int32_t C_Can::CAN_Init(const C_SclString & orc_InterfaceName, const int32_t os32_RxTimeout)
{
   int32_t s32_RetVal = C_NO_ERR;
   struct sockaddr_can c_Addr;
   struct ifreq c_IfRequest;

   (void)memcpy(c_IfRequest.ifr_name, orc_InterfaceName.c_str(), sizeof (c_IfRequest.ifr_name));

   // ************************* RxTx Socket configuration*************************
   // open socket
   // function shall create an unbound socket in a communications domain, and return a file descriptor
   // CAN_RAW = Raw protocol of PF_CAN, applicable to socket type SOCK_RAW
   ms32_Socket = socket(PF_CAN, SOCK_RAW, CAN_RAW);
   if (ms32_Socket >= 0)
   {
      // SIOCGIFINDEX is an macro, which retrieve the interface index of the interface into c_IfRequest.ifr_index
      if (ioctl(ms32_Socket, SIOCGIFINDEX, &c_IfRequest) >= 0)
      {
         /*
         Disable default receive filter on this raw socket.

         CAN filter definition
         A CAN raw filter list with elements of struct can_filter can be installed with setsockopt. This
         list is used upon reception of CAN frames to decide whether the bound socket will receive a
         frame. An empty filter list can also be defined using optlen = 0, which is recommended for write-only
         sockets. If the socket was already bound with BIND, the old filter list gets replaced with the new one.
         Be aware that already received, but not read out CAN frames may stay in the socket buffer.

         This filter works as follows:
         A filter matches, when
            <received_can_id> & mask == can_id & mask.
         This also includes the CAN_EFF_FLAG and CAN_RTR_FLAG.
         If this comparison is true, the message will be received by the socket.
         The logic can be inverted with the can_id flag CAN_INV_FILTER:

          if (can_id & CAN_INV_FILTER) {
             if ((received_can_id & can_mask) != ((can_id & ~CAN_INV_FILTER) & can_mask))
                accept-message;
          } else {
             if ((received_can_id & can_mask) == (can_id & can_mask))
                accept-message;
          }

         Multiple filters can be arranged in a filter list and set with Sockopts.
         If one of these filters matches a CAN ID upon reception of a CAN frame, this frame is accepted.

         Example:

         struct can_filter rfilter[4];

         rfilter[0].can_id   = 0x80001234;   // Exactly this EFF frame
         rfilter[0].can_mask = CAN_EFF_MASK; // 0x1FFFFFFFU all bits valid
         rfilter[1].can_id   = 0x123;        // Exactly this SFF frame
         rfilter[1].can_mask = CAN_SFF_MASK; // 0x7FFU all bits valid
         rfilter[2].can_id   = 0x200 | CAN_INV_FILTER; // all, but 0x200-0x2FF
         rfilter[2].can_mask = 0xF00;        // (CAN_INV_FILTER set in can_id)
         rfilter[3].can_id   = 0;            // don't care
         rfilter[3].can_mask = 0;            // ALL frames will pass this filter

         setsockopt(s, SOL_CAN_RAW, CAN_RAW_FILTER, &rfilter, sizeof(rfilter));
         */
         struct can_filter ac_CanFilter[1];

         if (mu32_RxId == CAN_RX_ID_INVALID)
         {
            ac_CanFilter[0].can_id = 0;   // don't care
            ac_CanFilter[0].can_mask = 0; // ALL frames will pass this filter
         }
         else
         {
            ac_CanFilter[0].can_id = mu32_RxId;
            ac_CanFilter[0].can_mask = ((mu32_RxId & 0x1fffffffU) != 0U) ? CAN_EFF_MASK : CAN_SFF_MASK;
         }
         if (setsockopt(ms32_Socket, SOL_CAN_RAW, CAN_RAW_FILTER, &ac_CanFilter, sizeof (ac_CanFilter)) == 0)
         {
            c_Addr.can_family = AF_CAN;
            // set the received index from the ioctl!!!
            c_Addr.can_ifindex = c_IfRequest.ifr_ifindex;
            // bind socket
            // The function shall assign a local socket address address to a socket identified by descriptor socket
            //lint -e{9176}  Side-effect of the POSIX-style API. Match is guaranteed by the API.
            if (bind(ms32_Socket, reinterpret_cast<struct sockaddr *>(&c_Addr), sizeof(c_Addr)) < 0)
            {
               // close socket
               (void)CAN_Exit();
               // set return value
               s32_RetVal = C_CONFIG;
            }
            ms32_RxTimeout = os32_RxTimeout;
            mc_CanIfName = orc_InterfaceName;
            // TODO: Use SetCommChannel to set channel number in CanBase class
         }
         else
         {
            // close socket
            (void)CAN_Exit();
            // set return value
            s32_RetVal = C_CONFIG;
         }
      }
      else
      {
         // close socket
         (void)CAN_Exit();
         // set return value
         s32_RetVal = C_CONFIG;
      }
   }
   else
   {
      // error occurred while creating new Socket
      s32_RetVal = C_CONFIG;
   }
   return s32_RetVal;
}

//----------------------------------------------------------------------------------------------------------------------

int32_t C_Can::CAN_Init(void)
{
   return this->CAN_Init(mc_CanIfName, 0);
}
//----------------------------------------------------------------------------------------------------------------------

int32_t C_Can::CAN_Init(const int32_t os32_BitrateKBitS)
{
   (void)os32_BitrateKBitS;
   return CAN_Init();
}
//----------------------------------------------------------------------------------------------------------------------

int32_t C_Can::CAN_Send_Msg(const T_STWCAN_Msg_TX & orc_Message)
{
   int32_t s32_Error;

   if (this->ms32_Socket >= 0)
   {
      struct pollfd c_PollFd;
      struct can_frame c_Frame;
      int32_t s32_Ret;

      // prepare CAN frame
      c_Frame.can_id =  CAN_ERR_MASK & orc_Message.u32_ID;
      if (orc_Message.u8_XTD != 0)
      {
         c_Frame.can_id |= CAN_EFF_FLAG;
      }
      if (orc_Message.u8_RTR != 0)
      {
         c_Frame.can_id |= CAN_RTR_FLAG;
      }
      c_Frame.can_dlc = orc_Message.u8_DLC;
      for (uint8_t u8_Byte = 0; u8_Byte < 8; ++u8_Byte)
      {
         c_Frame.data[u8_Byte] = orc_Message.au8_Data[u8_Byte];
      }

      // check if socket is ready (sleep max. 50ms)
      c_PollFd.fd = this->ms32_Socket;
      c_PollFd.events = POLLOUT;
      s32_Ret = poll(&c_PollFd, 1, 50);
      if (s32_Ret > 0)
      {
         if ((c_PollFd.revents & POLLOUT) != 0)
         {
            s32_Ret = write(this->ms32_Socket, &c_Frame, sizeof(c_Frame));
            s32_Error = (s32_Ret ==  sizeof(c_Frame)) ? C_NO_ERR : C_COM;
         }
         else
         {
            // Unexpected error, should never happen
            s32_Error = C_COM;
         }
      }
      else if (s32_Ret == 0)
      {
         // timeout
         s32_Error = C_BUSY;
      }
      else
      {
         s32_Error = ((errno == EAGAIN) || (errno == EINTR)) ? C_BUSY : C_COM;
      }
   }
   else
   {
      s32_Error = C_CONFIG;
   }
   return s32_Error;
}

//----------------------------------------------------------------------------------------------------------------------

int32_t C_Can::m_CAN_Read_Msg(T_STWCAN_Msg_RX & orc_Message)
{
   return m_CanReadMsgTimeout(static_cast<uint32_t>(ms32_RxTimeout), orc_Message);
}

//----------------------------------------------------------------------------------------------------------------------

int32_t C_Can::m_CanReadMsgTimeout(const uint32_t ou32_MaxWaitTimeMs, T_STWCAN_Msg_RX & orc_Message) const
{
   int32_t s32_Error;
   int32_t s32_Ret;

   if (this->ms32_Socket >= 0)
   {
      struct pollfd c_PollFd;

      c_PollFd.fd = this->ms32_Socket;
      c_PollFd.events = POLLIN;
      s32_Ret = poll(&c_PollFd, 1, ou32_MaxWaitTimeMs);
      if (s32_Ret > 0)
      {
         s32_Error = C_COM; // indicates that an unexpected error occurred, should never happen
         if ((c_PollFd.revents & POLLIN) != 0)
         {
            struct can_frame c_Frame;
            const ssize_t x_Ret = //lint !e8080 //using type to match library interface
                                  read(this->ms32_Socket, &c_Frame, sizeof(c_Frame));
            if (x_Ret == static_cast<ssize_t>(sizeof(c_Frame)))
            {
               // new message received
               orc_Message.u64_TimeStamp = TglGetTickCountUs(); // better than ioctl(SIOCGSTAMP)
                                                                // since this is synchronous to system time

               orc_Message.u32_ID = c_Frame.can_id & CAN_ERR_MASK;                     // get received ID
               orc_Message.u8_XTD = ((c_Frame.can_id & CAN_EFF_FLAG) == 0U) ? 0U : 1U; // get extended flag
               orc_Message.u8_RTR = ((c_Frame.can_id & CAN_RTR_FLAG) == 0U) ? 0U : 1U; // get RTR flag
               orc_Message.u8_DLC = c_Frame.can_dlc;                                   // get data length code
               for (uint8_t u8_Byte = 0; u8_Byte < 8; ++u8_Byte)
               {
                  orc_Message.au8_Data[u8_Byte] = c_Frame.data[u8_Byte];
               }
               s32_Error = C_NO_ERR;
            }
         }
      }
      else if (s32_Ret == 0)
      {
         // timeout
         s32_Error = C_NOACT;
      }
      else
      {
         s32_Error = ((errno == EAGAIN) || (errno == EINTR)) ? C_NOACT : C_COM;
      }
   }
   else
   {
      s32_Error = C_CONFIG;
   }
   return s32_Error;
}

//----------------------------------------------------------------------------------------------------------------------

int32_t C_Can::CAN_Get_System_Time(uint64_t & oru64_SystemTimeUs) const
{
   oru64_SystemTimeUs = TglGetTickCountUs();
   return C_NO_ERR;
}

//----------------------------------------------------------------------------------------------------------------------

//lint -e{8001} //keep name so this class can serve as a stand-in replacement for pre-existing Windows applications
void C_Can::SetLimitRXID(const uint32_t ou32_LimitRxId)
{
   mu32_RxId = ou32_LimitRxId;
}

//----------------------------------------------------------------------------------------------------------------------

//lint -e{8001} //keep name so this class can serve as a stand-in replacement for pre-existing Windows applications
uint32_t C_Can::GetLimitRXID(void) const
{
   return mu32_RxId;
}
