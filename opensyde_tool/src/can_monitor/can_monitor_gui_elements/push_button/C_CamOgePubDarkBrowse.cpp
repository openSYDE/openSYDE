//-----------------------------------------------------------------------------
/*!
   \internal
   \file
   \brief       Push button with left border styled for seamless transition to browse-line-edit. (implementation)

   Push button with left border styled for seamless transition to browse-line-edit.
   This class does not contain any functionality,
   but needs to exist, to have a unique group,
   to apply a specific stylesheet for.

   \implementation
   project     openSYDE
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     16.01.2019  STW/G.Landsgesell
   \endimplementation
*/
//-----------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------- */
#include "precomp_headers.h"

#include "C_CamOgePubDarkBrowse.h"

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

   \created     16.01.2019  STW/G.Landsgesell
*/
//-----------------------------------------------------------------------------
C_CamOgePubDarkBrowse::C_CamOgePubDarkBrowse(QWidget * const opc_Parent) :
   C_OgePubToolTipBase(opc_Parent)
{
   QIcon c_Icon = QIcon("://images/IconBrowseBright.svg").pixmap(16, 16);

   c_Icon.addPixmap(QIcon("://images/IconBrowseBrightDisabled.svg").pixmap(16, 16), QIcon::Disabled);
   this->setIcon(c_Icon);
}
