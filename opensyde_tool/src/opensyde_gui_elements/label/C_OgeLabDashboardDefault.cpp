//-----------------------------------------------------------------------------
/*!
   \internal
   \file
   \brief       Default label for dashboard, scaling (implementation)

   Default label for dashboard, scaling.

   \implementation
   project     openSYDE
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     17.08.2017  STW/M.Echtler
   \endimplementation
*/
//-----------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------- */
#include "precomp_headers.h"

#include "C_OgeLabDashboardDefault.h"

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

   \created     17.08.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
C_OgeLabDashboardDefault::C_OgeLabDashboardDefault(QWidget * const opc_Parent) :
   C_OgeLabAdaptiveSize(opc_Parent)
{
}
