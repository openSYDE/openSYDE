//-----------------------------------------------------------------------------
/*!
   \internal
   \file
   \brief       Widget for showing a specific application or data block as part of a node

   \implementation
   project     project name
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     09.02.2018  STW/B.Bayer
   \endimplementation
*/
//-----------------------------------------------------------------------------
#include <QFileInfo>

#include <cmath>

#include "C_SyvUpUpdatePackageListNodeAppWidget.h"
#include "ui_C_SyvUpUpdatePackageListNodeAppWidget.h"

#include "stwerrors.h"
#include "C_GtGetText.h"

#include "C_OsyHexFile.h"
#include "C_PuiProject.h"
#include "C_PuiSvData.h"
#include "C_PuiSvNodeUpdate.h"
#include "C_PuiSvHandler.h"
#include "TGLUtils.h"
#include "C_OgeWiCustomMessage.h"
#include "C_Uti.h"

/* -- Used Namespaces ------------------------------------------------------ */
using namespace stw_types;
using namespace stw_errors;
using namespace stw_opensyde_gui;
using namespace stw_opensyde_gui_elements;
using namespace stw_opensyde_gui_logic;
using namespace stw_opensyde_core;

/* -- Module Global Constants ---------------------------------------------- */

/* -- Types ---------------------------------------------------------------- */

/* -- Global Variables ----------------------------------------------------- */

/* -- Module Global Variables ---------------------------------------------- */

/* -- Module Global Function Prototypes ------------------------------------ */

/* -- Implementation ------------------------------------------------------- */

//-----------------------------------------------------------------------------
/*!
   \brief   Default constructor

   Set up GUI with all elements.

   \param[in]     ou32_ViewIndex       View index
   \param[in]     ou32_NodeIndex       Node index
   \param[in]     orc_DeviceName       Name of device for comparing with application block
   \param[in]     oq_FileBased         Flag if node is a file based device
   \param[in]     oq_StwFlashloader    Flag if node has not an openSYDE flashloader but a STW flashloader
   \param[in,out] opc_parent           Optional pointer to parent

   \created     09.02.2018  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
C_SyvUpUpdatePackageListNodeAppWidget::C_SyvUpUpdatePackageListNodeAppWidget(const uint32 ou32_ViewIndex,
                                                                             const uint32 ou32_NodeIndex,
                                                                             const QString & orc_DeviceName,
                                                                             const bool oq_FileBased,
                                                                             const bool oq_StwFlashloader,
                                                                             QWidget * const opc_Parent) :
   C_OgeWiOnlyBackground(opc_Parent),
   mpc_Ui(new Ui::C_SyvUpUpdatePackageListNodeAppWidget),
   mu32_ViewIndex(ou32_ViewIndex),
   mu32_NodeIndex(ou32_NodeIndex),
   mc_DeviceName(orc_DeviceName),
   mq_Selected(false),
   mc_Name(""),
   mc_Title(""),
   me_Type(stw_opensyde_core::C_OSCNodeApplication::eBINARY),
   mu32_Number(0U),
   mc_FilePath(""),
   mc_DefaultFilePath(""),
   mq_DefaultFilePath(true),
   mq_FileBased(oq_FileBased),
   mq_StwFlashloader(oq_StwFlashloader),
   //lint -e{1938}  static const is guaranteed preinitialized before main
   mu32_State(hu32_STATE_DEFAULT),
   mq_Connected(false),
   mc_ProjectName("NA"),
   mc_Date(""),
   mc_Time(""),
   mc_AbsoluteFilePath(""),
   mc_Version(""),
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

   this->mpc_Ui->pc_LabelPath->setText(C_GtGetText::h_GetText("<Add File>"));

   //lint -e{1938}  static const is guaranteed preinitialized before main
   this->SetState(hu32_STATE_DEFAULT);
}

//-----------------------------------------------------------------------------
/*!
   \brief   default destructor

   Clean up.

   \created     09.02.2018  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
C_SyvUpUpdatePackageListNodeAppWidget::~C_SyvUpUpdatePackageListNodeAppWidget()
{
   delete mpc_Ui;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Sets the selected state

   \param[in]     oq_Selected         Flag if selected

   \created     13.02.2018  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
void C_SyvUpUpdatePackageListNodeAppWidget::SetSelected(const bool oq_Selected)
{
   this->m_SetSelectColor(oq_Selected);

   this->mq_Selected = oq_Selected;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Sets the state of the application

   The icon will be adapted

   \param[in]     ou32_State         State of application

   \created     dd.02.2018  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
void C_SyvUpUpdatePackageListNodeAppWidget::SetState(const uint32 ou32_State)
{
   QSize c_Size(30, 30);

   this->mu32_State = ou32_State;

   switch (ou32_State)
   {
   case hu32_STATE_DEFAULT:
      if (this->mq_FileBased == false)
      {
         this->mpc_Ui->pc_LabelAppIcon->setPixmap(QIcon(":images/system_views/IconApp.svg").pixmap(c_Size));
      }
      else
      {
         this->mpc_Ui->pc_LabelAppIcon->setPixmap(QIcon(":images/system_views/IconFile.svg").pixmap(c_Size));
      }
      break;
   case hu32_STATE_ERROR:
      if (this->mq_FileBased == false)
      {
         this->mpc_Ui->pc_LabelAppIcon->setPixmap(QIcon(":images/system_views/IconAppError.svg").pixmap(c_Size));
      }
      else
      {
         this->mpc_Ui->pc_LabelAppIcon->setPixmap(QIcon(":images/system_views/IconFileError.svg").pixmap(c_Size));
      }
      break;
   case hu32_STATE_FINISHED:
      if (this->mq_FileBased == false)
      {
         this->mpc_Ui->pc_LabelAppIcon->setPixmap(QIcon(":images/system_views/IconAppFinished.svg").pixmap(c_Size));
      }
      else
      {
         this->mpc_Ui->pc_LabelAppIcon->setPixmap(QIcon(":images/system_views/IconFileFinished.svg").pixmap(c_Size));
      }
      break;
   case hu32_STATE_IN_PROGRESS:
      if (this->mq_FileBased == false)
      {
         this->mpc_Ui->pc_LabelAppIcon->setPixmap(QIcon(":images/system_views/IconAppInProgress.svg").pixmap(c_Size));
      }
      else
      {
         this->mpc_Ui->pc_LabelAppIcon->setPixmap(QIcon(":images/system_views/IconFileInProgress.svg").pixmap(c_Size));
      }
      break;
   case hu32_STATE_TO_DO:
      if (this->mq_FileBased == false)
      {
         this->mpc_Ui->pc_LabelAppIcon->setPixmap(QIcon(":images/system_views/IconAppToDo.svg").pixmap(c_Size));
      }
      else
      {
         this->mpc_Ui->pc_LabelAppIcon->setPixmap(QIcon(":images/system_views/IconFileToDo.svg").pixmap(c_Size));
      }
      break;
   default:
      this->mpc_Ui->pc_LabelAppIcon->setPixmap(QIcon(":images/system_views/IconApp.svg").pixmap(c_Size));
      break;
   }
}

//-----------------------------------------------------------------------------
/*!
   \brief   Sets application name

   \param[in]     orc_Name         Application name

   \created     13.02.2018  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
void C_SyvUpUpdatePackageListNodeAppWidget::SetAppName(const QString & orc_Name)
{
   this->mc_Name = orc_Name;

   this->m_UpdateTitle();
}

//-----------------------------------------------------------------------------
/*!
   \brief   Sets application number

   Adapts the background of the widget

   \param[in]     ou32_Number         Application number

   \created     13.02.2018  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
void C_SyvUpUpdatePackageListNodeAppWidget::SetAppNumber(const stw_types::uint32 ou32_Number)
{
   this->mu32_Number = ou32_Number;

   if ((this->mu32_Number % 2) == 0)
   {
      this->SetBackgroundColor(11);
   }
   else
   {
      this->SetBackgroundColor(12);
   }

   this->m_UpdateTitle();
}

//-----------------------------------------------------------------------------
/*!
   \brief   Sets application path

   \param[in]     orc_Path         Application path
   \param[in]     oq_DefaultPath   Flag if default or specific view path is used

   \created     13.02.2018  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
void C_SyvUpUpdatePackageListNodeAppWidget::SetAppFile(const QString & orc_File, const bool oq_DefaultFile)
{
   this->mc_FilePath = orc_File;
   this->mq_DefaultFilePath = oq_DefaultFile;

   this->m_UpdateAbsolutePath();
   this->m_UpateFilePathLabel();

   // Reset timestamp to a null (invalid) timestamp value for check
   this->mc_FileTimestamp = QDateTime();
}

//-----------------------------------------------------------------------------
/*!
   \brief   Sets the default path for comparing with an import configuration

   \param[in]     orc_Path         Application default path

   \created     21.02.2018  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
void C_SyvUpUpdatePackageListNodeAppWidget::SetDefaultFile(const QString & orc_File)
{
   this->mc_DefaultFilePath = orc_File;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Sets application type

   \param[in]     oe_Type         Application type

   \created     20.02.2018  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
void C_SyvUpUpdatePackageListNodeAppWidget::SetAppType(const C_OSCNodeApplication::E_Type oe_Type)
{
   this->me_Type = oe_Type;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Adapts the widget for the connected state

   \param[in]  oq_Connected   Flag if node with this application is connected

   \created     31.07.2018  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
void C_SyvUpUpdatePackageListNodeAppWidget::SetConnected(const bool oq_Connected)
{
   this->mq_Connected = oq_Connected;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Returns current state of application

   \return
   Current state of application

   \created     26.02.2018  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
uint32 C_SyvUpUpdatePackageListNodeAppWidget::GetState(void) const
{
   return this->mu32_State;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Returns the application name

   \return
   Application name

   \created     13.02.2018  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
QString C_SyvUpUpdatePackageListNodeAppWidget::GetAppName(void) const
{
   return this->mc_Name;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Returns the application number

   \return
   Application number

   \created     13.02.2018  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
uint32 C_SyvUpUpdatePackageListNodeAppWidget::GetAppNumber(void) const
{
   return this->mu32_Number;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Returns the application path

   \return
   Application path

   \created     13.02.2018  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
QString C_SyvUpUpdatePackageListNodeAppWidget::GetAppFilePath(void) const
{
   return this->mc_FilePath;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Returns the absolute application path

   \return
   Application path

   \created     13.02.2018  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
QString C_SyvUpUpdatePackageListNodeAppWidget::GetAppAbsoluteFilePath(void) const
{
   return this->mc_AbsoluteFilePath;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Returns if the default path is used

   \return
   true        Default path is used
   false       View path is used

   \created     15.02.2018  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
bool C_SyvUpUpdatePackageListNodeAppWidget::IsDefaultPathActive(void) const
{
   return this->mq_DefaultFilePath;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Checks if the application file is missing

   \return
   true        Application file is missing
   false       Application file exists

   \created     22.08.2018  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
bool C_SyvUpUpdatePackageListNodeAppWidget::IsAppFileMissing(void) const
{
   bool q_Return = false;

   if (this->mpc_Ui->pc_LabelVersion->text() == C_GtGetText::h_GetText("Missing"))
   {
      q_Return = true;
   }

   return q_Return;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Prepares the update package node configuration

   \param[out]     orc_AppConfig         Node configuration

   \created     20.02.2018  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
void C_SyvUpUpdatePackageListNodeAppWidget::PrepareExportConfig(C_SyvUpUpdatePackageConfigNodeApp & orc_AppConfig) const
{
   orc_AppConfig.c_Name = this->mc_Name;
   orc_AppConfig.c_Path = this->mc_FilePath;
   orc_AppConfig.e_Type = this->me_Type;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Loads all matching configurations for this application of the import node configuration

   \param[out]     orc_Config         Import configuration

   \created     21.02.2018  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
void C_SyvUpUpdatePackageListNodeAppWidget::LoadImportConfig(const C_SyvUpUpdatePackageConfigNode & orc_NodeConfig)
{
   uint32 u32_ConfigCounter;

   // Search the matching configuration
   for (u32_ConfigCounter = 0; u32_ConfigCounter < orc_NodeConfig.c_AppConfigs.size(); ++u32_ConfigCounter)
   {
      if ((orc_NodeConfig.c_AppConfigs[u32_ConfigCounter].c_Name == this->mc_Name) &&
          (orc_NodeConfig.c_AppConfigs[u32_ConfigCounter].e_Type == this->me_Type))
      {
         const QString c_Path = orc_NodeConfig.c_AppConfigs[u32_ConfigCounter].c_Path;

         // Configuration found and check for default path
         this->SetAppFile(c_Path, (c_Path == this->mc_DefaultFilePath));
         break;
      }
   }
}

//-----------------------------------------------------------------------------
/*!
   \brief   Checks the path for existence and updates the file information if necessary

   \return
   C_NO_ERR    All files are existent
   C_CONFIG    Minimum one file was not found

   \created     16.02.2018  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
sint32 C_SyvUpUpdatePackageListNodeAppWidget::CheckPath(void)
{
   sint32 s32_Return = C_NO_ERR;
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
      if (this->m_LoadApplicationInformation() == false)
      {
         s32_Return = C_CONFIG;
      }
      this->ms32_PreviousFileResult = s32_Return;
   }
   else
   {
      s32_Return = this->ms32_PreviousFileResult;
   }

   return s32_Return;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Checks if the application is identical with the parameters

   \param[in]     orc_AppName              Application / Project name (Not the name of the system definition)
   \param[in]     orc_AppVersion           Application version
   \param[in]     orc_AppBuildTime         Application build time
   \param[in]     orc_AppBuildDate         Application build date

   \return
   true     Is identical
   false    Is not identical

   \created     23.02.2018  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
bool C_SyvUpUpdatePackageListNodeAppWidget::IsAppIdentical(const QString & orc_AppName, const QString & orc_AppVersion,
                                                           const QString & orc_AppBuildTime,
                                                           const QString & orc_AppBuildDate) const
{
   bool q_Return = false;

   if ((orc_AppName != "") &&
       (orc_AppVersion != "") &&
       (orc_AppBuildTime != "") &&
       (orc_AppBuildDate != "") &&
       (orc_AppName == this->mc_ProjectName) &&
       (orc_AppVersion == this->mc_Version) &&
       (orc_AppBuildTime == this->mc_Time) &&
       (orc_AppBuildDate == this->mc_Date))
   {
      q_Return = true;
   }

   return q_Return;
}

//-----------------------------------------------------------------------------
/*!
   \brief   short description of function

   long description of function within several lines

   \param[in]     ou8_Aa         input parameter description
   \param[out]    opu32_Bb       output parameter description
   \param[in,out] opu16_Cc       input/output parameter description

   \return
   possible return value(s) and description

   \created     23.07.2018  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
bool C_SyvUpUpdatePackageListNodeAppWidget::h_GetAbsolutePath(const QString & orc_InputPath, QString & orc_OutputPath)
{
   QFileInfo c_FileInfo;
   bool q_FileExists;

   orc_OutputPath = orc_InputPath;
   c_FileInfo.setFile(orc_OutputPath);
   q_FileExists = (c_FileInfo.exists() && c_FileInfo.isFile());
   if (q_FileExists == false)
   {
      // Check the path if the path was shortened
      orc_OutputPath = C_PuiProject::h_GetInstance()->GetFolderPath() + orc_OutputPath;
      c_FileInfo.setFile(orc_OutputPath);
      q_FileExists = c_FileInfo.exists() && c_FileInfo.isFile();

      if (q_FileExists == false)
      {
         orc_OutputPath = orc_InputPath;
      }
   }

   return q_FileExists;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Overwritten show event slot

   Here: Adapt the path text

   \param[in,out] opc_Event Event identification and information

   \created     dd.01.2018  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
void C_SyvUpUpdatePackageListNodeAppWidget::resizeEvent(QResizeEvent * const opc_Event)
{
   C_OgeWiOnlyBackground::resizeEvent(opc_Event);

   // Update the label text for adapting the elided text if necessary
   this->m_UpdateTitle();
   this->m_UpateFilePathLabel();
}

//-----------------------------------------------------------------------------
/*!
   \brief   Overwritten default event slot

   Here: Handle background color on mouse hover

   \param[in,out] opc_Event Event identification and information

   \return
   True  Event was recognized and processed
   False Event ignored

   \created     19.07.2018  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
bool C_SyvUpUpdatePackageListNodeAppWidget::event(QEvent * const opc_Event)
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

   return stw_opensyde_gui_elements::C_OgeWiOnlyBackground::event(opc_Event);
}

//-----------------------------------------------------------------------------
bool C_SyvUpUpdatePackageListNodeAppWidget::m_UpdateAbsolutePath(void)
{
   const bool q_FileExists = C_SyvUpUpdatePackageListNodeAppWidget::h_GetAbsolutePath(this->mc_FilePath,
                                                                                      this->mc_AbsoluteFilePath);

   return q_FileExists;
}

//-----------------------------------------------------------------------------
void C_SyvUpUpdatePackageListNodeAppWidget::m_UpdateTitle(void)
{
   this->mc_Title = QString("#%1 - %2").arg(QString::number(this->mu32_Number + 1U),
                                            this->mc_Name);

   this->mpc_Ui->pc_LabelTitle->setText(C_Uti::h_AdaptStringToSize(this->mc_Title,
                                                                   this->mpc_Ui->pc_LabelTitle->fontMetrics(),
                                                                   this->width() - mhsn_TitleOffset));
}

//-----------------------------------------------------------------------------
bool C_SyvUpUpdatePackageListNodeAppWidget::m_LoadApplicationInformation(void)
{
   bool q_FileExists = this->m_UpdateAbsolutePath();

   // Default text
   this->mc_ProjectName = C_GtGetText::h_GetText("NA");
   this->mc_Version = C_GtGetText::h_GetText("NA");
   this->mc_Date = C_GtGetText::h_GetText("NA");
   this->mc_Time = "";
   this->mu32_FileSize = 0U;

   if (q_FileExists == true)
   {
      bool q_Reset = false;
      QFileInfo c_FileInfo;
      float64 f64_SizeFloat;

      c_FileInfo.setFile(this->mc_AbsoluteFilePath);

      // File size in kB
      f64_SizeFloat = std::ceil(static_cast<float64>(c_FileInfo.size()) / 1024.0);
      // 64 Bit is not necessary for our hex files
      this->mu32_FileSize = static_cast<uint32>(f64_SizeFloat);

      if (this->mq_FileBased == false)
      {
         C_OsyHexFile * const pc_HexFile = new C_OsyHexFile();

         // File information
         if (pc_HexFile->LoadFromFile(this->mc_AbsoluteFilePath.toStdString().c_str()) == stw_hex_file::NO_ERR)
         {
            stw_diag_lib::C_XFLECUInformation c_FileApplicationInfo;

            if (pc_HexFile->ScanApplicationInformationBlockFromHexFile(c_FileApplicationInfo) == C_NO_ERR)
            {
               // openSYDE nodes must use HEX files with matching devices
               // STW flashloader must not match. A warning in C_SyvUpUpdatePackageListNodeWidget will be shown when
               // choosing a HEX file with a different device name for STW flashloader node.
               if ((this->mq_StwFlashloader == true) ||
                   (this->mc_DeviceName == c_FileApplicationInfo.acn_DeviceID))
               {
                  // Project name
                  this->mc_ProjectName = c_FileApplicationInfo.acn_ProjectName;

                  // Version
                  this->mc_Version = c_FileApplicationInfo.acn_ProjectVersion;
                  if (this->mc_Version != "")
                  {
                     this->mpc_Ui->pc_LabelVersion->setText(this->mc_Version);
                  }
                  else
                  {
                     this->mc_Version = C_GtGetText::h_GetText("NA");
                     this->mpc_Ui->pc_LabelVersion->setText(C_GtGetText::h_GetText("?"));
                  }

                  // Timestamp
                  this->mc_Date = c_FileApplicationInfo.acn_Date;
                  this->mc_Time = c_FileApplicationInfo.acn_Time;
               }
               else
               {
                  C_OgeWiCustomMessage c_Message(this, C_OgeWiCustomMessage::E_Type::eERROR);
                  c_Message.SetHeading(C_GtGetText::h_GetText("Update Package configuration"));
                  c_Message.SetDescription(C_GtGetText::h_GetText("Device Type of selected HEX file does not "
                                                                  "match the node type."));
                  c_Message.SetDetails(QString(C_GtGetText::h_GetText("Device type of %1 does not match node type %2."))
                                       .arg(c_FileApplicationInfo.acn_DeviceID, this->mc_DeviceName));
                  c_Message.Execute();
                  q_Reset = true;
               }
            }
            else
            {
               this->mpc_Ui->pc_LabelVersion->setText(C_GtGetText::h_GetText("?"));
            }
         }
         else
         {
            // Invalid file
            C_OgeWiCustomMessage c_Message(this, C_OgeWiCustomMessage::E_Type::eERROR);
            c_Message.SetHeading(C_GtGetText::h_GetText("Update Package configuration"));
            c_Message.SetDescription(C_GtGetText::h_GetText("File is not a valid HEX file!"));
            c_Message.Execute();

            q_Reset = true;
         }

         delete pc_HexFile;
      }
      else
      {
         QDateTime c_DateTime = c_FileInfo.lastModified();

         // Get the matching string. The toString function use the localized variant
         // It shall be the same format as the date of C_OsyHexFile
         switch (c_DateTime.date().month())
         {
         case 1:
            this->mc_Date = "Jan";
            break;
         case 2:
            this->mc_Date = "Feb";
            break;
         case 3:
            this->mc_Date = "Mar";
            break;
         case 4:
            this->mc_Date = "Apr";
            break;
         case 5:
            this->mc_Date = "May";
            break;
         case 6:
            this->mc_Date = "Jun";
            break;
         case 7:
            this->mc_Date = "Jul";
            break;
         case 8:
            this->mc_Date = "Aug";
            break;
         case 9:
            this->mc_Date = "Sep";
            break;
         case 10:
            this->mc_Date = "Oct";
            break;
         case 11:
            this->mc_Date = "Nov";
            break;
         case 12:
            this->mc_Date = "Dec";
            break;
         default:
            // Ok, that is really not expected...
            this->mc_Date = "Xxx";
            break;
         }

         this->mc_Date = this->mc_Date + c_DateTime.date().toString(" d yyyy");
         this->mc_Time = c_DateTime.time().toString("h:mm:ss");

         this->mpc_Ui->pc_LabelVersion->setText("");
      }

      if (q_Reset == true)
      {
         const C_PuiSvData * const pc_View = C_PuiSvHandler::h_GetInstance()->GetView(this->mu32_ViewIndex);

         q_FileExists = false;

         // Remove the file
         this->mc_FilePath = C_GtGetText::h_GetText("<Add File>");
         this->mc_AbsoluteFilePath = this->mc_FilePath;
         this->m_UpateFilePathLabel();

         if (this->mq_FileBased == false)
         {
            this->mpc_Ui->pc_LabelVersion->setText(C_GtGetText::h_GetText("Missing"));
         }
         else
         {
            this->mpc_Ui->pc_LabelVersion->setText("");
         }

         // Path changed, update the view

         if (pc_View != NULL)
         {
            C_PuiSvNodeUpdate c_UpdateInfo = *pc_View->GetNodeUpdateInformation(this->mu32_NodeIndex);
            std::vector<QString> c_ViewAppPaths = c_UpdateInfo.GetApplicationPaths();

            tgl_assert(this->mu32_Number < c_ViewAppPaths.size());
            if (this->mu32_Number < c_ViewAppPaths.size())
            {
               c_ViewAppPaths[this->mu32_Number] = this->mc_FilePath;
               c_UpdateInfo.SetApplicationPaths(c_ViewAppPaths);

               C_PuiSvHandler::h_GetInstance()->SetNodeUpdateInformation(this->mu32_ViewIndex, this->mu32_NodeIndex,
                                                                         c_UpdateInfo);
            }
         }
      }
   }
   else
   {
      this->mpc_Ui->pc_LabelVersion->setText(C_GtGetText::h_GetText("Missing"));
   }

   this->m_UpdateToolTip();

   return q_FileExists;
}

//-----------------------------------------------------------------------------
void C_SyvUpUpdatePackageListNodeAppWidget::m_UpdateToolTip(void)
{
   QString c_Content = "";

   if (this->mq_FileBased == false)
   {
      c_Content += C_GtGetText::h_GetText("Project Name: ");
      c_Content += this->mc_ProjectName;

      c_Content += C_GtGetText::h_GetText("\nTimestamp: ");
      c_Content += this->mc_Date + " " + this->mc_Time;

      c_Content += C_GtGetText::h_GetText("\nVersion: ");
      c_Content += this->mc_Version;
   }
   else
   {
      c_Content += C_GtGetText::h_GetText("Last modified: ");
      c_Content += this->mc_Date + " " + this->mc_Time;
   }

   c_Content += C_GtGetText::h_GetText("\nFile path: ");
   c_Content += this->mc_AbsoluteFilePath;

   c_Content += C_GtGetText::h_GetText("\nFile size: ");
   c_Content += QString::number(this->mu32_FileSize);
   c_Content += C_GtGetText::h_GetText(" KB");

   this->SetToolTipInformation(this->mc_Title, c_Content);
}

//-----------------------------------------------------------------------------
void C_SyvUpUpdatePackageListNodeAppWidget::m_UpateFilePathLabel(void) const
{
   if ((this->mc_AbsoluteFilePath != "") &&
       (this->mc_AbsoluteFilePath != C_GtGetText::h_GetText("<Add File>")))
   {
      const sintn sn_Width = this->width() - mhsn_PathOffset;
      const QString c_AdaptedString =
         C_Uti::h_MinimizePath(this->mc_AbsoluteFilePath,
                               this->mpc_Ui->pc_LabelPath->font(), static_cast<uint32>(sn_Width), 0U);
      this->mpc_Ui->pc_LabelPath->setText(c_AdaptedString);
   }
   else
   {
      this->mpc_Ui->pc_LabelPath->setText(C_GtGetText::h_GetText("<Add File>"));
   }
}

//-----------------------------------------------------------------------------
void C_SyvUpUpdatePackageListNodeAppWidget::m_SetSelectColor(const bool oq_Active)
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
