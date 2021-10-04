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
#include "stwtypes.h"
#include "C_GiSvSubNodeData.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw_opensyde_gui
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_GiSvNodeData
{
public:
   C_GiSvNodeData();

   void Init(const stw_types::uint32 ou32_ViewIndex, const stw_types::uint32 ou32_NodeIndex);

   void SetConnected(const bool oq_Active);
   void SetUpdating(const bool oq_Active);
   void SetNodeUpdateInProgress(const bool oq_Active, const bool oq_Aborted,
                                const stw_types::uint32 ou32_FailedApplicationIndex,
                                const stw_types::uint32 ou32_UpdatedNodeIndex,
                                const stw_types::uint32 ou32_CurrentNodeIndex);
   void SetNoResponse(const stw_types::uint32 ou32_NodeIndex);
   void UpdateInitialPackageStatus(const stw_opensyde_gui_logic::C_SyvUpDeviceInfo & orc_DeviceApplicationInfos,
                                   const stw_types::uint32 ou32_NodeIndex);

   void CopyInitialStatus(C_GiSvNodeData & orc_NodeData) const;
   void CopyUpdateStatus(C_GiSvNodeData & orc_NodeData) const;
   void CopySTWDeviceInfo(C_GiSvNodeData & orc_NodeData) const;
   void CopyOSYDeviceInfo(C_GiSvNodeData & orc_NodeData) const;
   void CopyDiscardedStatus(C_GiSvNodeData & orc_NodeData) const;

   bool IsStwDevice(void) const;
   bool AreAllNodesValid(void) const;
   bool IsThereAnyHexFileInformation(void) const;
   bool IsDeviceInfoDiscarded(const stw_types::uint32 ou32_SubDeviceIndex) const;
   bool IsThereAnyHexFileInformationForDevice(const stw_types::uint32 ou32_SubDeviceIndex) const;

   void DiscardInfo(void);
   bool CheckAlwaysUpdate(void) const;
   bool CheckUpdateDisabledState(void) const;

   bool GetUpdateConnectedStatus(void) const;
   bool GetUpdateInProgressStatus(void) const;

   stw_opensyde_gui_logic::C_SyvUtil::E_NodeUpdateStatus GetOverallUpdateState(void) const;
   stw_opensyde_gui_logic::C_SyvUtil::E_NodeUpdateInitialStatus GetOverallInitialState(void) const;

   stw_types::uint32 GetSubNodeCount(void) const;
   stw_opensyde_gui_logic::C_SyvUtil::E_NodeUpdateInitialStatus GetInitialStateByNodeIndex(
      const stw_types::uint32 ou32_NodeIndex) const;
   bool GetSTWDeviceInfoByNodeIndex(const stw_types::uint32 ou32_NodeIndex) const;
   const C_GiSvSubNodeData * GetSubNodeByNodeIndex(const stw_types::uint32 ou32_NodeIndex) const;

private:
   bool mq_UpdateConnected;
   bool mq_UpdateInProgress;

   std::vector<C_GiSvSubNodeData> mc_SubNodes;

   stw_types::uint32 m_GetCorrespondingSubNodeIndex(const stw_types::uint32 ou32_NodeIndex) const;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif
