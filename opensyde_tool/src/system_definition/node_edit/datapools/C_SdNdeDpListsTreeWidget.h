//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Widget to store and manage multiple data pool list entries (header)

   See cpp file for detailed description

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_SDNDEDPLISTSTREEWIDGET_H
#define C_SDNDEDPLISTSTREEWIDGET_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */

#include <QTreeWidget>
#include <QDropEvent>
#include <QKeyEvent>
#include <vector>
#include "stwtypes.h"
#include "C_SdNdeDpListHeaderWidget.h"
#include "C_SdNdeUnoDataPoolManager.h"
#include "C_OgeContextMenu.h"
#include "C_SdNdeDpListTableWidget.h"
#include "C_SdNdeDpListsTreeDelegate.h"
#include "C_SdNdeDpListModelViewManager.h"
#include "C_SdNdeDpListErrorManager.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw_opensyde_gui
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_SdNdeDpListsTreeWidget :
   public QTreeWidget
{
   Q_OBJECT

public:
   C_SdNdeDpListsTreeWidget(QWidget * const opc_Parent = NULL);
   virtual ~C_SdNdeDpListsTreeWidget(void);
   void SetDataPool(const stw_types::uint32 & oru32_NodeIndex, const stw_types::uint32 & oru32_DataPoolIndex);
   void AddEntry(void);
   void UpdateUI(void);
   void InsertRowWithoutData(const stw_types::uint32 & oru32_TargetRow);
   void Copy(void) const;
   void Cut(void);
   void Paste(void);
   void Delete(void);
   void InsertAction(void);
   void Insert(const bool & orq_SetFocus = true);
   void DoMoveUp(void);
   void DoMoveDown(void);
   void Edit(void) const;
   void PopUp(void) const;
   bool CheckDataPoolFull(void) const;
   void OpenDetail(const stw_types::sint32 os32_ListIndex, const stw_types::sint32 os32_DataElementIndex);
   void UpdateModels(void);
   void HandleErrorChange(void);

   //The signals keyword is necessary for Qt signal slot functionality
   //lint -save -e1736

Q_SIGNALS:
   //lint -restore
   void SigDrop(void);
   void SigErrorChange(const bool & orq_Error);
   void SigSizeChange(void);
   void SigSelectionChanged(const stw_types::uint32 & oru32_Count, const bool & orq_List);
   void SigButtonChange(const bool & orq_AddActive, const bool & orq_CutActive, const bool & orq_CopyActive,
                        const bool & orq_PasteActive, const bool & orq_DeleteActive, const bool & orq_MoveDownActive,
                        const bool & orq_MoveUpActive);
   void SigChanged(void);
   void SigSave(void);
   void SigSaveAs(void);

protected:
   virtual void dropEvent(QDropEvent * const opc_Event) override;
   virtual void dragEnterEvent(QDragEnterEvent * const opc_Event) override;
   virtual void dragMoveEvent(QDragMoveEvent * const opc_Event) override;
   virtual void selectionChanged(const QItemSelection & orc_Selected, const QItemSelection & orc_Deselected) override;
   virtual void keyPressEvent(QKeyEvent * const opc_Event) override;

private:
   stw_opensyde_gui_logic::C_SdNdeUnoDataPoolManager mc_UndoManager;
   stw_opensyde_gui_logic::C_SdNdeDpListModelViewManager mc_ModelViewManager;
   stw_types::uint32 mu32_NodeIndex;
   stw_types::uint32 mu32_DataPoolIndex;
   stw_opensyde_gui_elements::C_OgeContextMenu * mpc_ContextMenu;
   stw_opensyde_gui_logic::C_SdNdeDpListsTreeDelegate mc_Delegate;
   stw_opensyde_gui_logic::C_SdNdeDpListErrorManager mc_ErrorManager;
   QAction * mpc_ActionAdd;
   QAction * mpc_ActionMoveUp;
   QAction * mpc_ActionMoveDown;
   bool mq_AllowAdd;
   bool mq_AllowMoveUp;
   bool mq_AllowMoveDown;
   bool mq_InitialUserSettings;

   void m_OnExpandRequestedHeader(const C_SdNdeDpListHeaderWidget * const opc_Item, const bool oq_Expand);
   void m_OnExpandRequestedIndex(const stw_types::sint32 os32_Index, const bool oq_Expand);
   void m_InitialItemConfigure(QTreeWidgetItem * const opc_Item, const stw_types::sint32 os32_Index);
   void m_Move(const std::vector<stw_types::uint32> & oru32_SourceIndices,
               const std::vector<stw_types::uint32> & oru32_TargetIndices);
   void m_InitFromData(void);
   void m_OnCustomContextMenuRequested(const QPoint & orc_Pos);
   void m_SetupContextMenu(void);
   void m_OnCollapse(const QModelIndex & orc_Index) const;
   void m_OnExpand(const QModelIndex & orc_Index) const;
   void m_HandleDataSetErrorChange(const stw_types::uint32 & oru32_NodeIndex,
                                   const stw_types::uint32 & oru32_DataPoolIndex,
                                   const stw_types::uint32 & oru32_ListIndex) const;
   void m_HandleTableSelection(const stw_types::uint32 & oru32_ListIndex, const stw_types::uint32 & oru32_Count);
   std::vector<stw_types::uint32> m_GetSelectedIndices(void) const;
   void m_OnButtonChange(const bool & orq_AddActive, const bool & orq_CutActive, const bool & orq_CopyActive,
                         const bool & orq_PasteActive, const bool & orq_DeleteActive, const bool & orq_MoveDownActive,
                         const bool & orq_MoveUpActive);
   void m_CheckActions(void);
   C_SdNdeDpListTableWidget * m_GetActiveTable(const bool & orq_IgnoreSelectedItems = false) const;
   QTreeWidgetItem * m_GetActiveTableTreeWidget(const bool & orq_IgnoreSelectedItems = false) const;
   static void mh_AdaptDropTargetIndex(const std::vector<stw_types::uint32> & orc_SelectedIndices,
                                       const DropIndicatorPosition & ore_DropIndicatorPosition,
                                       stw_types::sint32 & ors32_TargetPosition);
   void m_HandleChanged(void);
   void m_UpdateDataSetCount(const stw_types::uint32 & oru32_NodeIndex, const stw_types::uint32 & oru32_DataPoolIndex,
                             const stw_types::uint32 & oru32_ListIndex) const;
   void m_ScrollBarRangeChanged(const stw_types::sintn osn_Min, const stw_types::sintn osn_Max) const;
   stw_types::uint32 m_GetOneAfterHighestSelected(void) const;
   void m_ClearTableSelection(const stw_types::sint32 & ors32_Exception = -1) const;
   void m_HandleExclusiveListSelection(const stw_types::uint32 & oru32_ListIndex) const;

   void m_StoreUserSettings(void) const;
   void m_RestoreUserSettings(void);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif
