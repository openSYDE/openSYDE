//-----------------------------------------------------------------------------
/*!
   \internal
   \file
   \brief       Tab widget for page navigation (implementation)

   Tab widget for page navigation.
   This class does not contain any functionality,
   but needs to exist, to have a unique group,
   to apply a specific stylesheet for.

   \implementation
   project     openSYDE
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     27.09.2016  STW/M.Echtler
   \endimplementation
*/
//-----------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------- */
#include "precomp_headers.h"

#include <QTabBar>

#include "C_OgeTawPageNavi.h"

/* -- Used Namespaces ------------------------------------------------------ */

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

   Set up GUI with all elements.

   \param[in,out] opc_Parent Optional pointer to parent

   \created     27.09.2016  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
C_OgeTawPageNavi::C_OgeTawPageNavi(QWidget * const opc_Parent) :
   C_OgeTawToolTipBase(opc_Parent)
{
   //This property holds when expanding is true QTabBar will expand the tabs to use the empty space.
   this->tabBar()->setExpanding(false);
}
