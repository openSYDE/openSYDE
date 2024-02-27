//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Button with menu for selecting path variables

   Button with menu for selecting path variables. Styled to fit seamless between
   path line edit and browse button.

   \copyright   Copyright 2019 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.hpp"

#include "constants.hpp"
#include "cam_constants.hpp"
#include "C_GtGetText.hpp"
#include "C_CamOgePubPathVariables.hpp"
#include "C_OscUtils.hpp"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw::opensyde_gui_elements;
using namespace stw::opensyde_gui_logic;
using namespace stw::opensyde_gui;
using namespace stw::opensyde_core;

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
C_CamOgePubPathVariables::C_CamOgePubPathVariables(QWidget * const opc_Parent) :
   C_CamOgePubPathVariablesBase(opc_Parent)
{
   // menu
   this->m_AddHeading(C_GtGetText::h_GetText("CAN Monitor"));
   //lint -save -e1938
   //  we don't create global objects of this class; no race conditions can occur and
   //  static const is guaranteed preinitialized before main
   this->m_AddEntry(C_GtGetText::h_GetText("CAN Monitor Binary"), mc_PATH_VARIABLE_CAN_MONITOR_BIN);
   this->m_AddEntry(C_GtGetText::h_GetText("CAN Monitor Project"), mc_PATH_VARIABLE_CAN_MONITOR_PROJ);
   this->m_AddHeading(C_GtGetText::h_GetText("System"));
   this->m_AddEntry(C_GtGetText::h_GetText("User Name"), C_OscUtils::hc_PATH_VARIABLE_USER_NAME.c_str());
   this->m_AddEntry(C_GtGetText::h_GetText("Computer Name"), C_OscUtils::hc_PATH_VARIABLE_COMPUTER_NAME.c_str());
   //lint -restore
   this->m_SetMenuMinWidth(140);
}
