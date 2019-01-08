//-----------------------------------------------------------------------------
/*!
   \internal
   \file
   \brief       Push button for main page project operations (implementation)

   Push button for main page project operations:
   New Project, Open Project, Save As

   This class does not contain any functionality,
   but needs to exist, to have a unique group,
   to apply a specific stylesheet for.

   \implementation
   project     openSYDE
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     26.07.2018  STW/G.Scupin
   \endimplementation
*/
//-----------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------- */
#include "precomp_headers.h"

#include <QPainter>

#include "C_OgePubProjAction.h"
#include "constants.h"

/* -- Used Namespaces ------------------------------------------------------ */
using namespace stw_opensyde_gui_elements;
using namespace stw_opensyde_gui;

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

   \created     26.07.2018  STW/G.Scupin
*/
//-----------------------------------------------------------------------------
C_OgePubProjAction::C_OgePubProjAction(QWidget * const opc_Parent) :
   C_OgePubToolTipBase(opc_Parent)
{
   this->setIconSize(mc_ICON_SIZE_24);
}

//-----------------------------------------------------------------------------
/*!
   \brief   Overwritten paint event slot

   Here: Draw icon

   \param[in,out] opc_Event Event identification and information

   \created     26.07.2018  STW/G.Scupin
*/
//-----------------------------------------------------------------------------
void C_OgePubProjAction::paintEvent(QPaintEvent * const opc_Event)
{
   QPushButton::paintEvent(opc_Event);

   // draw the icon
   if (this->isEnabled() == true)
   {
      QPainter c_Painter(this);
      QPixmap c_PixmapEnabled;
      c_PixmapEnabled = this->mc_IconEnabled.pixmap(this->iconSize());
      //Calculate icon top left based on fixed center
      c_Painter.drawPixmap(26 - (this->iconSize().width() / 2), 21 - (this->iconSize().height() / 2), c_PixmapEnabled);
   }
   else
   {
      QPainter c_Painter(this);
      QPixmap c_PixmapDisabled;
      c_PixmapDisabled = this->mc_IconDisabled.pixmap(this->iconSize());
      //Calculate icon top left based on fixed center
      c_Painter.drawPixmap(26 - (this->iconSize().width() / 2), 21 - (this->iconSize().height() / 2), c_PixmapDisabled);
   }
}

//-----------------------------------------------------------------------------
/*!
   \brief   Setting the Icon for Enabled and Disabled mode

   \param[in]     oc_IconPathEnabled      String for Enabled Icon Path
                  oc_IconPathDisabled     String for Disabled Icon Path

   \created     26.07.2018  STW/G.Scupin
*/
//-----------------------------------------------------------------------------
void C_OgePubProjAction::SetCustomIcon(QString oc_IconPathEnabled, QString oc_IconPathDisabled)
{
   mc_IconEnabled.addPixmap(QPixmap(oc_IconPathEnabled));
   mc_IconDisabled.addPixmap(QPixmap(oc_IconPathDisabled));
}
