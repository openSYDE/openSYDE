//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Node data pool reader/writer (V3) (implementation)

   Node data pool reader/writer (V3)

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.h"

#include <sstream>
#include "stwtypes.h"
#include "stwerrors.h"
#include "C_OSCSystemFilerUtil.h"
#include "C_OSCNodeDataPoolFiler.h"
#include "TGLUtils.h"
#include "C_OSCLoggingHandler.h"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw_types;
using namespace stw_opensyde_core;
using namespace stw_errors;
using namespace stw_scl;
using namespace stw_tgl;

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
C_OSCNodeDataPoolFiler::C_OSCNodeDataPoolFiler(void)
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Load datapool file

   \param[out] orc_NodeDataPool Data storage
   \param[in]  orc_FilePath     File path

   \return
   C_NO_ERR   data read
   C_CONFIG   content of file is invalid or incomplete
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_OSCNodeDataPoolFiler::h_LoadDataPoolFile(C_OSCNodeDataPool & orc_NodeDataPool,
                                                  const C_SCLString & orc_FilePath)
{
   C_OSCXMLParser c_XMLParser;
   sint32 s32_Retval = C_OSCSystemFilerUtil::mh_GetParserForExistingFile(c_XMLParser, orc_FilePath,
                                                                         "opensyde-dp-core-definition");

   if (s32_Retval == C_NO_ERR)
   {
      if (c_XMLParser.SelectNodeChild("data-pool") == "data-pool")
      {
         s32_Retval = C_OSCNodeDataPoolFiler::h_LoadDataPool(orc_NodeDataPool, c_XMLParser);
      }
      else
      {
         osc_write_log_error("Loading Datapool", "Could not find \"data-pool\" node.");
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
/*! \brief  Load node data pool

   Load node data from XML file
   pre-condition: the passed XML parser has the active node set to "data-pool"
   post-condition: the passed XML parser has the active node set to the same "data-pool"

   \param[out]    orc_NodeDataPool data storage
   \param[in,out] orc_XMLParser    XML with data-pool active

   \return
   C_NO_ERR   data read
   C_CONFIG   content of file is invalid or incomplete
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_OSCNodeDataPoolFiler::h_LoadDataPool(C_OSCNodeDataPool & orc_NodeDataPool, C_OSCXMLParserBase & orc_XMLParser)
{
   sint32 s32_Retval = C_NO_ERR;

   //Related application
   if (orc_XMLParser.AttributeExists("related-application-index") == true)
   {
      orc_NodeDataPool.s32_RelatedDataBlockIndex = orc_XMLParser.GetAttributeSint32("related-application-index");
   }
   else
   {
      orc_NodeDataPool.s32_RelatedDataBlockIndex = -1;
   }
   orc_NodeDataPool.q_IsSafety = orc_XMLParser.GetAttributeBool("is-safety");
   if (orc_XMLParser.AttributeExists("scope-is-private") == true)
   {
      orc_NodeDataPool.q_ScopeIsPrivate = orc_XMLParser.GetAttributeBool("scope-is-private");
   }
   else
   {
      orc_NodeDataPool.q_ScopeIsPrivate = true;
   }
   orc_NodeDataPool.u32_NvMStartAddress = orc_XMLParser.GetAttributeUint32("nvm-start-address");
   orc_NodeDataPool.u32_NvMSize = orc_XMLParser.GetAttributeUint32("nvm-size");
   if (orc_XMLParser.SelectNodeChild("type") == "type")
   {
      s32_Retval = h_StringToDataPool(orc_XMLParser.GetNodeContent(), orc_NodeDataPool.e_Type);
      if (s32_Retval == C_NO_ERR)
      {
         //Return
         tgl_assert(orc_XMLParser.SelectNodeParent() == "data-pool");
      }
   }
   else
   {
      osc_write_log_error("Loading Datapool", "Could not find \"type\" node.");
      s32_Retval = C_CONFIG;
   }
   if (orc_XMLParser.SelectNodeChild("name") == "name")
   {
      orc_NodeDataPool.c_Name = orc_XMLParser.GetNodeContent();
      //Return
      tgl_assert(orc_XMLParser.SelectNodeParent() == "data-pool");
   }
   else
   {
      osc_write_log_error("Loading Datapool", "Could not find \"name\" node.");
      s32_Retval = C_CONFIG;
   }
   if (orc_XMLParser.SelectNodeChild("version") == "version")
   {
      orc_NodeDataPool.au8_Version[0] = static_cast<uint8>(orc_XMLParser.GetAttributeUint32("major"));
      orc_NodeDataPool.au8_Version[1] = static_cast<uint8>(orc_XMLParser.GetAttributeUint32("minor"));
      orc_NodeDataPool.au8_Version[2] = static_cast<uint8>(orc_XMLParser.GetAttributeUint32("release"));
      //Return
      tgl_assert(orc_XMLParser.SelectNodeParent() == "data-pool");
   }
   else
   {
      osc_write_log_error("Loading Datapool", "Could not find \"version\" node.");
      s32_Retval = C_CONFIG;
   }
   if (orc_XMLParser.SelectNodeChild("comment") == "comment")
   {
      orc_NodeDataPool.c_Comment = orc_XMLParser.GetNodeContent();
      //Return
      tgl_assert(orc_XMLParser.SelectNodeParent() == "data-pool");
   }
   else
   {
      osc_write_log_error("Loading Datapool", "Could not find \"comment\" node.");
      s32_Retval = C_CONFIG;
   }
   if ((orc_XMLParser.SelectNodeChild("lists") == "lists") && (s32_Retval == C_NO_ERR))
   {
      s32_Retval = h_LoadDataPoolLists(orc_NodeDataPool.c_Lists, orc_XMLParser);

      if (s32_Retval == C_NO_ERR)
      {
         //Return
         tgl_assert(orc_XMLParser.SelectNodeParent() == "data-pool");
      }
   }
   else
   {
      osc_write_log_error("Loading Datapool", "Could not find \"lists\" node.");
      s32_Retval = C_CONFIG;
   }
   //Export settings to be defined
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Save node data pool

   Save node to XML file
   pre-condition: the passed XML parser has the active node set to "data-pool"
   post-condition: the passed XML parser has the active node set to the same "data-pool"

   \param[in]     orc_NodeDataPool data storage
   \param[in,out] orc_XMLParser    XML with data-pool active
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OSCNodeDataPoolFiler::h_SaveDataPool(const C_OSCNodeDataPool & orc_NodeDataPool,
                                            C_OSCXMLParserBase & orc_XMLParser)
{
   orc_XMLParser.SetAttributeSint32("related-application-index", orc_NodeDataPool.s32_RelatedDataBlockIndex);
   orc_XMLParser.SetAttributeBool("is-safety", orc_NodeDataPool.q_IsSafety);
   orc_XMLParser.SetAttributeBool("scope-is-private", orc_NodeDataPool.q_ScopeIsPrivate);
   orc_XMLParser.SetAttributeUint32("nvm-start-address", orc_NodeDataPool.u32_NvMStartAddress);
   orc_XMLParser.SetAttributeUint32("nvm-size", orc_NodeDataPool.u32_NvMSize);
   orc_XMLParser.CreateNodeChild("type", h_DataPoolToString(orc_NodeDataPool.e_Type));
   orc_XMLParser.CreateNodeChild("name", orc_NodeDataPool.c_Name);
   orc_XMLParser.CreateAndSelectNodeChild("version");
   orc_XMLParser.SetAttributeUint32("major", orc_NodeDataPool.au8_Version[0]);
   orc_XMLParser.SetAttributeUint32("minor", orc_NodeDataPool.au8_Version[1]);
   orc_XMLParser.SetAttributeUint32("release", orc_NodeDataPool.au8_Version[2]);
   //Return
   tgl_assert(orc_XMLParser.SelectNodeParent() == "data-pool");

   orc_XMLParser.CreateNodeChild("comment", orc_NodeDataPool.c_Comment);
   //Lists
   orc_XMLParser.CreateAndSelectNodeChild("lists");
   h_SaveDataPoolLists(orc_NodeDataPool.c_Lists, orc_XMLParser);
   //Return
   tgl_assert(orc_XMLParser.SelectNodeParent() == "data-pool");
   orc_XMLParser.CreateNodeChild("export-settings", "");
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Load node data pool list

   Load node data from XML file
   pre-condition: the passed XML parser has the active node set to "list"
   post-condition: the passed XML parser has the active node set to the same "list"

   \param[out]    orc_NodeDataPoolList data storage
   \param[in,out] orc_XMLParser        XML with data-pool active

   \return
   C_NO_ERR   data read
   C_CONFIG   content of file is invalid or incomplete
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_OSCNodeDataPoolFiler::h_LoadDataPoolList(C_OSCNodeDataPoolList & orc_NodeDataPoolList,
                                                  C_OSCXMLParserBase & orc_XMLParser)
{
   sint32 s32_Retval = C_NO_ERR;

   orc_NodeDataPoolList.q_NvMCRCActive = orc_XMLParser.GetAttributeBool("nvm-crc-active");
   orc_NodeDataPoolList.u32_NvMCRC = orc_XMLParser.GetAttributeUint32("nvm-crc");
   orc_NodeDataPoolList.u32_NvMStartAddress = orc_XMLParser.GetAttributeUint32("nvm-start-address");
   orc_NodeDataPoolList.u32_NvMSize = orc_XMLParser.GetAttributeUint32("nvm-size");
   if (orc_XMLParser.SelectNodeChild("name") == "name")
   {
      orc_NodeDataPoolList.c_Name = orc_XMLParser.GetNodeContent();
      //Return
      tgl_assert(orc_XMLParser.SelectNodeParent() == "list");
   }
   else
   {
      osc_write_log_error("Loading Datapool", "Could not find \"lists\".\"list\".\"name\" node.");
      s32_Retval = C_CONFIG;
   }
   if (orc_XMLParser.SelectNodeChild("comment") == "comment")
   {
      orc_NodeDataPoolList.c_Comment = orc_XMLParser.GetNodeContent();
      //Return
      tgl_assert(orc_XMLParser.SelectNodeParent() == "list");
   }
   else
   {
      osc_write_log_error("Loading Datapool", "Could not find \"lists\".\"list\".\"comment\" node.");
      s32_Retval = C_CONFIG;
   }
   //Data elements
   if (s32_Retval == C_NO_ERR)
   {
      if (orc_XMLParser.SelectNodeChild("data-elements") == "data-elements")
      {
         s32_Retval = h_LoadDataPoolListElements(orc_NodeDataPoolList.c_Elements, orc_XMLParser);
         if (s32_Retval == C_NO_ERR)
         {
            //Return
            tgl_assert(orc_XMLParser.SelectNodeParent() == "list");
         }
      }
      else
      {
         osc_write_log_error("Loading Datapool", "Could not find \"lists\".\"list\".\"data-elements\" node.");
         s32_Retval = C_CONFIG;
      }
   }
   //Data sets
   if ((orc_XMLParser.SelectNodeChild("data-sets") == "data-sets") && (s32_Retval == C_NO_ERR))
   {
      s32_Retval = h_LoadDataPoolListDataSets(orc_NodeDataPoolList.c_DataSets, orc_XMLParser);
      if (s32_Retval == C_NO_ERR)
      {
         //Return
         tgl_assert(orc_XMLParser.SelectNodeParent() == "list");
      }
   }
   else
   {
      osc_write_log_error("Loading Datapool", "Could not find \"lists\".\"list\".\"data-sets\" node.");
      s32_Retval = C_CONFIG;
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Save node data pool list

   Save node to XML file
   pre-condition: the passed XML parser has the active node set to "list"
   post-condition: the passed XML parser has the active node set to the same "list"

   \param[in]     orc_NodeDataPoolList data storage
   \param[in,out] orc_XMLParser        XML with data-pool active
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OSCNodeDataPoolFiler::h_SaveDataPoolList(const C_OSCNodeDataPoolList & orc_NodeDataPoolList,
                                                C_OSCXMLParserBase & orc_XMLParser)
{
   orc_XMLParser.SetAttributeBool("nvm-crc-active", orc_NodeDataPoolList.q_NvMCRCActive);
   orc_XMLParser.SetAttributeUint32("nvm-crc", orc_NodeDataPoolList.u32_NvMCRC);
   orc_XMLParser.SetAttributeUint32("nvm-start-address", orc_NodeDataPoolList.u32_NvMStartAddress);
   orc_XMLParser.SetAttributeUint32("nvm-size", orc_NodeDataPoolList.u32_NvMSize);
   orc_XMLParser.CreateNodeChild("name", orc_NodeDataPoolList.c_Name);
   orc_XMLParser.CreateNodeChild("comment", orc_NodeDataPoolList.c_Comment);
   //Data elements
   orc_XMLParser.CreateAndSelectNodeChild("data-elements");
   h_SaveDataPoolListElements(orc_NodeDataPoolList.c_Elements, orc_XMLParser);
   //Return
   tgl_assert(orc_XMLParser.SelectNodeParent() == "list");
   //Data sets
   orc_XMLParser.CreateAndSelectNodeChild("data-sets");
   h_SaveDataPoolListDataSets(orc_NodeDataPoolList.c_DataSets, orc_XMLParser);
   //Return
   tgl_assert(orc_XMLParser.SelectNodeParent() == "list");
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Load node data pool element

   Load node data from XML file
   pre-condition: the passed XML parser has the active node set to "data-element"
   post-condition: the passed XML parser has the active node set to the same "data-element"

   \param[out]    orc_NodeDataPoolListElement data storage
   \param[in,out] orc_XMLParser               XML with list active

   \return
   C_NO_ERR   data read
   C_CONFIG   content of file is invalid or incomplete
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_OSCNodeDataPoolFiler::h_LoadDataPoolElement(C_OSCNodeDataPoolListElement & orc_NodeDataPoolListElement,
                                                     C_OSCXMLParserBase & orc_XMLParser)
{
   sint32 s32_Retval = C_NO_ERR;

   orc_NodeDataPoolListElement.f64_Factor = orc_XMLParser.GetAttributeFloat64("factor");
   orc_NodeDataPoolListElement.f64_Offset = orc_XMLParser.GetAttributeFloat64("offset");
   orc_NodeDataPoolListElement.q_DiagEventCall = orc_XMLParser.GetAttributeBool("diag-event-call");
   orc_NodeDataPoolListElement.u32_NvMStartAddress = orc_XMLParser.GetAttributeUint32("nvm-start-address");
   if (orc_XMLParser.SelectNodeChild("name") == "name")
   {
      orc_NodeDataPoolListElement.c_Name = orc_XMLParser.GetNodeContent();
      //Return
      tgl_assert(orc_XMLParser.SelectNodeParent() == "data-element");
   }
   else
   {
      osc_write_log_error("Loading data element", "Could not find \"name\" node.");
      s32_Retval = C_CONFIG;
   }
   if (s32_Retval == C_NO_ERR)
   {
      s32_Retval = h_LoadDataPoolElementType(orc_NodeDataPoolListElement.c_Value, orc_XMLParser);
   }
   if (s32_Retval == C_NO_ERR)
   {
      if (orc_XMLParser.SelectNodeChild("min-value") == "min-value")
      {
         //copy over value so we have the correct type:
         orc_NodeDataPoolListElement.c_MinValue = orc_NodeDataPoolListElement.c_Value;

         s32_Retval = h_LoadDataPoolElementValue(orc_NodeDataPoolListElement.c_MinValue, orc_XMLParser);
         // Use minimum value as init value for value and NVM value
         orc_NodeDataPoolListElement.c_NvmValue = orc_NodeDataPoolListElement.c_MinValue;
         orc_NodeDataPoolListElement.c_Value = orc_NodeDataPoolListElement.c_MinValue;
         //Return
         tgl_assert(orc_XMLParser.SelectNodeParent() == "data-element");
      }
      else
      {
         osc_write_log_error("Loading data element", "Could not find \"min-value\" node.");
         s32_Retval = C_CONFIG;
      }
   }

   if (s32_Retval == C_NO_ERR)
   {
      if (orc_XMLParser.SelectNodeChild("max-value") == "max-value")
      {
         //copy over value so we have the correct type:
         orc_NodeDataPoolListElement.c_MaxValue = orc_NodeDataPoolListElement.c_Value;

         s32_Retval = h_LoadDataPoolElementValue(orc_NodeDataPoolListElement.c_MaxValue, orc_XMLParser);
         //Return
         tgl_assert(orc_XMLParser.SelectNodeParent() == "data-element");
      }
      else
      {
         osc_write_log_error("Loading data element", "Could not find \"max-value\" node.");
         s32_Retval = C_CONFIG;
      }
   }

   if (s32_Retval == C_NO_ERR)
   {
      if (orc_XMLParser.SelectNodeChild("comment") == "comment")
      {
         orc_NodeDataPoolListElement.c_Comment = orc_XMLParser.GetNodeContent();
         //Return
         tgl_assert(orc_XMLParser.SelectNodeParent() == "data-element");
      }
      else
      {
         osc_write_log_error("Loading data element", "Could not find \"comment\" node.");
         s32_Retval = C_CONFIG;
      }
   }

   if (s32_Retval == C_NO_ERR)
   {
      if (orc_XMLParser.SelectNodeChild("unit") == "unit")
      {
         orc_NodeDataPoolListElement.c_Unit = orc_XMLParser.GetNodeContent();
         //Return
         tgl_assert(orc_XMLParser.SelectNodeParent() == "data-element");
      }
      else
      {
         osc_write_log_error("Loading data element", "Could not find \"unit\" node.");
         s32_Retval = C_CONFIG;
      }
   }

   if (s32_Retval == C_NO_ERR)
   {
      if (orc_XMLParser.SelectNodeChild("access") == "access")
      {
         s32_Retval =
            mh_StringToNodeDataPoolElementAccess(orc_XMLParser.GetNodeContent(), orc_NodeDataPoolListElement.e_Access);
         //Return
         tgl_assert(orc_XMLParser.SelectNodeParent() == "data-element");
      }
      else
      {
         osc_write_log_error("Loading data element", "Could not find \"access\" node.");
         s32_Retval = C_CONFIG;
      }
   }

   if (s32_Retval == C_NO_ERR)
   {
      if (orc_XMLParser.SelectNodeChild("data-set-values") == "data-set-values")
      {
         s32_Retval = h_LoadDataPoolListElementDataSetValues(orc_NodeDataPoolListElement.c_Value,
                                                             orc_NodeDataPoolListElement.c_DataSetValues,
                                                             orc_XMLParser);
         //Return
         tgl_assert(orc_XMLParser.SelectNodeParent() == "data-element");
      }
      else
      {
         osc_write_log_error("Loading data element", "Could not find \"data-set-values\" node.");
         s32_Retval = C_CONFIG;
      }
   }

   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Save node data pool element

   Save node to XML file
   pre-condition: the passed XML parser has the active node set to "data-element"
   post-condition: the passed XML parser has the active node set to the same "data-element"

   \param[in]     orc_NodeDataPoolListElement data storage
   \param[in,out] orc_XMLParser               XML with list active
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OSCNodeDataPoolFiler::h_SaveDataPoolElement(const C_OSCNodeDataPoolListElement & orc_NodeDataPoolListElement,
                                                   C_OSCXMLParserBase & orc_XMLParser)
{
   orc_XMLParser.SetAttributeFloat64("factor", orc_NodeDataPoolListElement.f64_Factor);
   orc_XMLParser.SetAttributeFloat64("offset", orc_NodeDataPoolListElement.f64_Offset);
   orc_XMLParser.SetAttributeBool("diag-event-call", orc_NodeDataPoolListElement.q_DiagEventCall);
   orc_XMLParser.SetAttributeUint32("nvm-start-address", orc_NodeDataPoolListElement.u32_NvMStartAddress);
   orc_XMLParser.CreateNodeChild("name", orc_NodeDataPoolListElement.c_Name);
   h_SaveDataPoolElementType(orc_NodeDataPoolListElement.c_Value, orc_XMLParser);
   orc_XMLParser.CreateNodeChild("comment", orc_NodeDataPoolListElement.c_Comment);
   h_SaveDataPoolElementValue("min-value", orc_NodeDataPoolListElement.c_MinValue, orc_XMLParser);
   h_SaveDataPoolElementValue("max-value", orc_NodeDataPoolListElement.c_MaxValue, orc_XMLParser);
   orc_XMLParser.CreateNodeChild("unit", orc_NodeDataPoolListElement.c_Unit);
   orc_XMLParser.CreateNodeChild("access", mh_NodeDataPoolElementAccessToString(orc_NodeDataPoolListElement.e_Access));
   orc_XMLParser.CreateAndSelectNodeChild("data-set-values");
   h_SaveDataPoolListElementDataSetValues(orc_NodeDataPoolListElement.c_DataSetValues, orc_XMLParser);
   //Return to parent
   tgl_assert(orc_XMLParser.SelectNodeParent() == "data-element");
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Load node data pool lists

   Load node data from XML file
   pre-condition: the passed XML parser has the active node set to "lists"
   post-condition: the passed XML parser has the active node set to the same "lists"

   \param[out]    orc_NodeDataPoolLists data storage
   \param[in,out] orc_XMLParser         XML with data-pool active

   \return
   C_NO_ERR   data read
   C_CONFIG   content of file is invalid or incomplete
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_OSCNodeDataPoolFiler::h_LoadDataPoolLists(std::vector<C_OSCNodeDataPoolList> & orc_NodeDataPoolLists,
                                                   C_OSCXMLParserBase & orc_XMLParser)
{
   sint32 s32_Retval = C_NO_ERR;
   C_SCLString c_CurNodeList;
   uint32 u32_ExpectedSize = 0UL;
   const bool q_ExpectedSizeHere = orc_XMLParser.AttributeExists("length");

   //Check optional length
   if (q_ExpectedSizeHere == true)
   {
      u32_ExpectedSize = orc_XMLParser.GetAttributeUint32("length");
      orc_NodeDataPoolLists.reserve(u32_ExpectedSize);
   }

   c_CurNodeList = orc_XMLParser.SelectNodeChild("list");

   //Clear
   orc_NodeDataPoolLists.clear();
   if (c_CurNodeList == "list")
   {
      do
      {
         C_OSCNodeDataPoolList c_CurList;

         if (s32_Retval == C_NO_ERR)
         {
            s32_Retval = h_LoadDataPoolList(c_CurList, orc_XMLParser);
         }

         orc_NodeDataPoolLists.push_back(c_CurList);
         //Next
         c_CurNodeList = orc_XMLParser.SelectNodeNext("list");
      }
      while (c_CurNodeList == "list");

      if (s32_Retval == C_NO_ERR)
      {
         //Return
         tgl_assert(orc_XMLParser.SelectNodeParent() == "lists");
      }
   }
   //Compare length
   if ((s32_Retval == C_NO_ERR) && (q_ExpectedSizeHere == true))
   {
      if (u32_ExpectedSize != orc_NodeDataPoolLists.size())
      {
         C_SCLString c_Tmp;
         c_Tmp.PrintFormatted("Unexpected list count, expected: %i, got %i", u32_ExpectedSize,
                              orc_NodeDataPoolLists.size());
         osc_write_log_warning("Load file", c_Tmp.c_str());
      }
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Save node data pool lists

   Save node to XML file
   pre-condition: the passed XML parser has the active node set to "data-pool"
   post-condition: the passed XML parser has the active node set to the same "data-pool"

   \param[in]     orc_NodeDataPoolLists data storage
   \param[in,out] orc_XMLParser         XML with data-pool active
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OSCNodeDataPoolFiler::h_SaveDataPoolLists(const std::vector<C_OSCNodeDataPoolList> & orc_NodeDataPoolLists,
                                                 C_OSCXMLParserBase & orc_XMLParser)
{
   orc_XMLParser.SetAttributeUint32("length", orc_NodeDataPoolLists.size());
   for (uint32 u32_ItList = 0; u32_ItList < orc_NodeDataPoolLists.size(); ++u32_ItList)
   {
      orc_XMLParser.CreateAndSelectNodeChild("list");
      h_SaveDataPoolList(orc_NodeDataPoolLists[u32_ItList], orc_XMLParser);
      //Return
      tgl_assert(orc_XMLParser.SelectNodeParent() == "lists");
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Load node data pool elements

   Load node data from XML file
   pre-condition: the passed XML parser has the active node set to "data-elements"
   post-condition: the passed XML parser has the active node set to the same "data-elements"

   \param[out]    orc_NodeDataPoolListElements data storage
   \param[in,out] orc_XMLParser                XML with list active

   \return
   C_NO_ERR   data read
   C_CONFIG   content of file is invalid or incomplete
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_OSCNodeDataPoolFiler::h_LoadDataPoolListElements(
   std::vector<C_OSCNodeDataPoolListElement> & orc_NodeDataPoolListElements, C_OSCXMLParserBase & orc_XMLParser)
{
   sint32 s32_Retval = C_NO_ERR;
   C_SCLString c_CurNodeDataElement;
   uint32 u32_ExpectedSize = 0UL;
   const bool q_ExpectedSizeHere = orc_XMLParser.AttributeExists("length");

   //Check optional length
   if (q_ExpectedSizeHere == true)
   {
      u32_ExpectedSize = orc_XMLParser.GetAttributeUint32("length");
      orc_NodeDataPoolListElements.reserve(u32_ExpectedSize);
   }

   c_CurNodeDataElement = orc_XMLParser.SelectNodeChild("data-element");

   //Clear
   orc_NodeDataPoolListElements.clear();
   if (c_CurNodeDataElement == "data-element")
   {
      do
      {
         C_OSCNodeDataPoolListElement c_CurDataElement;

         if (s32_Retval == C_NO_ERR)
         {
            s32_Retval = h_LoadDataPoolElement(c_CurDataElement, orc_XMLParser);
         }

         //Append
         orc_NodeDataPoolListElements.push_back(c_CurDataElement);

         //Next
         c_CurNodeDataElement = orc_XMLParser.SelectNodeNext("data-element");
      }
      while (c_CurNodeDataElement == "data-element");
      //Return
      tgl_assert(orc_XMLParser.SelectNodeParent() == "data-elements");
   }
   //Compare length
   if ((s32_Retval == C_NO_ERR) && (q_ExpectedSizeHere == true))
   {
      if (u32_ExpectedSize != orc_NodeDataPoolListElements.size())
      {
         C_SCLString c_Tmp;
         c_Tmp.PrintFormatted("Unexpected data element count, expected: %i, got %i", u32_ExpectedSize,
                              orc_NodeDataPoolListElements.size());
         osc_write_log_warning("Load file", c_Tmp.c_str());
      }
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Save node data pool elements

   Save node to XML file
   pre-condition: the passed XML parser has the active node set to "data-elements"
   post-condition: the passed XML parser has the active node set to the same "data-elements"

   \param[in]     orc_NodeDataPoolListElements data storage
   \param[in,out] orc_XMLParser                XML with list active
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OSCNodeDataPoolFiler::h_SaveDataPoolListElements(
   const std::vector<C_OSCNodeDataPoolListElement> & orc_NodeDataPoolListElements, C_OSCXMLParserBase & orc_XMLParser)
{
   orc_XMLParser.SetAttributeUint32("length", orc_NodeDataPoolListElements.size());
   for (uint32 u32_ItDataElement = 0; u32_ItDataElement < orc_NodeDataPoolListElements.size();
        ++u32_ItDataElement)
   {
      orc_XMLParser.CreateAndSelectNodeChild("data-element");
      h_SaveDataPoolElement(orc_NodeDataPoolListElements[u32_ItDataElement], orc_XMLParser);
      //Return
      tgl_assert(orc_XMLParser.SelectNodeParent() == "data-elements");
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Load node data pool list element data set values

   Load node data from XML file
   pre-condition: the passed XML parser has the active node set to "data-set-values"
   post-condition: the passed XML parser has the active node set to the same "data-set-values"

   All returned elements will be of the type defined by orc_ContentType.

   \param[in]     orc_ContentType                          type reference (see description)
   \param[out]    orc_NodeDataPoolListElementDataSetValues data storage
   \param[in,out] orc_XMLParser                            XML with list active

   \return
   C_NO_ERR   data read
   C_CONFIG   content of file is invalid or incomplete
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_OSCNodeDataPoolFiler::h_LoadDataPoolListElementDataSetValues(const C_OSCNodeDataPoolContent & orc_ContentType,
                                                                      std::vector<C_OSCNodeDataPoolContent> & orc_NodeDataPoolListElementDataSetValues,
                                                                      C_OSCXMLParserBase & orc_XMLParser)
{
   sint32 s32_Retval = C_NO_ERR;
   C_SCLString c_CurNodeDataSetValue = orc_XMLParser.SelectNodeChild("data-set-value");

   if (c_CurNodeDataSetValue == "data-set-value")
   {
      do
      {
         C_OSCNodeDataPoolContent c_CurDataSetValue = orc_ContentType; //pre set content type

         if (s32_Retval == C_NO_ERR)
         {
            s32_Retval = h_LoadDataPoolElementValue(c_CurDataSetValue, orc_XMLParser);
         }

         //Append
         orc_NodeDataPoolListElementDataSetValues.push_back(c_CurDataSetValue);

         //Next
         c_CurNodeDataSetValue = orc_XMLParser.SelectNodeNext("data-set-value");
      }
      while (c_CurNodeDataSetValue == "data-set-value");
      //Return
      tgl_assert(orc_XMLParser.SelectNodeParent() == "data-set-values");
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Save node data pool list element data set values

   Save node to XML file
   pre-condition: the passed XML parser has the active node set to "data-set-values"
   post-condition: the passed XML parser has the active node set to the same "data-set-values"

   \param[in]     orc_NodeDataPoolListElementDataSetValues data storage
   \param[in,out] orc_XMLParser                            XML with list active
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OSCNodeDataPoolFiler::h_SaveDataPoolListElementDataSetValues(
   const std::vector<C_OSCNodeDataPoolContent> & orc_NodeDataPoolListElementDataSetValues,
   C_OSCXMLParserBase & orc_XMLParser)
{
   //Data set values
   for (uint32 u32_ItDataSetValue = 0; u32_ItDataSetValue < orc_NodeDataPoolListElementDataSetValues.size();
        ++u32_ItDataSetValue)
   {
      h_SaveDataPoolElementValue("data-set-value", orc_NodeDataPoolListElementDataSetValues[u32_ItDataSetValue],
                                 orc_XMLParser);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Load node data pool list data sets

   Load node data from XML file
   pre-condition: the passed XML parser has the active node set to "data-sets"
   post-condition: the passed XML parser has the active node set to the same "data-sets"

   \param[out]    orc_NodeDataPoolListDataSets data storage
   \param[in,out] orc_XMLParser                XML with list active

   \return
   C_NO_ERR   data read
   C_CONFIG   content of file is invalid or incomplete
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_OSCNodeDataPoolFiler::h_LoadDataPoolListDataSets(
   std::vector<C_OSCNodeDataPoolDataSet> & orc_NodeDataPoolListDataSets, C_OSCXMLParserBase & orc_XMLParser)
{
   sint32 s32_Retval = C_NO_ERR;
   C_SCLString c_CurNodeDataSet = orc_XMLParser.SelectNodeChild("data-set");

   orc_NodeDataPoolListDataSets.clear();
   if (c_CurNodeDataSet == "data-set")
   {
      do
      {
         C_OSCNodeDataPoolDataSet c_CurDataSet;

         if (orc_XMLParser.SelectNodeChild("name") == "name")
         {
            c_CurDataSet.c_Name = orc_XMLParser.GetNodeContent();
            //Return
            tgl_assert(orc_XMLParser.SelectNodeParent() == "data-set");
         }
         else
         {
            s32_Retval = C_CONFIG;
         }

         if (orc_XMLParser.SelectNodeChild("comment") == "comment")
         {
            c_CurDataSet.c_Comment = orc_XMLParser.GetNodeContent();
            //Return
            tgl_assert(orc_XMLParser.SelectNodeParent() == "data-set");
         }
         else
         {
            s32_Retval = C_CONFIG;
         }

         //Append
         orc_NodeDataPoolListDataSets.push_back(c_CurDataSet);

         //Next
         c_CurNodeDataSet = orc_XMLParser.SelectNodeNext("data-set");
      }
      while (c_CurNodeDataSet == "data-set");
      //Return
      tgl_assert(orc_XMLParser.SelectNodeParent() == "data-sets");
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Save node data pool list data sets

   Save node to XML file
   pre-condition: the passed XML parser has the active node set to "data-sets"
   post-condition: the passed XML parser has the active node set to the same "data-sets"

   \param[in]     orc_NodeDataPoolListDataSets data storage
   \param[in,out] orc_XMLParser                XML with list active
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OSCNodeDataPoolFiler::h_SaveDataPoolListDataSets(
   const std::vector<C_OSCNodeDataPoolDataSet> & orc_NodeDataPoolListDataSets, C_OSCXMLParserBase & orc_XMLParser)
{
   for (uint32 u32_ItDataSet = 0; u32_ItDataSet < orc_NodeDataPoolListDataSets.size(); ++u32_ItDataSet)
   {
      const C_OSCNodeDataPoolDataSet & rc_DataSet = orc_NodeDataPoolListDataSets[u32_ItDataSet];

      orc_XMLParser.CreateAndSelectNodeChild("data-set");
      orc_XMLParser.CreateNodeChild("name", rc_DataSet.c_Name);
      orc_XMLParser.CreateNodeChild("comment", rc_DataSet.c_Comment);
      //Return
      tgl_assert(orc_XMLParser.SelectNodeParent() == "data-sets");
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Transform data pool type to string

   \param[in] ore_DataPool Data pool type

   \return
   Stringified data pool type
*/
//----------------------------------------------------------------------------------------------------------------------
C_SCLString C_OSCNodeDataPoolFiler::h_DataPoolToString(const C_OSCNodeDataPool::E_Type & ore_DataPool)
{
   C_SCLString c_Retval;

   switch (ore_DataPool)
   {
   case C_OSCNodeDataPool::eDIAG:
      c_Retval = "diag";
      break;
   case C_OSCNodeDataPool::eCOM:
      c_Retval = "com";
      break;
   case C_OSCNodeDataPool::eNVM:
      c_Retval = "nvm";
      break;
   case C_OSCNodeDataPool::eHALC:
      c_Retval = "halc";
      break;
   default:
      c_Retval = "invalid";
      break;
   }
   return c_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Transform string to data pool type

   \param[in]  orc_String String to interpret
   \param[out] ore_Type   Data pool type

   \return
   C_NO_ERR   no error
   C_RANGE    String unknown
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_OSCNodeDataPoolFiler::h_StringToDataPool(const C_SCLString & orc_String, C_OSCNodeDataPool::E_Type & ore_Type)
{
   sint32 s32_Retval = C_NO_ERR;

   if (orc_String == "com")
   {
      ore_Type = C_OSCNodeDataPool::eCOM;
   }
   else if (orc_String == "nvm")
   {
      ore_Type = C_OSCNodeDataPool::eNVM;
   }
   else if (orc_String == "diag")
   {
      ore_Type = C_OSCNodeDataPool::eDIAG;
   }
   else if (orc_String == "halc")
   {
      ore_Type = C_OSCNodeDataPool::eHALC;
   }
   else
   {
      osc_write_log_error("Loading Datapool", "Invalid Datapool type:" + orc_String);
      s32_Retval = C_RANGE;
   }

   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Load data pool element type

   Load element type from the node "type"
   pre-condition: the passed XML parser has the active node set to the parent node of the "type" node

   The function does not change the active node.

   \param[out]    orc_NodeDataPoolContent data storage
   \param[in,out] orc_XMLParser           XML with unknown (Node to store data pool variable) active

   \return
   C_NO_ERR   data read
   C_CONFIG   content of file is invalid or incomplete
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_OSCNodeDataPoolFiler::h_LoadDataPoolElementType(C_OSCNodeDataPoolContent & orc_NodeDataPoolContent,
                                                         C_OSCXMLParserBase & orc_XMLParser)
{
   sint32 s32_Retval = C_CONFIG;

   if (orc_XMLParser.SelectNodeChild("type") == "type")
   {
      C_OSCNodeDataPoolContent::E_Type e_Type;
      s32_Retval = mh_StringToNodeDataPoolContent(orc_XMLParser.GetAttributeString("base-type"), e_Type);
      if (s32_Retval == C_NO_ERR)
      {
         orc_NodeDataPoolContent.SetType(e_Type);
         orc_NodeDataPoolContent.SetArray(orc_XMLParser.GetAttributeBool("is-array"));
         if (orc_NodeDataPoolContent.GetArray() == true)
         {
            orc_NodeDataPoolContent.SetArraySize(orc_XMLParser.GetAttributeUint32("array-size"));
         }
      }
      //Return
      orc_XMLParser.SelectNodeParent();
   }
   else
   {
      osc_write_log_error("Loading Datapool element", "Could not find \"type\" node.");
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Save data pool element type

   Save data pool element type,isarray,arraysize to XML parser
   Will create a node and write the value there.
   Does not modify the active node.

   \param[in]     orc_NodeDataPoolContent data storage
   \param[in,out] orc_XMLParser           XML parser
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OSCNodeDataPoolFiler::h_SaveDataPoolElementType(const C_OSCNodeDataPoolContent & orc_NodeDataPoolContent,
                                                       C_OSCXMLParserBase & orc_XMLParser)
{
   orc_XMLParser.CreateAndSelectNodeChild("type");
   orc_XMLParser.SetAttributeString("base-type", mh_NodeDataPoolContentToString(orc_NodeDataPoolContent.GetType()));
   orc_XMLParser.SetAttributeBool("is-array", orc_NodeDataPoolContent.GetArray());
   if (orc_NodeDataPoolContent.GetArray() == true)
   {
      orc_XMLParser.SetAttributeUint32("array-size", orc_NodeDataPoolContent.GetArraySize());
   }

   orc_XMLParser.SelectNodeParent();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Load data pool element value

   Load node data from XML file
   pre-conditions:
   * the passed XML parser has the active node set to the node containing the value
   * the element type of orc_NodeDataPoolContent must match the data contained in the file
   The function does not change the active node.

   \param[out]    orc_NodeDataPoolContent data storage
   \param[in,out] orc_XMLParser           XML with unknown (Node to store data pool variable) active

   \return
   C_NO_ERR   data read
   C_CONFIG   content of file is invalid or incomplete
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_OSCNodeDataPoolFiler::h_LoadDataPoolElementValue(C_OSCNodeDataPoolContent & orc_NodeDataPoolContent,
                                                          C_OSCXMLParserBase & orc_XMLParser)
{
   sint32 s32_Retval = C_NO_ERR;

   if (orc_NodeDataPoolContent.GetArray() == false)
   {
      //Single
      switch (orc_NodeDataPoolContent.GetType())
      {
      case C_OSCNodeDataPoolContent::eUINT8:
         orc_NodeDataPoolContent.SetValueU8(static_cast<uint8>(orc_XMLParser.GetAttributeUint32("value")));
         break;
      case C_OSCNodeDataPoolContent::eUINT16:
         orc_NodeDataPoolContent.SetValueU16(static_cast<uint16>(orc_XMLParser.GetAttributeUint32("value")));
         break;
      case C_OSCNodeDataPoolContent::eUINT32:
         orc_NodeDataPoolContent.SetValueU32(orc_XMLParser.GetAttributeUint32("value"));
         break;
      case C_OSCNodeDataPoolContent::eUINT64:
         orc_NodeDataPoolContent.SetValueU64(mh_GetAttributeUint64(orc_XMLParser, "value"));
         break;
      case C_OSCNodeDataPoolContent::eSINT8:
         orc_NodeDataPoolContent.SetValueS8(static_cast<sint8>(orc_XMLParser.GetAttributeSint64("value")));
         break;
      case C_OSCNodeDataPoolContent::eSINT16:
         orc_NodeDataPoolContent.SetValueS16(static_cast<sint16>(orc_XMLParser.GetAttributeSint64("value")));
         break;
      case C_OSCNodeDataPoolContent::eSINT32:
         orc_NodeDataPoolContent.SetValueS32(static_cast<sint32>(orc_XMLParser.GetAttributeSint64("value")));
         break;
      case C_OSCNodeDataPoolContent::eSINT64:
         orc_NodeDataPoolContent.SetValueS64(orc_XMLParser.GetAttributeSint64("value"));
         break;
      case C_OSCNodeDataPoolContent::eFLOAT32:
         orc_NodeDataPoolContent.SetValueF32(orc_XMLParser.GetAttributeFloat32("value"));
         break;
      case C_OSCNodeDataPoolContent::eFLOAT64:
         orc_NodeDataPoolContent.SetValueF64(orc_XMLParser.GetAttributeFloat64("value"));
         break;
      }
   }
   else
   {
      //Array
      C_SCLString c_CurNode = orc_XMLParser.SelectNodeChild("element");
      if (c_CurNode == "element")
      {
         uint32 u32_CurIndex = 0U;
         do
         {
            if (u32_CurIndex > orc_NodeDataPoolContent.GetArraySize())
            {
               break; //too much information ...
            }

            switch (orc_NodeDataPoolContent.GetType())
            {
            case C_OSCNodeDataPoolContent::eUINT8:
               orc_NodeDataPoolContent.SetValueAU8Element(static_cast<uint8>(orc_XMLParser.GetAttributeUint32(
                                                                                "value")), u32_CurIndex);
               break;
            case C_OSCNodeDataPoolContent::eUINT16:
               orc_NodeDataPoolContent.SetValueAU16Element(static_cast<uint16>(orc_XMLParser.GetAttributeUint32(
                                                                                  "value")), u32_CurIndex);
               break;
            case C_OSCNodeDataPoolContent::eUINT32:
               orc_NodeDataPoolContent.SetValueAU32Element(orc_XMLParser.GetAttributeUint32("value"),
                                                           u32_CurIndex);
               break;
            case C_OSCNodeDataPoolContent::eUINT64:
               orc_NodeDataPoolContent.SetValueAU64Element(mh_GetAttributeUint64(orc_XMLParser,
                                                                                 "value"), u32_CurIndex);
               break;
            case C_OSCNodeDataPoolContent::eSINT8:
               orc_NodeDataPoolContent.SetValueAS8Element(static_cast<sint8>(orc_XMLParser.GetAttributeSint64(
                                                                                "value")), u32_CurIndex);
               break;
            case C_OSCNodeDataPoolContent::eSINT16:
               orc_NodeDataPoolContent.SetValueAS16Element(static_cast<sint16>(orc_XMLParser.GetAttributeSint64(
                                                                                  "value")), u32_CurIndex);
               break;
            case C_OSCNodeDataPoolContent::eSINT32:
               orc_NodeDataPoolContent.SetValueAS32Element(static_cast<sint32>(orc_XMLParser.GetAttributeSint64(
                                                                                  "value")), u32_CurIndex);
               break;
            case C_OSCNodeDataPoolContent::eSINT64:
               orc_NodeDataPoolContent.SetValueAS64Element(orc_XMLParser.GetAttributeSint64("value"),
                                                           u32_CurIndex);
               break;
            case C_OSCNodeDataPoolContent::eFLOAT32:
               orc_NodeDataPoolContent.SetValueAF32Element(orc_XMLParser.GetAttributeFloat32("value"),
                                                           u32_CurIndex);
               break;
            case C_OSCNodeDataPoolContent::eFLOAT64:
               orc_NodeDataPoolContent.SetValueAF64Element(orc_XMLParser.GetAttributeFloat64("value"),
                                                           u32_CurIndex);
               break;
            }
            u32_CurIndex++; //next element
            c_CurNode = orc_XMLParser.SelectNodeNext("element");
         }
         while (c_CurNode == "element");

         //check whether we have the correct number of elements:
         if (u32_CurIndex != orc_NodeDataPoolContent.GetArraySize())
         {
            osc_write_log_error("Loading Datapool", "Incorrect size of value for array value.");
            s32_Retval = C_CONFIG;
         }
      }
      //Return
      orc_XMLParser.SelectNodeParent();
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Save data pool element value

   Save data pool element value to XML parser
   Will create a node and write the value there.
   Does not modify the active node.

   \param[in]     orc_NodeName            name of node to create value in
   \param[in]     orc_NodeDataPoolContent data storage
   \param[in,out] orc_XMLParser           XML parser
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OSCNodeDataPoolFiler::h_SaveDataPoolElementValue(const stw_scl::C_SCLString & orc_NodeName,
                                                        const C_OSCNodeDataPoolContent & orc_NodeDataPoolContent,
                                                        C_OSCXMLParserBase & orc_XMLParser)
{
   orc_XMLParser.CreateAndSelectNodeChild(orc_NodeName);
   if (orc_NodeDataPoolContent.GetArray() == false)
   {
      //Single
      switch (orc_NodeDataPoolContent.GetType())
      {
      case C_OSCNodeDataPoolContent::eUINT8:
         orc_XMLParser.SetAttributeUint32("value", static_cast<uint32>(orc_NodeDataPoolContent.GetValueU8()));
         break;
      case C_OSCNodeDataPoolContent::eUINT16:
         orc_XMLParser.SetAttributeUint32("value", static_cast<uint32>(orc_NodeDataPoolContent.GetValueU16()));
         break;
      case C_OSCNodeDataPoolContent::eUINT32:
         orc_XMLParser.SetAttributeUint32("value", orc_NodeDataPoolContent.GetValueU32());
         break;
      case C_OSCNodeDataPoolContent::eUINT64:
         mh_SetAttributeUint64(orc_XMLParser, "value", orc_NodeDataPoolContent.GetValueU64());
         break;
      case C_OSCNodeDataPoolContent::eSINT8:
         orc_XMLParser.SetAttributeSint64("value", static_cast<sint64>(orc_NodeDataPoolContent.GetValueS8()));
         break;
      case C_OSCNodeDataPoolContent::eSINT16:
         orc_XMLParser.SetAttributeSint64("value", static_cast<sint64>(orc_NodeDataPoolContent.GetValueS16()));
         break;
      case C_OSCNodeDataPoolContent::eSINT32:
         orc_XMLParser.SetAttributeSint64("value", static_cast<sint64>(orc_NodeDataPoolContent.GetValueS32()));
         break;
      case C_OSCNodeDataPoolContent::eSINT64:
         orc_XMLParser.SetAttributeSint64("value", orc_NodeDataPoolContent.GetValueS64());
         break;
      case C_OSCNodeDataPoolContent::eFLOAT32:
         orc_XMLParser.SetAttributeFloat32("value", orc_NodeDataPoolContent.GetValueF32());
         break;
      case C_OSCNodeDataPoolContent::eFLOAT64:
         orc_XMLParser.SetAttributeFloat64("value", orc_NodeDataPoolContent.GetValueF64());
         break;
      }
   }
   else
   {
      //Array
      for (uint32 u32_ItElem = 0; u32_ItElem < orc_NodeDataPoolContent.GetArraySize(); ++u32_ItElem)
      {
         orc_XMLParser.CreateAndSelectNodeChild("element");
         switch (orc_NodeDataPoolContent.GetType())
         {
         case C_OSCNodeDataPoolContent::eUINT8:
            orc_XMLParser.SetAttributeUint32("value",
                                             static_cast<uint32>(orc_NodeDataPoolContent.GetValueAU8Element(u32_ItElem)));
            break;
         case C_OSCNodeDataPoolContent::eUINT16:
            orc_XMLParser.SetAttributeUint32("value",
                                             static_cast<uint32>(orc_NodeDataPoolContent.GetValueAU16Element(
                                                                    u32_ItElem)));
            break;
         case C_OSCNodeDataPoolContent::eUINT32:
            orc_XMLParser.SetAttributeUint32("value", orc_NodeDataPoolContent.GetValueAU32Element(u32_ItElem));
            break;
         case C_OSCNodeDataPoolContent::eUINT64:
            mh_SetAttributeUint64(orc_XMLParser, "value", orc_NodeDataPoolContent.GetValueAU64Element(u32_ItElem));
            break;
         case C_OSCNodeDataPoolContent::eSINT8:
            orc_XMLParser.SetAttributeSint64("value",
                                             static_cast<sint64>(orc_NodeDataPoolContent.GetValueAS8Element(u32_ItElem)));
            break;
         case C_OSCNodeDataPoolContent::eSINT16:
            orc_XMLParser.SetAttributeSint64("value",
                                             static_cast<sint64>(orc_NodeDataPoolContent.GetValueAS16Element(
                                                                    u32_ItElem)));
            break;
         case C_OSCNodeDataPoolContent::eSINT32:
            orc_XMLParser.SetAttributeSint64("value",
                                             static_cast<sint64>(orc_NodeDataPoolContent.GetValueAS32Element(
                                                                    u32_ItElem)));
            break;
         case C_OSCNodeDataPoolContent::eSINT64:
            orc_XMLParser.SetAttributeSint64("value", orc_NodeDataPoolContent.GetValueAS64Element(u32_ItElem));
            break;
         case C_OSCNodeDataPoolContent::eFLOAT32:
            orc_XMLParser.SetAttributeFloat32("value", orc_NodeDataPoolContent.GetValueAF32Element(u32_ItElem));
            break;
         case C_OSCNodeDataPoolContent::eFLOAT64:
            orc_XMLParser.SetAttributeFloat64("value", orc_NodeDataPoolContent.GetValueAF64Element(u32_ItElem));
            break;
         }
         //Return to parent
         tgl_assert(orc_XMLParser.SelectNodeParent() == orc_NodeName);
      }
   }
   //Return to parent
   orc_XMLParser.SelectNodeParent();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Save node data pool content

   Save node to XML file in V1 format.
   Not used by core. But may be of some use to applications.

   pre-condition: the passed XML parser has the active node set to variable (Node to store data pool content)
   post-condition: the passed XML parser has the active node set to the same variable (Node to store data pool content)

   \param[in]     orc_NodeDataPoolContent data storage
   \param[in,out] orc_XMLParser           XML with variable (Node to store data pool content) active
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OSCNodeDataPoolFiler::h_SaveDataPoolContentV1(const C_OSCNodeDataPoolContent & orc_NodeDataPoolContent,
                                                     C_OSCXMLParserBase & orc_XMLParser)
{
   orc_XMLParser.SetAttributeBool("array", orc_NodeDataPoolContent.GetArray());
   orc_XMLParser.CreateNodeChild("type", mh_NodeDataPoolContentToString(orc_NodeDataPoolContent.GetType()));
   if (orc_NodeDataPoolContent.GetArray() == false)
   {
      //Single
      switch (orc_NodeDataPoolContent.GetType())
      {
      case C_OSCNodeDataPoolContent::eUINT8:
         orc_XMLParser.SetAttributeUint32("value", static_cast<uint32>(orc_NodeDataPoolContent.GetValueU8()));
         break;
      case C_OSCNodeDataPoolContent::eUINT16:
         orc_XMLParser.SetAttributeUint32("value", static_cast<uint32>(orc_NodeDataPoolContent.GetValueU16()));
         break;
      case C_OSCNodeDataPoolContent::eUINT32:
         orc_XMLParser.SetAttributeUint32("value", orc_NodeDataPoolContent.GetValueU32());
         break;
      case C_OSCNodeDataPoolContent::eUINT64:
         mh_SetAttributeUint64(orc_XMLParser, "value", orc_NodeDataPoolContent.GetValueU64());
         break;
      case C_OSCNodeDataPoolContent::eSINT8:
         orc_XMLParser.SetAttributeSint64("value", static_cast<sint64>(orc_NodeDataPoolContent.GetValueS8()));
         break;
      case C_OSCNodeDataPoolContent::eSINT16:
         orc_XMLParser.SetAttributeSint64("value", static_cast<sint64>(orc_NodeDataPoolContent.GetValueS16()));
         break;
      case C_OSCNodeDataPoolContent::eSINT32:
         orc_XMLParser.SetAttributeSint64("value", static_cast<sint64>(orc_NodeDataPoolContent.GetValueS32()));
         break;
      case C_OSCNodeDataPoolContent::eSINT64:
         orc_XMLParser.SetAttributeSint64("value", orc_NodeDataPoolContent.GetValueS64());
         break;
      case C_OSCNodeDataPoolContent::eFLOAT32:
         orc_XMLParser.SetAttributeFloat32("value", orc_NodeDataPoolContent.GetValueF32());
         break;
      case C_OSCNodeDataPoolContent::eFLOAT64:
         orc_XMLParser.SetAttributeFloat64("value", orc_NodeDataPoolContent.GetValueF64());
         break;
      }
   }
   else
   {
      //Array
      orc_XMLParser.CreateAndSelectNodeChild("array");
      for (uint32 u32_ItElem = 0; u32_ItElem < orc_NodeDataPoolContent.GetArraySize(); ++u32_ItElem)
      {
         orc_XMLParser.CreateAndSelectNodeChild("element");
         orc_XMLParser.SetAttributeUint32("index", u32_ItElem);
         switch (orc_NodeDataPoolContent.GetType())
         {
         case C_OSCNodeDataPoolContent::eUINT8:
            orc_XMLParser.SetAttributeUint32("content",
                                             static_cast<uint32>(orc_NodeDataPoolContent.GetValueAU8Element(u32_ItElem)));
            break;
         case C_OSCNodeDataPoolContent::eUINT16:
            orc_XMLParser.SetAttributeUint32("content",
                                             static_cast<uint32>(orc_NodeDataPoolContent.GetValueAU16Element(
                                                                    u32_ItElem)));
            break;
         case C_OSCNodeDataPoolContent::eUINT32:
            orc_XMLParser.SetAttributeUint32("content", orc_NodeDataPoolContent.GetValueAU32Element(u32_ItElem));
            break;
         case C_OSCNodeDataPoolContent::eUINT64:
            mh_SetAttributeUint64(orc_XMLParser, "content", orc_NodeDataPoolContent.GetValueAU64Element(u32_ItElem));
            break;
         case C_OSCNodeDataPoolContent::eSINT8:
            orc_XMLParser.SetAttributeSint64("content",
                                             static_cast<sint64>(orc_NodeDataPoolContent.GetValueAS8Element(u32_ItElem)));
            break;
         case C_OSCNodeDataPoolContent::eSINT16:
            orc_XMLParser.SetAttributeSint64("content",
                                             static_cast<sint64>(orc_NodeDataPoolContent.GetValueAS16Element(
                                                                    u32_ItElem)));
            break;
         case C_OSCNodeDataPoolContent::eSINT32:
            orc_XMLParser.SetAttributeSint64("content",
                                             static_cast<sint64>(orc_NodeDataPoolContent.GetValueAS32Element(
                                                                    u32_ItElem)));
            break;
         case C_OSCNodeDataPoolContent::eSINT64:
            orc_XMLParser.SetAttributeSint64("content", orc_NodeDataPoolContent.GetValueAS64Element(u32_ItElem));
            break;
         case C_OSCNodeDataPoolContent::eFLOAT32:
            orc_XMLParser.SetAttributeFloat32("content", orc_NodeDataPoolContent.GetValueAF32Element(u32_ItElem));
            break;
         case C_OSCNodeDataPoolContent::eFLOAT64:
            orc_XMLParser.SetAttributeFloat64("content", orc_NodeDataPoolContent.GetValueAF64Element(u32_ItElem));
            break;
         }
         //Return
         tgl_assert(orc_XMLParser.SelectNodeParent() == "array");
      }
      //Return
      orc_XMLParser.SelectNodeParent();
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get automatically generated file name

   \param[in] orc_DatapoolName Datapool name

   \return
   Automatically generated file name
*/
//----------------------------------------------------------------------------------------------------------------------
C_SCLString C_OSCNodeDataPoolFiler::h_GetFileName(const C_SCLString & orc_DatapoolName)
{
   return "dp_" + C_OSCSystemFilerUtil::mh_PrepareItemNameForFileName(orc_DatapoolName) + "_core.xml";
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Load node data pool content

   Load node data from XML file in V1 format.
   pre-condition: the passed XML parser has the active node set to unknown (Node to store data pool variable)
   post-condition: the passed XML parser has the active node set to the same unknown (Node to store data pool variable)

   \param[out]    orc_NodeDataPoolContent data storage
   \param[in,out] orc_XMLParser           XML with unknown (Node to store data pool variable) active

   \return
   C_NO_ERR   data read
   C_CONFIG   content of file is invalid or incomplete
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_OSCNodeDataPoolFiler::h_LoadDataPoolContentV1(C_OSCNodeDataPoolContent & orc_NodeDataPoolContent,
                                                       C_OSCXMLParserBase & orc_XMLParser)
{
   sint32 s32_Retval = C_NO_ERR;

   if (orc_XMLParser.SelectNodeChild("type") == "type")
   {
      C_OSCNodeDataPoolContent::E_Type e_Type;
      s32_Retval = mh_StringToNodeDataPoolContent(orc_XMLParser.GetNodeContent(), e_Type);
      orc_NodeDataPoolContent.SetType(e_Type);
      //Return
      orc_XMLParser.SelectNodeParent();
   }
   else
   {
      s32_Retval = C_CONFIG;
   }
   orc_NodeDataPoolContent.SetArray(orc_XMLParser.GetAttributeBool("array"));

   if (orc_NodeDataPoolContent.GetArray() == false)
   {
      //same format as in newer file version:
      h_LoadDataPoolElementValue(orc_NodeDataPoolContent, orc_XMLParser);
   }
   else
   {
      //Array
      if (orc_XMLParser.SelectNodeChild("array") == "array")
      {
         C_SCLString c_CurNode = orc_XMLParser.SelectNodeChild("element");
         if (c_CurNode == "element")
         {
            uint32 u32_CurIndex;
            do
            {
               u32_CurIndex = orc_XMLParser.GetAttributeUint32("index");
               if (u32_CurIndex >= orc_NodeDataPoolContent.GetArraySize())
               {
                  orc_NodeDataPoolContent.SetArraySize(u32_CurIndex + 1);
               }
               switch (orc_NodeDataPoolContent.GetType())
               {
               case C_OSCNodeDataPoolContent::eUINT8:
                  orc_NodeDataPoolContent.SetValueAU8Element(static_cast<uint8>(orc_XMLParser.GetAttributeUint32(
                                                                                   "content")), u32_CurIndex);
                  break;
               case C_OSCNodeDataPoolContent::eUINT16:
                  orc_NodeDataPoolContent.SetValueAU16Element(static_cast<uint16>(orc_XMLParser.GetAttributeUint32(
                                                                                     "content")), u32_CurIndex);
                  break;
               case C_OSCNodeDataPoolContent::eUINT32:
                  orc_NodeDataPoolContent.SetValueAU32Element(orc_XMLParser.GetAttributeUint32("content"),
                                                              u32_CurIndex);
                  break;
               case C_OSCNodeDataPoolContent::eUINT64:
                  orc_NodeDataPoolContent.SetValueAU64Element(mh_GetAttributeUint64(orc_XMLParser,
                                                                                    "content"), u32_CurIndex);
                  break;
               case C_OSCNodeDataPoolContent::eSINT8:
                  orc_NodeDataPoolContent.SetValueAS8Element(static_cast<sint8>(orc_XMLParser.GetAttributeSint64(
                                                                                   "content")), u32_CurIndex);
                  break;
               case C_OSCNodeDataPoolContent::eSINT16:
                  orc_NodeDataPoolContent.SetValueAS16Element(static_cast<sint16>(orc_XMLParser.GetAttributeSint64(
                                                                                     "content")), u32_CurIndex);
                  break;
               case C_OSCNodeDataPoolContent::eSINT32:
                  orc_NodeDataPoolContent.SetValueAS32Element(static_cast<sint32>(orc_XMLParser.GetAttributeSint64(
                                                                                     "content")), u32_CurIndex);
                  break;
               case C_OSCNodeDataPoolContent::eSINT64:
                  orc_NodeDataPoolContent.SetValueAS64Element(orc_XMLParser.GetAttributeSint64("content"),
                                                              u32_CurIndex);
                  break;
               case C_OSCNodeDataPoolContent::eFLOAT32:
                  orc_NodeDataPoolContent.SetValueAF32Element(orc_XMLParser.GetAttributeFloat32("content"),
                                                              u32_CurIndex);
                  break;
               case C_OSCNodeDataPoolContent::eFLOAT64:
                  orc_NodeDataPoolContent.SetValueAF64Element(orc_XMLParser.GetAttributeFloat64("content"),
                                                              u32_CurIndex);
                  break;
               }
               c_CurNode = orc_XMLParser.SelectNodeNext("element");
            }
            while (c_CurNode == "element");
            //Return
            tgl_assert(orc_XMLParser.SelectNodeParent() == "array");
         }
         //Return
         orc_XMLParser.SelectNodeParent();
      }
      else
      {
         osc_write_log_error("Loading data element", "Could not find \"array\" node.");
         s32_Retval = C_CONFIG;
      }
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Save datapool

   Save node to XML file

   \param[in]     orc_NodeDataPool data storage
   \param[in,out] orc_FilePath     File path for xml

   \return
   C_NO_ERR   data saved
   C_CONFIG   file could not be created
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_OSCNodeDataPoolFiler::h_SaveDataPoolFile(const C_OSCNodeDataPool & orc_NodeDataPool,
                                                  const C_SCLString & orc_FilePath)
{
   C_OSCXMLParser c_XMLParser;
   sint32 s32_Retval = C_OSCSystemFilerUtil::mh_GetParserForNewFile(c_XMLParser, orc_FilePath,
                                                                    "opensyde-dp-core-definition");

   if (s32_Retval == C_NO_ERR)
   {
      //Version
      c_XMLParser.CreateNodeChild("file-version", "1");
      tgl_assert(c_XMLParser.CreateAndSelectNodeChild("data-pool") == "data-pool");
      //node
      C_OSCNodeDataPoolFiler::h_SaveDataPool(orc_NodeDataPool, c_XMLParser);
      //Don't forget to save!
      if (c_XMLParser.SaveToFile(orc_FilePath) != C_NO_ERR)
      {
         osc_write_log_error("Saving node definition", "Could not create file for node.");
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
/*! \brief  Transform node data pool content type to string

   \param[in] ore_NodeDataPoolContent Node data pool content type

   \return
   Stringified node data pool content type
*/
//----------------------------------------------------------------------------------------------------------------------
C_SCLString C_OSCNodeDataPoolFiler::mh_NodeDataPoolContentToString(
   const C_OSCNodeDataPoolContent::E_Type & ore_NodeDataPoolContent)
{
   C_SCLString c_Retval;

   switch (ore_NodeDataPoolContent)
   {
   case C_OSCNodeDataPoolContent::eUINT8:
      c_Retval = "uint8";
      break;
   case C_OSCNodeDataPoolContent::eUINT16:
      c_Retval = "uint16";
      break;
   case C_OSCNodeDataPoolContent::eUINT32:
      c_Retval = "uint32";
      break;
   case C_OSCNodeDataPoolContent::eUINT64:
      c_Retval = "uint64";
      break;
   case C_OSCNodeDataPoolContent::eSINT8:
      c_Retval = "sint8";
      break;
   case C_OSCNodeDataPoolContent::eSINT16:
      c_Retval = "sint16";
      break;
   case C_OSCNodeDataPoolContent::eSINT32:
      c_Retval = "sint32";
      break;
   case C_OSCNodeDataPoolContent::eSINT64:
      c_Retval = "sint64";
      break;
   case C_OSCNodeDataPoolContent::eFLOAT32:
      c_Retval = "float32";
      break;
   case C_OSCNodeDataPoolContent::eFLOAT64:
      c_Retval = "float64";
      break;
   default:
      c_Retval = "invalid";
      break;
   }
   return c_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Transform string to node data pool content type

   \param[in]  orc_String String to interpret
   \param[out] ore_Type   Node data pool content type

   \return
   C_NO_ERR   no error
   C_RANGE    String unknown
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_OSCNodeDataPoolFiler::mh_StringToNodeDataPoolContent(const C_SCLString & orc_String,
                                                              C_OSCNodeDataPoolContent::E_Type & ore_Type)
{
   sint32 s32_Retval = C_NO_ERR;

   if (orc_String == "uint8")
   {
      ore_Type = C_OSCNodeDataPoolContent::eUINT8;
   }
   else if (orc_String == "uint16")
   {
      ore_Type = C_OSCNodeDataPoolContent::eUINT16;
   }
   else if (orc_String == "uint32")
   {
      ore_Type = C_OSCNodeDataPoolContent::eUINT32;
   }
   else if (orc_String == "uint64")
   {
      ore_Type = C_OSCNodeDataPoolContent::eUINT64;
   }
   else if (orc_String == "sint8")
   {
      ore_Type = C_OSCNodeDataPoolContent::eSINT8;
   }
   else if (orc_String == "sint16")
   {
      ore_Type = C_OSCNodeDataPoolContent::eSINT16;
   }
   else if (orc_String == "sint32")
   {
      ore_Type = C_OSCNodeDataPoolContent::eSINT32;
   }
   else if (orc_String == "sint64")
   {
      ore_Type = C_OSCNodeDataPoolContent::eSINT64;
   }
   else if (orc_String == "float32")
   {
      ore_Type = C_OSCNodeDataPoolContent::eFLOAT32;
   }
   else if (orc_String == "float64")
   {
      ore_Type = C_OSCNodeDataPoolContent::eFLOAT64;
   }
   else
   {
      osc_write_log_error("Loading data element", "Invalid \"type\": " + orc_String);
      s32_Retval = C_RANGE;
   }

   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Transform node data pool element access type to string

   \param[in] ore_NodeDataPoolElementAccess Node data pool element access type

   \return
   Stringified node data pool element access type
*/
//----------------------------------------------------------------------------------------------------------------------
C_SCLString C_OSCNodeDataPoolFiler::mh_NodeDataPoolElementAccessToString(
   const C_OSCNodeDataPoolListElement::E_Access & ore_NodeDataPoolElementAccess)
{
   C_SCLString c_Retval;

   switch (ore_NodeDataPoolElementAccess)
   {
   case C_OSCNodeDataPoolListElement::eACCESS_RO:
      c_Retval = "read-only";
      break;
   case C_OSCNodeDataPoolListElement::eACCESS_RW:
      c_Retval = "read-write";
      break;
   default:
      c_Retval = "invalid";
      break;
   }
   return c_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Transform string to node data pool element access type

   \param[in]  orc_String String to interpret
   \param[out] ore_Type   Node data pool element access type

   \return
   C_NO_ERR   no error
   C_RANGE    String unknown
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_OSCNodeDataPoolFiler::mh_StringToNodeDataPoolElementAccess(const C_SCLString & orc_String,
                                                                    C_OSCNodeDataPoolListElement::E_Access & ore_Type)
{
   sint32 s32_Retval = C_NO_ERR;

   if (orc_String == "read-write")
   {
      ore_Type = C_OSCNodeDataPoolListElement::eACCESS_RW;
   }
   else if (orc_String == "read-only")
   {
      ore_Type = C_OSCNodeDataPoolListElement::eACCESS_RO;
   }
   else
   {
      osc_write_log_error("Loading data element", "Invalid \"access\": " + orc_String);

      s32_Retval = C_RANGE;
   }

   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Set unit64 attribute

   \param[in,out] orc_XMLParser XML
   \param[in]     orc_String    Attribute
   \param[in]     ou64_Input    Value
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OSCNodeDataPoolFiler::mh_SetAttributeUint64(C_OSCXMLParserBase & orc_XMLParser, const C_SCLString & orc_String,
                                                   const uint64 ou64_Input)
{
   std::stringstream c_Stream;
   c_Stream << ou64_Input;

   orc_XMLParser.SetAttributeString(orc_String, std::string(c_Stream.str()).c_str());
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get unit64 attribute

   \param[in] orc_XMLParser XML
   \param[in] orc_String    Attribute

   \return
   Value
*/
//----------------------------------------------------------------------------------------------------------------------
uint64 C_OSCNodeDataPoolFiler::mh_GetAttributeUint64(const C_OSCXMLParserBase & orc_XMLParser,
                                                     const C_SCLString & orc_String)
{
   uint64 u64_Retval = 0;

   std::stringstream c_Stream;
   c_Stream << orc_XMLParser.GetAttributeString(orc_String).c_str();

   c_Stream >> u64_Retval;
   return u64_Retval;
}
