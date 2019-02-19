//-----------------------------------------------------------------------------
/*!
   \internal
   \file
   \brief       Line edit for message ID in hex or integer format, styled for pop up dialog. (implementation)

   Line edit for message ID in hex or integer format, styled for pop up dialog..
   This class does not contain any functionality,
   but needs to exist, to have a unique group,
   to apply a specific stylesheet for.

   \implementation
   project     openSYDE
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     04.12.2018  STW/G.Landsgesell
   \endimplementation
*/
//-----------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------- */
#include "precomp_headers.h"

#include "C_CamOgeLeIDPopUp.h"

/* -- Used Namespaces ------------------------------------------------------ */
using namespace stw_opensyde_gui_elements;
using namespace stw_opensyde_gui;

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

   \created     04.12.2018  STW/G.Landsgesell
*/
//-----------------------------------------------------------------------------
C_CamOgeLeIDPopUp::C_CamOgeLeIDPopUp(QWidget * const opc_Parent) :
   C_TblEditLineEditBase(opc_Parent)
{
}
