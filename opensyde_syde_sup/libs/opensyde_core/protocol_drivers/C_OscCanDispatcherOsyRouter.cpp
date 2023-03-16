//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       CAN dispatcher for routing of legacy protocols (implementation)

   Implementation of C_CAN_Dispatcher interface to provide "CAN"-like communication via
   an openSYDE server that provides routing of legacy protocols.
   The application instances this class instead of using a "physical" CAN dispatcher and
   registers it with the legacy protocol driver (e.g. KEFEX protocol driver).
   Thus the requests and responses send and received by the protocol driver will not be sent
   directly on CAN but be communicated to the openSYDE server node using the routing services.

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.hpp"

#include "stwerrors.hpp"

#include "C_OscCanDispatcherOsyRouter.hpp"

#include "TglTime.hpp"
#include "TglUtils.hpp"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */

using namespace stw::errors;
using namespace stw::opensyde_core;
using namespace stw::can;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

/* -- Global Variables ---------------------------------------------------------------------------------------------- */

/* -- Module Global Variables --------------------------------------------------------------------------------------- */

/* -- Module Global Function Prototypes ----------------------------------------------------------------------------- */

/* -- Implementation ------------------------------------------------------------------------------------------------ */

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default constructor

   \param[in]  orc_OsyProtocol   openSYDE protocol instance to use for communication for the "last mile"

   Initialize instance
*/
//----------------------------------------------------------------------------------------------------------------------
C_OscCanDispatcherOsyRouter::C_OscCanDispatcherOsyRouter(C_OscProtocolDriverOsy & orc_OsyProtocol) :
   C_CanDispatcher(0U),
   mrc_OsyProtocol(orc_OsyProtocol),
   mu8_RoutingChannel(0U),
   mu32_FilterId(0U),
   mu32_FilterMask(0xFFFFFFFFU)
{
   orc_OsyProtocol.InitializeTunnelCanMessage(&C_OscCanDispatcherOsyRouter::mh_OsyTunnelCanMessageReceived, this);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default constructor

   Initialize instance

   \param[in]  orc_OsyProtocol  openSYDE protocol driver to communicate over
   \param[in]  ou8_CommChannel  communication driver channel
*/
//----------------------------------------------------------------------------------------------------------------------
C_OscCanDispatcherOsyRouter::C_OscCanDispatcherOsyRouter(C_OscProtocolDriverOsy & orc_OsyProtocol,
                                                         const uint8_t ou8_CommChannel) :
   C_CanDispatcher(ou8_CommChannel),
   mrc_OsyProtocol(orc_OsyProtocol),
   mu8_RoutingChannel(0U),
   mu32_FilterId(0U),
   mu32_FilterMask(0xFFFFFFFFU)
{
   orc_OsyProtocol.InitializeTunnelCanMessage(&C_OscCanDispatcherOsyRouter::mh_OsyTunnelCanMessageReceived, this);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Sets the routing configuration

   The functions sets the parameter for routing configuration, but does not activate it!
   Call CAN_Init to activate the routing.

   \param[in]     ou8_RoutingChannel      Output channel for legacy routing on node
   \param[in]     ou32_FilterId           Filter id for all relevant IDs
                                          The MSB is the flag for 11 Bit (= 0) or 29 Bit (= 1)
                                          Bits 29 and 30 are reserved (shall be set to zero)
   \param[in]     ou32_FilterMask         Mask that is applied to both the received CAN frame identifiers and the
                                          FilterId before comparison.
                                          Bits 29, 30 and 31 are reserved (shall be set to zero)
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OscCanDispatcherOsyRouter::SetFilterParameters(const uint8_t ou8_RoutingChannel, const uint32_t ou32_FilterId,
                                                      const uint32_t ou32_FilterMask)
{
   this->mu8_RoutingChannel = ou8_RoutingChannel;
   this->mu32_FilterId = ou32_FilterId;
   this->mu32_FilterMask = ou32_FilterMask;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Initialization of CAN legacy routing

   Parameters shall be set with SetFilterParameters first before calling this function.

   \return
   C_NO_ERR   request sent, positive response received
   C_TIMEOUT  expected response not received within timeout
   C_NOACT    could not put request in Tx queue ...
   C_CONFIG   no transport protocol installed
   C_WARN     error response (negative response code placed in *opu8_NrCode)
   C_RD_WR    unexpected content in response (here: wrong routine identifier ID)
   C_COM      communication driver reported error
*/
//----------------------------------------------------------------------------------------------------------------------
//lint -e{8001}  //name of function dictated by base class
int32_t C_OscCanDispatcherOsyRouter::CAN_Init(void)
{
   return this->mrc_OsyProtocol.OsySetTunnelCanMessages(this->mu8_RoutingChannel, this->mu32_FilterId,
                                                        this->mu32_FilterMask);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Function has no functionality

   Not bitrate can be set by this dispatcher. Use CAN_Init without parameter for initialization of the
   CAN legacy routing.

   \param[in]     os32_BitrateKBitS         Bitrate

   \return
   C_NOACT
*/
//----------------------------------------------------------------------------------------------------------------------
//lint -e{8001}  //name of function dictated by base class
int32_t C_OscCanDispatcherOsyRouter::CAN_Init(const int32_t os32_BitrateKBitS)
{
   (void)os32_BitrateKBitS;
   return C_NOACT;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Stop of CAN legacy routing

   \return
   C_NO_ERR   request sent, positive response received
   C_TIMEOUT  expected response not received within timeout
   C_NOACT    could not put request in Tx queue ...
   C_CONFIG   no transport protocol installed
   C_WARN     error response (negative response code placed in *opu8_NrCode)
   C_RD_WR    unexpected content in response (here: wrong routine identifier ID)
   C_COM      communication driver reported error
*/
//----------------------------------------------------------------------------------------------------------------------
//lint -e{8001}  //name of function dictated by base class
int32_t C_OscCanDispatcherOsyRouter::CAN_Exit(void)
{
   return this->mrc_OsyProtocol.OsyStopTunnelCanMessages();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Resets the current CAN session

   \return
   C_NO_ERR   request sent, positive response received
   C_TIMEOUT  expected response not received within timeout
   C_NOACT    could not put request in Tx queue ...
   C_CONFIG   no transport protocol installed
   C_WARN     error response (negative response code placed in *opu8_NrCode)
   C_RD_WR    unexpected content in response (here: wrong routine identifier ID)
   C_COM      communication driver reported error
*/
//----------------------------------------------------------------------------------------------------------------------
//lint -e{8001}  //name of function dictated by base class
int32_t C_OscCanDispatcherOsyRouter::CAN_Reset(void)
{
   int32_t s32_Return;

   s32_Return = this->CAN_Exit();

   if (s32_Return == C_NO_ERR)
   {
      s32_Return = this->CAN_Init();
   }

   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Sending a CAN message with legacy routing

   \param[in]  orc_Message     CAN message to send

   \return
   C_NO_ERR   request sent, positive response received
   C_TIMEOUT  expected response not received within timeout
   C_RANGE    CAN message invalid (RTR bit set; ID out of range; DLC out of range)
   C_NOACT    could not put request in Tx queue ...
   C_CONFIG   no transport protocol installed
   C_WARN     error response (negative response code placed in *opu8_NrCode)
   C_RD_WR    unexpected content in response (here: wrong data identifier ID)
   C_COM      communication driver reported error
*/
//----------------------------------------------------------------------------------------------------------------------
//lint -e{8001}  //name of function dictated by base class
int32_t C_OscCanDispatcherOsyRouter::CAN_Send_Msg(const stw::can::T_STWCAN_Msg_TX & orc_Message)
{
   return this->mrc_OsyProtocol.OsySendCanMessage(this->mu8_RoutingChannel, orc_Message);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Check timestamp basis of driver

   Return system time in micro seconds as the driver sees it.
   Can be used by an application to have a time basis for putting the timestamps of received messages into a
    timeframe.

   \param[out]  oru64_SystemTimeUs  system time in micro seconds

   \return
   C_NO_ERR   information read
*/
//----------------------------------------------------------------------------------------------------------------------
//lint -e{8001}  //name of function dictated by base class
int32_t C_OscCanDispatcherOsyRouter::CAN_Get_System_Time(uint64_t & oru64_SystemTimeUs) const
{
   oru64_SystemTimeUs = stw::tgl::TglGetTickCountUs();

   return C_NO_ERR;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Read the first received message from the list

   Functions shall read and return one asynchronous received tunneled CAN message.

   \param[out]  orc_Message  read message

   \return
   C_NO_ERR   message read
   C_WARN     no message read
*/
//----------------------------------------------------------------------------------------------------------------------
//lint -e{8001}  //name of function dictated by base class
int32_t C_OscCanDispatcherOsyRouter::m_CAN_Read_Msg(stw::can::T_STWCAN_Msg_RX & orc_Message)
{
   int32_t s32_Return = C_WARN;

   mrc_OsyProtocol.Cycle();

   if (this->mc_AsyncMessages.size() > 0)
   {
      std::list<stw::can::T_STWCAN_Msg_RX>::iterator c_ItItem;

      // Get the first message
      c_ItItem = this->mc_AsyncMessages.begin();
      orc_Message = *c_ItItem;
      // Remove it from the list
      this->mc_AsyncMessages.erase(c_ItItem);

      s32_Return = C_NO_ERR;
   }

   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   In C_OscProtocolDriverOsy registered function for receiving asynchronous messages

   \param[in]     opv_Instance     Pointer to the instance of C_OscCanDispatcherOsyRouter
   \param[in]     ou8_Channel      Channel the message was received on
   \param[in]     orc_CanMessage   Async received CAN message
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OscCanDispatcherOsyRouter::mh_OsyTunnelCanMessageReceived(void * const opv_Instance, const uint8_t ou8_Channel,
                                                                 const stw::can::T_STWCAN_Msg_RX & orc_CanMessage)
{
   //lint -e{9079}  This class is the only one which registers itself at the caller of this function. It must match.
   C_OscCanDispatcherOsyRouter * const pc_Dispatcher =
      reinterpret_cast<C_OscCanDispatcherOsyRouter *>(opv_Instance);

   tgl_assert(pc_Dispatcher != NULL);
   if (pc_Dispatcher != NULL)
   {
      pc_Dispatcher->m_OsyTunnelCanMessageReceived(ou8_Channel, orc_CanMessage);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   In C_OscProtocolDriverOsy registered function for receiving asynchronous messages

   \param[in]     ou8_Channel      Channel the message was received on
   \param[in]     orc_CanMessage   Async received CAN message
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OscCanDispatcherOsyRouter::m_OsyTunnelCanMessageReceived(const uint8_t ou8_Channel,
                                                                const stw::can::T_STWCAN_Msg_RX & orc_CanMessage)
{
   if (ou8_Channel == mu8_RoutingChannel)
   {
      this->mc_AsyncMessages.push_back(orc_CanMessage);
   }
}
