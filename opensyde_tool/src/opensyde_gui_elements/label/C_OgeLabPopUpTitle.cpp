//-----------------------------------------------------------------------------
/*!
   \internal
   \file
   \brief       Custom label for pop up title (implementation)

   Custom label for pop up title

   \implementation
   project     openSYDE
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     21.07.2016  STW/M.Echtler
   \endimplementation
*/
//-----------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------- */
#include "precomp_headers.h"

#include "C_OgeLabPopUpTitle.h"

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
C_OgeLabPopUpTitle::C_OgeLabPopUpTitle(QWidget * const opc_Parent) :
   QLabel(opc_Parent)
{
}

//-----------------------------------------------------------------------------
/*!
   \brief   Overwritten mouse press event slot

   Here: Emit public signal

   \param[in,out] opc_Event Event identification and information

   \created     21.07.2016  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_OgeLabPopUpTitle::mousePressEvent(QMouseEvent * const opc_Event)
{
   Q_EMIT this->SigMousePress(opc_Event);

   QLabel::mousePressEvent(opc_Event);
}

//-----------------------------------------------------------------------------
/*!
   \brief   Overwritten mouse release event slot

   Here: Emit public signal

   \param[in,out] opc_Event Event identification and information

   \created     21.07.2016  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_OgeLabPopUpTitle::mouseReleaseEvent(QMouseEvent * const opc_Event)
{
   Q_EMIT this->SigMouseRelease(opc_Event);

   QLabel::mouseReleaseEvent(opc_Event);
}

//-----------------------------------------------------------------------------
/*!
   \brief   Overwritten mouse move event slot

   Here: Emit public signal

   \param[in,out] opc_Event Event identification and information

   \created     21.07.2016  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_OgeLabPopUpTitle::mouseMoveEvent(QMouseEvent * const opc_Event)
{
   Q_EMIT this->SigMouseMove(opc_Event);

   QLabel::mouseMoveEvent(opc_Event);
}
