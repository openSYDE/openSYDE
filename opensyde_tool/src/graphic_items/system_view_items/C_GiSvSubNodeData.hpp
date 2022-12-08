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
#include "stwtypes.hpp"
#include "C_OsyHexFile.hpp"
#include "C_SyvUtil.hpp"
#include "C_SyvUpDeviceInfo.hpp"
#include "C_PuiSvNodeUpdate.hpp"
#include "C_OscSuSequencesNodeStates.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_gui
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

   C_GiSvSubNodeData(const uint32_t ou32_ViewIndex, const uint32_t ou32_NodeIndex);
   virtual ~C_GiSvSubNodeData(void) noexcept;

   void SetConnected(const bool oq_Active);
   void SetUpdating(const bool oq_Active);
   void SetNodeUpdateInProgress(const bool oq_Active, const bool oq_Aborted,
                                const uint32_t ou32_FailedApplicationIndex);
   void SetErrorState(void);
   void UpdateInitialPackageStatus(const stw::opensyde_gui_logic::C_SyvUpDeviceInfo & orc_DeviceApplicationInfos);
   bool CheckUpdateDisabledState() const;
   bool HasNodeAnAvailableFlashloader(void) const;
   bool CheckAlwaysUpdate(void) const;
   void SetNodeConnectStates(const stw::opensyde_core::C_OscSuSequencesNodeConnectStates & orc_NodeStates,
                             const C_GiSvSubNodeDataPreconditionErrors & orc_PreconditionErrors);
   void SetNodeUpdateStates(const stw::opensyde_core::C_OscSuSequencesNodeUpdateStates & orc_NodeStates);

   void CopyInitialStatus(C_GiSvSubNodeData & orc_NodeData) const;
   void CopyUpdateStatus(C_GiSvSubNodeData & orc_NodeData) const;
   void CopyStwDeviceInfo(C_GiSvSubNodeData & orc_NodeData) const;
   void CopyOpenSydeDeviceInfo(C_GiSvSubNodeData & orc_NodeData) const;
   void CopyDiscardedStatus(C_GiSvSubNodeData & orc_NodeData) const;

   bool IsDeviceInfoDiscarded(void) const;
   bool IsThereAnyHexFileInformation(void) const;

   void DiscardInfo(void);

   uint32_t GetNodeIndex(void) const;
   bool GetDataBlockFoundStatus(void) const;
   bool GetUpdateFailedStatus(void) const;
   bool GetUpdateSuccessStatus(void) const;
   bool GetValidStatus(void) const;
   stw::opensyde_gui_logic::C_SyvUtil::E_NodeUpdateInitialStatus GetInitialStatus(void) const;
   stw::opensyde_gui_logic::C_SyvUtil::E_NodeUpdateStatus GetUpdateStatus(void) const;
   uint32_t GetFailedApplicationIndex(void) const;
   const stw::diag_lib::C_XFLECUInformation * GetHexFileInfo(const uint32_t ou32_ApplicationIndex) const;
   uint32_t GetHexFileInfosCount(void) const;
   uint32_t GetParamFileInfosCount(void) const;
   uint32_t GetFileInfosCount(void) const;
   bool IsPemFileInfoSet(void) const;
   uint32_t GetHexAppInfoAmbiguousSize(void) const;
   bool GetHexAppInfoAmbiguous(const uint32_t ou32_ApplicationIndex) const;
   stw::opensyde_gui_logic::C_SyvUpDeviceInfo GetDeviceInfo(void) const;
   const stw::opensyde_core::C_OscSuSequencesNodeConnectStates & GetNodeConnectStates(void) const;
   const C_GiSvSubNodeDataPreconditionErrors & GetNodeConnectPreconditionErrors(void) const;
   const stw::opensyde_core::C_OscSuSequencesNodeUpdateStates & GetNodeUpdateStates(void) const;
   bool IsNodeConnectStatesSet(void) const;
   bool IsNodeUpdateStatesSet(void) const;

private:
   void m_CheckThirdParty(void);

   void m_InitPackageData(const stw::opensyde_core::C_OscNode & orc_Node,
                          const stw::opensyde_gui_logic::C_PuiSvNodeUpdate & orc_UpdateInformation);
   void m_InitPackageDataForApplicationsFromFiles(const std::vector<QString> & orc_FinalFilePaths);
   void m_InitPackageDataForOtherFiles(const stw::opensyde_gui_logic::C_PuiSvNodeUpdate & orc_UpdateInformation);
   void m_InitStatusFromPackage(const stw::opensyde_gui_logic::C_SyvUpDeviceInfo & orc_DeviceApplicationInfos,
                                const stw::opensyde_core::C_OscNode & orc_Node);

   uint32_t mu32_ViewIndex;
   uint32_t mu32_NodeIndex;

   bool mq_UpdateInProgress;
   bool mq_UpdateFailed;
   bool mq_UpdateSuccess;
   bool mq_ValidStatus;
   bool mq_Discarded;
   stw::opensyde_gui_logic::C_SyvUtil::E_NodeUpdateInitialStatus me_InitialStatus;
   stw::opensyde_gui_logic::C_SyvUtil::E_NodeUpdateStatus me_UpdateStatus;
   uint32_t mu32_FailedApplicationIndex;

   bool mq_AnyDatablockFound;
   std::vector<stw::diag_lib::C_XFLECUInformation> mc_HexFileInfos;
   std::vector<QString> mc_ParamFileInfos;
   std::vector<QString> mc_FileInfos;
   QString mc_PemFileInfo;
   std::vector<bool> mc_HexAppInfoAmbiguous;
   stw::opensyde_gui_logic::C_SyvUpDeviceInfo mc_DeviceInfo;

   // Precondition check errors
   C_GiSvSubNodeDataPreconditionErrors mc_PreconditionErrors;

   // Sequence states
   stw::opensyde_core::C_OscSuSequencesNodeConnectStates mc_ConnectStates;
   stw::opensyde_core::C_OscSuSequencesNodeUpdateStates mc_UpdateStates;
   bool mq_ConnectStatesSet;
   bool mq_UpdateStatesSet;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
