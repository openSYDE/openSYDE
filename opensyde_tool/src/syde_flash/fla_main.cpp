//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       main function of SYDEflash project

   \copyright   Copyright 2018 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */

#include "precomp_headers.hpp"

#include <QApplication>

#include "stwtypes.hpp"
#include "C_Uti.hpp"
#include "C_GtGetText.hpp"
#include "C_FlaMainWindow.hpp"
#include "C_PopErrorHandling.hpp"
#include "C_OscLoggingHandler.hpp"
#include "C_FlaUtiStyleSheets.hpp"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */

/* -- Module Global Constants --------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

/* -- Global Variables ---------------------------------------------------------------------------------------------- */

/* -- Module Global Variables --------------------------------------------------------------------------------------- */

/* -- Module Global Function Prototypes ----------------------------------------------------------------------------- */

/* -- Implementation ------------------------------------------------------------------------------------------------ */

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   default main

   Set up and start application.

   \param[in]   os32_Argc     Number of command line arguments
   \param[in]   opacn_Argv   Command line arguments

   \return
   0: success
   else: error
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t main(int32_t os32_Argc, char_t * opacn_Argv[])
{
   int32_t s32_Result;

   // turn on the DPI support**
   QApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
   QGuiApplication::setHighDpiScaleFactorRoundingPolicy(Qt::HighDpiScaleFactorRoundingPolicy::PassThrough);

   QApplication c_Appl(os32_Argc, opacn_Argv);
   {
      const QString c_BinaryHash = stw::opensyde_gui_logic::C_Uti::h_GetHashValueAsQtString();
      const QString c_FilePath = stw::opensyde_gui_logic::C_Uti::h_GetCompleteLogFileLocation(".syde_fla_log");

      //Set up logging (FIRST)
      stw::opensyde_core::C_OscLoggingHandler::h_SetWriteToConsoleActive(false);
      stw::opensyde_core::C_OscLoggingHandler::h_SetWriteToFileActive(true);
      stw::opensyde_core::C_OscLoggingHandler::h_SetCompleteLogFileLocation(c_FilePath.toStdString().c_str());

      osc_write_log_info("Startup", static_cast<QString>("Starting SYDEflash tool(MD5-Checksum: " +
                                                         c_BinaryHash + ")").toStdString().c_str());
   }
   {
      //Set stylesheet (SECOND)
      c_Appl.setStyleSheet(stw::opensyde_gui_logic::C_FlaUtiStyleSheets::h_GetStylesheet());
   }
   {
      const QString c_TranslationLocation = stw::opensyde_gui_logic::C_Uti::h_GetExePath() + "/Translations";
      //Set language (THIRD)
      //stw::opensyde_core::C_OscGetText::h_SetLanguage("en_US");
      stw::opensyde_gui_logic::C_PopErrorHandling::h_GetTextInitializeErr(
         stw::opensyde_gui_logic::C_GtGetText::h_Initialize(c_TranslationLocation.toStdString().c_str(), "en_US"));
   }
   {
      //Application (FOURTH)
      stw::opensyde_gui::C_FlaMainWindow c_Window;

      c_Window.show();

      //lint -e{1705} Working Qt example code
      s32_Result = c_Appl.exec();
   }
   return s32_Result;
}
