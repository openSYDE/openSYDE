//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       C_CAN implementation

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

#include "CSCLString.h"
#include "TGLTime.h"
#include "stwerrors.h"
#include "stwtypes.h"
#include "CCAN.h"

//----------------------------------------------------------------------------------------------------------------------

using namespace stw_types;
using namespace stw_errors;
using namespace stw_scl;
using namespace stw_can;
using namespace stw_tgl;

/* -- Defines ------------------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

/* -- Global Variables ---------------------------------------------------------------------------------------------- */

/* -- Function Prototypes ------------------------------------------------------------------------------------------- */

/* -- Implementation ------------------------------------------------------------------------------------------------ */

//----------------------------------------------------------------------------------------------------------------------

void C_CAN::m_InitClass(const uint8 ou8_CommChannel)
{
   msn_Socket = -1;
   ms32_RxTimeout = 0;
   mu32_RXID = CAN_RX_ID_INVALID;
   mc_CanIfName.PrintFormatted("can%u", (unsigned int)ou8_CommChannel);
}

//----------------------------------------------------------------------------------------------------------------------

C_CAN::C_CAN(const uint8 ou8_CommChannel) :
   C_CAN_Dispatcher(ou8_CommChannel)
{
   m_InitClass(ou8_CommChannel);
}

//----------------------------------------------------------------------------------------------------------------------

C_CAN::C_CAN(void) :
   C_CAN_Dispatcher()
{
   m_InitClass(0);
}

//----------------------------------------------------------------------------------------------------------------------

C_CAN::~C_CAN(void)
{
   if (msn_Socket >= 0)
   {
      (void)this->CAN_Exit();
   }
}

//----------------------------------------------------------------------------------------------------------------------

sint32 C_CAN::CAN_Exit(void)
{
   sint32 s32_return = C_NO_ERR;

   // check if the socket is correct bind
   if (msn_Socket >= 0)
   {
      // close previously initialized socket
      (void) close(msn_Socket);
   }
   else
   {
      s32_return = C_CONFIG;
   }
   return s32_return;
}

//----------------------------------------------------------------------------------------------------------------------

sint32 C_CAN::CAN_Reset(void)
{
   (void)CAN_Exit();
   return CAN_Init();
}

//----------------------------------------------------------------------------------------------------------------------

sint32 C_CAN::CAN_Init(const stw_scl::C_SCLString & orc_InterfaceName, const sint32 os32_RxTimeout)
{
   sint32 s32_retval = C_NO_ERR;
   struct sockaddr_can t_addr;
   struct ifreq t_ifr;

   (void)memcpy(t_ifr.ifr_name, orc_InterfaceName.c_str(), sizeof (t_ifr.ifr_name));

   // ************************* RxTx Socket configuration*************************
   // open socket
   // function shall create an unbound socket in a communications domain, and return a file descriptor
   // CAN_RAW = Raw protocol of PF_CAN, applicable to socket type SOCK_RAW
   msn_Socket = socket(PF_CAN, SOCK_RAW, CAN_RAW);
   if (msn_Socket >= 0)
   {
      // SIOCGIFINDEX is an macro, which retrieve the interface index of the interface into t_ifr.ifr_index
      if (ioctl(msn_Socket, SIOCGIFINDEX, &t_ifr) >= 0)
      {
         /*
         Disable default receive filter on this raw socket.

         CAN filter definition
         A CAN raw filter list with elements of struct can_filter can be installed with setsockopt. This
         list is used upon reception of CAN frames to decide whether the bound socket will receive a
         freame. An empty filter list can also be defined using optlen = 0, which is recommended for write-only
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
         struct can_filter at_canFilter[1];

         if (mu32_RXID == CAN_RX_ID_INVALID)
         {
            at_canFilter[0].can_id = 0;   // don't care
            at_canFilter[0].can_mask = 0; // ALL frames will pass this filter
         }
         else
         {
            at_canFilter[0].can_id = mu32_RXID;
            at_canFilter[0].can_mask = (mu32_RXID & 0x1fffffff) ? CAN_EFF_MASK : CAN_SFF_MASK;
         }
         if (setsockopt(msn_Socket, SOL_CAN_RAW, CAN_RAW_FILTER, &at_canFilter, sizeof (at_canFilter)) == 0)
         {
            t_addr.can_family = AF_CAN;
            // set the received index from the ioctl!!!
            t_addr.can_ifindex = t_ifr.ifr_ifindex;
            // bind socket
            // The function shall assign a local socket address address to a socket identified by descriptor socket
            if (bind(msn_Socket, (struct sockaddr *)&t_addr, sizeof(t_addr)) < 0)
            {
               // close socket
               (void)CAN_Exit();
               // set return value
               s32_retval = C_CONFIG;
            }
            ms32_RxTimeout = os32_RxTimeout;
            mc_CanIfName = orc_InterfaceName;
            // TODO: Use SetCommChannel to set cahnnel nmber in CanBase class
         }
         else
         {
            // close socket
            (void)CAN_Exit();
            // set return value
            s32_retval = C_CONFIG;
         }
      }
      else
      {
         // close socket
         (void)CAN_Exit();
         // set return value
         s32_retval = C_CONFIG;
      }
   }
   else
   {
      // error occured while creating new Socket
      s32_retval = C_CONFIG;
   }
   return s32_retval;
}
//----------------------------------------------------------------------------------------------------------------------

sint32 C_CAN::CAN_Init(void)
{
   sint32 s32_Error = C_NO_ERR;

   s32_Error = this->CAN_Init(mc_CanIfName, 0);

   return s32_Error;
}
//----------------------------------------------------------------------------------------------------------------------

sint32 C_CAN::CAN_Init(const sint32 os32_Bitrate)
{
   (void)os32_Bitrate;
   return CAN_Init();
}
//----------------------------------------------------------------------------------------------------------------------

sint32 C_CAN::CAN_Send_Msg(const T_STWCAN_Msg_TX & orc_Message)
{
   sint32 s32_Error;
   sintn sn_Ret;

   if (this->msn_Socket >= 0)
   {
      struct pollfd t_PollFd;
      struct can_frame t_Frame;

      // prepare CAN frame
      t_Frame.can_id =  CAN_ERR_MASK & orc_Message.u32_ID;
      if (orc_Message.u8_XTD != 0)
      {
         t_Frame.can_id |= CAN_EFF_FLAG;
      }
      if (orc_Message.u8_RTR != 0)
      {
         t_Frame.can_id |= CAN_RTR_FLAG;
      }
      t_Frame.can_dlc = orc_Message.u8_DLC;
      for (int i = 0; i < 8; ++i)
      {
         t_Frame.data[i] = orc_Message.au8_Data[i];
      }

      // check if socket is ready (sleep max. 50ms)
      t_PollFd.fd = this->msn_Socket;
      t_PollFd.events = POLLOUT;
      sn_Ret = poll(&t_PollFd, 1, 50);
      if (sn_Ret > 0)
      {
         if ((t_PollFd.revents & POLLOUT) != 0)
         {
            sn_Ret = write(this->msn_Socket, &t_Frame, sizeof(t_Frame));
            s32_Error = (sn_Ret ==  sizeof(t_Frame)) ? C_NO_ERR : C_COM;
         }
         else
         {
            // Unexpected error, should never happen
            s32_Error = C_COM;
         }
      }
      else if (sn_Ret == 0)
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

sint32 C_CAN::m_CAN_Read_Msg(T_STWCAN_Msg_RX & orc_Message)
{
   return CAN_Read_Msg_Timeout((uint32)ms32_RxTimeout, orc_Message);
}

//----------------------------------------------------------------------------------------------------------------------

sint32 C_CAN::CAN_Read_Msg_Timeout(const uint32 ou32_MaxWaitTimeMS, T_STWCAN_Msg_RX & orc_Message)
{
   sint32 s32_Error;
   sintn sn_Ret;

   if (this->msn_Socket >= 0)
   {
      struct pollfd t_PollFd;

      t_PollFd.fd = this->msn_Socket;
      t_PollFd.events = POLLIN;
      sn_Ret = poll(&t_PollFd, 1, ou32_MaxWaitTimeMS);
      if (sn_Ret > 0)
      {
         s32_Error = C_COM; // indicates that an unexpected error occurred, should never happen
         if ((t_PollFd.revents & POLLIN) != 0)
         {
            struct can_frame t_Frame;

            sn_Ret = read(this->msn_Socket, &t_Frame, sizeof(t_Frame));
            if (sn_Ret == sizeof(t_Frame))
            {
               // new message received
               orc_Message.u64_TimeStamp = TGL_GetTickCountUS();                    // better than ioctl(SIOCGSTAMP)
                                                                                    // since
                                                                                    // this is synchronous to system
                                                                                    // time
               orc_Message.u32_ID = t_Frame.can_id & CAN_ERR_MASK;                  // get received ID
               orc_Message.u8_XTD = ((t_Frame.can_id & CAN_EFF_FLAG) == 0) ? 0 : 1; // get extended flag
               orc_Message.u8_RTR = ((t_Frame.can_id & CAN_RTR_FLAG) == 0) ? 0 : 1; // get RTR flag
               orc_Message.u8_DLC = t_Frame.can_dlc;                                // get data length code
               for (int i = 0; i < 8; ++i)
               {
                  orc_Message.au8_Data[i] = t_Frame.data[i];
               }
               s32_Error = C_NO_ERR;
            }
         }
      }
      else if (sn_Ret == 0)
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

sint32 C_CAN::CAN_Get_System_Time(uint64 & oru64_SystemTime) const
{
   oru64_SystemTime = TGL_GetTickCountUS();
   return C_NO_ERR;
}

//----------------------------------------------------------------------------------------------------------------------

void C_CAN::SetLimitRXID(const uint32 ou32_LimitRXID)
{
   mu32_RXID = ou32_LimitRXID;
}

//----------------------------------------------------------------------------------------------------------------------

uint32 C_CAN::GetLimitRXID(void) const
{
   return mu32_RXID;
}
