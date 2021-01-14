//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Base implementation of QChartView for showing C_OgeChaChartBase (header)

   See cpp file for detailed description

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_OGECHAVIEWBASE_H
#define C_OGECHAVIEWBASE_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <QPoint>
#include <QChartView>
#include <QChart>
//#include <QRubberBand>

#include "stwtypes.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw_opensyde_gui_elements
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_OgeChaViewBase :
   public QtCharts::QChartView
{
   Q_OBJECT

public:
   enum E_SettingZoomMode
   {
      eSETTING_ZM_XY,
      eSETTING_ZM_X,
      eSETTING_ZM_Y
   };

   C_OgeChaViewBase(QtCharts::QChart * const opc_Chart = NULL, QWidget * const opc_Parent = NULL);

   void SetDrawingActive(const bool oq_Active);
   void SetZoomMode(const E_SettingZoomMode oe_ZoomMode);

   //The signals keyword is necessary for Qt signal slot functionality
   //lint -save -e1736

Q_SIGNALS:
   //lint -restore
   void SigYZoomed(const stw_types::float64 of64_Factor) const;
   void SigScrolled(const stw_types::float64 of64_X, const stw_types::float64 of64_Y) const;
   void SigZoomReseted(void) const;

protected:
   virtual void paintEvent(QPaintEvent * const opc_Event) override;
   virtual bool viewportEvent(QEvent * const opc_Event) override;
   virtual void mousePressEvent(QMouseEvent * const opc_Event) override;
   virtual void mouseMoveEvent(QMouseEvent * const opc_Event) override;
   virtual void mouseReleaseEvent(QMouseEvent * const opc_Event) override;
   virtual void keyPressEvent(QKeyEvent * const opc_Event) override;
   virtual void wheelEvent(QWheelEvent * const opc_Event) override;

private:
   bool mq_IsTouching;
   bool mq_DragMoveActive;
   bool mq_DrawingActive;
   QPointF mc_DragMovePos;
   QPoint mc_RubberBandStartPos;
   E_SettingZoomMode me_ZoomMode;
   //QRubberBand * mpc_RubberBand;

   void m_Zoom(const bool oq_ZoomIn, const QPointF & orc_ZoomPos) const;
   void m_Scroll(const QPointF & orc_Start, const QPointF & orc_End) const;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif
