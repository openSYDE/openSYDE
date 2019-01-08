//-----------------------------------------------------------------------------
/*!
   \internal
   \file
   \brief       Custom widget to make mouse move possible (implementation)

   This widget is made for messages (error, information, question and warning).
   If the widget is added on the dialog window and properly connected, it is
   possible to move the window by mouse-grabbing it at the widget.

   Adapted from C_OgeLabPopUpTitle

   \implementation
   project     openSYDE
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     20.04.2018  STW/G.Scupin
   \endimplementation
*/
//-----------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------- */
#include "precomp_headers.h"

#include "C_OgeGbxMouseMove.h"

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

   \created     20.04.2018  STW/G.Scupin
*/
//-----------------------------------------------------------------------------
C_OgeGbxMouseMove::C_OgeGbxMouseMove(QWidget * const opc_Parent) :
   QGroupBox(opc_Parent)
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
void C_OgeGbxMouseMove::mousePressEvent(QMouseEvent * const opc_Event)
{
   Q_EMIT this->SigMousePress(opc_Event);

   QGroupBox::mousePressEvent(opc_Event);
}

//-----------------------------------------------------------------------------
/*!
   \brief   Overwritten mouse release event slot

   Here: Emit public signal

   \param[in,out] opc_Event Event identification and information

   \created     21.07.2016  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_OgeGbxMouseMove::mouseReleaseEvent(QMouseEvent * const opc_Event)
{
   Q_EMIT this->SigMouseRelease(opc_Event);

   QGroupBox::mouseReleaseEvent(opc_Event);
}

//-----------------------------------------------------------------------------
/*!
   \brief   Overwritten mouse move event slot

   Here: Emit public signal

   \param[in,out] opc_Event Event identification and information

   \created     21.07.2016  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_OgeGbxMouseMove::mouseMoveEvent(QMouseEvent * const opc_Event)
{
   Q_EMIT this->SigMouseMove(opc_Event);

   QGroupBox::mouseMoveEvent(opc_Event);
}
