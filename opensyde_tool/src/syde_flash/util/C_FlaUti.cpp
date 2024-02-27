//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       SydeFlash util class

   SydeFlash util class

   \copyright   Copyright 2024 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.hpp"

#include "C_Uti.hpp"
#include "stwtypes.hpp"
#include "C_FlaUti.hpp"
#include "C_OscUtils.hpp"
#include "fla_constants.hpp"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw::opensyde_gui;
using namespace stw::opensyde_core;
using namespace stw::opensyde_gui_logic;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

/* -- Global Variables ---------------------------------------------------------------------------------------------- */

/* -- Module Global Variables --------------------------------------------------------------------------------------- */

/* -- Module Global Function Prototypes ----------------------------------------------------------------------------- */

/* -- Implementation ------------------------------------------------------------------------------------------------ */

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Check if path contains variables (e.g. %{CAN_MONITOR_PROJECT}) and resolve them.

   \param[in]  orc_Path    path that probably contains variables

   \return
   Resolved path
*/
//----------------------------------------------------------------------------------------------------------------------
QString C_FlaUti::h_ResolvePlaceholderVariables(const QString & orc_Path)
{
   QString c_Return = orc_Path;

   // first check for indicator %
   if (c_Return.contains("%") == true)
   {
      // replace general path variables
      c_Return = C_OscUtils::h_ResolveProjIndependentPlaceholderVariables(c_Return.toStdString().c_str()).c_str();

      // resolve CAN-monitor- and project-specific variables
      if (c_Return.contains(mc_PATH_VARIABLE_SYDE_FLASH_BIN) == true)
      {
         c_Return.replace(mc_PATH_VARIABLE_SYDE_FLASH_BIN, C_Uti::h_GetExePath() + '/');
      }
   }

   return c_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Resolve variables and interpret relative paths as relative to executable

   Resolve all path variables (e.g. %{CAN_MONITOR_PROJ}). If the resulting path is relative
   it is meant as relative to CANMonitor.exe, so we make it absolute by concatenating
   with executable path.

   \param[in]  orc_Path    path that probably contains path variables and if relative is relative to executable

   \return
   Absolute and resolved path
*/
//----------------------------------------------------------------------------------------------------------------------
QString C_FlaUti::h_GetResolvedAbsolutePathFromExe(const QString & orc_Path)
{
   QString c_Return = orc_Path;

   // first resolve, then make absolute (resolving might change from relative to absolute!)
   c_Return = C_FlaUti::h_ResolvePlaceholderVariables(c_Return);
   c_Return = C_Uti::h_GetAbsolutePathFromExe(c_Return);

   return c_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Default constructor
*/
//----------------------------------------------------------------------------------------------------------------------
C_FlaUti::C_FlaUti(void)
{
}
