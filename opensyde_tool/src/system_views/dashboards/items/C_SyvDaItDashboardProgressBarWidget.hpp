//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Widget base for dashboard progress bar widget (header)

   See cpp file for detailed description

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_SYVDAITDASHBOARDPROGRESSBARWIDGET_HPP
#define C_SYVDAITDASHBOARDPROGRESSBARWIDGET_HPP

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <QWidget>
#include <QPixmap>
#include <QGroupBox>
#include "stwtypes.hpp"
#include "C_OgeLabDashboardDefault.hpp"
#include "C_PuiSvDbProgressBar.hpp"
#include "C_OgeDashboardProgressBar.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace Ui
{
class C_SyvDaItDashboardProgressBarWidget;
}

namespace stw
{
namespace opensyde_gui
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */
class C_SyvDaItDashboardProgressBarWidget :
   public QWidget
{
   Q_OBJECT

public:
   explicit C_SyvDaItDashboardProgressBarWidget(QWidget * const opc_Parent = NULL);
   ~C_SyvDaItDashboardProgressBarWidget(void) override;

   void SetDisplayStyle(const stw::opensyde_gui_logic::C_PuiSvDbWidgetBase::E_Style oe_Style,
                        const stw::opensyde_gui_logic::C_PuiSvDbProgressBar::E_Type oe_Type,
                        const stw::opensyde_gui_logic::C_PuiSvDbProgressBar::E_Alignment oe_Alignment,
                        const bool oq_DarkMode);
   void AdjustFontToSize(void);
   void SetValue(const int32_t os32_Value, const QString & orc_Value);
   void SetMin(const int32_t os32_Value, const QString & orc_Value);
   void SetMax(const int32_t os32_Value, const QString & orc_Value);
   void SetShowMinMax(const bool oq_Value);
   void SetUnit(const QString & orc_Value);
   void SetTransparency(const int32_t os32_Value);
   stw::opensyde_gui_logic::C_PuiSvDbProgressBar::E_Type GetType() const;

protected:
   void resizeEvent(QResizeEvent * const opc_Event) override;
   void paintEvent(QPaintEvent * const opc_Event) override;

private:
   //Avoid call
   C_SyvDaItDashboardProgressBarWidget(const C_SyvDaItDashboardProgressBarWidget &);
   C_SyvDaItDashboardProgressBarWidget & operator =(const C_SyvDaItDashboardProgressBarWidget &) &;

   void m_SetStyle(QPainter & orc_Painter, QPen & orc_Pen) const;
   void m_SetBarMargins(void);

   //-------Set Styling-------------------------------------------------------------------------------------------------
   void m_SetDarkModeStyle(QPainter & orc_Painter, QPen & orc_Pen) const;
   void m_SetBrightModeStyle(QPainter & orc_Painter, QPen & orc_Pen) const;
   void m_SetMinimumProgressbarSize(void);

   //-------Variables---------------------------------------------------------------------------------------------------
   Ui::C_SyvDaItDashboardProgressBarWidget * mpc_Ui;
   stw::opensyde_gui_logic::C_PuiSvDbProgressBar::E_Type me_Type;
   stw::opensyde_gui_logic::C_PuiSvDbProgressBar::E_Alignment me_Alignment;

   QString mc_Value;
   QString mc_MinValue;
   QString mc_MaxValue;
   QString mc_Unit;
   stw::opensyde_gui_logic::C_PuiSvDbWidgetBase::E_Style me_Style;
   bool mq_ShowMinMax;
   bool mq_DarkMode;
   int32_t ms32_Transparency;

   //-------Constants---------------------------------------------------------------------------------------------------
   const float32_t mf32_MinimumFontSize = 15.0F;
   const float32_t mf32_BorderOffset = 5.0F;

   const float32_t mf32_ArrowOffset = 10.0F;
   const QString mc_ArrowSpacerString = "8";
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
