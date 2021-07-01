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
#include "C_PuiSvDbSlider.h"
#include "C_OSCNodeDataPoolContent.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace Ui
{
class C_SyvDaItDashboardSliderWidget;
}

namespace stw_opensyde_gui
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */
class C_SyvDaItDashboardSliderWidget :
   public QWidget
{
   Q_OBJECT

public:
   explicit C_SyvDaItDashboardSliderWidget(QWidget * const opc_Parent = NULL);
   ~C_SyvDaItDashboardSliderWidget(void);

   void SetCurrentStyle(const stw_opensyde_gui_logic::C_PuiSvDbSlider::E_Style oe_Style, const bool oq_IsDark) const;
   void SetDisplayStyle(const stw_opensyde_gui_logic::C_PuiSvDbSlider::E_Type oe_Type) const;
   void AdjustFontToSize(void) const;
   void SetMinMax(const stw_types::sintn osn_MinValue, const QString & orc_MinString,
                  const stw_types::sintn osn_MaxValue, const QString & orc_MaxString);
   void SetShowMinMax(const bool oq_Value);
   void SetValue(const stw_types::sintn osn_Value) const;
   stw_types::sintn GetValue(void) const;
   void SetToolTipParameters(const stw_types::float64 of64_ToolTipRangeOffset,
                             const stw_types::float64 of64_ToolTipOffset, const stw_types::float64 of64_ToolTipFactor,
                             const stw_opensyde_core::C_OSCNodeDataPoolContent::E_Type oe_RepresentationType) const;

   //The signals keyword is necessary for Qt signal slot functionality
   //lint -save -e1736

Q_SIGNALS:
   //lint -restore
   void SigOnChange(void);

protected:
   virtual void resizeEvent(QResizeEvent * const opc_Event) override;

private:
   Ui::C_SyvDaItDashboardSliderWidget * mpc_Ui;
   QString mc_Min;
   QString mc_Max;
   bool mq_ShowMinMax;

   void m_UpdateLabels(void) const;
   //Avoid call
   C_SyvDaItDashboardSliderWidget(const C_SyvDaItDashboardSliderWidget &);
   C_SyvDaItDashboardSliderWidget & operator =(const C_SyvDaItDashboardSliderWidget &);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif
