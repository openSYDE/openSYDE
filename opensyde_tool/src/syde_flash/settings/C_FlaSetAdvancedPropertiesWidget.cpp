//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Widget for advanced properties in SYDEflash

   Widget for advanced properties in SYDEflash

   \copyright   Copyright 2023 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.hpp"

#include "C_GtGetText.hpp"
#include "C_UsHandler.hpp"

#include "C_FlaSetAdvancedPropertiesWidget.hpp"
#include "ui_C_FlaSetAdvancedPropertiesWidget.h"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
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

   \param[in,out] opc_Parent Optional pointer to parent
*/
//----------------------------------------------------------------------------------------------------------------------
C_FlaSetAdvancedPropertiesWidget::C_FlaSetAdvancedPropertiesWidget(QWidget * const opc_Parent) :
   C_OgeWiOnlyBackground(opc_Parent),
   mpc_Ui(new Ui::C_FlaSetAdvancedPropertiesWidget)
{
   this->mpc_Ui->setupUi(this);

   this->SetBackgroundColor(5);

   // initialize labels
   this->mpc_Ui->pc_LabFlashloaderResetWaitTime->SetForegroundColor(0);
   this->mpc_Ui->pc_LabRequestDownloadTimeout->SetForegroundColor(0);
   this->mpc_Ui->pc_LabTransferDataTimeout->SetForegroundColor(0);

   this->mpc_Ui->pc_LabFlashloaderResetWaitTime->SetFontPixel(13);
   this->mpc_Ui->pc_LabRequestDownloadTimeout->SetFontPixel(13);
   this->mpc_Ui->pc_LabTransferDataTimeout->SetFontPixel(13);

   this->mpc_Ui->pc_LabFlashloaderResetWaitTime->setText(C_GtGetText::h_GetText("Flashloader Reset Wait Time"));
   this->mpc_Ui->pc_LabFlashloaderResetWaitTime->SetToolTipInformation(
      C_GtGetText::h_GetText("Flashloader Reset Wait Time"),
      C_GtGetText::h_GetText("Wait time for transition from application to flashloader. (Default: 1000 ms)"));

   this->mpc_Ui->pc_LabRequestDownloadTimeout->setText(C_GtGetText::h_GetText("Request Download Timeout"));
   this->mpc_Ui->pc_LabRequestDownloadTimeout->SetToolTipInformation(
      C_GtGetText::h_GetText("Request Download Timeout"),
      C_GtGetText::h_GetText("Timeout for erasing flash memory. (Default: 40000 ms)"));

   this->mpc_Ui->pc_LabTransferDataTimeout->setText(C_GtGetText::h_GetText("Transfer Data Timeout"));
   this->mpc_Ui->pc_LabTransferDataTimeout->SetToolTipInformation(C_GtGetText::h_GetText("Transfer Data Timeout"),
                                                                  C_GtGetText::h_GetText(
                                                                     "Timeout for writing to flash. (Default: 1000 ms)"));

   this->mpc_Ui->pc_SpxFlashloaderResetWaitTime->setSuffix(C_GtGetText::h_GetText(" ms"));
   this->mpc_Ui->pc_SpxRequestDownloadTimeout->setSuffix(C_GtGetText::h_GetText(" ms"));
   this->mpc_Ui->pc_SpxTransferDataTimeout->setSuffix(C_GtGetText::h_GetText(" ms"));

   //set max
   this->mpc_Ui->pc_SpxFlashloaderResetWaitTime->SetMaximumCustom(std::numeric_limits<int32_t>::max());
   this->mpc_Ui->pc_SpxRequestDownloadTimeout->SetMaximumCustom(std::numeric_limits<int32_t>::max());
   this->mpc_Ui->pc_SpxTransferDataTimeout->SetMaximumCustom(std::numeric_limits<int32_t>::max());

   // initialize title widget
   this->mpc_Ui->pc_WiHeader->SetTitle(C_GtGetText::h_GetText("Advanced Properties"));
   this->mpc_Ui->pc_WiHeader->SetIcon("://images/IconProperties.svg");
   this->mpc_Ui->pc_WiHeader->SetToggle(false);
   connect(this->mpc_Ui->pc_WiHeader, &C_CamOgeWiSettingSubSection::SigExpandSection,
           this, &C_FlaSetAdvancedPropertiesWidget::m_OnExpand);
   connect(this->mpc_Ui->pc_WiHeader, &C_CamOgeWiSettingSubSection::SigHide,
           this, &C_FlaSetAdvancedPropertiesWidget::SigHide);

   this->LoadUserSettings();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default destructor
*/
//----------------------------------------------------------------------------------------------------------------------
C_FlaSetAdvancedPropertiesWidget::~C_FlaSetAdvancedPropertiesWidget()
{
   delete this->mpc_Ui;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Enable/disable settings widgets

   \param[in]  oq_Enabled  true: enable, false: disable
*/
//----------------------------------------------------------------------------------------------------------------------
void C_FlaSetAdvancedPropertiesWidget::EnableSettings(const bool oq_Enabled)
{
   this->mpc_Ui->pc_WiContent->setEnabled(oq_Enabled);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Slot of expand signal.

   \param[in]  oq_Expand   true: expand; false: collapse
*/
//----------------------------------------------------------------------------------------------------------------------
void C_FlaSetAdvancedPropertiesWidget::m_OnExpand(const bool oq_Expand) const
{
   this->mpc_Ui->pc_WiContent->setVisible(oq_Expand);

   C_UsHandler::h_GetInstance()->SetWiAdvSettExpanded(oq_Expand);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Load user settings
*/
//----------------------------------------------------------------------------------------------------------------------
void C_FlaSetAdvancedPropertiesWidget::LoadUserSettings() const
{
   this->mpc_Ui->pc_SpxFlashloaderResetWaitTime->setValue(C_UsHandler::h_GetInstance()->GetFlashloaderResetWaitTime());
   this->mpc_Ui->pc_SpxRequestDownloadTimeout->setValue(C_UsHandler::h_GetInstance()->GetRequestDownloadTimeout());
   this->mpc_Ui->pc_SpxTransferDataTimeout->setValue(C_UsHandler::h_GetInstance()->GetTransferDataTimeout());
   this->mpc_Ui->pc_WiHeader->SetExpanded(C_UsHandler::h_GetInstance()->GetWiAdvSettExpanded());
   this->m_OnExpand(C_UsHandler::h_GetInstance()->GetWiAdvSettExpanded());
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Prepare widget for menu view in collapsed state resp. expanded view.

   \param[in]  oq_Expand   true: prepare for expanded, false: prepare for collapsed menu
*/
//----------------------------------------------------------------------------------------------------------------------
void C_FlaSetAdvancedPropertiesWidget::PrepareForExpanded(const bool oq_Expand) const
{
   if (oq_Expand == false)
   {
      this->mpc_Ui->pc_WiContent->setVisible(true);
   }
   else
   {
      this->mpc_Ui->pc_WiContent->setVisible(C_UsHandler::h_GetInstance()->GetWiAdvSettExpanded());
   }

   // hide arrow button
   this->mpc_Ui->pc_WiHeader->ShowExpandButton(oq_Expand);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Save user settings
*/
//----------------------------------------------------------------------------------------------------------------------
void C_FlaSetAdvancedPropertiesWidget::SaveUserSettings() const
{
   C_UsHandler::h_GetInstance()->SetFlashloaderResetWaitTime(this->mpc_Ui->pc_SpxFlashloaderResetWaitTime->value());
   C_UsHandler::h_GetInstance()->SetRequestDownloadTimeout(this->mpc_Ui->pc_SpxRequestDownloadTimeout->value());
   C_UsHandler::h_GetInstance()->SetTransferDataTimeout(this->mpc_Ui->pc_SpxTransferDataTimeout->value());
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get Flashloader Reset Wait Time

   \return
   Current Flashloader Reset Wait Time value
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_FlaSetAdvancedPropertiesWidget::GetFlashloaderResetWaitTime(void) const
{
   return this->mpc_Ui->pc_SpxFlashloaderResetWaitTime->value();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get Request Download Timeout

   \return
   Current Request Download Timeout value
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_FlaSetAdvancedPropertiesWidget::GetRequestDownloadTimeout(void) const
{
   return this->mpc_Ui->pc_SpxRequestDownloadTimeout->value();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get Transfer Data Timeout

   \return
   Current Transfer Data Timeout value
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_FlaSetAdvancedPropertiesWidget::GetTransferDataTimeout(void) const
{
   return this->mpc_Ui->pc_SpxTransferDataTimeout->value();
}
