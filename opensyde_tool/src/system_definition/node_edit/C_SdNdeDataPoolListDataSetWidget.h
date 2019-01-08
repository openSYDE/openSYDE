//-----------------------------------------------------------------------------
/*!
   \file
   \brief       Data set edit widget (header)

   See cpp file for detailed description

   \implementation
   project     openSYDE
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     15.03.2017  STW/M.Echtler
   \endimplementation
*/
//-----------------------------------------------------------------------------
#ifndef C_SDNDEDATAPOOLLISTDATASETWIDGET_H
#define C_SDNDEDATAPOOLLISTDATASETWIDGET_H

/* -- Includes ------------------------------------------------------------- */

#include <QWidget>
#include <QUndoCommand>
#include "stwtypes.h"
#include "C_OgePopUpDialog.h"
#include "C_SdNdeDataPoolListModelViewManager.h"
#include "C_OgeContextMenu.h"

/* -- Namespace ------------------------------------------------------------ */

namespace Ui
{
class C_SdNdeDataPoolListDataSetWidget;
}

namespace stw_opensyde_gui
{
/* -- Global Constants ----------------------------------------------------- */

/* -- Types ---------------------------------------------------------------- */

class C_SdNdeDataPoolListDataSetWidget :
   public QWidget
{
   Q_OBJECT

public:
   explicit C_SdNdeDataPoolListDataSetWidget(stw_opensyde_gui_elements::C_OgePopUpDialog & orc_Parent,
                                             const stw_types::uint32 & oru32_NodeIndex,
                                             const stw_types::uint32 & oru32_DataPoolIndex,
                                             const stw_types::uint32 & oru32_ListIndex);
   ~C_SdNdeDataPoolListDataSetWidget(void);
   void InitStaticNames(void) const;
   QUndoCommand * TakeUndoCommand(void) const;
   void SetModelViewManager(stw_opensyde_gui_logic::C_SdNdeDataPoolListModelViewManager * const opc_Value) const;

protected:
   // The naming of the Qt parameters can't be changed and are not compliant with the naming conventions
   //lint -save -e1960
   virtual void keyPressEvent(QKeyEvent * const opc_Event) override;
   //lint -restore

private:
   Ui::C_SdNdeDataPoolListDataSetWidget * const mpc_Ui;
   stw_opensyde_gui_elements::C_OgeContextMenu * mpc_ContextMenu;
   QAction * mpc_ActionAdd;
   QAction * mpc_ActionDelete;
   QAction * mpc_ActionCopy;
   QAction * mpc_ActionPaste;
   QAction * mpc_ActionCut;
   QAction * mpc_ActionMoveLeft;
   QAction * mpc_ActionMoveRight;
   //lint -e{1725} Only problematic if copy or assignment is allowed
   stw_opensyde_gui_elements::C_OgePopUpDialog & mrc_Parent;
   const stw_types::uint32 mu32_NodeIndex;
   const stw_types::uint32 mu32_DataPoolIndex;
   const stw_types::uint32 mu32_ListIndex;

   void m_InitButtonIcons(void) const;
   void m_OkClicked(void);
   void m_CancelClicked(void);
   void m_HandleButtonChange(const bool & orq_AddActive, const bool & orq_CutActive, const bool & orq_CopyActive,
                             const bool & orq_PasteActive, const bool & orq_DeleteActive,
                             const bool & orq_MoveLeftActive, const bool & orq_MoveRightActive);
   void m_OnDataSetCountChange(const stw_types::sint32 & ors32_NewColumnCount) const;
   void m_OnDataSetSelectionChange(const stw_types::uint32 & oru32_SelectionCount) const;
   void m_SetupContextMenu(void);
   void m_OnCustomContextMenuRequested(const QPoint & orc_Pos);
   void m_DoInsert(void) const;
   //Avoid call
   C_SdNdeDataPoolListDataSetWidget(const C_SdNdeDataPoolListDataSetWidget &);
   C_SdNdeDataPoolListDataSetWidget & operator =(const C_SdNdeDataPoolListDataSetWidget &);
};

/* -- Extern Global Variables ---------------------------------------------- */
} //end of namespace

#endif
