//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Widget for STW flashloader options (implementation)

   Widget for STW flashloader options

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "C_GtGetText.h"
#include "C_PuiSdHandler.h"
#include "C_SdNdeStwFlashloaderOptions.h"
#include "ui_C_SdNdeStwFlashloaderOptions.h"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw_types;
using namespace stw_opensyde_gui;
using namespace stw_opensyde_core;
using namespace stw_opensyde_gui_logic;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

/* -- Global Variables ---------------------------------------------------------------------------------------------- */

/* -- Module Global Variables --------------------------------------------------------------------------------------- */

/* -- Module Global Function Prototypes ----------------------------------------------------------------------------- */

/* -- Implementation ------------------------------------------------------------------------------------------------ */

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default constructor

   Set up GUI with all elements.

   \param[in,out] orc_Parent Optional pointer to parent
   \param[in]     ou32_Index Node index
*/
//----------------------------------------------------------------------------------------------------------------------
C_SdNdeStwFlashloaderOptions::C_SdNdeStwFlashloaderOptions(stw_opensyde_gui_elements::C_OgePopUpDialog & orc_Parent,
                                                           const uint32 ou32_Index) :
   QWidget(&orc_Parent),
   mpc_Ui(new Ui::C_SdNdeStwFlashloaderOptions),
   mrc_ParentDialog(orc_Parent),
   mu32_Index(ou32_Index)
{
   mpc_Ui->setupUi(this);

   // register the widget for showing
   this->mrc_ParentDialog.SetWidget(this);

   this->mrc_ParentDialog.SetTitle(QString(C_GtGetText::h_GetText("STW Flashloader")));
   this->mrc_ParentDialog.SetSubTitle(QString(C_GtGetText::h_GetText("Settings")));

   //Init
   InitStaticNames();

   m_LoadData();

   // connects
   connect(this->mpc_Ui->pc_BushButtonOk, &QPushButton::clicked, this, &C_SdNdeStwFlashloaderOptions::m_OkClicked);
   connect(this->mpc_Ui->pc_BushButtonCancel, &QPushButton::clicked, this,
           &C_SdNdeStwFlashloaderOptions::m_CancelClicked);
   connect(this->mpc_Ui->pc_CheckBoxUseResetMessage, &QCheckBox::toggled, this,
           &C_SdNdeStwFlashloaderOptions::m_OnUseResetMessageChanged);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default destructor

   Clean up.
*/
//----------------------------------------------------------------------------------------------------------------------
C_SdNdeStwFlashloaderOptions::~C_SdNdeStwFlashloaderOptions(void)
{
   delete mpc_Ui;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Initialize all displayed static names
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeStwFlashloaderOptions::InitStaticNames(void) const
{
   this->mpc_Ui->pc_BushButtonOk->setText(C_GtGetText::h_GetText("OK"));
   this->mpc_Ui->pc_BushButtonCancel->setText(C_GtGetText::h_GetText("Cancel"));
   this->mpc_Ui->pc_LabelDataElement->setText(C_GtGetText::h_GetText("STW Flashloader Settings"));
   this->mpc_Ui->pc_CheckBoxUseResetMessage->setText(C_GtGetText::h_GetText("Use reset message"));
   this->mpc_Ui->pc_LabelSendId->setText(C_GtGetText::h_GetText("Send ID"));
   this->mpc_Ui->pc_LabelCompanyId->setText(C_GtGetText::h_GetText("Company ID"));
   this->mpc_Ui->pc_LabelSendIdValue->setText(C_GtGetText::h_GetText("0x51 (11 bit)"));
   this->mpc_Ui->pc_LabelCompanyIdValue->setText(C_GtGetText::h_GetText("Auto detect"));
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Trigger reload data
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeStwFlashloaderOptions::m_LoadData(void) const
{
   const C_OSCNode * const pc_Node = C_PuiSdHandler::h_GetInstance()->GetOSCNodeConst(this->mu32_Index);

   this->mpc_Ui->pc_TableView->SetNode(this->mu32_Index);
   if (pc_Node != NULL)
   {
      this->mpc_Ui->pc_CheckBoxUseResetMessage->setChecked(
         pc_Node->c_Properties.c_STWFlashloaderSettings.q_ResetMessageActive);
      this->mpc_Ui->pc_TableView->setEnabled(pc_Node->c_Properties.c_STWFlashloaderSettings.q_ResetMessageActive);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Slot of Ok button click
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeStwFlashloaderOptions::m_OkClicked(void)
{
   this->mrc_ParentDialog.accept();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Slot of Cancel button
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeStwFlashloaderOptions::m_CancelClicked(void)
{
   this->mrc_ParentDialog.reject();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Save new reset message state
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeStwFlashloaderOptions::m_OnUseResetMessageChanged(void) const
{
   const C_OSCNode * const pc_Node = C_PuiSdHandler::h_GetInstance()->GetOSCNodeConst(this->mu32_Index);

   if (pc_Node != NULL)
   {
      C_OSCNodeStwFlashloaderSettings c_Settings = pc_Node->c_Properties.c_STWFlashloaderSettings;

      c_Settings.q_ResetMessageActive = this->mpc_Ui->pc_CheckBoxUseResetMessage->isChecked();
      this->mpc_Ui->pc_TableView->setEnabled(c_Settings.q_ResetMessageActive);

      C_PuiSdHandler::h_GetInstance()->SetStwFlashloaderSettings(this->mu32_Index, c_Settings);
   }
}
