//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Filer helper for dashboard components

   Filer helper for dashboard components

   \copyright   Copyright 2021 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.hpp"

#include "TglUtils.hpp"
#include "stwerrors.hpp"
#include "C_PuiBsElementsFiler.hpp"
#include "C_PuiSvDashboardFiler.hpp"
#include "C_OscNodeDataPoolFiler.hpp"

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
/*! \brief  Default constructor
*/
//----------------------------------------------------------------------------------------------------------------------
C_PuiSvDashboardFiler::C_PuiSvDashboardFiler()
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Load dashboard element

   \param[in,out]  orc_Dashboard             Dashboard element
   \param[in,out]  orc_XmlParser             XML parser with the "current" element set to the "dashboard" element
   \param[in]      oq_IgnoreMostErrorCases   Optional flag to allow some error cases to still return C_NO_ERR

   \return
   C_NO_ERR    information loaded
   C_CONFIG    error loading information
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_PuiSvDashboardFiler::h_LoadDashboard(C_PuiSvDashboard & orc_Dashboard, C_OscXmlParserBase & orc_XmlParser,
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
      C_PuiSvDbTabChart c_Widget;
      s32_Retval = mh_LoadTabChart(c_Widget, orc_XmlParser);
      orc_Dashboard.SetTabChart(c_Widget);
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
   if (orc_XmlParser.AttributeExists("tab-index"))
   {
      orc_Dashboard.SetTabIndex(orc_XmlParser.GetAttributeSint32("tab-index"));
   }
   else
   {
      orc_Dashboard.SetTabIndex(-1);
   }
   if (orc_XmlParser.AttributeExists("tab-type"))
   {
      const stw::scl::C_SclString c_TabTypeString = orc_XmlParser.GetAttributeString("tab-type");
      C_PuiSvDashboard::E_TabType e_TabType;
      if (C_PuiSvDashboardFiler::mh_StringToTabType(c_TabTypeString.c_str(), e_TabType) == C_NO_ERR)
      {
         orc_Dashboard.SetType(e_TabType);
      }
      else
      {
         s32_Retval = C_CONFIG;
      }
   }
   else
   {
      orc_Dashboard.SetType(C_PuiSvDashboard::eSCENE);
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Save dashboard element

   \param[in]      orc_Dashboard    Dashboard element
   \param[in,out]  orc_XmlParser    XML parser with the "current" element set to the "dashboard" element
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PuiSvDashboardFiler::h_SaveDashboard(const C_PuiSvDashboard & orc_Dashboard, C_OscXmlParserBase & orc_XmlParser)
{
   orc_XmlParser.SetAttributeBool("active", orc_Dashboard.GetActive());
   orc_XmlParser.SetAttributeSint32("tab-index", orc_Dashboard.GetTabIndex());
   orc_XmlParser.SetAttributeString("tab-type", C_PuiSvDashboardFiler::mh_TabTypeToString(
                                       orc_Dashboard.GetType()).toStdString().c_str());
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
   if (orc_Dashboard.GetType() == C_PuiSvDashboard::eCHART)
   {
      mh_SaveTabChart(orc_Dashboard.GetTabChart(), orc_XmlParser);
   }
   C_PuiBsElementsFiler::h_SaveBaseElements(orc_Dashboard, orc_XmlParser);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Load a UI index

   \param[out]     orc_Id           Read index
   \param[in,out]  orc_XmlParser    XML parser
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PuiSvDashboardFiler::h_LoadUiIndex(C_PuiSvDbNodeDataPoolListElementId & orc_Id,
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
      bool q_UseArrayElementIndex;
      uint32_t u32_ArrayElementIndex;
      C_PuiSvDbNodeDataPoolListElementId::E_Type e_SourceType;

      //Return
      orc_XmlParser.SelectNodeParent();
      if (orc_XmlParser.SelectNodeChild("source-type") == "source-type")
      {
         C_PuiSvDashboardFiler::mh_StringToSourceType(orc_XmlParser.GetNodeContent().c_str(), e_SourceType);
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
         C_OscNodeDataPoolFiler::h_StringToDataPool(orc_XmlParser.GetNodeContent(), e_InvalidTypePlaceholder);
         //Return
         orc_XmlParser.SelectNodeParent();
      }
      if (orc_XmlParser.SelectNodeChild("invalid-name-placeholder") == "invalid-name-placeholder")
      {
         c_InvalidNamePlaceholder = orc_XmlParser.GetNodeContent().c_str();
         //Return
         orc_XmlParser.SelectNodeParent();
      }
      if (orc_XmlParser.AttributeExists("array-element-index"))
      {
         u32_ArrayElementIndex = orc_XmlParser.GetAttributeUint32("array-element-index");
      }
      else
      {
         u32_ArrayElementIndex = 0UL;
      }
      if (orc_XmlParser.AttributeExists("use-array-element-index"))
      {
         q_UseArrayElementIndex = orc_XmlParser.GetAttributeBool("use-array-element-index");
      }
      else
      {
         q_UseArrayElementIndex = false;
      }
      orc_Id = C_PuiSvDbNodeDataPoolListElementId(c_Base, e_SourceType, q_UseArrayElementIndex, u32_ArrayElementIndex,
                                                  q_IsValid, e_InvalidTypePlaceholder, c_InvalidNamePlaceholder);

      if (orc_XmlParser.SelectNodeChild("hal-channel-name") == "hal-channel-name")
      {
         orc_Id.SetHalChannelName(orc_XmlParser.GetNodeContent().c_str());
         //Return
         orc_XmlParser.SelectNodeParent();
      }
      else
      {
         orc_Id.SetHalChannelName("");
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Save UI index to CURRENT node

   \param[in]      orc_Id           ID to store
   \param[in,out]  orc_XmlParser    XML parser
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PuiSvDashboardFiler::h_SaveUiIndex(const C_PuiSvDbNodeDataPoolListElementId & orc_Id,
                                          C_OscXmlParserBase & orc_XmlParser)
{
   orc_XmlParser.SetAttributeBool("is-valid", orc_Id.GetIsValid());
   orc_XmlParser.SetAttributeBool("use-array-element-index", orc_Id.GetUseArrayElementIndex());
   orc_XmlParser.SetAttributeUint32("array-element-index", orc_Id.GetArrayElementIndex());
   orc_XmlParser.CreateAndSelectNodeChild("index");
   orc_XmlParser.SetAttributeUint32("node", orc_Id.u32_NodeIndex);
   orc_XmlParser.SetAttributeUint32("data-pool", orc_Id.u32_DataPoolIndex);
   orc_XmlParser.SetAttributeUint32("list", orc_Id.u32_ListIndex);
   orc_XmlParser.SetAttributeUint32("element", orc_Id.u32_ElementIndex);
   //Return
   orc_XmlParser.SelectNodeParent();
   orc_XmlParser.CreateNodeChild("source-type",
                                 C_PuiSvDashboardFiler::mh_SourceTypeToString(orc_Id.GetType()).toStdString().c_str());
   orc_XmlParser.CreateNodeChild("invalid-type-placeholder",
                                 C_OscNodeDataPoolFiler::h_DataPoolToString(orc_Id.GetInvalidTypePlaceholder()));
   orc_XmlParser.CreateNodeChild("invalid-name-placeholder", orc_Id.GetInvalidNamePlaceholder().toStdString().c_str());
   orc_XmlParser.CreateNodeChild("hal-channel-name", orc_Id.GetHalChannelName().toStdString().c_str());
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Load widget elements

   \param[in,out]  orc_Widgets      Widget elements (Cleared if necessary)
   \param[in,out]  orc_XmlParser    XML parser with the "current" element set to the "dashboard" element

   \return
   C_NO_ERR    information loaded
   C_CONFIG    error loading information
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_PuiSvDashboardFiler::mh_LoadCharts(std::vector<C_PuiSvDbChart> & orc_Widgets,
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
            s32_Retval = C_PuiSvDashboardFiler::mh_LoadWidgetBase(c_Box, orc_XmlParser);
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

            if (orc_XmlParser.SelectNodeChild("zoom-mode") == "zoom-mode")
            {
               if (C_PuiSvDashboardFiler::mh_StringToChartSettingZoomMode(orc_XmlParser.GetNodeContent().c_str(),
                                                                          c_Box.e_SettingZoomMode) != C_NO_ERR)
               {
                  s32_Retval = C_CONFIG;
               }

               //Return
               tgl_assert(orc_XmlParser.SelectNodeParent() == "chart");
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
/*! \brief   Load tab chart

   \param[in,out]  orc_Widget       Tab chart
   \param[in,out]  orc_XmlParser    XML parser with the "current" element set to the "dashboard" element

   \return
   C_NO_ERR    information loaded
   C_CONFIG    error loading information
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_PuiSvDashboardFiler::mh_LoadTabChart(C_PuiSvDbTabChart & orc_Widget, C_OscXmlParserBase & orc_XmlParser)
{
   int32_t s32_Retval = C_NO_ERR;

   if (orc_XmlParser.SelectNodeChild("tab-chart") == "tab-chart")
   {
      s32_Retval = C_PuiSvDashboardFiler::mh_LoadWidgetBase(orc_Widget, orc_XmlParser);
      if (orc_XmlParser.SelectNodeChild("active-flags") == "active-flags")
      {
         C_SclString c_CurrentWidgetNode2 = orc_XmlParser.SelectNodeChild("active-flag");
         if (c_CurrentWidgetNode2 == "active-flag")
         {
            do
            {
               if (orc_XmlParser.AttributeExists("state") == true)
               {
                  orc_Widget.c_DataPoolElementsActive.push_back(orc_XmlParser.GetAttributeBool("state"));
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
         tgl_assert(orc_XmlParser.SelectNodeParent() == "tab-chart");
      }

      if (orc_Widget.c_DataPoolElementsActive.size() != orc_Widget.c_DataPoolElementsConfig.size())
      {
         // Size shall be identical
         orc_Widget.c_DataPoolElementsActive.resize(orc_Widget.c_DataPoolElementsConfig.size(), true);
      }

      if (orc_XmlParser.SelectNodeChild("color-indexes") == "color-indexes")
      {
         C_SclString c_CurrentWidgetNode2 = orc_XmlParser.SelectNodeChild("color-index");
         if (c_CurrentWidgetNode2 == "color-index")
         {
            do
            {
               if (orc_XmlParser.AttributeExists("index") == true)
               {
                  orc_Widget.c_DataPoolElementsColorIndex.push_back(
                     static_cast<uint8_t>(orc_XmlParser.GetAttributeUint32("index")));
               }
               else
               {
                  s32_Retval = C_CONFIG;
               }
               //Next
               c_CurrentWidgetNode2 = orc_XmlParser.SelectNodeNext("color-index");
            }
            while ((c_CurrentWidgetNode2 == "color-index") && (s32_Retval == C_NO_ERR));

            //Return
            tgl_assert(orc_XmlParser.SelectNodeParent() == "color-indexes");
         }
         //Return
         tgl_assert(orc_XmlParser.SelectNodeParent() == "tab-chart");
      }

      if (orc_Widget.c_DataPoolElementsColorIndex.size() != orc_Widget.c_DataPoolElementsConfig.size())
      {
         // Size shall be identical
         uint32_t u32_Counter;

         for (u32_Counter = orc_Widget.c_DataPoolElementsColorIndex.size();
              u32_Counter < orc_Widget.c_DataPoolElementsConfig.size(); ++u32_Counter)
         {
            orc_Widget.c_DataPoolElementsColorIndex.push_back(static_cast<uint8_t>(u32_Counter));
         }
      }

      if (orc_XmlParser.AttributeExists("splitter-left-width"))
      {
         orc_Widget.s32_SplitterLeftWidth = orc_XmlParser.GetAttributeSint32("splitter-left-width");
      }
      else
      {
         s32_Retval = C_CONFIG;
      }

      if (orc_XmlParser.SelectNodeChild("zoom-mode") == "zoom-mode")
      {
         if (C_PuiSvDashboardFiler::mh_StringToTabChartSettingZoomMode(orc_XmlParser.GetNodeContent().c_str(),
                                                                       orc_Widget.e_SettingZoomMode) != C_NO_ERR)
         {
            s32_Retval = C_CONFIG;
         }

         //Return
         tgl_assert(orc_XmlParser.SelectNodeParent() == "tab-chart");
      }
      else
      {
         s32_Retval = C_CONFIG;
      }

      if (orc_XmlParser.SelectNodeChild("yaxis-mode") == "yaxis-mode")
      {
         if (C_PuiSvDashboardFiler::mh_StringToTabChartSettingVerticalAxisMode(orc_XmlParser.GetNodeContent().c_str(),
                                                                               orc_Widget.e_SettingVerticalAxisMode) !=
             C_NO_ERR)
         {
            s32_Retval = C_CONFIG;
         }

         //Return
         tgl_assert(orc_XmlParser.SelectNodeParent() == "tab-chart");
      }
      else
      {
         s32_Retval = C_CONFIG;
      }

      if (orc_XmlParser.AttributeExists("is-paused"))
      {
         orc_Widget.q_IsPaused = orc_XmlParser.GetAttributeBool("is-paused");
      }
      else
      {
         s32_Retval = C_CONFIG;
      }

      if (orc_XmlParser.AttributeExists("is-zoom-mode-active"))
      {
         orc_Widget.q_IsZoomModeActive = orc_XmlParser.GetAttributeBool("is-zoom-mode-active");
      }
      else
      {
         s32_Retval = C_CONFIG;
      }

      if (orc_XmlParser.AttributeExists("are-sample-points-shown"))
      {
         orc_Widget.q_AreSamplePointsShown = orc_XmlParser.GetAttributeBool("are-sample-points-shown");
      }
      else
      {
         s32_Retval = C_CONFIG;
      }

      if (s32_Retval == C_NO_ERR)
      {
         s32_Retval = mh_LoadTabChartScreenRegion(orc_Widget.c_VisibleScreen, orc_XmlParser);
      }

      //Return
      orc_XmlParser.SelectNodeParent();
   }

   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Load widget elements

   \param[in,out]  orc_Widgets      Widget elements (Cleared if necessary)
   \param[in,out]  orc_XmlParser    XML parser with the "current" element set to the "dashboard" element

   \return
   C_NO_ERR    information loaded
   C_CONFIG    error loading information
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_PuiSvDashboardFiler::mh_LoadLabels(std::vector<C_PuiSvDbLabel> & orc_Widgets,
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
            s32_Retval = C_PuiSvDashboardFiler::mh_LoadWidgetBase(c_Box, orc_XmlParser);
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

   \param[in,out]  orc_Widgets      Widget elements (Cleared if necessary)
   \param[in,out]  orc_XmlParser    XML parser with the "current" element set to the "dashboard" element

   \return
   C_NO_ERR    information loaded
   C_CONFIG    error loading information
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_PuiSvDashboardFiler::mh_LoadParams(std::vector<C_PuiSvDbParam> & orc_Widgets,
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
            s32_Retval = C_PuiSvDashboardFiler::mh_LoadWidgetBase(c_Box, orc_XmlParser);
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

   \param[out]     orc_Items        Tree elements
   \param[in,out]  orc_XmlParser    XML parser with the "current" element set to the "param" element
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PuiSvDashboardFiler::mh_LoadParamExpandedItems(std::vector<C_PuiSvDbExpandedTreeIndex> & orc_Items,
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
            C_PuiSvDashboardFiler::h_LoadUiIndex(c_Item.c_ExpandedId, orc_XmlParser);
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

   \param[out]     orc_Items        Column position indices elements
   \param[in,out]  orc_XmlParser    XML parser with the "current" element set to the "param" element
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PuiSvDashboardFiler::mh_LoadParamColumnPositionIndices(std::vector<int32_t> & orc_Items,
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

   \param[in,out]  orc_Widgets      Widget elements (Cleared if necessary)
   \param[in,out]  orc_XmlParser    XML parser with the "current" element set to the "dashboard" element

   \return
   C_NO_ERR    information loaded
   C_CONFIG    error loading information
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_PuiSvDashboardFiler::mh_LoadPieCharts(std::vector<C_PuiSvDbPieChart> & orc_Widgets,
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
            s32_Retval = C_PuiSvDashboardFiler::mh_LoadWidgetBase(c_Box, orc_XmlParser);
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

   \param[in,out]  orc_Widgets      Widget elements (Cleared if necessary)
   \param[in,out]  orc_XmlParser    XML parser with the "current" element set to the "dashboard" element

   \return
   C_NO_ERR    information loaded
   C_CONFIG    error loading information
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_PuiSvDashboardFiler::mh_LoadSpinBoxes(std::vector<C_PuiSvDbSpinBox> & orc_Widgets,
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
            s32_Retval = C_PuiSvDashboardFiler::mh_LoadWidgetBase(c_Box, orc_XmlParser);
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
               if (C_OscNodeDataPoolFiler::h_LoadDataPoolContentV1(c_Box.c_Value, orc_XmlParser) != C_NO_ERR)
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

   \param[in,out]  orc_Widgets      Widget elements (Cleared if necessary)
   \param[in,out]  orc_XmlParser    XML parser with the "current" element set to the "dashboard" element

   \return
   C_NO_ERR    information loaded
   C_CONFIG    error loading information
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_PuiSvDashboardFiler::mh_LoadTables(std::vector<C_PuiSvDbTable> & orc_Widgets,
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
            s32_Retval = C_PuiSvDashboardFiler::mh_LoadWidgetBase(c_Box, orc_XmlParser);

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

   \param[in,out]  orc_Widgets      Widget elements (Cleared if necessary)
   \param[in,out]  orc_XmlParser    XML parser with the "current" element set to the "dashboard" element

   \return
   C_NO_ERR    information loaded
   C_CONFIG    error loading information
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_PuiSvDashboardFiler::mh_LoadSliders(std::vector<C_PuiSvDbSlider> & orc_Widgets,
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
            s32_Retval = C_PuiSvDashboardFiler::mh_LoadWidgetBase(c_Box, orc_XmlParser);
            c_Box.q_ShowMinMax = orc_XmlParser.GetAttributeBool("show-min-max");
            if (s32_Retval == C_NO_ERR)
            {
               if (orc_XmlParser.SelectNodeChild("type") == "type")
               {
                  if (C_PuiSvDashboardFiler::mh_StringToSliderType(orc_XmlParser.GetNodeContent().c_str(),
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

   \param[in,out]  orc_Widgets      Widget elements (Cleared if necessary)
   \param[in,out]  orc_XmlParser    XML parser with the "current" element set to the "dashboard" element

   \return
   C_NO_ERR    information loaded
   C_CONFIG    error loading information
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_PuiSvDashboardFiler::mh_LoadProgressBars(std::vector<C_PuiSvDbProgressBar> & orc_Widgets,
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
            s32_Retval = C_PuiSvDashboardFiler::mh_LoadWidgetBase(c_Box, orc_XmlParser);
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
                  if (C_PuiSvDashboardFiler::mh_StringToProgressBarType(orc_XmlParser.GetNodeContent().c_str(),
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
                     if (C_PuiSvDashboardFiler::mh_StringToProgressBarAlignmentType(orc_XmlParser.GetNodeContent().c_str(),
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

   \param[in,out]  orc_Widgets      Widget elements (Cleared if necessary)
   \param[in,out]  orc_XmlParser    XML parser with the "current" element set to the "dashboard" element

   \return
   C_NO_ERR    information loaded
   C_CONFIG    error loading information
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_PuiSvDashboardFiler::mh_LoadToggles(std::vector<C_PuiSvDbToggle> & orc_Widgets,
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
            s32_Retval = C_PuiSvDashboardFiler::mh_LoadWidgetBase(c_Box, orc_XmlParser);
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
int32_t C_PuiSvDashboardFiler::mh_LoadWidgetBase(C_PuiSvDbWidgetBase & orc_Widget, C_OscXmlParserBase & orc_XmlParser)
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
               s32_Retval = C_PuiSvDashboardFiler::mh_LoadDataElementConfig(c_ElementConfig, orc_XmlParser);

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
      if ((orc_XmlParser.SelectNodeChild("write-mode") == "write-mode") && (s32_Retval == C_NO_ERR))
      {
         if (C_PuiSvDashboardFiler::mh_StringToWriteMode(orc_XmlParser.GetNodeContent().c_str(),
                                                         orc_Widget.e_ElementWriteMode) != C_NO_ERR)
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
      if ((orc_XmlParser.SelectNodeChild("display-style") == "display-style") && (s32_Retval == C_NO_ERR))
      {
         if (C_PuiSvDashboardFiler::mh_StringToDisplayStyle(orc_XmlParser.GetNodeContent().c_str(),
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
/*! \brief  Load data element config

   \param[in,out]  orc_Config       Config
   \param[in,out]  orc_XmlParser    XML parser

   \return
   C_NO_ERR    information loaded
   C_CONFIG    error loading information
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_PuiSvDashboardFiler::mh_LoadDataElementConfig(C_PuiSvDbNodeDataElementConfig & orc_Config,
                                                        C_OscXmlParserBase & orc_XmlParser)
{
   int32_t s32_Retval;

   h_LoadUiIndex(orc_Config.c_ElementId, orc_XmlParser);
   s32_Retval = C_PuiSvDashboardFiler::mh_LoadDataScalingConfig(orc_Config.c_ElementScaling, orc_XmlParser);
   if (s32_Retval == C_NO_ERR)
   {
      s32_Retval = C_PuiSvDashboardFiler::mh_LoadDataFormatterConfig(orc_Config.c_DisplayFormatter, orc_XmlParser);
   }
   if (s32_Retval == C_NO_ERR)
   {
      if (orc_XmlParser.SelectNodeChild("display-name") == "display-name")
      {
         orc_Config.c_DisplayName = orc_XmlParser.GetNodeContent().c_str();
         //Return
         tgl_assert(orc_XmlParser.SelectNodeParent() == "data-pool-element");
      }
      else
      {
         orc_Config.c_DisplayName = "";
      }
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Load data scaling config

   \param[in,out]  orc_Config       Config
   \param[in,out]  orc_XmlParser    XML parser

   \return
   C_NO_ERR    information loaded
   C_CONFIG    error loading information
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_PuiSvDashboardFiler::mh_LoadDataScalingConfig(C_PuiSvDbDataElementScaling & orc_Config,
                                                        C_OscXmlParserBase & orc_XmlParser)
{
   int32_t s32_Retval = C_NO_ERR;

   if (orc_XmlParser.SelectNodeChild("scaling") == "scaling")
   {
      orc_Config.q_UseDefault = orc_XmlParser.GetAttributeBool("use-default");
      orc_Config.f64_Factor = orc_XmlParser.GetAttributeFloat64("factor");
      orc_Config.f64_Offset = orc_XmlParser.GetAttributeFloat64("offset");
      if (orc_XmlParser.SelectNodeChild("unit") == "unit")
      {
         orc_Config.c_Unit = orc_XmlParser.GetNodeContent().c_str();
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
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Load data formatter config

   \param[in,out]  orc_Config       Config
   \param[in,out]  orc_XmlParser    XML parser

   \return
   C_NO_ERR    information loaded
   C_CONFIG    error loading information
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_PuiSvDashboardFiler::mh_LoadDataFormatterConfig(C_PuiSvDbDataElementDisplayFormatter & orc_Config,
                                                          C_OscXmlParserBase & orc_XmlParser)
{
   int32_t s32_Retval = C_NO_ERR;

   if (orc_XmlParser.SelectNodeChild("display-formatter") == "display-formatter")
   {
      s32_Retval = orc_XmlParser.GetAttributeBoolError("is-active", orc_Config.q_IsActive);
      if (s32_Retval == C_NO_ERR)
      {
         s32_Retval = orc_XmlParser.SelectNodeChildError("string");
      }
      if (s32_Retval == C_NO_ERR)
      {
         orc_Config.c_FormatterString = orc_XmlParser.GetNodeContent().c_str();
         //Return
         tgl_assert(orc_XmlParser.SelectNodeParent() == "display-formatter");
      }
      //Return
      tgl_assert(orc_XmlParser.SelectNodeParent() == "data-pool-element");
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Load values

   \param[in,out]  orc_Values       Values
   \param[in,out]  orc_XmlParser    XML parser with the "current" element set to the "param" element

   \return
   C_NO_ERR    information loaded
   C_CONFIG    error loading information
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_PuiSvDashboardFiler::mh_LoadParamDataSetIndices(std::vector<int32_t> & orc_Values,
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

   \param[in,out]  orc_Values       Values
   \param[in,out]  orc_XmlParser    XML parser with the "current" element set to the "param" element

   \return
   C_NO_ERR    information loaded
   C_CONFIG    error loading information
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_PuiSvDashboardFiler::mh_LoadParamValues(std::vector<C_OscNodeDataPoolContent> & orc_Values,
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
            s32_Retval = C_OscNodeDataPoolFiler::h_LoadDataPoolContentV1(c_Value, orc_XmlParser);
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

   \param[in,out]  orc_Values       Values
   \param[in,out]  orc_XmlParser    XML parser with the "current" element set to the "param" element

   \return
   C_NO_ERR    information loaded
   C_CONFIG    error loading information
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_PuiSvDashboardFiler::mh_LoadParamTables(std::vector<std::vector<int32_t> > & orc_Values,
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

   \param[in,out]  orc_Values       Values
   \param[in,out]  orc_XmlParser    XML parser with the "current" element set to the "table" element

   \return
   C_NO_ERR    information loaded
   C_CONFIG    error loading information
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_PuiSvDashboardFiler::mh_LoadParamColumns(std::vector<int32_t> & orc_Values,
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
/*! \brief  Load tab chart screen region

   \param[in,out]  orc_ScreenRegion    Screen region
   \param[in,out]  orc_XmlParser       XML parser

   \return
   C_NO_ERR    information loaded
   C_CONFIG    error loading information
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_PuiSvDashboardFiler::mh_LoadTabChartScreenRegion(std::vector<std::array<float64_t, 4> > & orc_ScreenRegion,
                                                           C_OscXmlParserBase & orc_XmlParser)
{
   int32_t s32_Retval = C_NO_ERR;

   orc_ScreenRegion.clear();
   if (orc_XmlParser.SelectNodeChild("screen-regions") == "screen-regions")
   {
      C_SclString c_CurrentValueNode = orc_XmlParser.SelectNodeChild("screen-region");
      if (c_CurrentValueNode == "screen-region")
      {
         do
         {
            if (((orc_XmlParser.AttributeExists("value1") && orc_XmlParser.AttributeExists("value2")) &&
                 orc_XmlParser.AttributeExists("value3")) && orc_XmlParser.AttributeExists("value4"))
            {
               const float64_t f64_Value1 = orc_XmlParser.GetAttributeFloat64("value1");
               const float64_t f64_Value2 = orc_XmlParser.GetAttributeFloat64("value2");
               const float64_t f64_Value3 = orc_XmlParser.GetAttributeFloat64("value3");
               const float64_t f64_Value4 = orc_XmlParser.GetAttributeFloat64("value4");
               const std::array<float64_t, 4> c_ScreenRegion = {
                  {f64_Value1, f64_Value2, f64_Value3, f64_Value4}
               };
               orc_ScreenRegion.push_back(c_ScreenRegion);
            }
            else
            {
               s32_Retval = C_CONFIG;
            }
            //Next
            c_CurrentValueNode = orc_XmlParser.SelectNodeNext("screen-region");
         }
         while ((c_CurrentValueNode == "screen-region") && (s32_Retval == C_NO_ERR));
         //Return
         tgl_assert(orc_XmlParser.SelectNodeParent() == "screen-regions");
      }
      //Return
      tgl_assert(orc_XmlParser.SelectNodeParent() == "tab-chart");
   }
   else
   {
      s32_Retval = C_CONFIG;
   }

   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Save chart dummy

   For loading compatibility we keep an empty "charts" node.

   \param[in]      orc_Widgets      Widget elements
   \param[in,out]  orc_XmlParser    XML parser with the "current" element set to the "dashboard" element
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PuiSvDashboardFiler::mh_SaveCharts(const std::vector<C_PuiSvDbChart> & orc_Widgets,
                                          C_OscXmlParserBase & orc_XmlParser)
{
   Q_UNUSED(orc_Widgets)

   orc_XmlParser.CreateAndSelectNodeChild("charts");
   //Return
   orc_XmlParser.SelectNodeParent();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Save tab chart

   \param[in]      orc_Widget       Tab chart
   \param[in,out]  orc_XmlParser    XML parser with the "current" element set to the "dashboard" element
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PuiSvDashboardFiler::mh_SaveTabChart(const C_PuiSvDbTabChart & orc_Widget, C_OscXmlParserBase & orc_XmlParser)
{
   orc_XmlParser.CreateAndSelectNodeChild("tab-chart");
   C_PuiSvDashboardFiler::mh_SaveWidgetBase(orc_Widget, orc_XmlParser);
   orc_XmlParser.CreateAndSelectNodeChild("active-flags");
   for (uint32_t u32_ItActive = 0; u32_ItActive < orc_Widget.c_DataPoolElementsActive.size(); ++u32_ItActive)
   {
      orc_XmlParser.CreateAndSelectNodeChild("active-flag");
      orc_XmlParser.SetAttributeBool("state", orc_Widget.c_DataPoolElementsActive[u32_ItActive]);
      //Return
      tgl_assert(orc_XmlParser.SelectNodeParent() == "active-flags");
   }

   //Return
   tgl_assert(orc_XmlParser.SelectNodeParent() == "tab-chart");

   orc_XmlParser.CreateAndSelectNodeChild("color-indexes");
   for (uint32_t u32_ItActive = 0; u32_ItActive < orc_Widget.c_DataPoolElementsColorIndex.size(); ++u32_ItActive)
   {
      orc_XmlParser.CreateAndSelectNodeChild("color-index");
      orc_XmlParser.SetAttributeUint32("index", orc_Widget.c_DataPoolElementsColorIndex[u32_ItActive]);
      //Return
      tgl_assert(orc_XmlParser.SelectNodeParent() == "color-indexes");
   }

   //Return
   tgl_assert(orc_XmlParser.SelectNodeParent() == "tab-chart");

   orc_XmlParser.CreateNodeChild("zoom-mode",
                                 C_PuiSvDashboardFiler::mh_TabChartZoomModeToString(
                                    orc_Widget.e_SettingZoomMode).toStdString().c_str());
   orc_XmlParser.CreateNodeChild("yaxis-mode",
                                 C_PuiSvDashboardFiler::mh_TabChartVerticalAxisModeToString(
                                    orc_Widget.e_SettingVerticalAxisMode).toStdString().c_str());
   orc_XmlParser.SetAttributeBool("is-paused", orc_Widget.q_IsPaused);
   orc_XmlParser.SetAttributeSint32("splitter-left-width", orc_Widget.s32_SplitterLeftWidth);
   orc_XmlParser.SetAttributeBool("is-zoom-mode-active", orc_Widget.q_IsZoomModeActive);
   orc_XmlParser.SetAttributeBool("are-sample-points-shown", orc_Widget.q_AreSamplePointsShown);

   mh_SaveTabChartScreenRegion(orc_Widget.c_VisibleScreen, orc_XmlParser);
   //Return
   orc_XmlParser.SelectNodeParent();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Save widget elements

   \param[in]      orc_Widgets      Widget elements
   \param[in,out]  orc_XmlParser    XML parser with the "current" element set to the "dashboard" element
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PuiSvDashboardFiler::mh_SaveLabels(const std::vector<C_PuiSvDbLabel> & orc_Widgets,
                                          C_OscXmlParserBase & orc_XmlParser)
{
   orc_XmlParser.CreateAndSelectNodeChild("labels");
   for (uint32_t u32_ItWidget = 0; u32_ItWidget < orc_Widgets.size(); ++u32_ItWidget)
   {
      const C_PuiSvDbLabel & rc_Label = orc_Widgets[u32_ItWidget];
      orc_XmlParser.CreateAndSelectNodeChild("label");
      orc_XmlParser.SetAttributeBool("show-caption", rc_Label.q_ShowCaption);
      orc_XmlParser.SetAttributeBool("show-unit", rc_Label.q_ShowUnit);
      C_PuiSvDashboardFiler::mh_SaveWidgetBase(rc_Label, orc_XmlParser);
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

   \param[in]      orc_Widgets      Widget elements
   \param[in,out]  orc_XmlParser    XML parser with the "current" element set to the "dashboard" element
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PuiSvDashboardFiler::mh_SaveParams(const std::vector<C_PuiSvDbParam> & orc_Widgets,
                                          C_OscXmlParserBase & orc_XmlParser)
{
   orc_XmlParser.CreateAndSelectNodeChild("params");
   for (uint32_t u32_ItWidget = 0; u32_ItWidget < orc_Widgets.size(); ++u32_ItWidget)
   {
      const C_PuiSvDbParam & rc_Param = orc_Widgets[u32_ItWidget];
      orc_XmlParser.CreateAndSelectNodeChild("param");
      C_PuiSvDashboardFiler::mh_SaveWidgetBase(rc_Param, orc_XmlParser);
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
         C_OscNodeDataPoolFiler::h_SaveDataPoolContentV1(rc_Param.c_ListValues[u32_ItValue], orc_XmlParser);
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

   \param[in]      orc_Items        Tree elements
   \param[in,out]  orc_XmlParser    XML parser with the "current" element set to the "param" element
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PuiSvDashboardFiler::mh_SaveParamExpandedItems(const std::vector<C_PuiSvDbExpandedTreeIndex> & orc_Items,
                                                      C_OscXmlParserBase & orc_XmlParser)
{
   //Columns
   orc_XmlParser.CreateAndSelectNodeChild("expanded-tree-items");
   for (uint32_t u32_ItTable = 0; u32_ItTable < orc_Items.size(); ++u32_ItTable)
   {
      const C_PuiSvDbExpandedTreeIndex & rc_TreeElement = orc_Items[u32_ItTable];
      orc_XmlParser.CreateAndSelectNodeChild("expanded-tree-item");
      orc_XmlParser.SetAttributeUint32("valid-layers", rc_TreeElement.u32_Layer);
      h_SaveUiIndex(rc_TreeElement.c_ExpandedId, orc_XmlParser);
      //Return
      tgl_assert(orc_XmlParser.SelectNodeParent() == "expanded-tree-items");
   }
   //Return
   tgl_assert(orc_XmlParser.SelectNodeParent() == "param");
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Save column position indices elements

   \param[in]      orc_Items        Column position indices elements
   \param[in,out]  orc_XmlParser    XML parser with the "current" element set to the "param" element
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PuiSvDashboardFiler::mh_SaveParamColumnPositionIndices(const std::vector<int32_t> & orc_Items,
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

   \param[in]      orc_Widgets      Widget elements
   \param[in,out]  orc_XmlParser    XML parser with the "current" element set to the "dashboard" element
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PuiSvDashboardFiler::mh_SavePieCharts(const std::vector<C_PuiSvDbPieChart> & orc_Widgets,
                                             C_OscXmlParserBase & orc_XmlParser)
{
   orc_XmlParser.CreateAndSelectNodeChild("pie-charts");
   for (uint32_t u32_ItWidget = 0; u32_ItWidget < orc_Widgets.size(); ++u32_ItWidget)
   {
      const C_PuiSvDbPieChart & rc_PieChart = orc_Widgets[u32_ItWidget];
      orc_XmlParser.CreateAndSelectNodeChild("pie-chart");
      orc_XmlParser.SetAttributeBool("show-value", rc_PieChart.q_ShowValue);
      orc_XmlParser.SetAttributeBool("show-unit", rc_PieChart.q_ShowUnit);

      C_PuiSvDashboardFiler::mh_SaveWidgetBase(rc_PieChart, orc_XmlParser);
      //Return
      tgl_assert(orc_XmlParser.SelectNodeParent() == "pie-charts");
   }
   //Return
   orc_XmlParser.SelectNodeParent();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Save widget elements

   \param[in]      orc_Widgets      Widget elements
   \param[in,out]  orc_XmlParser    XML parser with the "current" element set to the "dashboard" element
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PuiSvDashboardFiler::mh_SaveSpinBoxes(const std::vector<C_PuiSvDbSpinBox> & orc_Widgets,
                                             C_OscXmlParserBase & orc_XmlParser)
{
   orc_XmlParser.CreateAndSelectNodeChild("spin-boxes");
   for (uint32_t u32_ItWidget = 0; u32_ItWidget < orc_Widgets.size(); ++u32_ItWidget)
   {
      const C_PuiSvDbSpinBox & rc_SpinBox = orc_Widgets[u32_ItWidget];
      orc_XmlParser.CreateAndSelectNodeChild("spin-box");

      orc_XmlParser.SetAttributeBool("show-unit", rc_SpinBox.q_ShowUnit);

      C_PuiSvDashboardFiler::mh_SaveWidgetBase(rc_SpinBox, orc_XmlParser);

      orc_XmlParser.CreateNodeChild("type", C_PuiSvDashboardFiler::mh_SpinBoxTypeToString(
                                       rc_SpinBox.e_Type).toStdString().c_str());
      orc_XmlParser.CreateAndSelectNodeChild("content");
      C_OscNodeDataPoolFiler::h_SaveDataPoolContentV1(rc_SpinBox.c_Value, orc_XmlParser);
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

   \param[in]      orc_Widgets      Widget elements
   \param[in,out]  orc_XmlParser    XML parser with the "current" element set to the "dashboard" element
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PuiSvDashboardFiler::mh_SaveTables(const std::vector<C_PuiSvDbTable> & orc_Widgets,
                                          C_OscXmlParserBase & orc_XmlParser)
{
   orc_XmlParser.CreateAndSelectNodeChild("tables");
   for (uint32_t u32_ItWidget = 0; u32_ItWidget < orc_Widgets.size(); ++u32_ItWidget)
   {
      const C_PuiSvDbTable & rc_Table = orc_Widgets[u32_ItWidget];
      orc_XmlParser.CreateAndSelectNodeChild("table");

      C_PuiSvDashboardFiler::mh_SaveWidgetBase(rc_Table, orc_XmlParser);

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

   \param[in]      orc_Widgets      Widget elements
   \param[in,out]  orc_XmlParser    XML parser with the "current" element set to the "dashboard" element
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PuiSvDashboardFiler::mh_SaveSliders(const std::vector<C_PuiSvDbSlider> & orc_Widgets,
                                           C_OscXmlParserBase & orc_XmlParser)
{
   orc_XmlParser.CreateAndSelectNodeChild("sliders");
   for (uint32_t u32_ItWidget = 0; u32_ItWidget < orc_Widgets.size(); ++u32_ItWidget)
   {
      const C_PuiSvDbSlider & rc_Slider = orc_Widgets[u32_ItWidget];
      orc_XmlParser.CreateAndSelectNodeChild("slider");
      C_PuiSvDashboardFiler::mh_SaveWidgetBase(rc_Slider, orc_XmlParser);
      orc_XmlParser.CreateNodeChild("type", C_PuiSvDashboardFiler::mh_SliderTypeToString(
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

   \param[in]      orc_Widgets      Widget elements
   \param[in,out]  orc_XmlParser    XML parser with the "current" element set to the "dashboard" element
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PuiSvDashboardFiler::mh_SaveProgressBars(const std::vector<C_PuiSvDbProgressBar> & orc_Widgets,
                                                C_OscXmlParserBase & orc_XmlParser)
{
   orc_XmlParser.CreateAndSelectNodeChild("progress-bars");
   for (uint32_t u32_ItWidget = 0; u32_ItWidget < orc_Widgets.size(); ++u32_ItWidget)
   {
      const C_PuiSvDbProgressBar & rc_ProgressBar = orc_Widgets[u32_ItWidget];
      orc_XmlParser.CreateAndSelectNodeChild("progress-bar");
      C_PuiSvDashboardFiler::mh_SaveWidgetBase(rc_ProgressBar, orc_XmlParser);
      orc_XmlParser.CreateNodeChild("type", C_PuiSvDashboardFiler::mh_ProgressBarTypeToString(
                                       rc_ProgressBar.e_Type).toStdString().c_str());
      orc_XmlParser.CreateNodeChild("alignment",
                                    C_PuiSvDashboardFiler::mh_ProgressBarAlignmentTypeToString(
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

   \param[in]      orc_Widgets      Widget elements
   \param[in,out]  orc_XmlParser    XML parser with the "current" element set to the "dashboard" element
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PuiSvDashboardFiler::mh_SaveToggles(const std::vector<C_PuiSvDbToggle> & orc_Widgets,
                                           C_OscXmlParserBase & orc_XmlParser)
{
   orc_XmlParser.CreateAndSelectNodeChild("toggles");
   for (uint32_t u32_ItWidget = 0; u32_ItWidget < orc_Widgets.size(); ++u32_ItWidget)
   {
      const C_PuiSvDbToggle & rc_Toggle = orc_Widgets[u32_ItWidget];
      orc_XmlParser.CreateAndSelectNodeChild("toggle");

      C_PuiSvDashboardFiler::mh_SaveWidgetBase(rc_Toggle, orc_XmlParser);

      orc_XmlParser.CreateNodeChild("type", C_PuiSvDashboardFiler::mh_ToggleTypeToString(
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
void C_PuiSvDashboardFiler::mh_SaveWidgetBase(const C_PuiSvDbWidgetBase & orc_Widget,
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
      C_PuiSvDashboardFiler::mh_SaveDataElementConfig(orc_Widget.c_DataPoolElementsConfig[u32_ItDataElement],
                                                      orc_XmlParser);
   }

   //Return
   tgl_assert(orc_XmlParser.SelectNodeParent() == "base");

   orc_XmlParser.CreateNodeChild("write-mode", C_PuiSvDashboardFiler::mh_WriteModeToString(
                                    orc_Widget.e_ElementWriteMode).toStdString().c_str());
   orc_XmlParser.CreateNodeChild("display-style", C_PuiSvDashboardFiler::mh_DisplayStyleToString(
                                    orc_Widget.e_DisplayStyle).toStdString().c_str());
   //Return
   orc_XmlParser.SelectNodeParent();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Save data element config

   \param[in]      orc_Config       Config
   \param[in,out]  orc_XmlParser    XML parser
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PuiSvDashboardFiler::mh_SaveDataElementConfig(const C_PuiSvDbNodeDataElementConfig & orc_Config,
                                                     C_OscXmlParserBase & orc_XmlParser)
{
   orc_XmlParser.CreateAndSelectNodeChild("data-pool-element");
   h_SaveUiIndex(orc_Config.c_ElementId, orc_XmlParser);
   //Scaling
   C_PuiSvDashboardFiler::mh_SaveDataScalingConfig(orc_Config.c_ElementScaling, orc_XmlParser);
   //Formatter
   C_PuiSvDashboardFiler::mh_SaveDataFormatterConfig(orc_Config.c_DisplayFormatter, orc_XmlParser);
   orc_XmlParser.CreateNodeChild("display-name", orc_Config.c_DisplayName.toStdString().c_str());
   //Return
   tgl_assert(orc_XmlParser.SelectNodeParent() == "data-pool-elements");
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Save data scaling config

   \param[in]      orc_Config       Config
   \param[in,out]  orc_XmlParser    XML parser
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PuiSvDashboardFiler::mh_SaveDataScalingConfig(const C_PuiSvDbDataElementScaling & orc_Config,
                                                     C_OscXmlParserBase & orc_XmlParser)
{
   orc_XmlParser.CreateAndSelectNodeChild("scaling");
   orc_XmlParser.SetAttributeBool("use-default", orc_Config.q_UseDefault);
   orc_XmlParser.SetAttributeFloat64("factor", orc_Config.f64_Factor);
   orc_XmlParser.SetAttributeFloat64("offset", orc_Config.f64_Offset);
   orc_XmlParser.CreateNodeChild("unit", orc_Config.c_Unit.toStdString().c_str());
   //Return
   tgl_assert(orc_XmlParser.SelectNodeParent() == "data-pool-element");
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Save data formatter config

   \param[in]      orc_Config       Config
   \param[in,out]  orc_XmlParser    XML parser
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PuiSvDashboardFiler::mh_SaveDataFormatterConfig(const C_PuiSvDbDataElementDisplayFormatter & orc_Config,
                                                       C_OscXmlParserBase & orc_XmlParser)
{
   orc_XmlParser.CreateAndSelectNodeChild("display-formatter");
   orc_XmlParser.SetAttributeBool("is-active", orc_Config.q_IsActive);
   orc_XmlParser.CreateNodeChild("string", orc_Config.c_FormatterString.toStdString().c_str());
   //Return
   tgl_assert(orc_XmlParser.SelectNodeParent() == "data-pool-element");
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Save tab chart screen region

   \param[in]      orc_ScreenRegion    Screen region
   \param[in,out]  orc_XmlParser       XML parser
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PuiSvDashboardFiler::mh_SaveTabChartScreenRegion(const std::vector<std::array<float64_t, 4> > & orc_ScreenRegion,
                                                        C_OscXmlParserBase & orc_XmlParser)
{
   orc_XmlParser.CreateAndSelectNodeChild("screen-regions");
   for (std::vector<std::array<float64_t, 4> >::const_iterator c_It = orc_ScreenRegion.begin();
        c_It != orc_ScreenRegion.end(); ++c_It)
   {
      const std::array<float64_t, 4> & rc_Vals = *c_It;
      orc_XmlParser.CreateAndSelectNodeChild("screen-region");
      orc_XmlParser.SetAttributeFloat64("value1", rc_Vals[0]);
      orc_XmlParser.SetAttributeFloat64("value2", rc_Vals[1]);
      orc_XmlParser.SetAttributeFloat64("value3", rc_Vals[2]);
      orc_XmlParser.SetAttributeFloat64("value4", rc_Vals[3]);
      //Return
      tgl_assert(orc_XmlParser.SelectNodeParent() == "screen-regions");
   }
   //Return
   tgl_assert(orc_XmlParser.SelectNodeParent() == "tab-chart");
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Transform tab type to string

   \param[in]  oe_Type  Tab type

   \return
   Stringified tab type
*/
//----------------------------------------------------------------------------------------------------------------------
QString C_PuiSvDashboardFiler::mh_TabTypeToString(const C_PuiSvDashboard::E_TabType oe_Type)
{
   QString c_Retval;

   switch (oe_Type)
   {
   case C_PuiSvDashboard::eCHART:
      c_Retval = "chart";
      break;
   case C_PuiSvDashboard::eSCENE:
      c_Retval = "scene";
      break;
   default:
      break;
   }

   return c_Retval;
}
//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Transform display style to string

   \param[in]  oe_Style    Display style

   \return
   Stringified display style
*/
//----------------------------------------------------------------------------------------------------------------------
QString C_PuiSvDashboardFiler::mh_DisplayStyleToString(const C_PuiSvDbWidgetBase::E_Style oe_Style)
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

   \param[in]  oe_Type  Toggle type

   \return
   Stringified toggle type
*/
//----------------------------------------------------------------------------------------------------------------------
QString C_PuiSvDashboardFiler::mh_ToggleTypeToString(const C_PuiSvDbToggle::E_Type oe_Type)
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

   \param[in]  oe_Type  Spin box type

   \return
   Stringified spin box type
*/
//----------------------------------------------------------------------------------------------------------------------
QString C_PuiSvDashboardFiler::mh_SpinBoxTypeToString(const C_PuiSvDbSpinBox::E_Type oe_Type)
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

   \param[in]  oe_Type  Label type

   \return
   Stringified label type
*/
//----------------------------------------------------------------------------------------------------------------------
QString C_PuiSvDashboardFiler::mh_LabelTypeToString(const C_PuiSvDbLabel::E_Type oe_Type)
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

   \param[in]  oe_Type  Slider type

   \return
   Stringified slider type
*/
//----------------------------------------------------------------------------------------------------------------------
QString C_PuiSvDashboardFiler::mh_SliderTypeToString(const C_PuiSvDbSlider::E_Type oe_Type)
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

   \param[in]  oe_Type  Progress bar type

   \return
   Stringified progress bar type
*/
//----------------------------------------------------------------------------------------------------------------------
QString C_PuiSvDashboardFiler::mh_ProgressBarTypeToString(const C_PuiSvDbProgressBar::E_Type oe_Type)
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

   \param[in]  oe_Alignment   Alignment

   \return
   Stringified progress bar alignment type
*/
//----------------------------------------------------------------------------------------------------------------------
QString C_PuiSvDashboardFiler::mh_ProgressBarAlignmentTypeToString(const C_PuiSvDbProgressBar::E_Alignment oe_Alignment)
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
/*! \brief   Transform chart zoom mode to string

   \param[in]  oe_ZoomMode    Zoom mode

   \return
   Stringified zoom mode setting
*/
//----------------------------------------------------------------------------------------------------------------------
QString C_PuiSvDashboardFiler::mh_TabChartZoomModeToString(const C_PuiSvDbTabChart::E_SettingZoomMode oe_ZoomMode)
{
   QString c_Retval;

   switch (oe_ZoomMode)
   {
   case C_PuiSvDbTabChart::eSETTING_ZM_XY:
      c_Retval = "setting_zm_xy";
      break;
   case C_PuiSvDbTabChart::eSETTING_ZM_X:
      c_Retval = "setting_zm_x";
      break;
   case C_PuiSvDbTabChart::eSETTING_ZM_Y:
      c_Retval = "setting_zm_y";
      break;
   default:
      break;
   }

   return c_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Transform chart zoom mode to string

   \param[in]  oe_VerticalAxisMode   Y axis mode

   \return
   Stringified zoom mode setting
*/
//----------------------------------------------------------------------------------------------------------------------
QString C_PuiSvDashboardFiler::mh_TabChartVerticalAxisModeToString(
   const C_PuiSvDbTabChart::E_SettingVerticalAxisMode oe_VerticalAxisMode)
{
   QString c_Retval;

   switch (oe_VerticalAxisMode)
   {
   case C_PuiSvDbTabChart::eSETTING_YA_ONE_VISIBLE:
      c_Retval = "setting_ya_one";
      break;
   case C_PuiSvDbTabChart::eSETTING_YA_ALL_VISIBLE:
      c_Retval = "setting_ya_all";
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
QString C_PuiSvDashboardFiler::mh_WriteModeToString(const C_PuiSvDbWidgetBase::E_WriteMode oe_Mode)
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
   default:
      break;
   }

   return c_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Transform source type to string

   \param[in]  oe_Type  Source type

   \return
   Stringified source type
*/
//----------------------------------------------------------------------------------------------------------------------
QString C_PuiSvDashboardFiler::mh_SourceTypeToString(const C_PuiSvDbNodeDataPoolListElementId::E_Type oe_Type)
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
/*! \brief   Transform string to display style

   \param[in]   orc_String    String to interpret
   \param[out]  ore_Style     Display style

   \return
   C_NO_ERR   no error
   C_RANGE    String unknown
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_PuiSvDashboardFiler::mh_StringToDisplayStyle(const QString & orc_String,
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
/*! \brief   Transform string to tab type

   \param[in]   orc_String    String to interpret
   \param[out]  ore_TabType   Tab type

   \return
   C_NO_ERR   no error
   C_RANGE    String unknown
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_PuiSvDashboardFiler::mh_StringToTabType(const QString & orc_String, C_PuiSvDashboard::E_TabType & ore_TabType)
{
   int32_t s32_Retval = C_NO_ERR;

   if (orc_String.compare("scene") == 0)
   {
      ore_TabType = C_PuiSvDashboard::eSCENE;
   }
   else if (orc_String.compare("chart") == 0)
   {
      ore_TabType = C_PuiSvDashboard::eCHART;
   }
   else
   {
      //Default
      ore_TabType = C_PuiSvDashboard::eSCENE;
      s32_Retval = C_RANGE;
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Transform string to toggle type

   \param[in]   orc_String    String to interpret
   \param[out]  ore_Type      Toggle type

   \return
   C_NO_ERR   no error
   C_RANGE    String unknown
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_PuiSvDashboardFiler::mh_StringToToggleType(const QString & orc_String, C_PuiSvDbToggle::E_Type & ore_Type)
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

   \param[in]   orc_String    String to interpret
   \param[out]  ore_Type      Spin box type

   \return
   C_NO_ERR   no error
   C_RANGE    String unknown
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_PuiSvDashboardFiler::mh_StringToSpinBoxType(const QString & orc_String, C_PuiSvDbSpinBox::E_Type & ore_Type)
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

   \param[in]   orc_String    String to interpret
   \param[out]  ore_Type      Label type

   \return
   C_NO_ERR   no error
   C_RANGE    String unknown
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_PuiSvDashboardFiler::mh_StringToLabelType(const QString & orc_String, C_PuiSvDbLabel::E_Type & ore_Type)
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

   \param[in]   orc_String    String to interpret
   \param[out]  ore_Type      Slider type

   \return
   C_NO_ERR   no error
   C_RANGE    String unknown
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_PuiSvDashboardFiler::mh_StringToSliderType(const QString & orc_String, C_PuiSvDbSlider::E_Type & ore_Type)
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

   \param[in]   orc_String    String to interpret
   \param[out]  ore_Type      Progress bar type

   \return
   C_NO_ERR   no error
   C_RANGE    String unknown
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_PuiSvDashboardFiler::mh_StringToProgressBarType(const QString & orc_String,
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

   \param[in]   orc_String       String to interpret
   \param[out]  ore_Alignment    Progress bar alignment type

   \return
   C_NO_ERR   no error
   C_RANGE    String unknown
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_PuiSvDashboardFiler::mh_StringToProgressBarAlignmentType(const QString & orc_String,
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
/*! \brief   Transform string to source type

   \param[in]   orc_String    String to interpret
   \param[out]  ore_ZoomMode  Zoom mode

   \return
   C_NO_ERR   no error
   C_RANGE    String unknown
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_PuiSvDashboardFiler::mh_StringToChartSettingZoomMode(const QString & orc_String,
                                                               C_PuiSvDbChart::E_SettingZoomMode & ore_ZoomMode)
{
   int32_t s32_Retval = C_NO_ERR;

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
/*! \brief   Transform string to source type

   \param[in]   orc_String    String to interpret
   \param[out]  ore_ZoomMode  Zoom mode

   \return
   C_NO_ERR   no error
   C_RANGE    String unknown
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_PuiSvDashboardFiler::mh_StringToTabChartSettingZoomMode(const QString & orc_String,
                                                                  C_PuiSvDbTabChart::E_SettingZoomMode & ore_ZoomMode)
{
   int32_t s32_Retval = C_NO_ERR;

   if (orc_String.compare("setting_zm_xy") == 0)
   {
      ore_ZoomMode = C_PuiSvDbTabChart::eSETTING_ZM_XY;
   }
   else if (orc_String.compare("setting_zm_x") == 0)
   {
      ore_ZoomMode = C_PuiSvDbTabChart::eSETTING_ZM_X;
   }
   else if (orc_String.compare("setting_zm_y") == 0)
   {
      ore_ZoomMode = C_PuiSvDbTabChart::eSETTING_ZM_Y;
   }
   else
   {
      //Default
      ore_ZoomMode = C_PuiSvDbTabChart::eSETTING_ZM_XY;
      s32_Retval = C_RANGE;
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Transform string to source type

   \param[in]   orc_String       String to interpret
   \param[out]  ore_VerticalAxisMode    Y axis mode

   \return
   C_NO_ERR   no error
   C_RANGE    String unknown
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_PuiSvDashboardFiler::mh_StringToTabChartSettingVerticalAxisMode(const QString & orc_String,
                                                                          C_PuiSvDbTabChart::E_SettingVerticalAxisMode & ore_VerticalAxisMode)
{
   int32_t s32_Retval = C_NO_ERR;

   if (orc_String.compare("setting_ya_one") == 0)
   {
      ore_VerticalAxisMode = C_PuiSvDbTabChart::eSETTING_YA_ONE_VISIBLE;
   }
   else if (orc_String.compare("setting_ya_all") == 0)
   {
      ore_VerticalAxisMode = C_PuiSvDbTabChart::eSETTING_YA_ALL_VISIBLE;
   }
   else
   {
      //Default
      ore_VerticalAxisMode = C_PuiSvDbTabChart::eSETTING_YA_ONE_VISIBLE;
      s32_Retval = C_RANGE;
   }
   return s32_Retval;
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
int32_t C_PuiSvDashboardFiler::mh_StringToWriteMode(const QString & orc_String,
                                                    C_PuiSvDbWidgetBase::E_WriteMode & ore_Mode)
{
   int32_t s32_Retval = C_NO_ERR;

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
/*! \brief   Transform string to source type

   \param[in]   orc_String    String to interpret
   \param[out]  ore_Type      Source type

   \return
   C_NO_ERR   no error
   C_RANGE    String unknown
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_PuiSvDashboardFiler::mh_StringToSourceType(const QString & orc_String,
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
