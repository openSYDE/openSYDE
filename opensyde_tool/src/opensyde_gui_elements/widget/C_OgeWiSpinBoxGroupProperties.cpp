//-----------------------------------------------------------------------------
/*!
   \internal
   \file
   \brief       Spin box types group with properties styling (implementation)

   Spin box types group with properties styling.
   This class does not contain any functionality,
   but needs to exist, to have a unique group,
   to apply a specific stylesheet for.

   \implementation
   project     openSYDE
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     25.04.2018  STW/M.Echtler
   \endimplementation
*/
//-----------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------- */
#include "precomp_headers.h"

#include "C_OgeWiSpinBoxGroupProperties.h"

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

   \created     25.04.2018  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
C_OgeWiSpinBoxGroupProperties::C_OgeWiSpinBoxGroupProperties(QWidget * const opc_Parent) :
   C_OgeWiSpinBoxGroup(opc_Parent)
{
   m_ResizeToDefault();
}
