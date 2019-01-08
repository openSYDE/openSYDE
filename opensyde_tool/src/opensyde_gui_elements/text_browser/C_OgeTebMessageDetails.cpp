//-----------------------------------------------------------------------------
/*!
   \internal
   \file
   \brief       Text browser field for message details (implementation)

   Made for message dialog (error, information, question, warning).
   Text browser field for property comment.
   This class does not contain any functionality,
   but needs to exist, to have a unique group,
   to apply a specific stylesheet for.

   \implementation
   project     openSYDE
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     19.04.2018  STW/G.Scupin
   \endimplementation
*/
//-----------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------- */
#include "precomp_headers.h"

#include "C_OgeTebMessageDetails.h"

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
   \brief   Default constructor.

   Set up GUI with all elements.

   \param[in,out] opc_Parent Optional pointer to parent

   \created     19.04.2018  STW/G.Scupin
*/
//-----------------------------------------------------------------------------
C_OgeTebMessageDetails::C_OgeTebMessageDetails(QWidget * const opc_Parent) :
   C_OgeTebContextMenuBase(opc_Parent)
{
}
