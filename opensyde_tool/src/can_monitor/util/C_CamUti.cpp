//-----------------------------------------------------------------------------
/*!
   \internal
   \file
   \brief       CAN monitor utility functions. (implementation)

   CAN monitor utility functions

   \implementation
   project     openSYDE
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     05.02.2019  STW/G.Landsgesell
   \endimplementation
*/
//-----------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------- */
#include "precomp_headers.h"

#include <QDir>

#include "C_CamUti.h"
#include "C_CamProHandler.h"
#include "C_Uti.h"
#include "C_OgeWiCustomMessage.h"
#include "C_GtGetText.h"

/* -- Used Namespaces ------------------------------------------------------ */
using namespace stw_types;
using namespace stw_opensyde_gui_elements;
using namespace stw_opensyde_gui_logic;

/* -- Module Global Constants ---------------------------------------------- */

/* -- Types ---------------------------------------------------------------- */

/* -- Global Variables ----------------------------------------------------- */

/* -- Module Global Variables ---------------------------------------------- */

/* -- Module Global Function Prototypes ------------------------------------ */

/* -- Implementation ------------------------------------------------------- */

//-----------------------------------------------------------------------------
/*!
   \brief   Always get absolute path from path relative to project file.

   \param[in]  orc_Path     Absolute or relative path

   \return
   Absolute path

   \created     11.02.2019  STW/G.Landsgesell
*/
//-----------------------------------------------------------------------------
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

//-----------------------------------------------------------------------------
/*!
   \brief   Handle paths after file save dialog.

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

   \return
   path the user wants to save

   \created     11.02.2019  STW/G.Landsgesell
*/
//-----------------------------------------------------------------------------
QString C_CamUti::h_AskUserToSaveRelativePath(QWidget * const opc_Parent, const QString & orc_Path,
                                              const QString & orc_AbsoluteReferenceDir)
{
   QString c_Return = orc_Path;
   QString c_PathRelative;
   QString c_PathAbsolute;

   if (C_Uti::h_IsPathRelativeToDir(orc_Path, orc_AbsoluteReferenceDir, c_PathAbsolute, c_PathRelative) == true)
   {
      // ask user
      C_OgeWiCustomMessage c_Message(opc_Parent, C_OgeWiCustomMessage::eQUESTION);
      c_Message.SetHeading(C_GtGetText::h_GetText("Relative Path"));
      c_Message.SetDescription(C_GtGetText::h_GetText("Do you want to save the selected path relative or absolute?"));
      c_Message.SetDetails(QString(C_GtGetText::h_GetText("Relative path: %1 \nAbsolute path: %2")).
                           arg(c_PathRelative).arg(c_PathAbsolute));
      c_Message.SetOKButtonText(C_GtGetText::h_GetText("Relative"));
      c_Message.SetNOButtonText(C_GtGetText::h_GetText("Absolute"));

      if (c_Message.Execute() == C_OgeWiCustomMessage::eOK)
      {
         c_Return = c_PathRelative;
      }
      else
      {
         c_Return = c_PathAbsolute;
      }
   }

   return c_Return;
}

C_CamUti::C_CamUti(void)
{
}
