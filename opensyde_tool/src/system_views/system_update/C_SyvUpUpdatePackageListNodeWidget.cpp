//-----------------------------------------------------------------------------
/*!
   \internal
   \file
   \brief       Widget for showing a specific node with its applications and data blocks as list item

   \implementation
   project     project name
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     09.02.2018  STW/B.Bayer
   \endimplementation
*/
//-----------------------------------------------------------------------------
#include <QFileInfo>
#include <QDragEnterEvent>
#include <QDragMoveEvent>
#include <QScrollBar>

#include "C_SyvUpUpdatePackageListNodeWidget.h"
#include "ui_C_SyvUpUpdatePackageListNodeWidget.h"

#include "stwerrors.h"
#include "constants.h"

#include "C_Uti.h"
#include "C_OgeWiUtil.h"

#include "C_PuiSdHandler.h"
#include "C_PuiSvHandler.h"
#include "C_OSCNode.h"
#include "C_PuiProject.h"
#include "C_PuiSvData.h"
#include "C_PuiSvNodeUpdate.h"
#include "TGLUtils.h"
#include "C_GtGetText.h"
#include "C_OSCLoggingHandler.h"
#include "C_OsyHexFile.h"
#include "C_OgeWiCustomMessage.h"
#include "C_ImpUtil.h"

/* -- Used Namespaces ------------------------------------------------------ */
using namespace stw_types;
using namespace stw_errors;
using namespace stw_opensyde_gui;
using namespace stw_opensyde_gui_logic;
using namespace stw_opensyde_gui_elements;
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
   \param[in]     ou32_PositionNumber  Position number of node for showing
   \param[in]     ou32_NodeIndex       Node index
   \param[in]     orc_NodeName         Name of the node
   \param[in,out] opc_parent           Optional pointer to parent

   \created     09.02.2018  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
C_SyvUpUpdatePackageListNodeWidget::C_SyvUpUpdatePackageListNodeWidget(const uint32 ou32_ViewIndex,
                                                                       const uint32 ou32_PositionNumber,
                                                                       const uint32 ou32_NodeIndex,
                                                                       const QString & orc_NodeName,
                                                                       QWidget * const opc_Parent) :
   QWidget(opc_Parent),
   mpc_Ui(new Ui::C_SyvUpUpdatePackageListNodeWidget),
   mu32_ViewIndex(ou32_ViewIndex),
   mu32_NodeIndex(ou32_NodeIndex),
   mc_NodeName(orc_NodeName),
   mc_DeviceType(""),
   mq_FileBased(false),
   mq_StwFlashloader(false),
   mu32_PositionNumber(ou32_PositionNumber),
   mu32_AppCount(0U),
   mu32_AppsUpdated(0U),
   mq_Connected(false)
{
   mpc_Ui->setupUi(this);

   this->mpc_Ui->pc_WidgetTitle->SetColorReserved(mc_STYLE_GUIDE_COLOR_10);
   this->mpc_Ui->pc_WidgetTitle->SetColorTooMuch(mc_STYLE_GUIDE_COLOR_10);
   this->mpc_Ui->pc_WidgetTitle->SetColorFree(mc_STYLE_GUIDE_COLOR_0);

   this->mpc_Ui->pc_WidgetAdd->setVisible(false);
   //Add button
   this->mpc_Ui->pc_PbAddFile->setIconSize(QSize(13, 13));
   this->mpc_Ui->pc_PbAddFile->SetCustomIcon("://images/IconAddEnabled.svg", "://images/IconAddDisabled.svg");

   this->InitStaticNames();

   stw_opensyde_gui_logic::C_OgeWiUtil::h_ApplyStylesheetProperty(this->mpc_Ui->pc_FrameSperator,
                                                                "HasColor8Background",
                                                                true);

   this->m_UpdateTitle();

   // Load applications
   this->m_InitApplications();

   this->mpc_LabelStateImg = new QLabel(this);
   this->mpc_LabelStateImg->raise();
   this->mpc_LabelStateImg->setGeometry(this->width() - 40, 0, 24, 24);
   this->mpc_LabelStateImg->setPixmap(QIcon(":images/system_views/IconWarning.svg").pixmap(mc_ICON_SIZE_24));
   this->mpc_LabelStateImg->setVisible(false);

   //Drag & drop of files
   this->setAcceptDrops(true);

   // Deactivate custom context menu of scroll bar
   this->mpc_Ui->pc_ScrollArea->verticalScrollBar()->setContextMenuPolicy(Qt::NoContextMenu);
   this->mpc_Ui->pc_ScrollArea->horizontalScrollBar()->setContextMenuPolicy(Qt::NoContextMenu);

   connect(this->mpc_Ui->pc_PbAddFile, &C_OgePubOptions::clicked, this,
           &C_SyvUpUpdatePackageListNodeWidget::m_ButtonAddNewFile);
}

//-----------------------------------------------------------------------------
/*!
   \brief   default destructor

   Clean up.

   \created     09.02.2018  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
C_SyvUpUpdatePackageListNodeWidget::~C_SyvUpUpdatePackageListNodeWidget()
{
   delete mpc_Ui;
   //lint -e{1740}  no memory leak because of the parent of mpc_LabelStateImg and the Qt memory management
}

//-----------------------------------------------------------------------------
/*!
   \brief   Initialize all displayed static names

   \created     12.06.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_SyvUpUpdatePackageListNodeWidget::InitStaticNames(void) const
{
   this->mpc_Ui->pc_PbAddFile->setText(C_GtGetText::h_GetText("Add new File"));
}

//-----------------------------------------------------------------------------
/*!
   \brief   Adapts the widget for the connected state

   \created     14.02.2018  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
void C_SyvUpUpdatePackageListNodeWidget::SetConnected(void)
{
   uint32 u32_Counter;

   this->mq_Connected = true;

   // Reset progress bar
   this->mpc_Ui->pc_WidgetTitle->SetProgress(0U);
   this->mpc_Ui->pc_WidgetAdd->setEnabled(false);

   for (u32_Counter = 0U; u32_Counter < this->mu32_AppCount; ++u32_Counter)
   {
      this->m_SetApplicationConnected(u32_Counter, true);
   }
}

//-----------------------------------------------------------------------------
/*!
   \brief   Adapts the widget when the updated was started

   \created     14.02.2018  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
void C_SyvUpUpdatePackageListNodeWidget::SetUpdateStarted(void) const
{
   sintn sn_Counter;
   bool q_ApplicationUpdateNecessary = false;

   for (sn_Counter = 0; sn_Counter < (this->mpc_Ui->pc_AppVerticalLayout->count() - 1); ++sn_Counter)
   {
      if (this->m_GetApplicationState(sn_Counter) != C_SyvUpUpdatePackageListNodeAppWidget::hu32_STATE_FINISHED)
      {
         q_ApplicationUpdateNecessary = true;
      }
   }

   if (q_ApplicationUpdateNecessary == true)
   {
      // Reset progress bar
      this->mpc_Ui->pc_WidgetTitle->SetProgress(0U);
   }
   else
   {
      // No update necessary.
      this->mpc_Ui->pc_WidgetTitle->SetProgress(100U);
   }
}

//-----------------------------------------------------------------------------
/*!
   \brief   Adapts the widget when the application update was started

   \created     14.02.2018  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
void C_SyvUpUpdatePackageListNodeWidget::SetUpdateApplicationStarted(void) const
{
   this->m_SetApplicationState(this->m_GetFirstNotFinishedApplication(),
                               C_SyvUpUpdatePackageListNodeAppWidget::hu32_STATE_IN_PROGRESS);
}

//-----------------------------------------------------------------------------
/*!
   \brief   Adapts the widget when the updated was started

   \created     14.02.2018  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
void C_SyvUpUpdatePackageListNodeWidget::SetUpdateApplicationFinished(void)
{
   this->m_SetApplicationState(
      this->m_GetFirstNotFinishedApplication(), C_SyvUpUpdatePackageListNodeAppWidget::hu32_STATE_FINISHED);

   ++this->mu32_AppsUpdated;

   if (this->mu32_AppCount > 0)
   {
      this->mpc_Ui->pc_WidgetTitle->SetProgress((this->mu32_AppsUpdated * 100U) / this->mu32_AppCount);
   }
   else
   {
      this->mpc_Ui->pc_WidgetTitle->SetProgress(100U);
   }
}

//-----------------------------------------------------------------------------
/*!
   \brief   Adapts the widget when the application update hat an error

   \param[in]     ou32_NodeIndex         Index of node

   \created     14.02.2018  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
void C_SyvUpUpdatePackageListNodeWidget::SetUpdateApplicationError(void) const
{
   this->m_SetApplicationState(
      this->m_GetFirstNotFinishedApplication(), C_SyvUpUpdatePackageListNodeAppWidget::hu32_STATE_ERROR);
}

//-----------------------------------------------------------------------------
/*!
   \brief   Adapts the widget when the entire update was finished

   \created     19.07.2018  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
void C_SyvUpUpdatePackageListNodeWidget::SetUpdateFinished(void) const
{
   if (this->mq_FileBased == true)
   {
      uint32 u32_Counter;

      // Special case: After updating a file based node, it is not possible to read back or confirm
      // the state of the node. Reset the state of all applications
      for (u32_Counter = 0U; u32_Counter < this->mu32_AppCount; ++u32_Counter)
      {
         this->m_SetApplicationState(u32_Counter, C_SyvUpUpdatePackageListNodeAppWidget::hu32_STATE_TO_DO);
      }
   }
}

//-----------------------------------------------------------------------------
/*!
   \brief   Trigger device application information discard

   \created     22.06.2018  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_SyvUpUpdatePackageListNodeWidget::DiscardApplicationStatus(void) const
{
   for (uint32 u32_Counter = 0U; u32_Counter < this->mu32_AppCount; ++u32_Counter)
   {
      this->m_SetApplicationState(u32_Counter, C_SyvUpUpdatePackageListNodeAppWidget::hu32_STATE_TO_DO);
   }
}

//-----------------------------------------------------------------------------
/*!
   \brief   Adapts the widget for the disconnected state

   Reset the application states

   \created     14.02.2018  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
void C_SyvUpUpdatePackageListNodeWidget::SetDisconnected(void)
{
   uint32 u32_Counter;

   this->mq_Connected = false;

   for (u32_Counter = 0U; u32_Counter < this->mu32_AppCount; ++u32_Counter)
   {
      this->m_SetApplicationState(u32_Counter, C_SyvUpUpdatePackageListNodeAppWidget::hu32_STATE_DEFAULT);
      this->m_SetApplicationConnected(u32_Counter, false);
   }

   this->mu32_AppsUpdated = 0U;
   this->SetProgress(0U);
   this->mpc_Ui->pc_WidgetAdd->setEnabled(true);
}

//-----------------------------------------------------------------------------
/*!
   \brief   Sets the current progress for showing for all applications

   \param[in]     ou32_Percentage   Percentage of progress

   \created     12.02.2018  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
void C_SyvUpUpdatePackageListNodeWidget::SetProgress(const uint32 ou32_Percentage) const
{
   if (this->mu32_AppCount > 0)
   {
      const uint32 u32_FinishedNodesPercentage = (this->mu32_AppsUpdated * 100U) / this->mu32_AppCount;
      this->mpc_Ui->pc_WidgetTitle->SetProgress(u32_FinishedNodesPercentage + (ou32_Percentage / this->mu32_AppCount));
   }
   else
   {
      this->mpc_Ui->pc_WidgetTitle->SetProgress(0U);
   }
}

//-----------------------------------------------------------------------------
/*!
   \brief   Update device information

   \param[in] orc_DeviceInformation Device info

   \created     23.02.2018  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
void C_SyvUpUpdatePackageListNodeWidget::UpdateDeviceInformation(const C_SyvUpDeviceInfo & orc_DeviceInformation) const
{
   uint32 u32_AppCountOnTarget;
   sintn sn_AppWidgetCounter;

   //get number of applications present on target
   if (orc_DeviceInformation.pc_OSYDevice != NULL)
   {
      u32_AppCountOnTarget = orc_DeviceInformation.pc_OSYDevice->c_Applications.size();
   }
   else if (orc_DeviceInformation.pc_STWDevice != NULL)
   {
      u32_AppCountOnTarget = orc_DeviceInformation.pc_STWDevice->c_BasicInformation.c_DeviceInfoBlocks.GetLength();
   }
   else
   {
      // Nothing to do
      u32_AppCountOnTarget = 0U;
   }
   //go through all applications in the UI
   for (sn_AppWidgetCounter = 0; sn_AppWidgetCounter < this->mpc_Ui->pc_AppVerticalLayout->count();
        ++sn_AppWidgetCounter)
   {
      QLayoutItem * const pc_Item = this->mpc_Ui->pc_AppVerticalLayout->itemAt(sn_AppWidgetCounter);

      if (pc_Item != NULL)
      {
         //lint -e{929}  false positive in PC-Lint: allowed by MISRA 5-2-2
         C_SyvUpUpdatePackageListNodeAppWidget * const pc_App =
            dynamic_cast<C_SyvUpUpdatePackageListNodeAppWidget *>(pc_Item->widget());

         if (pc_App != NULL)
         {
            pc_App->SetState(C_SyvUpUpdatePackageListNodeAppWidget::hu32_STATE_TO_DO);

            // Compare all applications
            for (uint32 u32_AppCounter = 0U; u32_AppCounter < u32_AppCountOnTarget; ++u32_AppCounter)
            {
               QString c_AppName = "";
               QString c_AppVersion = "";
               QString c_AppBuildTime = "";
               QString c_AppBuildDate = "";
               bool q_Valid = false;

               if (orc_DeviceInformation.pc_OSYDevice != NULL)
               {
                  c_AppName =
                     orc_DeviceInformation.pc_OSYDevice->c_Applications[u32_AppCounter].c_ApplicationName.c_str();
                  c_AppVersion =
                     orc_DeviceInformation.pc_OSYDevice->c_Applications[u32_AppCounter].c_ApplicationVersion.c_str();
                  c_AppBuildTime =
                     orc_DeviceInformation.pc_OSYDevice->c_Applications[u32_AppCounter].c_BuildTime.c_str();
                  c_AppBuildDate =
                     orc_DeviceInformation.pc_OSYDevice->c_Applications[u32_AppCounter].c_BuildDate.c_str();

                  q_Valid =
                     (orc_DeviceInformation.pc_OSYDevice->c_Applications[u32_AppCounter].u8_SignatureValid == 0U);
               }
               else
               {
                  //STW flashloader device
                  c_AppName =
                     orc_DeviceInformation.pc_STWDevice->c_BasicInformation.c_DeviceInfoBlocks[u32_AppCounter].
                     GetProjectName().c_str();
                  c_AppVersion =
                     orc_DeviceInformation.pc_STWDevice->c_BasicInformation.c_DeviceInfoBlocks[u32_AppCounter].
                     GetProjectVersion().c_str();
                  c_AppBuildTime =
                     orc_DeviceInformation.pc_STWDevice->c_BasicInformation.c_DeviceInfoBlocks[u32_AppCounter].GetTime()
                     .c_str();
                  c_AppBuildDate =
                     orc_DeviceInformation.pc_STWDevice->c_BasicInformation.c_DeviceInfoBlocks[u32_AppCounter].GetDate()
                     .c_str();

                  // No application valid flag
                  q_Valid = true;
               }
               if ((q_Valid == true) &&
                   (pc_App->IsAppIdentical(c_AppName, c_AppVersion, c_AppBuildTime, c_AppBuildDate) == true))
               {
                  pc_App->SetState(C_SyvUpUpdatePackageListNodeAppWidget::hu32_STATE_FINISHED);
                  break;
               }
            }
         }
      }
   }
}

//-----------------------------------------------------------------------------
/*!
   \brief   Updates the position number

   \param[in]     ou32_PositionNumber         New position number

   \created     12.02.2018  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
void C_SyvUpUpdatePackageListNodeWidget::UpdatePositionNumber(const uint32 ou32_PositionNumber)
{
   this->mu32_PositionNumber = ou32_PositionNumber;
   this->m_UpdateTitle();
}

//-----------------------------------------------------------------------------
/*!
   \brief   Adds a new file for the file container

   Only possible for file based devices

   \param[in]     orc_File      New path

   \created     15.02.2018  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
void C_SyvUpUpdatePackageListNodeWidget::AddFile(const QString & orc_File)
{
   const C_OSCNode * const pc_Node = C_PuiSdHandler::h_GetInstance()->GetOSCNode(this->mu32_NodeIndex);
   const C_PuiSvData * const pc_View = C_PuiSvHandler::h_GetInstance()->GetView(this->mu32_ViewIndex);

   if ((this->mq_FileBased == true) &&
       (pc_Node != NULL) &&
       (pc_View != NULL) &&
       (pc_Node->c_Applications.size() > 0))
   {
      bool q_NewFile = true;
      uint32 u32_Counter;

      // Check if file was already added
      for (u32_Counter = 0U; u32_Counter < this->mu32_AppCount; ++u32_Counter)
      {
         if (orc_File == this->m_GetApplicationPath(u32_Counter))
         {
            q_NewFile = false;
            break;
         }
      }

      if (q_NewFile == true)
      {
         C_PuiSvNodeUpdate c_UpdateInfo = *pc_View->GetNodeUpdateInformation(this->mu32_NodeIndex);
         std::vector<QString> c_ViewAppPaths = c_UpdateInfo.GetApplicationPaths();

         C_SyvUpUpdatePackageListNodeAppWidget * const pc_AppWidget =
            new C_SyvUpUpdatePackageListNodeAppWidget(this->mu32_ViewIndex, this->mu32_NodeIndex,
                                                      this->mc_DeviceType,
                                                      this->mq_FileBased,
                                                      this->mq_StwFlashloader,
                                                      this);
         QFileInfo c_File(orc_File);

         // Pre initialization of the size. If this is not set, the eliding of the path label will cause a
         // visible resizing
         pc_AppWidget->setGeometry(this->width(), 40, 0, 0);

         pc_AppWidget->SetAppFile(orc_File, false);
         pc_AppWidget->SetAppName(c_File.fileName());
         pc_AppWidget->SetAppType(C_OSCNodeApplication::eFILE_CONTAINER);
         pc_AppWidget->SetAppNumber(static_cast<uint32>(this->mpc_Ui->pc_AppVerticalLayout->count() - 1));

         this->mpc_Ui->pc_AppVerticalLayout->insertWidget(this->mpc_Ui->pc_AppVerticalLayout->count() - 1,
                                                          pc_AppWidget);

         // Save the new file as application path
         c_ViewAppPaths.push_back(orc_File);

         c_UpdateInfo.SetApplicationPaths(c_ViewAppPaths);

         C_PuiSvHandler::h_GetInstance()->SetNodeUpdateInformation(this->mu32_ViewIndex, this->mu32_NodeIndex,
                                                                   c_UpdateInfo);

         this->mpc_Ui->pc_WidgetAdd->setVisible(false);
         ++this->mu32_AppCount;
         //lint -e{429}  no memory leak because of the parent of pc_AppWidget and the Qt memory management
      }
   }
}

//-----------------------------------------------------------------------------
/*!
   \brief   Sets a new file for the application

   \param[in]     orc_File      New path
   \param[in]     opc_App       Application widget

   \created     15.02.2018  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
void C_SyvUpUpdatePackageListNodeWidget::AdaptFile(const QString & orc_File,
                                                   C_SyvUpUpdatePackageListNodeAppWidget * const opc_App) const
{
   if (opc_App != NULL)
   {
      bool q_Continue = true;

      // In case of STW flashloader warn the user in case of different device names, but it is allowed.
      // In case of openSYDE flashloader different device names are not allowed, but will be checked later.
      if (this->mq_StwFlashloader == true)
      {
         // Reporting of other errors not necessary here. Will be handled in C_SyvUpUpdatePackageListNodeAppWidget.
         C_OsyHexFile * const pc_HexFile = new C_OsyHexFile();
         QString c_AbsolutePath;

         C_SyvUpUpdatePackageListNodeAppWidget::h_GetAbsolutePath(orc_File, c_AbsolutePath);

         if (pc_HexFile->LoadFromFile(c_AbsolutePath.toStdString().c_str()) == stw_hex_file::NO_ERR)
         {
            stw_diag_lib::C_XFLECUInformation c_FileApplicationInfo;

            if (pc_HexFile->ScanApplicationInformationBlockFromHexFile(c_FileApplicationInfo) == C_NO_ERR)
            {
               if (this->mc_DeviceType != c_FileApplicationInfo.acn_DeviceID)
               {
                  C_OgeWiCustomMessage c_MessageBox(this->parentWidget(), C_OgeWiCustomMessage::E_Type::eQUESTION);
                  C_OgeWiCustomMessage::E_Outputs e_ReturnMessageBox;

                  c_MessageBox.SetHeading(C_GtGetText::h_GetText("Update Package configuration"));
                  c_MessageBox.SetDescription(C_GtGetText::h_GetText("Device Type of selected HEX file does not "
                                                                     "match the node type. Do you want to use the "
                                                                     "HEX file anyway?"));
                  c_MessageBox.SetDetails(QString(C_GtGetText::h_GetText(
                                                     "Device type of %1 does not match node type %2."))
                                          .arg(c_FileApplicationInfo.acn_DeviceID, this->mc_DeviceType));
                  c_MessageBox.SetOKButtonText(C_GtGetText::h_GetText("Yes"));
                  c_MessageBox.SetNOButtonText(C_GtGetText::h_GetText("No"));
                  e_ReturnMessageBox = c_MessageBox.Execute();

                  if (e_ReturnMessageBox ==  C_OgeWiCustomMessage::eNO)
                  {
                     // do not continue and do nothing
                     q_Continue = false;
                  }
               }
            }
         }

         delete pc_HexFile;
      }

      if (q_Continue == true)
      {
         const C_PuiSvData * const pc_View = C_PuiSvHandler::h_GetInstance()->GetView(this->mu32_ViewIndex);

         // Adapt the widget itself
         opc_App->SetAppFile(orc_File, false);

         if (this->mq_FileBased == true)
         {
            QFileInfo c_File(orc_File);

            opc_App->SetAppName(c_File.fileName());
         }

         if (pc_View != NULL)
         {
            C_PuiSvNodeUpdate c_UpdateInfo = *pc_View->GetNodeUpdateInformation(this->mu32_NodeIndex);
            std::vector<QString> c_ViewAppPaths = c_UpdateInfo.GetApplicationPaths();

            tgl_assert(opc_App->GetAppNumber() < c_ViewAppPaths.size());
            if (opc_App->GetAppNumber() < c_ViewAppPaths.size())
            {
               c_ViewAppPaths[opc_App->GetAppNumber()] = orc_File;
               c_UpdateInfo.SetApplicationPaths(c_ViewAppPaths);

               C_PuiSvHandler::h_GetInstance()->SetNodeUpdateInformation(this->mu32_ViewIndex, this->mu32_NodeIndex,
                                                                         c_UpdateInfo);
            }
         }
      }
   }
}

//-----------------------------------------------------------------------------
/*!
   \brief   Revert the current application to the system definition default path

   Only for address based devices

   \param[in]     opc_App       Application widget

   \created     15.02.2018  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
void C_SyvUpUpdatePackageListNodeWidget::RevertFile(C_SyvUpUpdatePackageListNodeAppWidget * const opc_App) const
{
   // Reverting path only available for not file based applications
   if (this->mq_FileBased == false)
   {
      const C_PuiSvData * const pc_View = C_PuiSvHandler::h_GetInstance()->GetView(this->mu32_ViewIndex);

      if ((pc_View != NULL) &&
          (opc_App != NULL))
      {
         C_PuiSvNodeUpdate c_UpdateInfo = *pc_View->GetNodeUpdateInformation(this->mu32_NodeIndex);
         std::vector<QString> c_ViewAppPaths = c_UpdateInfo.GetApplicationPaths();
         uint32 u32_AppNumber = opc_App->GetAppNumber();
         const C_OSCNode * const pc_Node = C_PuiSdHandler::h_GetInstance()->GetOSCNode(this->mu32_NodeIndex);

         // Remove the view specific path
         if (u32_AppNumber < c_ViewAppPaths.size())
         {
            c_ViewAppPaths[u32_AppNumber] = "";
         }
         c_UpdateInfo.SetApplicationPaths(c_ViewAppPaths);

         C_PuiSvHandler::h_GetInstance()->SetNodeUpdateInformation(this->mu32_ViewIndex, this->mu32_NodeIndex,
                                                                   c_UpdateInfo);

         // Restore the default path
         if ((pc_Node != NULL) &&
             (u32_AppNumber < pc_Node->c_Applications.size()))
         {
            opc_App->SetAppFile(
               C_Uti::h_ConcatPathIfNecessary(
                  C_ImpUtil::h_GetAbsoluteProjectPath(pc_Node->c_Applications[u32_AppNumber].c_ProjectPath.c_str()),
                  pc_Node->c_Applications[u32_AppNumber].c_ResultPath.c_str()),
               true);
         }
      }
   }
}

//-----------------------------------------------------------------------------
/*!
   \brief   Removes the current application

   Only for file based devices

   \param[in]     opc_App       Application widget

   \created     27.03.2018  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
void C_SyvUpUpdatePackageListNodeWidget::RemoveFile(C_SyvUpUpdatePackageListNodeAppWidget * const opc_App)
{
   if ((this->mq_FileBased == true) &&
       (opc_App != NULL))
   {
      const C_PuiSvData * const pc_View = C_PuiSvHandler::h_GetInstance()->GetView(this->mu32_ViewIndex);
      if (pc_View != NULL)
      {
         C_PuiSvNodeUpdate c_UpdateInfo = *pc_View->GetNodeUpdateInformation(this->mu32_NodeIndex);
         std::vector<QString> c_ViewAppPaths = c_UpdateInfo.GetApplicationPaths();
         uint32 u32_AppNumber = opc_App->GetAppNumber();

         if (u32_AppNumber < c_ViewAppPaths.size())
         {
            // Remove the application file path
            c_ViewAppPaths.erase(c_ViewAppPaths.begin() + u32_AppNumber);

            // Remove the widget
            this->mpc_Ui->pc_AppVerticalLayout->removeWidget(opc_App);
            opc_App->setParent(NULL);
            delete opc_App;

            c_UpdateInfo.SetApplicationPaths(c_ViewAppPaths);
            C_PuiSvHandler::h_GetInstance()->SetNodeUpdateInformation(this->mu32_ViewIndex, this->mu32_NodeIndex,
                                                                      c_UpdateInfo);

            this->m_UpdateNumbers();
            if (this->mu32_AppCount > 0)
            {
               --this->mu32_AppCount;
            }
         }
      }
   }

   this->mpc_Ui->pc_WidgetAdd->setVisible(this->mpc_Ui->pc_AppVerticalLayout->count() <= 1);
}

//-----------------------------------------------------------------------------
/*!
   \brief   Removes all application paths

   \created     15.02.2018  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
void C_SyvUpUpdatePackageListNodeWidget::RemoveAllFiles(void)
{
   const C_PuiSvData * const pc_View = C_PuiSvHandler::h_GetInstance()->GetView(this->mu32_ViewIndex);
   sintn sn_Counter;

   for (sn_Counter = this->mpc_Ui->pc_AppVerticalLayout->count() - 1; sn_Counter >= 0; --sn_Counter)
   {
      QLayoutItem * const pc_Item = this->mpc_Ui->pc_AppVerticalLayout->itemAt(sn_Counter);

      if (pc_Item != NULL)
      {
         //lint -e{929}  false positive in PC-Lint: allowed by MISRA 5-2-2
         C_SyvUpUpdatePackageListNodeAppWidget * const pc_App =
            dynamic_cast<C_SyvUpUpdatePackageListNodeAppWidget *>(pc_Item->widget());

         if (pc_App != NULL)
         {
            if (this->mq_FileBased == false)
            {
               // Adapt the widget itself
               pc_App->SetAppFile(C_GtGetText::h_GetText("<Add File>"), false);
            }
            else
            {
               // In case of file based devices remove the file/application entry really
               this->RemoveFile(pc_App);
            }
         }
      }
   }

   if (pc_View != NULL)
   {
      C_PuiSvNodeUpdate c_UpdateInfo = *pc_View->GetNodeUpdateInformation(this->mu32_NodeIndex);
      std::vector<QString> c_ViewAppPaths = c_UpdateInfo.GetApplicationPaths();

      if (this->mq_FileBased == false)
      {
         uint32 u32_Counter;

         for (u32_Counter = 0U; u32_Counter < c_ViewAppPaths.size(); ++u32_Counter)
         {
            c_ViewAppPaths[u32_Counter] = C_GtGetText::h_GetText("<Add File>");
         }
      }
      else
      {
         // In case of file based devices remove the file/application entry really
         c_ViewAppPaths.clear();
      }

      c_UpdateInfo.SetApplicationPaths(c_ViewAppPaths);

      C_PuiSvHandler::h_GetInstance()->SetNodeUpdateInformation(this->mu32_ViewIndex, this->mu32_NodeIndex,
                                                                c_UpdateInfo);
   }
}

//-----------------------------------------------------------------------------
/*!
   \brief   Prepares the update package node configuration

   \param[out]     orc_NodeConfig         Node configuration

   \created     20.02.2018  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
void C_SyvUpUpdatePackageListNodeWidget::PrepareExportConfig(C_SyvUpUpdatePackageConfigNode & orc_NodeConfig) const
{
   // One spacer is in the layout and is not relevant
   if (this->mpc_Ui->pc_AppVerticalLayout->count() > 1)
   {
      sintn sn_Counter;
      const sintn sn_AppCount = this->mpc_Ui->pc_AppVerticalLayout->count() - 1;

      orc_NodeConfig.c_AppConfigs.reserve(sn_AppCount);

      orc_NodeConfig.c_Name = this->mc_NodeName;
      orc_NodeConfig.c_DeviceType = this->mc_DeviceType;

      for (sn_Counter = 0; sn_Counter < this->mpc_Ui->pc_AppVerticalLayout->count(); ++sn_Counter)
      {
         QLayoutItem * const pc_Item = this->mpc_Ui->pc_AppVerticalLayout->itemAt(sn_Counter);

         if (pc_Item != NULL)
         {
            //lint -e{929}  false positive in PC-Lint: allowed by MISRA 5-2-2
            C_SyvUpUpdatePackageListNodeAppWidget * const pc_App =
               dynamic_cast<C_SyvUpUpdatePackageListNodeAppWidget *>(pc_Item->widget());

            if (pc_App != NULL)
            {
               C_SyvUpUpdatePackageConfigNodeApp c_AppConfig;

               pc_App->PrepareExportConfig(c_AppConfig);

               orc_NodeConfig.c_AppConfigs.push_back(c_AppConfig);
            }
         }
      }
   }
}

//-----------------------------------------------------------------------------
/*!
   \brief   Loads all matching configurations for this node of the import configuration

   \param[out]     orc_Config         Import configuration

   \created     21.02.2018  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
void C_SyvUpUpdatePackageListNodeWidget::LoadImportConfig(const C_SyvUpUpdatePackageConfig & orc_Config)
{
   uint32 u32_ConfigCounter;

   // Search the matching configuration
   for (u32_ConfigCounter = 0; u32_ConfigCounter < orc_Config.c_NodeConfigs.size(); ++u32_ConfigCounter)
   {
      if ((orc_Config.c_NodeConfigs[u32_ConfigCounter].c_Name == this->mc_NodeName) &&
          (orc_Config.c_NodeConfigs[u32_ConfigCounter].c_DeviceType == this->mc_DeviceType))
      {
         if (this->mq_FileBased == false)
         {
            sintn sn_AppCounter;
            // Matching configuration found. Load the configuration for the applications
            for (sn_AppCounter = 0; sn_AppCounter < this->mpc_Ui->pc_AppVerticalLayout->count(); ++sn_AppCounter)
            {
               QLayoutItem * const pc_Item = this->mpc_Ui->pc_AppVerticalLayout->itemAt(sn_AppCounter);

               if (pc_Item != NULL)
               {
                  //lint -e{929}  false positive in PC-Lint: allowed by MISRA 5-2-2
                  C_SyvUpUpdatePackageListNodeAppWidget * const pc_App =
                     dynamic_cast<C_SyvUpUpdatePackageListNodeAppWidget *>(pc_Item->widget());

                  if (pc_App != NULL)
                  {
                     pc_App->LoadImportConfig(orc_Config.c_NodeConfigs[u32_ConfigCounter]);
                  }
               }
            }
         }
         else
         {
            uintn un_AppConfigCounter;

            // Remove the previous applications
            this->RemoveAllFiles();

            // Add the imported configuration as new applications
            for (un_AppConfigCounter = 0U;
                 un_AppConfigCounter < orc_Config.c_NodeConfigs[u32_ConfigCounter].c_AppConfigs.size();
                 ++un_AppConfigCounter)
            {
               this->AddFile(orc_Config.c_NodeConfigs[u32_ConfigCounter].c_AppConfigs[un_AppConfigCounter].c_Path);
            }
         }

         break;
      }
   }
}

//-----------------------------------------------------------------------------
/*!
   \brief   Checks all paths for existence

   \param[out]    oru32_CountMissingFiles    Number of not found files
   \param[out]    oru32_CountFiles           Number of files

   \return
   C_NO_ERR    All files are existent
   C_CONFIG    Minimum one file was not found

   \created     16.02.2018  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
sint32 C_SyvUpUpdatePackageListNodeWidget::CheckAllFiles(stw_types::uint32 & oru32_CountMissingFiles,
                                                         stw_types::uint32 & oru32_CountFiles)
{
   sintn sn_Counter;
   sint32 s32_Return = C_NO_ERR;

   for (sn_Counter = 0; sn_Counter < this->mpc_Ui->pc_AppVerticalLayout->count(); ++sn_Counter)
   {
      QLayoutItem * const pc_Item = this->mpc_Ui->pc_AppVerticalLayout->itemAt(sn_Counter);

      if (pc_Item != NULL)
      {
         //lint -e{929}  false positive in PC-Lint: allowed by MISRA 5-2-2
         C_SyvUpUpdatePackageListNodeAppWidget * const pc_App =
            dynamic_cast<C_SyvUpUpdatePackageListNodeAppWidget *>(pc_Item->widget());

         if (pc_App != NULL)
         {
            ++oru32_CountFiles;

            // Check the app path
            if (pc_App->CheckPath() != C_NO_ERR)
            {
               s32_Return = C_CONFIG;
               ++oru32_CountMissingFiles;
            }
         }
      }
   }

   this->mpc_LabelStateImg->setVisible(s32_Return != C_NO_ERR);

   return s32_Return;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Returns and select the application on the position

   \param[in]     orc_Pos     Position of application

   \return
   Pointer to application
   NULL if no application was at the position

   \created     15.02.2018  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
C_SyvUpUpdatePackageListNodeAppWidget * C_SyvUpUpdatePackageListNodeWidget::GetAndSelectApplication(
   const QPoint & orc_Pos) const
{
   C_SyvUpUpdatePackageListNodeAppWidget * const pc_App = this->GetApplication(orc_Pos);

   if (pc_App != NULL)
   {
      pc_App->SetSelected(true);
   }

   return pc_App;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Returns the application on the position

   \param[in]     orc_Pos        Position of application
   \param[out]    opu32_Number   Optional parameter for found app number

   \return
   Pointer to application
   NULL if no application was at the position

   \created     15.02.2018  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
C_SyvUpUpdatePackageListNodeAppWidget * C_SyvUpUpdatePackageListNodeWidget::GetApplication(const QPoint & orc_Pos,
                                                                                           uint32 * const opu32_Number)
const
{
   C_SyvUpUpdatePackageListNodeAppWidget * pc_App = NULL;
   sintn sn_Counter;

   // The header widget area shall be excluded, it can cause problems if a scrollbar is in the list available
   if (this->mpc_Ui->pc_WidgetApplications->mapFrom(this->parentWidget(), orc_Pos).y() > 0)
   {
      for (sn_Counter = 0; sn_Counter < this->mpc_Ui->pc_AppVerticalLayout->count(); ++sn_Counter)
      {
         QLayoutItem * const pc_Item = this->mpc_Ui->pc_AppVerticalLayout->itemAt(sn_Counter);

         if (pc_Item != NULL)
         {
            //lint -e{929}  false positive in PC-Lint: allowed by MISRA 5-2-2
            pc_App = dynamic_cast<C_SyvUpUpdatePackageListNodeAppWidget *>(pc_Item->widget());

            if (pc_App != NULL)
            {
               const QPoint c_AppPos = pc_App->mapFrom(this->parentWidget(), orc_Pos);

               if (pc_App->rect().contains(c_AppPos) == true)
               {
                  if (opu32_Number != NULL)
                  {
                     *opu32_Number = static_cast<uint32>(sn_Counter);
                  }
                  break;
               }
            }
         }
      }
   }

   return pc_App;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Returns view index

   \return
   View index

   \created     13.02.2018  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
uint32 C_SyvUpUpdatePackageListNodeWidget::GetViewIndex(void) const
{
   return this->mu32_ViewIndex;
}

//-----------------------------------------------------------------------------
/*!
   \brief   short description of function

   possible return value(s) and description

   \created     12.02.2018  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
QString C_SyvUpUpdatePackageListNodeWidget::GetNodeName(void) const
{
   return this->mc_NodeName;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Returns the current position number

   \return
   Current position number

   \created     12.02.2018  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
uint32 C_SyvUpUpdatePackageListNodeWidget::GetPositionNumber(void) const
{
   return this->mu32_PositionNumber;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Returns the current position number

   \return
   Current position number

   \created     12.02.2018  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
uint32 C_SyvUpUpdatePackageListNodeWidget::GetNodeIndex(void) const
{
   return this->mu32_NodeIndex;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Returns the flag if the node is a file based device

   \return
   true     Node is file based
   false    Node is not file based

   \created     26.03.2018  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
bool C_SyvUpUpdatePackageListNodeWidget::IsFileBased(void) const
{
   return this->mq_FileBased;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Creates and returns the update package with all information for the system update for the concrete node

   \param[out]    orc_ApplicationsToWrite       Vector with node update configuration

   \return
   C_NO_ERR    Update package with all information created
   C_RD_WR     At least one file does not exist
   C_NOACT     No files for applications added

   \created     23.02.2018  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
sint32 C_SyvUpUpdatePackageListNodeWidget::GetUpdatePackage(C_OSCSuSequences::C_DoFlash & orc_ApplicationsToWrite)
{
   sint32 s32_Return = C_NO_ERR;
   sintn sn_Counter;

   this->mu32_AppsUpdated = 0U;

   for (sn_Counter = 0; sn_Counter < this->mpc_Ui->pc_AppVerticalLayout->count(); ++sn_Counter)
   {
      QLayoutItem * const pc_Item = this->mpc_Ui->pc_AppVerticalLayout->itemAt(sn_Counter);

      if (pc_Item != NULL)
      {
         //lint -e{929}  false positive in PC-Lint: allowed by MISRA 5-2-2
         C_SyvUpUpdatePackageListNodeAppWidget * const pc_App =
            dynamic_cast<C_SyvUpUpdatePackageListNodeAppWidget *>(pc_Item->widget());

         // Add only if application is not already finished. In this case an update of this application is not
         // necessary.
         if (pc_App != NULL)
         {
            if (pc_App->GetState() != C_SyvUpUpdatePackageListNodeAppWidget::hu32_STATE_FINISHED)
            {
               const QString c_Path = pc_App->GetAppAbsoluteFilePath();
               QFileInfo c_File(c_Path);

               // Check file
               if ((c_File.exists() == true) &&
                   (c_File.isFile() == true))
               {
                  const QString c_LogEntry =
                     "Generate Update Package: For node \"%1\" and application \"%2\" used file: %3";

                  orc_ApplicationsToWrite.c_FilesToFlash.push_back(stw_scl::C_SCLString(c_Path.toStdString().c_str()));

                  osc_write_log_info("Generate Update Package",
                                     C_GtGetText::h_GetText(c_LogEntry.arg(this->mc_NodeName,
                                                                           pc_App->GetAppName(),
                                                                           c_Path).toStdString().c_str()));
               }
               else
               {
                  const QString c_LogEntry =
                     "Generate Update Package: The path of Node \"%1\" and application \"%2\" is invalid: %3";

                  osc_write_log_info("Generate Update Package",
                                     C_GtGetText::h_GetText(c_LogEntry.arg(this->mc_NodeName,
                                                                           pc_App->GetAppName(),
                                                                           c_Path).toStdString().c_str()));

                  s32_Return = C_RD_WR;
               }
            }
            else
            {
               // Update of application is not necessary
               ++this->mu32_AppsUpdated;
            }
         }
      }

      if (s32_Return != C_NO_ERR)
      {
         break;
      }
   }

   if (orc_ApplicationsToWrite.c_FilesToFlash.size() == 0)
   {
      // No files added
      s32_Return = C_NOACT;
   }

   return s32_Return;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Overwritten show event slot

   Here: Adapt the icon position

   \param[in,out] opc_Event Event identification and information

   \created     16.02.2018  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
void C_SyvUpUpdatePackageListNodeWidget::resizeEvent(QResizeEvent * const opc_Event)
{
   QWidget::resizeEvent(opc_Event);

   this->mpc_LabelStateImg->setGeometry(this->width() - 40, 0, 24, 24);
}

//-----------------------------------------------------------------------------
/*!
   \brief   Overwritten drag enter event slot

   Here: Accept external files

   \param[in,out] opc_Event Event identification and information

   \created     22.02.2018  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
void C_SyvUpUpdatePackageListNodeWidget::dragEnterEvent(QDragEnterEvent * const opc_Event)
{
   if (this->mq_Connected == false)
   {
      const QMimeData * const pc_MimeData = opc_Event->mimeData();

      if (pc_MimeData->hasUrls() == true)
      {
         opc_Event->accept();
      }
   }

   QWidget::dragEnterEvent(opc_Event);
}

//-----------------------------------------------------------------------------
/*!
   \brief   Overwritten drag move event slot

   Here: Accept external files

   \param[in,out] opc_Event Event identification and information

   \created     22.02.2018  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
void C_SyvUpUpdatePackageListNodeWidget::dragMoveEvent(QDragMoveEvent * const opc_Event)
{
   if (this->mq_Connected == false)
   {
      const QMimeData * const pc_MimeData = opc_Event->mimeData();

      if (pc_MimeData->hasUrls() == true)
      {
         // Get Application expects coordinate from the parent of this widget
         if (m_CheckMime(pc_MimeData, this->mapToParent(opc_Event->pos())) == true)
         {
            opc_Event->accept();
         }
         else
         {
            opc_Event->ignore();
         }
      }
   }
   else
   {
      opc_Event->ignore();
   }

   QWidget::dragMoveEvent(opc_Event);
}

//-----------------------------------------------------------------------------
/*!
   \brief   Overwritten drag leave event slot

   Here: Deactivate selection of apps

   \param[in,out] opc_Event Event identification and information

   \created     19.07.2018  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
void C_SyvUpUpdatePackageListNodeWidget::dragLeaveEvent(QDragLeaveEvent * const opc_Event)
{
   uint32 u32_AppCounter;

   for (u32_AppCounter = 0U; u32_AppCounter < this->mu32_AppCount; ++u32_AppCounter)
   {
      this->m_SetApplicationSelect(u32_AppCounter, false);
   }

   QWidget::dragLeaveEvent(opc_Event);
}

//-----------------------------------------------------------------------------
/*!
   \brief   Overwritten drop event slot

   Here: Handle dropped files

   \param[in,out] opc_Event Event identification and information

   \created     22.02.2018  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
void C_SyvUpUpdatePackageListNodeWidget::dropEvent(QDropEvent * const opc_Event)
{
   if (this->mq_Connected == false)
   {
      const QMimeData * const pc_MimeData = opc_Event->mimeData();
      QStringList c_FilePaths;
      C_SyvUpUpdatePackageListNodeAppWidget * pc_App = NULL;

      if (m_CheckMime(pc_MimeData, this->mapToParent(opc_Event->pos()), &c_FilePaths, &pc_App) == true)
      {
         const QString c_Folder = C_PuiProject::h_GetInstance()->GetFolderPath();
         sintn sn_FileCounter;

         for (sn_FileCounter = 0; sn_FileCounter < c_FilePaths.size(); ++sn_FileCounter)
         {
            if (c_FilePaths[sn_FileCounter] != "")
            {
               // If possible make the path relative
               if (c_FilePaths[sn_FileCounter].contains(c_Folder, Qt::CaseInsensitive) == true)
               {
                  // Remove project path
                  c_FilePaths[sn_FileCounter] = c_FilePaths[sn_FileCounter].remove(c_Folder, Qt::CaseInsensitive);
               }
            }
         }

         if ((pc_App != NULL) &&
             (c_FilePaths.size() == 1))
         {
            this->AdaptFile(c_FilePaths[0], pc_App);
         }
         else if (this->mq_FileBased == true)
         {
            for (sn_FileCounter = 0; sn_FileCounter < c_FilePaths.size(); ++sn_FileCounter)
            {
               // Add new file
               this->AddFile(c_FilePaths[sn_FileCounter]);
            }
         }
         else
         {
            // Nothing to do
         }

         if (pc_App != NULL)
         {
            pc_App->SetSelected(false);
         }
      }
   }

   QWidget::dropEvent(opc_Event);
}

//-----------------------------------------------------------------------------
void C_SyvUpUpdatePackageListNodeWidget::m_InitApplications(void)
{
   const C_OSCNode * const pc_Node = C_PuiSdHandler::h_GetInstance()->GetOSCNode(this->mu32_NodeIndex);
   QSpacerItem * const pc_Spacer = new QSpacerItem(0, 3, QSizePolicy::Minimum, QSizePolicy::Expanding);

   tgl_assert(pc_Node != NULL);
   if (pc_Node != NULL)
   {
      uint32 u32_Counter;
      const C_PuiSvData * const pc_View = C_PuiSvHandler::h_GetInstance()->GetView(this->mu32_ViewIndex);

      this->mc_DeviceType = pc_Node->c_DeviceType.c_str();

      tgl_assert(pc_Node->pc_DeviceDefinition != NULL);
      this->mq_FileBased = pc_Node->pc_DeviceDefinition->q_FlashloaderOpenSydeIsFileBased;
      this->mq_StwFlashloader = pc_Node->pc_DeviceDefinition->q_FlashloaderStwCan;

      if ((pc_View != NULL) &&
          (pc_Node->c_Applications.size() > 0))
      {
         const C_PuiSvNodeUpdate * const pc_UpdateInfo = pc_View->GetNodeUpdateInformation(this->mu32_NodeIndex);

         tgl_assert(pc_UpdateInfo != NULL);

         if (pc_UpdateInfo != NULL)
         {
            const std::vector<QString> c_ViewAppPaths = pc_UpdateInfo->GetApplicationPaths();
            bool q_ShowAddButton = true;

            if (this->mq_FileBased == false)
            {
               // No error with file based devices
               tgl_assert(pc_Node->c_Applications.size() == c_ViewAppPaths.size());
            }

            for (u32_Counter = 0U; u32_Counter < c_ViewAppPaths.size(); ++u32_Counter)
            {
               C_SyvUpUpdatePackageListNodeAppWidget * const pc_AppWidget =
                  new C_SyvUpUpdatePackageListNodeAppWidget(this->mu32_ViewIndex, this->mu32_NodeIndex,
                                                            this->mc_DeviceType,
                                                            this->mq_FileBased, this->mq_StwFlashloader, this);
               const C_OSCNodeApplication * pc_App;

               if (this->mq_FileBased == false)
               {
                  pc_App = &pc_Node->c_Applications[u32_Counter];
                  pc_AppWidget->SetAppName(pc_App->c_Name.c_str());

                  // Check if a specific path is available
                  if (c_ViewAppPaths[u32_Counter] != "")
                  {
                     pc_AppWidget->SetAppFile(c_ViewAppPaths[u32_Counter], false);
                  }
                  else
                  {
                     pc_AppWidget->SetAppFile(
                        C_Uti::h_ConcatPathIfNecessary(C_ImpUtil::h_GetAbsoluteProjectPath(pc_App->c_ProjectPath.c_str()),
                                                       pc_App->c_ResultPath.c_str()), true);
                  }

                  // Set the default path for comparing with import configuration
                  pc_AppWidget->SetDefaultFile(
                     C_Uti::h_ConcatPathIfNecessary(C_ImpUtil::h_GetAbsoluteProjectPath(pc_App->c_ProjectPath.c_str()),
                                                    pc_App->c_ResultPath.c_str()));
               }
               else
               {
                  QFileInfo c_File(c_ViewAppPaths[u32_Counter]);
                  pc_App = &pc_Node->c_Applications[0];

                  pc_AppWidget->SetAppFile(c_ViewAppPaths[u32_Counter], false);
                  pc_AppWidget->SetAppName(c_File.fileName());
               }

               pc_AppWidget->SetAppType(pc_App->e_Type);
               pc_AppWidget->SetAppNumber(u32_Counter);

               ++this->mu32_AppCount;
               this->mpc_Ui->pc_AppVerticalLayout->addWidget(pc_AppWidget);
               q_ShowAddButton = false;
               //lint -e{429}  no memory leak because of the parent of pc_AppWidget and the Qt memory management
            }

            this->mpc_Ui->pc_AppVerticalLayout->addSpacerItem(pc_Spacer);
            this->mpc_Ui->pc_AppVerticalLayout->setStretch(this->mpc_Ui->pc_AppVerticalLayout->count() - 1, 1);

            if ((this->mq_FileBased == true) &&
                (q_ShowAddButton == true))
            {
               this->mpc_Ui->pc_WidgetAdd->setVisible(true);
            }
         }
      }
   }

   //lint -e{429}  no memory leak because of the parent of pc_Spacer and the Qt memory management
}

//-----------------------------------------------------------------------------
void C_SyvUpUpdatePackageListNodeWidget::m_UpdateTitle(void) const
{
   this->mpc_Ui->pc_Title->setText(QString("#%1 - %2").arg(QString::number(this->mu32_PositionNumber + 1U),
                                                           this->mc_NodeName));
}

//-----------------------------------------------------------------------------
void C_SyvUpUpdatePackageListNodeWidget::m_SetApplicationState(const uint32 ou32_Application,
                                                               const uint32 ou32_State) const
{
   QLayoutItem * const pc_Item = this->mpc_Ui->pc_AppVerticalLayout->itemAt(ou32_Application);

   // Adapt the icon of the finished application
   if (pc_Item != NULL)
   {
      //lint -e{929}  false positive in PC-Lint: allowed by MISRA 5-2-2
      C_SyvUpUpdatePackageListNodeAppWidget * const pc_App =
         dynamic_cast<C_SyvUpUpdatePackageListNodeAppWidget *>(pc_Item->widget());

      if (pc_App != NULL)
      {
         pc_App->SetState(ou32_State);
      }
   }
}

//-----------------------------------------------------------------------------
uint32 C_SyvUpUpdatePackageListNodeWidget::m_GetApplicationState(const uint32 ou32_Application) const
{
   QLayoutItem * const pc_Item = this->mpc_Ui->pc_AppVerticalLayout->itemAt(ou32_Application);
   // Undefined state in error case
   uint32 u32_Return = 0xFFFFFFFFUL;

   // Adapt the icon of the finished application
   if (pc_Item != NULL)
   {
      //lint -e{929}  false positive in PC-Lint: allowed by MISRA 5-2-2
      C_SyvUpUpdatePackageListNodeAppWidget * const pc_App =
         dynamic_cast<C_SyvUpUpdatePackageListNodeAppWidget *>(pc_Item->widget());

      if (pc_App != NULL)
      {
         u32_Return = pc_App->GetState();
      }
   }

   return u32_Return;
}

//-----------------------------------------------------------------------------
void C_SyvUpUpdatePackageListNodeWidget::m_SetApplicationSelect(const uint32 ou32_Application,
                                                                const bool oq_Select) const
{
   QLayoutItem * const pc_Item = this->mpc_Ui->pc_AppVerticalLayout->itemAt(ou32_Application);

   // Return the path of the application
   if (pc_Item != NULL)
   {
      //lint -e{929}  false positive in PC-Lint: allowed by MISRA 5-2-2
      C_SyvUpUpdatePackageListNodeAppWidget * const pc_App =
         dynamic_cast<C_SyvUpUpdatePackageListNodeAppWidget *>(pc_Item->widget());

      if (pc_App != NULL)
      {
         pc_App->SetSelected(oq_Select);
      }
   }
}

//-----------------------------------------------------------------------------
QString C_SyvUpUpdatePackageListNodeWidget::m_GetApplicationPath(const uint32 ou32_Application) const
{
   QLayoutItem * const pc_Item = this->mpc_Ui->pc_AppVerticalLayout->itemAt(ou32_Application);
   QString c_Path = "";

   // Return the path of the application
   if (pc_Item != NULL)
   {
      //lint -e{929}  false positive in PC-Lint: allowed by MISRA 5-2-2
      C_SyvUpUpdatePackageListNodeAppWidget * const pc_App =
         dynamic_cast<C_SyvUpUpdatePackageListNodeAppWidget *>(pc_Item->widget());

      if (pc_App != NULL)
      {
         c_Path = pc_App->GetAppFilePath();
      }
   }

   return c_Path;
}

//-----------------------------------------------------------------------------
void C_SyvUpUpdatePackageListNodeWidget::m_SetApplicationConnected(const uint32 ou32_Application,
                                                                   const bool oq_Connected) const
{
   QLayoutItem * const pc_Item = this->mpc_Ui->pc_AppVerticalLayout->itemAt(ou32_Application);

   // Return the path of the application
   if (pc_Item != NULL)
   {
      //lint -e{929}  false positive in PC-Lint: allowed by MISRA 5-2-2
      C_SyvUpUpdatePackageListNodeAppWidget * const pc_App =
         dynamic_cast<C_SyvUpUpdatePackageListNodeAppWidget *>(pc_Item->widget());

      if (pc_App != NULL)
      {
         pc_App->SetConnected(oq_Connected);
      }
   }
}

//-----------------------------------------------------------------------------
uint32 C_SyvUpUpdatePackageListNodeWidget::m_GetFirstNotFinishedApplication(void) const
{
   sint32 s32_ApplicationCounter;

   for (s32_ApplicationCounter = 0U;
        s32_ApplicationCounter < static_cast<sint32>(this->mpc_Ui->pc_AppVerticalLayout->count() - 1);
        ++s32_ApplicationCounter)
   {
      if (this->m_GetApplicationState(s32_ApplicationCounter) !=
          C_SyvUpUpdatePackageListNodeAppWidget::hu32_STATE_FINISHED)
      {
         break;
      }
   }

   return static_cast<uint32>(s32_ApplicationCounter);
}

//-----------------------------------------------------------------------------
/*!
   \brief   Check if mime valid

   \param[in]     opc_Mime     Mime to check
   \param[in]     orc_Pos      Mouse position
   \param[in,out] opc_FilePath Optional parameter for file path output if valid
   \param[out]    oppc_App     Found application widget

   \return
   true  Valid
   false Invalid

   \created     22.02.2018  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
bool C_SyvUpUpdatePackageListNodeWidget::m_CheckMime(const QMimeData * const opc_Mime, const QPoint & orc_Pos,
                                                     QStringList * const opc_FilePaths,
                                                     C_SyvUpUpdatePackageListNodeAppWidget ** const oppc_App) const
{
   bool q_Retval = false;

   // Check the mime parameter
   if (opc_Mime != NULL)
   {
      if (opc_Mime->hasUrls() == true)
      {
         QStringList c_PathList;
         QList<QUrl> c_UrlList = opc_Mime->urls();

         // extract the local paths of the files
         for (sintn sn_It = 0; sn_It < c_UrlList.size(); ++sn_It)
         {
            c_PathList.append(c_UrlList.at(sn_It).toLocalFile());
         }

         if (((this->mq_FileBased == false) && (c_PathList.size() == 1)) ||
             ((this->mq_FileBased == true) && (c_PathList.size() > 0)))
         {
            QFileInfo c_File;
            sintn sn_FileCounter;

            q_Retval = true;

            // Check all paths
            for (sn_FileCounter = 0U; sn_FileCounter < c_PathList.size(); ++sn_FileCounter)
            {
               c_File.setFile(c_PathList[sn_FileCounter]);
               if (c_File.exists() == false)
               {
                  q_Retval = false;
                  break;
               }
            }

            if (opc_FilePaths != NULL)
            {
               // Return the paths
               *opc_FilePaths = c_PathList;
            }
         }
      }
   }

   if (q_Retval == true)
   {
      uint32 u32_FoundAppNumber;
      uint32 u32_CounterApp;

      // Check the conditions of the node
      q_Retval = false;

      // Address based nodes can only adapt existing applications
      C_SyvUpUpdatePackageListNodeAppWidget * const pc_App = this->GetApplication(orc_Pos, &u32_FoundAppNumber);

      if (oppc_App != NULL)
      {
         *oppc_App = pc_App;
      }

      for (u32_CounterApp = 0U; u32_CounterApp < this->mu32_AppCount; ++u32_CounterApp)
      {
         if ((u32_CounterApp == u32_FoundAppNumber) &&
             (pc_App != NULL))
         {
            pc_App->SetSelected(true);
         }
         else
         {
            this->m_SetApplicationSelect(u32_CounterApp, false);
         }
      }

      // File based nodes can add new files, so it is allowed always
      if ((pc_App != NULL) ||
          (this->mq_FileBased == true))
      {
         q_Retval = true;
      }
   }

   return q_Retval;
}

//-----------------------------------------------------------------------------
void C_SyvUpUpdatePackageListNodeWidget::m_UpdateNumbers(void) const
{
   sintn sn_AppWidgetCounter;

   for (sn_AppWidgetCounter = 0; sn_AppWidgetCounter < this->mpc_Ui->pc_AppVerticalLayout->count();
        ++sn_AppWidgetCounter)
   {
      QLayoutItem * const pc_Item = this->mpc_Ui->pc_AppVerticalLayout->itemAt(sn_AppWidgetCounter);

      if (pc_Item != NULL)
      {
         //lint -e{929}  false positive in PC-Lint: allowed by MISRA 5-2-2
         C_SyvUpUpdatePackageListNodeAppWidget * const pc_App =
            dynamic_cast<C_SyvUpUpdatePackageListNodeAppWidget *>(pc_Item->widget());

         if (pc_App != NULL)
         {
            pc_App->SetAppNumber(static_cast<uint32>(sn_AppWidgetCounter));
         }
      }
   }
}

//-----------------------------------------------------------------------------
void C_SyvUpUpdatePackageListNodeWidget::m_ButtonAddNewFile(void)
{
   Q_EMIT this->SigButtonAddFileClicked(this);
}
