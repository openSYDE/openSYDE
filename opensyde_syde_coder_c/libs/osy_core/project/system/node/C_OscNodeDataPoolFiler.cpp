//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Node data pool reader/writer (V3) (implementation)

   Node data pool reader/writer (V3)

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.hpp"

#include <limits>
#include <sstream>
#include "stwtypes.hpp"
#include "stwerrors.hpp"
#include "C_OscSystemFilerUtil.hpp"
#include "C_OscNodeDataPoolFiler.hpp"
#include "TglUtils.hpp"
#include "C_OscLoggingHandler.hpp"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */

using namespace stw::opensyde_core;
using namespace stw::errors;
using namespace stw::scl;
using namespace stw::tgl;

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
C_OscNodeDataPoolFiler::C_OscNodeDataPoolFiler(void)
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Load datapool file

   \param[out]  orc_NodeDataPool    Data storage
   \param[in]   orc_FilePath        File path

   \return
   C_NO_ERR   data read
   C_CONFIG   content of file is invalid or incomplete
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_OscNodeDataPoolFiler::h_LoadDataPoolFile(C_OscNodeDataPool & orc_NodeDataPool,
                                                   const C_SclString & orc_FilePath)
{
   C_OscXmlParser c_XmlParser;
   int32_t s32_Retval = C_OscSystemFilerUtil::h_GetParserForExistingFile(c_XmlParser, orc_FilePath,
                                                                         "opensyde-dp-core-definition");

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
         osc_write_log_error("Loading Datapool", "\"file-version\" could not be converted to a number.");
         s32_Retval = C_CONFIG;
      }

      //is the file version one we know ?
      if (s32_Retval == C_NO_ERR)
      {
         osc_write_log_info("Loading Datapool", "Value of \"file-version\": " +
                            C_SclString::IntToStr(u16_FileVersion));
         //Check file version
         if (u16_FileVersion != 1U)
         {
            osc_write_log_error("Loading Datapool",
                                "Version defined by \"file-version\" is not supported.");
            s32_Retval = C_CONFIG;
         }
      }

      //Return
      c_XmlParser.SelectNodeParent();
   }
   else
   {
      osc_write_log_error("Loading Datapool", "Could not find \"file-version\" node.");
      s32_Retval = C_CONFIG;
   }
   if (s32_Retval == C_NO_ERR)
   {
      if (c_XmlParser.SelectNodeChild("data-pool") == "data-pool")
      {
         s32_Retval = C_OscNodeDataPoolFiler::h_LoadDataPool(orc_NodeDataPool, c_XmlParser);
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

   \param[out]     orc_NodeDataPool    data storage
   \param[in,out]  orc_XmlParser       XML with data-pool active

   \return
   C_NO_ERR   data read
   C_CONFIG   content of file is invalid or incomplete
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_OscNodeDataPoolFiler::h_LoadDataPool(C_OscNodeDataPool & orc_NodeDataPool, C_OscXmlParserBase & orc_XmlParser)
{
   int32_t s32_Retval = C_NO_ERR;

   //Related application
   if (orc_XmlParser.AttributeExists("related-application-index") == true)
   {
      orc_NodeDataPool.s32_RelatedDataBlockIndex = orc_XmlParser.GetAttributeSint32("related-application-index");
   }
   else
   {
      orc_NodeDataPool.s32_RelatedDataBlockIndex = -1;
   }
   orc_NodeDataPool.q_IsSafety = orc_XmlParser.GetAttributeBool("is-safety");
   if (orc_XmlParser.AttributeExists("scope-is-private") == true)
   {
      orc_NodeDataPool.q_ScopeIsPrivate = orc_XmlParser.GetAttributeBool("scope-is-private");
   }
   else
   {
      orc_NodeDataPool.q_ScopeIsPrivate = true;
   }
   if (orc_XmlParser.AttributeExists("definition-crc-version"))
   {
      orc_NodeDataPool.u16_DefinitionCrcVersion =
         static_cast<uint16_t>(orc_XmlParser.GetAttributeUint32("definition-crc-version"));
   }
   else
   {
      // probably deprecated project -> default to first version
      orc_NodeDataPool.u16_DefinitionCrcVersion = 1U;
   }
   orc_NodeDataPool.u32_NvmStartAddress = orc_XmlParser.GetAttributeUint32("nvm-start-address");
   orc_NodeDataPool.u32_NvmSize = orc_XmlParser.GetAttributeUint32("nvm-size");
   if (orc_XmlParser.SelectNodeChild("type") == "type")
   {
      s32_Retval = h_StringToDataPool(orc_XmlParser.GetNodeContent(), orc_NodeDataPool.e_Type);
      if (s32_Retval == C_NO_ERR)
      {
         //Return
         tgl_assert(orc_XmlParser.SelectNodeParent() == "data-pool");
      }
   }
   else
   {
      osc_write_log_error("Loading Datapool", "Could not find \"type\" node.");
      s32_Retval = C_CONFIG;
   }
   if (orc_XmlParser.SelectNodeChild("name") == "name")
   {
      orc_NodeDataPool.c_Name = orc_XmlParser.GetNodeContent();
      //Return
      tgl_assert(orc_XmlParser.SelectNodeParent() == "data-pool");
   }
   else
   {
      osc_write_log_error("Loading Datapool", "Could not find \"name\" node.");
      s32_Retval = C_CONFIG;
   }
   if (orc_XmlParser.SelectNodeChild("version") == "version")
   {
      orc_NodeDataPool.au8_Version[0] = static_cast<uint8_t>(orc_XmlParser.GetAttributeUint32("major"));
      orc_NodeDataPool.au8_Version[1] = static_cast<uint8_t>(orc_XmlParser.GetAttributeUint32("minor"));
      orc_NodeDataPool.au8_Version[2] = static_cast<uint8_t>(orc_XmlParser.GetAttributeUint32("release"));
      //Return
      tgl_assert(orc_XmlParser.SelectNodeParent() == "data-pool");
   }
   else
   {
      osc_write_log_error("Loading Datapool", "Could not find \"version\" node.");
      s32_Retval = C_CONFIG;
   }
   if (orc_XmlParser.SelectNodeChild("comment") == "comment")
   {
      orc_NodeDataPool.c_Comment = orc_XmlParser.GetNodeContent();
      //Return
      tgl_assert(orc_XmlParser.SelectNodeParent() == "data-pool");
   }
   else
   {
      osc_write_log_error("Loading Datapool", "Could not find \"comment\" node.");
      s32_Retval = C_CONFIG;
   }
   if ((orc_XmlParser.SelectNodeChild("lists") == "lists") && (s32_Retval == C_NO_ERR))
   {
      s32_Retval = h_LoadDataPoolLists(orc_NodeDataPool.c_Lists, orc_XmlParser);

      if (s32_Retval == C_NO_ERR)
      {
         //Return
         tgl_assert(orc_XmlParser.SelectNodeParent() == "data-pool");
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

   \param[in]      orc_NodeDataPool    data storage
   \param[in,out]  orc_XmlParser       XML with data-pool active
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OscNodeDataPoolFiler::h_SaveDataPool(const C_OscNodeDataPool & orc_NodeDataPool,
                                            C_OscXmlParserBase & orc_XmlParser)
{
   orc_XmlParser.SetAttributeSint32("related-application-index", orc_NodeDataPool.s32_RelatedDataBlockIndex);
   orc_XmlParser.SetAttributeBool("is-safety", orc_NodeDataPool.q_IsSafety);
   orc_XmlParser.SetAttributeBool("scope-is-private", orc_NodeDataPool.q_ScopeIsPrivate);
   orc_XmlParser.SetAttributeUint32("definition-crc-version", orc_NodeDataPool.u16_DefinitionCrcVersion);
   if ((orc_NodeDataPool.e_Type == C_OscNodeDataPool::eNVM) ||
       (orc_NodeDataPool.e_Type == C_OscNodeDataPool::eHALC_NVM))
   {
      orc_XmlParser.SetAttributeUint32("nvm-start-address", orc_NodeDataPool.u32_NvmStartAddress);
      orc_XmlParser.SetAttributeUint32("nvm-size", orc_NodeDataPool.u32_NvmSize);
   }
   orc_XmlParser.CreateNodeChild("type", h_DataPoolToString(orc_NodeDataPool.e_Type));
   orc_XmlParser.CreateNodeChild("name", orc_NodeDataPool.c_Name);
   orc_XmlParser.CreateAndSelectNodeChild("version");
   orc_XmlParser.SetAttributeUint32("major", orc_NodeDataPool.au8_Version[0]);
   orc_XmlParser.SetAttributeUint32("minor", orc_NodeDataPool.au8_Version[1]);
   orc_XmlParser.SetAttributeUint32("release", orc_NodeDataPool.au8_Version[2]);
   //Return
   tgl_assert(orc_XmlParser.SelectNodeParent() == "data-pool");

   orc_XmlParser.CreateNodeChild("comment", orc_NodeDataPool.c_Comment);
   //Lists
   orc_XmlParser.CreateAndSelectNodeChild("lists");
   h_SaveDataPoolLists(orc_NodeDataPool.c_Lists, orc_XmlParser, orc_NodeDataPool.e_Type);
   //Return
   tgl_assert(orc_XmlParser.SelectNodeParent() == "data-pool");
   orc_XmlParser.CreateNodeChild("export-settings", "");
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Load node data pool list

   Load node data from XML file
   pre-condition: the passed XML parser has the active node set to "list"
   post-condition: the passed XML parser has the active node set to the same "list"

   \param[out]     orc_NodeDataPoolList   data storage
   \param[in,out]  orc_XmlParser          XML with data-pool active

   \return
   C_NO_ERR   data read
   C_CONFIG   content of file is invalid or incomplete
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_OscNodeDataPoolFiler::h_LoadDataPoolList(C_OscNodeDataPoolList & orc_NodeDataPoolList,
                                                   C_OscXmlParserBase & orc_XmlParser)
{
   int32_t s32_Retval = C_NO_ERR;

   orc_NodeDataPoolList.q_NvmCrcActive = orc_XmlParser.GetAttributeBool("nvm-crc-active");
   orc_NodeDataPoolList.u32_NvmCrc = orc_XmlParser.GetAttributeUint32("nvm-crc");
   orc_NodeDataPoolList.u32_NvmStartAddress = orc_XmlParser.GetAttributeUint32("nvm-start-address");
   orc_NodeDataPoolList.u32_NvmSize = orc_XmlParser.GetAttributeUint32("nvm-size");
   if (orc_XmlParser.SelectNodeChild("name") == "name")
   {
      orc_NodeDataPoolList.c_Name = orc_XmlParser.GetNodeContent();
      //Return
      tgl_assert(orc_XmlParser.SelectNodeParent() == "list");
   }
   else
   {
      osc_write_log_error("Loading Datapool", "Could not find \"lists\".\"list\".\"name\" node.");
      s32_Retval = C_CONFIG;
   }
   if (orc_XmlParser.SelectNodeChild("comment") == "comment")
   {
      orc_NodeDataPoolList.c_Comment = orc_XmlParser.GetNodeContent();
      //Return
      tgl_assert(orc_XmlParser.SelectNodeParent() == "list");
   }
   else
   {
      osc_write_log_error("Loading Datapool", "Could not find \"lists\".\"list\".\"comment\" node.");
      s32_Retval = C_CONFIG;
   }
   //Data elements
   if (s32_Retval == C_NO_ERR)
   {
      if (orc_XmlParser.SelectNodeChild("data-elements") == "data-elements")
      {
         s32_Retval = h_LoadDataPoolListElements(orc_NodeDataPoolList.c_Elements, orc_XmlParser);
         if (s32_Retval == C_NO_ERR)
         {
            //Return
            tgl_assert(orc_XmlParser.SelectNodeParent() == "list");
         }
      }
      else
      {
         osc_write_log_error("Loading Datapool", "Could not find \"lists\".\"list\".\"data-elements\" node.");
         s32_Retval = C_CONFIG;
      }
   }
   //Data sets
   if ((orc_XmlParser.SelectNodeChild("data-sets") == "data-sets") && (s32_Retval == C_NO_ERR))
   {
      s32_Retval = h_LoadDataPoolListDataSets(orc_NodeDataPoolList.c_DataSets, orc_XmlParser);
      if (s32_Retval == C_NO_ERR)
      {
         //Return
         tgl_assert(orc_XmlParser.SelectNodeParent() == "list");
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

   \param[in]      orc_NodeDataPoolList   data storage
   \param[in,out]  orc_XmlParser          XML with data-pool active
   \param[in]      oe_DatapoolType        Datapool type
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OscNodeDataPoolFiler::h_SaveDataPoolList(const C_OscNodeDataPoolList & orc_NodeDataPoolList,
                                                C_OscXmlParserBase & orc_XmlParser,
                                                const C_OscNodeDataPool::E_Type oe_DatapoolType)
{
   if ((oe_DatapoolType == C_OscNodeDataPool::eNVM) || (oe_DatapoolType == C_OscNodeDataPool::eHALC_NVM))
   {
      orc_XmlParser.SetAttributeBool("nvm-crc-active", orc_NodeDataPoolList.q_NvmCrcActive);
      orc_XmlParser.SetAttributeUint32("nvm-crc", orc_NodeDataPoolList.u32_NvmCrc);
      orc_XmlParser.SetAttributeUint32("nvm-start-address", orc_NodeDataPoolList.u32_NvmStartAddress);
      orc_XmlParser.SetAttributeUint32("nvm-size", orc_NodeDataPoolList.u32_NvmSize);
   }
   orc_XmlParser.CreateNodeChild("name", orc_NodeDataPoolList.c_Name);
   orc_XmlParser.CreateNodeChild("comment", orc_NodeDataPoolList.c_Comment);
   //Data elements
   orc_XmlParser.CreateAndSelectNodeChild("data-elements");
   h_SaveDataPoolListElements(orc_NodeDataPoolList.c_Elements, orc_XmlParser, oe_DatapoolType);
   //Return
   tgl_assert(orc_XmlParser.SelectNodeParent() == "list");
   //Data sets
   orc_XmlParser.CreateAndSelectNodeChild("data-sets");
   h_SaveDataPoolListDataSets(orc_NodeDataPoolList.c_DataSets, orc_XmlParser);
   //Return
   tgl_assert(orc_XmlParser.SelectNodeParent() == "list");
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Load node data pool element

   Load node data from XML file
   pre-condition: the passed XML parser has the active node set to "data-element"
   post-condition: the passed XML parser has the active node set to the same "data-element"

   \param[out]     orc_NodeDataPoolListElement  data storage
   \param[in,out]  orc_XmlParser                XML with list active

   \return
   C_NO_ERR   data read
   C_CONFIG   content of file is invalid or incomplete
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_OscNodeDataPoolFiler::h_LoadDataPoolElement(C_OscNodeDataPoolListElement & orc_NodeDataPoolListElement,
                                                      C_OscXmlParserBase & orc_XmlParser)
{
   int32_t s32_Retval = C_NO_ERR;

   orc_NodeDataPoolListElement.f64_Factor = orc_XmlParser.GetAttributeFloat64("factor");
   orc_NodeDataPoolListElement.f64_Offset = orc_XmlParser.GetAttributeFloat64("offset");
   orc_NodeDataPoolListElement.q_DiagEventCall = orc_XmlParser.GetAttributeBool("diag-event-call");
   orc_NodeDataPoolListElement.u32_NvmStartAddress = orc_XmlParser.GetAttributeUint32("nvm-start-address");
   if (orc_XmlParser.SelectNodeChild("name") == "name")
   {
      orc_NodeDataPoolListElement.c_Name = orc_XmlParser.GetNodeContent();
      //Return
      tgl_assert(orc_XmlParser.SelectNodeParent() == "data-element");
   }
   else
   {
      osc_write_log_error("Loading data element", "Could not find \"name\" node.");
      s32_Retval = C_CONFIG;
   }
   if (s32_Retval == C_NO_ERR)
   {
      s32_Retval = h_LoadDataPoolElementType(orc_NodeDataPoolListElement.c_Value, orc_XmlParser);
   }
   if (s32_Retval == C_NO_ERR)
   {
      if (orc_XmlParser.SelectNodeChild("min-value") == "min-value")
      {
         //copy over value so we have the correct type:
         orc_NodeDataPoolListElement.c_MinValue = orc_NodeDataPoolListElement.c_Value;

         s32_Retval = h_LoadDataPoolElementValue(orc_NodeDataPoolListElement.c_MinValue, orc_XmlParser, false);
         // Use minimum value as init value for value and NVM value
         orc_NodeDataPoolListElement.c_NvmValue = orc_NodeDataPoolListElement.c_MinValue;
         orc_NodeDataPoolListElement.c_Value = orc_NodeDataPoolListElement.c_MinValue;
         //Return
         tgl_assert(orc_XmlParser.SelectNodeParent() == "data-element");
      }
      else
      {
         osc_write_log_error("Loading data element", "Could not find \"min-value\" node.");
         s32_Retval = C_CONFIG;
      }
   }

   if (s32_Retval == C_NO_ERR)
   {
      if (orc_XmlParser.SelectNodeChild("max-value") == "max-value")
      {
         //copy over value so we have the correct type:
         orc_NodeDataPoolListElement.c_MaxValue = orc_NodeDataPoolListElement.c_Value;

         s32_Retval = h_LoadDataPoolElementValue(orc_NodeDataPoolListElement.c_MaxValue, orc_XmlParser, false);
         //Return
         tgl_assert(orc_XmlParser.SelectNodeParent() == "data-element");
      }
      else
      {
         osc_write_log_error("Loading data element", "Could not find \"max-value\" node.");
         s32_Retval = C_CONFIG;
      }
   }

   if (s32_Retval == C_NO_ERR)
   {
      if (orc_XmlParser.SelectNodeChild("comment") == "comment")
      {
         orc_NodeDataPoolListElement.c_Comment = orc_XmlParser.GetNodeContent();
         //Return
         tgl_assert(orc_XmlParser.SelectNodeParent() == "data-element");
      }
      else
      {
         osc_write_log_error("Loading data element", "Could not find \"comment\" node.");
         s32_Retval = C_CONFIG;
      }
   }

   if (s32_Retval == C_NO_ERR)
   {
      if (orc_XmlParser.SelectNodeChild("unit") == "unit")
      {
         orc_NodeDataPoolListElement.c_Unit = orc_XmlParser.GetNodeContent();
         //Return
         tgl_assert(orc_XmlParser.SelectNodeParent() == "data-element");
      }
      else
      {
         osc_write_log_error("Loading data element", "Could not find \"unit\" node.");
         s32_Retval = C_CONFIG;
      }
   }

   if (s32_Retval == C_NO_ERR)
   {
      if (orc_XmlParser.SelectNodeChild("access") == "access")
      {
         s32_Retval =
            mh_StringToNodeDataPoolElementAccess(orc_XmlParser.GetNodeContent(), orc_NodeDataPoolListElement.e_Access);
         //Return
         tgl_assert(orc_XmlParser.SelectNodeParent() == "data-element");
      }
      else
      {
         osc_write_log_error("Loading data element", "Could not find \"access\" node.");
         s32_Retval = C_CONFIG;
      }
   }

   if (s32_Retval == C_NO_ERR)
   {
      if (orc_XmlParser.SelectNodeChild("data-set-values") == "data-set-values")
      {
         s32_Retval = h_LoadDataPoolListElementDataSetValues(orc_NodeDataPoolListElement.c_Value,
                                                             orc_NodeDataPoolListElement.c_DataSetValues,
                                                             orc_XmlParser);
         //Return
         tgl_assert(orc_XmlParser.SelectNodeParent() == "data-element");
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

   \param[in]      orc_NodeDataPoolListElement  data storage
   \param[in,out]  orc_XmlParser                XML with list active
   \param[in]      oe_DatapoolType              Datapool type
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OscNodeDataPoolFiler::h_SaveDataPoolElement(const C_OscNodeDataPoolListElement & orc_NodeDataPoolListElement,
                                                   C_OscXmlParserBase & orc_XmlParser,
                                                   const C_OscNodeDataPool::E_Type oe_DatapoolType)
{
   orc_XmlParser.SetAttributeFloat64("factor", orc_NodeDataPoolListElement.f64_Factor);
   orc_XmlParser.SetAttributeFloat64("offset", orc_NodeDataPoolListElement.f64_Offset);
   if (oe_DatapoolType == C_OscNodeDataPool::eDIAG)
   {
      orc_XmlParser.SetAttributeBool("diag-event-call", orc_NodeDataPoolListElement.q_DiagEventCall);
   }
   else if ((oe_DatapoolType == C_OscNodeDataPool::eNVM) || (oe_DatapoolType == C_OscNodeDataPool::eHALC_NVM))
   {
      orc_XmlParser.SetAttributeUint32("nvm-start-address", orc_NodeDataPoolListElement.u32_NvmStartAddress);
   }
   else
   {
      //No relevant variables in this section
   }
   orc_XmlParser.CreateNodeChild("name", orc_NodeDataPoolListElement.c_Name);
   h_SaveDataPoolElementType(orc_NodeDataPoolListElement.c_Value, orc_XmlParser);
   orc_XmlParser.CreateNodeChild("comment", orc_NodeDataPoolListElement.c_Comment);
   h_SaveDataPoolElementValue("min-value", orc_NodeDataPoolListElement.c_MinValue, orc_XmlParser);
   h_SaveDataPoolElementValue("max-value", orc_NodeDataPoolListElement.c_MaxValue, orc_XmlParser);
   orc_XmlParser.CreateNodeChild("unit", orc_NodeDataPoolListElement.c_Unit);
   orc_XmlParser.CreateNodeChild("access", mh_NodeDataPoolElementAccessToString(orc_NodeDataPoolListElement.e_Access));
   orc_XmlParser.CreateAndSelectNodeChild("data-set-values");
   h_SaveDataPoolListElementDataSetValues(orc_NodeDataPoolListElement.c_DataSetValues, orc_XmlParser);
   //Return to parent
   tgl_assert(orc_XmlParser.SelectNodeParent() == "data-element");
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Load node data pool lists

   Load node data from XML file
   pre-condition: the passed XML parser has the active node set to "lists"
   post-condition: the passed XML parser has the active node set to the same "lists"

   \param[out]     orc_NodeDataPoolLists  data storage
   \param[in,out]  orc_XmlParser          XML with data-pool active

   \return
   C_NO_ERR   data read
   C_CONFIG   content of file is invalid or incomplete
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_OscNodeDataPoolFiler::h_LoadDataPoolLists(std::vector<C_OscNodeDataPoolList> & orc_NodeDataPoolLists,
                                                    C_OscXmlParserBase & orc_XmlParser)
{
   int32_t s32_Retval = C_NO_ERR;
   C_SclString c_CurNodeList;
   uint32_t u32_ExpectedSize = 0UL;
   const bool q_ExpectedSizeHere = orc_XmlParser.AttributeExists("length");

   //Check optional length
   if (q_ExpectedSizeHere == true)
   {
      u32_ExpectedSize = orc_XmlParser.GetAttributeUint32("length");
      orc_NodeDataPoolLists.reserve(u32_ExpectedSize);
   }

   c_CurNodeList = orc_XmlParser.SelectNodeChild("list");

   //Clear
   orc_NodeDataPoolLists.clear();
   if (c_CurNodeList == "list")
   {
      do
      {
         C_OscNodeDataPoolList c_CurList;

         if (s32_Retval == C_NO_ERR)
         {
            s32_Retval = h_LoadDataPoolList(c_CurList, orc_XmlParser);
         }

         orc_NodeDataPoolLists.push_back(c_CurList);
         //Next
         c_CurNodeList = orc_XmlParser.SelectNodeNext("list");
      }
      while (c_CurNodeList == "list");

      if (s32_Retval == C_NO_ERR)
      {
         //Return
         tgl_assert(orc_XmlParser.SelectNodeParent() == "lists");
      }
   }
   //Compare length
   if ((s32_Retval == C_NO_ERR) && (q_ExpectedSizeHere == true))
   {
      if (u32_ExpectedSize != orc_NodeDataPoolLists.size())
      {
         C_SclString c_Tmp;
         c_Tmp.PrintFormatted("Unexpected list count, expected: %u, got %u", u32_ExpectedSize,
                              static_cast<uint32_t>(orc_NodeDataPoolLists.size()));
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

   \param[in]      orc_NodeDataPoolLists  data storage
   \param[in,out]  orc_XmlParser          XML with data-pool active
   \param[in]      oe_DatapoolType        Datapool type
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OscNodeDataPoolFiler::h_SaveDataPoolLists(const std::vector<C_OscNodeDataPoolList> & orc_NodeDataPoolLists,
                                                 C_OscXmlParserBase & orc_XmlParser,
                                                 const C_OscNodeDataPool::E_Type oe_DatapoolType)
{
   orc_XmlParser.SetAttributeUint32("length", static_cast<uint32_t>(orc_NodeDataPoolLists.size()));
   for (uint32_t u32_ItList = 0; u32_ItList < orc_NodeDataPoolLists.size(); ++u32_ItList)
   {
      orc_XmlParser.CreateAndSelectNodeChild("list");
      h_SaveDataPoolList(orc_NodeDataPoolLists[u32_ItList], orc_XmlParser, oe_DatapoolType);
      //Return
      tgl_assert(orc_XmlParser.SelectNodeParent() == "lists");
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Load node data pool elements

   Load node data from XML file
   pre-condition: the passed XML parser has the active node set to "data-elements"
   post-condition: the passed XML parser has the active node set to the same "data-elements"

   \param[out]     orc_NodeDataPoolListElements    data storage
   \param[in,out]  orc_XmlParser                   XML with list active

   \return
   C_NO_ERR   data read
   C_CONFIG   content of file is invalid or incomplete
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_OscNodeDataPoolFiler::h_LoadDataPoolListElements(
   std::vector<C_OscNodeDataPoolListElement> & orc_NodeDataPoolListElements, C_OscXmlParserBase & orc_XmlParser)
{
   int32_t s32_Retval = C_NO_ERR;
   C_SclString c_CurNodeDataElement;
   uint32_t u32_ExpectedSize = 0UL;
   const bool q_ExpectedSizeHere = orc_XmlParser.AttributeExists("length");

   //Check optional length
   if (q_ExpectedSizeHere == true)
   {
      u32_ExpectedSize = orc_XmlParser.GetAttributeUint32("length");
      orc_NodeDataPoolListElements.reserve(u32_ExpectedSize);
   }

   c_CurNodeDataElement = orc_XmlParser.SelectNodeChild("data-element");

   //Clear
   orc_NodeDataPoolListElements.clear();
   if (c_CurNodeDataElement == "data-element")
   {
      do
      {
         C_OscNodeDataPoolListElement c_CurDataElement;

         if (s32_Retval == C_NO_ERR)
         {
            s32_Retval = h_LoadDataPoolElement(c_CurDataElement, orc_XmlParser);
         }

         //Append
         orc_NodeDataPoolListElements.push_back(c_CurDataElement);

         //Next
         c_CurNodeDataElement = orc_XmlParser.SelectNodeNext("data-element");
      }
      while (c_CurNodeDataElement == "data-element");
      //Return
      tgl_assert(orc_XmlParser.SelectNodeParent() == "data-elements");
   }
   //Compare length
   if ((s32_Retval == C_NO_ERR) && (q_ExpectedSizeHere == true))
   {
      if (u32_ExpectedSize != orc_NodeDataPoolListElements.size())
      {
         C_SclString c_Tmp;
         c_Tmp.PrintFormatted("Unexpected data element count, expected: %u, got %u", u32_ExpectedSize,
                              static_cast<uint32_t>(orc_NodeDataPoolListElements.size()));
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

   \param[in]      orc_NodeDataPoolListElements    data storage
   \param[in,out]  orc_XmlParser                   XML with list active
   \param[in]      oe_DatapoolType                 Datapool type
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OscNodeDataPoolFiler::h_SaveDataPoolListElements(
   const std::vector<C_OscNodeDataPoolListElement> & orc_NodeDataPoolListElements, C_OscXmlParserBase & orc_XmlParser,
   const C_OscNodeDataPool::E_Type oe_DatapoolType)
{
   orc_XmlParser.SetAttributeUint32("length", static_cast<uint32_t>(orc_NodeDataPoolListElements.size()));
   for (uint32_t u32_ItDataElement = 0; u32_ItDataElement < orc_NodeDataPoolListElements.size();
        ++u32_ItDataElement)
   {
      orc_XmlParser.CreateAndSelectNodeChild("data-element");
      h_SaveDataPoolElement(orc_NodeDataPoolListElements[u32_ItDataElement], orc_XmlParser, oe_DatapoolType);
      //Return
      tgl_assert(orc_XmlParser.SelectNodeParent() == "data-elements");
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Load node data pool list element data set values

   Load node data from XML file
   pre-condition: the passed XML parser has the active node set to "data-set-values"
   post-condition: the passed XML parser has the active node set to the same "data-set-values"

   All returned elements will be of the type defined by orc_ContentType.

   \param[in]      orc_ContentType                             type reference (see description)
   \param[out]     orc_NodeDataPoolListElementDataSetValues    data storage
   \param[in,out]  orc_XmlParser                               XML with list active

   \return
   C_NO_ERR   data read
   C_CONFIG   content of file is invalid or incomplete
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_OscNodeDataPoolFiler::h_LoadDataPoolListElementDataSetValues(const C_OscNodeDataPoolContent & orc_ContentType,
                                                                       std::vector<C_OscNodeDataPoolContent> & orc_NodeDataPoolListElementDataSetValues,
                                                                       C_OscXmlParserBase & orc_XmlParser)
{
   int32_t s32_Retval = C_NO_ERR;
   C_SclString c_CurNodeDataSetValue = orc_XmlParser.SelectNodeChild("data-set-value");

   if (c_CurNodeDataSetValue == "data-set-value")
   {
      do
      {
         C_OscNodeDataPoolContent c_CurDataSetValue = orc_ContentType; //pre set content type

         if (s32_Retval == C_NO_ERR)
         {
            s32_Retval = h_LoadDataPoolElementValue(c_CurDataSetValue, orc_XmlParser, false);
         }

         //Append
         orc_NodeDataPoolListElementDataSetValues.push_back(c_CurDataSetValue);

         //Next
         c_CurNodeDataSetValue = orc_XmlParser.SelectNodeNext("data-set-value");
      }
      while (c_CurNodeDataSetValue == "data-set-value");
      //Return
      tgl_assert(orc_XmlParser.SelectNodeParent() == "data-set-values");
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Save node data pool list element data set values

   Save node to XML file
   pre-condition: the passed XML parser has the active node set to "data-set-values"
   post-condition: the passed XML parser has the active node set to the same "data-set-values"

   \param[in]      orc_NodeDataPoolListElementDataSetValues    data storage
   \param[in,out]  orc_XmlParser                               XML with list active
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OscNodeDataPoolFiler::h_SaveDataPoolListElementDataSetValues(
   const std::vector<C_OscNodeDataPoolContent> & orc_NodeDataPoolListElementDataSetValues,
   C_OscXmlParserBase & orc_XmlParser)
{
   //Data set values
   for (uint32_t u32_ItDataSetValue = 0; u32_ItDataSetValue < orc_NodeDataPoolListElementDataSetValues.size();
        ++u32_ItDataSetValue)
   {
      h_SaveDataPoolElementValue("data-set-value", orc_NodeDataPoolListElementDataSetValues[u32_ItDataSetValue],
                                 orc_XmlParser);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Load node data pool list data sets

   Load node data from XML file
   pre-condition: the passed XML parser has the active node set to "data-sets"
   post-condition: the passed XML parser has the active node set to the same "data-sets"

   \param[out]     orc_NodeDataPoolListDataSets    data storage
   \param[in,out]  orc_XmlParser                   XML with list active

   \return
   C_NO_ERR   data read
   C_CONFIG   content of file is invalid or incomplete
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_OscNodeDataPoolFiler::h_LoadDataPoolListDataSets(
   std::vector<C_OscNodeDataPoolDataSet> & orc_NodeDataPoolListDataSets, C_OscXmlParserBase & orc_XmlParser)
{
   int32_t s32_Retval = C_NO_ERR;
   C_SclString c_CurNodeDataSet = orc_XmlParser.SelectNodeChild("data-set");

   orc_NodeDataPoolListDataSets.clear();
   if (c_CurNodeDataSet == "data-set")
   {
      do
      {
         C_OscNodeDataPoolDataSet c_CurDataSet;

         if (orc_XmlParser.SelectNodeChild("name") == "name")
         {
            c_CurDataSet.c_Name = orc_XmlParser.GetNodeContent();
            //Return
            tgl_assert(orc_XmlParser.SelectNodeParent() == "data-set");
         }
         else
         {
            s32_Retval = C_CONFIG;
         }

         if (orc_XmlParser.SelectNodeChild("comment") == "comment")
         {
            c_CurDataSet.c_Comment = orc_XmlParser.GetNodeContent();
            //Return
            tgl_assert(orc_XmlParser.SelectNodeParent() == "data-set");
         }
         else
         {
            s32_Retval = C_CONFIG;
         }

         //Append
         orc_NodeDataPoolListDataSets.push_back(c_CurDataSet);

         //Next
         c_CurNodeDataSet = orc_XmlParser.SelectNodeNext("data-set");
      }
      while (c_CurNodeDataSet == "data-set");
      //Return
      tgl_assert(orc_XmlParser.SelectNodeParent() == "data-sets");
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Save node data pool list data sets

   Save node to XML file
   pre-condition: the passed XML parser has the active node set to "data-sets"
   post-condition: the passed XML parser has the active node set to the same "data-sets"

   \param[in]      orc_NodeDataPoolListDataSets    data storage
   \param[in,out]  orc_XmlParser                   XML with list active
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OscNodeDataPoolFiler::h_SaveDataPoolListDataSets(
   const std::vector<C_OscNodeDataPoolDataSet> & orc_NodeDataPoolListDataSets, C_OscXmlParserBase & orc_XmlParser)
{
   for (uint32_t u32_ItDataSet = 0; u32_ItDataSet < orc_NodeDataPoolListDataSets.size(); ++u32_ItDataSet)
   {
      const C_OscNodeDataPoolDataSet & rc_DataSet = orc_NodeDataPoolListDataSets[u32_ItDataSet];

      orc_XmlParser.CreateAndSelectNodeChild("data-set");
      orc_XmlParser.CreateNodeChild("name", rc_DataSet.c_Name);
      orc_XmlParser.CreateNodeChild("comment", rc_DataSet.c_Comment);
      //Return
      tgl_assert(orc_XmlParser.SelectNodeParent() == "data-sets");
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Transform data pool type to string

   \param[in]  ore_DataPool   Data pool type

   \return
   Stringified data pool type
*/
//----------------------------------------------------------------------------------------------------------------------
C_SclString C_OscNodeDataPoolFiler::h_DataPoolToString(const C_OscNodeDataPool::E_Type & ore_DataPool)
{
   C_SclString c_Retval;

   switch (ore_DataPool)
   {
   case C_OscNodeDataPool::eDIAG:
      c_Retval = "diag";
      break;
   case C_OscNodeDataPool::eCOM:
      c_Retval = "com";
      break;
   case C_OscNodeDataPool::eNVM:
      c_Retval = "nvm";
      break;
   case C_OscNodeDataPool::eHALC:
      c_Retval = "halc";
      break;
   case C_OscNodeDataPool::eHALC_NVM:
      c_Retval = "halc-nvm";
      break;
   default:
      c_Retval = "invalid";
      break;
   }
   return c_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Transform string to data pool type

   \param[in]   orc_String    String to interpret
   \param[out]  ore_Type      Data pool type

   \return
   C_NO_ERR   no error
   C_RANGE    String unknown
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_OscNodeDataPoolFiler::h_StringToDataPool(const C_SclString & orc_String, C_OscNodeDataPool::E_Type & ore_Type)
{
   int32_t s32_Retval = C_NO_ERR;

   if (orc_String == "com")
   {
      ore_Type = C_OscNodeDataPool::eCOM;
   }
   else if (orc_String == "nvm")
   {
      ore_Type = C_OscNodeDataPool::eNVM;
   }
   else if (orc_String == "diag")
   {
      ore_Type = C_OscNodeDataPool::eDIAG;
   }
   else if (orc_String == "halc")
   {
      ore_Type = C_OscNodeDataPool::eHALC;
   }
   else if (orc_String == "halc-nvm")
   {
      ore_Type = C_OscNodeDataPool::eHALC_NVM;
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

   \param[out]     orc_NodeDataPoolContent   data storage
   \param[in,out]  orc_XmlParser             XML with unknown (Node to store data pool variable) active

   \return
   C_NO_ERR   data read
   C_CONFIG   content of file is invalid or incomplete
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_OscNodeDataPoolFiler::h_LoadDataPoolElementType(C_OscNodeDataPoolContent & orc_NodeDataPoolContent,
                                                          C_OscXmlParserBase & orc_XmlParser)
{
   int32_t s32_Retval = C_CONFIG;

   if (orc_XmlParser.SelectNodeChild("type") == "type")
   {
      C_OscNodeDataPoolContent::E_Type e_Type;
      s32_Retval = mh_StringToNodeDataPoolContent(orc_XmlParser.GetAttributeString("base-type"), e_Type);
      if (s32_Retval == C_NO_ERR)
      {
         orc_NodeDataPoolContent.SetType(e_Type);
         orc_NodeDataPoolContent.SetArray(orc_XmlParser.GetAttributeBool("is-array"));
         if (orc_NodeDataPoolContent.GetArray() == true)
         {
            orc_NodeDataPoolContent.SetArraySize(orc_XmlParser.GetAttributeUint32("array-size"));
         }
      }
      //Return
      orc_XmlParser.SelectNodeParent();
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

   \param[in]      orc_NodeDataPoolContent   data storage
   \param[in,out]  orc_XmlParser             XML parser
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OscNodeDataPoolFiler::h_SaveDataPoolElementType(const C_OscNodeDataPoolContent & orc_NodeDataPoolContent,
                                                       C_OscXmlParserBase & orc_XmlParser)
{
   orc_XmlParser.CreateAndSelectNodeChild("type");
   orc_XmlParser.SetAttributeString("base-type", mh_NodeDataPoolContentToString(orc_NodeDataPoolContent.GetType()));
   orc_XmlParser.SetAttributeBool("is-array", orc_NodeDataPoolContent.GetArray());
   if (orc_NodeDataPoolContent.GetArray() == true)
   {
      orc_XmlParser.SetAttributeUint32("array-size", orc_NodeDataPoolContent.GetArraySize());
   }

   orc_XmlParser.SelectNodeParent();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Load data pool element value

   Load node data from XML file
   pre-conditions:
   * the passed XML parser has the active node set to the node containing the value
   * the element type of orc_NodeDataPoolContent must match the data contained in the file
   The function does not change the active node.

   \param[out]     orc_NodeDataPoolContent         data storage
   \param[in,out]  orc_XmlParser                   XML with unknown (Node to store data pool variable) active
   \param[in]      oq_CheckDataType                Check data type
   \param[in,out]  opc_CheckDataTypeErrorDetails   Check data type error details

   \return
   C_NO_ERR   data read
   C_CONFIG   content of file is invalid or incomplete
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_OscNodeDataPoolFiler::h_LoadDataPoolElementValue(C_OscNodeDataPoolContent & orc_NodeDataPoolContent,
                                                           C_OscXmlParserBase & orc_XmlParser,
                                                           const bool oq_CheckDataType,
                                                           stw::scl::C_SclString * const opc_CheckDataTypeErrorDetails)
{
   int32_t s32_Retval = C_NO_ERR;

   if (orc_NodeDataPoolContent.GetArray() == false)
   {
      if (oq_CheckDataType)
      {
         s32_Retval = C_OscNodeDataPoolFiler::h_CheckDataPoolElementValueType(
            orc_NodeDataPoolContent.GetType(), orc_XmlParser, opc_CheckDataTypeErrorDetails);
      }
      if (s32_Retval == C_NO_ERR)
      {
         //Single
         switch (orc_NodeDataPoolContent.GetType())
         {
         case C_OscNodeDataPoolContent::eUINT8:
            orc_NodeDataPoolContent.SetValueU8(static_cast<uint8_t>(orc_XmlParser.GetAttributeUint32("value")));
            break;
         case C_OscNodeDataPoolContent::eUINT16:
            orc_NodeDataPoolContent.SetValueU16(static_cast<uint16_t>(orc_XmlParser.GetAttributeUint32("value")));
            break;
         case C_OscNodeDataPoolContent::eUINT32:
            orc_NodeDataPoolContent.SetValueU32(orc_XmlParser.GetAttributeUint32("value"));
            break;
         case C_OscNodeDataPoolContent::eUINT64:
            orc_NodeDataPoolContent.SetValueU64(orc_XmlParser.GetAttributeUint64("value"));
            break;
         case C_OscNodeDataPoolContent::eSINT8:
            orc_NodeDataPoolContent.SetValueS8(static_cast<int8_t>(orc_XmlParser.GetAttributeSint64("value")));
            break;
         case C_OscNodeDataPoolContent::eSINT16:
            orc_NodeDataPoolContent.SetValueS16(static_cast<int16_t>(orc_XmlParser.GetAttributeSint64("value")));
            break;
         case C_OscNodeDataPoolContent::eSINT32:
            orc_NodeDataPoolContent.SetValueS32(static_cast<int32_t>(orc_XmlParser.GetAttributeSint64("value")));
            break;
         case C_OscNodeDataPoolContent::eSINT64:
            orc_NodeDataPoolContent.SetValueS64(orc_XmlParser.GetAttributeSint64("value"));
            break;
         case C_OscNodeDataPoolContent::eFLOAT32:
            orc_NodeDataPoolContent.SetValueF32(orc_XmlParser.GetAttributeFloat32("value"));
            break;
         case C_OscNodeDataPoolContent::eFLOAT64:
            orc_NodeDataPoolContent.SetValueF64(orc_XmlParser.GetAttributeFloat64("value"));
            break;
         default:
            break;
         }
      }
   }
   else
   {
      //Array
      C_SclString c_CurNode = orc_XmlParser.SelectNodeChild("element");
      if (c_CurNode == "element")
      {
         uint32_t u32_CurIndex = 0U;
         do
         {
            if (u32_CurIndex > orc_NodeDataPoolContent.GetArraySize())
            {
               break; //too much information ...
            }

            if (oq_CheckDataType)
            {
               s32_Retval = C_OscNodeDataPoolFiler::h_CheckDataPoolElementValueType(
                  orc_NodeDataPoolContent.GetType(), orc_XmlParser, opc_CheckDataTypeErrorDetails);
            }
            switch (orc_NodeDataPoolContent.GetType())
            {
            case C_OscNodeDataPoolContent::eUINT8:
               orc_NodeDataPoolContent.SetValueArrU8Element(static_cast<uint8_t>(orc_XmlParser.GetAttributeUint32(
                                                                                    "value")), u32_CurIndex);
               break;
            case C_OscNodeDataPoolContent::eUINT16:
               orc_NodeDataPoolContent.SetValueArrU16Element(static_cast<uint16_t>(orc_XmlParser.GetAttributeUint32(
                                                                                      "value")), u32_CurIndex);
               break;
            case C_OscNodeDataPoolContent::eUINT32:
               orc_NodeDataPoolContent.SetValueArrU32Element(orc_XmlParser.GetAttributeUint32("value"),
                                                             u32_CurIndex);
               break;
            case C_OscNodeDataPoolContent::eUINT64:
               orc_NodeDataPoolContent.SetValueArrU64Element(orc_XmlParser.GetAttributeUint64("value"), u32_CurIndex);
               break;
            case C_OscNodeDataPoolContent::eSINT8:
               orc_NodeDataPoolContent.SetValueArrS8Element(static_cast<int8_t>(orc_XmlParser.GetAttributeSint64(
                                                                                   "value")), u32_CurIndex);
               break;
            case C_OscNodeDataPoolContent::eSINT16:
               orc_NodeDataPoolContent.SetValueArrS16Element(static_cast<int16_t>(orc_XmlParser.GetAttributeSint64(
                                                                                     "value")), u32_CurIndex);
               break;
            case C_OscNodeDataPoolContent::eSINT32:
               orc_NodeDataPoolContent.SetValueArrS32Element(static_cast<int32_t>(orc_XmlParser.GetAttributeSint64(
                                                                                     "value")), u32_CurIndex);
               break;
            case C_OscNodeDataPoolContent::eSINT64:
               orc_NodeDataPoolContent.SetValueArrS64Element(orc_XmlParser.GetAttributeSint64("value"),
                                                             u32_CurIndex);
               break;
            case C_OscNodeDataPoolContent::eFLOAT32:
               orc_NodeDataPoolContent.SetValueArrF32Element(orc_XmlParser.GetAttributeFloat32("value"),
                                                             u32_CurIndex);
               break;
            case C_OscNodeDataPoolContent::eFLOAT64:
               orc_NodeDataPoolContent.SetValueArrF64Element(orc_XmlParser.GetAttributeFloat64("value"),
                                                             u32_CurIndex);
               break;
            default:
               break;
            }
            u32_CurIndex++; //next element
            c_CurNode = orc_XmlParser.SelectNodeNext("element");
         }
         while ((c_CurNode == "element") && (s32_Retval == C_NO_ERR));

         //check whether we have the correct number of elements:
         if (u32_CurIndex != orc_NodeDataPoolContent.GetArraySize())
         {
            osc_write_log_error("Loading Datapool", "Incorrect size of value for array value.");
            s32_Retval = C_CONFIG;
         }
      }
      //Return
      orc_XmlParser.SelectNodeParent();
   }

   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Save data pool element value

   Save data pool element value to XML parser
   Will create a node and write the value there.
   Does not modify the active node.

   \param[in]      orc_NodeName              name of node to create value in
   \param[in]      orc_NodeDataPoolContent   data storage
   \param[in,out]  orc_XmlParser             XML parser
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OscNodeDataPoolFiler::h_SaveDataPoolElementValue(const stw::scl::C_SclString & orc_NodeName,
                                                        const C_OscNodeDataPoolContent & orc_NodeDataPoolContent,
                                                        C_OscXmlParserBase & orc_XmlParser)
{
   orc_XmlParser.CreateAndSelectNodeChild(orc_NodeName);
   if (orc_NodeDataPoolContent.GetArray() == false)
   {
      //Single
      switch (orc_NodeDataPoolContent.GetType())
      {
      case C_OscNodeDataPoolContent::eUINT8:
         orc_XmlParser.SetAttributeUint32("value", static_cast<uint32_t>(orc_NodeDataPoolContent.GetValueU8()));
         break;
      case C_OscNodeDataPoolContent::eUINT16:
         orc_XmlParser.SetAttributeUint32("value", static_cast<uint32_t>(orc_NodeDataPoolContent.GetValueU16()));
         break;
      case C_OscNodeDataPoolContent::eUINT32:
         orc_XmlParser.SetAttributeUint32("value", orc_NodeDataPoolContent.GetValueU32());
         break;
      case C_OscNodeDataPoolContent::eUINT64:
         orc_XmlParser.SetAttributeUint64("value", orc_NodeDataPoolContent.GetValueU64());
         break;
      case C_OscNodeDataPoolContent::eSINT8:
         orc_XmlParser.SetAttributeSint64("value", static_cast<int64_t>(orc_NodeDataPoolContent.GetValueS8()));
         break;
      case C_OscNodeDataPoolContent::eSINT16:
         orc_XmlParser.SetAttributeSint64("value", static_cast<int64_t>(orc_NodeDataPoolContent.GetValueS16()));
         break;
      case C_OscNodeDataPoolContent::eSINT32:
         orc_XmlParser.SetAttributeSint64("value", static_cast<int64_t>(orc_NodeDataPoolContent.GetValueS32()));
         break;
      case C_OscNodeDataPoolContent::eSINT64:
         orc_XmlParser.SetAttributeSint64("value", orc_NodeDataPoolContent.GetValueS64());
         break;
      case C_OscNodeDataPoolContent::eFLOAT32:
         orc_XmlParser.SetAttributeFloat32("value", orc_NodeDataPoolContent.GetValueF32());
         break;
      case C_OscNodeDataPoolContent::eFLOAT64:
         orc_XmlParser.SetAttributeFloat64("value", orc_NodeDataPoolContent.GetValueF64());
         break;
      default:
         break;
      }
   }
   else
   {
      //Array
      for (uint32_t u32_ItElem = 0; u32_ItElem < orc_NodeDataPoolContent.GetArraySize(); ++u32_ItElem)
      {
         orc_XmlParser.CreateAndSelectNodeChild("element");
         switch (orc_NodeDataPoolContent.GetType())
         {
         case C_OscNodeDataPoolContent::eUINT8:
            orc_XmlParser.SetAttributeUint32("value",
                                             static_cast<uint32_t>(orc_NodeDataPoolContent.GetValueArrU8Element(
                                                                      u32_ItElem)));
            break;
         case C_OscNodeDataPoolContent::eUINT16:
            orc_XmlParser.SetAttributeUint32("value",
                                             static_cast<uint32_t>(orc_NodeDataPoolContent.GetValueArrU16Element(
                                                                      u32_ItElem)));
            break;
         case C_OscNodeDataPoolContent::eUINT32:
            orc_XmlParser.SetAttributeUint32("value", orc_NodeDataPoolContent.GetValueArrU32Element(u32_ItElem));
            break;
         case C_OscNodeDataPoolContent::eUINT64:
            orc_XmlParser.SetAttributeUint64("value", orc_NodeDataPoolContent.GetValueArrU64Element(u32_ItElem));
            break;
         case C_OscNodeDataPoolContent::eSINT8:
            orc_XmlParser.SetAttributeSint64("value",
                                             static_cast<int64_t>(orc_NodeDataPoolContent.GetValueArrS8Element(
                                                                     u32_ItElem)));
            break;
         case C_OscNodeDataPoolContent::eSINT16:
            orc_XmlParser.SetAttributeSint64("value",
                                             static_cast<int64_t>(orc_NodeDataPoolContent.GetValueArrS16Element(
                                                                     u32_ItElem)));
            break;
         case C_OscNodeDataPoolContent::eSINT32:
            orc_XmlParser.SetAttributeSint64("value",
                                             static_cast<int64_t>(orc_NodeDataPoolContent.GetValueArrS32Element(
                                                                     u32_ItElem)));
            break;
         case C_OscNodeDataPoolContent::eSINT64:
            orc_XmlParser.SetAttributeSint64("value", orc_NodeDataPoolContent.GetValueArrS64Element(u32_ItElem));
            break;
         case C_OscNodeDataPoolContent::eFLOAT32:
            orc_XmlParser.SetAttributeFloat32("value", orc_NodeDataPoolContent.GetValueArrF32Element(u32_ItElem));
            break;
         case C_OscNodeDataPoolContent::eFLOAT64:
            orc_XmlParser.SetAttributeFloat64("value", orc_NodeDataPoolContent.GetValueArrF64Element(u32_ItElem));
            break;
         default:
            break;
         }
         //Return to parent
         tgl_assert(orc_XmlParser.SelectNodeParent() == orc_NodeName);
      }
   }
   //Return to parent
   orc_XmlParser.SelectNodeParent();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Save node data pool content

   Save node to XML file in V1 format.
   Not used by core. But may be of some use to applications.

   pre-condition: the passed XML parser has the active node set to variable (Node to store data pool content)
   post-condition: the passed XML parser has the active node set to the same variable (Node to store data pool content)

   \param[in]      orc_NodeDataPoolContent   data storage
   \param[in,out]  orc_XmlParser             XML with variable (Node to store data pool content) active
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OscNodeDataPoolFiler::h_SaveDataPoolContentV1(const C_OscNodeDataPoolContent & orc_NodeDataPoolContent,
                                                     C_OscXmlParserBase & orc_XmlParser)
{
   orc_XmlParser.SetAttributeBool("array", orc_NodeDataPoolContent.GetArray());
   orc_XmlParser.CreateNodeChild("type", mh_NodeDataPoolContentToString(orc_NodeDataPoolContent.GetType()));
   if (orc_NodeDataPoolContent.GetArray() == false)
   {
      //Single
      switch (orc_NodeDataPoolContent.GetType())
      {
      case C_OscNodeDataPoolContent::eUINT8:
         orc_XmlParser.SetAttributeUint32("value", static_cast<uint32_t>(orc_NodeDataPoolContent.GetValueU8()));
         break;
      case C_OscNodeDataPoolContent::eUINT16:
         orc_XmlParser.SetAttributeUint32("value", static_cast<uint32_t>(orc_NodeDataPoolContent.GetValueU16()));
         break;
      case C_OscNodeDataPoolContent::eUINT32:
         orc_XmlParser.SetAttributeUint32("value", orc_NodeDataPoolContent.GetValueU32());
         break;
      case C_OscNodeDataPoolContent::eUINT64:
         orc_XmlParser.SetAttributeUint64("value", orc_NodeDataPoolContent.GetValueU64());
         break;
      case C_OscNodeDataPoolContent::eSINT8:
         orc_XmlParser.SetAttributeSint64("value", static_cast<int64_t>(orc_NodeDataPoolContent.GetValueS8()));
         break;
      case C_OscNodeDataPoolContent::eSINT16:
         orc_XmlParser.SetAttributeSint64("value", static_cast<int64_t>(orc_NodeDataPoolContent.GetValueS16()));
         break;
      case C_OscNodeDataPoolContent::eSINT32:
         orc_XmlParser.SetAttributeSint64("value", static_cast<int64_t>(orc_NodeDataPoolContent.GetValueS32()));
         break;
      case C_OscNodeDataPoolContent::eSINT64:
         orc_XmlParser.SetAttributeSint64("value", orc_NodeDataPoolContent.GetValueS64());
         break;
      case C_OscNodeDataPoolContent::eFLOAT32:
         orc_XmlParser.SetAttributeFloat32("value", orc_NodeDataPoolContent.GetValueF32());
         break;
      case C_OscNodeDataPoolContent::eFLOAT64:
         orc_XmlParser.SetAttributeFloat64("value", orc_NodeDataPoolContent.GetValueF64());
         break;
      default:
         break;
      }
   }
   else
   {
      //Array
      orc_XmlParser.CreateAndSelectNodeChild("array");
      for (uint32_t u32_ItElem = 0; u32_ItElem < orc_NodeDataPoolContent.GetArraySize(); ++u32_ItElem)
      {
         orc_XmlParser.CreateAndSelectNodeChild("element");
         orc_XmlParser.SetAttributeUint32("index", u32_ItElem);
         switch (orc_NodeDataPoolContent.GetType())
         {
         case C_OscNodeDataPoolContent::eUINT8:
            orc_XmlParser.SetAttributeUint32(
               "content",
               static_cast<uint32_t>(orc_NodeDataPoolContent.GetValueArrU8Element(u32_ItElem)));
            break;
         case C_OscNodeDataPoolContent::eUINT16:
            orc_XmlParser.SetAttributeUint32("content",
                                             static_cast<uint32_t>(orc_NodeDataPoolContent.GetValueArrU16Element(
                                                                      u32_ItElem)));
            break;
         case C_OscNodeDataPoolContent::eUINT32:
            orc_XmlParser.SetAttributeUint32("content", orc_NodeDataPoolContent.GetValueArrU32Element(u32_ItElem));
            break;
         case C_OscNodeDataPoolContent::eUINT64:
            orc_XmlParser.SetAttributeUint64("content", orc_NodeDataPoolContent.GetValueArrU64Element(u32_ItElem));
            break;
         case C_OscNodeDataPoolContent::eSINT8:
            orc_XmlParser.SetAttributeSint64(
               "content",
               static_cast<int64_t>(orc_NodeDataPoolContent.GetValueArrS8Element(u32_ItElem)));
            break;
         case C_OscNodeDataPoolContent::eSINT16:
            orc_XmlParser.SetAttributeSint64("content",
                                             static_cast<int64_t>(orc_NodeDataPoolContent.GetValueArrS16Element(
                                                                     u32_ItElem)));
            break;
         case C_OscNodeDataPoolContent::eSINT32:
            orc_XmlParser.SetAttributeSint64("content",
                                             static_cast<int64_t>(orc_NodeDataPoolContent.GetValueArrS32Element(
                                                                     u32_ItElem)));
            break;
         case C_OscNodeDataPoolContent::eSINT64:
            orc_XmlParser.SetAttributeSint64("content", orc_NodeDataPoolContent.GetValueArrS64Element(u32_ItElem));
            break;
         case C_OscNodeDataPoolContent::eFLOAT32:
            orc_XmlParser.SetAttributeFloat32("content", orc_NodeDataPoolContent.GetValueArrF32Element(u32_ItElem));
            break;
         case C_OscNodeDataPoolContent::eFLOAT64:
            orc_XmlParser.SetAttributeFloat64("content", orc_NodeDataPoolContent.GetValueArrF64Element(u32_ItElem));
            break;
         default:
            break;
         }
         //Return
         tgl_assert(orc_XmlParser.SelectNodeParent() == "array");
      }
      //Return
      orc_XmlParser.SelectNodeParent();
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get automatically generated file name

   \param[in]  orc_DatapoolName  Datapool name

   \return
   Automatically generated file name
*/
//----------------------------------------------------------------------------------------------------------------------
C_SclString C_OscNodeDataPoolFiler::h_GetFileName(const C_SclString & orc_DatapoolName)
{
   return "dp_" + C_OscSystemFilerUtil::h_PrepareItemNameForFileName(orc_DatapoolName) + "_core.xml";
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Check data pool element value type

   \param[in]      oe_ContentType                  Content type
   \param[in]      orc_XmlParser                   XML parser
   \param[in,out]  opc_CheckDataTypeErrorDetails   Check data type error details

   \return
   C_NO_ERR   data read
   C_CONFIG   content of file is invalid or incomplete
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_OscNodeDataPoolFiler::h_CheckDataPoolElementValueType(const C_OscNodeDataPoolContent::E_Type oe_ContentType,
                                                                const C_OscXmlParserBase & orc_XmlParser,
                                                                stw::scl::C_SclString * const opc_CheckDataTypeErrorDetails)
{
   int32_t s32_Retval = C_CONFIG;

   //Load value
   const uint64_t u64_Val = orc_XmlParser.GetAttributeUint64("value");
   const int64_t s64_Val = orc_XmlParser.GetAttributeSint64("value");
   const float64_t f64_Val = orc_XmlParser.GetAttributeFloat64("value");

   switch (oe_ContentType)
   {
   case C_OscNodeDataPoolContent::eUINT8:
      if ((((u64_Val <= std::numeric_limits<uint8_t>::max())) &&
           ((s64_Val >= static_cast<int64_t>(std::numeric_limits<uint8_t>::min())) &&
            (s64_Val <= std::numeric_limits<uint8_t>::max()))) &&
          (((f64_Val >= static_cast<float64_t>(std::numeric_limits<uint8_t>::min())) &&
            (f64_Val <= static_cast<float64_t>(std::numeric_limits<uint8_t>::max())))))
      {
         s32_Retval = C_NO_ERR;
      }
      break;
   case C_OscNodeDataPoolContent::eUINT16:
      if ((((u64_Val <= std::numeric_limits<uint16_t>::max())) &&
           ((s64_Val >= static_cast<int64_t>(std::numeric_limits<uint16_t>::min())) &&
            (s64_Val <= std::numeric_limits<uint16_t>::max()))) &&
          ((f64_Val >= static_cast<float64_t>(std::numeric_limits<uint16_t>::min())) &&
           (f64_Val <= static_cast<float64_t>(std::numeric_limits<uint16_t>::max()))))
      {
         s32_Retval = C_NO_ERR;
      }
      break;
   case C_OscNodeDataPoolContent::eUINT32:
      if ((((u64_Val <= std::numeric_limits<uint32_t>::max())) &&
           ((s64_Val >= static_cast<int64_t>(std::numeric_limits<uint32_t>::min())) &&
            (s64_Val <= std::numeric_limits<uint32_t>::max()))) &&
          ((f64_Val >= static_cast<float64_t>(std::numeric_limits<uint32_t>::min())) &&
           (f64_Val <= static_cast<float64_t>(std::numeric_limits<uint32_t>::max()))))
      {
         s32_Retval = C_NO_ERR;
      }
      break;
   case C_OscNodeDataPoolContent::eUINT64:
      //Sint64 check not reliable as the range of uint64 and sint64 don't completely overlap
      // and overflows might mess up the check
      if (((f64_Val >= static_cast<float64_t>(std::numeric_limits<uint64_t>::min())) &&
           (f64_Val <= static_cast<float64_t>(std::numeric_limits<uint64_t>::max()))))
      {
         s32_Retval = C_NO_ERR;
      }
      break;
   case C_OscNodeDataPoolContent::eSINT8:
      //All unsigned checks not reliable as the range of uint64 and any signed value don't completely overlap
      // and overflows might mess up the check
      if ((((s64_Val >= std::numeric_limits<int8_t>::min()) &&
            (s64_Val <= std::numeric_limits<int8_t>::max()))) &&
          ((f64_Val >= static_cast<float64_t>(std::numeric_limits<int8_t>::min())) &&
           (f64_Val <= static_cast<float64_t>(std::numeric_limits<int8_t>::max()))))
      {
         s32_Retval = C_NO_ERR;
      }
      break;
   case C_OscNodeDataPoolContent::eSINT16:
      //All unsigned checks not reliable as the range of uint64 and any signed value don't completely overlap
      // and overflows might mess up the check
      if ((((s64_Val >= std::numeric_limits<int16_t>::min()) &&
            (s64_Val <= std::numeric_limits<int16_t>::max()))) &&
          ((f64_Val >= static_cast<float64_t>(std::numeric_limits<int16_t>::min())) &&
           (f64_Val <= static_cast<float64_t>(std::numeric_limits<int16_t>::max()))))
      {
         s32_Retval = C_NO_ERR;
      }
      break;
   case C_OscNodeDataPoolContent::eSINT32:
      //All unsigned checks not reliable as the range of uint64 and any signed value don't completely overlap
      // and overflows might mess up the check
      if ((((s64_Val >= std::numeric_limits<int32_t>::min()) &&
            (s64_Val <= std::numeric_limits<int32_t>::max()))) &&
          ((f64_Val >= static_cast<float64_t>(std::numeric_limits<int32_t>::min())) &&
           (f64_Val <= static_cast<float64_t>(std::numeric_limits<int32_t>::max()))))
      {
         s32_Retval = C_NO_ERR;
      }
      break;
   case C_OscNodeDataPoolContent::eSINT64:
      //All unsigned checks not reliable as the range of uint64 and any signed value don't completely overlap
      // and overflows might mess up the check
      if ((f64_Val >= static_cast<float64_t>(std::numeric_limits<int64_t>::min())) &&
          (f64_Val <= static_cast<float64_t>(std::numeric_limits<int64_t>::max())))
      {
         s32_Retval = C_NO_ERR;
      }
      break;
   case C_OscNodeDataPoolContent::eFLOAT32:
   case C_OscNodeDataPoolContent::eFLOAT64:
      //No range error or wrong type error possible for uint64 or sint64
      s32_Retval = C_NO_ERR;
      break;
   default:
      break;
   }
   if (s32_Retval != C_NO_ERR)
   {
      if (opc_CheckDataTypeErrorDetails != NULL)
      {
         const stw::scl::C_SclString c_Val = orc_XmlParser.GetAttributeString("value");
         const stw::scl::C_SclString c_DataType =
            C_OscNodeDataPoolFiler::mh_NodeDataPoolContentToString(oe_ContentType);

         *opc_CheckDataTypeErrorDetails = "\"" + c_Val + "\" not in range of data type " + c_DataType;
      }
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Load node data pool content

   Load node data from XML file in V1 format.
   pre-condition: the passed XML parser has the active node set to unknown (Node to store data pool variable)
   post-condition: the passed XML parser has the active node set to the same unknown (Node to store data pool variable)

   \param[out]     orc_NodeDataPoolContent   data storage
   \param[in,out]  orc_XmlParser             XML with unknown (Node to store data pool variable) active

   \return
   C_NO_ERR   data read
   C_CONFIG   content of file is invalid or incomplete
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_OscNodeDataPoolFiler::h_LoadDataPoolContentV1(C_OscNodeDataPoolContent & orc_NodeDataPoolContent,
                                                        C_OscXmlParserBase & orc_XmlParser)
{
   int32_t s32_Retval = C_NO_ERR;

   if (orc_XmlParser.SelectNodeChild("type") == "type")
   {
      C_OscNodeDataPoolContent::E_Type e_Type;
      s32_Retval = mh_StringToNodeDataPoolContent(orc_XmlParser.GetNodeContent(), e_Type);
      orc_NodeDataPoolContent.SetType(e_Type);
      //Return
      orc_XmlParser.SelectNodeParent();
   }
   else
   {
      s32_Retval = C_CONFIG;
   }
   orc_NodeDataPoolContent.SetArray(orc_XmlParser.GetAttributeBool("array"));

   if (orc_NodeDataPoolContent.GetArray() == false)
   {
      //same format as in newer file version:
      h_LoadDataPoolElementValue(orc_NodeDataPoolContent, orc_XmlParser, false);
   }
   else
   {
      //Array
      if (orc_XmlParser.SelectNodeChild("array") == "array")
      {
         C_SclString c_CurNode = orc_XmlParser.SelectNodeChild("element");
         if (c_CurNode == "element")
         {
            do
            {
               const uint32_t u32_CurIndex = orc_XmlParser.GetAttributeUint32("index");
               if (u32_CurIndex >= orc_NodeDataPoolContent.GetArraySize())
               {
                  orc_NodeDataPoolContent.SetArraySize(u32_CurIndex + 1);
               }
               switch (orc_NodeDataPoolContent.GetType())
               {
               case C_OscNodeDataPoolContent::eUINT8:
                  orc_NodeDataPoolContent.SetValueArrU8Element(static_cast<uint8_t>(orc_XmlParser.GetAttributeUint32(
                                                                                       "content")), u32_CurIndex);
                  break;
               case C_OscNodeDataPoolContent::eUINT16:
                  orc_NodeDataPoolContent.SetValueArrU16Element(static_cast<uint16_t>(orc_XmlParser.GetAttributeUint32(
                                                                                         "content")), u32_CurIndex);
                  break;
               case C_OscNodeDataPoolContent::eUINT32:
                  orc_NodeDataPoolContent.SetValueArrU32Element(orc_XmlParser.GetAttributeUint32("content"),
                                                                u32_CurIndex);
                  break;
               case C_OscNodeDataPoolContent::eUINT64:
                  orc_NodeDataPoolContent.SetValueArrU64Element(orc_XmlParser.GetAttributeUint64(
                                                                   "content"), u32_CurIndex);
                  break;
               case C_OscNodeDataPoolContent::eSINT8:
                  orc_NodeDataPoolContent.SetValueArrS8Element(static_cast<int8_t>(orc_XmlParser.GetAttributeSint64(
                                                                                      "content")), u32_CurIndex);
                  break;
               case C_OscNodeDataPoolContent::eSINT16:
                  orc_NodeDataPoolContent.SetValueArrS16Element(static_cast<int16_t>(orc_XmlParser.GetAttributeSint64(
                                                                                        "content")), u32_CurIndex);
                  break;
               case C_OscNodeDataPoolContent::eSINT32:
                  orc_NodeDataPoolContent.SetValueArrS32Element(static_cast<int32_t>(orc_XmlParser.GetAttributeSint64(
                                                                                        "content")), u32_CurIndex);
                  break;
               case C_OscNodeDataPoolContent::eSINT64:
                  orc_NodeDataPoolContent.SetValueArrS64Element(orc_XmlParser.GetAttributeSint64("content"),
                                                                u32_CurIndex);
                  break;
               case C_OscNodeDataPoolContent::eFLOAT32:
                  orc_NodeDataPoolContent.SetValueArrF32Element(orc_XmlParser.GetAttributeFloat32("content"),
                                                                u32_CurIndex);
                  break;
               case C_OscNodeDataPoolContent::eFLOAT64:
                  orc_NodeDataPoolContent.SetValueArrF64Element(orc_XmlParser.GetAttributeFloat64("content"),
                                                                u32_CurIndex);
                  break;
               default:
                  break;
               }
               c_CurNode = orc_XmlParser.SelectNodeNext("element");
            }
            while (c_CurNode == "element");
            //Return
            tgl_assert(orc_XmlParser.SelectNodeParent() == "array");
         }
         //Return
         orc_XmlParser.SelectNodeParent();
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

   \param[in]      orc_NodeDataPool    data storage
   \param[in,out]  orc_FilePath        File path for xml

   \return
   C_NO_ERR   data saved
   C_CONFIG   file could not be created
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_OscNodeDataPoolFiler::h_SaveDataPoolFile(const C_OscNodeDataPool & orc_NodeDataPool,
                                                   const C_SclString & orc_FilePath)
{
   C_OscXmlParser c_XmlParser;
   int32_t s32_Retval = C_OscSystemFilerUtil::h_GetParserForNewFile(c_XmlParser, orc_FilePath,
                                                                    "opensyde-dp-core-definition");

   if (s32_Retval == C_NO_ERR)
   {
      //Version
      c_XmlParser.CreateNodeChild("file-version", "1");
      tgl_assert(c_XmlParser.CreateAndSelectNodeChild("data-pool") == "data-pool");
      //node
      C_OscNodeDataPoolFiler::h_SaveDataPool(orc_NodeDataPool, c_XmlParser);
      //Don't forget to save!
      if (c_XmlParser.SaveToFile(orc_FilePath) != C_NO_ERR)
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

   \param[in]  ore_NodeDataPoolContent    Node data pool content type

   \return
   Stringified node data pool content type
*/
//----------------------------------------------------------------------------------------------------------------------
C_SclString C_OscNodeDataPoolFiler::mh_NodeDataPoolContentToString(
   const C_OscNodeDataPoolContent::E_Type & ore_NodeDataPoolContent)
{
   C_SclString c_Retval;

   switch (ore_NodeDataPoolContent)
   {
   case C_OscNodeDataPoolContent::eUINT8:
      c_Retval = "uint8";
      break;
   case C_OscNodeDataPoolContent::eUINT16:
      c_Retval = "uint16";
      break;
   case C_OscNodeDataPoolContent::eUINT32:
      c_Retval = "uint32";
      break;
   case C_OscNodeDataPoolContent::eUINT64:
      c_Retval = "uint64";
      break;
   case C_OscNodeDataPoolContent::eSINT8:
      c_Retval = "sint8";
      break;
   case C_OscNodeDataPoolContent::eSINT16:
      c_Retval = "sint16";
      break;
   case C_OscNodeDataPoolContent::eSINT32:
      c_Retval = "sint32";
      break;
   case C_OscNodeDataPoolContent::eSINT64:
      c_Retval = "sint64";
      break;
   case C_OscNodeDataPoolContent::eFLOAT32:
      c_Retval = "float32";
      break;
   case C_OscNodeDataPoolContent::eFLOAT64:
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

   \param[in]   orc_String    String to interpret
   \param[out]  ore_Type      Node data pool content type

   \return
   C_NO_ERR   no error
   C_RANGE    String unknown
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_OscNodeDataPoolFiler::mh_StringToNodeDataPoolContent(const C_SclString & orc_String,
                                                               C_OscNodeDataPoolContent::E_Type & ore_Type)
{
   int32_t s32_Retval = C_NO_ERR;

   if (orc_String == "uint8")
   {
      ore_Type = C_OscNodeDataPoolContent::eUINT8;
   }
   else if (orc_String == "uint16")
   {
      ore_Type = C_OscNodeDataPoolContent::eUINT16;
   }
   else if (orc_String == "uint32")
   {
      ore_Type = C_OscNodeDataPoolContent::eUINT32;
   }
   else if (orc_String == "uint64")
   {
      ore_Type = C_OscNodeDataPoolContent::eUINT64;
   }
   else if (orc_String == "sint8")
   {
      ore_Type = C_OscNodeDataPoolContent::eSINT8;
   }
   else if (orc_String == "sint16")
   {
      ore_Type = C_OscNodeDataPoolContent::eSINT16;
   }
   else if (orc_String == "sint32")
   {
      ore_Type = C_OscNodeDataPoolContent::eSINT32;
   }
   else if (orc_String == "sint64")
   {
      ore_Type = C_OscNodeDataPoolContent::eSINT64;
   }
   else if (orc_String == "float32")
   {
      ore_Type = C_OscNodeDataPoolContent::eFLOAT32;
   }
   else if (orc_String == "float64")
   {
      ore_Type = C_OscNodeDataPoolContent::eFLOAT64;
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

   \param[in]  ore_NodeDataPoolElementAccess    Node data pool element access type

   \return
   Stringified node data pool element access type
*/
//----------------------------------------------------------------------------------------------------------------------
C_SclString C_OscNodeDataPoolFiler::mh_NodeDataPoolElementAccessToString(
   const C_OscNodeDataPoolListElement::E_Access & ore_NodeDataPoolElementAccess)
{
   C_SclString c_Retval;

   switch (ore_NodeDataPoolElementAccess)
   {
   case C_OscNodeDataPoolListElement::eACCESS_RO:
      c_Retval = "read-only";
      break;
   case C_OscNodeDataPoolListElement::eACCESS_RW:
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

   \param[in]   orc_String    String to interpret
   \param[out]  ore_Type      Node data pool element access type

   \return
   C_NO_ERR   no error
   C_RANGE    String unknown
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_OscNodeDataPoolFiler::mh_StringToNodeDataPoolElementAccess(const C_SclString & orc_String,
                                                                     C_OscNodeDataPoolListElement::E_Access & ore_Type)
{
   int32_t s32_Retval = C_NO_ERR;

   if (orc_String == "read-write")
   {
      ore_Type = C_OscNodeDataPoolListElement::eACCESS_RW;
   }
   else if (orc_String == "read-only")
   {
      ore_Type = C_OscNodeDataPoolListElement::eACCESS_RO;
   }
   else
   {
      osc_write_log_error("Loading data element", "Invalid \"access\": " + orc_String);

      s32_Retval = C_RANGE;
   }

   return s32_Retval;
}
