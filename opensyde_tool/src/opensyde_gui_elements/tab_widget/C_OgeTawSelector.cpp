//-----------------------------------------------------------------------------
/*!
   \internal
   \file
   \brief       Tab widget for selecting modes (implementation)

   \implementation
   project     openSYDE
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     24.03.2017  STW/B.Bayer
   \endimplementation
*/
//-----------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------- */
#include "precomp_headers.h"

#include <QTabBar>
#include <QVariant>

#include "C_OgeTawSelector.h"

#include "C_OgeWiUtil.h"

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

   \created     24.03.2017  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
C_OgeTawSelector::C_OgeTawSelector(QWidget * const opc_Parent) :
   C_OgeTawToolTipBase(opc_Parent)
{
   //This property holds when expanding is true QTabBar will expand the tabs to use the empty space.
   this->tabBar()->setExpanding(false);
   stw_opensyde_gui_logic::C_OgeWiUtil::h_ApplyStylesheetProperty(this->tabBar(), "C_OgeTawSelector", true);
}
