//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       main function of project

   \copyright   Copyright 2016 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */

#include "precomp_headers.h"

#include <QString>
#include <QApplication>
#include "C_Uti.h"
#include "C_UtiStyleSheets.h"
#include "C_NagMainWindow.h"
#include "C_PuiProject.h"
#include "C_GtGetText.h"
#include "C_OSCLoggingHandler.h"
#include "C_PopErrorHandling.h"
#include "stwtypes.h"
#include "stwerrors.h"
#include "C_SdUtil.h"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw_types;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

/* -- Global Variables ---------------------------------------------------------------------------------------------- */

/* -- Module Global Variables --------------------------------------------------------------------------------------- */

/* -- Module Global Function Prototypes ----------------------------------------------------------------------------- */

/* -- Implementation ------------------------------------------------------------------------------------------------ */

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   default main

   Set up and start application.

   \param[in]   osn_Argc     Number of command line arguments
   \param[in]   oapcn_Argv   Command line arguments

   \return
   0: success
   else: error
*/
//----------------------------------------------------------------------------------------------------------------------
sintn main(sintn osn_Argc, charn * oapcn_Argv[])
{
   const stw_types::uint16 u16_Timer = osc_write_log_performance_start();

   // turn on the DPI support**
   QApplication::setAttribute(Qt::AA_EnableHighDpiScaling);

   QApplication c_Appl(osn_Argc, oapcn_Argv);
   {
      const QString c_FilePath = stw_opensyde_gui_logic::C_Uti::h_GetCompleteLogFileLocation(".syde_log");
      const QString c_ExeHash = stw_opensyde_gui_logic::C_Uti::h_GetHashValueAsQString();

      //Set up logging (FIRST)
      stw_opensyde_core::C_OSCLoggingHandler::h_SetWriteToConsoleActive(false);
      stw_opensyde_core::C_OSCLoggingHandler::h_SetWriteToFileActive(true);
      stw_opensyde_core::C_OSCLoggingHandler::h_SetCompleteLogFileLocation(c_FilePath.toStdString().c_str());

      osc_write_log_info("Startup", static_cast<QString>("Starting openSYDE Version: " +
                                                         stw_opensyde_gui_logic::C_Uti::h_GetApplicationVersion() +
                                                         ", MD5-Checksum: " +
                                                         c_ExeHash).toStdString().c_str());
   }
   {
      //Set stylesheet (SECOND)
      c_Appl.setStyleSheet(stw_opensyde_gui_logic::C_UtiStyleSheets::h_GetStylesheet());
   }
   {
      const QString c_TranslationLocation = stw_opensyde_gui_logic::C_Uti::h_GetExePath() + "/Translations";
      //Set language (THIRD)
      //stw_opensyde_core::C_OSCGetText::h_SetLanguage("en_US");
      stw_opensyde_gui_logic::C_PopErrorHandling::h_GetTextInitializeErr(
         stw_opensyde_gui_logic::C_GtGetText::h_Initialize(c_TranslationLocation.toStdString().c_str(), "en_US"));
   }
   {
      //Application (FOURTH)
      stw_opensyde_gui::C_NagMainWindow c_Window(u16_Timer);

      c_Window.show();

      //lint -e{1705} Working Qt example code
      return c_Appl.exec();
   }
}
