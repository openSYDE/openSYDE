//-----------------------------------------------------------------------------
/*!
   \internal
   \file
   \brief       Base implementation for Qt chart (implementation)

   \implementation
   project     openSYDE
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     04.05.2017  STW/B.Bayer
   \endimplementation
*/
//-----------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------- */
#include "precomp_headers.h"

#include "C_OgeChaChartBase.h"

/* -- Used Namespaces ------------------------------------------------------ */
using namespace QtCharts;
using namespace stw_opensyde_gui_elements;

/* -- Module Global Constants ---------------------------------------------- */

/* -- Types ---------------------------------------------------------------- */

/* -- Global Variables ----------------------------------------------------- */

/* -- Module Global Variables ---------------------------------------------- */

/* -- Module Global Function Prototypes ------------------------------------ */

/* -- Implementation ------------------------------------------------------- */

//-----------------------------------------------------------------------------
/*!
   \brief   Default constructor

   \param[in,out] opc_Parent Optional pointer to parent

   \created     04.05.2017  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
C_OgeChaChartBase::C_OgeChaChartBase(QGraphicsItem * const opc_Parent) :
   QChart(opc_Parent)
{
}
