//-----------------------------------------------------------------------------
/*!
   \internal
   \file
   \brief       Group box with white background and top border (implementation)

   Group box with white background and top border

   \implementation
   project     openSYDE
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     30.11.2018  STW/M.Echtler
   \endimplementation
*/
//-----------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------- */
#include "precomp_headers.h"

#include "C_CamOgeGbxMessageGeneratorBackground.h"

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

   \created     30.11.2018  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
C_CamOgeGbxMessageGeneratorBackground::C_CamOgeGbxMessageGeneratorBackground(QWidget * const opc_Parent) :
   QGroupBox(opc_Parent)
{
}
