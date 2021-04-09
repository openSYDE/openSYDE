//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Button for dashboard chart control buttons with background styles in dark and bright.

   \copyright   Copyright 2021 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.h"

#include "C_OgePubIconChart.h"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw_opensyde_gui_elements;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

/* -- Global Variables ---------------------------------------------------------------------------------------------- */

/* -- Module Global Variables --------------------------------------------------------------------------------------- */

/* -- Module Global Function Prototypes ----------------------------------------------------------------------------- */

/* -- Implementation ------------------------------------------------------------------------------------------------ */

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default constructor

   Set up GUI with all elements.

   \param[in,out]  opc_Parent    Optional pointer to parent
*/
//----------------------------------------------------------------------------------------------------------------------
C_OgePubIconChart::C_OgePubIconChart(QWidget * const opc_Parent) :
   C_OgePubSvgIconWithTextBase(opc_Parent)
{
   this->setIconSize(QSize(24, 24));
   this->SetMargins(0, 0);
   this->msn_LeftBorderEnabledWidth = 0;

   // those colors are styled in style sheets
   this->mc_BackgroundColorDisabled = Qt::transparent;
   this->mc_BackgroundColorEnabledDefault = Qt::transparent;
   this->mc_BackgroundColorEnabledHover = Qt::transparent;
   this->mc_BackgroundColorEnabledPressed = Qt::transparent;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Overridden paint event

   Here: draw background as defined via style sheets

   \param[in,out]  opc_Event  Event
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OgePubIconChart::paintEvent(QPaintEvent * const opc_Event)
{
   QPushButton::paintEvent(opc_Event);                 // as we have no text we can call this without problems
   C_OgePubSvgIconWithTextBase::paintEvent(opc_Event); // this does not call original paint event
}
