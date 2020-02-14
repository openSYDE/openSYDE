//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Widget for configuration of chart functionality

   \copyright   Copyright 2019 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_SYVDAITCHARTSETTINGSWIDGET_H
#define C_SYVDAITCHARTSETTINGSWIDGET_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <QWidget>

#include "stwtypes.h"

#include "C_PuiSvDbChart.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace Ui
{
class C_SyvDaItChartSettingsWidget;
}

namespace stw_opensyde_gui
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_SyvDaItChartSettingsWidget :
   public QWidget
{
   Q_OBJECT

public:
   explicit C_SyvDaItChartSettingsWidget(QWidget * const opc_Parent = NULL);
   ~C_SyvDaItChartSettingsWidget(void);

   void InitText(void) const;
   void SetData(const stw_opensyde_gui_logic::C_PuiSvDbChart & orc_Data) const;
   stw_opensyde_gui_logic::C_PuiSvDbChart::E_SettingZoomMode GetSettingZoomMode(void) const;
   void ApplyDarkMode(const stw_opensyde_gui_logic::C_PuiSvDbWidgetBase::E_Style oe_Style,
                      const bool oq_DarkMode) const;

   //The signals keyword is necessary for Qt signal slot functionality
   //lint -save -e1736

Q_SIGNALS:
   //lint -restore
   void SigZoomModeChanged(const stw_opensyde_gui_logic::C_PuiSvDbChart::E_SettingZoomMode oe_SettingZoomMode) const;

private:
   Ui::C_SyvDaItChartSettingsWidget * mpc_Ui;

   static const stw_types::sintn mhsn_INDEX_CB_SETTING_ZM_XY;
   static const stw_types::sintn mhsn_INDEX_CB_SETTING_ZM_X;
   static const stw_types::sintn mhsn_INDEX_CB_SETTING_ZM_Y;

   //Avoid call
   C_SyvDaItChartSettingsWidget(const C_SyvDaItChartSettingsWidget &);
   C_SyvDaItChartSettingsWidget & operator =(const C_SyvDaItChartSettingsWidget &);

   void m_ComboBoxZoomModeChanged(void) const;
   void m_SetComboBoxZoomModeValue(const stw_opensyde_gui_logic::C_PuiSvDbChart::E_SettingZoomMode oe_SettingZoomMode)
   const;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif
