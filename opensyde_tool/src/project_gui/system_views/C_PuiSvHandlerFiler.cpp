//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       System view filer (V2 format) (implementation)

   System view filer (V2 format)

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.h"

#include <QMap>
#include "stwtypes.h"
#include "stwerrors.h"
#include "TGLUtils.h"
#include "CSCLString.h"
#include "constants.h"
#include "C_OSCNodeDataPoolFiler.h"
#include "C_PuiSvHandlerFiler.h"
#include "C_PuiBsElementsFiler.h"
#include "C_PuiSdHandler.h"
#include "C_OSCNode.h"
#include "C_OSCLoggingHandler.h"
#include "C_OSCSystemFilerUtil.h"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw_scl;
using namespace stw_tgl;
using namespace stw_types;
using namespace stw_errors;
using namespace stw_opensyde_core;
using namespace stw_opensyde_gui_logic;

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
C_PuiSvHandlerFiler::C_PuiSvHandlerFiler(void)
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Load system views

   \param[in,out] orc_Views     System view elements (Cleared if necessary)
   \param[in,out] orc_XMLParser XML parser with the "current" element set to the "opensyde-system-views" element
   \param[in]     opc_BasePath  Base path (Optional for save to string)
   \param[in]     orc_OSCNodes  OSC node information (Necessary for update information)

   \return
   C_NO_ERR    information loaded
   C_CONFIG    error loading information
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_PuiSvHandlerFiler::h_LoadViews(std::vector<C_PuiSvData> & orc_Views,
                                        const std::vector<stw_opensyde_core::C_OSCNode> & orc_OSCNodes,
                                        C_OSCXMLParserBase & orc_XMLParser, const QDir * const opc_BasePath)
{
   sint32 s32_Retval = C_NO_ERR;

   if (orc_XMLParser.SelectNodeChild("opensyde-system-views") == "opensyde-system-views")
   {
      C_SCLString c_CurrentViewNode;
      uint32 u32_ExpectedSize = 0UL;
      const bool q_ExpectedSizeHere = orc_XMLParser.AttributeExists("length");

      //Check optional length
      if (q_ExpectedSizeHere == true)
      {
         u32_ExpectedSize = orc_XMLParser.GetAttributeUint32("length");
         orc_Views.reserve(u32_ExpectedSize);
      }

      //Clear last views
      orc_Views.clear();

      c_CurrentViewNode = orc_XMLParser.SelectNodeChild("opensyde-system-view");
      if (c_CurrentViewNode == "opensyde-system-view")
      {
         do
         {
            C_PuiSvData c_View;
            if (opc_BasePath != NULL)
            {
               const QString c_File = opc_BasePath->absoluteFilePath(orc_XMLParser.GetNodeContent().c_str());
               s32_Retval = mh_LoadViewFile(c_View, c_File, orc_OSCNodes);
            }
            else
            {
               s32_Retval = mh_LoadView(c_View, orc_XMLParser, orc_OSCNodes);
            }
            orc_Views.push_back(c_View);
            //Next
            c_CurrentViewNode = orc_XMLParser.SelectNodeNext("opensyde-system-view");
         }
         while ((c_CurrentViewNode == "opensyde-system-view") && (s32_Retval == C_NO_ERR));
         //Return
         tgl_assert(orc_XMLParser.SelectNodeParent() == "opensyde-system-views");
      }
      //Compare length
      if ((s32_Retval == C_NO_ERR) && (q_ExpectedSizeHere == true))
      {
         if (u32_ExpectedSize != orc_Views.size())
         {
            C_SCLString c_Tmp;
            c_Tmp.PrintFormatted("Unexpected view count, expected: %i, got %i", u32_ExpectedSize,
                                 orc_Views.size());
            osc_write_log_warning("Load file", c_Tmp.c_str());
         }
      }
   }
   else
   {
      s32_Retval = C_CONFIG;
      osc_write_log_error("Loading views", "Node \"opensyde-system-views\" not found.");
   }

   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Save data pools

   \param[in]     orc_Views     System view elements
   \param[in,out] orc_XMLParser XML parser with the "current" element set to the "opensyde-system-views" element
   \param[in]     opc_BasePath  Base path (Optional for save to string)

   \return
   C_NO_ERR   data saved
   C_CONFIG   file could not be created
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_PuiSvHandlerFiler::h_SaveViews(const std::vector<C_PuiSvData> & orc_Views, C_OSCXMLParserBase & orc_XMLParser,
                                        const QDir * const opc_BasePath)
{
   sint32 s32_Retval = C_NO_ERR;

   orc_XMLParser.CreateAndSelectNodeChild("opensyde-system-views");
   orc_XMLParser.SetAttributeUint32("length", orc_Views.size());
   for (uint32 u32_ItView = 0; (u32_ItView < orc_Views.size()) && (s32_Retval == C_NO_ERR); ++u32_ItView)
   {
      orc_XMLParser.CreateAndSelectNodeChild("opensyde-system-view");
      if (opc_BasePath != NULL)
      {
         const C_PuiSvData & rc_View = orc_Views[u32_ItView];
         const QString c_FilePath = C_PuiSvHandlerFiler::h_GetViewFileName(rc_View.GetName());
         const QString c_CombinedFilePath = opc_BasePath->absoluteFilePath(c_FilePath);
         s32_Retval = mh_SaveViewFile(rc_View, c_CombinedFilePath);
         //Store file name
         orc_XMLParser.SetNodeContent(c_FilePath.toStdString().c_str());
      }
      else
      {
         mh_SaveView(orc_Views[u32_ItView], orc_XMLParser);
      }
      //Return
      tgl_assert(orc_XMLParser.SelectNodeParent() == "opensyde-system-views");
   }
   //Return
   tgl_assert(orc_XMLParser.SelectNodeParent() == "opensyde-system-views");
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Load dashboard element

   \param[in,out] orc_Dashboard           Dashboard element
   \param[in,out] orc_XMLParser           XML parser with the "current" element set to the "dashboard" element
   \param[in]     oq_IgnoreMostErrorCases Optional flag to allow some error cases to still return C_NO_ERR

   \return
   C_NO_ERR    information loaded
   C_CONFIG    error loading information
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_PuiSvHandlerFiler::h_LoadDashboard(C_PuiSvDashboard & orc_Dashboard, C_OSCXMLParserBase & orc_XMLParser,
                                            const bool oq_IgnoreMostErrorCases)
{
   sint32 s32_Retval = C_NO_ERR;

   if (orc_XMLParser.SelectNodeChild("name") == "name")
   {
      orc_Dashboard.SetName(orc_XMLParser.GetNodeContent().c_str());
      //Return
      orc_XMLParser.SelectNodeParent();
   }
   else
   {
      if (oq_IgnoreMostErrorCases == false)
      {
         s32_Retval = C_CONFIG;
      }
   }

   if (orc_XMLParser.SelectNodeChild("comment") == "comment")
   {
      orc_Dashboard.SetComment(orc_XMLParser.GetNodeContent().c_str());
      //Return
      orc_XMLParser.SelectNodeParent();
   }

   if (s32_Retval == C_NO_ERR)
   {
      std::vector<C_PuiSvDbChart> c_Widgets;
      s32_Retval = mh_LoadCharts(c_Widgets, orc_XMLParser);
      orc_Dashboard.SetCharts(c_Widgets);
      if (oq_IgnoreMostErrorCases == true)
      {
         s32_Retval = C_NO_ERR;
      }
   }
   if (s32_Retval == C_NO_ERR)
   {
      std::vector<C_PuiSvDbLabel> c_Widgets;
      s32_Retval = mh_LoadLabels(c_Widgets, orc_XMLParser);
      orc_Dashboard.SetLabels(c_Widgets);
      if (oq_IgnoreMostErrorCases == true)
      {
         s32_Retval = C_NO_ERR;
      }
   }
   if (s32_Retval == C_NO_ERR)
   {
      std::vector<C_PuiSvDbParam> c_Widgets;
      s32_Retval = mh_LoadParams(c_Widgets, orc_XMLParser);
      orc_Dashboard.SetParams(c_Widgets);
      if (oq_IgnoreMostErrorCases == true)
      {
         s32_Retval = C_NO_ERR;
      }
   }
   if (s32_Retval == C_NO_ERR)
   {
      std::vector<C_PuiSvDbPieChart> c_Widgets;
      s32_Retval = mh_LoadPieCharts(c_Widgets, orc_XMLParser);
      orc_Dashboard.SetPieCharts(c_Widgets);
      if (oq_IgnoreMostErrorCases == true)
      {
         s32_Retval = C_NO_ERR;
      }
   }
   if (s32_Retval == C_NO_ERR)
   {
      std::vector<C_PuiSvDbSpinBox> c_Widgets;
      s32_Retval = mh_LoadSpinBoxes(c_Widgets, orc_XMLParser);
      orc_Dashboard.SetSpinBoxes(c_Widgets);
      if (oq_IgnoreMostErrorCases == true)
      {
         s32_Retval = C_NO_ERR;
      }
   }
   if (s32_Retval == C_NO_ERR)
   {
      std::vector<C_PuiSvDbSlider> c_Widgets;
      s32_Retval = mh_LoadSliders(c_Widgets, orc_XMLParser);
      orc_Dashboard.SetSliders(c_Widgets);
      if (oq_IgnoreMostErrorCases == true)
      {
         s32_Retval = C_NO_ERR;
      }
   }
   if (s32_Retval == C_NO_ERR)
   {
      std::vector<C_PuiSvDbProgressBar> c_Widgets;
      s32_Retval = mh_LoadProgressBars(c_Widgets, orc_XMLParser);
      orc_Dashboard.SetProgressBars(c_Widgets);
      if (oq_IgnoreMostErrorCases == true)
      {
         s32_Retval = C_NO_ERR;
      }
   }
   if (s32_Retval == C_NO_ERR)
   {
      std::vector<C_PuiSvDbTable> c_Widgets;
      s32_Retval = mh_LoadTables(c_Widgets, orc_XMLParser);
      orc_Dashboard.SetTables(c_Widgets);
      if (oq_IgnoreMostErrorCases == true)
      {
         s32_Retval = C_NO_ERR;
      }
   }
   if (s32_Retval == C_NO_ERR)
   {
      std::vector<C_PuiSvDbToggle> c_Widgets;
      s32_Retval = mh_LoadToggles(c_Widgets, orc_XMLParser);
      orc_Dashboard.SetToggles(c_Widgets);
      if (oq_IgnoreMostErrorCases == true)
      {
         s32_Retval = C_NO_ERR;
      }
   }
   if (s32_Retval == C_NO_ERR)
   {
      s32_Retval = C_PuiBsElementsFiler::h_LoadBaseElements(orc_Dashboard, orc_XMLParser);
   }
   orc_Dashboard.SetActive(orc_XMLParser.GetAttributeBool("active"));
   if (orc_XMLParser.AttributeExists("tab-index"))
   {
      orc_Dashboard.SetTabIndex(orc_XMLParser.GetAttributeSint32("tab-index"));
   }
   else
   {
      orc_Dashboard.SetTabIndex(-1);
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Save dashboard element

   \param[in]     orc_Dashboard Dashboard element
   \param[in,out] orc_XMLParser XML parser with the "current" element set to the "dashboard" element
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PuiSvHandlerFiler::h_SaveDashboard(const C_PuiSvDashboard & orc_Dashboard, C_OSCXMLParserBase & orc_XMLParser)
{
   orc_XMLParser.SetAttributeBool("active", orc_Dashboard.GetActive());
   orc_XMLParser.SetAttributeSint32("tab-index", orc_Dashboard.GetTabIndex());
   orc_XMLParser.CreateNodeChild("name", orc_Dashboard.GetName().toStdString().c_str());
   orc_XMLParser.CreateNodeChild("comment", orc_Dashboard.GetComment().toStdString().c_str());
   mh_SaveCharts(orc_Dashboard.GetCharts(), orc_XMLParser);
   mh_SaveLabels(orc_Dashboard.GetLabels(), orc_XMLParser);
   mh_SaveParams(orc_Dashboard.GetParams(), orc_XMLParser);
   mh_SavePieCharts(orc_Dashboard.GetPieCharts(), orc_XMLParser);
   mh_SaveSpinBoxes(orc_Dashboard.GetSpinBoxes(), orc_XMLParser);
   mh_SaveSliders(orc_Dashboard.GetSliders(), orc_XMLParser);
   mh_SaveProgressBars(orc_Dashboard.GetProgressBars(), orc_XMLParser);
   mh_SaveTables(orc_Dashboard.GetTables(), orc_XMLParser);
   mh_SaveToggles(orc_Dashboard.GetToggles(), orc_XMLParser);
   C_PuiBsElementsFiler::h_SaveBaseElements(orc_Dashboard, orc_XMLParser);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Load read rails

   \param[in,out] orc_Rails     Read rails
   \param[in,out] orc_XMLParser XML parser with the "current" element set to the "rail-assignments" element

   \return
   C_NO_ERR    information loaded
   C_CONFIG    error loading information
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_PuiSvHandlerFiler::h_LoadReadRails(QMap<C_OSCNodeDataPoolListElementId,
                                                 C_PuiSvReadDataConfiguration> & orc_Rails,
                                            C_OSCXMLParserBase & orc_XMLParser)
{
   sint32 s32_Retval = C_NO_ERR;

   C_SCLString c_CurrentRailNode = orc_XMLParser.SelectNodeChild("rail-assignment");

   if (c_CurrentRailNode == "rail-assignment")
   {
      do
      {
         C_PuiSvReadDataConfiguration c_DataConfiguration;
         C_OSCNodeDataPoolListElementId c_Id;
         c_DataConfiguration.u8_RailIndex = static_cast<uint8>(orc_XMLParser.GetAttributeUint32("rail-index"));
         if (orc_XMLParser.SelectNodeChild("id") == "id")
         {
            C_PuiSvHandlerFiler::mh_LoadDataElement(c_Id, orc_XMLParser);
            //Return
            tgl_assert(orc_XMLParser.SelectNodeParent() == "rail-assignment");
         }
         else
         {
            s32_Retval = C_CONFIG;
         }
         if (orc_XMLParser.SelectNodeChild("threshold") == "threshold")
         {
            if (C_OSCNodeDataPoolFiler::h_LoadDataPoolContentV1(c_DataConfiguration.c_ChangeThreshold,
                                                                orc_XMLParser) != C_NO_ERR)
            {
               s32_Retval = C_CONFIG;
            }
            //Return
            tgl_assert(orc_XMLParser.SelectNodeParent() == "rail-assignment");
         }
         else
         {
            s32_Retval = C_CONFIG;
         }
         if (orc_XMLParser.SelectNodeChild("transmission-mode") == "transmission-mode")
         {
            if (C_PuiSvHandlerFiler::mh_StringToTransmissionMode(orc_XMLParser.GetNodeContent().c_str(),
                                                                 c_DataConfiguration.e_TransmissionMode) != C_NO_ERR)
            {
               s32_Retval = C_CONFIG;
            }
            //Return
            tgl_assert(orc_XMLParser.SelectNodeParent() == "rail-assignment");
         }
         else
         {
            s32_Retval = C_CONFIG;
         }
         //Insert
         orc_Rails.insert(c_Id, c_DataConfiguration);
         //Next
         c_CurrentRailNode = orc_XMLParser.SelectNodeNext("rail-assignment");
      }
      while ((c_CurrentRailNode == "rail-assignment") && (s32_Retval == C_NO_ERR));
      //Return
      tgl_assert(orc_XMLParser.SelectNodeParent() == "rail-assignments");
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Save read rails

   \param[in]     orc_Rails     Read rails
   \param[in,out] orc_XMLParser XML parser with the "current" element set to the "rail-assignments" element
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PuiSvHandlerFiler::h_SaveReadRails(const QMap<C_OSCNodeDataPoolListElementId,
                                                     C_PuiSvReadDataConfiguration> & orc_Rails,
                                          C_OSCXMLParserBase & orc_XMLParser)
{
   for (QMap<C_OSCNodeDataPoolListElementId, C_PuiSvReadDataConfiguration>::const_iterator c_It = orc_Rails.begin();
        c_It != orc_Rails.end(); ++c_It)
   {
      const C_PuiSvReadDataConfiguration c_ReadData = c_It.value();
      orc_XMLParser.CreateAndSelectNodeChild("rail-assignment");
      orc_XMLParser.SetAttributeUint32("rail-index", static_cast<uint32>(c_ReadData.u8_RailIndex));
      orc_XMLParser.CreateAndSelectNodeChild("id");
      mh_SaveDataElement(c_It.key(), orc_XMLParser);
      //Return
      tgl_assert(orc_XMLParser.SelectNodeParent() == "rail-assignment");
      orc_XMLParser.CreateAndSelectNodeChild("threshold");
      C_OSCNodeDataPoolFiler::h_SaveDataPoolContentV1(c_ReadData.c_ChangeThreshold, orc_XMLParser);
      //Return
      tgl_assert(orc_XMLParser.SelectNodeParent() == "rail-assignment");
      orc_XMLParser.CreateNodeChild("transmission-mode",
                                    C_PuiSvHandlerFiler::mh_TransmissionModeToString(
                                       c_ReadData.e_TransmissionMode).toStdString().c_str());
      //Return
      tgl_assert(orc_XMLParser.SelectNodeParent() == "rail-assignments");
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get automatically generated file name

   \param[in] orc_ViewName View name

   \return
   Automatically generated file name
*/
//----------------------------------------------------------------------------------------------------------------------
QString C_PuiSvHandlerFiler::h_GetViewFileName(const QString & orc_ViewName)
{
   return C_SCLString("view_" + C_OSCSystemFilerUtil::h_PrepareItemNameForFileName(
                         orc_ViewName.toStdString().c_str()) +
                      ".xml").c_str();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Load node active flags

   \param[in,out] orc_NodeActiveFlags Node active flags (Cleared if necessary)
   \param[in,out] orc_XMLParser       XML parser with the "current" element set to the "opensyde-system-view" element

   \return
   C_NO_ERR    information loaded
   C_CONFIG    error loading information
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_PuiSvHandlerFiler::mh_LoadNodeActiveFlags(std::vector<uint8> & orc_NodeActiveFlags,
                                                   C_OSCXMLParserBase & orc_XMLParser)
{
   sint32 s32_Retval = C_NO_ERR;

   //Clear last node active flags
   orc_NodeActiveFlags.clear();
   if (orc_XMLParser.SelectNodeChild("active-nodes") == "active-nodes")
   {
      C_SCLString c_CurrentNodeActiveFlagNode = orc_XMLParser.SelectNodeChild("active-node");
      if (c_CurrentNodeActiveFlagNode == "active-node")
      {
         do
         {
            const bool q_CurrentValue = orc_XMLParser.GetAttributeBool("state");
            orc_NodeActiveFlags.push_back(static_cast<uint8>(q_CurrentValue));
            //Next
            c_CurrentNodeActiveFlagNode = orc_XMLParser.SelectNodeNext("active-node");
         }
         while (c_CurrentNodeActiveFlagNode == "active-node");
         //Return
         tgl_assert(orc_XMLParser.SelectNodeParent() == "active-nodes");
      }
      //Return
      tgl_assert(orc_XMLParser.SelectNodeParent() == "opensyde-system-view");
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
   \param[in,out] orc_XMLParser             XML parser with the "current" element set
                                            to the "opensyde-system-view" element
   \param[in]     orc_OSCNodes              OSC node information (Necessary for update information)

   \return
   C_NO_ERR    information loaded
   C_CONFIG    error loading information
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_PuiSvHandlerFiler::mh_LoadNodeUpdateInformation(std::vector<C_PuiSvNodeUpdate> & orc_NodeUpdateInformation,
                                                         C_OSCXMLParserBase & orc_XMLParser,
                                                         const std::vector<C_OSCNode> & orc_OSCNodes)
{
   sint32 s32_Retval = C_NO_ERR;

   //Clear last
   orc_NodeUpdateInformation.clear();
   //Previous implementation
   if (orc_XMLParser.SelectNodeChild("node-update-informations") == "node-update-informations")
   {
      C_SCLString c_CurrentNodeUpdateInformationNode = orc_XMLParser.SelectNodeChild("node-update-information");
      if (c_CurrentNodeUpdateInformationNode == "node-update-information")
      {
         uint32 u32_Counter = 0U;

         do
         {
            if (u32_Counter < orc_OSCNodes.size())
            {
               const C_OSCNode & rc_Node = orc_OSCNodes[u32_Counter];
               C_PuiSvNodeUpdate c_UpdateInfo;

               s32_Retval = mh_LoadOneNodeUpdateInformation(c_UpdateInfo, orc_XMLParser, rc_Node);
               orc_NodeUpdateInformation.push_back(c_UpdateInfo);
               //Next
               c_CurrentNodeUpdateInformationNode = orc_XMLParser.SelectNodeNext("node-update-information");
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
         tgl_assert(orc_XMLParser.SelectNodeParent() == "node-update-informations");
      }
      //Return
      tgl_assert(orc_XMLParser.SelectNodeParent() == "opensyde-system-view");
   }
   else
   {
      //New implementation
      if (orc_XMLParser.SelectNodeChild("node-update-information") == "node-update-information")
      {
         C_SCLString c_CurrentNodeUpdateInformationNode = orc_XMLParser.SelectNodeChild(
            "node-specific-update-information");
         if (c_CurrentNodeUpdateInformationNode == "node-specific-update-information")
         {
            uint32 u32_Counter = 0U;

            do
            {
               if (u32_Counter < orc_OSCNodes.size())
               {
                  const C_OSCNode & rc_Node = orc_OSCNodes[u32_Counter];
                  C_PuiSvNodeUpdate c_UpdateInfo;
                  s32_Retval = mh_LoadOneNodeUpdateInformation(c_UpdateInfo, orc_XMLParser, rc_Node);
                  orc_NodeUpdateInformation.push_back(c_UpdateInfo);
                  //Next
                  c_CurrentNodeUpdateInformationNode = orc_XMLParser.SelectNodeNext("node-specific-update-information");
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
            tgl_assert(orc_XMLParser.SelectNodeParent() == "node-update-information");
         }
         //Return
         tgl_assert(orc_XMLParser.SelectNodeParent() == "opensyde-system-view");
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
/*! \brief   Load any update information path

   \param[out]    orc_Paths          Read paths
   \param[in]     orc_XMLTagBaseName XML tag base name (parent with s and children as is)
   \param[in,out] orc_XMLParser      XML parser
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PuiSvHandlerFiler::mh_LoadNodeUpdateInformationPaths(std::vector<QString> & orc_Paths,
                                                            const QString & orc_XMLTagBaseName,
                                                            C_OSCXMLParserBase & orc_XMLParser)
{
   const C_SCLString c_ParentName = QString(orc_XMLTagBaseName + "s").toStdString().c_str();
   const C_SCLString c_ChildName = orc_XMLTagBaseName.toStdString().c_str();

   orc_Paths.clear();
   if (orc_XMLParser.SelectNodeChild(c_ParentName) == c_ParentName)
   {
      C_SCLString c_CurrentPathNode = orc_XMLParser.SelectNodeChild(c_ChildName);
      if (c_CurrentPathNode == c_ChildName)
      {
         do
         {
            const QString c_Content = orc_XMLParser.GetNodeContent().c_str();
            orc_Paths.push_back(c_Content);
            //Next
            c_CurrentPathNode = orc_XMLParser.SelectNodeNext(c_ChildName);
         }
         while (c_CurrentPathNode == c_ChildName);
         //Return
         tgl_assert(orc_XMLParser.SelectNodeParent() == c_ParentName);
      }
      //Return
      orc_XMLParser.SelectNodeParent();
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Load node update information

   \param[in,out] orc_NodeUpdateInformation Node update information
   \param[in,out] orc_XMLParser             XML parser with the "current" element set
                                            to the "node-update-information" or
                                                   "node-specific-update-information" element

   \return
   C_NO_ERR    information loaded
   C_CONFIG    error loading information
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_PuiSvHandlerFiler::mh_LoadOneNodeUpdateInformation(C_PuiSvNodeUpdate & orc_NodeUpdateInformation,
                                                            C_OSCXMLParserBase & orc_XMLParser,
                                                            const C_OSCNode & orc_Node)
{
   std::vector<QString> c_Paths;
   sint32 s32_Retval = C_NO_ERR;

   if (orc_XMLParser.AttributeExists("position") == true)
   {
      orc_NodeUpdateInformation.u32_NodeUpdatePosition = orc_XMLParser.GetAttributeUint32("position");
   }
   else
   {
      orc_NodeUpdateInformation.u32_NodeUpdatePosition = 0;
   }

   //Previous format (assuming these can only be data blocks!)
   if (orc_XMLParser.SelectNodeChild("paths") == "paths")
   {
      C_SCLString c_CurrentNodeUpdateInformationNode = orc_XMLParser.SelectNodeChild("path");
      if (c_CurrentNodeUpdateInformationNode == "path")
      {
         do
         {
            c_Paths.push_back(orc_XMLParser.GetNodeContent().c_str());
            //Next
            c_CurrentNodeUpdateInformationNode = orc_XMLParser.SelectNodeNext("path");
         }
         while (c_CurrentNodeUpdateInformationNode == "path");
         //Return
         tgl_assert(orc_XMLParser.SelectNodeParent() == "paths");
      }
      if (orc_Node.c_Applications.size() == c_Paths.size())
      {
         //matching size
         orc_NodeUpdateInformation.SetPaths(c_Paths, C_PuiSvNodeUpdate::eFTP_DATA_BLOCK);
      }
      else
      {
         //invalid size
         c_Paths.clear();
         c_Paths.resize(orc_Node.c_Applications.size(), "");
         orc_NodeUpdateInformation.SetPaths(c_Paths, C_PuiSvNodeUpdate::eFTP_DATA_BLOCK);
      }
      //Return
      orc_XMLParser.SelectNodeParent();
   }
   else
   {
      std::vector<C_PuiSvNodeUpdateParamInfo> c_ParamInfo;
      //New format
      C_PuiSvHandlerFiler::mh_LoadNodeUpdateInformationPaths(c_Paths, "data-block-path", orc_XMLParser);
      orc_NodeUpdateInformation.SetPaths(c_Paths, C_PuiSvNodeUpdate::eFTP_DATA_BLOCK);
      C_PuiSvHandlerFiler::mh_LoadNodeUpdateInformationParam(c_ParamInfo, orc_XMLParser);
      orc_NodeUpdateInformation.SetParamInfos(c_ParamInfo);
      C_PuiSvHandlerFiler::mh_LoadNodeUpdateInformationPaths(c_Paths, "file-based-path", orc_XMLParser);
      orc_NodeUpdateInformation.SetPaths(c_Paths, C_PuiSvNodeUpdate::eFTP_FILE_BASED);
   }

   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Load node parameter set update information

   \param[out]    orc_Info      Parsed parameter set update information
   \param[in,out] orc_XMLParser XML parser with the "current" element set
                                to the "node-update-information" or
                                       "node-specific-update-information" element
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PuiSvHandlerFiler::mh_LoadNodeUpdateInformationParam(std::vector<C_PuiSvNodeUpdateParamInfo> & orc_Info,
                                                            C_OSCXMLParserBase & orc_XMLParser)
{
   orc_Info.clear();
   if (orc_XMLParser.SelectNodeChild("param-sets") == "param-sets")
   {
      C_SCLString c_CurrentPathNode = orc_XMLParser.SelectNodeChild("param-set");
      if (c_CurrentPathNode == "param-set")
      {
         do
         {
            bool q_Error = false;
            QString c_Path;
            uint32 u32_LastKnownCrc;
            C_PuiSvNodeUpdateParamInfo c_Content;
            if (orc_XMLParser.SelectNodeChild("path") == "path")
            {
               c_Path = orc_XMLParser.GetNodeContent().c_str();
               //Return
               tgl_assert(orc_XMLParser.SelectNodeParent() == "param-set");
            }
            else
            {
               q_Error = true;
            }
            if (orc_XMLParser.AttributeExists("last-known-crc"))
            {
               u32_LastKnownCrc = orc_XMLParser.GetAttributeUint32("last-known-crc");
            }
            else
            {
               u32_LastKnownCrc = 0UL;
               q_Error = true;
            }
            if (q_Error == false)
            {
               c_Content.SetContent(c_Path, u32_LastKnownCrc);
               orc_Info.push_back(c_Content);
            }
            //Next
            c_CurrentPathNode = orc_XMLParser.SelectNodeNext("param-set");
         }
         while (c_CurrentPathNode == "param-set");
         //Return
         tgl_assert(orc_XMLParser.SelectNodeParent() == "param-sets");
      }
      //Return
      orc_XMLParser.SelectNodeParent();
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Load dashboard elements

   \param[in,out] orc_Dashboards Dashboard elements (Cleared if necessary)
   \param[in,out] orc_XMLParser  XML parser with the "current" element set to the "opensyde-system-view" element

   \return
   C_NO_ERR    information loaded
   C_CONFIG    error loading information
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_PuiSvHandlerFiler::mh_LoadDashboards(std::vector<C_PuiSvDashboard> & orc_Dashboards,
                                              C_OSCXMLParserBase & orc_XMLParser)
{
   sint32 s32_Retval = C_NO_ERR;

   //Clear last dashboards
   orc_Dashboards.clear();
   if (orc_XMLParser.SelectNodeChild("dashboards") == "dashboards")
   {
      C_SCLString c_CurrentDashboardNode = orc_XMLParser.SelectNodeChild("dashboard");
      if (c_CurrentDashboardNode == "dashboard")
      {
         do
         {
            C_PuiSvDashboard c_Dashboard;
            s32_Retval = h_LoadDashboard(c_Dashboard, orc_XMLParser);
            orc_Dashboards.push_back(c_Dashboard);
            //Next
            c_CurrentDashboardNode = orc_XMLParser.SelectNodeNext("dashboard");
         }
         while ((c_CurrentDashboardNode == "dashboard") && (s32_Retval == C_NO_ERR));
         //Return
         tgl_assert(orc_XMLParser.SelectNodeParent() == "dashboards");
      }
      //Return
      tgl_assert(orc_XMLParser.SelectNodeParent() == "opensyde-system-view");
   }
   else
   {
      s32_Retval = C_CONFIG;
   }

   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Load view

   \param[in,out] orc_View     View data storage
   \param[in]     orc_FilePath File path
   \param[in]     orc_OSCNodes OSC node information (Necessary for update information)

   \return
   C_NO_ERR    information loaded
   C_CONFIG    error loading information
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_PuiSvHandlerFiler::mh_LoadViewFile(C_PuiSvData & orc_View, const QString & orc_FilePath,
                                            const std::vector<C_OSCNode> & orc_OSCNodes)
{
   C_OSCXMLParser c_XMLParser;
   sint32 s32_Retval = C_OSCSystemFilerUtil::h_GetParserForExistingFile(c_XMLParser,
                                                                        orc_FilePath.toStdString().c_str(),
                                                                        "opensyde-view-definition");

   if (s32_Retval == C_NO_ERR)
   {
      if (c_XMLParser.SelectNodeChild("opensyde-system-view") == "opensyde-system-view")
      {
         s32_Retval = C_PuiSvHandlerFiler::mh_LoadView(orc_View, c_XMLParser, orc_OSCNodes);
      }
      else
      {
         osc_write_log_error("Loading view", "Could not find \"opensyde-system-view\" node.");
         s32_Retval = C_CONFIG;
      }
   }
   else
   {
      //More details are in log
      s32_Retval = C_CONFIG;
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Load view element

   \param[in,out] orc_View      View element
   \param[in,out] orc_XMLParser XML parser with the "current" element set to the "opensyde-system-view" element
   \param[in]     orc_OSCNodes  OSC node information (Necessary for update information)

   \return
   C_NO_ERR    information loaded
   C_CONFIG    error loading information
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_PuiSvHandlerFiler::mh_LoadView(C_PuiSvData & orc_View, C_OSCXMLParserBase & orc_XMLParser,
                                        const std::vector<C_OSCNode> & orc_OSCNodes)
{
   sint32 s32_Retval = C_NO_ERR;

   if (orc_XMLParser.AttributeExists("darkmode") == true)
   {
      orc_View.SetDarkModeActive(orc_XMLParser.GetAttributeBool("darkmode"));
   }
   else
   {
      //Default
      orc_View.SetDarkModeActive(false);
   }
   if (orc_XMLParser.AttributeExists("device-config-selected-bit-rate") == true)
   {
      orc_View.SetDeviceConfigSelectedBitRate(orc_XMLParser.GetAttributeUint32("device-config-selected-bit-rate"));
   }
   else
   {
      //Default
      orc_View.SetDeviceConfigSelectedBitRate(125U);
   }
   if (orc_XMLParser.SelectNodeChild("name") == "name")
   {
      orc_View.SetName(orc_XMLParser.GetNodeContent().c_str());
      //Return
      tgl_assert(orc_XMLParser.SelectNodeParent() == "opensyde-system-view");
   }
   else
   {
      osc_write_log_error("Loading view", "Node \"name\" not found.");
      s32_Retval = C_CONFIG;
   }
   if ((orc_XMLParser.SelectNodeChild("device-config-mode") == "device-config-mode") && (s32_Retval == C_NO_ERR))
   {
      orc_View.SetDeviceConfigMode(mh_StringToDeviceConfigMode(orc_XMLParser.GetNodeContent().c_str()));
      //Return
      tgl_assert(orc_XMLParser.SelectNodeParent() == "opensyde-system-view");
   }
   else
   {
      //Default
      orc_View.SetDeviceConfigMode(C_PuiSvData::eDCM_ALL_CONNECTED_INTERFACES);
   }
   if (orc_XMLParser.SelectNodeChild("update-rates") == "update-rates")
   {
      orc_View.SetUpdateRateFast(static_cast<uint16>(orc_XMLParser.GetAttributeUint32("fast")));
      orc_View.SetUpdateRateMedium(static_cast<uint16>(orc_XMLParser.GetAttributeUint32("medium")));
      orc_View.SetUpdateRateSlow(static_cast<uint16>(orc_XMLParser.GetAttributeUint32("slow")));
      //Return
      tgl_assert(orc_XMLParser.SelectNodeParent() == "opensyde-system-view");
   }
   else
   {
      osc_write_log_error("Loading view", "Node \"update-rates\" not found.");
      s32_Retval = C_CONFIG;
   }
   if (s32_Retval == C_NO_ERR)
   {
      std::vector<uint8> c_NodeActiveFlags;
      s32_Retval = mh_LoadNodeActiveFlags(c_NodeActiveFlags, orc_XMLParser);
      orc_View.SetNodeActiveFlags(c_NodeActiveFlags);
      if (s32_Retval == C_NO_ERR)
      {
         std::vector<C_PuiSvNodeUpdate> c_NodeUpdateInformation;
         //If you have an async project this might help
         //c_NodeUpdateInformation.resize(c_NodeActiveFlags.size(),C_PuiSvNodeUpdate());
         s32_Retval = mh_LoadNodeUpdateInformation(c_NodeUpdateInformation, orc_XMLParser, orc_OSCNodes);
         orc_View.SetNodeUpdateInformation(c_NodeUpdateInformation);
         if (s32_Retval == C_NO_ERR)
         {
            if (orc_XMLParser.SelectNodeChild("pc") == "pc")
            {
               C_PuiSvPc c_PcData;
               s32_Retval = mh_LoadPc(c_PcData, orc_XMLParser);
               orc_View.SetPcData(c_PcData);
               //Return
               tgl_assert(orc_XMLParser.SelectNodeParent() == "opensyde-system-view");
            }
            else
            {
               osc_write_log_error("Loading view", "Node \"pc\" not found.");
               s32_Retval = C_CONFIG;
            }
            if (s32_Retval == C_NO_ERR)
            {
               std::vector<C_PuiSvDashboard> c_Dashboards;
               s32_Retval = mh_LoadDashboards(c_Dashboards, orc_XMLParser);
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
      if (orc_XMLParser.SelectNodeChild("rail-assignments") == "rail-assignments")
      {
         QMap<C_OSCNodeDataPoolListElementId, C_PuiSvReadDataConfiguration> c_Rails;
         s32_Retval = h_LoadReadRails(c_Rails, orc_XMLParser);
         orc_View.SetReadRailAssignments(c_Rails);
         //Return
         tgl_assert(orc_XMLParser.SelectNodeParent() == "opensyde-system-view");
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

   \param[in,out] orc_Pc        PC element
   \param[in,out] orc_XMLParser XML parser with the "current" element set to the "pc" element

   \return
   C_NO_ERR    information loaded
   C_CONFIG    error loading information
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_PuiSvHandlerFiler::mh_LoadPc(C_PuiSvPc & orc_Pc, C_OSCXMLParserBase & orc_XMLParser)
{
   sint32 s32_Retval = C_NO_ERR;

   orc_Pc.SetConnected(orc_XMLParser.GetAttributeBool("connected"), orc_XMLParser.GetAttributeUint32("bus-index"),
                       true);
   if (orc_XMLParser.SelectNodeChild("dll-path") == "dll-path")
   {
      QString c_Path = orc_XMLParser.GetNodeContent().c_str();
      if (orc_XMLParser.AttributeExists("type"))
      {
         orc_Pc.SetCANDllType(static_cast<C_PuiSvPc::E_CANDllType>(orc_XMLParser.GetAttributeSint32("type")));
      }
      else
      {
         // translate from path to type+path for compatibility reasons (type was not present in old openSYDE versions)
         if ((c_Path == stw_opensyde_gui::mc_DLL_PATH_PEAK) || (c_Path.isEmpty() == true))
         {
            orc_Pc.SetCANDllType(C_PuiSvPc::E_CANDllType::ePEAK);
            c_Path = "";
         }
         else if (c_Path == stw_opensyde_gui::mc_DLL_PATH_VECTOR)
         {
            orc_Pc.SetCANDllType(C_PuiSvPc::E_CANDllType::eVECTOR);
            c_Path = "";
         }
         else
         {
            orc_Pc.SetCANDllType(C_PuiSvPc::E_CANDllType::eOTHER);
         }
      }
      orc_Pc.SetCustomCANDllPath(c_Path);
      //Return
      tgl_assert(orc_XMLParser.SelectNodeParent() == "pc");
   }
   else
   {
      s32_Retval = C_CONFIG;
   }
   if ((orc_XMLParser.SelectNodeChild("box") == "box") && (s32_Retval == C_NO_ERR))
   {
      s32_Retval = C_PuiBsElementsFiler::h_LoadBoxBase(orc_Pc, orc_XMLParser);
      //Return
      tgl_assert(orc_XMLParser.SelectNodeParent() == "pc");
   }
   else
   {
      s32_Retval = C_CONFIG;
   }
   if ((orc_XMLParser.SelectNodeChild("connection") == "connection") && (s32_Retval == C_NO_ERR))
   {
      if (orc_XMLParser.SelectNodeChild("line") == "line")
      {
         C_PuiBsLineBase c_Connection;
         s32_Retval = C_PuiBsElementsFiler::h_LoadLineBase(c_Connection, orc_XMLParser);
         orc_Pc.SetConnectionData(c_Connection);
         //Return
         tgl_assert(orc_XMLParser.SelectNodeParent() == "connection");
      }
      else
      {
         s32_Retval = C_CONFIG;
      }
      //Return
      tgl_assert(orc_XMLParser.SelectNodeParent() == "pc");
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
   \param[in,out] orc_XMLParser  XML parser with the "current" element set to the "dashboard" element

   \return
   C_NO_ERR    information loaded
   C_CONFIG    error loading information
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_PuiSvHandlerFiler::mh_LoadCharts(std::vector<C_PuiSvDbChart> & orc_Widgets, C_OSCXMLParserBase & orc_XMLParser)
{
   sint32 s32_Retval = C_NO_ERR;

   //Clear last widgets
   orc_Widgets.clear();
   if (orc_XMLParser.SelectNodeChild("charts") == "charts")
   {
      C_SCLString c_CurrentWidgetNode = orc_XMLParser.SelectNodeChild("chart");
      if (c_CurrentWidgetNode == "chart")
      {
         do
         {
            C_PuiSvDbChart c_Box;
            s32_Retval = C_PuiSvHandlerFiler::mh_LoadWidgetBase(c_Box, orc_XMLParser);
            if (orc_XMLParser.SelectNodeChild("active-flags") == "active-flags")
            {
               C_SCLString c_CurrentWidgetNode2 = orc_XMLParser.SelectNodeChild("active-flag");
               if (c_CurrentWidgetNode2 == "active-flag")
               {
                  do
                  {
                     if (orc_XMLParser.AttributeExists("state") == true)
                     {
                        c_Box.c_DataPoolElementsActive.push_back(orc_XMLParser.GetAttributeBool("state"));
                     }
                     else
                     {
                        s32_Retval = C_CONFIG;
                     }
                     //Next
                     c_CurrentWidgetNode2 = orc_XMLParser.SelectNodeNext("active-flag");
                  }
                  while ((c_CurrentWidgetNode2 == "active-flag") && (s32_Retval == C_NO_ERR));

                  //Return
                  tgl_assert(orc_XMLParser.SelectNodeParent() == "active-flags");
               }
               //Return
               tgl_assert(orc_XMLParser.SelectNodeParent() == "chart");
            }

            if (c_Box.c_DataPoolElementsActive.size() != c_Box.c_DataPoolElementsConfig.size())
            {
               // Size shall be identical
               c_Box.c_DataPoolElementsActive.resize(c_Box.c_DataPoolElementsConfig.size(), true);
            }

            if (orc_XMLParser.SelectNodeChild("zoom-mode") == "zoom-mode")
            {
               if (C_PuiSvHandlerFiler::mh_StringToChartSettingZoomMode(orc_XMLParser.GetNodeContent().c_str(),
                                                                        c_Box.e_SettingZoomMode) != C_NO_ERR)
               {
                  s32_Retval = C_CONFIG;
               }

               //Return
               tgl_assert(orc_XMLParser.SelectNodeParent() == "chart");
            }

            orc_Widgets.push_back(c_Box);
            //Next
            c_CurrentWidgetNode = orc_XMLParser.SelectNodeNext("chart");
         }
         while ((c_CurrentWidgetNode == "chart") && (s32_Retval == C_NO_ERR));
         //Return
         tgl_assert(orc_XMLParser.SelectNodeParent() == "charts");
      }
      //Return
      orc_XMLParser.SelectNodeParent();
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
   \param[in,out] orc_XMLParser  XML parser with the "current" element set to the "dashboard" element

   \return
   C_NO_ERR    information loaded
   C_CONFIG    error loading information
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_PuiSvHandlerFiler::mh_LoadLabels(std::vector<C_PuiSvDbLabel> & orc_Widgets, C_OSCXMLParserBase & orc_XMLParser)
{
   sint32 s32_Retval = C_NO_ERR;

   //Clear last widgets
   orc_Widgets.clear();
   if (orc_XMLParser.SelectNodeChild("labels") == "labels")
   {
      C_SCLString c_CurrentWidgetNode = orc_XMLParser.SelectNodeChild("label");
      if (c_CurrentWidgetNode == "label")
      {
         do
         {
            C_PuiSvDbLabel c_Box;
            c_Box.q_ShowCaption = orc_XMLParser.GetAttributeBool("show-caption");
            c_Box.q_ShowUnit = orc_XMLParser.GetAttributeBool("show-unit");
            s32_Retval = C_PuiSvHandlerFiler::mh_LoadWidgetBase(c_Box, orc_XMLParser);
            if (orc_XMLParser.SelectNodeChild("caption") == "caption")
            {
               c_Box.c_Caption = orc_XMLParser.GetNodeContent().c_str();
               //Return
               tgl_assert(orc_XMLParser.SelectNodeParent() == "label");
            }
            if (orc_XMLParser.SelectNodeChild("type") == "type")
            {
               if (mh_StringToLabelType(orc_XMLParser.GetNodeContent().c_str(), c_Box.e_Type) != C_NO_ERR)
               {
                  s32_Retval = C_CONFIG;
               }
               //Return
               tgl_assert(orc_XMLParser.SelectNodeParent() == "label");
            }
            else
            {
               s32_Retval = C_CONFIG;
            }
            orc_Widgets.push_back(c_Box);
            //Next
            c_CurrentWidgetNode = orc_XMLParser.SelectNodeNext("label");
         }
         while ((c_CurrentWidgetNode == "label") && (s32_Retval == C_NO_ERR));
         //Return
         tgl_assert(orc_XMLParser.SelectNodeParent() == "labels");
      }
      //Return
      orc_XMLParser.SelectNodeParent();
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
   \param[in,out] orc_XMLParser  XML parser with the "current" element set to the "dashboard" element

   \return
   C_NO_ERR    information loaded
   C_CONFIG    error loading information
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_PuiSvHandlerFiler::mh_LoadParams(std::vector<C_PuiSvDbParam> & orc_Widgets, C_OSCXMLParserBase & orc_XMLParser)
{
   sint32 s32_Retval = C_NO_ERR;

   //Clear last widgets
   orc_Widgets.clear();
   if (orc_XMLParser.SelectNodeChild("params") == "params")
   {
      C_SCLString c_CurrentWidgetNode = orc_XMLParser.SelectNodeChild("param");
      if (c_CurrentWidgetNode == "param")
      {
         do
         {
            C_PuiSvDbParam c_Box;
            s32_Retval = C_PuiSvHandlerFiler::mh_LoadWidgetBase(c_Box, orc_XMLParser);
            if (s32_Retval == C_NO_ERR)
            {
               s32_Retval = mh_LoadParamDataSetIndices(c_Box.c_DataSetSelectionIndices, orc_XMLParser);
               if (s32_Retval == C_NO_ERR)
               {
                  s32_Retval = mh_LoadParamValues(c_Box.c_ListValues, orc_XMLParser);
                  if (s32_Retval == C_NO_ERR)
                  {
                     s32_Retval = mh_LoadParamTables(c_Box.c_ColWidth, orc_XMLParser);
                     if (s32_Retval == C_NO_ERR)
                     {
                        mh_LoadParamExpandedItems(c_Box.c_ExpandedItems, orc_XMLParser);
                        mh_LoadParamColumnPositionIndices(c_Box.c_ColPosIndices, orc_XMLParser);
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
            c_CurrentWidgetNode = orc_XMLParser.SelectNodeNext("param");
         }
         while ((c_CurrentWidgetNode == "param") && (s32_Retval == C_NO_ERR));
         //Return
         tgl_assert(orc_XMLParser.SelectNodeParent() == "params");
      }
      //Return
      orc_XMLParser.SelectNodeParent();
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
   \param[in,out] orc_XMLParser XML parser with the "current" element set to the "param" element
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PuiSvHandlerFiler::mh_LoadParamExpandedItems(std::vector<C_PuiSvDbExpandedTreeIndex> & orc_Items,
                                                    C_OSCXMLParserBase & orc_XMLParser)
{
   orc_Items.clear();
   if (orc_XMLParser.SelectNodeChild("expanded-tree-items") == "expanded-tree-items")
   {
      C_SCLString c_CurrentWidgetNode = orc_XMLParser.SelectNodeChild("expanded-tree-item");
      if (c_CurrentWidgetNode == "expanded-tree-item")
      {
         do
         {
            C_PuiSvDbExpandedTreeIndex c_Item;
            if (orc_XMLParser.AttributeExists("valid-layers") == true)
            {
               c_Item.u32_Layer = orc_XMLParser.GetAttributeUint32("valid-layers");
            }
            else
            {
               c_Item.u32_Layer = 0UL;
            }
            C_PuiSvHandlerFiler::mh_LoadUiIndex(c_Item.c_ExpandedId, orc_XMLParser);
            orc_Items.push_back(c_Item);
            //Next
            c_CurrentWidgetNode = orc_XMLParser.SelectNodeNext("expanded-tree-item");
         }
         while (c_CurrentWidgetNode == "expanded-tree-item");
         //Return
         tgl_assert(orc_XMLParser.SelectNodeParent() == "expanded-tree-items");
      }
      //Return
      tgl_assert(orc_XMLParser.SelectNodeParent() == "param");
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Load column position indices elements

   \param[out]    orc_Items     Column position indices elements
   \param[in,out] orc_XMLParser XML parser with the "current" element set to the "param" element
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PuiSvHandlerFiler::mh_LoadParamColumnPositionIndices(std::vector<sint32> & orc_Items,
                                                            C_OSCXMLParserBase & orc_XMLParser)
{
   orc_Items.clear();
   if (orc_XMLParser.SelectNodeChild("column-position-indices") == "column-position-indices")
   {
      C_SCLString c_CurrentWidgetNode = orc_XMLParser.SelectNodeChild("column-position-index");
      if (c_CurrentWidgetNode == "column-position-index")
      {
         do
         {
            sint32 s32_Index;
            if (orc_XMLParser.AttributeExists("value") == true)
            {
               s32_Index = orc_XMLParser.GetAttributeSint32("value");
            }
            else
            {
               s32_Index = orc_Items.size();
            }
            orc_Items.push_back(s32_Index);
            //Next
            c_CurrentWidgetNode = orc_XMLParser.SelectNodeNext("column-position-index");
         }
         while (c_CurrentWidgetNode == "column-position-index");
         //Return
         tgl_assert(orc_XMLParser.SelectNodeParent() == "column-position-indices");
      }
      //Return
      tgl_assert(orc_XMLParser.SelectNodeParent() == "param");
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Load widget elements

   \param[in,out] orc_Widgets    Widget elements (Cleared if necessary)
   \param[in,out] orc_XMLParser  XML parser with the "current" element set to the "dashboard" element

   \return
   C_NO_ERR    information loaded
   C_CONFIG    error loading information
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_PuiSvHandlerFiler::mh_LoadPieCharts(std::vector<C_PuiSvDbPieChart> & orc_Widgets,
                                             C_OSCXMLParserBase & orc_XMLParser)
{
   sint32 s32_Retval = C_NO_ERR;

   //Clear last widgets
   orc_Widgets.clear();
   if (orc_XMLParser.SelectNodeChild("pie-charts") == "pie-charts")
   {
      C_SCLString c_CurrentWidgetNode = orc_XMLParser.SelectNodeChild("pie-chart");
      if (c_CurrentWidgetNode == "pie-chart")
      {
         do
         {
            C_PuiSvDbPieChart c_Box;
            c_Box.q_ShowValue = orc_XMLParser.GetAttributeBool("show-value");
            c_Box.q_ShowUnit = orc_XMLParser.GetAttributeBool("show-unit");
            s32_Retval = C_PuiSvHandlerFiler::mh_LoadWidgetBase(c_Box, orc_XMLParser);
            orc_Widgets.push_back(c_Box);
            //Next
            c_CurrentWidgetNode = orc_XMLParser.SelectNodeNext("pie-chart");
         }
         while ((c_CurrentWidgetNode == "pie-chart") && (s32_Retval == C_NO_ERR));
         //Return
         tgl_assert(orc_XMLParser.SelectNodeParent() == "pie-charts");
      }
      //Return
      orc_XMLParser.SelectNodeParent();
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
   \param[in,out] orc_XMLParser  XML parser with the "current" element set to the "dashboard" element

   \return
   C_NO_ERR    information loaded
   C_CONFIG    error loading information
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_PuiSvHandlerFiler::mh_LoadSpinBoxes(std::vector<C_PuiSvDbSpinBox> & orc_Widgets,
                                             C_OSCXMLParserBase & orc_XMLParser)
{
   sint32 s32_Retval = C_NO_ERR;

   //Clear last widgets
   orc_Widgets.clear();
   if (orc_XMLParser.SelectNodeChild("spin-boxes") == "spin-boxes")
   {
      C_SCLString c_CurrentWidgetNode = orc_XMLParser.SelectNodeChild("spin-box");
      if (c_CurrentWidgetNode == "spin-box")
      {
         do
         {
            C_PuiSvDbSpinBox c_Box;
            c_Box.q_ShowUnit = orc_XMLParser.GetAttributeBool("show-unit");
            s32_Retval = C_PuiSvHandlerFiler::mh_LoadWidgetBase(c_Box, orc_XMLParser);
            if (orc_XMLParser.SelectNodeChild("type") == "type")
            {
               if (mh_StringToSpinBoxType(orc_XMLParser.GetNodeContent().c_str(), c_Box.e_Type) != C_NO_ERR)
               {
                  s32_Retval = C_CONFIG;
               }
               //Return
               tgl_assert(orc_XMLParser.SelectNodeParent() == "spin-box");
            }
            else
            {
               s32_Retval = C_CONFIG;
            }
            if (orc_XMLParser.SelectNodeChild("content") == "content")
            {
               if (C_OSCNodeDataPoolFiler::h_LoadDataPoolContentV1(c_Box.c_Value, orc_XMLParser) != C_NO_ERR)
               {
                  s32_Retval = C_CONFIG;
               }
               //Return
               tgl_assert(orc_XMLParser.SelectNodeParent() == "spin-box");
            }
            orc_Widgets.push_back(c_Box);
            //Next
            c_CurrentWidgetNode = orc_XMLParser.SelectNodeNext("spin-box");
         }
         while ((c_CurrentWidgetNode == "spin-box") && (s32_Retval == C_NO_ERR));
         //Return
         tgl_assert(orc_XMLParser.SelectNodeParent() == "spin-boxes");
      }
      //Return
      orc_XMLParser.SelectNodeParent();
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
   \param[in,out] orc_XMLParser  XML parser with the "current" element set to the "dashboard" element

   \return
   C_NO_ERR    information loaded
   C_CONFIG    error loading information
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_PuiSvHandlerFiler::mh_LoadTables(std::vector<C_PuiSvDbTable> & orc_Widgets, C_OSCXMLParserBase & orc_XMLParser)
{
   sint32 s32_Retval = C_NO_ERR;

   //Clear last widgets
   orc_Widgets.clear();
   if (orc_XMLParser.SelectNodeChild("tables") == "tables")
   {
      C_SCLString c_CurrentWidgetNode = orc_XMLParser.SelectNodeChild("table");
      if (c_CurrentWidgetNode == "table")
      {
         do
         {
            C_PuiSvDbTable c_Box;
            s32_Retval = C_PuiSvHandlerFiler::mh_LoadWidgetBase(c_Box, orc_XMLParser);

            //Columns
            if (orc_XMLParser.SelectNodeChild("columns") == "columns")
            {
               C_SCLString c_CurrentColumnNode = orc_XMLParser.SelectNodeChild("column");
               if (c_CurrentColumnNode == "column")
               {
                  do
                  {
                     if (orc_XMLParser.AttributeExists("width") == true)
                     {
                        c_Box.c_ColumnWidth.push_back(orc_XMLParser.GetAttributeSint32("width"));
                     }
                     else
                     {
                        s32_Retval = C_CONFIG;
                     }
                     //Next
                     c_CurrentColumnNode = orc_XMLParser.SelectNodeNext("column");
                  }
                  while ((c_CurrentColumnNode == "column") && (s32_Retval == C_NO_ERR));
                  //Return
                  tgl_assert(orc_XMLParser.SelectNodeParent() == "columns");
               }
               //Return
               tgl_assert(orc_XMLParser.SelectNodeParent() == "table");
            }

            orc_Widgets.push_back(c_Box);
            //Next
            c_CurrentWidgetNode = orc_XMLParser.SelectNodeNext("table");
         }
         while ((c_CurrentWidgetNode == "table") && (s32_Retval == C_NO_ERR));
         //Return
         tgl_assert(orc_XMLParser.SelectNodeParent() == "tables");
      }
      //Return
      orc_XMLParser.SelectNodeParent();
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
   \param[in,out] orc_XMLParser  XML parser with the "current" element set to the "dashboard" element

   \return
   C_NO_ERR    information loaded
   C_CONFIG    error loading information
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_PuiSvHandlerFiler::mh_LoadSliders(std::vector<C_PuiSvDbSlider> & orc_Widgets,
                                           C_OSCXMLParserBase & orc_XMLParser)
{
   sint32 s32_Retval = C_NO_ERR;

   //Clear last widgets
   orc_Widgets.clear();
   if (orc_XMLParser.SelectNodeChild("sliders") == "sliders")
   {
      C_SCLString c_CurrentWidgetNode = orc_XMLParser.SelectNodeChild("slider");
      if (c_CurrentWidgetNode == "slider")
      {
         do
         {
            C_PuiSvDbSlider c_Box;
            s32_Retval = C_PuiSvHandlerFiler::mh_LoadWidgetBase(c_Box, orc_XMLParser);
            c_Box.q_ShowMinMax = orc_XMLParser.GetAttributeBool("show-min-max");
            if (s32_Retval == C_NO_ERR)
            {
               if (orc_XMLParser.SelectNodeChild("type") == "type")
               {
                  if (C_PuiSvHandlerFiler::mh_StringToSliderType(orc_XMLParser.GetNodeContent().c_str(),
                                                                 c_Box.e_Type) != C_NO_ERR)
                  {
                     s32_Retval = C_CONFIG;
                  }
                  //Return
                  tgl_assert(orc_XMLParser.SelectNodeParent() == "slider");
               }
               else
               {
                  s32_Retval = C_CONFIG;
               }
            }
            //Value
            if (orc_XMLParser.AttributeExists("value") == true)
            {
               c_Box.s32_Value = orc_XMLParser.GetAttributeSint32("value");
            }
            else
            {
               c_Box.s32_Value = 0;
            }
            //Add
            orc_Widgets.push_back(c_Box);
            //Next
            c_CurrentWidgetNode = orc_XMLParser.SelectNodeNext("slider");
         }
         while ((c_CurrentWidgetNode == "slider") && (s32_Retval == C_NO_ERR));
         //Return
         tgl_assert(orc_XMLParser.SelectNodeParent() == "sliders");
      }
      //Return
      orc_XMLParser.SelectNodeParent();
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
   \param[in,out] orc_XMLParser  XML parser with the "current" element set to the "dashboard" element

   \return
   C_NO_ERR    information loaded
   C_CONFIG    error loading information
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_PuiSvHandlerFiler::mh_LoadProgressBars(std::vector<C_PuiSvDbProgressBar> & orc_Widgets,
                                                C_OSCXMLParserBase & orc_XMLParser)
{
   sint32 s32_Retval = C_NO_ERR;

   //Clear last widgets
   orc_Widgets.clear();
   if (orc_XMLParser.SelectNodeChild("progress-bars") == "progress-bars")
   {
      C_SCLString c_CurrentWidgetNode = orc_XMLParser.SelectNodeChild("progress-bar");
      if (c_CurrentWidgetNode == "progress-bar")
      {
         do
         {
            C_PuiSvDbProgressBar c_Box;
            s32_Retval = C_PuiSvHandlerFiler::mh_LoadWidgetBase(c_Box, orc_XMLParser);
            c_Box.q_ShowMinMax = orc_XMLParser.GetAttributeBool("show-min-max");
            if (s32_Retval == C_NO_ERR)
            {
               if (orc_XMLParser.AttributeExists("show-unit") == true)
               {
                  c_Box.q_ShowUnit = orc_XMLParser.GetAttributeBool("show-unit");
               }
               else
               {
                  c_Box.q_ShowUnit = false;
               }
               if (orc_XMLParser.SelectNodeChild("type") == "type")
               {
                  if (C_PuiSvHandlerFiler::mh_StringToProgressBarType(orc_XMLParser.GetNodeContent().c_str(),
                                                                      c_Box.e_Type) != C_NO_ERR)
                  {
                     s32_Retval = C_CONFIG;
                  }
                  //Return
                  tgl_assert(orc_XMLParser.SelectNodeParent() == "progress-bar");
               }
               else
               {
                  s32_Retval = C_CONFIG;
               }
               if (s32_Retval == C_NO_ERR)
               {
                  if (orc_XMLParser.SelectNodeChild("alignment") == "alignment")
                  {
                     if (C_PuiSvHandlerFiler::mh_StringToProgressBarAlignmentType(orc_XMLParser.GetNodeContent().c_str(),
                                                                                  c_Box.e_Alignment) != C_NO_ERR)
                     {
                        s32_Retval = C_CONFIG;
                     }
                     //Return
                     tgl_assert(orc_XMLParser.SelectNodeParent() == "progress-bar");
                  }
                  else
                  {
                     s32_Retval = C_CONFIG;
                  }
               }
            }
            orc_Widgets.push_back(c_Box);
            //Next
            c_CurrentWidgetNode = orc_XMLParser.SelectNodeNext("progress-bar");
         }
         while ((c_CurrentWidgetNode == "progress-bar") && (s32_Retval == C_NO_ERR));
         //Return
         tgl_assert(orc_XMLParser.SelectNodeParent() == "progress-bars");
      }
      //Return
      orc_XMLParser.SelectNodeParent();
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
   \param[in,out] orc_XMLParser  XML parser with the "current" element set to the "dashboard" element

   \return
   C_NO_ERR    information loaded
   C_CONFIG    error loading information
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_PuiSvHandlerFiler::mh_LoadToggles(std::vector<C_PuiSvDbToggle> & orc_Widgets,
                                           C_OSCXMLParserBase & orc_XMLParser)
{
   sint32 s32_Retval = C_NO_ERR;

   //Clear last widgets
   orc_Widgets.clear();
   if (orc_XMLParser.SelectNodeChild("toggles") == "toggles")
   {
      C_SCLString c_CurrentWidgetNode = orc_XMLParser.SelectNodeChild("toggle");
      if (c_CurrentWidgetNode == "toggle")
      {
         do
         {
            C_PuiSvDbToggle c_Box;
            s32_Retval = C_PuiSvHandlerFiler::mh_LoadWidgetBase(c_Box, orc_XMLParser);
            if (orc_XMLParser.SelectNodeChild("type") == "type")
            {
               if (mh_StringToToggleType(orc_XMLParser.GetNodeContent().c_str(), c_Box.e_Type) != C_NO_ERR)
               {
                  s32_Retval = C_CONFIG;
               }
               //Return
               tgl_assert(orc_XMLParser.SelectNodeParent() == "toggle");
            }
            else
            {
               s32_Retval = C_CONFIG;
            }
            //Content
            if (orc_XMLParser.AttributeExists("state") == true)
            {
               c_Box.q_State = orc_XMLParser.GetAttributeBool("state");
            }
            else
            {
               c_Box.q_State = false;
            }
            //Add
            orc_Widgets.push_back(c_Box);
            //Next
            c_CurrentWidgetNode = orc_XMLParser.SelectNodeNext("toggle");
         }
         while ((c_CurrentWidgetNode == "toggle") && (s32_Retval == C_NO_ERR));
         //Return
         tgl_assert(orc_XMLParser.SelectNodeParent() == "toggles");
      }
      //Return
      orc_XMLParser.SelectNodeParent();
   }
   else
   {
      s32_Retval = C_CONFIG;
   }

   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Load widget element

   \param[in,out] orc_Widget    Widget element
   \param[in,out] orc_XMLParser XML parser with the "current" element set to the any widget element

   \return
   C_NO_ERR    information loaded
   C_CONFIG    error loading information
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_PuiSvHandlerFiler::mh_LoadWidgetBase(C_PuiSvDbWidgetBase & orc_Widget, C_OSCXMLParserBase & orc_XMLParser)
{
   sint32 s32_Retval = C_NO_ERR;

   if (orc_XMLParser.SelectNodeChild("base") == "base")
   {
      if (orc_XMLParser.SelectNodeChild("box") == "box")
      {
         C_PuiBsElementsFiler::h_LoadBoxBase(orc_Widget, orc_XMLParser);
         //Return
         tgl_assert(orc_XMLParser.SelectNodeParent() == "base");
      }
      else
      {
         s32_Retval = C_CONFIG;
      }
      if ((orc_XMLParser.SelectNodeChild("data-pool-elements") == "data-pool-elements") && (s32_Retval == C_NO_ERR))
      {
         C_SCLString c_CurrentDataPoolElementNode = orc_XMLParser.SelectNodeChild("data-pool-element");
         if (c_CurrentDataPoolElementNode == "data-pool-element")
         {
            do
            {
               C_PuiSvDbNodeDataElementConfig c_ElementConfig;
               mh_LoadUiIndex(c_ElementConfig.c_ElementId, orc_XMLParser);
               if (orc_XMLParser.SelectNodeChild("scaling") == "scaling")
               {
                  c_ElementConfig.c_ElementScaling.q_UseDefault = orc_XMLParser.GetAttributeBool("use-default");
                  c_ElementConfig.c_ElementScaling.f64_Factor = orc_XMLParser.GetAttributeFloat64("factor");
                  c_ElementConfig.c_ElementScaling.f64_Offset = orc_XMLParser.GetAttributeFloat64("offset");
                  if (orc_XMLParser.SelectNodeChild("unit") == "unit")
                  {
                     c_ElementConfig.c_ElementScaling.c_Unit = orc_XMLParser.GetNodeContent().c_str();
                     //Return
                     tgl_assert(orc_XMLParser.SelectNodeParent() == "scaling");
                  }
                  //Return
                  tgl_assert(orc_XMLParser.SelectNodeParent() == "data-pool-element");
               }
               else
               {
                  s32_Retval = C_CONFIG;
               }
               if (orc_XMLParser.SelectNodeChild("display-name") == "display-name")
               {
                  c_ElementConfig.c_DisplayName = orc_XMLParser.GetNodeContent().c_str();
                  //Return
                  tgl_assert(orc_XMLParser.SelectNodeParent() == "data-pool-element");
               }
               else
               {
                  c_ElementConfig.c_DisplayName = "";
               }

               orc_Widget.c_DataPoolElementsConfig.push_back(c_ElementConfig);
               //Next
               c_CurrentDataPoolElementNode = orc_XMLParser.SelectNodeNext("data-pool-element");
            }
            while ((c_CurrentDataPoolElementNode == "data-pool-element") && (s32_Retval == C_NO_ERR));
            //Return
            tgl_assert(orc_XMLParser.SelectNodeParent() == "data-pool-elements");
         }
         //Return
         tgl_assert(orc_XMLParser.SelectNodeParent() == "base");
      }
      else
      {
         s32_Retval = C_CONFIG;
      }
      if ((orc_XMLParser.SelectNodeChild("write-mode") == "write-mode") && (s32_Retval == C_NO_ERR))
      {
         if (C_PuiSvHandlerFiler::mh_StringToWriteMode(orc_XMLParser.GetNodeContent().c_str(),
                                                       orc_Widget.e_ElementWriteMode) != C_NO_ERR)
         {
            s32_Retval = C_CONFIG;
         }
         //Return
         tgl_assert(orc_XMLParser.SelectNodeParent() == "base");
      }
      else
      {
         s32_Retval = C_CONFIG;
      }
      if ((orc_XMLParser.SelectNodeChild("display-style") == "display-style") && (s32_Retval == C_NO_ERR))
      {
         if (C_PuiSvHandlerFiler::mh_StringToDisplayStyle(orc_XMLParser.GetNodeContent().c_str(),
                                                          orc_Widget.e_DisplayStyle) != C_NO_ERR)
         {
            s32_Retval = C_CONFIG;
         }
         //Return
         tgl_assert(orc_XMLParser.SelectNodeParent() == "base");
      }
      else
      {
         s32_Retval = C_CONFIG;
      }
      //Return
      orc_XMLParser.SelectNodeParent();
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
   \param[in,out] orc_XMLParser XML parser
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PuiSvHandlerFiler::mh_LoadUiIndex(C_PuiSvDbNodeDataPoolListElementId & orc_Id,
                                         C_OSCXMLParserBase & orc_XMLParser)
{
   const bool q_IsValid = orc_XMLParser.GetAttributeBool("is-valid");

   if (orc_XMLParser.SelectNodeChild("index") == "index")
   {
      const uint32 u32_NodeIndex = orc_XMLParser.GetAttributeUint32("node");
      const uint32 u32_DataPoolIndex = orc_XMLParser.GetAttributeUint32("data-pool");
      const uint32 u32_ListIndex = orc_XMLParser.GetAttributeUint32("list");
      const uint32 u32_ElementIndex = orc_XMLParser.GetAttributeUint32("element");
      const C_OSCNodeDataPoolListElementId c_Base(u32_NodeIndex, u32_DataPoolIndex, u32_ListIndex,
                                                  u32_ElementIndex);
      C_OSCNodeDataPool::E_Type e_InvalidTypePlaceholder = C_OSCNodeDataPool::eDIAG;
      QString c_InvalidNamePlaceholder = "";
      bool q_UseArrayElementIndex;
      uint32 u32_ArrayElementIndex;
      C_PuiSvDbNodeDataPoolListElementId::E_Type e_SourceType;

      //Return
      orc_XMLParser.SelectNodeParent();
      if (orc_XMLParser.SelectNodeChild("source-type") == "source-type")
      {
         C_PuiSvHandlerFiler::mh_StringToSourceType(orc_XMLParser.GetNodeContent().c_str(), e_SourceType);
         //Return
         orc_XMLParser.SelectNodeParent();
      }
      else
      {
         //Save default for previous projects
         e_SourceType = C_PuiSvDbNodeDataPoolListElementId::eDATAPOOL_ELEMENT;
      }
      if (orc_XMLParser.SelectNodeChild("invalid-type-placeholder") == "invalid-type-placeholder")
      {
         C_OSCNodeDataPoolFiler::h_StringToDataPool(orc_XMLParser.GetNodeContent(), e_InvalidTypePlaceholder);
         //Return
         orc_XMLParser.SelectNodeParent();
      }
      if (orc_XMLParser.SelectNodeChild("invalid-name-placeholder") == "invalid-name-placeholder")
      {
         c_InvalidNamePlaceholder = orc_XMLParser.GetNodeContent().c_str();
         //Return
         orc_XMLParser.SelectNodeParent();
      }
      if (orc_XMLParser.AttributeExists("array-element-index"))
      {
         u32_ArrayElementIndex = orc_XMLParser.GetAttributeUint32("array-element-index");
      }
      else
      {
         u32_ArrayElementIndex = 0UL;
      }
      if (orc_XMLParser.AttributeExists("use-array-element-index"))
      {
         q_UseArrayElementIndex = orc_XMLParser.GetAttributeBool("use-array-element-index");
      }
      else
      {
         q_UseArrayElementIndex = false;
      }
      orc_Id = C_PuiSvDbNodeDataPoolListElementId(c_Base, e_SourceType, q_UseArrayElementIndex, u32_ArrayElementIndex,
                                                  q_IsValid, e_InvalidTypePlaceholder, c_InvalidNamePlaceholder);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Load values

   \param[in,out] orc_Values    Values
   \param[in,out] orc_XMLParser XML parser with the "current" element set to the "param" element

   \return
   C_NO_ERR    information loaded
   C_CONFIG    error loading information
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_PuiSvHandlerFiler::mh_LoadParamDataSetIndices(std::vector<sint32> & orc_Values,
                                                       C_OSCXMLParserBase & orc_XMLParser)
{
   sint32 s32_Retval = C_NO_ERR;

   orc_Values.clear();
   if (orc_XMLParser.SelectNodeChild("data-set-selections") == "data-set-selections")
   {
      C_SCLString c_CurrentValueNode = orc_XMLParser.SelectNodeChild("data-set-selection");
      if (c_CurrentValueNode == "data-set-selection")
      {
         do
         {
            sint32 s32_Value;
            if (orc_XMLParser.AttributeExists("value") == true)
            {
               s32_Value = orc_XMLParser.GetAttributeSint32("value");
               orc_Values.push_back(s32_Value);
            }
            else
            {
               s32_Retval = C_CONFIG;
            }
            //Next
            c_CurrentValueNode = orc_XMLParser.SelectNodeNext("data-set-selection");
         }
         while ((c_CurrentValueNode == "data-set-selection") && (s32_Retval == C_NO_ERR));
         //Return
         tgl_assert(orc_XMLParser.SelectNodeParent() == "data-set-selections");
      }
      //Return
      tgl_assert(orc_XMLParser.SelectNodeParent() == "param");
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
   \param[in,out] orc_XMLParser XML parser with the "current" element set to the "param" element

   \return
   C_NO_ERR    information loaded
   C_CONFIG    error loading information
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_PuiSvHandlerFiler::mh_LoadParamValues(std::vector<C_OSCNodeDataPoolContent> & orc_Values,
                                               C_OSCXMLParserBase & orc_XMLParser)
{
   sint32 s32_Retval = C_NO_ERR;

   orc_Values.clear();
   if (orc_XMLParser.SelectNodeChild("values") == "values")
   {
      C_SCLString c_CurrentValueNode = orc_XMLParser.SelectNodeChild("value");
      if (c_CurrentValueNode == "value")
      {
         do
         {
            C_OSCNodeDataPoolContent c_Value;
            s32_Retval = C_OSCNodeDataPoolFiler::h_LoadDataPoolContentV1(c_Value, orc_XMLParser);
            orc_Values.push_back(c_Value);
            //Next
            c_CurrentValueNode = orc_XMLParser.SelectNodeNext("value");
         }
         while ((c_CurrentValueNode == "value") && (s32_Retval == C_NO_ERR));
         //Return
         tgl_assert(orc_XMLParser.SelectNodeParent() == "values");
      }
      //Return
      tgl_assert(orc_XMLParser.SelectNodeParent() == "param");
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
   \param[in,out] orc_XMLParser XML parser with the "current" element set to the "param" element

   \return
   C_NO_ERR    information loaded
   C_CONFIG    error loading information
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_PuiSvHandlerFiler::mh_LoadParamTables(std::vector<std::vector<sint32> > & orc_Values,
                                               C_OSCXMLParserBase & orc_XMLParser)
{
   sint32 s32_Retval = C_NO_ERR;

   orc_Values.clear();
   if (orc_XMLParser.SelectNodeChild("tables") == "tables")
   {
      C_SCLString c_CurrentValueNode = orc_XMLParser.SelectNodeChild("table");
      if (c_CurrentValueNode == "table")
      {
         do
         {
            std::vector<sint32> c_Table;
            s32_Retval = mh_LoadParamColumns(c_Table, orc_XMLParser);
            if (s32_Retval == C_NO_ERR)
            {
               orc_Values.push_back(c_Table);
            }
            //Next
            c_CurrentValueNode = orc_XMLParser.SelectNodeNext("table");
         }
         while ((c_CurrentValueNode == "table") && (s32_Retval == C_NO_ERR));
         //Return
         tgl_assert(orc_XMLParser.SelectNodeParent() == "tables");
      }
      //Return
      tgl_assert(orc_XMLParser.SelectNodeParent() == "param");
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
   \param[in,out] orc_XMLParser XML parser with the "current" element set to the "table" element

   \return
   C_NO_ERR    information loaded
   C_CONFIG    error loading information
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_PuiSvHandlerFiler::mh_LoadParamColumns(std::vector<sint32> & orc_Values, C_OSCXMLParserBase & orc_XMLParser)
{
   sint32 s32_Retval = C_NO_ERR;

   orc_Values.clear();
   if (orc_XMLParser.SelectNodeChild("columns") == "columns")
   {
      C_SCLString c_CurrentValueNode = orc_XMLParser.SelectNodeChild("column");
      if (c_CurrentValueNode == "column")
      {
         do
         {
            if (orc_XMLParser.AttributeExists("value") == true)
            {
               const sint32 s32_Value = orc_XMLParser.GetAttributeSint32("value");
               orc_Values.push_back(s32_Value);
            }
            else
            {
               s32_Retval = C_CONFIG;
            }
            //Next
            c_CurrentValueNode = orc_XMLParser.SelectNodeNext("column");
         }
         while ((c_CurrentValueNode == "column") && (s32_Retval == C_NO_ERR));
         //Return
         tgl_assert(orc_XMLParser.SelectNodeParent() == "columns");
      }
      //Return
      tgl_assert(orc_XMLParser.SelectNodeParent() == "table");
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
   \param[in,out] orc_XMLParser XML parser with the "current" element set to any element
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PuiSvHandlerFiler::mh_LoadDataElement(C_OSCNodeDataPoolListElementId & orc_Id,
                                             const C_OSCXMLParserBase & orc_XMLParser)
{
   orc_Id.u32_NodeIndex = orc_XMLParser.GetAttributeUint32("node");
   orc_Id.u32_DataPoolIndex = orc_XMLParser.GetAttributeUint32("data-pool");
   orc_Id.u32_ListIndex = orc_XMLParser.GetAttributeUint32("list");
   orc_Id.u32_ElementIndex = orc_XMLParser.GetAttributeUint32("element");
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Transform string to write mode

   \param[in]  orc_String String to interpret
   \param[out] ore_Mode   Write mode

   \return
   C_NO_ERR   no error
   C_RANGE    String unknown
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_PuiSvHandlerFiler::mh_StringToWriteMode(const QString & orc_String,
                                                 C_PuiSvDbWidgetBase::E_WriteMode & ore_Mode)
{
   sint32 s32_Retval = C_NO_ERR;

   if (orc_String.compare("manual") == 0)
   {
      ore_Mode = C_PuiSvDbWidgetBase::eWM_MANUAL;
   }
   else if (orc_String.compare("on-change") == 0)
   {
      ore_Mode = C_PuiSvDbWidgetBase::eWM_ON_CHANGE;
   }
   else
   {
      //Default
      ore_Mode = C_PuiSvDbWidgetBase::eWM_MANUAL;
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
sint32 C_PuiSvHandlerFiler::mh_StringToDisplayStyle(const QString & orc_String,
                                                    C_PuiSvDbWidgetBase::E_Style & ore_Style)
{
   sint32 s32_Retval = C_NO_ERR;

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
sint32 C_PuiSvHandlerFiler::mh_StringToToggleType(const QString & orc_String, C_PuiSvDbToggle::E_Type & ore_Type)
{
   sint32 s32_Retval = C_NO_ERR;

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
sint32 C_PuiSvHandlerFiler::mh_StringToSpinBoxType(const QString & orc_String, C_PuiSvDbSpinBox::E_Type & ore_Type)
{
   sint32 s32_Retval = C_NO_ERR;

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
sint32 C_PuiSvHandlerFiler::mh_StringToLabelType(const QString & orc_String, C_PuiSvDbLabel::E_Type & ore_Type)
{
   sint32 s32_Retval = C_NO_ERR;

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
sint32 C_PuiSvHandlerFiler::mh_StringToSliderType(const QString & orc_String, C_PuiSvDbSlider::E_Type & ore_Type)
{
   sint32 s32_Retval = C_NO_ERR;

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
sint32 C_PuiSvHandlerFiler::mh_StringToProgressBarType(const QString & orc_String,
                                                       C_PuiSvDbProgressBar::E_Type & ore_Type)
{
   sint32 s32_Retval = C_NO_ERR;

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
sint32 C_PuiSvHandlerFiler::mh_StringToProgressBarAlignmentType(const QString & orc_String,
                                                                C_PuiSvDbProgressBar::E_Alignment & ore_Alignment)
{
   sint32 s32_Retval = C_NO_ERR;

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
sint32 C_PuiSvHandlerFiler::mh_StringToTransmissionMode(const QString & orc_String,
                                                        C_PuiSvReadDataConfiguration::E_TransmissionMode & ore_Mode)
{
   sint32 s32_Retval = C_NO_ERR;

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
sint32 C_PuiSvHandlerFiler::mh_StringToSourceType(const QString & orc_String,
                                                  C_PuiSvDbNodeDataPoolListElementId::E_Type & ore_Type)
{
   sint32 s32_Retval = C_NO_ERR;

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
/*! \brief   Transform string to source type

   \param[in]  orc_String String to interpret
   \param[out] ore_Type   Source type

   \return
   C_NO_ERR   no error
   C_RANGE    String unknown
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_PuiSvHandlerFiler::mh_StringToChartSettingZoomMode(const QString & orc_String,
                                                            C_PuiSvDbChart::E_SettingZoomMode & ore_ZoomMode)
{
   sint32 s32_Retval = C_NO_ERR;

   if (orc_String.compare("setting_zm_xy") == 0)
   {
      ore_ZoomMode = C_PuiSvDbChart::eSETTING_ZM_XY;
   }
   else if (orc_String.compare("setting_zm_x") == 0)
   {
      ore_ZoomMode = C_PuiSvDbChart::eSETTING_ZM_X;
   }
   else if (orc_String.compare("setting_zm_y") == 0)
   {
      ore_ZoomMode = C_PuiSvDbChart::eSETTING_ZM_Y;
   }
   else
   {
      //Default
      ore_ZoomMode = C_PuiSvDbChart::eSETTING_ZM_XY;
      s32_Retval = C_RANGE;
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Save node active flags

   \param[in]     orc_NodeActiveFlags Node active flags
   \param[in,out] orc_XMLParser       XML parser with the "current" element set to the "opensyde-system-view" element
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PuiSvHandlerFiler::mh_SaveNodeActiveFlags(const std::vector<uint8> & orc_NodeActiveFlags,
                                                 C_OSCXMLParserBase & orc_XMLParser)
{
   orc_XMLParser.CreateAndSelectNodeChild("active-nodes");
   for (uint32 u32_ItNodeActiveFlag = 0; u32_ItNodeActiveFlag < orc_NodeActiveFlags.size(); ++u32_ItNodeActiveFlag)
   {
      orc_XMLParser.CreateAndSelectNodeChild("active-node");
      orc_XMLParser.SetAttributeBool("state", static_cast<bool>(orc_NodeActiveFlags[u32_ItNodeActiveFlag]));
      //Return
      tgl_assert(orc_XMLParser.SelectNodeParent() == "active-nodes");
   }
   //Return
   tgl_assert(orc_XMLParser.SelectNodeParent() == "opensyde-system-view");
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Save node update information

   \param[in]     orc_NodeUpdateInformation Node update information
   \param[in,out] orc_XMLParser             XML parser with the "current" element set
                                            to the "opensyde-system-view" element
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PuiSvHandlerFiler::mh_SaveNodeUpdateInformation(const std::vector<C_PuiSvNodeUpdate> & orc_NodeUpdateInformation,
                                                       C_OSCXMLParserBase & orc_XMLParser)
{
   orc_XMLParser.CreateAndSelectNodeChild("node-update-information");
   for (uint32 u32_ItNodeActiveFlag = 0; u32_ItNodeActiveFlag < orc_NodeUpdateInformation.size();
        ++u32_ItNodeActiveFlag)
   {
      const C_PuiSvNodeUpdate & rc_NodeUpdateInformation = orc_NodeUpdateInformation[u32_ItNodeActiveFlag];
      const std::vector<QString> & rc_DataBlockPaths = rc_NodeUpdateInformation.GetPaths(
         C_PuiSvNodeUpdate::eFTP_DATA_BLOCK);
      const std::vector<C_PuiSvNodeUpdateParamInfo> & rc_ParamSetPaths = rc_NodeUpdateInformation.GetParamInfos();
      const std::vector<QString> & rc_FileBasedPaths = rc_NodeUpdateInformation.GetPaths(
         C_PuiSvNodeUpdate::eFTP_FILE_BASED);
      orc_XMLParser.CreateAndSelectNodeChild("node-specific-update-information");
      orc_XMLParser.SetAttributeUint32("position", rc_NodeUpdateInformation.u32_NodeUpdatePosition);

      C_PuiSvHandlerFiler::mh_SaveNodeUpdateInformationPaths(rc_DataBlockPaths, "data-block-path", orc_XMLParser);

      mh_SaveNodeUpdateInformationParamInfo(rc_ParamSetPaths, orc_XMLParser);

      C_PuiSvHandlerFiler::mh_SaveNodeUpdateInformationPaths(rc_FileBasedPaths, "file-based-path", orc_XMLParser);

      //Return
      tgl_assert(orc_XMLParser.SelectNodeParent() == "node-update-information");
   }
   //Return
   tgl_assert(orc_XMLParser.SelectNodeParent() == "opensyde-system-view");
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Save node update path information

   \param[in]     orc_Paths          Paths to save
   \param[in]     orc_XMLTagBaseName XML tag to use for saving
   \param[in,out] orc_XMLParser      XML parser with the "current" element set
                                     to the "node-specific-update-information" element
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PuiSvHandlerFiler::mh_SaveNodeUpdateInformationPaths(const std::vector<QString> & orc_Paths,
                                                            const QString & orc_XMLTagBaseName,
                                                            C_OSCXMLParserBase & orc_XMLParser)
{
   const C_SCLString c_ParentName = QString(orc_XMLTagBaseName + "s").toStdString().c_str();
   const C_SCLString c_ChildName = orc_XMLTagBaseName.toStdString().c_str();

   orc_XMLParser.CreateAndSelectNodeChild(c_ParentName);
   for (uint32 u32_ItPath = 0; u32_ItPath < orc_Paths.size(); ++u32_ItPath)
   {
      const QString & rc_Path = orc_Paths[u32_ItPath];
      orc_XMLParser.CreateNodeChild(c_ChildName, rc_Path.toStdString().c_str());
   }
   //Return
   tgl_assert(orc_XMLParser.SelectNodeParent() == "node-specific-update-information");
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Save update information for parameter sets

   \param[in]     orc_Info       Update information for parameter sets
   \param[in,out] orc_XMLParser  XML parser with the "current" element set
                                 to the "node-specific-update-information" element
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PuiSvHandlerFiler::mh_SaveNodeUpdateInformationParamInfo(
   const std::vector<C_PuiSvNodeUpdateParamInfo> & orc_Info, C_OSCXMLParserBase & orc_XMLParser)
{
   orc_XMLParser.CreateAndSelectNodeChild("param-sets");
   for (uint32 u32_ItParamSet = 0UL; u32_ItParamSet < orc_Info.size(); ++u32_ItParamSet)
   {
      const C_PuiSvNodeUpdateParamInfo & rc_ParamSetInfo = orc_Info[u32_ItParamSet];
      orc_XMLParser.CreateAndSelectNodeChild("param-set");
      orc_XMLParser.SetAttributeUint32("last-known-crc", rc_ParamSetInfo.GetLastKnownCrc());
      orc_XMLParser.CreateNodeChild("path", rc_ParamSetInfo.GetPath().toStdString().c_str());
      //Return
      tgl_assert(orc_XMLParser.SelectNodeParent() == "param-sets");
   }
   //Return
   tgl_assert(orc_XMLParser.SelectNodeParent() == "node-specific-update-information");
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Save dashboard elements

   \param[in]     orc_Dashboards Dashboard elements
   \param[in,out] orc_XMLParser  XML parser with the "current" element set to the "opensyde-system-view" element
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PuiSvHandlerFiler::mh_SaveDashboards(const std::vector<C_PuiSvDashboard> & orc_Dashboards,
                                            C_OSCXMLParserBase & orc_XMLParser)
{
   orc_XMLParser.CreateAndSelectNodeChild("dashboards");
   for (uint32 u32_ItDashboard = 0; u32_ItDashboard < orc_Dashboards.size(); ++u32_ItDashboard)
   {
      orc_XMLParser.CreateAndSelectNodeChild("dashboard");
      h_SaveDashboard(orc_Dashboards[u32_ItDashboard], orc_XMLParser);
      //Return
      tgl_assert(orc_XMLParser.SelectNodeParent() == "dashboards");
   }
   //Return
   tgl_assert(orc_XMLParser.SelectNodeParent() == "opensyde-system-view");
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Save view

   Save view to XML file

   \param[in]     orc_View     View data storage
   \param[in,out] orc_FilePath File path for xml

   \return
   C_NO_ERR   data saved
   C_CONFIG   file could not be created
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_PuiSvHandlerFiler::mh_SaveViewFile(const C_PuiSvData & orc_View, const QString & orc_FilePath)
{
   C_OSCXMLParser c_XMLParser;
   sint32 s32_Retval = C_OSCSystemFilerUtil::h_GetParserForNewFile(c_XMLParser, orc_FilePath.toStdString().c_str(),
                                                                   "opensyde-view-definition");

   if (s32_Retval == C_NO_ERR)
   {
      //Version
      c_XMLParser.CreateNodeChild("file-version", "1");
      tgl_assert(c_XMLParser.CreateAndSelectNodeChild("opensyde-system-view") == "opensyde-system-view");
      //node
      C_PuiSvHandlerFiler::mh_SaveView(orc_View, c_XMLParser);
      //Don't forget to save!
      if (c_XMLParser.SaveToFile(orc_FilePath.toStdString().c_str()) != C_NO_ERR)
      {
         osc_write_log_error("Saving system definition UI", "Could not create file for node.");
         s32_Retval = C_CONFIG;
      }
   }
   else
   {
      //More details are in log
      s32_Retval = C_CONFIG;
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Save view element

   \param[in]     orc_View      View element
   \param[in,out] orc_XMLParser XML parser with the "current" element set to the "opensyde-system-view" element
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PuiSvHandlerFiler::mh_SaveView(const C_PuiSvData & orc_View, C_OSCXMLParserBase & orc_XMLParser)
{
   orc_XMLParser.SetAttributeBool("darkmode", orc_View.GetDarkModeActive());
   orc_XMLParser.SetAttributeUint32("device-config-selected-bit-rate", orc_View.GetDeviceConfigSelectedBitRate());
   orc_XMLParser.CreateAndSelectNodeChild("name");
   orc_XMLParser.SetNodeContent(orc_View.GetName().toStdString().c_str());
   //Return
   tgl_assert(orc_XMLParser.SelectNodeParent() == "opensyde-system-view");
   orc_XMLParser.CreateNodeChild("device-config-mode", mh_DeviceConfigModeToString(
                                    orc_View.GetDeviceConfigMode()).toStdString().c_str());
   orc_XMLParser.CreateAndSelectNodeChild("update-rates");
   orc_XMLParser.SetAttributeUint32("fast", static_cast<uint32>(orc_View.GetUpdateRateFast()));
   orc_XMLParser.SetAttributeUint32("medium", static_cast<uint32>(orc_View.GetUpdateRateMedium()));
   orc_XMLParser.SetAttributeUint32("slow", static_cast<uint32>(orc_View.GetUpdateRateSlow()));
   //Return
   tgl_assert(orc_XMLParser.SelectNodeParent() == "opensyde-system-view");
   mh_SaveNodeActiveFlags(orc_View.GetNodeActiveFlags(), orc_XMLParser);
   mh_SaveNodeUpdateInformation(orc_View.GetAllNodeUpdateInformation(), orc_XMLParser);
   orc_XMLParser.CreateAndSelectNodeChild("pc");
   mh_SavePc(orc_View.GetPcData(), orc_XMLParser);
   //Return
   tgl_assert(orc_XMLParser.SelectNodeParent() == "opensyde-system-view");
   mh_SaveDashboards(orc_View.GetDashboards(), orc_XMLParser);

   //Rails
   orc_XMLParser.CreateAndSelectNodeChild("rail-assignments");
   h_SaveReadRails(orc_View.GetReadRailAssignments(), orc_XMLParser);
   //Return
   tgl_assert(orc_XMLParser.SelectNodeParent() == "opensyde-system-view");
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Save PC element

   \param[in]     orc_Pc        PC element
   \param[in,out] orc_XMLParser XML parser with the "current" element set to the "pc" element
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PuiSvHandlerFiler::mh_SavePc(const C_PuiSvPc & orc_Pc, C_OSCXMLParserBase & orc_XMLParser)
{
   orc_XMLParser.SetAttributeBool("connected", orc_Pc.GetConnected());
   orc_XMLParser.SetAttributeUint32("bus-index", orc_Pc.GetBusIndex());

   orc_XMLParser.CreateAndSelectNodeChild("dll-path");
   orc_XMLParser.SetAttributeSint32("type", static_cast<sint32>(orc_Pc.GetCANDllType()));
   orc_XMLParser.SetNodeContent(orc_Pc.GetCustomCANDllPath().toStdString().c_str());
   //Return
   tgl_assert(orc_XMLParser.SelectNodeParent() == "pc");
   orc_XMLParser.CreateAndSelectNodeChild("box");
   C_PuiBsElementsFiler::h_SaveBoxBase(orc_Pc, orc_XMLParser);
   //Return
   tgl_assert(orc_XMLParser.SelectNodeParent() == "pc");
   orc_XMLParser.CreateAndSelectNodeChild("connection");
   orc_XMLParser.CreateAndSelectNodeChild("line");
   C_PuiBsElementsFiler::h_SaveLineBase(orc_Pc.GetConnectionData(), orc_XMLParser);
   //Return
   tgl_assert(orc_XMLParser.SelectNodeParent() == "connection");
   //Return
   tgl_assert(orc_XMLParser.SelectNodeParent() == "pc");
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Save widget elements

   \param[in]     orc_Widgets   Widget elements
   \param[in,out] orc_XMLParser XML parser with the "current" element set to the "dashboard" element
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PuiSvHandlerFiler::mh_SaveCharts(const std::vector<C_PuiSvDbChart> & orc_Widgets,
                                        C_OSCXMLParserBase & orc_XMLParser)
{
   orc_XMLParser.CreateAndSelectNodeChild("charts");
   for (uint32 u32_ItWidget = 0; u32_ItWidget < orc_Widgets.size(); ++u32_ItWidget)
   {
      const C_PuiSvDbChart & rc_Chart = orc_Widgets[u32_ItWidget];
      orc_XMLParser.CreateAndSelectNodeChild("chart");
      C_PuiSvHandlerFiler::mh_SaveWidgetBase(rc_Chart, orc_XMLParser);
      orc_XMLParser.CreateAndSelectNodeChild("active-flags");
      for (uint32 u32_ItActive = 0; u32_ItActive < rc_Chart.c_DataPoolElementsActive.size(); ++u32_ItActive)
      {
         orc_XMLParser.CreateAndSelectNodeChild("active-flag");
         orc_XMLParser.SetAttributeBool("state", rc_Chart.c_DataPoolElementsActive[u32_ItActive]);
         //Return
         tgl_assert(orc_XMLParser.SelectNodeParent() == "active-flags");
      }

      //Return
      tgl_assert(orc_XMLParser.SelectNodeParent() == "chart");

      orc_XMLParser.CreateNodeChild("zoom-mode",
                                    C_PuiSvHandlerFiler::mh_ChartZoomModeToString(
                                       rc_Chart.e_SettingZoomMode).toStdString().c_str());

      //Return
      tgl_assert(orc_XMLParser.SelectNodeParent() == "charts");
   }
   //Return
   orc_XMLParser.SelectNodeParent();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Save widget elements

   \param[in]     orc_Widgets   Widget elements
   \param[in,out] orc_XMLParser XML parser with the "current" element set to the "dashboard" element
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PuiSvHandlerFiler::mh_SaveLabels(const std::vector<C_PuiSvDbLabel> & orc_Widgets,
                                        C_OSCXMLParserBase & orc_XMLParser)
{
   orc_XMLParser.CreateAndSelectNodeChild("labels");
   for (uint32 u32_ItWidget = 0; u32_ItWidget < orc_Widgets.size(); ++u32_ItWidget)
   {
      const C_PuiSvDbLabel & rc_Label = orc_Widgets[u32_ItWidget];
      orc_XMLParser.CreateAndSelectNodeChild("label");
      orc_XMLParser.SetAttributeBool("show-caption", rc_Label.q_ShowCaption);
      orc_XMLParser.SetAttributeBool("show-unit", rc_Label.q_ShowUnit);
      C_PuiSvHandlerFiler::mh_SaveWidgetBase(rc_Label, orc_XMLParser);
      orc_XMLParser.CreateNodeChild("caption", rc_Label.c_Caption.toStdString().c_str());
      orc_XMLParser.CreateNodeChild("type", mh_LabelTypeToString(rc_Label.e_Type).toStdString().c_str());
      //Return
      tgl_assert(orc_XMLParser.SelectNodeParent() == "labels");
   }
   //Return
   orc_XMLParser.SelectNodeParent();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Save widget elements

   \param[in]     orc_Widgets   Widget elements
   \param[in,out] orc_XMLParser XML parser with the "current" element set to the "dashboard" element
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PuiSvHandlerFiler::mh_SaveParams(const std::vector<C_PuiSvDbParam> & orc_Widgets,
                                        C_OSCXMLParserBase & orc_XMLParser)
{
   orc_XMLParser.CreateAndSelectNodeChild("params");
   for (uint32 u32_ItWidget = 0; u32_ItWidget < orc_Widgets.size(); ++u32_ItWidget)
   {
      const C_PuiSvDbParam & rc_Param = orc_Widgets[u32_ItWidget];
      orc_XMLParser.CreateAndSelectNodeChild("param");
      C_PuiSvHandlerFiler::mh_SaveWidgetBase(rc_Param, orc_XMLParser);
      //Data set indices
      orc_XMLParser.CreateAndSelectNodeChild("data-set-selections");
      for (uint32 u32_ItDataSet = 0; u32_ItDataSet < rc_Param.c_DataSetSelectionIndices.size(); ++u32_ItDataSet)
      {
         orc_XMLParser.CreateAndSelectNodeChild("data-set-selection");
         orc_XMLParser.SetAttributeSint32("value", rc_Param.c_DataSetSelectionIndices[u32_ItDataSet]);
         //Return
         tgl_assert(orc_XMLParser.SelectNodeParent() == "data-set-selections");
      }
      //Return
      tgl_assert(orc_XMLParser.SelectNodeParent() == "param");
      //Values
      orc_XMLParser.CreateAndSelectNodeChild("values");
      for (uint32 u32_ItValue = 0; u32_ItValue < rc_Param.c_ListValues.size(); ++u32_ItValue)
      {
         orc_XMLParser.CreateAndSelectNodeChild("value");
         C_OSCNodeDataPoolFiler::h_SaveDataPoolContentV1(rc_Param.c_ListValues[u32_ItValue], orc_XMLParser);
         //Return
         tgl_assert(orc_XMLParser.SelectNodeParent() == "values");
      }
      //Return
      tgl_assert(orc_XMLParser.SelectNodeParent() == "param");
      //Columns
      orc_XMLParser.CreateAndSelectNodeChild("tables");
      for (uint32 u32_ItTable = 0; u32_ItTable < rc_Param.c_ColWidth.size(); ++u32_ItTable)
      {
         const std::vector<sint32> & rc_Cols = rc_Param.c_ColWidth[u32_ItTable];
         orc_XMLParser.CreateAndSelectNodeChild("table");
         orc_XMLParser.CreateAndSelectNodeChild("columns");
         for (uint32 u32_ItColumn = 0; u32_ItColumn < rc_Cols.size(); ++u32_ItColumn)
         {
            orc_XMLParser.CreateAndSelectNodeChild("column");
            orc_XMLParser.SetAttributeSint32("value", rc_Cols[u32_ItColumn]);
            //Return
            tgl_assert(orc_XMLParser.SelectNodeParent() == "columns");
         }
         //Return
         tgl_assert(orc_XMLParser.SelectNodeParent() == "table");
         //Return
         tgl_assert(orc_XMLParser.SelectNodeParent() == "tables");
      }
      //Return
      tgl_assert(orc_XMLParser.SelectNodeParent() == "param");
      mh_SaveParamExpandedItems(rc_Param.c_ExpandedItems, orc_XMLParser);
      mh_SaveParamColumnPositionIndices(rc_Param.c_ColPosIndices, orc_XMLParser);
      //Return
      tgl_assert(orc_XMLParser.SelectNodeParent() == "params");
   }
   //Return
   orc_XMLParser.SelectNodeParent();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Save tree elements

   \param[in]     orc_Items     Tree elements
   \param[in,out] orc_XMLParser XML parser with the "current" element set to the "param" element
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PuiSvHandlerFiler::mh_SaveParamExpandedItems(const std::vector<C_PuiSvDbExpandedTreeIndex> & orc_Items,
                                                    C_OSCXMLParserBase & orc_XMLParser)
{
   //Columns
   orc_XMLParser.CreateAndSelectNodeChild("expanded-tree-items");
   for (uint32 u32_ItTable = 0; u32_ItTable < orc_Items.size(); ++u32_ItTable)
   {
      const C_PuiSvDbExpandedTreeIndex & rc_TreeElement = orc_Items[u32_ItTable];
      orc_XMLParser.CreateAndSelectNodeChild("expanded-tree-item");
      orc_XMLParser.SetAttributeUint32("valid-layers", rc_TreeElement.u32_Layer);
      mh_SaveUiIndex(rc_TreeElement.c_ExpandedId, orc_XMLParser);
      //Return
      tgl_assert(orc_XMLParser.SelectNodeParent() == "expanded-tree-items");
   }
   //Return
   tgl_assert(orc_XMLParser.SelectNodeParent() == "param");
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Save column position indices elements

   \param[in]     orc_Items     Column position indices elements
   \param[in,out] orc_XMLParser XML parser with the "current" element set to the "param" element
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PuiSvHandlerFiler::mh_SaveParamColumnPositionIndices(const std::vector<sint32> & orc_Items,
                                                            C_OSCXMLParserBase & orc_XMLParser)
{
   orc_XMLParser.CreateAndSelectNodeChild("column-position-indices");
   for (uint32 u32_ItTable = 0; u32_ItTable < orc_Items.size(); ++u32_ItTable)
   {
      orc_XMLParser.CreateAndSelectNodeChild("column-position-index");
      orc_XMLParser.SetAttributeSint32("value", orc_Items[u32_ItTable]);
      //Return
      tgl_assert(orc_XMLParser.SelectNodeParent() == "column-position-indices");
   }
   //Return
   tgl_assert(orc_XMLParser.SelectNodeParent() == "param");
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Save widget elements

   \param[in]     orc_Widgets   Widget elements
   \param[in,out] orc_XMLParser XML parser with the "current" element set to the "dashboard" element
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PuiSvHandlerFiler::mh_SavePieCharts(const std::vector<C_PuiSvDbPieChart> & orc_Widgets,
                                           C_OSCXMLParserBase & orc_XMLParser)
{
   orc_XMLParser.CreateAndSelectNodeChild("pie-charts");
   for (uint32 u32_ItWidget = 0; u32_ItWidget < orc_Widgets.size(); ++u32_ItWidget)
   {
      const C_PuiSvDbPieChart & rc_PieChart = orc_Widgets[u32_ItWidget];
      orc_XMLParser.CreateAndSelectNodeChild("pie-chart");
      orc_XMLParser.SetAttributeBool("show-value", rc_PieChart.q_ShowValue);
      orc_XMLParser.SetAttributeBool("show-unit", rc_PieChart.q_ShowUnit);

      C_PuiSvHandlerFiler::mh_SaveWidgetBase(rc_PieChart, orc_XMLParser);
      //Return
      tgl_assert(orc_XMLParser.SelectNodeParent() == "pie-charts");
   }
   //Return
   orc_XMLParser.SelectNodeParent();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Save widget elements

   \param[in]     orc_Widgets   Widget elements
   \param[in,out] orc_XMLParser XML parser with the "current" element set to the "dashboard" element
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PuiSvHandlerFiler::mh_SaveSpinBoxes(const std::vector<C_PuiSvDbSpinBox> & orc_Widgets,
                                           C_OSCXMLParserBase & orc_XMLParser)
{
   orc_XMLParser.CreateAndSelectNodeChild("spin-boxes");
   for (uint32 u32_ItWidget = 0; u32_ItWidget < orc_Widgets.size(); ++u32_ItWidget)
   {
      const C_PuiSvDbSpinBox & rc_SpinBox = orc_Widgets[u32_ItWidget];
      orc_XMLParser.CreateAndSelectNodeChild("spin-box");

      orc_XMLParser.SetAttributeBool("show-unit", rc_SpinBox.q_ShowUnit);

      C_PuiSvHandlerFiler::mh_SaveWidgetBase(rc_SpinBox, orc_XMLParser);

      orc_XMLParser.CreateNodeChild("type", C_PuiSvHandlerFiler::mh_SpinBoxTypeToString(
                                       rc_SpinBox.e_Type).toStdString().c_str());
      orc_XMLParser.CreateAndSelectNodeChild("content");
      C_OSCNodeDataPoolFiler::h_SaveDataPoolContentV1(rc_SpinBox.c_Value, orc_XMLParser);
      //Return
      tgl_assert(orc_XMLParser.SelectNodeParent() == "spin-box");
      //Return
      tgl_assert(orc_XMLParser.SelectNodeParent() == "spin-boxes");
   }
   //Return
   orc_XMLParser.SelectNodeParent();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Save widget elements

   \param[in]     orc_Widgets   Widget elements
   \param[in,out] orc_XMLParser XML parser with the "current" element set to the "dashboard" element
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PuiSvHandlerFiler::mh_SaveTables(const std::vector<C_PuiSvDbTable> & orc_Widgets,
                                        C_OSCXMLParserBase & orc_XMLParser)
{
   orc_XMLParser.CreateAndSelectNodeChild("tables");
   for (uint32 u32_ItWidget = 0; u32_ItWidget < orc_Widgets.size(); ++u32_ItWidget)
   {
      const C_PuiSvDbTable & rc_Table = orc_Widgets[u32_ItWidget];
      orc_XMLParser.CreateAndSelectNodeChild("table");

      C_PuiSvHandlerFiler::mh_SaveWidgetBase(rc_Table, orc_XMLParser);

      //Columns
      orc_XMLParser.CreateAndSelectNodeChild("columns");
      for (uint32 u32_ItColumn = 0; u32_ItColumn < rc_Table.c_ColumnWidth.size(); ++u32_ItColumn)
      {
         orc_XMLParser.CreateAndSelectNodeChild("column");
         orc_XMLParser.SetAttributeSint32("width", rc_Table.c_ColumnWidth[u32_ItColumn]);
         //Return
         tgl_assert(orc_XMLParser.SelectNodeParent() == "columns");
      }
      //Return
      tgl_assert(orc_XMLParser.SelectNodeParent() == "table");

      //Return
      tgl_assert(orc_XMLParser.SelectNodeParent() == "tables");
   }
   //Return
   orc_XMLParser.SelectNodeParent();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Save widget elements

   \param[in]     orc_Widgets   Widget elements
   \param[in,out] orc_XMLParser XML parser with the "current" element set to the "dashboard" element
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PuiSvHandlerFiler::mh_SaveSliders(const std::vector<C_PuiSvDbSlider> & orc_Widgets,
                                         C_OSCXMLParserBase & orc_XMLParser)
{
   orc_XMLParser.CreateAndSelectNodeChild("sliders");
   for (uint32 u32_ItWidget = 0; u32_ItWidget < orc_Widgets.size(); ++u32_ItWidget)
   {
      const C_PuiSvDbSlider & rc_Slider = orc_Widgets[u32_ItWidget];
      orc_XMLParser.CreateAndSelectNodeChild("slider");
      C_PuiSvHandlerFiler::mh_SaveWidgetBase(rc_Slider, orc_XMLParser);
      orc_XMLParser.CreateNodeChild("type", C_PuiSvHandlerFiler::mh_SliderTypeToString(
                                       rc_Slider.e_Type).toStdString().c_str());
      orc_XMLParser.SetAttributeBool("show-min-max", rc_Slider.q_ShowMinMax);
      orc_XMLParser.SetAttributeSint32("value", rc_Slider.s32_Value);
      //Return
      tgl_assert(orc_XMLParser.SelectNodeParent() == "sliders");
   }
   //Return
   orc_XMLParser.SelectNodeParent();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Save widget elements

   \param[in]     orc_Widgets   Widget elements
   \param[in,out] orc_XMLParser XML parser with the "current" element set to the "dashboard" element
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PuiSvHandlerFiler::mh_SaveProgressBars(const std::vector<C_PuiSvDbProgressBar> & orc_Widgets,
                                              C_OSCXMLParserBase & orc_XMLParser)
{
   orc_XMLParser.CreateAndSelectNodeChild("progress-bars");
   for (uint32 u32_ItWidget = 0; u32_ItWidget < orc_Widgets.size(); ++u32_ItWidget)
   {
      const C_PuiSvDbProgressBar & rc_ProgressBar = orc_Widgets[u32_ItWidget];
      orc_XMLParser.CreateAndSelectNodeChild("progress-bar");
      C_PuiSvHandlerFiler::mh_SaveWidgetBase(rc_ProgressBar, orc_XMLParser);
      orc_XMLParser.CreateNodeChild("type", C_PuiSvHandlerFiler::mh_ProgressBarTypeToString(
                                       rc_ProgressBar.e_Type).toStdString().c_str());
      orc_XMLParser.CreateNodeChild("alignment",
                                    C_PuiSvHandlerFiler::mh_ProgressBarAlignmentTypeToString(
                                       rc_ProgressBar.e_Alignment).toStdString().c_str());
      orc_XMLParser.SetAttributeBool("show-min-max", rc_ProgressBar.q_ShowMinMax);
      orc_XMLParser.SetAttributeBool("show-unit", rc_ProgressBar.q_ShowUnit);
      //Return
      tgl_assert(orc_XMLParser.SelectNodeParent() == "progress-bars");
   }
   //Return
   orc_XMLParser.SelectNodeParent();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Save widget elements

   \param[in]     orc_Widgets   Widget elements
   \param[in,out] orc_XMLParser XML parser with the "current" element set to the "dashboard" element
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PuiSvHandlerFiler::mh_SaveToggles(const std::vector<C_PuiSvDbToggle> & orc_Widgets,
                                         C_OSCXMLParserBase & orc_XMLParser)
{
   orc_XMLParser.CreateAndSelectNodeChild("toggles");
   for (uint32 u32_ItWidget = 0; u32_ItWidget < orc_Widgets.size(); ++u32_ItWidget)
   {
      const C_PuiSvDbToggle & rc_Toggle = orc_Widgets[u32_ItWidget];
      orc_XMLParser.CreateAndSelectNodeChild("toggle");

      C_PuiSvHandlerFiler::mh_SaveWidgetBase(rc_Toggle, orc_XMLParser);

      orc_XMLParser.CreateNodeChild("type", C_PuiSvHandlerFiler::mh_ToggleTypeToString(
                                       rc_Toggle.e_Type).toStdString().c_str());

      //Content
      orc_XMLParser.SetAttributeBool("state", rc_Toggle.q_State);

      //Return
      tgl_assert(orc_XMLParser.SelectNodeParent() == "toggles");
   }
   //Return
   orc_XMLParser.SelectNodeParent();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Save widget element

   \param[in]     orc_Widget    Widget element
   \param[in,out] orc_XMLParser XML parser with the "current" element set to the any widget element
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PuiSvHandlerFiler::mh_SaveWidgetBase(const C_PuiSvDbWidgetBase & orc_Widget, C_OSCXMLParserBase & orc_XMLParser)
{
   orc_XMLParser.CreateAndSelectNodeChild("base");
   orc_XMLParser.CreateAndSelectNodeChild("box");
   C_PuiBsElementsFiler::h_SaveBoxBase(orc_Widget, orc_XMLParser);
   //Return
   tgl_assert(orc_XMLParser.SelectNodeParent() == "base");
   orc_XMLParser.CreateAndSelectNodeChild("data-pool-elements");

   for (uint32 u32_ItDataElement = 0;
        u32_ItDataElement < orc_Widget.c_DataPoolElementsConfig.size();
        ++u32_ItDataElement)
   {
      const C_PuiSvDbNodeDataElementConfig & rc_Config = orc_Widget.c_DataPoolElementsConfig[u32_ItDataElement];
      const C_PuiSvDbDataElementScaling & rc_Scaling = rc_Config.c_ElementScaling;
      orc_XMLParser.CreateAndSelectNodeChild("data-pool-element");
      mh_SaveUiIndex(rc_Config.c_ElementId, orc_XMLParser);
      //Scaling
      orc_XMLParser.CreateAndSelectNodeChild("scaling");
      orc_XMLParser.SetAttributeBool("use-default", rc_Scaling.q_UseDefault);
      orc_XMLParser.SetAttributeFloat64("factor", rc_Scaling.f64_Factor);
      orc_XMLParser.SetAttributeFloat64("offset", rc_Scaling.f64_Offset);
      orc_XMLParser.CreateNodeChild("unit", rc_Scaling.c_Unit.toStdString().c_str());
      //Return
      tgl_assert(orc_XMLParser.SelectNodeParent() == "data-pool-element");
      orc_XMLParser.CreateNodeChild("display-name", rc_Config.c_DisplayName.toStdString().c_str());
      //Return
      tgl_assert(orc_XMLParser.SelectNodeParent() == "data-pool-elements");
   }

   //Return
   tgl_assert(orc_XMLParser.SelectNodeParent() == "base");

   orc_XMLParser.CreateNodeChild("write-mode", C_PuiSvHandlerFiler::mh_WriteModeToString(
                                    orc_Widget.e_ElementWriteMode).toStdString().c_str());
   orc_XMLParser.CreateNodeChild("display-style", C_PuiSvHandlerFiler::mh_DisplayStyleToString(
                                    orc_Widget.e_DisplayStyle).toStdString().c_str());
   //Return
   orc_XMLParser.SelectNodeParent();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Save UI index to CURRENT node

   \param[in]     orc_Id        ID to store
   \param[in,out] orc_XMLParser XML parser
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PuiSvHandlerFiler::mh_SaveUiIndex(const C_PuiSvDbNodeDataPoolListElementId & orc_Id,
                                         C_OSCXMLParserBase & orc_XMLParser)
{
   orc_XMLParser.SetAttributeBool("is-valid", orc_Id.GetIsValid());
   orc_XMLParser.SetAttributeBool("use-array-element-index", orc_Id.GetUseArrayElementIndex());
   orc_XMLParser.SetAttributeUint32("array-element-index", orc_Id.GetArrayElementIndex());
   orc_XMLParser.CreateAndSelectNodeChild("index");
   orc_XMLParser.SetAttributeUint32("node", orc_Id.u32_NodeIndex);
   orc_XMLParser.SetAttributeUint32("data-pool", orc_Id.u32_DataPoolIndex);
   orc_XMLParser.SetAttributeUint32("list", orc_Id.u32_ListIndex);
   orc_XMLParser.SetAttributeUint32("element", orc_Id.u32_ElementIndex);
   //Return
   orc_XMLParser.SelectNodeParent();
   orc_XMLParser.CreateNodeChild("source-type",
                                 C_PuiSvHandlerFiler::mh_SourceTypeToString(orc_Id.GetType()).toStdString().c_str());
   orc_XMLParser.CreateNodeChild("invalid-type-placeholder",
                                 C_OSCNodeDataPoolFiler::h_DataPoolToString(orc_Id.GetInvalidTypePlaceholder()));
   orc_XMLParser.CreateNodeChild("invalid-name-placeholder", orc_Id.GetInvalidNamePlaceholder().toStdString().c_str());
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Save ID

   \param[in]     orc_Id        ID
   \param[in,out] orc_XMLParser XML parser with the "current" element set to the any element
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PuiSvHandlerFiler::mh_SaveDataElement(const C_OSCNodeDataPoolListElementId & orc_Id,
                                             C_OSCXMLParserBase & orc_XMLParser)
{
   orc_XMLParser.SetAttributeUint32("node", orc_Id.u32_NodeIndex);
   orc_XMLParser.SetAttributeUint32("data-pool", orc_Id.u32_DataPoolIndex);
   orc_XMLParser.SetAttributeUint32("list", orc_Id.u32_ListIndex);
   orc_XMLParser.SetAttributeUint32("element", orc_Id.u32_ElementIndex);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Transform device config mode to string

   \param[in] oe_Mode Device config mode

   \return
   Stringified device config mode
*/
//----------------------------------------------------------------------------------------------------------------------
QString C_PuiSvHandlerFiler::mh_DeviceConfigModeToString(const C_PuiSvData::E_DeviceConfigurationMode oe_Mode)
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
   }

   return c_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Transform write mode to string

   \param[in] oe_Mode Write mode

   \return
   Stringified write mode
*/
//----------------------------------------------------------------------------------------------------------------------
QString C_PuiSvHandlerFiler::mh_WriteModeToString(const C_PuiSvDbWidgetBase::E_WriteMode oe_Mode)
{
   QString c_Retval;

   switch (oe_Mode)
   {
   case C_PuiSvDbWidgetBase::eWM_MANUAL:
      c_Retval = "manual";
      break;
   case C_PuiSvDbWidgetBase::eWM_ON_CHANGE:
      c_Retval = "on-change";
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
QString C_PuiSvHandlerFiler::mh_DisplayStyleToString(const C_PuiSvDbWidgetBase::E_Style oe_Style)
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
QString C_PuiSvHandlerFiler::mh_ToggleTypeToString(const C_PuiSvDbToggle::E_Type oe_Type)
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
QString C_PuiSvHandlerFiler::mh_SpinBoxTypeToString(const C_PuiSvDbSpinBox::E_Type oe_Type)
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
QString C_PuiSvHandlerFiler::mh_LabelTypeToString(const C_PuiSvDbLabel::E_Type oe_Type)
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
QString C_PuiSvHandlerFiler::mh_SliderTypeToString(const C_PuiSvDbSlider::E_Type oe_Type)
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
QString C_PuiSvHandlerFiler::mh_ProgressBarTypeToString(const C_PuiSvDbProgressBar::E_Type oe_Type)
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
QString C_PuiSvHandlerFiler::mh_ProgressBarAlignmentTypeToString(const C_PuiSvDbProgressBar::E_Alignment oe_Alignment)
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
QString C_PuiSvHandlerFiler::mh_TransmissionModeToString(const C_PuiSvReadDataConfiguration::E_TransmissionMode oe_Mode)
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
QString C_PuiSvHandlerFiler::mh_SourceTypeToString(const C_PuiSvDbNodeDataPoolListElementId::E_Type oe_Type)
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
C_PuiSvData::E_DeviceConfigurationMode C_PuiSvHandlerFiler::mh_StringToDeviceConfigMode(const QString & orc_Input)
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

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Transform chart zoom mode to string

   \param[in] oe_Type Zoom mode setting

   \return
   Stringified zoom mode setting
*/
//----------------------------------------------------------------------------------------------------------------------
QString C_PuiSvHandlerFiler::mh_ChartZoomModeToString(const C_PuiSvDbChart::E_SettingZoomMode oe_ZoomMode)
{
   QString c_Retval;

   switch (oe_ZoomMode)
   {
   case C_PuiSvDbChart::eSETTING_ZM_XY:
      c_Retval = "setting_zm_xy";
      break;
   case C_PuiSvDbChart::eSETTING_ZM_X:
      c_Retval = "setting_zm_x";
      break;
   case C_PuiSvDbChart::eSETTING_ZM_Y:
      c_Retval = "setting_zm_y";
      break;
   }

   return c_Retval;
}
