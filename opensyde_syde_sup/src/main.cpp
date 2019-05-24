//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       main of project

   Instantiate C_SYDEsup and call main functionality.

   \copyright   Copyright 2018 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.h"
#include <iostream>

#include "stwtypes.h"
#include "stwerrors.h"
#include "C_SYDEsup.h"

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

sintn main(const sintn osn_Argc, charn * const oapcn_Argv[])
{
   C_SYDEsup::E_Result e_Return;
   C_SYDEsup c_SYDEServiceUpdater;

   // ascii art
   std::cout <<
      " ________       ___    ___ ________  _______   ________  ___  ___  ________\n"
      "|\\   ____\\     |\\  \\  /  /|\\   ___ \\|\\  ___ \\ |\\   ____\\|\\  \\|\\  \\|\\   __  \\\n"
      "\\ \\  \\___|_    \\ \\  \\/  / | \\  \\_|\\ \\ \\   __/|\\ \\  \\___|\\ \\  \\\\\\  \\ \\  \\|\\  \\\n"
      " \\ \\_____  \\    \\ \\    / / \\ \\  \\ \\\\ \\ \\  \\_|/_\\ \\_____  \\ \\  \\\\\\  \\ \\   ____\\\n"
      "  \\|____|\\  \\    \\/  /  /   \\ \\  \\_\\\\ \\ \\  \\_|\\ \\|____|\\  \\ \\  \\\\\\  \\ \\  \\___|\n"
      "    ____\\_\\  \\ __/  / /      \\ \\_______\\ \\_______\\____\\_\\  \\ \\_______\\ \\__\\\n"
      "   |\\_________\\\\___/ /        \\|_______|\\|_______|\\_________\\|_______|\\|__|\n"
      "   \\|_________\\|___|/                            \\|_________|\n" << &std::endl;

   e_Return = c_SYDEServiceUpdater.ParseCommandLine(osn_Argc, oapcn_Argv);

   if (e_Return == C_SYDEsup::eOK)
   {
      e_Return = c_SYDEServiceUpdater.Update();
   }

   return static_cast<sintn>(e_Return);
}
