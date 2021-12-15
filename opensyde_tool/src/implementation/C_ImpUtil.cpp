//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Class for use case implementation functionality (implementation)

   Class for use case implementation functionality

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.h"

#include <windows.h> //tlhelp32 does not do this by itself ...
#include <tlhelp32.h>
#include <QDir>
#include <QProcess>
#include <QTextStream>
#include <QApplication>

#include "C_ImpUtil.h"
#include "TGLUtils.h"
#include "TGLFile.h"
#include "stwerrors.h"
#include "constants.h"
#include "C_GtGetText.h"
#include "C_UsHandler.h"
#include "C_PuiProject.h"
#include "C_PuiSdHandler.h"
#include "C_OSCLoggingHandler.h"
#include "C_OgeWiCustomMessage.h"
#include "C_OgeWiUtil.h"
#include "C_Uti.h"
#include "C_PopUtil.h"
#include "C_OSCUtils.h"
#include "C_PuiUtil.h"
#include "C_SdCodeGenerationDialog.h"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw_scl;
using namespace stw_tgl;
using namespace stw_types;
using namespace stw_errors;
using namespace stw_opensyde_core;
using namespace stw_opensyde_gui_logic;
using namespace stw_opensyde_gui_elements;
using namespace stw_opensyde_gui;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

/* -- Global Variables ---------------------------------------------------------------------------------------------- */

/* -- Module Global Variables --------------------------------------------------------------------------------------- */

/* -- Module Global Function Prototypes ----------------------------------------------------------------------------- */

/* -- Implementation ------------------------------------------------------------------------------------------------ */

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default constructor
*/
//----------------------------------------------------------------------------------------------------------------------
C_ImpUtil::C_ImpUtil(void)
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Wrapper function to export files for all nodes

   Errors are handled internally.

   \param[in]  opc_Parent  parent widget
*/
//----------------------------------------------------------------------------------------------------------------------
void C_ImpUtil::h_ExportCodeAll(QWidget * const opc_Parent)
{
   std::vector<uint32> c_Indices;
   c_Indices.reserve(C_PuiSdHandler::h_GetInstance()->GetOSCNodesSize());
   for (uint32 u32_ItNode = 0; u32_ItNode < C_PuiSdHandler::h_GetInstance()->GetOSCNodesSize(); ++u32_ItNode)
   {
      c_Indices.push_back(u32_ItNode);
   }
   C_ImpUtil::h_ExportCodeNodes(c_Indices, opc_Parent);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Export files for all specified nodes and specified applications

   Errors are handled internally.

   \param[in]  orc_NodeIndices         Node indices
   \param[in]  orc_AppIndicesPerNode   Vector of vectors of application indices (one app-indices-vector per node index)
   \param[in]  opc_Parent              parent widget
*/
//----------------------------------------------------------------------------------------------------------------------
void C_ImpUtil::h_ExportCode(const std::vector<uint32> & orc_NodeIndices,
                             const std::vector< std::vector<uint32> > & orc_AppIndicesPerNode,
                             QWidget * const opc_Parent)
{
   sint32 s32_Result = C_NO_ERR;
   bool q_Continue = true;
   C_OgeWiCustomMessage c_Message(opc_Parent);

   // number of nodes must equal number of nodes for that applications were given
   tgl_assert(orc_NodeIndices.size() == orc_AppIndicesPerNode.size());
   if (orc_NodeIndices.size() != orc_AppIndicesPerNode.size())
   {
      q_Continue = false;
   }

   // check if all datapools are assigned
   if (q_Continue == true)
   {
      QString c_DataPoolErrorMessage;
      //Check data pools
      for (uint32 u32_ItInNode = 0; u32_ItInNode < orc_NodeIndices.size(); ++u32_ItInNode)
      {
         const C_OSCNode * const pc_Node = C_PuiSdHandler::h_GetInstance()->GetOSCNodeConst(
            orc_NodeIndices[u32_ItInNode]);
         if (pc_Node != NULL)
         {
            //Only check nodes that really have programmable applications (assignment not mandatory for PSI generation)
            if (C_PuiSdHandler::h_GetInstance()->GetProgrammableApplications(orc_NodeIndices[u32_ItInNode]).size() > 0)
            {
               for (uint32 u32_ItDataPool = 0; u32_ItDataPool < pc_Node->c_DataPools.size(); ++u32_ItDataPool)
               {
                  const C_OSCNodeDataPool & rc_DataPool = pc_Node->c_DataPools[u32_ItDataPool];
                  if (rc_DataPool.s32_RelatedDataBlockIndex < 0L)
                  {
                     q_Continue = false;
                     c_DataPoolErrorMessage += "- " + static_cast<QString>(pc_Node->c_Properties.c_Name.c_str()) +
                                               ", Datapool \"" +
                                               rc_DataPool.c_Name.c_str() + "\"\n";
                  }
               }
            }
         }
      }

      // pop up error info if there are unassigned datapools
      if (q_Continue == false)
      {
         c_Message.SetType(C_OgeWiCustomMessage::E_Type::eERROR);
         c_Message.SetHeading(C_GtGetText::h_GetText("File Generation"));
         c_Message.SetDescription(C_GtGetText::h_GetText(
                                     "Cannot generate files. Assign all Datapools and retry."));
         c_Message.SetDetails(static_cast<QString>(C_GtGetText::h_GetText(
                                                      "The following Datapools have no assigned application:\n%1")).arg(
                                 c_DataPoolErrorMessage));
         c_Message.SetCustomMinHeight(180, 250);
         c_Message.Execute();
      }
   }

   // check for system definition errors
   if (q_Continue == true)
   {
      bool q_SysDefInvalid = false;
      //Node error
      for (uint32 u32_ItNode = 0; u32_ItNode < C_PuiSdHandler::h_GetInstance()->GetOSCNodesSize();
           ++u32_ItNode)
      {
         if (C_PuiSdHandler::h_GetInstance()->CheckNodeConflict(u32_ItNode) == true)
         {
            q_SysDefInvalid = true;
            break;
         }
      }
      //Bus error
      for (uint32 u32_ItBus = 0; u32_ItBus < C_PuiSdHandler::h_GetInstance()->GetOSCBusesSize(); ++u32_ItBus)
      {
         if (C_PuiSdHandler::h_GetInstance()->CheckBusConflict(u32_ItBus) == true)
         {
            q_SysDefInvalid = true;
            break;
         }
      }
      // Ask user to continue although there are system definition errors
      if (q_SysDefInvalid == true)
      {
         C_OgeWiCustomMessage c_Question(opc_Parent, C_OgeWiCustomMessage::E_Type::eWARNING);
         c_Question.SetHeading("CONFIRM FILE GENERATION");
         c_Question.SetDescription(C_GtGetText::h_GetText("There are SYSTEM DEFINITION errors. "
                                                          "Do you really want to generate files?"));
         c_Question.SetOKButtonText(C_GtGetText::h_GetText("Continue"));
         c_Question.SetNOButtonText(C_GtGetText::h_GetText("Cancel"));
         c_Question.SetCustomMinHeight(180, 180);
         if (c_Question.Execute() == C_OgeWiCustomMessage::eYES)
         {
            q_Continue = true;
         }
         else
         {
            q_Continue = false;
         }
      }
   }

   // inform user which files will get generated, check if all file generators are not empty and ask for confirmation
   if (q_Continue == true)
   {
      QString c_DataBlockInfoMessage;
      QString c_EraseInfoMessage;
      QString c_ErrorMessage;
      bool q_CodeGeneratorMissing = false;

      c_DataBlockInfoMessage += C_GtGetText::h_GetText("There will be files generated for the following node(s):<br>");
      for (uint32 u32_ItNode = 0; u32_ItNode < orc_NodeIndices.size(); ++u32_ItNode)
      {
         const C_OSCNode * const pc_Node =
            C_PuiSdHandler::h_GetInstance()->GetOSCNodeConst(orc_NodeIndices[u32_ItNode]);
         tgl_assert(pc_Node != NULL);
         if (pc_Node != NULL)
         {
            for (uint32 u32_ItApp = 0; u32_ItApp < orc_AppIndicesPerNode[u32_ItNode].size(); ++u32_ItApp)
            {
               const C_OSCNodeApplication * const pc_Application =
                  C_PuiSdHandler::h_GetInstance()->GetApplication(orc_NodeIndices[u32_ItNode],
                                                                  orc_AppIndicesPerNode[u32_ItNode][u32_ItApp]);
               tgl_assert(pc_Application != NULL);
               if (pc_Application != NULL)
               {
                  // message for generating
                  //Translation: 1 = Node name, 2 = Application name
                  c_DataBlockInfoMessage +=
                     static_cast<QString>(C_GtGetText::h_GetText("- %1, Data Block \"%2\"<br>")).arg(
                        pc_Node->c_Properties.c_Name.c_str()).arg(pc_Application->c_Name.c_str());

                  // message for erase information
                  const QDir c_GenerationDir(h_GetAbsoluteGeneratedDir(*pc_Application, pc_Node->c_Properties.c_Name));
                  // only show already existing directories (if directory does not exist erasing is no problem)
                  if (c_GenerationDir.exists() == true)
                  {
                     c_EraseInfoMessage += C_Uti::h_GetLink(c_GenerationDir.path(), mc_STYLE_GUIDE_COLOR_LINK,
                                                            "file:" + c_GenerationDir.path());
                     c_EraseInfoMessage += "<br>";
                  }

                  // check if file generator is missing and adapt error message
                  if (pc_Application->c_CodeGeneratorPath == "")
                  {
                     q_CodeGeneratorMissing = true;
                     c_ErrorMessage += "- " + static_cast<QString>(pc_Node->c_Properties.c_Name.c_str()) +
                                       ", Data Block \"" + pc_Application->c_Name.c_str() + "\"<br>";
                  }
               }
            }
         }
      }

      // every data block has non-empty file generator path
      if (q_CodeGeneratorMissing == false)
      {
         QString c_Description;
         //  add surroundings if there are existing directories that will get erased
         if (c_EraseInfoMessage != "")
         {
            c_EraseInfoMessage.prepend(C_GtGetText::h_GetText("The content of the following existing directories "
                                                              "will get deleted permanently:<br>"));
            c_EraseInfoMessage.append("<br>");
            c_Description = static_cast<QString>(C_GtGetText::h_GetText("All target directories will be erased and its "
                                                                        "content will be deleted permanently! "));
            c_Message.SetType(C_OgeWiCustomMessage::E_Type::eQUESTION);
            c_Message.SetHeading(C_GtGetText::h_GetText("Erase target directories"));
            c_Message.SetDescription(c_Description);
            c_Message.SetOKButtonText(C_GtGetText::h_GetText("Generate Files"));
            c_Message.SetNOButtonText(C_GtGetText::h_GetText("Cancel"));
            c_Message.SetDetails("<a/>" + c_EraseInfoMessage + c_DataBlockInfoMessage);
            c_Message.SetCustomMinHeight(200, 400);
            c_Message.SetCustomMinWidth(650);
            if (c_Message.Execute() != C_OgeWiCustomMessage::eYES)
            {
               q_Continue = false;
            }
         }
      }
      // tell user to set file generators for every data block
      else
      {
         c_Message.SetType(C_OgeWiCustomMessage::E_Type::eERROR);
         c_Message.SetHeading(C_GtGetText::h_GetText("File generation"));
         c_Message.SetDescription(C_GtGetText::h_GetText("Cannot generate files. "
                                                         "Set a file generator for every Data Block."));
         c_Message.SetDetails(static_cast<QString>(C_GtGetText::h_GetText("<a/>The following Data Blocks "
                                                                          "have no file generator:<br>%1")).arg(
                                 c_ErrorMessage));
         c_Message.SetCustomMinHeight(180, 300);
         c_Message.Execute();
         q_Continue = false;
      }
   }

   // finally we are ready to generate file
   if (q_Continue == true)
   {
      std::vector<C_ImpCodeGenerationReportWidget::C_ReportData> c_ExportInfo;

      // export files for each node
      QApplication::setOverrideCursor(Qt::WaitCursor);
      for (uint32 u32_ItNode = 0; (u32_ItNode < orc_NodeIndices.size()) && (s32_Result == C_NO_ERR); ++u32_ItNode)
      {
         // Maybe once replace "true" flag with user-confirmed value
         s32_Result = C_ImpUtil::mh_ExportCodeNode(orc_NodeIndices[u32_ItNode], orc_AppIndicesPerNode[u32_ItNode],
                                                   c_ExportInfo, true);
      }
      QApplication::restoreOverrideCursor();

      // give user detailed feedback about success or fail
      if (s32_Result == C_NO_ERR) // inform about success with file generation report
      {
         QPointer<C_OgePopUpDialog> const c_PopUpDialogReportDialog = new C_OgePopUpDialog(opc_Parent, opc_Parent);
         C_ImpCodeGenerationReportWidget * const pc_DialogExportReport =  new C_ImpCodeGenerationReportWidget(
            *c_PopUpDialogReportDialog);

         c_PopUpDialogReportDialog->SetSize(QSize(1100, 700));
         pc_DialogExportReport->CreateReport(c_ExportInfo);

         // display message report
         c_PopUpDialogReportDialog->exec();
         if (c_PopUpDialogReportDialog != NULL)
         {
            c_PopUpDialogReportDialog->HideOverlay();
            c_PopUpDialogReportDialog->deleteLater();
         }
      } //lint !e429  //no memory leak because of the parent of pc_Dialog and the Qt memory management
      else
      // inform about fail with error message box
      {
         C_OgeWiCustomMessage c_MessageResult(opc_Parent);
         c_MessageResult.SetType(C_OgeWiCustomMessage::E_Type::eERROR);
         c_MessageResult.SetHeading(C_GtGetText::h_GetText("File generation"));
         c_MessageResult.SetDescription(C_GtGetText::h_GetText("File generation failed."));
         //Update log file
         C_OSCLoggingHandler::h_Flush();
         const QString c_Details = C_GtGetText::h_GetText("For details see ") +
                                   C_Uti::h_GetLink(C_GtGetText::h_GetText("log file"), mc_STYLESHEET_GUIDE_COLOR_LINK,
                                                    C_OSCLoggingHandler::h_GetCompleteLogFileLocation().c_str()) +
                                   C_GtGetText::h_GetText(" or log file(s) of file generator(s).");
         c_MessageResult.SetDetails(c_Details);
         c_MessageResult.SetCustomMinHeight(180, 250);
         c_MessageResult.Execute();
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Export files for valid nodes

   Check given nodes for valid ones (i.e. ones with file generation Data Blocks)
   and export files for them.

   \param[in]  orc_NodeIndices   node indices (can be a single node too - wrapped in a 1-length-vector)
   \param[in]  opc_Parent        parent widget
*/
//----------------------------------------------------------------------------------------------------------------------
void C_ImpUtil::h_ExportCodeNodes(const std::vector<uint32> & orc_NodeIndices, QWidget * const opc_Parent)
{
   std::vector<uint32> c_ValidNodeIndices;
   std::vector<uint32> c_ProgAppsNodeIndices;
   std::vector<std::vector<uint32> > c_AllProgApps;
   C_OgeWiCustomMessage c_Message(opc_Parent);

   // get valid nodes (i.e. ones with file generation Data Blocks)
   for (uint32 u32_ItInNode = 0; u32_ItInNode < orc_NodeIndices.size(); ++u32_ItInNode)
   {
      c_ProgAppsNodeIndices.clear();
      const C_OSCNode * const pc_Node = C_PuiSdHandler::h_GetInstance()->GetOSCNodeConst(orc_NodeIndices[u32_ItInNode]);
      tgl_assert(pc_Node != NULL);
      if (pc_Node != NULL)
      {
         c_ProgAppsNodeIndices = C_PuiSdHandler::h_GetInstance()->GetFileGenAppIndices(
            orc_NodeIndices[u32_ItInNode]);
         if (c_ProgAppsNodeIndices.size() > 0)
         {
            // add node to valid nodes
            c_ValidNodeIndices.push_back(orc_NodeIndices[u32_ItInNode]);
            //add Data Blocks
            c_AllProgApps.push_back(c_ProgAppsNodeIndices);
         }
      }
   }

   // inform user that there is no file generation Data Block
   if (c_ValidNodeIndices.size() == 0)
   {
      QString c_Text;
      if (orc_NodeIndices.size() == 1)
      {
         c_Text = C_GtGetText::h_GetText(
            "There are no Data Blocks with enabled file generation found. "
            "\nFiles cannot be generated.");
      }
      else
      {
         c_Text = C_GtGetText::h_GetText(
            "There are no nodes with Data Blocks with enabled file generation found."
            "\nFiles cannot be generated.");
      }
      c_Message.SetType(C_OgeWiCustomMessage::E_Type::eINFORMATION);
      c_Message.SetHeading(C_GtGetText::h_GetText("File generation"));
      c_Message.SetDescription(c_Text);
      c_Message.SetCustomMinHeight(180, 180);
      c_Message.Execute();
   }
   // generate files
   else
   {
      C_ImpUtil::h_ExportCode(c_ValidNodeIndices, c_AllProgApps, opc_Parent);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Export files for specified node

   \param[in]   ou32_NodeIndex   Node index
   \param[in]   orc_AppIndices   Application indices
   \param[out]  orc_ExportInfo   data structure containing information for report
   \param[in]   orq_Erase        flag for file generator: true -> erase target folder and all sub-folders

   \return
   C_NO_ERR       Operation success
   C_RANGE        Node index or application index invalid
   see C_ImpUtil::mh_ExecuteCodeGenerator for further errors
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_ImpUtil::mh_ExportCodeNode(const uint32 ou32_NodeIndex, const std::vector<stw_types::uint32> & orc_AppIndices,
                                    std::vector<C_ImpCodeGenerationReportWidget::C_ReportData> & orc_ExportInfo,
                                    const bool & orq_Erase)
{
   sint32 s32_Retval = C_NO_ERR;
   C_OSCNode c_Node;

   if (C_PuiSdHandler::h_GetInstance()->GetSortedOSCNodeConst(ou32_NodeIndex, c_Node) == C_NO_ERR)
   {
      // call file generator for each application
      for (uint32 u32_Pos = 0; (u32_Pos < orc_AppIndices.size()) && (s32_Retval == C_NO_ERR); u32_Pos++)
      {
         // check if valid application index
         if (orc_AppIndices[u32_Pos] < c_Node.c_Applications.size())
         {
            const C_OSCNodeApplication * const pc_Application =
               C_PuiSdHandler::h_GetInstance()->GetApplication(ou32_NodeIndex, orc_AppIndices[u32_Pos]);
            tgl_assert(pc_Application != NULL);
            if (pc_Application != NULL)
            {
               const QString c_CompleteExportFolderName =
                  h_GetAbsoluteGeneratedDir(*pc_Application, c_Node.c_Properties.c_Name);
               const QString c_CompleteCodeGenerator =
                  C_PuiUtil::h_GetResolvedAbsPathFromExe(
                     pc_Application->c_CodeGeneratorPath.c_str(),
                     C_PuiUtil::h_GetResolvedAbsPathFromProject(pc_Application->c_ProjectPath.c_str()));

               C_ImpCodeGenerationReportWidget::C_ReportData c_ReportInfo;
               c_ReportInfo.c_NodeName = c_Node.c_Properties.c_Name.c_str();
               c_ReportInfo.c_AppName = pc_Application->c_Name.c_str();
               c_ReportInfo.c_CodeGeneratorPath = c_CompleteCodeGenerator;
               c_ReportInfo.u16_CodeVersion = pc_Application->u16_GenCodeVersion;
               c_ReportInfo.c_Directory = c_CompleteExportFolderName;

               s32_Retval = mh_ExecuteCodeGenerator(c_ReportInfo.c_NodeName, c_ReportInfo.c_AppName,
                                                    c_ReportInfo.c_Directory, c_ReportInfo.c_GeneratedFiles,
                                                    c_ReportInfo.c_CodeGeneratorPath,
                                                    orq_Erase);

               if (s32_Retval == C_NO_ERR)
               {
                  // Add report information for this application
                  orc_ExportInfo.push_back(c_ReportInfo);
               }
            }
         }
         else
         {
            s32_Retval = C_RANGE;
         }
      }
   }
   else
   {
      s32_Retval = C_RANGE;
   }

   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Open IDE

   \param[in]  orc_IdeExeCall    Command line command for executable call (eventually including flags)

   \return
   C_NO_ERR Operation success
   C_CONFIG Operation failure: cannot find IDE
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_ImpUtil::h_OpenIDE(const QString & orc_IdeExeCall)
{
   sint32 s32_Retval = C_NO_ERR;

   if (orc_IdeExeCall.compare("") != 0)
   {
      std::vector<HWND> c_Windows;
      QString c_ExeOnly;
      QStringList c_HelpList;
      bool q_ContinueWithExeOpening = false;

      // Note:
      // A command line call either begins with a path in quotation marks or the first space is the space
      // that separates the executable from its arguments.
      // If none of the above is the case, QProcess will also fail to start because it can not distinguish
      // spaces from command line separators.

      // remove flags from call (cf. note above)
      if (orc_IdeExeCall.startsWith('\"'))
      {
         // search for next ", which is the closing one and marks end of executable path
         c_HelpList = orc_IdeExeCall.split('\"');
         c_ExeOnly = c_HelpList[1];
      }
      else
      {
         // split at " "
         c_HelpList = orc_IdeExeCall.split(' ');
         c_ExeOnly = c_HelpList[0];
      }

      // remove trouble characters
      c_ExeOnly.remove("\"");          // remove " (paths do not contain such)
      c_ExeOnly = c_ExeOnly.trimmed(); // remove trailing whitespace

      // check if executable file exists
      const QFileInfo c_ExeFile(c_ExeOnly);
      if (c_ExeFile.exists() == true)
      {
         c_ExeOnly = c_ExeFile.fileName();

         C_ImpUtil::mh_GetExistingApplicationHandle(c_ExeOnly.toStdWString().c_str(), c_Windows);
         if (c_Windows.size() > 0)
         {
            for (uint32 u32_ItWindow = 0; u32_ItWindow < c_Windows.size(); ++u32_ItWindow)
            {
               // bring window to top
               BringWindowToTop(c_Windows[u32_ItWindow]);
               // restore if minimized
               if (IsIconic(c_Windows[u32_ItWindow]) == true)
               {
                  PostMessage(c_Windows[u32_ItWindow], WM_SYSCOMMAND, SC_RESTORE, 0);
               }
            }
         }
         else // we know that there is no open application with this executable name
         {
            q_ContinueWithExeOpening = true;
         }
      }
      // we could not extract valid executable name, but want to try to open it anyway (maybe results in a second
      // instance of already opened program or in an error because user inserted invalid path)
      else
      {
         q_ContinueWithExeOpening = true;
      }

      if (q_ContinueWithExeOpening == true)
      {
         const bool q_Temp = QProcess::startDetached(orc_IdeExeCall, QStringList());

         if (q_Temp == false)
         {
            s32_Retval = C_CONFIG;
            osc_write_log_error("Open IDE",
                                static_cast<QString>(
                                   "Could not start IDE. Reason: Most likely due to insufficient permissions or the executable "
                                   " \"%1\" is missing.").arg(orc_IdeExeCall).toStdString().c_str());
         }
      }
   }
   else
   {
      s32_Retval = C_CONFIG;
      osc_write_log_error("Open IDE", "No path to IDE executable given.");
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get location of syde coder c (standard file generator)

   \return
   Connectors directory location
*/
//----------------------------------------------------------------------------------------------------------------------
QString C_ImpUtil::h_GetSydeCoderCPath()
{
   return "../connectors/syde_coder_c/osy_syde_coder_c.exe";
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get directory of generated files of application.

   Combine paths for given application to get absolute path of directory
   at which files should get generated.

   \param[in]  orc_Application   Data block for which path is requested
   \param[in]  orc_NodeName      Node name of node where application belongs to

   \return
   Absolute path to location of generated files.
*/
//----------------------------------------------------------------------------------------------------------------------
QString C_ImpUtil::h_GetAbsoluteGeneratedDir(const C_OSCNodeApplication & orc_Application,
                                             const C_SCLString & orc_NodeName)
{
   QString c_Return;
   QString c_GenerateDir = orc_Application.c_GeneratePath.c_str();

   // check generate path: empty? --> use default relative path
   if ((c_GenerateDir.isEmpty() == true) && (orc_Application.e_Type != C_OSCNodeApplication::ePARAMETER_SET_HALC))
   {
      c_GenerateDir = h_GetDefaultGeneratedDir(orc_Application.c_Name, orc_NodeName);
   }

   // resolve path variables and make absolute
   c_Return = C_PuiUtil::h_GetResolvedAbsPathFromDbProject(orc_Application.c_ProjectPath.c_str(), c_GenerateDir);

   return c_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get default directory for generated files

   \param[in]  orc_ApplicationName  Application name
   \param[in]  orc_NodeName         Node name

   \return
   Default directory for generated files
*/
//----------------------------------------------------------------------------------------------------------------------
QString C_ImpUtil::h_GetDefaultGeneratedDir(const C_SCLString & orc_ApplicationName, const C_SCLString & orc_NodeName)
{
   C_SCLString c_Return = "./opensyde_generated/";

   c_Return += C_OSCUtils::h_NiceifyStringForFileName(orc_NodeName);
   c_Return += "/";
   c_Return += C_OSCUtils::h_NiceifyStringForFileName(orc_ApplicationName);
   return c_Return.c_str();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief Check project saving before file generation.

   Show error message if user wants to generate file without
   saving an empty project.

   \param[in]  opc_Parent  Parent widget (for message box)

   \return
      true     project was saved -> can continue with file generation
      false    project was not saved -> do not continue
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_ImpUtil::h_CheckProjForCodeGeneration(QWidget * const opc_Parent)
{
   bool q_Return = true;

   if (C_PopUtil::h_AskUserToContinue(opc_Parent, false) == true)
   {
      // if the project is empty do not continue
      if (C_PuiProject::h_GetInstance()->IsEmptyProject() == true)
      {
         C_OgeWiCustomMessage c_Message(opc_Parent, C_OgeWiCustomMessage::E_Type::eERROR);
         c_Message.SetHeading(C_GtGetText::h_GetText("File generation"));
         c_Message.SetDescription(C_GtGetText::h_GetText("Files cannot be generated without a valid project path."));
         c_Message.SetDetails(C_GtGetText::h_GetText(
                                 "The path where the generated files are saved must be set. "
                                 "Save the project to set a default path."));
         c_Message.SetCustomMinHeight(180, 250);
         c_Message.Execute();
         q_Return = false;
      }
   }
   else
   {
      // user canceled "safe system?"-dialog
      q_Return = false;
   }

   return q_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Handle paths after file save dialog.

   Check if path could be made relative and ask user if she wants to save the path
   relative or absolute.
   The function checks orc_Path for valid characters. See C_OSCUtils::h_IsStringNiceifiedForFilePath for
   details of the check. If the check fails an empty string will be returned.

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
QString C_ImpUtil::h_AskUserToSaveRelativePath(QWidget * const opc_Parent, const QString & orc_Path,
                                               const QString & orc_AbsoluteReferenceDir)
{
   QString c_Return;
   QString c_PathRelative;
   QString c_PathAbsolute;

   // Check first if path is a valid path with no unwanted characters
   if (C_OSCUtils::h_CheckValidFilePath(orc_Path.toStdString().c_str()) == false)
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
      c_Message.SetOKButtonText(C_GtGetText::h_GetText("Relative"));
      c_Message.SetNOButtonText(C_GtGetText::h_GetText("Absolute"));
      c_Message.SetCustomMinHeight(230, 250);

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
/*! \brief   Overloaded function that allows multiple file paths and only asks the user once.

   See original for more details.

   \param[in]  opc_Parent                 parent widget (for parent of message box)
   \param[in]  orc_Paths                  relative or absolute paths of files or directories
   \param[in]  orc_AbsoluteReferenceDir   absolute path of reference directory

   \retval   Strings with paths  Paths the user wants to save or input paths if relativeness is not possible at all
   \retval   Empty list          At least one string in orc_Paths has at least one invalid character
*/
//----------------------------------------------------------------------------------------------------------------------
QStringList C_ImpUtil::h_AskUserToSaveRelativePath(QWidget * const opc_Parent, const QStringList & orc_Paths,
                                                   const QString & orc_AbsoluteReferenceDir)
{
   QStringList c_Return = orc_Paths;
   QString c_PathRelative;
   QString c_PathAbsolute;

   // use original in case of one file only
   if (orc_Paths.size() == 1)
   {
      c_Return[0] = C_ImpUtil::h_AskUserToSaveRelativePath(opc_Parent, orc_Paths[0], orc_AbsoluteReferenceDir);

      if (c_Return[0] == "")
      {
         c_Return.clear();
      }
   }
   else if (orc_Paths.isEmpty() == false)
   {
      sint32 s32_Pos;
      QStringList c_PathsRelative;
      QStringList c_PathsAbsolute;
      QString c_InvalidPaths = "";
      QString c_Details;
      bool q_RelativePossible = false;

      // Check first if all paths are valid paths with no unwanted characters
      for (s32_Pos = 0; s32_Pos < c_Return.size(); ++s32_Pos)
      {
         if (C_OSCUtils::h_CheckValidFilePath(c_Return[s32_Pos].toStdString().c_str()) == false)
         {
            c_InvalidPaths += "- " + c_Return[s32_Pos] + "\n";
         }
      }

      if (c_InvalidPaths.size() > 0)
      {
         C_OgeWiUtil::h_ShowPathInvalidError(opc_Parent, c_InvalidPaths);
         c_Return.clear();
      }

      // make all paths relative
      for (s32_Pos = 0; s32_Pos < c_Return.size(); ++s32_Pos)
      {
         if (C_Uti::h_IsPathRelativeToDir(c_Return[s32_Pos], orc_AbsoluteReferenceDir, c_PathAbsolute,
                                          c_PathRelative) == true)
         {
            q_RelativePossible = true;
         }

         c_PathsAbsolute.append(c_PathAbsolute);
         c_PathsRelative.append(c_PathRelative);
      }

      if (q_RelativePossible == true)
      {
         // ask user
         C_OgeWiCustomMessage c_Message(opc_Parent, C_OgeWiCustomMessage::eQUESTION,
                                        C_GtGetText::h_GetText("There are paths that could be stored relative. "
                                                               "Do you want to save the paths relative or absolute?"));
         c_Message.SetHeading(C_GtGetText::h_GetText("Relative Path"));
         c_Details = C_GtGetText::h_GetText("Relative paths: ");
         c_Details += "\n   ";
         c_Details += c_PathsRelative.join("\n   ");
         c_Details += "\n";
         c_Details += C_GtGetText::h_GetText("Absolute paths: ");
         c_Details += "\n   ";
         c_Details += c_PathsAbsolute.join("\n   ");
         c_Message.SetDetails(c_Details);
         c_Message.SetOKButtonText(C_GtGetText::h_GetText("Relative"));
         c_Message.SetNOButtonText(C_GtGetText::h_GetText("Absolute"));
         c_Message.SetCustomMinHeight(230, 400);

         if (c_Message.Execute() == C_OgeWiCustomMessage::eOK)
         {
            c_Return = c_PathsRelative;
         }
         else
         {
            c_Return = c_PathsAbsolute;
         }
      }
   }
   else
   {
      // no paths provided - do nothing
   }

   return c_Return;
}
//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Assemble source file information for HTML report.

   Output is a HTML table which looks something like this:
   Path:         <orc_FilePath>
   Read Content: <orc_ReadContent>

   \param[in]  orc_FilePath      File path of source file
   \param[in]  orc_ReadContent   Summary of read content

   \return
   HTML table formatted string
*/
//----------------------------------------------------------------------------------------------------------------------
QString C_ImpUtil::h_FormatSourceFileInfoForReport(const QString & orc_FilePath, const QString & orc_ReadContent)
{
   QString c_Text = "";

   const QString c_HtmlTableDataStart =
      "<td align=\"left\" valign=\"top\" style=\"padding: 5px 18px 5px 0px;white-space:pre;\">";

   //File
   c_Text += "<h3>";
   c_Text += C_GtGetText::h_GetText("Source File Information");
   c_Text += "</h3>";
   c_Text += "<table><tr>";
   c_Text += c_HtmlTableDataStart;
   c_Text += C_GtGetText::h_GetText("Path:");
   c_Text += "</td>";
   c_Text += c_HtmlTableDataStart;
   c_Text += C_Uti::h_GetLink(orc_FilePath, mc_STYLE_GUIDE_COLOR_LINK, orc_FilePath);
   c_Text += "</td></tr><tr>";

   //Content
   c_Text += c_HtmlTableDataStart;
   c_Text += C_GtGetText::h_GetText("Read Content:");
   c_Text += "</td>";
   c_Text += c_HtmlTableDataStart;
   c_Text += orc_ReadContent;
   c_Text += "</td></tr></table>";

   return c_Text;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get active window handle if already existing

   \param[in]      orc_ExeName   Name of executable without path (e.g. LogiCAD3.exe)
   \param[in,out]  orc_Windows   All found windows
*/
//----------------------------------------------------------------------------------------------------------------------
void C_ImpUtil::mh_GetExistingApplicationHandle(const std::wstring & orc_ExeName, std::vector<HWND> & orc_Windows)
{
   PROCESSENTRY32 c_Entry;
   bool q_Exists = false;
   HANDLE pv_Snapshot;
   uint32 u32_ProcessID = 0;

   //Get process ID
   c_Entry.dwSize = sizeof(PROCESSENTRY32);

   pv_Snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);

   //lint -e{909} Windows library interface
   if (Process32First(pv_Snapshot, &c_Entry) != 0)
   {
      while (Process32Next(pv_Snapshot, &c_Entry) != 0)
      {
         if (std::wcscmp(c_Entry.szExeFile, orc_ExeName.c_str()) == 0) //lint !e64 //Windows library interface
         {
            u32_ProcessID = c_Entry.th32ProcessID;
            q_Exists = true;
         }
      }
   }

   CloseHandle(pv_Snapshot);

   if (q_Exists == true)
   {
      // get main window from process ID
      // https://stackoverflow.com/questions/221730/bat-file-to-run-a-exe-at-the-command-prompt
      C_ImpUtil::T_HandleData c_Data;
      c_Data.u32_ProcessId = u32_ProcessID;
      c_Data.pc_WindowHandle = NULL;
      //lint -e{9091} required by EnumWindows
      EnumWindows(&C_ImpUtil::mh_EnumWindowsCallback, reinterpret_cast<LPARAM>(&c_Data));
      orc_Windows.push_back(c_Data.pc_WindowHandle);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Help function to get window from process ID

   Adopted from here: https://stackoverflow.com/questions/221730/bat-file-to-run-a-exe-at-the-command-prompt

   \param[in]  opc_Handle     Handle
   \param[in]  os32_LParam    Parameter

   \return
   TRUE, FALSE
*/
//----------------------------------------------------------------------------------------------------------------------
//lint -e{8080} //using type expected by the library for compatibility
WINBOOL CALLBACK C_ImpUtil::mh_EnumWindowsCallback(HWND opc_Handle, const LPARAM os32_LParam)
{
   //lint -e{9010} //interface defined by Windows API
   T_HandleData & rc_Data = *reinterpret_cast<T_HandleData *>(os32_LParam);
   DWORD u32_ProcessId = 0;
   bool q_IsMainWindow = false;

   GetWindowThreadProcessId(opc_Handle, &u32_ProcessId);

   //lint -e{9010} //interface defined by Windows API
   if (GetWindow(opc_Handle, GW_OWNER) == reinterpret_cast<HWND>(NULL))
   {
      if (IsWindowVisible(opc_Handle) == true)
      {
         q_IsMainWindow = true;
      }
   }
   if ((rc_Data.u32_ProcessId != u32_ProcessId) || (q_IsMainWindow == false))
   {
      return TRUE;
   }
   rc_Data.pc_WindowHandle = opc_Handle;
   return FALSE;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Call external file generation tool.

   Standard file generator is osy_syde_coder_c.exe, but it is not used as default,
   so the openSYDE user must provide one.

   Assumption to non-default file-generator mygenerator.exe or mygenerator.bat:
   Behave like osy_syde_coder_c.exe, i.e.
      - same console parameters
      - same return values
      - create in directory of executable a file mygenerator_file_list.txt containing generated files

   The paths provided as arguments must be absolute.
   Path checks are done by QProcess or by executable.

   \param[in]   orc_NodeName        name of node to generate files for
   \param[in]   orc_AppName         name of application to generate files for
   \param[in]   orc_ExportFolder    path where exported files get saved at
   \param[out]  orc_ExportedFiles   Exported files
   \param[in]   orc_CodeGenerator   file generator executable or batch file
   \param[in]   orq_Erase           flag for file generator: true -> erase target folder and all sub-folders

   \return
   C_NO_ERR       everything worked
   C_UNKNOWN_ERR  unknown error occurred
   C_RD_WR        problems accessing file system
                  (could not read or write file generation .exe or system definition or file list)
   C_NOACT        Could not generate file for at least one application.
   C_CONFIG       Application or device for given node has wrong configuration
   C_TIMEOUT      Timeout for file generator call.
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_ImpUtil::mh_ExecuteCodeGenerator(const QString & orc_NodeName, const QString & orc_AppName,
                                          const QString & orc_ExportFolder, QStringList & orc_ExportedFiles,
                                          const QString & orc_CodeGenerator, const bool & orq_Erase)
{
   sint32 s32_Return = C_NO_ERR;
   QStringList c_Arguments;
   QProcess * const pc_Process = new QProcess(new QObject());
   QFile c_FileListFile;
   C_SCLString c_ErrorText;
   QString c_SysDefPath;

   // get system definition path (always absolute because pui project get path is always absolute)
   C_PuiProject::h_AdaptProjectPathToSystemDefinition(C_PuiProject::h_GetInstance()->GetPath(), c_SysDefPath);

   // build file list file path from executable (file which contains information which files were generated)
   const QFileInfo c_CodeGenFileInfo(orc_CodeGenerator);
   tgl_assert(c_CodeGenFileInfo.isAbsolute());
   c_FileListFile.setFileName(
      c_CodeGenFileInfo.absolutePath() + "/" + c_CodeGenFileInfo.completeBaseName() + "_file_list.txt");

   // provide arguments
   c_Arguments.push_back("-s"); // system definition
   c_Arguments.push_back(c_SysDefPath);
   c_Arguments.push_back("-d"); // devices ini file
   c_Arguments.push_back(C_Uti::h_GetAbsolutePathFromExe("../devices/devices.ini"));
   c_Arguments.push_back("-o"); // export folder
   c_Arguments.push_back(orc_ExportFolder);
   c_Arguments.push_back("-n"); // node name
   c_Arguments.push_back(orc_NodeName);
   c_Arguments.push_back("-a"); // application name
   c_Arguments.push_back(orc_AppName);
   if (orq_Erase == true)
   {
      c_Arguments.push_back("-e"); // erase folder (only if user confirmed)
   }

   // call file generation exe with arguments
   pc_Process->start(c_CodeGenFileInfo.absoluteFilePath(), c_Arguments);
   bool q_Tmp = pc_Process->waitForStarted();
   if (q_Tmp == true)
   {
      q_Tmp = pc_Process->waitForFinished(); // 30 seconds (default)
      if (q_Tmp == true)
      {
         const sintn sn_ProcessExitCode = pc_Process->exitCode();

         switch (sn_ProcessExitCode)
         {
         case 0: // eRESULT_OK
            // everything ok
            // read from file which files were generated
            if (c_FileListFile.open(QIODevice::ReadOnly | QIODevice::Text) == true)
            {
               // Use QTextstream to avoid problems with line feeds and backslashes
               QTextStream c_InputStream(&c_FileListFile);
               while (c_InputStream.atEnd() == false)
               {
                  const QString c_Line = c_InputStream.readLine();
                  orc_ExportedFiles.push_back(c_Line);
               }
               c_FileListFile.close();
            }
            else
            {
               osc_write_log_warning("Generate Files", static_cast<QString>("Could not open file list file: \"" +
                                                                            c_FileListFile.fileName() +
                                                                            "\"").toStdString().c_str());
               // no error code because generation worked, only result file was not found
            }
            break;
         case 11: // eRESULT_ERASE_FILE_LIST_ERROR
            c_ErrorText =
               "Could not remove pre-existing file list file of generator (lies next to file generator exe).";
            s32_Return = C_RD_WR;
            break;
         case 12: // eRESULT_WRITE_FILE_LIST_ERROR
            c_ErrorText =
               "Could not remove pre-existing file list file of generator (lies next to file generator exe).";
            s32_Return = C_RD_WR;
            break;
         case 13: // eRESULT_ERASE_TARGET_FOLDER_ERROR
            c_ErrorText =
               static_cast<QString>("Could not erase pre-existing target directory \"" + orc_ExportFolder +
                                    "\".").toStdString().c_str();
            s32_Return = C_RD_WR;
            break;
         case 14: // eRESULT_CREATE_TARGET_FOLDER_ERROR
            c_ErrorText =
               static_cast<QString>("Could not create target directory \"" + orc_ExportFolder +
                                    "\".").toStdString().c_str();
            s32_Return = C_RD_WR;
            break;
         case 20: // eRESULT_INVALID_CLI_PARAMETERS
            c_ErrorText =
               ("File generator could not parse command line arguments: " +
                c_Arguments.join(" ").toStdString()).c_str();
            s32_Return = C_CONFIG;
            break;
         case 30: // eRESULT_SYSTEM_DEFINITION_OPEN_ERROR
            c_ErrorText = "Could not open system definition file.";
            s32_Return = C_RD_WR;
            break;
         case 40: // eRESULT_CODE_GENERATION_ERROR
            c_ErrorText = "Could not generate files for at least one Data Block.";
            s32_Return = C_NOACT;
            break;
         case 41: // eRESULT_DEVICE_NOT_FOUND
            c_ErrorText = "Could not find device.";
            s32_Return = C_CONFIG;
            break;
         case 42: // eRESULT_DEVICE_NOT_COMPATIBLE
            c_ErrorText = "Device does not support openSYDE.";
            s32_Return = C_CONFIG;
            break;
         case 43: // eRESULT_APPLICATION_NOT_FOUND
            c_ErrorText = "Could not find Data Block.";
            s32_Return = C_CONFIG;
            break;
         case 44: // eRESULT_APPLICATION_NOT_PROGRAMMABLE
            c_ErrorText = "Data Block is not defined as programmable.";
            s32_Return = C_CONFIG;
            break;
         case 45: // eRESULT_APPLICATION_UNKNOWN_CODE_VERSION
            c_ErrorText = "Data Block has unknown file structure version.";
            s32_Return = C_CONFIG;
            break;
         case 9009: // error code if a batch script includes unrecognized commands (e.g. missing executable)
            c_ErrorText = "Batch script returned \"not recognized as an internal or external command\".";
            s32_Return = C_CONFIG;
            break;
         case 10: // eRESULT_HELPING --> we do not call help option, so why should this error occur? --> unknown
         default:
            c_ErrorText = "Unknown error occurred. Exit code: " + C_SCLString::IntToStr(sn_ProcessExitCode);
            s32_Return = C_UNKNOWN_ERR;
            break;
         }
         if (s32_Return != C_NO_ERR)
         {
            //clarify that this is a problem that was reported by the invoked file generator
            c_ErrorText = "File generator call reported problems: " + c_ErrorText;
         }
      }
   }

   // q_Tmp is false if start failed or if stop failed --> report process errors
   if (q_Tmp == false)
   {
      const QProcess::ProcessError e_ProcessError = pc_Process->error();
      switch (e_ProcessError)
      {
      case QProcess::FailedToStart:
         c_ErrorText =
            static_cast<QString>("Could not start file generator most likely due to insufficient permissions to "
                                 "invoke this program or the executable is missing: " +
                                 c_CodeGenFileInfo.absoluteFilePath()).toStdString().c_str();
         s32_Return = C_UNKNOWN_ERR;
         break;
      case QProcess::Crashed:
         c_ErrorText = "File generator crashed some time after starting successfully.";
         s32_Return = C_CONFIG;
         break;
      case QProcess::Timedout:
         c_ErrorText = "Timeout for file generator after 30 seconds.";
         s32_Return = C_TIMEOUT;
         break;
      case QProcess::ReadError:
         c_ErrorText = "Error when attempting to read from file generator process.";
         s32_Return = C_RD_WR;
         break;
      case QProcess::WriteError:
         c_ErrorText = "Error when attempting to write to the file generator process.";
         s32_Return = C_RD_WR;
         break;
      case QProcess::UnknownError:
      default:
         c_ErrorText = "Execution of file generator process not successful. Can't get any more information.";
         s32_Return = C_UNKNOWN_ERR;
         break;
      }
   }
   pc_Process->close();
   pc_Process->deleteLater();

   // write text to log
   if (s32_Return != C_NO_ERR)
   {
      osc_write_log_error("Generate Files", c_ErrorText);
   }

   return s32_Return; //lint !e429  //no memory leak for pc_Process because of the Qt memory management
}
