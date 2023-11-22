//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Data set edit widget (header)

   See cpp file for detailed description

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_SDNDEDPLISTDATASETWIDGET_HPP
#define C_SDNDEDPLISTDATASETWIDGET_HPP

/* -- Includes ------------------------------------------------------------------------------------------------------ */

#include <QWidget>
#include <QUndoCommand>
#include "stwtypes.hpp"
#include "C_OgePopUpDialog.hpp"
#include "C_SdNdeDpListModelViewManager.hpp"
#include "C_OgeContextMenu.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */

namespace Ui
{
class C_SdNdeDpListDataSetWidget;
}

namespace stw
{
namespace opensyde_gui
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_SdNdeDpListDataSetWidget :
   public QWidget
{
   Q_OBJECT

public:
   explicit C_SdNdeDpListDataSetWidget(stw::opensyde_gui_elements::C_OgePopUpDialog & orc_Parent,
                                       const uint32_t & oru32_NodeIndex, const uint32_t & oru32_DataPoolIndex,
                                       const uint32_t & oru32_ListIndex);
   ~C_SdNdeDpListDataSetWidget(void) override;
   void InitStaticNames(void) const;
   QUndoCommand * TakeUndoCommand(void) const;
   void SetModelViewManager(stw::opensyde_gui_logic::C_SdNdeDpListModelViewManager * const opc_Value) const;

protected:
   void keyPressEvent(QKeyEvent * const opc_Event) override;

private:
   Ui::C_SdNdeDpListDataSetWidget * const mpc_Ui;
   stw::opensyde_gui_elements::C_OgeContextMenu * mpc_ContextMenu;
   QAction * mpc_ActionAdd;
   QAction * mpc_ActionDelete;
   QAction * mpc_ActionCopy;
   QAction * mpc_ActionPaste;
   QAction * mpc_ActionCut;
   QAction * mpc_ActionMoveLeft;
   QAction * mpc_ActionMoveRight;
   //lint -e{1725} Only problematic if copy or assignment is allowed
   stw::opensyde_gui_elements::C_OgePopUpDialog & mrc_Parent;
   const uint32_t mu32_NodeIndex;
   const uint32_t mu32_DataPoolIndex;
   const uint32_t mu32_ListIndex;

   bool m_IsThereStillSpaceForDataSets(const uint32_t ou32_NumberOfNewDataSets);

   void m_InitButtonIcons(void) const;
   void m_OkClicked(void);
   void m_CancelClicked(void);
   void m_HandleButtonChange(const bool & orq_AddActive, const bool & orq_CutActive, const bool & orq_CopyActive,
                             const bool & orq_PasteActive, const bool & orq_DeleteActive,
                             const bool & orq_MoveLeftActive, const bool & orq_MoveRightActive);
   void m_OnDataSetCountChange(const int32_t & ors32_NewColumnCount) const;
   void m_OnDataSetSelectionChange(const uint32_t & oru32_SelectionCount) const;
   void m_SetupContextMenu(void);
   void m_OnCustomContextMenuRequested(const QPoint & orc_Pos);
   void m_DoInsert(void);
   void m_DoPaste(void);
   //Avoid call
   C_SdNdeDpListDataSetWidget(const C_SdNdeDpListDataSetWidget &);
   C_SdNdeDpListDataSetWidget & operator =(const C_SdNdeDpListDataSetWidget &) &;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
