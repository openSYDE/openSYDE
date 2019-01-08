//-----------------------------------------------------------------------------
/*!
   \internal
   \file
   \brief       Custom default check box with a disabled checked state (implementation)

   Custom default check box with a disabled checked state

   \implementation
   project     openSYDE
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     15.01.2018  STW/M.Echtler
   \endimplementation
*/
//-----------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------- */
#include "precomp_headers.h"

#include "C_OgeWiUtil.h"
#include "C_OgeChxDefaultCheckedDisabled.h"

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

   \created     15.01.2018  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
C_OgeChxDefaultCheckedDisabled::C_OgeChxDefaultCheckedDisabled(QWidget * const opc_Parent) :
   C_OgeChxToolTipBase(opc_Parent)
{
}
