//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       System view filer (V2 format) (implementation)

   System view filer (V2 format)

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
#include "C_OscNodeDataPoolFiler.hpp"
#include "C_OscViewFiler.hpp"
#include "C_PuiSvHandlerFiler.hpp"
#include "C_PuiBsElementsFiler.hpp"
#include "C_PuiSdHandler.hpp"
#include "C_OscNode.hpp"
#include "C_OscLoggingHandler.hpp"
#include "C_OscSystemFilerUtil.hpp"
#include "C_PuiSvDashboardFiler.hpp"

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
C_PuiSvHandlerFiler::C_PuiSvHandlerFiler(void)
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Load system views

   \param[in,out]  orc_Views        System view elements (Cleared if necessary)
   \param[in]      orc_OscNodes     OSC node information (Necessary for update information)
   \param[in,out]  orc_XmlParser    XML parser with the "current" element set to the "opensyde-system-views" element
   \param[in]      opc_BasePath     Base path (Optional for save to string)

   \return
   C_NO_ERR    information loaded
   C_CONFIG    error loading information
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_PuiSvHandlerFiler::h_LoadViews(std::vector<C_PuiSvData> & orc_Views,
                                         const std::vector<stw::opensyde_core::C_OscNode> & orc_OscNodes,
                                         C_OscXmlParserBase & orc_XmlParser, const QDir * const opc_BasePath)
{
   int32_t s32_Retval = C_NO_ERR;

   if (orc_XmlParser.SelectNodeChild("opensyde-system-views") == "opensyde-system-views")
   {
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
            if (opc_BasePath != NULL)
            {
               const QString c_File = opc_BasePath->absoluteFilePath(orc_XmlParser.GetNodeContent().c_str());
               s32_Retval = mh_LoadViewFile(c_View, c_File, orc_OscNodes);
            }
            else
            {
               s32_Retval = mh_LoadView(c_View, orc_XmlParser, orc_OscNodes);
            }
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
            c_Tmp.PrintFormatted("Unexpected view count, expected: %u, got %zu", u32_ExpectedSize,
                                 orc_Views.size());
            osc_write_log_warning("Load file", c_Tmp.c_str());
         }
      }
      //Return
      tgl_assert(orc_XmlParser.SelectNodeParent() == "opensyde-system-views");
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

   \param[in]      orc_Views        System view elements
   \param[in,out]  orc_XmlParser    XML parser with the "current" element set to the "opensyde-system-views" element
   \param[in]      opc_BasePath     Base path (Optional for save to string)

   \return
   C_NO_ERR   data saved
   C_CONFIG   file could not be created
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_PuiSvHandlerFiler::h_SaveViews(const std::vector<C_PuiSvData> & orc_Views, C_OscXmlParserBase & orc_XmlParser,
                                         const QDir * const opc_BasePath)
{
   int32_t s32_Retval = C_NO_ERR;

   orc_XmlParser.CreateAndSelectNodeChild("opensyde-system-views");
   orc_XmlParser.SetAttributeUint32("length", orc_Views.size());
   for (uint32_t u32_ItView = 0; (u32_ItView < orc_Views.size()) && (s32_Retval == C_NO_ERR); ++u32_ItView)
   {
      orc_XmlParser.CreateAndSelectNodeChild("opensyde-system-view");
      if (opc_BasePath != NULL)
      {
         const C_PuiSvData & rc_View = orc_Views[u32_ItView];
         const QString c_FilePath = C_PuiSvHandlerFiler::h_GetViewFileName(rc_View.GetName().c_str());
         const QString c_CombinedFilePath = opc_BasePath->absoluteFilePath(c_FilePath);
         s32_Retval = mh_SaveViewFile(rc_View, c_CombinedFilePath);
         //Store file name
         orc_XmlParser.SetNodeContent(c_FilePath.toStdString().c_str());
      }
      else
      {
         mh_SaveView(orc_Views[u32_ItView], orc_XmlParser);
      }
      //Return
      tgl_assert(orc_XmlParser.SelectNodeParent() == "opensyde-system-views");
   }
   //Return
   tgl_assert(orc_XmlParser.SelectNodeParent() == "opensyde-system-views");
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Load read rails

   \param[in,out]  orc_Rails        Read rails
   \param[in,out]  orc_XmlParser    XML parser with the "current" element set to the "rail-assignments" element

   \return
   C_NO_ERR    information loaded
   C_CONFIG    error loading information
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_PuiSvHandlerFiler::h_LoadReadRails(QMap<C_OscNodeDataPoolListElementId,
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
            C_PuiSvHandlerFiler::mh_LoadDataElement(c_Id, orc_XmlParser);
            //Return
            tgl_assert(orc_XmlParser.SelectNodeParent() == "rail-assignment");
         }
         else
         {
            s32_Retval = C_CONFIG;
         }
         if (orc_XmlParser.SelectNodeChild("threshold") == "threshold")
         {
            if (C_OscNodeDataPoolFiler::h_LoadDataPoolContentV1(c_DataConfiguration.c_ChangeThreshold,
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
            if (C_PuiSvHandlerFiler::mh_StringToTransmissionMode(orc_XmlParser.GetNodeContent().c_str(),
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

   \param[in]      orc_Rails        Read rails
   \param[in,out]  orc_XmlParser    XML parser with the "current" element set to the "rail-assignments" element
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PuiSvHandlerFiler::h_SaveReadRails(const QMap<C_OscNodeDataPoolListElementId,
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
      C_OscNodeDataPoolFiler::h_SaveDataPoolContentV1(c_ReadData.c_ChangeThreshold, orc_XmlParser);
      //Return
      tgl_assert(orc_XmlParser.SelectNodeParent() == "rail-assignment");
      orc_XmlParser.CreateNodeChild("transmission-mode",
                                    C_PuiSvHandlerFiler::mh_TransmissionModeToString(
                                       c_ReadData.e_TransmissionMode).toStdString().c_str());
      //Return
      tgl_assert(orc_XmlParser.SelectNodeParent() == "rail-assignments");
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get automatically generated file name

   \param[in]  orc_ViewName   View name

   \return
   Automatically generated file name
*/
//----------------------------------------------------------------------------------------------------------------------
QString C_PuiSvHandlerFiler::h_GetViewFileName(const QString & orc_ViewName)
{
   return ("view_" + C_OscSystemFilerUtil::h_PrepareItemNameForFileName(
              orc_ViewName.toStdString().c_str()) +
           ".xml").c_str();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Load dashboard elements

   \param[in,out]  orc_Dashboards   Dashboard elements (Cleared if necessary)
   \param[in,out]  orc_XmlParser    XML parser with the "current" element set to the "opensyde-system-view" element

   \return
   C_NO_ERR    information loaded
   C_CONFIG    error loading information
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_PuiSvHandlerFiler::mh_LoadDashboards(std::vector<C_PuiSvDashboard> & orc_Dashboards,
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
            s32_Retval = C_PuiSvDashboardFiler::h_LoadDashboard(c_Dashboard, orc_XmlParser);
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
/*! \brief   Load view

   \param[in,out]  orc_View      View data storage
   \param[in]      orc_FilePath  File path
   \param[in]      orc_OscNodes  OSC node information (Necessary for update information)

   \return
   C_NO_ERR    information loaded
   C_CONFIG    error loading information
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_PuiSvHandlerFiler::mh_LoadViewFile(C_PuiSvData & orc_View, const QString & orc_FilePath,
                                             const std::vector<C_OscNode> & orc_OscNodes)
{
   C_OscXmlParser c_XmlParser;
   int32_t s32_Retval = C_OscSystemFilerUtil::h_GetParserForExistingFile(c_XmlParser,
                                                                         orc_FilePath.toStdString().c_str(),
                                                                         "opensyde-view-definition");

   //File version
   if (c_XmlParser.SelectNodeChild("file-version") == "file-version")
   {
      uint16_t u16_FileVersion = 0U;
      try
      {
         u16_FileVersion = static_cast<uint16_t>(c_XmlParser.GetNodeContent().ToInt());
      }
      catch (...)
      {
         osc_write_log_error("Loading view", "\"file-version\" could not be converted to a number.");
         s32_Retval = C_CONFIG;
      }

      //is the file version one we know ?
      if (s32_Retval == C_NO_ERR)
      {
         osc_write_log_info("Loading view", "Value of \"file-version\": " +
                            C_SclString::IntToStr(u16_FileVersion));
         //Check file version
         if ((u16_FileVersion != 1U) && (u16_FileVersion != 2U))
         {
            osc_write_log_error("Loading view",
                                "Version defined by \"file-version\" is not supported.");
            s32_Retval = C_CONFIG;
         }
      }

      //Return
      c_XmlParser.SelectNodeParent();
   }
   else
   {
      osc_write_log_error("Loading view", "Could not find \"file-version\" node.");
      s32_Retval = C_CONFIG;
   }

   if (s32_Retval == C_NO_ERR)
   {
      if (c_XmlParser.SelectNodeChild("opensyde-system-view") == "opensyde-system-view")
      {
         s32_Retval = C_PuiSvHandlerFiler::mh_LoadView(orc_View, c_XmlParser, orc_OscNodes);
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

   \param[in,out]  orc_View         View element
   \param[in,out]  orc_XmlParser    XML parser with the "current" element set to the "opensyde-system-view" element
   \param[in]      orc_OscNodes     OSC node information (Necessary for update information)

   \return
   C_NO_ERR    information loaded
   C_CONFIG    error loading information
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_PuiSvHandlerFiler::mh_LoadView(C_PuiSvData & orc_View, C_OscXmlParserBase & orc_XmlParser,
                                         const std::vector<C_OscNode> & orc_OscNodes)
{
   int32_t s32_Retval;

   // Fill core data
   s32_Retval = C_OscViewFiler::h_LoadViewOsc(orc_View, orc_XmlParser, orc_OscNodes);

   // Load GUI data
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
   if (orc_XmlParser.SelectNodeChild("service-mode") == "service-mode")
   {
      if (orc_XmlParser.AttributeExists("active"))
      {
         orc_View.SetServiceModeActive(orc_XmlParser.GetAttributeBool("active"));
      }
      else
      {
         s32_Retval = C_CONFIG;
         osc_write_log_error("Loading view",
                             "Attribute \"active\" not found in node \"service-mode\".");
      }
      if (orc_XmlParser.SelectNodeChild("availability") == "availability")
      {
         if (orc_XmlParser.AttributeExists("setup"))
         {
            orc_View.SetServiceModeSetupActive(orc_XmlParser.GetAttributeBool("setup"));
         }
         else
         {
            s32_Retval = C_CONFIG;
            osc_write_log_error("Loading view",
                                "Attribute \"setup\" not found in node \"availability\".");
         }
         if (orc_XmlParser.AttributeExists("update"))
         {
            orc_View.SetServiceModeUpdateActive(orc_XmlParser.GetAttributeBool("update"));
         }
         else
         {
            s32_Retval = C_CONFIG;
            osc_write_log_error("Loading view",
                                "Attribute \"update\" not found in node \"availability\".");
         }
         if (orc_XmlParser.AttributeExists("dashboard"))
         {
            orc_View.SetServiceModeDashboardActive(orc_XmlParser.GetAttributeBool("dashboard"));
         }
         else
         {
            s32_Retval = C_CONFIG;
            osc_write_log_error("Loading view",
                                "Attribute \"dashboard\" not found in node \"availability\".");
         }
         //Return
         tgl_assert(orc_XmlParser.SelectNodeParent() == "service-mode");
      }
      else
      {
         s32_Retval = C_CONFIG;
         osc_write_log_error("Loading view",
                             "Node \"availability\" not found in node \"service-mode\".");
      }
      tgl_assert(orc_XmlParser.SelectNodeParent() == "opensyde-system-view");
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
      if (orc_XmlParser.SelectNodeChild("pc") == "pc")
      {
         C_PuiSvPc c_PuiPcData;
         s32_Retval = mh_LoadPc(c_PuiPcData, orc_XmlParser);
         orc_View.SetPuiPcData(c_PuiPcData);
         //Return
         tgl_assert(orc_XmlParser.SelectNodeParent() == "opensyde-system-view");
      }
      else
      {
         osc_write_log_error("Loading view", "Node \"pc\" not found.");
         s32_Retval = C_CONFIG;
      }
   }
   if (s32_Retval == C_NO_ERR)
   {
      std::vector<C_PuiSvDashboard> c_Dashboards;
      s32_Retval = mh_LoadDashboards(c_Dashboards, orc_XmlParser);
      orc_View.SetDashboards(c_Dashboards);
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

   \param[in,out]  orc_PuiPc        PC element (GUI part)
   \param[in,out]  orc_XmlParser    XML parser with the "current" element set to the "pc" element

   \return
   C_NO_ERR    information loaded
   C_CONFIG    error loading information
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_PuiSvHandlerFiler::mh_LoadPc(C_PuiSvPc & orc_PuiPc, C_OscXmlParserBase & orc_XmlParser)
{
   int32_t s32_Retval = C_NO_ERR;

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
/*! \brief   Load data element id

   \param[in,out]  orc_Id           Data element id
   \param[in,out]  orc_XmlParser    XML parser with the "current" element set to any element
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PuiSvHandlerFiler::mh_LoadDataElement(C_OscNodeDataPoolListElementId & orc_Id,
                                             const C_OscXmlParserBase & orc_XmlParser)
{
   orc_Id.u32_NodeIndex = orc_XmlParser.GetAttributeUint32("node");
   orc_Id.u32_DataPoolIndex = orc_XmlParser.GetAttributeUint32("data-pool");
   orc_Id.u32_ListIndex = orc_XmlParser.GetAttributeUint32("list");
   orc_Id.u32_ElementIndex = orc_XmlParser.GetAttributeUint32("element");
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Transform string to transmission mode

   \param[in]   orc_String    String to interpret
   \param[out]  ore_Mode      Mode

   \return
   C_NO_ERR   no error
   C_RANGE    String unknown
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_PuiSvHandlerFiler::mh_StringToTransmissionMode(const QString & orc_String,
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
/*! \brief   Save dashboard elements

   \param[in]      orc_Dashboards   Dashboard elements
   \param[in,out]  orc_XmlParser    XML parser with the "current" element set to the "opensyde-system-view" element
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PuiSvHandlerFiler::mh_SaveDashboards(const std::vector<C_PuiSvDashboard> & orc_Dashboards,
                                            C_OscXmlParserBase & orc_XmlParser)
{
   orc_XmlParser.CreateAndSelectNodeChild("dashboards");
   for (uint32_t u32_ItDashboard = 0; u32_ItDashboard < orc_Dashboards.size(); ++u32_ItDashboard)
   {
      orc_XmlParser.CreateAndSelectNodeChild("dashboard");
      C_PuiSvDashboardFiler::h_SaveDashboard(orc_Dashboards[u32_ItDashboard], orc_XmlParser);
      //Return
      tgl_assert(orc_XmlParser.SelectNodeParent() == "dashboards");
   }
   //Return
   tgl_assert(orc_XmlParser.SelectNodeParent() == "opensyde-system-view");
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Save view

   Save view to XML file

   \param[in]      orc_View      View data storage
   \param[in,out]  orc_FilePath  File path for xml

   \return
   C_NO_ERR   data saved
   C_CONFIG   file could not be created
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_PuiSvHandlerFiler::mh_SaveViewFile(const C_PuiSvData & orc_View, const QString & orc_FilePath)
{
   C_OscXmlParser c_XmlParser;
   int32_t s32_Retval = C_OscSystemFilerUtil::h_GetParserForNewFile(c_XmlParser, orc_FilePath.toStdString().c_str(),
                                                                    "opensyde-view-definition");

   if (s32_Retval == C_NO_ERR)
   {
      //Version
      c_XmlParser.CreateNodeChild("file-version", "1");
      tgl_assert(c_XmlParser.CreateAndSelectNodeChild("opensyde-system-view") == "opensyde-system-view");
      //node
      C_PuiSvHandlerFiler::mh_SaveView(orc_View, c_XmlParser);
      //Don't forget to save!
      if (c_XmlParser.SaveToFile(orc_FilePath.toStdString().c_str()) != C_NO_ERR)
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

   \param[in]      orc_View         View element
   \param[in,out]  orc_XmlParser    XML parser with the "current" element set to the "opensyde-system-view" element
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PuiSvHandlerFiler::mh_SaveView(const C_PuiSvData & orc_View, C_OscXmlParserBase & orc_XmlParser)
{
   orc_XmlParser.SetAttributeBool("darkmode", orc_View.GetDarkModeActive());
   orc_XmlParser.SetAttributeUint32("device-config-selected-bit-rate", orc_View.GetDeviceConfigSelectedBitRate());
   orc_XmlParser.CreateAndSelectNodeChild("name");
   orc_XmlParser.SetNodeContent(orc_View.GetName());
   //Return
   tgl_assert(orc_XmlParser.SelectNodeParent() == "opensyde-system-view");
   //Service mode
   tgl_assert(orc_XmlParser.CreateAndSelectNodeChild("service-mode") == "service-mode");
   orc_XmlParser.SetAttributeBool("active", orc_View.GetServiceModeActive());
   tgl_assert(orc_XmlParser.CreateAndSelectNodeChild("availability") == "availability");
   orc_XmlParser.SetAttributeBool("setup", orc_View.GetServiceModeSetupActive());
   orc_XmlParser.SetAttributeBool("update", orc_View.GetServiceModeUpdateActive());
   orc_XmlParser.SetAttributeBool("dashboard", orc_View.GetServiceModeDashboardActive());
   //Return
   tgl_assert(orc_XmlParser.SelectNodeParent() == "service-mode");
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
   C_OscViewFiler::h_SaveNodeActiveFlags(orc_View.GetNodeActiveFlags(), orc_XmlParser);
   C_OscViewFiler::h_SaveNodeUpdateInformation(orc_View.GetAllNodeUpdateInformation(), orc_XmlParser);
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

   \param[in]      orc_OscPc        PC element (core part)
   \param[in]      orc_PuiPc        PC element (GUI part)
   \param[in,out]  orc_XmlParser    XML parser with the "current" element set to the "pc" element
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PuiSvHandlerFiler::mh_SavePc(const C_OscViewPc & orc_OscPc, const C_PuiSvPc & orc_PuiPc,
                                    C_OscXmlParserBase & orc_XmlParser)
{
   C_OscViewFiler::h_SavePc(orc_OscPc, orc_XmlParser);

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
/*! \brief   Save ID

   \param[in]      orc_Id           ID
   \param[in,out]  orc_XmlParser    XML parser with the "current" element set to the any element
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PuiSvHandlerFiler::mh_SaveDataElement(const C_OscNodeDataPoolListElementId & orc_Id,
                                             C_OscXmlParserBase & orc_XmlParser)
{
   orc_XmlParser.SetAttributeUint32("node", orc_Id.u32_NodeIndex);
   orc_XmlParser.SetAttributeUint32("data-pool", orc_Id.u32_DataPoolIndex);
   orc_XmlParser.SetAttributeUint32("list", orc_Id.u32_ListIndex);
   orc_XmlParser.SetAttributeUint32("element", orc_Id.u32_ElementIndex);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Transform device config mode to string

   \param[in]  oe_Mode  Device config mode

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
   default:
      break;
   }

   return c_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Transform transmission mode to string

   \param[in]  oe_Mode  Transmission mode

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
   default:
      break;
   }

   return c_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Interpret device config mode string

   \param[in]  orc_Input   String to interpret

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
