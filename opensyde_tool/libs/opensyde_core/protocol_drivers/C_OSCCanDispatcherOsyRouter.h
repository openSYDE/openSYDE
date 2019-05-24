//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       CAN dispatcher for routing of legacy protocols (header)

   See cpp file for detailed description

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_OSCCANDISPATCHEROSYROUTER_H
#define C_OSCCANDISPATCHEROSYROUTER_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <list>

#include "stwtypes.h"

#include "stw_can.h"

#include "CCANDispatcher.h"
#include "C_OSCProtocolDriverOsy.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw_opensyde_core
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_OSCCanDispatcherOsyRouter :
   public stw_can::C_CAN_Dispatcher
{
public:
   C_OSCCanDispatcherOsyRouter(C_OSCProtocolDriverOsy & orc_OsyProtocol);
   C_OSCCanDispatcherOsyRouter(C_OSCProtocolDriverOsy & orc_OsyProtocol, const stw_types::uint8 ou8_CommChannel);

   void SetFilterParameters(const stw_types::uint8 ou8_RoutingChannel, const stw_types::uint32 ou32_FilterId,
                            const stw_types::uint32 ou32_FilterMask);

   virtual stw_types::sint32 CAN_Init(void);
   virtual stw_types::sint32 CAN_Init(const stw_types::sint32 os32_BitrateKBitS);
   virtual stw_types::sint32 CAN_Exit(void);
   virtual stw_types::sint32 CAN_Reset(void);
   virtual stw_types::sint32 CAN_Send_Msg(const stw_can::T_STWCAN_Msg_TX & orc_Message);
   virtual stw_types::sint32 CAN_Get_System_Time(stw_types::uint64 & oru64_SystemTimeUs) const;

protected:
   virtual stw_types::sint32 m_CAN_Read_Msg(stw_can::T_STWCAN_Msg_RX & orc_Message);

private:
   //this class can not be copied:
   C_OSCCanDispatcherOsyRouter(const C_OSCCanDispatcherOsyRouter & orc_Source);
   C_OSCCanDispatcherOsyRouter & operator = (const C_OSCCanDispatcherOsyRouter & orc_Source);

   static void mh_OsyTunnelCanMessageReceived(void * const opv_Instance, const stw_types::uint8 ou8_Channel,
                                              const stw_can::T_STWCAN_Msg_RX & orc_CanMessage);
   void m_OsyTunnelCanMessageReceived(const stw_types::uint8 ou8_Channel,
                                      const stw_can::T_STWCAN_Msg_RX & orc_CanMessage);

   //lint -e{1725} //class cannot be copied; no problem
   C_OSCProtocolDriverOsy & mrc_OsyProtocol;
   stw_types::uint8 mu8_RoutingChannel; ///< Output channel for legacy routing on node
   stw_types::uint32 mu32_FilterId;
   stw_types::uint32 mu32_FilterMask;

   std::list<stw_can::T_STWCAN_Msg_RX> mc_AsyncMessages;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif
