//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief      Main of project for windows target

   Instantiate C_SYDEsupWindows and call main functionality.

   \copyright   Copyright 2018 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.h"

#include "stwtypes.h"
#include "stwerrors.h"
#include "C_SYDEsupWindows.h"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw_types;
using namespace stw_errors;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

/* -- Global Variables ---------------------------------------------------------------------------------------------- */

/* -- Module Global Variables --------------------------------------------------------------------------------------- */

/* -- Module Global Function Prototypes ----------------------------------------------------------------------------- */

/* -- Implementation ------------------------------------------------------------------------------------------------ */

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   main

   \param[in]   osn_Argc     number of command line arguments
   \param[in]   oapcn_Argv   command line arguments

   \return
   C_SYDEsup::e_Result::eOK                        success
   C_SYDEsup::e_Result::eERR_PARSE_COMMAND_LINE    error occurred on parsing command line (e.g. missing parameters)
   else                                            see C_SYDEsup::Update for precise error descriptions
*/
//----------------------------------------------------------------------------------------------------------------------
sintn main(const sintn osn_Argc, charn * const oppcn_Argv[])
{
   C_SYDEsup::E_Result e_Return;
   C_SYDEsupWindows c_SYDEServiceUpdater;

   e_Return = c_SYDEServiceUpdater.ParseCommandLine(osn_Argc, oppcn_Argv);


   if (e_Return == C_SYDEsup::eOK)
   {
      e_Return = c_SYDEServiceUpdater.Update();
   }

   return static_cast<sintn>(e_Return);
}
