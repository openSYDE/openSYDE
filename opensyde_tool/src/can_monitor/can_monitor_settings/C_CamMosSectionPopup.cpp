//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Popup widget for displaying settings widgets in collapsed state (implementation)

   Popup widget for displaying settings widgets in collapsed state.

   \copyright   Copyright 2019 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.hpp"

#include <QGraphicsDropShadowEffect>

#include "C_CamMosSectionPopup.hpp"
#include "constants.hpp"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw::opensyde_gui;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

/* -- Global Variables ---------------------------------------------------------------------------------------------- */

/* -- Module Global Variables --------------------------------------------------------------------------------------- */

/* -- Module Global Function Prototypes ----------------------------------------------------------------------------- */

/* -- Implementation ------------------------------------------------------------------------------------------------ */

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default constructor.

   \param[in]  opc_Parent  Optional pointer to parent.
*/
//----------------------------------------------------------------------------------------------------------------------
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
   } //lint !e429  no memory leak because of the parent of pc_Shadow and the Qt memory management
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default destructor.
*/
//----------------------------------------------------------------------------------------------------------------------
C_CamMosSectionPopup::~C_CamMosSectionPopup(void)
{
   delete this->mpc_Layout;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Show widget on right position.

   \param[in]     orc_Pos           global position of top left point of button
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CamMosSectionPopup::Show(const QPoint & orc_Pos)
{
   this->setVisible(true);
   this->DoMove(orc_Pos);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Insert specific settings widget.

   \param[in]     opc_Widget  Widget to insert.
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CamMosSectionPopup::SetWidget(QWidget * const opc_Widget)
{
   if (opc_Widget != NULL)
   {
      this->mpc_Layout->addWidget(opc_Widget);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Move popup to align to button.

   \param[in]     orc_Pos           global position
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CamMosSectionPopup::DoMove(const QPoint & orc_Pos)
{
   const QSize c_Size = this->sizeHint();
   const QPoint c_Pos = orc_Pos + QPoint(-c_Size.width(), 15); // to align to button

   this->setGeometry(QRect(c_Pos, c_Size));
}
