//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       NodeSquad reader/writer

   NodeSquad reader/writer

   \copyright   Copyright 2021 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.hpp"

#include "TglUtils.hpp"
#include "stwtypes.hpp"
#include "stwerrors.hpp"
#include "C_OscNodeSquadFiler.hpp"
#include "C_OscLoggingHandler.hpp"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw::tgl;

using namespace stw::errors;
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
C_OscNodeSquadFiler::C_OscNodeSquadFiler()
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Load node groups

   \param[in,out]  orc_NodeGroups   Node groups
   \param[in,out]  orc_XmlParser    XML parser

   \return
   C_NO_ERR   no error
   C_CONFIG   content is invalid or incomplete
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_OscNodeSquadFiler::h_LoadNodeGroups(std::vector<C_OscNodeSquad> & orc_NodeGroups,
                                              C_OscXmlParserBase & orc_XmlParser)
{
   int32_t s32_Retval = C_NO_ERR;

   orc_NodeGroups.clear();
   if (orc_XmlParser.SelectNodeChild("node-groups") == "node-groups")
   {
      uint32_t u32_LengthNodeGroup;

      s32_Retval = orc_XmlParser.GetAttributeUint32Error("length", u32_LengthNodeGroup);
      if (s32_Retval == C_NO_ERR)
      {
         stw::scl::C_SclString c_NodeName;
         orc_NodeGroups.reserve(u32_LengthNodeGroup);
         c_NodeName = orc_XmlParser.SelectNodeChild("node-group");
         if (c_NodeName == "node-group")
         {
            do
            {
               C_OscNodeSquad c_NewNodeGroup;
               s32_Retval = C_OscNodeSquadFiler::h_LoadNodeGroup(c_NewNodeGroup, orc_XmlParser);
               if (s32_Retval == C_NO_ERR)
               {
                  orc_NodeGroups.push_back(c_NewNodeGroup);
               }
               c_NodeName = orc_XmlParser.SelectNodeNext("node-group");
            }
            while ((c_NodeName == "node-group") && (s32_Retval == C_NO_ERR));
            if (s32_Retval == C_NO_ERR)
            {
               if (u32_LengthNodeGroup != orc_NodeGroups.size())
               {
                  s32_Retval = C_CONFIG;
                  osc_write_log_error("Loading system definition",
                                      "Expected " + stw::scl::C_SclString::IntToStr(
                                         u32_LengthNodeGroup) + " node groups but received " +
                                      stw::scl::C_SclString::IntToStr(orc_NodeGroups.size()) + " node groups.");
               }
               if (s32_Retval == C_NO_ERR)
               {
                  tgl_assert(orc_XmlParser.SelectNodeParent() == "node-groups");
               }
            }
         }
      }
      if (s32_Retval == C_NO_ERR)
      {
         orc_XmlParser.SelectNodeParent();
      }
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Load node group

   \param[in,out]  orc_NodeGroup    Node group
   \param[in,out]  orc_XmlParser    XML parser

   \return
   C_NO_ERR   no error
   C_CONFIG   content is invalid or incomplete
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_OscNodeSquadFiler::h_LoadNodeGroup(C_OscNodeSquad & orc_NodeGroup, C_OscXmlParserBase & orc_XmlParser)
{
   int32_t s32_Retval = orc_XmlParser.SelectNodeChildError("base-name");

   if (s32_Retval == C_NO_ERR)
   {
      orc_NodeGroup.c_BaseName = orc_XmlParser.GetNodeContent();
      orc_XmlParser.SelectNodeParent();
      s32_Retval = orc_XmlParser.SelectNodeChildError("sub-node-indices");
      if (s32_Retval == C_NO_ERR)
      {
         uint32_t u32_LengthSubIndices;
         s32_Retval = orc_XmlParser.GetAttributeUint32Error("length", u32_LengthSubIndices);
         if (s32_Retval == C_NO_ERR)
         {
            orc_NodeGroup.c_SubNodeIndexes.clear();
            orc_NodeGroup.c_SubNodeIndexes.reserve(u32_LengthSubIndices);
            s32_Retval = orc_XmlParser.SelectNodeChildError("sub-node-index");
            if (s32_Retval == C_NO_ERR)
            {
               stw::scl::C_SclString c_NodeName;
               do
               {
                  uint32_t u32_Value;
                  s32_Retval = orc_XmlParser.GetAttributeUint32Error("value", u32_Value);
                  if (s32_Retval == C_NO_ERR)
                  {
                     orc_NodeGroup.c_SubNodeIndexes.push_back(u32_Value);
                  }
                  c_NodeName = orc_XmlParser.SelectNodeNext("sub-node-index");
               }
               while ((c_NodeName == "sub-node-index") && (s32_Retval == C_NO_ERR));
               if (s32_Retval == C_NO_ERR)
               {
                  if (u32_LengthSubIndices != orc_NodeGroup.c_SubNodeIndexes.size())
                  {
                     s32_Retval = C_CONFIG;
                     osc_write_log_error("Loading system definition",
                                         "Expected " + stw::scl::C_SclString::IntToStr(
                                            u32_LengthSubIndices) + " sub node indices but received " +
                                         stw::scl::C_SclString::IntToStr(
                                            orc_NodeGroup.c_SubNodeIndexes.size()) + " sub node indices.");
                  }
               }
               if (s32_Retval == C_NO_ERR)
               {
                  tgl_assert(orc_XmlParser.SelectNodeParent() == "sub-node-indices");
               }
            }
         }
         if (s32_Retval == C_NO_ERR)
         {
            orc_XmlParser.SelectNodeParent();
         }
      }
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Save node groups

   \param[in]      orc_NodeGroups   Node groups
   \param[in,out]  orc_XmlParser    XML parser
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OscNodeSquadFiler::h_SaveNodeGroups(const std::vector<C_OscNodeSquad> & orc_NodeGroups,
                                           C_OscXmlParserBase & orc_XmlParser)
{
   orc_XmlParser.CreateAndSelectNodeChild("node-groups");
   orc_XmlParser.SetAttributeUint32("length", static_cast<uint32_t>(orc_NodeGroups.size()));
   for (uint32_t u32_ItGroup = 0UL; u32_ItGroup < orc_NodeGroups.size(); ++u32_ItGroup)
   {
      const C_OscNodeSquad & rc_NodeGroup = orc_NodeGroups[u32_ItGroup];
      C_OscNodeSquadFiler::h_SaveNodeGroup(rc_NodeGroup, orc_XmlParser);
   }
   orc_XmlParser.SelectNodeParent();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Save node group

   \param[in]      orc_NodeGroup    Node group
   \param[in,out]  orc_XmlParser    XML parser
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OscNodeSquadFiler::h_SaveNodeGroup(const C_OscNodeSquad & orc_NodeGroup, C_OscXmlParserBase & orc_XmlParser)
{
   orc_XmlParser.CreateAndSelectNodeChild("node-group");
   orc_XmlParser.CreateNodeChild("base-name", orc_NodeGroup.c_BaseName);
   orc_XmlParser.CreateAndSelectNodeChild("sub-node-indices");
   orc_XmlParser.SetAttributeUint32("length", static_cast<uint32_t>(orc_NodeGroup.c_SubNodeIndexes.size()));
   for (uint32_t u32_ItIndex = 0UL; u32_ItIndex < orc_NodeGroup.c_SubNodeIndexes.size(); ++u32_ItIndex)
   {
      orc_XmlParser.CreateAndSelectNodeChild("sub-node-index");
      orc_XmlParser.SetAttributeUint32("value", orc_NodeGroup.c_SubNodeIndexes[u32_ItIndex]);
      tgl_assert(orc_XmlParser.SelectNodeParent() == "sub-node-indices");
   }
   tgl_assert(orc_XmlParser.SelectNodeParent() == "node-group");
   tgl_assert(orc_XmlParser.SelectNodeParent() == "node-groups");
}
