//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       main function of openSYDE CAN Monitor project

   \copyright   Copyright 2018 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */

#include "precomp_headers.h"

#include <QApplication>

#include "stwtypes.h"
#include "C_Uti.h"
#include "C_GtGetText.h"
#include "C_CamMainWindow.h"
#include "C_PopErrorHandling.h"
#include "C_OSCLoggingHandler.h"
#include "C_CamUtiStyleSheets.h"

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
   // turn on the DPI support**
   QApplication::setAttribute(Qt::AA_EnableHighDpiScaling);

   QApplication c_Appl(osn_Argc, oapcn_Argv);
   {
      QString c_BinaryHash = stw_opensyde_gui_logic::C_Uti::h_GetHashValueAsQString();
      const QString c_FilePath = stw_opensyde_gui_logic::C_Uti::h_GetCompleteLogFileLocation(".syde_cam_log");

      //Set up logging (FIRST)
      stw_opensyde_core::C_OSCLoggingHandler::h_SetWriteToConsoleActive(false);
      stw_opensyde_core::C_OSCLoggingHandler::h_SetWriteToFileActive(true);
      stw_opensyde_core::C_OSCLoggingHandler::h_SetCompleteLogFileLocation(c_FilePath.toStdString().c_str());

      osc_write_log_info("Startup", QString("Starting openSYDE CAN Monitor(MD5-Checksum: " +
                                            c_BinaryHash +
                                            ")").toStdString().c_str());
   }
   {
      //Set stylesheet (SECOND)
      c_Appl.setStyleSheet(stw_opensyde_gui_logic::C_CamUtiStyleSheets::h_GetStylesheet());
   }
   {
      const QString c_TranslationLoacation = stw_opensyde_gui_logic::C_Uti::h_GetExePath() + "/Translations";
      //Set language (THIRD)
      //stw_opensyde_core::C_OSCGetText::h_SetLanguage("en_US");
      stw_opensyde_gui_logic::C_PopErrorHandling::mh_GetTextInitializeErr(
         stw_opensyde_gui_logic::C_GtGetText::h_Initialize(c_TranslationLoacation.toStdString().c_str(), "en_US"));
   }
   {
      //Application (FOURTH)
      stw_opensyde_gui::C_CamMainWindow c_Window;

      c_Window.show();

      //lint -e{1705} Working Qt example code
      return c_Appl.exec();
   }
}
