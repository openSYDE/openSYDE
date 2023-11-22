//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       System view filer (V1 format) (implementation)

   System view filer (V1 format)

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.hpp"

#include <QMap>
#include "stwtypes.hpp"
#include "stwerrors.hpp"
#include "TglUtils.hpp"
#include "C_SclString.hpp"
#include "constants.hpp"
#include "C_OscNodeDataPoolFilerV2.hpp"
#include "C_PuiSvHandlerFilerV1.hpp"
#include "C_PuiBsElementsFiler.hpp"
#include "C_PuiSdHandler.hpp"
#include "C_OscNode.hpp"
#include "C_OscLoggingHandler.hpp"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw::scl;
using namespace stw::tgl;
using namespace stw::errors;
using namespace stw::opensyde_core;
using namespace stw::opensyde_gui_logic;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

/* -- Global Variables ---------------------------------------------------------------------------------------------- */

/* -- Module Global Variables --------------------------------------------------------------------------------------- */

/* -- Module Global Function Prototypes ----------------------------------------------------------------------------- */

/* -- Implementation ------------------------------------------------------------------------------------------------ */

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default constructor
*/
//----------------------------------------------------------------------------------------------------------------------
C_PuiSvHandlerFilerV1::C_PuiSvHandlerFilerV1(void)
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Load system views

   \param[in,out] orc_Views     System view elements (Cleared if necessary)
   \param[in,out] orc_XmlParser XML parser with the "current" element set to the "opensyde-system-views" element

   \return
   C_NO_ERR    information loaded
   C_CONFIG    error loading information
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_PuiSvHandlerFilerV1::h_LoadViews(std::vector<C_PuiSvData> & orc_Views, C_OscXmlParserBase & orc_XmlParser)
{
   int32_t s32_Retval = C_NO_ERR;
   C_SclString c_CurrentViewNode;
   uint32_t u32_ExpectedSize = 0UL;
   const bool q_ExpectedSizeHere = orc_XmlParser.AttributeExists("length");

   //Check optional length
   if (q_ExpectedSizeHere == true)
   {
      u32_ExpectedSize = orc_XmlParser.GetAttributeUint32("length");
      orc_Views.reserve(u32_ExpectedSize);
   }

   //Clear last views
   orc_Views.clear();

   c_CurrentViewNode = orc_XmlParser.SelectNodeChild("opensyde-system-view");
   if (c_CurrentViewNode == "opensyde-system-view")
   {
      do
      {
         C_PuiSvData c_View;
         s32_Retval = mh_LoadView(c_View, orc_XmlParser);
         orc_Views.push_back(c_View);
         //Next
         c_CurrentViewNode = orc_XmlParser.SelectNodeNext("opensyde-system-view");
      }
      while ((c_CurrentViewNode == "opensyde-system-view") && (s32_Retval == C_NO_ERR));
      //Return
      tgl_assert(orc_XmlParser.SelectNodeParent() == "opensyde-system-views");
   }
   //Compare length
   if ((s32_Retval == C_NO_ERR) && (q_ExpectedSizeHere == true))
   {
      if (u32_ExpectedSize != orc_Views.size())
      {
         C_SclString c_Tmp;
         c_Tmp.PrintFormatted("Unexpected view count, expected: %i, got %i", u32_ExpectedSize,
                              orc_Views.size());
         osc_write_log_warning("Load file", c_Tmp.c_str());
      }
   }

   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Save data pools

   \param[in]     orc_Views     System view elements
   \param[in,out] orc_XmlParser XML parser with the "current" element set to the "opensyde-system-views" element
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PuiSvHandlerFilerV1::h_SaveViews(const std::vector<C_PuiSvData> & orc_Views, C_OscXmlParserBase & orc_XmlParser)
{
   orc_XmlParser.SetAttributeUint32("length", orc_Views.size());
   for (uint32_t u32_ItView = 0; u32_ItView < orc_Views.size(); ++u32_ItView)
   {
      orc_XmlParser.CreateAndSelectNodeChild("opensyde-system-view");
      mh_SaveView(orc_Views[u32_ItView], orc_XmlParser);
      //Return
      tgl_assert(orc_XmlParser.SelectNodeParent() == "opensyde-system-views");
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Load dashboard element

   \param[in,out] orc_Dashboard           Dashboard element
   \param[in,out] orc_XmlParser           XML parser with the "current" element set to the "dashboard" element
   \param[in]     oq_IgnoreMostErrorCases Optional flag to allow some error cases to still return C_NO_ERR

   \return
   C_NO_ERR    information loaded
   C_CONFIG    error loading information
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_PuiSvHandlerFilerV1::h_LoadDashboard(C_PuiSvDashboard & orc_Dashboard, C_OscXmlParserBase & orc_XmlParser,
                                               const bool oq_IgnoreMostErrorCases)
{
   int32_t s32_Retval = C_NO_ERR;

   if (orc_XmlParser.SelectNodeChild("name") == "name")
   {
      orc_Dashboard.SetName(orc_XmlParser.GetNodeContent().c_str());
      //Return
      orc_XmlParser.SelectNodeParent();
   }
   else
   {
      if (oq_IgnoreMostErrorCases == false)
      {
         s32_Retval = C_CONFIG;
      }
   }

   if (orc_XmlParser.SelectNodeChild("comment") == "comment")
   {
      orc_Dashboard.SetComment(orc_XmlParser.GetNodeContent().c_str());
      //Return
      orc_XmlParser.SelectNodeParent();
   }

   if (s32_Retval == C_NO_ERR)
   {
      std::vector<C_PuiSvDbChart> c_Widgets;
      s32_Retval = mh_LoadCharts(c_Widgets, orc_XmlParser);
      orc_Dashboard.SetCharts(c_Widgets);
      if (oq_IgnoreMostErrorCases == true)
      {
         s32_Retval = C_NO_ERR;
      }
   }
   if (s32_Retval == C_NO_ERR)
   {
      std::vector<C_PuiSvDbLabel> c_Widgets;
      s32_Retval = mh_LoadLabels(c_Widgets, orc_XmlParser);
      orc_Dashboard.SetLabels(c_Widgets);
      if (oq_IgnoreMostErrorCases == true)
      {
         s32_Retval = C_NO_ERR;
      }
   }
   if (s32_Retval == C_NO_ERR)
   {
      std::vector<C_PuiSvDbParam> c_Widgets;
      s32_Retval = mh_LoadParams(c_Widgets, orc_XmlParser);
      orc_Dashboard.SetParams(c_Widgets);
      if (oq_IgnoreMostErrorCases == true)
      {
         s32_Retval = C_NO_ERR;
      }
   }
   if (s32_Retval == C_NO_ERR)
   {
      std::vector<C_PuiSvDbPieChart> c_Widgets;
      s32_Retval = mh_LoadPieCharts(c_Widgets, orc_XmlParser);
      orc_Dashboard.SetPieCharts(c_Widgets);
      if (oq_IgnoreMostErrorCases == true)
      {
         s32_Retval = C_NO_ERR;
      }
   }
   if (s32_Retval == C_NO_ERR)
   {
      std::vector<C_PuiSvDbSpinBox> c_Widgets;
      s32_Retval = mh_LoadSpinBoxes(c_Widgets, orc_XmlParser);
      orc_Dashboard.SetSpinBoxes(c_Widgets);
      if (oq_IgnoreMostErrorCases == true)
      {
         s32_Retval = C_NO_ERR;
      }
   }
   if (s32_Retval == C_NO_ERR)
   {
      std::vector<C_PuiSvDbSlider> c_Widgets;
      s32_Retval = mh_LoadSliders(c_Widgets, orc_XmlParser);
      orc_Dashboard.SetSliders(c_Widgets);
      if (oq_IgnoreMostErrorCases == true)
      {
         s32_Retval = C_NO_ERR;
      }
   }
   if (s32_Retval == C_NO_ERR)
   {
      std::vector<C_PuiSvDbProgressBar> c_Widgets;
      s32_Retval = mh_LoadProgressBars(c_Widgets, orc_XmlParser);
      orc_Dashboard.SetProgressBars(c_Widgets);
      if (oq_IgnoreMostErrorCases == true)
      {
         s32_Retval = C_NO_ERR;
      }
   }
   if (s32_Retval == C_NO_ERR)
   {
      std::vector<C_PuiSvDbTable> c_Widgets;
      s32_Retval = mh_LoadTables(c_Widgets, orc_XmlParser);
      orc_Dashboard.SetTables(c_Widgets);
      if (oq_IgnoreMostErrorCases == true)
      {
         s32_Retval = C_NO_ERR;
      }
   }
   if (s32_Retval == C_NO_ERR)
   {
      std::vector<C_PuiSvDbToggle> c_Widgets;
      s32_Retval = mh_LoadToggles(c_Widgets, orc_XmlParser);
      orc_Dashboard.SetToggles(c_Widgets);
      if (oq_IgnoreMostErrorCases == true)
      {
         s32_Retval = C_NO_ERR;
      }
   }
   if (s32_Retval == C_NO_ERR)
   {
      s32_Retval = C_PuiBsElementsFiler::h_LoadBaseElements(orc_Dashboard, orc_XmlParser);
   }
   orc_Dashboard.SetActive(orc_XmlParser.GetAttributeBool("active"));
   //Default
   orc_Dashboard.SetType(C_PuiSvDashboard::eSCENE);
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Save dashboard element

   \param[in]     orc_Dashboard Dashboard element
   \param[in,out] orc_XmlParser XML parser with the "current" element set to the "dashboard" element
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PuiSvHandlerFilerV1::h_SaveDashboard(const C_PuiSvDashboard & orc_Dashboard, C_OscXmlParserBase & orc_XmlParser)
{
   orc_XmlParser.SetAttributeBool("active", orc_Dashboard.GetActive());
   orc_XmlParser.CreateNodeChild("name", orc_Dashboard.GetName().toStdString().c_str());
   orc_XmlParser.CreateNodeChild("comment", orc_Dashboard.GetComment().toStdString().c_str());
   mh_SaveCharts(orc_Dashboard.GetCharts(), orc_XmlParser);
   mh_SaveLabels(orc_Dashboard.GetLabels(), orc_XmlParser);
   mh_SaveParams(orc_Dashboard.GetParams(), orc_XmlParser);
   mh_SavePieCharts(orc_Dashboard.GetPieCharts(), orc_XmlParser);
   mh_SaveSpinBoxes(orc_Dashboard.GetSpinBoxes(), orc_XmlParser);
   mh_SaveSliders(orc_Dashboard.GetSliders(), orc_XmlParser);
   mh_SaveProgressBars(orc_Dashboard.GetProgressBars(), orc_XmlParser);
   mh_SaveTables(orc_Dashboard.GetTables(), orc_XmlParser);
   mh_SaveToggles(orc_Dashboard.GetToggles(), orc_XmlParser);
   C_PuiBsElementsFiler::h_SaveBaseElements(orc_Dashboard, orc_XmlParser);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Load read rails

   \param[in,out] orc_Rails     Read rails
   \param[in,out] orc_XmlParser XML parser with the "current" element set to the "rail-assignments" element

   \return
   C_NO_ERR    information loaded
   C_CONFIG    error loading information
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_PuiSvHandlerFilerV1::h_LoadReadRails(QMap<C_OscNodeDataPoolListElementId,
                                                    C_PuiSvReadDataConfiguration> & orc_Rails,
                                               C_OscXmlParserBase & orc_XmlParser)
{
   int32_t s32_Retval = C_NO_ERR;

   C_SclString c_CurrentRailNode = orc_XmlParser.SelectNodeChild("rail-assignment");

   if (c_CurrentRailNode == "rail-assignment")
   {
      do
      {
         C_PuiSvReadDataConfiguration c_DataConfiguration;
         C_OscNodeDataPoolListElementId c_Id;
         c_DataConfiguration.u8_RailIndex = static_cast<uint8_t>(orc_XmlParser.GetAttributeUint32("rail-index"));
         if (orc_XmlParser.SelectNodeChild("id") == "id")
         {
            C_PuiSvHandlerFilerV1::mh_LoadDataElement(c_Id, orc_XmlParser);
            //Return
            tgl_assert(orc_XmlParser.SelectNodeParent() == "rail-assignment");
         }
         else
         {
            s32_Retval = C_CONFIG;
         }
         if (orc_XmlParser.SelectNodeChild("threshold") == "threshold")
         {
            if (C_OscNodeDataPoolFilerV2::h_LoadDataPoolContentV1(c_DataConfiguration.c_ChangeThreshold,
                                                                  orc_XmlParser) != C_NO_ERR)
            {
               s32_Retval = C_CONFIG;
            }
            //Return
            tgl_assert(orc_XmlParser.SelectNodeParent() == "rail-assignment");
         }
         else
         {
            s32_Retval = C_CONFIG;
         }
         if (orc_XmlParser.SelectNodeChild("transmission-mode") == "transmission-mode")
         {
            if (C_PuiSvHandlerFilerV1::mh_StringToTransmissionMode(orc_XmlParser.GetNodeContent().c_str(),
                                                                   c_DataConfiguration.e_TransmissionMode) != C_NO_ERR)
            {
               s32_Retval = C_CONFIG;
            }
            //Return
            tgl_assert(orc_XmlParser.SelectNodeParent() == "rail-assignment");
         }
         else
         {
            s32_Retval = C_CONFIG;
         }
         //Insert
         orc_Rails.insert(c_Id, c_DataConfiguration);
         //Next
         c_CurrentRailNode = orc_XmlParser.SelectNodeNext("rail-assignment");
      }
      while ((c_CurrentRailNode == "rail-assignment") && (s32_Retval == C_NO_ERR));
      //Return
      tgl_assert(orc_XmlParser.SelectNodeParent() == "rail-assignments");
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Save read rails

   \param[in]     orc_Rails     Read rails
   \param[in,out] orc_XmlParser XML parser with the "current" element set to the "rail-assignments" element
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PuiSvHandlerFilerV1::h_SaveReadRails(const QMap<C_OscNodeDataPoolListElementId,
                                                       C_PuiSvReadDataConfiguration> & orc_Rails,
                                            C_OscXmlParserBase & orc_XmlParser)
{
   for (QMap<C_OscNodeDataPoolListElementId, C_PuiSvReadDataConfiguration>::const_iterator c_It = orc_Rails.begin();
        c_It != orc_Rails.end(); ++c_It)
   {
      const C_PuiSvReadDataConfiguration c_ReadData = c_It.value();
      orc_XmlParser.CreateAndSelectNodeChild("rail-assignment");
      orc_XmlParser.SetAttributeUint32("rail-index", static_cast<uint32_t>(c_ReadData.u8_RailIndex));
      orc_XmlParser.CreateAndSelectNodeChild("id");
      mh_SaveDataElement(c_It.key(), orc_XmlParser);
      //Return
      tgl_assert(orc_XmlParser.SelectNodeParent() == "rail-assignment");
      orc_XmlParser.CreateAndSelectNodeChild("threshold");
      C_OscNodeDataPoolFilerV2::h_SaveDataPoolContentV1(c_ReadData.c_ChangeThreshold, orc_XmlParser);
      //Return
      tgl_assert(orc_XmlParser.SelectNodeParent() == "rail-assignment");
      orc_XmlParser.CreateNodeChild("transmission-mode",
                                    C_PuiSvHandlerFilerV1::mh_TransmissionModeToString(
                                       c_ReadData.e_TransmissionMode).toStdString().c_str());
      //Return
      tgl_assert(orc_XmlParser.SelectNodeParent() == "rail-assignments");
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Load node active flags

   \param[in,out] orc_NodeActiveFlags Node active flags (Cleared if necessary)
   \param[in,out] orc_XmlParser       XML parser with the "current" element set to the "opensyde-system-view" element

   \return
   C_NO_ERR    information loaded
   C_CONFIG    error loading information
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_PuiSvHandlerFilerV1::mh_LoadNodeActiveFlags(std::vector<uint8_t> & orc_NodeActiveFlags,
                                                      C_OscXmlParserBase & orc_XmlParser)
{
   int32_t s32_Retval = C_NO_ERR;

   //Clear last node active flags
   orc_NodeActiveFlags.clear();
   if (orc_XmlParser.SelectNodeChild("active-nodes") == "active-nodes")
   {
      C_SclString c_CurrentNodeActiveFlagNode = orc_XmlParser.SelectNodeChild("active-node");
      if (c_CurrentNodeActiveFlagNode == "active-node")
      {
         do
         {
            const bool q_CurrentValue = orc_XmlParser.GetAttributeBool("state");
            orc_NodeActiveFlags.push_back(static_cast<uint8_t>(q_CurrentValue));
            //Next
            c_CurrentNodeActiveFlagNode = orc_XmlParser.SelectNodeNext("active-node");
         }
         while (c_CurrentNodeActiveFlagNode == "active-node");
         //Return
         tgl_assert(orc_XmlParser.SelectNodeParent() == "active-nodes");
      }
      //Return
      tgl_assert(orc_XmlParser.SelectNodeParent() == "opensyde-system-view");
   }
   else
   {
      s32_Retval = C_CONFIG;
   }

   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Load node update information

   System definition must be loaded already.

   \param[in,out] orc_NodeUpdateInformation Node update information (Cleared if necessary)
   \param[in,out] orc_XmlParser             XML parser with the "current" element set
                                            to the "opensyde-system-view" element

   \return
   C_NO_ERR    information loaded
   C_CONFIG    error loading information
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_PuiSvHandlerFilerV1::mh_LoadNodeUpdateInformation(
   std::vector<C_OscViewNodeUpdate> & orc_NodeUpdateInformation, C_OscXmlParserBase & orc_XmlParser)
{
   int32_t s32_Retval = C_NO_ERR;

   //Clear last
   orc_NodeUpdateInformation.clear();
   //Previous implementation
   if (orc_XmlParser.SelectNodeChild("node-update-informations") == "node-update-informations")
   {
      C_SclString c_CurrentNodeUpdateInformationNode = orc_XmlParser.SelectNodeChild("node-update-information");
      if (c_CurrentNodeUpdateInformationNode == "node-update-information")
      {
         uint32_t u32_Counter = 0U;

         do
         {
            const C_OscNode * const pc_Node = C_PuiSdHandler::h_GetInstance()->GetOscNodeConst(u32_Counter);

            if (pc_Node != NULL)
            {
               C_OscViewNodeUpdate c_UpdateInfo;

               s32_Retval = mh_LoadOneNodeUpdateInformation(c_UpdateInfo, orc_XmlParser, *pc_Node);
               orc_NodeUpdateInformation.push_back(c_UpdateInfo);
               //Next
               c_CurrentNodeUpdateInformationNode = orc_XmlParser.SelectNodeNext("node-update-information");
               //Iterate the counter!
               ++u32_Counter;
            }
            else
            {
               // For each update information must exist a node
               s32_Retval = C_CONFIG;
            }
         }
         while ((c_CurrentNodeUpdateInformationNode == "node-update-information") && (s32_Retval == C_NO_ERR));
         //Return
         tgl_assert(orc_XmlParser.SelectNodeParent() == "node-update-informations");
      }
      //Return
      tgl_assert(orc_XmlParser.SelectNodeParent() == "opensyde-system-view");
   }
   else
   {
      //New implementation
      if (orc_XmlParser.SelectNodeChild("node-update-information") == "node-update-information")
      {
         C_SclString c_CurrentNodeUpdateInformationNode = orc_XmlParser.SelectNodeChild(
            "node-specific-update-information");
         if (c_CurrentNodeUpdateInformationNode == "node-specific-update-information")
         {
            uint32_t u32_Counter = 0U;

            do
            {
               const C_OscNode * const pc_Node = C_PuiSdHandler::h_GetInstance()->GetOscNodeConst(u32_Counter);

               if (pc_Node != NULL)
               {
                  C_OscViewNodeUpdate c_UpdateInfo;
                  s32_Retval = mh_LoadOneNodeUpdateInformation(c_UpdateInfo, orc_XmlParser, *pc_Node);
                  orc_NodeUpdateInformation.push_back(c_UpdateInfo);
                  //Next
                  c_CurrentNodeUpdateInformationNode = orc_XmlParser.SelectNodeNext("node-specific-update-information");
                  //Iterate the counter!
                  ++u32_Counter;
               }
               else
               {
                  // For each update information must exist a node
                  s32_Retval = C_CONFIG;
               }
            }
            while ((c_CurrentNodeUpdateInformationNode == "node-specific-update-information") &&
                   (s32_Retval == C_NO_ERR));
            //Return
            tgl_assert(orc_XmlParser.SelectNodeParent() == "node-update-information");
         }
         //Return
         tgl_assert(orc_XmlParser.SelectNodeParent() == "opensyde-system-view");
      }
      else
      {
         osc_write_log_error("Loading view", "Node \"node-update-information\" not found.");
         s32_Retval = C_CONFIG;
      }
   }

   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Load node update information

   \param[in,out] orc_NodeUpdateInformation Node update information
   \param[in,out] orc_XmlParser             XML parser with the "current" element set
                                            to the "node-update-information" or
                                                   "node-specific-update-information" element

   \return
   C_NO_ERR    information loaded
   C_CONFIG    error loading information
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_PuiSvHandlerFilerV1::mh_LoadOneNodeUpdateInformation(C_OscViewNodeUpdate & orc_NodeUpdateInformation,
                                                               C_OscXmlParserBase & orc_XmlParser,
                                                               const C_OscNode & orc_Node)
{
   int32_t s32_Retval = C_NO_ERR;

   if (orc_XmlParser.AttributeExists("position") == true)
   {
      orc_NodeUpdateInformation.u32_NodeUpdatePosition = orc_XmlParser.GetAttributeUint32("position");
   }
   else
   {
      orc_NodeUpdateInformation.u32_NodeUpdatePosition = 0;
   }

   if (orc_XmlParser.SelectNodeChild("paths") == "paths")
   {
      C_SclString c_CurrentNodeUpdateInformationNode = orc_XmlParser.SelectNodeChild("path");
      if (c_CurrentNodeUpdateInformationNode == "path")
      {
         std::vector<C_SclString> c_Paths;
         std::vector<bool> c_PathsSkipFlags;
         do
         {
            c_Paths.push_back(orc_XmlParser.GetNodeContent());
            //Next
            c_CurrentNodeUpdateInformationNode = orc_XmlParser.SelectNodeNext("path");
         }
         while (c_CurrentNodeUpdateInformationNode == "path");

         // Adapt size for skip flags for compatibility
         c_PathsSkipFlags.resize(orc_Node.c_Applications.size(), false);

         if (orc_Node.c_Applications.size() == c_Paths.size())
         {
            //matching size
            orc_NodeUpdateInformation.SetPaths(c_Paths, C_OscViewNodeUpdate::eFTP_DATA_BLOCK);
            orc_NodeUpdateInformation.SetSkipUpdateOfPathsFlags(c_PathsSkipFlags, C_OscViewNodeUpdate::eFTP_DATA_BLOCK);
         }
         else
         {
            //invalid size
            c_Paths.clear();
            c_Paths.resize(orc_Node.c_Applications.size(), "");
            orc_NodeUpdateInformation.SetPaths(c_Paths, C_OscViewNodeUpdate::eFTP_DATA_BLOCK);
            orc_NodeUpdateInformation.SetSkipUpdateOfPathsFlags(c_PathsSkipFlags, C_OscViewNodeUpdate::eFTP_DATA_BLOCK);
         }
         //Return
         tgl_assert(orc_XmlParser.SelectNodeParent() == "paths");
      }
      //Return
      orc_XmlParser.SelectNodeParent();
   }
   else
   {
      s32_Retval = C_CONFIG;
   }

   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Load dashboard elements

   \param[in,out] orc_Dashboards Dashboard elements (Cleared if necessary)
   \param[in,out] orc_XmlParser  XML parser with the "current" element set to the "opensyde-system-view" element

   \return
   C_NO_ERR    information loaded
   C_CONFIG    error loading information
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_PuiSvHandlerFilerV1::mh_LoadDashboards(std::vector<C_PuiSvDashboard> & orc_Dashboards,
                                                 C_OscXmlParserBase & orc_XmlParser)
{
   int32_t s32_Retval = C_NO_ERR;

   //Clear last dashboards
   orc_Dashboards.clear();
   if (orc_XmlParser.SelectNodeChild("dashboards") == "dashboards")
   {
      C_SclString c_CurrentDashboardNode = orc_XmlParser.SelectNodeChild("dashboard");
      if (c_CurrentDashboardNode == "dashboard")
      {
         do
         {
            C_PuiSvDashboard c_Dashboard;
            s32_Retval = h_LoadDashboard(c_Dashboard, orc_XmlParser);
            orc_Dashboards.push_back(c_Dashboard);
            //Next
            c_CurrentDashboardNode = orc_XmlParser.SelectNodeNext("dashboard");
         }
         while ((c_CurrentDashboardNode == "dashboard") && (s32_Retval == C_NO_ERR));
         //Return
         tgl_assert(orc_XmlParser.SelectNodeParent() == "dashboards");
      }
      //Return
      tgl_assert(orc_XmlParser.SelectNodeParent() == "opensyde-system-view");
   }
   else
   {
      s32_Retval = C_CONFIG;
   }

   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Load view element

   \param[in,out] orc_View      View element
   \param[in,out] orc_XmlParser XML parser with the "current" element set to the "opensyde-system-view" element

   \return
   C_NO_ERR    information loaded
   C_CONFIG    error loading information
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_PuiSvHandlerFilerV1::mh_LoadView(C_PuiSvData & orc_View, C_OscXmlParserBase & orc_XmlParser)
{
   int32_t s32_Retval = C_NO_ERR;

   if (orc_XmlParser.AttributeExists("darkmode") == true)
   {
      orc_View.SetDarkModeActive(orc_XmlParser.GetAttributeBool("darkmode"));
   }
   else
   {
      //Default
      orc_View.SetDarkModeActive(false);
   }
   if (orc_XmlParser.AttributeExists("device-config-selected-bit-rate") == true)
   {
      orc_View.SetDeviceConfigSelectedBitRate(orc_XmlParser.GetAttributeUint32("device-config-selected-bit-rate"));
   }
   else
   {
      //Default
      orc_View.SetDeviceConfigSelectedBitRate(125U);
   }
   if (orc_XmlParser.SelectNodeChild("name") == "name")
   {
      orc_View.SetName(orc_XmlParser.GetNodeContent().c_str());
      //Return
      tgl_assert(orc_XmlParser.SelectNodeParent() == "opensyde-system-view");
   }
   else
   {
      osc_write_log_error("Loading view", "Node \"name\" not found.");
      s32_Retval = C_CONFIG;
   }
   if ((orc_XmlParser.SelectNodeChild("device-config-mode") == "device-config-mode") && (s32_Retval == C_NO_ERR))
   {
      orc_View.SetDeviceConfigMode(mh_StringToDeviceConfigMode(orc_XmlParser.GetNodeContent().c_str()));
      //Return
      tgl_assert(orc_XmlParser.SelectNodeParent() == "opensyde-system-view");
   }
   else
   {
      //Default
      orc_View.SetDeviceConfigMode(C_PuiSvData::eDCM_ALL_CONNECTED_INTERFACES);
   }
   if (orc_XmlParser.SelectNodeChild("update-rates") == "update-rates")
   {
      orc_View.SetUpdateRateFast(static_cast<uint16_t>(orc_XmlParser.GetAttributeUint32("fast")));
      orc_View.SetUpdateRateMedium(static_cast<uint16_t>(orc_XmlParser.GetAttributeUint32("medium")));
      orc_View.SetUpdateRateSlow(static_cast<uint16_t>(orc_XmlParser.GetAttributeUint32("slow")));
      //Return
      tgl_assert(orc_XmlParser.SelectNodeParent() == "opensyde-system-view");
   }
   else
   {
      osc_write_log_error("Loading view", "Node \"update-rates\" not found.");
      s32_Retval = C_CONFIG;
   }
   if (s32_Retval == C_NO_ERR)
   {
      std::vector<uint8_t> c_NodeActiveFlags;
      s32_Retval = mh_LoadNodeActiveFlags(c_NodeActiveFlags, orc_XmlParser);
      orc_View.SetNodeActiveFlags(c_NodeActiveFlags);
      if (s32_Retval == C_NO_ERR)
      {
         std::vector<C_OscViewNodeUpdate> c_NodeUpdateInformation;
         //If you have an async project this might help
         //c_NodeUpdateInformation.resize(c_NodeActiveFlags.size(),C_OscViewNodeUpdate());
         s32_Retval = mh_LoadNodeUpdateInformation(c_NodeUpdateInformation, orc_XmlParser);
         orc_View.SetNodeUpdateInformation(c_NodeUpdateInformation);
         if (s32_Retval == C_NO_ERR)
         {
            if (orc_XmlParser.SelectNodeChild("pc") == "pc")
            {
               C_OscViewPc c_OscPcData;
               C_PuiSvPc c_PuiPcData;
               s32_Retval = mh_LoadPc(c_OscPcData, c_PuiPcData, orc_XmlParser);
               orc_View.SetOscPcData(c_OscPcData);
               orc_View.SetPuiPcData(c_PuiPcData);
               //Return
               tgl_assert(orc_XmlParser.SelectNodeParent() == "opensyde-system-view");
            }
            else
            {
               osc_write_log_error("Loading view", "Node \"pc\" not found.");
               s32_Retval = C_CONFIG;
            }
            if (s32_Retval == C_NO_ERR)
            {
               std::vector<C_PuiSvDashboard> c_Dashboards;
               s32_Retval = mh_LoadDashboards(c_Dashboards, orc_XmlParser);
               orc_View.SetDashboards(c_Dashboards);
            }
         }
         else
         {
            osc_write_log_error("Loading view", "Could not load update information");
         }
      }
   }
   if (s32_Retval == C_NO_ERR)
   {
      if (orc_XmlParser.SelectNodeChild("rail-assignments") == "rail-assignments")
      {
         QMap<C_OscNodeDataPoolListElementId, C_PuiSvReadDataConfiguration> c_Rails;
         s32_Retval = h_LoadReadRails(c_Rails, orc_XmlParser);
         orc_View.SetReadRailAssignments(c_Rails);
         //Return
         tgl_assert(orc_XmlParser.SelectNodeParent() == "opensyde-system-view");
      }
      else
      {
         osc_write_log_error("Loading view", "Node \"rail-assignments\" not found.");
         s32_Retval = C_CONFIG;
      }
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Load PC element

   \param[in,out]  orc_OscPc    PC element (core part)
   \param[in,out]  orc_PuiPc    PC element (GUI part)
   \param[in,out] orc_XmlParser XML parser with the "current" element set to the "pc" element

   \return
   C_NO_ERR    information loaded
   C_CONFIG    error loading information
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_PuiSvHandlerFilerV1::mh_LoadPc(C_OscViewPc & orc_OscPc, C_PuiSvPc & orc_PuiPc,
                                         C_OscXmlParserBase & orc_XmlParser)
{
   int32_t s32_Retval = C_NO_ERR;

   orc_OscPc.SetConnected(orc_XmlParser.GetAttributeBool("connected"), orc_XmlParser.GetAttributeUint32("bus-index"),
                          true);
   if (orc_XmlParser.SelectNodeChild("dll-path") == "dll-path")
   {
      QString c_Path = orc_XmlParser.GetNodeContent().c_str();
      if (orc_XmlParser.AttributeExists("type"))
      {
         orc_PuiPc.SetCanDllType(static_cast<C_PuiSvPc::E_CanDllType>(orc_XmlParser.GetAttributeSint32("type")));
      }
      else
      {
         // translate from path to type+path for compatibility reasons (type was not present in old openSYDE versions)
         if ((c_Path == stw::opensyde_gui::mc_DLL_PATH_PEAK) || (c_Path.isEmpty() == true))
         {
            orc_PuiPc.SetCanDllType(C_PuiSvPc::E_CanDllType::ePEAK);
            c_Path = "";
         }
         else if (c_Path == stw::opensyde_gui::mc_DLL_PATH_VECTOR)
         {
            orc_PuiPc.SetCanDllType(C_PuiSvPc::E_CanDllType::eVECTOR);
            c_Path = "";
         }
         else
         {
            orc_PuiPc.SetCanDllType(C_PuiSvPc::E_CanDllType::eOTHER);
         }
      }
      orc_PuiPc.SetCustomCanDllPath(c_Path);
      //Return
      tgl_assert(orc_XmlParser.SelectNodeParent() == "pc");
   }
   else
   {
      s32_Retval = C_CONFIG;
   }
   if ((orc_XmlParser.SelectNodeChild("box") == "box") && (s32_Retval == C_NO_ERR))
   {
      s32_Retval = C_PuiBsElementsFiler::h_LoadBoxBase(orc_PuiPc, orc_XmlParser);
      //Return
      tgl_assert(orc_XmlParser.SelectNodeParent() == "pc");
   }
   else
   {
      s32_Retval = C_CONFIG;
   }
   if ((orc_XmlParser.SelectNodeChild("connection") == "connection") && (s32_Retval == C_NO_ERR))
   {
      if (orc_XmlParser.SelectNodeChild("line") == "line")
      {
         C_PuiBsLineBase c_Connection;
         s32_Retval = C_PuiBsElementsFiler::h_LoadLineBase(c_Connection, orc_XmlParser);
         orc_PuiPc.SetConnectionData(c_Connection);
         //Return
         tgl_assert(orc_XmlParser.SelectNodeParent() == "connection");
      }
      else
      {
         s32_Retval = C_CONFIG;
      }
      //Return
      tgl_assert(orc_XmlParser.SelectNodeParent() == "pc");
   }
   else
   {
      s32_Retval = C_CONFIG;
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Load widget elements

   \param[in,out] orc_Widgets    Widget elements (Cleared if necessary)
   \param[in,out] orc_XmlParser  XML parser with the "current" element set to the "dashboard" element

   \return
   C_NO_ERR    information loaded
   C_CONFIG    error loading information
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_PuiSvHandlerFilerV1::mh_LoadCharts(std::vector<C_PuiSvDbChart> & orc_Widgets,
                                             C_OscXmlParserBase & orc_XmlParser)
{
   int32_t s32_Retval = C_NO_ERR;

   //Clear last widgets
   orc_Widgets.clear();
   if (orc_XmlParser.SelectNodeChild("charts") == "charts")
   {
      C_SclString c_CurrentWidgetNode = orc_XmlParser.SelectNodeChild("chart");
      if (c_CurrentWidgetNode == "chart")
      {
         do
         {
            C_PuiSvDbChart c_Box;
            s32_Retval = C_PuiSvHandlerFilerV1::mh_LoadWidgetBase(c_Box, orc_XmlParser);
            if (orc_XmlParser.SelectNodeChild("active-flags") == "active-flags")
            {
               C_SclString c_CurrentWidgetNode2 = orc_XmlParser.SelectNodeChild("active-flag");
               if (c_CurrentWidgetNode2 == "active-flag")
               {
                  do
                  {
                     if (orc_XmlParser.AttributeExists("state") == true)
                     {
                        c_Box.c_DataPoolElementsActive.push_back(orc_XmlParser.GetAttributeBool("state"));
                     }
                     else
                     {
                        s32_Retval = C_CONFIG;
                     }
                     //Next
                     c_CurrentWidgetNode2 = orc_XmlParser.SelectNodeNext("active-flag");
                  }
                  while ((c_CurrentWidgetNode2 == "active-flag") && (s32_Retval == C_NO_ERR));

                  //Return
                  tgl_assert(orc_XmlParser.SelectNodeParent() == "active-flags");
               }
               //Return
               tgl_assert(orc_XmlParser.SelectNodeParent() == "chart");
            }

            if (c_Box.c_DataPoolElementsActive.size() != c_Box.c_DataPoolElementsConfig.size())
            {
               // Size shall be identical
               c_Box.c_DataPoolElementsActive.resize(c_Box.c_DataPoolElementsConfig.size(), true);
            }

            orc_Widgets.push_back(c_Box);
            //Next
            c_CurrentWidgetNode = orc_XmlParser.SelectNodeNext("chart");
         }
         while ((c_CurrentWidgetNode == "chart") && (s32_Retval == C_NO_ERR));
         //Return
         tgl_assert(orc_XmlParser.SelectNodeParent() == "charts");
      }
      //Return
      orc_XmlParser.SelectNodeParent();
   }
   else
   {
      s32_Retval = C_CONFIG;
   }

   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Load widget elements

   \param[in,out] orc_Widgets    Widget elements (Cleared if necessary)
   \param[in,out] orc_XmlParser  XML parser with the "current" element set to the "dashboard" element

   \return
   C_NO_ERR    information loaded
   C_CONFIG    error loading information
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_PuiSvHandlerFilerV1::mh_LoadLabels(std::vector<C_PuiSvDbLabel> & orc_Widgets,
                                             C_OscXmlParserBase & orc_XmlParser)
{
   int32_t s32_Retval = C_NO_ERR;

   //Clear last widgets
   orc_Widgets.clear();
   if (orc_XmlParser.SelectNodeChild("labels") == "labels")
   {
      C_SclString c_CurrentWidgetNode = orc_XmlParser.SelectNodeChild("label");
      if (c_CurrentWidgetNode == "label")
      {
         do
         {
            C_PuiSvDbLabel c_Box;
            c_Box.q_ShowCaption = orc_XmlParser.GetAttributeBool("show-caption");
            c_Box.q_ShowUnit = orc_XmlParser.GetAttributeBool("show-unit");
            s32_Retval = C_PuiSvHandlerFilerV1::mh_LoadWidgetBase(c_Box, orc_XmlParser);
            if (orc_XmlParser.SelectNodeChild("caption") == "caption")
            {
               c_Box.c_Caption = orc_XmlParser.GetNodeContent().c_str();
               //Return
               tgl_assert(orc_XmlParser.SelectNodeParent() == "label");
            }
            if (orc_XmlParser.SelectNodeChild("type") == "type")
            {
               if (mh_StringToLabelType(orc_XmlParser.GetNodeContent().c_str(), c_Box.e_Type) != C_NO_ERR)
               {
                  s32_Retval = C_CONFIG;
               }
               //Return
               tgl_assert(orc_XmlParser.SelectNodeParent() == "label");
            }
            else
            {
               s32_Retval = C_CONFIG;
            }
            orc_Widgets.push_back(c_Box);
            //Next
            c_CurrentWidgetNode = orc_XmlParser.SelectNodeNext("label");
         }
         while ((c_CurrentWidgetNode == "label") && (s32_Retval == C_NO_ERR));
         //Return
         tgl_assert(orc_XmlParser.SelectNodeParent() == "labels");
      }
      //Return
      orc_XmlParser.SelectNodeParent();
   }
   else
   {
      s32_Retval = C_CONFIG;
   }

   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Load widget elements

   \param[in,out] orc_Widgets    Widget elements (Cleared if necessary)
   \param[in,out] orc_XmlParser  XML parser with the "current" element set to the "dashboard" element

   \return
   C_NO_ERR    information loaded
   C_CONFIG    error loading information
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_PuiSvHandlerFilerV1::mh_LoadParams(std::vector<C_PuiSvDbParam> & orc_Widgets,
                                             C_OscXmlParserBase & orc_XmlParser)
{
   int32_t s32_Retval = C_NO_ERR;

   //Clear last widgets
   orc_Widgets.clear();
   if (orc_XmlParser.SelectNodeChild("params") == "params")
   {
      C_SclString c_CurrentWidgetNode = orc_XmlParser.SelectNodeChild("param");
      if (c_CurrentWidgetNode == "param")
      {
         do
         {
            C_PuiSvDbParam c_Box;
            s32_Retval = C_PuiSvHandlerFilerV1::mh_LoadWidgetBase(c_Box, orc_XmlParser);
            if (s32_Retval == C_NO_ERR)
            {
               s32_Retval = mh_LoadParamDataSetIndices(c_Box.c_DataSetSelectionIndices, orc_XmlParser);
               if (s32_Retval == C_NO_ERR)
               {
                  s32_Retval = mh_LoadParamValues(c_Box.c_ListValues, orc_XmlParser);
                  if (s32_Retval == C_NO_ERR)
                  {
                     s32_Retval = mh_LoadParamTables(c_Box.c_ColWidth, orc_XmlParser);
                     if (s32_Retval == C_NO_ERR)
                     {
                        mh_LoadParamExpandedItems(c_Box.c_ExpandedItems, orc_XmlParser);
                        mh_LoadParamColumnPositionIndices(c_Box.c_ColPosIndices, orc_XmlParser);
                     }
                     //Fix to delete param widget
                     //if (s32_Retval != C_NO_ERR)
                     //{
                     //   c_Box.c_ColWidth.resize(c_Box.c_ListValues.size());
                     //   s32_Retval = C_NO_ERR;
                     //}
                  }
               }
            }
            orc_Widgets.push_back(c_Box);
            //Next
            c_CurrentWidgetNode = orc_XmlParser.SelectNodeNext("param");
         }
         while ((c_CurrentWidgetNode == "param") && (s32_Retval == C_NO_ERR));
         //Return
         tgl_assert(orc_XmlParser.SelectNodeParent() == "params");
      }
      //Return
      orc_XmlParser.SelectNodeParent();
   }
   else
   {
      s32_Retval = C_CONFIG;
   }

   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Load tree elements

   \param[out]    orc_Items     Tree elements
   \param[in,out] orc_XmlParser XML parser with the "current" element set to the "param" element
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PuiSvHandlerFilerV1::mh_LoadParamExpandedItems(std::vector<C_PuiSvDbExpandedTreeIndex> & orc_Items,
                                                      C_OscXmlParserBase & orc_XmlParser)
{
   orc_Items.clear();
   if (orc_XmlParser.SelectNodeChild("expanded-tree-items") == "expanded-tree-items")
   {
      C_SclString c_CurrentWidgetNode = orc_XmlParser.SelectNodeChild("expanded-tree-item");
      if (c_CurrentWidgetNode == "expanded-tree-item")
      {
         do
         {
            C_PuiSvDbExpandedTreeIndex c_Item;
            if (orc_XmlParser.AttributeExists("valid-layers") == true)
            {
               c_Item.u32_Layer = orc_XmlParser.GetAttributeUint32("valid-layers");
            }
            else
            {
               c_Item.u32_Layer = 0UL;
            }
            C_PuiSvHandlerFilerV1::mh_LoadUiIndex(c_Item.c_ExpandedId, orc_XmlParser);
            orc_Items.push_back(c_Item);
            //Next
            c_CurrentWidgetNode = orc_XmlParser.SelectNodeNext("expanded-tree-item");
         }
         while (c_CurrentWidgetNode == "expanded-tree-item");
         //Return
         tgl_assert(orc_XmlParser.SelectNodeParent() == "expanded-tree-items");
      }
      //Return
      tgl_assert(orc_XmlParser.SelectNodeParent() == "param");
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Load column position indices elements

   \param[out]    orc_Items     Column position indices elements
   \param[in,out] orc_XmlParser XML parser with the "current" element set to the "param" element
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PuiSvHandlerFilerV1::mh_LoadParamColumnPositionIndices(std::vector<int32_t> & orc_Items,
                                                              C_OscXmlParserBase & orc_XmlParser)
{
   orc_Items.clear();
   if (orc_XmlParser.SelectNodeChild("column-position-indices") == "column-position-indices")
   {
      C_SclString c_CurrentWidgetNode = orc_XmlParser.SelectNodeChild("column-position-index");
      if (c_CurrentWidgetNode == "column-position-index")
      {
         do
         {
            int32_t s32_Index;
            if (orc_XmlParser.AttributeExists("value") == true)
            {
               s32_Index = orc_XmlParser.GetAttributeSint32("value");
            }
            else
            {
               s32_Index = orc_Items.size();
            }
            orc_Items.push_back(s32_Index);
            //Next
            c_CurrentWidgetNode = orc_XmlParser.SelectNodeNext("column-position-index");
         }
         while (c_CurrentWidgetNode == "column-position-index");
         //Return
         tgl_assert(orc_XmlParser.SelectNodeParent() == "column-position-indices");
      }
      //Return
      tgl_assert(orc_XmlParser.SelectNodeParent() == "param");
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Load widget elements

   \param[in,out] orc_Widgets    Widget elements (Cleared if necessary)
   \param[in,out] orc_XmlParser  XML parser with the "current" element set to the "dashboard" element

   \return
   C_NO_ERR    information loaded
   C_CONFIG    error loading information
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_PuiSvHandlerFilerV1::mh_LoadPieCharts(std::vector<C_PuiSvDbPieChart> & orc_Widgets,
                                                C_OscXmlParserBase & orc_XmlParser)
{
   int32_t s32_Retval = C_NO_ERR;

   //Clear last widgets
   orc_Widgets.clear();
   if (orc_XmlParser.SelectNodeChild("pie-charts") == "pie-charts")
   {
      C_SclString c_CurrentWidgetNode = orc_XmlParser.SelectNodeChild("pie-chart");
      if (c_CurrentWidgetNode == "pie-chart")
      {
         do
         {
            C_PuiSvDbPieChart c_Box;
            c_Box.q_ShowValue = orc_XmlParser.GetAttributeBool("show-value");
            c_Box.q_ShowUnit = orc_XmlParser.GetAttributeBool("show-unit");
            s32_Retval = C_PuiSvHandlerFilerV1::mh_LoadWidgetBase(c_Box, orc_XmlParser);
            orc_Widgets.push_back(c_Box);
            //Next
            c_CurrentWidgetNode = orc_XmlParser.SelectNodeNext("pie-chart");
         }
         while ((c_CurrentWidgetNode == "pie-chart") && (s32_Retval == C_NO_ERR));
         //Return
         tgl_assert(orc_XmlParser.SelectNodeParent() == "pie-charts");
      }
      //Return
      orc_XmlParser.SelectNodeParent();
   }
   else
   {
      s32_Retval = C_CONFIG;
   }

   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Load widget elements

   \param[in,out] orc_Widgets    Widget elements (Cleared if necessary)
   \param[in,out] orc_XmlParser  XML parser with the "current" element set to the "dashboard" element

   \return
   C_NO_ERR    information loaded
   C_CONFIG    error loading information
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_PuiSvHandlerFilerV1::mh_LoadSpinBoxes(std::vector<C_PuiSvDbSpinBox> & orc_Widgets,
                                                C_OscXmlParserBase & orc_XmlParser)
{
   int32_t s32_Retval = C_NO_ERR;

   //Clear last widgets
   orc_Widgets.clear();
   if (orc_XmlParser.SelectNodeChild("spin-boxes") == "spin-boxes")
   {
      C_SclString c_CurrentWidgetNode = orc_XmlParser.SelectNodeChild("spin-box");
      if (c_CurrentWidgetNode == "spin-box")
      {
         do
         {
            C_PuiSvDbSpinBox c_Box;
            c_Box.q_ShowUnit = orc_XmlParser.GetAttributeBool("show-unit");
            s32_Retval = C_PuiSvHandlerFilerV1::mh_LoadWidgetBase(c_Box, orc_XmlParser);
            if (orc_XmlParser.SelectNodeChild("type") == "type")
            {
               if (mh_StringToSpinBoxType(orc_XmlParser.GetNodeContent().c_str(), c_Box.e_Type) != C_NO_ERR)
               {
                  s32_Retval = C_CONFIG;
               }
               //Return
               tgl_assert(orc_XmlParser.SelectNodeParent() == "spin-box");
            }
            else
            {
               s32_Retval = C_CONFIG;
            }
            if (orc_XmlParser.SelectNodeChild("content") == "content")
            {
               if (C_OscNodeDataPoolFilerV2::h_LoadDataPoolContentV1(c_Box.c_Value, orc_XmlParser) != C_NO_ERR)
               {
                  s32_Retval = C_CONFIG;
               }
               //Return
               tgl_assert(orc_XmlParser.SelectNodeParent() == "spin-box");
            }
            orc_Widgets.push_back(c_Box);
            //Next
            c_CurrentWidgetNode = orc_XmlParser.SelectNodeNext("spin-box");
         }
         while ((c_CurrentWidgetNode == "spin-box") && (s32_Retval == C_NO_ERR));
         //Return
         tgl_assert(orc_XmlParser.SelectNodeParent() == "spin-boxes");
      }
      //Return
      orc_XmlParser.SelectNodeParent();
   }
   else
   {
      s32_Retval = C_CONFIG;
   }

   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Load widget elements

   \param[in,out] orc_Widgets    Widget elements (Cleared if necessary)
   \param[in,out] orc_XmlParser  XML parser with the "current" element set to the "dashboard" element

   \return
   C_NO_ERR    information loaded
   C_CONFIG    error loading information
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_PuiSvHandlerFilerV1::mh_LoadTables(std::vector<C_PuiSvDbTable> & orc_Widgets,
                                             C_OscXmlParserBase & orc_XmlParser)
{
   int32_t s32_Retval = C_NO_ERR;

   //Clear last widgets
   orc_Widgets.clear();
   if (orc_XmlParser.SelectNodeChild("tables") == "tables")
   {
      C_SclString c_CurrentWidgetNode = orc_XmlParser.SelectNodeChild("table");
      if (c_CurrentWidgetNode == "table")
      {
         do
         {
            C_PuiSvDbTable c_Box;
            s32_Retval = C_PuiSvHandlerFilerV1::mh_LoadWidgetBase(c_Box, orc_XmlParser);

            //Columns
            if (orc_XmlParser.SelectNodeChild("columns") == "columns")
            {
               C_SclString c_CurrentColumnNode = orc_XmlParser.SelectNodeChild("column");
               if (c_CurrentColumnNode == "column")
               {
                  do
                  {
                     if (orc_XmlParser.AttributeExists("width") == true)
                     {
                        c_Box.c_ColumnWidth.push_back(orc_XmlParser.GetAttributeSint32("width"));
                     }
                     else
                     {
                        s32_Retval = C_CONFIG;
                     }
                     //Next
                     c_CurrentColumnNode = orc_XmlParser.SelectNodeNext("column");
                  }
                  while ((c_CurrentColumnNode == "column") && (s32_Retval == C_NO_ERR));
                  //Return
                  tgl_assert(orc_XmlParser.SelectNodeParent() == "columns");
               }
               //Return
               tgl_assert(orc_XmlParser.SelectNodeParent() == "table");
            }

            orc_Widgets.push_back(c_Box);
            //Next
            c_CurrentWidgetNode = orc_XmlParser.SelectNodeNext("table");
         }
         while ((c_CurrentWidgetNode == "table") && (s32_Retval == C_NO_ERR));
         //Return
         tgl_assert(orc_XmlParser.SelectNodeParent() == "tables");
      }
      //Return
      orc_XmlParser.SelectNodeParent();
   }
   else
   {
      s32_Retval = C_CONFIG;
   }

   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Load widget elements

   \param[in,out] orc_Widgets    Widget elements (Cleared if necessary)
   \param[in,out] orc_XmlParser  XML parser with the "current" element set to the "dashboard" element

   \return
   C_NO_ERR    information loaded
   C_CONFIG    error loading information
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_PuiSvHandlerFilerV1::mh_LoadSliders(std::vector<C_PuiSvDbSlider> & orc_Widgets,
                                              C_OscXmlParserBase & orc_XmlParser)
{
   int32_t s32_Retval = C_NO_ERR;

   //Clear last widgets
   orc_Widgets.clear();
   if (orc_XmlParser.SelectNodeChild("sliders") == "sliders")
   {
      C_SclString c_CurrentWidgetNode = orc_XmlParser.SelectNodeChild("slider");
      if (c_CurrentWidgetNode == "slider")
      {
         do
         {
            C_PuiSvDbSlider c_Box;
            s32_Retval = C_PuiSvHandlerFilerV1::mh_LoadWidgetBase(c_Box, orc_XmlParser);
            c_Box.q_ShowMinMax = orc_XmlParser.GetAttributeBool("show-min-max");
            if (s32_Retval == C_NO_ERR)
            {
               if (orc_XmlParser.SelectNodeChild("type") == "type")
               {
                  if (C_PuiSvHandlerFilerV1::mh_StringToSliderType(orc_XmlParser.GetNodeContent().c_str(),
                                                                   c_Box.e_Type) != C_NO_ERR)
                  {
                     s32_Retval = C_CONFIG;
                  }
                  //Return
                  tgl_assert(orc_XmlParser.SelectNodeParent() == "slider");
               }
               else
               {
                  s32_Retval = C_CONFIG;
               }
            }
            //Value
            if (orc_XmlParser.AttributeExists("value") == true)
            {
               c_Box.s32_Value = orc_XmlParser.GetAttributeSint32("value");
            }
            else
            {
               c_Box.s32_Value = 0;
            }
            //Add
            orc_Widgets.push_back(c_Box);
            //Next
            c_CurrentWidgetNode = orc_XmlParser.SelectNodeNext("slider");
         }
         while ((c_CurrentWidgetNode == "slider") && (s32_Retval == C_NO_ERR));
         //Return
         tgl_assert(orc_XmlParser.SelectNodeParent() == "sliders");
      }
      //Return
      orc_XmlParser.SelectNodeParent();
   }
   else
   {
      s32_Retval = C_CONFIG;
   }

   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Load widget elements

   \param[in,out] orc_Widgets    Widget elements (Cleared if necessary)
   \param[in,out] orc_XmlParser  XML parser with the "current" element set to the "dashboard" element

   \return
   C_NO_ERR    information loaded
   C_CONFIG    error loading information
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_PuiSvHandlerFilerV1::mh_LoadProgressBars(std::vector<C_PuiSvDbProgressBar> & orc_Widgets,
                                                   C_OscXmlParserBase & orc_XmlParser)
{
   int32_t s32_Retval = C_NO_ERR;

   //Clear last widgets
   orc_Widgets.clear();
   if (orc_XmlParser.SelectNodeChild("progress-bars") == "progress-bars")
   {
      C_SclString c_CurrentWidgetNode = orc_XmlParser.SelectNodeChild("progress-bar");
      if (c_CurrentWidgetNode == "progress-bar")
      {
         do
         {
            C_PuiSvDbProgressBar c_Box;
            s32_Retval = C_PuiSvHandlerFilerV1::mh_LoadWidgetBase(c_Box, orc_XmlParser);
            c_Box.q_ShowMinMax = orc_XmlParser.GetAttributeBool("show-min-max");
            if (s32_Retval == C_NO_ERR)
            {
               if (orc_XmlParser.AttributeExists("show-unit") == true)
               {
                  c_Box.q_ShowUnit = orc_XmlParser.GetAttributeBool("show-unit");
               }
               else
               {
                  c_Box.q_ShowUnit = false;
               }
               if (orc_XmlParser.SelectNodeChild("type") == "type")
               {
                  if (C_PuiSvHandlerFilerV1::mh_StringToProgressBarType(orc_XmlParser.GetNodeContent().c_str(),
                                                                        c_Box.e_Type) != C_NO_ERR)
                  {
                     s32_Retval = C_CONFIG;
                  }
                  //Return
                  tgl_assert(orc_XmlParser.SelectNodeParent() == "progress-bar");
               }
               else
               {
                  s32_Retval = C_CONFIG;
               }
               if (s32_Retval == C_NO_ERR)
               {
                  if (orc_XmlParser.SelectNodeChild("alignment") == "alignment")
                  {
                     if (C_PuiSvHandlerFilerV1::mh_StringToProgressBarAlignmentType(orc_XmlParser.GetNodeContent().c_str(),
                                                                                    c_Box.e_Alignment) != C_NO_ERR)
                     {
                        s32_Retval = C_CONFIG;
                     }
                     //Return
                     tgl_assert(orc_XmlParser.SelectNodeParent() == "progress-bar");
                  }
                  else
                  {
                     s32_Retval = C_CONFIG;
                  }
               }
            }
            orc_Widgets.push_back(c_Box);
            //Next
            c_CurrentWidgetNode = orc_XmlParser.SelectNodeNext("progress-bar");
         }
         while ((c_CurrentWidgetNode == "progress-bar") && (s32_Retval == C_NO_ERR));
         //Return
         tgl_assert(orc_XmlParser.SelectNodeParent() == "progress-bars");
      }
      //Return
      orc_XmlParser.SelectNodeParent();
   }
   else
   {
      s32_Retval = C_CONFIG;
   }

   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Load widget elements

   \param[in,out] orc_Widgets    Widget elements (Cleared if necessary)
   \param[in,out] orc_XmlParser  XML parser with the "current" element set to the "dashboard" element

   \return
   C_NO_ERR    information loaded
   C_CONFIG    error loading information
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_PuiSvHandlerFilerV1::mh_LoadToggles(std::vector<C_PuiSvDbToggle> & orc_Widgets,
                                              C_OscXmlParserBase & orc_XmlParser)
{
   int32_t s32_Retval = C_NO_ERR;

   //Clear last widgets
   orc_Widgets.clear();
   if (orc_XmlParser.SelectNodeChild("toggles") == "toggles")
   {
      C_SclString c_CurrentWidgetNode = orc_XmlParser.SelectNodeChild("toggle");
      if (c_CurrentWidgetNode == "toggle")
      {
         do
         {
            C_PuiSvDbToggle c_Box;
            s32_Retval = C_PuiSvHandlerFilerV1::mh_LoadWidgetBase(c_Box, orc_XmlParser);
            if (orc_XmlParser.SelectNodeChild("type") == "type")
            {
               if (mh_StringToToggleType(orc_XmlParser.GetNodeContent().c_str(), c_Box.e_Type) != C_NO_ERR)
               {
                  s32_Retval = C_CONFIG;
               }
               //Return
               tgl_assert(orc_XmlParser.SelectNodeParent() == "toggle");
            }
            else
            {
               s32_Retval = C_CONFIG;
            }
            //Content
            if (orc_XmlParser.AttributeExists("state") == true)
            {
               c_Box.q_State = orc_XmlParser.GetAttributeBool("state");
            }
            else
            {
               c_Box.q_State = false;
            }
            //Add
            orc_Widgets.push_back(c_Box);
            //Next
            c_CurrentWidgetNode = orc_XmlParser.SelectNodeNext("toggle");
         }
         while ((c_CurrentWidgetNode == "toggle") && (s32_Retval == C_NO_ERR));
         //Return
         tgl_assert(orc_XmlParser.SelectNodeParent() == "toggles");
      }
      //Return
      orc_XmlParser.SelectNodeParent();
   }
   else
   {
      s32_Retval = C_CONFIG;
   }

   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Load widget element

   \param[in,out]  orc_Widget       Widget element
   \param[in,out]  orc_XmlParser    XML parser with the "current" element set to the any widget element

   \return
   C_NO_ERR    information loaded
   C_CONFIG    error loading information
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_PuiSvHandlerFilerV1::mh_LoadWidgetBase(C_PuiSvDbWidgetBase & orc_Widget, C_OscXmlParserBase & orc_XmlParser)
{
   int32_t s32_Retval = C_NO_ERR;

   if (orc_XmlParser.SelectNodeChild("base") == "base")
   {
      if (orc_XmlParser.SelectNodeChild("box") == "box")
      {
         C_PuiBsElementsFiler::h_LoadBoxBase(orc_Widget, orc_XmlParser);
         //Return
         tgl_assert(orc_XmlParser.SelectNodeParent() == "base");
      }
      else
      {
         s32_Retval = C_CONFIG;
      }
      if ((orc_XmlParser.SelectNodeChild("data-pool-elements") == "data-pool-elements") && (s32_Retval == C_NO_ERR))
      {
         C_SclString c_CurrentDataPoolElementNode = orc_XmlParser.SelectNodeChild("data-pool-element");
         if (c_CurrentDataPoolElementNode == "data-pool-element")
         {
            do
            {
               C_PuiSvDbNodeDataElementConfig c_ElementConfig;
               mh_LoadUiIndex(c_ElementConfig.c_ElementId, orc_XmlParser);
               if (orc_XmlParser.SelectNodeChild("scaling") == "scaling")
               {
                  c_ElementConfig.c_ElementScaling.q_UseDefault = orc_XmlParser.GetAttributeBool("use-default");
                  c_ElementConfig.c_ElementScaling.f64_Factor = orc_XmlParser.GetAttributeFloat64("factor");
                  c_ElementConfig.c_ElementScaling.f64_Offset = orc_XmlParser.GetAttributeFloat64("offset");
                  if (orc_XmlParser.SelectNodeChild("unit") == "unit")
                  {
                     c_ElementConfig.c_ElementScaling.c_Unit = orc_XmlParser.GetNodeContent().c_str();
                     //Return
                     tgl_assert(orc_XmlParser.SelectNodeParent() == "scaling");
                  }
                  //Return
                  tgl_assert(orc_XmlParser.SelectNodeParent() == "data-pool-element");
               }
               else
               {
                  s32_Retval = C_CONFIG;
               }
               if (orc_XmlParser.SelectNodeChild("display-name") == "display-name")
               {
                  c_ElementConfig.c_DisplayName = orc_XmlParser.GetNodeContent().c_str();
                  //Return
                  tgl_assert(orc_XmlParser.SelectNodeParent() == "data-pool-element");
               }
               else
               {
                  c_ElementConfig.c_DisplayName = "";
               }

               orc_Widget.c_DataPoolElementsConfig.push_back(c_ElementConfig);
               //Next
               c_CurrentDataPoolElementNode = orc_XmlParser.SelectNodeNext("data-pool-element");
            }
            while ((c_CurrentDataPoolElementNode == "data-pool-element") && (s32_Retval == C_NO_ERR));
            //Return
            tgl_assert(orc_XmlParser.SelectNodeParent() == "data-pool-elements");
         }
         //Return
         tgl_assert(orc_XmlParser.SelectNodeParent() == "base");
      }
      else
      {
         s32_Retval = C_CONFIG;
      }
      if ((orc_XmlParser.SelectNodeChild("display-style") == "display-style") && (s32_Retval == C_NO_ERR))
      {
         if (C_PuiSvHandlerFilerV1::mh_StringToDisplayStyle(orc_XmlParser.GetNodeContent().c_str(),
                                                            orc_Widget.e_DisplayStyle) != C_NO_ERR)
         {
            s32_Retval = C_CONFIG;
         }
         //Return
         tgl_assert(orc_XmlParser.SelectNodeParent() == "base");
      }
      else
      {
         s32_Retval = C_CONFIG;
      }
      if (s32_Retval == C_NO_ERR)
      {
         C_PuiSvDbWriteWidgetBase * const pc_WriteBase = dynamic_cast<C_PuiSvDbWriteWidgetBase * const>(&orc_Widget);
         if (pc_WriteBase != NULL)
         {
            s32_Retval = C_PuiSvHandlerFilerV1::mh_LoadWriteWidgetBase(*pc_WriteBase, orc_XmlParser);
         }
      }
      //Return
      orc_XmlParser.SelectNodeParent();
   }
   else
   {
      s32_Retval = C_CONFIG;
   }

   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Load write widget base

   \param[in,out]  orc_WriteWidget  Write widget
   \param[in,out]  orc_XmlParser    XML parser with the "current" element set to the base element

   \return
   C_NO_ERR    information loaded
   C_CONFIG    error loading information
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_PuiSvHandlerFilerV1::mh_LoadWriteWidgetBase(C_PuiSvDbWriteWidgetBase & orc_WriteWidget,
                                                      C_OscXmlParserBase & orc_XmlParser)
{
   int32_t s32_Retval = C_NO_ERR;

   if (orc_XmlParser.SelectNodeChild("write-mode") == "write-mode")
   {
      if (C_PuiSvHandlerFilerV1::mh_StringToWriteMode(orc_XmlParser.GetNodeContent().c_str(),
                                                      orc_WriteWidget.e_ElementWriteMode) != C_NO_ERR)
      {
         s32_Retval = C_CONFIG;
      }
      //Return
      tgl_assert(orc_XmlParser.SelectNodeParent() == "base");
   }
   else
   {
      s32_Retval = C_CONFIG;
   }

   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Load a UI index

   \param[out]    orc_Id        Read index
   \param[in,out] orc_XmlParser XML parser
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PuiSvHandlerFilerV1::mh_LoadUiIndex(C_PuiSvDbNodeDataPoolListElementId & orc_Id,
                                           C_OscXmlParserBase & orc_XmlParser)
{
   const bool q_IsValid = orc_XmlParser.GetAttributeBool("is-valid");

   if (orc_XmlParser.SelectNodeChild("index") == "index")
   {
      const uint32_t u32_NodeIndex = orc_XmlParser.GetAttributeUint32("node");
      const uint32_t u32_DataPoolIndex = orc_XmlParser.GetAttributeUint32("data-pool");
      const uint32_t u32_ListIndex = orc_XmlParser.GetAttributeUint32("list");
      const uint32_t u32_ElementIndex = orc_XmlParser.GetAttributeUint32("element");
      const C_OscNodeDataPoolListElementId c_Base(u32_NodeIndex, u32_DataPoolIndex, u32_ListIndex,
                                                  u32_ElementIndex);
      C_OscNodeDataPool::E_Type e_InvalidTypePlaceholder = C_OscNodeDataPool::eDIAG;
      QString c_InvalidNamePlaceholder = "";
      C_PuiSvDbNodeDataPoolListElementId::E_Type e_SourceType;

      //Return
      orc_XmlParser.SelectNodeParent();
      if (orc_XmlParser.SelectNodeChild("source-type") == "source-type")
      {
         C_PuiSvHandlerFilerV1::mh_StringToSourceType(orc_XmlParser.GetNodeContent().c_str(), e_SourceType);
         //Return
         orc_XmlParser.SelectNodeParent();
      }
      else
      {
         //Save default for previous projects
         e_SourceType = C_PuiSvDbNodeDataPoolListElementId::eDATAPOOL_ELEMENT;
      }
      if (orc_XmlParser.SelectNodeChild("invalid-type-placeholder") == "invalid-type-placeholder")
      {
         C_OscNodeDataPoolFilerV2::h_StringToDataPool(orc_XmlParser.GetNodeContent(), e_InvalidTypePlaceholder);
         //Return
         orc_XmlParser.SelectNodeParent();
      }
      if (orc_XmlParser.SelectNodeChild("invalid-name-placeholder") == "invalid-name-placeholder")
      {
         c_InvalidNamePlaceholder = orc_XmlParser.GetNodeContent().c_str();
         //Return
         orc_XmlParser.SelectNodeParent();
      }
      orc_Id = C_PuiSvDbNodeDataPoolListElementId(c_Base, e_SourceType, false, 0UL, q_IsValid,
                                                  e_InvalidTypePlaceholder,
                                                  c_InvalidNamePlaceholder);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Load values

   \param[in,out] orc_Values    Values
   \param[in,out] orc_XmlParser XML parser with the "current" element set to the "param" element

   \return
   C_NO_ERR    information loaded
   C_CONFIG    error loading information
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_PuiSvHandlerFilerV1::mh_LoadParamDataSetIndices(std::vector<int32_t> & orc_Values,
                                                          C_OscXmlParserBase & orc_XmlParser)
{
   int32_t s32_Retval = C_NO_ERR;

   orc_Values.clear();
   if (orc_XmlParser.SelectNodeChild("data-set-selections") == "data-set-selections")
   {
      C_SclString c_CurrentValueNode = orc_XmlParser.SelectNodeChild("data-set-selection");
      if (c_CurrentValueNode == "data-set-selection")
      {
         do
         {
            if (orc_XmlParser.AttributeExists("value") == true)
            {
               const int32_t s32_Value = orc_XmlParser.GetAttributeSint32("value");
               orc_Values.push_back(s32_Value);
            }
            else
            {
               s32_Retval = C_CONFIG;
            }
            //Next
            c_CurrentValueNode = orc_XmlParser.SelectNodeNext("data-set-selection");
         }
         while ((c_CurrentValueNode == "data-set-selection") && (s32_Retval == C_NO_ERR));
         //Return
         tgl_assert(orc_XmlParser.SelectNodeParent() == "data-set-selections");
      }
      //Return
      tgl_assert(orc_XmlParser.SelectNodeParent() == "param");
   }
   else
   {
      s32_Retval = C_CONFIG;
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Load values

   \param[in,out] orc_Values    Values
   \param[in,out] orc_XmlParser XML parser with the "current" element set to the "param" element

   \return
   C_NO_ERR    information loaded
   C_CONFIG    error loading information
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_PuiSvHandlerFilerV1::mh_LoadParamValues(std::vector<C_OscNodeDataPoolContent> & orc_Values,
                                                  C_OscXmlParserBase & orc_XmlParser)
{
   int32_t s32_Retval = C_NO_ERR;

   orc_Values.clear();
   if (orc_XmlParser.SelectNodeChild("values") == "values")
   {
      C_SclString c_CurrentValueNode = orc_XmlParser.SelectNodeChild("value");
      if (c_CurrentValueNode == "value")
      {
         do
         {
            C_OscNodeDataPoolContent c_Value;
            C_OscNodeDataPoolFilerV2::h_LoadDataPoolContentV1(c_Value, orc_XmlParser);
            orc_Values.push_back(c_Value);
            //Next
            c_CurrentValueNode = orc_XmlParser.SelectNodeNext("value");
         }
         while ((c_CurrentValueNode == "value") && (s32_Retval == C_NO_ERR));
         //Return
         tgl_assert(orc_XmlParser.SelectNodeParent() == "values");
      }
      //Return
      tgl_assert(orc_XmlParser.SelectNodeParent() == "param");
   }
   else
   {
      s32_Retval = C_CONFIG;
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Load values

   \param[in,out] orc_Values    Values
   \param[in,out] orc_XmlParser XML parser with the "current" element set to the "param" element

   \return
   C_NO_ERR    information loaded
   C_CONFIG    error loading information
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_PuiSvHandlerFilerV1::mh_LoadParamTables(std::vector<std::vector<int32_t> > & orc_Values,
                                                  C_OscXmlParserBase & orc_XmlParser)
{
   int32_t s32_Retval = C_NO_ERR;

   orc_Values.clear();
   if (orc_XmlParser.SelectNodeChild("tables") == "tables")
   {
      C_SclString c_CurrentValueNode = orc_XmlParser.SelectNodeChild("table");
      if (c_CurrentValueNode == "table")
      {
         do
         {
            std::vector<int32_t> c_Table;
            s32_Retval = mh_LoadParamColumns(c_Table, orc_XmlParser);
            if (s32_Retval == C_NO_ERR)
            {
               orc_Values.push_back(c_Table);
            }
            //Next
            c_CurrentValueNode = orc_XmlParser.SelectNodeNext("table");
         }
         while ((c_CurrentValueNode == "table") && (s32_Retval == C_NO_ERR));
         //Return
         tgl_assert(orc_XmlParser.SelectNodeParent() == "tables");
      }
      //Return
      tgl_assert(orc_XmlParser.SelectNodeParent() == "param");
   }
   else
   {
      s32_Retval = C_CONFIG;
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Load values

   \param[in,out] orc_Values    Values
   \param[in,out] orc_XmlParser XML parser with the "current" element set to the "table" element

   \return
   C_NO_ERR    information loaded
   C_CONFIG    error loading information
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_PuiSvHandlerFilerV1::mh_LoadParamColumns(std::vector<int32_t> & orc_Values,
                                                   C_OscXmlParserBase & orc_XmlParser)
{
   int32_t s32_Retval = C_NO_ERR;

   orc_Values.clear();
   if (orc_XmlParser.SelectNodeChild("columns") == "columns")
   {
      C_SclString c_CurrentValueNode = orc_XmlParser.SelectNodeChild("column");
      if (c_CurrentValueNode == "column")
      {
         do
         {
            if (orc_XmlParser.AttributeExists("value") == true)
            {
               const int32_t s32_Value = orc_XmlParser.GetAttributeSint32("value");
               orc_Values.push_back(s32_Value);
            }
            else
            {
               s32_Retval = C_CONFIG;
            }
            //Next
            c_CurrentValueNode = orc_XmlParser.SelectNodeNext("column");
         }
         while ((c_CurrentValueNode == "column") && (s32_Retval == C_NO_ERR));
         //Return
         tgl_assert(orc_XmlParser.SelectNodeParent() == "columns");
      }
      //Return
      tgl_assert(orc_XmlParser.SelectNodeParent() == "table");
   }
   else
   {
      s32_Retval = C_CONFIG;
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Load data element id

   \param[in,out] orc_Id        Data element id
   \param[in,out] orc_XmlParser XML parser with the "current" element set to any element
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PuiSvHandlerFilerV1::mh_LoadDataElement(C_OscNodeDataPoolListElementId & orc_Id,
                                               const C_OscXmlParserBase & orc_XmlParser)
{
   orc_Id.u32_NodeIndex = orc_XmlParser.GetAttributeUint32("node");
   orc_Id.u32_DataPoolIndex = orc_XmlParser.GetAttributeUint32("data-pool");
   orc_Id.u32_ListIndex = orc_XmlParser.GetAttributeUint32("list");
   orc_Id.u32_ElementIndex = orc_XmlParser.GetAttributeUint32("element");
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Transform string to write mode

   \param[in]   orc_String    String to interpret
   \param[out]  ore_Mode      Write mode

   \return
   C_NO_ERR   no error
   C_RANGE    String unknown
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_PuiSvHandlerFilerV1::mh_StringToWriteMode(const QString & orc_String,
                                                    C_PuiSvDbWriteWidgetBase::E_WriteMode & ore_Mode)
{
   int32_t s32_Retval = C_NO_ERR;

   if (orc_String.compare("manual") == 0)
   {
      ore_Mode = C_PuiSvDbWriteWidgetBase::eWM_MANUAL;
   }
   else if (orc_String.compare("on-change") == 0)
   {
      ore_Mode = C_PuiSvDbWriteWidgetBase::eWM_ON_CHANGE;
   }
   else
   {
      //Default
      ore_Mode = C_PuiSvDbWriteWidgetBase::eWM_MANUAL;
      s32_Retval = C_RANGE;
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Transform string to display style

   \param[in]  orc_String String to interpret
   \param[out] ore_Style  Display style

   \return
   C_NO_ERR   no error
   C_RANGE    String unknown
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_PuiSvHandlerFilerV1::mh_StringToDisplayStyle(const QString & orc_String,
                                                       C_PuiSvDbWidgetBase::E_Style & ore_Style)
{
   int32_t s32_Retval = C_NO_ERR;

   if (orc_String.compare("opensyde") == 0)
   {
      ore_Style = C_PuiSvDbWidgetBase::eOPENSYDE;
   }
   else if (orc_String.compare("opensyde2") == 0)
   {
      ore_Style = C_PuiSvDbWidgetBase::eOPENSYDE_2;
   }
   else if (orc_String.compare("flat") == 0)
   {
      ore_Style = C_PuiSvDbWidgetBase::eFLAT;
   }
   else if (orc_String.compare("skeuomorph") == 0)
   {
      ore_Style = C_PuiSvDbWidgetBase::eSKEUOMORPH;
   }
   else
   {
      //Default
      ore_Style = C_PuiSvDbWidgetBase::eOPENSYDE;
      s32_Retval = C_RANGE;
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Transform string to toggle type

   \param[in]  orc_String String to interpret
   \param[out] ore_Type   Toggle type

   \return
   C_NO_ERR   no error
   C_RANGE    String unknown
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_PuiSvHandlerFilerV1::mh_StringToToggleType(const QString & orc_String, C_PuiSvDbToggle::E_Type & ore_Type)
{
   int32_t s32_Retval = C_NO_ERR;

   if (orc_String.compare("type1") == 0)
   {
      ore_Type = C_PuiSvDbToggle::eTYPE1;
   }
   else if (orc_String.compare("type2") == 0)
   {
      ore_Type = C_PuiSvDbToggle::eTYPE2;
   }
   else if (orc_String.compare("type3") == 0)
   {
      ore_Type = C_PuiSvDbToggle::eTYPE3;
   }
   else
   {
      //Default
      ore_Type = C_PuiSvDbToggle::eTYPE1;
      s32_Retval = C_RANGE;
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Transform string to spin box type

   \param[in]  orc_String String to interpret
   \param[out] ore_Type   Spin box type

   \return
   C_NO_ERR   no error
   C_RANGE    String unknown
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_PuiSvHandlerFilerV1::mh_StringToSpinBoxType(const QString & orc_String, C_PuiSvDbSpinBox::E_Type & ore_Type)
{
   int32_t s32_Retval = C_NO_ERR;

   if (orc_String.compare("type1") == 0)
   {
      ore_Type = C_PuiSvDbSpinBox::eTYPE1;
   }
   else if (orc_String.compare("type2") == 0)
   {
      ore_Type = C_PuiSvDbSpinBox::eTYPE2;
   }
   else
   {
      //Default
      ore_Type = C_PuiSvDbSpinBox::eTYPE1;
      s32_Retval = C_RANGE;
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Transform string to label type

   \param[in]  orc_String String to interpret
   \param[out] ore_Type   Label type

   \return
   C_NO_ERR   no error
   C_RANGE    String unknown
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_PuiSvHandlerFilerV1::mh_StringToLabelType(const QString & orc_String, C_PuiSvDbLabel::E_Type & ore_Type)
{
   int32_t s32_Retval = C_NO_ERR;

   if (orc_String.compare("default") == 0)
   {
      ore_Type = C_PuiSvDbLabel::eDEFAULT;
   }
   else if (orc_String.compare("transparent") == 0)
   {
      ore_Type = C_PuiSvDbLabel::eTRANSPARENT;
   }
   else
   {
      //Default
      ore_Type = C_PuiSvDbLabel::eDEFAULT;
      s32_Retval = C_RANGE;
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Transform string to slider type

   \param[in]  orc_String String to interpret
   \param[out] ore_Type   Slider type

   \return
   C_NO_ERR   no error
   C_RANGE    String unknown
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_PuiSvHandlerFilerV1::mh_StringToSliderType(const QString & orc_String, C_PuiSvDbSlider::E_Type & ore_Type)
{
   int32_t s32_Retval = C_NO_ERR;

   if (orc_String.compare("small-color-1") == 0)
   {
      ore_Type = C_PuiSvDbSlider::eTYPE_SMALL_COLOR_1;
   }
   else if (orc_String.compare("big-color-1") == 0)
   {
      ore_Type = C_PuiSvDbSlider::eTYPE_BIG_COLOR_1;
   }
   else if (orc_String.compare("small-color-2") == 0)
   {
      ore_Type = C_PuiSvDbSlider::eTYPE_SMALL_COLOR_2;
   }
   else if (orc_String.compare("big-color-2") == 0)
   {
      ore_Type = C_PuiSvDbSlider::eTYPE_BIG_COLOR_2;
   }
   else
   {
      //Default
      ore_Type = C_PuiSvDbSlider::eTYPE_SMALL_COLOR_1;
      s32_Retval = C_RANGE;
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Transform string to progress bar type

   \param[in]  orc_String String to interpret
   \param[out] ore_Type   Progress bar type

   \return
   C_NO_ERR   no error
   C_RANGE    String unknown
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_PuiSvHandlerFilerV1::mh_StringToProgressBarType(const QString & orc_String,
                                                          C_PuiSvDbProgressBar::E_Type & ore_Type)
{
   int32_t s32_Retval = C_NO_ERR;

   if (orc_String.compare("type1") == 0)
   {
      ore_Type = C_PuiSvDbProgressBar::eTYPE_1;
   }
   else if (orc_String.compare("type2") == 0)
   {
      ore_Type = C_PuiSvDbProgressBar::eTYPE_2;
   }
   else if (orc_String.compare("type3") == 0)
   {
      ore_Type = C_PuiSvDbProgressBar::eTYPE_3;
   }
   else
   {
      //Default
      ore_Type = C_PuiSvDbProgressBar::eTYPE_1;
      s32_Retval = C_RANGE;
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Transform string to progress bar alignment type

   \param[in]  orc_String    String to interpret
   \param[out] ore_Alignment Progress bar alignment type

   \return
   C_NO_ERR   no error
   C_RANGE    String unknown
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_PuiSvHandlerFilerV1::mh_StringToProgressBarAlignmentType(const QString & orc_String,
                                                                   C_PuiSvDbProgressBar::E_Alignment & ore_Alignment)
{
   int32_t s32_Retval = C_NO_ERR;

   if (orc_String.compare("top") == 0)
   {
      ore_Alignment = C_PuiSvDbProgressBar::eTOP;
   }
   else if (orc_String.compare("left") == 0)
   {
      ore_Alignment = C_PuiSvDbProgressBar::eLEFT;
   }
   else if (orc_String.compare("right") == 0)
   {
      ore_Alignment = C_PuiSvDbProgressBar::eRIGHT;
   }
   else if (orc_String.compare("bottom") == 0)
   {
      ore_Alignment = C_PuiSvDbProgressBar::eBOTTOM;
   }
   else
   {
      //Default
      ore_Alignment = C_PuiSvDbProgressBar::eTOP;
      s32_Retval = C_RANGE;
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Transform string to transmission mode

   \param[in]  orc_String String to interpret
   \param[out] ore_Type   Transmission mode

   \return
   C_NO_ERR   no error
   C_RANGE    String unknown
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_PuiSvHandlerFilerV1::mh_StringToTransmissionMode(const QString & orc_String,
                                                           C_PuiSvReadDataConfiguration::E_TransmissionMode & ore_Mode)
{
   int32_t s32_Retval = C_NO_ERR;

   if (orc_String.compare("manual") == 0)
   {
      ore_Mode = C_PuiSvReadDataConfiguration::eTM_ON_TRIGGER;
   }
   else if (orc_String.compare("cyclic") == 0)
   {
      ore_Mode = C_PuiSvReadDataConfiguration::eTM_CYCLIC;
   }
   else if (orc_String.compare("cyclic-on-change") == 0)
   {
      ore_Mode = C_PuiSvReadDataConfiguration::eTM_ON_CHANGE;
   }
   else
   {
      //Default
      ore_Mode = C_PuiSvReadDataConfiguration::eTM_ON_TRIGGER;
      s32_Retval = C_RANGE;
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Transform string to source type

   \param[in]  orc_String String to interpret
   \param[out] ore_Type   Source type

   \return
   C_NO_ERR   no error
   C_RANGE    String unknown
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_PuiSvHandlerFilerV1::mh_StringToSourceType(const QString & orc_String,
                                                     C_PuiSvDbNodeDataPoolListElementId::E_Type & ore_Type)
{
   int32_t s32_Retval = C_NO_ERR;

   if (orc_String.compare("datapool-element") == 0)
   {
      ore_Type = C_PuiSvDbNodeDataPoolListElementId::eDATAPOOL_ELEMENT;
   }
   else if (orc_String.compare("bus-signal") == 0)
   {
      ore_Type = C_PuiSvDbNodeDataPoolListElementId::eBUS_SIGNAL;
   }
   else
   {
      //Default
      ore_Type = C_PuiSvDbNodeDataPoolListElementId::eDATAPOOL_ELEMENT;
      s32_Retval = C_RANGE;
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Save node active flags

   \param[in]     orc_NodeActiveFlags Node active flags
   \param[in,out] orc_XmlParser       XML parser with the "current" element set to the "opensyde-system-view" element
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PuiSvHandlerFilerV1::mh_SaveNodeActiveFlags(const std::vector<uint8_t> & orc_NodeActiveFlags,
                                                   C_OscXmlParserBase & orc_XmlParser)
{
   orc_XmlParser.CreateAndSelectNodeChild("active-nodes");
   for (uint32_t u32_ItNodeActiveFlag = 0; u32_ItNodeActiveFlag < orc_NodeActiveFlags.size(); ++u32_ItNodeActiveFlag)
   {
      orc_XmlParser.CreateAndSelectNodeChild("active-node");
      orc_XmlParser.SetAttributeBool("state", static_cast<bool>(orc_NodeActiveFlags[u32_ItNodeActiveFlag]));
      //Return
      tgl_assert(orc_XmlParser.SelectNodeParent() == "active-nodes");
   }
   //Return
   tgl_assert(orc_XmlParser.SelectNodeParent() == "opensyde-system-view");
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Save node update information

   \param[in]     orc_NodeUpdateInformation Node update information
   \param[in,out] orc_XmlParser             XML parser with the "current" element set
                                            to the "opensyde-system-view" element
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PuiSvHandlerFilerV1::mh_SaveNodeUpdateInformation(
   const std::vector<C_OscViewNodeUpdate> & orc_NodeUpdateInformation, C_OscXmlParserBase & orc_XmlParser)
{
   orc_XmlParser.CreateAndSelectNodeChild("node-update-information");
   for (uint32_t u32_ItNodeActiveFlag = 0; u32_ItNodeActiveFlag < orc_NodeUpdateInformation.size();
        ++u32_ItNodeActiveFlag)
   {
      const C_OscViewNodeUpdate & rc_NodeUpdateInformation = orc_NodeUpdateInformation[u32_ItNodeActiveFlag];
      const std::vector<C_SclString> & rc_ApplicationPaths = rc_NodeUpdateInformation.GetPaths(
         C_OscViewNodeUpdate::eFTP_DATA_BLOCK);
      orc_XmlParser.CreateAndSelectNodeChild("node-specific-update-information");
      orc_XmlParser.SetAttributeUint32("position", rc_NodeUpdateInformation.u32_NodeUpdatePosition);
      orc_XmlParser.CreateAndSelectNodeChild("paths");
      for (uint32_t u32_ItApplicationPath = 0; u32_ItApplicationPath < rc_ApplicationPaths.size();
           ++u32_ItApplicationPath)
      {
         orc_XmlParser.CreateNodeChild("path", rc_ApplicationPaths[u32_ItApplicationPath]);
      }
      //Return
      tgl_assert(orc_XmlParser.SelectNodeParent() == "node-specific-update-information");
      //Return
      tgl_assert(orc_XmlParser.SelectNodeParent() == "node-update-information");
   }
   //Return
   tgl_assert(orc_XmlParser.SelectNodeParent() == "opensyde-system-view");
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Save dashboard elements

   \param[in]     orc_Dashboards Dashboard elements
   \param[in,out] orc_XmlParser  XML parser with the "current" element set to the "opensyde-system-view" element
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PuiSvHandlerFilerV1::mh_SaveDashboards(const std::vector<C_PuiSvDashboard> & orc_Dashboards,
                                              C_OscXmlParserBase & orc_XmlParser)
{
   orc_XmlParser.CreateAndSelectNodeChild("dashboards");
   for (uint32_t u32_ItDashboard = 0; u32_ItDashboard < orc_Dashboards.size(); ++u32_ItDashboard)
   {
      orc_XmlParser.CreateAndSelectNodeChild("dashboard");
      h_SaveDashboard(orc_Dashboards[u32_ItDashboard], orc_XmlParser);
      //Return
      tgl_assert(orc_XmlParser.SelectNodeParent() == "dashboards");
   }
   //Return
   tgl_assert(orc_XmlParser.SelectNodeParent() == "opensyde-system-view");
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Save view element

   \param[in]     orc_View      View element
   \param[in,out] orc_XmlParser XML parser with the "current" element set to the "opensyde-system-view" element
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PuiSvHandlerFilerV1::mh_SaveView(const C_PuiSvData & orc_View, C_OscXmlParserBase & orc_XmlParser)
{
   orc_XmlParser.SetAttributeBool("darkmode", orc_View.GetDarkModeActive());
   orc_XmlParser.SetAttributeUint32("device-config-selected-bit-rate", orc_View.GetDeviceConfigSelectedBitRate());
   orc_XmlParser.CreateAndSelectNodeChild("name");
   orc_XmlParser.SetNodeContent(orc_View.GetName());
   //Return
   tgl_assert(orc_XmlParser.SelectNodeParent() == "opensyde-system-view");
   orc_XmlParser.CreateNodeChild("device-config-mode", mh_DeviceConfigModeToString(
                                    orc_View.GetDeviceConfigMode()).toStdString().c_str());
   orc_XmlParser.CreateAndSelectNodeChild("update-rates");
   orc_XmlParser.SetAttributeUint32("fast", static_cast<uint32_t>(orc_View.GetUpdateRateFast()));
   orc_XmlParser.SetAttributeUint32("medium", static_cast<uint32_t>(orc_View.GetUpdateRateMedium()));
   orc_XmlParser.SetAttributeUint32("slow", static_cast<uint32_t>(orc_View.GetUpdateRateSlow()));
   //Return
   tgl_assert(orc_XmlParser.SelectNodeParent() == "opensyde-system-view");
   mh_SaveNodeActiveFlags(orc_View.GetNodeActiveFlags(), orc_XmlParser);
   mh_SaveNodeUpdateInformation(orc_View.GetAllNodeUpdateInformation(), orc_XmlParser);
   orc_XmlParser.CreateAndSelectNodeChild("pc");
   mh_SavePc(orc_View.GetOscPcData(), orc_View.GetPuiPcData(), orc_XmlParser);
   //Return
   tgl_assert(orc_XmlParser.SelectNodeParent() == "opensyde-system-view");
   mh_SaveDashboards(orc_View.GetDashboards(), orc_XmlParser);

   //Rails
   orc_XmlParser.CreateAndSelectNodeChild("rail-assignments");
   h_SaveReadRails(orc_View.GetReadRailAssignments(), orc_XmlParser);
   //Return
   tgl_assert(orc_XmlParser.SelectNodeParent() == "opensyde-system-view");
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Save PC element

   \param[in]     orc_Pc        PC element
   \param[in,out] orc_XmlParser XML parser with the "current" element set to the "pc" element
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PuiSvHandlerFilerV1::mh_SavePc(const C_OscViewPc & orc_OscPc, const C_PuiSvPc & orc_PuiPc,
                                      C_OscXmlParserBase & orc_XmlParser)
{
   orc_XmlParser.SetAttributeBool("connected", orc_OscPc.GetConnected());
   orc_XmlParser.SetAttributeUint32("bus-index", orc_OscPc.GetBusIndex());

   orc_XmlParser.CreateAndSelectNodeChild("dll-path");
   orc_XmlParser.SetAttributeSint32("type", static_cast<int32_t>(orc_PuiPc.GetCanDllType()));
   orc_XmlParser.SetNodeContent(orc_PuiPc.GetCustomCanDllPath().toStdString().c_str());
   //Return
   tgl_assert(orc_XmlParser.SelectNodeParent() == "pc");
   orc_XmlParser.CreateAndSelectNodeChild("box");
   C_PuiBsElementsFiler::h_SaveBoxBase(orc_PuiPc, orc_XmlParser);
   //Return
   tgl_assert(orc_XmlParser.SelectNodeParent() == "pc");
   orc_XmlParser.CreateAndSelectNodeChild("connection");
   orc_XmlParser.CreateAndSelectNodeChild("line");
   C_PuiBsElementsFiler::h_SaveLineBase(orc_PuiPc.GetConnectionData(), orc_XmlParser);
   //Return
   tgl_assert(orc_XmlParser.SelectNodeParent() == "connection");
   //Return
   tgl_assert(orc_XmlParser.SelectNodeParent() == "pc");
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Save widget elements

   \param[in]     orc_Widgets   Widget elements
   \param[in,out] orc_XmlParser XML parser with the "current" element set to the "dashboard" element
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PuiSvHandlerFilerV1::mh_SaveCharts(const std::vector<C_PuiSvDbChart> & orc_Widgets,
                                          C_OscXmlParserBase & orc_XmlParser)
{
   orc_XmlParser.CreateAndSelectNodeChild("charts");
   for (uint32_t u32_ItWidget = 0; u32_ItWidget < orc_Widgets.size(); ++u32_ItWidget)
   {
      const C_PuiSvDbChart & rc_Chart = orc_Widgets[u32_ItWidget];
      orc_XmlParser.CreateAndSelectNodeChild("chart");
      C_PuiSvHandlerFilerV1::mh_SaveWidgetBase(rc_Chart, orc_XmlParser);
      orc_XmlParser.CreateAndSelectNodeChild("active-flags");
      for (uint32_t u32_ItActive = 0; u32_ItActive < rc_Chart.c_DataPoolElementsActive.size(); ++u32_ItActive)
      {
         orc_XmlParser.CreateAndSelectNodeChild("active-flag");
         orc_XmlParser.SetAttributeBool("state", rc_Chart.c_DataPoolElementsActive[u32_ItActive]);
         //Return
         tgl_assert(orc_XmlParser.SelectNodeParent() == "active-flags");
      }
      //Return
      tgl_assert(orc_XmlParser.SelectNodeParent() == "chart");
      //Return
      tgl_assert(orc_XmlParser.SelectNodeParent() == "charts");
   }
   //Return
   orc_XmlParser.SelectNodeParent();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Save widget elements

   \param[in]     orc_Widgets   Widget elements
   \param[in,out] orc_XmlParser XML parser with the "current" element set to the "dashboard" element
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PuiSvHandlerFilerV1::mh_SaveLabels(const std::vector<C_PuiSvDbLabel> & orc_Widgets,
                                          C_OscXmlParserBase & orc_XmlParser)
{
   orc_XmlParser.CreateAndSelectNodeChild("labels");
   for (uint32_t u32_ItWidget = 0; u32_ItWidget < orc_Widgets.size(); ++u32_ItWidget)
   {
      const C_PuiSvDbLabel & rc_Label = orc_Widgets[u32_ItWidget];
      orc_XmlParser.CreateAndSelectNodeChild("label");
      orc_XmlParser.SetAttributeBool("show-caption", rc_Label.q_ShowCaption);
      orc_XmlParser.SetAttributeBool("show-unit", rc_Label.q_ShowUnit);
      C_PuiSvHandlerFilerV1::mh_SaveWidgetBase(rc_Label, orc_XmlParser);
      orc_XmlParser.CreateNodeChild("caption", rc_Label.c_Caption.toStdString().c_str());
      orc_XmlParser.CreateNodeChild("type", mh_LabelTypeToString(rc_Label.e_Type).toStdString().c_str());
      //Return
      tgl_assert(orc_XmlParser.SelectNodeParent() == "labels");
   }
   //Return
   orc_XmlParser.SelectNodeParent();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Save widget elements

   \param[in]     orc_Widgets   Widget elements
   \param[in,out] orc_XmlParser XML parser with the "current" element set to the "dashboard" element
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PuiSvHandlerFilerV1::mh_SaveParams(const std::vector<C_PuiSvDbParam> & orc_Widgets,
                                          C_OscXmlParserBase & orc_XmlParser)
{
   orc_XmlParser.CreateAndSelectNodeChild("params");
   for (uint32_t u32_ItWidget = 0; u32_ItWidget < orc_Widgets.size(); ++u32_ItWidget)
   {
      const C_PuiSvDbParam & rc_Param = orc_Widgets[u32_ItWidget];
      orc_XmlParser.CreateAndSelectNodeChild("param");
      C_PuiSvHandlerFilerV1::mh_SaveWidgetBase(rc_Param, orc_XmlParser);
      //Data set indices
      orc_XmlParser.CreateAndSelectNodeChild("data-set-selections");
      for (uint32_t u32_ItDataSet = 0; u32_ItDataSet < rc_Param.c_DataSetSelectionIndices.size(); ++u32_ItDataSet)
      {
         orc_XmlParser.CreateAndSelectNodeChild("data-set-selection");
         orc_XmlParser.SetAttributeSint32("value", rc_Param.c_DataSetSelectionIndices[u32_ItDataSet]);
         //Return
         tgl_assert(orc_XmlParser.SelectNodeParent() == "data-set-selections");
      }
      //Return
      tgl_assert(orc_XmlParser.SelectNodeParent() == "param");
      //Values
      orc_XmlParser.CreateAndSelectNodeChild("values");
      for (uint32_t u32_ItValue = 0; u32_ItValue < rc_Param.c_ListValues.size(); ++u32_ItValue)
      {
         orc_XmlParser.CreateAndSelectNodeChild("value");
         C_OscNodeDataPoolFilerV2::h_SaveDataPoolContentV1(rc_Param.c_ListValues[u32_ItValue], orc_XmlParser);
         //Return
         tgl_assert(orc_XmlParser.SelectNodeParent() == "values");
      }
      //Return
      tgl_assert(orc_XmlParser.SelectNodeParent() == "param");
      //Columns
      orc_XmlParser.CreateAndSelectNodeChild("tables");
      for (uint32_t u32_ItTable = 0; u32_ItTable < rc_Param.c_ColWidth.size(); ++u32_ItTable)
      {
         const std::vector<int32_t> & rc_Cols = rc_Param.c_ColWidth[u32_ItTable];
         orc_XmlParser.CreateAndSelectNodeChild("table");
         orc_XmlParser.CreateAndSelectNodeChild("columns");
         for (uint32_t u32_ItColumn = 0; u32_ItColumn < rc_Cols.size(); ++u32_ItColumn)
         {
            orc_XmlParser.CreateAndSelectNodeChild("column");
            orc_XmlParser.SetAttributeSint32("value", rc_Cols[u32_ItColumn]);
            //Return
            tgl_assert(orc_XmlParser.SelectNodeParent() == "columns");
         }
         //Return
         tgl_assert(orc_XmlParser.SelectNodeParent() == "table");
         //Return
         tgl_assert(orc_XmlParser.SelectNodeParent() == "tables");
      }
      //Return
      tgl_assert(orc_XmlParser.SelectNodeParent() == "param");
      mh_SaveParamExpandedItems(rc_Param.c_ExpandedItems, orc_XmlParser);
      mh_SaveParamColumnPositionIndices(rc_Param.c_ColPosIndices, orc_XmlParser);
      //Return
      tgl_assert(orc_XmlParser.SelectNodeParent() == "params");
   }
   //Return
   orc_XmlParser.SelectNodeParent();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Save tree elements

   \param[in]     orc_Items     Tree elements
   \param[in,out] orc_XmlParser XML parser with the "current" element set to the "param" element
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PuiSvHandlerFilerV1::mh_SaveParamExpandedItems(const std::vector<C_PuiSvDbExpandedTreeIndex> & orc_Items,
                                                      C_OscXmlParserBase & orc_XmlParser)
{
   //Columns
   orc_XmlParser.CreateAndSelectNodeChild("expanded-tree-items");
   for (uint32_t u32_ItTable = 0; u32_ItTable < orc_Items.size(); ++u32_ItTable)
   {
      const C_PuiSvDbExpandedTreeIndex & rc_TreeElement = orc_Items[u32_ItTable];
      orc_XmlParser.CreateAndSelectNodeChild("expanded-tree-item");
      orc_XmlParser.SetAttributeUint32("valid-layers", rc_TreeElement.u32_Layer);
      mh_SaveUiIndex(rc_TreeElement.c_ExpandedId, orc_XmlParser);
      //Return
      tgl_assert(orc_XmlParser.SelectNodeParent() == "expanded-tree-items");
   }
   //Return
   tgl_assert(orc_XmlParser.SelectNodeParent() == "param");
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Save column position indices elements

   \param[in]     orc_Items     Column position indices elements
   \param[in,out] orc_XmlParser XML parser with the "current" element set to the "param" element
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PuiSvHandlerFilerV1::mh_SaveParamColumnPositionIndices(const std::vector<int32_t> & orc_Items,
                                                              C_OscXmlParserBase & orc_XmlParser)
{
   orc_XmlParser.CreateAndSelectNodeChild("column-position-indices");
   for (uint32_t u32_ItTable = 0; u32_ItTable < orc_Items.size(); ++u32_ItTable)
   {
      orc_XmlParser.CreateAndSelectNodeChild("column-position-index");
      orc_XmlParser.SetAttributeSint32("value", orc_Items[u32_ItTable]);
      //Return
      tgl_assert(orc_XmlParser.SelectNodeParent() == "column-position-indices");
   }
   //Return
   tgl_assert(orc_XmlParser.SelectNodeParent() == "param");
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Save widget elements

   \param[in]     orc_Widgets   Widget elements
   \param[in,out] orc_XmlParser XML parser with the "current" element set to the "dashboard" element
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PuiSvHandlerFilerV1::mh_SavePieCharts(const std::vector<C_PuiSvDbPieChart> & orc_Widgets,
                                             C_OscXmlParserBase & orc_XmlParser)
{
   orc_XmlParser.CreateAndSelectNodeChild("pie-charts");
   for (uint32_t u32_ItWidget = 0; u32_ItWidget < orc_Widgets.size(); ++u32_ItWidget)
   {
      const C_PuiSvDbPieChart & rc_PieChart = orc_Widgets[u32_ItWidget];
      orc_XmlParser.CreateAndSelectNodeChild("pie-chart");
      orc_XmlParser.SetAttributeBool("show-value", rc_PieChart.q_ShowValue);
      orc_XmlParser.SetAttributeBool("show-unit", rc_PieChart.q_ShowUnit);

      C_PuiSvHandlerFilerV1::mh_SaveWidgetBase(rc_PieChart, orc_XmlParser);
      //Return
      tgl_assert(orc_XmlParser.SelectNodeParent() == "pie-charts");
   }
   //Return
   orc_XmlParser.SelectNodeParent();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Save widget elements

   \param[in]     orc_Widgets   Widget elements
   \param[in,out] orc_XmlParser XML parser with the "current" element set to the "dashboard" element
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PuiSvHandlerFilerV1::mh_SaveSpinBoxes(const std::vector<C_PuiSvDbSpinBox> & orc_Widgets,
                                             C_OscXmlParserBase & orc_XmlParser)
{
   orc_XmlParser.CreateAndSelectNodeChild("spin-boxes");
   for (uint32_t u32_ItWidget = 0; u32_ItWidget < orc_Widgets.size(); ++u32_ItWidget)
   {
      const C_PuiSvDbSpinBox & rc_SpinBox = orc_Widgets[u32_ItWidget];
      orc_XmlParser.CreateAndSelectNodeChild("spin-box");

      orc_XmlParser.SetAttributeBool("show-unit", rc_SpinBox.q_ShowUnit);

      C_PuiSvHandlerFilerV1::mh_SaveWidgetBase(rc_SpinBox, orc_XmlParser);

      orc_XmlParser.CreateNodeChild("type", C_PuiSvHandlerFilerV1::mh_SpinBoxTypeToString(
                                       rc_SpinBox.e_Type).toStdString().c_str());
      orc_XmlParser.CreateAndSelectNodeChild("content");
      C_OscNodeDataPoolFilerV2::h_SaveDataPoolContentV1(rc_SpinBox.c_Value, orc_XmlParser);
      //Return
      tgl_assert(orc_XmlParser.SelectNodeParent() == "spin-box");
      //Return
      tgl_assert(orc_XmlParser.SelectNodeParent() == "spin-boxes");
   }
   //Return
   orc_XmlParser.SelectNodeParent();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Save widget elements

   \param[in]     orc_Widgets   Widget elements
   \param[in,out] orc_XmlParser XML parser with the "current" element set to the "dashboard" element
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PuiSvHandlerFilerV1::mh_SaveTables(const std::vector<C_PuiSvDbTable> & orc_Widgets,
                                          C_OscXmlParserBase & orc_XmlParser)
{
   orc_XmlParser.CreateAndSelectNodeChild("tables");
   for (uint32_t u32_ItWidget = 0; u32_ItWidget < orc_Widgets.size(); ++u32_ItWidget)
   {
      const C_PuiSvDbTable & rc_Table = orc_Widgets[u32_ItWidget];
      orc_XmlParser.CreateAndSelectNodeChild("table");

      C_PuiSvHandlerFilerV1::mh_SaveWidgetBase(rc_Table, orc_XmlParser);

      //Columns
      orc_XmlParser.CreateAndSelectNodeChild("columns");
      for (uint32_t u32_ItColumn = 0; u32_ItColumn < rc_Table.c_ColumnWidth.size(); ++u32_ItColumn)
      {
         orc_XmlParser.CreateAndSelectNodeChild("column");
         orc_XmlParser.SetAttributeSint32("width", rc_Table.c_ColumnWidth[u32_ItColumn]);
         //Return
         tgl_assert(orc_XmlParser.SelectNodeParent() == "columns");
      }
      //Return
      tgl_assert(orc_XmlParser.SelectNodeParent() == "table");

      //Return
      tgl_assert(orc_XmlParser.SelectNodeParent() == "tables");
   }
   //Return
   orc_XmlParser.SelectNodeParent();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Save widget elements

   \param[in]     orc_Widgets   Widget elements
   \param[in,out] orc_XmlParser XML parser with the "current" element set to the "dashboard" element
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PuiSvHandlerFilerV1::mh_SaveSliders(const std::vector<C_PuiSvDbSlider> & orc_Widgets,
                                           C_OscXmlParserBase & orc_XmlParser)
{
   orc_XmlParser.CreateAndSelectNodeChild("sliders");
   for (uint32_t u32_ItWidget = 0; u32_ItWidget < orc_Widgets.size(); ++u32_ItWidget)
   {
      const C_PuiSvDbSlider & rc_Slider = orc_Widgets[u32_ItWidget];
      orc_XmlParser.CreateAndSelectNodeChild("slider");
      C_PuiSvHandlerFilerV1::mh_SaveWidgetBase(rc_Slider, orc_XmlParser);
      orc_XmlParser.CreateNodeChild("type", C_PuiSvHandlerFilerV1::mh_SliderTypeToString(
                                       rc_Slider.e_Type).toStdString().c_str());
      orc_XmlParser.SetAttributeBool("show-min-max", rc_Slider.q_ShowMinMax);
      orc_XmlParser.SetAttributeSint32("value", rc_Slider.s32_Value);
      //Return
      tgl_assert(orc_XmlParser.SelectNodeParent() == "sliders");
   }
   //Return
   orc_XmlParser.SelectNodeParent();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Save widget elements

   \param[in]     orc_Widgets   Widget elements
   \param[in,out] orc_XmlParser XML parser with the "current" element set to the "dashboard" element
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PuiSvHandlerFilerV1::mh_SaveProgressBars(const std::vector<C_PuiSvDbProgressBar> & orc_Widgets,
                                                C_OscXmlParserBase & orc_XmlParser)
{
   orc_XmlParser.CreateAndSelectNodeChild("progress-bars");
   for (uint32_t u32_ItWidget = 0; u32_ItWidget < orc_Widgets.size(); ++u32_ItWidget)
   {
      const C_PuiSvDbProgressBar & rc_ProgressBar = orc_Widgets[u32_ItWidget];
      orc_XmlParser.CreateAndSelectNodeChild("progress-bar");
      C_PuiSvHandlerFilerV1::mh_SaveWidgetBase(rc_ProgressBar, orc_XmlParser);
      orc_XmlParser.CreateNodeChild("type", C_PuiSvHandlerFilerV1::mh_ProgressBarTypeToString(
                                       rc_ProgressBar.e_Type).toStdString().c_str());
      orc_XmlParser.CreateNodeChild("alignment",
                                    C_PuiSvHandlerFilerV1::mh_ProgressBarAlignmentTypeToString(
                                       rc_ProgressBar.e_Alignment).toStdString().c_str());
      orc_XmlParser.SetAttributeBool("show-min-max", rc_ProgressBar.q_ShowMinMax);
      orc_XmlParser.SetAttributeBool("show-unit", rc_ProgressBar.q_ShowUnit);
      //Return
      tgl_assert(orc_XmlParser.SelectNodeParent() == "progress-bars");
   }
   //Return
   orc_XmlParser.SelectNodeParent();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Save widget elements

   \param[in]     orc_Widgets   Widget elements
   \param[in,out] orc_XmlParser XML parser with the "current" element set to the "dashboard" element
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PuiSvHandlerFilerV1::mh_SaveToggles(const std::vector<C_PuiSvDbToggle> & orc_Widgets,
                                           C_OscXmlParserBase & orc_XmlParser)
{
   orc_XmlParser.CreateAndSelectNodeChild("toggles");
   for (uint32_t u32_ItWidget = 0; u32_ItWidget < orc_Widgets.size(); ++u32_ItWidget)
   {
      const C_PuiSvDbToggle & rc_Toggle = orc_Widgets[u32_ItWidget];
      orc_XmlParser.CreateAndSelectNodeChild("toggle");

      C_PuiSvHandlerFilerV1::mh_SaveWidgetBase(rc_Toggle, orc_XmlParser);

      orc_XmlParser.CreateNodeChild("type", C_PuiSvHandlerFilerV1::mh_ToggleTypeToString(
                                       rc_Toggle.e_Type).toStdString().c_str());

      //Content
      orc_XmlParser.SetAttributeBool("state", rc_Toggle.q_State);

      //Return
      tgl_assert(orc_XmlParser.SelectNodeParent() == "toggles");
   }
   //Return
   orc_XmlParser.SelectNodeParent();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Save widget element

   \param[in]      orc_Widget       Widget element
   \param[in,out]  orc_XmlParser    XML parser with the "current" element set to the any widget element
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PuiSvHandlerFilerV1::mh_SaveWidgetBase(const C_PuiSvDbWidgetBase & orc_Widget,
                                              C_OscXmlParserBase & orc_XmlParser)
{
   orc_XmlParser.CreateAndSelectNodeChild("base");
   orc_XmlParser.CreateAndSelectNodeChild("box");
   C_PuiBsElementsFiler::h_SaveBoxBase(orc_Widget, orc_XmlParser);
   //Return
   tgl_assert(orc_XmlParser.SelectNodeParent() == "base");
   orc_XmlParser.CreateAndSelectNodeChild("data-pool-elements");

   for (uint32_t u32_ItDataElement = 0;
        u32_ItDataElement < orc_Widget.c_DataPoolElementsConfig.size();
        ++u32_ItDataElement)
   {
      const C_PuiSvDbNodeDataElementConfig & rc_Config = orc_Widget.c_DataPoolElementsConfig[u32_ItDataElement];
      const C_PuiSvDbDataElementScaling & rc_Scaling = rc_Config.c_ElementScaling;
      orc_XmlParser.CreateAndSelectNodeChild("data-pool-element");
      mh_SaveUiIndex(rc_Config.c_ElementId, orc_XmlParser);
      //Scaling
      orc_XmlParser.CreateAndSelectNodeChild("scaling");
      orc_XmlParser.SetAttributeBool("use-default", rc_Scaling.q_UseDefault);
      orc_XmlParser.SetAttributeFloat64("factor", rc_Scaling.f64_Factor);
      orc_XmlParser.SetAttributeFloat64("offset", rc_Scaling.f64_Offset);
      orc_XmlParser.CreateNodeChild("unit", rc_Scaling.c_Unit.toStdString().c_str());
      //Return
      tgl_assert(orc_XmlParser.SelectNodeParent() == "data-pool-element");
      orc_XmlParser.CreateNodeChild("display-name", rc_Config.c_DisplayName.toStdString().c_str());
      //Return
      tgl_assert(orc_XmlParser.SelectNodeParent() == "data-pool-elements");
   }

   //Return
   tgl_assert(orc_XmlParser.SelectNodeParent() == "base");

   orc_XmlParser.CreateNodeChild("display-style", C_PuiSvHandlerFilerV1::mh_DisplayStyleToString(
                                    orc_Widget.e_DisplayStyle).toStdString().c_str());

   {
      const C_PuiSvDbWriteWidgetBase * const pc_WriteWidget =
         dynamic_cast<const C_PuiSvDbWriteWidgetBase * const>(&orc_Widget);
      if (pc_WriteWidget != NULL)
      {
         C_PuiSvHandlerFilerV1::mh_SaveWriteWidgetBase(*pc_WriteWidget, orc_XmlParser);
      }
   }
   //Return
   orc_XmlParser.SelectNodeParent();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Save write widget base

   \param[in]      orc_WriteWidget  Write widget
   \param[in,out]  orc_XmlParser    XML parser with the "current" element set to the base element
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PuiSvHandlerFilerV1::mh_SaveWriteWidgetBase(const C_PuiSvDbWriteWidgetBase & orc_WriteWidget,
                                                   C_OscXmlParserBase & orc_XmlParser)
{
   orc_XmlParser.CreateNodeChild("write-mode", C_PuiSvHandlerFilerV1::mh_WriteModeToString(
                                    orc_WriteWidget.e_ElementWriteMode).toStdString().c_str());
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Save UI index to CURRENT node

   \param[in]      orc_Id           ID to store
   \param[in,out]  orc_XmlParser    XML parser
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PuiSvHandlerFilerV1::mh_SaveUiIndex(const C_PuiSvDbNodeDataPoolListElementId & orc_Id,
                                           C_OscXmlParserBase & orc_XmlParser)
{
   orc_XmlParser.SetAttributeBool("is-valid", orc_Id.GetIsValid());
   orc_XmlParser.CreateAndSelectNodeChild("index");
   orc_XmlParser.SetAttributeUint32("node", orc_Id.u32_NodeIndex);
   orc_XmlParser.SetAttributeUint32("data-pool", orc_Id.u32_DataPoolIndex);
   orc_XmlParser.SetAttributeUint32("list", orc_Id.u32_ListIndex);
   orc_XmlParser.SetAttributeUint32("element", orc_Id.u32_ElementIndex);
   //Return
   orc_XmlParser.SelectNodeParent();
   orc_XmlParser.CreateNodeChild("source-type",
                                 C_PuiSvHandlerFilerV1::mh_SourceTypeToString(orc_Id.GetType()).toStdString().c_str());
   orc_XmlParser.CreateNodeChild("invalid-type-placeholder",
                                 C_OscNodeDataPoolFilerV2::h_DataPoolToString(orc_Id.GetInvalidTypePlaceholder()));
   orc_XmlParser.CreateNodeChild("invalid-name-placeholder", orc_Id.GetInvalidNamePlaceholder().toStdString().c_str());
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Save ID

   \param[in]     orc_Id        ID
   \param[in,out] orc_XmlParser XML parser with the "current" element set to the any element
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PuiSvHandlerFilerV1::mh_SaveDataElement(const C_OscNodeDataPoolListElementId & orc_Id,
                                               C_OscXmlParserBase & orc_XmlParser)
{
   orc_XmlParser.SetAttributeUint32("node", orc_Id.u32_NodeIndex);
   orc_XmlParser.SetAttributeUint32("data-pool", orc_Id.u32_DataPoolIndex);
   orc_XmlParser.SetAttributeUint32("list", orc_Id.u32_ListIndex);
   orc_XmlParser.SetAttributeUint32("element", orc_Id.u32_ElementIndex);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Transform device config mode to string

   \param[in] oe_Mode Device config mode

   \return
   Stringified device config mode
*/
//----------------------------------------------------------------------------------------------------------------------
QString C_PuiSvHandlerFilerV1::mh_DeviceConfigModeToString(const C_PuiSvData::E_DeviceConfigurationMode oe_Mode)
{
   QString c_Retval;

   switch (oe_Mode)
   {
   case C_PuiSvData::eDCM_ALL_CONNECTED_INTERFACES:
      c_Retval = "all-connected-interfaces";
      break;
   case C_PuiSvData::eDCM_ONLY_USED_INTERFACES:
      c_Retval = "only-used-interfaces";
      break;
   default:
      break;
   }

   return c_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Transform write mode to string

   \param[in]  oe_Mode  Write mode

   \return
   Stringified write mode
*/
//----------------------------------------------------------------------------------------------------------------------
QString C_PuiSvHandlerFilerV1::mh_WriteModeToString(const C_PuiSvDbWriteWidgetBase::E_WriteMode oe_Mode)
{
   QString c_Retval;

   switch (oe_Mode)
   {
   case C_PuiSvDbWriteWidgetBase::eWM_MANUAL:
      c_Retval = "manual";
      break;
   case C_PuiSvDbWriteWidgetBase::eWM_ON_CHANGE:
      c_Retval = "on-change";
      break;
   default:
      break;
   }

   return c_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Transform display style to string

   \param[in] oe_Style Display style

   \return
   Stringified display style
*/
//----------------------------------------------------------------------------------------------------------------------
QString C_PuiSvHandlerFilerV1::mh_DisplayStyleToString(const C_PuiSvDbWidgetBase::E_Style oe_Style)
{
   QString c_Retval;

   switch (oe_Style)
   {
   case C_PuiSvDbWidgetBase::eOPENSYDE:
      c_Retval = "opensyde";
      break;
   case C_PuiSvDbWidgetBase::eOPENSYDE_2:
      c_Retval = "opensyde2";
      break;
   case C_PuiSvDbWidgetBase::eFLAT:
      c_Retval = "flat";
      break;
   case C_PuiSvDbWidgetBase::eSKEUOMORPH:
      c_Retval = "skeuomorph";
      break;
   default:
      break;
   }

   return c_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Transform toggle type to string

   \param[in] oe_Type Toggle type

   \return
   Stringified toggle type
*/
//----------------------------------------------------------------------------------------------------------------------
QString C_PuiSvHandlerFilerV1::mh_ToggleTypeToString(const C_PuiSvDbToggle::E_Type oe_Type)
{
   QString c_Retval;

   switch (oe_Type)
   {
   case C_PuiSvDbToggle::eTYPE1:
      c_Retval = "type1";
      break;
   case C_PuiSvDbToggle::eTYPE2:
      c_Retval = "type2";
      break;
   case C_PuiSvDbToggle::eTYPE3:
      c_Retval = "type3";
      break;
   default:
      break;
   }

   return c_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Transform spin box type to string

   \param[in] oe_Type Spin box type

   \return
   Stringified spin box type
*/
//----------------------------------------------------------------------------------------------------------------------
QString C_PuiSvHandlerFilerV1::mh_SpinBoxTypeToString(const C_PuiSvDbSpinBox::E_Type oe_Type)
{
   QString c_Retval;

   switch (oe_Type)
   {
   case C_PuiSvDbSpinBox::eTYPE1:
      c_Retval = "type1";
      break;
   case C_PuiSvDbSpinBox::eTYPE2:
      c_Retval = "type2";
      break;
   default:
      break;
   }

   return c_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Transform label type to string

   \param[in] oe_Type Label type

   \return
   Stringified label type
*/
//----------------------------------------------------------------------------------------------------------------------
QString C_PuiSvHandlerFilerV1::mh_LabelTypeToString(const C_PuiSvDbLabel::E_Type oe_Type)
{
   QString c_Retval;

   switch (oe_Type)
   {
   case C_PuiSvDbLabel::eDEFAULT:
      c_Retval = "default";
      break;
   case C_PuiSvDbLabel::eTRANSPARENT:
      c_Retval = "transparent";
      break;
   default:
      break;
   }

   return c_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Transform slider type to string

   \param[in] oe_Type Slider type

   \return
   Stringified slider type
*/
//----------------------------------------------------------------------------------------------------------------------
QString C_PuiSvHandlerFilerV1::mh_SliderTypeToString(const C_PuiSvDbSlider::E_Type oe_Type)
{
   QString c_Retval;

   switch (oe_Type)
   {
   case C_PuiSvDbSlider::eTYPE_SMALL_COLOR_1:
      c_Retval = "small-color-1";
      break;
   case C_PuiSvDbSlider::eTYPE_BIG_COLOR_1:
      c_Retval = "big-color-1";
      break;
   case C_PuiSvDbSlider::eTYPE_SMALL_COLOR_2:
      c_Retval = "small-color-2";
      break;
   case C_PuiSvDbSlider::eTYPE_BIG_COLOR_2:
      c_Retval = "big-color-2";
      break;
   default:
      break;
   }

   return c_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Transform progress bar type to string

   \param[in] oe_Type Progress bar type

   \return
   Stringified progress bar type
*/
//----------------------------------------------------------------------------------------------------------------------
QString C_PuiSvHandlerFilerV1::mh_ProgressBarTypeToString(const C_PuiSvDbProgressBar::E_Type oe_Type)
{
   QString c_Retval;

   switch (oe_Type)
   {
   case C_PuiSvDbProgressBar::eTYPE_1:
      c_Retval = "type1";
      break;
   case C_PuiSvDbProgressBar::eTYPE_2:
      c_Retval = "type2";
      break;
   case C_PuiSvDbProgressBar::eTYPE_3:
      c_Retval = "type3";
      break;
   default:
      break;
   }

   return c_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Transform progress bar alignment type to string

   \param[in] oe_Type Progress bar alignment type

   \return
   Stringified progress bar alignment type
*/
//----------------------------------------------------------------------------------------------------------------------
QString C_PuiSvHandlerFilerV1::mh_ProgressBarAlignmentTypeToString(const C_PuiSvDbProgressBar::E_Alignment oe_Alignment)
{
   QString c_Retval;

   switch (oe_Alignment)
   {
   case C_PuiSvDbProgressBar::eTOP:
      c_Retval = "top";
      break;
   case C_PuiSvDbProgressBar::eLEFT:
      c_Retval = "left";
      break;
   case C_PuiSvDbProgressBar::eBOTTOM:
      c_Retval = "bottom";
      break;
   case C_PuiSvDbProgressBar::eRIGHT:
      c_Retval = "right";
      break;
   default:
      break;
   }

   return c_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Transform transmission mode to string

   \param[in] oe_Mode Transmission mode

   \return
   Stringified transmission mode
*/
//----------------------------------------------------------------------------------------------------------------------
QString C_PuiSvHandlerFilerV1::mh_TransmissionModeToString(
   const C_PuiSvReadDataConfiguration::E_TransmissionMode oe_Mode)
{
   QString c_Retval;

   switch (oe_Mode)
   {
   case C_PuiSvReadDataConfiguration::eTM_ON_TRIGGER:
      c_Retval = "manual";
      break;
   case C_PuiSvReadDataConfiguration::eTM_CYCLIC:
      c_Retval = "cyclic";
      break;
   case C_PuiSvReadDataConfiguration::eTM_ON_CHANGE:
      c_Retval = "cyclic-on-change";
      break;
   default:
      break;
   }

   return c_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Transform source type to string

   \param[in] oe_Type Source type

   \return
   Stringified source type
*/
//----------------------------------------------------------------------------------------------------------------------
QString C_PuiSvHandlerFilerV1::mh_SourceTypeToString(const C_PuiSvDbNodeDataPoolListElementId::E_Type oe_Type)
{
   QString c_Retval;

   switch (oe_Type)
   {
   case C_PuiSvDbNodeDataPoolListElementId::eDATAPOOL_ELEMENT:
      c_Retval = "datapool-element";
      break;
   case C_PuiSvDbNodeDataPoolListElementId::eBUS_SIGNAL:
      c_Retval = "bus-signal";
      break;
   default:
      break;
   }

   return c_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Interpret device config mode string

   \param[in] orc_Input String to interpret

   \return
   Device config mode
*/
//----------------------------------------------------------------------------------------------------------------------
C_PuiSvData::E_DeviceConfigurationMode C_PuiSvHandlerFilerV1::mh_StringToDeviceConfigMode(const QString & orc_Input)
{
   C_PuiSvData::E_DeviceConfigurationMode e_Retval;

   if (orc_Input.compare("all-connected-interfaces") == 0)
   {
      e_Retval = C_PuiSvData::eDCM_ALL_CONNECTED_INTERFACES;
   }
   else if (orc_Input.compare("only-used-interfaces") == 0)
   {
      e_Retval = C_PuiSvData::eDCM_ONLY_USED_INTERFACES;
   }
   else
   {
      e_Retval = C_PuiSvData::eDCM_ALL_CONNECTED_INTERFACES;
   }

   return e_Retval;
}
