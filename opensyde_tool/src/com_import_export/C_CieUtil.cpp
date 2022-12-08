//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Utility class with functions for comm import export (implementation)

   \copyright   Copyright 2018 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.hpp"

#include <QApplication>

#include "stwtypes.hpp"
#include "stwerrors.hpp"
#include "C_CieUtil.hpp"

#include "TglUtils.hpp"
#include "C_SdUtil.hpp"
#include "C_PuiUtil.hpp"
#include "C_GtGetText.hpp"
#include "C_PuiProject.hpp"
#include "C_PuiSdHandler.hpp"
#include "C_OgePopUpDialog.hpp"
#include "C_CieImportDbc.hpp"
#include "C_UsHandler.hpp"
#include "C_OgeWiUtil.hpp"
#include "C_OgeWiCustomMessage.hpp"
#include "C_OscLoggingHandler.hpp"
#include "C_CieImportReportWidget.hpp"
#include "C_PuiSdNodeCanMessageSyncManager.hpp"
#include "C_CieDbcImportNodeAssignmentWidget.hpp"
#include "C_CieDcfEdsImportNodeSelectWidget.hpp"
#include "C_OgeWiCustomMessage.hpp"
#include "C_CieDataPoolListAdapter.hpp"
#include "C_OscImportEdsDcf.hpp"
#include "C_OscNode.hpp"
#include "C_CieExportDbc.hpp"
#include "C_CieExportReportWidget.hpp"
#include "constants.hpp"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw::errors;
using namespace stw::opensyde_core;
using namespace stw::opensyde_gui;
using namespace stw::opensyde_gui_logic;
using namespace stw::opensyde_gui_elements;
using namespace stw::scl;

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

   \param[in]   ou32_BusIndex          Index of bus in system definition
   \param[in]   oe_ProtocolType        Type of comm protocol
   \param[in]   opc_Parent             Parent for dialog
   \param[out]  orc_NodeIndexes        Node indexes that got messages
   \param[out]  orc_InterfaceIndexes   Interface indexes that got messages

   \return
   C_NO_ERR    File imported
   C_NOACT     Nothing done, aborted by user
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_CieUtil::h_ImportFile(const uint32_t ou32_BusIndex, const C_OscCanProtocol::E_Type oe_ProtocolType,
                                QWidget * const opc_Parent, std::vector<uint32_t> & orc_NodeIndexes,
                                std::vector<uint32_t> & orc_InterfaceIndexes)
{
   //Load user settings value
   QString c_Folder = C_UsHandler::h_GetInstance()->GetProjSdTopologyLastKnownImportPath();
   // open file selector with DBC filter
   const QString c_Filter = static_cast<QString>(C_GtGetText::h_GetText("COMM Messages Files")) +
                            " (*.dbc *.eds *.dcf)";
   int32_t s32_Return = C_NOACT;

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
         s32_Return = C_CieUtil::mh_ImportDbcFile(ou32_BusIndex, oe_ProtocolType, c_FullFilePath, opc_Parent,
                                                  orc_NodeIndexes, orc_InterfaceIndexes);
      }
      else if ((c_Extension.contains("eds") == true) || (c_Extension.contains("dcf") == true))
      {
         s32_Return = C_CieUtil::mh_ImportDcfEdsFile(ou32_BusIndex, oe_ProtocolType, c_FullFilePath, opc_Parent,
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

   \param[in]  orc_CommDef          Complete network definition to export.
   \param[in]  opc_Parent           Parent for dialogs.
   \param[in]  ou32_NumOfNodes      number of input nodes for x-check.
   \param[in]  ou32_NumOfMessages   number of input messages for x-check.
   \param[in]  ou32_NumOfSignals    number of input signals for x-check.

   \return
   C_NO_ERR    Network to file exported
   C_NOACT     Nothing done, aborted by user
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_CieUtil::h_ExportFile(const stw::opensyde_gui_logic::C_CieConverter::C_CieCommDefinition & orc_CommDef,
                                QWidget * const opc_Parent, const uint32_t ou32_NumOfNodes,
                                const uint32_t ou32_NumOfMessages, const uint32_t ou32_NumOfSignals)
{
   //Load user settings value
   QString c_Folder = C_UsHandler::h_GetInstance()->GetProjSdTopologyLastKnownExportPath();
   const QString c_FilterName = static_cast<QString>(C_GtGetText::h_GetText("openSYDE DBC export")) + " (*.dbc)";
   const QString c_DefaultFilename = static_cast<QString>(orc_CommDef.c_Bus.c_Name.c_str()) + ".dbc";
   QString c_FullFilePath;
   int32_t s32_Return = C_NOACT;

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
         C_SclString c_Error;
         C_SclStringList c_Warnings;

         // import network of DBC file
         QApplication::setOverrideCursor(Qt::WaitCursor); // big DBC file can take some time to load
         QApplication::processEvents();                   // update cursor
         s32_Return = C_CieExportDbc::h_ExportNetwork(c_FullFilePath.toStdString().c_str(), orc_CommDef,
                                                      c_Warnings, c_Error);
         QApplication::restoreOverrideCursor(); // get old cursor again
         QApplication::processEvents();         // update cursor

         if ((s32_Return == C_NO_ERR) || (s32_Return == C_WARN))
         {
            std::map<C_SclString, C_SclString> c_NodeMapping;
            int32_t s32_Tmp = C_CieExportDbc::h_GetNodeMapping(c_NodeMapping);

            tgl_assert(s32_Tmp == C_NO_ERR);
            if (s32_Tmp == C_NO_ERR)
            {
               const uint32_t u32_NumOfOutputNodes = static_cast<uint32_t>(c_NodeMapping.size());

               C_CieExportDbc::C_ExportStatistic c_ExportStatistic;
               s32_Tmp = C_CieExportDbc::h_GetExportStatistic(c_ExportStatistic);
               tgl_assert(s32_Tmp == C_NO_ERR);

               if (s32_Tmp == C_NO_ERR)
               {
                  const uint32_t u32_NumOfOutputMessages = c_ExportStatistic.u32_NumOfMessages;
                  const uint32_t u32_NumOfOutputSignals = c_ExportStatistic.u32_NumOfSignals;

                  // check number of input and output items
                  if ((ou32_NumOfNodes != u32_NumOfOutputNodes) ||
                      (ou32_NumOfMessages != u32_NumOfOutputMessages) ||
                      (ou32_NumOfSignals != u32_NumOfOutputSignals))
                  {
                     // build up additional warnings (there can be also one from export itself)
                     if (ou32_NumOfNodes != u32_NumOfOutputNodes)
                     {
                        c_Warnings.Add("Number of input nodes (" + C_SclString::IntToStr(ou32_NumOfNodes) +
                                       ") does not match number of exported nodes (" +
                                       C_SclString::IntToStr(u32_NumOfOutputNodes) + ").");
                     }
                     if (ou32_NumOfMessages != u32_NumOfOutputMessages)
                     {
                        c_Warnings.Add("Number of input messages (" + C_SclString::IntToStr(
                                          ou32_NumOfMessages) +
                                       ") does not match number of exported messages (" +
                                       C_SclString::IntToStr(u32_NumOfOutputMessages) + ").");
                     }
                     if (ou32_NumOfSignals != u32_NumOfOutputSignals)
                     {
                        c_Warnings.Add("Number of input signals (" + C_SclString::IntToStr(ou32_NumOfSignals) +
                                       ") does not match number of exported signals (" +
                                       C_SclString::IntToStr(u32_NumOfOutputSignals) + ").");
                     }
                  }
               }

               // show export report

               // create message report for user
               const QPointer<C_OgePopUpDialog> c_PopUpDialogReportDialog =
                  new C_OgePopUpDialog(opc_Parent, opc_Parent);
               C_CieExportReportWidget * const pc_DialogExportReport =
                  new C_CieExportReportWidget(*c_PopUpDialogReportDialog, c_FullFilePath);

               pc_DialogExportReport->SetMessageData(c_NodeMapping, c_ExportStatistic, c_Warnings);

               // resize
               c_PopUpDialogReportDialog->SetSize(mhc_SIZE_REPORT);

               // display message report
               if (c_PopUpDialogReportDialog->exec() == static_cast<int32_t>(QDialog::Accepted))
               {
                  s32_Return = C_NO_ERR;
               }
               if (c_PopUpDialogReportDialog != NULL)
               {
                  c_PopUpDialogReportDialog->HideOverlay();
                  c_PopUpDialogReportDialog->deleteLater();
               }
            } //lint !e429  //no memory leak because of the parent of pc_Dialog and the Qt memory management
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
         c_Message.SetDetails(static_cast<QString>("Invalid extension: \"%1\"").arg("." + c_Extension));
         c_Message.SetCustomMinHeight(180, 250);
         c_Message.Execute();
      }
   }
   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Adapt name as specified

   * Eliminate spaces
   * Cut after 31 characters

   \param[in,out]  orc_Name      Old/New name
   \param[in,out]  orc_Comment   Comment
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CieUtil::h_AdaptName(stw::scl::C_SclString & orc_Name, stw::scl::C_SclString & orc_Comment)
{
   stw::scl::C_SclString c_NewName;
   //Remove spaces
   for (uint32_t u32_ItOldName = 0; u32_ItOldName < orc_Name.Length(); ++u32_ItOldName)
   {
      if (orc_Name[static_cast<int32_t>(u32_ItOldName + 1UL)] == ' ')
      {
         //Skip
      }
      else
      {
         c_NewName += orc_Name[static_cast<int32_t>(u32_ItOldName + 1UL)];
         //Cut string
         if (c_NewName.Length() >= ms32_C_ITEM_MAX_CHAR_COUNT)
         {
            break;
         }
      }
   }
   if (orc_Name == c_NewName)
   {
      //No change
   }
   else
   {
      stw::scl::C_SclString c_Addition;
      if (orc_Comment != "")
      {
         c_Addition = "\n";
      }
      //Set comment before overwriting the original name
      //Translation: 1=Original name from file,2=Optional line break if there was some more content
      orc_Comment +=
         static_cast<QString>(C_GtGetText::h_GetText("%2Original name (from import source): %1")).arg(orc_Name.c_str()).
         arg(c_Addition.c_str()).toStdString().c_str();
      orc_Name = c_NewName;
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Adapt import messages

   \param[in,out]  orc_ImportDataAssignment  Import data assignment
   \param[in]      oe_ProtocolType           Protocol type
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CieUtil::h_AdaptImportMessages(std::vector<C_CieImportDataAssignment> & orc_ImportDataAssignment,
                                      const C_OscCanProtocol::E_Type oe_ProtocolType)
{
   mh_AdaptMessagesToProtocolType(orc_ImportDataAssignment, oe_ProtocolType);
   mh_AdaptMessageNames(orc_ImportDataAssignment);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Insert messages

   \param[in]  orc_ImportDataAssignment   Import data assignment
   \param[in]  oe_ProtocolType            Protocol type
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CieUtil::h_InsertMessages(const std::vector<C_CieImportDataAssignment> & orc_ImportDataAssignment,
                                 const C_OscCanProtocol::E_Type oe_ProtocolType)
{
   //Each imported node
   for (uint32_t u32_ItNodes = 0; u32_ItNodes < orc_ImportDataAssignment.size(); u32_ItNodes++)
   {
      const C_CieImportDataAssignment & rc_CurData = orc_ImportDataAssignment[u32_ItNodes];

      tgl_assert(rc_CurData.s32_DatapoolIndexForNew >= 0);
      if (rc_CurData.s32_DatapoolIndexForNew >= 0)
      {
         // Insert Tx messages
         int32_t s32_Retval = mh_InsertMessages(rc_CurData.u32_OsyNodeIndex, oe_ProtocolType,
                                                rc_CurData.u32_OsyInterfaceIndex,
                                                rc_CurData.s32_DatapoolIndexForNew, true,
                                                rc_CurData.c_ImportData.c_Core.c_OscTxMessageData,
                                                rc_CurData.c_ImportData.c_Ui.c_UiTxMessageData,
                                                rc_CurData.c_ImportData.c_Core.c_OscTxSignalData,
                                                rc_CurData.c_ImportData.c_Ui.c_UiTxSignalData,
                                                rc_CurData.c_TxMessageOverrideIndices);

         tgl_assert(s32_Retval == C_NO_ERR);
         if (s32_Retval == C_NO_ERR)
         {
            // Insert Rx messages
            s32_Retval = mh_InsertMessages(rc_CurData.u32_OsyNodeIndex, oe_ProtocolType,
                                           rc_CurData.u32_OsyInterfaceIndex,
                                           rc_CurData.s32_DatapoolIndexForNew, false,
                                           rc_CurData.c_ImportData.c_Core.c_OscRxMessageData,
                                           rc_CurData.c_ImportData.c_Ui.c_UiRxMessageData,
                                           rc_CurData.c_ImportData.c_Core.c_OscRxSignalData,
                                           rc_CurData.c_ImportData.c_Ui.c_UiRxSignalData,
                                           rc_CurData.c_RxMessageOverrideIndices);
            tgl_assert(s32_Retval == C_NO_ERR);
         }
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get device info

   \param[in]      ou32_DeviceNodeIndex            Device node index
   \param[in]      ou32_DeviceNodeInterfaceIndex   Device node interface index
   \param[in]      ou32_ManagerNodeIndex           Manager node index
   \param[in]      ou8_ManagerNodeInterfaceNumber  Manager node interface number
   \param[in]      orc_EdsPath                     Eds path
   \param[in,out]  orc_NodeAssignment              Node assignment
   \param[in,out]  orc_InvalidNodeAssignment       Invalid node assignment
   \param[in,out]  orc_ParsingError                Parsing error

   \return
   C_NO_ERR Operation success
   C_RANGE  Operation failure: parameter invalid
   C_NOACT  Node Id not allowed
   C_CONFIG Parsing error
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_CieUtil::h_GetDeviceInfo(const uint32_t ou32_DeviceNodeIndex, const uint32_t ou32_DeviceNodeInterfaceIndex,
                                   const uint32_t ou32_ManagerNodeIndex, const uint8_t ou8_ManagerNodeInterfaceNumber,
                                   const QString & orc_EdsPath, C_CieImportDataAssignment & orc_NodeAssignment,
                                   C_CieImportDataAssignment & orc_InvalidNodeAssignment, QString & orc_ParsingError)
{
   int32_t s32_Retval = C_RANGE;
   const C_OscNode * const pc_Node = C_PuiSdHandler::h_GetInstance()->GetOscNodeConst(ou32_DeviceNodeIndex);

   tgl_assert(pc_Node != NULL);
   if (pc_Node != NULL)
   {
      tgl_assert(ou32_DeviceNodeInterfaceIndex < pc_Node->c_Properties.c_ComInterfaces.size());
      if (ou32_DeviceNodeInterfaceIndex < pc_Node->c_Properties.c_ComInterfaces.size())
      {
         const C_OscNodeComInterfaceSettings & rc_CurInterface =
            pc_Node->c_Properties.c_ComInterfaces[ou32_DeviceNodeInterfaceIndex];
         stw::scl::C_SclString c_ParsingError;
         std::vector<C_OscCanMessage> c_OscRxMessageData;
         std::vector<C_OscNodeDataPoolListElement> c_OscRxSignalData;
         std::vector<uint8_t> c_RxSignalDefaultMinMaxValuesUsed;
         std::vector<C_OscCanMessage> c_OscTxMessageData;
         std::vector<C_OscNodeDataPoolListElement> c_OscTxSignalData;
         std::vector<uint8_t> c_TxSignalDefaultMinMaxValuesUsed;
         std::vector<std::vector<stw::scl::C_SclString> > c_ImportMessagesPerMessage;
         std::vector<C_OscCanMessage> c_InvalidOscRxMessageData;
         std::vector<C_OscNodeDataPoolListElement> c_InvalidOscRxSignalData;
         std::vector<uint8_t> c_InvalidRxSignalDefaultMinMaxValuesUsed;
         std::vector<C_OscCanMessage> c_InvalidOscTxMessageData;
         std::vector<C_OscNodeDataPoolListElement> c_InvalidOscTxSignalData;
         std::vector<uint8_t> c_InvalidTxSignalDefaultMinMaxValuesUsed;
         std::vector<std::vector<stw::scl::C_SclString> > c_InvalidImportMessagesPerMessage;
         s32_Retval = C_OscImportEdsDcf::h_Import(
            C_PuiUtil::h_GetAbsolutePathFromProject(
               orc_EdsPath).toStdString().c_str(),
            rc_CurInterface.u8_NodeId,
            c_OscRxMessageData, c_OscRxSignalData, c_RxSignalDefaultMinMaxValuesUsed,
            c_OscTxMessageData, c_OscTxSignalData, c_TxSignalDefaultMinMaxValuesUsed,
            c_ImportMessagesPerMessage, c_ParsingError, true,
            c_InvalidOscRxMessageData, c_InvalidOscRxSignalData, c_InvalidRxSignalDefaultMinMaxValuesUsed,
            c_InvalidOscTxMessageData, c_InvalidOscTxSignalData, c_InvalidTxSignalDefaultMinMaxValuesUsed,
            c_InvalidImportMessagesPerMessage);
         orc_ParsingError = c_ParsingError.c_str();
         if (s32_Retval == C_NO_ERR)
         {
            uint8_t u8_InterfaceId;
            uint32_t u32_InterfaceIndex;
            orc_NodeAssignment.u32_OsyNodeIndex = ou32_ManagerNodeIndex;
            orc_InvalidNodeAssignment.u32_OsyNodeIndex = ou32_ManagerNodeIndex;
            if (C_PuiSdHandler::h_GetInstance()->TranslateCanInterfaceNumberToIndex(ou32_ManagerNodeIndex,
                                                                                    ou8_ManagerNodeInterfaceNumber,
                                                                                    u32_InterfaceIndex) == C_NO_ERR)
            {
               orc_NodeAssignment.u32_OsyInterfaceIndex = u32_InterfaceIndex;
               orc_InvalidNodeAssignment.u32_OsyInterfaceIndex = u32_InterfaceIndex;
            }

            orc_NodeAssignment.c_ImportData =
               C_CieDataPoolListAdapter::h_GetStructureFromDcfAndEdsFileImport(c_OscRxMessageData,
                                                                               c_OscRxSignalData,
                                                                               c_RxSignalDefaultMinMaxValuesUsed,
                                                                               c_OscTxMessageData,
                                                                               c_OscTxSignalData,
                                                                               c_TxSignalDefaultMinMaxValuesUsed,
                                                                               c_ImportMessagesPerMessage);
            orc_InvalidNodeAssignment.c_ImportData =
               C_CieDataPoolListAdapter::h_GetStructureFromDcfAndEdsFileImport(c_InvalidOscRxMessageData,
                                                                               c_InvalidOscRxSignalData,
                                                                               c_InvalidRxSignalDefaultMinMaxValuesUsed,
                                                                               c_InvalidOscTxMessageData,
                                                                               c_InvalidOscTxSignalData,
                                                                               c_InvalidTxSignalDefaultMinMaxValuesUsed,
                                                                               c_InvalidImportMessagesPerMessage);
            if (C_PuiSdHandler::h_GetInstance()->TranslateCanInterfaceIndexToId(ou32_DeviceNodeIndex,
                                                                                ou32_DeviceNodeInterfaceIndex,
                                                                                u8_InterfaceId) == C_NO_ERR)
            {
               const C_OscCanInterfaceId c_Id(ou32_DeviceNodeIndex, u8_InterfaceId);
               C_CieDataPoolListAdapter::h_AssignNode(c_Id, orc_NodeAssignment.c_ImportData.c_Core.c_OscRxMessageData);
               C_CieDataPoolListAdapter::h_AssignNode(c_Id, orc_NodeAssignment.c_ImportData.c_Core.c_OscTxMessageData);
               C_CieDataPoolListAdapter::h_AssignNode(c_Id,
                                                      orc_InvalidNodeAssignment.c_ImportData.c_Core.c_OscRxMessageData);
               C_CieDataPoolListAdapter::h_AssignNode(c_Id,
                                                      orc_InvalidNodeAssignment.c_ImportData.c_Core.c_OscTxMessageData);
            }
         }
      }
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Report eds import error

   \param[in,out]  opc_ParentWidget    Parent widget
   \param[in]      os32_ImportResult   Import result
   \param[in]      orc_ParsingError    Parsing error
   \param[in]      ou8_NodeId          Node id
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CieUtil::h_ReportEdsImportError(QWidget * const opc_ParentWidget, const int32_t os32_ImportResult,
                                       const QString & orc_ParsingError, const uint8_t ou8_NodeId)
{
   C_OgeWiCustomMessage c_Message(opc_ParentWidget, C_OgeWiCustomMessage::E_Type::eERROR);

   c_Message.SetHeading(C_GtGetText::h_GetText("EDS file"));
   switch (os32_ImportResult)
   {
   case C_RANGE:
      c_Message.SetDescription(C_GtGetText::h_GetText("Invalid parameter."));
      c_Message.SetDetails(orc_ParsingError);
      c_Message.SetCustomMinHeight(180, 300);
      break;
   case C_NOACT:
      c_Message.SetDescription(static_cast<QString>(C_GtGetText::h_GetText(
                                                       "EDS file import failed.\nNode ID %1 is invalid.")).arg(
                                  ou8_NodeId));
      c_Message.SetDetails(C_GtGetText::h_GetText("CANopen standard only supports node IDs in the range "
                                                  "of 1 to 127.\nThe node ID can be changed in node "
                                                  "properties."));
      c_Message.SetCustomMinHeight(180, 270);
      break;
   case C_CONFIG:
      c_Message.SetDescription(C_GtGetText::h_GetText("An error occured while parsing."));
      //Update log file
      C_OscLoggingHandler::h_Flush();
      c_Message.SetDetails(static_cast<QString>(
                              "%1<a href=\"file:%2\"><span style=\"color: %3;\">%4</span></a>.").
                           arg(C_GtGetText::h_GetText("For more information see ")).
                           arg(C_OscLoggingHandler::h_GetCompleteLogFileLocation().c_str()).
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

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get message name

   \param[in]  orc_Node                   Node
   \param[in]  oq_IsTx                    Flag if message is tx
   \param[in]  ou32_MessageIndex          Message index
   \param[in]  oe_ProtocolType            Protocol type
   \param[in]  opc_NodeNameReplacement    Node name replacement

   \return
   Message name
*/
//----------------------------------------------------------------------------------------------------------------------
QString C_CieUtil::h_GetMessageName(const C_OscNode & orc_Node, const bool oq_IsTx, const uint32_t ou32_MessageIndex,
                                    const C_OscCanProtocol::E_Type oe_ProtocolType,
                                    const stw::scl::C_SclString * const opc_NodeNameReplacement)
{
   QString c_Retval;

   stw::scl::C_SclString c_Nodename;
   stw::scl::C_SclString c_TpdoRpdo = "RPDO";
   if (opc_NodeNameReplacement != NULL)
   {
      c_Nodename = *opc_NodeNameReplacement;
   }
   else
   {
      c_Nodename = orc_Node.c_Properties.c_Name;
   }
   if (c_Nodename.Length() > 24)
   {
      c_Nodename = c_Nodename.SubString(1, 24);
   }
   if (oe_ProtocolType == C_OscCanProtocol::eCAN_OPEN)
   {
      if (oq_IsTx == false)
      {
         c_TpdoRpdo = "TPDO";
      }
   }
   else
   {
      if (oq_IsTx == true)
      {
         c_TpdoRpdo = "TPDO";
      }
   }
   c_Retval = static_cast<QString>((c_Nodename + "_" + c_TpdoRpdo + "%1").c_str()).
              arg(QString::number(ou32_MessageIndex + 1));
   return c_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Import a DBC file

   File path is assumed to be absolute and non empty.

   \param[in]   ou32_BusIndex          Index of bus in system definition
   \param[in]   oe_ProtocolType        Type of comm protocol
   \param[in]   orc_FullFilePath       Absolute file path to dbc file
   \param[in]   opc_Parent             Parent for dialog
   \param[out]  orc_NodeIndexes        Node indexes that got messages
   \param[out]  orc_InterfaceIndexes   Interface indexes that got messages

   \return
   C_NO_ERR    File imported
   C_NOACT     Nothing done, aborted by user
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_CieUtil::mh_ImportDbcFile(const uint32_t ou32_BusIndex, const C_OscCanProtocol::E_Type oe_ProtocolType,
                                    const QString & orc_FullFilePath, QWidget * const opc_Parent,
                                    std::vector<uint32_t> & orc_NodeIndexes,
                                    std::vector<uint32_t> & orc_InterfaceIndexes)
{
   int32_t s32_Return = C_NOACT;
   int32_t s32_ImportReturn;

   C_CieConverter::C_CieCommDefinition c_CommDef;
   C_SclStringList c_WarningMessages;
   C_SclString c_ErrorMessage;

   // import network of DBC file
   QApplication::setOverrideCursor(Qt::WaitCursor); // big DBC file can take some time to load
   QApplication::processEvents();                   // update cursor
   s32_ImportReturn = C_CieImportDbc::h_ImportNetwork(orc_FullFilePath.toStdString().c_str(),
                                                      c_CommDef, c_WarningMessages, c_ErrorMessage, true);
   QApplication::restoreOverrideCursor(); // get old cursor again
   QApplication::processEvents();         // update cursor

   if ((s32_ImportReturn == stw::errors::C_NO_ERR) || (s32_ImportReturn == stw::errors::C_WARN))
   {
      if ((c_CommDef.c_Nodes.size() == 0UL) && (c_CommDef.c_UnmappedMessages.size() == 0UL))

      {
         C_OgeWiCustomMessage c_NodesError(opc_Parent, C_OgeWiCustomMessage::eERROR);
         c_NodesError.SetHeading(C_GtGetText::h_GetText("DBC file import"));
         c_NodesError.SetDescription(C_GtGetText::h_GetText("No nodes or messages found in selected DBC file."));
         c_NodesError.SetDetails(static_cast<QString>(
                                    "%1<a href=\"file:%2\"><span style=\"color: %3;\">%4</span></a>.").
                                 arg(C_GtGetText::h_GetText(
                                        "For possible parsing errors, warnings and detailed information see ")).
                                 arg(C_OscLoggingHandler::h_GetCompleteLogFileLocation().c_str()).
                                 arg(mc_STYLESHEET_GUIDE_COLOR_LINK).
                                 arg(C_GtGetText::h_GetText("log file")));
         c_NodesError.SetCustomMinHeight(180, 270);
         c_NodesError.Execute();
      }
      else
      {
         if ((s32_ImportReturn == stw::errors::C_WARN) && (c_WarningMessages.GetCount() > 0))
         {
            // display global warning messages
            QString c_Warnings;
            for (uint32_t u32_Pos = 0; u32_Pos < c_WarningMessages.GetCount(); u32_Pos++)
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
         const QPointer<C_OgePopUpDialog> c_PopUpDialogNodeAssignment =
            new C_OgePopUpDialog(opc_Parent, opc_Parent);
         C_CieDbcImportNodeAssignmentWidget * const pc_DialogNodeSelection =
            new C_CieDbcImportNodeAssignmentWidget(*c_PopUpDialogNodeAssignment, orc_FullFilePath, ou32_BusIndex,
                                                   c_CommDef);

         c_PopUpDialogNodeAssignment->SetSize(QSize(740, 770));

         // display node assignment popup
         if (c_PopUpDialogNodeAssignment->exec() == static_cast<int32_t>(QDialog::Accepted))
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
                  C_CieDataPoolListAdapter::h_GetStructureFromDbcFileImport(rc_CurrentAssignment.c_CieNode);
               if ((rc_CurrentAssignment.s32_AssignedOsyNodeIndex >= 0) &&
                   (rc_CurrentAssignment.s32_AssignedOsyInterfaceIndex >= 0))
               {
                  c_NodeAssignmentConverted.u32_OsyNodeIndex =
                     static_cast<uint32_t>(rc_CurrentAssignment.s32_AssignedOsyNodeIndex);
                  c_NodeAssignmentConverted.u32_OsyInterfaceIndex =
                     static_cast<uint32_t>(rc_CurrentAssignment.s32_AssignedOsyInterfaceIndex);

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

            const std::vector<C_CieImportDataAssignment> c_SkippedImportDataAssigned;
            // create message report for user
            const QPointer<C_OgePopUpDialog> c_PopUpDialogReportDialog =
               new C_OgePopUpDialog(opc_Parent, opc_Parent);
            C_CieImportReportWidget * const pc_DialogImportReport =
               new C_CieImportReportWidget(*c_PopUpDialogReportDialog, orc_FullFilePath, ou32_BusIndex,
                                           oe_ProtocolType, c_NodeAssignmentsConverted, c_SkippedImportDataAssigned,
                                           NULL);

            Q_UNUSED(pc_DialogImportReport)

            //Hide previous overlay before showing the next one
            c_PopUpDialogNodeAssignment->HideOverlay();

            // resize
            c_PopUpDialogReportDialog->SetSize(mhc_SIZE_REPORT);

            // display message report
            if (c_PopUpDialogReportDialog->exec() == static_cast<int32_t>(QDialog::Accepted))
            {
               s32_Return = C_NO_ERR;
            }

            if (c_PopUpDialogReportDialog != NULL)
            {
               c_PopUpDialogReportDialog->HideOverlay();
               c_PopUpDialogReportDialog->deleteLater();
            }
         } //lint !e429  //no memory leak because of the parent of pc_Dialog and the Qt memory management
         if (c_PopUpDialogNodeAssignment != NULL)
         {
            c_PopUpDialogNodeAssignment->HideOverlay();
            c_PopUpDialogNodeAssignment->deleteLater();
         }
      } //lint !e429  //no memory leak because of the parent of pc_Dialog and the Qt memory management
   }
   else
   {
      // display error message to user
      const QString c_ErrorMsg = c_ErrorMessage.c_str();
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

   \param[in]   ou32_BusIndex          Index of bus in system definition
   \param[in]   oe_ProtocolType        Type of comm protocol
   \param[in]   orc_FullFilePath       Absolute file path to dbc file
   \param[in]   opc_Parent             Parent for dialog
   \param[out]  orc_NodeIndexes        Node indexes that got messages
   \param[out]  orc_InterfaceIndexes   Interface indexes that got messages

   \return
   C_NO_ERR    File imported
   C_NOACT     Nothing done, aborted by user
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_CieUtil::mh_ImportDcfEdsFile(const uint32_t ou32_BusIndex, const C_OscCanProtocol::E_Type oe_ProtocolType,
                                       const QString & orc_FullFilePath, QWidget * const opc_Parent,
                                       std::vector<uint32_t> & orc_NodeIndexes,
                                       std::vector<uint32_t> & orc_InterfaceIndexes)
{
   int32_t s32_Return = C_NOACT;

   // create node selection popup
   const QPointer<C_OgePopUpDialog> c_PopUpDialog = new C_OgePopUpDialog(opc_Parent, opc_Parent);
   C_CieDcfEdsImportNodeSelectWidget * const pc_DialogNodeSelection =
      new C_CieDcfEdsImportNodeSelectWidget(*c_PopUpDialog, orc_FullFilePath, ou32_BusIndex);

   Q_UNUSED(pc_DialogNodeSelection)

   if (c_PopUpDialog->exec() == static_cast<int32_t>(QDialog::Accepted))
   {
      // Let user select node and interface
      uint32_t u32_NodeIndex = 0;
      uint32_t u32_InterfaceIndex = 0;
      tgl_assert(pc_DialogNodeSelection->GetNodeSelection(u32_NodeIndex, u32_InterfaceIndex) == C_NO_ERR);
      const C_OscNode * const pc_Node = C_PuiSdHandler::h_GetInstance()->GetOscNodeConst(u32_NodeIndex);

      // add to output to forward information about which nodes should get active
      orc_NodeIndexes.push_back(u32_NodeIndex);
      orc_InterfaceIndexes.push_back(u32_InterfaceIndex);

      tgl_assert(pc_Node != NULL);
      if (pc_Node != NULL)
      {
         tgl_assert(u32_InterfaceIndex < pc_Node->c_Properties.c_ComInterfaces.size());
         if (u32_InterfaceIndex < pc_Node->c_Properties.c_ComInterfaces.size())
         {
            const C_OscNodeComInterfaceSettings & rc_CurInterface =
               pc_Node->c_Properties.c_ComInterfaces[u32_InterfaceIndex];
            C_SclString c_ParsingError;
            std::vector<C_OscCanMessage> c_OscRxMessageData;
            std::vector<C_OscNodeDataPoolListElement> c_OscRxSignalData;
            std::vector<uint8_t> c_RxSignalDefaultMinMaxValuesUsed;
            std::vector<C_OscCanMessage> c_OscTxMessageData;
            std::vector<C_OscNodeDataPoolListElement> c_OscTxSignalData;
            std::vector<uint8_t> c_TxSignalDefaultMinMaxValuesUsed;
            std::vector<std::vector<C_SclString> > c_ImportMessagesPerMessage;
            std::vector<C_OscCanMessage> c_InvalidOscRxMessageData;
            std::vector<C_OscNodeDataPoolListElement> c_InvalidOscRxSignalData;
            std::vector<uint8_t> c_InvalidRxSignalDefaultMinMaxValuesUsed;
            std::vector<C_OscCanMessage> c_InvalidOscTxMessageData;
            std::vector<C_OscNodeDataPoolListElement> c_InvalidOscTxSignalData;
            std::vector<uint8_t> c_InvalidTxSignalDefaultMinMaxValuesUsed;
            std::vector<std::vector<C_SclString> > c_InvalidImportMessagesPerMessage;
            const int32_t s32_ImportResult = C_OscImportEdsDcf::h_Import(
               orc_FullFilePath.toStdString().c_str(), rc_CurInterface.u8_NodeId,
               c_OscRxMessageData, c_OscRxSignalData, c_RxSignalDefaultMinMaxValuesUsed,
               c_OscTxMessageData, c_OscTxSignalData, c_TxSignalDefaultMinMaxValuesUsed,
               c_ImportMessagesPerMessage, c_ParsingError, false,
               c_InvalidOscRxMessageData, c_InvalidOscRxSignalData, c_InvalidRxSignalDefaultMinMaxValuesUsed,
               c_InvalidOscTxMessageData, c_InvalidOscTxSignalData, c_InvalidTxSignalDefaultMinMaxValuesUsed,
               c_InvalidImportMessagesPerMessage);
            if (s32_ImportResult == C_NO_ERR)
            {
               C_CieImportDataAssignment c_NodeAssignment;
               c_NodeAssignment.u32_OsyNodeIndex = u32_NodeIndex;
               c_NodeAssignment.u32_OsyInterfaceIndex = u32_InterfaceIndex;
               c_NodeAssignment.c_ImportData =
                  C_CieDataPoolListAdapter::h_GetStructureFromDcfAndEdsFileImport(c_OscRxMessageData,
                                                                                  c_OscRxSignalData,
                                                                                  c_RxSignalDefaultMinMaxValuesUsed,
                                                                                  c_OscTxMessageData,
                                                                                  c_OscTxSignalData,
                                                                                  c_TxSignalDefaultMinMaxValuesUsed,
                                                                                  c_ImportMessagesPerMessage);
               if ((c_NodeAssignment.c_ImportData.c_Core.c_OscRxMessageData.size() > 0UL) ||
                   (c_NodeAssignment.c_ImportData.c_Core.c_OscTxMessageData.size() > 0UL))
               {
                  std::vector<C_CieImportDataAssignment> c_NodeAssignmentVector;
                  c_NodeAssignmentVector.push_back(c_NodeAssignment);

                  const std::vector<C_CieImportDataAssignment> c_SkippedImportDataAssigned;

                  const QPointer<C_OgePopUpDialog> c_New = new C_OgePopUpDialog(opc_Parent, opc_Parent);
                  C_CieImportReportWidget * const pc_Dialog =
                     new C_CieImportReportWidget(*c_New, orc_FullFilePath, ou32_BusIndex, oe_ProtocolType,
                                                 c_NodeAssignmentVector, c_SkippedImportDataAssigned, NULL);

                  Q_UNUSED(pc_Dialog)

                  //Resize
                  c_New->SetSize(mhc_SIZE_REPORT);

                  if (c_New->exec() == static_cast<int32_t>(QDialog::Accepted))
                  {
                     s32_Return = C_NO_ERR;
                  }

                  if (c_New != NULL)
                  {
                     c_New->HideOverlay();
                     c_New->deleteLater();
                  }
               } //lint !e429  //no memory leak because of the parent of pc_Dialog and the Qt memory management
               else
               {
                  C_OgeWiCustomMessage c_Message(opc_Parent);
                  c_Message.SetHeading(C_GtGetText::h_GetText("Nothing found"));
                  c_Message.SetDescription(C_GtGetText::h_GetText("No messages found in file."));
                  c_Message.SetDetails(static_cast<QString>(
                                          "%1<a href=\"file:%2\"><span style=\"color: %3;\">%4</span></a>.").
                                       arg(C_GtGetText::h_GetText(
                                              "For possible parsing errors, warnings and detailed information see ")).
                                       arg(C_OscLoggingHandler::h_GetCompleteLogFileLocation().c_str()).
                                       arg(mc_STYLESHEET_GUIDE_COLOR_LINK).
                                       arg(C_GtGetText::h_GetText("log file")));
                  c_Message.SetCustomMinHeight(180, 270);
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
                  c_Message.SetDetails(static_cast<QString>(c_ParsingError.c_str()));
                  c_Message.SetCustomMinHeight(180, 300);
                  break;
               case C_NOACT:
                  c_Message.SetDescription(static_cast<QString>(C_GtGetText::h_GetText(
                                                                   "EDS file import failed.\nNode ID %1 is invalid.")).arg(
                                              rc_CurInterface.u8_NodeId));
                  c_Message.SetDetails(C_GtGetText::h_GetText("CANopen standard only supports node IDs in the range "
                                                              "of 1 to 127.\nThe node ID can be changed in node "
                                                              "properties."));
                  c_Message.SetCustomMinHeight(180, 270);
                  break;
               case C_CONFIG:
                  c_Message.SetDescription(C_GtGetText::h_GetText("An error occured while parsing."));
                  //Update log file
                  C_OscLoggingHandler::h_Flush();
                  c_Message.SetDetails(static_cast<QString>(
                                          "%1<a href=\"file:%2\"><span style=\"color: %3;\">%4</span></a>.").
                                       arg(C_GtGetText::h_GetText("For more information see ")).
                                       arg(C_OscLoggingHandler::h_GetCompleteLogFileLocation().c_str()).
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
   if (c_PopUpDialog != NULL)
   {
      c_PopUpDialog->HideOverlay();
      c_PopUpDialog->deleteLater();
   }
   return s32_Return;
} //lint !e429  //no memory leak because of the parent of pc_Dialog and the Qt memory management

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Adapt all stored message names as specified

   * Eliminate spaces
   * Cut after 31 characters

   \param[in,out]  orc_ImportDataAssignment  Import data assignment
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CieUtil::mh_AdaptMessageNames(std::vector<C_CieImportDataAssignment> & orc_ImportDataAssignment)
{
   //Each imported node
   for (uint32_t u32_ItNodes = 0; u32_ItNodes < orc_ImportDataAssignment.size(); u32_ItNodes++)
   {
      uint32_t u32_Count;
      C_CieImportDataAssignment & rc_CurData = orc_ImportDataAssignment[u32_ItNodes];

      //Each message
      for (u32_Count = 0; u32_Count < rc_CurData.c_ImportData.c_Core.c_OscRxMessageData.size(); ++u32_Count)
      {
         C_OscCanMessage & rc_CurMessage = rc_CurData.c_ImportData.c_Core.c_OscRxMessageData[u32_Count];
         C_CieUtil::h_AdaptName(rc_CurMessage.c_Name, rc_CurMessage.c_Comment);
      }
      for (u32_Count = 0; u32_Count < rc_CurData.c_ImportData.c_Core.c_OscTxMessageData.size(); ++u32_Count)
      {
         C_OscCanMessage & rc_CurMessage = rc_CurData.c_ImportData.c_Core.c_OscTxMessageData[u32_Count];
         C_CieUtil::h_AdaptName(rc_CurMessage.c_Name, rc_CurMessage.c_Comment);
      }
      //Each signal
      for (u32_Count = 0; u32_Count < rc_CurData.c_ImportData.c_Core.c_OscRxSignalData.size(); ++u32_Count)
      {
         C_OscNodeDataPoolListElement & rc_CurSignal = rc_CurData.c_ImportData.c_Core.c_OscRxSignalData[u32_Count];
         C_CieUtil::h_AdaptName(rc_CurSignal.c_Name, rc_CurSignal.c_Comment);
      }
      for (u32_Count = 0; u32_Count < rc_CurData.c_ImportData.c_Core.c_OscTxSignalData.size(); ++u32_Count)
      {
         C_OscNodeDataPoolListElement & rc_CurSignal = rc_CurData.c_ImportData.c_Core.c_OscTxSignalData[u32_Count];
         C_CieUtil::h_AdaptName(rc_CurSignal.c_Name, rc_CurSignal.c_Comment);
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Adapt all stored messages to protocol restrictions if necessary

   \param[in,out]  orc_ImportDataAssignment  Import data assignment
   \param[in]      oe_ProtocolType           Protocol type
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CieUtil::mh_AdaptMessagesToProtocolType(std::vector<C_CieImportDataAssignment> & orc_ImportDataAssignment,
                                               const C_OscCanProtocol::E_Type oe_ProtocolType)
{
   //Each imported node
   for (uint32_t u32_ItNodes = 0; u32_ItNodes < orc_ImportDataAssignment.size(); u32_ItNodes++)
   {
      C_CieImportDataAssignment & rc_CurData = orc_ImportDataAssignment[u32_ItNodes];
      //Each Rx message
      for (uint32_t u32_ItMessage = 0;
           u32_ItMessage < rc_CurData.c_ImportData.c_Core.c_OscRxMessageData.size();
           ++u32_ItMessage)
      {
         QStringList c_Infos;
         QString & rc_CurInfoMessage = rc_CurData.c_ImportData.c_Core.c_WarningMessagesPerRxMessage[u32_ItMessage];
         C_OscCanMessage & rc_CurMessage = rc_CurData.c_ImportData.c_Core.c_OscRxMessageData[u32_ItMessage];
         C_PuiSdNodeCanMessage & rc_CurUiMessage = rc_CurData.c_ImportData.c_Ui.c_UiRxMessageData[u32_ItMessage];
         C_SdUtil::h_AdaptMessageToProtocolType(rc_CurMessage, &rc_CurUiMessage, oe_ProtocolType, &c_Infos);
         //Add info
         if (rc_CurInfoMessage.isEmpty() == false)
         {
            rc_CurInfoMessage += "\n";
         }
         rc_CurInfoMessage += c_Infos.join("\n");
      }
      //Each Tx message
      for (uint32_t u32_ItMessage = 0; u32_ItMessage < rc_CurData.c_ImportData.c_Core.c_OscTxMessageData.size();
           ++u32_ItMessage)
      {
         QStringList c_Infos;
         QString & rc_CurInfoMessage = rc_CurData.c_ImportData.c_Core.c_WarningMessagesPerTxMessage[u32_ItMessage];
         C_OscCanMessage & rc_CurMessage = rc_CurData.c_ImportData.c_Core.c_OscTxMessageData[u32_ItMessage];
         C_PuiSdNodeCanMessage & rc_CurUiMessage = rc_CurData.c_ImportData.c_Ui.c_UiTxMessageData[u32_ItMessage];
         C_SdUtil::h_AdaptMessageToProtocolType(rc_CurMessage, &rc_CurUiMessage, oe_ProtocolType, &c_Infos);
         //Add info
         if (rc_CurInfoMessage.isEmpty() == false)
         {
            rc_CurInfoMessage += "\n";
         }
         rc_CurInfoMessage += c_Infos.join("\n");
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Insert all specified messages into the specified message container

   \param[in]  ou32_NodeIndex                   Node index (ID)
   \param[in]  oe_Type                          COMM protocol type (ID)
   \param[in]  ou32_InterfaceIndex              Interface index (ID)
   \param[in]  ou32_DatapoolIndexForNewContent  Datapool index for new content
   \param[in]  oq_MessagesAreTx                 Flag if messages are Tx (ID)
   \param[in]  orc_OscMessageData               Imported core message data
   \param[in]  orc_UiMessageData                Imported UI message data
   \param[in]  orc_OscSignalData                Imported core signal data
   \param[in]  orc_UiSignalData                 Imported UI signal data
   \param[in]  orc_MessageOverrideIndices       Indices indicating if message will get replaced or will be added

   \return
   C_NO_ERR Operation success
   C_RANGE  Operation failure: parameter invalid
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_CieUtil::mh_InsertMessages(const uint32_t ou32_NodeIndex, const C_OscCanProtocol::E_Type oe_Type,
                                     const uint32_t ou32_InterfaceIndex, const uint32_t ou32_DatapoolIndexForNewContent,
                                     const bool oq_MessagesAreTx,
                                     const std::vector<C_OscCanMessage> & orc_OscMessageData,
                                     const std::vector<C_PuiSdNodeCanMessage> & orc_UiMessageData,
                                     const std::vector<C_OscNodeDataPoolListElement> & orc_OscSignalData,
                                     const std::vector<C_PuiSdNodeDataPoolListElement> & orc_UiSignalData,
                                     const std::vector< std::pair<int32_t,
                                                                  int32_t> > & orc_MessageOverrideIndices)
{
   int32_t s32_Retval = C_NO_ERR;

   if (((orc_OscMessageData.size() == orc_MessageOverrideIndices.size()) &&
        (orc_OscSignalData.size() == orc_UiSignalData.size())) &&
       (orc_OscMessageData.size() == orc_UiMessageData.size()))
   {
      //Get relevant lists
      const C_OscNodeDataPoolList * pc_CurList = NULL;
      const C_OscNode * pc_Node = C_PuiSdHandler::h_GetInstance()->GetOscNodeConst(ou32_NodeIndex);
      if (pc_Node != NULL)
      {
         for (uint32_t u32_ItProtocol = 0; u32_ItProtocol < pc_Node->c_ComProtocols.size(); ++u32_ItProtocol)
         {
            const C_OscCanProtocol & rc_ComProtocol = pc_Node->c_ComProtocols[u32_ItProtocol];
            if (rc_ComProtocol.u32_DataPoolIndex < pc_Node->c_DataPools.size())
            {
               const C_OscNodeDataPool & rc_CurDataPool = pc_Node->c_DataPools[rc_ComProtocol.u32_DataPoolIndex];
               pc_CurList = C_OscCanProtocol::h_GetComListConst(rc_CurDataPool, ou32_InterfaceIndex, oq_MessagesAreTx);
            }
         }
      }
      for (uint32_t u32_ItMessage = 0; u32_ItMessage < orc_OscMessageData.size(); ++u32_ItMessage)
      {
         const C_OscCanMessage & rc_CurMessage = orc_OscMessageData[u32_ItMessage];
         const C_PuiSdNodeCanMessage & rc_UiMessage = orc_UiMessageData[u32_ItMessage];
         std::vector<C_OscNodeDataPoolListElement> c_CurOscSignalCommons;
         std::vector<C_PuiSdNodeDataPoolListElement> c_CurUiSignalCommons;

         c_CurOscSignalCommons.reserve(rc_CurMessage.c_Signals.size());
         c_CurUiSignalCommons.reserve(rc_CurMessage.c_Signals.size());
         //Copy all relevant signals
         for (uint32_t u32_ItSignal = 0; u32_ItSignal < rc_CurMessage.c_Signals.size(); ++u32_ItSignal)
         {
            const C_OscCanSignal & rc_CurSignal = rc_CurMessage.c_Signals[u32_ItSignal];
            if (rc_CurSignal.u32_ComDataElementIndex < orc_OscSignalData.size())
            {
               C_OscNodeDataPoolListElement c_Copy = orc_OscSignalData[rc_CurSignal.u32_ComDataElementIndex];
               if (pc_CurList != NULL)
               {
                  C_OscNodeDataPoolContent c_DataSet;
                  //Handle data sets
                  c_DataSet.SetArray(c_Copy.GetArray());
                  c_DataSet.SetType(c_Copy.GetType());
                  tgl_assert(pc_CurList->c_DataSets.size() == 1UL);
                  c_Copy.c_DataSetValues.resize(pc_CurList->c_DataSets.size(), c_DataSet);
               }
               c_CurOscSignalCommons.push_back(c_Copy);
               c_CurUiSignalCommons.push_back(orc_UiSignalData[rc_CurSignal.u32_ComDataElementIndex]);
            }
         }
         if (orc_MessageOverrideIndices[u32_ItMessage].second < 0L)
         {
            //Insert new messages
            if (C_PuiSdHandler::h_GetInstance()->AddCanMessage(ou32_NodeIndex, oe_Type, ou32_InterfaceIndex,
                                                               ou32_DatapoolIndexForNewContent, oq_MessagesAreTx,
                                                               rc_CurMessage,
                                                               c_CurOscSignalCommons, c_CurUiSignalCommons,
                                                               rc_UiMessage, false) != C_NO_ERR)
            {
               s32_Retval = C_RANGE;
            }
         }
         else
         {
            //Get updated node
            pc_Node = C_PuiSdHandler::h_GetInstance()->GetOscNodeConst(ou32_NodeIndex);
            if (pc_Node != NULL)
            {
               // Message already exists, adapt message in its datapool
               const uint32_t u32_PresentDatapoolIndex = orc_MessageOverrideIndices[u32_ItMessage].first;

               const stw::opensyde_core::C_OscCanMessageContainer * const pc_Container =
                  C_PuiSdHandler::h_GetInstance()->GetCanProtocolMessageContainer(ou32_NodeIndex, oe_Type,
                                                                                  ou32_InterfaceIndex,
                                                                                  u32_PresentDatapoolIndex);
               if ((pc_Container != NULL) && (ou32_InterfaceIndex < pc_Node->c_Properties.c_ComInterfaces.size()))
               {
                  const C_OscCanMessageIdentificationIndices c_Id(
                     ou32_NodeIndex, oe_Type, ou32_InterfaceIndex, u32_PresentDatapoolIndex, oq_MessagesAreTx,
                     static_cast<uint32_t>(orc_MessageOverrideIndices[u32_ItMessage].second));
                  const C_OscNodeComInterfaceSettings & rc_Interface =
                     pc_Node->c_Properties.c_ComInterfaces[ou32_InterfaceIndex];
                  if ((rc_Interface.GetBusConnected() == true) &&
                      (pc_Container->q_IsComProtocolUsedByInterface == true))
                  {
                     const C_OscCanMessage * const pc_Message = C_PuiSdHandler::h_GetInstance()->GetCanMessage(c_Id);
                     if (pc_Message != NULL)
                     {
                        C_PuiSdNodeCanMessageSyncManager c_SyncManager;
                        c_SyncManager.Init(rc_Interface.u32_BusIndex, oe_Type);
                        //Delete previous signals
                        for (uint32_t u32_ItSignal = pc_Message->c_Signals.size(); u32_ItSignal > 0UL; --u32_ItSignal)
                        {
                           if (c_SyncManager.DeleteCanSignal(c_Id, u32_ItSignal - 1UL) != C_NO_ERR)
                           {
                              s32_Retval = C_RANGE;
                           }
                        }
                        //Add signals (BEFORE replacing the message)
                        for (uint32_t u32_ItSignal = 0; u32_ItSignal < rc_CurMessage.c_Signals.size(); ++u32_ItSignal)
                        {
                           if (c_SyncManager.AddCanSignal(c_Id, rc_CurMessage.c_Signals[u32_ItSignal],
                                                          c_CurOscSignalCommons[u32_ItSignal],
                                                          c_CurUiSignalCommons[u32_ItSignal],
                                                          rc_UiMessage.c_Signals[u32_ItSignal]) != C_NO_ERR)
                           {
                              s32_Retval = C_RANGE;
                           }
                        }
                        //Replace message (LAST)
                        if (c_SyncManager.SetCanMessagePropertiesWithoutDirectionChangeAndWithoutTimeoutChange(
                               c_Id, rc_CurMessage) != C_NO_ERR)
                        {
                           s32_Retval = C_RANGE;
                        }
                     }
                     else
                     {
                        s32_Retval = C_RANGE;
                     }
                  }
                  else
                  {
                     //Replace message
                     if (C_PuiSdHandler::h_GetInstance()->DeleteCanMessage(c_Id) != C_NO_ERR)
                     {
                        s32_Retval = C_RANGE;
                     }
                     else
                     {
                        if (C_PuiSdHandler::h_GetInstance()->InsertCanMessage(c_Id, rc_CurMessage,
                                                                              c_CurOscSignalCommons,
                                                                              c_CurUiSignalCommons,
                                                                              rc_UiMessage,
                                                                              false) != C_NO_ERR)
                        {
                           s32_Retval = C_RANGE;
                        }
                     }
                  }
               }
            }
            else
            {
               s32_Retval = C_RANGE;
            }
         }
      }
   }
   else
   {
      s32_Retval = C_RANGE;
   }
   return s32_Retval;
}
