//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Label with custom context menu (implementation)

   Label with custom context menu in openSYDE style.
   This is only needed for labels with text interaction flag
   Qt::TextSelectableByMouse.

   Generic label backgroundcolor styled via setter

   \copyright   Copyright 2023 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */

#include "precomp_headers.hpp"

#include "C_OgeLabGenericWithContextMenu.hpp"

#include "C_OgeWiUtil.hpp"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */

using namespace stw::opensyde_gui_elements;
using namespace stw::opensyde_gui_logic;

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
C_OgeLabGenericWithContextMenu::C_OgeLabGenericWithContextMenu(QWidget * const opc_Parent) :
   C_OgeLabContextMenuBase(opc_Parent)
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set background color (stylesheet color index)

   \param[in] os32_Value New value
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OgeLabGenericWithContextMenu::SetBackgroundColor(const int32_t os32_Value)
{
   C_OgeWiUtil::h_ApplyStylesheetProperty(this, "Background", os32_Value);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set foreground color (stylesheet color index)

   \param[in] os32_Value New value
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OgeLabGenericWithContextMenu::SetForegroundColor(const int32_t os32_Value)
{
   C_OgeWiUtil::h_ApplyStylesheetProperty(this, "Foreground", os32_Value);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set Margins and Padding (stylesheet Label index)

   \param[in] os32_Value New value
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OgeLabGenericWithContextMenu::SetMarginsPadding(const int32_t os32_Value)
{
   C_OgeWiUtil::h_ApplyStylesheetProperty(this, "Padding", os32_Value);
}
