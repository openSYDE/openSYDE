//-----------------------------------------------------------------------------
/*!
   \internal
   \file
   \brief       main function of openSYDE CAN Monitor project

   \implementation
   project     openSYDE
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     31.08.2018  STW/M.Echtler
   \endimplementation
*/
//-----------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------- */

#include "precomp_headers.h"

#include <QApplication>

#include "stwtypes.h"
#include "C_Uti.h"
#include "C_GtGetText.h"
#include "C_CamMainWindow.h"
#include "C_PopErrorHandling.h"
#include "C_OSCLoggingHandler.h"
#include "C_CamUtiStyleSheets.h"

/* -- Used Namespaces ------------------------------------------------------ */
using namespace stw_types;

/* -- Module Global Constants ---------------------------------------------- */

/* -- Types ---------------------------------------------------------------- */

/* -- Global Variables ----------------------------------------------------- */

/* -- Module Global Variables ---------------------------------------------- */

/* -- Module Global Function Prototypes ------------------------------------ */

/* -- Implementation ------------------------------------------------------- */

//-----------------------------------------------------------------------------
/*!
   \brief   default main

   Set up and start application.

   \return
   0: success
   else: error

   \created     31.08.2018  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
sintn main(sintn osn_Argc, charn * opacn_Argv[])
{
   // turn on the DPI support**
   QApplication::setAttribute(Qt::AA_EnableHighDpiScaling);

   QApplication c_Appl(osn_Argc, opacn_Argv);
   {
      const QString c_FilePath = stw_opensyde_gui_logic::C_Uti::h_GetCompleteLogFileLocation(".syde_cam_log");
      //Set up logging (FIRST)
      stw_opensyde_core::C_OSCLoggingHandler::h_SetWriteToConsoleActive(false);
      stw_opensyde_core::C_OSCLoggingHandler::h_SetWriteToFileActive(true);
      stw_opensyde_core::C_OSCLoggingHandler::h_SetCompleteLogFileLocation(c_FilePath.toStdString().c_str());
      osc_write_log_info("Startup", "Starting openSYDE CAN Monitor");
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
