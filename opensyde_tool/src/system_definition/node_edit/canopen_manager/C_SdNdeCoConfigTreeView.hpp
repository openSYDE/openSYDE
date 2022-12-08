//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       View for tree for navigation and configuration of CANopen Manager

   \copyright   Copyright 2022 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_SDNDECOCONFIGTREEVIEW_H
#define C_SDNDECOCONFIGTREEVIEW_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "stwtypes.hpp"
#include "C_UsNode.hpp"
#include "C_OgeContextMenu.hpp"
#include "C_OgeTreeViewToolTipBase.hpp"
#include "C_SdNdeCoConfigTreeModel.hpp"
#include "C_CieImportDataAssignment.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_gui
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */
class C_SdNdeCoConfigTreeDelegate :
   public QStyledItemDelegate
{
   Q_OBJECT

public:
   C_SdNdeCoConfigTreeDelegate(QObject * const opc_Parent = NULL);

   void paint(QPainter * const opc_Painter, const QStyleOptionViewItem & orc_Option,
              const QModelIndex & orc_Index) const override;

   void SetCurrentIndexParentRow(const QModelIndex & orc_Index);

private:
   QModelIndex mc_Index;
};

class C_SdNdeCoConfigTreeView :
   public stw::opensyde_gui_elements::C_OgeTreeViewToolTipBase
{
   Q_OBJECT

public:
   explicit C_SdNdeCoConfigTreeView(QWidget * const opc_Parent = NULL);
   virtual ~C_SdNdeCoConfigTreeView(void);

   void SetNodeId(const uint32_t ou32_NodeIndex);

   void ResetDelegate(void);
   void LoadUserSettings(void);
   void SaveUserSettings(void) const;
   void SetInterfaceSelected(const uint8_t ou8_InterfaceNumber);
   void SetDeviceSelected(const uint8_t ou8_InterfaceNumber, const stw::opensyde_core::C_OscCanInterfaceId & orc_NodeId,
                          const uint32_t ou32_UseCaseIndex);
   void OpenManagerConfiguration(const uint8_t ou8_InterfaceNumber);
   void OpenDeviceConfiguration(const uint32_t ou32_DeviceNodeIndex);
   void CheckError(void);
   static stw::opensyde_core::C_OscCanOpenManagerDeviceInfo h_CreateNewDevice(const QString & orc_EdsPath);

   //The signals keyword is necessary for Qt signal slot functionality
   //lint -save -e1736
Q_SIGNALS:
   //lint -restore
   void SigInterfaceSelected(const uint8_t ou8_ManagerInterfaceNumber);
   void SigDeviceSelected(const uint8_t ou8_ManagerInterfaceNumber,
                          const stw::opensyde_core::C_OscCanInterfaceId & orc_DeviceNodeId,
                          const uint32_t ou32_UseCaseIndex);
   void SigCommDatapoolsChanged(void);
   void SigOpenOverview(void) const;
   void SigErrorChange(void) const;

protected:
   void selectionChanged(const QItemSelection & orc_SelectedItems, const QItemSelection & orc_DeselectedItems) override;
   void paintEvent(QPaintEvent * const opc_Event) override;

private:
   uint32_t mu32_NodeIndex;
   bool mq_Initialized;

   stw::opensyde_gui_logic::C_SdNdeCoConfigTreeModel mc_Model;
   C_SdNdeCoConfigTreeDelegate mc_Delegate;
   stw::opensyde_gui_elements::C_OgeContextMenu * mpc_ContextMenu;
   QAction * mpc_AddAction;
   QAction * mpc_RemoveAction;
   QPoint mc_LastKnownContextMenuPos;

   void m_SetupContextMenu(void);
   void m_OnCustomContextMenuRequested(const QPoint & orc_Pos);

   void m_OnAddDevice(void);
   void m_OnAddDeviceReport(const uint32_t ou32_SelectedNodeIndex, const uint32_t ou32_SelectedNodeInterfaceIndex,
                            const uint8_t ou8_OriginalNodeInterfaceNumber, const QString & orc_EdsPath);
   void m_OnAddDeviceReportAccepted(const uint32_t ou32_SelectedNodeIndex,
                                    const uint8_t ou8_SelectedNodeInterfaceNumber,
                                    const uint8_t ou8_OriginalNodeInterfaceNumber, const QString & orc_EdsPath);
   void m_SelectDevice(const uint8_t ou8_OriginalNodeInterfaceNumber,
                       const stw::opensyde_core::C_OscCanInterfaceId & orc_DeviceId);
   void m_DisplayDeviceAddFinishedMessage(void);
   void m_OnRemoveDevice(void);
   void m_OnItemSelected(void);
   static void mh_InitMappableSignals(
      std::vector<stw::opensyde_core::C_OscCanOpenManagerMappableSignal> & orc_MappableSignals,
      const stw::opensyde_core::C_OscCanOpenObjectDictionary & orc_EdsDictionary, const bool oq_IsEds);
   void m_SelectManager(const QModelIndex & orc_ManagerIndex);
   void m_HandleManagerUnchecked(const QModelIndex & orc_ManagerIndex);
   void m_HandleDeviceRemoved(void) const;
   void m_OnExpanded(const QModelIndex & orc_ExpandedIndex);
   static void mh_InitNewDeviceContent(stw::opensyde_core::C_OscCanOpenManagerDeviceInfo & orc_Device);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
