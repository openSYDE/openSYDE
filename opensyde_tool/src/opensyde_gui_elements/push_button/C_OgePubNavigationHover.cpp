//-----------------------------------------------------------------------------
/*!
   \internal
   \file
   \brief       Transparent push button with hover state (implementation)

   Transparent push button with hover state

   \implementation
   project     openSYDE
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     13.08.2018  STW/M.Echtler
   \endimplementation
*/
//-----------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------- */
#include "precomp_headers.h"

#include "C_OgeWiUtil.h"
#include "C_OgePubNavigationHover.h"

/* -- Used Namespaces ------------------------------------------------------ */
using namespace stw_opensyde_gui_logic;
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

   \created     13.08.2018  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
C_OgePubNavigationHover::C_OgePubNavigationHover(QWidget * const opc_Parent) :
   C_OgePubToolTipBase(opc_Parent)
{
   C_OgeWiUtil::h_ApplyStylesheetProperty(this, "Active", false);
}

//-----------------------------------------------------------------------------
/*!
   \brief   Set item active state

   \param[in] oq_Active Item active state

   \created     13.08.2018  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_OgePubNavigationHover::SetActive(const bool oq_Active)
{
   C_OgeWiUtil::h_ApplyStylesheetProperty(this, "Active", oq_Active);
}
