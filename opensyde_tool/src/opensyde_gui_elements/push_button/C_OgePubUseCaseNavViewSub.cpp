//-----------------------------------------------------------------------------
/*!
   \internal
   \file
   \brief       View sub button in material design of navigation bar

   detailed description

   \implementation
   project     opensyde
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     20.07.2016  STW/S.Singer
   \endimplementation
*/
//-----------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------- */
#include "precomp_headers.h"

#include <QFile>
#include "C_OgePubUseCaseNavViewSub.h"

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

   \created     20.07.2016  STW/S.Singer
*/
//-----------------------------------------------------------------------------
C_OgePubUseCaseNavViewSub::C_OgePubUseCaseNavViewSub(QWidget * const opc_Parent) :
   QPushButton(opc_Parent)
{
   this->setCheckable(true);
   this->setChecked(false);
}
