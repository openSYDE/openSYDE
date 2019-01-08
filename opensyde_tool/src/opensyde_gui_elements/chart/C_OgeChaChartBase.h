//-----------------------------------------------------------------------------
/*!
   \file
   \brief       Base implementation for Qt chart (header)

   See cpp file for detailed description

   \implementation
   project     openSYDE
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     04.05.2017  STW/B.Bayer
   \endimplementation
*/
//-----------------------------------------------------------------------------
#ifndef C_OGECHABASE_H
#define C_OGECHABASE_H

/* -- Includes ------------------------------------------------------------- */
#include <QChart>
#include <QGestureEvent>

/* -- Namespace ------------------------------------------------------------ */
namespace stw_opensyde_gui_elements
{
/* -- Global Constants ----------------------------------------------------- */

/* -- Types ---------------------------------------------------------------- */

class C_OgeChaChartBase :
   public QtCharts::QChart
{
public:
   C_OgeChaChartBase(QGraphicsItem * const opc_Parent = NULL);
};

/* -- Extern Global Variables ---------------------------------------------- */
} //end of namespace

#endif
