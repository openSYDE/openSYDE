//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Base widget for settings section

   Base widget for settings section

   \copyright   Copyright 2023 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.hpp"

#include "C_GtGetText.hpp"

#include "C_UsHandler.hpp"
#include "C_FlaSetWidget.hpp"
#include "ui_C_FlaSetWidget.h"

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

   \param[in,out]  opc_Parent    Optional pointer to parent
*/
//----------------------------------------------------------------------------------------------------------------------
C_FlaSetWidget::C_FlaSetWidget(QWidget * const opc_Parent) :
   C_CamOgeWiSettingsBase(opc_Parent),
   mpc_Ui(new Ui::C_FlaSetWidget),
   mpc_PopupProgress(new C_CamMosSectionPopup()),
   mpc_PopupDllConfig(new C_CamMosSectionPopup()),
   mpc_PopupAdvSett(new C_CamMosSectionPopup())
{
   this->mpc_Ui->setupUi(this);

   // initialize GUI elements
   this->SetBackgroundColor(5);
   this->mpc_Ui->pc_WiTitle->SetTitle(C_GtGetText::h_GetText("Settings"));
   this->mpc_Ui->pc_WiTitle->SetIconType(C_CamOgeWiSectionHeader::E_ButtonType::eLEFTRIGHT);

   this->m_InitSettingsSection(this->mpc_PopupDllConfig, this->mpc_Ui->pc_PbDll, opc_Parent,
                               "://images/IconConfig.svg");
   this->m_InitSettingsSection(this->mpc_PopupAdvSett, this->mpc_Ui->pc_PbAdvSett, opc_Parent,
                               "://images/IconProperties.svg");
   this->m_InitSettingsSection(this->mpc_PopupProgress, this->mpc_Ui->pc_PbProgress, opc_Parent,
                               "://images/IconProgressLog.svg");
   this->mpc_Ui->pc_WiCollapsed->SetBackgroundColor(1);
   this->mpc_Ui->pc_WiEmpty->SetBackgroundColor(5);
   this->mpc_Ui->pc_WiExpanded->SetBackgroundColor(1);

   // connect hide signal of widgets
   connect(this->mpc_Ui->pc_WiProgress, &C_FlaSetProgressWidget::SigHide, this, &C_FlaSetWidget::m_HidePopupProgress);
   connect(this->mpc_Ui->pc_WiDll, &C_CamMosDllWidget::SigHide, this, &C_FlaSetWidget::m_HidePopupDllConfig);
   connect(this->mpc_Ui->pc_WiAdvancedProperties, &C_FlaSetAdvancedPropertiesWidget::SigHide, this,
           &C_FlaSetWidget::m_HidePopupAdvSett);

   // expand collapse section
   connect(this->mpc_Ui->pc_WiTitle, &C_CamOgeWiSectionHeader::SigExpandSection,
           this, &C_FlaSetWidget::m_OnExpandSettings);

   // forward information about new settings
   connect(this->mpc_Ui->pc_WiDll, &C_CamMosDllWidget::SigCanDllConfigured,
           this, &C_FlaSetWidget::SigCanDllConfigured);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default destructor
*/
//----------------------------------------------------------------------------------------------------------------------
C_FlaSetWidget::~C_FlaSetWidget()
{
   delete this->mpc_Ui;
   delete this->mpc_PopupProgress;
   delete this->mpc_PopupDllConfig;
   delete this->mpc_PopupAdvSett;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Load user settings
*/
//----------------------------------------------------------------------------------------------------------------------
void C_FlaSetWidget::LoadUserSettings()
{
   this->mpc_Ui->pc_WiDll->LoadUserSettings();
   this->mpc_Ui->pc_WiAdvancedProperties->LoadUserSettings();
   this->mpc_Ui->pc_WiProgress->LoadUserSettings();

   // after single widgets
   this->ExpandSettings(C_UsHandler::h_GetInstance()->GetSettingsAreExpanded());
   // adapt GUI by hand (signal gets not received)
   this->m_OnExpandSettings(C_UsHandler::h_GetInstance()->GetSettingsAreExpanded());
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Save user settings
*/
//----------------------------------------------------------------------------------------------------------------------
void C_FlaSetWidget::SaveUserSettings() const
{
   this->mpc_Ui->pc_WiAdvancedProperties->SaveUserSettings();
   if (C_UsHandler::h_GetInstance()->GetSettingsAreExpanded() == false)
   {
      // remember last opened popup
      C_UsHandler::h_GetInstance()->SetPopOpenSection(this->m_GetPopOpenIdentity());
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Expand settings

   \param[in]  oq_Expand   true: expand settings subsections
                           false: collapse settings subsections
*/
//----------------------------------------------------------------------------------------------------------------------
void C_FlaSetWidget::ExpandSettings(const bool oq_Expand) const
{
   this->mpc_Ui->pc_WiTitle->SetOpen(oq_Expand);
   // this toggles the ">>" button and therefore emits a signal; on this signal we connect and adapt the GUI
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get selected CAN DLL file path

   \return
   CAN DLL file path
*/
//----------------------------------------------------------------------------------------------------------------------
QString C_FlaSetWidget::GetCanDllPath() const
{
   return C_UsHandler::h_GetInstance()->GetCanDllPath();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Clear progress
*/
//----------------------------------------------------------------------------------------------------------------------
void C_FlaSetWidget::ClearProgress()
{
   this->mpc_Ui->pc_WiProgress->ClearProgress();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Show progress

   \param[in]  orc_Text    Text
*/
//----------------------------------------------------------------------------------------------------------------------
void C_FlaSetWidget::ShowProgress(const QString & orc_Text)
{
   this->mpc_Ui->pc_WiProgress->ShowProgress(orc_Text);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Enable/disable settings widgets

   \param[in]  oq_Enabled  true: enable, false: disable
*/
//----------------------------------------------------------------------------------------------------------------------
void C_FlaSetWidget::EnableSettings(const bool oq_Enabled)
{
   this->mpc_Ui->pc_WiDll->OnCommunicationStarted(!oq_Enabled);
   this->mpc_Ui->pc_WiAdvancedProperties->EnableSettings(oq_Enabled);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get Flashloader Reset Wait Time

   \return
   Current Flashloader Reset Wait Time value
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_FlaSetWidget::GetFlashloaderResetWaitTime(void) const
{
   return this->mpc_Ui->pc_WiAdvancedProperties->GetFlashloaderResetWaitTime();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get Request Download Timeout

   \return
   Current Request Download Timeout value
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_FlaSetWidget::GetRequestDownloadTimeout(void) const
{
   return this->mpc_Ui->pc_WiAdvancedProperties->GetRequestDownloadTimeout();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get Transfer Data Timeout

   \return
   Current Transfer Data Timeout value
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_FlaSetWidget::GetTransferDataTimeout(void) const
{
   return this->mpc_Ui->pc_WiAdvancedProperties->GetTransferDataTimeout();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Overwritten move event.

   Here: Handle popup move.

   \param[in,out]  opc_Event  Event identification and information
*/
//----------------------------------------------------------------------------------------------------------------------
void C_FlaSetWidget::showEvent(QShowEvent * const opc_Event)
{
   this->m_PrepareInitialShow();

   C_OgeWiOnlyBackground::showEvent(opc_Event);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Prepare initial show
*/
//----------------------------------------------------------------------------------------------------------------------
void C_FlaSetWidget::m_PrepareInitialShow() const
{
   for (std::vector<QPair<stw::opensyde_gui::C_CamMosSectionPopup *,
                          opensyde_gui_elements::C_CamOgePubSettingsAdd *> >::const_iterator c_ItPair =
           this->mc_Settings.cbegin();
        c_ItPair != this->mc_Settings.cend(); ++c_ItPair)
   {
      if (mh_GetButton(*c_ItPair)->isVisible() && mh_GetButton(*c_ItPair)->isChecked())
      {
         const QPoint c_Point = this->m_GetPopupMovePoint(mh_GetButton(*c_ItPair));
         mh_GetPopUp(*c_ItPair)->DoMove(c_Point);
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  On expand settings

   \param[in]  oq_Expand   true: expand settings subsections
                           false: collapse settings subsections
*/
//----------------------------------------------------------------------------------------------------------------------
void C_FlaSetWidget::m_OnExpandSettings(const bool oq_Expand)
{
   // always show whole widget in popup state
   this->mpc_Ui->pc_WiAdvancedProperties->PrepareForExpanded(oq_Expand);
   this->mpc_Ui->pc_WiDll->PrepareForExpanded(oq_Expand);
   this->mpc_Ui->pc_WiProgress->PrepareForExpanded(oq_Expand);

   if (oq_Expand == true)
   {
      // remember last opened popup
      C_UsHandler::h_GetInstance()->SetPopOpenSection(this->m_GetPopOpenIdentity());

      // hide buttons
      this->mpc_Ui->pc_WiCollapsed->setVisible(false);
      this->mpc_Ui->pc_WiExpanded->setVisible(true);

      // move widgets from popups back (order matters!)
      this->mpc_Ui->pc_WiExpanded->layout()->addWidget(this->mpc_Ui->pc_WiDll);
      this->mpc_Ui->pc_WiExpanded->layout()->addWidget(this->mpc_Ui->pc_WiAdvancedProperties);
      this->mpc_Ui->pc_WiExpanded->layout()->addWidget(this->mpc_Ui->pc_WiProgress);
      this->mpc_Ui->pc_VerticalLayout2->setStretch(2, 1);

      // hide all popups
      this->mpc_Ui->pc_PbProgress->setChecked(false);
      this->mpc_Ui->pc_PbDll->setChecked(false);
      this->mpc_Ui->pc_PbAdvSett->setChecked(false);
   }
   else
   {
      // show buttons
      this->mpc_Ui->pc_WiExpanded->setVisible(false);
      this->mpc_Ui->pc_WiCollapsed->setVisible(true);

      // move widgets to popups
      this->mpc_PopupAdvSett->SetWidget(this->mpc_Ui->pc_WiAdvancedProperties);
      this->mpc_PopupDllConfig->SetWidget(this->mpc_Ui->pc_WiDll);
      this->mpc_PopupProgress->SetWidget(this->mpc_Ui->pc_WiProgress);

      // open last open popup
      switch (C_UsHandler::h_GetInstance()->GetPopOpenSection())
      {
      case C_UsHandler::ePROGRESS_LOG:
         this->mpc_Ui->pc_PbProgress->setChecked(true);
         break;
      case C_UsHandler::eDLLCONFIG:
         this->mpc_Ui->pc_PbDll->setChecked(true);
         break;
      case C_UsHandler::eADVANCED_SETTINGS:
         this->mpc_Ui->pc_PbAdvSett->setChecked(true);
         break;
      case C_UsHandler::eNONE:
      default:
         break;
      }
   }

   Q_EMIT (this->SigExpandSettings(oq_Expand));
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Hide progress popup.
*/
//----------------------------------------------------------------------------------------------------------------------
void C_FlaSetWidget::m_HidePopupProgress() const
{
   this->mpc_Ui->pc_PbProgress->setChecked(false);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Hide CAN DLL configuration popup.
*/
//----------------------------------------------------------------------------------------------------------------------
void C_FlaSetWidget::m_HidePopupDllConfig() const
{
   this->mpc_Ui->pc_PbDll->setChecked(false);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Hide advanced settings popup.
*/
//----------------------------------------------------------------------------------------------------------------------
void C_FlaSetWidget::m_HidePopupAdvSett() const
{
   this->mpc_Ui->pc_PbAdvSett->setChecked(false);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Check all popups for open one.

   \return
   opened popup enum or none if no popup is open
*/
//----------------------------------------------------------------------------------------------------------------------
C_UsHandler::E_SettingsSubSection C_FlaSetWidget::m_GetPopOpenIdentity() const
{
   C_UsHandler::E_SettingsSubSection e_Return;
   if (this->mpc_PopupProgress->isVisible() == true)
   {
      e_Return = C_UsHandler::ePROGRESS_LOG;
   }
   else if (this->mpc_PopupDllConfig->isVisible() == true)
   {
      e_Return = C_UsHandler::eDLLCONFIG;
   }
   else if (this->mpc_PopupAdvSett->isVisible() == true)
   {
      e_Return = C_UsHandler::eADVANCED_SETTINGS;
   }
   else
   {
      e_Return = C_UsHandler::eNONE;
   }

   return e_Return;
}
