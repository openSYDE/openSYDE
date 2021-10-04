//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       NodeSquad reader/writer

   NodeSquad reader/writer

   \copyright   Copyright 2021 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.h"

#include "TGLUtils.h"
#include "stwtypes.h"
#include "stwerrors.h"
#include "C_OSCNodeSquadFiler.h"
#include "C_OSCLoggingHandler.h"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw_tgl;
using namespace stw_types;
using namespace stw_errors;
using namespace stw_opensyde_core;

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
C_OSCNodeSquadFiler::C_OSCNodeSquadFiler()
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Load node groups

   \param[in,out]  orc_NodeGroups   Node groups
   \param[in,out]  orc_XMLParser    XML parser

   \return
   C_NO_ERR   no error
   C_CONFIG   content is invalid or incomplete
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_OSCNodeSquadFiler::h_LoadNodeGroups(std::vector<C_OSCNodeSquad> & orc_NodeGroups,
                                             C_OSCXMLParserBase & orc_XMLParser)
{
   sint32 s32_Retval = C_NO_ERR;

   orc_NodeGroups.clear();
   if (orc_XMLParser.SelectNodeChild("node-groups") == "node-groups")
   {
      uint32 u32_LengthNodeGroup;

      s32_Retval = orc_XMLParser.GetAttributeUint32Error("length", u32_LengthNodeGroup);
      if (s32_Retval == C_NO_ERR)
      {
         stw_scl::C_SCLString c_NodeName;
         orc_NodeGroups.reserve(u32_LengthNodeGroup);
         c_NodeName = orc_XMLParser.SelectNodeChild("node-group");
         if (c_NodeName == "node-group")
         {
            do
            {
               C_OSCNodeSquad c_NewNodeGroup;
               s32_Retval = C_OSCNodeSquadFiler::h_LoadNodeGroup(c_NewNodeGroup, orc_XMLParser);
               if (s32_Retval == C_NO_ERR)
               {
                  orc_NodeGroups.push_back(c_NewNodeGroup);
               }
               c_NodeName = orc_XMLParser.SelectNodeNext("node-group");
            }
            while ((c_NodeName == "node-group") && (s32_Retval == C_NO_ERR));
            if (s32_Retval == C_NO_ERR)
            {
               if (u32_LengthNodeGroup != orc_NodeGroups.size())
               {
                  s32_Retval = C_CONFIG;
                  osc_write_log_error("Loading system definition",
                                      "Expected " + stw_scl::C_SCLString::IntToStr(
                                         u32_LengthNodeGroup) + " node groups but received " +
                                      stw_scl::C_SCLString::IntToStr(orc_NodeGroups.size()) + " node groups.");
               }
               if (s32_Retval == C_NO_ERR)
               {
                  tgl_assert(orc_XMLParser.SelectNodeParent() == "node-groups");
               }
            }
         }
      }
      if (s32_Retval == C_NO_ERR)
      {
         orc_XMLParser.SelectNodeParent();
      }
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Load node group

   \param[in,out]  orc_NodeGroup    Node group
   \param[in,out]  orc_XMLParser    XML parser

   \return
   C_NO_ERR   no error
   C_CONFIG   content is invalid or incomplete
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_OSCNodeSquadFiler::h_LoadNodeGroup(C_OSCNodeSquad & orc_NodeGroup, C_OSCXMLParserBase & orc_XMLParser)
{
   sint32 s32_Retval = orc_XMLParser.SelectNodeChildError("base-name");

   if (s32_Retval == C_NO_ERR)
   {
      orc_NodeGroup.c_BaseName = orc_XMLParser.GetNodeContent();
      orc_XMLParser.SelectNodeParent();
      s32_Retval = orc_XMLParser.SelectNodeChildError("sub-node-indices");
      if (s32_Retval == C_NO_ERR)
      {
         uint32 u32_LengthSubIndices;
         s32_Retval = orc_XMLParser.GetAttributeUint32Error("length", u32_LengthSubIndices);
         if (s32_Retval == C_NO_ERR)
         {
            orc_NodeGroup.c_SubNodeIndexes.clear();
            orc_NodeGroup.c_SubNodeIndexes.reserve(u32_LengthSubIndices);
            s32_Retval = orc_XMLParser.SelectNodeChildError("sub-node-index");
            if (s32_Retval == C_NO_ERR)
            {
               stw_scl::C_SCLString c_NodeName;
               do
               {
                  uint32 u32_Value;
                  s32_Retval = orc_XMLParser.GetAttributeUint32Error("value", u32_Value);
                  if (s32_Retval == C_NO_ERR)
                  {
                     orc_NodeGroup.c_SubNodeIndexes.push_back(u32_Value);
                  }
                  c_NodeName = orc_XMLParser.SelectNodeNext("sub-node-index");
               }
               while ((c_NodeName == "sub-node-index") && (s32_Retval == C_NO_ERR));
               if (s32_Retval == C_NO_ERR)
               {
                  if (u32_LengthSubIndices != orc_NodeGroup.c_SubNodeIndexes.size())
                  {
                     s32_Retval = C_CONFIG;
                     osc_write_log_error("Loading system definition",
                                         "Expected " + stw_scl::C_SCLString::IntToStr(
                                            u32_LengthSubIndices) + " sub node indices but received " +
                                         stw_scl::C_SCLString::IntToStr(
                                            orc_NodeGroup.c_SubNodeIndexes.size()) + " sub node indices.");
                  }
               }
               if (s32_Retval == C_NO_ERR)
               {
                  tgl_assert(orc_XMLParser.SelectNodeParent() == "sub-node-indices");
               }
            }
         }
         if (s32_Retval == C_NO_ERR)
         {
            orc_XMLParser.SelectNodeParent();
         }
      }
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Save node groups

   \param[in]      orc_NodeGroups   Node groups
   \param[in,out]  orc_XMLParser    XML parser
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OSCNodeSquadFiler::h_SaveNodeGroups(const std::vector<C_OSCNodeSquad> & orc_NodeGroups,
                                           C_OSCXMLParserBase & orc_XMLParser)
{
   orc_XMLParser.CreateAndSelectNodeChild("node-groups");
   orc_XMLParser.SetAttributeUint32("length", orc_NodeGroups.size());
   for (uint32 u32_ItGroup = 0UL; u32_ItGroup < orc_NodeGroups.size(); ++u32_ItGroup)
   {
      const C_OSCNodeSquad & rc_NodeGroup = orc_NodeGroups[u32_ItGroup];
      C_OSCNodeSquadFiler::h_SaveNodeGroup(rc_NodeGroup, orc_XMLParser);
   }
   orc_XMLParser.SelectNodeParent();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Save node group

   \param[in]      orc_NodeGroup    Node group
   \param[in,out]  orc_XMLParser    XML parser
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OSCNodeSquadFiler::h_SaveNodeGroup(const C_OSCNodeSquad & orc_NodeGroup, C_OSCXMLParserBase & orc_XMLParser)
{
   orc_XMLParser.CreateAndSelectNodeChild("node-group");
   orc_XMLParser.CreateNodeChild("base-name", orc_NodeGroup.c_BaseName);
   orc_XMLParser.CreateAndSelectNodeChild("sub-node-indices");
   orc_XMLParser.SetAttributeUint32("length", orc_NodeGroup.c_SubNodeIndexes.size());
   for (uint32 u32_ItIndex = 0UL; u32_ItIndex < orc_NodeGroup.c_SubNodeIndexes.size(); ++u32_ItIndex)
   {
      orc_XMLParser.CreateAndSelectNodeChild("sub-node-index");
      orc_XMLParser.SetAttributeUint32("value", orc_NodeGroup.c_SubNodeIndexes[u32_ItIndex]);
      tgl_assert(orc_XMLParser.SelectNodeParent() == "sub-node-indices");
   }
   tgl_assert(orc_XMLParser.SelectNodeParent() == "node-group");
   tgl_assert(orc_XMLParser.SelectNodeParent() == "node-groups");
}
