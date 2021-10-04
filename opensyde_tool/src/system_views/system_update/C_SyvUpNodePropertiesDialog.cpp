//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Dialog for system update node properties view (implementation)

   Dialog for system update node properties view.

   \copyright   Copyright 2018 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "TGLUtils.h"

#include "TGLUtils.h"
#include "C_GtGetText.h"
#include "C_PuiSdHandler.h"
#include "C_PuiSdUtil.h"
#include "C_OgeWiCustomMessage.h"

#include "C_SyvUpNodePropertiesDialog.h"
#include "ui_C_SyvUpNodePropertiesDialog.h"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw_tgl;
using namespace stw_types;
using namespace stw_diag_lib;
using namespace stw_opensyde_gui;
using namespace stw_opensyde_core;
using namespace stw_opensyde_gui_logic;
using namespace stw_opensyde_gui_elements;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */
const QString C_SyvUpNodePropertiesDialog::mhc_HtmlCellTagStartMain = "<td style=\"padding: 0px 40px 10px 0px;\">";
const QString C_SyvUpNodePropertiesDialog::mhc_HtmlCellTagStartContent =
   "<td valign=\"middle\" style=\"padding: 0px 30px 0px 0px;\">";
const QString C_SyvUpNodePropertiesDialog::mhc_HtmlCellTagStartStatus = "<td style=\"padding: 0px 5px 0px 0px;\">";

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
C_SyvUpNodePropertiesDialog::C_SyvUpNodePropertiesDialog(stw_opensyde_gui_elements::C_OgePopUpDialog & orc_Parent,
                                                         const uint32 ou32_NodeIndex,
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
   this->mpc_Ui->pc_TabWidget->setTabText(1, C_GtGetText::h_GetText("Flashloader Information"));
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
   m_ReInitStatus();
   m_InitFlashloaderTable();
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
void C_SyvUpNodePropertiesDialog::CopySTWDeviceInfo(C_GiSvNodeData & orc_NodeData) const
{
   this->mc_NodeData.CopySTWDeviceInfo(orc_NodeData);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Copy OSY device info

   \param[in,out]  orc_NodeData  Node data
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvUpNodePropertiesDialog::CopyOSYDeviceInfo(C_GiSvNodeData & orc_NodeData) const
{
   this->mc_NodeData.CopyOSYDeviceInfo(orc_NodeData);
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
   if ((opc_KeyEvent->key() == static_cast<sintn>(Qt::Key_Enter)) ||
       (opc_KeyEvent->key() == static_cast<sintn>(Qt::Key_Return)))
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
         this->mpc_Ui->pc_LabelDescription->setText(C_GtGetText::h_GetText("Update required."));
         break;
      case C_SyvUtil::eI_NO_RESPONSE:
         c_Icon = QIcon("://images/Error_iconV2.svg");
         this->mpc_Ui->pc_LabelDescription->setText(C_GtGetText::h_GetText("Node not found. Check connection!"));
         break;
      case C_SyvUtil::eI_UPDATE_DISABLED:
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
      this->mpc_Ui->pc_LabelDescription->setText(C_GtGetText::h_GetText("Requesting info ..."));
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
   const std::vector<uint32> c_NodeIndices = C_PuiSdHandler::h_GetInstance()->GetAllNodeGroupIndicesUsingNodeIndex(
      this->mu32_NodeIndex);

   tgl_assert(c_NodeIndices.size() == this->mc_NodeData.GetSubNodeCount());
   if (c_NodeIndices.size() == this->mc_NodeData.GetSubNodeCount())
   {
      for (uint32 u32_ItDevice = 0UL; u32_ItDevice < c_NodeIndices.size(); ++u32_ItDevice)
      {
         const C_OSCNode * const pc_Node =
            C_PuiSdHandler::h_GetInstance()->GetOSCNodeConst(c_NodeIndices[u32_ItDevice]);
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
                                                               QString & orc_Content, const C_OSCNode & orc_Node,
                                                               const bool oq_IsMultiDevice)
{
   QString c_NewContent;

   if ((orc_NodeInfo.GetValidStatus()) &&
       (orc_NodeInfo.GetInitialStatus() != C_SyvUtil::eI_NO_RESPONSE))
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
                  for (uint32 u32_ItFile = 0; u32_ItFile < orc_NodeInfo.GetHexFileInfosCount();
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
   C_SyvUpNodePropertiesDialog::mh_InitDataBlockTableOtherSectionForNode(orc_NodeInfo, orc_Content);
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
                                                               const C_OSCNode & orc_Node,
                                                               const uint32 ou32_ApplicationIndex,
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
   const C_OSCNodeApplication & rc_Application = orc_Node.c_Applications[ou32_ApplicationIndex];
   bool q_Missing = true;
   bool q_Match = true;
   const C_SyvUpDeviceInfo c_DeviceInfo = orc_NodeInfo.GetDeviceInfo();

   tgl_assert(pc_FileInfo != NULL);
   if (pc_FileInfo != NULL)
   {
      orc_FileProjectName = pc_FileInfo->acn_ProjectName;
      orc_FileVersion = pc_FileInfo->acn_ProjectVersion;
      orc_FileBuildDate = static_cast<QString>(pc_FileInfo->acn_Date) + " " +
                          pc_FileInfo->acn_Time;
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
         ((c_DeviceInfo.pc_OSYDevice != NULL) ||
          (c_DeviceInfo.pc_STWDevice != NULL))) &&
        (orc_NodeInfo.GetUpdateFailedStatus() == false)) &&
       (orc_NodeInfo.IsDeviceInfoDiscarded() == false))
   {
      //Assuming known application status
      orc_DeviceProjectName = orc_FileProjectName;
      orc_DeviceFileVersion = orc_FileVersion;
      orc_DeviceBuildDate = orc_FileBuildDate;
      if (c_DeviceInfo.pc_OSYDevice != NULL)
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
      if (c_DeviceInfo.pc_STWDevice != NULL)
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
      if (c_DeviceInfo.pc_STWDevice != NULL)
      {
         C_SyvUpNodePropertiesDialog::mh_ExtractSTWDeviceInformation(orc_NodeInfo, orc_FileProjectName,
                                                                     orc_DeviceProjectName,
                                                                     orc_FileVersion, orc_DeviceFileVersion,
                                                                     orc_FileBuildDate, orc_DeviceBuildDate,
                                                                     orc_DeviceValidStatus, q_Missing, q_Match);
      }
      else if (c_DeviceInfo.pc_OSYDevice != NULL)
      {
         C_SyvUpNodePropertiesDialog::mh_ExtractOSYDeviceInformation(orc_NodeInfo, orc_FileProjectName,
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
void C_SyvUpNodePropertiesDialog::mh_ExtractSTWDeviceInformation(const C_GiSvSubNodeData & orc_NodeInfo,
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

   for (sint32 s32_ItDeviceInfoBlock = 0;
        s32_ItDeviceInfoBlock <
        c_DeviceInfo.pc_STWDevice->c_BasicInformation.c_DeviceInfoBlocks.
        GetLength();
        ++s32_ItDeviceInfoBlock)
   {
      const C_XFLECUInformation & rc_STWDeviceInfo =
         c_DeviceInfo.pc_STWDevice->c_BasicInformation.c_DeviceInfoBlocks[
            s32_ItDeviceInfoBlock];
      //Search for name match
      if (orc_FileProjectName.compare(rc_STWDeviceInfo.acn_ProjectName) == 0)
      {
         //Signal found
         orq_MissingStatus = false;
         //Apply
         orc_DeviceProjectName = rc_STWDeviceInfo.acn_ProjectName;
         orc_DeviceFileVersion = rc_STWDeviceInfo.acn_ProjectVersion;
         orc_DeviceBuildDate = static_cast<QString>(rc_STWDeviceInfo.acn_Date) + " " +
                               rc_STWDeviceInfo.acn_Time;
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
void C_SyvUpNodePropertiesDialog::mh_ExtractOSYDeviceInformation(const C_GiSvSubNodeData & orc_NodeInfo,
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
   for (uint32 u32_ItOsyApplication = 1;
        u32_ItOsyApplication <
        c_DeviceInfo.pc_OSYDevice->c_Applications.size();
        ++u32_ItOsyApplication)
   {
      const C_OSCProtocolDriverOsy::C_FlashBlockInfo & rc_OsyDeviceInfo =
         c_DeviceInfo.pc_OSYDevice->c_Applications[u32_ItOsyApplication];
      //Search for name match
      if (orc_FileProjectName.compare(rc_OsyDeviceInfo.c_ApplicationName.c_str()) == 0)
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

   if (orc_FileProjectName != orc_DeviceProjectName)
   {
      orc_DeviceProjectName = "<b>" + orc_DeviceProjectName + "</b>";
      q_Match = false;
   }
   if (orc_FileVersion != orc_DeviceFileVersion)
   {
      orc_DeviceFileVersion = "<b>" + orc_DeviceFileVersion + "</b>";
      q_Match = false;
   }
   if (orc_FileBuildDate != orc_DeviceBuildDate)
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
      c_NewContent += C_SyvUpNodePropertiesDialog::mhc_HtmlCellTagStartContent;
      //lint -e{1946} Qt interface
      c_NewContent += QString("<u>") + C_GtGetText::h_GetText("Property") + "</u>";
      c_NewContent += "</td>";
      c_NewContent += C_SyvUpNodePropertiesDialog::mhc_HtmlCellTagStartContent;
      //lint -e{1946} Qt interface
      c_NewContent += QString("<u>") + C_GtGetText::h_GetText("Update Package") + "</u>";
      c_NewContent += "</td>";
      c_NewContent += C_SyvUpNodePropertiesDialog::mhc_HtmlCellTagStartContent;
      //lint -e{1946} Qt interface
      c_NewContent += QString("<u>") + C_GtGetText::h_GetText("Device Status") + "</u>";
      c_NewContent += "</td>";
      c_NewContent += "</tr>";
      c_NewContent += "<tr>";
      c_NewContent += C_SyvUpNodePropertiesDialog::mhc_HtmlCellTagStartContent;
      c_NewContent += C_GtGetText::h_GetText("Project Name");
      c_NewContent += "</td>";
      c_NewContent += C_SyvUpNodePropertiesDialog::mhc_HtmlCellTagStartContent;
      c_NewContent += orc_FileProjectName;
      c_NewContent += "</td>";
      c_NewContent += C_SyvUpNodePropertiesDialog::mhc_HtmlCellTagStartContent;
      c_NewContent += orc_DeviceProjectName;
      c_NewContent += "</td>";
      c_NewContent += "</tr>";
      c_NewContent += "<tr>";
      c_NewContent += C_SyvUpNodePropertiesDialog::mhc_HtmlCellTagStartContent;
      c_NewContent += C_GtGetText::h_GetText("Version");
      c_NewContent += "</td>";
      c_NewContent += C_SyvUpNodePropertiesDialog::mhc_HtmlCellTagStartContent;
      c_NewContent += orc_FileVersion;
      c_NewContent += "</td>";
      c_NewContent += C_SyvUpNodePropertiesDialog::mhc_HtmlCellTagStartContent;
      c_NewContent += orc_DeviceFileVersion;
      c_NewContent += "</td>";
      c_NewContent += "</tr>";
      c_NewContent += "<tr>";
      c_NewContent += C_SyvUpNodePropertiesDialog::mhc_HtmlCellTagStartContent;
      c_NewContent += C_GtGetText::h_GetText("Timestamp");
      c_NewContent += "</td>";
      c_NewContent += C_SyvUpNodePropertiesDialog::mhc_HtmlCellTagStartContent;
      c_NewContent += orc_FileBuildDate;
      c_NewContent += "</td>";
      c_NewContent += C_SyvUpNodePropertiesDialog::mhc_HtmlCellTagStartContent;
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
void C_SyvUpNodePropertiesDialog::mh_AppendApplicationForNode(const uint32 ou32_ApplicationIndex,
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
   c_NewContent += C_SyvUpNodePropertiesDialog::mhc_HtmlCellTagStartMain;
   c_NewContent += C_GtGetText::h_GetText("Status");
   c_NewContent += "</td>";
   c_NewContent += "<td><table><tr>";
   c_NewContent += C_SyvUpNodePropertiesDialog::mhc_HtmlCellTagStartStatus;
   c_NewContent += orc_ApplicationStateIcon + "</td><td>" +
                   orc_ApplicationState + "</td>";
   c_NewContent += "</tr></table></td>";
   c_NewContent += "</tr>";
   c_NewContent += "<tr>";
   c_NewContent += C_SyvUpNodePropertiesDialog::mhc_HtmlCellTagStartMain;
   c_NewContent += C_GtGetText::h_GetText("Signature");
   c_NewContent += "</td>";
   c_NewContent += "<td>" + orc_DeviceValidStatus + "</td>";
   c_NewContent += "</tr>";
   c_NewContent += "<tr>";
   c_NewContent += C_SyvUpNodePropertiesDialog::mhc_HtmlCellTagStartMain;
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
       (orc_NodeInfo.GetFileInfosCount() > 0UL))
   {
      c_NewContent += "<h4>";
      c_NewContent += C_GtGetText::h_GetText("Other Files");
      c_NewContent += "</h4>";
      c_NewContent += "<table><tr>";
      c_NewContent += C_SyvUpNodePropertiesDialog::mhc_HtmlCellTagStartMain;
      c_NewContent += C_GtGetText::h_GetText("Status");
      c_NewContent += "</td>";
      c_NewContent += "<td><table><tr>";
      c_NewContent += C_SyvUpNodePropertiesDialog::mhc_HtmlCellTagStartStatus;
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

   const std::vector<uint32> c_NodeIndices = C_PuiSdHandler::h_GetInstance()->GetAllNodeGroupIndicesUsingNodeIndex(
      this->mu32_NodeIndex);

   tgl_assert(c_NodeIndices.size() == this->mc_NodeData.GetSubNodeCount());
   if (c_NodeIndices.size() == this->mc_NodeData.GetSubNodeCount())
   {
      for (uint32 u32_ItDevice = 0UL; u32_ItDevice < c_NodeIndices.size(); ++u32_ItDevice)
      {
         const C_OSCNode * const pc_Node =
            C_PuiSdHandler::h_GetInstance()->GetOSCNodeConst(c_NodeIndices[u32_ItDevice]);
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
                                                                 QString & orc_Content, const C_OSCNode & orc_Node,
                                                                 const bool oq_IsMultiDevice)
{
   QString c_NewContent;
   const C_SyvUpDeviceInfo c_DeviceInfo = orc_NodeInfo.GetDeviceInfo();

   stw_scl::C_SCLStringList c_List;

   if (c_DeviceInfo.pc_OSYDevice != NULL)
   {
      C_OSCSuSequences::h_OpenSydeFlashloaderInformationToText(*c_DeviceInfo.pc_OSYDevice, c_List);
   }
   else if (c_DeviceInfo.pc_STWDevice != NULL)
   {
      C_OSCSuSequences::h_StwFlashloaderInformationToText(*c_DeviceInfo.pc_STWDevice, c_List);
   }
   else
   {
      //Unexpected
   }
   if (c_List.Strings.GetLength() > 1)
   {
      c_NewContent += "<p>";
      for (sint32 s32_ItString = 0; s32_ItString < c_List.Strings.GetLength(); ++s32_ItString)
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
                                                         const C_OSCNode & orc_Node, const bool oq_IsMultiDevice)
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
