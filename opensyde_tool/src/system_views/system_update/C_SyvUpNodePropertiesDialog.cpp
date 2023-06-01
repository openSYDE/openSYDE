//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Dialog for system update node properties view (implementation)

   Dialog for system update node properties view.

   \copyright   Copyright 2018 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "TglUtils.hpp"

#include "TglUtils.hpp"
#include "constants.hpp"
#include "C_GtGetText.hpp"
#include "C_PuiSdHandler.hpp"
#include "C_PuiSdUtil.hpp"
#include "C_OgeWiCustomMessage.hpp"

#include "C_SyvUpNodePropertiesDialog.hpp"
#include "ui_C_SyvUpNodePropertiesDialog.h"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw::tgl;
using namespace stw::diag_lib;
using namespace stw::opensyde_gui;
using namespace stw::opensyde_core;
using namespace stw::opensyde_gui_logic;
using namespace stw::opensyde_gui_elements;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */
const QString C_SyvUpNodePropertiesDialog::mhc_HTML_CELL_TAG_START_MAIN = "<td style=\"padding: 0px 40px 10px 0px;\">";
const QString C_SyvUpNodePropertiesDialog::mhc_HTML_CELL_TAG_START_CONTENT =
   "<td valign=\"middle\" style=\"padding: 0px 30px 0px 0px;\">";
const QString C_SyvUpNodePropertiesDialog::mhc_HTML_CELL_TAG_START_STATUS = "<td style=\"padding: 0px 5px 0px 0px;\">";

/* -- Types --------------------------------------------------------------------------------------------------------- */

/* -- Global Variables ---------------------------------------------------------------------------------------------- */

/* -- Module Global Variables --------------------------------------------------------------------------------------- */

/* -- Module Global Function Prototypes ----------------------------------------------------------------------------- */

/* -- Implementation ------------------------------------------------------------------------------------------------ */

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default constructor

   Set up GUI with all elements.

   \param[in,out]  orc_Parent       Reference to parent
   \param[in]      ou32_NodeIndex   Node index
   \param[in]      orc_NodeData     Node data
*/
//----------------------------------------------------------------------------------------------------------------------
C_SyvUpNodePropertiesDialog::C_SyvUpNodePropertiesDialog(stw::opensyde_gui_elements::C_OgePopUpDialog & orc_Parent,
                                                         const uint32_t ou32_NodeIndex,
                                                         const C_GiSvNodeData & orc_NodeData) :
   QWidget(&orc_Parent),
   mpc_Ui(new Ui::C_SyvUpNodePropertiesDialog),
   mrc_ParentDialog(orc_Parent),
   mu32_NodeIndex(ou32_NodeIndex),
   mc_NodeData(orc_NodeData)
{
   mpc_Ui->setupUi(this);

   InitStaticNames();

   //Start with defined tab
   this->mpc_Ui->pc_TabWidget->setCurrentIndex(0);

   //Deactivate text for icon
   this->mpc_Ui->pc_LabelIcon->setText("");

   //Deactivate text edit interaction
   this->mpc_Ui->pc_TextEditDataBlocks->setReadOnly(true);
   this->mpc_Ui->pc_ProgressLog->setReadOnly(true);
   this->mpc_Ui->pc_TextEditFlash->setReadOnly(true);

   // Label style
   this->mpc_Ui->pc_LabelDescription->SetForegroundColor(6);
   this->mpc_Ui->pc_LabelDescription->SetFontPixel(16);

   // register the widget for showing
   this->mrc_ParentDialog.SetWidget(this);

   // set main title
   this->mrc_ParentDialog.SetTitle(C_PuiSdUtil::h_GetNodeBaseNameOrName(this->mu32_NodeIndex));

   this->mrc_ParentDialog.SetSubTitle(C_GtGetText::h_GetText("Device Status Information"));

   // connects
   connect(this->mpc_Ui->pc_PushButtonOk, &QPushButton::clicked, this, &C_SyvUpNodePropertiesDialog::m_OkClicked);
   connect(this->mpc_Ui->pc_PushButtonDiscard, &QPushButton::clicked, this, &C_SyvUpNodePropertiesDialog::m_OnDiscard);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default destructor

   Clean up.
*/
//----------------------------------------------------------------------------------------------------------------------
C_SyvUpNodePropertiesDialog::~C_SyvUpNodePropertiesDialog(void)
{
   delete mpc_Ui;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Initialize all displayed static names
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvUpNodePropertiesDialog::InitStaticNames(void) const
{
   this->mpc_Ui->pc_TabWidget->setTabText(0, C_GtGetText::h_GetText("Update File(s)"));
   this->mpc_Ui->pc_TabWidget->setTabText(1, C_GtGetText::h_GetText("Progress Log"));
   this->mpc_Ui->pc_TabWidget->setTabText(2, C_GtGetText::h_GetText("Flashloader Information"));
   this->mpc_Ui->pc_PushButtonDiscard->setText(C_GtGetText::h_GetText("Force Update"));
   this->mpc_Ui->pc_PushButtonDiscard->SetToolTipInformation(
      C_GtGetText::h_GetText("Force Update"),
      C_GtGetText::h_GetText("Option to force flashing of already up to date nodes."), C_NagToolTip::eDEFAULT);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Update status

   \param[in]  orc_NodeData   Node data
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvUpNodePropertiesDialog::SetStatus(const C_GiSvNodeData & orc_NodeData)
{
   this->mc_NodeData = orc_NodeData;

   //Handle initial no info state
   if (this->mc_NodeData.IsThereAnyHexFileInformation())
   {
      this->mpc_Ui->pc_PushButtonDiscard->setEnabled(true);
   }
   else
   {
      this->mpc_Ui->pc_PushButtonDiscard->setEnabled(false);
   }
   this->m_ReInitStatus();
   this->m_InitFlashloaderTable();
   this->m_UpdateProgressLog();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Copy initial status

   \param[in,out]  orc_NodeData  Node data
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvUpNodePropertiesDialog::CopyInitialStatus(C_GiSvNodeData & orc_NodeData) const
{
   this->mc_NodeData.CopyInitialStatus(orc_NodeData);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Copy update status

   \param[in,out]  orc_NodeData  Node data
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvUpNodePropertiesDialog::CopyUpdateStatus(C_GiSvNodeData & orc_NodeData) const
{
   this->mc_NodeData.CopyUpdateStatus(orc_NodeData);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Copy STW device info

   \param[in,out]  orc_NodeData  Node data
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvUpNodePropertiesDialog::CopyStwDeviceInfo(C_GiSvNodeData & orc_NodeData) const
{
   this->mc_NodeData.CopyStwDeviceInfo(orc_NodeData);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Copy OSY device info

   \param[in,out]  orc_NodeData  Node data
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvUpNodePropertiesDialog::CopyOpenSydeDeviceInfo(C_GiSvNodeData & orc_NodeData) const
{
   this->mc_NodeData.CopyOsyDeviceInfo(orc_NodeData);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Copy discarded status

   \param[in,out]  orc_NodeData  Node data
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvUpNodePropertiesDialog::CopyDiscardedStatus(C_GiSvNodeData & orc_NodeData) const
{
   this->mc_NodeData.CopyDiscardedStatus(orc_NodeData);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Overwritten key press event slot

   Here: Handle specific enter key cases

   \param[in,out]  opc_KeyEvent  Event identification and information
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvUpNodePropertiesDialog::keyPressEvent(QKeyEvent * const opc_KeyEvent)
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
         this->mrc_ParentDialog.accept();
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
/*! \brief   Slot of Ok button click
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvUpNodePropertiesDialog::m_OkClicked(void)
{
   this->mrc_ParentDialog.accept();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Init current status
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvUpNodePropertiesDialog::m_InitStatus(void) const
{
   QIcon c_Icon;

   if (this->mc_NodeData.AreAllNodesValid())
   {
      switch (this->mc_NodeData.GetOverallInitialState())
      {
      case C_SyvUtil::eI_APPLICATION_MATCH:
         c_Icon = QIcon("://images/system_views/IconUpdateSuccess.svg");
         this->mpc_Ui->pc_LabelDescription->setText(C_GtGetText::h_GetText("Node is up to date."));
         break;
      case C_SyvUtil::eI_TO_BE_UPDATED:
         c_Icon = QIcon("://images/system_views/IconUpdateWaiting.svg");
         if (this->mc_NodeData.GetUpdateInProgressStatus() == true)
         {
            this->mpc_Ui->pc_LabelDescription->setText(C_GtGetText::h_GetText("Update is running ..."));
         }
         else
         {
            this->mpc_Ui->pc_LabelDescription->setText(C_GtGetText::h_GetText("Update required."));
         }
         break;
      case C_SyvUtil::eI_ERROR:
         c_Icon = QIcon("://images/Error_iconV2.svg");
         this->mpc_Ui->pc_LabelDescription->setText(C_GtGetText::h_GetText(
                                                       "Error occurred. Check progress log for details!"));
         break;
      case C_SyvUtil::eI_UPDATE_DISABLED:
         c_Icon = QIcon("://images/system_views/IconUpdateStatusDisabled.svg");
         this->mpc_Ui->pc_LabelDescription->setText(C_GtGetText::h_GetText("Node update disabled."));
         break;
      case C_SyvUtil::eI_UNKNOWN:
         c_Icon = QIcon("");
         this->mpc_Ui->pc_LabelDescription->setText("");
         break;
      default:
         tgl_assert(false);
         break;
      }
   }
   else
   {
      c_Icon = QIcon("");
      if (this->mc_NodeData.GetConnectInProgressStatus() == true)
      {
         this->mpc_Ui->pc_LabelDescription->setText(C_GtGetText::h_GetText("Requesting info ..."));
      }
      else
      {
         this->mpc_Ui->pc_LabelDescription->setText(C_GtGetText::h_GetText("Enter update mode to get the node status"));
      }
   }
   //Apply default icon size
   this->mpc_Ui->pc_LabelIcon->setPixmap(c_Icon.pixmap(64, 64));
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Initialize data block table
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvUpNodePropertiesDialog::m_InitDataBlockTable(void) const
{
   QString c_Content = "<html><body>";
   const std::vector<uint32_t> c_NodeIndices = C_PuiSdHandler::h_GetInstance()->GetAllNodeGroupIndicesUsingNodeIndex(
      this->mu32_NodeIndex);

   tgl_assert(c_NodeIndices.size() == this->mc_NodeData.GetSubNodeCount());
   if (c_NodeIndices.size() == this->mc_NodeData.GetSubNodeCount())
   {
      for (uint32_t u32_ItDevice = 0UL; u32_ItDevice < c_NodeIndices.size(); ++u32_ItDevice)
      {
         const C_OscNode * const pc_Node =
            C_PuiSdHandler::h_GetInstance()->GetOscNodeConst(c_NodeIndices[u32_ItDevice]);
         const C_GiSvSubNodeData * const pc_SubDevice =
            this->mc_NodeData.GetSubNodeByNodeIndex(c_NodeIndices[u32_ItDevice]);
         tgl_assert((pc_Node != NULL) && (pc_SubDevice != NULL));
         if ((pc_Node != NULL) && (pc_SubDevice != NULL))
         {
            C_SyvUpNodePropertiesDialog::mh_InitDataBlockTableForNode(*pc_SubDevice,
                                                                      c_Content,
                                                                      *pc_Node, c_NodeIndices.size() > 1UL);
         }
      }
   }

   c_Content += "</body></html>";
   this->mpc_Ui->pc_TextEditDataBlocks->setHtml(c_Content);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Init data block table for node

   \param[in]      orc_NodeInfo        Node info
   \param[in,out]  orc_Content         Content
   \param[in]      orc_Node            Node
   \param[in]      oq_IsMultiDevice    Is multi device
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvUpNodePropertiesDialog::mh_InitDataBlockTableForNode(const C_GiSvSubNodeData & orc_NodeInfo,
                                                               QString & orc_Content, const C_OscNode & orc_Node,
                                                               const bool oq_IsMultiDevice)
{
   QString c_NewContent;

   if (orc_NodeInfo.GetValidStatus())
   {
      //Each application
      if (orc_NodeInfo.GetDataBlockFoundStatus())
      {
         tgl_assert(orc_Node.u32_SubDeviceIndex < orc_Node.pc_DeviceDefinition->c_SubDevices.size());
         if (orc_Node.pc_DeviceDefinition->c_SubDevices[orc_Node.u32_SubDeviceIndex].q_FlashloaderOpenSydeIsFileBased ==
             true)
         {
            c_NewContent += "<p>";
            c_NewContent += C_GtGetText::h_GetText("No status request supported. Update required.");
            c_NewContent += "</p>";
         }
         else
         {
            if (orc_Node.c_Applications.size() > 0)
            {
               if (orc_Node.c_Applications.size() == orc_NodeInfo.GetHexFileInfosCount())
               {
                  for (uint32_t u32_ItFile = 0; u32_ItFile < orc_NodeInfo.GetHexFileInfosCount();
                       ++u32_ItFile)
                  {
                     QString c_FileProjectName;
                     QString c_FileVersion;
                     QString c_FileBuild;
                     QString c_ApplicationName;
                     QString c_Icon;
                     QString c_State;
                     QString c_DeviceProjectName;
                     QString c_DeviceVersion;
                     QString c_DeviceBuild;
                     QString c_DeviceValid;
                     QString c_DetailsPart;
                     C_SyvUpNodePropertiesDialog::mh_GetApplicationDataForNode(
                        orc_NodeInfo, orc_Node, u32_ItFile, c_ApplicationName, c_Icon, c_State, c_DeviceValid,
                        c_FileProjectName, c_DeviceProjectName, c_FileVersion, c_DeviceVersion,
                        c_FileBuild, c_DeviceBuild);
                     C_SyvUpNodePropertiesDialog::mh_ExtractDetailsPartFromDataForNode(orc_NodeInfo.GetHexAppInfoAmbiguous(
                                                                                          u32_ItFile),
                                                                                       c_FileProjectName,
                                                                                       c_DeviceProjectName,
                                                                                       c_FileVersion, c_DeviceVersion,
                                                                                       c_FileBuild, c_DeviceBuild,
                                                                                       c_DetailsPart);
                     C_SyvUpNodePropertiesDialog::mh_AppendApplicationForNode(
                        u32_ItFile, c_ApplicationName, c_Icon, c_State, c_DeviceValid,
                        c_DetailsPart, c_NewContent);
                  }
               }
               else
               {
                  //No content if nothing found
               }
            }
            else
            {
               c_NewContent += "<p>";
               c_NewContent += C_GtGetText::h_GetText("No Data Block available. No update required.");
               c_NewContent += "</p>";
            }
         }
      }
   }
   C_SyvUpNodePropertiesDialog::mh_InitDataBlockTableOtherSectionForNode(orc_NodeInfo, c_NewContent);
   C_SyvUpNodePropertiesDialog::mh_HandleSectionAppend(c_NewContent, orc_Content, orc_Node, oq_IsMultiDevice);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get application data for node

   \param[in]      orc_NodeInfo              Node info
   \param[in]      orc_Node                  Node
   \param[in]      ou32_ApplicationIndex     Application index
   \param[in,out]  orc_ApplicationName       Application name
   \param[in,out]  orc_ApplicationStateIcon  Application state icon
   \param[in,out]  orc_ApplicationState      Application state
   \param[in,out]  orc_DeviceValidStatus     Device valid status
   \param[in,out]  orc_FileProjectName       File project name
   \param[in,out]  orc_DeviceProjectName     Device project name
   \param[in,out]  orc_FileVersion           File version
   \param[in,out]  orc_DeviceFileVersion     Device file version
   \param[in,out]  orc_FileBuildDate         File build date
   \param[in,out]  orc_DeviceBuildDate       Device build date
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvUpNodePropertiesDialog::mh_GetApplicationDataForNode(const C_GiSvSubNodeData & orc_NodeInfo,
                                                               const C_OscNode & orc_Node,
                                                               const uint32_t ou32_ApplicationIndex,
                                                               QString & orc_ApplicationName,
                                                               QString & orc_ApplicationStateIcon,
                                                               QString & orc_ApplicationState,
                                                               QString & orc_DeviceValidStatus,
                                                               QString & orc_FileProjectName,
                                                               QString & orc_DeviceProjectName,
                                                               QString & orc_FileVersion,
                                                               QString & orc_DeviceFileVersion,
                                                               QString & orc_FileBuildDate,
                                                               QString & orc_DeviceBuildDate)
{
   const C_XFLECUInformation * const pc_FileInfo = orc_NodeInfo.GetHexFileInfo(
      ou32_ApplicationIndex);
   const C_OscNodeApplication & rc_Application = orc_Node.c_Applications[ou32_ApplicationIndex];
   bool q_Missing = true;
   bool q_Match = true;
   const C_SyvUpDeviceInfo c_DeviceInfo = orc_NodeInfo.GetDeviceInfo();

   tgl_assert(pc_FileInfo != NULL);
   if (pc_FileInfo != NULL)
   {
      orc_FileProjectName = pc_FileInfo->GetProjectName().c_str();
      orc_FileVersion = pc_FileInfo->GetProjectVersion().c_str();
      orc_FileBuildDate = (pc_FileInfo->GetDate() + " " + pc_FileInfo->GetTime()).c_str();
   }

   orc_ApplicationName = rc_Application.c_Name.c_str();
   orc_ApplicationStateIcon = "";
   orc_ApplicationState = "";
   orc_DeviceProjectName = C_GtGetText::h_GetText("<b>Not present</b>");
   orc_DeviceFileVersion = C_GtGetText::h_GetText("<b>Not present</b>");
   orc_DeviceBuildDate = C_GtGetText::h_GetText("<b>Not present</b>");
   orc_DeviceValidStatus = C_GtGetText::h_GetText("<b>Not present</b>");

   //Check, exceptions:
   //If update successful all applications have to be up to date
   //If no node information is available skip this as this probably means: discarded information
   //If error happened also skip this
   if (((((orc_NodeInfo.GetInitialStatus() == C_SyvUtil::eI_APPLICATION_MATCH) ||
          (orc_NodeInfo.GetUpdateSuccessStatus())) &&
         ((c_DeviceInfo.pc_OpenSydeDevice != NULL) ||
          (c_DeviceInfo.pc_StwDevice != NULL))) &&
        (orc_NodeInfo.GetUpdateFailedStatus() == false)) &&
       (orc_NodeInfo.IsDeviceInfoDiscarded() == false))
   {
      //Assuming known application status
      orc_DeviceProjectName = orc_FileProjectName;
      orc_DeviceFileVersion = orc_FileVersion;
      orc_DeviceBuildDate = orc_FileBuildDate;
      if (c_DeviceInfo.pc_OpenSydeDevice != NULL)
      {
         orc_DeviceValidStatus = C_GtGetText::h_GetText("Valid");
      }
      else
      {
         orc_DeviceValidStatus = C_GtGetText::h_GetText("Not available");
      }
      q_Missing = false;
   }
   else if (((orc_NodeInfo.GetUpdateFailedStatus() == true) &&
             (ou32_ApplicationIndex < orc_NodeInfo.GetFailedApplicationIndex())) &&
            (orc_NodeInfo.IsDeviceInfoDiscarded() == false))
   {
      //Assuming known application status
      orc_DeviceProjectName = orc_FileProjectName;
      orc_DeviceFileVersion = orc_FileVersion;
      orc_DeviceBuildDate = orc_FileBuildDate;
      if (c_DeviceInfo.pc_StwDevice != NULL)
      {
         orc_DeviceValidStatus = C_GtGetText::h_GetText("Valid");
      }
      else
      {
         orc_DeviceValidStatus = C_GtGetText::h_GetText("Not available");
      }
      q_Missing = false;
   }
   else if (((orc_NodeInfo.GetUpdateFailedStatus() == true) &&
             (ou32_ApplicationIndex == orc_NodeInfo.GetFailedApplicationIndex())) ||
            (orc_NodeInfo.IsDeviceInfoDiscarded() == true))
   {
      //Assuming known application status
      q_Missing = true;
   }
   // Another exception: HEX files with multiple application blocks need to be flashed anyway
   else if (orc_NodeInfo.GetHexAppInfoAmbiguous(ou32_ApplicationIndex) == true)
   {
      // update required
      q_Match = false;
      q_Missing = false;
   }
   else
   {
      if (c_DeviceInfo.pc_StwDevice != NULL)
      {
         C_SyvUpNodePropertiesDialog::mh_ExtractStwDeviceInformation(orc_NodeInfo, orc_FileProjectName,
                                                                     orc_DeviceProjectName,
                                                                     orc_FileVersion, orc_DeviceFileVersion,
                                                                     orc_FileBuildDate, orc_DeviceBuildDate,
                                                                     orc_DeviceValidStatus, q_Missing, q_Match);
      }
      else if (c_DeviceInfo.pc_OpenSydeDevice != NULL)
      {
         C_SyvUpNodePropertiesDialog::mh_ExtractOpenSydeDeviceInformation(orc_NodeInfo, orc_FileProjectName,
                                                                          orc_DeviceProjectName,
                                                                          orc_FileVersion, orc_DeviceFileVersion,
                                                                          orc_FileBuildDate, orc_DeviceBuildDate,
                                                                          orc_DeviceValidStatus, q_Missing, q_Match);
      }
      else
      {
         //No information available: probably discarded
      }
   }

   //Icon
   if (q_Missing == true)
   {
      orc_ApplicationStateIcon += "<img src=\"://images/system_views/IconUpdateWaiting.svg\"";
      orc_ApplicationState = C_GtGetText::h_GetText("Update required");
   }
   else
   {
      if (q_Match == true)
      {
         orc_ApplicationStateIcon += "<img src=\"://images/system_views/IconUpdateSuccess.svg\"";
         orc_ApplicationState = C_GtGetText::h_GetText("Up to date");
      }
      else
      {
         orc_ApplicationStateIcon += "<img src=\"://images/system_views/IconUpdateWaiting.svg\"";
         orc_ApplicationState = C_GtGetText::h_GetText("Update required");
      }
   }
   orc_ApplicationStateIcon += "style=\"vertical-align: middle;\"/>";
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Extract STW device information

   \param[in]      orc_NodeInfo           Node info
   \param[in]      orc_FileProjectName    File project name
   \param[in,out]  orc_DeviceProjectName  Device project name
   \param[in]      orc_FileVersion        File version
   \param[in,out]  orc_DeviceFileVersion  Device file version
   \param[in]      orc_FileBuildDate      File build date
   \param[in,out]  orc_DeviceBuildDate    Device build date
   \param[in,out]  orc_DeviceValidStatus  Device valid status
   \param[in,out]  orq_MissingStatus      Missing status
   \param[in,out]  orq_MatchStatus        Match status
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvUpNodePropertiesDialog::mh_ExtractStwDeviceInformation(const C_GiSvSubNodeData & orc_NodeInfo,
                                                                 const QString & orc_FileProjectName,
                                                                 QString & orc_DeviceProjectName,
                                                                 const QString & orc_FileVersion,
                                                                 QString & orc_DeviceFileVersion,
                                                                 const QString & orc_FileBuildDate,
                                                                 QString & orc_DeviceBuildDate,
                                                                 QString & orc_DeviceValidStatus,
                                                                 bool & orq_MissingStatus, bool & orq_MatchStatus)
{
   const C_SyvUpDeviceInfo c_DeviceInfo = orc_NodeInfo.GetDeviceInfo();

   for (int32_t s32_ItDeviceInfoBlock = 0;
        s32_ItDeviceInfoBlock < c_DeviceInfo.pc_StwDevice->c_BasicInformation.c_DeviceInfoBlocks.GetLength();
        ++s32_ItDeviceInfoBlock)
   {
      const C_XFLECUInformation & rc_StwDeviceInfo =
         c_DeviceInfo.pc_StwDevice->c_BasicInformation.c_DeviceInfoBlocks[s32_ItDeviceInfoBlock];
      //Search for name match
      if (orc_FileProjectName.compare(rc_StwDeviceInfo.GetProjectName().Trim().c_str()) == 0)
      {
         //Signal found
         orq_MissingStatus = false;
         //Apply
         orc_DeviceProjectName = rc_StwDeviceInfo.GetProjectName().c_str();
         orc_DeviceFileVersion = rc_StwDeviceInfo.GetProjectVersion().c_str();
         orc_DeviceBuildDate = (rc_StwDeviceInfo.GetDate() + " " + rc_StwDeviceInfo.GetTime()).c_str();
         //Highlighting
         orq_MatchStatus = C_SyvUpNodePropertiesDialog::mh_HandleHighlighting(orc_FileProjectName,
                                                                              orc_DeviceProjectName,
                                                                              orc_FileVersion, orc_DeviceFileVersion,
                                                                              orc_FileBuildDate, orc_DeviceBuildDate);

         // Valid flag is not available for STW Flashloader
         orc_DeviceValidStatus = C_GtGetText::h_GetText("Not available");
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Extract OSY device information

   \param[in]      orc_NodeInfo           Node info
   \param[in]      orc_FileProjectName    File project name
   \param[in,out]  orc_DeviceProjectName  Device project name
   \param[in]      orc_FileVersion        File version
   \param[in,out]  orc_DeviceFileVersion  Device file version
   \param[in]      orc_FileBuildDate      File build date
   \param[in,out]  orc_DeviceBuildDate    Device build date
   \param[in,out]  orc_DeviceValidStatus  Device valid status
   \param[in,out]  orq_MissingStatus      Missing status
   \param[in,out]  orq_MatchStatus        Match status
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvUpNodePropertiesDialog::mh_ExtractOpenSydeDeviceInformation(const C_GiSvSubNodeData & orc_NodeInfo,
                                                                      const QString & orc_FileProjectName,
                                                                      QString & orc_DeviceProjectName,
                                                                      const QString & orc_FileVersion,
                                                                      QString & orc_DeviceFileVersion,
                                                                      const QString & orc_FileBuildDate,
                                                                      QString & orc_DeviceBuildDate,
                                                                      QString & orc_DeviceValidStatus,
                                                                      bool & orq_MissingStatus, bool & orq_MatchStatus)
{
   const C_SyvUpDeviceInfo c_DeviceInfo = orc_NodeInfo.GetDeviceInfo();

   //Skip first application because this is the flashloader (OSY ONLY!)
   for (uint32_t u32_ItOsyApplication = 1;
        u32_ItOsyApplication <
        c_DeviceInfo.pc_OpenSydeDevice->c_Applications.size();
        ++u32_ItOsyApplication)
   {
      const C_OscProtocolDriverOsy::C_FlashBlockInfo & rc_OsyDeviceInfo =
         c_DeviceInfo.pc_OpenSydeDevice->c_Applications[u32_ItOsyApplication];
      //Search for name match
      if (orc_FileProjectName.trimmed().compare(rc_OsyDeviceInfo.c_ApplicationName.Trim().c_str()) == 0)
      {
         //Signal found
         orq_MissingStatus = false;
         //Apply
         orc_DeviceProjectName = rc_OsyDeviceInfo.c_ApplicationName.c_str();
         orc_DeviceFileVersion = rc_OsyDeviceInfo.c_ApplicationVersion.c_str();
         orc_DeviceBuildDate =
            (rc_OsyDeviceInfo.c_BuildDate + " " + rc_OsyDeviceInfo.c_BuildTime).c_str();
         //Highlighting
         orq_MatchStatus = C_SyvUpNodePropertiesDialog::mh_HandleHighlighting(orc_FileProjectName,
                                                                              orc_DeviceProjectName,
                                                                              orc_FileVersion, orc_DeviceFileVersion,
                                                                              orc_FileBuildDate, orc_DeviceBuildDate);

         if (rc_OsyDeviceInfo.u8_SignatureValid == 0U)
         {
            orc_DeviceValidStatus = C_GtGetText::h_GetText("Valid");
         }
         else
         {
            orc_DeviceValidStatus = C_GtGetText::h_GetText("<b>Invalid</b>");
            orq_MatchStatus = false;
         }
         break;
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Handle highlighting

   \param[in]      orc_FileProjectName    File project name
   \param[in,out]  orc_DeviceProjectName  Device project name
   \param[in]      orc_FileVersion        File version
   \param[in,out]  orc_DeviceFileVersion  Device file version
   \param[in]      orc_FileBuildDate      File build date
   \param[in,out]  orc_DeviceBuildDate    Device build date

   \return
   Matching status
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_SyvUpNodePropertiesDialog::mh_HandleHighlighting(const QString & orc_FileProjectName,
                                                        QString & orc_DeviceProjectName,
                                                        const QString & orc_FileVersion,
                                                        QString & orc_DeviceFileVersion,
                                                        const QString & orc_FileBuildDate,
                                                        QString & orc_DeviceBuildDate)
{
   bool q_Match = true;

   if (orc_FileProjectName.trimmed() != orc_DeviceProjectName.trimmed())
   {
      orc_DeviceProjectName = "<b>" + orc_DeviceProjectName + "</b>";
      q_Match = false;
   }
   if (orc_FileVersion.trimmed() != orc_DeviceFileVersion.trimmed())
   {
      orc_DeviceFileVersion = "<b>" + orc_DeviceFileVersion + "</b>";
      q_Match = false;
   }
   if (orc_FileBuildDate.trimmed() != orc_DeviceBuildDate.trimmed())
   {
      orc_DeviceBuildDate = "<b>" + orc_DeviceBuildDate + "</b>";
      q_Match = false;
   }
   return q_Match;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Extract content from data for node

   \param[in]      oq_HexAppInfoAmbiguous    Hex app info ambiguous
   \param[in,out]  orc_FileProjectName       File project name
   \param[in]      orc_DeviceProjectName     Device project name
   \param[in]      orc_FileVersion           File version
   \param[in]      orc_DeviceFileVersion     Device file version
   \param[in]      orc_FileBuildDate         File build date
   \param[in]      orc_DeviceBuildDate       Device build date
   \param[in,out]  orc_Content               Content
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvUpNodePropertiesDialog::mh_ExtractDetailsPartFromDataForNode(const bool oq_HexAppInfoAmbiguous,
                                                                       const QString & orc_FileProjectName,
                                                                       const QString & orc_DeviceProjectName,
                                                                       const QString & orc_FileVersion,
                                                                       const QString & orc_DeviceFileVersion,
                                                                       const QString & orc_FileBuildDate,
                                                                       const QString & orc_DeviceBuildDate,
                                                                       QString & orc_Content)
{
   QString c_NewContent;

   if (oq_HexAppInfoAmbiguous == true)
   {
      c_NewContent += C_GtGetText::h_GetText("HEX file has multiple application blocks and therefore "
                                             "information is ambiguous.");
   }
   else
   {
      //Content
      c_NewContent += "<table style=\"width:100%;\">";
      c_NewContent += "<tr>";
      c_NewContent += C_SyvUpNodePropertiesDialog::mhc_HTML_CELL_TAG_START_CONTENT;
      //lint -e{1946} Qt interface
      c_NewContent += QString("<u>") + C_GtGetText::h_GetText("Property") + "</u>";
      c_NewContent += "</td>";
      c_NewContent += C_SyvUpNodePropertiesDialog::mhc_HTML_CELL_TAG_START_CONTENT;
      //lint -e{1946} Qt interface
      c_NewContent += QString("<u>") + C_GtGetText::h_GetText("Update Package") + "</u>";
      c_NewContent += "</td>";
      c_NewContent += C_SyvUpNodePropertiesDialog::mhc_HTML_CELL_TAG_START_CONTENT;
      //lint -e{1946} Qt interface
      c_NewContent += QString("<u>") + C_GtGetText::h_GetText("Device Status") + "</u>";
      c_NewContent += "</td>";
      c_NewContent += "</tr>";
      c_NewContent += "<tr>";
      c_NewContent += C_SyvUpNodePropertiesDialog::mhc_HTML_CELL_TAG_START_CONTENT;
      c_NewContent += C_GtGetText::h_GetText("Project Name");
      c_NewContent += "</td>";
      c_NewContent += C_SyvUpNodePropertiesDialog::mhc_HTML_CELL_TAG_START_CONTENT;
      c_NewContent += orc_FileProjectName;
      c_NewContent += "</td>";
      c_NewContent += C_SyvUpNodePropertiesDialog::mhc_HTML_CELL_TAG_START_CONTENT;
      c_NewContent += orc_DeviceProjectName;
      c_NewContent += "</td>";
      c_NewContent += "</tr>";
      c_NewContent += "<tr>";
      c_NewContent += C_SyvUpNodePropertiesDialog::mhc_HTML_CELL_TAG_START_CONTENT;
      c_NewContent += C_GtGetText::h_GetText("Version");
      c_NewContent += "</td>";
      c_NewContent += C_SyvUpNodePropertiesDialog::mhc_HTML_CELL_TAG_START_CONTENT;
      c_NewContent += orc_FileVersion;
      c_NewContent += "</td>";
      c_NewContent += C_SyvUpNodePropertiesDialog::mhc_HTML_CELL_TAG_START_CONTENT;
      c_NewContent += orc_DeviceFileVersion;
      c_NewContent += "</td>";
      c_NewContent += "</tr>";
      c_NewContent += "<tr>";
      c_NewContent += C_SyvUpNodePropertiesDialog::mhc_HTML_CELL_TAG_START_CONTENT;
      c_NewContent += C_GtGetText::h_GetText("Timestamp");
      c_NewContent += "</td>";
      c_NewContent += C_SyvUpNodePropertiesDialog::mhc_HTML_CELL_TAG_START_CONTENT;
      c_NewContent += orc_FileBuildDate;
      c_NewContent += "</td>";
      c_NewContent += C_SyvUpNodePropertiesDialog::mhc_HTML_CELL_TAG_START_CONTENT;
      c_NewContent += orc_DeviceBuildDate;
      c_NewContent += "</td>";
      c_NewContent += "</tr>";
      c_NewContent += "</table>";
   }
   orc_Content = c_NewContent;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Append application for node

   \param[in]      ou32_ApplicationIndex     Application index
   \param[in]      orc_ApplicationName       Application name
   \param[in]      orc_ApplicationStateIcon  Application state icon
   \param[in]      orc_ApplicationState      Application state
   \param[in]      orc_DeviceValidStatus     Device valid status
   \param[in]      orc_DetailsPart           Details part
   \param[in,out]  orc_Content               Content
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvUpNodePropertiesDialog::mh_AppendApplicationForNode(const uint32_t ou32_ApplicationIndex,
                                                              const QString & orc_ApplicationName,
                                                              const QString & orc_ApplicationStateIcon,
                                                              const QString & orc_ApplicationState,
                                                              const QString & orc_DeviceValidStatus,
                                                              const QString & orc_DetailsPart, QString & orc_Content)
{
   QString c_NewContent;

   //Heading
   c_NewContent += "<h4>";
   c_NewContent += static_cast<QString>("#%1 - %2 ").
                   arg(ou32_ApplicationIndex + 1).arg(orc_ApplicationName);
   c_NewContent += "</h4>";

   //Content
   c_NewContent += "<table>";
   c_NewContent += "<tr>";
   c_NewContent += C_SyvUpNodePropertiesDialog::mhc_HTML_CELL_TAG_START_MAIN;
   c_NewContent += C_GtGetText::h_GetText("Status");
   c_NewContent += "</td>";
   c_NewContent += "<td><table><tr>";
   c_NewContent += C_SyvUpNodePropertiesDialog::mhc_HTML_CELL_TAG_START_STATUS;
   c_NewContent += orc_ApplicationStateIcon + "</td><td>" +
                   orc_ApplicationState + "</td>";
   c_NewContent += "</tr></table></td>";
   c_NewContent += "</tr>";
   c_NewContent += "<tr>";
   c_NewContent += C_SyvUpNodePropertiesDialog::mhc_HTML_CELL_TAG_START_MAIN;
   c_NewContent += C_GtGetText::h_GetText("Signature");
   c_NewContent += "</td>";
   c_NewContent += "<td>" + orc_DeviceValidStatus + "</td>";
   c_NewContent += "</tr>";
   c_NewContent += "<tr>";
   c_NewContent += C_SyvUpNodePropertiesDialog::mhc_HTML_CELL_TAG_START_MAIN;
   c_NewContent += C_GtGetText::h_GetText("Summary");
   c_NewContent += "</td>";
   c_NewContent += "<td>";
   c_NewContent += orc_DetailsPart;
   c_NewContent += "</td>";
   c_NewContent += "</tr>";
   c_NewContent += "</table>";

   orc_Content += c_NewContent;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Init data block table other section for node

   \param[in]      orc_NodeInfo  Node info
   \param[in,out]  orc_Content   Content
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvUpNodePropertiesDialog::mh_InitDataBlockTableOtherSectionForNode(const C_GiSvSubNodeData & orc_NodeInfo,
                                                                           QString & orc_Content)
{
   QString c_NewContent;

   //Other files (parameter set images or for file based targets any file)
   if ((orc_NodeInfo.GetParamFileInfosCount() > 0UL) ||
       (orc_NodeInfo.IsPemFileInfoSet() == true) ||
       (orc_NodeInfo.GetFileInfosCount() > 0UL))
   {
      c_NewContent += "<h4>";
      c_NewContent += C_GtGetText::h_GetText("Other Files");
      c_NewContent += "</h4>";
      c_NewContent += "<table><tr>";
      c_NewContent += C_SyvUpNodePropertiesDialog::mhc_HTML_CELL_TAG_START_MAIN;
      c_NewContent += C_GtGetText::h_GetText("Status");
      c_NewContent += "</td>";
      c_NewContent += "<td><table><tr>";
      c_NewContent += C_SyvUpNodePropertiesDialog::mhc_HTML_CELL_TAG_START_STATUS;
      c_NewContent +=
         "<img src=\"://images/system_views/IconUpdateWaiting.svg\"style=\"vertical-align: middle;\"/></td><td>";
      c_NewContent += C_GtGetText::h_GetText("No status request supported. Update required.");
      c_NewContent += "</td>";
      c_NewContent += "</tr></table></td>";
      c_NewContent += "</tr>";
      c_NewContent += "</table>";
   }
   orc_Content += c_NewContent;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Initialize flashloader information table
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvUpNodePropertiesDialog::m_InitFlashloaderTable(void) const
{
   QString c_Content = "<html><body>";

   const std::vector<uint32_t> c_NodeIndices = C_PuiSdHandler::h_GetInstance()->GetAllNodeGroupIndicesUsingNodeIndex(
      this->mu32_NodeIndex);

   tgl_assert(c_NodeIndices.size() == this->mc_NodeData.GetSubNodeCount());
   if (c_NodeIndices.size() == this->mc_NodeData.GetSubNodeCount())
   {
      for (uint32_t u32_ItDevice = 0UL; u32_ItDevice < c_NodeIndices.size(); ++u32_ItDevice)
      {
         const C_OscNode * const pc_Node =
            C_PuiSdHandler::h_GetInstance()->GetOscNodeConst(c_NodeIndices[u32_ItDevice]);
         const C_GiSvSubNodeData * const pc_SubDevice =
            this->mc_NodeData.GetSubNodeByNodeIndex(c_NodeIndices[u32_ItDevice]);
         tgl_assert((pc_Node != NULL) && (pc_SubDevice != NULL));
         if ((pc_Node != NULL) && (pc_SubDevice != NULL))
         {
            C_SyvUpNodePropertiesDialog::mh_InitFlashloaderTableForNode(*pc_SubDevice,
                                                                        c_Content, *pc_Node,
                                                                        c_NodeIndices.size() > 1UL);
         }
      }
   }
   c_Content += "</body></html>";
   this->mpc_Ui->pc_TextEditFlash->setHtml(c_Content);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Init flashloader table for node

   \param[in]      orc_NodeInfo        Node info
   \param[in,out]  orc_Content         Content
   \param[in]      orc_Node            Node
   \param[in]      oq_IsMultiDevice    Is multi device
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvUpNodePropertiesDialog::mh_InitFlashloaderTableForNode(const C_GiSvSubNodeData & orc_NodeInfo,
                                                                 QString & orc_Content, const C_OscNode & orc_Node,
                                                                 const bool oq_IsMultiDevice)
{
   QString c_NewContent = "";
   const C_SyvUpDeviceInfo c_DeviceInfo = orc_NodeInfo.GetDeviceInfo();

   stw::scl::C_SclStringList c_List;

   if (c_DeviceInfo.pc_OpenSydeDevice != NULL)
   {
      C_OscSuSequences::h_OpenSydeFlashloaderInformationToText(*c_DeviceInfo.pc_OpenSydeDevice, c_List);
   }
   else if (c_DeviceInfo.pc_StwDevice != NULL)
   {
      C_OscSuSequences::h_StwFlashloaderInformationToText(*c_DeviceInfo.pc_StwDevice, c_List);
   }
   else
   {
      //Unexpected
   }
   if (c_List.Strings.GetLength() > 1)
   {
      c_NewContent +=
         static_cast<QString>(C_GtGetText::h_GetText("Note: Flashloader data is read during \"Enter Update Mode\""));
      c_NewContent += "<p>";
      for (int32_t s32_ItString = 0; s32_ItString < c_List.Strings.GetLength(); ++s32_ItString)
      {
         c_NewContent += static_cast<QString>(c_List.Strings[s32_ItString].c_str()) + "<br/>";
      }
      c_NewContent += "</p>";
   }
   C_SyvUpNodePropertiesDialog::mh_HandleSectionAppend(c_NewContent, orc_Content, orc_Node, oq_IsMultiDevice);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Handle section append

   \param[in]      orc_NewPart            New part
   \param[in,out]  orc_CompleteSection    Complete section
   \param[in]      orc_Node               Node
   \param[in]      oq_IsMultiDevice       Is multi device
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvUpNodePropertiesDialog::mh_HandleSectionAppend(const QString & orc_NewPart, QString & orc_CompleteSection,
                                                         const C_OscNode & orc_Node, const bool oq_IsMultiDevice)
{
   if ((!orc_NewPart.isEmpty()) && (oq_IsMultiDevice))
   {
      orc_CompleteSection += "<h3>";
      orc_CompleteSection += orc_Node.c_Properties.c_Name.c_str();
      orc_CompleteSection += "</h3>";
   }
   orc_CompleteSection += orc_NewPart;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Handle discard trigger
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvUpNodePropertiesDialog::m_OnDiscard(void)
{
   //Trigger changes
   this->mc_NodeData.DiscardInfo();
   m_ReInitStatus();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Handle status update
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvUpNodePropertiesDialog::m_ReInitStatus(void) const
{
   m_InitStatus();
   m_InitDataBlockTable();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Updates the second tab with the progress log based on the connect and update states
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvUpNodePropertiesDialog::m_UpdateProgressLog(void) const
{
   QString c_HtmlText;

   c_HtmlText = this->m_GetProgressLogConnectStatesString();
   c_HtmlText += this->m_GetProgressLogUpdateStatesString();

   this->mpc_Ui->pc_ProgressLog->setHtml(c_HtmlText);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Updates the second tab with the progress log based on the connect states
*/
//----------------------------------------------------------------------------------------------------------------------
QString C_SyvUpNodePropertiesDialog::m_GetProgressLogConnectStatesString(void) const
{
   QString c_Text = "";

   c_Text += mc_REPORT_HEADLINE_HTML_TAG_START;
   c_Text += static_cast<QString>(C_GtGetText::h_GetText("Enter Update Mode"));
   c_Text += mc_REPORT_HEADLINE_HTML_TAG_END;

   if (this->mc_NodeData.IsNodeConnectStatesSet() == true)
   {
      const uint32_t u32_SubNodeCount = this->mc_NodeData.GetSubNodeCount();
      uint32_t u32_SubNodeCounter;

      for (u32_SubNodeCounter = 0U; u32_SubNodeCounter < u32_SubNodeCount; ++u32_SubNodeCounter)
      {
         // Print only information about active sub nodes
         if (this->mc_NodeData.IsSubNodeActive(u32_SubNodeCounter) == true)
         {
            const C_GiSvSubNodeData * const pc_SubNodeData =
               this->mc_NodeData.GetSubNodeBySubNodeIndex(u32_SubNodeCounter);

            tgl_assert(pc_SubNodeData != NULL);
            if (pc_SubNodeData != NULL)
            {
               if (u32_SubNodeCount > 1)
               {
                  // In case of a multiple CPU node, printing each name
                  const uint32_t u32_NodeIndex = pc_SubNodeData->GetNodeIndex();
                  const C_OscNode * const opc_OscNode = C_PuiSdHandler::h_GetInstance()->GetOscNodeConst(u32_NodeIndex);
                  if (opc_OscNode != NULL)
                  {
                     c_Text += static_cast<QString>(C_GtGetText::h_GetText("Node: ")) +
                               static_cast<QString>(opc_OscNode->c_Properties.c_Name.c_str());
                  }
               }

               // Printing the states
               c_Text += mh_GetProgressLogConnectStatesStringForSubNode(pc_SubNodeData) + "<br>";
               if ((u32_SubNodeCounter + 1U) < u32_SubNodeCount)
               {
                  // Not the last sub node, add an additional space between sub nodes for better readability
                  c_Text += "<br>";
               }
            }
         }
      }
   }
   else if (this->mc_NodeData.GetConnectInProgressStatus() == true)
   {
      // States not available yet, but process is running now
      c_Text += static_cast<QString>(C_GtGetText::h_GetText("Connecting ...")) + "<br>";
   }
   else
   {
      // States not available
      c_Text += static_cast<QString>(C_GtGetText::h_GetText("Enter update mode to get the progress information.")) +
                "<br>";
   }

   return c_Text;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Updates the second tab with the progress log based on the connect states

   \param[in]   opc_SubNodeData   Pointer the sub node data

*/
//----------------------------------------------------------------------------------------------------------------------
QString C_SyvUpNodePropertiesDialog::mh_GetProgressLogConnectStatesStringForSubNode(
   const C_GiSvSubNodeData * const opc_SubNodeData)
{
   QString c_Text = "";

   if (opc_SubNodeData != NULL)
   {
      const QString c_Ok = static_cast<QString>(C_GtGetText::h_GetText("Ok"));
      const QString c_Failed = static_cast<QString>(C_GtGetText::h_GetText("Failed"));
      const QString c_Yes = static_cast<QString>(C_GtGetText::h_GetText("Yes"));
      const QString c_No = static_cast<QString>(C_GtGetText::h_GetText("No"));
      const QString c_Skipped = static_cast<QString>(C_GtGetText::h_GetText("Skipped"));
      const C_OscSuSequencesNodeConnectStates & rc_States = opc_SubNodeData->GetNodeConnectStates();
      const C_GiSvSubNodeData::C_GiSvSubNodeDataPreconditionErrors & rc_PreconditionErrors =
         opc_SubNodeData->GetNodeConnectPreconditionErrors();

      c_Text += mh_GetTableLineForBrowser(C_GtGetText::h_GetText("Request connection"),
                                          ((rc_States.q_Timeout == false) ? c_Ok : c_Failed),
                                          1);

      c_Text += mh_GetTableLineForBrowser(C_GtGetText::h_GetText("Node information read"),
                                          mh_GetSuSequenceNodeStateString(rc_States.e_InformationRead),
                                          1);

      c_Text += mh_GetTableLineForBrowser(C_GtGetText::h_GetText("Client authentication required?"),
                                          ((rc_States.q_AuthenticationNecessary == true) ? c_Yes : c_No),
                                          1);

      if (rc_States.q_AuthenticationNecessary == true)
      {
         c_Text += mh_GetTableLineForBrowser(C_GtGetText::h_GetText("Client authentication"),
                                             ((rc_States.q_AuthenticationError == false) ? c_Ok : c_Failed),
                                             2);
      }
      else
      {
         // No security active
         c_Text += mh_GetTableLineForBrowser(C_GtGetText::h_GetText("Running authentication"),
                                             c_Skipped,
                                             2);
      }

      c_Text +=
         mh_GetTableLineForBrowser(static_cast<QString>(C_GtGetText::h_GetText(
                                                           "Parameter Set Image file (syde_psi)")), "",
                                   1);
      c_Text += mh_GetTableLineForBrowser(C_GtGetText::h_GetText("Writing to NVM supported?"),
                                          mh_GetSuSequenceNodePreconditionString(
                                             rc_States.c_AvailableFeatures.q_FlashloaderCanWriteToNvm,
                                             rc_PreconditionErrors.q_NvmWriteError),
                                          2);

      c_Text += mh_GetTableLineForBrowser(static_cast<QString>(C_GtGetText::h_GetText("PEM file")), "",
                                          1);
      c_Text += mh_GetTableLineForBrowser(C_GtGetText::h_GetText(
                                             "Security supported?"),
                                          mh_GetSuSequenceNodePreconditionString(
                                             rc_States.c_AvailableFeatures.q_SupportsSecurity,
                                             rc_PreconditionErrors.q_PemWriteError),
                                          2);
      c_Text += mh_GetTableLineForBrowser(C_GtGetText::h_GetText("Activating debugger supported?"),
                                          mh_GetSuSequenceNodePreconditionString(
                                             rc_States.c_AvailableFeatures.q_SupportsDebuggerOn,
                                             rc_PreconditionErrors.q_DebuggerEnableError),
                                          2);
      c_Text += mh_GetTableLineForBrowser(C_GtGetText::h_GetText("Deactivating debugger supported?"),
                                          mh_GetSuSequenceNodePreconditionString(
                                             rc_States.c_AvailableFeatures.q_SupportsDebuggerOff,
                                             rc_PreconditionErrors.q_DebuggerDisableError),
                                          2);

      c_Text += mh_GetTableLineForBrowser(static_cast<QString>(C_GtGetText::h_GetText("Routing")), "",
                                          1);
      c_Text += mh_GetTableLineForBrowser(C_GtGetText::h_GetText(
                                             "Ethernet to Ethernet routing supported?"),
                                          mh_GetSuSequenceNodePreconditionString(
                                             rc_States.c_AvailableFeatures.q_EthernetToEthernetRoutingSupported,
                                             rc_PreconditionErrors.q_EthToEthError),
                                          2);
   }

   return c_Text;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Updates the second tab with the progress log based on the update states
*/
//----------------------------------------------------------------------------------------------------------------------
QString C_SyvUpNodePropertiesDialog::m_GetProgressLogUpdateStatesString(void) const
{
   QString c_Text = "";
   const bool q_UpdateStatesSet = this->mc_NodeData.IsNodeUpdateStatesSet();
   const bool q_UpdateInProgress = this->mc_NodeData.GetUpdateInProgressStatus();

   if ((q_UpdateStatesSet == true) || (q_UpdateInProgress == true))
   {
      c_Text += mc_REPORT_HEADLINE_HTML_TAG_START;
      c_Text += static_cast<QString>(C_GtGetText::h_GetText("Update System"));
      c_Text += mc_REPORT_HEADLINE_HTML_TAG_END;

      if (q_UpdateStatesSet == true)
      {
         const uint32_t u32_SubNodeCount = this->mc_NodeData.GetSubNodeCount();
         uint32_t u32_SubNodeCounter;

         for (u32_SubNodeCounter = 0U; u32_SubNodeCounter < u32_SubNodeCount; ++u32_SubNodeCounter)
         {
            // Print only information about active sub nodes
            if (this->mc_NodeData.IsSubNodeActive(u32_SubNodeCounter) == true)
            {
               const C_GiSvSubNodeData * const pc_SubNodeData =
                  this->mc_NodeData.GetSubNodeBySubNodeIndex(u32_SubNodeCounter);

               tgl_assert(pc_SubNodeData != NULL);
               if (pc_SubNodeData != NULL)
               {
                  if (u32_SubNodeCount > 1)
                  {
                     // In case of a multiple CPU node, printing each name
                     const uint32_t u32_NodeIndex = pc_SubNodeData->GetNodeIndex();
                     const C_OscNode * const opc_OscNode = C_PuiSdHandler::h_GetInstance()->GetOscNodeConst(
                        u32_NodeIndex);
                     if (opc_OscNode != NULL)
                     {
                        c_Text += static_cast<QString>(C_GtGetText::h_GetText("Node: ")) +
                                  static_cast<QString>(opc_OscNode->c_Properties.c_Name.c_str());
                     }
                  }

                  // Printing the states
                  c_Text += mh_GetProgressLogUpdateStatesStringForSubNode(pc_SubNodeData) + "<br>";
                  if ((u32_SubNodeCounter + 1U) < u32_SubNodeCount)
                  {
                     // Not the last sub node, add an additional space between sub nodes for better readability
                     c_Text += "<br>";
                  }
               }
            }
         }
      }
      else
      {
         // States not available yet
         c_Text += static_cast<QString>(C_GtGetText::h_GetText("Updating ...")) + "<br>";
      }
   }

   return c_Text;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Updates the second tab with the progress log based on the connect states

   \param[in]   opc_SubNodeData   Pointer the sub node data

*/
//----------------------------------------------------------------------------------------------------------------------
QString C_SyvUpNodePropertiesDialog::mh_GetProgressLogUpdateStatesStringForSubNode(
   const C_GiSvSubNodeData * const opc_SubNodeData)
{
   QString c_Text = "";

   if (opc_SubNodeData != NULL)
   {
      const C_OscSuSequencesNodeUpdateStates & rc_States = opc_SubNodeData->GetNodeUpdateStates();
      uint32_t u32_FileCounter;
      bool q_AtLeastOneFile = false;

      // Hex files
      for (u32_FileCounter = 0U; u32_FileCounter < rc_States.c_StateHexFiles.size(); ++u32_FileCounter)
      {
         const C_OscSuSequencesNodeHexFileStates & rc_FileState = rc_States.c_StateHexFiles[u32_FileCounter];

         q_AtLeastOneFile = true;

         c_Text += mh_GetTableLineForBrowser(
            C_GtGetText::h_GetText("File ") + static_cast<QString>(rc_FileState.c_FileName.c_str()) + ":",
            "",
            1);

         c_Text += mh_GetTableLineForBrowser(
            C_GtGetText::h_GetText("Loading file"),
            mh_GetSuSequenceNodeFileLoadStateString(rc_FileState.e_FileExists, rc_FileState.e_FileLoaded),
            2);

         // Checking preconditions
         c_Text += mh_GetTableLineForBrowser(
            C_GtGetText::h_GetText("Reading file signature block address"),
            mh_GetSuSequenceNodeStateString(rc_FileState.e_SignatureBlockAddressFromFileRead),
            2);

         c_Text += mh_GetTableLineForBrowser(
            C_GtGetText::h_GetText("Comparing node name"),
            mh_GetSuSequenceNodeStateString(rc_FileState.e_NodeNameCompared),
            2);

         c_Text += mh_GetTableLineForBrowser(
            C_GtGetText::h_GetText("Reading dump from file"),
            mh_GetSuSequenceNodeStateString(rc_FileState.e_DataDumpFromFileRead),
            2);

         c_Text += mh_GetTableLineForBrowser(
            C_GtGetText::h_GetText("Checking available flash memory"),
            mh_GetSuSequenceNodeStateString(rc_FileState.e_AvailableFlashMemoryChecked),
            2);

         // Transfers
         c_Text += mh_GetTableLineForBrowser(
            C_GtGetText::h_GetText("Sending download request"),
            mh_GetSuSequenceNodeStateString(rc_FileState.e_RequestDownloadSent),
            2);

         c_Text += mh_GetTableLineForBrowser(
            C_GtGetText::h_GetText("Sending all transfer data"),
            mh_GetSuSequenceNodeStateString(rc_FileState.e_AllTransferDataSent),
            2);

         c_Text += mh_GetTableLineForBrowser(
            C_GtGetText::h_GetText("Sending transfer exit request"),
            mh_GetSuSequenceNodeStateString(rc_FileState.e_RequestTransferAddressExitSent),
            2);
      }

      // STW flashloader files
      for (u32_FileCounter = 0U; u32_FileCounter < rc_States.c_StateStwFlHexFiles.size(); ++u32_FileCounter)
      {
         const C_OscSuSequencesNodeStwFlHexFileStates & rc_FileState = rc_States.c_StateStwFlHexFiles[u32_FileCounter];

         q_AtLeastOneFile = true;

         c_Text += mh_GetTableLineForBrowser(
            C_GtGetText::h_GetText("File ") + static_cast<QString>(rc_FileState.c_FileName.c_str())  + ":",
            "",
            1);

         // STW flashloader file states have no information about the loading of the file
         c_Text += mh_GetTableLineForBrowser(
            C_GtGetText::h_GetText("Checking file"),
            mh_GetSuSequenceNodeStateString(rc_FileState.e_FileExists),
            2);

         // Transfers
         c_Text += mh_GetTableLineForBrowser(
            C_GtGetText::h_GetText("Sending file"),
            mh_GetSuSequenceNodeStateString(rc_FileState.e_FileSent),
            2);
      }

      // Other files
      for (u32_FileCounter = 0U; u32_FileCounter < rc_States.c_StateOtherFiles.size(); ++u32_FileCounter)
      {
         const C_OscSuSequencesNodeOtherFileStates & rc_FileState = rc_States.c_StateOtherFiles[u32_FileCounter];

         q_AtLeastOneFile = true;

         c_Text += mh_GetTableLineForBrowser(
            C_GtGetText::h_GetText("File ") + static_cast<QString>(rc_FileState.c_FileName.c_str()) + ":",
            "",
            1);

         c_Text += mh_GetTableLineForBrowser(
            C_GtGetText::h_GetText("Loading file"),
            mh_GetSuSequenceNodeFileLoadStateString(rc_FileState.e_FileExists, rc_FileState.e_FileLoaded),
            2);

         // Transfers
         c_Text += mh_GetTableLineForBrowser(
            C_GtGetText::h_GetText("Sending transfer request"),
            mh_GetSuSequenceNodeStateString(rc_FileState.e_RequestFileTransferSent),
            2);

         c_Text += mh_GetTableLineForBrowser(
            C_GtGetText::h_GetText("Sending all transfer data"),
            mh_GetSuSequenceNodeStateString(rc_FileState.e_AllTransferDataSent),
            2);

         c_Text += mh_GetTableLineForBrowser(
            C_GtGetText::h_GetText("Sending transfer exit request"),
            mh_GetSuSequenceNodeStateString(rc_FileState.e_RequestTransferFileExitSent),
            2);

         c_Text += mh_GetTableLineForBrowser(
            C_GtGetText::h_GetText("Sending transfer exit result request"),
            mh_GetSuSequenceNodeStateString(rc_FileState.e_RequestTransferFileExitResultSent),
            2);
      }

      // PSI files
      for (u32_FileCounter = 0U; u32_FileCounter < rc_States.c_StatePsiFiles.size(); ++u32_FileCounter)
      {
         const C_OscSuSequencesNodePsiFileStates & rc_FileState = rc_States.c_StatePsiFiles[u32_FileCounter];

         q_AtLeastOneFile = true;

         c_Text += mh_GetTableLineForBrowser(
            C_GtGetText::h_GetText("File ") + static_cast<QString>(rc_FileState.c_FileName.c_str()) + ":",
            "",
            1);

         c_Text += mh_GetTableLineForBrowser(
            C_GtGetText::h_GetText("Loading file"),
            mh_GetSuSequenceNodeFileLoadStateString(rc_FileState.e_FileExists, rc_FileState.e_FileLoaded),
            2);

         // Transfers
         c_Text += mh_GetTableLineForBrowser(
            C_GtGetText::h_GetText("Writing file"),
            mh_GetSuSequenceNodeStateString(rc_FileState.e_PsiFileWrote),
            2);
      }

      // PEM file
      if (rc_States.c_StatePemFile.c_FileName != "")
      {
         const C_OscSuSequencesNodePemFileStates & rc_FileState = rc_States.c_StatePemFile;

         q_AtLeastOneFile = true;

         c_Text += mh_GetTableLineForBrowser(
            C_GtGetText::h_GetText("File ") + static_cast<QString>(rc_FileState.c_FileName.c_str()) + ":",
            "",
            1);

         c_Text += mh_GetTableLineForBrowser(
            C_GtGetText::h_GetText("Loading file"),
            mh_GetSuSequenceNodeFileLoadStateString(rc_FileState.e_FileExists, rc_FileState.e_FileLoaded),
            2);

         // PEM file itself
         c_Text += mh_GetTableLineForBrowser(
            C_GtGetText::h_GetText("Extracting file"),
            mh_GetSuSequenceNodeStateString(rc_FileState.e_PemFileExtracted),
            2);

         c_Text += mh_GetTableLineForBrowser(
            C_GtGetText::h_GetText("Sending public key of file"),
            mh_GetSuSequenceNodeStateString(rc_FileState.e_SecurityKeySent),
            2);

         // PEM file states
         c_Text += mh_GetTableLineForBrowser(
            C_GtGetText::h_GetText("Sending security enable flag"),
            mh_GetSuSequenceNodeStateString(rc_FileState.e_SecurityFlagSent),
            2);

         c_Text += mh_GetTableLineForBrowser(
            C_GtGetText::h_GetText("Sending debugger enable flag"),
            mh_GetSuSequenceNodeStateString(rc_FileState.e_DebuggerFlagSent),
            2);
      }

      if (q_AtLeastOneFile == false)
      {
         c_Text += mh_GetTableLineForBrowser(
            C_GtGetText::h_GetText("No files for updating"),
            "",
            1);
      }
   }

   return c_Text;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Returns the two input strings in HTML table format separated in two column for one row

   \param[in]       orc_FirstRow     String for the first column
   \param[in]       orc_SecondRow    String for the second column
   \param[in]       ou32_Layer       Layer of "tree" visualization. 1 or 2

   \return
   HTML formatted table row with two columns
*/
//----------------------------------------------------------------------------------------------------------------------
QString C_SyvUpNodePropertiesDialog::mh_GetTableLineForBrowser(const QString & orc_FirstRow,
                                                               const QString & orc_SecondRow, const uint32_t ou32_Layer)
{
   QString c_Text = "";
   int32_t s32_UsedMargin;
   int32_t s32_UsedWidthCol1;

   // Configuration of the table layout dependent of the layer
   switch (ou32_Layer)
   {
   case 2:
      s32_UsedMargin = 29;
      s32_UsedWidthCol1 = 59;
      break;
   case 1:
   default: // 1 is default
      s32_UsedMargin = 10;
      s32_UsedWidthCol1 = 60;
      break;
   }

   c_Text += "<table width=\"100%\" style =\" margin-left:" + QString::number(s32_UsedMargin) + "px\">";
   c_Text += "<tr>";
   c_Text += "<td width=\"" + QString::number(s32_UsedWidthCol1) + "%\">" + orc_FirstRow + "</td>";
   c_Text += "<td width=\"40%\">" + orc_SecondRow + "</td>";
   c_Text += "</tr>";
   c_Text += "</table>";

   return c_Text;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Returns the state value in text form

   \param[in]       oe_State     State value

   \return
   Text interpretation of state
*/
//----------------------------------------------------------------------------------------------------------------------
QString C_SyvUpNodePropertiesDialog::mh_GetSuSequenceNodeStateString(const E_OscSuSequencesNodeState oe_State)
{
   QString c_Text;
   const QString c_Ok = static_cast<QString>(C_GtGetText::h_GetText("Ok"));
   const QString c_Failed = static_cast<QString>(C_GtGetText::h_GetText("Failed"));
   const QString c_Skipped = static_cast<QString>(C_GtGetText::h_GetText("Skipped"));
   const QString c_SkippedPrevErr =
      static_cast<QString>(C_GtGetText::h_GetText("<b>Skipped</b> (error occurred during process)"));

   switch (oe_State)
   {
   case eSUSEQ_STATE_NOT_EXECUTED:
      c_Text = c_SkippedPrevErr;
      break;
   case eSUSEQ_STATE_NOT_NEEDED:
      c_Text = c_Skipped;
      break;
   case eSUSEQ_STATE_NO_ERR:
      c_Text = c_Ok;
      break;
   case eSUSEQ_STATE_ERROR:
      c_Text = c_Failed;
      break;
   default:
      break;
   }

   return c_Text;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Returns the state value in text form

   \param[in]       oe_StateExists      State value of file existing check
   \param[in]       oe_StateLoadedd     State value of file loading process

   \return
   Text interpretation of state
*/
//----------------------------------------------------------------------------------------------------------------------
QString C_SyvUpNodePropertiesDialog::mh_GetSuSequenceNodeFileLoadStateString(
   const stw::opensyde_core::E_OscSuSequencesNodeState oe_StateExists,
   const stw::opensyde_core::E_OscSuSequencesNodeState oe_StateLoadedd)
{
   QString c_Text;
   const QString c_Ok = static_cast<QString>(C_GtGetText::h_GetText("Ok"));
   const QString c_Failed = static_cast<QString>(C_GtGetText::h_GetText("Failed"));
   const QString c_SkippedPrevErr =
      static_cast<QString>(C_GtGetText::h_GetText("<b>Skipped</b> (error occurred during process)"));

   if ((oe_StateExists == eSUSEQ_STATE_NO_ERR) &&
       (oe_StateLoadedd == eSUSEQ_STATE_NO_ERR))
   {
      c_Text = c_Ok;
   }
   else if (((oe_StateExists == eSUSEQ_STATE_NOT_EXECUTED) || (oe_StateExists == eSUSEQ_STATE_NO_ERR)) &&
            (oe_StateLoadedd == eSUSEQ_STATE_NOT_EXECUTED))
   {
      c_Text = c_SkippedPrevErr;
   }
   else
   {
      c_Text = c_Failed;
   }

   return c_Text;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Returns the precondition state value in text form

   \param[in]       oq_FeatureSupported     Flag if the feature is supported
   \param[in]       oq_FeatureError         Flag if the feature is not supported but needed

   \return
   Text interpretation of feature support and error state
*/
//----------------------------------------------------------------------------------------------------------------------
QString C_SyvUpNodePropertiesDialog::mh_GetSuSequenceNodePreconditionString(const bool oq_FeatureSupported,
                                                                            const bool oq_FeatureError)
{
   QString c_Text;
   const QString c_Yes = static_cast<QString>(C_GtGetText::h_GetText("Yes"));
   const QString c_No = static_cast<QString>(C_GtGetText::h_GetText("No"));
   const QString c_NoButNeeded = static_cast<QString>(C_GtGetText::h_GetText("<b>No (Is needed!)</b>"));

   if (oq_FeatureSupported == true)
   {
      c_Text = c_Yes;
   }
   else if (oq_FeatureError == false)
   {
      c_Text = c_No;
   }
   else
   {
      c_Text = c_NoButNeeded;
   }

   return c_Text;
}
