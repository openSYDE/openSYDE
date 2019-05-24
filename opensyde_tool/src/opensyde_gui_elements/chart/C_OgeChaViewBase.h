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
#include <QChartView>
#include <QChart>

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
   C_OgeChaViewBase(QtCharts::QChart * const opc_Chart, QWidget * const opc_Parent = NULL);

   void SetDrawingActive(const bool oq_Active);

protected:
   // The naming of the Qt parameters can't be changed and are not compliant with the naming conventions
   //lint -save -e1960

   virtual void paintEvent(QPaintEvent * const opc_Event) override;
   virtual bool viewportEvent(QEvent * const opc_Event) override;
   virtual void mousePressEvent(QMouseEvent * const opc_Event) override;
   virtual void mouseMoveEvent(QMouseEvent * const opc_Event) override;
   virtual void mouseReleaseEvent(QMouseEvent * const opc_Event) override;
   virtual void keyPressEvent(QKeyEvent * const opc_Event) override;
   virtual void wheelEvent(QWheelEvent * const opc_Event) override;
   //lint -restore

private:
   bool mq_IsTouching;
   bool mq_DragMoveActive;
   bool mq_DrawingActive;
   QPointF mc_DragMovePos;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif
