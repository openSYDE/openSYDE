//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Widget for listing and handling any files

   \copyright   Copyright 2019 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.h"

#include <QFileInfo>

#include "stwtypes.h"
#include "stwerrors.h"
#include "C_GtGetText.h"
#include "TGLUtils.h"
#include "C_ImpUtil.h"
#include "constants.h"

#include "C_SyvUpPackageListNodeFilesWidget.h"
#include "ui_C_SyvUpUpdatePackageListNodeWidget.h"
#include "C_SyvUpPackageListNodeItemFileWidget.h"

#include "C_PuiSdHandler.h"
#include "C_PuiSvHandler.h"
#include "C_PuiSvData.h"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw_types;
using namespace stw_errors;
using namespace stw_opensyde_gui;
using namespace stw_opensyde_gui_logic;
using namespace stw_opensyde_gui_elements;
using namespace stw_opensyde_core;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

/* -- Global Variables ---------------------------------------------------------------------------------------------- */

/* -- Module Global Variables --------------------------------------------------------------------------------------- */

/* -- Module Global Function Prototypes ----------------------------------------------------------------------------- */

/* -- Implementation ------------------------------------------------------------------------------------------------ */

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Default constructor

   Set up GUI with all elements.

   \param[in,out] opc_Parent           Optional pointer to parent
*/
//----------------------------------------------------------------------------------------------------------------------
C_SyvUpPackageListNodeFilesWidget::C_SyvUpPackageListNodeFilesWidget(QWidget * const opc_Parent) :
   C_SyvUpUpdatePackageListNodeWidget(opc_Parent)
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Adds a new file for the file container

   Only possible for file based devices

   \param[in]     orc_File      New path
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvUpPackageListNodeFilesWidget::AddFile(const QString & orc_File)
{
   if (this->mq_FileBased == true)
   {
      const C_OSCNode * const pc_Node = C_PuiSdHandler::h_GetInstance()->GetOSCNodeConst(this->mu32_NodeIndex);
      const C_PuiSvData * const pc_View = C_PuiSvHandler::h_GetInstance()->GetView(this->mu32_ViewIndex);

      if ((pc_Node != NULL) &&
          (pc_View != NULL))
      {
         // Add the file
         C_SyvUpPackageListNodeItemFileWidget * const pc_AppWidget =
            new C_SyvUpPackageListNodeItemFileWidget(this->mu32_ViewIndex, this->mu32_NodeIndex, this->mc_DeviceType,
                                                     this->mq_FileBased, this->mq_StwFlashloader, this);
         QFileInfo c_File(orc_File);

         // Pre initialization of the size. If this is not set, the eliding of the path label will cause a
         // visible resizing
         pc_AppWidget->setGeometry(this->width(), 40, 0, 0);

         pc_AppWidget->SetAppFile(orc_File, false);
         pc_AppWidget->SetAppName(c_File.fileName());
         pc_AppWidget->SetAppType(C_OSCNodeApplication::eBINARY);
         pc_AppWidget->SetAppNumber(static_cast<uint32>(this->mpc_Ui->pc_AppVerticalLayout->count() - 1));

         this->mpc_Ui->pc_AppVerticalLayout->insertWidget(this->mpc_Ui->pc_AppVerticalLayout->count() - 1,
                                                          pc_AppWidget);

         // Save the new file as application path
         tgl_assert(C_PuiSvHandler::h_GetInstance()->AddNodeUpdateInformationPath(
                       this->mu32_ViewIndex, this->mu32_NodeIndex, orc_File,
                       C_PuiSvNodeUpdate::eFTP_FILE_BASED) == C_NO_ERR);

         this->mpc_Ui->pc_WidgetAdd->setVisible(false);
         ++this->mu32_FileCount;

         this->m_FileCountChanged();
      }  //lint !e429  //no memory leak because of the parent of pc_AppWidget and the Qt memory management
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Sets a new file for the application

   \param[in]     orc_File      New path
   \param[in]     opc_App       Application widget
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvUpPackageListNodeFilesWidget::AdaptFile(const QString & orc_File,
                                                  C_SyvUpUpdatePackageListNodeItemWidget * const opc_App)
{
   if (opc_App != NULL)
   {
      QFileInfo c_File(orc_File);

      C_SyvUpUpdatePackageListNodeWidget::AdaptFile(orc_File, opc_App);

      opc_App->SetAppName(c_File.fileName());

      tgl_assert(C_PuiSvHandler::h_GetInstance()->SetNodeUpdateInformationPath(
                    this->mu32_ViewIndex, this->mu32_NodeIndex, opc_App->GetAppNumber(), orc_File,
                    C_PuiSvNodeUpdate::eFTP_FILE_BASED) == C_NO_ERR);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Removes the current application

   \param[in]     opc_App       Application widget
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvUpPackageListNodeFilesWidget::RemoveFile(C_SyvUpUpdatePackageListNodeItemWidget * const opc_App)
{
   if (opc_App != NULL)
   {
      const uint32 u32_AppNumber = opc_App->GetAppNumber();

      // Remove the application file path
      tgl_assert(C_PuiSvHandler::h_GetInstance()->RemoveNodeUpdateInformationPath(this->mu32_ViewIndex,
                                                                                  this->mu32_NodeIndex,
                                                                                  u32_AppNumber,
                                                                                  C_PuiSvNodeUpdate::
                                                                                  eFTP_FILE_BASED) == C_NO_ERR);
   }

   C_SyvUpUpdatePackageListNodeWidget::RemoveFile(opc_App);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Prepares the update package node configuration

   \param[out]     orc_NodeConfig         Node configuration
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvUpPackageListNodeFilesWidget::PrepareExportConfig(C_SyvUpUpdatePackageConfigNode & orc_NodeConfig) const
{
   // One spacer is in the layout and is not relevant
   if (this->mpc_Ui->pc_AppVerticalLayout->count() > 1)
   {
      sintn sn_Counter;
      const sintn sn_AppCount = this->mpc_Ui->pc_AppVerticalLayout->count() - 1;

      orc_NodeConfig.c_FileConfigs.reserve(sn_AppCount);

      for (sn_Counter = 0; sn_Counter < this->mpc_Ui->pc_AppVerticalLayout->count(); ++sn_Counter)
      {
         QLayoutItem * const pc_Item = this->mpc_Ui->pc_AppVerticalLayout->itemAt(sn_Counter);

         if (pc_Item != NULL)
         {

            C_SyvUpUpdatePackageListNodeItemWidget * const pc_App =
               dynamic_cast<C_SyvUpUpdatePackageListNodeItemWidget *>(pc_Item->widget());

            if (pc_App != NULL)
            {
               orc_NodeConfig.c_FileConfigs.push_back(pc_App->GetAppFilePath());
            }
         }
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Loads all matching configurations for this node of the import configuration

   \param[out]     orc_Config         Import configuration
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvUpPackageListNodeFilesWidget::LoadImportConfig(const C_SyvUpUpdatePackageConfig & orc_Config)
{
   uint32 u32_ConfigCounter;

   // Search the matching configuration
   for (u32_ConfigCounter = 0; u32_ConfigCounter < orc_Config.c_NodeConfigs.size(); ++u32_ConfigCounter)
   {
      if ((orc_Config.c_NodeConfigs[u32_ConfigCounter].c_Name == this->mc_NodeName) &&
          (orc_Config.c_NodeConfigs[u32_ConfigCounter].c_DeviceType == this->mc_DeviceType))
      {
         uintn un_FileCounter;

         // Remove the previous applications
         this->RemoveAllFiles();

         // Add the imported configuration as new applications
         for (un_FileCounter = 0U;
              un_FileCounter < orc_Config.c_NodeConfigs[u32_ConfigCounter].c_FileConfigs.size();
              ++un_FileCounter)
         {
            // Update UI and update view data handling
            this->AddFile(orc_Config.c_NodeConfigs[u32_ConfigCounter].c_FileConfigs[un_FileCounter]);
         }

         break;
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Check if mime valid

   File based list accept multiple files

   \param[in,out]   orc_PathList             File paths to analyze, relevant paths in opc_RelevantFilePaths in
                                             will be removed here
   \param[in]       orc_Pos                  Mouse position
   \param[out]      opc_RelevantFilePaths    File paths which could be used for this list
   \param[out]      oppc_App                 Found application widget

   \return
   true  Valid
   false Invalid
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_SyvUpPackageListNodeFilesWidget::CheckMime(QStringList & orc_PathList, const QPoint & orc_Pos,
                                                  QStringList * const opc_RelevantFilePaths,
                                                  C_SyvUpUpdatePackageListNodeItemWidget ** const oppc_App) const
{
   bool q_Retval = false;

   if (orc_PathList.size() > 0)
   {
      C_SyvUpUpdatePackageListNodeItemWidget * pc_App = NULL;

      q_Retval = C_SyvUpUpdatePackageListNodeWidget::CheckMime(orc_PathList, orc_Pos, opc_RelevantFilePaths, &pc_App);

      if (q_Retval == true)
      {
         if (oppc_App != NULL)
         {
            *oppc_App = pc_App;
         }

         if (opc_RelevantFilePaths != NULL)
         {
            *opc_RelevantFilePaths = orc_PathList;
            orc_PathList.clear();
         }
      }
   }

   return q_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get type of subsection (data block/parameter set file/file based node file)

   \retval type file of a file based node
*/
//----------------------------------------------------------------------------------------------------------------------
uint32 C_SyvUpPackageListNodeFilesWidget::Type(void) const
{
   return mu32_UPDATE_PACKAGE_NODE_SECTION_TYPE_FILE;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Initialization of the applications in case of datablock configuration

   \param[in]       orc_Node         Current node
   \param[in]       orc_UpdateInfo   Configured update configuration of view
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvUpPackageListNodeFilesWidget::m_InitSpecificItem(const stw_opensyde_core::C_OSCNode & orc_Node,
                                                           const stw_opensyde_gui_logic::C_PuiSvNodeUpdate & orc_UpdateInfo)
{
   const std::vector<QString> c_ViewAppPaths = orc_UpdateInfo.GetPaths(C_PuiSvNodeUpdate::eFTP_FILE_BASED);
   uint32 u32_Counter;

   Q_UNUSED(orc_Node)

   for (u32_Counter = 0U; u32_Counter < c_ViewAppPaths.size(); ++u32_Counter)
   {
      C_SyvUpPackageListNodeItemFileWidget * const pc_AppWidget =
         new C_SyvUpPackageListNodeItemFileWidget(this->mu32_ViewIndex, this->mu32_NodeIndex,
                                                  this->mc_DeviceType,
                                                  this->mq_FileBased, this->mq_StwFlashloader, this);

      QFileInfo c_File(c_ViewAppPaths[u32_Counter]);
      pc_AppWidget->SetAppFile(c_ViewAppPaths[u32_Counter], false);
      pc_AppWidget->SetAppName(c_File.fileName());
      pc_AppWidget->SetAppNumber(u32_Counter);

      ++this->mu32_FileCount;
      this->mpc_Ui->pc_AppVerticalLayout->addWidget(pc_AppWidget);
   }  //lint !e429  //no memory leak because of the parent of pc_AppWidget and the Qt memory management

   this->mq_ShowAddButton = true;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Initialize all displayed static names
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvUpPackageListNodeFilesWidget::m_InitStaticNames(void) const
{
   this->mpc_Ui->pc_LabTitle->setText(C_GtGetText::h_GetText("Files (optional)"));
   this->mpc_Ui->pc_LabIcon->SetSvg(":images/system_views/IconApp.svg");
}
