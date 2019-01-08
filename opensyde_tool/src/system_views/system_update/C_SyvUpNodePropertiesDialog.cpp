//-----------------------------------------------------------------------------
/*!
   \internal
   \file
   \brief       Dialog for system update node properties view (implementation)

   Dialog for system update node properties view.

   \implementation
   project     openSYDE
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     19.02.2018  STW/M.Echtler
   \endimplementation
*/
//-----------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------- */
#include "C_GtGetText.h"
#include "C_PuiSdHandler.h"
#include "C_OgeWiCustomMessage.h"

#include "C_SyvUpNodePropertiesDialog.h"
#include "ui_C_SyvUpNodePropertiesDialog.h"

/* -- Used Namespaces ------------------------------------------------------ */
using namespace stw_types;
using namespace stw_diag_lib;
using namespace stw_opensyde_gui;
using namespace stw_opensyde_core;
using namespace stw_opensyde_gui_logic;
using namespace stw_opensyde_gui_elements;

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

   \param[in,out] orc_Parent                  Reference to parent
   \param[in]     ou32_NodeIndex              Node index
   \param[in]     oq_UpdateFailed             Flag if update somehow failed
   \param[in]     ou32_FailedApplicationIndex Optional info about which application did fail

   \created     19.02.2018  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
C_SyvUpNodePropertiesDialog::C_SyvUpNodePropertiesDialog(stw_opensyde_gui_elements::C_OgePopUpDialog & orc_Parent,
                                                         const uint32 ou32_NodeIndex, const bool oq_UpdateFailed,
                                                         const uint32 ou32_FailedApplicationIndex) :
   QWidget(&orc_Parent),
   mpc_Ui(new Ui::C_SyvUpNodePropertiesDialog),
   mrc_ParentDialog(orc_Parent),
   mu32_NodeIndex(ou32_NodeIndex),
   me_Status(C_SyvUtil::eI_UNKNOWN),
   mpc_FileInfos(NULL),
   mpc_STWDevice(NULL),
   mpc_OSYDevice(NULL),
   mq_UpdateFailed(oq_UpdateFailed),
   mq_Discarded(false),
   mu32_FailedApplicationIndex(ou32_FailedApplicationIndex)
{
   const C_OSCNode * const pc_Node = C_PuiSdHandler::h_GetInstance()->GetOSCNodeConst(this->mu32_NodeIndex);

   mpc_Ui->setupUi(this);

   InitStaticNames();

   //Start with defined tab
   this->mpc_Ui->pc_TabWidget->setCurrentIndex(0);

   //Deactivate text for icon
   this->mpc_Ui->pc_LabelIcon->setText("");

   //Deactivate text edit interaction
   this->mpc_Ui->pc_TextEditDataBlocks->setReadOnly(true);
   this->mpc_Ui->pc_TextEditFlash->setReadOnly(true);

   // register the widget for showing
   this->mrc_ParentDialog.SetWidget(this);

   // set main title
   if (pc_Node != NULL)
   {
      this->mrc_ParentDialog.SetTitle(pc_Node->c_Properties.c_Name.c_str());
   }
   this->mrc_ParentDialog.SetSubTitle(C_GtGetText::h_GetText("Device Status Information"));

   // connects
   connect(this->mpc_Ui->pc_PushButtonOk, &QPushButton::clicked, this, &C_SyvUpNodePropertiesDialog::m_OkClicked);
   connect(this->mpc_Ui->pc_PushButtonDiscard, &QPushButton::clicked, this, &C_SyvUpNodePropertiesDialog::m_OnDiscard);
}

//-----------------------------------------------------------------------------
/*!
   \brief   Default destructor

   Clean up.

   \created     19.02.2018  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
C_SyvUpNodePropertiesDialog::~C_SyvUpNodePropertiesDialog(void)
{
   delete mpc_Ui;
   //lint -e{1740} Never took ownership of mpc_FileInfos, mpc_STWDevice, mpc_OSYDevice
}

//-----------------------------------------------------------------------------
/*!
   \brief   Initialize all displayed static names

   \created     20.02.2018  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_SyvUpNodePropertiesDialog::InitStaticNames(void) const
{
   this->mpc_Ui->pc_TabWidget->setTabText(0, C_GtGetText::h_GetText("Data Blocks Information"));
   this->mpc_Ui->pc_TabWidget->setTabText(1, C_GtGetText::h_GetText("Flashloader Information"));
   this->mpc_Ui->pc_LabelDetails->setText(C_GtGetText::h_GetText("Details"));
   this->mpc_Ui->pc_LabelHeadingFlash->setText(C_GtGetText::h_GetText("Details"));
   this->mpc_Ui->pc_PushButtonDiscard->setText(C_GtGetText::h_GetText("Discard device application info"));
   this->mpc_Ui->pc_PushButtonDiscard->SetToolTipInformation(C_GtGetText::h_GetText(
                                                                "Discard device application info"),
                                                             C_GtGetText::h_GetText(
                                                                "Option to allow flashing of already up to date nodes"),
                                                             C_NagToolTip::eDEFAULT);
}

//-----------------------------------------------------------------------------
/*!
   \brief   short description of function

   \param[in] oe_Status     Node status
   \param[in] opc_FileInfos File information
   \param[in] opc_STWDevice STW device information
   \param[in] opc_OSYDevice OSY device information

   \created     18.06.2018  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_SyvUpNodePropertiesDialog::SetStatus(const C_SyvUtil::E_NodeUpdateInitialStatus oe_Status,
                                            const std::vector<C_XFLECUInformation> * const opc_FileInfos,
                                            const C_OSCSuSequences::C_XflDeviceInformation * const opc_STWDevice,
                                            const C_OSCSuSequences::C_OsyDeviceInformation * const opc_OSYDevice)
{
   this->me_Status = oe_Status;
   this->mpc_FileInfos = opc_FileInfos;
   this->mpc_STWDevice = opc_STWDevice;
   this->mpc_OSYDevice = opc_OSYDevice;
   //Handle initial no info state
   if ((this->mpc_FileInfos != NULL) && (this->mpc_FileInfos->size() > 0UL))
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

//-----------------------------------------------------------------------------
/*!
   \brief   Get overall status

   \return
   Current overall status

   \created     22.06.2018  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
C_SyvUtil::E_NodeUpdateInitialStatus C_SyvUpNodePropertiesDialog::GetStatus(void) const
{
   return this->me_Status;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Get STW device status

   \return
   Current STW device status

   \created     22.06.2018  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
const C_OSCSuSequences::C_XflDeviceInformation * C_SyvUpNodePropertiesDialog::GetSTWDevice(void) const
{
   return this->mpc_STWDevice;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Get OSY device status

   \return
   Current OSY device status

   \created     22.06.2018  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
const C_OSCSuSequences::C_OsyDeviceInformation * C_SyvUpNodePropertiesDialog::GetOSYDevice(void) const
{
   return this->mpc_OSYDevice;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Overwritten key press event slot

   Here: Handle specific enter key cases

   \param[in,out] opc_KeyEvent Event identification and information

   \created     19.02.2018  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
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

//-----------------------------------------------------------------------------
/*!
   \brief   Get discard status

   \return
   Discard status

   \created     25.06.2018  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
bool C_SyvUpNodePropertiesDialog::GetDiscardedStatus(void) const
{
   return mq_Discarded;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Slot of Ok button click

   \created     19.02.2018  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_SyvUpNodePropertiesDialog::m_OkClicked(void)
{
   this->mrc_ParentDialog.accept();
}

//-----------------------------------------------------------------------------
/*!
   \brief   Init current status

   \created     20.02.2018  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_SyvUpNodePropertiesDialog::m_InitStatus(void) const
{
   QIcon c_Icon;

   switch (this->me_Status)
   {
   case C_SyvUtil::eI_APPLICATION_MATCH:
      c_Icon = QIcon("://images/system_views/IconOk.svg");
      this->mpc_Ui->pc_LabelDescription->setText(C_GtGetText::h_GetText("Node is up to date."));
      break;
   case C_SyvUtil::eI_TO_BE_UPDATED:
      c_Icon = QIcon("://images/system_views/IconWarning.svg");
      this->mpc_Ui->pc_LabelDescription->setText(C_GtGetText::h_GetText("There are deviations. Update required."));
      break;
   case C_SyvUtil::eI_NO_INFO_SUPPORTED:
      c_Icon = QIcon("://images/system_views/UpdateUnknown.svg");
      this->mpc_Ui->pc_LabelDescription->setText(C_GtGetText::h_GetText(
                                                    "Status request not supported. Update required."));
      break;
   case C_SyvUtil::eI_NO_RESPONSE:
      c_Icon = QIcon("://images/Error_iconV2.svg");
      this->mpc_Ui->pc_LabelDescription->setText(C_GtGetText::h_GetText("Node not found. Check connection!"));
      break;
   default:
      c_Icon = QIcon("");
      this->mpc_Ui->pc_LabelDescription->setText("");
      break;
   }
   //Apply default icon size
   this->mpc_Ui->pc_LabelIcon->setPixmap(c_Icon.pixmap(64, 64));
}

//-----------------------------------------------------------------------------
/*!
   \brief   Initialize data block table

   \created     21.02.2018  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_SyvUpNodePropertiesDialog::m_InitDataBlockTable(void) const
{
   const C_OSCNode * const pc_Node = C_PuiSdHandler::h_GetInstance()->GetOSCNodeConst(this->mu32_NodeIndex);
   QString c_Content = "<html><body>";

   //Each application
   if (((this->mpc_FileInfos != NULL) && (pc_Node != NULL)) &&
       (pc_Node->c_Applications.size() == this->mpc_FileInfos->size()))
   {
      for (uint32 u32_ItFile = 0; u32_ItFile < this->mpc_FileInfos->size(); ++u32_ItFile)
      {
         const C_XFLECUInformation & rc_FileInfo = this->mpc_FileInfos->at(u32_ItFile);
         const C_OSCNodeApplication & rc_Application = pc_Node->c_Applications[u32_ItFile];

         const QString c_Column1TagStart = QString("<td style=\"padding: 0 9px 0 18px;\">");
         const QString c_ColumnContentTagStart = QString(
            "<td align=\"center\" valign=\"middle\" style=\"padding: 0 9px 0 9px;\">");
         const QString c_FileProjectName = rc_FileInfo.acn_ProjectName;
         const QString c_FileVersion = rc_FileInfo.acn_ProjectVersion;
         const QString c_FileBuild = QString(rc_FileInfo.acn_Date) + " " + rc_FileInfo.acn_Time;
         QString c_DeviceProjectName = C_GtGetText::h_GetText("<b>Missing</b>");
         QString c_DeviceVersion = C_GtGetText::h_GetText("<b>Missing</b>");
         QString c_DeviceBuild = C_GtGetText::h_GetText("<b>Missing</b>");
         QString c_DeviceValid = C_GtGetText::h_GetText("<b>Missing</b>");
         bool q_Missing = true;
         bool q_Match = true;

         //Check
         if (this->me_Status == C_SyvUtil::eI_APPLICATION_MATCH)
         {
            //Assuming known application status
            c_DeviceProjectName = c_FileProjectName;
            c_DeviceVersion = c_FileVersion;
            c_DeviceBuild = c_FileBuild;
            if (this->mpc_OSYDevice != NULL)
            {
               c_DeviceValid = C_GtGetText::h_GetText("Yes");
            }
            else
            {
               c_DeviceValid = C_GtGetText::h_GetText("Not available");
            }
            q_Missing = false;
         }
         else if ((this->mq_UpdateFailed == true) && (u32_ItFile < this->mu32_FailedApplicationIndex))
         {
            //Assuming known application status
            c_DeviceProjectName = c_FileProjectName;
            c_DeviceVersion = c_FileVersion;
            c_DeviceBuild = c_FileBuild;
            if (this->mpc_STWDevice != NULL)
            {
               c_DeviceValid = C_GtGetText::h_GetText("Yes");
            }
            else
            {
               c_DeviceValid = C_GtGetText::h_GetText("Not available");
            }
            q_Missing = false;
         }
         else if ((this->mq_UpdateFailed == true) && (u32_ItFile == this->mu32_FailedApplicationIndex))
         {
            //Assuming known application status
            q_Missing = true;
         }
         else
         {
            if (this->mpc_STWDevice != NULL)
            {
               for (sint32 s32_ItDeviceInfoBlock = 0;
                    s32_ItDeviceInfoBlock < this->mpc_STWDevice->c_BasicInformation.c_DeviceInfoBlocks.GetLength();
                    ++s32_ItDeviceInfoBlock)
               {
                  const C_XFLECUInformation & rc_STWDeviceInfo =
                     this->mpc_STWDevice->c_BasicInformation.c_DeviceInfoBlocks[s32_ItDeviceInfoBlock];
                  //Search for name match
                  if (c_FileProjectName.compare(rc_STWDeviceInfo.acn_ProjectName) == 0)
                  {
                     //Signal found
                     q_Missing = false;
                     //Apply
                     c_DeviceProjectName = rc_STWDeviceInfo.acn_ProjectName;
                     c_DeviceVersion = rc_STWDeviceInfo.acn_ProjectVersion;
                     c_DeviceBuild = QString(rc_STWDeviceInfo.acn_Date) + " " + rc_STWDeviceInfo.acn_Time;
                     //Highlighting
                     if (c_FileProjectName.compare(c_DeviceProjectName) != 0)
                     {
                        c_DeviceProjectName = "<b>" + c_DeviceProjectName + "</b>";
                        q_Match = false;
                     }
                     if (c_FileVersion.compare(c_DeviceVersion) != 0)
                     {
                        c_DeviceVersion = "<b>" + c_DeviceVersion + "</b>";
                        q_Match = false;
                     }
                     if (c_FileBuild.compare(c_DeviceBuild) != 0)
                     {
                        c_DeviceBuild = "<b>" + c_DeviceBuild + "</b>";
                        q_Match = false;
                     }

                     // Valid flag is not available for STW Flashloader
                     c_DeviceValid = C_GtGetText::h_GetText("Not available");
                  }
               }
            }
            else if (this->mpc_OSYDevice != NULL)
            {
               //Skip first application because this is the flashloader (OSY ONLY!)
               for (uint32 u32_ItOsyApplication = 1;
                    u32_ItOsyApplication < this->mpc_OSYDevice->c_Applications.size();
                    ++u32_ItOsyApplication)
               {
                  const C_OSCProtocolDriverOsy::C_FlashBlockInfo & rc_OsyDeviceInfo =
                     this->mpc_OSYDevice->c_Applications[u32_ItOsyApplication];
                  //Search for name match
                  if (c_FileProjectName.compare(rc_OsyDeviceInfo.c_ApplicationName.c_str()) == 0)
                  {
                     //Signal found
                     q_Missing = false;
                     //Apply
                     c_DeviceProjectName = rc_OsyDeviceInfo.c_ApplicationName.c_str();
                     c_DeviceVersion = rc_OsyDeviceInfo.c_ApplicationVersion.c_str();
                     c_DeviceBuild = (rc_OsyDeviceInfo.c_BuildDate + " " + rc_OsyDeviceInfo.c_BuildTime).c_str();
                     //Highlighting
                     if (c_FileProjectName != c_DeviceProjectName)
                     {
                        c_DeviceProjectName = "<b>" + c_DeviceProjectName + "</b>";
                        q_Match = false;
                     }
                     if (c_FileVersion != c_DeviceVersion)
                     {
                        c_DeviceVersion = "<b>" + c_DeviceVersion + "</b>";
                        q_Match = false;
                     }
                     if (c_FileBuild != c_DeviceBuild)
                     {
                        c_DeviceBuild = "<b>" + c_DeviceBuild + "</b>";
                        q_Match = false;
                     }

                     if (rc_OsyDeviceInfo.u8_SignatureValid == 0U)
                     {
                        c_DeviceValid = C_GtGetText::h_GetText("Yes");
                     }
                     else
                     {
                        c_DeviceValid = C_GtGetText::h_GetText("<b>No</b>");
                        q_Match = false;
                     }
                     break;
                  }
               }
            }
            else
            {
               //Unexpected
            }
         }

         //Heading
         c_Content += "<h3>";
         c_Content += "<table>";
         c_Content += "<tr>";
         c_Content += "<td valign=\"middle\">";
         c_Content += QString("#%1 - %2 ").arg(u32_ItFile + 1).arg(rc_Application.c_Name.c_str());
         c_Content += "</td>";
         c_Content += "<td valign=\"middle\">";
         //Image
         if (q_Missing == true)
         {
            c_Content += "<img src=\"://images/system_views/IconWarning.svg\""
                         "alt=\"Application unknown\"";
         }
         else
         {
            if (q_Match == true)
            {
               c_Content += "<img src=\"://images/system_views/IconOk.svg\""
                            "alt=\"Application match\"";
            }
            else
            {
               c_Content += "<img src=\"://images/system_views/IconWarning.svg\""
                            "alt=\"Application deviation\"";
            }
         }
         c_Content += "style=\"vertical-align: middle;\"/>";
         c_Content += "</td>";
         c_Content += "</tr>";
         c_Content += "</table>";
         c_Content += "</h3>";
         c_Content += "<p/>";

         //Content
         c_Content += "<table style=\"width:100%;\">";
         c_Content += "<tr>";
         c_Content += c_Column1TagStart + "</td>";
         c_Content += c_ColumnContentTagStart + C_GtGetText::h_GetText("Update Package") + "</td>";
         c_Content += c_ColumnContentTagStart + C_GtGetText::h_GetText("Device Status") + "</td>";
         c_Content += "</tr>";
         c_Content += "<tr>";
         c_Content += c_Column1TagStart + C_GtGetText::h_GetText("Project Name:") + "</td>";
         c_Content += c_ColumnContentTagStart + c_FileProjectName + "</td>";
         c_Content += c_ColumnContentTagStart + c_DeviceProjectName + "</td>";
         c_Content += "</tr>";
         c_Content += "<tr>";
         c_Content += c_Column1TagStart + C_GtGetText::h_GetText("Version:") + "</td>";
         c_Content += c_ColumnContentTagStart + c_FileVersion + "</td>";
         c_Content += c_ColumnContentTagStart + c_DeviceVersion + "</td>";
         c_Content += "</tr>";
         c_Content += "<tr>";
         c_Content += c_Column1TagStart + C_GtGetText::h_GetText("Timestamp:") + "</td>";
         c_Content += c_ColumnContentTagStart + c_FileBuild + "</td>";
         c_Content += c_ColumnContentTagStart + c_DeviceBuild + "</td>";
         c_Content += "</tr>";
         c_Content += "<tr>";
         c_Content += c_Column1TagStart + C_GtGetText::h_GetText("Signature Valid:") + "</td>";
         c_Content += c_ColumnContentTagStart + "" + "</td>";
         c_Content += c_ColumnContentTagStart + c_DeviceValid + "</td>";
         c_Content += "</tr>";
         c_Content += "</table>";
      }
   }

   c_Content += "</body></html>";
   this->mpc_Ui->pc_TextEditDataBlocks->setHtml(c_Content);
}

//-----------------------------------------------------------------------------
/*!
   \brief   Initialize flashloader information table

   \created     21.02.2018  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_SyvUpNodePropertiesDialog::m_InitFlashloaderTable(void) const
{
   QString c_Content = "<html><body>";

   stw_scl::C_SCLStringList c_List;

   if (this->mpc_OSYDevice != NULL)
   {
      C_OSCSuSequences::h_OpenSydeFlashloaderInformationToText(*this->mpc_OSYDevice, c_List);
   }
   else if (this->mpc_STWDevice != NULL)
   {
      C_OSCSuSequences::h_StwFlashloaderInformationToText(*this->mpc_STWDevice, c_List);
   }
   else
   {
      //Unexpected
   }
   c_Content += "<p>";
   for (sint32 s32_ItString = 0; s32_ItString < c_List.Strings.GetLength(); ++s32_ItString)
   {
      c_Content += QString(c_List.Strings[s32_ItString].c_str()) + "<br/>";
   }
   c_Content += "</p>";

   c_Content += "</body></html>";
   this->mpc_Ui->pc_TextEditFlash->setHtml(c_Content);
}

//-----------------------------------------------------------------------------
/*!
   \brief   Handle discard trigger

   \created     22.06.2018  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_SyvUpNodePropertiesDialog::m_OnDiscard(void)
{
   //Confirm
   C_OgeWiCustomMessage c_Message(this, C_OgeWiCustomMessage::E_Type::eQUESTION);

   C_OgeWiCustomMessage::E_Outputs e_Output;
   c_Message.SetHeading(C_GtGetText::h_GetText("Discarding application information"));
   c_Message.SetDescription(C_GtGetText::h_GetText(
                               "Do you really want to discard all read application information, so that all "
                               "applications will be flashed to this device again?\nUse-case: Force openSYDE "
                               "to update this device."));
   e_Output = c_Message.Execute();
   switch (e_Output)
   {
   case C_OgeWiCustomMessage::eYES:
      //Trigger changes
      this->me_Status = C_SyvUtil::eI_TO_BE_UPDATED;
      this->mpc_STWDevice = NULL;
      this->mpc_OSYDevice = NULL;
      this->mq_Discarded = true;
      m_ReInitStatus();
      break;
   default:
      //Abort
      break;
   }
}

//-----------------------------------------------------------------------------
/*!
   \brief   Handle status update

   \created     22.06.2018  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_SyvUpNodePropertiesDialog::m_ReInitStatus(void) const
{
   m_InitStatus();
   m_InitDataBlockTable();
}
