//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Data pool list elements table item (header)

   See cpp file for detailed description

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_SDNDEDPLISTTABLEVIEW_HPP
#define C_SDNDEDPLISTTABLEVIEW_HPP

/* -- Includes ------------------------------------------------------------------------------------------------------ */

#include <QUndoStack>
#include "stwtypes.hpp"
#include "C_TblViewScroll.hpp"
#include "C_PuiSdNodeDataPoolList.hpp"
#include "C_OscNodeDataPoolList.hpp"
#include "C_SdNdeUnoLeDataPoolListManager.hpp"
#include "C_SdNdeDpListTableDelegate.hpp"
#include "C_OgeContextMenu.hpp"
#include "C_SdNdeUnoDataPoolManager.hpp"
#include "C_SdNdeDpListModelViewManager.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_gui
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_SdNdeDpListTableView :
   public C_TblViewScroll
{
   Q_OBJECT

public:
   C_SdNdeDpListTableView(QWidget * const opc_Parent = NULL);
   ~C_SdNdeDpListTableView(void) override;
   void SetList(const uint32_t & oru32_NodeIndex, const uint32_t & oru32_DataPoolIndex,
                const uint32_t & oru32_ListIndex);
   bool Equals(const uint32_t & oru32_NodeIndex, const uint32_t & oru32_DataPoolIndex,
               const uint32_t & oru32_ListIndex) const;
   void Copy(void) const;
   void Cut(void);
   void Paste(void);
   void Delete(void);
   void InsertAction(void);
   void Insert(const bool & orq_SetFocus = true);
   void DoMoveUp(void);
   void DoMoveDown(void);
   void EditInitial(void);
   void SetUndoStack(stw::opensyde_gui_logic::C_SdNdeUnoDataPoolManager * const opc_Value);
   void OnErrorChangePossible(void);
   void OnSizeChangePossible(void);
   void OnDataChangeElements(const uint32_t & oru32_DataPoolListElementIndex, const QVariant & orc_NewData,
                             const stw::opensyde_gui_logic::C_SdNdeDpUtil::E_ElementDataChangeType & ore_DataChangeType,
                             const uint32_t & oru32_ArrayIndex, const int32_t & ors32_DataSetIndex);
   void SetModelViewManager(stw::opensyde_gui_logic::C_SdNdeDpListModelViewManager * const opc_Value);
   void SelectDataElement(const int32_t os32_DataElementIndex);
   void SelectRange(const uint32_t ou32_FirstRow, const uint32_t ou32_LastRow) const;
   bool IsSelected(void) const;
   void UpdateSelectionForNewDataSetColumn(const int32_t os32_DataSetColumn);
   void Reset(void);
   void SetSelectedVariableNames(const std::vector<QString> & orc_VariableNames);
   std::vector<QString> GetSelectedVariableNames(void) const;

   //The signals keyword is necessary for Qt signal slot functionality
   //lint -save -e1736

Q_SIGNALS:
   //lint -restore
   void SigErrorChangePossible(void);
   void SigSizeChangePossible(void);
   void SigSelectionChanged(const uint32_t & oru32_Count);
   void SigButtonChange(const bool & orq_AddActive, const bool & orq_CutActive, const bool & orq_CopyActive,
                        const bool & orq_PasteActive, const bool & orq_DeleteActive, const bool & orq_MoveDownActive,
                        const bool & orq_MoveUpActive);

protected:
   void keyPressEvent(QKeyEvent * const opc_Event) override;
   void dropEvent(QDropEvent * const opc_Event) override;
   void selectionChanged(const QItemSelection & orc_Selected, const QItemSelection & orc_Deselected) override;
   void startDrag(const Qt::DropActions oc_SupportedActions) override;

private:
   uint32_t mu32_NodeIndex;
   uint32_t mu32_DataPoolIndex;
   uint32_t mu32_ListIndex;
   int32_t ms32_LastIndicatorSize;
   stw::opensyde_gui_logic::C_SdNdeUnoLeDataPoolListManager mc_UndoManager;
   stw::opensyde_gui_logic::C_SdNdeDpListTableDelegate mc_Delegate;
   stw::opensyde_gui_elements::C_OgeContextMenu * mpc_ContextMenu;
   stw::opensyde_gui_logic::C_SdNdeDpListModelViewManager * mpc_ModelViewManager;
   QAction * mpc_ActionMoveUp;
   QAction * mpc_ActionMoveDown;
   QAction * mpc_ActionAdd;
   QAction * mpc_ActionCut;
   QAction * mpc_ActionCopy;
   QAction * mpc_ActionPaste;
   QAction * mpc_ActionDelete;
   QAction * mpc_ActionEditComment;
   bool mq_AllowMoveUp;
   bool mq_AllowMoveDown;
   bool mq_AllowAdd;

   void m_DeleteIndices(const std::vector<uint32_t> & orc_Indices);
   void m_Move(const std::vector<uint32_t> & orc_SourceIndices, const std::vector<uint32_t> & orc_TargetIndices);
   std::vector<uint32_t> m_GetSelectedIndices(void) const;
   void m_SetupContextMenu(void);
   void m_OnCustomContextMenuRequested(const QPoint & orc_Pos);
   void m_HandleColumnChange(void);
   void m_CheckActions(const std::vector<uint32_t> & orc_SelectedIndices);
   void m_UpdateModelView(void);
   void m_HandleLinkClicked(const QModelIndex & orc_Index);
   void m_CheckContextMenuText(void);
   void m_OnColumnResize(void);
   uint32_t m_GetOneAfterHighestSelected(void);
   void m_HandleColumnStateSave(void) const;
   void m_HandleColumnStateRestore(void);
   void m_CommentConfirmed(void);
   //Avoid call
   C_SdNdeDpListTableView(const C_SdNdeDpListTableView &);
   C_SdNdeDpListTableView & operator =(const C_SdNdeDpListTableView &) &;

   void m_EditCommentInEditor(void);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
