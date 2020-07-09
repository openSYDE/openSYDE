//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Popup dialog for HALC settings.

   Popup dialog for HALC settings.

   \copyright   Copyright 2020 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.h"

#include "stwerrors.h"
#include "TGLUtils.h"
#include "C_GtGetText.h"
#include "C_OSCNode.h"
#include "C_PuiSdHandler.h"

#include "C_SdNdeHalcSettingsDialog.h"
#include "ui_C_SdNdeHalcSettingsDialog.h"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw_types;
using namespace stw_errors;
using namespace stw_opensyde_core;
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

   \param[in,out]  orc_Parent       Reference to parent
   \param[in]      ou32_NodeIndex   Node index
*/
//----------------------------------------------------------------------------------------------------------------------
C_SdNdeHalcSettingsDialog::C_SdNdeHalcSettingsDialog(stw_opensyde_gui_elements::C_OgePopUpDialog & orc_Parent,
                                                     const stw_types::uint32 ou32_NodeIndex) :
   QWidget(&orc_Parent),
   mpc_Ui(new Ui::C_SdNdeHalcSettingsDialog),
   mrc_ParentDialog(orc_Parent),
   mu32_NodeIndex(ou32_NodeIndex)
{
   this->mpc_Ui->setupUi(this);

   InitStaticNames();

   // register the widget for showing
   this->mrc_ParentDialog.SetWidget(this);

   // initialize comboboxes
   this->m_InitComboboxes();

   connect(this->mpc_Ui->pc_PushButtonOk, &QPushButton::clicked, this, &C_SdNdeHalcSettingsDialog::m_OkClicked);
   connect(this->mpc_Ui->pc_PushButtonCancel, &QPushButton::clicked, this, &C_SdNdeHalcSettingsDialog::m_CancelClicked);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default destructor
*/
//----------------------------------------------------------------------------------------------------------------------
C_SdNdeHalcSettingsDialog::~C_SdNdeHalcSettingsDialog(void)
{
   delete this->mpc_Ui;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Initialize all displayed static names
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeHalcSettingsDialog::InitStaticNames(void) const
{
   this->mrc_ParentDialog.SetTitle(C_GtGetText::h_GetText("HALC Configuration"));
   this->mrc_ParentDialog.SetSubTitle(C_GtGetText::h_GetText("Settings"));
   this->mpc_Ui->pc_LabelHeadingPreview->setText(C_GtGetText::h_GetText("Data Block Assignment"));

   this->mpc_Ui->pc_LabSafe->setText(C_GtGetText::h_GetText("Safe Channels"));
   this->mpc_Ui->pc_LabNonSafe->setText(C_GtGetText::h_GetText("Non Safe Channels"));
   this->mpc_Ui->pc_LabSafe->SetToolTipInformation("Safe Channels",
                                                   "Select Data Block to assign all safe channels to.");
   this->mpc_Ui->pc_LabNonSafe->SetToolTipInformation("Non Safe Channels",
                                                      "Select Data Block to assign all non safe channels to.");

   this->mpc_Ui->pc_PushButtonOk->setText(C_GtGetText::h_GetText("OK"));
   this->mpc_Ui->pc_PushButtonCancel->setText(C_GtGetText::h_GetText("Cancel"));
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Overwritten key press event slot

   Here: Handle specific enter key cases

   \param[in,out]  opc_KeyEvent  Event identification and information
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeHalcSettingsDialog::keyPressEvent(QKeyEvent * const opc_KeyEvent)
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
/*! \brief  Initialize combobox by adding all existing Data Blocks.
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeHalcSettingsDialog::m_InitComboboxes(void) const
{
   const C_OSCNode * const pc_Node = C_PuiSdHandler::h_GetInstance()->GetOSCNodeConst(this->mu32_NodeIndex);
   const C_OSCHalcConfig * pc_Config = C_PuiSdHandler::h_GetInstance()->GetHALCConfig(this->mu32_NodeIndex);

   // fill combobox
   if (pc_Node != NULL)
   {
      std::vector<C_OSCNodeApplication>::const_iterator c_It;

      // clear configuration
      this->mpc_Ui->pc_CbxSafe->clear();
      this->mpc_Ui->pc_CbxNonSafe->clear();

      // add item for no application assigned case
      this->mpc_Ui->pc_CbxSafe->addItem(C_GtGetText::h_GetText("<not assigned>"));
      this->mpc_Ui->pc_CbxNonSafe->addItem(C_GtGetText::h_GetText("<not assigned>"));

      // add all data blocks to data block relation section
      for (c_It = pc_Node->c_Applications.begin(); c_It != pc_Node->c_Applications.end(); ++c_It)
      {
         const C_OSCNodeApplication & rc_Application = *c_It;
         this->mpc_Ui->pc_CbxSafe->addItem(rc_Application.c_Name.c_str());
         this->mpc_Ui->pc_CbxNonSafe->addItem(rc_Application.c_Name.c_str());
      }
   }

   // select current Data Block
   if (pc_Config != NULL)
   {
      // +1 because of not assigned case
      if (pc_Config->GetSafeDatablockAssigned() == true)
      {
         this->mpc_Ui->pc_CbxSafe->setCurrentIndex(static_cast<sintn>(pc_Config->GetSafeDatablockIndex() + 1));
      }

      if (pc_Config->GetUnsafeDatablockAssigned() == true)
      {
         this->mpc_Ui->pc_CbxNonSafe->setCurrentIndex(static_cast<sintn>(pc_Config->GetUnsafeDatablockIndex() + 1));
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Slot of Ok button click
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeHalcSettingsDialog::m_OkClicked(void)
{
   sint32 s32_Error;

   // save safe assignment
   if (this->mpc_Ui->pc_CbxSafe->currentIndex() > 0) // not assigned case
   {
      s32_Error = C_PuiSdHandler::h_GetInstance()->
                  SetHALCConfigSafeDataBlock(this->mu32_NodeIndex, true,
                                             static_cast<uint32>(this->mpc_Ui->pc_CbxSafe->currentIndex() - 1));
   }
   else
   {
      s32_Error = C_PuiSdHandler::h_GetInstance()->SetHALCConfigSafeDataBlock(this->mu32_NodeIndex, false, 0);
   }

   tgl_assert(s32_Error == C_NO_ERR);

   // save non-safe assignment
   if (this->mpc_Ui->pc_CbxNonSafe->currentIndex() > 0) // not assigned case
   {
      s32_Error = C_PuiSdHandler::h_GetInstance()->
                  SetHALCConfigUnsafeDataBlock(this->mu32_NodeIndex, true,
                                               static_cast<uint32>(this->mpc_Ui->pc_CbxNonSafe->currentIndex() - 1));
   }
   else
   {
      s32_Error = C_PuiSdHandler::h_GetInstance()->SetHALCConfigUnsafeDataBlock(this->mu32_NodeIndex, false, 0);
   }
   tgl_assert(s32_Error == C_NO_ERR);

   // accept dialog
   this->mrc_ParentDialog.accept();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Slot of Cancel button clicked
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeHalcSettingsDialog::m_CancelClicked(void)
{
   this->mrc_ParentDialog.reject();
}
