//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Offers system view update specific visualization and functionality of a node. (implementation)

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.hpp"

#include <QFileInfo>
#include <QGraphicsView>

#include "C_Uti.hpp"
#include "gitypes.hpp"
#include "stwerrors.hpp"
#include "constants.hpp"
#include "C_GtGetText.hpp"
#include "C_PuiProject.hpp"
#include "C_PuiSdHandler.hpp"
#include "C_PuiSvHandler.hpp"
#include "C_PuiSdUtil.hpp"
#include "C_OgePopUpDialog.hpp"
#include "C_GiSvNodeSyvUpdate.hpp"
#include "C_OscLoggingHandler.hpp"
#include "TglUtils.hpp"
#include "C_PuiUtil.hpp"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw::errors;
using namespace stw::opensyde_gui;
using namespace stw::opensyde_core;
using namespace stw::opensyde_gui_logic;
using namespace stw::opensyde_gui_elements;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

/* -- Global Variables ---------------------------------------------------------------------------------------------- */

/* -- Module Global Variables --------------------------------------------------------------------------------------- */

/* -- Module Global Function Prototypes ----------------------------------------------------------------------------- */

/* -- Implementation ------------------------------------------------------------------------------------------------ */

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default constructor

   Set up GUI with all elements.

   \param[in]      ou32_ViewIndex   Index of system view
   \param[in]      ors32_NodeIndex  Index of data element in system view
   \param[in]      oru64_Id         Unique ID
   \param[in]      orf64_Width      Width of node
   \param[in]      orf64_Height     Height of node
   \param[in,out]  opc_Parent       Optional pointer to parent
*/
//----------------------------------------------------------------------------------------------------------------------
C_GiSvNodeSyvUpdate::C_GiSvNodeSyvUpdate(const uint32_t ou32_ViewIndex, const int32_t & ors32_NodeIndex,
                                         const uint64_t & oru64_Id, const float64_t & orf64_Width,
                                         const float64_t & orf64_Height, QGraphicsItem * const opc_Parent) :
   //lint -e{1938}  static const is guaranteed preinitialized before main
   C_GiSvNodeSyvBase(ou32_ViewIndex, ors32_NodeIndex, oru64_Id, orf64_Width, orf64_Height, opc_Parent),
   mpc_IconTopLeft(NULL),
   mpc_IconBottom(NULL),
   mpc_InfoDialog(NULL)
{
   tgl_assert(this->ms32_Index >= 0);
   if (this->ms32_Index >= 0)
   {
      this->mc_NodeData.Init(ou32_ViewIndex, static_cast<uint32_t>(ors32_NodeIndex));
   }

   m_InitIcons();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   default destructor

   Clean up.
*/
//----------------------------------------------------------------------------------------------------------------------
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
   if (this->mpc_InfoDialog != NULL)
   {
      delete (mpc_InfoDialog);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Sets the node connected state

   \param[in]  oq_Connected   Flag if connected or not
*/
//----------------------------------------------------------------------------------------------------------------------
void C_GiSvNodeSyvUpdate::SetViewConnected(const bool oq_Connected)
{
   C_GiSvNodeSyvBase::SetViewConnected(oq_Connected);
   UpdateIcons();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Signal if connect in progress

   \param[in]  oq_Active   Flag if connect in progress
*/
//----------------------------------------------------------------------------------------------------------------------
void C_GiSvNodeSyvUpdate::SetConnecting(const bool oq_Active)
{
   if (this->mq_ViewConnected == true)
   {
      this->mc_NodeData.SetConnecting(oq_Active);
      this->m_RefreshDialog();
      this->UpdateIcons();
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set connection change

   \param[in]  oq_Active   Flag if connected
*/
//----------------------------------------------------------------------------------------------------------------------
void C_GiSvNodeSyvUpdate::SetConnected(const bool oq_Active)
{
   if (this->mq_ViewConnected == true)
   {
      this->mc_NodeData.SetConnected(oq_Active);
      this->UpdateIcons();
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Signal if update in progress

   \param[in]  oq_Active   Flag if update in progress
*/
//----------------------------------------------------------------------------------------------------------------------
void C_GiSvNodeSyvUpdate::SetUpdating(const bool oq_Active)
{
   if (this->mq_ViewConnected == true)
   {
      this->mc_NodeData.SetUpdating(oq_Active);
      this->m_RefreshDialog();
      this->UpdateIcons();
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set update waiting state

   False: Waiting
   True: In progress
   False after true once: Update success

   \param[in]  oq_Active                     Flag if update in progress
   \param[in]  oq_Aborted                    Flag if action was aborted
   \param[in]  ou32_FailedApplicationIndex   If aborted and currently updating this is the currently updated application
   \param[in]  ou32_NodeIndex                Node index
*/
//----------------------------------------------------------------------------------------------------------------------
void C_GiSvNodeSyvUpdate::SetNodeUpdateInProgress(const bool oq_Active, const bool oq_Aborted,
                                                  const uint32_t ou32_FailedApplicationIndex,
                                                  const uint32_t ou32_NodeIndex)
{
   if (this->ms32_Index >= 0)
   {
      this->mc_NodeData.SetNodeUpdateInProgress(oq_Active, oq_Aborted, ou32_FailedApplicationIndex, ou32_NodeIndex,
                                                static_cast<uint32_t>(this->ms32_Index));
   }
   this->m_RefreshDialog();
   this->UpdateIcons();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Apply error state

   \param[in]  ou32_NodeIndex    Node index
*/
//----------------------------------------------------------------------------------------------------------------------
void C_GiSvNodeSyvUpdate::SetNodeError(const uint32_t ou32_NodeIndex)
{
   this->mc_NodeData.SetErrorState(ou32_NodeIndex);

   this->m_RefreshDialog();
   this->UpdateIcons();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Sets the connect state of all sub nodes

   \param[in]       orc_NodeStates                 Node connect states
   \param[in]       orc_NodePreconditionErrors     Node precondition error states
*/
//----------------------------------------------------------------------------------------------------------------------
void C_GiSvNodeSyvUpdate::SetNodeConnectStates(const std::vector<C_OscSuSequencesNodeConnectStates> & orc_NodeStates,
                                               const C_GiSvNodeData::C_GiSvNodeDataPreconditionErrors & orc_NodePreconditionErrors)
{
   this->mc_NodeData.SetNodeConnectStates(orc_NodeStates, orc_NodePreconditionErrors);
   this->m_RefreshDialog();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Sets the connect state of all sub nodes

   \param[in]       orc_NodeStates     Node connect states
*/
//----------------------------------------------------------------------------------------------------------------------
void C_GiSvNodeSyvUpdate::SetNodeUpdateStates(const std::vector<C_OscSuSequencesNodeUpdateStates> & orc_NodeStates)
{
   this->mc_NodeData.SetNodeUpdateStates(orc_NodeStates);
   this->m_RefreshDialog();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Show info
*/
//----------------------------------------------------------------------------------------------------------------------
void C_GiSvNodeSyvUpdate::ShowInfo(void)
{
   //Open if active in view and valid node index
   if ((this->ms32_Index >= 0) &&
       (this->IsActiveInView() == true) &&
       (this->HasNodeAnAvailableFlashloader() == true))
   {
      QGraphicsView * const pc_View = this->scene()->views().at(0);
      const QPointer<C_OgePopUpDialog> c_New = new C_OgePopUpDialog(pc_View->parentWidget(), pc_View->parentWidget());
      this->mpc_InfoDialog =
         new C_SyvUpNodePropertiesDialog(*c_New,  static_cast<uint32_t>(this->ms32_Index),
                                         this->mc_NodeData);

      this->m_RefreshDialog();
      //Resize
      c_New->SetSize(QSize(1000, 820));

      if (c_New->exec() == static_cast<int32_t>(QDialog::Accepted))
      {
         //No confirmation
      }

      //Signal
      //Check if valid infos
      if (this->mc_NodeData.IsThereAnyHexFileInformation())
      {
         const std::vector<uint32_t> c_NodeIndices =
            C_PuiSdHandler::h_GetInstance()->GetAllNodeGroupIndicesUsingNodeIndex(
               static_cast<uint32_t>(this->ms32_Index));
         this->mpc_InfoDialog->CopyDiscardedStatus(this->mc_NodeData);
         for (uint32_t u32_ItNode = 0UL; u32_ItNode < c_NodeIndices.size(); ++u32_ItNode)
         {
            const C_OscNode * const pc_Node =
               C_PuiSdHandler::h_GetInstance()->GetOscNodeConst(c_NodeIndices[u32_ItNode]);
            if ((pc_Node != NULL) && (pc_Node->u32_SubDeviceIndex < this->mc_NodeData.GetSubNodeCount()))
            {
               if (this->mc_NodeData.IsThereAnyHexFileInformationForDevice(pc_Node->u32_SubDeviceIndex) &&
                   this->mc_NodeData.IsDeviceInfoDiscarded(pc_Node->u32_SubDeviceIndex))
               {
                  Q_EMIT (this->SigDiscardInfo(c_NodeIndices[u32_ItNode]));
               }
            }
         }
      }

      //Get new info
      this->mpc_InfoDialog->CopyInitialStatus(this->mc_NodeData);
      this->mpc_InfoDialog->CopyUpdateStatus(this->mc_NodeData);
      this->mpc_InfoDialog->CopyStwDeviceInfo(this->mc_NodeData);
      this->mpc_InfoDialog->CopyOpenSydeDeviceInfo(this->mc_NodeData);

      //Trigger icon update if necessary
      UpdateIcons();

      this->mpc_InfoDialog = NULL;
      if (c_New != NULL)
      {
         c_New->HideOverlay();
         c_New->deleteLater();
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Update package status

   \param[in]  orc_DeviceApplicationInfos    Device application information
   \param[in]  ou32_NodeIndex                Node index of node where the application information belongs to
*/
//----------------------------------------------------------------------------------------------------------------------
void C_GiSvNodeSyvUpdate::UpdateInitialPackageStatus(const C_SyvUpDeviceInfo & orc_DeviceApplicationInfos,
                                                     const uint32_t ou32_NodeIndex)
{
   this->mc_NodeData.UpdateInitialPackageStatus(orc_DeviceApplicationInfos, ou32_NodeIndex);
   this->UpdateIcons();
   this->m_RefreshDialog();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Check if in state no response and active

   \return
   True  No response
   False Any other state
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_GiSvNodeSyvUpdate::HasNoResponseAndIsActive(void) const
{
   bool q_Retval;

   if ((this->mq_ViewConnected == true) && (this->mc_NodeData.GetOverallInitialState() == C_SyvUtil::eI_ERROR))
   {
      q_Retval = true;
   }
   else
   {
      q_Retval = false;
   }
   return q_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get all active STW device indices

   \return
   All active STW device indices
*/
//----------------------------------------------------------------------------------------------------------------------
std::vector<uint32_t> C_GiSvNodeSyvUpdate::GetAllActiveStwDeviceIndices() const
{
   std::vector<uint32_t> c_Retval;
   if (this->ms32_Index >= 0)
   {
      std::vector<uint8_t> c_NodeActiveFlags;
      const int32_t s32_Retval = C_PuiSvHandler::h_GetInstance()->GetNodeActiveFlagsWithSquadAdaptions(
         this->mu32_ViewIndex,
         c_NodeActiveFlags);

      if (s32_Retval == C_NO_ERR)
      {
         const std::vector<uint32_t> c_NodeIndices =
            C_PuiSdHandler::h_GetInstance()->GetAllNodeGroupIndicesUsingNodeIndex(
               static_cast<uint32_t>(this->ms32_Index));
         for (uint32_t u32_ItNode = 0UL; u32_ItNode < c_NodeIndices.size(); ++u32_ItNode)
         {
            if (c_NodeActiveFlags[c_NodeIndices[u32_ItNode]] == 1U)
            {
               if (this->mc_NodeData.GetStwDeviceInfoByNodeIndex(c_NodeIndices[u32_ItNode]))
               {
                  c_Retval.push_back(c_NodeIndices[u32_ItNode]);
               }
            }
         }
      }
   }
   return c_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Check if node is active in view

   \retval   True    Node is active check result
   \retval   False   Node is active check result
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_GiSvNodeSyvUpdate::IsActiveInView(void) const
{
   bool q_Retval;

   if (this->ms32_Index >= 0)
   {
      const C_PuiSvData * const pc_View = C_PuiSvHandler::h_GetInstance()->GetView(this->mu32_ViewIndex);
      if (pc_View != NULL)
      {
         q_Retval = pc_View->GetNodeStatusDisplayedAsActive(static_cast<uint32_t>(this->ms32_Index));
      }
      else
      {
         q_Retval = false;
      }
   }
   else
   {
      q_Retval = false;
   }
   return q_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Check if device has a flashloader available to communicate with

   \return
   True  STW device
   False Third party
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_GiSvNodeSyvUpdate::HasNodeAnAvailableFlashloader(void) const
{
   const bool q_Retval = this->mc_NodeData.HasNodeAnAvailableFlashloader();

   return q_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Update icons
*/
//----------------------------------------------------------------------------------------------------------------------
void C_GiSvNodeSyvUpdate::UpdateIcons(void)
{
   bool q_TopLeftVisible;

   if (this->mq_ViewConnected)
   {
      //Should not happen while connected
      if (this->mc_NodeData.GetOverallInitialState() != C_SyvUtil::eI_ERROR)
      {
         if (this->mc_NodeData.CheckAlwaysUpdate())
         {
            if (this->mc_NodeData.GetOverallUpdateState() == C_SyvUtil::eU_UPDATING)
            {
               this->mpc_IconTopLeft->SetGif("://images/system_views/UpdateAnimationInProgress.gif");
            }
            else
            {
               this->mpc_IconTopLeft->SetSvg("://images/system_views/IconUpdateWaiting.svg");
            }
            q_TopLeftVisible = this->mc_NodeData.GetUpdateConnectedStatus();
         }
         else
         {
            if (this->mc_NodeData.CheckUpdateDisabledState())
            {
               this->mpc_IconTopLeft->SetSvg("://images/system_views/IconUpdateStatusDisabled.svg");
               q_TopLeftVisible = true;
            }
            else
            {
               switch (this->mc_NodeData.GetOverallUpdateState())
               {
               case C_SyvUtil::eU_UP_TO_DATE:
                  this->mpc_IconTopLeft->SetSvg("://images/system_views/IconUpdateSuccess.svg");
                  break;
               case C_SyvUtil::eU_UPDATE_SUCCESS:
                  this->mpc_IconTopLeft->SetSvg("://images/system_views/IconUpdateSuccess.svg");
                  break;
               case C_SyvUtil::eU_UPDATE_DISABLED:
                  this->mpc_IconTopLeft->SetSvg("://images/system_views/IconUpdateStatusDisabled.svg");
                  break;
               case C_SyvUtil::eU_UPDATING:
                  this->mpc_IconTopLeft->SetGif("://images/system_views/UpdateAnimationInProgress.gif");
                  break;
               case C_SyvUtil::eU_WAITING:
                  this->mpc_IconTopLeft->SetSvg("://images/system_views/IconUpdateWaiting.svg");
                  break;
               case C_SyvUtil::eU_UNKNOWN:
               default:
                  this->mpc_IconTopLeft->SetSvg("");
                  break;
               }
               q_TopLeftVisible = this->mc_NodeData.GetUpdateConnectedStatus();
            }
         }
         this->mpc_IconBottom->setVisible(false);
      }
      else
      {
         this->mpc_IconBottom->SetText(C_GtGetText::h_GetText("FAIL"));
         this->mpc_IconBottom->SetSvg("://images/system_views/UpdateNoResponse.svg");
         this->mpc_IconTopLeft->SetSvg("");

         this->mpc_IconBottom->SetBackgroundColor(mc_STYLE_GUIDE_COLOR_24);
         this->mpc_IconBottom->SetTextColor(Qt::white, static_cast<int32_t>(Qt::AlignCenter));
         this->mpc_IconBottom->setVisible(true);
         q_TopLeftVisible = false;
      }
   }
   else
   {
      //No icons for inactive nodes
      q_TopLeftVisible = false;
      this->mpc_IconBottom->setVisible(false);
   }
   //Only allow top left status if no error
   q_TopLeftVisible = q_TopLeftVisible && (this->m_GetErrorStatus() == false);
   //Apply
   this->mpc_IconTopLeft->setVisible(q_TopLeftVisible);
   //Update
   if (this->mpc_IconBottom->isVisible())
   {
      this->mpc_IconBottom->update();
   }
   if (this->mpc_IconTopLeft->isVisible())
   {
      this->mpc_IconTopLeft->update();
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Generate hint
*/
//----------------------------------------------------------------------------------------------------------------------
void C_GiSvNodeSyvUpdate::GenerateHint(void)
{
   if (this->mq_ErrorIconHovered == true)
   {
      C_GiSvNodeSyvBase::GenerateHint();
   }
   else
   {
      this->SetDefaultToolTipType(C_NagToolTip::eDEFAULT);
      this->SetDefaultToolTipHeading(C_PuiSdUtil::h_GetNodeBaseNameOrName(static_cast<uint32_t>(this->GetIndex())));

      if (this->mq_ViewConnected == true)
      {
         const C_SyvUtil::E_NodeUpdateInitialStatus e_OverAllInitialState =
            this->mc_NodeData.GetOverallInitialState();
         if ((this->mc_NodeData.CheckUpdateDisabledState() == true) &&
             (e_OverAllInitialState != C_SyvUtil::eI_ERROR))
         {
            this->SetDefaultToolTipContent(
               C_GtGetText::h_GetText(
                  "Node update disabled."
                  "\nPossible reasons: "
                  "\n- There are no active Data Blocks declared (SYSTEM DEFINITION / Node / Properties)"
                  "\n- Update setting is disabled on connected node interface (SYSTEM DEFINITION / Node / Properties)"
                  "\n- Node has no protocol support"
                  "\n- Update Package does not contain any files for this node"));
         }
         else
         {
            if (this->mc_NodeData.GetUpdateConnectedStatus() == true)
            {
               QString c_Text;
               if (this->mc_NodeData.GetUpdateInProgressStatus() == false)
               {
                  switch (e_OverAllInitialState)
                  {
                  case C_SyvUtil::eI_APPLICATION_MATCH:
                     c_Text = C_GtGetText::h_GetText("Node is up to date.");
                     break;
                  case C_SyvUtil::eI_TO_BE_UPDATED:
                     if (this->mc_NodeData.AreAllNodesValid())
                     {
                        c_Text = C_GtGetText::h_GetText("Update required.");
                     }
                     else
                     {
                        c_Text = C_GtGetText::h_GetText("Requesting info ...");
                     }
                     break;
                  case C_SyvUtil::eI_ERROR:
                     c_Text = C_GtGetText::h_GetText("Error occurred. Check node progress log for details!");
                     break;
                  case C_SyvUtil::eI_UPDATE_DISABLED:
                  case C_SyvUtil::eI_UNKNOWN:
                  default:
                     break;
                  }
               }
               else
               {
                  switch (e_OverAllInitialState)
                  {
                  case C_SyvUtil::eI_APPLICATION_MATCH:
                     c_Text = C_GtGetText::h_GetText("Node is up to date.");
                     break;
                  case C_SyvUtil::eI_TO_BE_UPDATED:
                     c_Text = C_GtGetText::h_GetText("Update required.");
                     break;
                  case C_SyvUtil::eI_ERROR:
                     c_Text = C_GtGetText::h_GetText("Error occurred. Check node progress log for details!");
                     break;
                  case C_SyvUtil::eI_UPDATE_DISABLED:
                  case C_SyvUtil::eI_UNKNOWN:
                  default:
                     break;
                  }
                  switch (this->mc_NodeData.GetOverallUpdateState())
                  {
                  case C_SyvUtil::eU_UPDATING:
                     c_Text += C_GtGetText::h_GetText("\nUpdating...");
                     break;
                  case C_SyvUtil::eU_WAITING:
                     c_Text += C_GtGetText::h_GetText("\nWaiting for update...");
                     break;
                  case C_SyvUtil::eU_UP_TO_DATE:
                  case C_SyvUtil::eU_UPDATE_SUCCESS:
                  case C_SyvUtil::eU_UPDATE_DISABLED:
                  case C_SyvUtil::eU_UNKNOWN:
                  default:
                     break;
                  }
               }
               this->SetDefaultToolTipContent(c_Text);
            }
            else
            {
               this->SetDefaultToolTipContent(C_GtGetText::h_GetText("Enter update mode to get the node status."));
            }
         }
      }
      else
      {
         this->SetDefaultToolTipContent(C_GtGetText::h_GetText("Inactive node."));
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Signal for update of current scaling

   \param[in]  orc_Transform  Current scaling
*/
//----------------------------------------------------------------------------------------------------------------------
void C_GiSvNodeSyvUpdate::UpdateTransform(const QTransform & orc_Transform)
{
   C_GiSvNodeSyvBase::UpdateTransform(orc_Transform);
   if (this->mpc_IconTopLeft != NULL)
   {
      this->mpc_IconTopLeft->UpdateTransform(orc_Transform);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Prepare icons
*/
//----------------------------------------------------------------------------------------------------------------------
void C_GiSvNodeSyvUpdate::m_InitIcons(void)
{
   const uint32_t u32_ScaleCategory = m_GetScaleCategory();
   const int32_t s32_IconSize = this->m_GetIconSize();
   const QRectF c_Rect = this->m_GetBoundary()->boundingRect();
   const int32_t s32_BORDER_SIZE = 9;
   const int32_t s32_BORDER_SIZE_ICON_BOTTOM = 15;
   const int32_t s32_IconOffset = std::max(1L, s32_IconSize / 4L);
   int32_t s32_PosHorizontal;
   int32_t s32_PosVertical;
   //Helper Variable to avoid pc lint warnings
   float64_t f64_HelpVar;

   //Create + placement

   //handling of this->mpc_IconTopLeft
   s32_PosHorizontal = (static_cast<int32_t>(c_Rect.width()) - s32_IconSize) - s32_BORDER_SIZE;
   s32_PosVertical = s32_BORDER_SIZE;
   this->mpc_IconTopLeft = new C_GiInfo(QRect(s32_PosHorizontal, s32_PosVertical, s32_IconSize, s32_IconSize));

   //handling of this->mpc_IconBottom
   s32_PosHorizontal = s32_BORDER_SIZE_ICON_BOTTOM;

   //Note:
   //   0.77: vertical position factor
   //   0.75: icon size factor

   f64_HelpVar = static_cast<float64_t>(c_Rect.height() * 0.77);
   s32_PosVertical = static_cast<int32_t>(f64_HelpVar) - s32_IconSize;

   f64_HelpVar = static_cast<float64_t>(s32_IconSize) * 0.75;
   this->mpc_IconBottom =
      new C_GiInfo(QRect(static_cast<int32_t>(s32_PosHorizontal), static_cast<int32_t>(s32_PosVertical),
                         static_cast<int32_t>(static_cast<int32_t>(c_Rect.width()) -
                                              (2L * s32_BORDER_SIZE_ICON_BOTTOM)),
                         static_cast<int32_t>(static_cast<int32_t>(f64_HelpVar) + s32_IconOffset)));

   //Colors
   this->mpc_IconBottom->SetBackgroundColor(Qt::transparent);
   this->mpc_IconBottom->SetTextColor(mc_STYLE_GUIDE_COLOR_8, static_cast<int32_t>(Qt::AlignLeft));
   this->mpc_IconBottom->SetText("Unexpected");

   //Font
   if (u32_ScaleCategory == mhu32_SCALE_CATEGORY_1)
   {
      this->mpc_IconBottom->SetTextFont(mc_STYLE_GUIDE_FONT_REGULAR_10);
   }
   else if (u32_ScaleCategory == mhu32_SCALE_CATEGORY_2)
   {
      this->mpc_IconBottom->SetTextFont(mc_STYLE_GUIDE_FONT_REGULAR_15);
   }
   else if (u32_ScaleCategory == mhu32_SCALE_CATEGORY_3)
   {
      this->mpc_IconBottom->SetTextFont(mc_STYLE_GUIDE_FONT_REGULAR_20);
   }
   else if (u32_ScaleCategory == mhu32_SCALE_CATEGORY_4)
   {
      this->mpc_IconBottom->SetTextFont(mc_STYLE_GUIDE_FONT_REGULAR_25);
   }
   else if (u32_ScaleCategory == mhu32_SCALE_CATEGORY_5)
   {
      this->mpc_IconBottom->SetTextFont(mc_STYLE_GUIDE_FONT_REGULAR_30);
   }
   else if (u32_ScaleCategory == mhu32_SCALE_CATEGORY_6)
   {
      this->mpc_IconBottom->SetTextFont(mc_STYLE_GUIDE_FONT_REGULAR_35);
   }
   else if (u32_ScaleCategory == mhu32_SCALE_CATEGORY_7)
   {
      this->mpc_IconBottom->SetTextFont(mc_STYLE_GUIDE_FONT_REGULAR_40);
   }
   else
   {
      this->mpc_IconBottom->SetTextFont(mc_STYLE_GUIDE_FONT_REGULAR_7);
   }

   //Icons
   this->mpc_IconTopLeft->SetIconSize(s32_IconSize);
   f64_HelpVar = static_cast<float64_t>(s32_IconSize) * 0.75;
   this->mpc_IconBottom->SetIconSize(static_cast<int32_t>(f64_HelpVar));

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

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Update dialog if necessary
*/
//----------------------------------------------------------------------------------------------------------------------
void C_GiSvNodeSyvUpdate::m_RefreshDialog()
{
   if (this->mpc_InfoDialog != NULL)
   {
      this->mpc_InfoDialog->SetStatus(this->mc_NodeData);
   }
}
