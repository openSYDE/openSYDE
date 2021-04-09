//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Widget base for dashboard progress bar widget (header)

   See cpp file for detailed description

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_SYVDAITDASHBOARDPROGRESSBARWIDGET_H
#define C_SYVDAITDASHBOARDPROGRESSBARWIDGET_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <QWidget>
#include <QPixmap>
#include <QGroupBox>
#include "stwtypes.h"
#include "C_OgeLabDashboardDefault.h"
#include "C_PuiSvDbProgressBar.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace Ui
{
class C_SyvDaItDashboardProgressBarWidget;
}

namespace stw_opensyde_gui
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */
class C_SyvDaItDashboardProgressBarWidget :
   public QWidget
{
   Q_OBJECT

public:
   explicit C_SyvDaItDashboardProgressBarWidget(QWidget * const opc_Parent = NULL);
   ~C_SyvDaItDashboardProgressBarWidget(void);

   void SetDisplayStyle(const stw_opensyde_gui_logic::C_PuiSvDbWidgetBase::E_Style oe_Style,
                        const stw_opensyde_gui_logic::C_PuiSvDbProgressBar::E_Type oe_Type,
                        const stw_opensyde_gui_logic::C_PuiSvDbProgressBar::E_Alignment oe_Alignment,
                        const bool oq_DarkMode);
   void AdjustFontToSize(void);
   void SetValue(const stw_types::sintn osn_Value, const QString & orc_Value);
   void SetMin(const stw_types::sintn osn_Value, const QString & orc_Value);
   void SetMax(const stw_types::sintn osn_Value, const QString & orc_Value);
   void SetShowMinMax(const bool oq_Value);
   void SetUnit(const QString & orc_Value);
   void SetTransparence(const stw_types::sintn osn_Value);
   stw_opensyde_gui_logic::C_PuiSvDbProgressBar::E_Type GetType() const;

protected:
   virtual void resizeEvent(QResizeEvent * const opc_Event) override;
   virtual void paintEvent(QPaintEvent * const opc_Event);

private:
   Ui::C_SyvDaItDashboardProgressBarWidget * mpc_Ui;
   stw_opensyde_gui_logic::C_PuiSvDbProgressBar::E_Type me_Type;
   stw_opensyde_gui_logic::C_PuiSvDbProgressBar::E_Alignment me_Alignment;

   //Avoid call
   C_SyvDaItDashboardProgressBarWidget(const C_SyvDaItDashboardProgressBarWidget &);
   C_SyvDaItDashboardProgressBarWidget & operator =(const C_SyvDaItDashboardProgressBarWidget &);

   static void mh_DrawArrow(QPainter & orc_Painter, const QPointF & orc_P1, const QPointF & orc_P2,
                            const QPointF & orc_P3, const QPen & orc_Pen);
   static QPolygonF mh_GetOuterRect(const QPointF & orc_P1, const QPointF & orc_P2, const QPointF & orc_POpposite,
                                    const stw_types::float64 of64_Width);
   static QRectF mh_GetCircle(const QPointF & orc_P1, const stw_types::float64 of64_Width);
   static void mh_SetArc(QPainterPath & orc_Path, const QPointF & orc_PCenter, const QPointF & orc_P2,
                         const QPointF & orc_P3, const stw_types::float64 of64_Radius);
   static stw_types::float64 mh_GetArc(const QPointF & orc_V1, const QPointF & orc_V2);

   void m_SetLabelColor(QPainter & orc_Painter, QFont & orc_TextFont, QPen & orc_ArrowPen, QPen & orc_MinMaxPen,
                        QPen & orc_ValuePen) const;
   void m_SetBarMargins();
   QString mc_Value;
   QString mc_MinValue;
   QString mc_MaxValue;
   QString mc_Unit;
   stw_opensyde_gui_logic::C_PuiSvDbWidgetBase::E_Style me_Style;
   bool mq_ShowMinMax;
   bool mq_DarkMode;
   stw_types::float32 mf32_BarMarginTop;    // To Size the Progress Bar dependent on this->rect()
   stw_types::float32 mf32_BarMarginBottom; // To Size the Progress Bar dependent on this->rect()
   stw_types::float32 mf32_BarMarginLeft;   // To Size the Progress Bar dependent on this->rect()
   stw_types::float32 mf32_BarMarginRight;  // To Size the Progress Bar dependent on this->rect()
   stw_types::sintn msn_Transparence;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif
