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

#include "stwtypes.hpp"

#include "C_OscSystemDefinition.hpp"
#include "C_OscComDriverFlash.hpp"

#include "C_SclString.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_core
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_OscComSequencesBase
{
public:
   explicit C_OscComSequencesBase(const bool oq_RoutingActive, const bool oq_UpdateRoutingMode);
   virtual ~C_OscComSequencesBase(void);

   virtual int32_t Init(C_OscSystemDefinition & orc_SystemDefinition, const uint32_t ou32_ActiveBusIndex,
                        const std::vector<uint8_t> & orc_ActiveNodes,
                        stw::can::C_CanDispatcher * const opc_CanDispatcher, C_OscIpDispatcher * const opc_IpDispatcher,
                        C_OscSecurityPemDatabase * const opc_SecurityPemDb);

   bool IsInitialized(void) const;

   bool GetNodeIndex(const stw::opensyde_core::C_OscProtocolDriverOsyNode & orc_ServerId,
                     uint32_t & oru32_NodeIndex) const;

   bool IsAtLeastOneOpenSydeNodeActive(void) const;
   bool IsAtLeastOneStwFlashloaderNodeActive(void) const;
   bool IsAtLeastOneStwFlashloaderNodeActiveOnLocalBus(void) const;

   bool IsEthToEthRoutingNecessary(const uint32_t ou32_RouterNodeIndex) const;
   uint32_t GetMinimumFlashloaderResetWaitTime(const C_OscComDriverFlash::E_MinimumFlashloaderResetWaitTimeType oe_Type)
   const;
   int32_t GetMinimumFlashloaderResetWaitTime(const C_OscComDriverFlash::E_MinimumFlashloaderResetWaitTimeType oe_Type,
                                              const C_OscProtocolDriverOsyNode & orc_ServerId,
                                              uint32_t & oru32_TimeValue) const;

protected:
   C_OscComDriverFlash * const mpc_ComDriver;

   //class configuration parameters:
   C_OscSystemDefinition * mpc_SystemDefinition;
   uint32_t mu32_ActiveBusIndex;
   std::vector<uint8_t> mc_ActiveNodes;
   std::vector<uint8_t> mc_TimeoutNodes; // Flag if a node had a timeout and is not reachable

   bool mq_OpenSydeDevicesActive;
   bool mq_StwFlashloaderDevicesActive;
   bool mq_StwFlashloaderDevicesActiveOnLocalBus;
   C_OscProtocolDriverOsyNode mc_StwFlashloaderDeviceOnLocalBus;

   bool m_IsNodeReachable(const uint32_t ou32_NodeIndex) const;
   virtual int32_t m_XflReportProgress(const uint8_t ou8_Progress, const stw::scl::C_SclString & orc_Text) = 0;

private:
   static int32_t mh_MyXflReportProgress(void * const opv_Instance, const uint8_t ou8_Progress,
                                         const stw::scl::C_SclString & orc_Text);
   int32_t m_MyXflReportProgress(const uint8_t ou8_Progress, const stw::scl::C_SclString & orc_Text);

   C_OscComSequencesBase(const C_OscComSequencesBase & orc_Source);               //not implemented -> prevent copying
   C_OscComSequencesBase & operator = (const C_OscComSequencesBase & orc_Source); //not implemented -> prevent
                                                                                  // assignment

   bool m_IsAtLeastOneOpenSydeNodeActive(void) const;
   bool m_IsAtLeastOneStwFlashloaderNodeActive(void) const;
   bool m_IsAtLeastOneStwFlashloaderNodeOnLocalBusActive(
      C_OscProtocolDriverOsyNode & orc_StwFlashloaderDeviceOnLocalBus)
   const;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
