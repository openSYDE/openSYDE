//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Push button style for system commisioning edit bar

   Push button style for system commisioning edit bar.
   This class does not contain any functionality,
   but needs to exist, to have a unique group,
   to apply a specific stylesheet for.

   \copyright   Copyright 2019 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.hpp"

#include "C_Uti.hpp"
#include "constants.hpp"
#include "C_OgePubSystemCommissioningEdit.hpp"

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
C_OgePubSystemCommissioningEdit::C_OgePubSystemCommissioningEdit(QWidget * const opc_Parent) :
   C_OgePubSvgIconWithTextBase(opc_Parent)
{
   this->SetMargins(10, 20);
   this->ms32_LeftBorderEnabledWidth = 1;
   this->ms32_RightBorderDisabledWidth = 1;
   this->ms32_LeftBorderDisabledWidth = 1;
   this->ms32_TopBorderDisabledWidth = 1;
   this->ms32_BottomBorderDisabledWidth = 1;
   this->mc_BorderColorEnabledLeft = mc_STYLE_GUIDE_COLOR_0;
   this->mc_BorderColorDisabledLeft = mc_STYLE_GUIDE_COLOR_10;
   this->mc_BorderColorDisabledBottom = mc_STYLE_GUIDE_COLOR_10;
   this->mc_BorderColorDisabledRight = mc_STYLE_GUIDE_COLOR_10;
   this->mc_BorderColorDisabledTop = mc_STYLE_GUIDE_COLOR_10;
   this->mc_TextColorEnabled = mc_STYLE_GUIDE_COLOR_0;
   this->mc_TextColorDisabled = mc_STYLE_GUIDE_COLOR_10;
   //lint -e{1938} //we don't create global objects of this class; no race conditions can occur
   this->mc_FontPixel = C_Uti::h_GetFontPixel(mc_STYLE_GUIDE_FONT_SEMIBOLD_14);
   this->mc_BackgroundColorDisabled = mc_STYLE_GUIDE_COLOR_11;
   this->mc_BackgroundColorEnabledDefault = mc_STYLE_GUIDE_COLOR_3001;
   this->mc_BackgroundColorEnabledHover = mc_STYLE_GUIDE_COLOR_3;
   this->mc_BackgroundColorEnabledPressed = mc_STYLE_GUIDE_COLOR_2;
   this->setIconSize(QSize(20, 20));
}
