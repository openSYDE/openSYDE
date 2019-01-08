//-----------------------------------------------------------------------------
/*!
   \file
   \brief       Base implementation of QChartView for showing C_OgeChaChartBase (header)

   See cpp file for detailed description

   \implementation
   project     openSYDE
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     04.05.2017  STW/B.Bayer
   \endimplementation
*/
//-----------------------------------------------------------------------------
#ifndef C_OGECHAVIEWBASE_H
#define C_OGECHAVIEWBASE_H

/* -- Includes ------------------------------------------------------------- */
#include <QChartView>
#include <QChart>

/* -- Namespace ------------------------------------------------------------ */
namespace stw_opensyde_gui_elements
{
/* -- Global Constants ----------------------------------------------------- */

/* -- Types ---------------------------------------------------------------- */

class C_OgeChaViewBase :
   public QtCharts::QChartView
{
   Q_OBJECT

public:
   C_OgeChaViewBase(QtCharts::QChart * const opc_Chart, QWidget * const opc_Parent = NULL);

protected:
   // The naming of the Qt parameters can't be changed and are not compliant with the naming conventions
   //lint -save -e1960
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
   QPointF mc_DragMovePos;
};

/* -- Extern Global Variables ---------------------------------------------- */
} //end of namespace

#endif
