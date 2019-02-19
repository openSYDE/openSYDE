//-----------------------------------------------------------------------------
/*!
   \file
   \brief       Widget to display one data pool list content (header)

   See cpp file for detailed description

   \implementation
   project     openSYDE
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     24.01.2017  STW/M.Echtler
   \endimplementation
*/
//-----------------------------------------------------------------------------
#ifndef C_SDNDEDATAPOOLLISTTABLEWIDGET_H
#define C_SDNDEDATAPOOLLISTTABLEWIDGET_H

/* -- Includes ------------------------------------------------------------- */

#include <QWidget>
#include <QTreeWidgetItem>
#include <QUndoStack>
#include "stwtypes.h"
#include "C_PuiSdNodeDataPoolList.h"
#include "C_OSCNodeDataPoolList.h"
#include "C_SdNdeUnoDataPoolManager.h"
#include "C_SdNdeDataPoolListModelViewManager.h"

/* -- Namespace ------------------------------------------------------------ */

namespace Ui
{
class C_SdNdeDataPoolListTableWidget;
}

namespace stw_opensyde_gui
{
/* -- Global Constants ----------------------------------------------------- */

/* -- Types ---------------------------------------------------------------- */
class C_SdNdeDataPoolListTableWidget :
   public QWidget
{
   Q_OBJECT

public:
   explicit C_SdNdeDataPoolListTableWidget(QWidget * const opc_Parent = NULL, QTreeWidget * const opc_TreeWidget = NULL, stw_opensyde_gui_logic::C_SdNdeUnoDataPoolManager * const opc_UndoStack =
                                              NULL, const bool oq_PopUp=true);
   ~C_SdNdeDataPoolListTableWidget(void);
   void InitStaticNames(void) const;
   void SetTreeWidgetAndUndoManager(QTreeWidget * const opc_TreeWidget,
                                    stw_opensyde_gui_logic::C_SdNdeUnoDataPoolManager * const opc_UndoStack);
   void SetList(const stw_types::uint32 & oru32_NodeIndex, const stw_types::uint32 & oru32_DataPoolIndex,
                const stw_types::uint32 & oru32_ListIndex);
   void ClearSelection(void) const;
   void Copy(void) const;
   void Cut(void) const;
   void Paste(void) const;
   void Delete(void) const;
   void InsertAction(void) const;
   void Insert(const bool & orq_SetFocus = true) const;
   void DoMoveUp(void) const;
   void DoMoveDown(void) const;
   void Edit(void) const;
   void SetModelViewManager(stw_opensyde_gui_logic::C_SdNdeDataPoolListModelViewManager * const opc_Value) const;
   void SelectDataElement(const stw_types::sint32 os32_DataElementIndex) const;
   bool IsSelected(void) const;
   void Reset(void);
   void SetSelectedVariableNames(const std::vector<QString> & orc_VariableNames) const;
   std::vector<QString> GetSelectedVariableNames(void) const;

   //The signals keyword is necessary for Qt signal slot functionality
   //lint -save -e1736

Q_SIGNALS:
   //lint -restore
   void SigErrorChangePossible(void);
   void SigSizeChangePossible(void);
   void SigSelectionChanged(const stw_types::uint32 & oru32_ListIndex, const stw_types::uint32 & oru32_Count);
   void SigButtonChange(const bool & orq_AddActive, const bool & orq_CutActive, const bool & orq_CopyActive,
                        const bool & orq_PasteActive, const bool & orq_DeleteActive, const bool & orq_MoveDownActive,
                        const bool & orq_MoveUpActive);

private:
   Ui::C_SdNdeDataPoolListTableWidget * mpc_Ui;
   QTreeWidget * mpc_TreeWidget;
   stw_opensyde_gui_logic::C_SdNdeUnoDataPoolManager * mpc_UndoStack;
   stw_types::uint32 mu32_NodeIndex;
   stw_types::uint32 mu32_DataPoolIndex;
   stw_types::uint32 mu32_ListIndex;
   bool mq_PopUp;

   void m_OnErrorChangePossible(void);
   void m_OnSizeChangePossible(void);
   void m_OnSelectionChanged(const stw_types::uint32 & oru32_Count);
   void m_OnButtonChange(const bool & orq_AddActive, const bool & orq_CutActive, const bool & orq_CopyActive,
                         const bool & orq_PasteActive, const bool & orq_DeleteActive, const bool & orq_MoveDownActive,
                         const bool & orq_MoveUpActive);
   void m_OnCRCOptionChange(const bool & orq_Value);
   void m_InitCRCOption(void);
   void m_SetCRCVisibility(const bool & orq_Visible) const;
   void m_AdjustToItems(const bool & orq_Initial = false);
   //Avoid call
   C_SdNdeDataPoolListTableWidget(const C_SdNdeDataPoolListTableWidget &);
   C_SdNdeDataPoolListTableWidget & operator =(const C_SdNdeDataPoolListTableWidget &);
};

/* -- Extern Global Variables ---------------------------------------------- */
} //end of namespace

#endif
