//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Popup dialog for global tool settings

   Gives the user the opportunity to customize tool behaviour

   \copyright   Copyright 2024 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.hpp"

#include "stwtypes.hpp"
#include "C_GtGetText.hpp"
#include "C_NagToolSettingsPopupDialog.hpp"
#include "ui_C_NagToolSettingsPopupDialog.h"
#include "C_OscUtils.hpp"
#include "C_PuiSdHandler.hpp"
#include "C_PuiProject.hpp"
#include "C_OgeWiCustomMessage.hpp"
#include "C_PopUtil.hpp"
#include "C_NagUnUsedProjectFilesPopUpDialog.hpp"
#include "C_UsHandler.hpp"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw::opensyde_gui;
using namespace stw::opensyde_gui_logic;
using namespace stw::opensyde_gui_elements;
using namespace stw::opensyde_core;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

/* -- Global Variables ---------------------------------------------------------------------------------------------- */

/* -- Module Global Variables --------------------------------------------------------------------------------------- */

/* -- Module Global Function Prototypes ----------------------------------------------------------------------------- */

/* -- Implementation ------------------------------------------------------------------------------------------------ */

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default constructor

   Set up GUI with all elements.

   \param[in,out] orc_Parent Reference to parent
*/
//----------------------------------------------------------------------------------------------------------------------
C_NagToolSettingsPopupDialog::C_NagToolSettingsPopupDialog(stw::opensyde_gui_elements::C_OgePopUpDialog & orc_Parent) :
   QWidget(&orc_Parent),
   mpc_Ui(new Ui::C_NagToolSettingsPopupDialog),
   mrc_ParentDialog(orc_Parent)
{
   this->mpc_Ui->setupUi(this);
   InitStaticNames();

   this->m_InitEnvironmentSection();

   // register the widget for showing
   this->mrc_ParentDialog.SetWidget(this);

   connect(this->mpc_Ui->pc_PushButtonOk, &QPushButton::clicked, this, &C_NagToolSettingsPopupDialog::m_OkClicked);
   connect(this->mpc_Ui->pc_PushButtonCancel, &QPushButton::clicked, this,
           &C_NagToolSettingsPopupDialog::m_CancelClicked);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default destructor
*/
//----------------------------------------------------------------------------------------------------------------------
C_NagToolSettingsPopupDialog::~C_NagToolSettingsPopupDialog(void)
{
   delete this->mpc_Ui;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Initialize all displayed static names
*/
//----------------------------------------------------------------------------------------------------------------------
void C_NagToolSettingsPopupDialog::InitStaticNames(void) const
{
   //Global settings section
   this->mrc_ParentDialog.SetTitle(C_GtGetText::h_GetText("Tool"));
   this->mrc_ParentDialog.SetSubTitle(C_GtGetText::h_GetText("Settings"));
   this->mpc_Ui->pc_LabelGlobalSettings->setText(C_GtGetText::h_GetText("General"));

   this->mpc_Ui->pc_LabelPathHandling->setText(C_GtGetText::h_GetText("Relative/absolute path handling"));
   this->mpc_Ui->pc_LabelSkipTsp->setText(C_GtGetText::h_GetText("Skip TSP Import assistance"));

   this->mpc_Ui->pc_LabelPathHandling->SetToolTipInformation(C_GtGetText::h_GetText("Path handling"),
                                                             C_GtGetText::h_GetText(
                                                                "Choose if file paths shall be handled as relative or absolute paths."
                                                                "\nIf the option \"Ask User\" is active, openSYDE will ask everytime a file action is performed."));

   this->mpc_Ui->pc_LabelSkipTsp->SetToolTipInformation(C_GtGetText::h_GetText("TSP Import assistance"),
                                                        C_GtGetText::h_GetText(
                                                           "Choose if openSYDE automatically redirects you to TSP Import when a new node is added to the Topology."
                                                           "\nIf the option \"Ask User\" is active, openSYDE will ask everytime a node is added."));

   this->mpc_Ui->pc_CbxPathHandling->addItem(C_GtGetText::h_GetText("Ask User"));
   this->mpc_Ui->pc_CbxPathHandling->addItem(C_GtGetText::h_GetText("Relative"));
   this->mpc_Ui->pc_CbxPathHandling->addItem(C_GtGetText::h_GetText("Absolute"));

   this->mpc_Ui->pc_CbxSkipTsp->addItem(C_GtGetText::h_GetText("Ask User"));
   this->mpc_Ui->pc_CbxSkipTsp->addItem(C_GtGetText::h_GetText("Skip"));
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Overwritten key press event slot

   Here: Handle specific enter key cases

   \param[in,out] opc_KeyEvent Event identification and information
*/
//----------------------------------------------------------------------------------------------------------------------
void C_NagToolSettingsPopupDialog::keyPressEvent(QKeyEvent * const opc_KeyEvent)
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
void C_NagToolSettingsPopupDialog::m_OkClicked(void)
{
   //write settings to user settings

   C_UsHandler::h_GetInstance()->SetPathHandlingSelection(this->mpc_Ui->pc_CbxPathHandling->currentText());
   C_UsHandler::h_GetInstance()->SetSkipTspSelection(this->mpc_Ui->pc_CbxSkipTsp->currentText());

   this->mrc_ParentDialog.accept();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Slot of cancel button click
*/
//----------------------------------------------------------------------------------------------------------------------
void C_NagToolSettingsPopupDialog::m_CancelClicked()
{
   this->mrc_ParentDialog.reject();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Reads the corresponding user settings and initializes UI elements accordingly
*/
//----------------------------------------------------------------------------------------------------------------------
void C_NagToolSettingsPopupDialog::m_InitEnvironmentSection()
{
   this->mpc_Ui->pc_CbxPathHandling->setCurrentText(C_UsHandler::h_GetInstance()->GetPathHandlingSelection());
   this->mpc_Ui->pc_CbxSkipTsp->setCurrentText(C_UsHandler::h_GetInstance()->GetSkipTspSelection());
}
