//-----------------------------------------------------------------------------
/*!
   \internal
   \file
   \brief       Custom label for pop up sub title (implementation)

   Custom label for pop up sub title

   \implementation
   project     openSYDE
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     28.10.2016  STW/B.Bayer
   \endimplementation
*/
//-----------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------- */
#include "precomp_headers.h"

#include "C_OgeLabPopUpSubTitle.h"

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

   \created     21.07.2016  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
C_OgeLabPopUpSubTitle::C_OgeLabPopUpSubTitle(QWidget * const opc_Parent) :
   C_OgeLabPopUpTitle(opc_Parent)
{
}
