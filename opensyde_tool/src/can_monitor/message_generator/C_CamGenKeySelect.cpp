//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Widget for user key selection (implementation)

   Widget for user key selection

   \copyright   Copyright 2019 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.h"

#include "stwerrors.h"
#include "TGLUtils.h"
#include "C_GtGetText.h"
#include "C_CamProHandler.h"
#include "C_CamGenKeySelect.h"
#include "ui_C_CamGenKeySelect.h"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw_tgl;
using namespace stw_types;
using namespace stw_errors;
using namespace stw_opensyde_gui;
using namespace stw_opensyde_gui_logic;
using namespace stw_opensyde_gui_elements;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

/* -- Global Variables ---------------------------------------------------------------------------------------------- */

/* -- Module Global Variables --------------------------------------------------------------------------------------- */

/* -- Module Global Function Prototypes ----------------------------------------------------------------------------- */

/* -- Implementation ------------------------------------------------------------------------------------------------ */

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default constructor

   Set up GUI with all elements.

   \param[in,out] orc_Parent           Reference to parent
   \param[in]     ou32_MessageIndex    Index of message the key belongs to
*/
//----------------------------------------------------------------------------------------------------------------------
C_CamGenKeySelect::C_CamGenKeySelect(stw_opensyde_gui_elements::C_OgePopUpDialog & orc_Parent,
                                     const stw_types::uint32 ou32_MessageIndex) :
   QWidget(&orc_Parent),
   mpc_Ui(new Ui::C_CamGenKeySelect),
   mrc_ParentDialog(orc_Parent),
   mu32_MessageIndex(ou32_MessageIndex)
{
   QRegularExpressionValidator * pc_Validator;
   const QRegularExpression c_Regex("^[^ ]$");

   this->mpc_Ui->setupUi(this);

   InitStaticNames();

   pc_Validator = new QRegularExpressionValidator(c_Regex, this);
   this->mpc_Ui->pc_LineEditKey->setValidator(pc_Validator);

   // label "Settings"
   this->mpc_Ui->pc_LabelHeadingPreview->SetForegroundColor(4);
   this->mpc_Ui->pc_LabelHeadingPreview->SetBackgroundColor(-1);
   this->mpc_Ui->pc_LabelHeadingPreview->SetFontPixel(14);

   // label "Key"
   this->mpc_Ui->pc_LabelKey->SetForegroundColor(8);
   this->mpc_Ui->pc_LabelKey->SetBackgroundColor(-1);
   this->mpc_Ui->pc_LabelKey->SetFontPixel(12);

   // label "Offset"
   this->mpc_Ui->pc_LabelOffset->SetForegroundColor(8);
   this->mpc_Ui->pc_LabelOffset->SetBackgroundColor(-1);
   this->mpc_Ui->pc_LabelOffset->SetFontPixel(12);

   //Restrictions
   this->mpc_Ui->pc_LineEditKey->setMaxLength(1);
   this->mpc_Ui->pc_SpinBoxOffset->SetMinimumCustom(0);
   this->mpc_Ui->pc_SpinBoxOffset->SetMaximumCustom(65000);

   m_LoadData();

   // register the widget for showing
   this->mrc_ParentDialog.SetWidget(this);

   connect(this->mpc_Ui->pc_PushButtonOk, &QPushButton::clicked, this, &C_CamGenKeySelect::m_OkClicked);
   connect(this->mpc_Ui->pc_PushButtonCancel, &QPushButton::clicked, this, &C_CamGenKeySelect::m_CancelClicked);
   //lint -e{429}  no memory leak because of the parent of pc_Validator and the Qt memory management
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default destructor
*/
//----------------------------------------------------------------------------------------------------------------------
C_CamGenKeySelect::~C_CamGenKeySelect(void)
{
   delete this->mpc_Ui;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Initialize all displayed static names
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CamGenKeySelect::InitStaticNames(void) const
{
   this->mrc_ParentDialog.SetTitle(C_GtGetText::h_GetText("Trigger Key"));
   this->mrc_ParentDialog.SetSubTitle(C_GtGetText::h_GetText("Configuration"));
   this->mpc_Ui->pc_LabelHeadingPreview->setText(C_GtGetText::h_GetText("Settings"));
   this->mpc_Ui->pc_LabelKey->setText(C_GtGetText::h_GetText("Trigger Key"));
   this->mpc_Ui->pc_LineEditKey->setPlaceholderText(C_GtGetText::h_GetText("Enter a single character"));
   this->mpc_Ui->pc_LabelOffset->setText(C_GtGetText::h_GetText("Trigger Offset"));
   this->mpc_Ui->pc_SpinBoxOffset->setSuffix(C_GtGetText::h_GetText(" ms"));
   this->mpc_Ui->pc_PushButtonOk->setText(C_GtGetText::h_GetText("OK"));
   this->mpc_Ui->pc_PushButtonCancel->setText(C_GtGetText::h_GetText("Cancel"));
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Overwritten key press event slot

   Here: Handle specific enter key cases

   \param[in,out] opc_KeyEvent Event identification and information
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CamGenKeySelect::keyPressEvent(QKeyEvent * const opc_KeyEvent)
{
   bool q_CallOrg = true;

   //Handle all enter key cases manually
   if ((opc_KeyEvent->key() == static_cast<sintn>(Qt::Key_Enter)) ||
       (opc_KeyEvent->key() == static_cast<sintn>(Qt::Key_Return)))
   {
      if (((opc_KeyEvent->modifiers().testFlag(Qt::ControlModifier) == true) &&
           (opc_KeyEvent->modifiers().testFlag(Qt::AltModifier) == false)) &&
          (opc_KeyEvent->modifiers().testFlag(Qt::ShiftModifier) == false))
      {
         this->mrc_ParentDialog.accept();
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
void C_CamGenKeySelect::m_OkClicked(void)
{
   //Update data
   tgl_assert(C_CamProHandler::h_GetInstance()->SetMessageKey(this->mu32_MessageIndex,
                                                              this->mpc_Ui->pc_LineEditKey->text(),
                                                              this->mpc_Ui->pc_SpinBoxOffset->value()) == C_NO_ERR);
   this->mrc_ParentDialog.accept();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Slot of Cancel button click
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CamGenKeySelect::m_CancelClicked(void)
{
   this->mrc_ParentDialog.reject();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Load data from handler
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CamGenKeySelect::m_LoadData(void) const
{
   const C_CamProMessageData * const pc_Message = C_CamProHandler::h_GetInstance()->GetMessageConst(
      this->mu32_MessageIndex);

   if (pc_Message != NULL)
   {
      this->mpc_Ui->pc_LineEditKey->setText(pc_Message->c_Key.c_str());
      this->mpc_Ui->pc_SpinBoxOffset->setValue(pc_Message->u32_KeyPressOffset);
   }
}
