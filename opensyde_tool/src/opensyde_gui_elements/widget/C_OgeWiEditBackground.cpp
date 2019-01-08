//-----------------------------------------------------------------------------
/*!
   \internal
   \file
   \brief       Widget for showing the edit background (implementation)

   This class does not contain any functionality,
   but needs to exist, to have a unique group,
   to apply a specific stylesheet for.

   \implementation
   project     openSYDE
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     20.06.2017  STW/B.Bayer
   \endimplementation
*/
//-----------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------- */
#include "precomp_headers.h"

#include <QVariant>

#include "C_OgeWiEditBackground.h"

#include "C_OgeWiUtil.h"

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

   \created     20.06.2017  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
C_OgeWiEditBackground::C_OgeWiEditBackground(QWidget * const opc_Parent) :
   QWidget(opc_Parent)
{
}

//-----------------------------------------------------------------------------
/*!
   \brief   Sets the edit mode

   \param[in]     oq_Active       Flag for edit mode

   \created     20.06.2017  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
void C_OgeWiEditBackground::SetEditBackground(const bool oq_Active)
{
   stw_opensyde_gui_logic::C_OgeWiUtil::h_ApplyStylesheetProperty(this, "EditMode", oq_Active);
}

//-----------------------------------------------------------------------------
/*!
   \brief   Overwritten paint event slot

   Here: draw background
   (Not automatically drawn in any QWidget derivative)

   \param[in,out] opc_Event Event identification and information

   \created     17.10.2016  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_OgeWiEditBackground::paintEvent(QPaintEvent * const opc_Event)
{
   stw_opensyde_gui_logic::C_OgeWiUtil::h_DrawBackground(this);

   QWidget::paintEvent(opc_Event);
}
