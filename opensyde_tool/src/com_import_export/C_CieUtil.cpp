//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Utility class with functions for comm import export (implementation)

   \copyright   Copyright 2018 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.h"

#include "stwtypes.h"
#include "stwerrors.h"
#include "C_CieUtil.h"

#include "TGLUtils.h"
#include "C_GtGetText.h"
#include "C_PuiProject.h"
#include "C_PuiSdHandler.h"
#include "C_OgePopUpDialog.h"
#include "C_CieImportDbc.h"
#include "C_UsHandler.h"
#include "C_OgeWiUtil.h"
#include "C_OgeWiCustomMessage.h"
#include "C_OSCLoggingHandler.h"
#include "C_CieImportReportWidget.h"
#include "C_CieDbcImportNodeAssignmentWidget.h"
#include "C_CieDcfEdsImportNodeSelectWidget.h"
#include "C_OgeWiCustomMessage.h"
#include "C_CieDataPoolListAdapter.h"
#include "C_OSCImportEdsDcf.h"
#include "C_OSCNode.h"
#include "C_CieExportDbc.h"
#include "C_CieExportReportWidget.h"
#include "constants.h"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw_types;
using namespace stw_errors;
using namespace stw_opensyde_core;
using namespace stw_opensyde_gui;
using namespace stw_opensyde_gui_logic;
using namespace stw_opensyde_gui_elements;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */
const QSize C_CieUtil::mhc_SIZE_REPORT(1210, 790);

/* -- Types --------------------------------------------------------------------------------------------------------- */

/* -- Global Variables ---------------------------------------------------------------------------------------------- */

/* -- Module Global Variables --------------------------------------------------------------------------------------- */

/* -- Module Global Function Prototypes ----------------------------------------------------------------------------- */

/* -- Implementation ------------------------------------------------------------------------------------------------ */

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default constructor
*/
//----------------------------------------------------------------------------------------------------------------------
C_CieUtil::C_CieUtil(void)
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Imports a specific file

   Supported are dbc, eds and dcf files.

   \param[in]     ou32_BusIndex           Index of bus in system definition
   \param[in]     oe_ProtocolType         Type of comm protocol
   \param[in]     opc_Parent              Parent for dialog
   \param[out]    orc_NodeIndexes         Node indexes that got messages
   \param[out]    orc_InterfaceIndexes    Interface indexes that got messages

   \return
   C_NO_ERR    File imported
   C_NOACT     Nothing done, aborted by user
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_CieUtil::h_ImportFile(const uint32 ou32_BusIndex, const C_OSCCanProtocol::E_Type oe_ProtocolType,
                               QWidget * const opc_Parent, std::vector<uint32> & orc_NodeIndexes,
                               std::vector<uint32> & orc_InterfaceIndexes)
{
   //Load user settings value
   QString c_Folder = C_UsHandler::h_GetInstance()->GetProjSdTopologyLastKnownImportPath();
   // open file selector with DBC filter
   const QString c_Filter = QString(C_GtGetText::h_GetText("COMM Messages Files")) +
                            " (*.dbc *.eds *.dcf)";
   sint32 s32_Return = C_NOACT;

   //Replace default path if necessary
   if (c_Folder.compare("") == 0)
   {
      c_Folder = C_PuiProject::h_GetInstance()->GetFolderPath();
   }

   const QString c_FullFilePath =
      C_OgeWiUtil::h_GetOpenFileName(opc_Parent, C_GtGetText::h_GetText("Select COMM Messages File"),
                                     c_Folder, c_Filter, "*.dbc");

   // check for user abort (empty string)
   if (c_FullFilePath != "")
   {
      const QFileInfo c_FileInfo(c_FullFilePath);
      //Only use lower case suffix
      const QString c_Extension = c_FileInfo.completeSuffix().toLower();
      //Store new user settings value
      C_UsHandler::h_GetInstance()->SetProjSdTopologyLastKnownImportPath(c_FileInfo.absoluteDir().absolutePath());
      if (c_Extension.contains("dbc") == true)
      {
         s32_Return = C_CieUtil::mh_ImportDBCFile(ou32_BusIndex, oe_ProtocolType, c_FullFilePath, opc_Parent,
                                                  orc_NodeIndexes, orc_InterfaceIndexes);
      }
      else if ((c_Extension.contains("eds") == true) || (c_Extension.contains("dcf") == true))
      {
         s32_Return = C_CieUtil::mh_ImportDCFEDSFile(ou32_BusIndex, oe_ProtocolType, c_FullFilePath, opc_Parent,
                                                     orc_NodeIndexes, orc_InterfaceIndexes);
      }
      else
      {
         // strange
         C_OgeWiCustomMessage c_Message(opc_Parent, C_OgeWiCustomMessage::E_Type::eERROR);
         c_Message.SetHeading(C_GtGetText::h_GetText("Import from File"));
         c_Message.SetDescription(C_GtGetText::h_GetText("File type not allowed."));
         c_Message.SetCustomMinHeight(180, 180);
         c_Message.Execute();
      }
   }

   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Exports network with active nodes in a specific file.

   Only DBC files are supported yet.

   \param[in]  orc_CommDef           Complete network definition to export.
   \param[in]  opc_Parent            Parent for dialogs.
   \param[in]  ou32_NumOfNodes       number of input nodes for x-check.
   \param[in]  ou32_NumOfMessages    number of input messages for x-check.
   \param[in]  ou32_NumOfSignals     number of input signals for x-check.

   \return
   C_NO_ERR    Network to file exported
   C_NOACT     Nothing done, aborted by user
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_CieUtil::h_ExportFile(const stw_opensyde_gui_logic::C_CieConverter::C_CIECommDefinition & orc_CommDef,
                               QWidget * const opc_Parent, const uint32 ou32_NumOfNodes,
                               const uint32 ou32_NumOfMessages, const uint32 ou32_NumOfSignals)
{
   //Load user settings value
   QString c_Folder = C_UsHandler::h_GetInstance()->GetProjSdTopologyLastKnownExportPath();
   const QString c_FilterName = QString(C_GtGetText::h_GetText("openSYDE DBC export")) + " (*.dbc)";
   const QString c_DefaultFilename = QString(orc_CommDef.c_Bus.c_Name.c_str()) + ".dbc";
   QString c_FullFilePath;
   sint32 s32_Return = C_NOACT;

   //Replace default path if necessary
   if (c_Folder.compare("") == 0)
   {
      c_Folder = C_PuiProject::h_GetInstance()->GetFolderPath();
   }

   //Get file name
   c_FullFilePath =
      C_OgeWiUtil::h_GetSaveFileName(opc_Parent, C_GtGetText::h_GetText("Select File for openSYDE DBC Export"),
                                     c_Folder, c_FilterName, c_DefaultFilename);

   //Check for user abort (empty string)
   if (c_FullFilePath.compare("") != 0)
   {
      const QFileInfo c_FileInfo(c_FullFilePath);
      //Only use lower case suffix
      const QString c_Extension = c_FileInfo.completeSuffix().toLower();
      //Store new user settings value
      C_UsHandler::h_GetInstance()->SetProjSdTopologyLastKnownExportPath(c_FileInfo.absoluteDir().absolutePath());
      if (c_Extension == "dbc")
      {
         stw_scl::C_SCLString c_Error;
         stw_scl::C_SCLStringList c_Warnings;

         // import network of DBC file
         QApplication::setOverrideCursor(Qt::WaitCursor); // big DBC file can take some time to load
         QApplication::processEvents();                   // update cursor
         s32_Return = C_CieExportDbc::h_ExportNetwork(c_FullFilePath.toStdString().c_str(), orc_CommDef,
                                                      c_Warnings, c_Error);
         QApplication::restoreOverrideCursor(); // get old cursor again
         QApplication::processEvents();         // update cursor

         if ((s32_Return == C_NO_ERR) || (s32_Return == C_WARN))
         {
            std::map<stw_scl::C_SCLString, stw_scl::C_SCLString> c_NodeMapping;
            sint32 s32_Tmp = C_CieExportDbc::h_GetNodeMapping(c_NodeMapping);

            tgl_assert(s32_Tmp == C_NO_ERR);
            if (s32_Tmp == C_NO_ERR)
            {
               uint32 u32_NumOfOutputNodes = static_cast<uint32>(c_NodeMapping.size());

               C_CieExportDbc::C_ExportStatistic c_ExportStatistic;
               s32_Tmp = C_CieExportDbc::h_GetExportStatistic(c_ExportStatistic);
               tgl_assert(s32_Tmp == C_NO_ERR);

               if (s32_Tmp == C_NO_ERR)
               {
                  uint32 u32_NumOfOutputMessages = c_ExportStatistic.u32_NumOfMessages;
                  uint32 u32_NumOfOutputSignals = c_ExportStatistic.u32_NumOfSignals;

                  // check number of input and output items
                  if ((ou32_NumOfNodes != u32_NumOfOutputNodes) ||
                      (ou32_NumOfMessages != u32_NumOfOutputMessages) ||
                      (ou32_NumOfSignals != u32_NumOfOutputSignals))
                  {
                     // build up additional warnings (there can be also one from export itself)
                     if (ou32_NumOfNodes != u32_NumOfOutputNodes)
                     {
                        c_Warnings.Add("Number of input nodes (" + stw_scl::C_SCLString(ou32_NumOfNodes) +
                                       ") does not match number of exported nodes (" +
                                       stw_scl::C_SCLString(u32_NumOfOutputNodes) + ").");
                     }
                     if (ou32_NumOfMessages != u32_NumOfOutputMessages)
                     {
                        c_Warnings.Add("Number of input messages (" + stw_scl::C_SCLString(ou32_NumOfMessages) +
                                       ") does not match number of exported messages (" +
                                       stw_scl::C_SCLString(u32_NumOfOutputMessages) + ").");
                     }
                     if (ou32_NumOfSignals != u32_NumOfOutputSignals)
                     {
                        c_Warnings.Add("Number of input signals (" + stw_scl::C_SCLString(ou32_NumOfSignals) +
                                       ") does not match number of exported signals (" +
                                       stw_scl::C_SCLString(u32_NumOfOutputSignals) + ").");
                     }
                  }
               }

               // show export report

               // create message report for user
               QPointer<C_OgePopUpDialog> const c_PopUpDialogReportDialog =
                  new C_OgePopUpDialog(opc_Parent, opc_Parent);
               C_CieExportReportWidget * const pc_DialogExportReport =
                  new C_CieExportReportWidget(*c_PopUpDialogReportDialog, c_FullFilePath);

               pc_DialogExportReport->SetMessageData(c_NodeMapping, c_ExportStatistic, c_Warnings);

               // resize
               c_PopUpDialogReportDialog->SetSize(mhc_SIZE_REPORT);

               // display message report
               if (c_PopUpDialogReportDialog->exec() == static_cast<sintn>(QDialog::Accepted))
               {
                  s32_Return = C_NO_ERR;
               }
               if (c_PopUpDialogReportDialog != NULL)
               {
                  c_PopUpDialogReportDialog->HideOverlay();
               }
               //lint -e{429}  no memory leak because of the parent of pc_Dialog and the Qt memory management
            }
         }
         else
         {
            // display error
            C_OgeWiCustomMessage c_ExportError(opc_Parent, C_OgeWiCustomMessage::E_Type::eERROR);
            c_ExportError.SetHeading(C_GtGetText::h_GetText("DBC file export"));
            c_ExportError.SetDescription(C_GtGetText::h_GetText("There occurred an error on DBC file export."));
            c_ExportError.SetDetails(c_Error.c_str());
            c_ExportError.SetCustomMinHeight(180, 250);
            c_ExportError.Execute();
         }
      }
      else
      {
         C_OgeWiCustomMessage c_Message(opc_Parent, C_OgeWiCustomMessage::E_Type::eERROR);
         c_Message.SetDescription(C_GtGetText::h_GetText("The specified file has the wrong extension, use: \".dbc\"."));
         c_Message.SetDetails(QString("Invalid extension: \"%1\"").arg("." + c_Extension));
         c_Message.SetCustomMinHeight(180, 250);
         c_Message.Execute();
      }
   }
   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Import a DBC file

   File path is assumed to be absolute and non empty.

   \param[in]     ou32_BusIndex           Index of bus in system definition
   \param[in]     oe_ProtocolType         Type of comm protocol
   \param[in]     orc_FullFilePath        Absolute file path to dbc file
   \param[in]     opc_Parent              Parent for dialog
   \param[out]    orc_NodeIndexes         Node indexes that got messages
   \param[out]    orc_InterfaceIndexes    Interface indexes that got messages

   \return
   C_NO_ERR    File imported
   C_NOACT     Nothing done, aborted by user
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_CieUtil::mh_ImportDBCFile(const uint32 ou32_BusIndex, const C_OSCCanProtocol::E_Type oe_ProtocolType,
                                   const QString & orc_FullFilePath, QWidget * const opc_Parent,
                                   std::vector<uint32> & orc_NodeIndexes, std::vector<uint32> & orc_InterfaceIndexes)
{
   sint32 s32_Return = C_NOACT;
   sint32 s32_ImportReturn;

   C_CieConverter::C_CIECommDefinition c_CommDef;
   stw_scl::C_SCLStringList c_WarningMessages;
   stw_scl::C_SCLString c_ErrorMessage;

   // import network of DBC file
   QApplication::setOverrideCursor(Qt::WaitCursor); // big DBC file can take some time to load
   QApplication::processEvents();                   // update cursor
   s32_ImportReturn = C_CieImportDbc::h_ImportNetwork(orc_FullFilePath.toStdString().c_str(),
                                                      c_CommDef, c_WarningMessages, c_ErrorMessage, true);
   QApplication::restoreOverrideCursor(); // get old cursor again
   QApplication::processEvents();         // update cursor

   if ((s32_ImportReturn == stw_errors::C_NO_ERR) || (s32_ImportReturn == stw_errors::C_WARN))
   {
      if ((c_CommDef.c_Nodes.size() == 0UL) && (c_CommDef.c_UnmappedMessages.size() == 0UL))

      {
         C_OgeWiCustomMessage c_NodesError(opc_Parent, C_OgeWiCustomMessage::eERROR);
         c_NodesError.SetHeading(C_GtGetText::h_GetText("DBC file import"));
         c_NodesError.SetDescription(C_GtGetText::h_GetText("No nodes or messages found in selected DBC file."));
         c_NodesError.SetCustomMinHeight(180, 180);
         c_NodesError.Execute();
      }
      else
      {
         if ((s32_ImportReturn == stw_errors::C_WARN) && (c_WarningMessages.GetCount() > 0))
         {
            // display global warning messages
            QString c_Warnings;
            for (uint32 u32_Pos = 0; u32_Pos < c_WarningMessages.GetCount(); u32_Pos++)
            {
               c_Warnings += c_WarningMessages.Strings[u32_Pos].c_str();
               c_Warnings += "\n";
            }
            C_OgeWiCustomMessage c_ImportWarnings(opc_Parent, C_OgeWiCustomMessage::E_Type::eWARNING);
            c_ImportWarnings.SetHeading(C_GtGetText::h_GetText("DBC file import"));
            c_ImportWarnings.SetDescription(C_GtGetText::h_GetText("Warnings occurred during DBC file import."));
            c_ImportWarnings.SetDetails(c_Warnings);
            c_ImportWarnings.SetCustomMinWidth(750);
            c_ImportWarnings.SetCustomMinHeight(180, 400);
            c_ImportWarnings.Execute();
         }

         // create node assignment popup
         QPointer<C_OgePopUpDialog> const c_PopUpDialogNodeAssignment =
            new C_OgePopUpDialog(opc_Parent, opc_Parent);
         C_CieDbcImportNodeAssignmentWidget * const pc_DialogNodeSelection =
            new C_CieDbcImportNodeAssignmentWidget(*c_PopUpDialogNodeAssignment, orc_FullFilePath, ou32_BusIndex,
                                                   c_CommDef);

         c_PopUpDialogNodeAssignment->SetSize(QSize(740, 770));

         // display node assignment popup
         if (c_PopUpDialogNodeAssignment->exec() == static_cast<sintn>(QDialog::Accepted))
         {
            const std::vector<C_CieDbcOsyNodeAssignment> c_DbcNodeAssignment =
               pc_DialogNodeSelection->GetNodeAssignments();
            std::vector<C_CieDbcOsyNodeAssignment>::const_iterator c_It;
            std::vector<C_CieImportDataAssignment> c_NodeAssignmentsConverted;

            for (c_It = c_DbcNodeAssignment.begin(); c_It != c_DbcNodeAssignment.end(); ++c_It)
            {
               const C_CieDbcOsyNodeAssignment & rc_CurrentAssignment = *c_It;

               // get openSYDE Ui data pool list structure
               C_CieImportDataAssignment c_NodeAssignmentConverted;
               c_NodeAssignmentConverted.c_ImportData =
                  C_CieDataPoolListAdapter::h_GetStructureFromDBCFileImport(rc_CurrentAssignment.c_CieNode);
               if ((rc_CurrentAssignment.s32_AssignedOsyNodeIndex >= 0) &&
                   (rc_CurrentAssignment.s32_AssignedOsyInterfaceIndex >= 0))
               {
                  c_NodeAssignmentConverted.u32_OsyNodeIndex =
                     static_cast<uint32>(rc_CurrentAssignment.s32_AssignedOsyNodeIndex);
                  c_NodeAssignmentConverted.u32_OsyInterfaceIndex =
                     static_cast<uint32>(rc_CurrentAssignment.s32_AssignedOsyInterfaceIndex);

                  // add to output to forward information about which nodes should get active
                  orc_NodeIndexes.push_back(c_NodeAssignmentConverted.u32_OsyNodeIndex);
                  orc_InterfaceIndexes.push_back(c_NodeAssignmentConverted.u32_OsyInterfaceIndex);
               }
               else
               {
                  tgl_assert(false);
               }

               c_NodeAssignmentsConverted.push_back(c_NodeAssignmentConverted);
            }

            // create message report for user
            QPointer<C_OgePopUpDialog> const c_PopUpDialogReportDialog =
               new C_OgePopUpDialog(opc_Parent, opc_Parent);
            C_CieImportReportWidget * const pc_DialogImportReport =
               new C_CieImportReportWidget(*c_PopUpDialogReportDialog, orc_FullFilePath, ou32_BusIndex,
                                           oe_ProtocolType, c_NodeAssignmentsConverted);

            Q_UNUSED(pc_DialogImportReport);

            //Hide previous overlay before showing the next one
            c_PopUpDialogNodeAssignment->HideOverlay();

            // resize
            c_PopUpDialogReportDialog->SetSize(mhc_SIZE_REPORT);

            // display message report
            if (c_PopUpDialogReportDialog->exec() == static_cast<sintn>(QDialog::Accepted))
            {
               s32_Return = C_NO_ERR;
            }

            if (c_PopUpDialogReportDialog != NULL)
            {
               c_PopUpDialogReportDialog->HideOverlay();
            }
            //lint -e{429}  no memory leak because of the parent of pc_Dialog and the Qt memory management
         }
         if (c_PopUpDialogNodeAssignment != NULL)
         {
            c_PopUpDialogNodeAssignment->HideOverlay();
         }
         //lint -e{429}  no memory leak because of the parent of pc_Dialog and the Qt memory management
      }
   }
   else
   {
      // display error message to user
      QString c_ErrorMsg =  c_ErrorMessage.c_str();
      C_OgeWiCustomMessage c_MessageResult(opc_Parent, C_OgeWiCustomMessage::E_Type::eERROR);
      c_MessageResult.SetHeading(C_GtGetText::h_GetText("DBC file import"));
      c_MessageResult.SetDescription(C_GtGetText::h_GetText("DBC file import error occurred."));
      c_MessageResult.SetDetails(C_GtGetText::h_GetText("Error code: \n") +
                                 QString::number(s32_ImportReturn) +
                                 C_GtGetText::h_GetText("\nError message(s):\n") +
                                 c_ErrorMsg);
      c_MessageResult.SetCustomMinHeight(180, 300);
      c_MessageResult.Execute();
   }

   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Import a DCF or EDS file

   File path is assumed to be absolute and non empty.

   \param[in]     ou32_BusIndex           Index of bus in system definition
   \param[in]     oe_ProtocolType         Type of comm protocol
   \param[in]     orc_FullFilePath        Absolute file path to dbc file
   \param[in]     opc_Parent              Parent for dialog
   \param[out]    orc_NodeIndexes         Node indexes that got messages
   \param[out]    orc_InterfaceIndexes    Interface indexes that got messages

   \return
   C_NO_ERR    File imported
   C_NOACT     Nothing done, aborted by user
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_CieUtil::mh_ImportDCFEDSFile(const uint32 ou32_BusIndex, const C_OSCCanProtocol::E_Type oe_ProtocolType,
                                      const QString & orc_FullFilePath, QWidget * const opc_Parent,
                                      std::vector<uint32> & orc_NodeIndexes, std::vector<uint32> & orc_InterfaceIndexes)
{
   sint32 s32_Return = C_NOACT;

   // create node selection popup
   QPointer<C_OgePopUpDialog> const c_PopUpDialog = new C_OgePopUpDialog(opc_Parent, opc_Parent);
   C_CieDcfEdsImportNodeSelectWidget * const pc_DialogNodeSelection =
      new C_CieDcfEdsImportNodeSelectWidget(*c_PopUpDialog, orc_FullFilePath, ou32_BusIndex);

   Q_UNUSED(pc_DialogNodeSelection);

   if (c_PopUpDialog->exec() == static_cast<sintn>(QDialog::Accepted))
   {
      // Let user select node and interface
      uint32 u32_NodeIndex = 0;
      uint32 u32_InterfaceIndex = 0;
      tgl_assert(pc_DialogNodeSelection->GetNodeSelection(u32_NodeIndex, u32_InterfaceIndex) == C_NO_ERR);
      const C_OSCNode * const pc_Node = C_PuiSdHandler::h_GetInstance()->GetOSCNodeConst(u32_NodeIndex);

      // add to output to forward information about which nodes should get active
      orc_NodeIndexes.push_back(u32_NodeIndex);
      orc_InterfaceIndexes.push_back(u32_InterfaceIndex);

      tgl_assert(pc_Node != NULL);
      if (pc_Node != NULL)
      {
         tgl_assert(u32_InterfaceIndex < pc_Node->c_Properties.c_ComInterfaces.size());
         if (u32_InterfaceIndex < pc_Node->c_Properties.c_ComInterfaces.size())
         {
            const C_OSCNodeComInterfaceSettings & rc_CurInterface =
               pc_Node->c_Properties.c_ComInterfaces[u32_InterfaceIndex];
            stw_scl::C_SCLString c_ParsingError;
            std::vector<C_OSCCanMessage> c_OSCRxMessageData;
            std::vector<C_OSCNodeDataPoolListElement> c_OSCRxSignalData;
            std::vector<C_OSCCanMessage> c_OSCTxMessageData;
            std::vector<C_OSCNodeDataPoolListElement> c_OSCTxSignalData;
            std::vector<std::vector<stw_scl::C_SCLString> > c_ImportMessagesPerMessage;
            const sint32 s32_ImportResult = C_OSCImportEdsDcf::h_Import(
               orc_FullFilePath.toStdString().c_str(), rc_CurInterface.u8_NodeID, c_OSCRxMessageData,
               c_OSCRxSignalData, c_OSCTxMessageData, c_OSCTxSignalData, c_ImportMessagesPerMessage,
               c_ParsingError);
            if (s32_ImportResult == C_NO_ERR)
            {
               C_CieImportDataAssignment c_NodeAssignment;
               c_NodeAssignment.u32_OsyNodeIndex = u32_NodeIndex;
               c_NodeAssignment.u32_OsyInterfaceIndex = u32_InterfaceIndex;
               c_NodeAssignment.c_ImportData =
                  C_CieDataPoolListAdapter::h_GetStructureFromDCFAndEDSFileImport(c_OSCRxMessageData,
                                                                                  c_OSCRxSignalData,
                                                                                  c_OSCTxMessageData,
                                                                                  c_OSCTxSignalData,
                                                                                  c_ImportMessagesPerMessage);
               if ((c_NodeAssignment.c_ImportData.c_Core.c_OSCRxMessageData.size() > 0UL) ||
                   (c_NodeAssignment.c_ImportData.c_Core.c_OSCTxMessageData.size() > 0UL))
               {
                  std::vector<C_CieImportDataAssignment> c_NodeAssignmentVector;
                  c_NodeAssignmentVector.push_back(c_NodeAssignment);

                  QPointer<C_OgePopUpDialog> const c_New = new C_OgePopUpDialog(opc_Parent, opc_Parent);
                  C_CieImportReportWidget * const pc_Dialog =
                     new C_CieImportReportWidget(*c_New, orc_FullFilePath, ou32_BusIndex, oe_ProtocolType,
                                                 c_NodeAssignmentVector);

                  Q_UNUSED(pc_Dialog);

                  //Resize
                  c_New->SetSize(mhc_SIZE_REPORT);

                  if (c_New->exec() == static_cast<sintn>(QDialog::Accepted))
                  {
                     s32_Return = C_NO_ERR;
                  }

                  if (c_New != NULL)
                  {
                     c_New->HideOverlay();
                  }
                  //lint -e{429}  no memory leak because of the parent of pc_Dialog and the Qt memory management
               }
               else
               {
                  C_OgeWiCustomMessage c_Message(opc_Parent);
                  c_Message.SetHeading(C_GtGetText::h_GetText("Nothing found"));
                  c_Message.SetDescription(C_GtGetText::h_GetText("No messages found in file."));
                  c_Message.SetCustomMinHeight(180, 180);
                  c_Message.Execute();
               }
            }
            else
            {
               C_OgeWiCustomMessage c_Message(opc_Parent, C_OgeWiCustomMessage::E_Type::eERROR);
               c_Message.SetHeading(C_GtGetText::h_GetText("Import from file"));
               switch (s32_ImportResult)
               {
               case C_RANGE:
                  c_Message.SetDescription(C_GtGetText::h_GetText("Invalid parameter."));
                  c_Message.SetDetails(QString(c_ParsingError.c_str()));
                  c_Message.SetCustomMinHeight(180, 300);
                  break;
               case C_NOACT:
                  c_Message.SetDescription(QString(C_GtGetText::h_GetText(
                                                      "EDS file import failed.\nNode ID %1 is invalid.")).arg(
                                              rc_CurInterface.u8_NodeID));
                  c_Message.SetDetails(C_GtGetText::h_GetText("CANopen standard only supports node IDs in the range "
                                                              "of 1 to 127.\nThe node ID can be changed in node "
                                                              "properties."));
                  c_Message.SetCustomMinHeight(180, 270);
                  break;
               case C_CONFIG:
                  c_Message.SetDescription(C_GtGetText::h_GetText("An error occured while parsing."));
                  //Update log file
                  C_OSCLoggingHandler::h_Flush();
                  c_Message.SetDetails(QString("%1<a href=\"file:%2\"><span style=\"color: %3;\">%4</span></a>.").
                                       arg(C_GtGetText::h_GetText("For more information see ")).
                                       arg(C_OSCLoggingHandler::h_GetCompleteLogFileLocation().c_str()).
                                       arg(mc_STYLESHEET_GUIDE_COLOR_LINK).
                                       arg(C_GtGetText::h_GetText("log file")));
                  c_Message.SetCustomMinHeight(180, 250);
                  break;
               default:
                  c_Message.SetDescription(C_GtGetText::h_GetText("Unknown reason."));
                  c_Message.SetCustomMinHeight(180, 180);
                  break;
               }
               c_Message.Execute();
            }
         }
      }
   }
   return s32_Return;
}
