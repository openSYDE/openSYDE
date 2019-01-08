//-----------------------------------------------------------------------------
/*!
   \internal
   \file
   \brief       Widget with a simple background (implementation)

   This class does not contain any functionality,
   but needs to exist, to have a unique group,
   to apply a specific stylesheet for.

   \implementation
   project     openSYDE
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     13.02.2018  STW/B.Bayer
   \endimplementation
*/
//-----------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------- */
#include "precomp_headers.h"

#include "C_OgeWiOnlyBackground.h"

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

   \created     13.02.2018  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
C_OgeWiOnlyBackground::C_OgeWiOnlyBackground(QWidget * const opc_Parent) :
   C_OgeWiWithToolTip(opc_Parent)
{
   this->SetBackgroundColor(0);
}

//-----------------------------------------------------------------------------
/*!
   \brief   Set background color (stylesheet color index)

   \param[in]     osn_Value New value

   \created     03.09.2018  STW/G.Landsgesell
*/
//-----------------------------------------------------------------------------
void C_OgeWiOnlyBackground::SetBackgroundColor(const stw_types::sintn osn_Value)
{
   stw_opensyde_gui_logic::C_OgeWiUtil::h_ApplyStylesheetProperty(this, "Background", osn_Value);
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
void C_OgeWiOnlyBackground::paintEvent(QPaintEvent * const opc_Event)
{
   stw_opensyde_gui_logic::C_OgeWiUtil::h_DrawBackground(this);

   QWidget::paintEvent(opc_Event);
}
