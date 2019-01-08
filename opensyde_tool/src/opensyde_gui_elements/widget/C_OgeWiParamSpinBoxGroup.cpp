//-----------------------------------------------------------------------------
/*!
   \internal
   \file
   \brief       Widget for param widget spin boxes (implementation)

   Widget for param widget spin boxes.
   This class does not contain any functionality,
   but needs to exist, to have a unique group,
   to apply a specific stylesheet for.

   \implementation
   project     openSYDE
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     29.10.2018  STW/M.Echtler
   \endimplementation
*/
//-----------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------- */
#include "precomp_headers.h"

#include "C_OgeWiParamSpinBoxGroup.h"

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

   \created     29.10.2018  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
C_OgeWiParamSpinBoxGroup::C_OgeWiParamSpinBoxGroup(QWidget * const opc_Parent) :
   C_OgeWiSpinBoxGroup(opc_Parent)
{
}
