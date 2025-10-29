//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Central handling for errors (implementation)

   Central handling for errors

   \copyright   Copyright 2016 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.hpp"

#include <QEventLoop>
#include "stwerrors.hpp"
#include "constants.hpp"
#include "C_GtGetText.hpp"
#include "C_OgeWiError.hpp"
#include "C_Uti.hpp"
#include "C_OgePopUpDialog.hpp"
#include "C_PopErrorHandling.hpp"
#include "C_OscLoggingHandler.hpp"
#include "C_OgeWiCustomMessage.hpp"
#include "C_OscSystemDefinitionFiler.hpp"

#include <unordered_set>

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */

using namespace stw::errors;
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
/*! \brief   Load with error handling

   \param[in]  ors32_Err                        Load error
   \param[in]  orc_Path                         path of project to load
   \param[in]  opc_Parent                       parent widget
   \param[in]  ou16_SystemDefinitionVersion     System definition version
   \param[in]  orc_ErrorDetailsMissingDevices   if C_OVERFLOW contains types of all missing devices
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PopErrorHandling::h_ProjectLoadErr(const int32_t & ors32_Err, const QString & orc_Path,
                                          QWidget * const opc_Parent, const uint16_t ou16_SystemDefinitionVersion,
                                          const std::vector<stw::scl::C_SclString> & orc_ErrorDetailsMissingDevices)
{
   if (ors32_Err == C_WARN)
   {
      // Do nothing here. Special case when a user cancels the password dialog for opening a service project.
      // Project file version is not known (because encrypted zip file), hence we don't want to hit the else
      // statement below.
   }
   else if ((ors32_Err != C_NO_ERR) && (ors32_Err != C_WARN))
   {
      std::unordered_set<std::string> c_UniqueDeviceNames;
      QString c_Details;
      QString c_Description;
      C_OgeWiCustomMessage c_Message(opc_Parent, C_OgeWiCustomMessage::eERROR);
      c_Message.SetHeading(C_GtGetText::h_GetText("Project load"));
      c_Description = C_GtGetText::h_GetText("Failed to load project: ") + orc_Path;
      c_Message.SetCustomMinHeight(250, 425);

      switch (ors32_Err)
      {
      case C_RD_WR:
         c_Details = C_GtGetText::h_GetText("There are problems accessing the file system.<br/>"
                                            "For example, there may be no read access to the file.");
         break;
      case C_RANGE:
         c_Details = C_GtGetText::h_GetText("At least one project file is missing.");
         break;
      case C_NOACT:
         c_Details = C_GtGetText::h_GetText("A project file is present but its structure is invalid.<br/>"
                                            "For example this can be caused by an invalid XML file.");
         break;
      case C_CONFIG:
         c_Details = C_GtGetText::h_GetText("The content of a project file is invalid or incomplete.");
         break;
      case C_CHECKSUM:
         c_Details = C_GtGetText::h_GetText("UI part does not match core part.");
         break;
      case C_COM:
         c_Details = C_GtGetText::h_GetText("List of not found nodes in the toolbox:");
         for (const auto & rc_Device : orc_ErrorDetailsMissingDevices)
         {
            if (c_UniqueDeviceNames.emplace(rc_Device.c_str()).second)
            {
               c_Details += C_GtGetText::h_GetText("<br/> - ");
               c_Details += rc_Device.c_str();
            }
         }
         c_Description += C_GtGetText::h_GetText(
            "<br/><br/>The project contains nodes which are not part of the toolbox.");
         c_Details += "<br/><br/>Possible reasons: <br/>";
         c_Details +=
            " - User nodes are used in the project. If yes: Add these missing user nodes in the toolbox and retry. <br/>"
            " - Project was created with newer version. If yes: Use a newer version of openSYDE where these nodes are supported. <br/>";
         break;
      case C_BUSY:
         c_Details = C_GtGetText::h_GetText("Problems with cleaning up temporary folders.");
         break;
      default:
         c_Details = C_GtGetText::h_GetText("Unknown cause.");
         break;
      }

      //Update log file
      C_OscLoggingHandler::h_Flush();

      // Show error message
      c_Details += C_GtGetText::h_GetText("<br/>For more information see ");
      c_Details += C_Uti::h_GetLink(C_GtGetText::h_GetText("log file"), mc_STYLE_GUIDE_COLOR_LINK,
                                    C_OscLoggingHandler::h_GetCompleteLogFileLocation().c_str());
      c_Details += ".";
      c_Message.SetDescription(c_Description);
      c_Message.SetDetails(c_Details);
      c_Message.Execute();
   }
   else
   {
      if ((ou16_SystemDefinitionVersion < C_OscSystemDefinitionFiler::hu16_FILE_VERSION_LATEST) &&
          (orc_Path.isEmpty() == false))
      {
         //Handle version update
         C_OgeWiCustomMessage c_Message(opc_Parent, C_OgeWiCustomMessage::eINFORMATION);
         c_Message.SetHeading(C_GtGetText::h_GetText("Project load"));
         c_Message.SetDescription(C_GtGetText::h_GetText("Loading an older project version. On the next save the "
                                                         "project is updated to the new project version."));
         c_Message.SetCustomMinHeight(180, 180);
         c_Message.Execute();
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Save with error handling

   \param[in]  ors32_Err   Save error
   \param[in]  opc_Parent  parent widget
*/
//----------------------------------------------------------------------------------------------------------------------

void C_PopErrorHandling::h_ProjectSaveErr(const int32_t & ors32_Err, QWidget * const opc_Parent)
{
   if (ors32_Err != C_NO_ERR)
   {
      C_OgeWiCustomMessage c_Message(opc_Parent, C_OgeWiCustomMessage::E_Type::eERROR);
      c_Message.SetHeading(C_GtGetText::h_GetText("Project save"));
      switch (ors32_Err)
      {
      case C_RD_WR:
         {
            const QString c_LogLink = C_GtGetText::h_GetText("For details see ") +
                                      C_Uti::h_GetLink(C_GtGetText::h_GetText("log file."), mc_STYLE_GUIDE_COLOR_LINK,
                                                       C_OscLoggingHandler::h_GetCompleteLogFileLocation().c_str());
            QString c_Details = C_GtGetText::h_GetText(
               "Possible reasons:<br/>- The chosen path is too long. The project might have been partially written, "
               "but will most likely be corrupt and cannot be used.<br/>- Missing write permissions on chosen directory.<br/>");
            c_Details += c_LogLink;
            c_Message.SetDescription(C_GtGetText::h_GetText("Problems accessing file system."));
            c_Message.SetDetails(c_Details);
            break;
         }
      case C_RANGE:
         c_Message.SetDescription(C_GtGetText::h_GetText("Path is empty."));
         break;
      case C_NOACT:
         c_Message.SetDescription(C_GtGetText::h_GetText("Could not create project directory."));
         break;
      default:
         c_Message.SetDescription(C_GtGetText::h_GetText("Unknown cause."));
         break;
      }

      if (ors32_Err == C_RD_WR)
      {
         c_Message.SetCustomMinHeight(180, 300);
      }
      else
      {
         c_Message.SetCustomMinHeight(180, 180);
      }
      c_Message.Execute();
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  \brief   Create service project error handling

   \param[in]   ors32_Err     Save error
   \param[in]   opc_Parent    parent widget
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PopErrorHandling::h_ServiceProjectSaveErr(const int32_t & ors32_Err, QWidget * const opc_Parent)
{
   if (ors32_Err != C_NO_ERR)
   {
      C_OgeWiCustomMessage c_Message(opc_Parent, C_OgeWiCustomMessage::E_Type::eERROR);
      c_Message.SetHeading(C_GtGetText::h_GetText("Create Service Project"));
      switch (ors32_Err)
      {
      case C_RANGE:
         c_Message.SetDescription(C_GtGetText::h_GetText("Invalid view configuration."));
         break;
      case C_RD_WR:
         c_Message.SetDescription(C_GtGetText::h_GetText("Problems accessing file system."));
         break;
      case C_NOACT:
         c_Message.SetDescription(C_GtGetText::h_GetText("Could not create temporary project directory."));
         break;
      case C_BUSY:
         c_Message.SetDescription(C_GtGetText::h_GetText("Could not delete temporary folder."));
         break;
      case C_COM:
         c_Message.SetDescription(C_GtGetText::h_GetText("Bus sorting failed."));
         break;
      case C_WARN:
         c_Message.SetDescription(C_GtGetText::h_GetText("No view or part of it is active."));
         break;
      case C_CONFIG:
         c_Message.SetDescription(C_GtGetText::h_GetText("Expected file for project generation missing."));
         break;
      default:
         c_Message.SetDescription(C_GtGetText::h_GetText("Unknown cause."));
         break;
      }
      c_Message.SetCustomMinHeight(180, 180);
      c_Message.Execute();
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Handle gettext initialize error

   \param[in]   ors32_Err   Error
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PopErrorHandling::h_GetTextInitializeErr(const int32_t & ors32_Err)
{
   if (ors32_Err != C_NO_ERR)
   {
      C_OgeWiError * const pc_Widget = new C_OgeWiError();
      C_OgePopUpDialog c_Dialog;
      c_Dialog.SetTitle("Error occurred!");
      c_Dialog.SetWidget(pc_Widget);
      pc_Widget->SaveParentDialog(&c_Dialog);

      switch (ors32_Err)
      {
      case C_CONFIG:
         pc_Widget->SetErrorDescription(C_GtGetText::h_GetText("C_OscGetText - h_Initialize:\n"
                                                               "orc_BasePath does not point to a valid directory"));
         c_Dialog.exec();
         break;
      case C_NOACT:
         pc_Widget->SetErrorDescription(C_GtGetText::h_GetText("C_OscGetText - h_Initialize:\n"
                                                               "internal error in gettext library"));
         c_Dialog.exec();
         break;
      default:
         pc_Widget->SetErrorDescription(C_GtGetText::h_GetText("C_OscGetText - h_Initialize: unknown error"));
         c_Dialog.exec();
         break;
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Constructor
*/
//----------------------------------------------------------------------------------------------------------------------
C_PopErrorHandling::C_PopErrorHandling()
{
}
