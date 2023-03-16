//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Main of project for linux target

   Instantiate C_SYDEsupLinux and call main functionality.

   \copyright   Copyright 2018 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.hpp"

#include "stwtypes.hpp"
#include "stwerrors.hpp"
#include "C_SydeSupLinux.hpp"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw::errors;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

/* -- Global Variables ---------------------------------------------------------------------------------------------- */

/* -- Module Global Variables --------------------------------------------------------------------------------------- */

/* -- Module Global Function Prototypes ----------------------------------------------------------------------------- */

/* -- Implementation ------------------------------------------------------------------------------------------------ */

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   main

   \param[in]   os32_Argc    number of command line arguments
   \param[in]   oapcn_Argv   command line arguments

   \return
   C_SydeSup::e_Result::eOK                        success
   C_SydeSup::e_Result::eERR_PARSE_COMMAND_LINE    error occurred on parsing command line (e.g. missing parameters)
   else                                            see C_SydeSup::SelectModeAndStart for precise error descriptions
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t main(const int32_t os32_Argc, char_t * const oppcn_Argv[])
{
   C_SydeSup::E_Result e_Return;
   C_SydeSupLinux c_SydeServiceUpdater;

   e_Return = c_SydeServiceUpdater.ParseCommandLine(os32_Argc, oppcn_Argv);

   if (e_Return == C_SydeSup::eOK)
   {
      e_Return = c_SydeServiceUpdater.SelectModeAndStart();
   }

   return static_cast<int32_t>(e_Return);
}
