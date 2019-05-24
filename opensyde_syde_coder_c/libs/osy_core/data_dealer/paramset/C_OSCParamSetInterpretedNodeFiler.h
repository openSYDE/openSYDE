//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Interpreted parameter set file reader/writer (header)

   See cpp file for detailed description

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_OSCPARAMSETINTERPRETEDNODEFILER_H
#define C_OSCPARAMSETINTERPRETEDNODEFILER_H

/* -- Includes ------------------------------------------------------------- */
#include "C_OSCParamSetFilerBase.h"
#include "C_OSCParamSetInterpretedNode.h"

/* -- Namespace ------------------------------------------------------------ */
namespace stw_opensyde_core
{
/* -- Global Constants ----------------------------------------------------- */

/* -- Types ---------------------------------------------------------------- */

///Interpreted parameter set file reader/writer
class C_OSCParamSetInterpretedNodeFiler :
   public C_OSCParamSetFilerBase
{
public:
   static stw_types::sint32 h_LoadInterpretedNode(C_OSCParamSetInterpretedNode & orc_Node,
                                                  C_OSCXMLParserBase & orc_XMLParser);
   static void h_SaveInterpretedNode(const C_OSCParamSetInterpretedNode & orc_Node, C_OSCXMLParserBase & orc_XMLParser);

private:
   C_OSCParamSetInterpretedNodeFiler(void);

   static stw_types::sint32 mh_LoadDataPools(std::vector<C_OSCParamSetInterpretedDataPool> & orc_DataPools,
                                             const std::vector<C_OSCParamSetDataPoolInfo> & orc_DataPoolInfos,
                                             C_OSCXMLParserBase & orc_XMLParser);
   static void mh_SaveDataPools(const std::vector<C_OSCParamSetInterpretedDataPool> & orc_DataPools,
                                C_OSCXMLParserBase & orc_XMLParser);
   static stw_types::sint32 mh_LoadDataPool(C_OSCParamSetInterpretedDataPool & orc_DataPool,
                                            C_OSCXMLParserBase & orc_XMLParser);
   static void mh_SaveDataPool(const C_OSCParamSetInterpretedDataPool & orc_DataPool,
                               C_OSCXMLParserBase & orc_XMLParser);

   static stw_types::sint32 mh_LoadLists(std::vector<C_OSCParamSetInterpretedList> & orc_Lists,
                                         C_OSCXMLParserBase & orc_XMLParser);
   static void mh_SaveLists(const std::vector<C_OSCParamSetInterpretedList> & orc_Lists,
                            C_OSCXMLParserBase & orc_XMLParser);
   static stw_types::sint32 mh_LoadList(C_OSCParamSetInterpretedList & orc_List, C_OSCXMLParserBase & orc_XMLParser);
   static void mh_SaveList(const C_OSCParamSetInterpretedList & orc_List, C_OSCXMLParserBase & orc_XMLParser);

   static stw_types::sint32 mh_LoadElements(std::vector<C_OSCParamSetInterpretedElement> & orc_Elements,
                                            C_OSCXMLParserBase & orc_XMLParser);
   static void mh_SaveElements(const std::vector<C_OSCParamSetInterpretedElement> & orc_Elements,
                               C_OSCXMLParserBase & orc_XMLParser);
   static stw_types::sint32 mh_LoadElement(C_OSCParamSetInterpretedElement & orc_Element,
                                           C_OSCXMLParserBase & orc_XMLParser);
   static void mh_SaveElement(const C_OSCParamSetInterpretedElement & orc_Element, C_OSCXMLParserBase & orc_XMLParser);
};

/* -- Extern Global Variables ---------------------------------------------- */
} //end of namespace

#endif
