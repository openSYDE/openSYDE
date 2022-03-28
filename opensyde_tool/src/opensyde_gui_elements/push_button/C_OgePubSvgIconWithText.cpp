//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Button with  text and SVG item, drawn manually (implementation)

   Button with  text and SVG item, drawn manually

   \copyright   Copyright 2018 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.h"

#include "C_Uti.h"
#include "constants.h"
#include "C_OgePubSvgIconWithText.h"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw_opensyde_gui;
using namespace stw_opensyde_gui_logic;
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

   \param[in,out] opc_Parent Optional pointer to parent
*/
//----------------------------------------------------------------------------------------------------------------------
C_OgePubSvgIconWithText::C_OgePubSvgIconWithText(QWidget * const opc_Parent) :
   C_OgePubSvgIconWithTextBase(opc_Parent)
{
   this->SetMargins(20, 10);
   this->msn_LeftBorderEnabledWidth = 0;
   this->mc_TextColorEnabled = mc_STYLE_GUIDE_COLOR_4;
   this->mc_TextColorDisabled = mc_STYLE_GUIDE_COLOR_9;
   this->mc_TextColorDarkMode = mc_STYLE_GUIDE_COLOR_27;
   //lint -e{1938} //we don't create global objects of this class; no race conditions can occur
   this->mc_FontPixel = C_Uti::h_GetFontPixel(mc_STYLE_GUIDE_FONT_SEMIBOLD_13);
   this->mc_BackgroundColorDisabled = Qt::transparent;
   this->mc_BackgroundColorEnabledDefault = Qt::transparent;
   this->mc_BackgroundColorEnabledHover = mc_STYLE_GUIDE_COLOR_11;
   this->mc_BackgroundColorEnabledPressed = mc_STYLE_GUIDE_COLOR_10;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Hide tooltip
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OgePubSvgIconWithText::HideToolTip(void)
{
   this->m_HideToolTip();
}
