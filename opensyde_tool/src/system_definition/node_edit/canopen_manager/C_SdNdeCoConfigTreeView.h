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
#include "stwtypes.h"
#include "C_OgeContextMenu.h"
#include "C_OgeTreeViewToolTipBase.h"
#include "C_SdNdeCoConfigTreeModel.h"
#include "C_UsNode.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw_opensyde_gui
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */
class C_SdNdeCoConfigTreeDelegate :
   public QStyledItemDelegate
{
   Q_OBJECT

public:
   C_SdNdeCoConfigTreeDelegate(QObject * const opc_Parent = NULL);

   virtual void paint(QPainter * const opc_Painter, const QStyleOptionViewItem & orc_Option,
                      const QModelIndex & orc_Index) const;

   void SetCurrentIndexParentRow(const QModelIndex & orc_Index);

private:
   QModelIndex mc_Index;
};

class C_SdNdeCoConfigTreeView :
   public stw_opensyde_gui_elements::C_OgeTreeViewToolTipBase
{
   Q_OBJECT

public:
   explicit C_SdNdeCoConfigTreeView(QWidget * const opc_Parent = NULL);
   virtual ~C_SdNdeCoConfigTreeView(void);

   void SetNodeId(const stw_types::uint32 ou32_NodeIndex);

   void ResetDelegate(void);
   void LoadUserSettings(void);
   void SaveUserSettings(void) const;
   void SetInterfaceSelected(const stw_types::uint8 ou8_InterfaceNumber);
   void SetDeviceSelected(const stw_types::uint8 ou8_InterfaceNumber,
                          const stw_opensyde_core::C_OSCCanInterfaceId & orc_NodeId,
                          const stw_types::uint32 ou32_UseCaseIndex);
   void OpenManagerConfiguration(const stw_types::uint8 ou8_InterfaceNumber);
   void OpenDeviceConfiguration(const stw_types::uint32 ou32_DeviceNodeIndex);
   void CheckError(void);

   //The signals keyword is necessary for Qt signal slot functionality
   //lint -save -e1736
Q_SIGNALS:
   //lint -restore
   void SigInterfaceSelected(const stw_types::uint8 ou8_ManagerInterfaceNumber);
   void SigDeviceSelected(const stw_types::uint8 ou8_ManagerInterfaceNumber,
                          const stw_opensyde_core::C_OSCCanInterfaceId & orc_DeviceNodeId,
                          const stw_types::uint32 ou32_UseCaseIndex);
   void SigCommDatapoolsChanged(void);
   void SigOpenOverview(void) const;
   void SigErrorChange(void) const;

protected:
   virtual void selectionChanged(const QItemSelection & orc_SelectedItems,
                                 const QItemSelection & orc_DeselectedItems) override;
   virtual void paintEvent(QPaintEvent * const opc_Event) override;

private:
   stw_types::uint32 mu32_NodeIndex;
   bool mq_Initialized;

   stw_opensyde_gui_logic::C_SdNdeCoConfigTreeModel mc_Model;
   C_SdNdeCoConfigTreeDelegate mc_Delegate;
   stw_opensyde_gui_elements::C_OgeContextMenu * mpc_ContextMenu;
   QAction * mpc_AddAction;
   QAction * mpc_RemoveAction;
   QPoint mc_LastKnownContextMenuPos;

   void m_SetupContextMenu(void);
   void m_OnCustomContextMenuRequested(const QPoint & orc_Pos);

   void m_OnAddDevice(void);
   void m_OnAddDeviceReport(const stw_types::uint32 ou32_SelectedNodeIndex,
                            const stw_types::uint32 ou32_SelectedNodeInterfaceIndex,
                            const stw_types::uint8 ou8_OriginalNodeInterfaceNumber, const QString & orc_EdsPath);
   void m_OnAddDeviceReportAccepted(const stw_types::uint32 ou32_SelectedNodeIndex,
                                    const stw_types::uint8 ou8_SelectedNodeInterfaceNumber,
                                    const stw_types::uint8 ou8_OriginalNodeInterfaceNumber,
                                    const QString & orc_EdsPath);
   void m_SelectDevice(const stw_types::uint8 ou8_OriginalNodeInterfaceNumber,
                       const stw_opensyde_core::C_OSCCanInterfaceId & orc_DeviceId);
   void m_DisplayDeviceAddFinishedMessage(void);
   void m_OnRemoveDevice(void);
   void m_OnItemSelected(void);
   stw_opensyde_core::C_OSCCanOpenManagerDeviceInfo m_CreateNewDevice(const QString & orc_EdsPath);
   static void mh_InitMappableSignals(
      std::vector<stw_opensyde_core::C_OSCCanOpenManagerMappableSignal> & orc_MappableSignals,
      const stw_opensyde_core::C_OSCCanOpenObjectDictionary & orc_EdsDictionary);
   void m_SelectManager(const QModelIndex & orc_ManagerIndex);
   void m_HandleManagerUnchecked(const QModelIndex & orc_ManagerIndex);
   void m_HandleDeviceRemoved(void) const;
   void m_OnExpanded(const QModelIndex & orc_ExpandedIndex);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif
