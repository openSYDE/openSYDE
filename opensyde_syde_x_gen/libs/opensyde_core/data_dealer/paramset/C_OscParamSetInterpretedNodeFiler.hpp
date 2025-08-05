//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Interpreted parameter set file reader/writer (header)

   See cpp file for detailed description

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_OSCPARAMSETINTERPRETEDNODEFILER_HPP
#define C_OSCPARAMSETINTERPRETEDNODEFILER_HPP

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "C_OscParamSetFilerBase.hpp"
#include "C_OscParamSetInterpretedNode.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_core
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

///Interpreted parameter set file reader/writer
class C_OscParamSetInterpretedNodeFiler :
   public C_OscParamSetFilerBase
{
public:
   static int32_t h_LoadInterpretedNode(C_OscParamSetInterpretedNode & orc_Node, C_OscXmlParserBase & orc_XmlParser,
                                        bool & orq_MissingOptionalContent);
   static void h_SaveInterpretedNode(const C_OscParamSetInterpretedNode & orc_Node, C_OscXmlParserBase & orc_XmlParser);

private:
   C_OscParamSetInterpretedNodeFiler(void);

   static int32_t mh_LoadDataPools(std::vector<C_OscParamSetInterpretedDataPool> & orc_DataPools,
                                   const std::vector<C_OscParamSetDataPoolInfo> & orc_DataPoolInfos,
                                   C_OscXmlParserBase & orc_XmlParser);
   static void mh_SaveDataPools(const std::vector<C_OscParamSetInterpretedDataPool> & orc_DataPools,
                                C_OscXmlParserBase & orc_XmlParser);
   static int32_t mh_LoadDataPool(C_OscParamSetInterpretedDataPool & orc_DataPool, C_OscXmlParserBase & orc_XmlParser);
   static void mh_SaveDataPool(const C_OscParamSetInterpretedDataPool & orc_DataPool,
                               C_OscXmlParserBase & orc_XmlParser);

   static int32_t mh_LoadLists(std::vector<C_OscParamSetInterpretedList> & orc_Lists,
                               C_OscXmlParserBase & orc_XmlParser);
   static void mh_SaveLists(const std::vector<C_OscParamSetInterpretedList> & orc_Lists,
                            C_OscXmlParserBase & orc_XmlParser);
   static int32_t mh_LoadList(C_OscParamSetInterpretedList & orc_List, C_OscXmlParserBase & orc_XmlParser);
   static void mh_SaveList(const C_OscParamSetInterpretedList & orc_List, C_OscXmlParserBase & orc_XmlParser);

   static int32_t mh_LoadElements(std::vector<C_OscParamSetInterpretedElement> & orc_Elements,
                                  C_OscXmlParserBase & orc_XmlParser);
   static void mh_SaveElements(const std::vector<C_OscParamSetInterpretedElement> & orc_Elements,
                               C_OscXmlParserBase & orc_XmlParser);
   static int32_t mh_LoadElement(C_OscParamSetInterpretedElement & orc_Element, C_OscXmlParserBase & orc_XmlParser);
   static void mh_SaveElement(const C_OscParamSetInterpretedElement & orc_Element, C_OscXmlParserBase & orc_XmlParser);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
