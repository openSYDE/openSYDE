//-----------------------------------------------------------------------------
/*!
   \internal
   \file
   \brief       Line edit field for properties with tool tip (implementation)

   Line edit field for properties with tool tip

   \implementation
   project     openSYDE
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     17.01.2018  STW/M.Echtler
   \endimplementation
*/
//-----------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------- */
#include "precomp_headers.h"

#include "C_OgeLePropertiesToolTip.h"

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

   \created     17.01.2018  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
C_OgeLePropertiesToolTip::C_OgeLePropertiesToolTip(QWidget * const opc_Parent) :
   C_OgeLeToolTipBase(opc_Parent)
{
}
