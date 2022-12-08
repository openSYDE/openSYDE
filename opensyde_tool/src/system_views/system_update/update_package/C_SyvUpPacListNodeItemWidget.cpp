//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Widget for showing a specific application or data block as part of a node

   \copyright   Copyright 2018 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#include <QFileInfo>
#include <QDir>

#include <cmath>

#include "C_SyvUpPacListNodeItemWidget.hpp"
#include "ui_C_SyvUpPacListNodeItemWidget.h"

#include "stwerrors.hpp"
#include "C_GtGetText.hpp"

#include "C_PuiProject.hpp"
#include "C_PuiSvData.hpp"
#include "C_PuiSvNodeUpdate.hpp"
#include "C_PuiSvHandler.hpp"
#include "TglUtils.hpp"
#include "C_Uti.hpp"
#include "C_PuiUtil.hpp"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw::errors;
using namespace stw::opensyde_gui;
using namespace stw::opensyde_gui_elements;
using namespace stw::opensyde_gui_logic;
using namespace stw::opensyde_core;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

/* -- Global Variables ---------------------------------------------------------------------------------------------- */

/* -- Module Global Variables --------------------------------------------------------------------------------------- */

/* -- Module Global Function Prototypes ----------------------------------------------------------------------------- */

/* -- Implementation ------------------------------------------------------------------------------------------------ */

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default constructor

   Set up GUI with all elements.

   \param[in]      ou32_ViewIndex      View index
   \param[in]      ou32_NodeIndex      Node index
   \param[in]      orc_DeviceName      Name of device for comparing with application block
   \param[in]      oq_FileBased        Flag if node is a file based device
   \param[in]      oq_StwFlashloader   Flag if node has not an openSYDE Flashloader but a STW Flashloader
   \param[in,out]  opc_Parent          Pointer to parent and the list which holds the widget. The original parent could
                                       be replaced by the layout handling of Qt. This must be the list widget which
                                       holds all items (base class must be C_SyvUpPackageSectionNodeWidget).
*/
//----------------------------------------------------------------------------------------------------------------------
C_SyvUpPacListNodeItemWidget::C_SyvUpPacListNodeItemWidget(const uint32_t ou32_ViewIndex, const uint32_t ou32_NodeIndex,
                                                           const QString & orc_DeviceName, const bool oq_FileBased,
                                                           const bool oq_StwFlashloader, QWidget * const opc_Parent) :
   C_OgeWiOnlyBackground(opc_Parent),
   mpc_Ui(new Ui::C_SyvUpPacListNodeItemWidget),
   mu32_ViewIndex(ou32_ViewIndex),
   mu32_NodeIndex(ou32_NodeIndex),
   mc_NodeDeviceType(orc_DeviceName),
   mc_AppDeviceType(""),
   mq_AppInfoAmbiguous(false),
   mq_FileBased(oq_FileBased),
   mq_StwFlashloader(oq_StwFlashloader),
   mq_SkipOfUpdateFile(false),
   mc_OwnerSectionName("Title"),
   mu32_Number(0U),
   mc_FilePath(""),
   mc_Date(""),
   mc_Time(""),
   mc_AbsoluteFilePath(""),
   mpc_ListParent(opc_Parent),
   mq_Selected(false),
   mc_DefaultFilePath(""),
   mq_DefaultFilePath(true),
   mq_DefaultFilePathUsed(false),
   mu32_State(hu32_STATE_DEFAULT),
   mq_Connected(false),
   mu32_FileSize(0U),
   ms32_PreviousFileResult(C_NO_ERR)
{
   mpc_Ui->setupUi(this);

   if (this->mq_FileBased == false)
   {
      this->mpc_Ui->pc_LabelVersion->setText(C_GtGetText::h_GetText("Missing"));
   }
   else
   {
      this->mpc_Ui->pc_LabelVersion->setText("");
   }

   this->mpc_Ui->pc_LabelUserHint->setText(C_GtGetText::h_GetText("User"));
   this->mpc_Ui->pc_LabelUserHint->SetFontPixel(10);
   this->mpc_Ui->pc_LabelUserHint->SetForegroundColor(0);
   this->mpc_Ui->pc_LabelUserHint->SetBackgroundColor(9);

   this->mpc_Ui->pc_LabelSkip->setText(C_GtGetText::h_GetText("Skip"));
   this->mpc_Ui->pc_LabelSkip->SetFontPixel(10);
   this->mpc_Ui->pc_LabelSkip->SetForegroundColor(0);
   this->mpc_Ui->pc_LabelSkip->SetBackgroundColor(24);
   this->mpc_Ui->pc_LabelSkip->setVisible(false);

   // Will be handled in specific derived items
   this->mpc_Ui->pc_LabelSecurity->setVisible(false);
   this->mpc_Ui->pc_LabelDebugger->setVisible(false);

   this->mpc_Ui->pc_LabelPath->setText(C_GtGetText::h_GetText("<Add File>"));
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   default destructor

   Clean up.
*/
//----------------------------------------------------------------------------------------------------------------------
C_SyvUpPacListNodeItemWidget::~C_SyvUpPacListNodeItemWidget()
{
   delete mpc_Ui;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Returns the list parent, which is set in the parent constructor parameter

   In case of a changed parent, this function returns the first parent

   \return
   Pointer to list widget
*/
//----------------------------------------------------------------------------------------------------------------------
QWidget * C_SyvUpPacListNodeItemWidget::GetListParent(void)
{
   return this->mpc_ListParent;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Sets the selected state

   \param[in]  oq_Selected    Flag if selected
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvUpPacListNodeItemWidget::SetSelected(const bool oq_Selected)
{
   this->m_SetSelectColor(oq_Selected);

   this->mq_Selected = oq_Selected;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Sets the state of the application

   The icon will be adapted

   \param[in]  ou32_State  State of application
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvUpPacListNodeItemWidget::SetState(const uint32_t ou32_State)
{
   this->mu32_State = ou32_State;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Sets the name of the owner section for tooltip

   \param[in]       orc_Name     Name of section
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvUpPacListNodeItemWidget::SetOwnerSectionName(const QString & orc_Name)
{
   this->mc_OwnerSectionName = orc_Name;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Sets application number

   Adapts the background of the widget

   \param[in]  ou32_Number    Application number
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvUpPacListNodeItemWidget::SetAppNumber(const uint32_t ou32_Number)
{
   this->mu32_Number = ou32_Number;

   this->SetBackgroundColor(0);

   this->m_UpdateToolTip();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Sets application path.

   Can be relative to openSYDE project. In case of a data block project the data block project path must be included.

   \param[in]  orc_File          Application path
   \param[in]  oq_DefaultFile    Flag if default or specific view path is used
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvUpPacListNodeItemWidget::SetAppFile(const QString & orc_File, const bool oq_DefaultFile)
{
   const QFileInfo c_FileInfo(orc_File);
   bool q_ShowUserHint;

   if ((orc_File.isEmpty() == false) && (c_FileInfo.isDir() == false))
   {
      this->mc_FilePath = orc_File;
   }
   else
   {
      this->mc_FilePath = C_GtGetText::h_GetText("<Add File>");
   }

   this->mq_DefaultFilePath = oq_DefaultFile;

   // Update the state of user (system view specific configuration over system definition) hint
   q_ShowUserHint = (!this->mq_DefaultFilePath) && this->mq_DefaultFilePathUsed;
   this->mpc_Ui->pc_LabelUserHint->setVisible(q_ShowUserHint);
   this->mpc_Ui->pc_HorizontalSpacerUserHint->changeSize(((q_ShowUserHint == true) ? static_cast<int32_t>(15) : 0),
                                                         0, QSizePolicy::Fixed);

   this->m_UpdateAbsolutePath();
   this->m_UpateFilePathLabel();
   this->m_UpdateToolTip();

   // Reset timestamp to a null (invalid) timestamp value for check
   this->mc_FileTimestamp = QDateTime();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Sets the default path for comparing with an import configuration

   \param[in]  orc_File    Application default path
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvUpPacListNodeItemWidget::SetDefaultFile(const QString & orc_File)
{
   this->mc_DefaultFilePath = orc_File;
   this->mq_DefaultFilePathUsed = true;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Adapts the widget for the connected state

   \param[in]  oq_Connected   Flag if node with this application is connected
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvUpPacListNodeItemWidget::SetConnected(const bool oq_Connected)
{
   this->mq_Connected = oq_Connected;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Sets the skip update file flag

   \param[in]       oq_Skip     New skip flag
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvUpPacListNodeItemWidget::SetSkipOfUpdateFile(const bool oq_Skip)
{
   this->mq_SkipOfUpdateFile = oq_Skip;
   this->mpc_Ui->pc_LabelSkip->setVisible(oq_Skip);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Returns current state of application

   \return
   Current state of application
*/
//----------------------------------------------------------------------------------------------------------------------
uint32_t C_SyvUpPacListNodeItemWidget::GetState(void) const
{
   uint32_t u32_State = this->mu32_State;

   if (this->mq_SkipOfUpdateFile == true)
   {
      // Special case: Handle file as finished to skip the file
      u32_State = C_SyvUpPacListNodeItemWidget::hu32_STATE_FINISHED;
   }

   return u32_State;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Returns the application number

   \return
   Application number
*/
//----------------------------------------------------------------------------------------------------------------------
uint32_t C_SyvUpPacListNodeItemWidget::GetAppNumber(void) const
{
   return this->mu32_Number;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Returns the file name with extension without path

   \return
   File name
*/
//----------------------------------------------------------------------------------------------------------------------
QString C_SyvUpPacListNodeItemWidget::GetAppFileName(void) const
{
   const QFileInfo c_File(this->mc_FilePath);

   return c_File.fileName();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Returns the application path

   \return
   Application path
*/
//----------------------------------------------------------------------------------------------------------------------
QString C_SyvUpPacListNodeItemWidget::GetAppFilePath(void) const
{
   return this->mc_FilePath;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Returns the application path

   \return
   Default application path
*/
//----------------------------------------------------------------------------------------------------------------------
QString C_SyvUpPacListNodeItemWidget::GetDefaultFilePath(void) const
{
   return this->mc_DefaultFilePath;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Returns the absolute application path

   \return
   Application path
*/
//----------------------------------------------------------------------------------------------------------------------
QString C_SyvUpPacListNodeItemWidget::GetAppAbsoluteFilePath(void) const
{
   return this->mc_AbsoluteFilePath;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Return device name read from HEX file.

   We support different names if node is of STW Flashloader type.

   \return
   device name from application
*/
//----------------------------------------------------------------------------------------------------------------------
QString C_SyvUpPacListNodeItemWidget::GetAppDeviceType(void) const
{
   return this->mc_AppDeviceType;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Returns the skip update file flag

   \retval   true    Skip the file of this item in the update process
   \retval   false   Do not skip the file of this item in the update process
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_SyvUpPacListNodeItemWidget::GetSkipOfUpdateFile(void) const
{
   return this->mq_SkipOfUpdateFile;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Returns if the application information is ambiguous

   In fact this only occurs for Data blocks, but we need the information above the whole update package and do
   not want to cast there (choose lesser evil).

   \retval   true    application information is ambiguous (because HEX file contained multiple application blocks)
   \retval   false   application information is useful
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_SyvUpPacListNodeItemWidget::IsAppInfoAmbiguous(void) const
{
   return this->mq_AppInfoAmbiguous;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Returns if the default file path of system definition is used

   \retval   true    Default file path is active
   \retval   false   Default file path is not active
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_SyvUpPacListNodeItemWidget::IsDefaultPathActive(void) const
{
   return this->mq_DefaultFilePath;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Prepares the update package node configuration

   \param[out]  orc_AppConfig    Node configuration
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvUpPacListNodeItemWidget::PrepareExportConfig(C_SyvUpPacConfigNodeApp & orc_AppConfig)
const
{
   orc_AppConfig.c_Path = this->mc_FilePath;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Loads all matching configurations for this application of the import node configuration

   \param[in]  orc_AppConfig    Import configuration
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvUpPacListNodeItemWidget::LoadImportConfig(const C_SyvUpPacConfigNodeApp & orc_AppConfig)
{
   const QString c_Path = orc_AppConfig.c_Path;

   // Configuration found and check for default path
   this->SetAppFile(c_Path, (c_Path == this->mc_DefaultFilePath));

   // Data handling
   tgl_assert(C_PuiSvHandler::h_GetInstance()->SetNodeUpdateInformationPath(
                 this->mu32_ViewIndex,
                 this->mu32_NodeIndex,
                 this->mu32_Number, c_Path,
                 C_PuiSvNodeUpdate::eFTP_DATA_BLOCK) == C_NO_ERR);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Checks the path for existence and updates the file information if necessary

   \return
   C_NO_ERR    All files are existent
   C_CONFIG    File was not found
   C_WARN      Flashware warning occured (only relevant for STW Flashloader; wrong device name in hex file)
   C_TIMEOUT   Trigger remove of this widget
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_SyvUpPacListNodeItemWidget::CheckPath(void)
{
   int32_t s32_Return = C_NO_ERR;
   QFileInfo c_FileInfo;
   bool q_Changed = true;

   // Check file for changes
   c_FileInfo.setFile(this->mc_AbsoluteFilePath);

   if ((c_FileInfo.exists() == true) &&
       (c_FileInfo.isFile() == true))
   {
      const QDateTime c_Timestamp = c_FileInfo.lastModified();

      if (this->mc_FileTimestamp == c_Timestamp)
      {
         q_Changed = false;
      }
      else
      {
         this->mc_FileTimestamp = c_Timestamp;
      }
   }
   else
   {
      // Reset timestamp to a null (invalid) timestamp value
      this->mc_FileTimestamp = QDateTime();
   }

   if (q_Changed == true)
   {
      bool q_FileExists;
      bool q_FlashwareWarning;
      bool q_TriggerRemoveFile;
      this->m_LoadFileInformation(q_FileExists, q_FlashwareWarning, q_TriggerRemoveFile);
      this->m_UpdateToolTip();
      if (q_FileExists == false)
      {
         s32_Return = C_CONFIG;
      }
      if (q_FlashwareWarning == true)
      {
         s32_Return = C_WARN;
      }
      if (q_TriggerRemoveFile == true)
      {
         s32_Return = C_TIMEOUT;
      }
      this->ms32_PreviousFileResult = s32_Return;
   }
   else
   {
      s32_Return = this->ms32_PreviousFileResult;
   }

   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Checks if the application is identical with the parameters

   \param[in]  orc_AppName       Application / Project name (Not the name of the system definition)
   \param[in]  orc_AppVersion    Application version
   \param[in]  orc_AppBuildTime  Application build time
   \param[in]  orc_AppBuildDate  Application build date

   \return
   true     Is identical
   false    Is not identical
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_SyvUpPacListNodeItemWidget::IsFileIdentical(const QString & orc_AppName, const QString & orc_AppVersion,
                                                   const QString & orc_AppBuildTime,
                                                   const QString & orc_AppBuildDate) const
{
   Q_UNUSED(orc_AppName)

   bool q_Return = false;

   if ((orc_AppName != "") &&
       (orc_AppVersion != "") &&
       (orc_AppBuildTime != "") &&
       (orc_AppBuildDate != "") &&
       (orc_AppBuildTime == this->mc_Time) &&
       (orc_AppBuildDate == this->mc_Date))
   {
      q_Return = true;
   }

   return q_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Shows file information

   Default implementation does nothing
*/
//----------------------------------------------------------------------------------------------------------------------
//lint -e{9175}  //intentionally no functionality in default implementation
void C_SyvUpPacListNodeItemWidget::ViewFileInfo(void)
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Overwritten show event slot

   Here: Adapt the path text

   \param[in,out]  opc_Event  Event identification and information
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvUpPacListNodeItemWidget::resizeEvent(QResizeEvent * const opc_Event)
{
   C_OgeWiOnlyBackground::resizeEvent(opc_Event);

   // Update the label text for adapting the elided text if necessary
   this->m_UpateFilePathLabel();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Overwritten default event slot

   Here: Handle background color on mouse hover

   \param[in,out]  opc_Event  Event identification and information

   \return
   True  Event was recognized and processed
   False Event ignored
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_SyvUpPacListNodeItemWidget::event(QEvent * const opc_Event)
{
   if (this->mq_Selected == false)
   {
      if (opc_Event->type() == QEvent::Leave)
      {
         this->m_SetSelectColor(false);
      }
      else if (opc_Event->type() == QEvent::Enter)
      {
         this->m_SetSelectColor(true);
      }
      else
      {
         // Nothing to do
      }
   }

   return stw::opensyde_gui_elements::C_OgeWiOnlyBackground::event(opc_Event);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Load application information.

   \param[out]  orq_FileExists         true: file exists  false: file not found
   \param[out]  orq_FlashwareWarning   true: flashware warning occured
                                       (only relevant for STW Flashloader; wrong device name in hex file)
                                       false: no warnings occured
   \param[out]  orq_TriggerRemove      true: remove item widget
                                       (only relevant if user discards file)
                                       false: keep widget
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvUpPacListNodeItemWidget::m_LoadFileInformation(bool & orq_FileExists, bool & orq_FlashwareWarning,
                                                         bool & orq_TriggerRemove)
{
   const QFileInfo c_FileInfo(this->mc_AbsoluteFilePath);

   orq_FlashwareWarning = false;
   orq_TriggerRemove = false;
   orq_FileExists = c_FileInfo.exists() && c_FileInfo.isFile();

   // Default text
   this->mc_Date = C_GtGetText::h_GetText("NA");
   this->mc_Time = "";
   this->mu32_FileSize = 0U;

   if (orq_FileExists == true)
   {
      float64_t f64_SizeFloat;
      const QDateTime c_DateTime = c_FileInfo.lastModified();

      // File size in kB
      f64_SizeFloat = std::ceil(static_cast<float64_t>(c_FileInfo.size()) / 1024.0);
      // 64 Bit is not necessary for our hex files
      this->mu32_FileSize = static_cast<uint32_t>(f64_SizeFloat);

      // General date format (hex files have special format and overwrite this value)
      this->mc_Date = c_DateTime.date().toString("dd.MM.yyyy");
      this->mc_Time = c_DateTime.time().toString("h:mm:ss");

      this->mpc_Ui->pc_LabelVersion->setText("");
   }
   else
   {
      this->mpc_Ui->pc_LabelVersion->setText(C_GtGetText::h_GetText("Missing"));
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Creates and returns the title specific tool tip content

   \return
   Tool tip title
*/
//----------------------------------------------------------------------------------------------------------------------
QString C_SyvUpPacListNodeItemWidget::m_CreateToolTipTitle(void) const
{
   QString c_Title = static_cast<QString>("%1: File #%2").
                     arg(this->mc_OwnerSectionName).
                     arg(this->mu32_Number + 1U);

   return c_Title;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Creates and returns the content specific tool tip content

   \return
   Tool tip content
*/
//----------------------------------------------------------------------------------------------------------------------
QString C_SyvUpPacListNodeItemWidget::m_CreateToolTipContent(void) const
{
   QString c_Content = "";

   c_Content += C_GtGetText::h_GetText("Last modified: ");
   c_Content += this->mc_Date + " " + this->mc_Time;

   return c_Content;
}

//----------------------------------------------------------------------------------------------------------------------
void C_SyvUpPacListNodeItemWidget::m_UpateFilePathLabel(void) const
{
   if ((this->mc_AbsoluteFilePath != "") &&
       (this->mc_AbsoluteFilePath != C_GtGetText::h_GetText("<Add File>")))
   {
      //Use topmost widget (in list) as reference for width
      int32_t s32_Width = this->parentWidget()->parentWidget()->width() - mhs32_PATH_OFFSET;
      QString c_AdaptedString;

      if (this->mpc_Ui->pc_LabelUserHint->isVisible() == true)
      {
         // In this case the label for the path does not have so much space to use
         s32_Width -= this->mpc_Ui->pc_LabelUserHint->width();
      }

      if (this->mpc_Ui->pc_LabelSecurity->isVisible() == true)
      {
         // In this case the label for the path does not have so much space to use
         s32_Width -= this->mpc_Ui->pc_LabelSecurity->width();
      }

      if (this->mpc_Ui->pc_LabelDebugger->isVisible() == true)
      {
         // In this case the label for the path does not have so much space to use
         s32_Width -= this->mpc_Ui->pc_LabelDebugger->width();
      }
      if (((this->mpc_Ui->pc_LabelUserHint->isVisible() == true) ||
           (this->mpc_Ui->pc_LabelDebugger->isVisible() == true)) ||
          (this->mpc_Ui->pc_LabelSecurity->isVisible() == true))
      {
         // In this case the label for the path does not have so much space to use
         s32_Width -= this->mpc_Ui->pc_HorizontalSpacerUserHint->geometry().width();
      }

      c_AdaptedString =
         C_Uti::h_MinimizePath(this->mc_FilePath,
                               this->mpc_Ui->pc_LabelPath->font(), static_cast<uint32_t>(s32_Width), 0U);
      this->mpc_Ui->pc_LabelPath->setText(c_AdaptedString);
   }
   else
   {
      this->mpc_Ui->pc_LabelPath->setText(C_GtGetText::h_GetText("<Add File>"));
   }
}

//----------------------------------------------------------------------------------------------------------------------
void C_SyvUpPacListNodeItemWidget::m_UpdateAbsolutePath(void)
{
   if (this->mc_FilePath == C_GtGetText::h_GetText("<Add File>"))
   {
      this->mc_AbsoluteFilePath = this->mc_FilePath;
   }
   else
   {
      this->mc_AbsoluteFilePath = C_PuiUtil::h_GetResolvedAbsPathFromProject(this->mc_FilePath);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Updated of the item tool tip
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvUpPacListNodeItemWidget::m_UpdateToolTip(void)
{
   QString c_Content = this->m_CreateToolTipContent();
   const QString c_Title = this->m_CreateToolTipTitle();

   // Default tool tip part
   if (this->mq_DefaultFilePathUsed == true)
   {
      if (this->mq_DefaultFilePath == false)
      {
         c_Content += C_GtGetText::h_GetText("\nFile path origin: Defined by User");
      }
      else
      {
         c_Content += C_GtGetText::h_GetText("\nFile path origin: Default, defined in Data Block properties");
      }
   }

   if (this->mc_FilePath == this->mc_AbsoluteFilePath)
   {
      c_Content += C_GtGetText::h_GetText("\nFile path: ");
      c_Content += this->mc_AbsoluteFilePath;
   }
   else
   {
      c_Content += C_GtGetText::h_GetText("\nFile path: ");
      c_Content += this->mc_FilePath;
      c_Content += C_GtGetText::h_GetText("\nFile path absolute: ");
      c_Content += this->mc_AbsoluteFilePath;
   }

   c_Content += C_GtGetText::h_GetText("\nFile size: ");

   c_Content += QString::number(this->mu32_FileSize);
   c_Content += C_GtGetText::h_GetText(" kB");

   this->SetToolTipInformation(c_Title, c_Content);
}

//----------------------------------------------------------------------------------------------------------------------
void C_SyvUpPacListNodeItemWidget::m_SetSelectColor(const bool oq_Active)
{
   if ((oq_Active == true) &&
       (this->mq_Connected == false))
   {
      this->SetBackgroundColor(10);
   }
   else
   {
      this->SetAppNumber(this->mu32_Number);
   }
}
