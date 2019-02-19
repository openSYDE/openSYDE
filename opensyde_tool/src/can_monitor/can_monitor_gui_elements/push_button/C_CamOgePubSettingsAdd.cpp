//-----------------------------------------------------------------------------
/*!
   \internal
   \file
   \brief       Push button for "Add Filter" and "Add Database". (implementation)

   Push button for "Add Filter" and "Add Database"..
   This class does not contain any functionality,
   but needs to exist, to have a unique group,
   to apply a specific stylesheet for.

   \implementation
   project     openSYDE
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     27.11.2018  STW/G.Landsgesell
   \endimplementation
*/
//-----------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------- */
#include "precomp_headers.h"

#include "C_CamOgePubSettingsAdd.h"

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

   \created     27.11.2018  STW/G.Landsgesell
*/
//-----------------------------------------------------------------------------
C_CamOgePubSettingsAdd::C_CamOgePubSettingsAdd(QWidget * const opc_Parent) :
   C_OgePubToolTipBase(opc_Parent)
{
}
