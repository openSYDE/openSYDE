//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Model for tree for navigation and configuration of CANopen Manager
   \copyright   Copyright 2022 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_SDNDECOCONFIGTREEMODEL_H
#define C_SDNDECOCONFIGTREEMODEL_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <QIcon>

#include "C_OSCNode.h"
#include "C_TblTreeModelCheckable.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw_opensyde_gui_logic
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_SdNdeCoConfigTreeModel :
   public C_TblTreeModelCheckable
{
   Q_OBJECT

public:
   C_SdNdeCoConfigTreeModel(QWidget * const opc_Parent = NULL);
   ~C_SdNdeCoConfigTreeModel(void);

   void SetNodeId(const stw_types::uint32 ou32_NodeIndex);

   virtual stw_types::sint32 Init(const std::vector<stw_types::uint32> & orc_ElementIndices) override;
   virtual void GetCheckedItems(std::vector<stw_types::uint32> & orc_ElementIndices,
                                std::vector<std::vector<stw_types::uint32> > & orc_ChildIndicesPerElement) const
   override;
   virtual stw_types::uint32 GetCheckedItemCount(void) const override;

   virtual bool setData(const QModelIndex & orc_Index, const QVariant & orc_Value,
                        const stw_types::sintn osn_Role) override;

   stw_types::uint32 GetNodeIndex(void) const;
   stw_types::sint32 GetInterfaceIdForModelIndex(const QModelIndex & orc_Index,
                                                 stw_types::uint8 & oru8_InterfaceNumber) const;
   stw_types::sint32 GetDeviceIndexForModelIndex(const QModelIndex & orc_Index, stw_types::uint8 & oru8_InterfaceNumber,
                                                 stw_opensyde_core::C_OSCCanInterfaceId & orc_DeviceInterfaceId) const;
   stw_types::sint32 GetDeviceAndUseCaseIndexForModelIndex(const QModelIndex & orc_Index,
                                                           stw_types::uint8 & oru8_ManagerInterfaceNumber,
                                                           stw_opensyde_core::C_OSCCanInterfaceId & orc_DeviceInterfaceId, stw_types::uint32 & oru32_UseCaseIndex)
   const;

   void AddDevice(const stw_types::uint32 ou32_SelectedNodeIndex,
                  const stw_types::uint8 ou8_SelectedNodeInterfaceNumber,
                  const stw_types::uint8 ou8_OriginalNodeInterfaceNumber,
                  const stw_opensyde_core::C_OSCCanOpenManagerDeviceInfo & orc_Config);
   void RemoveDevice(const stw_types::uint8 ou8_OriginalNodeInterfaceNumber,
                     const stw_opensyde_core::C_OSCCanInterfaceId & orc_DeviceId);

   QModelIndex GetInterfaceModelIndex(const stw_types::uint8 ou8_InterfaceNumber) const;
   QModelIndex GetDeviceUseCaseModelIndex(const stw_types::uint8 ou8_InterfaceNumber,
                                          const stw_opensyde_core::C_OSCCanInterfaceId & orc_DeviceId,
                                          const stw_types::uint32 ou32_UseCaseIndex);
   QModelIndex GetDevicesModelIndex(const stw_types::uint8 ou8_InterfaceNumber) const;
   QModelIndex GetDeviceModelIndex(const stw_types::uint8 ou8_InterfaceNumber,
                                   const stw_opensyde_core::C_OSCCanInterfaceId & orc_DeviceId) const;

   void CheckError(const QModelIndex & orc_InterfaceEntry);

   static const stw_types::uint32 hu32_INDEX_DEVICE_USE_CASE_CONFIGURATION;
   static const stw_types::uint32 hu32_INDEX_DEVICE_USE_CASE_PDOS;
   static const stw_types::uint32 hu32_INDEX_DEVICE_USE_CASE_EDS_FILE;

   //The signals keyword is necessary for Qt signal slot functionality
   //lint -save -e1736
Q_SIGNALS:
   //lint -restore
   void SigCommDatapoolsChanged(void) const;
   void SigErrorChange(void) const;
   void SigSelectManager(const QModelIndex & orc_ManagerIndex) const;
   void SigManagerUnchecked(const QModelIndex & orc_ManagerIndex) const;
   void SigDeviceRemoved(void) const;

private:
   QWidget * const mpc_Parent;
   stw_types::uint32 mu32_NodeIndex;
   QIcon mc_IconInterface;
   QIcon mc_IconInterfaceError;
   QIcon mc_IconNode;
   QIcon mc_IconNodeError;
   QIcon mc_IconNodeItems;
   QIcon mc_IconNodeItemsError;
   QMap<stw_types::uint8, QMap<stw_opensyde_core::C_OSCCanInterfaceId, stw_types::uint32> > mc_LookupTreeIndex;

   void m_InitInterfaceNode(const stw_opensyde_core::C_OSCNodeComInterfaceSettings & orc_Interface,
                            const stw_opensyde_core::C_OSCNode & orc_Node,
                            C_TblTreeModelCheckableItem & orc_InterfaceEntry);
   void m_InitInterfaceNodeContent(const stw_opensyde_core::C_OSCNodeComInterfaceSettings & orc_Interface,
                                   const bool oq_ContainsChildren,
                                   C_TblTreeModelCheckableItem & orc_InterfaceEntry) const;
   void m_InitDevicesNode(const stw_types::uint8 ou8_InterfaceNumber,
                          const stw_opensyde_core::C_OSCCanOpenManagerInfo & orc_CANOpenManager,
                          C_TblTreeModelCheckableItem & orc_DevicesEntry);
   void m_InitDevicesNodeContent(const stw_opensyde_core::C_OSCCanOpenManagerInfo & orc_CANOpenManager,
                                 C_TblTreeModelCheckableItem & orc_DevicesEntry) const;
   void m_InitDeviceNode(const stw_types::uint8 ou8_InterfaceNumber,
                         const stw_opensyde_core::C_OSCCanInterfaceId & orc_DeviceInterfaceId,
                         C_TblTreeModelCheckableItem & orc_DeviceEntry);
   void m_InitDeviceNodeContent(const stw_types::uint8 ou8_InterfaceNumber,
                                const stw_opensyde_core::C_OSCCanInterfaceId & orc_DeviceInterfaceId,
                                C_TblTreeModelCheckableItem & orc_DeviceEntry);
   void m_InitEDSFileNodeContent(C_TblTreeModelCheckableItem & orc_EDSFileEntry) const;
   void m_InitConfigurationNodeContent(C_TblTreeModelCheckableItem & orc_ConfigurationEntry) const;
   void m_InitPDOsNodeContent(C_TblTreeModelCheckableItem & orc_PDOsEntry) const;
   void m_InitIcons();
   bool m_CheckInterface(const QModelIndex & orc_Index, C_TblTreeModelCheckableItem & orc_InterfaceItem);
   void m_HandleCheckInterfaceAction(const QModelIndex & orc_Index, C_TblTreeModelCheckableItem & orc_InterfaceItem);
   void m_HandleUncheckInterfaceAction(const QModelIndex & orc_Index, C_TblTreeModelCheckableItem & orc_InterfaceItem);
   stw_types::uint32 m_InsertNewInterfaceIntoLookupTree(const stw_types::uint8 ou8_InterfaceNumber,
                                                        const stw_opensyde_core::C_OSCCanInterfaceId & orc_DeviceInterfaceId);
   static stw_types::uint32 mh_SearchNewUniqueValue(const QMap<stw_opensyde_core::C_OSCCanInterfaceId,
                                                               stw_types::uint32> & orc_Map);
   stw_types::uint32 m_GetExpectedPosInTree(const stw_types::uint8 ou8_InterfaceNumber,
                                            const stw_opensyde_core::C_OSCCanInterfaceId & orc_DeviceInterfaceId);
   stw_types::sint32 m_RetrieveInterfaceFromLookupTree(const stw_types::uint8 ou8_InterfaceNumber,
                                                       const stw_types::uint32 ou32_TreeNodeIndex,
                                                       stw_opensyde_core::C_OSCCanInterfaceId & orc_DeviceInterfaceId)
   const;
   C_TblTreeModelCheckableItem * m_GetInterfaceModelNode(const stw_types::uint8 ou8_InterfaceNumber) const;
   C_TblTreeModelCheckableItem * m_GetDevicesModelNode(const stw_types::uint8 ou8_InterfaceNumber) const;
   C_TblTreeModelCheckableItem * m_GetDeviceModelNode(const stw_types::uint8 ou8_InterfaceNumber,
                                                      const stw_opensyde_core::C_OSCCanInterfaceId & orc_DeviceId);
   void m_TriggerUpdateDeviceCount(const stw_types::uint8 ou8_InterfaceNumber);

   void m_CheckError(C_TblTreeModelCheckableItem * const opc_InterfaceEntry);
   void m_CheckError(const QModelIndex & orc_InterfaceIndex, C_TblTreeModelCheckableItem * const opc_InterfaceEntry);
   bool m_CheckIfCoManagerCanBeActivated(const stw_types::uint32 ou32_NodeIndex,
                                         const stw_types::uint32 ou32_InterfaceIndex) const;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif
