//-----------------------------------------------------------------------------
/*!
   \internal
   \file
   \brief       Sub button in material design of navigation bar

   \implementation
   project     opensyde
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     08.07.2016  STW/B.Bayer
   \endimplementation
*/
//-----------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------- */
#include "precomp_headers.h"

#include <QFile>
#include "C_OgePubUseCaseNavSub.h"

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

   \created     08.07.2016  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
C_OgePubUseCaseNavSub::C_OgePubUseCaseNavSub(QWidget * const opc_Parent) :
   QPushButton(opc_Parent)
{
   this->setCheckable(true);
   this->setChecked(false);
}
