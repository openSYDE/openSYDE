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
#include "stwtypes.hpp"
#include "C_SdNdeDpListHeaderWidget.hpp"
#include "C_SdNdeUnoDataPoolManager.hpp"
#include "C_OgeContextMenu.hpp"
#include "C_SdNdeDpListTableWidget.hpp"
#include "C_SdNdeDpListsTreeDelegate.hpp"
#include "C_SdNdeDpListModelViewManager.hpp"
#include "C_SdNdeDpListErrorManager.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_gui
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_SdNdeDpListsTreeWidget :
   public QTreeWidget
{
   Q_OBJECT

public:
   C_SdNdeDpListsTreeWidget(QWidget * const opc_Parent = NULL);
   ~C_SdNdeDpListsTreeWidget(void) override;
   void SetDataPool(const uint32_t & oru32_NodeIndex, const uint32_t & oru32_DataPoolIndex);
   void AddEntry(void);
   void UpdateUi(void);
   void InsertRowWithoutData(const uint32_t & oru32_TargetRow);
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
   void OpenDetail(const int32_t os32_ListIndex, const int32_t os32_DataElementIndex);
   void UpdateModels(void);
   void HandleErrorChange(void);

   //The signals keyword is necessary for Qt signal slot functionality
   //lint -save -e1736

Q_SIGNALS:
   //lint -restore
   void SigDrop(void);
   void SigErrorChange(const bool & orq_Error);
   void SigSizeChange(void);
   void SigSelectionChanged(const uint32_t & oru32_Count, const bool & orq_List);
   void SigButtonChange(const bool & orq_AddActive, const bool & orq_CutActive, const bool & orq_CopyActive,
                        const bool & orq_PasteActive, const bool & orq_DeleteActive, const bool & orq_MoveDownActive,
                        const bool & orq_MoveUpActive);
   void SigChanged(void);
   void SigSave(void);
   void SigSaveAs(void);

protected:
   void dropEvent(QDropEvent * const opc_Event) override;
   void dragEnterEvent(QDragEnterEvent * const opc_Event) override;
   void dragMoveEvent(QDragMoveEvent * const opc_Event) override;
   void selectionChanged(const QItemSelection & orc_Selected, const QItemSelection & orc_Deselected) override;
   void keyPressEvent(QKeyEvent * const opc_Event) override;

private:
   stw::opensyde_gui_logic::C_SdNdeUnoDataPoolManager mc_UndoManager;
   stw::opensyde_gui_logic::C_SdNdeDpListModelViewManager mc_ModelViewManager;
   uint32_t mu32_NodeIndex;
   uint32_t mu32_DataPoolIndex;
   stw::opensyde_gui_elements::C_OgeContextMenu * mpc_ContextMenu;
   stw::opensyde_gui_logic::C_SdNdeDpListsTreeDelegate mc_Delegate;
   stw::opensyde_gui_logic::C_SdNdeDpListErrorManager mc_ErrorManager;
   QAction * mpc_ActionAdd;
   QAction * mpc_ActionMoveUp;
   QAction * mpc_ActionMoveDown;
   bool mq_AllowAdd;
   bool mq_AllowMoveUp;
   bool mq_AllowMoveDown;
   bool mq_InitialUserSettings;

   void m_OnExpandRequestedHeader(const C_SdNdeDpListHeaderWidget * const opc_Item, const bool oq_Expand);
   void m_OnExpandRequestedIndex(const int32_t os32_Index, const bool oq_Expand);
   void m_InitialItemConfigure(QTreeWidgetItem * const opc_Item, const int32_t os32_Index);
   void m_Move(const std::vector<uint32_t> & orc_SourceIndices, const std::vector<uint32_t> & orc_TargetIndices);
   void m_InitFromData(void);
   void m_OnCustomContextMenuRequested(const QPoint & orc_Pos);
   void m_SetupContextMenu(void);
   void m_OnCollapse(const QModelIndex & orc_Index) const;
   void m_OnExpand(const QModelIndex & orc_Index) const;
   void m_HandleDataSetErrorChange(const uint32_t & oru32_NodeIndex, const uint32_t & oru32_DataPoolIndex,
                                   const uint32_t & oru32_ListIndex) const;
   void m_HandleTableSelection(const uint32_t & oru32_ListIndex, const uint32_t & oru32_Count);
   std::vector<uint32_t> m_GetSelectedIndices(void) const;
   void m_OnButtonChange(const bool & orq_AddActive, const bool & orq_CutActive, const bool & orq_CopyActive,
                         const bool & orq_PasteActive, const bool & orq_DeleteActive, const bool & orq_MoveDownActive,
                         const bool & orq_MoveUpActive);
   void m_CheckActions(void);
   C_SdNdeDpListTableWidget * m_GetActiveTable(const bool & orq_IgnoreSelectedItems = false) const;
   QTreeWidgetItem * m_GetActiveTableTreeWidget(const bool & orq_IgnoreSelectedItems = false) const;
   static void mh_AdaptDropTargetIndex(const std::vector<uint32_t> & orc_SelectedIndices,
                                       const DropIndicatorPosition & ore_DropIndicatorPosition,
                                       int32_t & ors32_TargetPosition);
   void m_HandleChanged(void);
   void m_UpdateDataSetCount(const uint32_t & oru32_NodeIndex, const uint32_t & oru32_DataPoolIndex,
                             const uint32_t & oru32_ListIndex) const;
   void m_ScrollBarRangeChanged(const int32_t os32_Min, const int32_t os32_Max) const;
   uint32_t m_GetOneAfterHighestSelected(void) const;
   void m_ClearTableSelection(const int32_t & ors32_Exception = -1) const;
   void m_HandleExclusiveListSelection(const uint32_t & oru32_ListIndex) const;

   void m_StoreUserSettings(void) const;
   void m_RestoreUserSettings(void);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
