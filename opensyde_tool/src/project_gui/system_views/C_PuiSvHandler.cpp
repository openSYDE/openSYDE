//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       System views data manager (implementation)

   System views data manager

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.hpp"

#include <iostream>

#include <QFileInfo>

#include "stwtypes.hpp"
#include "stwerrors.hpp"
#include "constants.hpp"
#include "C_Uti.hpp"
#include "TglFile.hpp"
#include "TglUtils.hpp"
#include "C_GtGetText.hpp"
#include "C_SclChecksums.hpp"
#include "C_OscXmlParser.hpp"
#include "C_OscXmlParserLog.hpp"
#include "C_PuiSdHandler.hpp"
#include "C_PuiSdUtil.hpp"
#include "C_PuiSvHandler.hpp"
#include "C_OscLoggingHandler.hpp"
#include "C_PuiSvHandlerFiler.hpp"
#include "C_PuiSvHandlerFilerV1.hpp"
#include "C_SyvRoRouteCalculation.hpp"
#include "C_OscRoutingCalculation.hpp"
#include "C_OscHalcMagicianUtil.hpp"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw::tgl;
using namespace stw::errors;
using namespace stw::opensyde_gui;
using namespace stw::opensyde_core;
using namespace stw::opensyde_gui_logic;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

/* -- Global Variables ---------------------------------------------------------------------------------------------- */

/* -- Module Global Variables --------------------------------------------------------------------------------------- */
C_PuiSvHandler * C_PuiSvHandler::mhpc_Singleton = NULL;

/* -- Module Global Function Prototypes ----------------------------------------------------------------------------- */

/* -- Implementation ------------------------------------------------------------------------------------------------ */

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Load system views

   Load system views and store in information in our instance data.

   \param[in]  orc_Path    Path to system views file

   \return
   C_NO_ERR   data read and placed into instance data
   C_RD_WR    problems accessing file system (e.g. no read access to file)
   C_RANGE    specified file does not exist (when loading)
   C_NOACT    specified file is present but structure is invalid (e.g. invalid XML file)
   C_CONFIG   content of file is invalid or incomplete
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_PuiSvHandler::LoadFromFile(const QString & orc_Path)
{
   return this->m_LoadFromFile(orc_Path,
                               C_PuiSdHandler::h_GetInstance()->GetOscSystemDefinitionConst().c_Nodes);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Save system views

   Save UI data part of system views to XML file.

   \param[in]  orc_Path                   Path to system views file
   \param[in]  oq_UseDeprecatedV1Format   Optional flag to save in deprecated V1 format

   \return
   C_NO_ERR   data saved
   C_RD_WR    problems accessing file system (e.g. could not erase pre-existing file before saving)
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_PuiSvHandler::SaveToFile(const QString & orc_Path, const bool oq_UseDeprecatedV1Format)
{
   int32_t s32_Return = C_NO_ERR;

   if (TglFileExists(orc_Path.toStdString().c_str()) == true)
   {
      //erase it:
      s32_Return = std::remove(orc_Path.toStdString().c_str());
      if (s32_Return != 0)
      {
         s32_Return = C_RD_WR;
      }
      else
      {
         s32_Return = C_NO_ERR;
      }
   }
   if (s32_Return == C_NO_ERR)
   {
      const QFileInfo c_Info(orc_Path);
      const QDir c_BasePath = c_Info.dir();
      if (c_BasePath.mkpath(".") == true)
      {
         C_OscXmlParser c_XmlParser;
         c_XmlParser.CreateAndSelectNodeChild("opensyde-system-views");
         if (oq_UseDeprecatedV1Format == true)
         {
            c_XmlParser.CreateNodeChild("file-version", "1");
            C_PuiSvHandlerFilerV1::h_SaveViews(this->mc_Views, c_XmlParser);
         }
         else
         {
            c_XmlParser.CreateNodeChild("file-version", "2");
            if (this->mq_IsServiceModeActive)
            {
               tgl_assert(c_XmlParser.CreateAndSelectNodeChild("service-mode") == "service-mode");
               c_XmlParser.SetAttributeBool("active", this->mq_IsServiceModeActive);
               c_XmlParser.SelectNodeParent();
            }
            s32_Return = C_PuiSvHandlerFiler::h_SaveViews(this->mc_Views, c_XmlParser, &c_BasePath);

            C_PuiSvHandlerFiler::h_SaveLastKnownHalcCrcs(this->mc_LastKnownHalcCrcs, c_XmlParser);
            //Only update hash in non deprecated mode
            //calculate the hash value and save it for comparing
            this->mu32_CalculatedHashSystemViews = this->m_CalcHashSystemViews();
         }
         if (s32_Return == C_NO_ERR)
         {
            s32_Return = c_XmlParser.SaveToFile(orc_Path.toStdString().c_str());
            if (s32_Return != C_NO_ERR)
            {
               s32_Return = C_RD_WR;
            }
         }
         else
         {
            s32_Return = C_RD_WR;
         }
      }
      else
      {
         s32_Return = C_RD_WR;
      }
   }
   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Compares the last saved hash value against the actual hash

   \return
   false    Hash has not changed
   true     Hash has changed
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_PuiSvHandler::HasHashChanged(void) const
{
   const uint32_t u32_NewHash = this->m_CalcHashSystemViews();
   bool q_Changed = true;

   if (u32_NewHash == this->mu32_CalculatedHashSystemViews)
   {
      q_Changed = false;
   }

   return q_Changed;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get view

   \param[in]  ou32_Index  View index

   \return
   NULL View not found
   Else Valid view
*/
//----------------------------------------------------------------------------------------------------------------------
const C_PuiSvData * C_PuiSvHandler::GetView(const uint32_t ou32_Index) const
{
   const C_PuiSvData * pc_Retval;

   if (ou32_Index < this->mc_Views.size())
   {
      pc_Retval = &this->mc_Views[ou32_Index];
   }
   else
   {
      pc_Retval = NULL;
   }
   return pc_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get number of views

   \return
   Number of views
*/
//----------------------------------------------------------------------------------------------------------------------
uint32_t C_PuiSvHandler::GetViewCount(void) const
{
   return this->mc_Views.size();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get last known HALC CRCs

   \return
   Last known HALC crcs
*/
//----------------------------------------------------------------------------------------------------------------------
const std::map<C_PuiSvDbNodeDataPoolListElementId, C_PuiSvLastKnownHalElementId> & C_PuiSvHandler::GetLastKnownHalcCrcs(
   void) const
{
   return this->mc_LastKnownHalcCrcs;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get service mode active

   \return
   Is service mode active
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_PuiSvHandler::GetServiceModeActive(void) const
{
   return this->mq_IsServiceModeActive;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Getting the node active flags with deactivated sub nodes of squad nodes when not connected

   Special case for active squad nodes:
   * When a sub node has no interface connected, the sub node will be deactivated
   * When a sub node has an interface connected, check for an error on the route.
      * If the route is valid, the sub node remains active
      * If the route is invalid, the sub node will be deactivated

   But at least one sub node of a squad must be active. If this is not the case, the sub nodes will remain as active

   \param[in]   ou32_ViewIndex            View index
   \param[out]  orc_ActiveFlags           Adapted node active flags if return value equals C_NO_ERR
   \param[in]   oq_IncludeRoutingResults  Include routing results

   \retval   C_NO_ERR   Operation success
   \retval   C_RANGE    Operation failure: parameter invalid
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_PuiSvHandler::GetNodeActiveFlagsWithSquadAdaptions(const uint32_t ou32_ViewIndex,
                                                             std::vector<uint8_t> & orc_ActiveFlags,
                                                             const bool oq_IncludeRoutingResults)
{
   int32_t s32_Return = C_RANGE;

   orc_ActiveFlags.clear();

   if (ou32_ViewIndex < this->mc_Views.size())
   {
      const uint32_t u32_Hash = this->GetViewHash(ou32_ViewIndex);
      const QPair<uint32_t, bool> c_HashRef(u32_Hash, oq_IncludeRoutingResults);
      const QMap<QPair<uint32_t, bool>,
                 std::vector<uint8_t> >::const_iterator c_It =
         this->mc_PreviousNodeActiveFlagsWithSquadAdaptionsResults.find(c_HashRef);

      s32_Return = C_NO_ERR;

      if (c_It == this->mc_PreviousNodeActiveFlagsWithSquadAdaptionsResults.end())
      {
         C_PuiSvData & rc_View = this->mc_Views[ou32_ViewIndex];
         const C_OscNodeSquad * pc_Squad = NULL;
         uint32_t u32_SquadCounter = 0U;

         // activate all where only one active
         rc_View.ActivateAllRelevantSubDevices();

         // Get the original user configured active flags
         orc_ActiveFlags = rc_View.GetNodeActiveFlags();

         // Search for squads
         do
         {
            pc_Squad = C_PuiSdHandler::h_GetInstance()->GetOscNodeSquadConst(u32_SquadCounter);

            if (pc_Squad != NULL)
            {
               uint32_t u32_SubNodeCounter;
               bool q_AtLeastOneSubNodeActive = false;
               std::vector<uint8_t> c_SubNodeActiveFlags;

               c_SubNodeActiveFlags.resize(pc_Squad->c_SubNodeIndexes.size(), 0U);

               // Check each sub node
               for (u32_SubNodeCounter = 0U; u32_SubNodeCounter < pc_Squad->c_SubNodeIndexes.size();
                    ++u32_SubNodeCounter)
               {
                  const uint32_t u32_NodeIndex = pc_Squad->c_SubNodeIndexes[u32_SubNodeCounter];

                  // Check only necessary if node is active in the first place
                  if (orc_ActiveFlags[u32_NodeIndex] != 0U)
                  {
                     const C_OscNode * const pc_Node = C_PuiSdHandler::h_GetInstance()->GetOscNodeConst(u32_NodeIndex);
                     tgl_assert(pc_Node != NULL);
                     if (pc_Node != NULL)
                     {
                        // Check for connected interfaces
                        uint32_t u32_IntfCounter;
                        bool q_AtLeastOneInftConnected = false;

                        for (u32_IntfCounter = 0U; u32_IntfCounter < pc_Node->c_Properties.c_ComInterfaces.size();
                             ++u32_IntfCounter)
                        {
                           const C_OscNodeComInterfaceSettings & rc_Intf =
                              pc_Node->c_Properties.c_ComInterfaces[u32_IntfCounter];

                           if (rc_Intf.GetBusConnected() == true)
                           {
                              q_AtLeastOneInftConnected = true;
                              break;
                           }
                        }

                        if (q_AtLeastOneInftConnected == true)
                        {
                           // Check for an existing valid route
                           const C_SyvRoRouteCalculation c_RouteCalcCheck(
                              ou32_ViewIndex, u32_NodeIndex,
                              stw::opensyde_core::C_OscRoutingCalculation::eROUTING_CHECK);
                           const C_OscRoutingRoute * const pc_BestRoute = c_RouteCalcCheck.GetBestRoute();

                           if ((c_RouteCalcCheck.GetState() == C_NO_ERR) &&
                               (oq_IncludeRoutingResults ||
                                ((pc_BestRoute != NULL) &&
                                 (pc_BestRoute->c_VecRoutePoints.size() == 0))))
                           {
                              // Valid route found
                              c_SubNodeActiveFlags[u32_SubNodeCounter] = 1U;
                              q_AtLeastOneSubNodeActive = true;
                           }
                        }
                     }
                  }
               }

               // In case of no active sub node of the squad, do not change the original active flag configuration
               // to let the normal error handling do its job
               if (q_AtLeastOneSubNodeActive == true)
               {
                  // In the case of at least one active sub node, transfer the adapted active flags of the sub node
                  // to the output parameter
                  for (u32_SubNodeCounter = 0U; u32_SubNodeCounter < c_SubNodeActiveFlags.size(); ++u32_SubNodeCounter)
                  {
                     const uint32_t u32_NodeIndex = pc_Squad->c_SubNodeIndexes[u32_SubNodeCounter];
                     orc_ActiveFlags[u32_NodeIndex] = c_SubNodeActiveFlags[u32_SubNodeCounter];
                  }
               }
               ++u32_SquadCounter;
            }
         }
         while (pc_Squad != NULL);

         //Store results
         this->mc_PreviousNodeActiveFlagsWithSquadAdaptionsResults.insert(c_HashRef, orc_ActiveFlags);
      }
      else
      {
         orc_ActiveFlags = c_It.value();
      }
   }

   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Set service mode active

   \param[in]  oq_NewValue    New value
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PuiSvHandler::SetServiceModeActive(const bool oq_NewValue)
{
   this->mq_IsServiceModeActive = oq_NewValue;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Set last known HALC CRCs

   \param[in]  orc_Value   Value
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PuiSvHandler::SetLastKnownHalcCrcs(const std::map<C_PuiSvDbNodeDataPoolListElementId,
                                                         C_PuiSvLastKnownHalElementId> & orc_Value)
{
   this->mc_LastKnownHalcCrcs = orc_Value;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set view name

   \param[in]  ou32_Index  View index
   \param[in]  orc_Name    View name

   \return
   C_NO_ERR Operation success
   C_RANGE  Operation failure: parameter invalid
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_PuiSvHandler::SetViewName(const uint32_t ou32_Index, const QString & orc_Name)
{
   int32_t s32_Retval = C_NO_ERR;

   if (ou32_Index < this->mc_Views.size())
   {
      C_PuiSvData & rc_View = this->mc_Views[ou32_Index];
      rc_View.SetName(orc_Name.toStdString().c_str());
   }
   else
   {
      s32_Retval = C_RANGE;
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set checked flag for node in view

   \param[in]  ou32_ViewIndex    View index
   \param[in]  ou32_NodeIndex    Node index
   \param[in]  oq_Checked        Checked flag

   \return
   C_NO_ERR Operation success
   C_RANGE  Operation failure: parameter invalid
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_PuiSvHandler::SetViewNodeCheckedState(const uint32_t ou32_ViewIndex, const uint32_t ou32_NodeIndex,
                                                const bool oq_Checked)
{
   int32_t s32_Retval = C_NO_ERR;

   if (ou32_ViewIndex < this->mc_Views.size())
   {
      C_PuiSvData & rc_View = this->mc_Views[ou32_ViewIndex];
      rc_View.SetNodeCheckedState(ou32_NodeIndex, static_cast<uint8_t>(oq_Checked));
   }
   else
   {
      s32_Retval = C_RANGE;
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set View PC UI box

   \param[in]  ou32_Index  View index
   \param[in]  orc_Box     New PC UI box

   \return
   C_NO_ERR Operation success
   C_RANGE  Operation failure: parameter invalid
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_PuiSvHandler::SetViewPcBox(const uint32_t ou32_Index, const C_PuiBsBox & orc_Box)
{
   int32_t s32_Retval = C_NO_ERR;

   if (ou32_Index < this->mc_Views.size())
   {
      C_PuiSvData & rc_View = this->mc_Views[ou32_Index];
      rc_View.SetPcBox(orc_Box);
   }
   else
   {
      s32_Retval = C_RANGE;
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set View PC connection UI data

   \param[in]  ou32_Index  View index
   \param[in]  orc_Line    New View PC connection UI data

   \return
   C_NO_ERR Operation success
   C_RANGE  Operation failure: parameter invalid
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_PuiSvHandler::SetViewPcConnection(const uint32_t ou32_Index, const C_PuiBsLineBase & orc_Line)
{
   int32_t s32_Retval = C_NO_ERR;

   if (ou32_Index < this->mc_Views.size())
   {
      C_PuiSvData & rc_View = this->mc_Views[ou32_Index];
      rc_View.SetPcConnection(orc_Line);
   }
   else
   {
      s32_Retval = C_RANGE;
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set connected state of pc in view

   \param[in]  ou32_Index     View index
   \param[in]  oq_Connected   Flag if pc is connected
   \param[in]  ou32_BusIndex  Bus index PC is connected to

   \return
   C_NO_ERR Operation success
   C_RANGE  Operation failure: parameter invalid
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_PuiSvHandler::SetViewPcConnected(const uint32_t ou32_Index, const bool oq_Connected,
                                           const uint32_t ou32_BusIndex)
{
   int32_t s32_Retval = C_NO_ERR;

   if (ou32_Index < this->mc_Views.size())
   {
      C_PuiSvData & rc_View = this->mc_Views[ou32_Index];
      rc_View.SetPcConnected(oq_Connected, ou32_BusIndex);
   }
   else
   {
      s32_Retval = C_RANGE;
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set path for the CAN DLL

   \param[in]  ou32_Index     View index
   \param[in]  oe_Type        CAN DLL Type
   \param[in]  orc_DllPath    Path for the CAN DLL

   \return
   C_NO_ERR Operation success
   C_RANGE  Operation failure: parameter invalid
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_PuiSvHandler::SetViewPcCanDll(const uint32_t ou32_Index, const C_PuiSvPc::E_CanDllType oe_Type,
                                        const QString & orc_DllPath)
{
   int32_t s32_Retval = C_RANGE;

   if (ou32_Index < this->mc_Views.size())
   {
      C_PuiSvData & rc_View = this->mc_Views[ou32_Index];
      rc_View.SetPcCanDllType(oe_Type);
      rc_View.SetPcCanDllPath(orc_DllPath);

      s32_Retval = C_NO_ERR;
   }

   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Set service mode active

   \param[in]  ou32_Index     View index
   \param[in]  oq_NewValue    New value

   \return
   C_NO_ERR Operation success
   C_RANGE  View index out of range
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_PuiSvHandler::SetViewServiceModeActive(const uint32_t ou32_Index, const bool oq_NewValue)
{
   int32_t s32_Retval = C_RANGE;

   if (ou32_Index < this->mc_Views.size())
   {
      C_PuiSvData & rc_View = this->mc_Views[ou32_Index];
      rc_View.SetServiceModeActive(oq_NewValue);
      s32_Retval = C_NO_ERR;
   }

   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Set service mode setup active

   \param[in]  ou32_Index     View index
   \param[in]  oq_NewValue    New value

   \return
   C_NO_ERR Operation success
   C_RANGE  View index out of range
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_PuiSvHandler::SetViewServiceModeSetupActive(const uint32_t ou32_Index, const bool oq_NewValue)
{
   int32_t s32_Retval = C_RANGE;

   if (ou32_Index < this->mc_Views.size())
   {
      C_PuiSvData & rc_View = this->mc_Views[ou32_Index];
      rc_View.SetServiceModeSetupActive(oq_NewValue);
      s32_Retval = C_NO_ERR;
   }

   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Set service mode update active

   \param[in]  ou32_Index     View index
   \param[in]  oq_NewValue    New value

   \return
   C_NO_ERR Operation success
   C_RANGE  View index out of range
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_PuiSvHandler::SetViewServiceModeUpdateActive(const uint32_t ou32_Index, const bool oq_NewValue)
{
   int32_t s32_Retval = C_RANGE;

   if (ou32_Index < this->mc_Views.size())
   {
      C_PuiSvData & rc_View = this->mc_Views[ou32_Index];
      rc_View.SetServiceModeUpdateActive(oq_NewValue);
      s32_Retval = C_NO_ERR;
   }

   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Set service mode dashboard active

   \param[in]  ou32_Index     View index
   \param[in]  oq_NewValue    New value

   \return
   C_NO_ERR Operation success
   C_RANGE  View index out of range
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_PuiSvHandler::SetViewServiceModeDashboardActive(const uint32_t ou32_Index, const bool oq_NewValue)
{
   int32_t s32_Retval = C_RANGE;

   if (ou32_Index < this->mc_Views.size())
   {
      C_PuiSvData & rc_View = this->mc_Views[ou32_Index];
      rc_View.SetServiceModeDashboardActive(oq_NewValue);
      s32_Retval = C_NO_ERR;
   }

   return s32_Retval;
}
//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set dashboard name

   \param[in]  ou32_ViewIndex       View index
   \param[in]  ou32_DashboardIndex  Dashboard index
   \param[in]  orc_Name             New dashboard name

   \return
   C_NO_ERR Operation success
   C_RANGE  Operation failure: parameter invalid
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_PuiSvHandler::SetDashboardName(const uint32_t ou32_ViewIndex, const uint32_t ou32_DashboardIndex,
                                         const QString & orc_Name)
{
   int32_t s32_Retval = C_NO_ERR;

   if (ou32_ViewIndex < this->mc_Views.size())
   {
      C_PuiSvData & rc_View = this->mc_Views[ou32_ViewIndex];
      s32_Retval = rc_View.SetDashboardName(ou32_DashboardIndex, orc_Name);
   }
   else
   {
      s32_Retval = C_RANGE;
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set dashboard comment

   \param[in]  ou32_ViewIndex       View index
   \param[in]  ou32_DashboardIndex  Dashboard index
   \param[in]  orc_Comment          New dashboard comment

   \return
   C_NO_ERR Operation success
   C_RANGE  Operation failure: parameter invalid
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_PuiSvHandler::SetDashboardComment(const uint32_t ou32_ViewIndex, const uint32_t ou32_DashboardIndex,
                                            const QString & orc_Comment)
{
   int32_t s32_Retval = C_NO_ERR;

   if (ou32_ViewIndex < this->mc_Views.size())
   {
      C_PuiSvData & rc_View = this->mc_Views[ou32_ViewIndex];
      s32_Retval = rc_View.SetDashboardComment(ou32_DashboardIndex, orc_Comment);
   }
   else
   {
      s32_Retval = C_RANGE;
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set dashboard type

   \param[in]  ou32_ViewIndex       View index
   \param[in]  ou32_DashboardIndex  Dashboard index
   \param[in]  oe_Type              Type

   \return
   C_NO_ERR Operation success
   C_RANGE  Operation failure: parameter invalid
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_PuiSvHandler::SetDashboardType(const uint32_t ou32_ViewIndex, const uint32_t ou32_DashboardIndex,
                                         const C_PuiSvDashboard::E_TabType oe_Type)
{
   int32_t s32_Retval = C_NO_ERR;

   if (ou32_ViewIndex < this->mc_Views.size())
   {
      C_PuiSvData & rc_View = this->mc_Views[ou32_ViewIndex];
      s32_Retval = rc_View.SetDashboardType(ou32_DashboardIndex, oe_Type);
   }
   else
   {
      s32_Retval = C_RANGE;
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set dashboard active flag

   \param[in]  ou32_ViewIndex       View index
   \param[in]  ou32_DashboardIndex  Dashboard index
   \param[in]  oq_Active            New active flag

   \return
   C_NO_ERR Operation success
   C_RANGE  Operation failure: parameter invalid
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_PuiSvHandler::SetDashboardActive(const uint32_t ou32_ViewIndex, const uint32_t ou32_DashboardIndex,
                                           const bool oq_Active)
{
   int32_t s32_Retval = C_NO_ERR;

   if (ou32_ViewIndex < this->mc_Views.size())
   {
      C_PuiSvData & rc_View = this->mc_Views[ou32_ViewIndex];
      s32_Retval = rc_View.SetDashboardActive(ou32_DashboardIndex, oq_Active);
   }
   else
   {
      s32_Retval = C_RANGE;
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set dashboard current tab index

   \param[in]  ou32_ViewIndex       View index
   \param[in]  ou32_DashboardIndex  Dashboard index
   \param[in]  os32_Value           New current tab index

   \return
   C_NO_ERR Operation success
   C_RANGE  Operation failure: parameter invalid
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_PuiSvHandler::SetDashboardTabIndex(const uint32_t ou32_ViewIndex, const uint32_t ou32_DashboardIndex,
                                             const int32_t os32_Value)
{
   int32_t s32_Retval = C_NO_ERR;

   if (ou32_ViewIndex < this->mc_Views.size())
   {
      C_PuiSvData & rc_View = this->mc_Views[ou32_ViewIndex];
      s32_Retval = rc_View.SetDashboardTabIndex(ou32_DashboardIndex, os32_Value);
   }
   else
   {
      s32_Retval = C_RANGE;
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set dashboard widget box

   \param[in]  ou32_ViewIndex       View index
   \param[in]  ou32_DashboardIndex  Dashboard index
   \param[in]  ou32_WidgetIndex     Widget index
   \param[in]  opc_Box              New widget value
   \param[in]  oe_Type              New widget type

   \return
   C_NO_ERR Operation success
   C_RANGE  Operation failure: parameter invalid
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_PuiSvHandler::SetDashboardWidget(const uint32_t ou32_ViewIndex, const uint32_t ou32_DashboardIndex,
                                           const uint32_t ou32_WidgetIndex, const C_PuiSvDbWidgetBase * const opc_Box,
                                           const C_PuiSvDbDataElement::E_Type oe_Type)
{
   int32_t s32_Retval = C_NO_ERR;

   if (ou32_ViewIndex < this->mc_Views.size())
   {
      C_PuiSvData & rc_View = this->mc_Views[ou32_ViewIndex];
      s32_Retval = rc_View.SetDashboardWidget(ou32_DashboardIndex, ou32_WidgetIndex, opc_Box, oe_Type);
   }
   else
   {
      s32_Retval = C_RANGE;
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set item

   \param[in]  ou32_ViewIndex       View index
   \param[in]  ou32_DashboardIndex  Dashboard index
   \param[in]  oru32_Index          Item index
   \param[in]  orc_Data             New value

   \return
   C_NO_ERR Operation success
   C_RANGE  Operation failure: parameter invalid
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_PuiSvHandler::SetDashboardBoundary(const uint32_t ou32_ViewIndex, const uint32_t ou32_DashboardIndex,
                                             const uint32_t & oru32_Index, const C_PuiBsBoundary & orc_Data)
{
   int32_t s32_Retval = C_NO_ERR;

   if (ou32_ViewIndex < this->mc_Views.size())
   {
      C_PuiSvData & rc_View = this->mc_Views[ou32_ViewIndex];
      s32_Retval = rc_View.SetDashboardBoundary(ou32_DashboardIndex, oru32_Index, orc_Data);
   }
   else
   {
      s32_Retval = C_RANGE;
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set item

   \param[in]  ou32_ViewIndex       View index
   \param[in]  ou32_DashboardIndex  Dashboard index
   \param[in]  oru32_Index          Item index
   \param[in]  orc_Data             New value

   \return
   C_NO_ERR Operation success
   C_RANGE  Operation failure: parameter invalid
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_PuiSvHandler::SetDashboardImage(const uint32_t ou32_ViewIndex, const uint32_t ou32_DashboardIndex,
                                          const uint32_t & oru32_Index, const C_PuiBsImage & orc_Data)
{
   int32_t s32_Retval = C_NO_ERR;

   if (ou32_ViewIndex < this->mc_Views.size())
   {
      C_PuiSvData & rc_View = this->mc_Views[ou32_ViewIndex];
      s32_Retval = rc_View.SetDashboardImage(ou32_DashboardIndex, oru32_Index, orc_Data);
   }
   else
   {
      s32_Retval = C_RANGE;
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set item

   \param[in]  ou32_ViewIndex       View index
   \param[in]  ou32_DashboardIndex  Dashboard index
   \param[in]  oru32_Index          Item index
   \param[in]  orc_Data             New value

   \return
   C_NO_ERR Operation success
   C_RANGE  Operation failure: parameter invalid
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_PuiSvHandler::SetDashboardLineArrow(const uint32_t ou32_ViewIndex, const uint32_t ou32_DashboardIndex,
                                              const uint32_t & oru32_Index, const C_PuiBsLineArrow & orc_Data)
{
   int32_t s32_Retval = C_NO_ERR;

   if (ou32_ViewIndex < this->mc_Views.size())
   {
      C_PuiSvData & rc_View = this->mc_Views[ou32_ViewIndex];
      s32_Retval = rc_View.SetDashboardLineArrow(ou32_DashboardIndex, oru32_Index, orc_Data);
   }
   else
   {
      s32_Retval = C_RANGE;
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set item

   \param[in]  ou32_ViewIndex       View index
   \param[in]  ou32_DashboardIndex  Dashboard index
   \param[in]  oru32_Index          Item index
   \param[in]  orc_Data             New value

   \return
   C_NO_ERR Operation success
   C_RANGE  Operation failure: parameter invalid
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_PuiSvHandler::SetDashboardTextElement(const uint32_t ou32_ViewIndex, const uint32_t ou32_DashboardIndex,
                                                const uint32_t & oru32_Index, const C_PuiBsTextElement & orc_Data)
{
   int32_t s32_Retval = C_NO_ERR;

   if (ou32_ViewIndex < this->mc_Views.size())
   {
      C_PuiSvData & rc_View = this->mc_Views[ou32_ViewIndex];
      s32_Retval = rc_View.SetDashboardTextElement(ou32_DashboardIndex, oru32_Index, orc_Data);
   }
   else
   {
      s32_Retval = C_RANGE;
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set fast update rate

   \param[in]  ou32_ViewIndex    View index
   \param[in]  ou16_Value        New fast update rate

   \return
   C_NO_ERR Operation success
   C_RANGE  Operation failure: parameter invalid
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_PuiSvHandler::SetViewUpdateRateFast(const uint32_t ou32_ViewIndex, const uint16_t ou16_Value)
{
   int32_t s32_Retval = C_NO_ERR;

   if (ou32_ViewIndex < this->mc_Views.size())
   {
      C_PuiSvData & rc_View = this->mc_Views[ou32_ViewIndex];
      rc_View.SetUpdateRateFast(ou16_Value);
   }
   else
   {
      s32_Retval = C_RANGE;
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set medium update rate

   \param[in]  ou32_ViewIndex    View index
   \param[in]  ou16_Value        New medium update rate

   \return
   C_NO_ERR Operation success
   C_RANGE  Operation failure: parameter invalid
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_PuiSvHandler::SetViewUpdateRateMedium(const uint32_t ou32_ViewIndex, const uint16_t ou16_Value)
{
   int32_t s32_Retval = C_NO_ERR;

   if (ou32_ViewIndex < this->mc_Views.size())
   {
      C_PuiSvData & rc_View = this->mc_Views[ou32_ViewIndex];
      rc_View.SetUpdateRateMedium(ou16_Value);
   }
   else
   {
      s32_Retval = C_RANGE;
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set slow update rate

   \param[in]  ou32_ViewIndex    View index
   \param[in]  ou16_Value        New slow update rate

   \return
   C_NO_ERR Operation success
   C_RANGE  Operation failure: parameter invalid
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_PuiSvHandler::SetViewUpdateRateSlow(const uint32_t ou32_ViewIndex, const uint16_t ou16_Value)
{
   int32_t s32_Retval = C_NO_ERR;

   if (ou32_ViewIndex < this->mc_Views.size())
   {
      C_PuiSvData & rc_View = this->mc_Views[ou32_ViewIndex];
      rc_View.SetUpdateRateSlow(ou16_Value);
   }
   else
   {
      s32_Retval = C_RANGE;
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set view device config screen selected bit rate

   \param[in]  ou32_ViewIndex    View index
   \param[in]  ou32_Value        New device config screen selected bit rate in kbit/s

   \return
   C_NO_ERR Operation success
   C_RANGE  Operation failure: parameter invalid
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_PuiSvHandler::SetViewDeviceConfigSelectedBitRate(const uint32_t ou32_ViewIndex, const uint32_t ou32_Value)
{
   int32_t s32_Retval = C_NO_ERR;

   if (ou32_ViewIndex < this->mc_Views.size())
   {
      C_PuiSvData & rc_View = this->mc_Views[ou32_ViewIndex];
      rc_View.SetDeviceConfigSelectedBitRate(ou32_Value);
   }
   else
   {
      s32_Retval = C_RANGE;
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set view device config selected mode

   \param[in]  ou32_ViewIndex    View index (identifier)
   \param[in]  oe_Value          New device config screen selected mode

   \return
   C_NO_ERR Operation success
   C_RANGE  Operation failure: parameter invalid
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_PuiSvHandler::SetViewDeviceConfigMode(const uint32_t ou32_ViewIndex,
                                                const C_PuiSvData::E_DeviceConfigurationMode oe_Value)
{
   int32_t s32_Retval = C_NO_ERR;

   if (ou32_ViewIndex < this->mc_Views.size())
   {
      C_PuiSvData & rc_View = this->mc_Views[ou32_ViewIndex];
      rc_View.SetDeviceConfigMode(oe_Value);
   }
   else
   {
      s32_Retval = C_RANGE;
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set dark mode active flag

   \param[in]  ou32_ViewIndex    View index
   \param[in]  oq_Value          New dark mode active flag

   \return
   C_NO_ERR Operation success
   C_RANGE  Operation failure: parameter invalid
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_PuiSvHandler::SetViewDarkModeActive(const uint32_t ou32_ViewIndex, const bool oq_Value)
{
   int32_t s32_Retval = C_NO_ERR;

   if (ou32_ViewIndex < this->mc_Views.size())
   {
      C_PuiSvData & rc_View = this->mc_Views[ou32_ViewIndex];
      rc_View.SetDarkModeActive(oq_Value);
   }
   else
   {
      s32_Retval = C_RANGE;
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set view read rail assignment

   \param[in]  ou32_ViewIndex    View index
   \param[in]  orc_Id            ID
   \param[in]  orc_Config        Config

   \return
   C_NO_ERR Operation success
   C_RANGE  Operation failure: parameter invalid
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_PuiSvHandler::SetViewReadRailAssignment(const uint32_t ou32_ViewIndex,
                                                  const C_OscNodeDataPoolListElementId & orc_Id,
                                                  const C_PuiSvReadDataConfiguration & orc_Config)
{
   int32_t s32_Retval = C_NO_ERR;

   if (ou32_ViewIndex < this->mc_Views.size())
   {
      C_PuiSvData & rc_View = this->mc_Views[ou32_ViewIndex];
      rc_View.SetReadRailAssignment(orc_Id, orc_Config);
   }
   else
   {
      s32_Retval = C_RANGE;
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set node update information

   \param[in]  ou32_ViewIndex             View index
   \param[in]  orc_NodeUpdateInformation  New node update information

   \return
   C_NO_ERR Operation success
   C_RANGE  Operation failure: parameter invalid
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_PuiSvHandler::SetNodeUpdateInformation(const uint32_t ou32_ViewIndex,
                                                 const std::vector<C_OscViewNodeUpdate> & orc_NodeUpdateInformation)
{
   int32_t s32_Retval = C_NO_ERR;

   if (ou32_ViewIndex < this->mc_Views.size())
   {
      C_PuiSvData & rc_View = this->mc_Views[ou32_ViewIndex];
      rc_View.SetNodeUpdateInformation(orc_NodeUpdateInformation);
   }
   else
   {
      s32_Retval = C_RANGE;
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set node update information

   \param[in]  ou32_ViewIndex             View index
   \param[in]  ou32_NodeIndex             Node index
   \param[in]  orc_NodeUpdateInformation  New node update information

   \return
   C_NO_ERR    No error
   C_RANGE     Node index invalid
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_PuiSvHandler::SetNodeUpdateInformation(const uint32_t ou32_ViewIndex, const uint32_t ou32_NodeIndex,
                                                 const C_OscViewNodeUpdate & orc_NodeUpdateInformation)
{
   int32_t s32_Retval = C_NO_ERR;

   if (ou32_ViewIndex < this->mc_Views.size())
   {
      C_PuiSvData & rc_View = this->mc_Views[ou32_ViewIndex];
      rc_View.SetNodeUpdateInformation(ou32_NodeIndex, orc_NodeUpdateInformation);
   }
   else
   {
      s32_Retval = C_RANGE;
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set node update information path

   \param[in]  ou32_ViewIndex    View index
   \param[in]  ou32_NodeIndex    Node index
   \param[in]  ou32_Index        Index to access
   \param[in]  orc_Value         New path
   \param[in]  oe_Type           Selector for structure

   \return
   C_NO_ERR Operation success
   C_RANGE  Operation failure: parameter invalid
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_PuiSvHandler::SetNodeUpdateInformationPath(const uint32_t ou32_ViewIndex, const uint32_t ou32_NodeIndex,
                                                     const uint32_t ou32_Index, const QString & orc_Value,
                                                     const C_OscViewNodeUpdate::E_GenericFileType oe_Type)
{
   int32_t s32_Retval;

   if (ou32_ViewIndex < this->mc_Views.size())
   {
      C_PuiSvData & rc_View = this->mc_Views[ou32_ViewIndex];
      s32_Retval = rc_View.SetNodeUpdateInformationPath(ou32_NodeIndex, ou32_Index,
                                                        orc_Value.toStdString().c_str(), oe_Type);
   }
   else
   {
      s32_Retval = C_RANGE;
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set node update information parameter set information

   \param[in]  ou32_ViewIndex    View index
   \param[in]  ou32_NodeIndex    Node index
   \param[in]  ou32_Index        Index to access
   \param[in]  orc_Value         New path

   \return
   C_NO_ERR Operation success
   C_RANGE  Operation failure: parameter invalid
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_PuiSvHandler::SetNodeUpdateInformationParamInfo(const uint32_t ou32_ViewIndex, const uint32_t ou32_NodeIndex,
                                                          const uint32_t ou32_Index,
                                                          const C_OscViewNodeUpdateParamInfo & orc_Value)
{
   int32_t s32_Retval;

   if (ou32_ViewIndex < this->mc_Views.size())
   {
      C_PuiSvData & rc_View = this->mc_Views[ou32_ViewIndex];
      s32_Retval = rc_View.SetNodeUpdateInformationParamInfo(ou32_NodeIndex, ou32_Index, orc_Value);
   }
   else
   {
      s32_Retval = C_RANGE;
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set node update information PEM file path

   \param[in]  ou32_ViewIndex    View index
   \param[in]  ou32_NodeIndex    Node index
   \param[in]  orc_Value         New path

   \return
   C_NO_ERR Operation success
   C_RANGE  Operation failure: parameter invalid
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_PuiSvHandler::SetNodeUpdateInformationPemFilePath(const uint32_t ou32_ViewIndex,
                                                            const uint32_t ou32_NodeIndex, const QString & orc_Value)
{
   int32_t s32_Retval;

   if (ou32_ViewIndex < this->mc_Views.size())
   {
      C_PuiSvData & rc_View = this->mc_Views[ou32_ViewIndex];
      s32_Retval = rc_View.SetNodeUpdateInformationPemFilePath(ou32_NodeIndex, orc_Value.toStdString().c_str());
   }
   else
   {
      s32_Retval = C_RANGE;
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set node update information skip flag

   \param[in]  ou32_ViewIndex    View index
   \param[in]  ou32_NodeIndex    Node index
   \param[in]  ou32_Index        Index to access
   \param[in]  oq_SkipFile       New skip flag
   \param[in]  oe_Type           Selector for structure

   \return
   C_NO_ERR Operation success
   C_RANGE  Operation failure: parameter invalid
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_PuiSvHandler::SetNodeUpdateInformationSkipUpdateOfPath(const uint32_t ou32_ViewIndex,
                                                                 const uint32_t ou32_NodeIndex,
                                                                 const uint32_t ou32_Index, const bool oq_SkipFile,
                                                                 const C_OscViewNodeUpdate::E_GenericFileType oe_Type)
{
   int32_t s32_Retval;

   if (ou32_ViewIndex < this->mc_Views.size())
   {
      C_PuiSvData & rc_View = this->mc_Views[ou32_ViewIndex];
      s32_Retval = rc_View.SetNodeUpdateInformationSkipUpdateOfPath(ou32_NodeIndex, ou32_Index, oq_SkipFile, oe_Type);
   }
   else
   {
      s32_Retval = C_RANGE;
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set node update information parameter set skip flag

   \param[in]  ou32_ViewIndex    View index
   \param[in]  ou32_NodeIndex    Node index
   \param[in]  ou32_Index        Index to access
   \param[in]  oq_SkipFile       New skip flag

   \return
   C_NO_ERR Operation success
   C_RANGE  Operation failure: parameter invalid
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_PuiSvHandler::SetNodeUpdateInformationSkipUpdateOfParamInfo(const uint32_t ou32_ViewIndex,
                                                                      const uint32_t ou32_NodeIndex,
                                                                      const uint32_t ou32_Index, const bool oq_SkipFile)
{
   int32_t s32_Retval;

   if (ou32_ViewIndex < this->mc_Views.size())
   {
      C_PuiSvData & rc_View = this->mc_Views[ou32_ViewIndex];
      s32_Retval = rc_View.SetNodeUpdateInformationSkipUpdateOfParamInfo(ou32_NodeIndex, ou32_Index, oq_SkipFile);
   }
   else
   {
      s32_Retval = C_RANGE;
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set node update information PEM file skip flag

   \param[in]  ou32_ViewIndex    View index
   \param[in]  ou32_NodeIndex    Node index
   \param[in]  oq_SkipFile       New skip flag

   \return
   C_NO_ERR Operation success
   C_RANGE  Operation failure: parameter invalid
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_PuiSvHandler::SetNodeUpdateInformationSkipUpdateOfPemFile(const uint32_t ou32_ViewIndex,
                                                                    const uint32_t ou32_NodeIndex,
                                                                    const bool oq_SkipFile)
{
   int32_t s32_Retval;

   if (ou32_ViewIndex < this->mc_Views.size())
   {
      C_PuiSvData & rc_View = this->mc_Views[ou32_ViewIndex];
      s32_Retval = rc_View.SetNodeUpdateInformationSkipUpdateOfPemFile(ou32_NodeIndex, oq_SkipFile);
   }
   else
   {
      s32_Retval = C_RANGE;
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set node update information states

   \param[in]  ou32_ViewIndex    View index
   \param[in]  ou32_NodeIndex    Node index
   \param[in]  oe_StateSecurity  Security state of node
   \param[in]  oe_StateDebugger  Debugger state of node

   \return
   C_NO_ERR Operation success
   C_RANGE  Operation failure: parameter invalid
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_PuiSvHandler::SetNodeUpdateInformationStates(const uint32_t ou32_ViewIndex, const uint32_t ou32_NodeIndex,
                                                       const C_OscViewNodeUpdate::E_StateSecurity oe_StateSecurity,
                                                       const C_OscViewNodeUpdate::E_StateDebugger oe_StateDebugger)
{
   int32_t s32_Retval;

   if (ou32_ViewIndex < this->mc_Views.size())
   {
      C_PuiSvData & rc_View = this->mc_Views[ou32_ViewIndex];
      s32_Retval = rc_View.SetNodeUpdateInformationStates(ou32_NodeIndex, oe_StateSecurity, oe_StateDebugger);
   }
   else
   {
      s32_Retval = C_RANGE;
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set node update information parameter set information

   \param[in]  ou32_ViewIndex       View index
   \param[in]  ou32_NodeIndex       Node index
   \param[in]  ou32_Index           Index to access
   \param[in]  orc_FilePath         New path
   \param[in]  ou32_LastKnownCrc    Last known CRC for this file

   \return
   C_NO_ERR Operation success
   C_RANGE  Operation failure: parameter invalid
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_PuiSvHandler::SetNodeUpdateInformationParamInfoContent(const uint32_t ou32_ViewIndex,
                                                                 const uint32_t ou32_NodeIndex,
                                                                 const uint32_t ou32_Index,
                                                                 const QString & orc_FilePath,
                                                                 const uint32_t ou32_LastKnownCrc)
{
   int32_t s32_Retval;

   if (ou32_ViewIndex < this->mc_Views.size())
   {
      C_PuiSvData & rc_View = this->mc_Views[ou32_ViewIndex];
      s32_Retval =
         rc_View.SetNodeUpdateInformationParamInfoContent(ou32_NodeIndex, ou32_Index,
                                                          orc_FilePath.toStdString().c_str(), ou32_LastKnownCrc);
   }
   else
   {
      s32_Retval = C_RANGE;
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Add view

   \param[in]  orc_View             View item
   \param[in]  oq_AutoAdaptName     Optional flag to allow or block automatic adaption of input name
   \param[in]  oq_AutoAdaptContent  Optional flag to allow or block automatic adaption of input content
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PuiSvHandler::AddView(const C_PuiSvData & orc_View, const bool oq_AutoAdaptName, const bool oq_AutoAdaptContent)
{
   tgl_assert(this->InsertView(this->mc_Views.size(), orc_View, oq_AutoAdaptName, oq_AutoAdaptContent) == C_NO_ERR);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set view

   The previous view will be overwritten

   \param[in]  ou32_Index  View index
   \param[in]  orc_View    View item

   \return
   C_NO_ERR Operation success
   C_RANGE  Operation failure: parameter invalid
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_PuiSvHandler::SetView(const uint32_t ou32_Index, const C_PuiSvData & orc_View)
{
   int32_t s32_Retval = C_RANGE;

   if (ou32_Index <= this->mc_Views.size())
   {
      this->mc_Views[ou32_Index] = orc_View;

      s32_Retval = C_NO_ERR;
   }

   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Insert view

   \param[in]  ou32_Index           View index
   \param[in]  orc_View             View item
   \param[in]  oq_AutoAdaptName     Optional flag to allow or block automatic adaption of input name
   \param[in]  oq_AutoAdaptContent  Optional flag to allow or block automatic adaption of input content

   \return
   C_NO_ERR Operation success
   C_RANGE  Operation failure: parameter invalid
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_PuiSvHandler::InsertView(const uint32_t ou32_Index, const C_PuiSvData & orc_View, const bool oq_AutoAdaptName,
                                   const bool oq_AutoAdaptContent)
{
   int32_t s32_Retval = C_NO_ERR;

   if (ou32_Index <= this->mc_Views.size())
   {
      //Init
      C_PuiSvData c_TmpView = orc_View;
      if (oq_AutoAdaptContent == true)
      {
         c_TmpView.InitFromSystemDefinition();
      }
      if (oq_AutoAdaptName == true)
      {
         c_TmpView.SetName(C_Uti::h_GetUniqueNameQt(this->m_GetExistingViewNames(),
                                                    c_TmpView.GetName().c_str()).toStdString().c_str());
      }
      this->mc_Views.insert(this->mc_Views.begin() + ou32_Index, c_TmpView);
   }
   else
   {
      s32_Retval = C_RANGE;
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Add new read rail item to configuration

   \param[in]  ou32_ViewIndex    View index
   \param[in]  orc_Id            ID
   \param[in]  orc_Config        Configuration

   \return
   C_NO_ERR Operation success
   C_RANGE  Operation failure: parameter invalid
   C_NOACT  Already exists
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_PuiSvHandler::AddViewReadRailItem(const uint32_t ou32_ViewIndex,
                                            const C_OscNodeDataPoolListElementId & orc_Id,
                                            const C_PuiSvReadDataConfiguration & orc_Config)
{
   int32_t s32_Retval = C_NO_ERR;

   if (ou32_ViewIndex < this->mc_Views.size())
   {
      C_PuiSvData & rc_View = this->mc_Views[ou32_ViewIndex];
      s32_Retval = rc_View.AddReadRailItem(orc_Id, orc_Config);
   }
   else
   {
      s32_Retval = C_RANGE;
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Add node update information path

   \param[in]  ou32_ViewIndex    View index
   \param[in]  ou32_NodeIndex    Node index
   \param[in]  orc_Value         New path
   \param[in]  oe_Type           Selector for structure

   \return
   C_NO_ERR Operation success
   C_RANGE  Operation failure: parameter invalid
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_PuiSvHandler::AddNodeUpdateInformationPath(const uint32_t ou32_ViewIndex, const uint32_t ou32_NodeIndex,
                                                     const QString & orc_Value,
                                                     const C_OscViewNodeUpdate::E_GenericFileType oe_Type)
{
   int32_t s32_Retval;

   if (ou32_ViewIndex < this->mc_Views.size())
   {
      C_PuiSvData & rc_View = this->mc_Views[ou32_ViewIndex];
      s32_Retval = rc_View.AddNodeUpdateInformationPath(ou32_NodeIndex, orc_Value.toStdString().c_str(), oe_Type);
   }
   else
   {
      s32_Retval = C_RANGE;
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Add node update information for a parameter set

   \param[in]  ou32_ViewIndex    View index
   \param[in]  ou32_NodeIndex    Node index
   \param[in]  orc_Value         New path

   \return
   C_NO_ERR Operation success
   C_RANGE  Operation failure: parameter invalid
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_PuiSvHandler::AddNodeUpdateInformationParamInfo(const uint32_t ou32_ViewIndex, const uint32_t ou32_NodeIndex,
                                                          const C_OscViewNodeUpdateParamInfo & orc_Value)
{
   int32_t s32_Retval;

   if (ou32_ViewIndex < this->mc_Views.size())
   {
      C_PuiSvData & rc_View = this->mc_Views[ou32_ViewIndex];
      s32_Retval = rc_View.AddNodeUpdateInformationParamInfo(ou32_NodeIndex, orc_Value);
   }
   else
   {
      s32_Retval = C_RANGE;
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Add dashboard to view

   \param[in]  ou32_ViewIndex    View index
   \param[in]  orc_Dashboard     Dashboard data
   \param[in]  oq_AutoAdapt      Auto adapt name flag

   \return
   C_NO_ERR Operation success
   C_RANGE  Operation failure: parameter invalid
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_PuiSvHandler::AddDashboard(const uint32_t ou32_ViewIndex, const C_PuiSvDashboard & orc_Dashboard,
                                     const bool oq_AutoAdapt)
{
   int32_t s32_Retval = C_NO_ERR;

   if (ou32_ViewIndex < this->mc_Views.size())
   {
      C_PuiSvData & rc_View = this->mc_Views[ou32_ViewIndex];
      rc_View.AddDashboard(orc_Dashboard, oq_AutoAdapt);
   }
   else
   {
      s32_Retval = C_RANGE;
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Insert dashboard to view

   \param[in]  ou32_ViewIndex       View index
   \param[in]  ou32_DashboardIndex  Dashboard index
   \param[in]  orc_Dashboard        Dashboard data
   \param[in]  oq_AutoAdapt         Auto adapt name flag
   \param[in]  opc_Rails            Read rail item configuration

   \return
   C_NO_ERR Operation success
   C_RANGE  Operation failure: parameter invalid
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_PuiSvHandler::InsertDashboard(const uint32_t ou32_ViewIndex, const uint32_t ou32_DashboardIndex,
                                        const C_PuiSvDashboard & orc_Dashboard, const bool oq_AutoAdapt,
                                        const QMap<C_OscNodeDataPoolListElementId,
                                                   C_PuiSvReadDataConfiguration> * const opc_Rails)
{
   int32_t s32_Retval = C_NO_ERR;

   if (ou32_ViewIndex < this->mc_Views.size())
   {
      C_PuiSvData & rc_View = this->mc_Views[ou32_ViewIndex];
      s32_Retval = rc_View.InsertDashboard(ou32_DashboardIndex, orc_Dashboard, oq_AutoAdapt, opc_Rails);
   }
   else
   {
      s32_Retval = C_RANGE;
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Add widget to view dashboard

   \param[in]  ou32_ViewIndex       View index
   \param[in]  ou32_DashboardIndex  Dashboard index
   \param[in]  opc_Box              New widget value
   \param[in]  oe_Type              New widget type

   \return
   C_NO_ERR Operation success
   C_RANGE  Operation failure: parameter invalid
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_PuiSvHandler::AddDashboardWidget(const uint32_t ou32_ViewIndex, const uint32_t ou32_DashboardIndex,
                                           const C_PuiSvDbWidgetBase * const opc_Box,
                                           const C_PuiSvDbDataElement::E_Type oe_Type)
{
   int32_t s32_Retval = C_NO_ERR;

   if (ou32_ViewIndex < this->mc_Views.size())
   {
      C_PuiSvData & rc_View = this->mc_Views[ou32_ViewIndex];
      s32_Retval = rc_View.AddDashboardWidget(ou32_DashboardIndex, opc_Box, oe_Type);
   }
   else
   {
      s32_Retval = C_RANGE;
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Insert widget to view dashboard

   \param[in]  ou32_ViewIndex       View index
   \param[in]  ou32_DashboardIndex  Dashboard index
   \param[in]  ou32_WidgetIndex     Widget index
   \param[in]  opc_Box              New widget value
   \param[in]  oe_Type              New widget type

   \return
   C_NO_ERR Operation success
   C_RANGE  Operation failure: parameter invalid
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_PuiSvHandler::InsertDashboardWidget(const uint32_t ou32_ViewIndex, const uint32_t ou32_DashboardIndex,
                                              const uint32_t ou32_WidgetIndex,
                                              const C_PuiSvDbWidgetBase * const opc_Box,
                                              const C_PuiSvDbDataElement::E_Type oe_Type)
{
   int32_t s32_Retval = C_NO_ERR;

   if (ou32_ViewIndex < this->mc_Views.size())
   {
      C_PuiSvData & rc_View = this->mc_Views[ou32_ViewIndex];
      s32_Retval = rc_View.InsertDashboardWidget(ou32_DashboardIndex, ou32_WidgetIndex, opc_Box, oe_Type);
   }
   else
   {
      s32_Retval = C_RANGE;
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Add new item

   \param[in]  ou32_ViewIndex       View index
   \param[in]  ou32_DashboardIndex  Dashboard index
   \param[in]  orc_Data             New value

   \return
   C_NO_ERR Operation success
   C_RANGE  Operation failure: parameter invalid
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_PuiSvHandler::AddDashboardBoundary(const uint32_t ou32_ViewIndex, const uint32_t ou32_DashboardIndex,
                                             const C_PuiBsBoundary & orc_Data)
{
   int32_t s32_Retval = C_NO_ERR;

   if (ou32_ViewIndex < this->mc_Views.size())
   {
      C_PuiSvData & rc_View = this->mc_Views[ou32_ViewIndex];
      s32_Retval = rc_View.AddDashboardBoundary(ou32_DashboardIndex, orc_Data);
   }
   else
   {
      s32_Retval = C_RANGE;
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Insert new item

   \param[in]  ou32_ViewIndex       View index
   \param[in]  ou32_DashboardIndex  Dashboard index
   \param[in]  oru32_Index          Item index
   \param[in]  orc_Data             New value

   \return
   C_NO_ERR Operation success
   C_RANGE  Operation failure: parameter invalid
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_PuiSvHandler::InsertDashboardBoundary(const uint32_t ou32_ViewIndex, const uint32_t ou32_DashboardIndex,
                                                const uint32_t & oru32_Index, const C_PuiBsBoundary & orc_Data)
{
   int32_t s32_Retval = C_NO_ERR;

   if (ou32_ViewIndex < this->mc_Views.size())
   {
      C_PuiSvData & rc_View = this->mc_Views[ou32_ViewIndex];
      s32_Retval = rc_View.InsertDashboardBoundary(ou32_DashboardIndex, oru32_Index, orc_Data);
   }
   else
   {
      s32_Retval = C_RANGE;
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Add new item

   \param[in]  ou32_ViewIndex       View index
   \param[in]  ou32_DashboardIndex  Dashboard index
   \param[in]  orc_Data             New value

   \return
   C_NO_ERR Operation success
   C_RANGE  Operation failure: parameter invalid
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_PuiSvHandler::AddDashboardImage(const uint32_t ou32_ViewIndex, const uint32_t ou32_DashboardIndex,
                                          const C_PuiBsImage & orc_Data)
{
   int32_t s32_Retval = C_NO_ERR;

   if (ou32_ViewIndex < this->mc_Views.size())
   {
      C_PuiSvData & rc_View = this->mc_Views[ou32_ViewIndex];
      s32_Retval = rc_View.AddDashboardImage(ou32_DashboardIndex, orc_Data);
   }
   else
   {
      s32_Retval = C_RANGE;
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Insert new item

   \param[in]  ou32_ViewIndex       View index
   \param[in]  ou32_DashboardIndex  Dashboard index
   \param[in]  oru32_Index          Item index
   \param[in]  orc_Data             New value

   \return
   C_NO_ERR Operation success
   C_RANGE  Operation failure: parameter invalid
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_PuiSvHandler::InsertDashboardImage(const uint32_t ou32_ViewIndex, const uint32_t ou32_DashboardIndex,
                                             const uint32_t & oru32_Index, const C_PuiBsImage & orc_Data)
{
   int32_t s32_Retval = C_NO_ERR;

   if (ou32_ViewIndex < this->mc_Views.size())
   {
      C_PuiSvData & rc_View = this->mc_Views[ou32_ViewIndex];
      s32_Retval = rc_View.InsertDashboardImage(ou32_DashboardIndex, oru32_Index, orc_Data);
   }
   else
   {
      s32_Retval = C_RANGE;
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Add new item

   \param[in]  ou32_ViewIndex       View index
   \param[in]  ou32_DashboardIndex  Dashboard index
   \param[in]  orc_Data             New value

   \return
   C_NO_ERR Operation success
   C_RANGE  Operation failure: parameter invalid
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_PuiSvHandler::AddDashboardLineArrow(const uint32_t ou32_ViewIndex, const uint32_t ou32_DashboardIndex,
                                              const C_PuiBsLineArrow & orc_Data)
{
   int32_t s32_Retval = C_NO_ERR;

   if (ou32_ViewIndex < this->mc_Views.size())
   {
      C_PuiSvData & rc_View = this->mc_Views[ou32_ViewIndex];
      s32_Retval = rc_View.AddDashboardLineArrow(ou32_DashboardIndex, orc_Data);
   }
   else
   {
      s32_Retval = C_RANGE;
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Insert new item

   \param[in]  ou32_ViewIndex       View index
   \param[in]  ou32_DashboardIndex  Dashboard index
   \param[in]  oru32_Index          Item index
   \param[in]  orc_Data             New value

   \return
   C_NO_ERR Operation success
   C_RANGE  Operation failure: parameter invalid
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_PuiSvHandler::InsertDashboardLineArrow(const uint32_t ou32_ViewIndex, const uint32_t ou32_DashboardIndex,
                                                 const uint32_t & oru32_Index, const C_PuiBsLineArrow & orc_Data)
{
   int32_t s32_Retval = C_NO_ERR;

   if (ou32_ViewIndex < this->mc_Views.size())
   {
      C_PuiSvData & rc_View = this->mc_Views[ou32_ViewIndex];
      s32_Retval = rc_View.InsertDashboardLineArrow(ou32_DashboardIndex, oru32_Index, orc_Data);
   }
   else
   {
      s32_Retval = C_RANGE;
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Add new item

   \param[in]  ou32_ViewIndex       View index
   \param[in]  ou32_DashboardIndex  Dashboard index
   \param[in]  orc_Data             New value

   \return
   C_NO_ERR Operation success
   C_RANGE  Operation failure: parameter invalid
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_PuiSvHandler::AddDashboardTextElement(const uint32_t ou32_ViewIndex, const uint32_t ou32_DashboardIndex,
                                                const C_PuiBsTextElement & orc_Data)
{
   int32_t s32_Retval = C_NO_ERR;

   if (ou32_ViewIndex < this->mc_Views.size())
   {
      C_PuiSvData & rc_View = this->mc_Views[ou32_ViewIndex];
      s32_Retval = rc_View.AddDashboardTextElement(ou32_DashboardIndex, orc_Data);
   }
   else
   {
      s32_Retval = C_RANGE;
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Insert new item

   \param[in]  ou32_ViewIndex       View index
   \param[in]  ou32_DashboardIndex  Dashboard index
   \param[in]  oru32_Index          Item index
   \param[in]  orc_Data             New value

   \return
   C_NO_ERR Operation success
   C_RANGE  Operation failure: parameter invalid
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_PuiSvHandler::InsertDashboardTextElement(const uint32_t ou32_ViewIndex, const uint32_t ou32_DashboardIndex,
                                                   const uint32_t & oru32_Index, const C_PuiBsTextElement & orc_Data)
{
   int32_t s32_Retval = C_NO_ERR;

   if (ou32_ViewIndex < this->mc_Views.size())
   {
      C_PuiSvData & rc_View = this->mc_Views[ou32_ViewIndex];
      s32_Retval = rc_View.InsertDashboardTextElement(ou32_DashboardIndex, oru32_Index, orc_Data);
   }
   else
   {
      s32_Retval = C_RANGE;
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Delete view

   \param[in]  ou32_Index  View index

   \return
   C_NO_ERR Operation success
   C_RANGE  Operation failure: parameter invalid
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_PuiSvHandler::DeleteView(const uint32_t ou32_Index)
{
   int32_t s32_Retval = C_NO_ERR;

   if (ou32_Index < this->mc_Views.size())
   {
      this->mc_Views.erase(this->mc_Views.begin() + ou32_Index);
   }
   else
   {
      s32_Retval = C_RANGE;
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Remove read rail item configuration

   WARNING: No check if still in use

   \param[in]  ou32_ViewIndex    View index
   \param[in]  orc_Id            ID

   \return
   C_NO_ERR Operation success
   C_RANGE  Operation failure: parameter invalid
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_PuiSvHandler::RemoveViewReadRailItem(const uint32_t ou32_ViewIndex,
                                               const C_OscNodeDataPoolListElementId & orc_Id)
{
   int32_t s32_Retval = C_NO_ERR;

   if (ou32_ViewIndex < this->mc_Views.size())
   {
      C_PuiSvData & rc_View = this->mc_Views[ou32_ViewIndex];
      s32_Retval = rc_View.RemoveReadRailItem(orc_Id);
   }
   else
   {
      s32_Retval = C_RANGE;
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Remove node update information path

   \param[in]  ou32_ViewIndex    View index
   \param[in]  ou32_NodeIndex    Node index
   \param[in]  ou32_Index        Index to remove
   \param[in]  oe_Type           Selector for structure

   \return
   C_NO_ERR Operation success
   C_RANGE  Operation failure: parameter invalid
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_PuiSvHandler::RemoveNodeUpdateInformationPath(const uint32_t ou32_ViewIndex, const uint32_t ou32_NodeIndex,
                                                        const uint32_t ou32_Index,
                                                        const C_OscViewNodeUpdate::E_GenericFileType oe_Type)
{
   int32_t s32_Retval;

   if (ou32_ViewIndex < this->mc_Views.size())
   {
      C_PuiSvData & rc_View = this->mc_Views[ou32_ViewIndex];
      s32_Retval = rc_View.RemoveNodeUpdateInformationPath(ou32_NodeIndex, ou32_Index, oe_Type);
   }
   else
   {
      s32_Retval = C_RANGE;
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Remove node update information parameter set information

   \param[in]  ou32_ViewIndex    View index
   \param[in]  ou32_NodeIndex    Node index
   \param[in]  ou32_Index        Index to remove

   \return
   C_NO_ERR Operation success
   C_RANGE  Operation failure: parameter invalid
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_PuiSvHandler::RemoveNodeUpdateInformationParamInfo(const uint32_t ou32_ViewIndex,
                                                             const uint32_t ou32_NodeIndex, const uint32_t ou32_Index)
{
   int32_t s32_Retval;

   if (ou32_ViewIndex < this->mc_Views.size())
   {
      C_PuiSvData & rc_View = this->mc_Views[ou32_ViewIndex];
      s32_Retval = rc_View.RemoveNodeUpdateInformationParamInfo(ou32_NodeIndex, ou32_Index);
   }
   else
   {
      s32_Retval = C_RANGE;
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Remove node update information PEM file path

   \param[in]  ou32_ViewIndex    View index
   \param[in]  ou32_NodeIndex    Node index

   \return
   C_NO_ERR Operation success
   C_RANGE  Operation failure: parameter invalid
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_PuiSvHandler::RemoveNodeUpdateInformationPemFilePath(const uint32_t ou32_ViewIndex,
                                                               const uint32_t ou32_NodeIndex)
{
   int32_t s32_Retval;

   if (ou32_ViewIndex < this->mc_Views.size())
   {
      C_PuiSvData & rc_View = this->mc_Views[ou32_ViewIndex];
      s32_Retval = rc_View.RemoveNodeUpdateInformationPemFilePath(ou32_NodeIndex);
   }
   else
   {
      s32_Retval = C_RANGE;
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Clear all node update information paths as appropriate for the type

   \param[in]  ou32_ViewIndex    View index
   \param[in]  ou32_NodeIndex    Node index
   \param[in]  oe_Type           Selector for structure

   \return
   C_NO_ERR Operation success
   C_RANGE  Operation failure: parameter invalid
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_PuiSvHandler::ClearNodeUpdateInformationAsAppropriate(const uint32_t ou32_ViewIndex,
                                                                const uint32_t ou32_NodeIndex,
                                                                const C_OscViewNodeUpdate::E_GenericFileType oe_Type)
{
   int32_t s32_Retval;

   if (ou32_ViewIndex < this->mc_Views.size())
   {
      C_PuiSvData & rc_View = this->mc_Views[ou32_ViewIndex];
      s32_Retval = rc_View.ClearNodeUpdateInformationAsAppropriate(ou32_NodeIndex, oe_Type);
   }
   else
   {
      s32_Retval = C_RANGE;
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Clear all node update information parameter set paths for this node

   \param[in]  ou32_ViewIndex    View index
   \param[in]  ou32_NodeIndex    Node index

   \return
   C_NO_ERR Operation success
   C_RANGE  Operation failure: parameter invalid
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_PuiSvHandler::ClearNodeUpdateInformationParamPaths(const uint32_t ou32_ViewIndex,
                                                             const uint32_t ou32_NodeIndex)
{
   int32_t s32_Retval;

   if (ou32_ViewIndex < this->mc_Views.size())
   {
      C_PuiSvData & rc_View = this->mc_Views[ou32_ViewIndex];
      s32_Retval = rc_View.ClearNodeUpdateInformationParamPaths(ou32_NodeIndex);
   }
   else
   {
      s32_Retval = C_RANGE;
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Delete dashboard from view

   \param[in]  ou32_ViewIndex       View index
   \param[in]  ou32_DashboardIndex  Dashboard index

   \return
   C_NO_ERR Operation success
   C_RANGE  Operation failure: parameter invalid
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_PuiSvHandler::DeleteDashboard(const uint32_t ou32_ViewIndex, const uint32_t ou32_DashboardIndex)
{
   int32_t s32_Retval = C_NO_ERR;

   if (ou32_ViewIndex < this->mc_Views.size())
   {
      C_PuiSvData & rc_View = this->mc_Views[ou32_ViewIndex];
      s32_Retval = rc_View.DeleteDashboard(ou32_DashboardIndex);
   }
   else
   {
      s32_Retval = C_RANGE;
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Delete widget from view dashboard

   \param[in]  ou32_ViewIndex       View index
   \param[in]  ou32_DashboardIndex  Dashboard index
   \param[in]  ou32_WidgetIndex     Widget index
   \param[in]  oe_Type              New widget type

   \return
   C_NO_ERR Operation success
   C_RANGE  Operation failure: parameter invalid
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_PuiSvHandler::DeleteDashboardWidget(const uint32_t ou32_ViewIndex, const uint32_t ou32_DashboardIndex,
                                              const uint32_t ou32_WidgetIndex,
                                              const C_PuiSvDbDataElement::E_Type oe_Type)
{
   int32_t s32_Retval = C_NO_ERR;

   if (ou32_ViewIndex < this->mc_Views.size())
   {
      C_PuiSvData & rc_View = this->mc_Views[ou32_ViewIndex];
      s32_Retval = rc_View.DeleteDashboardWidget(ou32_DashboardIndex, ou32_WidgetIndex, oe_Type);
   }
   else
   {
      s32_Retval = C_RANGE;
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Delete item

   \param[in]  ou32_ViewIndex       View index
   \param[in]  ou32_DashboardIndex  Dashboard index
   \param[in]  oru32_Index          Item index

   \return
   C_NO_ERR Operation success
   C_RANGE  Operation failure: parameter invalid
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_PuiSvHandler::DeleteDashboardBoundary(const uint32_t ou32_ViewIndex, const uint32_t ou32_DashboardIndex,
                                                const uint32_t & oru32_Index)
{
   int32_t s32_Retval = C_NO_ERR;

   if (ou32_ViewIndex < this->mc_Views.size())
   {
      C_PuiSvData & rc_View = this->mc_Views[ou32_ViewIndex];
      s32_Retval = rc_View.DeleteDashboardBoundary(ou32_DashboardIndex, oru32_Index);
   }
   else
   {
      s32_Retval = C_RANGE;
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Delete item

   \param[in]  ou32_ViewIndex       View index
   \param[in]  ou32_DashboardIndex  Dashboard index
   \param[in]  oru32_Index          Item index

   \return
   C_NO_ERR Operation success
   C_RANGE  Operation failure: parameter invalid
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_PuiSvHandler::DeleteDashboardImage(const uint32_t ou32_ViewIndex, const uint32_t ou32_DashboardIndex,
                                             const uint32_t & oru32_Index)
{
   int32_t s32_Retval = C_NO_ERR;

   if (ou32_ViewIndex < this->mc_Views.size())
   {
      C_PuiSvData & rc_View = this->mc_Views[ou32_ViewIndex];
      s32_Retval = rc_View.DeleteDashboardImage(ou32_DashboardIndex, oru32_Index);
   }
   else
   {
      s32_Retval = C_RANGE;
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Delete item

   \param[in]  ou32_ViewIndex       View index
   \param[in]  ou32_DashboardIndex  Dashboard index
   \param[in]  oru32_Index          Item index

   \return
   C_NO_ERR Operation success
   C_RANGE  Operation failure: parameter invalid
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_PuiSvHandler::DeleteDashboardLineArrow(const uint32_t ou32_ViewIndex, const uint32_t ou32_DashboardIndex,
                                                 const uint32_t & oru32_Index)
{
   int32_t s32_Retval = C_NO_ERR;

   if (ou32_ViewIndex < this->mc_Views.size())
   {
      C_PuiSvData & rc_View = this->mc_Views[ou32_ViewIndex];
      s32_Retval = rc_View.DeleteDashboardLineArrow(ou32_DashboardIndex, oru32_Index);
   }
   else
   {
      s32_Retval = C_RANGE;
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Delete item

   \param[in]  ou32_ViewIndex       View index
   \param[in]  ou32_DashboardIndex  Dashboard index
   \param[in]  oru32_Index          Item index

   \return
   C_NO_ERR Operation success
   C_RANGE  Operation failure: parameter invalid
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_PuiSvHandler::DeleteDashboardTextElement(const uint32_t ou32_ViewIndex, const uint32_t ou32_DashboardIndex,
                                                   const uint32_t & oru32_Index)
{
   int32_t s32_Retval = C_NO_ERR;

   if (ou32_ViewIndex < this->mc_Views.size())
   {
      C_PuiSvData & rc_View = this->mc_Views[ou32_ViewIndex];
      s32_Retval = rc_View.DeleteDashboardTextElement(ou32_DashboardIndex, oru32_Index);
   }
   else
   {
      s32_Retval = C_RANGE;
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Clear system views
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PuiSvHandler::Clear(void)
{
   this->mc_Views.clear();
   this->mc_LastKnownHalcCrcs.clear();

   this->mc_SdNodeErrors.clear();
   this->mc_SdBusErrors.clear();
   this->mq_IsServiceModeActive = false;

   //Reset hash
   this->mu32_CalculatedHashSystemViews = this->m_CalcHashSystemViews();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Move view

   \param[in]  ou32_StartIndex   Start index
   \param[in]  ou32_TargetIndex  Target index

   \return
   C_NO_ERR Operation success
   C_RANGE  Operation failure: parameter invalid
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_PuiSvHandler::MoveView(const uint32_t ou32_StartIndex, const uint32_t ou32_TargetIndex)
{
   int32_t s32_Retval;
   const C_PuiSvData * const pc_View = this->GetView(ou32_StartIndex);

   if (pc_View != NULL)
   {
      const C_PuiSvData c_View = *pc_View;
      s32_Retval = this->DeleteView(ou32_StartIndex);
      if (s32_Retval == C_NO_ERR)
      {
         s32_Retval = this->InsertView(ou32_TargetIndex, c_View, false, false);
      }
   }
   else
   {
      s32_Retval = C_RANGE;
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Synchronise internally stored scaling information with current system definition

   \param[in]  ou32_ViewIndex       View index
   \param[in]  ou32_DashboardIndex  Dashboard index

   \return
   C_NO_ERR Operation success
   C_RANGE  Operation failure: parameter invalid
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_PuiSvHandler::SyncDashboardScalingInformation(const uint32_t ou32_ViewIndex,
                                                        const uint32_t ou32_DashboardIndex)
{
   int32_t s32_Retval = C_NO_ERR;

   if (ou32_ViewIndex < this->mc_Views.size())
   {
      C_PuiSvData & rc_View = this->mc_Views[ou32_ViewIndex];
      s32_Retval = rc_View.SyncDashboardScalingInformation(ou32_DashboardIndex);
   }
   else
   {
      s32_Retval = C_RANGE;
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Update system definition errors
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PuiSvHandler::UpdateSystemDefintionErrors(void)
{
   const uint32_t u32_SysDefHash = C_PuiSdHandler::h_GetInstance()->CalcHashSystemDefinition();

   if (u32_SysDefHash != this->mu32_PreviousSystemDefintionHash)
   {
      //Clear! Important!
      this->mc_SdNodeErrors.clear();
      this->mc_SdBusErrors.clear();
      //Node error
      this->mc_SdNodeErrors.reserve(C_PuiSdHandler::h_GetInstance()->GetOscNodesSize());
      for (uint32_t u32_ItNode = 0; u32_ItNode < C_PuiSdHandler::h_GetInstance()->GetOscNodesSize(); ++u32_ItNode)
      {
         const bool q_Error = C_PuiSdHandler::h_GetInstance()->CheckNodeConflict(u32_ItNode);
         this->mc_SdNodeErrors.push_back(q_Error);
      }
      //Bus error
      this->mc_SdBusErrors.reserve(C_PuiSdHandler::h_GetInstance()->GetOscBusesSize());
      for (uint32_t u32_ItBus = 0; u32_ItBus < C_PuiSdHandler::h_GetInstance()->GetOscBusesSize(); ++u32_ItBus)
      {
         const bool q_Error = C_PuiSdHandler::h_GetInstance()->CheckBusConflict(u32_ItBus);
         this->mc_SdBusErrors.push_back(q_Error);
      }
      //Clear all previous view errors (might have changed with new SD)
      this->mc_PreviousErrorCheckResults.clear();
      this->mc_PreviousNodeActiveFlagsWithSquadAdaptionsResults.clear();
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get buffered error for node

   \param[in]  ou32_Index  Index

   \return
   Buffered error for node
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_PuiSvHandler::GetErrorNode(const uint32_t ou32_Index) const
{
   bool q_Retval = true;

   if (ou32_Index < this->mc_SdNodeErrors.size())
   {
      q_Retval = this->mc_SdNodeErrors[ou32_Index];
   }
   return q_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get buffered error for bus

   \param[in]  ou32_Index  Index

   \return
   Buffered error for bus
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_PuiSvHandler::GetErrorBus(const uint32_t ou32_Index) const
{
   bool q_Retval = true;

   if (ou32_Index < this->mc_SdBusErrors.size())
   {
      q_Retval = this->mc_SdBusErrors[ou32_Index];
   }
   return q_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Check view error

   \param[in]      ou32_Index                            View index
   \param[in,out]  opq_NameInvalid                       Name conflict
   \param[in,out]  opq_PcNotConnected                    PC not connected
   \param[in,out]  opq_RoutingInvalid                    Routing invalid
   \param[in,out]  opq_RoutingUpdateInvalid              Update routing invalid
   \param[in,out]  opq_RoutingDashboardInvalid           Dashboard routing invalid
   \param[in,out]  opq_SysDefInvalid                     System definition invalid
   \param[in,out]  opq_NoNodesActive                     No nodes active
   \param[in,out]  opc_RoutingErrorDetails               Error details for routing error if any
                                                         Warning: only set if routing error checked
   \param[in,out]  opc_SetupRoutingWarningDetails        Setup specific warning details

   \return
   C_NO_ERR Operation success
   C_RANGE  Operation failure: parameter invalid
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_PuiSvHandler::CheckViewError(const uint32_t ou32_Index, bool * const opq_NameInvalid,
                                       bool * const opq_PcNotConnected, bool * const opq_RoutingInvalid,
                                       bool * const opq_RoutingUpdateInvalid, bool * const opq_RoutingDashboardInvalid,
                                       bool * const opq_SysDefInvalid, bool * const opq_NoNodesActive,
                                       std::vector<QString> * const opc_RoutingErrorDetails,
                                       QString * const opc_SetupRoutingWarningDetails)
{
   int32_t s32_Retval = C_NO_ERR;

   if (ou32_Index < this->mc_Views.size())
   {
      const uint32_t u32_Hash = this->GetViewHash(ou32_Index);
      const QMap<uint32_t,
                 C_PuiSvViewErrorDetails>::const_iterator c_It = this->mc_PreviousErrorCheckResults.find(u32_Hash);
      if (c_It == this->mc_PreviousErrorCheckResults.end())
      {
         const C_PuiSvData & rc_CheckedData = this->mc_Views[ou32_Index];
         C_PuiSvViewErrorDetails c_Details;
         std::vector<QString> c_ErrorMessages;
         std::set<uint32_t> c_NodesWithDashboardRoutingError;
         std::set<uint32_t> c_NodesRelevantForDashboardRouting;
         QString c_SetupWarningMessage;
         std::vector<uint8_t> c_NodeActiveFlags;

         // Get the adapted node active flags in which the not reachable sub nodes of squads are deactivated
         this->GetNodeActiveFlagsWithSquadAdaptions(ou32_Index, c_NodeActiveFlags);
         //Name check
         c_Details.q_NameInvalid = false;
         for (uint32_t u32_ItView = 0; u32_ItView < this->mc_Views.size(); ++u32_ItView)
         {
            if (u32_ItView != ou32_Index)
            {
               const C_PuiSvData & rc_CurData = this->mc_Views[u32_ItView];
               if (rc_CurData.GetName().AnsiCompare(rc_CheckedData.GetName()) == 0)
               {
                  c_Details.q_NameInvalid = true;
               }
            }
         }
         //Check PC connected
         c_Details.q_PcNotConnected = !rc_CheckedData.GetOscPcData().GetConnected();
         //Check all routing details
         tgl_assert(this->m_CheckRouting(ou32_Index, c_NodeActiveFlags, c_SetupWarningMessage, c_ErrorMessages,
                                         c_NodesWithDashboardRoutingError,
                                         c_NodesRelevantForDashboardRouting) == C_NO_ERR);
         c_Details.c_RoutingErrorMessages = c_ErrorMessages;
         c_Details.c_RoutingSetupWarningMessage = c_SetupWarningMessage;
         c_Details.c_ResultNodesWithDashboardRoutingError = c_NodesWithDashboardRoutingError;
         c_Details.c_ResultNodesRelevantForDashboardRouting = c_NodesRelevantForDashboardRouting;

         //Check system definition
         c_Details.q_SysDefInvalid = false;
         //Node error
         for (uint32_t u32_ItNode = 0; u32_ItNode < c_NodeActiveFlags.size(); ++u32_ItNode)
         {
            if (c_NodeActiveFlags[u32_ItNode] == 1)
            {
               if (this->GetErrorNode(u32_ItNode) == true)
               {
                  c_Details.q_SysDefInvalid = true;
                  break;
               }
            }
         }
         //Bus error
         if (c_Details.q_SysDefInvalid == false)
         {
            for (uint32_t u32_ItBus = 0; u32_ItBus < C_PuiSdHandler::h_GetInstance()->GetOscBusesSize(); ++u32_ItBus)
            {
               if (this->CheckBusDisabled(ou32_Index, u32_ItBus) == false)
               {
                  if (this->GetErrorBus(u32_ItBus) == true)
                  {
                     c_Details.q_SysDefInvalid = true;
                     break;
                  }
               }
            }
         }
         //Check any nodes active
         c_Details.q_NoNodesActive = true;
         //Node error
         for (uint32_t u32_ItNode = 0; u32_ItNode < c_NodeActiveFlags.size(); ++u32_ItNode)
         {
            if (c_NodeActiveFlags[u32_ItNode] == 1)
            {
               c_Details.q_NoNodesActive = false;
               break;
            }
         }
         c_Details.c_ResultingNodeActiveStatus = rc_CheckedData.GetNodeActiveFlags();

         //Return error details
         c_Details.GetResults(opq_NameInvalid, opq_PcNotConnected, opq_RoutingInvalid,
                              opq_RoutingUpdateInvalid, opq_RoutingDashboardInvalid, opq_SysDefInvalid,
                              opq_NoNodesActive,
                              opc_RoutingErrorDetails, opc_SetupRoutingWarningDetails);
         //Store results
         this->mc_PreviousErrorCheckResults.insert(u32_Hash, c_Details);
      }
      else
      {
         //Always update node active flags
         C_PuiSvData & rc_CheckedData = this->mc_Views[ou32_Index];
         rc_CheckedData.SetNodeActiveFlags(c_It.value().c_ResultingNodeActiveStatus);
         c_It.value().GetResults(opq_NameInvalid, opq_PcNotConnected, opq_RoutingInvalid,
                                 opq_RoutingUpdateInvalid, opq_RoutingDashboardInvalid, opq_SysDefInvalid,
                                 opq_NoNodesActive,
                                 opc_RoutingErrorDetails, opc_SetupRoutingWarningDetails);
      }
   }
   else
   {
      s32_Retval = C_RANGE;
   }

   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Check view reconnect necessary

   \param[in]   ou32_ViewIndex            View index
   \param[out]  orq_ReconnectNecessary    Flag if reconnect necessary

   \return
   C_NO_ERR Operation success
   C_RANGE  Operation failure: parameter invalid
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_PuiSvHandler::CheckViewReconnectNecessary(const uint32_t ou32_ViewIndex, bool & orq_ReconnectNecessary)
{
   int32_t s32_Retval = C_NO_ERR;

   orq_ReconnectNecessary = false;

   if (ou32_ViewIndex < this->mc_Views.size())
   {
      const C_PuiSvData & rc_View = this->mc_Views[ou32_ViewIndex];
      // Trigger view check
      // includes hash and is necessary for node indices update, which should be done before loading any view
      this->CheckViewError(ou32_ViewIndex, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL);
      //Check if bus exists
      if (rc_View.GetOscPcData().GetConnected() == true)
      {
         if (rc_View.GetOscPcData().GetBusIndex() >= C_PuiSdHandler::h_GetInstance()->GetOscBusesSize())
         {
            orq_ReconnectNecessary = true;
         }
         else
         {
            //Check if bus enabled
            if (this->CheckBusDisabled(ou32_ViewIndex, rc_View.GetOscPcData().GetBusIndex()) == true)
            {
               orq_ReconnectNecessary = true;
            }
         }
      }
      else
      {
         //Check if any bus enabled
         for (uint32_t u32_ItBus = 0; u32_ItBus < C_PuiSdHandler::h_GetInstance()->GetOscBusesSize(); ++u32_ItBus)
         {
            if (this->CheckBusDisabled(ou32_ViewIndex, u32_ItBus) == false)
            {
               orq_ReconnectNecessary = true;
            }
         }
      }
   }
   else
   {
      s32_Retval = C_RANGE;
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Check node for a specific dashboard routing error for a specific view

   \param[in]   ou32_ViewIndex                View index
   \param[in]   ou32_NodeIndex                Node index
   \param[out]  orq_RoutingDashboardError     Flag if node has a view specific dashboard routing error

   \return
   C_NO_ERR Operation success
   C_RANGE  Operation failure: parameter invalid
   C_CONFIG Operation failure: CheckViewError was not executed for the specific view with the current configuration
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_PuiSvHandler::CheckViewNodeDashboardRoutingError(const uint32_t ou32_ViewIndex, const uint32_t ou32_NodeIndex,
                                                           bool & orq_RoutingDashboardError)
{
   int32_t s32_Retval = C_NO_ERR;

   orq_RoutingDashboardError = false;

   if (ou32_ViewIndex < this->mc_Views.size())
   {
      const uint32_t u32_Hash = this->GetViewHash(ou32_ViewIndex);
      QMap<uint32_t, C_PuiSvViewErrorDetails>::const_iterator c_It = this->mc_PreviousErrorCheckResults.find(
         u32_Hash);
      if (c_It != this->mc_PreviousErrorCheckResults.end())
      {
         if (c_It.value().c_ResultNodesWithDashboardRoutingError.find(ou32_NodeIndex) !=
             c_It.value().c_ResultNodesWithDashboardRoutingError.end())
         {
            // Node id was added
            orq_RoutingDashboardError = true;
         }
      }
      else
      {
         this->CheckViewError(ou32_ViewIndex, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL);

         c_It = this->mc_PreviousErrorCheckResults.find(u32_Hash);
         if (c_It != this->mc_PreviousErrorCheckResults.end())
         {
            if (c_It.value().c_ResultNodesWithDashboardRoutingError.find(ou32_NodeIndex) !=
                c_It.value().c_ResultNodesWithDashboardRoutingError.end())
            {
               // Node id was added
               orq_RoutingDashboardError = true;
            }
         }
         else
         {
            s32_Retval = C_CONFIG;
         }
      }
   }
   else
   {
      s32_Retval = C_RANGE;
   }

   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Gets nodes with errors for dashboard for a specific view

   CheckViewError must be executed for the specific view at least one time

   \param[in]   ou32_ViewIndex                View index
   \param[out]  orc_NodesWithErrors           All nodes with errors

   \return
   C_NO_ERR Operation success
   C_RANGE  Operation failure: parameter invalid
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_PuiSvHandler::GetViewNodeDashboardRoutingErrors(const uint32_t ou32_ViewIndex,
                                                          std::set<uint32_t> & orc_NodesWithErrors)
{
   int32_t s32_Retval = C_NO_ERR;

   if (ou32_ViewIndex < this->mc_Views.size())
   {
      const uint32_t u32_Hash = this->GetViewHash(ou32_ViewIndex);
      const QMap<uint32_t,
                 C_PuiSvViewErrorDetails>::const_iterator c_It = this->mc_PreviousErrorCheckResults.find(u32_Hash);
      if (c_It != this->mc_PreviousErrorCheckResults.end())
      {
         orc_NodesWithErrors = c_It.value().c_ResultNodesWithDashboardRoutingError;
      }
   }
   else
   {
      s32_Retval = C_RANGE;
   }

   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Gets relevant nodes for dashboard communication (routing and dashboard nodes)

   CheckViewError must be executed for the specific view at least one time

   \param[in]   ou32_ViewIndex                View index
   \param[out]  orc_RelevantNodes             All nodes which are relevant for communication

   \return
   C_NO_ERR Operation success
   C_RANGE  Operation failure: parameter invalid
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_PuiSvHandler::GetViewRelevantNodesForDashboardRouting(const uint32_t ou32_ViewIndex,
                                                                std::set<uint32_t> & orc_RelevantNodes)
{
   int32_t s32_Retval = C_NO_ERR;

   if (ou32_ViewIndex < this->mc_Views.size())
   {
      const uint32_t u32_Hash = this->GetViewHash(ou32_ViewIndex);
      const QMap<uint32_t,
                 C_PuiSvViewErrorDetails>::const_iterator c_It = this->mc_PreviousErrorCheckResults.find(u32_Hash);
      if (c_It != this->mc_PreviousErrorCheckResults.end())
      {
         orc_RelevantNodes = c_It.value().c_ResultNodesRelevantForDashboardRouting;
      }
   }
   else
   {
      s32_Retval = C_RANGE;
   }

   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Check and handle new element

   \param[in]  orc_NewId   New id

   \return
   C_NO_ERR Operation success
   C_RANGE  Operation failure: parameter invalid
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_PuiSvHandler::CheckAndHandleNewElement(const C_PuiSvDbNodeDataPoolListElementId & orc_NewId)
{
   int32_t s32_Retval = C_NO_ERR;

   if (orc_NewId.GetIsValid())
   {
      C_OscNodeDataPool::E_Type e_Type;
      if (C_PuiSdHandler::h_GetInstance()->GetDataPoolType(orc_NewId.u32_NodeIndex, orc_NewId.u32_DataPoolIndex,
                                                           e_Type) == C_NO_ERR)
      {
         if ((e_Type == C_OscNodeDataPool::eHALC) ||
             (e_Type == C_OscNodeDataPool::eHALC_NVM))
         {
            const C_OscNode * const pc_Node =
               C_PuiSdHandler::h_GetInstance()->GetOscNodeConst(orc_NewId.u32_NodeIndex);
            const C_OscNodeDataPool * const pc_Dp =
               C_PuiSdHandler::h_GetInstance()->GetOscDataPool(orc_NewId.u32_NodeIndex,
                                                               orc_NewId.u32_DataPoolIndex);
            const C_OscNodeDataPoolListElement * const pc_Element =
               C_PuiSdHandler::h_GetInstance()->GetOscDataPoolListElement(orc_NewId.u32_NodeIndex,
                                                                          orc_NewId.u32_DataPoolIndex,
                                                                          orc_NewId.u32_ListIndex,
                                                                          orc_NewId.u32_ElementIndex);
            if ((pc_Node != NULL) && ((pc_Element != NULL) && (pc_Dp != NULL)))
            {
               const std::string c_Tmp = orc_NewId.GetHalChannelName().toStdString();
               uint32_t u32_Hash = 0UL;
               if (pc_Node->c_HalcConfig.e_SafetyMode == C_OscHalcDefBase::eTWO_LEVELS_WITH_DROPPING)
               {
                  pc_Element->CalcHashElement(u32_Hash, orc_NewId.GetArrayElementIndexOrZero());
               }
               else
               {
                  pc_Element->CalcHashStructure(u32_Hash);
               }
               stw::scl::C_SclChecksums::CalcCRC32(
                  c_Tmp.c_str(), c_Tmp.length(), u32_Hash);
               this->mc_LastKnownHalcCrcs[orc_NewId] = C_PuiSvLastKnownHalElementId(u32_Hash, pc_Dp->c_Name.c_str());
            }
            else
            {
               s32_Retval = C_CONFIG;
            }
         }
      }
      else
      {
         s32_Retval = C_CONFIG;
      }
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Calc view routing crc name

   \param[in]   ou32_ViewIndex   View index
   \param[in]   orc_NodeName     Node name
   \param[out]  oru32_Crc        Crc

   \return
   C_NO_ERR Operation success
   C_RANGE  Operation failure: parameter invalid
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_PuiSvHandler::CalcViewRoutingCrcName(const uint32_t ou32_ViewIndex, const QString & orc_NodeName,
                                               uint32_t & oru32_Crc) const
{
   uint32_t u32_NodeIndex;
   int32_t s32_Retval = C_PuiSdHandler::h_GetInstance()->MapNodeNameToIndex(orc_NodeName, u32_NodeIndex);

   if (s32_Retval == C_NO_ERR)
   {
      s32_Retval = this->CalcViewRoutingCrcIndex(ou32_ViewIndex, u32_NodeIndex, oru32_Crc);
   }

   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Calc view routing crc index

   \param[in]   ou32_ViewIndex   View index
   \param[in]   ou32_NodeIndex   Node index
   \param[out]  oru32_Crc        Crc

   \return
   C_NO_ERR Operation success
   C_RANGE  Operation failure: parameter invalid
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_PuiSvHandler::CalcViewRoutingCrcIndex(const uint32_t ou32_ViewIndex, const uint32_t ou32_NodeIndex,
                                                uint32_t & oru32_Crc) const
{
   int32_t s32_Retval = C_NO_ERR;

   const C_PuiSvData * const pc_View = this->GetView(ou32_ViewIndex);

   if (pc_View != NULL)
   {
      if (pc_View->GetNodeActive(ou32_NodeIndex))
      {
         const stw::opensyde_core::C_OscNode * const pc_Node = C_PuiSdHandler::h_GetInstance()->GetOscNodeConst(
            ou32_NodeIndex);

         if (pc_Node != NULL)
         {
            // Check update routes
            const C_SyvRoRouteCalculation c_RouteCalcUpdate(ou32_ViewIndex, ou32_NodeIndex,
                                                            stw::opensyde_core::C_OscRoutingCalculation::eUPDATE);
            if (c_RouteCalcUpdate.GetState() == C_NO_ERR)
            {
               const C_OscRoutingRoute * const pc_Route = c_RouteCalcUpdate.GetBestRoute();
               if (pc_Route != NULL)
               {
                  QString c_Name;

                  //Calc CRC

                  //Init
                  oru32_Crc = 0xFFFFFFFFUL;

                  if (C_PuiSdHandler::h_GetInstance()->MapBusIndexToName(pc_View->GetOscPcData().GetBusIndex(),
                                                                         c_Name) == C_NO_ERR)
                  {
                     stw::scl::C_SclChecksums::CalcCRC32(c_Name.toStdString().c_str(), c_Name.length(), oru32_Crc);
                     if (C_PuiSdHandler::h_GetInstance()->MapNodeIndexToName(pc_Route->u32_TargetNodeIndex,
                                                                             c_Name) == C_NO_ERR)
                     {
                        stw::scl::C_SclChecksums::CalcCRC32(c_Name.toStdString().c_str(), c_Name.length(), oru32_Crc);

                        for (uint32_t u32_ItRoute = 0UL;
                             (u32_ItRoute < pc_Route->c_VecRoutePoints.size()) && (s32_Retval == C_NO_ERR);
                             ++u32_ItRoute)
                        {
                           const C_OscRoutingRoutePoint & rc_Route = pc_Route->c_VecRoutePoints[u32_ItRoute];

                           //Others
                           stw::scl::C_SclChecksums::CalcCRC32(&rc_Route.e_InInterfaceType,
                                                               sizeof(rc_Route.e_InInterfaceType), oru32_Crc);
                           stw::scl::C_SclChecksums::CalcCRC32(&rc_Route.e_OutInterfaceType,
                                                               sizeof(rc_Route.e_OutInterfaceType), oru32_Crc);
                           stw::scl::C_SclChecksums::CalcCRC32(&rc_Route.u8_InInterfaceNumber,
                                                               sizeof(rc_Route.u8_InInterfaceNumber), oru32_Crc);
                           stw::scl::C_SclChecksums::CalcCRC32(&rc_Route.u8_OutInterfaceNumber,
                                                               sizeof(rc_Route.u8_OutInterfaceNumber), oru32_Crc);
                           stw::scl::C_SclChecksums::CalcCRC32(&rc_Route.u8_InNodeId,
                                                               sizeof(rc_Route.u8_InNodeId), oru32_Crc);
                           stw::scl::C_SclChecksums::CalcCRC32(&rc_Route.u8_OutNodeId,
                                                               sizeof(rc_Route.u8_OutNodeId), oru32_Crc);

                           //Names
                           if (C_PuiSdHandler::h_GetInstance()->MapBusIndexToName(rc_Route.u32_InBusIndex,
                                                                                  c_Name) == C_NO_ERR)
                           {
                              stw::scl::C_SclChecksums::CalcCRC32(c_Name.toStdString().c_str(),
                                                                  c_Name.length(), oru32_Crc);
                           }
                           else
                           {
                              s32_Retval = C_RANGE;
                           }
                           if (s32_Retval == C_NO_ERR)
                           {
                              if (C_PuiSdHandler::h_GetInstance()->MapBusIndexToName(rc_Route.u32_OutBusIndex,
                                                                                     c_Name) == C_NO_ERR)
                              {
                                 stw::scl::C_SclChecksums::CalcCRC32(c_Name.toStdString().c_str(),
                                                                     c_Name.length(), oru32_Crc);
                                 if (C_PuiSdHandler::h_GetInstance()->MapNodeIndexToName(rc_Route.u32_NodeIndex,
                                                                                         c_Name) == C_NO_ERR)
                                 {
                                    stw::scl::C_SclChecksums::CalcCRC32(c_Name.toStdString().c_str(),
                                                                        c_Name.length(), oru32_Crc);
                                 }
                                 else
                                 {
                                    s32_Retval = C_RANGE;
                                 }
                              }
                              else
                              {
                                 s32_Retval = C_RANGE;
                              }
                           }
                        }
                     }
                     else
                     {
                        s32_Retval = C_RANGE;
                     }
                  }
                  else
                  {
                     s32_Retval = C_RANGE;
                  }
               }
               else
               {
                  s32_Retval = C_RANGE;
               }
            }
            else
            {
               s32_Retval = C_RANGE;
            }
         }
      }
      else
      {
         s32_Retval = C_RANGE;
      }
   }
   else
   {
      s32_Retval = C_RANGE;
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get namespace for ID

   \param[in]  orc_Id   ID

   \return
   Namespace
*/
//----------------------------------------------------------------------------------------------------------------------
QString C_PuiSvHandler::h_GetNamespace(const C_PuiSvDbNodeDataPoolListElementId & orc_Id)
{
   QString c_Retval;

   if (orc_Id.GetIsValid())
   {
      if (orc_Id.GetType() == C_PuiSvDbNodeDataPoolListElementId::eDATAPOOL_ELEMENT)
      {
         C_OscNodeDataPool::E_Type e_Type;
         if ((C_PuiSdHandler::h_GetInstance()->GetDataPoolType(orc_Id.u32_NodeIndex, orc_Id.u32_DataPoolIndex,
                                                               e_Type) == C_NO_ERR) &&
             ((e_Type == C_OscNodeDataPool::eHALC) || (e_Type == C_OscNodeDataPool::eHALC_NVM)))
         {
            c_Retval = C_PuiSdUtil::h_GetHalcNamespace(orc_Id);
         }
         else
         {
            c_Retval = C_PuiSdUtil::h_GetNamespace(orc_Id);
            if (orc_Id.GetUseArrayElementIndex())
            {
               //Append array element index
               c_Retval += static_cast<QString>(C_GtGetText::h_GetText("[%1]")).arg(orc_Id.GetArrayElementIndex());
            }
         }
      }
      else
      {
         c_Retval = C_PuiSdUtil::h_GetSignalNamespace(orc_Id);
      }
   }
   return c_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get short namespace from Id

   \param[in]  orc_Id   Datapool element ID

   \return
   short namespace
*/
//----------------------------------------------------------------------------------------------------------------------
QString C_PuiSvHandler::h_GetShortNamespace(const C_PuiSvDbNodeDataPoolListElementId & orc_Id)
{
   QString c_Retval = C_GtGetText::h_GetText("Unknown HAL data element");
   uint32_t u32_DomainIndex;
   bool q_UseChannelIndex;
   uint32_t u32_ChannelIndex;

   C_OscHalcDefDomain::E_VariableSelector e_Selector;
   uint32_t u32_ParameterIndex;
   bool q_UseElementIndex;
   uint32_t u32_ParameterElementIndex;
   bool q_IsUseCaseIndex;
   bool q_IsChanNumIndex;
   bool q_IsSafetyFlagIndex;

   if (C_PuiSdHandler::h_GetInstance()->TranslateToHalcIndex(orc_Id, orc_Id.GetArrayElementIndexOrZero(),
                                                             u32_DomainIndex, q_UseChannelIndex,
                                                             u32_ChannelIndex, e_Selector, u32_ParameterIndex,
                                                             q_UseElementIndex,
                                                             u32_ParameterElementIndex, q_IsUseCaseIndex,
                                                             q_IsChanNumIndex, q_IsSafetyFlagIndex) == C_NO_ERR)
   {
      {
         const C_OscNodeDataPoolList * const pc_List =
            C_PuiSdHandler::h_GetInstance()->GetOscDataPoolList(orc_Id.u32_NodeIndex,
                                                                orc_Id.u32_DataPoolIndex,
                                                                orc_Id.u32_ListIndex);
         const C_OscHalcConfigChannel * const pc_Config =
            C_PuiSdHandler::h_GetInstance()->GetHalcDomainChannelConfigData(orc_Id.u32_NodeIndex,
                                                                            u32_DomainIndex,
                                                                            u32_ChannelIndex,
                                                                            q_UseChannelIndex);
         const C_OscHalcDefDomain * const pc_Domain =
            C_PuiSdHandler::h_GetInstance()->GetHalcDomainFileDataConst(orc_Id.u32_NodeIndex,
                                                                        u32_DomainIndex);

         if ((((pc_List != NULL)) && (pc_Config != NULL)) && (pc_Domain != NULL))
         {
            QString c_ElementName;

            if (q_IsUseCaseIndex)
            {
               c_ElementName = C_OscHalcMagicianUtil::h_GetUseCaseVariableName(pc_Domain->c_SingularName).c_str();
            }
            else if (q_IsChanNumIndex)
            {
               c_ElementName = C_OscHalcMagicianUtil::h_GetChanNumVariableName(pc_Domain->c_SingularName).c_str();
            }
            else if (q_IsSafetyFlagIndex)
            {
               c_ElementName = C_OscHalcMagicianUtil::h_GetSafetyFlagVariableName(pc_Domain->c_SingularName).c_str();
            }
            else
            {
               const C_OscHalcDefStruct * const pc_Param =
                  C_PuiSdHandler::h_GetInstance()->GetHalcDomainFileVariableData(orc_Id.u32_NodeIndex,
                                                                                 u32_DomainIndex, e_Selector,
                                                                                 u32_ParameterIndex);
               c_ElementName = pc_Param->c_Display.c_str();
               if (q_UseElementIndex)
               {
                  if (u32_ParameterElementIndex < pc_Param->c_StructElements.size())
                  {
                     const C_OscHalcDefElement & rc_Param = pc_Param->c_StructElements[u32_ParameterElementIndex];
                     c_ElementName = rc_Param.c_Display.c_str();
                  }
               }
            }
            c_Retval = static_cast<QString>("%1::%2::%3").
                       arg(pc_Config->c_Name.c_str()).
                       arg(pc_List->c_Name.c_str()).
                       arg(c_ElementName);
         }
      }
   }
   return c_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Check bus for disabled look

   \param[in]  ou32_ViewIndex    View index
   \param[in]  ou32_BusIndex     Bus index

   \return
   True  Disabled
   False Enabled
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_PuiSvHandler::CheckBusDisabled(const uint32_t ou32_ViewIndex, const uint32_t ou32_BusIndex) const
{
   bool q_Disabled = true;

   std::vector<uint32_t> c_NodeIndexes;
   std::vector<uint32_t> c_InterfaceIndexes;
   C_PuiSdHandler::h_GetInstance()->GetOscSystemDefinitionConst().GetNodeIndexesOfBus(ou32_BusIndex, c_NodeIndexes,
                                                                                      c_InterfaceIndexes);
   if (c_NodeIndexes.size() == c_InterfaceIndexes.size())
   {
      const C_PuiSvData * const pc_View = this->GetView(ou32_ViewIndex);
      if (pc_View != NULL)
      {
         const std::vector<uint8_t> & rc_Nodes = pc_View->GetNodeActiveFlags();
         for (uint32_t u32_ItNode = 0; u32_ItNode < c_NodeIndexes.size(); ++u32_ItNode)
         {
            if (c_NodeIndexes[u32_ItNode] < rc_Nodes.size())
            {
               if (rc_Nodes[c_NodeIndexes[u32_ItNode]] == 1)
               {
                  q_Disabled = false;
               }
            }
         }
      }
   }

   return q_Disabled;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   View dashboard param widget clear all data pool elements

   \param[in]  ou32_ViewIndex          View index
   \param[in]  ou32_DashboardIndex     Dashboard index
   \param[in]  ou32_ParamWidgetIndex   Param index

   \return
   C_NO_ERR Operation success
   C_RANGE  Operation failure: parameter invalid
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_PuiSvHandler::ClearViewDashboardParamDataPoolElements(const uint32_t ou32_ViewIndex,
                                                                const uint32_t ou32_DashboardIndex,
                                                                const uint32_t ou32_ParamWidgetIndex)
{
   int32_t s32_Retval = C_NO_ERR;

   if (ou32_ViewIndex < this->mc_Views.size())
   {
      C_PuiSvData & rc_View = this->mc_Views[ou32_ViewIndex];
      s32_Retval = rc_View.ClearDashboardParamDataPoolElements(ou32_DashboardIndex, ou32_ParamWidgetIndex);
   }
   else
   {
      s32_Retval = C_RANGE;
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   View dashboard param widget add new data pool element

   \param[in]  ou32_ViewIndex          View index
   \param[in]  ou32_DashboardIndex     Dashboard index
   \param[in]  ou32_ParamWidgetIndex   Param index
   \param[in]  orc_NewId               New ID
   \param[in]  opc_Content             Optional init value

   \return
   C_NO_ERR Operation success
   C_RANGE  Operation failure: parameter invalid
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_PuiSvHandler::AddViewDashboardParamNewDataPoolElement(const uint32_t ou32_ViewIndex,
                                                                const uint32_t ou32_DashboardIndex,
                                                                const uint32_t ou32_ParamWidgetIndex,
                                                                const C_OscNodeDataPoolListElementId & orc_NewId,
                                                                const C_OscNodeDataPoolContent * const opc_Content)
{
   int32_t s32_Retval = C_NO_ERR;

   if (ou32_ViewIndex < this->mc_Views.size())
   {
      C_PuiSvData & rc_View = this->mc_Views[ou32_ViewIndex];
      s32_Retval = rc_View.AddDashboardParamNewDataPoolElement(ou32_DashboardIndex, ou32_ParamWidgetIndex, orc_NewId,
                                                               opc_Content);
   }
   else
   {
      s32_Retval = C_RANGE;
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get hash for view

   \param[in]  ou32_ViewIndex    Index

   \return
   Hash for view
*/
//----------------------------------------------------------------------------------------------------------------------
uint32_t C_PuiSvHandler::GetViewHash(const uint32_t ou32_ViewIndex) const
{
   uint32_t u32_Retval = 0xFFFFFFFFU;

   if (ou32_ViewIndex < this->mc_Views.size())
   {
      const C_PuiSvData & rc_View = this->mc_Views[ou32_ViewIndex];
      rc_View.CalcHash(u32_Retval);
   }
   return u32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get singleton (Create if necessary)

   \return
   Pointer to singleton
*/
//----------------------------------------------------------------------------------------------------------------------
C_PuiSvHandler * C_PuiSvHandler::h_GetInstance(void)
{
   if (C_PuiSvHandler::mhpc_Singleton == NULL)
   {
      C_PuiSvHandler::mhpc_Singleton = new C_PuiSvHandler();
   }
   return C_PuiSvHandler::mhpc_Singleton;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Clean up singleton
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PuiSvHandler::h_Destroy(void)
{
   delete C_PuiSvHandler::mhpc_Singleton;
   C_PuiSvHandler::mhpc_Singleton = NULL;
}

//----------------------------------------------------------------------------------------------------------------------
/*!
\brief   Load system views

   Load system views and store in information in our instance data.

   \param[in] orc_Path     Path to system views file
   \param[in] orc_OscNodes OSC node information (Necessary for update information)

   \return
   C_NO_ERR   data read and placed into instance data
   C_RD_WR    problems accessing file system (e.g. no read access to file)
   C_RANGE    specified file does not exist (when loading)
   C_NOACT    specified file is present but structure is invalid (e.g. invalid XML file)
   C_CONFIG   content of file is invalid or incomplete
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_PuiSvHandler::m_LoadFromFile(const QString & orc_Path,
                                       const std::vector<stw::opensyde_core::C_OscNode> & orc_OscNodes)
{
   int32_t s32_Retval = C_NO_ERR;

   if (TglFileExists(orc_Path.toStdString().c_str()) == true)
   {
      C_OscXmlParserLog c_XmlParser;
      c_XmlParser.SetLogHeading("Loading views");
      s32_Retval = c_XmlParser.LoadFromFile(orc_Path.toStdString().c_str());
      if (s32_Retval == C_NO_ERR)
      {
         if (c_XmlParser.SelectRoot() == "opensyde-system-views")
         {
            if (c_XmlParser.SelectNodeChild("file-version") == "file-version")
            {
               bool q_Ok;
               const int32_t s32_FileVersion =
                  static_cast<QString>(c_XmlParser.GetNodeContent().c_str()).toInt(&q_Ok, 0);

               //is the file version one we know ?
               if (q_Ok)
               {
                  osc_write_log_info("Loading views", "Value of \"file-version\": " +
                                     stw::scl::C_SclString::IntToStr(s32_FileVersion));
                  //Check file version
                  if ((s32_FileVersion != 1) && (s32_FileVersion != 2))
                  {
                     osc_write_log_error("Loading views",
                                         "Version defined by \"file-version\" is not supported.");
                     s32_Retval = C_CONFIG;
                  }
               }
               else
               {
                  osc_write_log_error("Loading views", "\"file-version\" could not be converted to a number.");
                  s32_Retval = C_CONFIG;
               }

               if (s32_Retval == C_NO_ERR)
               {
                  tgl_assert(c_XmlParser.SelectNodeParent() == "opensyde-system-views");
                  if (s32_FileVersion == 1)
                  {
                     s32_Retval = C_PuiSvHandlerFilerV1::h_LoadViews(this->mc_Views, c_XmlParser);
                  }
                  else
                  {
                     if (c_XmlParser.SelectNodeChild("service-mode") == "service-mode")
                     {
                        if (c_XmlParser.AttributeExists("active"))
                        {
                           this->SetServiceModeActive(c_XmlParser.GetAttributeBool("active"));
                        }
                        else
                        {
                           s32_Retval = C_CONFIG;
                           osc_write_log_error("Loading views",
                                               "Attribute \"active\" not found in node \"service-mode\".");
                        }
                        tgl_assert(c_XmlParser.SelectNodeParent() == "opensyde-system-views");
                     }
                     else
                     {
                        //Default
                        this->SetServiceModeActive(false);
                     }
                     if (s32_Retval == C_NO_ERR)
                     {
                        const QFileInfo c_Info(orc_Path);
                        const QDir c_BasePath = c_Info.dir();
                        s32_Retval = C_PuiSvHandlerFiler::h_LoadViews(this->mc_Views, orc_OscNodes,
                                                                      c_XmlParser, &c_BasePath);
                        if (s32_Retval == C_NO_ERR)
                        {
                           s32_Retval = C_PuiSvHandlerFiler::h_LoadLastKnownHalcCrcs(this->mc_LastKnownHalcCrcs,
                                                                                     c_XmlParser);
                           if (s32_Retval == C_NO_ERR)
                           {
                              //calculate the hash value and save it for comparing (only for new file version!)
                              this->mu32_CalculatedHashSystemViews = this->m_CalcHashSystemViews();
                           }
                        }
                     }
                  }
                  m_FixInvalidRailConfig();
                  m_HandleCompatibilityChart();
               }
               else
               {
                  s32_Retval = C_CONFIG;
               }
            }
            else
            {
               osc_write_log_error("Loading views", "Could not find \"file-version\" node.");
               s32_Retval = C_CONFIG;
            }
         }
         else
         {
            s32_Retval = C_CONFIG;
         }
      }
      else
      {
         s32_Retval = C_NOACT;
      }
   }
   else
   {
      s32_Retval = C_RANGE;
   }

   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Add last known HAL CRC

   \param[in]  orc_Id   Id
   \param[in]  orc_Crc  Crc
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PuiSvHandler::m_AddLastKnownHalcCrc(const C_PuiSvDbNodeDataPoolListElementId & orc_Id,
                                           const C_PuiSvLastKnownHalElementId & orc_Crc)
{
   this->mc_LastKnownHalcCrcs[orc_Id] = orc_Crc;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default constructor

   \param[in,out]  opc_Parent    Optional pointer to parent
*/
//----------------------------------------------------------------------------------------------------------------------
C_PuiSvHandler::C_PuiSvHandler(QObject * const opc_Parent) :
   QObject(opc_Parent),
   mu32_CalculatedHashSystemViews(0U),
   mu32_PreviousSystemDefintionHash(0U),
   mq_IsServiceModeActive(false)
{
   //Connects for synchronisation
   connect(C_PuiSdHandler::h_GetInstance(), &C_PuiSdHandler::SigSyncNodeAdded, this,
           &C_PuiSvHandler::m_OnSyncNodeAdded);
   connect(C_PuiSdHandler::h_GetInstance(), &C_PuiSdHandler::SigSyncNodeHalc, this,
           &C_PuiSvHandler::m_OnSyncNodeHalc);
   connect(C_PuiSdHandler::h_GetInstance(), &C_PuiSdHandler::SigSyncNodeReplace, this,
           &C_PuiSvHandler::m_OnSyncNodeReplace);
   connect(
      C_PuiSdHandler::h_GetInstance(), &C_PuiSdHandler::SigSyncNodeAboutToBeDeleted, this,
      &C_PuiSvHandler::m_OnSyncNodeAboutToBeDeleted);
   connect(C_PuiSdHandler::h_GetInstance(), &C_PuiSdHandler::SigSyncBusAdded, this, &C_PuiSvHandler::m_OnSyncBusAdded);
   connect(
      C_PuiSdHandler::h_GetInstance(), &C_PuiSdHandler::SigSyncBusDeleted, this, &C_PuiSvHandler::m_OnSyncBusDeleted);
   connect(C_PuiSdHandler::h_GetInstance(), &C_PuiSdHandler::SigSyncClear, this, &C_PuiSvHandler::m_OnSyncClear);
   connect(
      C_PuiSdHandler::h_GetInstance(), &C_PuiSdHandler::SigSyncNodeDataPoolAdded, this,
      &C_PuiSvHandler::m_OnSyncNodeDataPoolAdded);
   connect(
      C_PuiSdHandler::h_GetInstance(), &C_PuiSdHandler::SigSyncNodeDataPoolMoved, this,
      &C_PuiSvHandler::m_OnSyncNodeDataPoolMoved);
   connect(
      C_PuiSdHandler::h_GetInstance(), &C_PuiSdHandler::SigSyncNodeDataPoolAboutToBeDeleted, this,
      &C_PuiSvHandler::m_OnSyncNodeDataPoolAboutToBeDeleted);
   connect(
      C_PuiSdHandler::h_GetInstance(), &C_PuiSdHandler::SigSyncNodeApplicationAdded, this,
      &C_PuiSvHandler::m_OnSyncNodeApplicationAdded);
   connect(
      C_PuiSdHandler::h_GetInstance(), &C_PuiSdHandler::SigSyncNodeApplicationMoved, this,
      &C_PuiSvHandler::m_OnSyncNodeApplicationMoved);
   connect(
      C_PuiSdHandler::h_GetInstance(), &C_PuiSdHandler::SigSyncNodeApplicationAboutToBeDeleted, this,
      &C_PuiSvHandler::m_OnSyncNodeApplicationAboutToBeDeleted);
   connect(
      C_PuiSdHandler::h_GetInstance(), &C_PuiSdHandler::SigSyncNodeApplicationAboutToBeChangedFromParamSetHalc, this,
      &C_PuiSvHandler::m_OnSyncNodeApplicationAboutToBeChangedFromParamSetHalc);
   connect(
      C_PuiSdHandler::h_GetInstance(), &C_PuiSdHandler::SigSyncNodeApplicationChangedToParamSetHalc, this,
      &C_PuiSvHandler::m_OnSyncNodeApplicationChangedToParamSetHalc);
   connect(
      C_PuiSdHandler::h_GetInstance(), &C_PuiSdHandler::SigSyncNodeApplicationResultPathSizeChanged, this,
      &C_PuiSvHandler::m_OnSyncNodeApplicationResultPathSizeChanged);
   connect(
      C_PuiSdHandler::h_GetInstance(), &C_PuiSdHandler::SigSyncNodeDataPoolListAdded, this,
      &C_PuiSvHandler::m_OnSyncNodeDataPoolListAdded);
   connect(
      C_PuiSdHandler::h_GetInstance(), &C_PuiSdHandler::SigSyncNodeDataPoolListMoved, this,
      &C_PuiSvHandler::m_OnSyncNodeDataPoolListMoved);
   connect(
      C_PuiSdHandler::h_GetInstance(), &C_PuiSdHandler::SigSyncNodeDataPoolListAboutToBeDeleted, this,
      &C_PuiSvHandler::m_OnSyncNodeDataPoolListAboutToBeDeleted);
   connect(
      C_PuiSdHandler::h_GetInstance(), &C_PuiSdHandler::SigSyncNodeDataPoolListDataSetAdded, this,
      &C_PuiSvHandler::m_OnSyncNodeDataPoolListDataSetAdded);
   connect(
      C_PuiSdHandler::h_GetInstance(), &C_PuiSdHandler::SigSyncNodeDataPoolListDataSetMoved, this,
      &C_PuiSvHandler::m_OnSyncNodeDataPoolListDataSetMoved);
   connect(
      C_PuiSdHandler::h_GetInstance(), &C_PuiSdHandler::SigSyncNodeDataPoolListDataSetAboutToBeDeleted, this,
      &C_PuiSvHandler::m_OnSyncNodeDataPoolListDataSetAboutToBeDeleted);
   connect(
      C_PuiSdHandler::h_GetInstance(), &C_PuiSdHandler::SigSyncNodeDataPoolListElementAdded, this,
      &C_PuiSvHandler::m_OnSyncNodeDataPoolListElementAdded);
   connect(
      C_PuiSdHandler::h_GetInstance(), &C_PuiSdHandler::SigSyncNodeDataPoolListElementMoved, this,
      &C_PuiSvHandler::m_OnSyncNodeDataPoolListElementMoved);
   connect(
      C_PuiSdHandler::h_GetInstance(), &C_PuiSdHandler::SigSyncNodeDataPoolListElementTypeChanged, this,
      &C_PuiSvHandler::m_OnSyncElementTypeOrArrayChanged);
   connect(
      C_PuiSdHandler::h_GetInstance(), &C_PuiSdHandler::SigSyncNodeDataPoolListElementRangeChanged, this,
      &C_PuiSvHandler::m_OnSyncElementRangeChanged);
   connect(
      C_PuiSdHandler::h_GetInstance(), &C_PuiSdHandler::SigSyncNodeDataPoolListElementAccessChanged, this,
      &C_PuiSvHandler::m_OnSyncNodeDataPoolListElementAccessChanged);
   connect(
      C_PuiSdHandler::h_GetInstance(), &C_PuiSdHandler::SigSyncNodeDataPoolListElementAboutToBeDeleted, this,
      &C_PuiSvHandler::m_OnSyncNodeDataPoolListElementAboutToBeDeleted);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default destructor

   Clean up.
*/
//----------------------------------------------------------------------------------------------------------------------
C_PuiSvHandler::~C_PuiSvHandler(void)
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Sync view node indices to added node index

   \param[in]  ou32_Index  Added node index
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PuiSvHandler::m_OnSyncNodeAdded(const uint32_t ou32_Index)
{
   for (uint32_t u32_ItView = 0; u32_ItView < this->mc_Views.size(); ++u32_ItView)
   {
      C_PuiSvData & rc_View = this->mc_Views[u32_ItView];
      rc_View.OnSyncNodeAdded(ou32_Index);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Sync view node indices to node HALC change

   \param[in]  ou32_Index  Index
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PuiSvHandler::m_OnSyncNodeHalc(const uint32_t ou32_Index)
{
   const C_OscNode * const pc_Node = C_PuiSdHandler::h_GetInstance()->GetOscNodeConst(ou32_Index);

   if (pc_Node != NULL)
   {
      std::map<C_PuiSvDbNodeDataPoolListElementId, C_PuiSvLastKnownHalElementId> c_NewMap;
      std::map<C_PuiSvDbNodeDataPoolListElementId, C_PuiSvDbNodeDataPoolListElementId> c_MapCurToNew;
      for (uint32_t u32_ItDp = 0UL; u32_ItDp < pc_Node->c_DataPools.size(); ++u32_ItDp)
      {
         const C_OscNodeDataPool & rc_Dp = pc_Node->c_DataPools[u32_ItDp];
         if ((rc_Dp.e_Type == C_OscNodeDataPool::eHALC) || (rc_Dp.e_Type == C_OscNodeDataPool::eHALC_NVM))
         {
            for (uint32_t u32_ItLi = 0UL; u32_ItLi < rc_Dp.c_Lists.size(); ++u32_ItLi)
            {
               const C_OscNodeDataPoolList & rc_Li = rc_Dp.c_Lists[u32_ItLi];
               for (uint32_t u32_ItEl = 0UL; u32_ItEl < rc_Li.c_Elements.size(); ++u32_ItEl)
               {
                  const C_OscNodeDataPoolListElement & rc_El = rc_Li.c_Elements[u32_ItEl];
                  for (uint32_t u32_ItAr = 0UL; u32_ItAr < rc_El.GetArraySize(); ++u32_ItAr)
                  {
                     C_PuiSvDbNodeDataPoolListElementId c_NewId(ou32_Index, u32_ItDp, u32_ItLi, u32_ItEl,
                                                                C_PuiSvDbNodeDataPoolListElementId::eDATAPOOL_ELEMENT,
                                                                rc_El.GetArraySize() != 1, u32_ItAr);
                     uint32_t u32_DomainIndex;
                     bool q_UseChannelIndex;
                     uint32_t u32_ChannelIndex;

                     C_OscHalcDefDomain::E_VariableSelector e_Selector;
                     uint32_t u32_ParameterIndex;
                     bool q_UseElementIndex;
                     uint32_t u32_ParameterElementIndex;
                     bool q_IsUseCaseIndex;
                     bool q_IsChanNumIndex;
                     bool q_IsSafetyFlagIndex;

                     if (C_PuiSdHandler::h_GetInstance()->TranslateToHalcIndex(c_NewId,
                                                                               c_NewId.GetArrayElementIndexOrZero(),
                                                                               u32_DomainIndex, q_UseChannelIndex,
                                                                               u32_ChannelIndex, e_Selector,
                                                                               u32_ParameterIndex,
                                                                               q_UseElementIndex,
                                                                               u32_ParameterElementIndex,
                                                                               q_IsUseCaseIndex,
                                                                               q_IsChanNumIndex,
                                                                               q_IsSafetyFlagIndex) == C_NO_ERR)
                     {
                        QString c_Name;
                        if (C_PuiSdHandler::h_GetInstance()->GetHalChannelOrDomainName(ou32_Index,
                                                                                       u32_DomainIndex,
                                                                                       u32_ChannelIndex,
                                                                                       c_Name) == C_NO_ERR)
                        {
                           const std::string c_Tmp = c_Name.toStdString();
                           uint32_t u32_Hash = 0UL;
                           if (pc_Node->c_HalcConfig.e_SafetyMode == C_OscHalcDefBase::eTWO_LEVELS_WITH_DROPPING)
                           {
                              rc_El.CalcHashElement(u32_Hash, u32_ItAr);
                           }
                           else
                           {
                              rc_El.CalcHashStructure(u32_Hash);
                           }
                           stw::scl::C_SclChecksums::CalcCRC32(
                              c_Tmp.c_str(), c_Tmp.length(), u32_Hash);
                           c_NewId.SetHalChannelName(c_Name);
                           for (std::map<C_PuiSvDbNodeDataPoolListElementId,
                                         C_PuiSvLastKnownHalElementId>::const_iterator c_ItCur =
                                   this->mc_LastKnownHalcCrcs.begin();
                                c_ItCur != this->mc_LastKnownHalcCrcs.end(); ++c_ItCur)
                           {
                              if (((c_ItCur->second.u32_Crc == u32_Hash) &&
                                   (c_ItCur->second.c_HalDpName.compare(rc_Dp.c_Name.c_str()) == 0)) &&
                                  ((pc_Node->c_HalcConfig.e_SafetyMode ==
                                    C_OscHalcDefBase::eTWO_LEVELS_WITH_DROPPING) ||
                                   (c_ItCur->first.GetArrayElementIndexOrZero() == u32_ParameterElementIndex)))
                              {
                                 c_NewMap[c_NewId] = C_PuiSvLastKnownHalElementId(u32_Hash, rc_Dp.c_Name.c_str());
                                 c_MapCurToNew[c_ItCur->first] = c_NewId;
                              }
                           }
                        }
                     }
                  }
               }
            }
         }
      }
      //Check any removed elements
      for (uint32_t u32_ItView = 0; u32_ItView < this->mc_Views.size(); ++u32_ItView)
      {
         C_PuiSvData & rc_View = this->mc_Views[u32_ItView];
         rc_View.OnSyncNodeHalc(ou32_Index, c_MapCurToNew);
      }
      //Store new map
      this->mc_LastKnownHalcCrcs = c_NewMap;
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Sync view node indices to node replace change

   \param[in]  ou32_Index  Index
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PuiSvHandler::m_OnSyncNodeReplace(const uint32_t ou32_Index)
{
   for (uint32_t u32_ItView = 0; u32_ItView < this->mc_Views.size(); ++u32_ItView)
   {
      C_PuiSvData & rc_View = this->mc_Views[u32_ItView];
      rc_View.OnSyncNodeReplace(ou32_Index);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Sync view node indices to deleted node index

   \param[in]  ou32_Index           Deleted node index
   \param[in]  oq_OnlyMarkInvalid   Only mark invalid
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PuiSvHandler::m_OnSyncNodeAboutToBeDeleted(const uint32_t ou32_Index, const bool oq_OnlyMarkInvalid)
{
   for (uint32_t u32_ItView = 0; u32_ItView < this->mc_Views.size(); ++u32_ItView)
   {
      C_PuiSvData & rc_View = this->mc_Views[u32_ItView];
      rc_View.OnSyncNodeAboutToBeDeleted(ou32_Index, oq_OnlyMarkInvalid);
   }
   // HALC
   {
      std::map<C_PuiSvDbNodeDataPoolListElementId, C_PuiSvLastKnownHalElementId> c_Tmp;
      for (std::map<C_PuiSvDbNodeDataPoolListElementId, C_PuiSvLastKnownHalElementId>::const_iterator c_It =
              this->mc_LastKnownHalcCrcs.begin();
           c_It != this->mc_LastKnownHalcCrcs.end(); ++c_It)
      {
         C_PuiSvDbNodeDataPoolListElementId c_Id = c_It->first;
         C_PuiSvDashboard::h_OnSyncNodeAboutToBeDeleted(c_Id, ou32_Index, oq_OnlyMarkInvalid);
         c_Tmp[c_Id] = c_It->second;
      }
      this->mc_LastKnownHalcCrcs = c_Tmp;
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Sync view bus index to added bus index

   \param[in]  ou32_Index  Added bus index
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PuiSvHandler::m_OnSyncBusAdded(const uint32_t ou32_Index)
{
   for (uint32_t u32_ItView = 0; u32_ItView < this->mc_Views.size(); ++u32_ItView)
   {
      C_PuiSvData & rc_View = this->mc_Views[u32_ItView];
      rc_View.OnSyncBusAdded(ou32_Index);
   }
   //HALC not affected
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Sync view bus index to deleted bus index

   \param[in]  ou32_Index  Deleted bus index
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PuiSvHandler::m_OnSyncBusDeleted(const uint32_t ou32_Index)
{
   for (uint32_t u32_ItView = 0; u32_ItView < this->mc_Views.size(); ++u32_ItView)
   {
      C_PuiSvData & rc_View = this->mc_Views[u32_ItView];
      rc_View.OnSyncBusDeleted(ou32_Index);
   }
   //HALC not affected
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Adapt to system definition change

   \param[in]  ou32_NodeIndex       Node index
   \param[in]  ou32_DataPoolIndex   Data pool index
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PuiSvHandler::m_OnSyncNodeDataPoolAdded(const uint32_t ou32_NodeIndex, const uint32_t ou32_DataPoolIndex)
{
   for (uint32_t u32_ItView = 0; u32_ItView < this->mc_Views.size(); ++u32_ItView)
   {
      C_PuiSvData & rc_View = this->mc_Views[u32_ItView];
      rc_View.OnSyncNodeDataPoolAdded(ou32_NodeIndex, ou32_DataPoolIndex);
   }
   // HALC
   {
      std::map<C_PuiSvDbNodeDataPoolListElementId, C_PuiSvLastKnownHalElementId> c_Tmp;
      for (std::map<C_PuiSvDbNodeDataPoolListElementId, C_PuiSvLastKnownHalElementId>::const_iterator c_It =
              this->mc_LastKnownHalcCrcs.begin();
           c_It != this->mc_LastKnownHalcCrcs.end(); ++c_It)
      {
         C_PuiSvDbNodeDataPoolListElementId c_Id = c_It->first;
         C_PuiSvDashboard::h_OnSyncNodeDataPoolAdded(c_Id, ou32_NodeIndex, ou32_DataPoolIndex);
         c_Tmp[c_Id] = c_It->second;
      }
      this->mc_LastKnownHalcCrcs = c_Tmp;
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Adapt to system definition change

   \param[in]  ou32_NodeIndex             Node index
   \param[in]  ou32_DataPoolSourceIndex   Source data pool index
   \param[in]  ou32_DataPoolTargetIndex   Target data pool index
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PuiSvHandler::m_OnSyncNodeDataPoolMoved(const uint32_t ou32_NodeIndex, const uint32_t ou32_DataPoolSourceIndex,
                                               const uint32_t ou32_DataPoolTargetIndex)
{
   for (uint32_t u32_ItView = 0; u32_ItView < this->mc_Views.size(); ++u32_ItView)
   {
      C_PuiSvData & rc_View = this->mc_Views[u32_ItView];
      rc_View.OnSyncNodeDataPoolMoved(ou32_NodeIndex, ou32_DataPoolSourceIndex, ou32_DataPoolTargetIndex);
   }
   // HALC
   {
      std::map<C_PuiSvDbNodeDataPoolListElementId, C_PuiSvLastKnownHalElementId> c_Tmp;
      for (std::map<C_PuiSvDbNodeDataPoolListElementId, C_PuiSvLastKnownHalElementId>::const_iterator c_It =
              this->mc_LastKnownHalcCrcs.begin();
           c_It != this->mc_LastKnownHalcCrcs.end(); ++c_It)
      {
         C_PuiSvDbNodeDataPoolListElementId c_Id = c_It->first;
         C_PuiSvDashboard::h_OnSyncNodeDataPoolMoved(c_Id, ou32_NodeIndex, ou32_DataPoolSourceIndex,
                                                     ou32_DataPoolTargetIndex);
         c_Tmp[c_Id] = c_It->second;
      }
      this->mc_LastKnownHalcCrcs = c_Tmp;
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Adapt to system definition change

   \param[in]  ou32_NodeIndex       Node index
   \param[in]  ou32_DataPoolIndex   Data pool index
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PuiSvHandler::m_OnSyncNodeDataPoolAboutToBeDeleted(const uint32_t ou32_NodeIndex,
                                                          const uint32_t ou32_DataPoolIndex)
{
   for (uint32_t u32_ItView = 0; u32_ItView < this->mc_Views.size(); ++u32_ItView)
   {
      C_PuiSvData & rc_View = this->mc_Views[u32_ItView];
      rc_View.OnSyncNodeDataPoolAboutToBeDeleted(ou32_NodeIndex, ou32_DataPoolIndex);
   }
   // HALC
   {
      std::map<C_PuiSvDbNodeDataPoolListElementId, C_PuiSvLastKnownHalElementId> c_Tmp;
      for (std::map<C_PuiSvDbNodeDataPoolListElementId, C_PuiSvLastKnownHalElementId>::const_iterator c_It =
              this->mc_LastKnownHalcCrcs.begin();
           c_It != this->mc_LastKnownHalcCrcs.end(); ++c_It)
      {
         C_PuiSvDbNodeDataPoolListElementId c_Id = c_It->first;
         C_PuiSvDashboard::h_OnSyncNodeDataPoolAboutToBeDeleted(c_Id, ou32_NodeIndex, ou32_DataPoolIndex);
         c_Tmp[c_Id] = c_It->second;
      }
      this->mc_LastKnownHalcCrcs = c_Tmp;
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Adapt to system definition change

   \param[in]  ou32_NodeIndex          Node index
   \param[in]  ou32_ApplicationIndex   Application index
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PuiSvHandler::m_OnSyncNodeApplicationAdded(const uint32_t ou32_NodeIndex, const uint32_t ou32_ApplicationIndex)
{
   for (uint32_t u32_ItView = 0; u32_ItView < this->mc_Views.size(); ++u32_ItView)
   {
      C_PuiSvData & rc_View = this->mc_Views[u32_ItView];
      rc_View.OnSyncNodeApplicationAdded(ou32_NodeIndex, ou32_ApplicationIndex);
   }
   //HALC not affected
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Adapt to system definition change

   \param[in]  ou32_NodeIndex                Node index
   \param[in]  ou32_ApplicationSourceIndex   Application source index
   \param[in]  ou32_ApplicationTargetIndex   Application target index
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PuiSvHandler::m_OnSyncNodeApplicationMoved(const uint32_t ou32_NodeIndex,
                                                  const uint32_t ou32_ApplicationSourceIndex,
                                                  const uint32_t ou32_ApplicationTargetIndex)
{
   for (uint32_t u32_ItView = 0; u32_ItView < this->mc_Views.size(); ++u32_ItView)
   {
      C_PuiSvData & rc_View = this->mc_Views[u32_ItView];
      rc_View.OnSyncNodeApplicationMoved(ou32_NodeIndex, ou32_ApplicationSourceIndex, ou32_ApplicationTargetIndex);
   }
   //HALC not affected
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Adapt to system definition change

   \param[in]  ou32_NodeIndex          Node index
   \param[in]  ou32_ApplicationIndex   Application index
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PuiSvHandler::m_OnSyncNodeApplicationAboutToBeDeleted(const uint32_t ou32_NodeIndex,
                                                             const uint32_t ou32_ApplicationIndex)
{
   for (uint32_t u32_ItView = 0; u32_ItView < this->mc_Views.size(); ++u32_ItView)
   {
      C_PuiSvData & rc_View = this->mc_Views[u32_ItView];
      rc_View.OnSyncNodeApplicationAboutToBeDeleted(ou32_NodeIndex, ou32_ApplicationIndex);
   }
   //HALC not affected
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Adapt to system definition change

   \param[in]  ou32_NodeIndex          Node index
   \param[in]  ou32_ApplicationIndex   Application index
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PuiSvHandler::m_OnSyncNodeApplicationAboutToBeChangedFromParamSetHalc(const uint32_t ou32_NodeIndex,
                                                                             const uint32_t ou32_ApplicationIndex)
{
   for (uint32_t u32_ItView = 0; u32_ItView < this->mc_Views.size(); ++u32_ItView)
   {
      C_PuiSvData & rc_View = this->mc_Views[u32_ItView];
      rc_View.OnSyncNodeApplicationAboutToBeChangedFromParamSetHalc(ou32_NodeIndex, ou32_ApplicationIndex);
   }
   //HALC not affected
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Adapt to system definition change

   \param[in]  ou32_NodeIndex          Node index
   \param[in]  ou32_ApplicationIndex   Application index
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PuiSvHandler::m_OnSyncNodeApplicationChangedToParamSetHalc(const uint32_t ou32_NodeIndex,
                                                                  const uint32_t ou32_ApplicationIndex)
{
   for (uint32_t u32_ItView = 0; u32_ItView < this->mc_Views.size(); ++u32_ItView)
   {
      C_PuiSvData & rc_View = this->mc_Views[u32_ItView];
      rc_View.OnSyncNodeApplicationChangedToParamSetHalc(ou32_NodeIndex, ou32_ApplicationIndex);
   }
   //HALC not affected
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Adapt to system definition change

   \param[in]  ou32_NodeIndex          Node index
   \param[in]  ou32_ApplicationIndex   Application index
   \param[in]  ou32_OldSize            Size of application result path before change
   \param[in]  ou32_NewSize            Size of application result path after change
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PuiSvHandler::m_OnSyncNodeApplicationResultPathSizeChanged(const uint32_t ou32_NodeIndex,
                                                                  const uint32_t ou32_ApplicationIndex,
                                                                  const uint32_t ou32_OldSize,
                                                                  const uint32_t ou32_NewSize)
{
   for (uint32_t u32_ItView = 0; u32_ItView < this->mc_Views.size(); ++u32_ItView)
   {
      C_PuiSvData & rc_View = this->mc_Views[u32_ItView];
      rc_View.OnSyncNodeApplicationResultPathSizeChanged(ou32_NodeIndex, ou32_ApplicationIndex, ou32_OldSize,
                                                         ou32_NewSize);
   }
   //HALC not affected
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Adapt to system definition change

   \param[in]  ou32_NodeIndex       Node index
   \param[in]  ou32_DataPoolIndex   Data pool index
   \param[in]  ou32_ListIndex       List index
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PuiSvHandler::m_OnSyncNodeDataPoolListAdded(const uint32_t ou32_NodeIndex, const uint32_t ou32_DataPoolIndex,
                                                   const uint32_t ou32_ListIndex)
{
   for (uint32_t u32_ItView = 0; u32_ItView < this->mc_Views.size(); ++u32_ItView)
   {
      C_PuiSvData & rc_View = this->mc_Views[u32_ItView];
      rc_View.OnSyncNodeDataPoolListAdded(ou32_NodeIndex, ou32_DataPoolIndex, ou32_ListIndex);
   }
   //HALC not affected
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Adapt to system definition change

   \param[in]  ou32_NodeIndex          Node index
   \param[in]  ou32_DataPoolIndex      Data pool index
   \param[in]  ou32_ListSourceIndex    Source list index
   \param[in]  ou32_ListTargetIndex    Target list index
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PuiSvHandler::m_OnSyncNodeDataPoolListMoved(const uint32_t ou32_NodeIndex, const uint32_t ou32_DataPoolIndex,
                                                   const uint32_t ou32_ListSourceIndex,
                                                   const uint32_t ou32_ListTargetIndex)
{
   for (uint32_t u32_ItView = 0; u32_ItView < this->mc_Views.size(); ++u32_ItView)
   {
      C_PuiSvData & rc_View = this->mc_Views[u32_ItView];
      rc_View.OnSyncNodeDataPoolListMoved(ou32_NodeIndex, ou32_DataPoolIndex, ou32_ListSourceIndex,
                                          ou32_ListTargetIndex);
   }
   //HALC not affected
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Adapt to system definition change

   \param[in]  ou32_NodeIndex       Node index
   \param[in]  ou32_DataPoolIndex   Data pool index
   \param[in]  ou32_ListIndex       List index
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PuiSvHandler::m_OnSyncNodeDataPoolListAboutToBeDeleted(const uint32_t ou32_NodeIndex,
                                                              const uint32_t ou32_DataPoolIndex,
                                                              const uint32_t ou32_ListIndex)
{
   for (uint32_t u32_ItView = 0; u32_ItView < this->mc_Views.size(); ++u32_ItView)
   {
      C_PuiSvData & rc_View = this->mc_Views[u32_ItView];
      rc_View.OnSyncNodeDataPoolListAboutToBeDeleted(ou32_NodeIndex, ou32_DataPoolIndex, ou32_ListIndex);
   }
   //HALC not affected
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Adapt to system definition change

   \param[in]  ou32_NodeIndex       Node index
   \param[in]  ou32_DataPoolIndex   Data pool index
   \param[in]  ou32_ListIndex       List index
   \param[in]  ou32_DataSetIndex    Data set index
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PuiSvHandler::m_OnSyncNodeDataPoolListDataSetAdded(const uint32_t ou32_NodeIndex,
                                                          const uint32_t ou32_DataPoolIndex,
                                                          const uint32_t ou32_ListIndex,
                                                          const uint32_t ou32_DataSetIndex)
{
   for (uint32_t u32_ItView = 0; u32_ItView < this->mc_Views.size(); ++u32_ItView)
   {
      C_PuiSvData & rc_View = this->mc_Views[u32_ItView];
      rc_View.OnSyncNodeDataPoolListDataSetAdded(ou32_NodeIndex, ou32_DataPoolIndex, ou32_ListIndex, ou32_DataSetIndex);
   }
   //HALC not affected
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Adapt to system definition change

   \param[in]  ou32_NodeIndex             Node index
   \param[in]  ou32_DataPoolIndex         Data pool index
   \param[in]  ou32_ListIndex             List index
   \param[in]  ou32_DataSetSourceIndex    Source data set index
   \param[in]  ou32_DataSetTargetIndex    Target data set index
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PuiSvHandler::m_OnSyncNodeDataPoolListDataSetMoved(const uint32_t ou32_NodeIndex,
                                                          const uint32_t ou32_DataPoolIndex,
                                                          const uint32_t ou32_ListIndex,
                                                          const uint32_t ou32_DataSetSourceIndex,
                                                          const uint32_t ou32_DataSetTargetIndex)
{
   for (uint32_t u32_ItView = 0; u32_ItView < this->mc_Views.size(); ++u32_ItView)
   {
      C_PuiSvData & rc_View = this->mc_Views[u32_ItView];
      rc_View.OnSyncNodeDataPoolListDataSetMoved(ou32_NodeIndex, ou32_DataPoolIndex, ou32_ListIndex,
                                                 ou32_DataSetSourceIndex, ou32_DataSetTargetIndex);
   }
   //HALC not affected
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Adapt to system definition change

   \param[in]  ou32_NodeIndex       Node index
   \param[in]  ou32_DataPoolIndex   Data pool index
   \param[in]  ou32_ListIndex       List index
   \param[in]  ou32_DataSetIndex    Data set index
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PuiSvHandler::m_OnSyncNodeDataPoolListDataSetAboutToBeDeleted(const uint32_t ou32_NodeIndex,
                                                                     const uint32_t ou32_DataPoolIndex,
                                                                     const uint32_t ou32_ListIndex,
                                                                     const uint32_t ou32_DataSetIndex)
{
   for (uint32_t u32_ItView = 0; u32_ItView < this->mc_Views.size(); ++u32_ItView)
   {
      C_PuiSvData & rc_View = this->mc_Views[u32_ItView];
      rc_View.OnSyncNodeDataPoolListDataSetAboutToBeDeleted(ou32_NodeIndex, ou32_DataPoolIndex, ou32_ListIndex,
                                                            ou32_DataSetIndex);
   }
   //HALC not affected
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Adapt to system definition change

   \param[in]  ou32_NodeIndex       Node index
   \param[in]  ou32_DataPoolIndex   Data pool index
   \param[in]  ou32_ListIndex       List index
   \param[in]  ou32_ElementIndex    Element index
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PuiSvHandler::m_OnSyncNodeDataPoolListElementAdded(const uint32_t ou32_NodeIndex,
                                                          const uint32_t ou32_DataPoolIndex,
                                                          const uint32_t ou32_ListIndex,
                                                          const uint32_t ou32_ElementIndex)
{
   for (uint32_t u32_ItView = 0; u32_ItView < this->mc_Views.size(); ++u32_ItView)
   {
      C_PuiSvData & rc_View = this->mc_Views[u32_ItView];
      rc_View.OnSyncNodeDataPoolListElementAdded(ou32_NodeIndex, ou32_DataPoolIndex, ou32_ListIndex, ou32_ElementIndex);
   }
   //HALC not affected
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Adapt to system definition change

   \param[in]  ou32_NodeIndex             Node index
   \param[in]  ou32_DataPoolIndex         Data pool index
   \param[in]  ou32_ListIndex             List index
   \param[in]  ou32_ElementSourceIndex    Source element index
   \param[in]  ou32_ElementTargetIndex    Target element index
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PuiSvHandler::m_OnSyncNodeDataPoolListElementMoved(const uint32_t ou32_NodeIndex,
                                                          const uint32_t ou32_DataPoolIndex,
                                                          const uint32_t ou32_ListIndex,
                                                          const uint32_t ou32_ElementSourceIndex,
                                                          const uint32_t ou32_ElementTargetIndex)
{
   for (uint32_t u32_ItView = 0; u32_ItView < this->mc_Views.size(); ++u32_ItView)
   {
      C_PuiSvData & rc_View = this->mc_Views[u32_ItView];
      rc_View.OnSyncNodeDataPoolListElementMoved(ou32_NodeIndex, ou32_DataPoolIndex, ou32_ListIndex,
                                                 ou32_ElementSourceIndex, ou32_ElementTargetIndex);
   }
   //HALC not affected
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Adapt to system definition change

   \param[in]  ou32_NodeIndex       Node index
   \param[in]  ou32_DataPoolIndex   Data pool index
   \param[in]  ou32_ListIndex       List index
   \param[in]  ou32_ElementIndex    Element index
   \param[in]  oe_Type              New element type
   \param[in]  oq_IsArray           New array type
   \param[in]  ou32_ArraySize       New array size
   \param[in]  oq_IsString          Flag if new type is string
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PuiSvHandler::m_OnSyncElementTypeOrArrayChanged(const uint32_t ou32_NodeIndex, const uint32_t ou32_DataPoolIndex,
                                                       const uint32_t ou32_ListIndex, const uint32_t ou32_ElementIndex,
                                                       const C_OscNodeDataPoolContent::E_Type oe_Type,
                                                       const bool oq_IsArray, const uint32_t ou32_ArraySize,
                                                       const bool oq_IsString)
{
   for (uint32_t u32_ItView = 0; u32_ItView < this->mc_Views.size(); ++u32_ItView)
   {
      C_PuiSvData & rc_View = this->mc_Views[u32_ItView];
      rc_View.OnSyncElementTypeOrArrayChanged(ou32_NodeIndex, ou32_DataPoolIndex, ou32_ListIndex,
                                              ou32_ElementIndex, oe_Type, oq_IsArray, ou32_ArraySize,
                                              oq_IsString);
   }
   //HALC not affected
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Adapt to system definition change

   \param[in]  ou32_NodeIndex       Node index
   \param[in]  ou32_DataPoolIndex   Data pool index
   \param[in]  ou32_ListIndex       List index
   \param[in]  ou32_ElementIndex    Element index
   \param[in]  orc_MinElement       New min element
   \param[in]  orc_MaxElement       New max element
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PuiSvHandler::m_OnSyncElementRangeChanged(const uint32_t ou32_NodeIndex, const uint32_t ou32_DataPoolIndex,
                                                 const uint32_t ou32_ListIndex, const uint32_t ou32_ElementIndex,
                                                 const C_OscNodeDataPoolContent & orc_MinElement,
                                                 const C_OscNodeDataPoolContent & orc_MaxElement)
{
   for (uint32_t u32_ItView = 0; u32_ItView < this->mc_Views.size(); ++u32_ItView)
   {
      C_PuiSvData & rc_View = this->mc_Views[u32_ItView];
      rc_View.OnSyncElementRangeChanged(ou32_NodeIndex, ou32_DataPoolIndex, ou32_ListIndex,
                                        ou32_ElementIndex, orc_MinElement, orc_MaxElement);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Adapt to system definition change

   \param[in]  ou32_NodeIndex       Node index
   \param[in]  ou32_DataPoolIndex   Data pool index
   \param[in]  ou32_ListIndex       List index
   \param[in]  ou32_ElementIndex    Element index
   \param[in]  oe_Access            New access type
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PuiSvHandler::m_OnSyncNodeDataPoolListElementAccessChanged(const uint32_t ou32_NodeIndex,
                                                                  const uint32_t ou32_DataPoolIndex,
                                                                  const uint32_t ou32_ListIndex,
                                                                  const uint32_t ou32_ElementIndex,
                                                                  const C_OscNodeDataPoolListElement::E_Access oe_Access)
{
   for (uint32_t u32_ItView = 0; u32_ItView < this->mc_Views.size(); ++u32_ItView)
   {
      C_PuiSvData & rc_View = this->mc_Views[u32_ItView];
      rc_View.OnSyncNodeDataPoolListElementAccessChanged(ou32_NodeIndex, ou32_DataPoolIndex, ou32_ListIndex,
                                                         ou32_ElementIndex, oe_Access);
   }
   //HALC not affected
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Adapt to system definition change

   \param[in]  ou32_NodeIndex       Node index
   \param[in]  ou32_DataPoolIndex   Data pool index
   \param[in]  ou32_ListIndex       List index
   \param[in]  ou32_ElementIndex    Element index
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PuiSvHandler::m_OnSyncNodeDataPoolListElementAboutToBeDeleted(const uint32_t ou32_NodeIndex,
                                                                     const uint32_t ou32_DataPoolIndex,
                                                                     const uint32_t ou32_ListIndex,
                                                                     const uint32_t ou32_ElementIndex)
{
   for (uint32_t u32_ItView = 0; u32_ItView < this->mc_Views.size(); ++u32_ItView)
   {
      C_PuiSvData & rc_View = this->mc_Views[u32_ItView];
      rc_View.OnSyncNodeDataPoolListElementAboutToBeDeleted(ou32_NodeIndex, ou32_DataPoolIndex, ou32_ListIndex,
                                                            ou32_ElementIndex);
   }
   //HALC not affected
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   React to system definition clear
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PuiSvHandler::m_OnSyncClear(void)
{
   this->mc_Views.clear();
   this->mc_LastKnownHalcCrcs.clear();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Calculates the hash value of the system definition

   Start value is 0xFFFFFFFF

   \return
   Calculated hash value
*/
//----------------------------------------------------------------------------------------------------------------------
uint32_t C_PuiSvHandler::m_CalcHashSystemViews(void) const
{
   // init value of CRC
   uint32_t u32_Hash = 0xFFFFFFFFU;

   for (std::map<C_PuiSvDbNodeDataPoolListElementId, C_PuiSvLastKnownHalElementId>::const_iterator c_It =
           this->mc_LastKnownHalcCrcs.begin();
        c_It != this->mc_LastKnownHalcCrcs.end(); ++c_It)
   {
      c_It->first.CalcHash(u32_Hash);
      c_It->second.CalcHash(u32_Hash);
      stw::scl::C_SclChecksums::CalcCRC32(&c_It->second, sizeof(uint32_t), u32_Hash);
   }

   for (uint32_t u32_Counter = 0U; u32_Counter < this->mc_Views.size(); ++u32_Counter)
   {
      this->mc_Views[u32_Counter].CalcHash(u32_Hash);
   }

   return u32_Hash;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Fix invalid rail configurations

   Hint: may be introduced by unknown issue not cleaning up rail configurations properly
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PuiSvHandler::m_FixInvalidRailConfig(void)
{
   for (uint32_t u32_Counter = 0U; u32_Counter < this->mc_Views.size(); ++u32_Counter)
   {
      this->mc_Views[u32_Counter].FixInvalidRailConfig();
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Handle compatibility chart
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PuiSvHandler::m_HandleCompatibilityChart(void)
{
   for (uint32_t u32_Counter = 0U; u32_Counter < this->mc_Views.size(); ++u32_Counter)
   {
      this->mc_Views[u32_Counter].HandleCompatibilityChart();
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Fix dashboard write content type
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PuiSvHandler::m_FixDashboardWriteContentType()
{
   for (uint32_t u32_Counter = 0U; u32_Counter < this->mc_Views.size(); ++u32_Counter)
   {
      this->mc_Views[u32_Counter].FixDashboardWriteContentType();
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Check routing error

   \param[in]      ou32_ViewIndex                        View index
   \param[in]      orc_CheckedNodeActiveFlags            Flags for active nodes
   \param[in,out]  orc_SetupWarningRoutingDetails        Warning details for setup case: generic routing,
                                                         update and dashboard routing
   \param[in,out]  orc_ErrorRoutingDetails               Error details for each case: generic routing,
                                                         update and dashboard routing
   \param[in,out]  orc_NodesWithDashboardRoutingError    All nodes with dashboard specific routing errors
   \param[in,out]  orc_NodesRelevantForDashboardRouting  All nodes which are capable for dashboard communication
                                                         or are involved in the route for dashboard communication
                                                         without detected errors

   \return
   C_NO_ERR Operation success
   C_RANGE  Operation failure: parameter invalid
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_PuiSvHandler::m_CheckRoutingDetails(const uint32_t ou32_ViewIndex,
                                              const std::vector<uint8_t> & orc_CheckedNodeActiveFlags,
                                              std::map<uint32_t, QString> & orc_SetupWarningRoutingDetails,
                                              std::vector<std::map<uint32_t, QString> > & orc_ErrorRoutingDetails,
                                              std::set<uint32_t> & orc_NodesWithDashboardRoutingError,
                                              std::set<uint32_t> & orc_NodesRelevantForDashboardRouting)
const
{
   int32_t s32_Retval = C_NO_ERR;

   orc_ErrorRoutingDetails.clear();
   orc_ErrorRoutingDetails.resize(ms32_SUBMODE_SYSVIEW_DASHBOARD + 1);

   if (ou32_ViewIndex < this->mc_Views.size())
   {
      uint32_t u32_Counter;

      // check connection state of all active nodes
      for (u32_Counter = 0U; u32_Counter < orc_CheckedNodeActiveFlags.size(); ++u32_Counter)
      {
         if (orc_CheckedNodeActiveFlags[u32_Counter] == 1)
         {
            const stw::opensyde_core::C_OscNode * const pc_Node = C_PuiSdHandler::h_GetInstance()->GetOscNodeConst(
               u32_Counter);

            if (pc_Node != NULL)
            {
               // Check generic route independent of the specific functionality
               const C_SyvRoRouteCalculation c_RouteCalcCheck(
                  ou32_ViewIndex, u32_Counter,
                  stw::opensyde_core::C_OscRoutingCalculation::eROUTING_CHECK);
               // Check update routes
               const C_SyvRoRouteCalculation c_RouteCalcUpdate(ou32_ViewIndex, u32_Counter,
                                                               stw::opensyde_core::C_OscRoutingCalculation::eUPDATE);
               // Check diagnostic routes
               const C_SyvRoRouteCalculation c_RouteCalcDiag(ou32_ViewIndex, u32_Counter,
                                                             stw::opensyde_core::C_OscRoutingCalculation::eDIAGNOSTIC);

               // Normally both variants must fail in this case on the same way
               if ((c_RouteCalcCheck.GetState() == C_CONFIG) ||
                   (c_RouteCalcUpdate.GetState() == C_CONFIG) ||
                   (c_RouteCalcDiag.GetState() == C_CONFIG))
               {
                  const QString c_Text = static_cast<QString>(pc_Node->c_Properties.c_Name.c_str()) +
                                         C_GtGetText::h_GetText(" (CAN to Ethernet routing not supported)");

                  orc_ErrorRoutingDetails[ms32_SUBMODE_SYSVIEW_SETUP][u32_Counter] = c_Text;
               }
               else if ((c_RouteCalcCheck.GetState() != C_NOACT) ||
                        (c_RouteCalcUpdate.GetState() != C_NOACT) ||
                        (c_RouteCalcDiag.GetState() != C_NOACT))
               {
                  //Error check
                  const bool q_RoutingCheckError = ((c_RouteCalcCheck.WasPcAvailable() == false) ||
                                                    (c_RouteCalcCheck.GetState() != C_NO_ERR));

                  if (q_RoutingCheckError == true)
                  {
                     const QString c_Text = static_cast<QString>(pc_Node->c_Properties.c_Name.c_str()) +
                                            C_GtGetText::h_GetText(
                        " (Communication interface flags \"Routing\" disabled or not supported.)");

                     // Generic routing error independent of the functionality
                     orc_ErrorRoutingDetails[ms32_SUBMODE_SYSVIEW_SETUP][u32_Counter] = c_Text;
                  }
                  else
                  {
                     const bool q_RoutingUpdateError = ((c_RouteCalcUpdate.WasPcAvailable() == false) ||
                                                        (c_RouteCalcUpdate.GetState() != C_NO_ERR));
                     const bool q_RoutingDiagnosticError = ((c_RouteCalcDiag.WasPcAvailable() == false) ||
                                                            (c_RouteCalcDiag.GetState() != C_NO_ERR));

                     if (q_RoutingUpdateError == true)
                     {
                        //Update error
                        orc_ErrorRoutingDetails[ms32_SUBMODE_SYSVIEW_UPDATE][u32_Counter] =
                           static_cast<QString>(pc_Node->c_Properties.c_Name.c_str());
                     }

                     if (q_RoutingDiagnosticError == true)
                     {
                        //Diagnostic error
                        orc_ErrorRoutingDetails[ms32_SUBMODE_SYSVIEW_DASHBOARD][u32_Counter] =
                           static_cast<QString>(pc_Node->c_Properties.c_Name.c_str());

                        // Add the node index to the diagnostic specific errors
                        orc_NodesWithDashboardRoutingError.insert(u32_Counter);
                     }
                     else
                     {
                        // No diagnostic error
                        uint32_t u32_RoutePointCounter;
                        const std::vector<C_OscRoutingRoutePoint> & rc_RoutePoints =
                           c_RouteCalcDiag.GetBestRoute()->c_VecRoutePoints;

                        // Add the target node as relevant for dashboard communication
                        orc_NodesRelevantForDashboardRouting.insert(u32_Counter);

                        // Add all nodes on the route for this node as relevant too
                        for (u32_RoutePointCounter = 0; u32_RoutePointCounter < rc_RoutePoints.size();
                             ++u32_RoutePointCounter)
                        {
                           orc_NodesRelevantForDashboardRouting.insert(
                              rc_RoutePoints[u32_RoutePointCounter].u32_NodeIndex);
                        }
                     }

                     // Only update or dashboard specific -> Warnings for the setup
                     if ((q_RoutingDiagnosticError == true) || (q_RoutingUpdateError == true))
                     {
                        QString c_Text = static_cast<QString>(pc_Node->c_Properties.c_Name.c_str()) +
                                         C_GtGetText::h_GetText(" (");

                        if (q_RoutingUpdateError == true)
                        {
                           c_Text += C_GtGetText::h_GetText("Update ");
                        }

                        if ((q_RoutingDiagnosticError == true) && (q_RoutingUpdateError == true))
                        {
                           // Combined warning
                           c_Text += C_GtGetText::h_GetText("and ");
                        }

                        if (q_RoutingDiagnosticError == true)
                        {
                           c_Text += C_GtGetText::h_GetText("Dashboard ");
                        }

                        c_Text += C_GtGetText::h_GetText("disabled)");

                        orc_SetupWarningRoutingDetails[u32_Counter] = c_Text;
                     }
                  }
               }
               else
               {
                  // Nothing to do
                  // All three calculations returned C_NOACT. This scenario is a node without any openSYDE
                  // or STW Flashloade/KEFEX functionality like a third party device. No error and not relevant
                  // for the protocol specific communication
               }
            }
            else
            {
               // Should not happen
               s32_Retval = C_RANGE;
            }
         }
      }
   }
   else
   {
      s32_Retval = C_RANGE;
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Check routing

   \param[in]      ou32_ViewIndex                       View index
   \param[in]      orc_CheckedNodeActiveFlags           Flags for active nodes
   \param[out]     orq_RoutingError                     Routing error
   \param[in,out]  orc_SetupWarningMessage              Setup specific warning message
   \param[in,out]  orc_ErrorMessages                    Error messages for each sub mode
   \param[in,out]  orc_NodesWithDashboardRoutingError   All nodes with dashboard specific routing errors
   \param[in,out]  orc_NodesRelevantForDashboardRouting All nodes which are capable for dashboard communication
                                                        or are involved in the route for dashboard communication
                                                        without detected errors

   \return
   C_NO_ERR Operation success
   C_RANGE  Operation failure: parameter invalid
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_PuiSvHandler::m_CheckRouting(const uint32_t ou32_ViewIndex,
                                       const std::vector<uint8_t> & orc_CheckedNodeActiveFlags,
                                       QString & orc_SetupWarningMessage, std::vector<QString> & orc_ErrorMessages,
                                       std::set<uint32_t> & orc_NodesWithDashboardRoutingError,
                                       std::set<uint32_t> & orc_NodesRelevantForDashboardRouting) const
{
   std::map<uint32_t, QString> c_SetupWarningDetails;
   std::vector<std::map<uint32_t, QString> > c_ErrorDetails;
   const int32_t s32_Retval = this->m_CheckRoutingDetails(ou32_ViewIndex, orc_CheckedNodeActiveFlags,
                                                          c_SetupWarningDetails, c_ErrorDetails,
                                                          orc_NodesWithDashboardRoutingError,
                                                          orc_NodesRelevantForDashboardRouting);
   uint32_t u32_ErrorCounter;

   orc_ErrorMessages.resize(c_ErrorDetails.size());

   //Error Messages
   for (u32_ErrorCounter = 0; u32_ErrorCounter < c_ErrorDetails.size(); ++u32_ErrorCounter)
   {
      if (c_ErrorDetails[u32_ErrorCounter].size() > 0)
      {
         QString c_Space = " ";
         switch (u32_ErrorCounter)
         {
         case ms32_SUBMODE_SYSVIEW_SETUP:
            orc_ErrorMessages[u32_ErrorCounter] = C_GtGetText::h_GetText(
               "Following nodes can not be reached by the PC:");
            break;
         case ms32_SUBMODE_SYSVIEW_UPDATE:
            if (orc_ErrorMessages[ms32_SUBMODE_SYSVIEW_SETUP].size() == 0)
            {
               // Update view specific error
               orc_ErrorMessages[u32_ErrorCounter] = C_GtGetText::h_GetText(
                  "Following node communication interface flags are disabled for Update:");
            }
            else
            {
               // Generic routing error due to an error in the setup too
               orc_ErrorMessages[u32_ErrorCounter] = C_GtGetText::h_GetText(
                  "Following nodes can not be reached by the PC for Update:");
            }
            break;
         case ms32_SUBMODE_SYSVIEW_DASHBOARD:
            if (orc_ErrorMessages[ms32_SUBMODE_SYSVIEW_SETUP].size() == 0)
            {
               // Update view specific error
               orc_ErrorMessages[u32_ErrorCounter] = C_GtGetText::h_GetText(
                  "Following node communication interface flags are disabled for Dashboard:");
            }
            else
            {
               // Generic routing error due to an error in the setup too
               orc_ErrorMessages[u32_ErrorCounter] = C_GtGetText::h_GetText(
                  "Following nodes can not be reached by the PC for Dashboard:");
            }
            break;
         default:
            break;
         }

         for (std::map<uint32_t, QString>::const_iterator c_It = c_ErrorDetails[u32_ErrorCounter].begin();
              c_It != c_ErrorDetails[u32_ErrorCounter].end();
              ++c_It)
         {
            orc_ErrorMessages[u32_ErrorCounter] += c_Space;
            orc_ErrorMessages[u32_ErrorCounter] += c_It->second;
            c_Space = ", ";
         }
      }
   }

   // Warning message
   if (c_SetupWarningDetails.size() > 0)
   {
      QString c_Space = " ";
      orc_SetupWarningMessage = C_GtGetText::h_GetText(
         "Following node communication interface flags are disabled:");

      for (std::map<uint32_t, QString>::const_iterator c_It = c_SetupWarningDetails.begin();
           c_It != c_SetupWarningDetails.end();
           ++c_It)
      {
         orc_SetupWarningMessage += c_Space;
         orc_SetupWarningMessage += c_It->second;
         c_Space = ", ";
      }
   }

   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get pointers to all currently registered view names

   \return
   Vector of pointers to all currently registered view names
*/
//----------------------------------------------------------------------------------------------------------------------
std::map<stw::scl::C_SclString, bool> C_PuiSvHandler::m_GetExistingViewNames(void) const
{
   std::map<stw::scl::C_SclString, bool> c_Retval;
   for (uint32_t u32_ItView = 0; u32_ItView < this->mc_Views.size(); ++u32_ItView)
   {
      const C_PuiSvData & rc_Data = this->mc_Views[u32_ItView];
      c_Retval[rc_Data.GetName()] = true;
   }
   return c_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get error detail result.

   \param[out]     opq_NameInvalid                       Name invalid
   \param[out]     opq_PcNotConnected                    PC not connected
   \param[out]     opq_RoutingInvalid                    Routing invalid
   \param[out]     opq_RoutingUpdateInvalid              Update routing invalid
   \param[out]     opq_RoutingDashboardInvalid           Dashboard routing invalid
   \param[out]     opq_SysDefInvalid                     System definition invalid
   \param[out]     opq_NoNodesActive                     No nodes active
   \param[out]     opc_RoutingErrorDetails               Routing error details
   \param[out]     opc_SetupRoutingWarningDetails        Setup specific warning details
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PuiSvHandler::C_PuiSvViewErrorDetails::GetResults(bool * const opq_NameInvalid, bool * const opq_PcNotConnected,
                                                         bool * const opq_RoutingInvalid,
                                                         bool * const opq_RoutingUpdateInvalid,
                                                         bool * const opq_RoutingDashboardInvalid,
                                                         bool * const opq_SysDefInvalid, bool * const opq_NoNodesActive,
                                                         std::vector<QString> * const opc_RoutingErrorDetails,
                                                         QString * const opc_SetupRoutingWarningDetails) const
{
   if (opq_NameInvalid != NULL)
   {
      *opq_NameInvalid = this->q_NameInvalid;
   }
   if (opq_PcNotConnected != NULL)
   {
      *opq_PcNotConnected = this->q_PcNotConnected;
   }

   // Routing errors depending of the submode
   if (opq_RoutingInvalid != NULL)
   {
      *opq_RoutingInvalid = (this->c_RoutingErrorMessages[ms32_SUBMODE_SYSVIEW_SETUP].size() > 0);
   }
   if (opq_RoutingUpdateInvalid != NULL)
   {
      *opq_RoutingUpdateInvalid = (this->c_RoutingErrorMessages[ms32_SUBMODE_SYSVIEW_UPDATE].size() > 0);
   }
   if (opq_RoutingDashboardInvalid != NULL)
   {
      *opq_RoutingDashboardInvalid = (this->c_RoutingErrorMessages[ms32_SUBMODE_SYSVIEW_DASHBOARD].size() > 0);
   }
   if (opc_RoutingErrorDetails != NULL)
   {
      *opc_RoutingErrorDetails = this->c_RoutingErrorMessages;
   }

   // Routing warnings for setup sub mode
   if (opc_SetupRoutingWarningDetails != NULL)
   {
      *opc_SetupRoutingWarningDetails = this->c_RoutingSetupWarningMessage;
   }

   if (opq_SysDefInvalid != NULL)
   {
      *opq_SysDefInvalid = this->q_SysDefInvalid;
   }
   if (opq_NoNodesActive != NULL)
   {
      *opq_NoNodesActive = this->q_NoNodesActive;
   }
}
