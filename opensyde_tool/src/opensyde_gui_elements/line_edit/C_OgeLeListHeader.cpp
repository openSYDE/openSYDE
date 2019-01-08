//-----------------------------------------------------------------------------
/*!
   \internal
   \file
   \brief       Line edit for list header (implementation)

   Line edit for list header.
   This class does not contain any functionality,
   but needs to exist, to have a unique group,
   to apply a specific stylesheet for.

   \implementation
   project     openSYDE
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     18.01.2017  STW/M.Echtler
   \endimplementation
*/
//-----------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------- */
#include "precomp_headers.h"

#include "C_OgeLeListHeader.h"

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

   \created     17.01.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
C_OgeLeListHeader::C_OgeLeListHeader(QWidget * const opc_Parent) :
   C_OgeLeToolTipBase(opc_Parent),
   mq_JustGotFocus(false)
{
}

//-----------------------------------------------------------------------------
/*!
   \brief   Overwritten focus in event slot

   Here: Signal and remember

   \param[in,out] opc_Event Event identification and information

   \created     18.01.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_OgeLeListHeader::focusInEvent(QFocusEvent * const opc_Event)
{
   QLineEdit::focusInEvent(opc_Event);
   Q_EMIT this->SigFocus();
   this->mq_JustGotFocus = true;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Overwritten mouse press event slot

   Here: Select all if necessary

   \param[in,out] opc_Event Event identification and information

   \created     18.01.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_OgeLeListHeader::mousePressEvent(QMouseEvent * const opc_Event)
{
   QLineEdit::mousePressEvent(opc_Event);
   if (this->mq_JustGotFocus == true)
   {
      this->mq_JustGotFocus = false;
      this->selectAll();
   }
}
