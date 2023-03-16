//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       View filer (core parts)

   \copyright   Copyright 2022 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.hpp"

#include "stwtypes.hpp"
#include "stwerrors.hpp"
#include "TglFile.hpp"
#include "TglUtils.hpp"
#include "C_OscXmlParserLog.hpp"
#include "C_OscLoggingHandler.hpp"
#include "C_OscSystemFilerUtil.hpp"
#include "C_OscViewFiler.hpp"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw::errors;
using namespace stw::scl;
using namespace stw::tgl;
using namespace stw::opensyde_core;

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
C_OscViewFiler::C_OscViewFiler(void)
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Load system views file

   \param[in,out]  orc_Views              System view elements
   \param[in]      orc_PathSystemViews    Path to system views file
   \param[in]      orc_OscNodes           OSC node information

   \return
   C_NO_ERR    information loaded
   C_CONFIG    error loading information
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_OscViewFiler::h_LoadSystemViewsFile(std::vector<C_OscViewData> & orc_Views,
                                              const C_SclString & orc_PathSystemViews,
                                              const std::vector<C_OscNode> & orc_OscNodes)
{
   int32_t s32_Retval = C_NO_ERR;

   if (TglFileExists(orc_PathSystemViews) == true)
   {
      C_OscXmlParserLog c_XmlParser;
      c_XmlParser.SetLogHeading("Loading System Views");
      s32_Retval = c_XmlParser.LoadFromFile(orc_PathSystemViews);
      if (s32_Retval == C_NO_ERR)
      {
         s32_Retval = h_LoadViewsOsc(orc_Views, orc_OscNodes, c_XmlParser, orc_PathSystemViews);
      }
      else
      {
         osc_write_log_error("Loading System Views",
                             "File \"" + orc_PathSystemViews + "\" could not be opened.");
         s32_Retval = C_NOACT;
      }
   }
   else
   {
      osc_write_log_error("Loading System Views", "File \"" + orc_PathSystemViews + "\" does not exist.");
      s32_Retval = C_RANGE;
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Load core part of system views

   \param[in,out]  orc_Views        System view elements (Cleared if necessary)
   \param[in]      orc_OscNodes     OSC node information (Necessary for update information)
   \param[in,out]  orc_XmlParser    XML parser with the "current" element set to the "opensyde-system-views" element
   \param[in]      orc_BasePath     Base path

   \return
   C_NO_ERR    information loaded
   C_CONFIG    error loading information
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_OscViewFiler::h_LoadViewsOsc(std::vector<C_OscViewData> & orc_Views,
                                       const std::vector<C_OscNode> & orc_OscNodes, C_OscXmlParserBase & orc_XmlParser,
                                       const C_SclString & orc_BasePath)
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
            C_OscViewData c_View;
            if (orc_BasePath.IsEmpty() == false)
            {
               const C_SclString c_File =
                  C_OscSystemFilerUtil::h_CombinePaths(orc_BasePath, orc_XmlParser.GetNodeContent());
               s32_Retval = mh_LoadViewFileOsc(c_View, c_File, orc_OscNodes);
            }
            else
            {
               s32_Retval = h_LoadViewOsc(c_View, orc_XmlParser, orc_OscNodes);
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
/*! \brief  Load core parts of view element

   \param[in,out]  orc_View         View element
   \param[in,out]  orc_XmlParser    XML parser with the "current" element set to the "opensyde-system-view" element
   \param[in]      orc_OscNodes     OSC node information (Necessary for update information)

   \return
   C_NO_ERR    information loaded
   C_CONFIG    error loading information
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_OscViewFiler::h_LoadViewOsc(C_OscViewData & orc_View, C_OscXmlParserBase & orc_XmlParser,
                                      const std::vector<C_OscNode> & orc_OscNodes)
{
   int32_t s32_Retval = C_NO_ERR;

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
   if (s32_Retval == C_NO_ERR)
   {
      std::vector<uint8_t> c_NodeActiveFlags;
      s32_Retval = C_OscViewFiler::mh_LoadNodeActiveFlags(c_NodeActiveFlags, orc_XmlParser);
      orc_View.SetNodeActiveFlags(c_NodeActiveFlags);
      if (s32_Retval == C_NO_ERR)
      {
         std::vector<C_OscViewNodeUpdate> c_NodeUpdateInformation;
         //If you have an async project this might help
         //c_NodeUpdateInformation.resize(c_NodeActiveFlags.size(),C_OscViewNodeUpdate());
         s32_Retval =
            C_OscViewFiler::mh_LoadNodeUpdateInformation(c_NodeUpdateInformation, orc_XmlParser, orc_OscNodes);
         orc_View.SetNodeUpdateInformation(c_NodeUpdateInformation);
         if (s32_Retval == C_NO_ERR)
         {
            if (orc_XmlParser.SelectNodeChild("pc") == "pc")
            {
               C_OscViewPc c_OscPcData;
               mh_LoadPc(c_OscPcData, orc_XmlParser);
               orc_View.SetOscPcData(c_OscPcData);
               //Return
               tgl_assert(orc_XmlParser.SelectNodeParent() == "opensyde-system-view");
            }
            else
            {
               osc_write_log_error("Loading view", "Node \"pc\" not found.");
               s32_Retval = C_CONFIG;
            }
         }
         else
         {
            osc_write_log_error("Loading view", "Could not load update information");
         }
      }
   }

   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Save node active flags

   \param[in]      orc_NodeActiveFlags    Node active flags
   \param[in,out]  orc_XmlParser          XML parser with the "current" element set to the "opensyde-system-view" element
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OscViewFiler::h_SaveNodeActiveFlags(const std::vector<uint8_t> & orc_NodeActiveFlags,
                                           C_OscXmlParserBase & orc_XmlParser)
{
   orc_XmlParser.CreateAndSelectNodeChild("active-nodes");
   for (uint32_t u32_ItNodeActiveFlag = 0; u32_ItNodeActiveFlag < orc_NodeActiveFlags.size(); ++u32_ItNodeActiveFlag)
   {
      orc_XmlParser.CreateAndSelectNodeChild("active-node");
      orc_XmlParser.SetAttributeBool("state", (orc_NodeActiveFlags[u32_ItNodeActiveFlag] == 1U));
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
void C_OscViewFiler::h_SaveNodeUpdateInformation(const std::vector<C_OscViewNodeUpdate> & orc_NodeUpdateInformation,
                                                 C_OscXmlParserBase & orc_XmlParser)
{
   orc_XmlParser.CreateAndSelectNodeChild("node-update-information");
   for (uint32_t u32_ItNodeActiveFlag = 0; u32_ItNodeActiveFlag < orc_NodeUpdateInformation.size();
        ++u32_ItNodeActiveFlag)
   {
      const C_OscViewNodeUpdate & rc_NodeUpdateInformation = orc_NodeUpdateInformation[u32_ItNodeActiveFlag];
      const std::vector<C_SclString> & rc_DataBlockPaths = rc_NodeUpdateInformation.GetPaths(
         C_OscViewNodeUpdate::eFTP_DATA_BLOCK);
      const std::vector<C_OscViewNodeUpdateParamInfo> & rc_ParamSetPaths = rc_NodeUpdateInformation.GetParamInfos();
      const std::vector<C_SclString> & rc_FileBasedPaths = rc_NodeUpdateInformation.GetPaths(
         C_OscViewNodeUpdate::eFTP_FILE_BASED);
      std::vector<bool> c_SkipFlags;
      const std::vector<bool> & rc_PathSkipFlags = rc_NodeUpdateInformation.GetSkipUpdateOfPathsFlags(
         C_OscViewNodeUpdate::eFTP_DATA_BLOCK);
      const std::vector<bool> & rc_ParamSetSkipFlags = rc_NodeUpdateInformation.GetSkipUpdateOfParamInfosFlags();
      const std::vector<bool> & rc_FileBasedSkipFlags = rc_NodeUpdateInformation.GetSkipUpdateOfPathsFlags(
         C_OscViewNodeUpdate::eFTP_FILE_BASED);

      orc_XmlParser.CreateAndSelectNodeChild("node-specific-update-information");
      orc_XmlParser.SetAttributeUint32("position", rc_NodeUpdateInformation.u32_NodeUpdatePosition);

      C_OscViewFiler::mh_SaveNodeUpdateInformationPaths(rc_DataBlockPaths, "data-block-path", orc_XmlParser);

      mh_SaveNodeUpdateInformationParamInfo(rc_ParamSetPaths, orc_XmlParser);

      C_OscViewFiler::mh_SaveNodeUpdateInformationPaths(rc_FileBasedPaths, "file-based-path", orc_XmlParser);

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

      C_OscViewFiler::mh_SaveNodeUpdateInformationPem(rc_NodeUpdateInformation, orc_XmlParser);

      //Return
      tgl_assert(orc_XmlParser.SelectNodeParent() == "node-update-information");
   }
   //Return
   tgl_assert(orc_XmlParser.SelectNodeParent() == "opensyde-system-view");
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Save PC element

   \param[in]      orc_OscPc        PC element (core part)
   \param[in,out]  orc_XmlParser    XML parser with the "current" element set to the "pc" element
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OscViewFiler::h_SavePc(const C_OscViewPc & orc_OscPc, C_OscXmlParserBase & orc_XmlParser)
{
   orc_XmlParser.SetAttributeBool("connected", orc_OscPc.GetConnected());
   orc_XmlParser.SetAttributeUint32("bus-index", orc_OscPc.GetBusIndex());
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
int32_t C_OscViewFiler::h_StringToPemFileStateDebugger(const C_SclString & orc_String,
                                                       C_OscViewNodeUpdate::E_StateDebugger & ore_State)
{
   int32_t s32_Retval = C_NO_ERR;

   if (orc_String.AnsiCompare("activate") == 0)
   {
      ore_State = C_OscViewNodeUpdate::eST_DEB_ACTIVATE;
   }
   else if (orc_String.AnsiCompare("deactivate") == 0)
   {
      ore_State = C_OscViewNodeUpdate::eST_DEB_DEACTIVATE;
   }
   else if (orc_String.AnsiCompare("no-change") == 0)
   {
      ore_State = C_OscViewNodeUpdate::eST_DEB_NO_CHANGE;
   }
   else
   {
      //Default
      ore_State = C_OscViewNodeUpdate::eST_DEB_NO_CHANGE;
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
int32_t C_OscViewFiler::h_StringToPemFileStateSecurity(const C_SclString & orc_String,
                                                       C_OscViewNodeUpdate::E_StateSecurity & ore_State)
{
   int32_t s32_Retval = C_NO_ERR;

   if (orc_String.AnsiCompare("activate") == 0)
   {
      ore_State = C_OscViewNodeUpdate::eST_SEC_ACTIVATE;
   }
   else if (orc_String.AnsiCompare("deactivate") == 0)
   {
      ore_State = C_OscViewNodeUpdate::eST_SEC_DEACTIVATE;
   }
   else if (orc_String.AnsiCompare("no-change") == 0)
   {
      ore_State = C_OscViewNodeUpdate::eST_SEC_NO_CHANGE;
   }
   else
   {
      //Default
      ore_State = C_OscViewNodeUpdate::eST_SEC_NO_CHANGE;
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
C_SclString C_OscViewFiler::h_PemFileStateDebuggerToString(const C_OscViewNodeUpdate::E_StateDebugger oe_State)
{
   C_SclString c_Retval;

   switch (oe_State)
   {
   case C_OscViewNodeUpdate::eST_DEB_ACTIVATE:
      c_Retval = "activate";
      break;
   case C_OscViewNodeUpdate::eST_DEB_DEACTIVATE:
      c_Retval = "deactivate";
      break;
   case C_OscViewNodeUpdate::eST_DEB_NO_CHANGE:
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
C_SclString C_OscViewFiler::h_PemFileStateSecurityToString(const C_OscViewNodeUpdate::E_StateSecurity oe_State)
{
   C_SclString c_Retval;

   switch (oe_State)
   {
   case C_OscViewNodeUpdate::eST_SEC_ACTIVATE:
      c_Retval = "activate";
      break;
   case C_OscViewNodeUpdate::eST_SEC_DEACTIVATE:
      c_Retval = "deactivate";
      break;
   case C_OscViewNodeUpdate::eST_SEC_NO_CHANGE:
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
int32_t C_OscViewFiler::mh_LoadNodeActiveFlags(std::vector<uint8_t> & orc_NodeActiveFlags,
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
int32_t C_OscViewFiler::mh_LoadNodeUpdateInformation(std::vector<C_OscViewNodeUpdate> & orc_NodeUpdateInformation,
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
               C_OscViewNodeUpdate c_UpdateInfo;

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
                  C_OscViewNodeUpdate c_UpdateInfo;
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
/*! \brief   Load PC element

   \param[in,out]  orc_OscPc        PC element (core part)
   \param[in,out]  orc_XmlParser    XML parser with the "current" element set to the "pc" element
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OscViewFiler::mh_LoadPc(C_OscViewPc & orc_OscPc, const C_OscXmlParserBase & orc_XmlParser)
{
   orc_OscPc.SetConnected(orc_XmlParser.GetAttributeBool("connected"), orc_XmlParser.GetAttributeUint32("bus-index"),
                          true);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Load any update information path

   \param[out]     orc_Paths           Read paths
   \param[in]      orc_XmlTagBaseName  XML tag base name (parent with s and children as is)
   \param[in,out]  orc_XmlParser       XML parser
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OscViewFiler::mh_LoadNodeUpdateInformationPaths(std::vector<C_SclString> & orc_Paths,
                                                       const C_SclString & orc_XmlTagBaseName,
                                                       C_OscXmlParserBase & orc_XmlParser)
{
   const C_SclString c_ParentName = static_cast<C_SclString>(orc_XmlTagBaseName + "s");
   const C_SclString c_ChildName = orc_XmlTagBaseName;

   orc_Paths.clear();
   if (orc_XmlParser.SelectNodeChild(c_ParentName) == c_ParentName)
   {
      C_SclString c_CurrentPathNode = orc_XmlParser.SelectNodeChild(c_ChildName);
      if (c_CurrentPathNode == c_ChildName)
      {
         do
         {
            const C_SclString c_Content = orc_XmlParser.GetNodeContent();
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
int32_t C_OscViewFiler::mh_LoadOneNodeUpdateInformation(C_OscViewNodeUpdate & orc_NodeUpdateInformation,
                                                        C_OscXmlParserBase & orc_XmlParser, const C_OscNode & orc_Node)
{
   std::vector<C_SclString> c_Paths;
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
            c_Paths.push_back(orc_XmlParser.GetNodeContent());
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
         orc_NodeUpdateInformation.SetPaths(c_Paths, C_OscViewNodeUpdate::eFTP_DATA_BLOCK);
         orc_NodeUpdateInformation.SetSkipUpdateOfPathsFlags(c_SkipFlags, C_OscViewNodeUpdate::eFTP_DATA_BLOCK);
      }
      else
      {
         //invalid size
         c_Paths.clear();
         c_Paths.resize(orc_Node.c_Applications.size(), "");
         orc_NodeUpdateInformation.SetPaths(c_Paths, C_OscViewNodeUpdate::eFTP_DATA_BLOCK);
         orc_NodeUpdateInformation.SetSkipUpdateOfPathsFlags(c_SkipFlags, C_OscViewNodeUpdate::eFTP_DATA_BLOCK);
      }
      //Return
      orc_XmlParser.SelectNodeParent();
   }
   else
   {
      std::vector<C_OscViewNodeUpdateParamInfo> c_ParamInfo;
      std::vector<C_SclString> c_FileBasedPaths;
      std::vector<bool> c_PathSkipFlags;
      std::vector<bool> c_ParamSetSkipFlags;
      std::vector<bool> c_FileBasedSkipFlags;

      //New format
      C_OscViewFiler::mh_LoadNodeUpdateInformationPaths(c_Paths, "data-block-path", orc_XmlParser);
      orc_NodeUpdateInformation.SetPaths(c_Paths, C_OscViewNodeUpdate::eFTP_DATA_BLOCK);

      C_OscViewFiler::mh_LoadNodeUpdateInformationParam(c_ParamInfo, orc_XmlParser);
      orc_NodeUpdateInformation.SetParamInfos(c_ParamInfo);

      C_OscViewFiler::mh_LoadNodeUpdateInformationPaths(c_FileBasedPaths, "file-based-path", orc_XmlParser);
      orc_NodeUpdateInformation.SetPaths(c_FileBasedPaths, C_OscViewNodeUpdate::eFTP_FILE_BASED);

      // Handling of the skip flags
      C_OscViewFiler::mh_LoadNodeUpdateInformationSkipUpdateOfFiles(c_SkipFlags, orc_XmlParser);
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
            c_SkipFlags.erase(c_SkipFlags.begin(), c_SkipFlags.begin() + c_ParamInfo.size());
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

      orc_NodeUpdateInformation.SetSkipUpdateOfPathsFlags(c_PathSkipFlags, C_OscViewNodeUpdate::eFTP_DATA_BLOCK);
      orc_NodeUpdateInformation.SetSkipUpdateOfParamInfosFlags(c_ParamSetSkipFlags);
      orc_NodeUpdateInformation.SetSkipUpdateOfPathsFlags(c_FileBasedSkipFlags, C_OscViewNodeUpdate::eFTP_FILE_BASED);

      s32_Retval = C_OscViewFiler::mh_LoadNodeUpdateInformationPem(orc_NodeUpdateInformation, orc_XmlParser);
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
void C_OscViewFiler::mh_LoadNodeUpdateInformationParam(std::vector<C_OscViewNodeUpdateParamInfo> & orc_Info,
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
            C_SclString c_Path;
            uint32_t u32_LastKnownCrc;
            C_OscViewNodeUpdateParamInfo c_Content;
            if (orc_XmlParser.SelectNodeChild("path") == "path")
            {
               c_Path = orc_XmlParser.GetNodeContent();
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
void C_OscViewFiler::mh_LoadNodeUpdateInformationSkipUpdateOfFiles(std::vector<bool> & orc_Flags,
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
int32_t C_OscViewFiler::mh_LoadNodeUpdateInformationPem(C_OscViewNodeUpdate & orc_NodeUpdateInformation,
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
         s32_Retval = C_OscViewFiler::mh_LoadNodeUpdateInformationPemStates(orc_NodeUpdateInformation,
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
int32_t C_OscViewFiler::mh_LoadNodeUpdateInformationPemStates(C_OscViewNodeUpdate & orc_NodeUpdateInformation,
                                                              C_OscXmlParserBase & orc_XmlParser)
{
   int32_t s32_Retval = orc_XmlParser.SelectNodeChildError("states");

   if (s32_Retval == C_NO_ERR)
   {
      C_OscViewNodeUpdate::E_StateSecurity e_StateSecurity = C_OscViewNodeUpdate::eST_SEC_NO_CHANGE;
      C_OscViewNodeUpdate::E_StateDebugger e_StateDebugger = C_OscViewNodeUpdate::eST_DEB_NO_CHANGE;
      s32_Retval = orc_XmlParser.SelectNodeChildError("security");
      if (s32_Retval == C_NO_ERR)
      {
         s32_Retval = C_OscViewFiler::h_StringToPemFileStateSecurity(
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
            s32_Retval = C_OscViewFiler::h_StringToPemFileStateDebugger(
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
/*! \brief  Load core parts of view file

   \param[in,out]  orc_View      View data storage
   \param[in]      orc_FilePath  File path
   \param[in]      orc_OscNodes  OSC node information (Necessary for update information)

   \return
   C_NO_ERR    information loaded
   C_CONFIG    error loading information
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_OscViewFiler::mh_LoadViewFileOsc(C_OscViewData & orc_View, const C_SclString & orc_FilePath,
                                           const std::vector<C_OscNode> & orc_OscNodes)
{
   C_OscXmlParser c_XmlParser;
   int32_t s32_Retval = C_OscSystemFilerUtil::h_GetParserForExistingFile(c_XmlParser, orc_FilePath,
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
         s32_Retval = C_OscViewFiler::h_LoadViewOsc(orc_View, c_XmlParser, orc_OscNodes);
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
/*! \brief   Save node update path information

   \param[in]      orc_Paths           Paths to save
   \param[in]      orc_XmlTagBaseName  XML tag to use for saving
   \param[in,out]  orc_XmlParser       XML parser with the "current" element set
                                       to the "node-specific-update-information" element
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OscViewFiler::mh_SaveNodeUpdateInformationPaths(const std::vector<C_SclString> & orc_Paths,
                                                       const C_SclString & orc_XmlTagBaseName,
                                                       C_OscXmlParserBase & orc_XmlParser)
{
   const C_SclString c_ParentName = static_cast<C_SclString>(orc_XmlTagBaseName + "s");
   const C_SclString c_ChildName = orc_XmlTagBaseName;

   orc_XmlParser.CreateAndSelectNodeChild(c_ParentName);
   for (uint32_t u32_ItPath = 0; u32_ItPath < orc_Paths.size(); ++u32_ItPath)
   {
      const C_SclString & rc_Path = orc_Paths[u32_ItPath];
      orc_XmlParser.CreateNodeChild(c_ChildName, rc_Path);
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
void C_OscViewFiler::mh_SaveNodeUpdateInformationParamInfo(const std::vector<C_OscViewNodeUpdateParamInfo> & orc_Info,
                                                           C_OscXmlParserBase & orc_XmlParser)
{
   orc_XmlParser.CreateAndSelectNodeChild("param-sets");
   for (uint32_t u32_ItParamSet = 0UL; u32_ItParamSet < orc_Info.size(); ++u32_ItParamSet)
   {
      const C_OscViewNodeUpdateParamInfo & rc_ParamSetInfo = orc_Info[u32_ItParamSet];
      orc_XmlParser.CreateAndSelectNodeChild("param-set");
      orc_XmlParser.SetAttributeUint32("last-known-crc", rc_ParamSetInfo.GetLastKnownCrc());
      orc_XmlParser.CreateNodeChild("path", rc_ParamSetInfo.GetPath());
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
void C_OscViewFiler::mh_SaveNodeUpdateInformationSkipUpdateOfFiles(const std::vector<bool> & orc_Flags,
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
void C_OscViewFiler::mh_SaveNodeUpdateInformationPem(const C_OscViewNodeUpdate & orc_NodeUpdateInformation,
                                                     C_OscXmlParserBase & orc_XmlParser)
{
   C_OscViewNodeUpdate::E_StateSecurity e_StateSecurity;
   C_OscViewNodeUpdate::E_StateDebugger e_StateDebugger;
   orc_XmlParser.CreateAndSelectNodeChild("pem-file");
   orc_XmlParser.SetAttributeBool("skip", orc_NodeUpdateInformation.GetSkipUpdateOfPemFile());
   orc_XmlParser.CreateNodeChild("path", orc_NodeUpdateInformation.GetPemFilePath());
   orc_XmlParser.CreateAndSelectNodeChild("states");
   orc_NodeUpdateInformation.GetStates(e_StateSecurity, e_StateDebugger);
   orc_XmlParser.CreateNodeChild("security", C_OscViewFiler::h_PemFileStateSecurityToString(e_StateSecurity));
   orc_XmlParser.CreateNodeChild("debugger", C_OscViewFiler::h_PemFileStateDebuggerToString(e_StateDebugger));
   //Return
   tgl_assert(orc_XmlParser.SelectNodeParent() == "pem-file");
   //Return
   tgl_assert(orc_XmlParser.SelectNodeParent() == "node-specific-update-information");
}
