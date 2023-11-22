//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Node data pool reader/writer (V3) (header)

   See cpp file for detailed description

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_OSCNODEDATAPOOLFILERV3_HPP
#define C_OSCNODEDATAPOOLFILERV3_HPP

/* -- Includes ------------------------------------------------------------------------------------------------------ */

#include "C_OscNodeDataPool.hpp"
#include "C_OscXmlParser.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_core
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_OscNodeDataPoolFiler
{
public:
   C_OscNodeDataPoolFiler();

   ///load functions
   static int32_t h_LoadDataPoolFile(C_OscNodeDataPool & orc_NodeDataPool, const stw::scl::C_SclString & orc_FilePath);
   static int32_t h_LoadDataPool(C_OscNodeDataPool & orc_NodeDataPool, C_OscXmlParserBase & orc_XmlParser);
   static int32_t h_LoadDataPoolList(C_OscNodeDataPoolList & orc_NodeDataPoolList, C_OscXmlParserBase & orc_XmlParser);
   static int32_t h_LoadDataPoolElement(C_OscNodeDataPoolListElement & orc_NodeDataPoolListElement,
                                        C_OscXmlParserBase & orc_XmlParser);
   static int32_t h_LoadDataPoolLists(std::vector<C_OscNodeDataPoolList> & orc_NodeDataPoolLists,
                                      C_OscXmlParserBase & orc_XmlParser);
   static int32_t h_LoadDataPoolListElements(std::vector<C_OscNodeDataPoolListElement> & orc_NodeDataPoolListElements,
                                             C_OscXmlParserBase & orc_XmlParser);
   static int32_t h_LoadDataPoolListElementDataSetValues(const C_OscNodeDataPoolContent & orc_ContentType,
                                                         std::vector<C_OscNodeDataPoolContent> & orc_NodeDataPoolListElementDataSetValues, C_OscXmlParserBase & orc_XmlParser);
   static int32_t h_LoadDataPoolElementType(C_OscNodeDataPoolContent & orc_NodeDataPoolContent,
                                            C_OscXmlParserBase & orc_XmlParser);
   static int32_t h_LoadDataPoolElementValue(C_OscNodeDataPoolContent & orc_NodeDataPoolContent,
                                             C_OscXmlParserBase & orc_XmlParser, const bool oq_CheckDataType,
                                             stw::scl::C_SclString * const opc_CheckDataTypeErrorDetails = NULL);
   static int32_t h_LoadDataPoolContentV1(C_OscNodeDataPoolContent & orc_NodeDataPoolContent,
                                          C_OscXmlParserBase & orc_XmlParser);

   ///save functions
   static int32_t h_SaveDataPoolFile(const C_OscNodeDataPool & orc_NodeDataPool,
                                     const stw::scl::C_SclString & orc_FilePath);
   static void h_SaveDataPool(const C_OscNodeDataPool & orc_NodeDataPool, C_OscXmlParserBase & orc_XmlParser);
   static void h_SaveDataPoolList(const C_OscNodeDataPoolList & orc_NodeDataPoolList,
                                  C_OscXmlParserBase & orc_XmlParser, const C_OscNodeDataPool::E_Type oe_DatapoolType);
   static void h_SaveDataPoolElement(const C_OscNodeDataPoolListElement & orc_NodeDataPoolListElement,
                                     C_OscXmlParserBase & orc_XmlParser,
                                     const C_OscNodeDataPool::E_Type oe_DatapoolType);
   static void h_SaveDataPoolLists(const std::vector<C_OscNodeDataPoolList> & orc_NodeDataPoolLists,
                                   C_OscXmlParserBase & orc_XmlParser, const C_OscNodeDataPool::E_Type oe_DatapoolType);
   static void h_SaveDataPoolListElements(
      const std::vector<C_OscNodeDataPoolListElement> & orc_NodeDataPoolListElements,
      C_OscXmlParserBase & orc_XmlParser, const C_OscNodeDataPool::E_Type oe_DatapoolType);
   static void h_SaveDataPoolListElementDataSetValues(
      const std::vector<C_OscNodeDataPoolContent> & orc_NodeDataPoolListElementDataSetValues,
      C_OscXmlParserBase & orc_XmlParser);
   static int32_t h_LoadDataPoolListDataSets(std::vector<C_OscNodeDataPoolDataSet> & orc_NodeDataPoolListDataSets,
                                             C_OscXmlParserBase & orc_XmlParser);
   static void h_SaveDataPoolListDataSets(const std::vector<C_OscNodeDataPoolDataSet> & orc_NodeDataPoolListDataSets,
                                          C_OscXmlParserBase & orc_XmlParser);
   static stw::scl::C_SclString h_DataPoolToString(const C_OscNodeDataPool::E_Type & ore_DataPool);
   static int32_t h_StringToDataPool(const stw::scl::C_SclString & orc_String, C_OscNodeDataPool::E_Type & ore_Type);

   static void h_SaveDataPoolElementType(const C_OscNodeDataPoolContent & orc_NodeDataPoolContent,
                                         C_OscXmlParserBase & orc_XmlParser);
   static void h_SaveDataPoolElementValue(const stw::scl::C_SclString & orc_NodeName,
                                          const C_OscNodeDataPoolContent & orc_NodeDataPoolContent,
                                          C_OscXmlParserBase & orc_XmlParser);
   static void h_SaveDataPoolContentV1(const C_OscNodeDataPoolContent & orc_NodeDataPoolContent,
                                       C_OscXmlParserBase & orc_XmlParser);

   //Misc
   static stw::scl::C_SclString h_GetFileName(const stw::scl::C_SclString & orc_DatapoolName);
   static int32_t h_CheckDataPoolElementValueType(const C_OscNodeDataPoolContent::E_Type oe_ContentType,
                                                  const C_OscXmlParserBase & orc_XmlParser,
                                                  stw::scl::C_SclString * const opc_CheckDataTypeErrorDetails = NULL);

private:
   static stw::scl::C_SclString mh_NodeDataPoolContentToString(
      const C_OscNodeDataPoolContent::E_Type & ore_NodeDataPoolContent);
   static int32_t mh_StringToNodeDataPoolContent(const stw::scl::C_SclString & orc_String,
                                                 C_OscNodeDataPoolContent::E_Type & ore_Type);
   static stw::scl::C_SclString mh_NodeDataPoolElementAccessToString(
      const C_OscNodeDataPoolListElement::E_Access & ore_NodeDataPoolElementAccess);
   static int32_t mh_StringToNodeDataPoolElementAccess(const stw::scl::C_SclString & orc_String,
                                                       C_OscNodeDataPoolListElement::E_Access & ore_Type);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
