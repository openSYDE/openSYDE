//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       List for showing all nodes of the update package

   \copyright   Copyright 2018 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.h"

#include <QFileDialog>
#include <QPainter>
#include <QProcess>

#include "stwtypes.h"
#include "stwerrors.h"
#include "constants.h"

#include "C_SyvUpUpdatePackageListWidget.h"

#include "TGLUtils.h"
#include "C_Uti.h"
#include "C_OgeWiUtil.h"
#include "C_PuiSvHandler.h"
#include "C_PuiSdHandler.h"
#include "C_PuiProject.h"
#include "C_OSCNode.h"
#include "C_GtGetText.h"
#include "C_SyvUpUpdatePackageConfig.h"
#include "C_SyvUpUpdatePackageConfigFiler.h"
#include "C_OSCLoggingHandler.h"
#include "C_OSCSuServiceUpdatePackage.h"
#include "TGLFile.h"
#include "C_OgeWiCustomMessage.h"
#include "C_ImpUtil.h"
#include "C_UsHandler.h"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw_types;
using namespace stw_errors;
using namespace stw_scl;
using namespace stw_opensyde_gui;
using namespace stw_opensyde_gui_logic;
using namespace stw_opensyde_core;
using namespace stw_tgl;
using namespace std;
using namespace stw_opensyde_gui_elements;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */
const QString C_SyvUpUpdatePackageListWidget::mhc_CONFIG_FILE_TYPE = ".syde_up";

/* -- Types --------------------------------------------------------------------------------------------------------- */

/* -- Global Variables ---------------------------------------------------------------------------------------------- */

/* -- Module Global Variables --------------------------------------------------------------------------------------- */

/* -- Module Global Function Prototypes ----------------------------------------------------------------------------- */

/* -- Implementation ------------------------------------------------------------------------------------------------ */

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default constructor

   \param[in,out] opc_Parent Optional pointer to parent
*/
//----------------------------------------------------------------------------------------------------------------------
C_SyvUpUpdatePackageListWidget::C_SyvUpUpdatePackageListWidget(QWidget * const opc_Parent) :
   C_OgeHorizontalListWidget(opc_Parent),
   mpc_ContextMenu(NULL),
   mpc_SelectedNode(NULL),
   mpc_SelectedSection(NULL),
   mpc_SelectedApp(NULL),
   mpc_AddFileAction(NULL),
   mpc_SelectFileAction(NULL),
   mpc_RevertFileAction(NULL),
   mpc_RemoveFileAction(NULL),
   mpc_ShowFileInfoAction(NULL),
   mpc_RemoveAllNodeFilesAction(NULL),
   mpc_HideShowOptionalSectionsAction(NULL),
   mpc_ShowInExplorerAction(NULL),
   mc_LastPath(""),
   mu32_ViewIndex(0U),
   mq_Connected(false),
   mq_EmptyOptionalSectionsVisible(true)
{
   this->setItemDelegate(&this->mc_Delegate);

   this->m_SetupContextMenu();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   default destructor

   Clean up.
*/
//----------------------------------------------------------------------------------------------------------------------
C_SyvUpUpdatePackageListWidget::~C_SyvUpUpdatePackageListWidget()
{
   // save user settings
   const C_PuiSvData * const pc_View = C_PuiSvHandler::h_GetInstance()->GetView(this->mu32_ViewIndex);

   // store configuration of the view
   if (pc_View != NULL)
   {
      // save visibility of empty optional widgets
      C_UsHandler::h_GetInstance()->SetProjSvUpdateEmptyOptionalSectionsVisible(pc_View->GetName(),
                                                                                this->mq_EmptyOptionalSectionsVisible);
   }

   //lint -e{1540}  no memory leak because of the parent of all elements and the Qt memory management
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Sets the view index and initializes the nodes in the list

   \param[in]     ou32_ViewIndex         View index
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvUpUpdatePackageListWidget::SetViewIndex(const uint32 ou32_ViewIndex)
{
   const C_PuiSvData * const pc_View = C_PuiSvHandler::h_GetInstance()->GetView(ou32_ViewIndex);

   this->clear();

   if (pc_View != NULL)
   {
      std::vector<uint8> c_ActiveNodes = pc_View->GetNodeActiveFlags();
      const std::vector<C_PuiSvNodeUpdate> & rc_NodeUpdate = pc_View->GetAllNodeUpdateInformation();
      uint32 u32_CurrentPosition = 0U;
      uint32 u32_FoundNodes = 0U;
      uint32 u32_NodeUpdateCounter;

      this->mu32_ViewIndex = ou32_ViewIndex;

      tgl_assert(rc_NodeUpdate.size() == c_ActiveNodes.size());

      do
      {
         for (u32_NodeUpdateCounter = 0U; u32_NodeUpdateCounter < rc_NodeUpdate.size(); ++u32_NodeUpdateCounter)
         {
            // Search for all nodes with the current position
            // In case of previous deactivated nodes or nodes with new added data blocks, adding a node with the
            // same position after the previous one too
            if (rc_NodeUpdate[u32_NodeUpdateCounter].u32_NodeUpdatePosition == u32_CurrentPosition)
            {
               if (c_ActiveNodes[u32_NodeUpdateCounter] == true)
               {
                  const C_OSCNode * const pc_Node = C_PuiSdHandler::h_GetInstance()->GetOSCNodeConst(
                     u32_NodeUpdateCounter);

                  if (pc_Node != NULL)
                  {
                     // Add openSYDE devices only due the possibility to add parameter set image files always
                     tgl_assert(pc_Node->pc_DeviceDefinition != NULL);
                     if (((pc_Node->c_Applications.size() > 0) ||
                          (pc_Node->c_Properties.e_FlashLoader == C_OSCNodeProperties::eFL_OPEN_SYDE)) ||
                         (pc_Node->pc_DeviceDefinition->q_FlashloaderOpenSydeIsFileBased == true))
                     {
                        this->m_AddNodeWidget(u32_NodeUpdateCounter, pc_Node->c_Properties.c_Name.c_str());
                     }
                  }
               }

               ++u32_FoundNodes;
            }
         }

         ++u32_CurrentPosition;
         // Run till all nodes were found, but not all must be added to the list
      }
      while (u32_FoundNodes < c_ActiveNodes.size());

      // Maximum 3 nodes shall be visible in the list at the same time
      if (this->count() == 2)
      {
         this->msn_ItemsPerLine = 2;
      }
      else if (this->count() >= 3)
      {
         this->msn_ItemsPerLine = 3;
      }
      else
      {
         this->msn_ItemsPerLine = 1;
      }

      // Get the line count
      this->msn_CountLines = this->count() / this->msn_ItemsPerLine;
      // Correct rounding error
      if ((this->count() % this->msn_ItemsPerLine) > 0)
      {
         ++this->msn_CountLines;
      }

      // load view specific user settings
      const C_UsSystemView c_View = C_UsHandler::h_GetInstance()->GetProjSvSetupView(pc_View->GetName());
      // hide show optional sections toggles visibility -> set flag to negotiated value:
      this->mq_EmptyOptionalSectionsVisible = !c_View.GetUpdatePackEmptyOptionalSectionsVisible();
      this->m_HideShowOptionalSections();
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Adapts the widget for the connected state
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvUpUpdatePackageListWidget::SetConnected(void)
{
   sintn sn_Counter;

   this->mq_Connected = true;
   this->setDragEnabled(false);

   for (sn_Counter = 0; sn_Counter < this->count(); ++sn_Counter)
   {
      // activate the first item and deactivate all other items
      QListWidgetItem * const pc_Item = this->item(sn_Counter);

      //lint -e{929}  false positive in PC-Lint: allowed by MISRA 5-2-2
      C_SyvUpUpdatePackageNodeWidget * const pc_WidgetItem =
         dynamic_cast<C_SyvUpUpdatePackageNodeWidget *>(this->itemWidget(pc_Item));
      if (pc_WidgetItem != NULL)
      {
         // Adapt size of the widget
         pc_WidgetItem->SetConnected();
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Adapts the widget when the updated was started
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvUpUpdatePackageListWidget::SetUpdateStarted(void) const
{
   sintn sn_Counter;

   for (sn_Counter = 0; sn_Counter < this->count(); ++sn_Counter)
   {
      // activate the first item and deactivate all other items
      QListWidgetItem * const pc_Item = this->item(sn_Counter);

      //lint -e{929}  false positive in PC-Lint: allowed by MISRA 5-2-2
      const C_SyvUpUpdatePackageNodeWidget * const pc_WidgetItem =
         dynamic_cast<const C_SyvUpUpdatePackageNodeWidget *>(this->itemWidget(pc_Item));
      if (pc_WidgetItem != NULL)
      {
         // Adapt size of the widget
         pc_WidgetItem->SetUpdateStarted();
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Adapts the widget when the application update was started

   \param[in]     ou32_NodeIndex         Index of node
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvUpUpdatePackageListWidget::SetUpdateApplicationStarted(const uint32 ou32_NodeIndex)
{
   sintn sn_Counter;

   for (sn_Counter = 0; sn_Counter < this->count(); ++sn_Counter)
   {
      // activate the first item and deactivate all other items
      QListWidgetItem * const pc_Item = this->item(sn_Counter);

      //lint -e{929}  false positive in PC-Lint: allowed by MISRA 5-2-2
      const C_SyvUpUpdatePackageNodeWidget * const pc_WidgetItem =
         dynamic_cast<const C_SyvUpUpdatePackageNodeWidget *>(this->itemWidget(pc_Item));
      if (pc_WidgetItem != NULL)
      {
         if (pc_WidgetItem->GetNodeIndex() == ou32_NodeIndex)
         {
            // Adapt size of the widget
            pc_WidgetItem->SetUpdateApplicationStarted();
            this->ScrollToItem(sn_Counter);
            break;
         }
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Adapts the widget when the application update was finished

   \param[in]     ou32_NodeIndex         Index of node
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvUpUpdatePackageListWidget::SetUpdateApplicationFinished(const uint32 ou32_NodeIndex) const
{
   sintn sn_Counter;

   for (sn_Counter = 0; sn_Counter < this->count(); ++sn_Counter)
   {
      // activate the first item and deactivate all other items
      QListWidgetItem * const pc_Item = this->item(sn_Counter);

      //lint -e{929}  false positive in PC-Lint: allowed by MISRA 5-2-2
      C_SyvUpUpdatePackageNodeWidget * const pc_WidgetItem =
         dynamic_cast<C_SyvUpUpdatePackageNodeWidget *>(this->itemWidget(pc_Item));
      if (pc_WidgetItem != NULL)
      {
         if (pc_WidgetItem->GetNodeIndex() == ou32_NodeIndex)
         {
            // Adapt size of the widget
            pc_WidgetItem->SetUpdateApplicationFinished();
            break;
         }
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Adapts the widget when the application update hat an error

   \param[in]     ou32_NodeIndex         Index of node
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvUpUpdatePackageListWidget::SetUpdateApplicationError(const uint32 ou32_NodeIndex) const
{
   sintn sn_Counter;

   for (sn_Counter = 0; sn_Counter < this->count(); ++sn_Counter)
   {
      // activate the first item and deactivate all other items
      QListWidgetItem * pc_Item = this->item(sn_Counter);

      //lint -e{929}  false positive in PC-Lint: allowed by MISRA 5-2-2
      const C_SyvUpUpdatePackageNodeWidget * pc_WidgetItem =
         dynamic_cast<const C_SyvUpUpdatePackageNodeWidget *>(this->itemWidget(pc_Item));
      if (pc_WidgetItem != NULL)
      {
         if (pc_WidgetItem->GetNodeIndex() == ou32_NodeIndex)
         {
            // Adapt size of the widget
            pc_WidgetItem->SetUpdateApplicationError();
            break;
         }
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Signal the widget to discard all device application information

   \param[in] ou32_NodeIndex Index of node
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvUpUpdatePackageListWidget::DiscardApplicationStatus(const uint32 ou32_NodeIndex) const
{
   sintn sn_Counter;

   for (sn_Counter = 0; sn_Counter < this->count(); ++sn_Counter)
   {
      // Search for match
      QListWidgetItem * const pc_Item = this->item(sn_Counter);

      //lint -e{929}  false positive in PC-Lint: allowed by MISRA 5-2-2
      C_SyvUpUpdatePackageNodeWidget * const pc_WidgetItem =
         dynamic_cast<C_SyvUpUpdatePackageNodeWidget *>(this->itemWidget(pc_Item));
      if (pc_WidgetItem != NULL)
      {
         if (pc_WidgetItem->GetNodeIndex() == ou32_NodeIndex)
         {
            // Discard info
            pc_WidgetItem->DiscardApplicationStatus();
            break;
         }
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Adapts the widget when the entire update was finished
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvUpUpdatePackageListWidget::SetUpdateFinished(void) const
{
   sintn sn_Counter;

   for (sn_Counter = 0; sn_Counter < this->count(); ++sn_Counter)
   {
      // activate the first item and deactivate all other items
      QListWidgetItem * const pc_Item = this->item(sn_Counter);

      //lint -e{929}  false positive in PC-Lint: allowed by MISRA 5-2-2
      C_SyvUpUpdatePackageNodeWidget * const pc_WidgetItem =
         dynamic_cast<C_SyvUpUpdatePackageNodeWidget *>(this->itemWidget(pc_Item));
      if (pc_WidgetItem != NULL)
      {
         // Adapt size of the widget
         pc_WidgetItem->SetUpdateFinished();
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Adapts the widget for the disconnected state
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvUpUpdatePackageListWidget::SetDisconnected(void)
{
   sintn sn_Counter;

   for (sn_Counter = 0; sn_Counter < this->count(); ++sn_Counter)
   {
      // activate the first item and deactivate all other items
      QListWidgetItem * const pc_Item = this->item(sn_Counter);

      //lint -e{929}  false positive in PC-Lint: allowed by MISRA 5-2-2
      C_SyvUpUpdatePackageNodeWidget * const pc_WidgetItem =
         dynamic_cast<C_SyvUpUpdatePackageNodeWidget *>(this->itemWidget(pc_Item));
      if (pc_WidgetItem != NULL)
      {
         // Adapt size of the widget
         pc_WidgetItem->SetDisconnected();
      }
   }

   this->mq_Connected = false;
   this->setDragEnabled(true);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Update device information

   \param[in] orc_NodeIndexes       Node indices
   \param[in] orc_DeviceInformation Device info
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvUpUpdatePackageListWidget::UpdateDeviceInformation(const std::vector<uint32> & orc_NodeIndexes,
                                                             const std::vector<C_SyvUpDeviceInfo> & orc_DeviceInformation)
const
{
   tgl_assert(orc_NodeIndexes.size() == orc_DeviceInformation.size());
   if (orc_NodeIndexes.size() == orc_DeviceInformation.size())
   {
      sintn sn_NodeWidgetCounter;

      for (sn_NodeWidgetCounter = 0; sn_NodeWidgetCounter < this->count(); ++sn_NodeWidgetCounter)
      {
         // activate the first item and deactivate all other items
         QListWidgetItem * const pc_Item = this->item(sn_NodeWidgetCounter);

         //lint -e{929}  false positive in PC-Lint: allowed by MISRA 5-2-2
         C_SyvUpUpdatePackageNodeWidget * const pc_WidgetItem =
            dynamic_cast<C_SyvUpUpdatePackageNodeWidget *>(this->itemWidget(pc_Item));
         if (pc_WidgetItem != NULL)
         {
            uint32 u32_NodeIndexCounter;

            for (u32_NodeIndexCounter = 0; u32_NodeIndexCounter < orc_NodeIndexes.size(); ++u32_NodeIndexCounter)
            {
               if (pc_WidgetItem->GetNodeIndex() == orc_NodeIndexes[u32_NodeIndexCounter])
               {
                  // Update device information
                  pc_WidgetItem->UpdateDeviceInformation(orc_DeviceInformation[u32_NodeIndexCounter]);
                  break;
               }
            }
         }
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   short description of function

   \param[in]     ou32_NodeIndex         Index of node
   \param[in]     ou8_Progress           Entire progress of node of update process
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvUpUpdatePackageListWidget::SetNodeProgress(const uint32 ou32_NodeIndex, const uint8 ou8_Progress) const
{
   sintn sn_Counter;

   for (sn_Counter = 0; sn_Counter < this->count(); ++sn_Counter)
   {
      // activate the first item and deactivate all other items
      QListWidgetItem * const pc_Item = this->item(sn_Counter);
      //lint -e{929}  false positive in PC-Lint: allowed by MISRA 5-2-2
      const C_SyvUpUpdatePackageNodeWidget * const pc_WidgetItem =
         dynamic_cast<const C_SyvUpUpdatePackageNodeWidget *>(this->itemWidget(pc_Item));
      if (pc_WidgetItem != NULL)
      {
         if (pc_WidgetItem->GetNodeIndex() == ou32_NodeIndex)
         {
            // Adapt size of the widget
            pc_WidgetItem->SetProgress(ou8_Progress);
            break;
         }
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
void C_SyvUpUpdatePackageListWidget::RemoveAllFiles(void) const
{
   sintn sn_Counter;

   for (sn_Counter = 0; sn_Counter < this->count(); ++sn_Counter)
   {
      // activate the first item and deactivate all other items
      QListWidgetItem * const pc_Item = this->item(sn_Counter);

      //lint -e{929}  false positive in PC-Lint: allowed by MISRA 5-2-2
      C_SyvUpUpdatePackageNodeWidget * const pc_WidgetItem =
         dynamic_cast<C_SyvUpUpdatePackageNodeWidget *>(this->itemWidget(pc_Item));
      if (pc_WidgetItem != NULL)
      {
         pc_WidgetItem->RemoveAllFiles();
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Exports the current configuration of the update package
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvUpUpdatePackageListWidget::ExportConfig(void)
{
   const QString c_Folder = this->m_GetDialogPath();
   QString c_FileName =
      C_OgeWiUtil::h_GetSaveFileName(this, C_GtGetText::h_GetText("Export Update Package Configuration"), c_Folder,
                                     QString(C_GtGetText::h_GetText("openSYDE Update Package Configuration File")) +
                                     " (*" + mhc_CONFIG_FILE_TYPE + ")", "");

   if (c_FileName != "")
   {
      // Check if file exists already
      QFile c_File;
      bool q_Continue = true;
      sint32 s32_Result;

      this->mc_LastPath = TGL_ExtractFilePath(c_FileName.toStdString().c_str()).c_str();

      // Remove old file
      c_File.setFileName(c_FileName);
      if (c_File.exists() == true)
      {
         q_Continue = c_File.remove();
      }

      if (q_Continue == true)
      {
         sintn sn_Counter;
         C_SyvUpUpdatePackageConfig c_Config;

         c_Config.c_NodeConfigs.reserve(this->count());

         // Prepare configuration
         for (sn_Counter = 0; sn_Counter < this->count(); ++sn_Counter)
         {
            // Check all paths from all nodes
            QListWidgetItem * const pc_Item = this->item(sn_Counter);

            //lint -e{929}  false positive in PC-Lint: allowed by MISRA 5-2-2
            C_SyvUpUpdatePackageNodeWidget * const pc_WidgetItem =
               dynamic_cast<C_SyvUpUpdatePackageNodeWidget *>(this->itemWidget(pc_Item));
            if (pc_WidgetItem != NULL)
            {
               C_SyvUpUpdatePackageConfigNode c_Node;

               pc_WidgetItem->PrepareExportConfig(c_Node);

               c_Config.c_NodeConfigs.push_back(c_Node);
            }
         }

         // Save configuration
         s32_Result = C_SyvUpUpdatePackageConfigFiler::h_SaveConfig(c_FileName, c_Config);
      }
      else
      {
         s32_Result = C_RD_WR;
      }

      if (s32_Result != C_NO_ERR)
      {
         // TODO: Error handling
         C_OgeWiCustomMessage c_MessageResultSave(this, C_OgeWiCustomMessage::E_Type::eERROR);
         c_MessageResultSave.SetHeading(C_GtGetText::h_GetText("Update Package configuration export"));
         c_MessageResultSave.SetDescription(C_GtGetText::h_GetText("Could not save the file."));
         c_MessageResultSave.SetDetails(C_GtGetText::h_GetText("Error code: \n") + QString::number(s32_Result));
         c_MessageResultSave.Execute();
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Imports a configuration for the update package

   long description of function within several lines
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvUpUpdatePackageListWidget::ImportConfig(void)
{
   const QString c_Folder = this->m_GetDialogPath();
   const QString c_Filter = QString(C_GtGetText::h_GetText("openSYDE Update Package Configuration File")) + "(*" +
                            mhc_CONFIG_FILE_TYPE + ")";
   const QString c_FileName =
      C_OgeWiUtil::h_GetSaveFileName(this, C_GtGetText::h_GetText("Import Update Package Configuration"), c_Folder,
                                     c_Filter, "", QFileDialog::DontConfirmOverwrite);

   if (c_FileName != "")
   {
      sint32 s32_Result;
      C_SyvUpUpdatePackageConfig c_Config;
      C_OgeWiCustomMessage::E_Outputs e_ReturnMessageBox;
      C_OgeWiCustomMessage c_MessageBox(this, C_OgeWiCustomMessage::E_Type::eQUESTION);

      this->mc_LastPath = TGL_ExtractFilePath(c_FileName.toStdString().c_str()).c_str();

      c_MessageBox.SetHeading(C_GtGetText::h_GetText("Update Package configuration import"));
      c_MessageBox.SetDescription(C_GtGetText::h_GetText(
                                     "Do you really want to overwrite the current Update Package configuration?"));
      c_MessageBox.SetOKButtonText(C_GtGetText::h_GetText("Import"));
      c_MessageBox.SetNOButtonText(C_GtGetText::h_GetText("Keep"));
      e_ReturnMessageBox = c_MessageBox.Execute();

      if (e_ReturnMessageBox == C_OgeWiCustomMessage::eYES)
      {
         // Load configuration
         s32_Result = C_SyvUpUpdatePackageConfigFiler::h_LoadConfig(c_FileName, c_Config);

         if (s32_Result == C_NO_ERR)
         {
            sintn sn_Counter;

            // Clear previous configuration first
            this->RemoveAllFiles();

            // Load configuration
            for (sn_Counter = 0; sn_Counter < this->count(); ++sn_Counter)
            {
               // Check all paths from all nodes
               QListWidgetItem * const pc_Item = this->item(sn_Counter);

               //lint -e{929}  false positive in PC-Lint: allowed by MISRA 5-2-2
               C_SyvUpUpdatePackageNodeWidget * const pc_WidgetItem =
                  dynamic_cast<C_SyvUpUpdatePackageNodeWidget *>(this->itemWidget(pc_Item));
               if (pc_WidgetItem != NULL)
               {
                  pc_WidgetItem->LoadImportConfig(c_Config);
               }
            }
         }

         if (s32_Result != C_NO_ERR)
         {
            //Error handling
            QString c_Details;
            C_OgeWiCustomMessage c_MessageResultSave(this, C_OgeWiCustomMessage::E_Type::eERROR);
            c_MessageResultSave.SetHeading(C_GtGetText::h_GetText("Update Package configuration import"));
            c_MessageResultSave.SetDescription(C_GtGetText::h_GetText("Could not load file."));
            switch (s32_Result)
            {
            case C_RD_WR:
               c_Details = C_GtGetText::h_GetText("There are problems accessing the file system.\n"
                                                  "For example, there may be no read access to the file.");
               break;
            case C_NOACT:
               c_Details = C_GtGetText::h_GetText("A file is present but its structure is invalid.\n"
                                                  "For example this can be caused by an invalid XML file.");
               break;
            case C_CONFIG:
               c_Details = C_GtGetText::h_GetText("Content of file is invalid or incomplete");
               break;
            default:
               c_Details = C_GtGetText::h_GetText(
                  "Error code: \n") + QString::number(s32_Result);
               break;
            }
            c_MessageResultSave.SetDetails(c_Details);
            c_MessageResultSave.Execute();
         }
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Creates the service update package
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvUpUpdatePackageListWidget::CreateServiceUpdatePackage(void)
{
   const C_PuiSvData * const pc_ViewData = C_PuiSvHandler::h_GetInstance()->GetView(this->mu32_ViewIndex);

   tgl_assert(pc_ViewData != NULL);

   QString c_DefaultFilename;
   if (pc_ViewData != NULL)
   {
      c_DefaultFilename = pc_ViewData->GetName();
   }

   c_DefaultFilename += "_ServiceUpdatePackage" + QString(C_OSCSuServiceUpdatePackage::h_GetPackageExtension().c_str());
   const QString c_FilterName = QString(C_GtGetText::h_GetText("openSYDE Service Update Package File")) + " (*" +
                                QString(C_GtGetText::h_GetText(C_OSCSuServiceUpdatePackage::h_GetPackageExtension().
                                                               c_str())) + ")";
   const QString c_Folder = this->m_GetDialogPath();
   const QString c_FullPackagePath =
      C_OgeWiUtil::h_GetSaveFileName(this, C_GtGetText::h_GetText(
                                        "Select Directory for Service Update Package"), c_Folder, c_FilterName,
                                     c_DefaultFilename);

   // check for user abort (empty string)
   if (c_FullPackagePath != "")
   {
      sint32 s32_Return = C_NO_ERR;

      this->mc_LastPath = TGL_ExtractFilePath(c_FullPackagePath.toStdString().c_str()).c_str();

      // check for old zip archive
      if (TGL_FileExists(c_FullPackagePath.toStdString().c_str()) == true)
      {
         // delete old zip archive
         if (remove(c_FullPackagePath.toStdString().c_str()) != 0)
         {
            C_OgeWiCustomMessage c_MessageResult(this, C_OgeWiCustomMessage::E_Type::eERROR);
            c_MessageResult.SetHeading(C_GtGetText::h_GetText("Create Service Update Package"));
            c_MessageResult.SetDescription(C_GtGetText::h_GetText("Could not delete old Service Update Package."));
            c_MessageResult.SetDetails(C_GtGetText::h_GetText("Could not delete ") + c_FullPackagePath);
            c_MessageResult.Execute();

            s32_Return = C_BUSY;
         }
      }

      if (s32_Return == C_NO_ERR)
      {
         // system definition
         const stw_opensyde_core::C_OSCSystemDefinition & rc_SystemDefinition =
            C_PuiSdHandler::h_GetInstance()->GetOSCSystemDefinitionConst();

         // active bus index
         const C_PuiSvData * const pc_View = C_PuiSvHandler::h_GetInstance()->GetView(this->mu32_ViewIndex);
         //No check for connected because error check passed
         const stw_types::uint32 u32_ActiveBusIndex = pc_View->GetPcData().GetBusIndex();

         // active nodes
         std::vector<uint8> c_ActiveNodes = pc_View->GetNodeActiveFlags();

         // update applications of nodes, update position of nodes,
         std::vector<C_OSCSuSequences::C_DoFlash> c_ApplicationsToWrite;
         vector<uint32> c_NodesUpdateOrder;

         s32_Return = this->GetUpdatePackage(c_ApplicationsToWrite, c_NodesUpdateOrder);
         tgl_assert(s32_Return == C_NO_ERR);

         C_SCLStringList c_Warnings;
         C_SCLString c_Error;
         if (s32_Return == C_NO_ERR)
         {
            s32_Return = C_OSCSuServiceUpdatePackage::h_CreatePackage(c_FullPackagePath.toStdString().c_str(),
                                                                      rc_SystemDefinition,
                                                                      u32_ActiveBusIndex,
                                                                      c_ActiveNodes,
                                                                      c_NodesUpdateOrder,
                                                                      c_ApplicationsToWrite,
                                                                      c_Warnings,
                                                                      c_Error);
         }

         if (s32_Return == C_NO_ERR)
         {
            C_OgeWiCustomMessage c_MessageResult(this);
            c_MessageResult.SetHeading(C_GtGetText::h_GetText("Create Service Update Package"));
            c_MessageResult.SetDescription(C_GtGetText::h_GetText("Service Update Package successfully created."));
            QString c_Details = QString("%1<a href=\"file:%2\"><span style=\"color: %3;\">%4</span></a>.").
                                arg(C_GtGetText::h_GetText("File saved at ")).
                                arg(c_FullPackagePath).
                                arg(mc_STYLESHEET_GUIDE_COLOR_LINK).
                                arg(c_FullPackagePath);
            c_MessageResult.SetDetails(c_Details);
            c_MessageResult.Execute();
         }
         else if (s32_Return == C_WARN)
         {
            C_OgeWiCustomMessage c_MessageResult(this, C_OgeWiCustomMessage::E_Type::eWARNING);
            c_MessageResult.SetHeading(C_GtGetText::h_GetText("Create Service Update Package"));
            c_MessageResult.SetDescription(C_GtGetText::h_GetText(
                                              "Created Service Update Package but there are warnings.") +
                                           c_FullPackagePath);
            c_MessageResult.SetDetails(C_GtGetText::h_GetText(c_Warnings.GetText().c_str()));
            c_MessageResult.Execute();
         }
         else
         {
            C_OgeWiCustomMessage c_MessageResult(this, C_OgeWiCustomMessage::E_Type::eERROR);
            c_MessageResult.SetHeading(C_GtGetText::h_GetText("Create Service Update Package"));
            c_MessageResult.SetDescription(C_GtGetText::h_GetText("Could not create Service Update Package!"));
            c_MessageResult.SetDetails(C_GtGetText::h_GetText("Error code: ") + QString::number(s32_Return) + "\n" +
                                       C_GtGetText::h_GetText(c_Error.c_str()));
            c_MessageResult.Execute();
         }
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Checks all paths for existence

   \param[out]    oru32_CountFiles           Number of files
   \param[out]    opc_FlashwareWarningsApps  App names of apps with flashware warnings
   \param[out]    opc_MissingDataBlocks      App names of apps with missing data block files
   \param[out]    opc_MissingParamFiles      App names of apps with missing parameter set image files
   \param[out]    opc_MissingFiles           App names of apps with missing files

   \return
   C_NO_ERR    All files are existent
   C_CONFIG    Minimum one file was not found
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_SyvUpUpdatePackageListWidget::CheckAllPaths(uint32 & oru32_CountFiles,
                                                     QStringList * const opc_FlashwareWarningsApps,
                                                     QStringList * const opc_MissingDataBlocks,
                                                     QStringList * const opc_MissingParamFiles,
                                                     QStringList * const opc_MissingFiles) const
{
   sintn sn_Counter;
   sint32 s32_Return = C_NO_ERR;

   for (sn_Counter = 0; sn_Counter < this->count(); ++sn_Counter)
   {
      // Check all paths from all nodes
      QListWidgetItem * const pc_Item = this->item(sn_Counter);

      //lint -e{929}  false positive in PC-Lint: allowed by MISRA 5-2-2
      C_SyvUpUpdatePackageNodeWidget * const pc_WidgetItem =
         dynamic_cast<C_SyvUpUpdatePackageNodeWidget *>(this->itemWidget(pc_Item));
      if (pc_WidgetItem != NULL)
      {
         uint32 u32_CountNodeFiles = 0U;

         // Check the node paths
         if (pc_WidgetItem->CheckAllFiles(u32_CountNodeFiles, opc_FlashwareWarningsApps, opc_MissingDataBlocks,
                                          opc_MissingParamFiles, opc_MissingFiles) != C_NO_ERR)
         {
            s32_Return = C_CONFIG;
         }

         // Count always
         oru32_CountFiles += u32_CountNodeFiles;
      }
   }

   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Creates and returns the update package with all information for the system update

   \param[out]    orc_ApplicationsToWrite Vector with node update configuration
   \param[out]    orc_NodesOrder          Vector with node update order (index is update position, value is node index)
   \param[out]    opc_AllApplications     Optional vector with all node applications

   \return
   C_NO_ERR    Update package with all information created
   C_NOACT     No nodes for updating
   C_CONFIG    Error in system definition
   C_RD_WR     At least one file does not exist
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_SyvUpUpdatePackageListWidget::GetUpdatePackage(
   std::vector<C_OSCSuSequences::C_DoFlash> & orc_ApplicationsToWrite, std::vector<uint32> & orc_NodesOrder,
   std::vector<stw_opensyde_core::C_OSCSuSequences::C_DoFlash> * const opc_AllApplications) const
{
   sint32 s32_Return = C_NOACT;
   sintn sn_Counter;
   bool q_AtLeastOneApplication = false;

   // Count of nodes in system definition must match with the size of c_NodesToFlash
   // But the not active nodes may not have a registered application
   orc_ApplicationsToWrite.clear();
   orc_NodesOrder.clear();

   orc_ApplicationsToWrite.resize(C_PuiSdHandler::h_GetInstance()->GetOSCNodesSize(), C_OSCSuSequences::C_DoFlash());
   // The order position of inactive nodes is not used. Initial value is -1 to differentiate to the active nodes
   orc_NodesOrder.reserve(this->count());

   if (opc_AllApplications != NULL)
   {
      *opc_AllApplications = orc_ApplicationsToWrite;
   }

   for (sn_Counter = 0; sn_Counter < this->count(); ++sn_Counter)
   {
      QListWidgetItem * const pc_Item = this->item(sn_Counter);

      //lint -e{929}  false positive in PC-Lint: allowed by MISRA 5-2-2
      C_SyvUpUpdatePackageNodeWidget * const pc_WidgetItem =
         dynamic_cast<C_SyvUpUpdatePackageNodeWidget *>(this->itemWidget(pc_Item));
      if (pc_WidgetItem != NULL)
      {
         const uint32 u32_NodeIndex = pc_WidgetItem->GetNodeIndex();
         const C_OSCNode * const pc_Node = C_PuiSdHandler::h_GetInstance()->GetOSCNodeConst(u32_NodeIndex);

         tgl_assert(pc_Node != NULL);
         if (pc_Node != NULL)
         {
            C_OSCSuSequences::C_DoFlash & rc_Flash = orc_ApplicationsToWrite[u32_NodeIndex];
            C_OSCSuSequences::C_DoFlash * pc_AllApplications = NULL;

            if (opc_AllApplications != NULL)
            {
               pc_AllApplications = &((*opc_AllApplications)[u32_NodeIndex]);
            }

            // Add the applications of the node
            s32_Return = pc_WidgetItem->GetUpdatePackage(rc_Flash, pc_AllApplications);

            if (s32_Return == C_NO_ERR)
            {
               // Update order position
               orc_NodesOrder.push_back(u32_NodeIndex);

               q_AtLeastOneApplication = true;
            }
            else if (s32_Return == C_NOACT)
            {
               // No application to update for the node.
               // It is an error if no node has at least one application
               s32_Return = C_NO_ERR;
            }
            else
            {
               // Nothing to do
            }
         }
         else
         {
            s32_Return = C_CONFIG;
         }

         if (s32_Return != C_NO_ERR)
         {
            break;
         }
      }
   }

   if ((s32_Return == C_NO_ERR) &&
       (q_AtLeastOneApplication == false))
   {
      // No applications for updating
      s32_Return = C_NOACT;

      osc_write_log_info("Generate Update Package", C_GtGetText::h_GetText(
                            "Generate Update Package: No applications for updating."));
   }
   else if (s32_Return == C_NOACT)
   {
      osc_write_log_info("Generate Update Package", C_GtGetText::h_GetText(
                            "Generate Update Package: No active nodes in the System View."));
   }
   else if (s32_Return == C_CONFIG)
   {
      osc_write_log_info("Generate Update Package", C_GtGetText::h_GetText(
                            "Generate Update Package: View configuration is invalid."));
   }
   else
   {
      // Nothing to do
   }

   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Overwritten resize event slot

   Here: Update integrated widget size

   \param[in,out] opc_Event Event identification and information
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvUpUpdatePackageListWidget::resizeEvent(QResizeEvent * const opc_Event)
{
   sintn sn_Counter;

   QListWidget::resizeEvent(opc_Event);

   for (sn_Counter = 0; sn_Counter < this->count(); ++sn_Counter)
   {
      // activate the first item and deactivate all other items
      QListWidgetItem * const pc_Item = this->item(sn_Counter);

      //lint -e{929}  false positive in PC-Lint: allowed by MISRA 5-2-2
      C_SyvUpUpdatePackageNodeWidget * const pc_WidgetItem =
         dynamic_cast<C_SyvUpUpdatePackageNodeWidget *>(this->itemWidget(pc_Item));
      if (pc_WidgetItem != NULL)
      {
         // Adapt size of the widget
         pc_WidgetItem->resize(((this->width() - 1) / this->msn_ItemsPerLine), this->height());

         pc_Item->setSizeHint(pc_WidgetItem->size());
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Overwritten paint event

   Draws the background element

   \param[in,out] opc_Event  Pointer to paint event
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvUpUpdatePackageListWidget::paintEvent(QPaintEvent * const opc_Event)
{
   QListWidget::paintEvent(opc_Event);

   if (this->count() == 0)
   {
      QPainter c_Painter(this->viewport());
      // show a tip for adding a datapool
      QPen c_Pen = c_Painter.pen();
      QFont c_Font;
      const QString c_Text = QString(C_GtGetText::h_GetText("There are no active nodes to be updated."));

      // configure color
      c_Pen.setColor(mc_STYLE_GUIDE_COLOR_8);
      c_Painter.setPen(c_Pen);

      // configure font
      c_Font = mc_STYLE_GUIDE_FONT_REGULAR_13;
      c_Font.setPixelSize(c_Font.pointSize());
      c_Painter.setFont(c_Font);

      c_Painter.drawText(this->rect(), static_cast<sintn>(Qt::AlignCenter), c_Text);

      c_Pen.setColor(mc_STYLE_GUIDE_COLOR_10);
      c_Painter.setPen(c_Pen);
      c_Painter.drawRect(QRect(5, 10, this->width() - 14, this->height() - 11));
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Overwritten double click event

   Select file on double click
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvUpUpdatePackageListWidget::mouseDoubleClickEvent(QMouseEvent * const opc_Event)
{
   QPoint c_Pos = opc_Event->pos();

   QListWidgetItem * const pc_Item = this->itemAt(c_Pos);

   this->mpc_SelectedNode = NULL;
   this->mpc_SelectedSection = NULL;
   this->mpc_SelectedApp = NULL;

   if ((pc_Item != NULL) &&
       (this->mq_Connected == false))
   {
      const QModelIndex c_Current = this->currentIndex();
      //lint -e{929}  false positive in PC-Lint: allowed by MISRA 5-2-2
      C_SyvUpUpdatePackageNodeWidget * const pc_WidgetItem =
         dynamic_cast<C_SyvUpUpdatePackageNodeWidget *>(this->itemWidget(pc_Item));

      if ((c_Current.isValid() == true) && (c_Current.parent().isValid() == false) && (pc_WidgetItem != NULL))
      {
         this->mpc_SelectedNode = pc_WidgetItem;
         this->mpc_SelectedSection = pc_WidgetItem->GetSectionList(c_Pos);
         this->mpc_SelectedApp = pc_WidgetItem->GetApplication(c_Pos);

         if (this->mpc_SelectedApp != NULL)
         {
            this->m_SelectFile();
         }
      }
   }
   C_OgeHorizontalListWidget::mouseDoubleClickEvent(opc_Event);
}

//----------------------------------------------------------------------------------------------------------------------
void C_SyvUpUpdatePackageListWidget::m_MoveItem(const sintn osn_SourceIndex, const sintn osn_TargetIndex)
{
   const C_PuiSvData * const pc_View = C_PuiSvHandler::h_GetInstance()->GetView(this->mu32_ViewIndex);

   Q_UNUSED(osn_SourceIndex)
   Q_UNUSED(osn_TargetIndex)

   // Update all position numbers in the node update information
   // The numbers in the widgets must be updated before
   if (pc_View != NULL)
   {
      std::vector<C_PuiSvNodeUpdate> c_NodeUpdate = pc_View->GetAllNodeUpdateInformation();
      sintn sn_Counter;

      for (sn_Counter = 0; sn_Counter < this->count(); ++sn_Counter)
      {
         // activate the first item and deactivate all other items
         QListWidgetItem * const pc_Item = this->item(sn_Counter);

         //lint -e{929}  false positive in PC-Lint: allowed by MISRA 5-2-2
         C_SyvUpUpdatePackageNodeWidget * const pc_WidgetItem =
            dynamic_cast<C_SyvUpUpdatePackageNodeWidget *>(this->itemWidget(pc_Item));
         if (pc_WidgetItem != NULL)
         {
            const uint32 u32_NodeIndex = pc_WidgetItem->GetNodeIndex();

            tgl_assert(u32_NodeIndex < c_NodeUpdate.size());
            if (u32_NodeIndex < c_NodeUpdate.size())
            {
               // Get the position and update the node update position
               c_NodeUpdate[u32_NodeIndex].u32_NodeUpdatePosition = pc_WidgetItem->GetPositionNumber();
            }
         }
      }

      // Update the entire node update information for all nodes
      // The return value is not relevant, the view was checked with GetView already
      C_PuiSvHandler::h_GetInstance()->SetNodeUpdateInformation(this->mu32_ViewIndex, c_NodeUpdate);
   }
}

//----------------------------------------------------------------------------------------------------------------------
void C_SyvUpUpdatePackageListWidget::m_UpdateNumbers(void) const
{
   sintn sn_Counter;

   // Update all visible numbers of the concrete widgets
   for (sn_Counter = 0; sn_Counter < this->count(); ++sn_Counter)
   {
      QListWidgetItem * const pc_Item = this->item(sn_Counter);
      //lint -e{929}  false positive in PC-Lint: allowed by MISRA 5-2-2
      C_SyvUpUpdatePackageNodeWidget * const pc_WidgetItem =
         dynamic_cast<C_SyvUpUpdatePackageNodeWidget *>(this->itemWidget(pc_Item));
      if (pc_WidgetItem != NULL)
      {
         pc_WidgetItem->UpdatePositionNumber(static_cast<uint32>(sn_Counter));
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
void C_SyvUpUpdatePackageListWidget::m_DelegateStartPaint(void)
{
   //lint -e{929}  false positive in PC-Lint: allowed by MISRA 5-2-2
   C_SyvUpUpdatePackageNodeWidget * pc_ItemWidget =
      dynamic_cast<C_SyvUpUpdatePackageNodeWidget *>(this->itemWidget(this->item(this->currentIndex().row())));

   this->mc_Delegate.StartPaint(this->currentIndex().row(), pc_ItemWidget);
}

//----------------------------------------------------------------------------------------------------------------------
void C_SyvUpUpdatePackageListWidget::m_DelegateStopPaint(void)
{
   this->mc_Delegate.StopPaint();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Adds a concrete node widget to the list

   \param[in]     ou32_NodeIndex  Node index
   \param[in]     orc_NodeName    Name of the node
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvUpUpdatePackageListWidget::m_AddNodeWidget(const uint32 ou32_NodeIndex, const QString & orc_NodeName)
{
   QListWidgetItem * pc_Item = new QListWidgetItem(NULL, static_cast<sintn>(QListWidgetItem::ItemType::UserType));
   C_SyvUpUpdatePackageNodeWidget * pc_ItemWidget = new C_SyvUpUpdatePackageNodeWidget(this->mu32_ViewIndex,
                                                                                       this->count(),
                                                                                       ou32_NodeIndex,
                                                                                       orc_NodeName, this);

   pc_Item->setSizeHint(pc_ItemWidget->size());

   this->addItem(pc_Item);
   this->setItemWidget(pc_Item, pc_ItemWidget);
   //lint -e{429}  no memory leak because of the parent of pc_Item and pc_ItemWidget and the Qt memory management
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Show custom context menu

   \param[in] orc_Pos Local context menu position
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvUpUpdatePackageListWidget::m_OnCustomContextMenuRequested(const QPoint & orc_Pos)
{
   bool q_ShowContextMenu = false;
   QListWidgetItem * const pc_Item = this->itemAt(orc_Pos);

   this->mpc_SelectedNode = NULL;
   this->mpc_SelectedSection = NULL;
   this->mpc_SelectedApp = NULL;

   if ((pc_Item != NULL) &&
       (this->mq_Connected == false))
   {
      const QModelIndex c_Current = this->currentIndex();
      //lint -e{929}  false positive in PC-Lint: allowed by MISRA 5-2-2
      C_SyvUpUpdatePackageNodeWidget * const pc_WidgetItem =
         dynamic_cast<C_SyvUpUpdatePackageNodeWidget *>(this->itemWidget(pc_Item));

      // add action shall be shown only if no item concrete was clicked
      if ((c_Current.isValid() == true) && (c_Current.parent().isValid() == false) && (pc_WidgetItem != NULL))
      {
         const bool q_IsStwFlashloader = pc_WidgetItem->IsStwFlashloader();
         this->mpc_SelectedNode = pc_WidgetItem;
         this->mpc_SelectedSection = pc_WidgetItem->GetSectionList(orc_Pos);
         this->mpc_SelectedApp = pc_WidgetItem->GetAndSelectApplication(orc_Pos);

         // Section depending actions
         this->mpc_RemoveAllSectionFilesAction->setVisible(this->mpc_SelectedSection != NULL);

         // Concrete application depending actions
         if ((this->mpc_SelectedApp != NULL) && (this->mpc_SelectedSection != NULL))
         {
            const QFileInfo c_FileInfo(this->mpc_SelectedApp->GetAppAbsoluteFilePath());
            this->mpc_RevertFileAction->setVisible(
               this->mpc_SelectedSection->Type() == mu32_UPDATE_PACKAGE_NODE_SECTION_TYPE_DATABLOCK);
            this->mpc_AddFileAction->setVisible(!(this->mpc_SelectedSection->Type() ==
                                                  mu32_UPDATE_PACKAGE_NODE_SECTION_TYPE_DATABLOCK));
            this->mpc_SelectFileAction->setVisible(true);
            this->mpc_RemoveFileAction->setVisible(true);
            this->mpc_RemoveFileAction->setEnabled(
               (this->mpc_SelectedSection->Type() != mu32_UPDATE_PACKAGE_NODE_SECTION_TYPE_DATABLOCK) ||
               ((this->mpc_SelectedSection->Type() == mu32_UPDATE_PACKAGE_NODE_SECTION_TYPE_DATABLOCK) &&
                (this->mpc_SelectedApp->GetAppFilePath() != C_GtGetText::h_GetText("<Add File>"))));
            this->mpc_ShowFileInfoAction->setEnabled(this->mpc_SelectedApp->IsViewFileInfoPossible());
            this->mpc_ShowFileInfoAction->setVisible(true);
            this->mpc_RemoveAllNodeFilesAction->setVisible(false);
            this->mpc_ShowInExplorerAction->setEnabled(c_FileInfo.exists());
            this->mpc_ShowInExplorerAction->setVisible(true);
         }
         else
         {
            this->mpc_SelectFileAction->setVisible(false);
            this->mpc_RevertFileAction->setVisible(false);
            this->mpc_RemoveFileAction->setVisible(false);
            this->mpc_ShowFileInfoAction->setVisible(false);
            this->mpc_RemoveAllNodeFilesAction->setVisible(true);

            // New file or new parameter set image file is possible with all openSYDE devices
            this->mpc_AddFileAction->setVisible(!q_IsStwFlashloader);
            this->mpc_ShowInExplorerAction->setVisible(false);
         }

         q_ShowContextMenu = true;
      }
   }

   if (q_ShowContextMenu == true)
   {
      this->mpc_ContextMenu->popup(this->mapToGlobal(orc_Pos));
   }
}

//----------------------------------------------------------------------------------------------------------------------
void C_SyvUpUpdatePackageListWidget::m_OnCustomContextMenuHide(void)
{
   if (this->mpc_SelectedApp != NULL)
   {
      this->mpc_SelectedApp->SetSelected(false);
   }
}

//----------------------------------------------------------------------------------------------------------------------
void C_SyvUpUpdatePackageListWidget::m_SetupContextMenu(void)
{
   mpc_ContextMenu = new stw_opensyde_gui_elements::C_OgeContextMenu(this);

   this->mpc_SelectFileAction = this->mpc_ContextMenu->addAction(
      C_GtGetText::h_GetText("Select File"), this,  &C_SyvUpUpdatePackageListWidget::m_SelectFile);

   this->mpc_AddFileAction = this->mpc_ContextMenu->addAction(
      C_GtGetText::h_GetText("Add new Files"), this, &C_SyvUpUpdatePackageListWidget::m_AddFileAction);

   this->mpc_RevertFileAction = this->mpc_ContextMenu->addAction(
      C_GtGetText::h_GetText("Revert to Default"), this, &C_SyvUpUpdatePackageListWidget::m_RevertFile);

   this->mpc_ContextMenu->addSeparator();

   this->mpc_ShowFileInfoAction = this->mpc_ContextMenu->addAction(
      C_GtGetText::h_GetText("View File Information"), this, &C_SyvUpUpdatePackageListWidget::m_ViewFileInfo);

   this->mpc_HideShowOptionalSectionsAction = this->mpc_ContextMenu->addAction(
      C_GtGetText::h_GetText("Hide Empty Optional Sections"), this,
      &C_SyvUpUpdatePackageListWidget::m_HideShowOptionalSections);

   this->mpc_ShowInExplorerAction = this->mpc_ContextMenu->addAction(
      C_GtGetText::h_GetText("Show in Explorer"), this,
      &C_SyvUpUpdatePackageListWidget::m_ShowInExplorer);

   this->mpc_ContextMenu->addSeparator();

   this->mpc_RemoveFileAction = this->mpc_ContextMenu->addAction(
      C_GtGetText::h_GetText("Remove File"), this, &C_SyvUpUpdatePackageListWidget::m_RemoveFile);

   this->mpc_RemoveAllSectionFilesAction = this->mpc_ContextMenu->addAction(
      C_GtGetText::h_GetText("Remove all Files of this Section"),
      this, &C_SyvUpUpdatePackageListWidget::m_RemoveAllSectionFiles);

   this->mpc_RemoveAllNodeFilesAction = this->mpc_ContextMenu->addAction(
      C_GtGetText::h_GetText("Remove all Files"), this, &C_SyvUpUpdatePackageListWidget::m_RemoveAllNodeFiles);

   this->setContextMenuPolicy(Qt::CustomContextMenu);

   connect(this, &C_SyvUpUpdatePackageListWidget::customContextMenuRequested, this,
           &C_SyvUpUpdatePackageListWidget::m_OnCustomContextMenuRequested);
   connect(this->mpc_ContextMenu, &stw_opensyde_gui_elements::C_OgeContextMenu::aboutToHide, this,
           &C_SyvUpUpdatePackageListWidget::m_OnCustomContextMenuHide);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Adding a new file based file to the node
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvUpUpdatePackageListWidget::m_AddFileAction(void)
{
   if ((this->mpc_SelectedNode != NULL) &&
       (this->mpc_SelectedNode->IsStwFlashloader() == false))
   {
      if (this->mpc_SelectedNode->IsFileBased() == false)
      {
         // No file based and a openSYDE device, only parameter set image files are possible to add
         this->m_AddNewFile(C_GtGetText::h_GetText("openSYDE parameter set image"),
                            QString(C_GtGetText::h_GetText("openSYDE parameter set image")) + " (*.syde_psi)");
      }
      else
      {
         // File based device. All is possible. A parameter set image file will be added by file extension
         this->m_AddNewFile(C_GtGetText::h_GetText("Add File"),
                            QString(C_GtGetText::h_GetText("All files")) + " (*.*)");
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Adding a new file to the selected node

   Depending on the file extension (.syde_psi), it will be added as parameter set image file or
   as file for file based nodes

   \param[in]     orc_DialogCaption   Caption for file dialog
   \param[in]     orc_DialogFilter    Filter configuration for file dialog
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvUpUpdatePackageListWidget::m_AddNewFile(const QString & orc_DialogCaption, const QString & orc_DialogFilter)
{
   Q_EMIT (this->SigStartCheck(false));

   if (this->mpc_SelectedNode != NULL)
   {
      const QString c_Folder = this->m_GetDialogPath();
      QStringList c_Files = QFileDialog::getOpenFileNames(this, orc_DialogCaption, c_Folder, orc_DialogFilter);

      if (c_Files.isEmpty() == false)
      {
         // check if relative path is possible and appreciated
         c_Files =
            C_ImpUtil::h_AskUserToSaveRelativePath(this, c_Files, C_PuiProject::h_GetInstance()->GetFolderPath());

         if (c_Files.isEmpty() == false)
         {
            for (sint32 s32_Pos = 0; s32_Pos < c_Files.size(); ++s32_Pos)
            {
               QFileInfo c_File(c_Files[s32_Pos]);
               const bool q_ParamsetFile = c_File.completeSuffix().toLower() == "syde_psi";

               // add file
               this->mpc_SelectedNode->AddNewFile(c_Files[s32_Pos], q_ParamsetFile);
            }

            // remember last path
            this->mc_LastPath = TGL_ExtractFilePath(c_Files.last().toStdString().c_str()).c_str();
         }
      }
   }

   Q_EMIT (this->SigStartCheck(true));
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Selecting a new file for an existing app
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvUpUpdatePackageListWidget::m_SelectFile(void)
{
   Q_EMIT (this->SigStartCheck(false));

   if (this->mpc_SelectedNode != NULL)
   {
      QString c_Filter = "";
      const QString c_Folder = this->m_GetDialogPath();
      QString c_File = "";
      bool q_HexFile = false;

      if ((this->mpc_SelectedApp != NULL) && (this->mpc_SelectedSection != NULL))
      {
         // Get the correct type
         switch (this->mpc_SelectedSection->Type())
         {
         case mu32_UPDATE_PACKAGE_NODE_SECTION_TYPE_DATABLOCK:
            if (this->mpc_SelectedNode->IsFileBased() == false)
            {
               c_Filter = QString(C_GtGetText::h_GetText("HEX files")) + " (*.hex);;" + c_Filter;
               q_HexFile = true;
            }
            else
            {
               c_Filter = QString(C_GtGetText::h_GetText("All files")) + " (*.*)";
            }
            break;
         case mu32_UPDATE_PACKAGE_NODE_SECTION_TYPE_PARAMSET:
            c_Filter = QString(C_GtGetText::h_GetText("openSYDE parameter set image")) + " (*.syde_psi)";
            break;
         case mu32_UPDATE_PACKAGE_NODE_SECTION_TYPE_FILE:
            c_Filter = QString(C_GtGetText::h_GetText("All files")) + " (*.*)";
            break;
         default:
            tgl_assert(false);
            break;
         }

         if (c_Filter != "")
         {
            // do not use QFileDialog::getOpenFileName because it does not support default suffix
            QFileDialog c_Dialog(this, C_GtGetText::h_GetText("Select File"), c_Folder, c_Filter);
            if (q_HexFile == true)
            {
               c_Dialog.setDefaultSuffix("hex");
            }

            if (c_Dialog.exec() == static_cast<sintn>(QDialog::Accepted))
            {
               c_File = c_Dialog.selectedFiles().at(0); // multi-selection is not possible

               // remember path
               this->mc_LastPath = TGL_ExtractFilePath(c_File.toStdString().c_str()).c_str();

               // check if relative path is possible and appreciated
               c_File = C_ImpUtil::h_AskUserToSaveRelativePath(this, c_File,
                                                               C_PuiProject::h_GetInstance()->GetFolderPath());

               if (c_File != "")
               {
                  // adapt file
                  this->m_AdaptFile(c_File);
               }
            }
         }
      }
   }
   Q_EMIT (this->SigStartCheck(true));
}

//----------------------------------------------------------------------------------------------------------------------
void C_SyvUpUpdatePackageListWidget::m_RevertFile(void)
{
   if ((this->mpc_SelectedNode != NULL) &&
       (this->mpc_SelectedApp != NULL))
   {
      this->mpc_SelectedNode->RevertFile(this->mpc_SelectedApp);
   }
}

//----------------------------------------------------------------------------------------------------------------------
void C_SyvUpUpdatePackageListWidget::m_RemoveFile(void)
{
   if ((this->mpc_SelectedNode != NULL) && (this->mpc_SelectedApp != NULL))
   {
      C_OgeWiCustomMessage c_MessageBox(this, C_OgeWiCustomMessage::eQUESTION);
      c_MessageBox.SetHeading(C_GtGetText::h_GetText("Remove file"));
      c_MessageBox.SetDescription(C_GtGetText::h_GetText("Do you really want to remove ") +
                                  this->mpc_SelectedApp->GetAppFilePath() +
                                  C_GtGetText::h_GetText(" from the Update Package?"));
      c_MessageBox.SetOKButtonText("Remove");
      c_MessageBox.SetNOButtonText("Keep");
      if (c_MessageBox.Execute() == C_OgeWiCustomMessage::eYES)
      {
         this->mpc_SelectedNode->RemoveFile(this->mpc_SelectedApp);
         this->mpc_SelectedApp = NULL;
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Show file information of selected file
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvUpUpdatePackageListWidget::m_ViewFileInfo(void)
{
   if (((this->mpc_SelectedApp != NULL) && (this->mpc_SelectedNode != NULL)) &&
       (this->mpc_SelectedApp->GetAppFilePath().compare("") != 0))
   {
      mpc_SelectedApp->ViewFileInfo();
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Remove all files of section
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvUpUpdatePackageListWidget::m_RemoveAllSectionFiles(void)
{
   if (this->mpc_SelectedSection != NULL)
   {
      C_OgeWiCustomMessage c_MessageBox(this, C_OgeWiCustomMessage::eQUESTION);
      QString c_Section = "";
      switch (this->mpc_SelectedSection->Type())
      {
      case mu32_UPDATE_PACKAGE_NODE_SECTION_TYPE_DATABLOCK:
         c_Section = C_GtGetText::h_GetText("Data Blocks");
         break;
      case mu32_UPDATE_PACKAGE_NODE_SECTION_TYPE_PARAMSET:
         c_Section = C_GtGetText::h_GetText("Parameter Set Images");
         break;
      case mu32_UPDATE_PACKAGE_NODE_SECTION_TYPE_FILE:
         c_Section = C_GtGetText::h_GetText("Files");
         break;
      default:
         tgl_assert(false);
         break;
      }
      c_MessageBox.SetHeading(C_GtGetText::h_GetText("Remove files"));
      c_MessageBox.SetDescription(C_GtGetText::h_GetText("Do you really want to remove all files of section \"") +
                                  c_Section + C_GtGetText::h_GetText("\" from the Update Package?"));
      c_MessageBox.SetOKButtonText("Remove");
      c_MessageBox.SetNOButtonText("Keep");
      if (c_MessageBox.Execute() == C_OgeWiCustomMessage::eYES)
      {
         this->mpc_SelectedSection->RemoveAllFiles();

         // update visibility of optional sections
         if (this->mpc_SelectedNode != NULL)
         {
            this->mpc_SelectedNode->SetEmptyOptionalSectionsVisible(this->mq_EmptyOptionalSectionsVisible);
         }
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Remove all files of node
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvUpUpdatePackageListWidget::m_RemoveAllNodeFiles(void)
{
   if (this->mpc_SelectedNode != NULL)
   {
      C_OgeWiCustomMessage c_MessageBox(this, C_OgeWiCustomMessage::eQUESTION);
      c_MessageBox.SetHeading(C_GtGetText::h_GetText("Remove files"));
      c_MessageBox.SetDescription(
         C_GtGetText::h_GetText("Do you really want to remove all files of node \"") +
         this->mpc_SelectedNode->GetNodeName() + C_GtGetText::h_GetText("\" from the Update Package?"));
      c_MessageBox.SetOKButtonText("Remove");
      c_MessageBox.SetNOButtonText("Keep");
      if (c_MessageBox.Execute() == C_OgeWiCustomMessage::eYES)
      {
         this->mpc_SelectedNode->RemoveAllFiles();
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Toggle visibility of empty optional sections for all nodes
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvUpUpdatePackageListWidget::m_HideShowOptionalSections(void)
{
   sintn sn_Counter;
   QString c_Help;

   // toggle state
   this->mq_EmptyOptionalSectionsVisible = !this->mq_EmptyOptionalSectionsVisible;

   // update menu text
   if (this->mq_EmptyOptionalSectionsVisible == true)
   {
      c_Help = C_GtGetText::h_GetText("Hide ");
   }
   else
   {
      c_Help = C_GtGetText::h_GetText("Show ");
   }
   this->mpc_HideShowOptionalSectionsAction->setText(c_Help + C_GtGetText::h_GetText("Empty Optional Sections"));

   // adapt all node widgets
   for (sn_Counter = 0; sn_Counter < this->count(); ++sn_Counter)
   {
      // activate the first item and deactivate all other items
      QListWidgetItem * const pc_Item = this->item(sn_Counter);

      //lint -e{929}  false positive in PC-Lint: allowed by MISRA 5-2-2
      C_SyvUpUpdatePackageNodeWidget * const pc_WidgetItem =
         dynamic_cast<C_SyvUpUpdatePackageNodeWidget *>(this->itemWidget(pc_Item));
      if (pc_WidgetItem != NULL)
      {
         // set visibility of empty optional sections
         pc_WidgetItem->SetEmptyOptionalSectionsVisible(this->mq_EmptyOptionalSectionsVisible);
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Show file in explorer
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvUpUpdatePackageListWidget::m_ShowInExplorer(void) const
{
   // analogue to Qt Creator functionality "Show in Explorer" but Windows only
   if (this->mpc_SelectedApp != NULL)
   {
      const QFileInfo c_FileInfo(this->mpc_SelectedApp->GetAppAbsoluteFilePath());
      if (c_FileInfo.exists() == true)
      {
         QStringList c_Param;

         if (c_FileInfo.isDir() == false)
         {
            c_Param += QLatin1String("/select,");
         }
         c_Param += QDir::toNativeSeparators(c_FileInfo.canonicalFilePath());
         QProcess::startDetached("explorer.exe", c_Param);
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
void C_SyvUpUpdatePackageListWidget::m_AdaptFile(const QString & orc_Path)
{
   if ((this->mpc_SelectedApp != NULL) &&
       (this->mpc_SelectedNode != NULL))
   {
      this->mpc_SelectedNode->AdaptFile(orc_Path, this->mpc_SelectedApp);
   }
}

//----------------------------------------------------------------------------------------------------------------------
QString C_SyvUpUpdatePackageListWidget::m_GetDialogPath(void)
{
   QString c_Folder;
   QFileInfo c_File;

   // first favorite: path of selected app
   if (this->mpc_SelectedApp != NULL)
   {
      this->mc_LastPath =
         TGL_ExtractFilePath(this->mpc_SelectedApp->GetAppAbsoluteFilePath().toStdString().c_str()).c_str();
   }

   c_File.setFile(this->mc_LastPath);
   // second favorite: last path
   if ((this->mc_LastPath != "") &&
       (c_File.exists() == true) &&
       (c_File.isDir() == true))
   {
      c_Folder = this->mc_LastPath;
   }
   // third favorite: project path
   else
   {
      c_Folder = C_PuiProject::h_GetInstance()->GetFolderPath();

      // default to exe if path is empty (i.e. project is not saved yet)
      if (c_Folder.isEmpty() == true)
      {
         c_Folder = C_Uti::h_GetExePath();
      }
   }

   return c_Folder;
}
