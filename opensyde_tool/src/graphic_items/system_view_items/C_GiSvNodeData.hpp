//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Data class to hold node specific data in C_GiSvNodeSyvUpdate.
   \copyright   Copyright 2021 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_GISVNODEDATA_H
#define C_GISVNODEDATA_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "stwtypes.hpp"
#include "C_GiSvSubNodeData.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_gui
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_GiSvNodeData
{
public:
   class C_GiSvNodeDataPreconditionErrors
   {
   public:
      void Clear(void);

      std::vector<uint32_t> c_NvmWriteError;
      std::vector<uint32_t> c_PemWriteError;
      std::vector<uint32_t> c_DebuggerEnableError;
      std::vector<uint32_t> c_DebuggerDisableError;
      std::vector<uint32_t> c_EthToEthError;
   };

   C_GiSvNodeData();

   void Init(const uint32_t ou32_ViewIndex, const uint32_t ou32_NodeIndex);

   void SetConnecting(const bool oq_Active);
   void SetConnected(const bool oq_Active);
   void SetUpdating(const bool oq_Active);
   void SetNodeUpdateInProgress(const bool oq_Active, const bool oq_Aborted, const uint32_t ou32_FailedApplicationIndex,
                                const uint32_t ou32_UpdatedNodeIndex, const uint32_t ou32_CurrentNodeIndex);
   void SetErrorState(const uint32_t ou32_NodeIndex);
   void SetNodeConnectStates(const std::vector<stw::opensyde_core::C_OscSuSequencesNodeConnectStates> & orc_NodeStates,
                             const C_GiSvNodeDataPreconditionErrors & orc_NodePreconditionErrors);
   void SetNodeUpdateStates(const std::vector<stw::opensyde_core::C_OscSuSequencesNodeUpdateStates> & orc_NodeStates);
   void UpdateInitialPackageStatus(const stw::opensyde_gui_logic::C_SyvUpDeviceInfo & orc_DeviceApplicationInfos,
                                   const uint32_t ou32_NodeIndex);

   void CopyInitialStatus(C_GiSvNodeData & orc_NodeData) const;
   void CopyUpdateStatus(C_GiSvNodeData & orc_NodeData) const;
   void CopyStwDeviceInfo(C_GiSvNodeData & orc_NodeData) const;
   void CopyOsyDeviceInfo(C_GiSvNodeData & orc_NodeData) const;
   void CopyDiscardedStatus(C_GiSvNodeData & orc_NodeData) const;

   bool HasNodeAnAvailableFlashloader(void) const;
   bool AreAllNodesValid(void) const;
   bool IsThereAnyHexFileInformation(void) const;
   bool IsDeviceInfoDiscarded(const uint32_t ou32_SubDeviceIndex) const;
   bool IsThereAnyHexFileInformationForDevice(const uint32_t ou32_SubDeviceIndex) const;
   bool IsSubNodeActive(const uint32_t ou32_SubDeviceIndex) const;

   void DiscardInfo(void);
   bool CheckAlwaysUpdate(void) const;
   bool CheckUpdateDisabledState(void) const;

   bool GetConnectInProgressStatus(void) const;
   bool GetUpdateConnectedStatus(void) const;
   bool GetUpdateInProgressStatus(void) const;

   stw::opensyde_gui_logic::C_SyvUtil::E_NodeUpdateStatus GetOverallUpdateState(void) const;
   stw::opensyde_gui_logic::C_SyvUtil::E_NodeUpdateInitialStatus GetOverallInitialState(void) const;

   uint32_t GetSubNodeCount(void) const;
   stw::opensyde_gui_logic::C_SyvUtil::E_NodeUpdateInitialStatus GetInitialStateByNodeIndex(
      const uint32_t ou32_NodeIndex) const;
   bool GetStwDeviceInfoByNodeIndex(const uint32_t ou32_NodeIndex) const;
   const C_GiSvSubNodeData * GetSubNodeByNodeIndex(const uint32_t ou32_NodeIndex) const;
   const C_GiSvSubNodeData * GetSubNodeBySubNodeIndex(const uint32_t ou32_SubNodeIndex) const;
   bool IsNodeConnectStatesSet(void) const;
   bool IsNodeUpdateStatesSet(void) const;

private:
   bool mq_ConnectInProgress;
   bool mq_IsConnected;
   bool mq_UpdateInProgress;

   std::vector<C_GiSvSubNodeData> mc_SubNodes;
   std::vector<uint8_t> mc_SubNodesActiveFlags;

   uint32_t m_GetCorrespondingSubNodeIndex(const uint32_t ou32_NodeIndex) const;
   static bool mh_IsPreconditionErrorSet(const std::vector<uint32_t> & orc_ErrorIndexes,
                                         const uint32_t ou32_CurrentNodeIndex);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
