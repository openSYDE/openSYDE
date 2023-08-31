//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Popup dialog for editing Datapool list comment.

   Popup dialog for editing Datapool list comment.

   \copyright   Copyright 2020 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.hpp"

#include "stwtypes.hpp"
#include "C_GtGetText.hpp"
#include "C_PuiSdHandler.hpp"
#include "C_SdNdeDpListCommentDialog.hpp"
#include "ui_C_SdNdeDpListCommentDialog.h"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw::opensyde_core;
using namespace stw::opensyde_gui;
using namespace stw::opensyde_gui_logic;
using namespace stw::opensyde_gui_elements;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

/* -- Global Variables ---------------------------------------------------------------------------------------------- */

/* -- Module Global Variables --------------------------------------------------------------------------------------- */

/* -- Module Global Function Prototypes ----------------------------------------------------------------------------- */

/* -- Implementation ------------------------------------------------------------------------------------------------ */

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default constructor

   Set up GUI with all elements.

   \param[in,out]  orc_Parent          Reference to parent
   \param[in]      ou32_NodeIndex      Node index
   \param[in]      ou32_DataPoolIndex  Data pool index
   \param[in]      ou32_ListIndex      List index
*/
//----------------------------------------------------------------------------------------------------------------------
C_SdNdeDpListCommentDialog::C_SdNdeDpListCommentDialog(stw::opensyde_gui_elements::C_OgePopUpDialog & orc_Parent,
                                                       const uint32_t ou32_NodeIndex, const uint32_t ou32_DataPoolIndex,
                                                       const uint32_t ou32_ListIndex) :
   QWidget(&orc_Parent),
   mpc_Ui(new Ui::C_SdNdeDpListCommentDialog),
   mrc_ParentDialog(orc_Parent),
   mu32_NodeIndex(ou32_NodeIndex),
   mu32_DataPoolIndex(ou32_DataPoolIndex),
   mu32_ListIndex(ou32_ListIndex)
{
   this->mpc_Ui->setupUi(this);

   InitStaticNames();

   // register the widget for showing
   this->mrc_ParentDialog.SetWidget(this);

   // start with all selected
   this->mpc_Ui->pc_TextEditComment->selectAll();

   connect(this->mpc_Ui->pc_PushButtonOk, &QPushButton::clicked, this, &C_SdNdeDpListCommentDialog::m_OkClicked);
   connect(this->mpc_Ui->pc_PushButtonCancel, &QPushButton::clicked,
           this, &C_SdNdeDpListCommentDialog::m_CancelClicked);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default destructor
*/
//----------------------------------------------------------------------------------------------------------------------
C_SdNdeDpListCommentDialog::~C_SdNdeDpListCommentDialog(void)
{
   delete this->mpc_Ui;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Initialize all displayed static names
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDpListCommentDialog::InitStaticNames(void) const
{
   const C_OscNodeDataPoolList * const pc_List = C_PuiSdHandler::h_GetInstance()->GetOscDataPoolList(
      this->mu32_NodeIndex, this->mu32_DataPoolIndex, this->mu32_ListIndex);

   if (pc_List != NULL)
   {
      mrc_ParentDialog.SetTitle(static_cast<QString>(C_GtGetText::h_GetText("List \"%1\"")).arg(pc_List->c_Name.c_str()));
      this->mpc_Ui->pc_TextEditComment->setPlaceholderText(C_GtGetText::h_GetText("Add your comment here ..."));
      this->mpc_Ui->pc_TextEditComment->setText(pc_List->c_Comment.c_str());
   }
   this->mrc_ParentDialog.SetSubTitle(C_GtGetText::h_GetText("Comment Edit"));
   this->mpc_Ui->pc_LabelHeadingPreview->setText(C_GtGetText::h_GetText("Comment"));
   this->mpc_Ui->pc_PushButtonOk->setText(C_GtGetText::h_GetText("OK"));
   this->mpc_Ui->pc_PushButtonCancel->setText(C_GtGetText::h_GetText("Cancel"));
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get comment from text field

   \return
   Comment
*/
//----------------------------------------------------------------------------------------------------------------------
QString C_SdNdeDpListCommentDialog::GetComment(void) const
{
   return this->mpc_Ui->pc_TextEditComment->toPlainText();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get existing comment to comment editor popup text edit

   \param[in]       oc_Comment     String to be placed in editor popup text edit
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDpListCommentDialog::GetCommentToEditor(const QString oc_Comment) const
{
   this->mpc_Ui->pc_TextEditComment->setText(oc_Comment);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Set title for edit comment popup dialog

   \param[in]       oc_Title     Title for popup dialog
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDpListCommentDialog::SetTitle(const QString oc_Title)
{
   mrc_ParentDialog.SetTitle(oc_Title);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Overwritten key press event slot

   Here: Handle specific enter key cases

   \param[in,out]  opc_KeyEvent  Event identification and information
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDpListCommentDialog::keyPressEvent(QKeyEvent * const opc_KeyEvent)
{
   bool q_CallOrg = true;

   //Handle all enter key cases manually
   if ((opc_KeyEvent->key() == static_cast<int32_t>(Qt::Key_Enter)) ||
       (opc_KeyEvent->key() == static_cast<int32_t>(Qt::Key_Return)))
   {
      if (((opc_KeyEvent->modifiers().testFlag(Qt::ControlModifier) == true) &&
           (opc_KeyEvent->modifiers().testFlag(Qt::AltModifier) == false)) &&
          (opc_KeyEvent->modifiers().testFlag(Qt::ShiftModifier) == false))
      {
         this->m_OkClicked();
      }
      else
      {
         q_CallOrg = false;
      }
   }
   if (q_CallOrg == true)
   {
      QWidget::keyPressEvent(opc_KeyEvent);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Slot of Ok button click
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDpListCommentDialog::m_OkClicked(void)
{
   this->mrc_ParentDialog.accept();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Slot of Cancel button click
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDpListCommentDialog::m_CancelClicked(void)
{
   this->mrc_ParentDialog.reject();
}
