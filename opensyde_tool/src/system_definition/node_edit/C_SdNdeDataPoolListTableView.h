//-----------------------------------------------------------------------------
/*!
   \file
   \brief       Data pool list elements table item (header)

   See cpp file for detailed description

   \implementation
   project     openSYDE
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     03.02.2017  STW/M.Echtler
   \endimplementation
*/
//-----------------------------------------------------------------------------
#ifndef C_SDNDEDATAPOOLLISTTABLEVIEW_H
#define C_SDNDEDATAPOOLLISTTABLEVIEW_H

/* -- Includes ------------------------------------------------------------- */

#include <QUndoStack>
#include "stwtypes.h"
#include "C_TblViewScroll.h"
#include "C_PuiSdNodeDataPoolList.h"
#include "C_OSCNodeDataPoolList.h"
#include "C_SdNdeUnoLeDataPoolListManager.h"
#include "C_SdNdeDataPoolListTableDelegate.h"
#include "C_OgeContextMenu.h"
#include "C_SdNdeUnoDataPoolManager.h"
#include "C_SdNdeDataPoolListModelViewManager.h"

/* -- Namespace ------------------------------------------------------------ */
namespace stw_opensyde_gui
{
/* -- Global Constants ----------------------------------------------------- */

/* -- Types ---------------------------------------------------------------- */

//lint -e{1960} Forward declarations necessary for automatic registration
class C_SdNdeDataPoolListTableView :
   public C_TblViewScroll
{
   Q_OBJECT

public:
   C_SdNdeDataPoolListTableView(QWidget * const opc_Parent = NULL);
   ~C_SdNdeDataPoolListTableView(void);
   void SetList(const stw_types::uint32 & oru32_NodeIndex, const stw_types::uint32 & oru32_DataPoolIndex,
                const stw_types::uint32 & oru32_ListIndex);
   bool Equals(const stw_types::uint32 & oru32_NodeIndex, const stw_types::uint32 & oru32_DataPoolIndex,
               const stw_types::uint32 & oru32_ListIndex) const;
   void Copy(void) const;
   void Cut(void);
   void Paste(void);
   void Delete(void);
   void InsertAction(void);
   void Insert(const bool & orq_SetFocus = true);
   void DoMoveUp(void);
   void DoMoveDown(void);
   void EditInitial(void);
   void SetUndoStack(stw_opensyde_gui_logic::C_SdNdeUnoDataPoolManager * const opc_Value);
   void OnErrorChangePossible(void);
   void OnSizeChangePossible(void);
   void OnDataChangeElements(const stw_types::uint32 & oru32_DataPoolListElementIndex, const QVariant & orc_NewData,
                             const stw_opensyde_gui_logic::C_SdNdeDataPoolUtil::E_ElementDataChangeType & ore_DataChangeType, const stw_types::uint32 & oru32_ArrayIndex, const stw_types::sint32 & ors32_DataSetIndex);
   void SetModelViewManager(stw_opensyde_gui_logic::C_SdNdeDataPoolListModelViewManager * const opc_Value);
   void SelectDataElement(const stw_types::sint32 os32_DataElementIndex);
   void SelectRange(const stw_types::uint32 ou32_FirstRow, const stw_types::uint32 ou32_LastRow) const;
   bool IsSelected(void) const;
   void UpdateSelectionForNewDataSetColumn(const stw_types::sint32 os32_DataSetColumn);
   void Reset(void);
   void SetSelectedVariableNames(const std::vector<QString> & orc_VariableNames);
   std::vector<QString> GetSelectedVariableNames(void) const;

   //The signals keyword is necessary for Qt signal slot functionality
   //lint -save -e1736

Q_SIGNALS:
   //lint -restore
   void SigErrorChangePossible(void);
   void SigSizeChangePossible(void);
   void SigSelectionChanged(const stw_types::uint32 & oru32_Count);
   void SigButtonChange(const bool & orq_AddActive, const bool & orq_CutActive, const bool & orq_CopyActive,
                        const bool & orq_PasteActive, const bool & orq_DeleteActive, const bool & orq_MoveDownActive,
                        const bool & orq_MoveUpActive);

protected:
   // The naming of the Qt parameters can't be changed and are not compliant with the naming conventions
   //lint -save -e1960
   virtual void keyPressEvent(QKeyEvent * const opc_Event) override;
   virtual void dropEvent(QDropEvent * const opc_Event) override;
   virtual void selectionChanged(const QItemSelection & orc_Selected, const QItemSelection & orc_Deselected) override;
   virtual void startDrag(const Qt::DropActions oc_SupportedActions) override;
   //lint -restore

private:
   stw_types::uint32 mu32_NodeIndex;
   stw_types::uint32 mu32_DataPoolIndex;
   stw_types::uint32 mu32_ListIndex;
   stw_types::sint32 ms32_LastIndicatorSize;
   stw_opensyde_gui_logic::C_SdNdeUnoLeDataPoolListManager mc_UndoManager;
   stw_opensyde_gui_logic::C_SdNdeDataPoolListTableDelegate mc_Delegate;
   stw_opensyde_gui_elements::C_OgeContextMenu * mpc_ContextMenu;
   stw_opensyde_gui_logic::C_SdNdeDataPoolListModelViewManager * mpc_ModelViewManager;
   QAction * mpc_ActionMoveUp;
   QAction * mpc_ActionMoveDown;
   QAction * mpc_ActionAdd;
   QAction * mpc_ActionCut;
   QAction * mpc_ActionCopy;
   QAction * mpc_ActionPaste;
   QAction * mpc_ActionDelete;
   bool mq_AllowMoveUp;
   bool mq_AllowMoveDown;
   bool mq_AllowAdd;

   void m_DeleteIndices(const std::vector<stw_types::uint32> & orc_Indices);
   void m_Move(const std::vector<stw_types::uint32> & oru32_SourceIndices,
               const std::vector<stw_types::uint32> & oru32_TargetIndices);
   std::vector<stw_types::uint32> m_GetSelectedIndices(void) const;
   void m_SetupContextMenu(void);
   void m_OnCustomContextMenuRequested(const QPoint & orc_Pos);
   void m_HandleColumnChange(void);
   void m_CheckActions(const std::vector<stw_types::uint32> & orc_SelectedIndices);
   void m_UpdateModelView(void);
   void m_HandleLinkClicked(const QModelIndex & orc_Index);
   void m_CheckContextMenuText(void);
   void m_OnColumnResize(void);
   stw_types::uint32 m_GetOneAfterHighestSelected(void);
   void m_HandleColumnStateSave(void) const;
   void m_HandleColumnStateRestore(void);
   //Avoid call
   C_SdNdeDataPoolListTableView(const C_SdNdeDataPoolListTableView &);
   C_SdNdeDataPoolListTableView & operator =(const C_SdNdeDataPoolListTableView &);
};

/* -- Extern Global Variables ---------------------------------------------- */
} //end of namespace

#endif
