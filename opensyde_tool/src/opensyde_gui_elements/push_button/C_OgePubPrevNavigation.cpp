//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       System definition lists previous navigation.

    Icon on the right hand side of text.

   \copyright   Copyright 2020 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.hpp"

#include "C_Uti.hpp"
#include "constants.hpp"
#include "C_OgePubPrevNavigation.hpp"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw::opensyde_gui;
using namespace stw::opensyde_gui_logic;
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
C_OgePubPrevNavigation::C_OgePubPrevNavigation(QWidget * const opc_Parent) :
   C_OgePubSvgIconWithTextBase(opc_Parent)
{
   this->SetMargins(1, 6);
   this->ms32_LeftBorderEnabledWidth = 0;
   this->mc_TextColorEnabled = mc_STYLE_GUIDE_COLOR_5;
   this->mc_TextColorDisabled = mc_STYLE_GUIDE_COLOR_9;
   this->mc_TextColorDarkMode = mc_STYLE_GUIDE_COLOR_5;
   //lint -e{1938} //we don't create global objects of this class; no race conditions can occur
   this->mc_FontPixel = C_Uti::h_GetFontPixel(mc_STYLE_GUIDE_FONT_SEMIBOLD_14);
   this->mc_BackgroundColorDisabled = Qt::transparent;
   this->mc_BackgroundColorEnabledDefault = Qt::transparent;
   this->mc_BackgroundColorEnabledHover = mc_STYLE_GUIDE_COLOR_11;
   this->mc_BackgroundColorEnabledPressed = mc_STYLE_GUIDE_COLOR_10;
   this->mq_IconLeft = false;
}
