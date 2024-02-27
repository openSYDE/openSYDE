//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Generic utility to be used within and outside CAN Monitor

   Generic utility to be used within and outside CAN Monitor

   \copyright   Copyright 2023 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.hpp"

#include "C_Uti.hpp"
#include "stwtypes.hpp"
#include "C_OscUtils.hpp"
#include "C_OgeWiUtil.hpp"
#include "C_GtGetText.hpp"
#include "C_CamUtiGeneric.hpp"
#include "C_OgeWiCustomMessage.hpp"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
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
/*! \brief   Handle paths after file save dialog.

   Check if path could be made relative and ask user if he wants to save the path
   relative or absolute.

   Note: If one of the paths is empty this simply returns the given path.
   If the reference path is a file path (ending on File.txt), behavior is undefined
   (handling directory AND files AND existing AND non-existing did not work well).
   If the reference path is not absolute, behavior is undefined
   (Qt then defaults to calling path, which is often but not always the path of the executable).

   \param[in]  opc_Parent                 parent widget (for parent of message box)
   \param[in]  orc_Path                   relative or absolute path of file or directory
   \param[in]  orc_AbsoluteReferenceDir   absolute path of reference directory

   \retval   String with path    Path the user wants to save or input path if relativeness is not possible
   \retval   Empty string        orc_Path has at least one invalid character
*/
//----------------------------------------------------------------------------------------------------------------------
QString C_CamUtiGeneric::h_AskUserToSaveRelativePath(QWidget * const opc_Parent, const QString & orc_Path,
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
/*! \brief  Default constructor
*/
//----------------------------------------------------------------------------------------------------------------------
C_CamUtiGeneric::C_CamUtiGeneric()
{
}
