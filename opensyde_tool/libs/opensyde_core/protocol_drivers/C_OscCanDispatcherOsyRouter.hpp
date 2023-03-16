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

#include "stwtypes.hpp"

#include "stw_can.hpp"

#include "C_CanDispatcher.hpp"
#include "C_OscProtocolDriverOsy.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_core
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_OscCanDispatcherOsyRouter :
   public stw::can::C_CanDispatcher
{
public:
   C_OscCanDispatcherOsyRouter(C_OscProtocolDriverOsy & orc_OsyProtocol);
   C_OscCanDispatcherOsyRouter(C_OscProtocolDriverOsy & orc_OsyProtocol, const uint8_t ou8_CommChannel);

   void SetFilterParameters(const uint8_t ou8_RoutingChannel, const uint32_t ou32_FilterId,
                            const uint32_t ou32_FilterMask);

   //lint -e{8001}  //name of function dictated by base class
   virtual int32_t CAN_Init(void);
   //lint -e{8001}  //name of function dictated by base class
   virtual int32_t CAN_Init(const int32_t os32_BitrateKBitS);
   //lint -e{8001}  //name of function dictated by base class
   virtual int32_t CAN_Exit(void);
   //lint -e{8001}  //name of function dictated by base class
   virtual int32_t CAN_Reset(void);
   //lint -e{8001}  //name of function dictated by base class
   virtual int32_t CAN_Send_Msg(const stw::can::T_STWCAN_Msg_TX & orc_Message);
   //lint -e{8001}  //name of function dictated by base class
   virtual int32_t CAN_Get_System_Time(uint64_t & oru64_SystemTimeUs) const;

protected:
   //lint -e{8001}  //name of function dictated by base class
   virtual int32_t m_CAN_Read_Msg(stw::can::T_STWCAN_Msg_RX & orc_Message);

private:
   //this class can not be copied:
   C_OscCanDispatcherOsyRouter(const C_OscCanDispatcherOsyRouter & orc_Source);
   C_OscCanDispatcherOsyRouter & operator = (const C_OscCanDispatcherOsyRouter & orc_Source);

   static void mh_OsyTunnelCanMessageReceived(void * const opv_Instance, const uint8_t ou8_Channel,
                                              const stw::can::T_STWCAN_Msg_RX & orc_CanMessage);
   void m_OsyTunnelCanMessageReceived(const uint8_t ou8_Channel, const stw::can::T_STWCAN_Msg_RX & orc_CanMessage);

   //lint -e{1725} //class cannot be copied; no problem
   C_OscProtocolDriverOsy & mrc_OsyProtocol;
   uint8_t mu8_RoutingChannel; ///< Output channel for legacy routing on node
   uint32_t mu32_FilterId;
   uint32_t mu32_FilterMask;

   std::list<stw::can::T_STWCAN_Msg_RX> mc_AsyncMessages;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
