//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       SydeFlash variant of path variables push button widget

   SydeFlash variant of path variables push button widget

   \copyright   Copyright 2024 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.hpp"

#include "stwtypes.hpp"
#include "C_OscUtils.hpp"
#include "C_GtGetText.hpp"
#include "fla_constants.hpp"
#include "C_FlaOgePubPathVariables.hpp"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw::opensyde_gui;
using namespace stw::opensyde_core;
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

   \param[in,out]  opc_Parent    Optional pointer to parent
*/
//----------------------------------------------------------------------------------------------------------------------
C_FlaOgePubPathVariables::C_FlaOgePubPathVariables(QWidget * const opc_Parent) :
   C_CamOgePubPathVariablesBase(opc_Parent)
{
   // menu
   this->m_AddHeading(C_GtGetText::h_GetText("SYDEflash"));
   //lint -save -e1938
   //  we don't create global objects of this class; no race conditions can occur and
   //  static const is guaranteed preinitialized before main
   this->m_AddEntry(C_GtGetText::h_GetText("SYDEflash Binary"), mc_PATH_VARIABLE_SYDE_FLASH_BIN);
   this->m_AddHeading(C_GtGetText::h_GetText("System"));
   this->m_AddEntry(C_GtGetText::h_GetText("User Name"), C_OscUtils::hc_PATH_VARIABLE_USER_NAME.c_str());
   this->m_AddEntry(C_GtGetText::h_GetText("Computer Name"), C_OscUtils::hc_PATH_VARIABLE_COMPUTER_NAME.c_str());
   //lint -restore
   this->m_SetMenuMinWidth(140);
}
