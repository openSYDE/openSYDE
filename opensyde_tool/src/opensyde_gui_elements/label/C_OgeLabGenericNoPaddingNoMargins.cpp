//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Generic label styled via setters (implementation)

   Generic label styled via setters

   \copyright   Copyright 2018 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.hpp"

#include "C_OgeWiUtil.hpp"
#include "C_OgeLabGenericNoPaddingNoMargins.hpp"

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
C_OgeLabGenericNoPaddingNoMargins::C_OgeLabGenericNoPaddingNoMargins(QWidget * const opc_Parent) :
   C_OgeLabToolTipBase(opc_Parent)
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set foreground color (stylesheet color index)

   \param[in] os32_Value New value
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OgeLabGenericNoPaddingNoMargins::SetForegroundColor(const int32_t os32_Value)
{
   C_OgeWiUtil::h_ApplyStylesheetProperty(this, "Foreground", os32_Value);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set background color (stylesheet color index)

   \param[in] os32_Value New value
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OgeLabGenericNoPaddingNoMargins::SetBackgroundColor(const int32_t os32_Value)
{
   C_OgeWiUtil::h_ApplyStylesheetProperty(this, "Background", os32_Value);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set font in pixel

   \param[in] os32_Value   New value
   \param[in] oq_SemiBold  Flag if semibold
   \param[in] oq_Bold      Flag if bold
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OgeLabGenericNoPaddingNoMargins::SetFontPixel(const int32_t os32_Value, const bool oq_SemiBold,
                                                     const bool oq_Bold)
{
   C_OgeWiUtil::h_ApplyStylesheetProperty(this, "Font", os32_Value);
   C_OgeWiUtil::h_ApplyStylesheetProperty(this, "SemiBold", oq_SemiBold);
   C_OgeWiUtil::h_ApplyStylesheetProperty(this, "Bold", oq_Bold);
}
