//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Widget for showing a specific file associated to concrete datablock as part of a node

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
#include "C_PuiSdHandler.h"
#include "C_OgeWiCustomMessage.h"
#include "C_OgePopUpDialog.h"

#include "C_SyvUpPackageListNodeItemDatablockWidget.h"
#include "ui_C_SyvUpUpdatePackageListNodeItemWidget.h"

#include "C_PuiSvHandler.h"
#include "C_OsyHexFile.h"
#include "C_SyvUpHexFileView.h"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw_types;
using namespace stw_errors;
using namespace stw_opensyde_gui;
using namespace stw_opensyde_gui_elements;
using namespace stw_opensyde_gui_logic;
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

   \param[in]      ou32_ViewIndex      View index
   \param[in]      ou32_NodeIndex      Node index
   \param[in]      orc_DeviceName      Name of device for comparing with application block
   \param[in]      oq_FileBased        Flag if node is a file based device
   \param[in]      oq_StwFlashloader   Flag if node has not an openSYDE Flashloader but a STW Flashloader
   \param[in,out]  opc_Parent          Optional pointer to parent
*/
//----------------------------------------------------------------------------------------------------------------------
C_SyvUpPackageListNodeItemDatablockWidget::C_SyvUpPackageListNodeItemDatablockWidget(const uint32 ou32_ViewIndex,
                                                                                     const uint32 ou32_NodeIndex,
                                                                                     const QString & orc_DeviceName,
                                                                                     const bool oq_FileBased,
                                                                                     const bool oq_StwFlashloader,
                                                                                     QWidget * const opc_Parent) :
   C_SyvUpUpdatePackageListNodeItemWidget(ou32_ViewIndex, ou32_NodeIndex, orc_DeviceName, oq_FileBased,
                                          oq_StwFlashloader, opc_Parent),
   mc_ProjectName("NA"),
   mc_Version("")
{
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
bool C_SyvUpPackageListNodeItemDatablockWidget::IsFileIdentical(const QString & orc_AppName,
                                                                const QString & orc_AppVersion,
                                                                const QString & orc_AppBuildTime,
                                                                const QString & orc_AppBuildDate) const
{
   bool q_Return = C_SyvUpUpdatePackageListNodeItemWidget::IsFileIdentical(orc_AppName,
                                                                           orc_AppVersion,
                                                                           orc_AppBuildTime,
                                                                           orc_AppBuildDate);

   if (q_Return == true)
   {
      q_Return = (orc_AppName == this->mc_ProjectName) && (orc_AppVersion == this->mc_Version);
   }

   return q_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Shows file information

   Handle view HEX file info
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvUpPackageListNodeItemDatablockWidget::ViewFileInfo(void)
{
   if (this->GetAppFilePath().compare("") != 0)
   {
      QPointer<C_OgePopUpDialog> const c_New = new C_OgePopUpDialog(this, this);
      C_SyvUpHexFileView * const pc_InfoDialog =
         new C_SyvUpHexFileView(*c_New, this->GetAppAbsoluteFilePath());

      Q_UNUSED(pc_InfoDialog)

      //Resize
      c_New->SetSize(QSize(1000, 761));

      if (c_New->exec() == static_cast<sintn>(QDialog::Accepted))
      {
         //No confirmation
      }

      if (c_New != NULL)
      {
         c_New->HideOverlay();
      }
      //lint -e{429}  no memory leak because of the parent of pc_InfoDialog and the Qt memory management
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Returns the flag if it is possible to show file information

   Return false if path is empty.

   \retval   true   View file info can show information
   \retval   false  View file info can not show information
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_SyvUpPackageListNodeItemDatablockWidget::IsViewFileInfoPossible(void) const
{
   return this->GetAppFilePath().compare("") != 0;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Returns the flag if it is possible to show the user hint label

   \retval   true   The user hint label can be shown
   \retval   false  The user hint label can not be shown
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_SyvUpPackageListNodeItemDatablockWidget::IsUserHintPossible(void) const
{
   return true;
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
void C_SyvUpPackageListNodeItemDatablockWidget::m_LoadFileInformation(bool & orq_FileExists,
                                                                      bool & orq_FlashwareWarning,
                                                                      bool & orq_TriggerRemove)
{
   // For general file information (call before hex file load!)
   C_SyvUpUpdatePackageListNodeItemWidget::m_LoadFileInformation(orq_FileExists, orq_FlashwareWarning,
                                                                 orq_TriggerRemove);

   // Default text
   this->mc_ProjectName = C_GtGetText::h_GetText("NA");
   this->mc_Version = C_GtGetText::h_GetText("NA");

   if (orq_FileExists == true)
   {
      bool q_Reset = false;

      if (this->mq_FileBased == false)
      {
         C_OsyHexFile * const pc_HexFile = new C_OsyHexFile();

         // File information
         if (pc_HexFile->LoadFromFile(this->mc_AbsoluteFilePath.toStdString().c_str()) == stw_hex_file::NO_ERR)
         {
            stw_diag_lib::C_XFLECUInformation c_FileApplicationInfo;
            const sint32 s32_Result = pc_HexFile->ScanApplicationInformationBlockFromHexFile(c_FileApplicationInfo);

            if ((s32_Result == C_NO_ERR) || (s32_Result == C_WARN))
            {
               this->mc_AppDeviceType = c_FileApplicationInfo.GetDeviceID().Trim().UpperCase().c_str();
               if (s32_Result == C_WARN)
               {
                  // Hex file has multiple application blocks so information is ambiguous
                  this->mc_ProjectName = C_GtGetText::h_GetText("ambiguous");
                  this->mc_Version = C_GtGetText::h_GetText("ambiguous");
                  this->mpc_Ui->pc_LabelVersion->setText(C_GtGetText::h_GetText("?"));
                  this->mc_Date = C_GtGetText::h_GetText("ambiguous");
                  this->mc_Time = C_GtGetText::h_GetText("");
                  this->mq_AppInfoAmbiguous = true;
                  orq_FlashwareWarning = true;
               }
               else
               {
                  // Use information from hex file
                  this->mc_ProjectName = c_FileApplicationInfo.acn_ProjectName;
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
                  this->mc_Date = c_FileApplicationInfo.acn_Date;
                  // This is in format Mar 14 2015 whereas openSYDE date format is usually 14.03.2015, but we do not
                  // convert, because we want to show the "real" information like it would look like in other tools
                  this->mc_Time = c_FileApplicationInfo.acn_Time;
               }

               // No check with Alias necessary due to check with real device type defined with target integration
               if (QString::compare(this->mc_NodeDeviceType.trimmed(), this->mc_AppDeviceType,
                                    Qt::CaseInsensitive) != 0)
               {
                  if (this->mq_StwFlashloader == true)
                  {
                     // STW Flashloader must not match but show a warning text
                     orq_FlashwareWarning = true;
                  }
                  else
                  {
                     bool q_FileIsOk = false;
                     const C_OSCNode * const pc_Node = C_PuiSdHandler::h_GetInstance()->GetOSCNodeConst(
                        this->mu32_NodeIndex);
                     if ((pc_Node != NULL) && (pc_Node->pc_DeviceDefinition != NULL))
                     {
                        for (uint32 u32_ItName = 0UL;
                             u32_ItName < pc_Node->pc_DeviceDefinition->c_OtherAcceptedNames.size(); ++u32_ItName)
                        {
                           const QString c_AllowedDevice =
                              pc_Node->pc_DeviceDefinition->c_OtherAcceptedNames[u32_ItName].Trim().UpperCase().c_str();
                           if (QString::compare(c_AllowedDevice, this->mc_AppDeviceType, Qt::CaseInsensitive) == 0)
                           {
                              q_FileIsOk = true;
                           }
                        }
                     }
                     if (q_FileIsOk == false)
                     {
                        // openSYDE nodes must use HEX files with matching device IDs
                        C_OgeWiCustomMessage c_Message(this, C_OgeWiCustomMessage::E_Type::eERROR);
                        c_Message.SetHeading(C_GtGetText::h_GetText("Update Package Configuration"));
                        c_Message.SetDescription(C_GtGetText::h_GetText("Device type of selected HEX file does not "
                                                                        "match the node type."));
                        c_Message.SetDetails(
                           QString(C_GtGetText::h_GetText("Device type of %1 does not match node type %2."))
                           .arg(this->mc_AppDeviceType.trimmed(), this->mc_NodeDeviceType.trimmed()));
                        c_Message.SetCustomMinHeight(230, 250);
                        c_Message.Execute();
                        q_Reset = true;
                     }
                  }
               }
            }
            else
            {
               // HEX file application block information is invalid
               C_OgeWiCustomMessage c_Message(this, C_OgeWiCustomMessage::E_Type::eERROR);
               c_Message.SetHeading(C_GtGetText::h_GetText("Update Package Configuration"));
               if (s32_Result == C_OVERFLOW)
               {
                  c_Message.SetDescription(C_GtGetText::h_GetText("HEX file has multiple application information "
                                                                  "blocks with non-equal device names!"));
                  c_Message.SetCustomMinHeight(180, 180);
               }
               else
               {
                  c_Message.SetDescription(C_GtGetText::h_GetText("HEX file has no application information block!"));
                  c_Message.SetCustomMinHeight(180, 180);
               }
               c_Message.Execute();
               q_Reset = true;
            }
         }
         else
         {
            const QString c_Details = QString(C_GtGetText::h_GetText("File path: %1")).arg(this->mc_AbsoluteFilePath);
            // Invalid file
            C_OgeWiCustomMessage c_Message(this, C_OgeWiCustomMessage::E_Type::eERROR);
            c_Message.SetHeading(C_GtGetText::h_GetText("Update Package configuration"));
            c_Message.SetDescription(C_GtGetText::h_GetText("File is not a valid HEX file!"));
            c_Message.SetDetails(c_Details);
            c_Message.SetCustomMinHeight(180, 250);
            c_Message.Execute();

            q_Reset = true;
         }

         delete pc_HexFile;
      }

      if (q_Reset == true)
      {
         orq_FileExists = false;

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
         tgl_assert(C_PuiSvHandler::h_GetInstance()->SetNodeUpdateInformationPath(this->mu32_ViewIndex,
                                                                                  this->mu32_NodeIndex,
                                                                                  this->mu32_Number,
                                                                                  this->mc_FilePath,
                                                                                  C_PuiSvNodeUpdate::
                                                                                  eFTP_DATA_BLOCK) == C_NO_ERR);
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Creates and returns the content specific tool tip content

   \return
   Tool tip content
*/
//----------------------------------------------------------------------------------------------------------------------
QString C_SyvUpPackageListNodeItemDatablockWidget::m_CreateToolTipContent(void) const
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
      c_Content = C_SyvUpUpdatePackageListNodeItemWidget::m_CreateToolTipContent();
   }

   return c_Content;
}
