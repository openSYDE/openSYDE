//-----------------------------------------------------------------------------
/*!
   \internal
   \file
   \brief       Label for dashboard label item value (implementation)

   Label for dashboard label item value.
   This class does not contain any functionality,
   but needs to exist, to have a unique group,
   to apply a specific stylesheet for.

   \implementation
   project     openSYDE
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     11.08.2017  STW/M.Echtler
   \endimplementation
*/
//-----------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------- */
#include "precomp_headers.h"

#include "C_OgeWiUtil.h"
#include "C_OgeLabDashboardLabelValue.h"

/* -- Used Namespaces ------------------------------------------------------ */
using namespace stw_opensyde_gui_logic;
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

   \created     11.08.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
C_OgeLabDashboardLabelValue::C_OgeLabDashboardLabelValue(QWidget * const opc_Parent) :
   C_OgeLabAdaptiveSize(opc_Parent)
{
   C_OgeWiUtil::h_ApplyStylesheetProperty(this, "Transparency", "255");
}
