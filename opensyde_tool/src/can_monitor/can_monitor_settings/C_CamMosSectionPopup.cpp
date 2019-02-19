//-----------------------------------------------------------------------------
/*!
   \internal
   \file
   \brief       Popup widget for displaying settings widgets in collapsed state (implementation)

   Popup widget for displaying settings widgets in collapsed state.

   \implementation
   project     openSYDE
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     11.01.2019  STW/G.Landsgesell
   \endimplementation
*/
//-----------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------- */
#include "precomp_headers.h"

#include <QGraphicsDropShadowEffect>

#include "C_CamMosSectionPopup.h"
#include "constants.h"

/* -- Used Namespaces ------------------------------------------------------ */
using namespace stw_opensyde_gui;

/* -- Module Global Constants ---------------------------------------------- */

/* -- Types ---------------------------------------------------------------- */

/* -- Global Variables ----------------------------------------------------- */

/* -- Module Global Variables ---------------------------------------------- */

/* -- Module Global Function Prototypes ------------------------------------ */

/* -- Implementation ------------------------------------------------------- */

//-----------------------------------------------------------------------------
/*!
   \brief   Default constructor.

   \param[in]  Optional pointer to parent.

   \created     11.01.2019  STW/G.Landsgesell
*/
//-----------------------------------------------------------------------------
C_CamMosSectionPopup::C_CamMosSectionPopup(QWidget * const opc_Parent) :
   C_OgeWiOnlyBackground(opc_Parent),
   mpc_Layout(new QVBoxLayout(this))
{
   this->mpc_Layout->setMargin(1); // to make border visible
   this->setLayout(mpc_Layout);

   //shadow (adopted from context menu)
   {
      QColor c_Color;
      QGraphicsDropShadowEffect * pc_Shadow;

      pc_Shadow = new QGraphicsDropShadowEffect(this);
      pc_Shadow->setBlurRadius(4.0);
      pc_Shadow->setOffset(4.0);
      c_Color = mc_STYLE_GUIDE_COLOR_31;
      c_Color.setAlpha(50);
      pc_Shadow->setColor(c_Color);
      this->setGraphicsEffect(pc_Shadow);
      //lint -e{429}  no memory leak because of the parent of pc_Shadow and the Qt memory management
   }
}

//-----------------------------------------------------------------------------
/*!
   \brief   Default destructor.

   \created     11.01.2019  STW/G.Landsgesell
*/
//-----------------------------------------------------------------------------
C_CamMosSectionPopup::~C_CamMosSectionPopup(void)
{
   delete this->mpc_Layout;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Show widget on right position.

   \param[in]     orc_Pos           global position of top left point of button

   \created     15.01.2019  STW/G.Landsgesell
*/
//-----------------------------------------------------------------------------
void C_CamMosSectionPopup::Show(const QPoint & orc_Pos)
{
   this->setVisible(true);
   this->DoMove(orc_Pos);
}

//-----------------------------------------------------------------------------
/*!
   \brief   Insert specific settings widget.

   \created     25.01.2019  STW/G.Landsgesell
*/
//-----------------------------------------------------------------------------
void C_CamMosSectionPopup::SetWidget(QWidget * const opc_Widget)
{
   if (opc_Widget != NULL)
   {
      this->mpc_Layout->addWidget(opc_Widget);
   }
}

//-----------------------------------------------------------------------------
/*!
   \brief   Move popup to align to button.

   \param[in]     orc_Pos           global position

   \created     25.01.2019  STW/G.Landsgesell
*/
//-----------------------------------------------------------------------------
void C_CamMosSectionPopup::DoMove(const QPoint & orc_Pos)
{
   QSize c_Size = this->sizeHint();
   QPoint c_Pos = orc_Pos + QPoint(-c_Size.width(), -41); // to align to button

   // use map from global of parent of button
   c_Pos = this->parentWidget()->parentWidget()->parentWidget()->mapFromGlobal(c_Pos);
   this->setGeometry(QRect(c_Pos, c_Size));
}
