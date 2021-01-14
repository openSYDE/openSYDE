//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Widget for progress log (implementation)

   Widget for progress log

   \copyright   Copyright 2018 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <QUrl>
#include <QScrollBar>
#include <QDesktopServices>
#include <QClipboard>
#include "stwtypes.h"
#include "constants.h"
#include "C_GtGetText.h"
#include "C_OSCLoggingHandler.h"
#include "C_OgeFrameSeparator.h"
#include "C_OgeLabProgressLogItem.h"

#include "C_SyvUpProgressLog.h"
#include "ui_C_SyvUpProgressLog.h"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw_types;
using namespace stw_opensyde_gui;
using namespace stw_opensyde_core;
using namespace stw_opensyde_gui_logic;
using namespace stw_opensyde_gui_elements;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

/* -- Global Variables ---------------------------------------------------------------------------------------------- */

/* -- Module Global Variables --------------------------------------------------------------------------------------- */

/* -- Module Global Function Prototypes ----------------------------------------------------------------------------- */

/* -- Implementation ------------------------------------------------------------------------------------------------ */

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default constructor

   Set up GUI with all elements.

   \param[in,out] opc_Parent Optional pointer to parent
*/
//----------------------------------------------------------------------------------------------------------------------
C_SyvUpProgressLog::C_SyvUpProgressLog(QWidget * const opc_Parent) :
   QWidget(opc_Parent),
   mpc_Ui(new Ui::C_SyvUpProgressLog),
   mpc_LogHyperlink(NULL)
{
   this->mpc_Ui->setupUi(this);
   this->mpc_Ui->pc_ScrollArea->DeactivateScrollbarResize();
   this->mpc_Ui->pc_ScrollAreaWidget->SetBackgroundColor(-1);

   //initially disable heading
   this->mpc_Ui->pc_LabelPreview->setVisible(false);
   this->mpc_Ui->pc_Frame->setVisible(false);

   //Always scroll to new entry
   connect(
      this->mpc_Ui->pc_ScrollArea->verticalScrollBar(), &QScrollBar::rangeChanged, this,
      &C_SyvUpProgressLog::m_ScrollToBottom);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default destructor

   Clean up.
*/
//----------------------------------------------------------------------------------------------------------------------
C_SyvUpProgressLog::~C_SyvUpProgressLog(void)
{
   Clear();
   delete mpc_Ui;
   //lint -e{1579}  Clean up in clear function
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Clear all known widgets
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvUpProgressLog::Clear(void)
{
   //Connect
   for (uint32 u32_ItEntry = 0; u32_ItEntry < this->mc_ConnectEntries.size(); ++u32_ItEntry)
   {
      C_SyvUpProgressLogEntry * const pc_Entry = this->mc_ConnectEntries[u32_ItEntry];
      if (pc_Entry != NULL)
      {
         this->mpc_Ui->pc_VerticalLayoutDynamic->removeWidget(pc_Entry);
         delete (pc_Entry);
      }
   }
   this->mc_ConnectEntries.clear();
   //Spacer
   for (uint32 u32_ItEntry = 0; u32_ItEntry < this->mc_SpacerEntries.size(); ++u32_ItEntry)
   {
      C_SyvUpProgressLogEntry * const pc_Entry = this->mc_SpacerEntries[u32_ItEntry];
      if (pc_Entry != NULL)
      {
         this->mpc_Ui->pc_VerticalLayoutDynamic->removeWidget(pc_Entry);
         delete (pc_Entry);
      }
   }
   this->mc_SpacerEntries.clear();
   //Update
   for (uint32 u32_ItEntry = 0; u32_ItEntry < this->mc_UpdateEntries.size(); ++u32_ItEntry)
   {
      C_SyvUpProgressLogEntry * const pc_Entry = this->mc_UpdateEntries[u32_ItEntry];
      if (pc_Entry != NULL)
      {
         this->mpc_Ui->pc_VerticalLayoutDynamic->removeWidget(pc_Entry);
         delete (pc_Entry);
      }
   }
   this->mc_UpdateEntries.clear();
   //Process
   for (uint32 u32_ItEntry = 0; u32_ItEntry < this->mc_ProcessEntries.size(); ++u32_ItEntry)
   {
      QLabel * const pc_Entry = this->mc_ProcessEntries[u32_ItEntry];
      if (pc_Entry != NULL)
      {
         this->mpc_Ui->pc_VerticalLayoutDynamic->removeWidget(pc_Entry);
         delete (pc_Entry);
      }
   }
   this->mc_ProcessEntries.clear();
   //Hyperlink
   delete (this->mpc_LogHyperlink);
   this->mpc_LogHyperlink = NULL;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set heading

   \param[in] orc_Value New heading
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvUpProgressLog::SetHeading(const QString & orc_Value) const
{
   m_SetHeadingVisible();
   this->mpc_Ui->pc_LabelPreview->setText(orc_Value);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Add log sub heading

   \param[in] orc_Value New log entry
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvUpProgressLog::AddSubHeading(const QString & orc_Value)
{
   QLabel * const pc_Entry = new C_OgeLabProgressLogItem(this);

   pc_Entry->setText(orc_Value);

   this->mc_ProcessEntries.push_back(pc_Entry);
   //Insert before spacer
   this->mpc_Ui->pc_VerticalLayoutDynamic->insertWidget(
      this->mpc_Ui->pc_VerticalLayoutDynamic->count() - 1, pc_Entry, 0);
} //lint !e429  //deleted later, stored internally

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Add log entry

   \param[in] ou32_NodeIndex Node index
   \param[in] orc_Value      New log entry
   \param[in] oq_Update      Flag if entry for update
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvUpProgressLog::AddEntry(const uint32 ou32_NodeIndex, const QString & orc_Value, const bool oq_Update)
{
   C_SyvUpProgressLogEntry * const pc_Entry = new C_SyvUpProgressLogEntry(ou32_NodeIndex, this);

   //Configure
   pc_Entry->SetEntryDescription(orc_Value);
   pc_Entry->SetEntryStatus("");

   m_SetHeadingVisible();

   //Add
   if (oq_Update == false)
   {
      this->mc_ConnectEntries.push_back(pc_Entry);
   }
   else
   {
      this->mc_UpdateEntries.push_back(pc_Entry);
   }

   //Add
   if (this->mpc_LogHyperlink == NULL)
   {
      //Insert before spacer
      this->mpc_Ui->pc_VerticalLayoutDynamic->insertWidget(
         this->mpc_Ui->pc_VerticalLayoutDynamic->count() - 1, pc_Entry, 0);
   }
   else
   {
      //Insert before error message
      this->mpc_Ui->pc_VerticalLayoutDynamic->insertWidget(
         this->mpc_Ui->pc_VerticalLayoutDynamic->count() - 3, pc_Entry, 0);
   }
} //lint !e429  //deleted later, stored internally

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Add spacer
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvUpProgressLog::AddSpacer(void)
{
   C_SyvUpProgressLogEntry * const pc_Entry = new C_SyvUpProgressLogEntry(0, this);

   //Configure
   pc_Entry->SetEntryDescription("");
   pc_Entry->SetEntryStatus("");

   //Add
   this->mc_SpacerEntries.push_back(pc_Entry);
   this->mpc_Ui->pc_VerticalLayoutDynamic->insertWidget(this->mpc_Ui->pc_VerticalLayoutDynamic->count() - 1, pc_Entry,
                                                        0);
} //lint !e429  //deleted later, stored internally

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Update log entry

   \param[in] ou32_NodeIndex Node index
   \param[in] orc_Value      New log status
   \param[in] oq_Update      Flag if entry for update
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvUpProgressLog::UpdateStatus(const uint32 ou32_NodeIndex, const QString & orc_Value,
                                      const bool oq_Update) const
{
   const std::vector<C_SyvUpProgressLogEntry *> * pc_Entries;

   if (oq_Update == true)
   {
      pc_Entries = &this->mc_UpdateEntries;
   }
   else
   {
      pc_Entries = &this->mc_ConnectEntries;
   }
   for (uint32 u32_ItEntry = 0; u32_ItEntry < pc_Entries->size(); ++u32_ItEntry)
   {
      C_SyvUpProgressLogEntry * const pc_Entry = pc_Entries->at(u32_ItEntry);
      if ((pc_Entry != NULL) && (pc_Entry->DoesIndexMatch(ou32_NodeIndex) == true))
      {
         pc_Entry->SetEntryStatus(orc_Value);
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Add log hyperlink
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvUpProgressLog::AddLogHyperlink(void)
{
   const QString c_LogFilePath = C_OSCLoggingHandler::h_GetCompleteLogFileLocation().c_str();

   if (this->mpc_LogHyperlink == NULL)
   {
      //Spacer
      this->AddSpacer();

      //Hyperlink
      this->mpc_LogHyperlink = new C_OgeLabProgressLogItem(this);

      //Update log file
      C_OSCLoggingHandler::h_Flush();
      //Configure
      this->mpc_LogHyperlink->setText(static_cast<QString>("%1<a href=\"file:\\\\\\%2\"><span style=\"color: %3;\">%4</span></a>.").
                                      arg(C_GtGetText::h_GetText("For more information see ")).
                                      arg(c_LogFilePath).
                                      arg(mc_STYLESHEET_GUIDE_COLOR_LINK).
                                      arg(C_GtGetText::h_GetText("log file")));
      this->mpc_LogHyperlink->setTextInteractionFlags(Qt::LinksAccessibleByMouse);
      this->mpc_LogHyperlink->setFocusPolicy(Qt::NoFocus);
      connect(this->mpc_LogHyperlink, &QLabel::linkActivated, this,
              &C_SyvUpProgressLog::m_OpenLink);

      //Context menu "Copy Link Location"
      this->mpc_LogHyperlink->setContextMenuPolicy(Qt::CustomContextMenu);
      this->mpc_ContextMenu = new C_OgeContextMenu(this);
      connect(this->mpc_LogHyperlink, &QLabel::customContextMenuRequested, this,
              &C_SyvUpProgressLog::m_OnCustomContextMenuRequested);

      //Add
      this->mpc_Ui->pc_VerticalLayoutDynamic->insertWidget(
         this->mpc_Ui->pc_VerticalLayoutDynamic->count() - 1, this->mpc_LogHyperlink, 0);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get all registered connect node entry indices

   \return
   All registered connect node entry indices
*/
//----------------------------------------------------------------------------------------------------------------------
std::vector<uint32> C_SyvUpProgressLog::GetConnectNodeEntryIndices(void)
{
   std::vector<uint32> c_Retval;
   c_Retval.reserve(mc_ConnectEntries.size());
   for (uint32 u32_ItEntry = 0; u32_ItEntry < mc_ConnectEntries.size(); ++u32_ItEntry)
   {
      const C_SyvUpProgressLogEntry * const pc_Entry = mc_ConnectEntries.at(u32_ItEntry);
      if (pc_Entry != NULL)
      {
         c_Retval.push_back(pc_Entry->GetNodeIndex());
      }
   }
   return c_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set heading visible
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvUpProgressLog::m_SetHeadingVisible(void) const
{
   if (this->mpc_Ui->pc_LabelPreview->isVisible() == false)
   {
      this->mpc_Ui->pc_LabelPreview->setVisible(true);
   }
   if (this->mpc_Ui->pc_Frame->isVisible() == false)
   {
      this->mpc_Ui->pc_Frame->setVisible(true);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Open log link
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvUpProgressLog::m_OpenLink(void) const
{
   const QString c_LogFilePath = C_OSCLoggingHandler::h_GetCompleteLogFileLocation().c_str();

   QDesktopServices::openUrl(QUrl(static_cast<QString>("file:%1").arg(c_LogFilePath)));
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Scroll to bottom

   \param[in] osn_Min New scroll bar minimum
   \param[in] osn_Max New scroll bar maximum
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvUpProgressLog::m_ScrollToBottom(const sintn osn_Min, const sintn osn_Max) const
{
   Q_UNUSED(osn_Min)
   this->mpc_Ui->pc_ScrollArea->verticalScrollBar()->setValue(osn_Max);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Setup context menu entries
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvUpProgressLog::m_SetupContextMenu(void)
{
   this->mpc_ContextMenu->clear();
   this->mpc_ContextMenu->addAction(C_GtGetText::h_GetText("Copy Link Location"),
                                    this,
                                    &C_SyvUpProgressLog::m_CopyHyperLink
                                    );
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Show custom context menu

   \param[in] orc_Pos Local context menu position
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvUpProgressLog::m_OnCustomContextMenuRequested(const QPoint & orc_Pos)
{
   m_SetupContextMenu(); // setup the custom menu here to have real "is-read-only" information
   QPoint c_PosGlobal = this->mpc_LogHyperlink->mapToGlobal(orc_Pos);

   this->mpc_ContextMenu->popup(c_PosGlobal);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Copy link location to clipboard
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvUpProgressLog::m_CopyHyperLink() const
{
   // get link location
   const QString c_LogFilePath = C_OSCLoggingHandler::h_GetCompleteLogFileLocation().c_str();

   QClipboard * const pc_Clipboard = QApplication::clipboard();

   if (pc_Clipboard != NULL)
   {
      //Copy to clipboard
      pc_Clipboard->clear();
      pc_Clipboard->setText(c_LogFilePath);
   }
}
