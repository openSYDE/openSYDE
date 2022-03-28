//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Data class to hold sub-node specific for all sub-nodes grouped together in a C_GiSvNodeSyvUpdate.
   \copyright   Copyright 2021 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_GISVSUBNODEDATA_H
#define C_GISVSUBNODEDATA_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "stwtypes.h"
#include "C_OsyHexFile.h"
#include "C_SyvUtil.h"
#include "C_SyvUpDeviceInfo.h"
#include "C_PuiSvNodeUpdate.h"
#include "C_OSCSuSequencesNodeStates.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw_opensyde_gui
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_GiSvSubNodeData
{
public:
   class C_GiSvSubNodeDataPreconditionErrors
   {
   public:
      bool q_NvmWriteError;
      bool q_PemWriteError;
      bool q_DebuggerEnableError;
      bool q_DebuggerDisableError;
      bool q_EthToEthError;
   };

   C_GiSvSubNodeData(const stw_types::uint32 ou32_ViewIndex, const stw_types::uint32 ou32_NodeIndex);
   ~C_GiSvSubNodeData(void);

   void SetConnected(const bool oq_Active);
   void SetUpdating(const bool oq_Active);
   void SetNodeUpdateInProgress(const bool oq_Active, const bool oq_Aborted,
                                const stw_types::uint32 ou32_FailedApplicationIndex);
   void SetErrorState(void);
   void UpdateInitialPackageStatus(const stw_opensyde_gui_logic::C_SyvUpDeviceInfo & orc_DeviceApplicationInfos);
   bool CheckUpdateDisabledState() const;
   bool HasNodeAnAvailableFlashloader(void) const;
   bool CheckAlwaysUpdate(void) const;
   void SetNodeConnectStates(const stw_opensyde_core::C_OSCSuSequencesNodeConnectStates & orc_NodeStates,
                             const C_GiSvSubNodeDataPreconditionErrors & orc_PreconditionErrors);
   void SetNodeUpdateStates(const stw_opensyde_core::C_OSCSuSequencesNodeUpdateStates & orc_NodeStates);

   void CopyInitialStatus(C_GiSvSubNodeData & orc_NodeData) const;
   void CopyUpdateStatus(C_GiSvSubNodeData & orc_NodeData) const;
   void CopySTWDeviceInfo(C_GiSvSubNodeData & orc_NodeData) const;
   void CopyOSYDeviceInfo(C_GiSvSubNodeData & orc_NodeData) const;
   void CopyDiscardedStatus(C_GiSvSubNodeData & orc_NodeData) const;

   bool IsDeviceInfoDiscarded(void) const;
   bool IsThereAnyHexFileInformation(void) const;

   void DiscardInfo(void);

   stw_types::uint32 GetNodeIndex(void) const;
   bool GetDataBlockFoundStatus(void) const;
   bool GetUpdateFailedStatus(void) const;
   bool GetUpdateSuccessStatus(void) const;
   bool GetValidStatus(void) const;
   stw_opensyde_gui_logic::C_SyvUtil::E_NodeUpdateInitialStatus GetInitialStatus(void) const;
   stw_opensyde_gui_logic::C_SyvUtil::E_NodeUpdateStatus GetUpdateStatus(void) const;
   stw_types::uint32 GetFailedApplicationIndex(void) const;
   const stw_diag_lib::C_XFLECUInformation * GetHexFileInfo(const stw_types::uint32 ou32_ApplicationIndex) const;
   stw_types::uint32 GetHexFileInfosCount(void) const;
   stw_types::uint32 GetParamFileInfosCount(void) const;
   stw_types::uint32 GetFileInfosCount(void) const;
   bool IsPemFileInfoSet(void) const;
   stw_types::uint32 GetHexAppInfoAmbiguousSize(void) const;
   bool GetHexAppInfoAmbiguous(const stw_types::uint32 ou32_ApplicationIndex) const;
   stw_opensyde_gui_logic::C_SyvUpDeviceInfo GetDeviceInfo(void) const;
   const stw_opensyde_core::C_OSCSuSequencesNodeConnectStates & GetNodeConnectStates(void) const;
   const C_GiSvSubNodeDataPreconditionErrors & GetNodeConnectPreconditionErrors(void) const;
   const stw_opensyde_core::C_OSCSuSequencesNodeUpdateStates & GetNodeUpdateStates(void) const;
   bool IsNodeConnectStatesSet(void) const;
   bool IsNodeUpdateStatesSet(void) const;

private:
   void m_CheckThirdParty(void);

   void m_InitPackageData(const stw_opensyde_core::C_OSCNode & orc_Node,
                          const stw_opensyde_gui_logic::C_PuiSvNodeUpdate & orc_UpdateInformation);
   void m_InitPackageDataForApplicationsFromFiles(const std::vector<QString> & orc_FinalFilePaths);
   void m_InitPackageDataForOtherFiles(const stw_opensyde_gui_logic::C_PuiSvNodeUpdate & orc_UpdateInformation);
   void m_InitStatusFromPackage(const stw_opensyde_gui_logic::C_SyvUpDeviceInfo & orc_DeviceApplicationInfos,
                                const stw_opensyde_core::C_OSCNode & orc_Node);

   stw_types::uint32 mu32_ViewIndex;
   stw_types::uint32 mu32_NodeIndex;

   bool mq_UpdateInProgress;
   bool mq_UpdateFailed;
   bool mq_UpdateSuccess;
   bool mq_ValidStatus;
   bool mq_Discarded;
   stw_opensyde_gui_logic::C_SyvUtil::E_NodeUpdateInitialStatus me_InitialStatus;
   stw_opensyde_gui_logic::C_SyvUtil::E_NodeUpdateStatus me_UpdateStatus;
   stw_types::uint32 mu32_FailedApplicationIndex;

   bool mq_AnyDatablockFound;
   std::vector<stw_diag_lib::C_XFLECUInformation> mc_HexFileInfos;
   std::vector<QString> mc_ParamFileInfos;
   std::vector<QString> mc_FileInfos;
   QString mc_PemFileInfo;
   std::vector<bool> mc_HexAppInfoAmbiguous;
   stw_opensyde_gui_logic::C_SyvUpDeviceInfo mc_DeviceInfo;

   // Precondition check errors
   C_GiSvSubNodeDataPreconditionErrors mc_PreconditionErrors;

   // Sequence states
   stw_opensyde_core::C_OSCSuSequencesNodeConnectStates mc_ConnectStates;
   stw_opensyde_core::C_OSCSuSequencesNodeUpdateStates mc_UpdateStates;
   bool mq_ConnectStatesSet;
   bool mq_UpdateStatesSet;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif
