//-----------------------------------------------------------------------------
/*!
   \internal
   \file
   \brief       Custom transparent tristate check box toggle (implementation)

   Custom transparent tristate check box toggle.
   This class does not contain any functionality,
   but needs to exist, to have a unique group,
   to apply a specific stylesheet for.

   \implementation
   project     openSYDE
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     14.02.2017  STW/M.Echtler
   \endimplementation
*/
//-----------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------- */
#include "precomp_headers.h"

#include "C_OgeChxTristateTransparentToggle.h"

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

   \created     14.02.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
C_OgeChxTristateTransparentToggle::C_OgeChxTristateTransparentToggle(QWidget * const opc_Parent) :
   C_OgeChxTristateBase(opc_Parent)
{
}
