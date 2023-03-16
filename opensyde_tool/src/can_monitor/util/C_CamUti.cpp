//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       CAN monitor utility functions. (implementation)

   CAN monitor utility functions

   \copyright   Copyright 2019 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.hpp"

#include <QDir>

#include "cam_constants.hpp"
#include "C_CamUti.hpp"
#include "C_CamProHandler.hpp"
#include "C_Uti.hpp"
#include "C_OgeWiCustomMessage.hpp"
#include "C_GtGetText.hpp"
#include "C_OscUtils.hpp"
#include "C_OgeWiUtil.hpp"

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
/*! \brief   Always get absolute path from path relative to project file.

   \param[in]  orc_Path     Absolute or relative path

   \return
   Absolute path
*/
//----------------------------------------------------------------------------------------------------------------------
QString C_CamUti::h_GetAbsPathFromProj(const QString & orc_Path)
{
   QString c_Folder = C_CamProHandler::h_GetInstance()->GetCurrentProjDir(); // always absolute or empty

   if (c_Folder == "")
   {
      // default: relative to executable path
      c_Folder = C_Uti::h_GetExePath();
   }

   return C_Uti::h_ConcatPathIfNecessary(c_Folder, orc_Path);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Handle paths after file save dialog.

   Check if path could be made relative and ask user if he wants to save the path
   relative or absolute.

   Note: If one of the paths is empty this simply returns the given path.
   If the reference path is a file path (ending on File.txt), behavior is undefined
   (handling directory AND files AND existing AND non-existing did not work well).
   If the reference path is not absolute, behavior is undefined
   (Qt then defaults to calling path, which is often but not always the path of the executable).

   \param[in]     opc_Parent                 parent widget (for parent of message box)
   \param[in]     orc_Path                   relative or absolute path of file or directory
   \param[in]     orc_AbsoluteReferenceDir   absolute path of reference directory

   \retval   String with path    Path the user wants to save or input path if relativeness is not possible
   \retval   Empty string        orc_Path has at least one invalid character
*/
//----------------------------------------------------------------------------------------------------------------------
QString C_CamUti::h_AskUserToSaveRelativePath(QWidget * const opc_Parent, const QString & orc_Path,
                                              const QString & orc_AbsoluteReferenceDir)
{
   QString c_Return;
   QString c_PathRelative;
   QString c_PathAbsolute;

   // Check first if path is a valid path with no unwanted characters
   if (C_OscUtils::h_CheckValidFilePath(orc_Path.toStdString().c_str()) == false)
   {
      C_OgeWiUtil::h_ShowPathInvalidError(opc_Parent, orc_Path);
      c_Return = "";
   }
   else if (C_Uti::h_IsPathRelativeToDir(orc_Path, orc_AbsoluteReferenceDir, c_PathAbsolute, c_PathRelative) == true)
   {
      // ask user
      C_OgeWiCustomMessage c_Message(opc_Parent, C_OgeWiCustomMessage::eQUESTION);
      c_Message.SetHeading(C_GtGetText::h_GetText("Relative Path"));
      c_Message.SetDescription(C_GtGetText::h_GetText("Do you want to save the selected path relative or absolute?"));
      c_Message.SetDetails(static_cast<QString>(C_GtGetText::h_GetText("Relative path: %1 \nAbsolute path: %2")).
                           arg(c_PathRelative).arg(c_PathAbsolute));
      c_Message.SetOkButtonText(C_GtGetText::h_GetText("Relative"));
      c_Message.SetNoButtonText(C_GtGetText::h_GetText("Absolute"));

      if (c_Message.Execute() == C_OgeWiCustomMessage::eOK)
      {
         c_Return = c_PathRelative;
      }
      else
      {
         c_Return = c_PathAbsolute;
      }
   }
   else
   {
      // Nothing to do
      c_Return = orc_Path;
   }

   return c_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Check if path contains variables (e.g. %{CAN_MONITOR_PROJECT}) and resolve them.

   \param[in]       orc_Path     path that probably contains variables

   \return
   Resolved path
*/
//----------------------------------------------------------------------------------------------------------------------
QString C_CamUti::h_ResolvePlaceholderVariables(const QString & orc_Path)
{
   QString c_Return = orc_Path;

   // first check for indicator %
   if (c_Return.contains("%") == true)
   {
      // replace general path variables
      c_Return = C_OscUtils::h_ResolveProjIndependentPlaceholderVariables(c_Return.toStdString().c_str()).c_str();

      // resolve CAN-monitor- and project-specific variables
      if (c_Return.contains(mc_PATH_VARIABLE_CAN_MONITOR_BIN) == true)
      {
         c_Return.replace(mc_PATH_VARIABLE_CAN_MONITOR_BIN, C_Uti::h_GetExePath() + '/');
      }
      if (c_Return.contains(mc_PATH_VARIABLE_CAN_MONITOR_PROJ) == true)
      {
         c_Return.replace(mc_PATH_VARIABLE_CAN_MONITOR_PROJ,
                          C_CamProHandler::h_GetInstance()->GetCurrentProjDir() + '/');
      }
   }

   return c_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Resolve variables and interpret relative paths as relative to project

   Resolve all path variables (e.g. %{CAN_MONITOR_PROJ}). If the resulting path is relative
   it is meant as relative to CAN Monitor project, so we make it absolute by concatenating
   with project path.

   \param[in]       orc_Path    path that probably contains path variables and if relative is relative to project

   \return
   Absolute and resolved path
*/
//----------------------------------------------------------------------------------------------------------------------
QString C_CamUti::h_GetResolvedAbsolutePathFromProj(const QString & orc_Path)
{
   QString c_Return = orc_Path;

   // first resolve, then make absolute (resolving might change from relative to absolute!)
   c_Return = C_CamUti::h_ResolvePlaceholderVariables(c_Return);
   c_Return = C_CamUti::h_GetAbsPathFromProj(c_Return);

   return c_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Resolve variables and interpret relative paths as relative to executable

   Resolve all path variables (e.g. %{CAN_MONITOR_PROJ}). If the resulting path is relative
   it is meant as relative to CANMonitor.exe, so we make it absolute by concatenating
   with executable path.

   \param[in]       orc_Path    path that probably contains path variables and if relative is relative to executable

   \return
   Absolute and resolved path
*/
//----------------------------------------------------------------------------------------------------------------------
QString C_CamUti::h_GetResolvedAbsolutePathFromExe(const QString & orc_Path)
{
   QString c_Return = orc_Path;

   // first resolve, then make absolute (resolving might change from relative to absolute!)
   c_Return = C_CamUti::h_ResolvePlaceholderVariables(c_Return);
   c_Return = C_Uti::h_GetAbsolutePathFromExe(c_Return);

   return c_Return;
}

C_CamUti::C_CamUti(void)
{
}
