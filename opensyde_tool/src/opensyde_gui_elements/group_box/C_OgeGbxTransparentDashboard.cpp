//-----------------------------------------------------------------------------
/*!
   \internal
   \file
   \brief       Custom transparent group box for dashboard (implementation)

   Custom transparent group box for dashboard

   \implementation
   project     openSYDE
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     25.08.2017  STW/M.Echtler
   \endimplementation
*/
//-----------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------- */
#include "precomp_headers.h"

#include "C_OgeGbxTransparentDashboard.h"

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

   \created     25.08.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
C_OgeGbxTransparentDashboard::C_OgeGbxTransparentDashboard(QWidget * const opc_Parent) :
   QGroupBox(opc_Parent)
{
}

//-----------------------------------------------------------------------------
/*!
   \brief   Overwritten resize event slot

   Here: signal resize

   \param[in,out] opc_Event Event identification and information

   \created     25.08.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_OgeGbxTransparentDashboard::resizeEvent(QResizeEvent * const opc_Event)
{
   QGroupBox::resizeEvent(opc_Event);
   Q_EMIT this->SigResized();
}
