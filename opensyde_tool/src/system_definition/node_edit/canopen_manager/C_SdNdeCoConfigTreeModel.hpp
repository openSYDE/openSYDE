//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Model for tree for navigation and configuration of CANopen Manager
   \copyright   Copyright 2022 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_SDNDECOCONFIGTREEMODEL_HPP
#define C_SDNDECOCONFIGTREEMODEL_HPP

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <QIcon>

#include "C_OscNode.hpp"
#include "C_TblTreeModelCheckable.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_gui_logic
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_SdNdeCoConfigTreeModel :
   public C_TblTreeModelCheckable
{
   Q_OBJECT

public:
   C_SdNdeCoConfigTreeModel(QWidget * const opc_Parent = NULL);
   ~C_SdNdeCoConfigTreeModel(void) override;

   void SetNodeId(const uint32_t ou32_NodeIndex);

   int32_t Init(const std::vector<uint32_t> & orc_ElementIndices) override;
   void GetCheckedItems(std::vector<uint32_t> & orc_ElementIndices,
                        std::vector<std::vector<uint32_t> > & orc_ChildIndicesPerElement) const override;
   uint32_t GetCheckedItemCount(void) const override;

   bool setData(const QModelIndex & orc_Index, const QVariant & orc_Value, const int32_t os32_Role) override;

   uint32_t GetNodeIndex(void) const;
   int32_t GetInterfaceIdForModelIndex(const QModelIndex & orc_Index, uint8_t & oru8_InterfaceNumber) const;
   int32_t GetDeviceIndexForModelIndex(const QModelIndex & orc_Index, uint8_t & oru8_InterfaceNumber,
                                       stw::opensyde_core::C_OscCanInterfaceId & orc_DeviceInterfaceId) const;
   int32_t GetDeviceAndUseCaseIndexForModelIndex(const QModelIndex & orc_Index, uint8_t & oru8_ManagerInterfaceNumber,
                                                 stw::opensyde_core::C_OscCanInterfaceId & orc_DeviceInterfaceId,
                                                 uint32_t & oru32_UseCaseIndex)
   const;

   void AddDevice(const uint32_t ou32_SelectedNodeIndex, const uint8_t ou8_SelectedNodeInterfaceNumber,
                  const uint8_t ou8_OriginalNodeInterfaceNumber,
                  const stw::opensyde_core::C_OscCanOpenManagerDeviceInfo & orc_Config);
   void RemoveDevice(const uint8_t ou8_OriginalNodeInterfaceNumber,
                     const stw::opensyde_core::C_OscCanInterfaceId & orc_DeviceId);

   QModelIndex GetInterfaceModelIndex(const uint8_t ou8_InterfaceNumber) const;
   QModelIndex GetDeviceUseCaseModelIndex(const uint8_t ou8_InterfaceNumber,
                                          const stw::opensyde_core::C_OscCanInterfaceId & orc_DeviceId,
                                          const uint32_t ou32_UseCaseIndex);
   QModelIndex GetDevicesModelIndex(const uint8_t ou8_InterfaceNumber) const;
   QModelIndex GetDeviceModelIndex(const uint8_t ou8_InterfaceNumber,
                                   const stw::opensyde_core::C_OscCanInterfaceId & orc_DeviceId) const;

   void CheckError(const QModelIndex & orc_InterfaceEntry);

   static const uint32_t hu32_INDEX_DEVICE_USE_CASE_CONFIGURATION;
   static const uint32_t hu32_INDEX_DEVICE_USE_CASE_PDOS;
   static const uint32_t hu32_INDEX_DEVICE_USE_CASE_EDS_FILE;

   //The signals keyword is necessary for Qt signal slot functionality
   //lint -save -e1736
Q_SIGNALS:
   //lint -restore
   void SigCommDatapoolsChanged(void) const;
   void SigErrorChange(void) const;
   void SigSelectManager(const QModelIndex & orc_ManagerIndex) const;
   void SigManagerUnchecked(const QModelIndex & orc_ManagerIndex) const;
   void SigDeviceRemoved(void) const;
   void SigNodeIdToBeChanged(uint32_t ou32_NodeIndex);

private:
   QWidget * const mpc_Parent;
   uint32_t mu32_NodeIndex;
   QIcon mc_IconInterface;
   QIcon mc_IconInterfaceError;
   QIcon mc_IconNode;
   QIcon mc_IconNodeError;
   QIcon mc_IconNodeItems;
   QIcon mc_IconNodeItemsError;
   QMap<uint8_t, QMap<stw::opensyde_core::C_OscCanInterfaceId, uint32_t> > mc_LookupTreeIndex;

   void m_InitInterfaceNode(const stw::opensyde_core::C_OscNodeComInterfaceSettings & orc_Interface,
                            const stw::opensyde_core::C_OscNode & orc_Node,
                            C_TblTreeModelCheckableItem & orc_InterfaceEntry);
   void m_InitInterfaceNodeContent(const stw::opensyde_core::C_OscNodeComInterfaceSettings & orc_Interface,
                                   const bool oq_ContainsChildren,
                                   C_TblTreeModelCheckableItem & orc_InterfaceEntry) const;
   void m_InitDevicesNode(const uint8_t ou8_InterfaceNumber,
                          const stw::opensyde_core::C_OscCanOpenManagerInfo & orc_CanOpenManager,
                          C_TblTreeModelCheckableItem & orc_DevicesEntry);
   void m_InitDevicesNodeContent(const stw::opensyde_core::C_OscCanOpenManagerInfo & orc_CanOpenManager,
                                 C_TblTreeModelCheckableItem & orc_DevicesEntry) const;
   void m_InitDeviceNode(const uint8_t ou8_InterfaceNumber,
                         const stw::opensyde_core::C_OscCanInterfaceId & orc_DeviceInterfaceId,
                         C_TblTreeModelCheckableItem & orc_DeviceEntry);
   void m_InitDeviceNodeContent(const uint8_t ou8_InterfaceNumber,
                                const stw::opensyde_core::C_OscCanInterfaceId & orc_DeviceInterfaceId,
                                C_TblTreeModelCheckableItem & orc_DeviceEntry);
   void m_InitEdsFileNodeContent(C_TblTreeModelCheckableItem & orc_EdsFileEntry) const;
   void m_InitConfigurationNodeContent(C_TblTreeModelCheckableItem & orc_ConfigurationEntry) const;
   void m_InitPdosNodeContent(C_TblTreeModelCheckableItem & orc_PdosEntry) const;
   void m_InitIcons();
   bool m_CheckInterface(const QModelIndex & orc_Index, C_TblTreeModelCheckableItem & orc_InterfaceItem);
   void m_HandleCheckInterfaceAction(const QModelIndex & orc_Index, C_TblTreeModelCheckableItem & orc_InterfaceItem);
   void m_HandleUncheckInterfaceAction(const QModelIndex & orc_Index, C_TblTreeModelCheckableItem & orc_InterfaceItem);
   uint32_t m_InsertNewInterfaceIntoLookupTree(const uint8_t ou8_InterfaceNumber,
                                               const stw::opensyde_core::C_OscCanInterfaceId & orc_DeviceInterfaceId);
   static uint32_t mh_SearchNewUniqueValue(const QMap<stw::opensyde_core::C_OscCanInterfaceId,
                                                      uint32_t> & orc_Map);
   uint32_t m_GetExpectedPosInTree(const uint8_t ou8_InterfaceNumber,
                                   const stw::opensyde_core::C_OscCanInterfaceId & orc_DeviceInterfaceId);
   int32_t m_RetrieveInterfaceFromLookupTree(const uint8_t ou8_InterfaceNumber, const uint32_t ou32_TreeNodeIndex,
                                             stw::opensyde_core::C_OscCanInterfaceId & orc_DeviceInterfaceId)
   const;
   C_TblTreeModelCheckableItem * m_GetInterfaceModelNode(const uint8_t ou8_InterfaceNumber) const;
   C_TblTreeModelCheckableItem * m_GetDevicesModelNode(const uint8_t ou8_InterfaceNumber) const;
   C_TblTreeModelCheckableItem * m_GetDeviceModelNode(const uint8_t ou8_InterfaceNumber,
                                                      const stw::opensyde_core::C_OscCanInterfaceId & orc_DeviceId);
   void m_TriggerUpdateDeviceCount(const uint8_t ou8_InterfaceNumber);

   void m_CheckError(C_TblTreeModelCheckableItem * const opc_InterfaceEntry);
   void m_CheckError(const QModelIndex & orc_InterfaceIndex, C_TblTreeModelCheckableItem * const opc_InterfaceEntry);
   bool m_CheckIfCoManagerCanBeActivated(const uint32_t ou32_NodeIndex, const uint32_t ou32_InterfaceIndex) const;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
