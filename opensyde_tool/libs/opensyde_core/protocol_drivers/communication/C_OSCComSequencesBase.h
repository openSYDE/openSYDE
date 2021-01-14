//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Basic sequences to communicate with flashloader protocols. (header)

   See cpp file for detailed description

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_OSCCOMSEQUENCESBASE_H
#define C_OSCCOMSEQUENCESBASE_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <vector>

#include "stwtypes.h"

#include "C_OSCSystemDefinition.h"
#include "C_OSCComDriverFlash.h"

#include "CSCLString.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw_opensyde_core
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_OSCComSequencesBase
{
public:
   C_OSCComSequencesBase(const bool oq_RoutingActive);
   virtual ~C_OSCComSequencesBase(void);

   virtual stw_types::sint32 Init(C_OSCSystemDefinition & orc_SystemDefinition,
                                  const stw_types::uint32 ou32_ActiveBusIndex,
                                  const std::vector<stw_types::uint8> & orc_ActiveNodes,
                                  stw_can::C_CAN_Dispatcher * const opc_CanDispatcher,
                                  C_OSCIpDispatcher * const opc_IpDispatcher);

   bool IsInitialized(void) const;

   bool GetNodeIndex(const stw_opensyde_core::C_OSCProtocolDriverOsyNode & orc_ServerId,
                     stw_types::uint32 & oru32_NodeIndex) const;

   bool IsAtLeastOneOpenSydeNodeActive(void) const;
   bool IsAtLeastOneStwFlashloaderNodeActive(void) const;
   bool IsAtLeastOneStwFlashloaderNodeActiveOnLocalBus(void) const;

   bool IsEthToEthRoutingNecessary(const stw_types::uint32 ou32_RouterNodeIndex) const;
   stw_types::uint32 GetMinimumFlashloaderResetWaitTime(
      const C_OSCComDriverFlash::E_MinimumFlashloaderResetWaitTimeType oe_Type) const;
   stw_types::sint32 GetMinimumFlashloaderResetWaitTime(
      const C_OSCComDriverFlash::E_MinimumFlashloaderResetWaitTimeType oe_Type,
      const C_OSCProtocolDriverOsyNode & orc_ServerId, stw_types::uint32 & oru32_TimeValue) const;

protected:
   C_OSCComDriverFlash * const mpc_ComDriver;

   //class configuration parameters:
   C_OSCSystemDefinition * mpc_SystemDefinition;
   stw_types::uint32 mu32_ActiveBusIndex;
   std::vector<stw_types::uint8> mc_ActiveNodes;
   std::vector<stw_types::uint8> mc_TimeoutNodes; // Flag if a node had a timeout and is not reachable

   bool mq_OpenSydeDevicesActive;
   bool mq_StwFlashloaderDevicesActive;
   bool mq_StwFlashloaderDevicesActiveOnLocalBus;
   C_OSCProtocolDriverOsyNode mc_StwFlashloaderDeviceOnLocalBus;

   bool m_IsNodeReachable(const stw_types::uint32 ou32_NodeIndex) const;
   virtual stw_types::sint32 m_XflReportProgress(const stw_types::uint8 ou8_Progress,
                                                 const stw_scl::C_SCLString & orc_Text) = 0;

private:
   static stw_types::sint32 mh_MyXflReportProgress(void * const opv_Instance, const stw_types::uint8 ou8_Progress,
                                                   const stw_scl::C_SCLString & orc_Text);
   stw_types::sint32 m_MyXflReportProgress(const stw_types::uint8 ou8_Progress, const stw_scl::C_SCLString & orc_Text);

   C_OSCComSequencesBase(const C_OSCComSequencesBase & orc_Source);               //not implemented -> prevent copying
   C_OSCComSequencesBase & operator = (const C_OSCComSequencesBase & orc_Source); //not implemented -> prevent
                                                                                  // assignment

   bool m_IsAtLeastOneOpenSydeNodeActive(void) const;
   bool m_IsAtLeastOneStwFlashloaderNodeActive(void) const;
   bool m_IsAtLeastOneStwFlashloaderNodeOnLocalBusActive(
      C_OSCProtocolDriverOsyNode & orc_StwFlashloaderDeviceOnLocalBus)
   const;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif
