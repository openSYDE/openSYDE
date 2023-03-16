//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Generic button with text and background color

   Generic button with text and background color.
   This class does not contain any functionality,
   but needs to exist, to have a unique group,
   to apply a specific stylesheet for.

   \copyright   Copyright 2022 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.hpp"

#include "C_OgeWiUtil.hpp"
#include "C_OgePubGenericTextWithBackground.hpp"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
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
C_OgePubGenericTextWithBackground::C_OgePubGenericTextWithBackground(QWidget * const opc_Parent) :
   C_OgePubToolTipBase(opc_Parent)
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set foreground color (stylesheet color index)

   \param[in] os32_Value New value
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OgePubGenericTextWithBackground::SetForegroundColor(const int32_t os32_Value)
{
   C_OgeWiUtil::h_ApplyStylesheetProperty(this, "Foreground", os32_Value);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set background color (stylesheet color index)

   \param[in] os32_Value New value
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OgePubGenericTextWithBackground::SetBackgroundColor(const int32_t os32_Value)
{
   C_OgeWiUtil::h_ApplyStylesheetProperty(this, "Background", os32_Value);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set checked background color (stylesheet color index)

   \param[in] os32_Value New value
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OgePubGenericTextWithBackground::SetCheckedBackgroundColor(const int32_t os32_Value)
{
   C_OgeWiUtil::h_ApplyStylesheetProperty(this, "CheckedBackground", os32_Value);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set font in pixel

   \param[in] os32_Value    New value
   \param[in] oq_SemiBold  Flag if semibold
   \param[in] oq_Bold      Flag if bold
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OgePubGenericTextWithBackground::SetFontPixel(const int32_t os32_Value, const bool oq_SemiBold,
                                                     const bool oq_Bold)
{
   C_OgeWiUtil::h_ApplyStylesheetProperty(this, "Font", os32_Value);
   C_OgeWiUtil::h_ApplyStylesheetProperty(this, "SemiBold", oq_SemiBold);
   C_OgeWiUtil::h_ApplyStylesheetProperty(this, "Bold", oq_Bold);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set border color (stylesheet color index) and activates the border

   \param[in] os32_Value New value
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OgePubGenericTextWithBackground::SetBorderColor(const int32_t os32_Value)
{
   C_OgeWiUtil::h_ApplyStylesheetProperty(this, "Border", os32_Value);
   C_OgeWiUtil::h_ApplyStylesheetProperty(this, "BorderActive", true);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set clicked colors (stylesheet color index) and activates the border

   \param[in] os32_ValueClicked New value for clicked
   \param[in] os32_ValueHovered New value for hovered before clicking
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OgePubGenericTextWithBackground::SetClickedColor(const int32_t os32_ValueClicked,
                                                        const int32_t os32_ValueHovered)
{
   C_OgeWiUtil::h_ApplyStylesheetProperty(this, "Hovered", os32_ValueHovered);
   C_OgeWiUtil::h_ApplyStylesheetProperty(this, "Clicked", os32_ValueClicked);
}
