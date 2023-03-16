//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Widget base for dashboard slider widget (header)

   See cpp file for detailed description

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_SYVDAITDASHBOARDSLIDERWIDGET_H
#define C_SYVDAITDASHBOARDSLIDERWIDGET_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <QWidget>
#include "C_PuiSvDbSlider.hpp"
#include "C_OscNodeDataPoolContent.hpp"
#include "C_PuiSvDbDataElementDisplayFormatterConfig.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace Ui
{
class C_SyvDaItDashboardSliderWidget;
}

namespace stw
{
namespace opensyde_gui
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */
class C_SyvDaItDashboardSliderWidget :
   public QWidget
{
   Q_OBJECT

public:
   explicit C_SyvDaItDashboardSliderWidget(QWidget * const opc_Parent = NULL);
   ~C_SyvDaItDashboardSliderWidget(void) override;

   void SetCurrentStyle(const stw::opensyde_gui_logic::C_PuiSvDbSlider::E_Style oe_Style, const bool oq_IsDark) const;
   void SetDisplayStyle(const stw::opensyde_gui_logic::C_PuiSvDbSlider::E_Type oe_Type) const;
   void AdjustFontToSize(void) const;
   void SetMinMax(const int32_t os32_MinValue, const QString & orc_MinString, const int32_t os32_MaxValue,
                  const QString & orc_MaxString);
   void SetShowMinMax(const bool oq_Value);
   void SetValue(const int32_t os32_Value) const;
   int32_t GetMinValue(void) const;
   int32_t GetValue(void) const;
   void SetToolTipParameters(const float64_t of64_ToolTipSliderRangeValueFactor,
                             const float64_t of64_ToolTipValueOffset,
                             const stw::opensyde_gui_logic::C_PuiSvDbDataElementScaling & orc_ToolTipUserScaling,
                             const stw::opensyde_core::C_OscNodeDataPoolContent::E_Type oe_RepresentationType,
                             const stw::opensyde_gui_logic::C_PuiSvDbDataElementDisplayFormatterConfig & orc_FormatterConfig)
   const;

   //The signals keyword is necessary for Qt signal slot functionality
   //lint -save -e1736

Q_SIGNALS:
   //lint -restore
   void SigOnChange(void);

protected:
   void resizeEvent(QResizeEvent * const opc_Event) override;

private:
   Ui::C_SyvDaItDashboardSliderWidget * mpc_Ui;
   QString mc_Min;
   QString mc_Max;
   bool mq_ShowMinMax;

   void m_UpdateLabels(void) const;
   //Avoid call
   C_SyvDaItDashboardSliderWidget(const C_SyvDaItDashboardSliderWidget &);
   C_SyvDaItDashboardSliderWidget & operator =(const C_SyvDaItDashboardSliderWidget &) &;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
