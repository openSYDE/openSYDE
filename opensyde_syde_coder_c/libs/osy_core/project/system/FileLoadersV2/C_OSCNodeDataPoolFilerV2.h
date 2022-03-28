//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Node data pool reader/writer (V2) (header)

   See cpp file for detailed description

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_OSCNODEDATAPOOLFILERV2_H
#define C_OSCNODEDATAPOOLFILERV2_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */

#include "C_OSCNodeDataPool.h"
#include "C_OSCXMLParser.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw_opensyde_core
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_OSCNodeDataPoolFilerV2
{
public:
   C_OSCNodeDataPoolFilerV2();

   ///load functions
   static stw_types::sint32 h_LoadDataPool(const stw_types::uint16 ou16_XmlFormatVersion,
                                           C_OSCNodeDataPool & orc_NodeDataPool, C_OSCXMLParserBase & orc_XMLParser);
   static stw_types::sint32 h_LoadDataPoolList(const stw_types::uint16 ou16_XmlFormatVersion,
                                               C_OSCNodeDataPoolList & orc_NodeDataPoolList,
                                               C_OSCXMLParserBase & orc_XMLParser);
   static stw_types::sint32 h_LoadDataPoolElement(const stw_types::uint16 ou16_XmlFormatVersion,
                                                  C_OSCNodeDataPoolListElement & orc_NodeDataPoolListElement,
                                                  C_OSCXMLParserBase & orc_XMLParser);
   static stw_types::sint32 h_LoadDataPoolLists(const stw_types::uint16 ou16_XmlFormatVersion,
                                                std::vector<C_OSCNodeDataPoolList> & orc_NodeDataPoolLists,
                                                C_OSCXMLParserBase & orc_XMLParser);
   static stw_types::sint32 h_LoadDataPoolListElements(const stw_types::uint16 ou16_XmlFormatVersion,
                                                       std::vector<C_OSCNodeDataPoolListElement> & orc_NodeDataPoolListElements, C_OSCXMLParserBase & orc_XMLParser);
   static stw_types::sint32 h_LoadDataPoolListElementDataSetValues(const stw_types::uint16 ou16_XmlFormatVersion,
                                                                   const C_OSCNodeDataPoolContent & orc_ContType,
                                                                   std::vector<C_OSCNodeDataPoolContent> & orc_NodeDataPoolListElementDataSetValues, C_OSCXMLParserBase & orc_XMLParser);
   static stw_types::sint32 h_LoadDataPoolElementType(C_OSCNodeDataPoolContent & orc_NodeDataPoolContent,
                                                      C_OSCXMLParserBase & orc_XMLParser);
   static stw_types::sint32 h_LoadDataPoolElementValue(C_OSCNodeDataPoolContent & orc_NodeDataPoolContent,
                                                       C_OSCXMLParserBase & orc_XMLParser);
   static stw_types::sint32 h_LoadDataPoolContentV1(C_OSCNodeDataPoolContent & orc_NodeDataPoolContent,
                                                    C_OSCXMLParserBase & orc_XMLParser);

   ///save functions
   static void h_SaveDataPool(const C_OSCNodeDataPool & orc_NodeDataPool, C_OSCXMLParserBase & orc_XMLParser);
   static void h_SaveDataPoolList(const C_OSCNodeDataPoolList & orc_NodeDataPoolList,
                                  C_OSCXMLParserBase & orc_XMLParser);
   static void h_SaveDataPoolElement(const C_OSCNodeDataPoolListElement & orc_NodeDataPoolListElement,
                                     C_OSCXMLParserBase & orc_XMLParser);
   static void h_SaveDataPoolLists(const std::vector<C_OSCNodeDataPoolList> & orc_NodeDataPoolLists,
                                   C_OSCXMLParserBase & orc_XMLParser);
   static void h_SaveDataPoolListElements(
      const std::vector<C_OSCNodeDataPoolListElement> & orc_NodeDataPoolListElements,
      C_OSCXMLParserBase & orc_XMLParser);
   static void h_SaveDataPoolListElementDataSetValues(
      const std::vector<C_OSCNodeDataPoolContent> & orc_NodeDataPoolListElementDataSetValues,
      C_OSCXMLParserBase & orc_XMLParser);
   static stw_types::sint32 h_LoadDataPoolListDataSets(
      std::vector<C_OSCNodeDataPoolDataSet> & orc_NodeDataPoolListDataSets, C_OSCXMLParserBase & orc_XMLParser);
   static void h_SaveDataPoolListDataSets(const std::vector<C_OSCNodeDataPoolDataSet> & orc_NodeDataPoolListDataSets,
                                          C_OSCXMLParserBase & orc_XMLParser);
   static stw_scl::C_SCLString h_DataPoolToString(const C_OSCNodeDataPool::E_Type & ore_DataPool);
   static stw_types::sint32 h_StringToDataPool(const stw_scl::C_SCLString & orc_String,
                                               C_OSCNodeDataPool::E_Type & ore_Type);

   static void h_SaveDataPoolElementType(const C_OSCNodeDataPoolContent & orc_NodeDataPoolContent,
                                         C_OSCXMLParserBase & orc_XMLParser);
   static void h_SaveDataPoolElementValue(const stw_scl::C_SCLString & orc_NodeName,
                                          const C_OSCNodeDataPoolContent & orc_NodeDataPoolContent,
                                          C_OSCXMLParserBase & orc_XMLParser);
   static void h_SaveDataPoolContentV1(const C_OSCNodeDataPoolContent & orc_NodeDataPoolContent,
                                       C_OSCXMLParserBase & orc_XMLParser);

private:
   static stw_scl::C_SCLString mh_NodeDataPoolContentToString(
      const C_OSCNodeDataPoolContent::E_Type & ore_NodeDataPoolContent);
   static stw_types::sint32 mh_StringToNodeDataPoolContent(const stw_scl::C_SCLString & orc_String,
                                                           C_OSCNodeDataPoolContent::E_Type & ore_Type);
   static stw_scl::C_SCLString mh_NodeDataPoolElementAccessToString(
      const C_OSCNodeDataPoolListElement::E_Access & ore_NodeDataPoolElementAccess);
   static stw_types::sint32 mh_StringToNodeDataPoolElementAccess(const stw_scl::C_SCLString & orc_String,
                                                                 C_OSCNodeDataPoolListElement::E_Access & ore_Type);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif
