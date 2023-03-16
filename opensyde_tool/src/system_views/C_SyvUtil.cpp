//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       System view utility class (implementation)

   System view utility class

   \copyright   Copyright 2018 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.hpp"

#include "C_SdUtil.hpp"
#include "stwerrors.hpp"
#include "constants.hpp"
#include "C_SyvUtil.hpp"
#include "C_GtGetText.hpp"
#include "C_PuiSdUtil.hpp"
#include "C_PuiSdHandler.hpp"
#include "C_PuiSvHandler.hpp"
#include "C_OscSystemBus.hpp"
#include "TglUtils.hpp"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw::errors;
using namespace stw::opensyde_gui;
using namespace stw::opensyde_core;
using namespace stw::opensyde_gui_logic;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

/* -- Global Variables ---------------------------------------------------------------------------------------------- */

/* -- Module Global Variables --------------------------------------------------------------------------------------- */

/* -- Module Global Function Prototypes ----------------------------------------------------------------------------- */

/* -- Implementation ------------------------------------------------------------------------------------------------ */

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get system view label info

   \param[in]      ou32_ViewIndex               View index
   \param[in]      os32_ViewSubMode             View sub mode (setup, update or dashboard)
   \param[out]     orc_ErrorLabelHeadingText    Error heading information
   \param[out]     orc_ErrorLabelText           Detailed error information
   \param[out]     orc_ErrorTooltipText         Error tooltip information
   \param[out]     ore_TooltipType              Tooltip type
   \param[in,out]  orc_IconPath                 Icon path
   \param[out]     ors32_ColorId                Color ID

   \return
   True  Show label
   False Hide label
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_SyvUtil::h_GetViewStatusLabelInfo(const uint32_t ou32_ViewIndex, const int32_t os32_ViewSubMode,
                                         QString & orc_ErrorLabelHeadingText, QString & orc_ErrorLabelText,
                                         QString & orc_ErrorTooltipText, C_NagToolTip::E_Type & ore_TooltipType,
                                         QString & orc_IconPath, int32_t & ors32_ColorId)
{
   bool q_Retval = false;
   bool q_NameInvalid;
   bool q_PcNotConnected;
   bool q_RoutingInvalid;
   bool q_UpdateRoutingInvalid;
   bool q_DashboardRoutingInvalid;
   bool q_SysDefInvalid;
   bool q_NoActiveNodes;

   std::vector<QString> c_RoutingErrorText;
   QString c_SetupRoutingWarningText;
   int32_t s32_Return;

   s32_Return = C_PuiSvHandler::h_GetInstance()->CheckViewError(ou32_ViewIndex,
                                                                &q_NameInvalid,
                                                                &q_PcNotConnected,
                                                                &q_RoutingInvalid, &q_UpdateRoutingInvalid,
                                                                &q_DashboardRoutingInvalid,
                                                                &q_SysDefInvalid, &q_NoActiveNodes,
                                                                &c_RoutingErrorText, &c_SetupRoutingWarningText);

   if ((q_NameInvalid == true) || (q_PcNotConnected == true) || (q_RoutingInvalid == true) ||
       (q_SysDefInvalid == true) || (q_NoActiveNodes == true) ||
       (s32_Return != C_NO_ERR))
   {
      if (s32_Return == C_RANGE)
      {
         orc_ErrorLabelHeadingText = C_GtGetText::h_GetText("Invalid View:");
         orc_ErrorLabelText = C_GtGetText::h_GetText("View is invalid.");
      }
      else if (q_NoActiveNodes == true)
      {
         orc_ErrorLabelHeadingText = C_GtGetText::h_GetText("Invalid View:");
         orc_ErrorLabelText = C_GtGetText::h_GetText("There are no active nodes.");
      }
      else if (q_NameInvalid == true)
      {
         orc_ErrorLabelHeadingText = C_GtGetText::h_GetText("Invalid View:");
         orc_ErrorLabelText = C_GtGetText::h_GetText("Name of view is already used.");
      }
      else if (q_PcNotConnected == true)
      {
         orc_ErrorLabelHeadingText = C_GtGetText::h_GetText("Invalid View:");
         orc_ErrorLabelText = C_GtGetText::h_GetText(
            "There are no active buses. PC connection could not be established.");
      }
      else if (q_RoutingInvalid == true)
      {
         // Setup error is relevant for all sub modes
         orc_ErrorLabelHeadingText = C_GtGetText::h_GetText("Invalid View:");
         orc_ErrorLabelText = c_RoutingErrorText[ms32_SUBMODE_SYSVIEW_SETUP];
      }
      else if (q_SysDefInvalid == true)
      {
         orc_ErrorLabelHeadingText = C_GtGetText::h_GetText("Invalid View:");
         orc_ErrorLabelText = C_GtGetText::h_GetText(
            "At least one of the active view items (node or bus) has invalid definition.");
      }
      else
      {
         // nothing to do and should not happen
      }

      q_Retval = true;
   }
   // Special cases: Error depends of sub mode
   else if ((os32_ViewSubMode == ms32_SUBMODE_SYSVIEW_UPDATE) && (q_UpdateRoutingInvalid == true))
   {
      // Handle the invalid update routing as error
      orc_ErrorLabelHeadingText = C_GtGetText::h_GetText("Invalid Update View:");
      orc_ErrorLabelText = c_RoutingErrorText[ms32_SUBMODE_SYSVIEW_UPDATE];
      q_Retval = true;
   }
   else if ((os32_ViewSubMode == ms32_SUBMODE_SYSVIEW_DASHBOARD) && (q_DashboardRoutingInvalid == true))
   {
      // Show the error as warning to inform the user why specific nodes are not available in the dashboard
      orc_ErrorLabelHeadingText = C_GtGetText::h_GetText("Note:");
      orc_ErrorLabelText = c_RoutingErrorText[ms32_SUBMODE_SYSVIEW_DASHBOARD];
   }
   else if ((os32_ViewSubMode == ms32_SUBMODE_SYSVIEW_SETUP) &&
            (c_SetupRoutingWarningText != ""))
   {
      // Show the warning if a warning exists as information
      orc_ErrorLabelHeadingText = C_GtGetText::h_GetText("Note:");
      orc_ErrorLabelText = c_SetupRoutingWarningText;
   }
   else
   {
      orc_ErrorLabelHeadingText = "";
      orc_ErrorLabelText = "";
   }

   if (q_Retval == true)
   {
      // Error message
      ore_TooltipType = C_NagToolTip::eERROR;
      orc_IconPath = "://images/Error_iconV2.svg";
      ors32_ColorId = 24;
   }
   else
   {
      // Info message
      ore_TooltipType = C_NagToolTip::eDEFAULT;
      orc_IconPath = "://images/Info_Icon_MessageBox.svg";
      ors32_ColorId = 3;

      if (orc_ErrorLabelHeadingText != "")
      {
         q_Retval = true;
      }
   }
   //Combine strings for tooltip
   if ((orc_ErrorLabelHeadingText.isEmpty() == false) || (orc_ErrorLabelText.isEmpty() == false))
   {
      orc_ErrorTooltipText = orc_ErrorLabelHeadingText + " " + orc_ErrorLabelText;
   }
   return q_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Convert bus ID and node id to SD indices

   \param[in]   ou8_BusIdentifier   Bus id
   \param[in]   ou8_NodeIdentifier  Node Id within bus
   \param[out]  oru32_NodeIndex     SD node index
   \param[out]  oru32_BusIndex      SD bus index

   \return
   C_NO_ERR Operation success
   C_CONFIG Operation failure: configuration invalid
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_SyvUtil::h_GetIndicesFromBusId(const uint8_t ou8_BusIdentifier, const uint8_t ou8_NodeIdentifier,
                                         uint32_t & oru32_NodeIndex, uint32_t & oru32_BusIndex)
{
   int32_t s32_Retval = C_NO_ERR;
   bool q_NodeFound = false;

   for (uint32_t u32_ItBus = 0; u32_ItBus < C_PuiSdHandler::h_GetInstance()->GetOscBusesSize(); ++u32_ItBus)
   {
      const C_OscSystemBus * const pc_Bus = C_PuiSdHandler::h_GetInstance()->GetOscBus(u32_ItBus);
      if ((pc_Bus != NULL) && (pc_Bus->u8_BusId == ou8_BusIdentifier))
      {
         std::vector<uint32_t> c_NodeIndexes;
         std::vector<uint32_t> c_InterfaceIndexes;
         oru32_BusIndex = u32_ItBus;
         //Node
         C_PuiSdHandler::h_GetInstance()->GetOscSystemDefinitionConst().GetNodeIndexesOfBus(u32_ItBus, c_NodeIndexes,
                                                                                            c_InterfaceIndexes);
         tgl_assert(c_NodeIndexes.size() == c_InterfaceIndexes.size());
         if (c_NodeIndexes.size() == c_InterfaceIndexes.size())
         {
            for (uint32_t u32_ItNode = 0; u32_ItNode < c_NodeIndexes.size(); ++u32_ItNode)
            {
               const C_OscNode * const pc_Node =
                  C_PuiSdHandler::h_GetInstance()->GetOscNodeConst(c_NodeIndexes[u32_ItNode]);
               if ((pc_Node != NULL) && (c_InterfaceIndexes[u32_ItNode] < pc_Node->c_Properties.c_ComInterfaces.size()))
               {
                  const C_OscNodeComInterfaceSettings & rc_ComInterface =
                     pc_Node->c_Properties.c_ComInterfaces[c_InterfaceIndexes[u32_ItNode]];
                  if (rc_ComInterface.u8_NodeId == ou8_NodeIdentifier)
                  {
                     oru32_NodeIndex = c_NodeIndexes[u32_ItNode];
                     q_NodeFound = true;
                  }
               }
            }
         }
      }
   }
   if (q_NodeFound == false)
   {
      s32_Retval = C_CONFIG;
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get update mode details for specified ID in specified view

   \param[in]  ou32_ViewIndex    Current view index
   \param[in]  orc_Id            ID to get details for

   \return
   Update mode details
*/
//----------------------------------------------------------------------------------------------------------------------
QString C_SyvUtil::h_GetUpdateModeDescription(const uint32_t ou32_ViewIndex,
                                              const C_PuiSvDbNodeDataPoolListElementId & orc_Id)
{
   QString c_Retval;

   if (orc_Id.GetIsValid() == true)
   {
      if (orc_Id.GetType() == C_PuiSvDbNodeDataPoolListElementId::eDATAPOOL_ELEMENT)
      {
         const C_PuiSvData * const pc_View = C_PuiSvHandler::h_GetInstance()->GetView(ou32_ViewIndex);

         if (pc_View != NULL)
         {
            const QMap<C_OscNodeDataPoolListElementId,
                       C_PuiSvReadDataConfiguration>::const_iterator c_Result = pc_View->GetReadRailAssignments().find(
               orc_Id);
            if (c_Result != pc_View->GetReadRailAssignments().end())
            {
               const C_OscNodeDataPoolContent & rc_Content = c_Result.value().c_ChangeThreshold;
               QString c_Text;
               QString c_CyclicText;
               QString c_ThresholdText;

               //Additional text for cyclic type
               switch (c_Result.value().u8_RailIndex)
               {
               case 0:
                  //Translation: 1 = Time in milliseconds
                  c_CyclicText = static_cast<QString>(C_GtGetText::h_GetText("Fast - %1 ms")).arg(
                     pc_View->GetUpdateRateFast());
                  break;
               case 1:
                  //Translation: 1 = Time in milliseconds
                  c_CyclicText = static_cast<QString>(C_GtGetText::h_GetText("Medium - %1 ms")).arg(
                     pc_View->GetUpdateRateMedium());
                  break;
               case 2:
                  //Translation: 1 = Time in milliseconds
                  c_CyclicText = static_cast<QString>(C_GtGetText::h_GetText("Slow - %1 ms")).arg(
                     pc_View->GetUpdateRateSlow());
                  break;
               default:
                  c_CyclicText = "";
                  break;
               }

               switch (c_Result.value().e_TransmissionMode)
               {
               case C_PuiSvReadDataConfiguration::eTM_ON_TRIGGER:
                  c_Text = C_GtGetText::h_GetText("On Trigger");
                  break;
               case C_PuiSvReadDataConfiguration::eTM_CYCLIC:
                  //Translation: 1 = Additional information
                  c_Text = static_cast<QString>(C_GtGetText::h_GetText("Cyclic (%1)")).arg(c_CyclicText);
                  break;
               case C_PuiSvReadDataConfiguration::eTM_ON_CHANGE:
                  //Additional text for threshold type
                  tgl_assert(rc_Content.GetArray() == false);
                  if (rc_Content.GetArray() == false)
                  {
                     switch (rc_Content.GetType())
                     {
                     case C_OscNodeDataPoolContent::E_Type::eUINT8:
                        c_ThresholdText = QString::number(static_cast<uint64_t>(rc_Content.GetValueU8()));
                        break;
                     case C_OscNodeDataPoolContent::E_Type::eUINT16:
                        c_ThresholdText = QString::number(static_cast<uint64_t>(rc_Content.GetValueU16()));
                        break;
                     case C_OscNodeDataPoolContent::E_Type::eUINT32:
                        c_ThresholdText = QString::number(static_cast<uint64_t>(rc_Content.GetValueU32()));
                        break;
                     case C_OscNodeDataPoolContent::E_Type::eUINT64:
                        c_ThresholdText = QString::number(rc_Content.GetValueU64());
                        break;
                     case C_OscNodeDataPoolContent::E_Type::eSINT8:
                        c_ThresholdText = QString::number(static_cast<int64_t>(rc_Content.GetValueS8()));
                        break;
                     case C_OscNodeDataPoolContent::E_Type::eSINT16:
                        c_ThresholdText = QString::number(static_cast<int64_t>(rc_Content.GetValueS16()));
                        break;
                     case C_OscNodeDataPoolContent::E_Type::eSINT32:
                        c_ThresholdText = QString::number(static_cast<int64_t>(rc_Content.GetValueS32()));
                        break;
                     case C_OscNodeDataPoolContent::E_Type::eSINT64:
                        c_ThresholdText = QString::number(rc_Content.GetValueS64());
                        break;
                     case C_OscNodeDataPoolContent::E_Type::eFLOAT32:
                        c_ThresholdText = QString::number(static_cast<float64_t>(rc_Content.GetValueF32()));
                        break;
                     case C_OscNodeDataPoolContent::E_Type::eFLOAT64:
                        c_ThresholdText = QString::number(rc_Content.GetValueF64());
                        break;
                     default:
                        //Nothing to do
                        break;
                     }
                  }
                  //Translation: 1 = Additional information, 2 = Threshold value
                  c_Text = static_cast<QString>(C_GtGetText::h_GetText("Cyclic on change (%1),Threshold: %2")).arg(
                     c_CyclicText).arg(
                     c_ThresholdText);
                  break;
               default:
                  tgl_assert(false);
                  break;
               }
               c_Retval = c_Text;
            }
         }
      }
      else
      {
         //Step 1 convert ID to message ID!
         uint32_t u32_SignalIndex;
         C_OscCanMessageIdentificationIndices c_MessageId;
         if (C_PuiSdUtil::h_ConvertIndex(orc_Id, c_MessageId, u32_SignalIndex) == C_NO_ERR)
         {
            //Step 2 Get message info and format it
            const C_OscCanMessage * const pc_Message = C_PuiSdHandler::h_GetInstance()->GetCanMessage(c_MessageId);
            if (pc_Message != NULL)
            {
               switch (pc_Message->e_TxMethod)
               {
               case C_OscCanMessage::eTX_METHOD_ON_EVENT:
                  c_Retval = static_cast<QString>(C_GtGetText::h_GetText("On Event"));
                  break;
               case C_OscCanMessage::eTX_METHOD_CAN_OPEN_TYPE_0:
                  c_Retval =
                     static_cast<QString>(C_GtGetText::h_GetText(
                                             "Type 0 - synchronous transmission after next SYNC and change"));
                  break;
               case C_OscCanMessage::eTX_METHOD_CAN_OPEN_TYPE_1_TO_240:
                  c_Retval =
                     static_cast<QString>(C_GtGetText::h_GetText(
                                             "Type 1 to 240 - synchronous transmission after 1st to 240th SYNC"));
                  break;
               case C_OscCanMessage::eTX_METHOD_CAN_OPEN_TYPE_254:
                  c_Retval =
                     static_cast<QString>(C_GtGetText::h_GetText("Type 254 - asynchronous manufacturer specific"));
                  break;
               case C_OscCanMessage::eTX_METHOD_CAN_OPEN_TYPE_255:
                  c_Retval =
                     static_cast<QString>(C_GtGetText::h_GetText("Type 255 - asynchronous device specific"));
                  break;
               case C_OscCanMessage::eTX_METHOD_CYCLIC:
                  c_Retval = static_cast<QString>(C_GtGetText::h_GetText("Cyclic (%1 ms)")).arg(
                     pc_Message->u32_CycleTimeMs);
                  break;
               case C_OscCanMessage::eTX_METHOD_ON_CHANGE:
                  //Translation1: Timeout, 2: Minimum delay, 3: "greater than or equal" - sign, 4: "less than or equal"
                  // - sign
                  c_Retval =
                     static_cast<QString>(C_GtGetText::h_GetText("On Change (%3 %2 ms; %4 %1 ms)")).
                     arg(pc_Message->u32_CycleTimeMs).arg(pc_Message->u16_DelayTimeMs).
                     arg(static_cast<QChar>(0x2265)).arg(static_cast<QChar>(0x2264));
                  break;
               default:
                  tgl_assert(false);
                  break;
               }
            }
         }
      }
   }
   return c_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get common dashboard item tool tip for specified ID in specified view

   \param[in]  ou32_ViewIndex    Current view index
   \param[in]  orc_Id            ID to get details for
   \param[in]  oq_ReadItem       Read widget?
   \param[in]  oe_WriteMode      Write mode

   \return
   Common dashboard item tool tip
*/
//----------------------------------------------------------------------------------------------------------------------
QString C_SyvUtil::h_GetCommonDashboardItemToolTip(const uint32_t ou32_ViewIndex,
                                                   const C_PuiSvDbNodeDataPoolListElementId & orc_Id,
                                                   const bool oq_ReadItem,
                                                   const stw::opensyde_gui_logic::C_PuiSvDbWidgetBase::E_WriteMode oe_WriteMode)
{
   QString c_Retval;
   QString c_DashboardDescription;

   c_DashboardDescription = C_GtGetText::h_GetText("Visualization Properties: \n");

   //Source
   c_DashboardDescription += static_cast<QString>("   ") + C_GtGetText::h_GetText("Source: ");
   c_DashboardDescription += C_PuiSvHandler::h_GetNamespace(orc_Id);
   c_DashboardDescription += "\n";

   //Update Mode
   c_DashboardDescription += static_cast<QString>("   ") + C_GtGetText::h_GetText("Update mode: ");

   if (oq_ReadItem == true)
   {
      c_DashboardDescription += C_SyvUtil::h_GetUpdateModeDescription(ou32_ViewIndex, orc_Id);
   }
   else
   {
      if (oe_WriteMode == C_PuiSvDbWidgetBase::eWM_MANUAL)
      {
         c_DashboardDescription += C_GtGetText::h_GetText("On Trigger");
      }
      else
      {
         c_DashboardDescription += C_GtGetText::h_GetText("On Change");
      }
   }

   c_Retval = C_SdUtil::h_GetToolTipContentDpListElement(orc_Id, c_DashboardDescription);
   return c_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get view name to display

   \param[in]   ou32_ViewIndex   View index
   \param[in]   os32_SubMode     Current sub mode (skipped if invalid)
   \param[out]  orc_SubMode      Sub mode heading
   \param[out]  orc_SubSubMode   Sub sub mode heading
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvUtil::h_GetViewDisplayName(const uint32_t ou32_ViewIndex, const int32_t os32_SubMode, QString & orc_SubMode,
                                     QString & orc_SubSubMode)
{
   const C_PuiSvData * const pc_View = C_PuiSvHandler::h_GetInstance()->GetView(ou32_ViewIndex);

   if (pc_View != NULL)
   {
      switch (os32_SubMode)
      {
      case ms32_SUBMODE_SYSVIEW_SETUP:
         orc_SubSubMode = static_cast<QString>(C_GtGetText::h_GetText("Setup"));
         break;
      case ms32_SUBMODE_SYSVIEW_UPDATE:
         orc_SubSubMode = static_cast<QString>(C_GtGetText::h_GetText("Update"));
         break;
      case ms32_SUBMODE_SYSVIEW_DASHBOARD:
         orc_SubSubMode = static_cast<QString>(C_GtGetText::h_GetText("Dashboards (%1)")).arg(
            pc_View->GetDashboards().size());
         break;
      default:
         //Skip addendum
         orc_SubSubMode = "";
         break;
      }
      orc_SubMode = static_cast<QString>(C_GtGetText::h_GetText("VIEW #%1 - %2"))
                    .arg(ou32_ViewIndex + 1).arg(pc_View->GetName().c_str());
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default constructor
*/
//----------------------------------------------------------------------------------------------------------------------
C_SyvUtil::C_SyvUtil(void)
{
}
