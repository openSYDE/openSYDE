//-----------------------------------------------------------------------------
/*!
   \internal
   \file
   \brief       Generic line edit for table edit style for can monitor (implementation)

   Generic line edit for table edit style for can monitor

   \implementation
   project     openSYDE
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     29.11.2018  STW/M.Echtler
   \endimplementation
*/
//-----------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------- */
#include "precomp_headers.h"

#include "C_CamOgeLeTableEdit.h"

/* -- Used Namespaces ------------------------------------------------------ */
using namespace stw_opensyde_gui;
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

   \created     29.11.2018  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
C_CamOgeLeTableEdit::C_CamOgeLeTableEdit(QWidget * const opc_Parent) :
   C_TblEditLineEditBase(opc_Parent)
{
}
