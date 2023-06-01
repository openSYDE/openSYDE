//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Pop up dialog for adding Pg messages from J1939 catalog

   \copyright   Copyright 2023 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.hpp"
#include "constants.hpp"
#include "stwtypes.hpp"

#include "C_GtGetText.hpp"
#include "C_SdBueJ1939AddMessagesFromCatalogDialog.hpp"
#include "ui_C_SdBueJ1939AddMessagesFromCatalogDialog.h"
#include "C_UsHandler.hpp"
#include "C_PuiProject.hpp"
#include "C_OgeWiUtil.hpp"
#include "C_SclStringList.hpp"
#include "C_OscLoggingHandler.hpp"
#include "C_CieImportDbc.hpp"
#include "C_OgeWiCustomMessage.hpp"
#include "C_Uti.hpp"
#include "C_PuiUtil.hpp"

#include <QFileInfo>
#include <QDesktopServices>

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw::opensyde_gui;
using namespace stw::opensyde_gui_logic;
using namespace stw::opensyde_gui_elements;
using namespace stw::opensyde_core;
using namespace stw::scl;
using namespace stw::errors;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

/* -- Global Variables ---------------------------------------------------------------------------------------------- */

/* -- Module Global Variables --------------------------------------------------------------------------------------- */

/* -- Module Global Function Prototypes ----------------------------------------------------------------------------- */

/* -- Implementation ------------------------------------------------------------------------------------------------ */

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default constructor

   Set up GUI with all elements.

   \param[in,out]  orc_Parent    Reference to parent
*/
//----------------------------------------------------------------------------------------------------------------------
C_SdBueJ1939AddMessagesFromCatalogDialog::C_SdBueJ1939AddMessagesFromCatalogDialog(
   stw::opensyde_gui_elements::C_OgePopUpDialog & orc_Parent) :
   QWidget(&orc_Parent),
   mpc_Ui(new Ui::C_SdBueJ1939AddMessagesFromCatalogDialog),
   mpc_ContextMenu(NULL),
   mrc_ParentDialog(orc_Parent),
   ms32_ImportCatalogReturn(0UL),
   me_Mode(E_MessageMode::eALL_MESSAGES)

{
   this->mpc_Ui->setupUi(this);

   InitStaticNames();

   // register the widget for display
   this->mrc_ParentDialog.SetWidget(this);

   m_SetupContextMenu();
   m_UpdateSelection(0);
   m_UpdateUi();

   // Combobox settings
   this->mpc_Ui->pc_ComboBoxMessageMode->addItem(C_GtGetText::h_GetText("Show all messages"));
   this->mpc_Ui->pc_ComboBoxMessageMode->addItem(C_GtGetText::h_GetText("Show only J1939 specific messages"));
   this->mpc_Ui->pc_ComboBoxMessageMode->setCurrentIndex(0);

   this->mpc_Ui->pc_TabelView->setSelectionMode(QAbstractItemView::ExtendedSelection);

   this->mpc_Ui->pc_TextBrowserStatusMessage->SetLinkOnly();

   // Set tool tip info
   this->mpc_Ui->pc_LabelCatalogPath->SetToolTipInformation(C_GtGetText::h_GetText(
                                                               "J1939 Catalog Path"),
                                                            C_GtGetText::h_GetText(
                                                               "Full path to J1939 Catalog (DBC file)"));
   this->mpc_Ui->pc_LabelMode->SetToolTipInformation(C_GtGetText::h_GetText("Display Mode"),
                                                     C_GtGetText::h_GetText(
                                                        "Message filter: Display all or only J1939 specific"));
   this->mpc_Ui->pc_LabelStatus->SetToolTipInformation(C_GtGetText::h_GetText(
                                                          "Status"),
                                                       C_GtGetText::h_GetText("Shows the state of loading the catalog"));

   connect(this->mpc_Ui->pc_PushButtonAdd, &QPushButton::clicked, this,
           &C_SdBueJ1939AddMessagesFromCatalogDialog::m_AddClicked);
   connect(this->mpc_Ui->pc_PushButtonCancel, &QPushButton::clicked, this,
           &C_SdBueJ1939AddMessagesFromCatalogDialog::m_CancelClicked);
   connect(this->mpc_Ui->pc_PushButtonCatalogPath, &QPushButton::clicked, this,
           &C_SdBueJ1939AddMessagesFromCatalogDialog::m_CatalogPathButtonClicked);
   connect(this->mpc_Ui->pc_ComboBoxMessageMode,
           static_cast<void (QComboBox::*)(int32_t)>(&QComboBox::currentIndexChanged), this,
           &C_SdBueJ1939AddMessagesFromCatalogDialog::m_OnModeChanged);
   connect(this->mpc_Ui->pc_TabelView, &C_SdBueJ1939AddMessagesFromCatalogTableView::SigSelectionChanged, this,
           &C_SdBueJ1939AddMessagesFromCatalogDialog::m_UpdateSelection);
   connect(this->mpc_Ui->pc_TabelView, &C_SdBueJ1939AddMessagesFromCatalogTableView::SigAccept, this,
           &C_SdBueJ1939AddMessagesFromCatalogDialog::m_AddClicked);
   connect(this->mpc_Ui->pc_LineEditMessageFilter, &QLineEdit::textChanged, this,
           &C_SdBueJ1939AddMessagesFromCatalogDialog::m_OnSearch);
   connect(this->mpc_Ui->pc_TextBrowserStatusMessage, &QTextBrowser::anchorClicked, this,
           &C_SdBueJ1939AddMessagesFromCatalogDialog::m_LinkClicked);
   connect(this->mpc_Ui->pc_LineEditCatalogPath, &QLineEdit::editingFinished, this,
           &C_SdBueJ1939AddMessagesFromCatalogDialog::m_CatalogPathEntered);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default destructor
*/
//----------------------------------------------------------------------------------------------------------------------
//lint -e{1540}  no memory leak because of the parent of mpc_ContextMenu and the Qt memory management
// NOTE: 'noexcept' keyword is added (also in .hpp) to eliminate the warning - (1906) exception specification.
// This warning appeared after adding the class variable "mc_CieCommDef".
C_SdBueJ1939AddMessagesFromCatalogDialog::~C_SdBueJ1939AddMessagesFromCatalogDialog(void) noexcept
{
   delete this->mpc_Ui;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Initialize all displayed static names
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueJ1939AddMessagesFromCatalogDialog::InitStaticNames(void) const
{
   this->mrc_ParentDialog.SetTitle(C_GtGetText::h_GetText("Add PG Messages"));
   this->mrc_ParentDialog.SetSubTitle(C_GtGetText::h_GetText("From J1939 Catalog"));
   this->mpc_Ui->pc_LabelHeadingPreview->setText(C_GtGetText::h_GetText("J1939 Catalog"));
   this->mpc_Ui->pc_PushButtonAdd->setText(C_GtGetText::h_GetText("Add"));
   this->mpc_Ui->pc_PushButtonCancel->setText(C_GtGetText::h_GetText("Cancel"));
   this->mpc_Ui->pc_LineEditMessageFilter->setPlaceholderText(C_GtGetText::h_GetText("Filter"));
   this->mpc_Ui->pc_LabelNoMessages->setText(C_GtGetText::h_GetText("No messages to display."));
   this->mpc_Ui->pc_LabelSelection->setText(C_GtGetText::h_GetText("No selected message"));
   this->mpc_Ui->pc_TextBrowserStatusMessage->setText(C_GtGetText::h_GetText("<span>-</span>"));
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get messages selected from J1939 catalog (DBC file)

   \return
   Messages
*/
//----------------------------------------------------------------------------------------------------------------------
std::vector<C_CieConverter::C_CieNodeMessage> C_SdBueJ1939AddMessagesFromCatalogDialog::GetMessagesImportedFromCatalog()
const
{
   return this->mc_MessagesImportedFromCatalog;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get the J1939 catalog file path (DBC file)


   \return
   File path
*/
//----------------------------------------------------------------------------------------------------------------------
const QString C_SdBueJ1939AddMessagesFromCatalogDialog::GetCatalogFilePath() const
{
   return this->mc_CatalogFilePath;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Overwritten key press event slot

   Here: Handle specific enter key cases

   \param[in,out] opc_KeyEvent Event identification and information
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueJ1939AddMessagesFromCatalogDialog::keyPressEvent(QKeyEvent * const opc_KeyEvent)
{
   bool q_CallOrg = true;

   //Handle all enter key cases manually

   if ((opc_KeyEvent->key() == static_cast<int32_t>(Qt::Key_Enter)) ||
       (opc_KeyEvent->key() == static_cast<int32_t>(Qt::Key_Return)))
   {
      if (((opc_KeyEvent->modifiers().testFlag(Qt::ControlModifier) == true) &&
           (opc_KeyEvent->modifiers().testFlag(Qt::AltModifier) == false)) &&
          (opc_KeyEvent->modifiers().testFlag(Qt::ShiftModifier) == false))
      {
         this->m_AddClicked();
      }
      else
      {
         q_CallOrg = false;
      }
   }
   if (q_CallOrg == true)
   {
      QWidget::keyPressEvent(opc_KeyEvent);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Slot of Add button click
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueJ1939AddMessagesFromCatalogDialog::m_AddClicked(void)
{
   this->mc_MessagesImportedFromCatalog = this->mpc_Ui->pc_TabelView->GetSelectedMessages();
   if (this->mc_MessagesImportedFromCatalog.size() == 0)
   {
      this->m_ShowNoMessagesSelectedPopup();
   }
   else
   {
      this->mrc_ParentDialog.accept();
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Slot of Cancel button click
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueJ1939AddMessagesFromCatalogDialog::m_CancelClicked(void)
{
   this->mrc_ParentDialog.reject();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Handle DBC path button click
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueJ1939AddMessagesFromCatalogDialog::m_CatalogPathButtonClicked()
{
   this->m_ProcessCatalogPath(false);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Handle change of message mode:
 *  1) All messages
 *  2) J1939 specific
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueJ1939AddMessagesFromCatalogDialog::m_OnModeChanged()
{
   this->m_SetMessageMode();

   if ((this->ms32_ImportCatalogReturn == stw::errors::C_NO_ERR) ||
       (this->ms32_ImportCatalogReturn == stw::errors::C_WARN))
   {
      // Process the messages
      // Filter J1939 specific messages
      if (this->me_Mode == E_MessageMode::eJ1939_SPECIFIC)
      {
         std::vector<stw::opensyde_gui_logic::C_CieConverter::C_CieNodeMessage> c_FilteredMessages;
         this->m_FilterJ1939SpecificMessages(c_FilteredMessages);
         this->mpc_Ui->pc_TabelView->UpdateData(c_FilteredMessages);
      }
      // All messages
      else
      {
         this->mpc_Ui->pc_TabelView->UpdateData(this->mc_MessagesImportedFromCatalog);
      }
   }

   m_UpdateUi();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Set message mode based on combo box selection
 *
 *  1) All messages
 *  2) J1939 specific
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueJ1939AddMessagesFromCatalogDialog::m_SetMessageMode()
{
   this->me_Mode = static_cast<E_MessageMode>(this->mpc_Ui->pc_ComboBoxMessageMode->currentIndex());
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief Open DBC file
 *
 *  \param[in,out]  opc_BaseWidget   Base widget
 *
 *  \return
 *  DBC file path
*/
//----------------------------------------------------------------------------------------------------------------------
QString C_SdBueJ1939AddMessagesFromCatalogDialog::m_GetCatalogFilePath(QWidget * const opc_BaseWidget)
{
   //Load user settings value
   QString c_Folder = C_UsHandler::h_GetInstance()->GetLastKnownJ1939CatalogPath();
   QString c_FilePath = "";
   QFileInfo c_FileInfo;
   const QString c_FilterName = static_cast<QString>(C_GtGetText::h_GetText("DBC file")) + " (*.dbc)";
   const QString c_DefaultSuffix = static_cast<QString>(C_GtGetText::h_GetText("dbc"));

   //Replace default path if necessary
   if (c_Folder.compare("") == 0)
   {
      c_Folder = C_PuiProject::h_GetInstance()->GetFolderPath();
   }

   // Opens file explorer
   c_FilePath =
      C_OgeWiUtil::h_GetOpenFileName(opc_BaseWidget, C_GtGetText::h_GetText("Select DBC File"),
                                     c_Folder, c_FilterName, c_DefaultSuffix);
   c_FileInfo.setFile(c_FilePath);

   return c_FilePath;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Remove duplicate messages, if any
 *
 *  Eliminates messages with same details i.e CAN Id
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueJ1939AddMessagesFromCatalogDialog::m_RemoveDuplicateMessages()
{
   if (this->mc_MessagesImportedFromCatalog.size() > 0)

   {
      std::sort(this->mc_MessagesImportedFromCatalog.begin(), this->mc_MessagesImportedFromCatalog.end());
      const std::vector<C_CieConverter::C_CieNodeMessage>::const_iterator c_Last = std::unique(
         this->mc_MessagesImportedFromCatalog.begin(), this->mc_MessagesImportedFromCatalog.end());
      this->mc_MessagesImportedFromCatalog.erase(c_Last, this->mc_MessagesImportedFromCatalog.end());
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Open document on link click

   Opens the log file in a text editor

   \param[in]       orc_Link     Link to the log file

*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueJ1939AddMessagesFromCatalogDialog::m_LinkClicked(const QUrl & orc_Link) const
{
   QDesktopServices::openUrl(orc_Link);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Load messages from the catalog i.e DBC file

   \param[in]      orc_NewPath     J1939 catalog file path
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueJ1939AddMessagesFromCatalogDialog::m_LoadCatalog()
{
   C_SclStringList c_WarningMessages;
   C_SclString c_ErrorMessage;
   const QString c_FullFilePath = this->mc_CatalogFilePath;

   stw::opensyde_gui_logic::C_CieConverter::C_CieCommDefinition c_CieCommDef;

   // import network of DBC file
   QApplication::setOverrideCursor(Qt::WaitCursor); // big DBC file can take some time to load
   QApplication::processEvents();                   // update cursor
   this->ms32_ImportCatalogReturn = C_CieImportDbc::h_ImportNetwork(
      c_FullFilePath.toStdString().c_str(),
      c_CieCommDef, c_WarningMessages, c_ErrorMessage,
      true);
   QApplication::restoreOverrideCursor(); // get old cursor again
   QApplication::processEvents();         // update cursor

   // Set the file import status
   m_SetStatus();

   //    Messages imported successfully from DBC file with no errors (or with warnings)
   if ((this->ms32_ImportCatalogReturn == stw::errors::C_NO_ERR) ||
       (this->ms32_ImportCatalogReturn == stw::errors::C_WARN))
   {
      // Save catalog path
      if (this->mc_CatalogFilePath.compare("") != 0)
      {
         C_UsHandler::h_GetInstance()->SetLastKnownJ1939CatalogPath(this->mc_CatalogFilePath);
      }

      // Extract messages from the DBC file
      this->m_ExtractMessagesFromDbc(c_CieCommDef);

      // Eliminate any duplicate messages (mostly coming from "dirty" non-J1939 files)
      this->m_RemoveDuplicateMessages();

      // Messages exist
      if ((c_CieCommDef.c_Nodes.size() > 0UL) || (c_CieCommDef.c_UnmappedMessages.size() > 0UL))
      {
         // Process the messages
         this->mpc_Ui->pc_TabelView->UpdateData(this->mc_MessagesImportedFromCatalog);
      }
   }
   else
   {
      this->m_ShowCatalogImportError();

      //Update table
      this->mc_MessagesImportedFromCatalog.clear();
      this->mpc_Ui->pc_TabelView->UpdateData(this->mc_MessagesImportedFromCatalog);
   }

   m_UpdateUi();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Set status message for DBC file import
 *
 *   \param[in]       os32_Error     Error returned while importing the file
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueJ1939AddMessagesFromCatalogDialog::m_SetStatus() const
{
   const QString c_StatusOk = C_GtGetText::h_GetText("File loaded. ");
   const QString c_StatusFail = C_GtGetText::h_GetText("Loading failed. ");
   QString c_StatusMessage("");

   if ((this->ms32_ImportCatalogReturn == stw::errors::C_NO_ERR) ||
       (this->ms32_ImportCatalogReturn == stw::errors::C_WARN))
   {
      c_StatusMessage = c_StatusOk;
   }
   else
   {
      c_StatusMessage = c_StatusFail;
   }

   c_StatusMessage += C_GtGetText::h_GetText("For detailed information see ");
   c_StatusMessage += C_Uti::h_GetLink(C_GtGetText::h_GetText("log file"), mc_STYLE_GUIDE_COLOR_LINK,
                                       C_OscLoggingHandler::h_GetCompleteLogFileLocation().c_str());

   this->mpc_Ui->pc_TextBrowserStatusMessage->setText(c_StatusMessage);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Handle the visibility of UI elements

*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueJ1939AddMessagesFromCatalogDialog::m_UpdateUi() const
{
   //  Messages exist
   if (this->mpc_Ui->pc_TabelView->IsEmpty() == false)
   {
      this->mpc_Ui->pc_TabelView->setVisible(true);
      this->mpc_Ui->pc_LabelSelection->setVisible(true);
      this->mpc_Ui->pc_LabelNoMessages->setVisible(false);
   }
   // no messages
   else
   {
      this->mpc_Ui->pc_TabelView->setVisible(false);
      this->mpc_Ui->pc_LabelSelection->setVisible(false);
      this->mpc_Ui->pc_LabelNoMessages->setVisible(true);
   }

   this->mpc_Ui->pc_LabelSelection->setText(C_GtGetText::h_GetText("No selected message"));
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Update number of selected items

   \param[in]  os32_SelectionCount   Number of selected items
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueJ1939AddMessagesFromCatalogDialog::m_UpdateSelection(const int32_t os32_SelectionCount) const
{
   if (this->mpc_Ui->pc_TabelView->IsEmpty() == true)
   {
      this->mpc_Ui->pc_LabelSelection->setVisible(false);
   }
   else
   {
      this->mpc_Ui->pc_LabelSelection->setVisible(true);
      if (os32_SelectionCount > 0)
      {
         this->mpc_Ui->pc_LabelSelection->setText(static_cast<QString>(C_GtGetText::h_GetText(
                                                                          "%1 selected message(s)")).
                                                  arg(os32_SelectionCount));
      }
      else
      {
         this->mpc_Ui->pc_LabelSelection->setText(static_cast<QString>(C_GtGetText::h_GetText(
                                                                          "No selected message")));
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Displays an error message popup when there is an error importing the
 *          J1939 catalog i.e the DBC file
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueJ1939AddMessagesFromCatalogDialog::m_ShowCatalogImportError()
{
   C_OgeWiCustomMessage c_Message(this, C_OgeWiCustomMessage::E_Type::eERROR);
   QString c_LogMessage("For detailed information see ");

   c_LogMessage += C_Uti::h_GetLink(C_GtGetText::h_GetText("log file"), mc_STYLE_GUIDE_COLOR_LINK,
                                    C_OscLoggingHandler::h_GetCompleteLogFileLocation().c_str());
   c_Message.SetHeading(C_GtGetText::h_GetText("Catalog Import Error"));
   c_Message.SetDescription(C_GtGetText::h_GetText("DBC file cannot be read. Check your file and retry."));
   c_Message.SetDetails(c_LogMessage);
   c_Message.SetCustomMinHeight(180, 270);
   c_Message.Execute();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Displays an info message popup when user tries to add messages
 *  without selecting any messages
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueJ1939AddMessagesFromCatalogDialog::m_ShowNoMessagesSelectedPopup()
{
   C_OgeWiCustomMessage c_Message(this, C_OgeWiCustomMessage::E_Type::eINFORMATION);

   c_Message.SetHeading(C_GtGetText::h_GetText("Add PG Message"));
   c_Message.SetDescription(C_GtGetText::h_GetText("No PG messages selected. Select at least one message."));
   c_Message.SetCustomMinHeight(180, 270);
   c_Message.Execute();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Process the catalog path

   \param[in]       q_PathManuallyEntered     Catalog path was entered manually or browsed through file explorer
 */
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueJ1939AddMessagesFromCatalogDialog::m_ProcessCatalogPath(const bool oq_PathManuallyEntered)
{
   const QString c_LastPath = this->mc_CatalogFilePath;

   // Path entered manually
   if (oq_PathManuallyEntered)
   {
      if (this->mpc_Ui->pc_LineEditCatalogPath->text().length() > 0)
      {
         this->mc_CatalogFilePath = C_PuiUtil::h_GetAbsolutePathFromProject(
            this->mpc_Ui->pc_LineEditCatalogPath->GetPath());
      }
      else
      {
         this->mc_CatalogFilePath.clear();
      }
   }
   // Path browsed using file explorer
   else
   {
      // opens the file explorer to select DBC file
      const QString c_PathReturned = m_GetCatalogFilePath(this);

      if (c_PathReturned.length() != 0) // Length is zero when dialog is canceled
      {
         this->mc_CatalogFilePath = c_PathReturned;
         this->mpc_Ui->pc_LineEditCatalogPath->SetPath(this->mc_CatalogFilePath,
                                                       C_PuiProject::h_GetInstance()->GetFolderPath());
      }
   }

   //Only load if there was a change
   if (this->mc_CatalogFilePath != c_LastPath)
   {
      // Initialize status label
      this->mpc_Ui->pc_TextBrowserStatusMessage->setText(C_GtGetText::h_GetText("<span>-</span>"));

      // Load the catalog if path not empty
      if (this->mc_CatalogFilePath.compare("") != 0)
      {
         this->m_LoadCatalog();
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Filter J1939 specific messages

   \param[in,out]   orc_FilteredMessages   Reference to the filtered messages

*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueJ1939AddMessagesFromCatalogDialog::m_FilterJ1939SpecificMessages(
   std::vector<stw::opensyde_gui_logic::C_CieConverter::C_CieNodeMessage> & orc_FilteredMessages)
{
   if (this->mc_MessagesImportedFromCatalog.size() > 0)
   {
      for (uint32_t u32_NodeIndex = 0; u32_NodeIndex < this->mc_MessagesImportedFromCatalog.size(); u32_NodeIndex++)
      {
         if ((this->mc_MessagesImportedFromCatalog[u32_NodeIndex].c_CanMessage.u16_Dlc == 8) &&
             (this->mc_MessagesImportedFromCatalog[u32_NodeIndex].c_CanMessage.q_IsExtended == true))
         {
            orc_FilteredMessages.push_back(this->mc_MessagesImportedFromCatalog[u32_NodeIndex]);
         }
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  When catalog path entered (manually)

*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueJ1939AddMessagesFromCatalogDialog::m_CatalogPathEntered()
{
   this->m_ProcessCatalogPath(true);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Extract the messages from the DBC file contents

   \param[in]       orc_CieCommDef     Detailed input parameter description
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueJ1939AddMessagesFromCatalogDialog::m_ExtractMessagesFromDbc(
   const C_CieConverter::C_CieCommDefinition & orc_CieCommDef)
{
   this->mc_MessagesImportedFromCatalog.clear();

   C_CieConverter::C_CieNodeMessage c_Message;

   // Get messages from the nodes
   for (uint32_t u32_NodeIndex = 0; u32_NodeIndex < orc_CieCommDef.c_Nodes.size(); u32_NodeIndex++)
   {
      // Tx messages
      for (uint32_t u32_MessageIndex = 0;
           u32_MessageIndex < (orc_CieCommDef.c_Nodes[u32_NodeIndex].c_TxMessages.size()); u32_MessageIndex++)
      {
         c_Message =  orc_CieCommDef.c_Nodes[u32_NodeIndex].c_TxMessages[u32_MessageIndex];
         this->mc_MessagesImportedFromCatalog.push_back(c_Message);
      }

      // Rx messages
      for (uint32_t u32_MessageIndex = 0;
           u32_MessageIndex < (orc_CieCommDef.c_Nodes[u32_NodeIndex].c_RxMessages.size()); u32_MessageIndex++)
      {
         c_Message = orc_CieCommDef.c_Nodes[u32_NodeIndex].c_RxMessages[u32_MessageIndex];
         this->mc_MessagesImportedFromCatalog.push_back(c_Message);
      }
   }

   // Get the unmapped messages
   for (uint32_t u32_MessageIndex = 0;
        u32_MessageIndex < (orc_CieCommDef.c_UnmappedMessages.size()); u32_MessageIndex++)
   {
      c_Message = orc_CieCommDef.c_UnmappedMessages[u32_MessageIndex];
      this->mc_MessagesImportedFromCatalog.push_back(c_Message);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Filter for string

   \param[in]  orc_Text    String
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueJ1939AddMessagesFromCatalogDialog::m_OnSearch(const QString & orc_Text) const
{
   this->mpc_Ui->pc_TabelView->Search(orc_Text);
   m_UpdateUi();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Setup context menu entries
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueJ1939AddMessagesFromCatalogDialog::m_SetupContextMenu(void)
{
   this->mpc_ContextMenu = new C_OgeContextMenu(this);

   // select all action
   this->mpc_ContextMenu->addAction(C_GtGetText::h_GetText("Select all"),
                                    this->mpc_Ui->pc_TabelView,
                                    &C_SdBueJ1939AddMessagesFromCatalogTableView::selectAll,
                                    static_cast<int32_t>(Qt::CTRL) + static_cast<int32_t>(Qt::Key_A));

   this->setContextMenuPolicy(Qt::CustomContextMenu);
   connect(this, &C_SdBueJ1939AddMessagesFromCatalogDialog::customContextMenuRequested, this,
           &C_SdBueJ1939AddMessagesFromCatalogDialog::m_OnCustomContextMenuRequested);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Show custom context menu

   \param[in]  orc_Pos  Local context menu position
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueJ1939AddMessagesFromCatalogDialog::m_OnCustomContextMenuRequested(const QPoint & orc_Pos)
{
   const QPoint c_PosGlobal = this->mapToGlobal(orc_Pos);

   // check if position is on tree view
   if (this->mpc_Ui->pc_TabelView->geometry().contains(orc_Pos))
   {
      this->mpc_ContextMenu->popup(c_PosGlobal);
   }
}
