//-----------------------------------------------------------------------------
/*!
   \internal
   \file
   \brief       Offers system view update specific visualization and functionality of a node. (implementation)

   \implementation
   project     openSYDE
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     19.06.2017  STW/B.Bayer
   \endimplementation
*/
//-----------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------- */
#include "precomp_headers.h"

#include <QFileInfo>
#include <QGraphicsView>

#include "C_Uti.h"
#include "gitypes.h"
#include "stwerrors.h"
#include "constants.h"
#include "C_GtGetText.h"
#include "C_PuiProject.h"
#include "C_PuiSdHandler.h"
#include "C_PuiSvHandler.h"
#include "C_OgePopUpDialog.h"
#include "C_GiSvNodeSyvUpdate.h"
#include "C_OSCLoggingHandler.h"
#include "TGLUtils.h"
#include "C_ImpUtil.h"

/* -- Used Namespaces ------------------------------------------------------ */
using namespace stw_types;
using namespace stw_errors;
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

   \param[in]       ou32_ViewIndex       Index of system view
   \param[in]       ors32_NodeIndex      Index of data element in system view
   \param[in]       oru64_ID             Unique ID
   \param[in]       orf64_Width          Width of node
   \param[in]       orf64_Height         Height of node
   \param[in,out]   opc_Parent           Optional pointer to parent

   \created     19.06.2017  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
C_GiSvNodeSyvUpdate::C_GiSvNodeSyvUpdate(const uint32 ou32_ViewIndex, const sint32 & ors32_NodeIndex,
                                         const uint64 & oru64_ID, const float64 & orf64_Width,
                                         const float64 & orf64_Height, QGraphicsItem * const opc_Parent) :
   //lint -e{1938}  static const is guaranteed preinitialized before main
   C_GiSvNodeSyvBase(ou32_ViewIndex, ors32_NodeIndex, oru64_ID, orf64_Width, orf64_Height, opc_Parent),
   mq_UpdateConnected(false),
   mq_UpdateInProgress(false),
   me_InitialStatus(C_SyvUtil::eI_UNKNOWN),
   me_UpdateStatus(eU_UNKNOWN),
   mq_UpdateFailed(false),
   mu32_FailedApplicationIndex(0),
   mpc_STWDevice(NULL),
   mpc_OSYDevice(NULL),
   mpc_IconTopLeft(NULL),
   mpc_IconBottom(NULL),
   mpc_InfoDialog(NULL)
{
   m_InitIcons();
   CheckThirdParty();
}

//-----------------------------------------------------------------------------
/*!
   \brief   default destructor

   Clean up.

   \created     19.06.2017  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
C_GiSvNodeSyvUpdate::~C_GiSvNodeSyvUpdate()
{
   if (this->mpc_IconTopLeft != NULL)
   {
      delete (this->mpc_IconTopLeft);
   }
   if (this->mpc_IconBottom != NULL)
   {
      delete (this->mpc_IconBottom);
   }
   if (this->mpc_STWDevice != NULL)
   {
      delete (mpc_STWDevice);
   }
   if (this->mpc_OSYDevice != NULL)
   {
      delete (mpc_OSYDevice);
   }
   if (this->mpc_InfoDialog != NULL)
   {
      delete (mpc_InfoDialog);
   }
}

//-----------------------------------------------------------------------------
/*!
   \brief   Sets the node connected state

   \param[in]     oq_Connected   Flag if connected or not

   \created     27.02.2018  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_GiSvNodeSyvUpdate::SetViewConnected(const bool oq_Connected)
{
   C_GiSvNodeSyvBase::SetViewConnected(oq_Connected);
   if ((oq_Connected == true) && (this->me_InitialStatus == C_SyvUtil::eI_UPDATE_DISABLED))
   {
      this->mpc_IconBottom->setVisible(true);
      m_UpdateIcons();
   }
}

//-----------------------------------------------------------------------------
/*!
   \brief   Set connection change

   \param[in] oq_Active Flag if connected

   \created     13.02.2018  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_GiSvNodeSyvUpdate::SetConnected(const bool oq_Active)
{
   if (this->mq_ViewConnected == true)
   {
      this->mq_UpdateConnected = oq_Active;
      this->mpc_IconTopLeft->setVisible(oq_Active);
      CheckThirdParty();
      if (this->me_InitialStatus == C_SyvUtil::eI_UPDATE_DISABLED)
      {
         this->mpc_IconBottom->setVisible(true);
      }
      //Reset status
      if (oq_Active == false)
      {
         if (this->me_InitialStatus != C_SyvUtil::eI_UPDATE_DISABLED)
         {
            this->me_InitialStatus = C_SyvUtil::eI_UNKNOWN;
         }
         if (this->me_UpdateStatus != eU_UPDATE_DISABLED)
         {
            this->me_UpdateStatus = eU_UNKNOWN;
         }
         //Discard info
         this->mc_FileInfos.clear();
         if (this->mpc_STWDevice != NULL)
         {
            delete (mpc_STWDevice);
            this->mpc_STWDevice = NULL;
         }
         if (this->mpc_OSYDevice != NULL)
         {
            delete (mpc_OSYDevice);
            this->mpc_OSYDevice = NULL;
         }
      }
      m_UpdateIcons();
   }
}

//-----------------------------------------------------------------------------
/*!
   \brief   Signal if update in progress

   \param[in] oq_Active Flag if update in progress

   \created     13.02.2018  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_GiSvNodeSyvUpdate::SetUpdating(const bool oq_Active)
{
   if (this->mq_ViewConnected == true)
   {
      this->mq_UpdateInProgress = oq_Active;
      if ((this->me_InitialStatus != C_SyvUtil::eI_UPDATE_DISABLED) &&
          (this->me_InitialStatus != C_SyvUtil::eI_NO_RESPONSE))
      {
         //Update status
         if ((this->me_UpdateStatus == eU_UPDATE_SUCCESS) &&
             (this->me_InitialStatus != C_SyvUtil::eI_NO_INFO_SUPPORTED))
         {
            //Set last updated node to success
            if (oq_Active == false)
            {
               this->me_InitialStatus = C_SyvUtil::eI_APPLICATION_MATCH;
            }
            this->me_UpdateStatus = eU_UP_TO_DATE;
            //Update dialog if necessary
            if (this->mpc_InfoDialog != NULL)
            {
               this->mpc_InfoDialog->SetStatus(this->me_InitialStatus, &this->mc_FileInfos, this->mpc_STWDevice,
                                               this->mpc_OSYDevice);
            }
         }
         this->mpc_IconBottom->setVisible(oq_Active);
      }
      m_UpdateIcons();
   }
}

//-----------------------------------------------------------------------------
/*!
   \brief   Set update waiting state

   False: Waiting
   True: In progress
   False after true once: Update success

   \param[in] oq_Active                   Flag if update in progress
   \param[in] oq_Aborted                  Flag if action was aborted
   \param[in] ou32_FailedApplicationIndex If aborted and currently updating this is the currently updated application

   \created     13.02.2018  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_GiSvNodeSyvUpdate::SetNodeUpdateInProgress(const bool oq_Active, const bool oq_Aborted,
                                                  const uint32 ou32_FailedApplicationIndex)
{
   if (oq_Active == true)
   {
      this->mq_UpdateFailed = false;
      this->me_UpdateStatus = eU_UPDATING;
   }
   else
   {
      switch (this->me_UpdateStatus)
      {
      case eU_UNKNOWN:
         this->me_UpdateStatus = eU_WAITING;
         break;
      case eU_UPDATING:
         if (this->me_InitialStatus != C_SyvUtil::eI_NO_INFO_SUPPORTED)
         {
            //If user aborted we assume the application has to be updated, else the application should be finished
            if (oq_Aborted == false)
            {
               this->me_InitialStatus = C_SyvUtil::eI_APPLICATION_MATCH;
               this->me_UpdateStatus = eU_UPDATE_SUCCESS;
               //Update dialog if necessary
               if (this->mpc_InfoDialog != NULL)
               {
                  this->mpc_InfoDialog->SetStatus(this->me_InitialStatus, &this->mc_FileInfos, this->mpc_STWDevice,
                                                  this->mpc_OSYDevice);
               }
            }
            else
            {
               this->me_InitialStatus = C_SyvUtil::eI_TO_BE_UPDATED;
               this->me_UpdateStatus = eU_UNKNOWN;
               this->mq_UpdateFailed = true;
               this->mu32_FailedApplicationIndex = ou32_FailedApplicationIndex;
            }
         }
         else
         {
            //For file based devices we never change the status
            this->me_UpdateStatus = eU_UNKNOWN;
         }
         break;
      default:
         //No change
         break;
      }
   }
   m_UpdateIcons();
}

//-----------------------------------------------------------------------------
/*!
   \brief   Check third party node

   \created     12.02.2018  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_GiSvNodeSyvUpdate::CheckThirdParty(void)
{
   if (this->ms32_Index >= 0)
   {
      const C_OSCNode * const pc_Node =
         C_PuiSdHandler::h_GetInstance()->GetOSCNodeConst(static_cast<uint32>(this->ms32_Index));
      if (pc_Node != NULL)
      {
         if (pc_Node->c_Applications.size() == 0)
         {
            this->me_InitialStatus = C_SyvUtil::eI_UPDATE_DISABLED;
            this->me_UpdateStatus = eU_UPDATE_DISABLED;
         }
         else
         {
            const C_OSCDeviceDefinition * const pc_Device = pc_Node->pc_DeviceDefinition;
            tgl_assert(pc_Device != NULL);
            if (pc_Device != NULL)
            {
               if (((pc_Device->q_FlashloaderStwCan == false) && (pc_Device->q_FlashloaderOpenSydeCan == false)) &&
                   (pc_Device->q_FlashloaderOpenSydeEthernet == false))
               {
                  this->me_InitialStatus = C_SyvUtil::eI_UPDATE_DISABLED;
                  this->me_UpdateStatus = eU_UPDATE_DISABLED;
               }
               else
               {
                  //Check file based
                  if (pc_Device->q_FlashloaderOpenSydeIsFileBased == true)
                  {
                     this->me_InitialStatus = C_SyvUtil::eI_NO_INFO_SUPPORTED;
                  }
               }
            }
         }
      }
   }
}

//-----------------------------------------------------------------------------
/*!
   \brief   Apply no response state

   \created     12.02.2018  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_GiSvNodeSyvUpdate::SetNoResponse(void)
{
   this->me_InitialStatus = C_SyvUtil::eI_NO_RESPONSE;
   this->mpc_IconBottom->setVisible(true);
   m_UpdateIcons();
}

//-----------------------------------------------------------------------------
/*!
   \brief   Show info

   \created     19.02.2018  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_GiSvNodeSyvUpdate::ShowInfo(void)
{
   //Open if active in view, valid node index, updateable, connected in update screen, and not while updating
   if (((((this->mq_ViewConnected == true) && (this->ms32_Index >= 0)) &&
         (this->me_InitialStatus != C_SyvUtil::eI_UPDATE_DISABLED)) &&
        (this->mq_UpdateConnected == true)) && (this->mq_UpdateInProgress == false))
   {
      QGraphicsView * const pc_View = this->scene()->views().at(0);
      QPointer<C_OgePopUpDialog> const c_New = new C_OgePopUpDialog(pc_View, pc_View);
      this->mpc_InfoDialog = new C_SyvUpNodePropertiesDialog(*c_New, static_cast<uint32>(this->ms32_Index),
                                                             this->mq_UpdateFailed, this->mu32_FailedApplicationIndex);

      this->mpc_InfoDialog->SetStatus(this->me_InitialStatus, &this->mc_FileInfos, this->mpc_STWDevice,
                                      this->mpc_OSYDevice);

      //Resize
      c_New->SetSize(QSize(1000, 795));

      if (c_New->exec() == static_cast<sintn>(QDialog::Accepted))
      {
         //No confirmation
      }

      //Signal
      //Check if valid infos
      if (this->mc_FileInfos.size() > 0UL)
      {
         if (this->mpc_InfoDialog->GetDiscardedStatus() == true)
         {
            Q_EMIT this->SigDiscardInfo(static_cast<uint32>(this->ms32_Index));
         }
      }

      //Get new info
      this->me_InitialStatus = this->mpc_InfoDialog->GetStatus();
      this->mpc_STWDevice = this->mpc_InfoDialog->GetSTWDevice();
      this->mpc_OSYDevice = this->mpc_InfoDialog->GetOSYDevice();

      //Trigger icon update if necessary
      m_UpdateIcons();

      this->mpc_InfoDialog = NULL;
      if (c_New != NULL)
      {
         c_New->HideOverlay();
      }
   }
}

//-----------------------------------------------------------------------------
/*!
   \brief   Update package status

   \param[in] orc_DeviceApplicationInfos Device application information

   \created     12.02.2018  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_GiSvNodeSyvUpdate::UpdateInitialPackageStatus(const C_SyvUpDeviceInfo & orc_DeviceApplicationInfos)
{
   if (this->me_InitialStatus != C_SyvUtil::eI_UPDATE_DISABLED)

   {
      //Clean up current info
      this->mc_FileInfos.clear();
      if (this->mpc_STWDevice != NULL)
      {
         delete (mpc_STWDevice);
         this->mpc_STWDevice = NULL;
      }
      if (this->mpc_OSYDevice != NULL)
      {
         delete (mpc_OSYDevice);
         this->mpc_OSYDevice = NULL;
      }

      //COPY current info
      if (orc_DeviceApplicationInfos.pc_OSYDevice != NULL)
      {
         this->mpc_OSYDevice =
            new C_OSCSuSequences::C_OsyDeviceInformation(*orc_DeviceApplicationInfos.pc_OSYDevice);
      }
      else if (orc_DeviceApplicationInfos.pc_STWDevice != NULL)
      {
         this->mpc_STWDevice =
            new C_OSCSuSequences::C_XflDeviceInformation(*orc_DeviceApplicationInfos.pc_STWDevice);
      }
      else
      {
         //Unexpected
      }
      if (this->me_InitialStatus != C_SyvUtil::eI_NO_INFO_SUPPORTED)
      {
         const C_PuiSvData * const pc_View = C_PuiSvHandler::h_GetInstance()->GetView(this->mu32_ViewIndex);

         this->me_InitialStatus = C_SyvUtil::eI_UNKNOWN;
         if ((pc_View != NULL) && (this->ms32_Index >= 0))
         {
            const C_OSCNode * const pc_Node =
               C_PuiSdHandler::h_GetInstance()->GetOSCNodeConst(static_cast<uint32>(this->ms32_Index));
            const C_PuiSvNodeUpdate * const pc_UpdateInformation =
               pc_View->GetNodeUpdateInformation(static_cast<uint32>(this->ms32_Index));
            if (((pc_UpdateInformation != NULL) && (pc_Node != NULL)) &&
                (pc_Node->c_Applications.size() == pc_UpdateInformation->GetApplicationPaths().size()))

            {
               std::vector<QString> c_FinalApplicationPaths;
               const std::vector<QString> & rc_ApplicationPaths = pc_UpdateInformation->GetApplicationPaths();
               c_FinalApplicationPaths.reserve(pc_Node->c_Applications.size());
               //Parse all application paths
               for (uint32 u32_ItApplication = 0; u32_ItApplication < rc_ApplicationPaths.size(); ++u32_ItApplication)
               {
                  const QString & rc_ViewApplicationPath = rc_ApplicationPaths[u32_ItApplication];
                  if (rc_ViewApplicationPath.compare("") == 0)
                  {
                     const C_OSCNodeApplication & rc_Application = pc_Node->c_Applications[u32_ItApplication];
                     c_FinalApplicationPaths.push_back(
                        C_Uti::h_ConcatPathIfNecessary(
                           C_ImpUtil::h_GetAbsoluteProjectPath(rc_Application.c_ProjectPath.c_str()),
                           rc_Application.c_ResultPath.c_str()));
                  }
                  else
                  {
                     c_FinalApplicationPaths.push_back(rc_ViewApplicationPath);
                  }
               }

               //Read all applications
               this->mc_FileInfos.reserve(pc_Node->c_Applications.size());
               for (uint32 u32_ItFile = 0; u32_ItFile < pc_Node->c_Applications.size(); ++u32_ItFile)
               {
                  const QString & rc_ApplicationPath = c_FinalApplicationPaths[u32_ItFile];
                  const QFileInfo c_FileInfo(rc_ApplicationPath);
                  C_OsyHexFile c_HexFile;
                  uint32 u32_Result;
                  stw_scl::C_SCLString c_Path;
                  if (c_FileInfo.isAbsolute() == true)
                  {
                     c_Path = rc_ApplicationPath.toStdString().c_str();
                  }
                  else
                  {
                     c_Path =
                        (C_PuiProject::h_GetInstance()->GetFolderPath() + '/' +
                         rc_ApplicationPath).toStdString().c_str();
                  }
                  //Application is relative to project files
                  u32_Result = c_HexFile.LoadFromFile(c_Path.c_str());
                  if (u32_Result == stw_hex_file::NO_ERR)
                  {
                     stw_diag_lib::C_XFLECUInformation c_FileApplicationInfo;
                     if (c_HexFile.ScanApplicationInformationBlockFromHexFile(c_FileApplicationInfo) == C_NO_ERR)
                     {
                        this->mc_FileInfos.push_back(c_FileApplicationInfo);
                     }
                  }
                  else
                  {
                     const stw_scl::C_SCLString c_Text = "Could not open HEX file \"" +
                                                         c_Path + "\" Details: " +
                                                         c_HexFile.ErrorCodeToErrorText(u32_Result);
                     osc_write_log_error("Preparing Flashloader Information", c_Text);
                  }
               }

               if ((pc_Node->c_Properties.e_FlashLoader == C_OSCNodeProperties::eFL_OPEN_SYDE) &&
                   (orc_DeviceApplicationInfos.pc_OSYDevice != NULL))
               {
                  bool q_AllApplicationsMatch = true;
                  for (uint32 u32_ItFile = 0; u32_ItFile < this->mc_FileInfos.size(); ++u32_ItFile)
                  {
                     bool q_Found = false;
                     const stw_diag_lib::C_XFLECUInformation & rc_FileInfo = this->mc_FileInfos[u32_ItFile];
                     //Skip first application because this is the flashloader (OSY ONLY!)
                     for (uint32 u32_ItApplication = 1;
                          u32_ItApplication < orc_DeviceApplicationInfos.pc_OSYDevice->c_Applications.size();
                          ++u32_ItApplication)
                     {
                        const C_OSCProtocolDriverOsy::C_FlashBlockInfo & rc_OsyDeviceInfo =
                           orc_DeviceApplicationInfos.pc_OSYDevice->c_Applications[u32_ItApplication];
                        //Search for match
                        if (((rc_OsyDeviceInfo.c_ApplicationName == rc_FileInfo.acn_ProjectName) &&
                             (rc_OsyDeviceInfo.c_ApplicationVersion == rc_FileInfo.acn_ProjectVersion)) &&
                            ((rc_OsyDeviceInfo.c_BuildDate + rc_OsyDeviceInfo.c_BuildTime) ==
                             (QString(rc_FileInfo.acn_Date) + rc_FileInfo.acn_Time).toStdString().c_str()))
                        {
                           if (rc_OsyDeviceInfo.u8_SignatureValid == 0U) //0 == valid
                           {
                              q_Found = true;
                              break;
                           }
                        }
                     }
                     if (q_Found == false)
                     {
                        q_AllApplicationsMatch = false;
                     }
                  }
                  if (q_AllApplicationsMatch == true)
                  {
                     this->me_InitialStatus = C_SyvUtil::eI_APPLICATION_MATCH;
                  }
                  else
                  {
                     this->me_InitialStatus = C_SyvUtil::eI_TO_BE_UPDATED;
                  }
               }
               else if ((pc_Node->c_Properties.e_FlashLoader == C_OSCNodeProperties::eFL_STW) &&
                        (orc_DeviceApplicationInfos.pc_STWDevice != NULL))
               {
                  bool q_AllApplicationsMatch = true;
                  for (uint32 u32_ItFile = 0; u32_ItFile < this->mc_FileInfos.size(); ++u32_ItFile)
                  {
                     bool q_Found = false;
                     const stw_diag_lib::C_XFLECUInformation & rc_FileInfo = this->mc_FileInfos[u32_ItFile];
                     for (sint32 s32_ItDeviceInfoBlock = 0;
                          s32_ItDeviceInfoBlock <
                          orc_DeviceApplicationInfos.pc_STWDevice->c_BasicInformation.c_DeviceInfoBlocks.GetLength();
                          ++s32_ItDeviceInfoBlock)
                     {
                        const stw_diag_lib::C_XFLECUInformation & rc_STWDeviceInfo =
                           orc_DeviceApplicationInfos.pc_STWDevice->c_BasicInformation.c_DeviceInfoBlocks[
                              s32_ItDeviceInfoBlock];
                        //Search for match
                        if (((QString(rc_STWDeviceInfo.acn_ProjectName).compare(rc_FileInfo.acn_ProjectName) == 0) &&
                             (QString(rc_STWDeviceInfo.acn_ProjectVersion).compare(rc_FileInfo.acn_ProjectVersion) ==
                              0)) &&
                            (QString((QString(rc_STWDeviceInfo.acn_Date) +
                                      rc_STWDeviceInfo.acn_Time)).compare((QString(rc_FileInfo.acn_Date) +
                                                                           rc_FileInfo.acn_Time)) == 0))
                        {
                           q_Found = true;
                        }
                     }
                     if (q_Found == false)
                     {
                        q_AllApplicationsMatch = false;
                     }
                  }
                  if (q_AllApplicationsMatch == true)
                  {
                     this->me_InitialStatus = C_SyvUtil::eI_APPLICATION_MATCH;
                  }
                  else
                  {
                     this->me_InitialStatus = C_SyvUtil::eI_TO_BE_UPDATED;
                  }
               }
               else
               {
                  //Should not happen
                  this->me_InitialStatus = C_SyvUtil::eI_TO_BE_UPDATED;
               }
            }
         }
      }
   }
   m_UpdateIcons();
   //Update dialog if necessary
   if (this->mpc_InfoDialog != NULL)
   {
      this->mpc_InfoDialog->SetStatus(this->me_InitialStatus, &this->mc_FileInfos, this->mpc_STWDevice,
                                      this->mpc_OSYDevice);
   }
}

//-----------------------------------------------------------------------------
/*!
   \brief   Check if in state no response and active

   \return
   True  No response
   False Any other state

   \created     21.03.2018  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
bool C_GiSvNodeSyvUpdate::HasNoResponseAndIsActive(void) const
{
   bool q_Retval;

   if ((this->mq_ViewConnected == true) && (this->me_InitialStatus == C_SyvUtil::eI_NO_RESPONSE))
   {
      q_Retval = true;
   }
   else
   {
      q_Retval = false;
   }
   return q_Retval;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Check if STW device and active

   Warning: requires previous third party check

   \return
   True  STW device
   False Third party

   \created     21.03.2018  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
bool C_GiSvNodeSyvUpdate::IsStwDeviceAndActive(void) const
{
   bool q_Retval;

   if (this->mq_ViewConnected == true)
   {
      if ((this->me_InitialStatus == C_SyvUtil::eI_UPDATE_DISABLED) || (this->me_UpdateStatus == eU_UPDATE_DISABLED))
      {
         q_Retval = false;
      }
      else
      {
         q_Retval = true;
      }
   }
   else
   {
      q_Retval = false;
   }
   return q_Retval;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Generate hint

   \created     14.02.2018  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_GiSvNodeSyvUpdate::GenerateHint(void)
{
   if (this->mq_ErrorIconHovered == true)
   {
      C_GiSvNodeSyvBase::GenerateHint();
   }
   else
   {
      const C_OSCNode * const pc_Node =
         C_PuiSdHandler::h_GetInstance()->GetOSCNodeConst(static_cast<uint32>(this->GetIndex()));

      this->SetDefaultToolTipType(C_NagToolTip::eDEFAULT);

      if (pc_Node != NULL)
      {
         this->SetDefaultToolTipHeading(pc_Node->c_Properties.c_Name.c_str());
      }
      else
      {
         this->SetDefaultToolTipHeading(C_GtGetText::h_GetText("Unknown"));
      }
      if (this->mq_ViewConnected == true)
      {
         if (this->me_InitialStatus == C_SyvUtil::eI_UPDATE_DISABLED)
         {
            this->SetDefaultToolTipContent(C_GtGetText::h_GetText("Node update disabled."
                                                                  "\nPossible reasons: "
                                                                  "\n- There are no active Data Blocks declared"
                                                                  "\n- Update setting is disabled on connected node interface"));
         }
         else
         {
            if (this->mq_UpdateConnected == true)
            {
               QString c_Text;
               if (this->mq_UpdateInProgress == false)
               {
                  switch (this->me_InitialStatus)
                  {
                  case C_SyvUtil::eI_APPLICATION_MATCH:
                     c_Text = C_GtGetText::h_GetText("Node is up to date.");
                     break;
                  case C_SyvUtil::eI_NO_INFO_SUPPORTED:
                     c_Text = C_GtGetText::h_GetText("Status request not supported. Update required.");
                     break;
                  case C_SyvUtil::eI_TO_BE_UPDATED:
                     c_Text = C_GtGetText::h_GetText("There are deviations. Update required.");
                     break;
                  case C_SyvUtil::eI_NO_RESPONSE:
                     c_Text = C_GtGetText::h_GetText("Node not found. Check connection!");
                     break;
                  default:
                     break;
                  }
               }
               else
               {
                  switch (this->me_InitialStatus)
                  {
                  case C_SyvUtil::eI_APPLICATION_MATCH:
                     c_Text = C_GtGetText::h_GetText("Node is up to date.");
                     break;
                  case C_SyvUtil::eI_NO_INFO_SUPPORTED:
                     c_Text = C_GtGetText::h_GetText("Status request not supported.");
                     break;
                  case C_SyvUtil::eI_TO_BE_UPDATED:
                     c_Text = C_GtGetText::h_GetText("There are deviations.");
                     break;
                  case C_SyvUtil::eI_NO_RESPONSE:
                     c_Text = C_GtGetText::h_GetText("Node not found. Check connection!");
                     break;
                  default:
                     break;
                  }
                  switch (this->me_UpdateStatus)
                  {
                  case eU_UPDATING:
                     c_Text += C_GtGetText::h_GetText("\nUpdating...");
                     break;
                  case eU_WAITING:
                     c_Text += C_GtGetText::h_GetText("\nWaiting for update...");
                     break;
                  case eU_UP_TO_DATE:
                     break;
                  case eU_UPDATE_SUCCESS:
                     break;
                  case eU_UPDATE_DISABLED:
                     break;
                  default:
                     break;
                  }
               }
               this->SetDefaultToolTipContent(c_Text);
            }
            else
            {
               this->SetDefaultToolTipContent(C_GtGetText::h_GetText("Connect to System to get the node status."));
            }
         }
      }
      else
      {
         this->SetDefaultToolTipContent(C_GtGetText::h_GetText("Inactive node."));
      }
   }
}

//-----------------------------------------------------------------------------
/*!
   \brief   Prepare icons

   \created     13.02.2018  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_GiSvNodeSyvUpdate::m_InitIcons(void)
{
   const uint32 u32_ScaleCategory = m_GetScaleCategory();
   const sint32 s32_IconSize = this->m_GetIconSize();
   const QRectF c_Rect = this->m_GetBoundary()->boundingRect();
   const sint32 s32_BorderSize = 9;
   const sint32 s32_IconOffset = std::max(1L, s32_IconSize / 4L);
   sint32 s32_PosX;
   sint32 s32_PosY;

   //Create + placement
   s32_PosX = (static_cast<sint32>(c_Rect.width()) - s32_IconSize) - s32_BorderSize;
   s32_PosY = s32_BorderSize;
   this->mpc_IconTopLeft = new C_GiInfo(QRect(s32_PosX, s32_PosY, s32_IconSize, s32_IconSize));
   s32_PosX = s32_BorderSize;
   s32_PosY = ((static_cast<sint32>(c_Rect.height()) - s32_IconSize) - s32_BorderSize) - s32_IconOffset;
   this->mpc_IconBottom =
      new C_GiInfo(QRect(static_cast<sintn>(s32_PosX), static_cast<sintn>(s32_PosY),
                         static_cast<sintn>(static_cast<sint32>(c_Rect.width()) - (2L * s32_BorderSize)),
                         static_cast<sintn>(s32_IconSize + s32_IconOffset)));

   //Colors
   this->mpc_IconBottom->SetBackgroundColor(Qt::transparent);
   this->mpc_IconBottom->SetTextColor(mc_STYLE_GUIDE_COLOR_8, static_cast<sint32>(Qt::AlignLeft));
   this->mpc_IconBottom->SetText("Unexpected");

   //Font
   if (u32_ScaleCategory == mhu32_ScaleCategory1)
   {
      this->mpc_IconBottom->SetTextFont(mc_STYLE_GUIDE_FONT_REGULAR_15);
   }
   else if (u32_ScaleCategory == mhu32_ScaleCategory2)
   {
      this->mpc_IconBottom->SetTextFont(mc_STYLE_GUIDE_FONT_REGULAR_20);
   }
   else if (u32_ScaleCategory == mhu32_ScaleCategory3)
   {
      this->mpc_IconBottom->SetTextFont(mc_STYLE_GUIDE_FONT_REGULAR_25);
   }
   else if (u32_ScaleCategory == mhu32_ScaleCategory4)
   {
      this->mpc_IconBottom->SetTextFont(mc_STYLE_GUIDE_FONT_REGULAR_30);
   }
   else if (u32_ScaleCategory == mhu32_ScaleCategory5)
   {
      this->mpc_IconBottom->SetTextFont(mc_STYLE_GUIDE_FONT_REGULAR_35);
   }
   else if (u32_ScaleCategory == mhu32_ScaleCategory6)
   {
      this->mpc_IconBottom->SetTextFont(mc_STYLE_GUIDE_FONT_REGULAR_40);
   }
   else
   {
      this->mpc_IconBottom->SetTextFont(mc_STYLE_GUIDE_FONT_REGULAR_10);
   }

   //Icons
   this->mpc_IconTopLeft->SetIconSize(s32_IconSize);
   this->mpc_IconBottom->SetIconSize(s32_IconSize);

   //Defaults
   this->mpc_IconTopLeft->setZValue(mf64_ZORDER_INIT_NODE + 1.0);
   this->mpc_IconBottom->setZValue(mf64_ZORDER_INIT_NODE + 1.0);

   //Initial disable
   this->mpc_IconTopLeft->setVisible(false);
   this->mpc_IconBottom->setVisible(false);

   //Add
   this->addToGroup(this->mpc_IconBottom);
   this->addToGroup(this->mpc_IconTopLeft);
}

//-----------------------------------------------------------------------------
/*!
   \brief   Update icons

   \created     13.02.2018  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_GiSvNodeSyvUpdate::m_UpdateIcons(void)
{
   switch (this->me_InitialStatus)
   {
   case C_SyvUtil::eI_APPLICATION_MATCH:
      this->mpc_IconTopLeft->SetSvg("://images/system_views/IconOk.svg");
      break;
   case C_SyvUtil::eI_TO_BE_UPDATED:
      this->mpc_IconTopLeft->SetSvg("://images/system_views/IconWarning.svg");
      break;
   case C_SyvUtil::eI_NO_INFO_SUPPORTED:
      this->mpc_IconTopLeft->SetSvg("://images/system_views/UpdateUnknown.svg");
      break;
   default:
      this->mpc_IconTopLeft->SetSvg("");
      break;
   }
   this->mpc_IconTopLeft->update();

   //Should not happen while connected
   if (this->me_InitialStatus != C_SyvUtil::eI_NO_RESPONSE)
   {
      switch (this->me_UpdateStatus)
      {
      case eU_UP_TO_DATE:
         this->mpc_IconBottom->SetText(C_GtGetText::h_GetText("Up-to-date"));
         this->mpc_IconBottom->SetSvg("://images/system_views/UpdateUpToDate.svg");
         break;
      case eU_UPDATE_SUCCESS:
         this->mpc_IconBottom->SetText(C_GtGetText::h_GetText("Successfully updated"));
         this->mpc_IconBottom->SetSvg("://images/system_views/UpdateUpToDate.svg");
         break;
      case eU_UPDATE_DISABLED:
         this->mpc_IconBottom->SetText(C_GtGetText::h_GetText("Update disabled"));
         this->mpc_IconBottom->SetSvg("://images/system_views/UpdateNotUpdateable.svg");
         break;
      case eU_UPDATING:
         this->mpc_IconBottom->SetText(C_GtGetText::h_GetText("Updating..."));
         this->mpc_IconBottom->SetSvg("://images/system_views/UpdateInProgress.svg");
         break;
      case eU_WAITING:
         this->mpc_IconBottom->SetText(C_GtGetText::h_GetText("Waiting..."));
         this->mpc_IconBottom->SetSvg("://images/system_views/UpdateWaiting.svg");
         break;
      default:
         this->mpc_IconBottom->SetText("");
         this->mpc_IconBottom->SetSvg("");
         break;
      }
      this->mpc_IconBottom->SetBackgroundColor(Qt::transparent);
      this->mpc_IconBottom->SetTextColor(mc_STYLE_GUIDE_COLOR_8, static_cast<sint32>(Qt::AlignLeft));
   }
   else
   {
      this->mpc_IconBottom->SetText(C_GtGetText::h_GetText("No response"));
      this->mpc_IconBottom->SetSvg("://images/system_views/UpdateNoResponse.svg");

      this->mpc_IconBottom->SetBackgroundColor(mc_STYLE_GUIDE_COLOR_24);
      this->mpc_IconBottom->SetTextColor(Qt::white, static_cast<sint32>(Qt::AlignCenter));
   }
   this->mpc_IconBottom->update();
}
