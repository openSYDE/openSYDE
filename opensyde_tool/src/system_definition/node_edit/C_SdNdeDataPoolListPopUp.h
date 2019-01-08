//-----------------------------------------------------------------------------
/*!
   \file
   \brief       List edit pop up widget (header)

   See cpp file for detailed description

   \implementation
   project     openSYDE
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     01.06.2017  STW/M.Echtler
   \endimplementation
*/
//-----------------------------------------------------------------------------
#ifndef C_SDNDEDATAPOOLLISTPOPUP_H
#define C_SDNDEDATAPOOLLISTPOPUP_H

/* -- Includes ------------------------------------------------------------- */
#include <QWidget>
#include <QTreeWidget>
#include "stwtypes.h"
#include "C_OgePopUpDialog.h"
#include "C_SdNdeDataPoolListModelViewManager.h"
#include "C_SdNdeUnoDataPoolManager.h"

/* -- Namespace ------------------------------------------------------------ */
namespace Ui
{
class C_SdNdeDataPoolListPopUp;
}

namespace stw_opensyde_gui
{
/* -- Global Constants ----------------------------------------------------- */

/* -- Types ---------------------------------------------------------------- */
class C_SdNdeDataPoolListPopUp :
   public QWidget
{
   Q_OBJECT

public:
   explicit C_SdNdeDataPoolListPopUp(stw_opensyde_gui_elements::C_OgePopUpDialog & orc_Parent,
                                     const stw_types::uint32 & oru32_NodeIndex,
                                     const stw_types::uint32 & oru32_DataPoolIndex,
                                     const stw_types::uint32 & oru32_ListIndex,
                                     stw_opensyde_gui_logic::C_SdNdeDataPoolListModelViewManager * const opc_ModelViewManager, QTreeWidget * const opc_TreeWidget, stw_opensyde_gui_logic::C_SdNdeUnoDataPoolManager * const opc_UndoManager,
                                     QWidget * const opc_Parent = NULL);
   ~C_SdNdeDataPoolListPopUp(void);
   void InitText(void) const;
   void Clear(void) const;
   void UpdateHeader(void) const;
   bool GetSaveAsAfterClose(void) const;

   //The signals keyword is necessary for Qt signal slot functionality
   //lint -save -e1736

Q_SIGNALS:
   //lint -restore
   void SigSave(void);

protected:
   // The naming of the Qt parameters can't be changed and are not compliant with the naming conventions
   //lint -save -e1960
   virtual void paintEvent(QPaintEvent * const opc_Event) override;
   virtual void keyPressEvent(QKeyEvent * const opc_Event) override;
   //lint -restore

private:
   Ui::C_SdNdeDataPoolListPopUp * mpc_Ui;
   //lint -e{1725} Only problematic if copy or assignment is allowed
   stw_opensyde_gui_elements::C_OgePopUpDialog & mrc_Parent;
   const stw_types::uint32 mu32_NodeIndex;
   const stw_types::uint32 mu32_DataPoolIndex;
   const stw_types::uint32 mu32_ListIndex;
   stw_opensyde_gui_logic::C_SdNdeDataPoolListModelViewManager * const mpc_ModelViewManager;
   QTreeWidget * const mpc_TreeWidget;
   stw_opensyde_gui_logic::C_SdNdeUnoDataPoolManager * const mpc_UndoManager;
   stw_types::sint32 ms32_UndoStartCount;
   bool mq_SaveAsAfterClose;

   void m_InitButtonIcons(void) const;
   void m_Close(void);
   void m_HandleSelection(const stw_types::uint32 & oru32_ListIndex, const stw_types::uint32 & oru32_Count) const;
   void m_HandleButtonChange(const bool & orq_AddActive, const bool & orq_CutActive, const bool & orq_CopyActive,
                             const bool & orq_PasteActive, const bool & orq_DeleteActive,
                             const bool & orq_MoveDownActive, const bool & orq_MoveUpActive) const;
   //Avoid call
   C_SdNdeDataPoolListPopUp(const C_SdNdeDataPoolListPopUp &);
   C_SdNdeDataPoolListPopUp & operator =(const C_SdNdeDataPoolListPopUp &);
};

/* -- Extern Global Variables ---------------------------------------------- */
} //end of namespace

#endif // C_SDNDEDATAPOOLLISTPOPUP_H
