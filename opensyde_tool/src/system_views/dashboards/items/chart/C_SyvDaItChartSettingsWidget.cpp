//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Widget for configuration of chart functionality

   Widget for configuration of chart functionality

   \copyright   Copyright 2019 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.h"

#include "C_SyvDaItChartSettingsWidget.h"
#include "ui_C_SyvDaItChartSettingsWidget.h"

#include "C_GtGetText.h"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw_types;
using namespace stw_opensyde_gui;
using namespace stw_opensyde_gui_logic;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */
const sintn C_SyvDaItChartSettingsWidget::mhsn_INDEX_CB_SETTING_ZM_XY = 0;
const sintn C_SyvDaItChartSettingsWidget::mhsn_INDEX_CB_SETTING_ZM_X = 1;
const sintn C_SyvDaItChartSettingsWidget::mhsn_INDEX_CB_SETTING_ZM_Y = 2;

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
C_SyvDaItChartSettingsWidget::C_SyvDaItChartSettingsWidget(QWidget * const opc_Parent) :
   QWidget(opc_Parent),
   mpc_Ui(new Ui::C_SyvDaItChartSettingsWidget)
{
   this->mpc_Ui->setupUi(this);

   this->InitText();

   //lint -e{929} Cast required to avoid ambiguous signal of qt interface
   connect(this->mpc_Ui->pc_ComboBoxZoomMode,
           static_cast<void (QComboBox::*)(sintn)>(&QComboBox::currentIndexChanged), this,
           &C_SyvDaItChartSettingsWidget::m_ComboBoxZoomModeChanged);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default destructor
*/
//----------------------------------------------------------------------------------------------------------------------
C_SyvDaItChartSettingsWidget::~C_SyvDaItChartSettingsWidget()
{
   delete this->mpc_Ui;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Initializes all visible strings on the widget
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaItChartSettingsWidget::InitText(void) const
{
   this->mpc_Ui->pc_LabelZoomMode->setText(C_GtGetText::h_GetText("Zoom mode:"));
   this->mpc_Ui->pc_LabelZoomMode->SetToolTipInformation(C_GtGetText::h_GetText("Zoom mode"),
                                                         C_GtGetText::h_GetText("Different zoom modes:\n"
                                                                                "Adjust all directions: Both axes will be zoomed\n"
                                                                                "Adjust X-Axis: Time axis will be zoomed\n"
                                                                                "Adjust Y-Axis: Value axis will be zoomed\n\n"
                                                                                "Shortcuts:\n"
                                                                                "\"+\": Zooming in\n"
                                                                                "\"-\": Zooming out\n"
                                                                                "\"0\": Reset the entire zoom and scrolling configuration\n"));

   // Init combo box
   this->mpc_Ui->pc_ComboBoxZoomMode->addItem(C_GtGetText::h_GetText("Adjust all directions"));
   this->mpc_Ui->pc_ComboBoxZoomMode->addItem(C_GtGetText::h_GetText("Adjust X-Axis"));
   this->mpc_Ui->pc_ComboBoxZoomMode->addItem(C_GtGetText::h_GetText("Adjust Y-Axis"));
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Sets the chart data as initialize configuration

   \param[in]     orc_Data       Chart data with configured data elements
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaItChartSettingsWidget::SetData(const C_PuiSvDbChart & orc_Data) const
{
   this->m_SetComboBoxZoomModeValue(orc_Data.e_SettingZoomMode);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Returns the current set setting for the zoom mode

   \retval   eSETTING_ZM_XY   Adjust all directions (default)
   \retval   eSETTING_ZM_X    Adjust X-Axis
   \retval   eSETTING_ZM_Y    Adjust Y-Axis
*/
//----------------------------------------------------------------------------------------------------------------------
C_PuiSvDbChart::E_SettingZoomMode C_SyvDaItChartSettingsWidget::GetSettingZoomMode(void) const
{
   C_PuiSvDbChart::E_SettingZoomMode e_Setting;

   switch (this->mpc_Ui->pc_ComboBoxZoomMode->currentIndex())
   {
   case mhsn_INDEX_CB_SETTING_ZM_XY:
      e_Setting = C_PuiSvDbChart::eSETTING_ZM_XY;
      break;
   case mhsn_INDEX_CB_SETTING_ZM_X:
      e_Setting = C_PuiSvDbChart::eSETTING_ZM_X;
      break;
   case mhsn_INDEX_CB_SETTING_ZM_Y:
      e_Setting = C_PuiSvDbChart::eSETTING_ZM_Y;
      break;
   default:
      e_Setting = C_PuiSvDbChart::eSETTING_ZM_XY;
      break;
   }

   return e_Setting;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Slot function for change of the zoom mode change
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaItChartSettingsWidget::m_ComboBoxZoomModeChanged(void) const
{
   Q_EMIT (this->SigZoomModeChanged(this->GetSettingZoomMode()));
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Sets the current zoom mode in the combo box

   \param[in]       oe_SettingZoomMode     Current used zoom mode
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaItChartSettingsWidget::m_SetComboBoxZoomModeValue(const C_PuiSvDbChart::E_SettingZoomMode oe_SettingZoomMode)
const
{
   switch (oe_SettingZoomMode)
   {
   case C_PuiSvDbChart::eSETTING_ZM_XY:
      this->mpc_Ui->pc_ComboBoxZoomMode->setCurrentIndex(mhsn_INDEX_CB_SETTING_ZM_XY);
      break;
   case C_PuiSvDbChart::eSETTING_ZM_X:
      this->mpc_Ui->pc_ComboBoxZoomMode->setCurrentIndex(mhsn_INDEX_CB_SETTING_ZM_X);
      break;
   case C_PuiSvDbChart::eSETTING_ZM_Y:
      this->mpc_Ui->pc_ComboBoxZoomMode->setCurrentIndex(mhsn_INDEX_CB_SETTING_ZM_Y);
      break;
   default:
      this->mpc_Ui->pc_ComboBoxZoomMode->setCurrentIndex(mhsn_INDEX_CB_SETTING_ZM_XY);
      break;
   }
}
