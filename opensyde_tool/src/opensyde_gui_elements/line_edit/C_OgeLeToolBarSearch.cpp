//-----------------------------------------------------------------------------
/*!
   \internal
   \file
   \brief       Line edit field for toolbar search (implementation)

   Line edit field for toolbar search

   \implementation
   project     openSYDE
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     26.09.2016  STW/M.Echtler
   \endimplementation
*/
//-----------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------- */
#include "precomp_headers.h"

#include "C_OgeLeToolBarSearch.h"

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

   \created     26.09.2016  STW/M.Echtler
*/
//-----------------------------------------------------------------------------

C_OgeLeToolBarSearch::C_OgeLeToolBarSearch(QWidget * const opc_Parent) :
   C_OgeLeContextMenuBase(opc_Parent)
{
}
