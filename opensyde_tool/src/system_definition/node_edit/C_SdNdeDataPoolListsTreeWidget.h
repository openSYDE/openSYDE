//-----------------------------------------------------------------------------
/*!
   \file
   \brief       Widget to store and manage multiple data pool list entries (header)

   See cpp file for detailed description

   \implementation
   project     openSYDE
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     17.01.2017  STW/M.Echtler
   \endimplementation
*/
//-----------------------------------------------------------------------------
#ifndef C_SDNDEDATAPOOLLISTSTREEWIDGET_H
#define C_SDNDEDATAPOOLLISTSTREEWIDGET_H

/* -- Includes ------------------------------------------------------------- */

#include <QTreeWidget>
#include <QDropEvent>
#include <QKeyEvent>
#include <vector>
#include "stwtypes.h"
#include "C_SdNdeDataPoolListHeaderWidget.h"
#include "C_SdNdeUnoDataPoolManager.h"
#include "C_OgeContextMenu.h"
#include "C_SdNdeDataPoolListTableWidget.h"
#include "C_SdNdeDataPoolListsTreeDelegate.h"
#include "C_SdNdeDataPoolListModelViewManager.h"
#include "C_SdNdeDataPoolListErrorManager.h"

/* -- Namespace ------------------------------------------------------------ */
namespace stw_opensyde_gui
{
/* -- Global Constants ----------------------------------------------------- */

/* -- Types ---------------------------------------------------------------- */

//lint -e{1960} Forward declarations necessary for automatic registration
class C_SdNdeDataPoolListsTreeWidget :
   public QTreeWidget
{
   Q_OBJECT

public:
   C_SdNdeDataPoolListsTreeWidget(QWidget * const opc_Parent = NULL);
   virtual ~C_SdNdeDataPoolListsTreeWidget(void);
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
   void RegisterSizeChange(void);
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
   // The naming of the Qt parameters can't be changed and are not compliant with the naming conventions
   //lint -save -e1960
   virtual void dropEvent(QDropEvent * const opc_Event) override;
   virtual void dragEnterEvent(QDragEnterEvent * const opc_Event) override;
   virtual void dragMoveEvent(QDragMoveEvent * const opc_Event) override;
   virtual void selectionChanged(const QItemSelection & orc_Selected, const QItemSelection & orc_Deselected) override;
   virtual void keyPressEvent(QKeyEvent * const opc_Event) override;
   virtual void startDrag(const Qt::DropActions oc_SupportedActions) override;
   //lint -restore

private:
   std::vector<C_SdNdeDataPoolListHeaderWidget *> mc_Widgets;
   stw_opensyde_gui_logic::C_SdNdeUnoDataPoolManager mc_UndoManager;
   stw_opensyde_gui_logic::C_SdNdeDataPoolListModelViewManager mc_ModelViewManager;
   stw_types::uint32 mu32_NodeIndex;
   stw_types::uint32 mu32_DataPoolIndex;
   stw_opensyde_gui_elements::C_OgeContextMenu * mpc_ContextMenu;
   stw_opensyde_gui_logic::C_SdNdeDataPoolListsTreeDelegate mc_Delegate;
   stw_opensyde_gui_logic::C_SdNdeDataPoolListErrorManager mc_ErrorManager;
   QAction * mpc_ActionAdd;
   QAction * mpc_ActionMoveUp;
   QAction * mpc_ActionMoveDown;
   bool mq_AllowAdd;
   bool mq_AllowMoveUp;
   bool mq_AllowMoveDown;
   bool mq_InitialUserSettings;

   void m_OnExpandRequested(QTreeWidgetItem * const opc_Item, const bool oq_Expand);
   void m_InitialItemConfigure(QTreeWidgetItem * const opc_Item, const stw_types::sint32 os32_Index);
   void m_Move(const std::vector<stw_types::uint32> & oru32_SourceIndices,
               const std::vector<stw_types::uint32> & oru32_TargetIndices);
   void m_InitFromData(void);
   void m_UpdateAddress(void);
   void m_OnCustomContextMenuRequested(const QPoint & orc_Pos);
   void m_SetupContextMenu(void);
   void m_OnCollapse(const QModelIndex & orc_Index) const;
   void m_OnExpand(const QModelIndex & orc_Index) const;
   void m_HandleDataSetErrorChange(const stw_types::uint32 & oru32_NodeIndex,
                                   const stw_types::uint32 & oru32_DataPoolIndex,
                                   const stw_types::uint32 & oru32_ListIndex) const;
   void m_HandleSizeChange(void);
   void m_HandleTableSelection(const stw_types::uint32 & oru32_ListIndex, const stw_types::uint32 & oru32_Count);
   std::vector<stw_types::uint32> m_GetSelectedIndices(void) const;
   void m_OnButtonChange(const bool & orq_AddActive, const bool & orq_CutActive, const bool & orq_CopyActive,
                         const bool & orq_PasteActive, const bool & orq_DeleteActive, const bool & orq_MoveDownActive,
                         const bool & orq_MoveUpActive);
   void m_CheckActions(void);
   C_SdNdeDataPoolListTableWidget * m_GetActiveTable(const bool & orq_IgnoreSelectedItems = false) const;
   QTreeWidgetItem * m_GetActiveTableTreeWidget(const bool & orq_IgnoreSelectedItems = false) const;
   static void mh_AdaptDropTargetIndex(const std::vector<stw_types::uint32> & orc_SelectedIndices,
                                       const DropIndicatorPosition & ore_DropIndicatorPosition,
                                       stw_types::sint32 & ors32_TargetPosition);
   void m_HandleChanged(void);
   void m_UpdateDataSetCount(const stw_types::uint32 & oru32_NodeIndex, const stw_types::uint32 & oru32_DataPoolIndex,
                             const stw_types::uint32 & oru32_ListIndex) const;
   void m_ScrollBarRangeChanged(const stw_types::sintn osn_Min, const stw_types::sintn osn_Max) const;
   stw_types::uint32 m_GetOneAfterHighestSelected(void) const;
   void m_OnErrorChangePossible(const bool & orq_Error);
   void m_ClearTableSelection(const stw_types::sint32 & ors32_Exception = -1) const;
   void m_HandleExclusiveListSelection(const stw_types::uint32 & oru32_ListIndex) const;

   void m_StoreUserSettings(void) const;
   void m_RestoreUserSettings(void);
};

/* -- Extern Global Variables ---------------------------------------------- */
} //end of namespace

#endif
