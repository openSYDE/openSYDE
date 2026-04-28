//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Button with icon only that can be used with a menu (arrow appears) and without. Does not support text.

   \copyright   Copyright 2026 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.hpp"

#include "C_OgePubIconMenu.hpp"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw::opensyde_gui_elements;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

/* -- Global Variables ---------------------------------------------------------------------------------------------- */

/* -- Module Global Variables --------------------------------------------------------------------------------------- */

/* -- Module Global Function Prototypes ----------------------------------------------------------------------------- */

/* -- Implementation ------------------------------------------------------------------------------------------------ */

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default constructor

   Set up GUI with all elements.

   \param[in,out] opc_Parent Optional pointer to parent
*/
//----------------------------------------------------------------------------------------------------------------------
C_OgePubIconMenu::C_OgePubIconMenu(QWidget * const opc_Parent) :
   C_OgePubSvgIconWithTextBase(opc_Parent)
{
   this->setIconSize(QSize(24, 24));
   this->mc_BackgroundColorDisabled = Qt::transparent;
   this->mc_BackgroundColorEnabledDefault = Qt::transparent;
   this->mc_BackgroundColorEnabledHover = Qt::transparent;
   this->mc_BackgroundColorEnabledPressed = Qt::transparent;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Overridden paint event

   Here: draw arrow as defined via style sheets

   \param[in,out]  opc_Event  Event
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OgePubIconMenu::paintEvent(QPaintEvent * const opc_Event)
{
   QPushButton::paintEvent(opc_Event);                 // as we have no text we can call this without problems
   C_OgePubSvgIconWithTextBase::paintEvent(opc_Event); // this does not call original paint event
}
