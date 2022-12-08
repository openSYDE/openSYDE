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
            c_Tmp.PrintFormatted("Unexpected view count, expected: %i, got %i", u32_ExpectedSize,
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
         const QString c_FilePath = C_PuiSvHandlerFiler::h_GetViewFileName(rc_View.GetName());
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
/*! \brief  Load last known halc crcs

   \param[in,out]  orc_Crcs         Crcs
   \param[in,out]  orc_XmlParser    XML parser

   \return
   C_NO_ERR    information loaded
   C_CONFIG    error loading information
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_PuiSvHandlerFiler::h_LoadLastKnownHalcCrcs(std::map<C_PuiSvDbNodeDataPoolListElementId,
                                                              C_PuiSvLastKnownHalElementId> & orc_Crcs,
                                                     C_OscXmlParserBase & orc_XmlParser)
{
   int32_t s32_Retval = C_NO_ERR;

   orc_Crcs.clear();
   if (orc_XmlParser.SelectNodeChild("last-known-halc-crcs") == "last-known-halc-crcs")
   {
      C_SclString c_CurrentNode = orc_XmlParser.SelectNodeChild("last-known-halc-crc");

      if (c_CurrentNode == "last-known-halc-crc")
      {
         do
         {
            C_PuiSvDbNodeDataPoolListElementId c_Id;
            C_PuiSvDashboardFiler::h_LoadUiIndex(c_Id, orc_XmlParser);
            if (orc_XmlParser.AttributeExists("crc"))
            {
               const uint32_t u32_Crc = orc_XmlParser.GetAttributeUint32("crc");
               QString c_DpName;
               if (orc_XmlParser.SelectNodeChild("hal-data-pool-name") == "hal-data-pool-name")
               {
                  c_DpName = orc_XmlParser.GetNodeContent().c_str();
                  //Return
                  tgl_assert(orc_XmlParser.SelectNodeParent() == "last-known-halc-crc");
               }
               else
               {
                  const C_OscNodeDataPool * const pc_Dp = C_PuiSdHandler::h_GetInstance()->GetOscDataPool(
                     c_Id.u32_NodeIndex, c_Id.u32_DataPoolIndex);
                  if (pc_Dp != NULL)
                  {
                     c_DpName = pc_Dp->c_Name.c_str();
                  }
               }
               //Insert
               orc_Crcs[c_Id] = C_PuiSvLastKnownHalElementId(u32_Crc, c_DpName);
            }
            else
            {
               s32_Retval = C_CONFIG;
            }
            //Next
            c_CurrentNode = orc_XmlParser.SelectNodeNext("last-known-halc-crc");
         }
         while ((c_CurrentNode == "last-known-halc-crc") && (s32_Retval == C_NO_ERR));
         //Return
         tgl_assert(orc_XmlParser.SelectNodeParent() == "last-known-halc-crcs");
      }
      //Return
      tgl_assert(orc_XmlParser.SelectNodeParent() == "opensyde-system-views");
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Save last known halc crcs

   \param[in]      orc_Crcs         Crcs
   \param[in,out]  orc_XmlParser    XML parser
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PuiSvHandlerFiler::h_SaveLastKnownHalcCrcs(const std::map<C_PuiSvDbNodeDataPoolListElementId,
                                                                 C_PuiSvLastKnownHalElementId> & orc_Crcs,
                                                  C_OscXmlParserBase & orc_XmlParser)
{
   orc_XmlParser.CreateAndSelectNodeChild("last-known-halc-crcs");
   for (std::map<C_PuiSvDbNodeDataPoolListElementId, C_PuiSvLastKnownHalElementId>::const_iterator c_It =
           orc_Crcs.begin();
        c_It != orc_Crcs.end(); ++c_It)
   {
      orc_XmlParser.CreateAndSelectNodeChild("last-known-halc-crc");
      C_PuiSvDashboardFiler::h_SaveUiIndex(c_It->first, orc_XmlParser);
      orc_XmlParser.SetAttributeUint32("crc", c_It->second.u32_Crc);
      orc_XmlParser.CreateNodeChild("hal-data-pool-name", c_It->second.c_HalDpName.toStdString().c_str());

      //Return
      tgl_assert(orc_XmlParser.SelectNodeParent() == "last-known-halc-crcs");
   }
   //Return
   tgl_assert(orc_XmlParser.SelectNodeParent() == "opensyde-system-views");
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
/*! \brief  String to pem file state debugger

   \param[in]   orc_String    String
   \param[out]  ore_State     State

   \return
   C_NO_ERR   no error
   C_RANGE    String unknown
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_PuiSvHandlerFiler::h_StringToPemFileStateDebugger(const QString & orc_String,
                                                            C_PuiSvNodeUpdate::E_StateDebugger & ore_State)
{
   int32_t s32_Retval = C_NO_ERR;

   if (orc_String.compare("activate") == 0)
   {
      ore_State = C_PuiSvNodeUpdate::eST_DEB_ACTIVATE;
   }
   else if (orc_String.compare("deactivate") == 0)
   {
      ore_State = C_PuiSvNodeUpdate::eST_DEB_DEACTIVATE;
   }
   else if (orc_String.compare("no-change") == 0)
   {
      ore_State = C_PuiSvNodeUpdate::eST_DEB_NO_CHANGE;
   }
   else
   {
      //Default
      ore_State = C_PuiSvNodeUpdate::eST_DEB_NO_CHANGE;
      s32_Retval = C_RANGE;
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  String to pem file state security

   \param[in]   orc_String    String
   \param[out]  ore_State     State

   \return
   C_NO_ERR   no error
   C_RANGE    String unknown
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_PuiSvHandlerFiler::h_StringToPemFileStateSecurity(const QString & orc_String,
                                                            C_PuiSvNodeUpdate::E_StateSecurity & ore_State)
{
   int32_t s32_Retval = C_NO_ERR;

   if (orc_String.compare("activate") == 0)
   {
      ore_State = C_PuiSvNodeUpdate::eST_SEC_ACTIVATE;
   }
   else if (orc_String.compare("deactivate") == 0)
   {
      ore_State = C_PuiSvNodeUpdate::eST_SEC_DEACTIVATE;
   }
   else if (orc_String.compare("no-change") == 0)
   {
      ore_State = C_PuiSvNodeUpdate::eST_SEC_NO_CHANGE;
   }
   else
   {
      //Default
      ore_State = C_PuiSvNodeUpdate::eST_SEC_NO_CHANGE;
      s32_Retval = C_RANGE;
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Pem file state debugger to string

   \param[in]  oe_State    State

   \return
   Stringified pem file state debugger to string
*/
//----------------------------------------------------------------------------------------------------------------------
QString C_PuiSvHandlerFiler::h_PemFileStateDebuggerToString(const C_PuiSvNodeUpdate::E_StateDebugger oe_State)
{
   QString c_Retval;

   switch (oe_State)
   {
   case C_PuiSvNodeUpdate::eST_DEB_ACTIVATE:
      c_Retval = "activate";
      break;
   case C_PuiSvNodeUpdate::eST_DEB_DEACTIVATE:
      c_Retval = "deactivate";
      break;
   case C_PuiSvNodeUpdate::eST_DEB_NO_CHANGE:
      c_Retval = "no-change";
      break;
   default:
      break;
   }

   return c_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Pem file state security to string

   \param[in]  oe_State    State

   \return
   Stringified pem file state security to string
*/
//----------------------------------------------------------------------------------------------------------------------
QString C_PuiSvHandlerFiler::h_PemFileStateSecurityToString(const C_PuiSvNodeUpdate::E_StateSecurity oe_State)
{
   QString c_Retval;

   switch (oe_State)
   {
   case C_PuiSvNodeUpdate::eST_SEC_ACTIVATE:
      c_Retval = "activate";
      break;
   case C_PuiSvNodeUpdate::eST_SEC_DEACTIVATE:
      c_Retval = "deactivate";
      break;
   case C_PuiSvNodeUpdate::eST_SEC_NO_CHANGE:
      c_Retval = "no-change";
      break;
   default:
      break;
   }

   return c_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Load node active flags

   \param[in,out]  orc_NodeActiveFlags    Node active flags (Cleared if necessary)
   \param[in,out]  orc_XmlParser          XML parser with the "current" element set to the "opensyde-system-view" element

   \return
   C_NO_ERR    information loaded
   C_CONFIG    error loading information
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_PuiSvHandlerFiler::mh_LoadNodeActiveFlags(std::vector<uint8_t> & orc_NodeActiveFlags,
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

   \param[in,out]  orc_NodeUpdateInformation    Node update information (Cleared if necessary)
   \param[in,out]  orc_XmlParser                XML parser with the "current" element set
                                                to the "opensyde-system-view" element
   \param[in]      orc_OscNodes                 OSC node information (Necessary for update information)

   \return
   C_NO_ERR    information loaded
   C_CONFIG    error loading information
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_PuiSvHandlerFiler::mh_LoadNodeUpdateInformation(std::vector<C_PuiSvNodeUpdate> & orc_NodeUpdateInformation,
                                                          C_OscXmlParserBase & orc_XmlParser,
                                                          const std::vector<C_OscNode> & orc_OscNodes)
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
            if (u32_Counter < orc_OscNodes.size())
            {
               const C_OscNode & rc_Node = orc_OscNodes[u32_Counter];
               C_PuiSvNodeUpdate c_UpdateInfo;

               s32_Retval = mh_LoadOneNodeUpdateInformation(c_UpdateInfo, orc_XmlParser, rc_Node);
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
               if (u32_Counter < orc_OscNodes.size())
               {
                  const C_OscNode & rc_Node = orc_OscNodes[u32_Counter];
                  C_PuiSvNodeUpdate c_UpdateInfo;
                  s32_Retval = mh_LoadOneNodeUpdateInformation(c_UpdateInfo, orc_XmlParser, rc_Node);
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
/*! \brief   Load any update information path

   \param[out]     orc_Paths           Read paths
   \param[in]      orc_XmlTagBaseName  XML tag base name (parent with s and children as is)
   \param[in,out]  orc_XmlParser       XML parser
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PuiSvHandlerFiler::mh_LoadNodeUpdateInformationPaths(std::vector<QString> & orc_Paths,
                                                            const QString & orc_XmlTagBaseName,
                                                            C_OscXmlParserBase & orc_XmlParser)
{
   const C_SclString c_ParentName = static_cast<QString>(orc_XmlTagBaseName + "s").toStdString().c_str();
   const C_SclString c_ChildName = orc_XmlTagBaseName.toStdString().c_str();

   orc_Paths.clear();
   if (orc_XmlParser.SelectNodeChild(c_ParentName) == c_ParentName)
   {
      C_SclString c_CurrentPathNode = orc_XmlParser.SelectNodeChild(c_ChildName);
      if (c_CurrentPathNode == c_ChildName)
      {
         do
         {
            const QString c_Content = orc_XmlParser.GetNodeContent().c_str();
            orc_Paths.push_back(c_Content);
            //Next
            c_CurrentPathNode = orc_XmlParser.SelectNodeNext(c_ChildName);
         }
         while (c_CurrentPathNode == c_ChildName);
         //Return
         tgl_assert(orc_XmlParser.SelectNodeParent() == c_ParentName);
      }
      //Return
      orc_XmlParser.SelectNodeParent();
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Load node update information

   \param[in,out]  orc_NodeUpdateInformation    Node update information
   \param[in,out]  orc_XmlParser                XML parser with the "current" element set
                                                to the "node-update-information" or
                                                "node-specific-update-information" element
   \param[in]      orc_Node                     Node

   \return
   C_NO_ERR    information loaded
   C_CONFIG    error loading information
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_PuiSvHandlerFiler::mh_LoadOneNodeUpdateInformation(C_PuiSvNodeUpdate & orc_NodeUpdateInformation,
                                                             C_OscXmlParserBase & orc_XmlParser,
                                                             const C_OscNode & orc_Node)
{
   std::vector<QString> c_Paths;
   std::vector<bool> c_SkipFlags;
   int32_t s32_Retval = C_NO_ERR;

   if (orc_XmlParser.AttributeExists("position") == true)
   {
      orc_NodeUpdateInformation.u32_NodeUpdatePosition = orc_XmlParser.GetAttributeUint32("position");
   }
   else
   {
      orc_NodeUpdateInformation.u32_NodeUpdatePosition = 0;
   }

   //Previous format (assuming these can only be data blocks!)
   if (orc_XmlParser.SelectNodeChild("paths") == "paths")
   {
      C_SclString c_CurrentNodeUpdateInformationNode = orc_XmlParser.SelectNodeChild("path");
      if (c_CurrentNodeUpdateInformationNode == "path")
      {
         do
         {
            c_Paths.push_back(orc_XmlParser.GetNodeContent().c_str());
            //Next
            c_CurrentNodeUpdateInformationNode = orc_XmlParser.SelectNodeNext("path");
         }
         while (c_CurrentNodeUpdateInformationNode == "path");
         //Return
         tgl_assert(orc_XmlParser.SelectNodeParent() == "paths");
      }

      // Adapt size for skip flags for compatibility
      c_SkipFlags.resize(orc_Node.c_Applications.size(), false);

      if (orc_Node.c_Applications.size() == c_Paths.size())
      {
         //matching size
         orc_NodeUpdateInformation.SetPaths(c_Paths, C_PuiSvNodeUpdate::eFTP_DATA_BLOCK);
         orc_NodeUpdateInformation.SetSkipUpdateOfPathsFlags(c_SkipFlags, C_PuiSvNodeUpdate::eFTP_DATA_BLOCK);
      }
      else
      {
         //invalid size
         c_Paths.clear();
         c_Paths.resize(orc_Node.c_Applications.size(), "");
         orc_NodeUpdateInformation.SetPaths(c_Paths, C_PuiSvNodeUpdate::eFTP_DATA_BLOCK);
         orc_NodeUpdateInformation.SetSkipUpdateOfPathsFlags(c_SkipFlags, C_PuiSvNodeUpdate::eFTP_DATA_BLOCK);
      }
      //Return
      orc_XmlParser.SelectNodeParent();
   }
   else
   {
      std::vector<C_PuiSvNodeUpdateParamInfo> c_ParamInfo;
      std::vector<QString> c_FileBasedPaths;
      std::vector<bool> c_PathSkipFlags;
      std::vector<bool> c_ParamSetSkipFlags;
      std::vector<bool> c_FileBasedSkipFlags;

      //New format
      C_PuiSvHandlerFiler::mh_LoadNodeUpdateInformationPaths(c_Paths, "data-block-path", orc_XmlParser);
      orc_NodeUpdateInformation.SetPaths(c_Paths, C_PuiSvNodeUpdate::eFTP_DATA_BLOCK);

      C_PuiSvHandlerFiler::mh_LoadNodeUpdateInformationParam(c_ParamInfo, orc_XmlParser);
      orc_NodeUpdateInformation.SetParamInfos(c_ParamInfo);

      C_PuiSvHandlerFiler::mh_LoadNodeUpdateInformationPaths(c_FileBasedPaths, "file-based-path", orc_XmlParser);
      orc_NodeUpdateInformation.SetPaths(c_FileBasedPaths, C_PuiSvNodeUpdate::eFTP_FILE_BASED);

      // Handling of the skip flags
      C_PuiSvHandlerFiler::mh_LoadNodeUpdateInformationSkipUpdateOfFiles(c_SkipFlags, orc_XmlParser);
      // Check for a correct loading of the flags
      if (c_SkipFlags.size() == (c_Paths.size() + c_ParamInfo.size() + c_FileBasedPaths.size()))
      {
         if (c_Paths.size() > 0)
         {
            std::copy(c_SkipFlags.begin(), c_SkipFlags.begin() + c_Paths.size(), std::back_inserter(c_PathSkipFlags));
            c_SkipFlags.erase(c_SkipFlags.begin(), c_SkipFlags.begin() + c_Paths.size());
         }

         if (c_ParamInfo.size() > 0)
         {
            std::copy(c_SkipFlags.begin(), c_SkipFlags.begin() + c_ParamInfo.size(),
                      std::back_inserter(c_ParamSetSkipFlags));
            c_SkipFlags.erase(c_SkipFlags.begin(), c_SkipFlags.begin() + c_Paths.size());
         }

         if (c_FileBasedPaths.size() > 0)
         {
            std::copy(c_SkipFlags.begin(), c_SkipFlags.begin() + c_FileBasedPaths.size(),
                      std::back_inserter(c_FileBasedSkipFlags));
         }
      }
      else
      {
         // Adapt for compatibility
         c_PathSkipFlags.resize(c_Paths.size(), false);
         c_ParamSetSkipFlags.resize(c_ParamInfo.size(), false);
         c_FileBasedSkipFlags.resize(c_FileBasedPaths.size(), false);
      }

      orc_NodeUpdateInformation.SetSkipUpdateOfPathsFlags(c_PathSkipFlags, C_PuiSvNodeUpdate::eFTP_DATA_BLOCK);
      orc_NodeUpdateInformation.SetSkipUpdateOfParamInfosFlags(c_ParamSetSkipFlags);
      orc_NodeUpdateInformation.SetSkipUpdateOfPathsFlags(c_FileBasedSkipFlags, C_PuiSvNodeUpdate::eFTP_FILE_BASED);

      s32_Retval = C_PuiSvHandlerFiler::mh_LoadNodeUpdateInformationPem(orc_NodeUpdateInformation, orc_XmlParser);
   }

   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Load node parameter set update information

   \param[out]     orc_Info         Parsed parameter set update information
   \param[in,out]  orc_XmlParser    XML parser with the "current" element set
                                    to the "node-update-information" or
                                    "node-specific-update-information" element
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PuiSvHandlerFiler::mh_LoadNodeUpdateInformationParam(std::vector<C_PuiSvNodeUpdateParamInfo> & orc_Info,
                                                            C_OscXmlParserBase & orc_XmlParser)
{
   orc_Info.clear();
   if (orc_XmlParser.SelectNodeChild("param-sets") == "param-sets")
   {
      C_SclString c_CurrentPathNode = orc_XmlParser.SelectNodeChild("param-set");
      if (c_CurrentPathNode == "param-set")
      {
         do
         {
            bool q_Error = false;
            QString c_Path;
            uint32_t u32_LastKnownCrc;
            C_PuiSvNodeUpdateParamInfo c_Content;
            if (orc_XmlParser.SelectNodeChild("path") == "path")
            {
               c_Path = orc_XmlParser.GetNodeContent().c_str();
               //Return
               tgl_assert(orc_XmlParser.SelectNodeParent() == "param-set");
            }
            else
            {
               q_Error = true;
            }
            if (orc_XmlParser.AttributeExists("last-known-crc"))
            {
               u32_LastKnownCrc = orc_XmlParser.GetAttributeUint32("last-known-crc");
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
            c_CurrentPathNode = orc_XmlParser.SelectNodeNext("param-set");
         }
         while (c_CurrentPathNode == "param-set");
         //Return
         tgl_assert(orc_XmlParser.SelectNodeParent() == "param-sets");
      }
      //Return
      orc_XmlParser.SelectNodeParent();
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Load node skip of file update flags update information

   \param[out]     orc_Flags        Parsed skip flags
   \param[in,out]  orc_XmlParser    XML parser with the "current" element set
                                    to the "node-update-information" or
                                    "node-specific-update-information" element
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PuiSvHandlerFiler::mh_LoadNodeUpdateInformationSkipUpdateOfFiles(std::vector<bool> & orc_Flags,
                                                                        C_OscXmlParserBase & orc_XmlParser)
{
   // No error in case of not existing flags due to compatibility
   orc_Flags.clear();
   if (orc_XmlParser.SelectNodeChild("skip-update-of-files") == "skip-update-of-files")
   {
      C_SclString c_CurrentNodeActiveFlagNode = orc_XmlParser.SelectNodeChild("skip-update-of-file");
      if (c_CurrentNodeActiveFlagNode == "skip-update-of-file")
      {
         do
         {
            const bool q_CurrentValue = orc_XmlParser.GetAttributeBool("state");
            orc_Flags.push_back(q_CurrentValue);
            //Next
            c_CurrentNodeActiveFlagNode = orc_XmlParser.SelectNodeNext("skip-update-of-file");
         }
         while (c_CurrentNodeActiveFlagNode == "skip-update-of-file");
         //Return
         tgl_assert(orc_XmlParser.SelectNodeParent() == "skip-update-of-files");
      }
      //Return
      orc_XmlParser.SelectNodeParent();
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Load node update information pem

   \param[in,out]  orc_NodeUpdateInformation    Node update information
   \param[in,out]  orc_XmlParser                XML parser

   \return
   STW error codes

   \retval   C_NO_ERR   Information loaded
   \retval   C_CONFIG   Error loading information
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_PuiSvHandlerFiler::mh_LoadNodeUpdateInformationPem(C_PuiSvNodeUpdate & orc_NodeUpdateInformation,
                                                             C_OscXmlParserBase & orc_XmlParser)
{
   int32_t s32_Retval = C_NO_ERR;

   if (orc_XmlParser.SelectNodeChild("pem-file") == "pem-file")
   {
      bool q_Value;
      s32_Retval = orc_XmlParser.GetAttributeBoolError("skip", q_Value);
      if (s32_Retval == C_NO_ERR)
      {
         orc_NodeUpdateInformation.SetSkipUpdateOfPemFile(q_Value);

         s32_Retval = orc_XmlParser.SelectNodeChildError("path");
         if (s32_Retval == C_NO_ERR)
         {
            orc_NodeUpdateInformation.SetPemFilePath(orc_XmlParser.GetNodeContent().c_str());
            //Return
            tgl_assert(orc_XmlParser.SelectNodeParent() == "pem-file");
         }
      }
      if (s32_Retval == C_NO_ERR)
      {
         s32_Retval = C_PuiSvHandlerFiler::mh_LoadNodeUpdateInformationPemStates(orc_NodeUpdateInformation,
                                                                                 orc_XmlParser);
      }
      if (s32_Retval == C_NO_ERR)
      {
         //Return
         tgl_assert(orc_XmlParser.SelectNodeParent() == "node-specific-update-information");
      }
   }

   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Load node update information pem states

   \param[in,out]  orc_NodeUpdateInformation    Node update information
   \param[in,out]  orc_XmlParser                XML parser

   \return
   STW error codes

   \retval   C_NO_ERR   Information loaded
   \retval   C_CONFIG   Error loading information
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_PuiSvHandlerFiler::mh_LoadNodeUpdateInformationPemStates(C_PuiSvNodeUpdate & orc_NodeUpdateInformation,
                                                                   C_OscXmlParserBase & orc_XmlParser)
{
   int32_t s32_Retval = orc_XmlParser.SelectNodeChildError("states");

   if (s32_Retval == C_NO_ERR)
   {
      C_PuiSvNodeUpdate::E_StateSecurity e_StateSecurity = C_PuiSvNodeUpdate::eST_SEC_NO_CHANGE;
      C_PuiSvNodeUpdate::E_StateDebugger e_StateDebugger = C_PuiSvNodeUpdate::eST_DEB_NO_CHANGE;
      s32_Retval = orc_XmlParser.SelectNodeChildError("security");
      if (s32_Retval == C_NO_ERR)
      {
         s32_Retval = C_PuiSvHandlerFiler::h_StringToPemFileStateSecurity(
            orc_XmlParser.GetNodeContent().c_str(), e_StateSecurity);
         if (s32_Retval == C_NO_ERR)
         {
            //Return
            tgl_assert(orc_XmlParser.SelectNodeParent() == "states");
         }
      }
      if (s32_Retval == C_NO_ERR)
      {
         s32_Retval = orc_XmlParser.SelectNodeChildError("debugger");
         if (s32_Retval == C_NO_ERR)
         {
            s32_Retval = C_PuiSvHandlerFiler::h_StringToPemFileStateDebugger(
               orc_XmlParser.GetNodeContent().c_str(), e_StateDebugger);
            if (s32_Retval == C_NO_ERR)
            {
               //Return
               tgl_assert(orc_XmlParser.SelectNodeParent() == "states");
            }
         }
      }
      if (s32_Retval == C_NO_ERR)
      {
         orc_NodeUpdateInformation.SetStates(e_StateSecurity, e_StateDebugger);

         //Return
         tgl_assert(orc_XmlParser.SelectNodeParent() == "pem-file");
      }
   }
   return s32_Retval;
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
         std::vector<C_PuiSvNodeUpdate> c_NodeUpdateInformation;
         //If you have an async project this might help
         //c_NodeUpdateInformation.resize(c_NodeActiveFlags.size(),C_PuiSvNodeUpdate());
         s32_Retval = mh_LoadNodeUpdateInformation(c_NodeUpdateInformation, orc_XmlParser, orc_OscNodes);
         orc_View.SetNodeUpdateInformation(c_NodeUpdateInformation);
         if (s32_Retval == C_NO_ERR)
         {
            if (orc_XmlParser.SelectNodeChild("pc") == "pc")
            {
               C_PuiSvPc c_PcData;
               s32_Retval = mh_LoadPc(c_PcData, orc_XmlParser);
               orc_View.SetPcData(c_PcData);
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

   \param[in,out]  orc_Pc           PC element
   \param[in,out]  orc_XmlParser    XML parser with the "current" element set to the "pc" element

   \return
   C_NO_ERR    information loaded
   C_CONFIG    error loading information
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_PuiSvHandlerFiler::mh_LoadPc(C_PuiSvPc & orc_Pc, C_OscXmlParserBase & orc_XmlParser)
{
   int32_t s32_Retval = C_NO_ERR;

   orc_Pc.SetConnected(orc_XmlParser.GetAttributeBool("connected"), orc_XmlParser.GetAttributeUint32("bus-index"),
                       true);
   if (orc_XmlParser.SelectNodeChild("dll-path") == "dll-path")
   {
      QString c_Path = orc_XmlParser.GetNodeContent().c_str();
      if (orc_XmlParser.AttributeExists("type"))
      {
         orc_Pc.SetCanDllType(static_cast<C_PuiSvPc::E_CanDllType>(orc_XmlParser.GetAttributeSint32("type")));
      }
      else
      {
         // translate from path to type+path for compatibility reasons (type was not present in old openSYDE versions)
         if ((c_Path == stw::opensyde_gui::mc_DLL_PATH_PEAK) || (c_Path.isEmpty() == true))
         {
            orc_Pc.SetCanDllType(C_PuiSvPc::E_CanDllType::ePEAK);
            c_Path = "";
         }
         else if (c_Path == stw::opensyde_gui::mc_DLL_PATH_VECTOR)
         {
            orc_Pc.SetCanDllType(C_PuiSvPc::E_CanDllType::eVECTOR);
            c_Path = "";
         }
         else
         {
            orc_Pc.SetCanDllType(C_PuiSvPc::E_CanDllType::eOTHER);
         }
      }
      orc_Pc.SetCustomCanDllPath(c_Path);
      //Return
      tgl_assert(orc_XmlParser.SelectNodeParent() == "pc");
   }
   else
   {
      s32_Retval = C_CONFIG;
   }
   if ((orc_XmlParser.SelectNodeChild("box") == "box") && (s32_Retval == C_NO_ERR))
   {
      s32_Retval = C_PuiBsElementsFiler::h_LoadBoxBase(orc_Pc, orc_XmlParser);
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
         orc_Pc.SetConnectionData(c_Connection);
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
/*! \brief   Save node active flags

   \param[in]      orc_NodeActiveFlags    Node active flags
   \param[in,out]  orc_XmlParser          XML parser with the "current" element set to the "opensyde-system-view" element
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PuiSvHandlerFiler::mh_SaveNodeActiveFlags(const std::vector<uint8_t> & orc_NodeActiveFlags,
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

   \param[in]      orc_NodeUpdateInformation    Node update information
   \param[in,out]  orc_XmlParser                XML parser with the "current" element set
                                                to the "opensyde-system-view" element
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PuiSvHandlerFiler::mh_SaveNodeUpdateInformation(const std::vector<C_PuiSvNodeUpdate> & orc_NodeUpdateInformation,
                                                       C_OscXmlParserBase & orc_XmlParser)
{
   orc_XmlParser.CreateAndSelectNodeChild("node-update-information");
   for (uint32_t u32_ItNodeActiveFlag = 0; u32_ItNodeActiveFlag < orc_NodeUpdateInformation.size();
        ++u32_ItNodeActiveFlag)
   {
      const C_PuiSvNodeUpdate & rc_NodeUpdateInformation = orc_NodeUpdateInformation[u32_ItNodeActiveFlag];
      const std::vector<QString> & rc_DataBlockPaths = rc_NodeUpdateInformation.GetPaths(
         C_PuiSvNodeUpdate::eFTP_DATA_BLOCK);
      const std::vector<C_PuiSvNodeUpdateParamInfo> & rc_ParamSetPaths = rc_NodeUpdateInformation.GetParamInfos();
      const std::vector<QString> & rc_FileBasedPaths = rc_NodeUpdateInformation.GetPaths(
         C_PuiSvNodeUpdate::eFTP_FILE_BASED);
      std::vector<bool> c_SkipFlags;
      const std::vector<bool> & rc_PathSkipFlags = rc_NodeUpdateInformation.GetSkipUpdateOfPathsFlags(
         C_PuiSvNodeUpdate::eFTP_DATA_BLOCK);
      const std::vector<bool> & rc_ParamSetSkipFlags = rc_NodeUpdateInformation.GetSkipUpdateOfParamInfosFlags();
      const std::vector<bool> & rc_FileBasedSkipFlags = rc_NodeUpdateInformation.GetSkipUpdateOfPathsFlags(
         C_PuiSvNodeUpdate::eFTP_FILE_BASED);

      orc_XmlParser.CreateAndSelectNodeChild("node-specific-update-information");
      orc_XmlParser.SetAttributeUint32("position", rc_NodeUpdateInformation.u32_NodeUpdatePosition);

      C_PuiSvHandlerFiler::mh_SaveNodeUpdateInformationPaths(rc_DataBlockPaths, "data-block-path", orc_XmlParser);

      mh_SaveNodeUpdateInformationParamInfo(rc_ParamSetPaths, orc_XmlParser);

      C_PuiSvHandlerFiler::mh_SaveNodeUpdateInformationPaths(rc_FileBasedPaths, "file-based-path", orc_XmlParser);

      // Save the skip flags
      if (rc_PathSkipFlags.size() > 0)
      {
         std::copy(rc_PathSkipFlags.begin(), rc_PathSkipFlags.begin() + rc_PathSkipFlags.size(),
                   std::back_inserter(c_SkipFlags));
      }
      if (rc_ParamSetSkipFlags.size() > 0)
      {
         std::copy(rc_ParamSetSkipFlags.begin(), rc_ParamSetSkipFlags.begin() + rc_ParamSetSkipFlags.size(),
                   std::back_inserter(c_SkipFlags));
      }
      if (rc_FileBasedSkipFlags.size() > 0)
      {
         std::copy(rc_FileBasedSkipFlags.begin(), rc_FileBasedSkipFlags.begin() + rc_FileBasedSkipFlags.size(),
                   std::back_inserter(c_SkipFlags));
      }
      mh_SaveNodeUpdateInformationSkipUpdateOfFiles(c_SkipFlags, orc_XmlParser);

      C_PuiSvHandlerFiler::mh_SaveNodeUpdateInformationPem(rc_NodeUpdateInformation, orc_XmlParser);

      //Return
      tgl_assert(orc_XmlParser.SelectNodeParent() == "node-update-information");
   }
   //Return
   tgl_assert(orc_XmlParser.SelectNodeParent() == "opensyde-system-view");
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Save node update path information

   \param[in]      orc_Paths           Paths to save
   \param[in]      orc_XmlTagBaseName  XML tag to use for saving
   \param[in,out]  orc_XmlParser       XML parser with the "current" element set
                                       to the "node-specific-update-information" element
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PuiSvHandlerFiler::mh_SaveNodeUpdateInformationPaths(const std::vector<QString> & orc_Paths,
                                                            const QString & orc_XmlTagBaseName,
                                                            C_OscXmlParserBase & orc_XmlParser)
{
   const C_SclString c_ParentName = static_cast<QString>(orc_XmlTagBaseName + "s").toStdString().c_str();
   const C_SclString c_ChildName = orc_XmlTagBaseName.toStdString().c_str();

   orc_XmlParser.CreateAndSelectNodeChild(c_ParentName);
   for (uint32_t u32_ItPath = 0; u32_ItPath < orc_Paths.size(); ++u32_ItPath)
   {
      const QString & rc_Path = orc_Paths[u32_ItPath];
      orc_XmlParser.CreateNodeChild(c_ChildName, rc_Path.toStdString().c_str());
   }
   //Return
   tgl_assert(orc_XmlParser.SelectNodeParent() == "node-specific-update-information");
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Save update information for parameter sets

   \param[in]      orc_Info         Update information for parameter sets
   \param[in,out]  orc_XmlParser    XML parser with the "current" element set
                                    to the "node-specific-update-information" element
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PuiSvHandlerFiler::mh_SaveNodeUpdateInformationParamInfo(
   const std::vector<C_PuiSvNodeUpdateParamInfo> & orc_Info, C_OscXmlParserBase & orc_XmlParser)
{
   orc_XmlParser.CreateAndSelectNodeChild("param-sets");
   for (uint32_t u32_ItParamSet = 0UL; u32_ItParamSet < orc_Info.size(); ++u32_ItParamSet)
   {
      const C_PuiSvNodeUpdateParamInfo & rc_ParamSetInfo = orc_Info[u32_ItParamSet];
      orc_XmlParser.CreateAndSelectNodeChild("param-set");
      orc_XmlParser.SetAttributeUint32("last-known-crc", rc_ParamSetInfo.GetLastKnownCrc());
      orc_XmlParser.CreateNodeChild("path", rc_ParamSetInfo.GetPath().toStdString().c_str());
      //Return
      tgl_assert(orc_XmlParser.SelectNodeParent() == "param-sets");
   }
   //Return
   tgl_assert(orc_XmlParser.SelectNodeParent() == "node-specific-update-information");
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Save skip of file update flags update information

   \param[in]      orc_Flags        Skip flags
   \param[in,out]  orc_XmlParser    XML parser with the "current" element set
                                    to the "node-specific-update-information" element
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PuiSvHandlerFiler::mh_SaveNodeUpdateInformationSkipUpdateOfFiles(const std::vector<bool> & orc_Flags,
                                                                        C_OscXmlParserBase & orc_XmlParser)
{
   orc_XmlParser.CreateAndSelectNodeChild("skip-update-of-files");
   for (uint32_t u32_ItSkipFlag = 0U; u32_ItSkipFlag < orc_Flags.size(); ++u32_ItSkipFlag)
   {
      orc_XmlParser.CreateAndSelectNodeChild("skip-update-of-file");
      orc_XmlParser.SetAttributeBool("state", orc_Flags[u32_ItSkipFlag]);
      //Return
      tgl_assert(orc_XmlParser.SelectNodeParent() == "skip-update-of-files");
   }
   //Return
   tgl_assert(orc_XmlParser.SelectNodeParent() == "node-specific-update-information");
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Save node update information pem

   \param[in]      orc_NodeUpdateInformation    Node update information
   \param[in,out]  orc_XmlParser                XML parser
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PuiSvHandlerFiler::mh_SaveNodeUpdateInformationPem(const C_PuiSvNodeUpdate & orc_NodeUpdateInformation,
                                                          C_OscXmlParserBase & orc_XmlParser)
{
   C_PuiSvNodeUpdate::E_StateSecurity e_StateSecurity;
   C_PuiSvNodeUpdate::E_StateDebugger e_StateDebugger;
   orc_XmlParser.CreateAndSelectNodeChild("pem-file");
   orc_XmlParser.SetAttributeBool("skip", orc_NodeUpdateInformation.GetSkipUpdateOfPemFile());
   orc_XmlParser.CreateNodeChild("path", orc_NodeUpdateInformation.GetPemFilePath().toStdString());
   orc_XmlParser.CreateAndSelectNodeChild("states");
   orc_NodeUpdateInformation.GetStates(e_StateSecurity, e_StateDebugger);
   orc_XmlParser.CreateNodeChild("security", C_PuiSvHandlerFiler::h_PemFileStateSecurityToString(
                                    e_StateSecurity).toStdString());
   orc_XmlParser.CreateNodeChild("debugger", C_PuiSvHandlerFiler::h_PemFileStateDebuggerToString(
                                    e_StateDebugger).toStdString());
   //Return
   tgl_assert(orc_XmlParser.SelectNodeParent() == "pem-file");
   //Return
   tgl_assert(orc_XmlParser.SelectNodeParent() == "node-specific-update-information");
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
   orc_XmlParser.SetNodeContent(orc_View.GetName().toStdString().c_str());
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
   mh_SaveNodeActiveFlags(orc_View.GetNodeActiveFlags(), orc_XmlParser);
   mh_SaveNodeUpdateInformation(orc_View.GetAllNodeUpdateInformation(), orc_XmlParser);
   orc_XmlParser.CreateAndSelectNodeChild("pc");
   mh_SavePc(orc_View.GetPcData(), orc_XmlParser);
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

   \param[in]      orc_Pc           PC element
   \param[in,out]  orc_XmlParser    XML parser with the "current" element set to the "pc" element
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PuiSvHandlerFiler::mh_SavePc(const C_PuiSvPc & orc_Pc, C_OscXmlParserBase & orc_XmlParser)
{
   orc_XmlParser.SetAttributeBool("connected", orc_Pc.GetConnected());
   orc_XmlParser.SetAttributeUint32("bus-index", orc_Pc.GetBusIndex());

   orc_XmlParser.CreateAndSelectNodeChild("dll-path");
   orc_XmlParser.SetAttributeSint32("type", static_cast<int32_t>(orc_Pc.GetCanDllType()));
   orc_XmlParser.SetNodeContent(orc_Pc.GetCustomCanDllPath().toStdString().c_str());
   //Return
   tgl_assert(orc_XmlParser.SelectNodeParent() == "pc");
   orc_XmlParser.CreateAndSelectNodeChild("box");
   C_PuiBsElementsFiler::h_SaveBoxBase(orc_Pc, orc_XmlParser);
   //Return
   tgl_assert(orc_XmlParser.SelectNodeParent() == "pc");
   orc_XmlParser.CreateAndSelectNodeChild("connection");
   orc_XmlParser.CreateAndSelectNodeChild("line");
   C_PuiBsElementsFiler::h_SaveLineBase(orc_Pc.GetConnectionData(), orc_XmlParser);
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
