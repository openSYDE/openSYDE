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
#include "C_PuiSvDashboardFiler.h"

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

   \param[in,out]  orc_Views        System view elements (Cleared if necessary)
   \param[in]      orc_OSCNodes     OSC node information (Necessary for update information)
   \param[in,out]  orc_XMLParser    XML parser with the "current" element set to the "opensyde-system-views" element
   \param[in]      opc_BasePath     Base path (Optional for save to string)

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
      //Return
      tgl_assert(orc_XMLParser.SelectNodeParent() == "opensyde-system-views");
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
   \param[in,out]  orc_XMLParser    XML parser with the "current" element set to the "opensyde-system-views" element
   \param[in]      opc_BasePath     Base path (Optional for save to string)

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
/*! \brief   Load read rails

   \param[in,out]  orc_Rails        Read rails
   \param[in,out]  orc_XMLParser    XML parser with the "current" element set to the "rail-assignments" element

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

   \param[in]      orc_Rails        Read rails
   \param[in,out]  orc_XMLParser    XML parser with the "current" element set to the "rail-assignments" element
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
/*! \brief  Load last known halc crcs

   \param[in,out]  orc_Crcs         Crcs
   \param[in,out]  orc_XMLParser    XML parser

   \return
   C_NO_ERR    information loaded
   C_CONFIG    error loading information
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_PuiSvHandlerFiler::h_LoadLastKnownHalcCrcs(std::map<C_PuiSvDbNodeDataPoolListElementId,
                                                             C_PuiSvLastKnownHalElementId> & orc_Crcs,
                                                    C_OSCXMLParserBase & orc_XMLParser)
{
   sint32 s32_Retval = C_NO_ERR;

   orc_Crcs.clear();
   if (orc_XMLParser.SelectNodeChild("last-known-halc-crcs") == "last-known-halc-crcs")
   {
      C_SCLString c_CurrentNode = orc_XMLParser.SelectNodeChild("last-known-halc-crc");

      if (c_CurrentNode == "last-known-halc-crc")
      {
         do
         {
            C_PuiSvDbNodeDataPoolListElementId c_Id;
            C_PuiSvDashboardFiler::h_LoadUiIndex(c_Id, orc_XMLParser);
            if (orc_XMLParser.AttributeExists("crc"))
            {
               const uint32 u32_Crc = orc_XMLParser.GetAttributeUint32("crc");
               QString c_DpName;
               if (orc_XMLParser.SelectNodeChild("hal-data-pool-name") == "hal-data-pool-name")
               {
                  c_DpName = orc_XMLParser.GetNodeContent().c_str();
                  //Return
                  tgl_assert(orc_XMLParser.SelectNodeParent() == "last-known-halc-crc");
               }
               else
               {
                  const C_OSCNodeDataPool * const pc_Dp = C_PuiSdHandler::h_GetInstance()->GetOSCDataPool(
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
            c_CurrentNode = orc_XMLParser.SelectNodeNext("last-known-halc-crc");
         }
         while ((c_CurrentNode == "last-known-halc-crc") && (s32_Retval == C_NO_ERR));
         //Return
         tgl_assert(orc_XMLParser.SelectNodeParent() == "last-known-halc-crcs");
      }
      //Return
      tgl_assert(orc_XMLParser.SelectNodeParent() == "opensyde-system-views");
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Save last known halc crcs

   \param[in]      orc_Crcs         Crcs
   \param[in,out]  orc_XMLParser    XML parser
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PuiSvHandlerFiler::h_SaveLastKnownHalcCrcs(const std::map<C_PuiSvDbNodeDataPoolListElementId,
                                                                 C_PuiSvLastKnownHalElementId> & orc_Crcs,
                                                  C_OSCXMLParserBase & orc_XMLParser)
{
   orc_XMLParser.CreateAndSelectNodeChild("last-known-halc-crcs");
   for (std::map<C_PuiSvDbNodeDataPoolListElementId, C_PuiSvLastKnownHalElementId>::const_iterator c_It =
           orc_Crcs.begin();
        c_It != orc_Crcs.end(); ++c_It)
   {
      orc_XMLParser.CreateAndSelectNodeChild("last-known-halc-crc");
      C_PuiSvDashboardFiler::h_SaveUiIndex(c_It->first, orc_XMLParser);
      orc_XMLParser.SetAttributeUint32("crc", c_It->second.u32_Crc);
      orc_XMLParser.CreateNodeChild("hal-data-pool-name", c_It->second.c_HalDpName.toStdString().c_str());

      //Return
      tgl_assert(orc_XMLParser.SelectNodeParent() == "last-known-halc-crcs");
   }
   //Return
   tgl_assert(orc_XMLParser.SelectNodeParent() == "opensyde-system-views");
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
   return ("view_" + C_OSCSystemFilerUtil::h_PrepareItemNameForFileName(
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
sint32 C_PuiSvHandlerFiler::h_StringToPemFileStateDebugger(const QString & orc_String,
                                                           C_PuiSvNodeUpdate::E_StateDebugger & ore_State)
{
   sint32 s32_Retval = C_NO_ERR;

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
sint32 C_PuiSvHandlerFiler::h_StringToPemFileStateSecurity(const QString & orc_String,
                                                           C_PuiSvNodeUpdate::E_StateSecurity & ore_State)
{
   sint32 s32_Retval = C_NO_ERR;

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
   \param[in,out]  orc_XMLParser          XML parser with the "current" element set to the "opensyde-system-view" element

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

   \param[in,out]  orc_NodeUpdateInformation    Node update information (Cleared if necessary)
   \param[in,out]  orc_XMLParser                XML parser with the "current" element set
                                                to the "opensyde-system-view" element
   \param[in]      orc_OSCNodes                 OSC node information (Necessary for update information)

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

   \param[out]     orc_Paths           Read paths
   \param[in]      orc_XMLTagBaseName  XML tag base name (parent with s and children as is)
   \param[in,out]  orc_XMLParser       XML parser
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PuiSvHandlerFiler::mh_LoadNodeUpdateInformationPaths(std::vector<QString> & orc_Paths,
                                                            const QString & orc_XMLTagBaseName,
                                                            C_OSCXMLParserBase & orc_XMLParser)
{
   const C_SCLString c_ParentName = static_cast<QString>(orc_XMLTagBaseName + "s").toStdString().c_str();
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

   \param[in,out]  orc_NodeUpdateInformation    Node update information
   \param[in,out]  orc_XMLParser                XML parser with the "current" element set
                                                to the "node-update-information" or
                                                "node-specific-update-information" element
   \param[in]      orc_Node                     Node

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
   std::vector<bool> c_SkipFlags;
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
      orc_XMLParser.SelectNodeParent();
   }
   else
   {
      std::vector<C_PuiSvNodeUpdateParamInfo> c_ParamInfo;
      std::vector<QString> c_FileBasedPaths;
      std::vector<bool> c_PathSkipFlags;
      std::vector<bool> c_ParamSetSkipFlags;
      std::vector<bool> c_FileBasedSkipFlags;

      //New format
      C_PuiSvHandlerFiler::mh_LoadNodeUpdateInformationPaths(c_Paths, "data-block-path", orc_XMLParser);
      orc_NodeUpdateInformation.SetPaths(c_Paths, C_PuiSvNodeUpdate::eFTP_DATA_BLOCK);

      C_PuiSvHandlerFiler::mh_LoadNodeUpdateInformationParam(c_ParamInfo, orc_XMLParser);
      orc_NodeUpdateInformation.SetParamInfos(c_ParamInfo);

      C_PuiSvHandlerFiler::mh_LoadNodeUpdateInformationPaths(c_FileBasedPaths, "file-based-path", orc_XMLParser);
      orc_NodeUpdateInformation.SetPaths(c_FileBasedPaths, C_PuiSvNodeUpdate::eFTP_FILE_BASED);

      // Handling of the skip flags
      C_PuiSvHandlerFiler::mh_LoadNodeUpdateInformationSkipUpdateOfFiles(c_SkipFlags, orc_XMLParser);
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

      s32_Retval = C_PuiSvHandlerFiler::mh_LoadNodeUpdateInformationPem(orc_NodeUpdateInformation, orc_XMLParser);
   }

   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Load node parameter set update information

   \param[out]     orc_Info         Parsed parameter set update information
   \param[in,out]  orc_XMLParser    XML parser with the "current" element set
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
/*! \brief   Load node skip of file update flags update information

   \param[out]     orc_Flags        Parsed skip flags
   \param[in,out]  orc_XMLParser    XML parser with the "current" element set
                                    to the "node-update-information" or
                                    "node-specific-update-information" element
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PuiSvHandlerFiler::mh_LoadNodeUpdateInformationSkipUpdateOfFiles(std::vector<bool> & orc_Flags,
                                                                        C_OSCXMLParserBase & orc_XMLParser)
{
   // No error in case of not existing flags due to compatibility
   orc_Flags.clear();
   if (orc_XMLParser.SelectNodeChild("skip-update-of-files") == "skip-update-of-files")
   {
      C_SCLString c_CurrentNodeActiveFlagNode = orc_XMLParser.SelectNodeChild("skip-update-of-file");
      if (c_CurrentNodeActiveFlagNode == "skip-update-of-file")
      {
         do
         {
            const bool q_CurrentValue = orc_XMLParser.GetAttributeBool("state");
            orc_Flags.push_back(q_CurrentValue);
            //Next
            c_CurrentNodeActiveFlagNode = orc_XMLParser.SelectNodeNext("skip-update-of-file");
         }
         while (c_CurrentNodeActiveFlagNode == "skip-update-of-file");
         //Return
         tgl_assert(orc_XMLParser.SelectNodeParent() == "skip-update-of-files");
      }
      //Return
      orc_XMLParser.SelectNodeParent();
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Load node update information pem

   \param[in,out]  orc_NodeUpdateInformation    Node update information
   \param[in,out]  orc_XMLParser                XML parser

   \return
   STW error codes

   \retval   C_NO_ERR   Information loaded
   \retval   C_CONFIG   Error loading information
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_PuiSvHandlerFiler::mh_LoadNodeUpdateInformationPem(C_PuiSvNodeUpdate & orc_NodeUpdateInformation,
                                                            C_OSCXMLParserBase & orc_XMLParser)
{
   sint32 s32_Retval = C_NO_ERR;

   if (orc_XMLParser.SelectNodeChild("pem-file") == "pem-file")
   {
      bool q_Value;
      s32_Retval = orc_XMLParser.GetAttributeBoolError("skip", q_Value);
      if (s32_Retval == C_NO_ERR)
      {
         orc_NodeUpdateInformation.SetSkipUpdateOfPemFile(q_Value);

         s32_Retval = orc_XMLParser.SelectNodeChildError("path");
         if (s32_Retval == C_NO_ERR)
         {
            orc_NodeUpdateInformation.SetPemFilePath(orc_XMLParser.GetNodeContent().c_str());
            //Return
            tgl_assert(orc_XMLParser.SelectNodeParent() == "pem-file");
         }
      }
      if (s32_Retval == C_NO_ERR)
      {
         s32_Retval = C_PuiSvHandlerFiler::mh_LoadNodeUpdateInformationPemStates(orc_NodeUpdateInformation,
                                                                                 orc_XMLParser);
      }
      if (s32_Retval == C_NO_ERR)
      {
         //Return
         tgl_assert(orc_XMLParser.SelectNodeParent() == "node-specific-update-information");
      }
   }

   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Load node update information pem states

   \param[in,out]  orc_NodeUpdateInformation    Node update information
   \param[in,out]  orc_XMLParser                XML parser

   \return
   STW error codes

   \retval   C_NO_ERR   Information loaded
   \retval   C_CONFIG   Error loading information
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_PuiSvHandlerFiler::mh_LoadNodeUpdateInformationPemStates(C_PuiSvNodeUpdate & orc_NodeUpdateInformation,
                                                                  C_OSCXMLParserBase & orc_XMLParser)
{
   sint32 s32_Retval = orc_XMLParser.SelectNodeChildError("states");

   if (s32_Retval == C_NO_ERR)
   {
      C_PuiSvNodeUpdate::E_StateSecurity e_StateSecurity = C_PuiSvNodeUpdate::eST_SEC_NO_CHANGE;
      C_PuiSvNodeUpdate::E_StateDebugger e_StateDebugger = C_PuiSvNodeUpdate::eST_DEB_NO_CHANGE;
      s32_Retval = orc_XMLParser.SelectNodeChildError("security");
      if (s32_Retval == C_NO_ERR)
      {
         s32_Retval = C_PuiSvHandlerFiler::h_StringToPemFileStateSecurity(
            orc_XMLParser.GetNodeContent().c_str(), e_StateSecurity);
         if (s32_Retval == C_NO_ERR)
         {
            //Return
            tgl_assert(orc_XMLParser.SelectNodeParent() == "states");
         }
      }
      if (s32_Retval == C_NO_ERR)
      {
         s32_Retval = orc_XMLParser.SelectNodeChildError("debugger");
         if (s32_Retval == C_NO_ERR)
         {
            s32_Retval = C_PuiSvHandlerFiler::h_StringToPemFileStateDebugger(
               orc_XMLParser.GetNodeContent().c_str(), e_StateDebugger);
            if (s32_Retval == C_NO_ERR)
            {
               //Return
               tgl_assert(orc_XMLParser.SelectNodeParent() == "states");
            }
         }
      }
      if (s32_Retval == C_NO_ERR)
      {
         orc_NodeUpdateInformation.SetStates(e_StateSecurity, e_StateDebugger);

         //Return
         tgl_assert(orc_XMLParser.SelectNodeParent() == "pem-file");
      }
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Load dashboard elements

   \param[in,out]  orc_Dashboards   Dashboard elements (Cleared if necessary)
   \param[in,out]  orc_XMLParser    XML parser with the "current" element set to the "opensyde-system-view" element

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
            s32_Retval = C_PuiSvDashboardFiler::h_LoadDashboard(c_Dashboard, orc_XMLParser);
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

   \param[in,out]  orc_View      View data storage
   \param[in]      orc_FilePath  File path
   \param[in]      orc_OSCNodes  OSC node information (Necessary for update information)

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

   //File version
   if (c_XMLParser.SelectNodeChild("file-version") == "file-version")
   {
      uint16 u16_FileVersion = 0U;
      try
      {
         u16_FileVersion = static_cast<uint16>(c_XMLParser.GetNodeContent().ToInt());
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
                            C_SCLString::IntToStr(u16_FileVersion));
         //Check file version
         if ((u16_FileVersion != 1U) && (u16_FileVersion != 2U))
         {
            osc_write_log_error("Loading view",
                                "Version defined by \"file-version\" is not supported.");
            s32_Retval = C_CONFIG;
         }
      }

      //Return
      c_XMLParser.SelectNodeParent();
   }
   else
   {
      osc_write_log_error("Loading view", "Could not find \"file-version\" node.");
      s32_Retval = C_CONFIG;
   }

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

   \param[in,out]  orc_View         View element
   \param[in,out]  orc_XMLParser    XML parser with the "current" element set to the "opensyde-system-view" element
   \param[in]      orc_OSCNodes     OSC node information (Necessary for update information)

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
   if (orc_XMLParser.SelectNodeChild("service-mode") == "service-mode")
   {
      if (orc_XMLParser.AttributeExists("active"))
      {
         orc_View.SetServiceModeActive(orc_XMLParser.GetAttributeBool("active"));
      }
      else
      {
         s32_Retval = C_CONFIG;
         osc_write_log_error("Loading view",
                             "Attribute \"active\" not found in node \"service-mode\".");
      }
      if (orc_XMLParser.SelectNodeChild("availability") == "availability")
      {
         if (orc_XMLParser.AttributeExists("setup"))
         {
            orc_View.SetServiceModeSetupActive(orc_XMLParser.GetAttributeBool("setup"));
         }
         else
         {
            s32_Retval = C_CONFIG;
            osc_write_log_error("Loading view",
                                "Attribute \"setup\" not found in node \"availability\".");
         }
         if (orc_XMLParser.AttributeExists("update"))
         {
            orc_View.SetServiceModeUpdateActive(orc_XMLParser.GetAttributeBool("update"));
         }
         else
         {
            s32_Retval = C_CONFIG;
            osc_write_log_error("Loading view",
                                "Attribute \"update\" not found in node \"availability\".");
         }
         if (orc_XMLParser.AttributeExists("dashboard"))
         {
            orc_View.SetServiceModeDashboardActive(orc_XMLParser.GetAttributeBool("dashboard"));
         }
         else
         {
            s32_Retval = C_CONFIG;
            osc_write_log_error("Loading view",
                                "Attribute \"dashboard\" not found in node \"availability\".");
         }
         //Return
         tgl_assert(orc_XMLParser.SelectNodeParent() == "service-mode");
      }
      else
      {
         s32_Retval = C_CONFIG;
         osc_write_log_error("Loading view",
                             "Node \"availability\" not found in node \"service-mode\".");
      }
      tgl_assert(orc_XMLParser.SelectNodeParent() == "opensyde-system-view");
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

   \param[in,out]  orc_Pc           PC element
   \param[in,out]  orc_XMLParser    XML parser with the "current" element set to the "pc" element

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
/*! \brief   Load data element id

   \param[in,out]  orc_Id           Data element id
   \param[in,out]  orc_XMLParser    XML parser with the "current" element set to any element
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
/*! \brief   Transform string to transmission mode

   \param[in]   orc_String    String to interpret
   \param[out]  ore_Mode      Mode

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
/*! \brief   Save node active flags

   \param[in]      orc_NodeActiveFlags    Node active flags
   \param[in,out]  orc_XMLParser          XML parser with the "current" element set to the "opensyde-system-view" element
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

   \param[in]      orc_NodeUpdateInformation    Node update information
   \param[in,out]  orc_XMLParser                XML parser with the "current" element set
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
      std::vector<bool> c_SkipFlags;
      const std::vector<bool> & rc_PathSkipFlags = rc_NodeUpdateInformation.GetSkipUpdateOfPathsFlags(
         C_PuiSvNodeUpdate::eFTP_DATA_BLOCK);
      const std::vector<bool> & rc_ParamSetSkipFlags = rc_NodeUpdateInformation.GetSkipUpdateOfParamInfosFlags();
      const std::vector<bool> & rc_FileBasedSkipFlags = rc_NodeUpdateInformation.GetSkipUpdateOfPathsFlags(
         C_PuiSvNodeUpdate::eFTP_FILE_BASED);

      orc_XMLParser.CreateAndSelectNodeChild("node-specific-update-information");
      orc_XMLParser.SetAttributeUint32("position", rc_NodeUpdateInformation.u32_NodeUpdatePosition);

      C_PuiSvHandlerFiler::mh_SaveNodeUpdateInformationPaths(rc_DataBlockPaths, "data-block-path", orc_XMLParser);

      mh_SaveNodeUpdateInformationParamInfo(rc_ParamSetPaths, orc_XMLParser);

      C_PuiSvHandlerFiler::mh_SaveNodeUpdateInformationPaths(rc_FileBasedPaths, "file-based-path", orc_XMLParser);

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
      mh_SaveNodeUpdateInformationSkipUpdateOfFiles(c_SkipFlags, orc_XMLParser);

      C_PuiSvHandlerFiler::mh_SaveNodeUpdateInformationPem(rc_NodeUpdateInformation, orc_XMLParser);

      //Return
      tgl_assert(orc_XMLParser.SelectNodeParent() == "node-update-information");
   }
   //Return
   tgl_assert(orc_XMLParser.SelectNodeParent() == "opensyde-system-view");
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Save node update path information

   \param[in]      orc_Paths           Paths to save
   \param[in]      orc_XMLTagBaseName  XML tag to use for saving
   \param[in,out]  orc_XMLParser       XML parser with the "current" element set
                                       to the "node-specific-update-information" element
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PuiSvHandlerFiler::mh_SaveNodeUpdateInformationPaths(const std::vector<QString> & orc_Paths,
                                                            const QString & orc_XMLTagBaseName,
                                                            C_OSCXMLParserBase & orc_XMLParser)
{
   const C_SCLString c_ParentName = static_cast<QString>(orc_XMLTagBaseName + "s").toStdString().c_str();
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

   \param[in]      orc_Info         Update information for parameter sets
   \param[in,out]  orc_XMLParser    XML parser with the "current" element set
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
/*! \brief   Save skip of file update flags update information

   \param[in]      orc_Flags        Skip flags
   \param[in,out]  orc_XMLParser    XML parser with the "current" element set
                                    to the "node-specific-update-information" element
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PuiSvHandlerFiler::mh_SaveNodeUpdateInformationSkipUpdateOfFiles(const std::vector<bool> & orc_Flags,
                                                                        C_OSCXMLParserBase & orc_XMLParser)
{
   orc_XMLParser.CreateAndSelectNodeChild("skip-update-of-files");
   for (uint32 u32_ItSkipFlag = 0U; u32_ItSkipFlag < orc_Flags.size(); ++u32_ItSkipFlag)
   {
      orc_XMLParser.CreateAndSelectNodeChild("skip-update-of-file");
      orc_XMLParser.SetAttributeBool("state", orc_Flags[u32_ItSkipFlag]);
      //Return
      tgl_assert(orc_XMLParser.SelectNodeParent() == "skip-update-of-files");
   }
   //Return
   tgl_assert(orc_XMLParser.SelectNodeParent() == "node-specific-update-information");
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Save node update information pem

   \param[in]      orc_NodeUpdateInformation    Node update information
   \param[in,out]  orc_XMLParser                XML parser
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PuiSvHandlerFiler::mh_SaveNodeUpdateInformationPem(const C_PuiSvNodeUpdate & orc_NodeUpdateInformation,
                                                          C_OSCXMLParserBase & orc_XMLParser)
{
   C_PuiSvNodeUpdate::E_StateSecurity e_StateSecurity;
   C_PuiSvNodeUpdate::E_StateDebugger e_StateDebugger;
   orc_XMLParser.CreateAndSelectNodeChild("pem-file");
   orc_XMLParser.SetAttributeBool("skip", orc_NodeUpdateInformation.GetSkipUpdateOfPemFile());
   orc_XMLParser.CreateNodeChild("path", orc_NodeUpdateInformation.GetPemFilePath().toStdString());
   orc_XMLParser.CreateAndSelectNodeChild("states");
   orc_NodeUpdateInformation.GetStates(e_StateSecurity, e_StateDebugger);
   orc_XMLParser.CreateNodeChild("security", C_PuiSvHandlerFiler::h_PemFileStateSecurityToString(
                                    e_StateSecurity).toStdString());
   orc_XMLParser.CreateNodeChild("debugger", C_PuiSvHandlerFiler::h_PemFileStateDebuggerToString(
                                    e_StateDebugger).toStdString());
   //Return
   tgl_assert(orc_XMLParser.SelectNodeParent() == "pem-file");
   //Return
   tgl_assert(orc_XMLParser.SelectNodeParent() == "node-specific-update-information");
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Save dashboard elements

   \param[in]      orc_Dashboards   Dashboard elements
   \param[in,out]  orc_XMLParser    XML parser with the "current" element set to the "opensyde-system-view" element
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PuiSvHandlerFiler::mh_SaveDashboards(const std::vector<C_PuiSvDashboard> & orc_Dashboards,
                                            C_OSCXMLParserBase & orc_XMLParser)
{
   orc_XMLParser.CreateAndSelectNodeChild("dashboards");
   for (uint32 u32_ItDashboard = 0; u32_ItDashboard < orc_Dashboards.size(); ++u32_ItDashboard)
   {
      orc_XMLParser.CreateAndSelectNodeChild("dashboard");
      C_PuiSvDashboardFiler::h_SaveDashboard(orc_Dashboards[u32_ItDashboard], orc_XMLParser);
      //Return
      tgl_assert(orc_XMLParser.SelectNodeParent() == "dashboards");
   }
   //Return
   tgl_assert(orc_XMLParser.SelectNodeParent() == "opensyde-system-view");
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

   \param[in]      orc_View         View element
   \param[in,out]  orc_XMLParser    XML parser with the "current" element set to the "opensyde-system-view" element
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
   //Service mode
   tgl_assert(orc_XMLParser.CreateAndSelectNodeChild("service-mode") == "service-mode");
   orc_XMLParser.SetAttributeBool("active", orc_View.GetServiceModeActive());
   tgl_assert(orc_XMLParser.CreateAndSelectNodeChild("availability") == "availability");
   orc_XMLParser.SetAttributeBool("setup", orc_View.GetServiceModeSetupActive());
   orc_XMLParser.SetAttributeBool("update", orc_View.GetServiceModeUpdateActive());
   orc_XMLParser.SetAttributeBool("dashboard", orc_View.GetServiceModeDashboardActive());
   //Return
   tgl_assert(orc_XMLParser.SelectNodeParent() == "service-mode");
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

   \param[in]      orc_Pc           PC element
   \param[in,out]  orc_XMLParser    XML parser with the "current" element set to the "pc" element
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
/*! \brief   Save ID

   \param[in]      orc_Id           ID
   \param[in,out]  orc_XMLParser    XML parser with the "current" element set to the any element
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
