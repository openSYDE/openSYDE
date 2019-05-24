//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       UI topology filer (V2 format) (header)

   See cpp file for detailed description

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_PUISDHANDLERFILERV2_H
#define C_PUISDHANDLERFILERV2_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <QColor>
#include <QFont>
#include "C_OSCXMLParser.h"
#include "C_PuiSdNode.h"
#include "C_PuiSdBus.h"
#include "C_PuiBsTextElement.h"
#include "C_PuiSdTextElementBus.h"
#include "C_PuiBsBoundary.h"
#include "C_PuiBsImage.h"
#include "C_PuiBsLineArrow.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw_opensyde_gui_logic
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_PuiSdHandlerFilerV2
{
public:
   C_PuiSdHandlerFilerV2(void);

   //Data pool
   static stw_types::sint32 h_LoadDataPools(std::vector<C_PuiSdNodeDataPool> & orc_DataPools,
                                            stw_opensyde_core::C_OSCXMLParserBase & orc_XMLParser);
   static stw_types::sint32 h_LoadDataPool(C_PuiSdNodeDataPool & orc_DataPool,
                                           stw_opensyde_core::C_OSCXMLParserBase & orc_XMLParser);
   static stw_types::sint32 h_LoadDataPoolLists(std::vector<C_PuiSdNodeDataPoolList> & orc_DataPoolLists,
                                                stw_opensyde_core::C_OSCXMLParserBase & orc_XMLParser);
   static stw_types::sint32 h_LoadDataPoolList(C_PuiSdNodeDataPoolList & orc_DataPoolList,
                                               stw_opensyde_core::C_OSCXMLParserBase & orc_XMLParser);
   static stw_types::sint32 h_LoadDataPoolListElements(
      std::vector<C_PuiSdNodeDataPoolListElement> & orc_DataPoolListElements,
      stw_opensyde_core::C_OSCXMLParserBase & orc_XMLParser);
   static void h_LoadDataPoolListElement(C_PuiSdNodeDataPoolListElement & orc_DataPoolListElement,
                                         const stw_opensyde_core::C_OSCXMLParserBase & orc_XMLParser);
   static void h_SaveDataPools(const std::vector<C_PuiSdNodeDataPool> & orc_DataPools,
                               stw_opensyde_core::C_OSCXMLParserBase & orc_XMLParser);
   static void h_SaveDataPool(const C_PuiSdNodeDataPool & orc_DataPool,
                              stw_opensyde_core::C_OSCXMLParserBase & orc_XMLParser);
   static void h_SaveDataPoolLists(const std::vector<C_PuiSdNodeDataPoolList> & orc_DataPoolLists,
                                   stw_opensyde_core::C_OSCXMLParserBase & orc_XMLParser);
   static void h_SaveDataPoolList(const C_PuiSdNodeDataPoolList & orc_DataPoolList,
                                  stw_opensyde_core::C_OSCXMLParserBase & orc_XMLParser);
   static void h_SaveDataPoolListElements(const std::vector<C_PuiSdNodeDataPoolListElement> & orc_DataPoolListElements,
                                          stw_opensyde_core::C_OSCXMLParserBase & orc_XMLParser);
   static void h_SaveDataPoolListElement(const C_PuiSdNodeDataPoolListElement & orc_DataPoolListElement,
                                         stw_opensyde_core::C_OSCXMLParserBase & orc_XMLParser);

   //Com
   static stw_types::sint32 h_LoadCanProtocols(std::vector<C_PuiSdNodeCanProtocol> & orc_CanProtocols,
                                               stw_opensyde_core::C_OSCXMLParserBase & orc_XMLParser);
   static stw_types::sint32 h_LoadCanProtocol(C_PuiSdNodeCanProtocol & orc_CanProtocol,
                                              stw_opensyde_core::C_OSCXMLParserBase & orc_XMLParser);
   static stw_types::sint32 h_LoadCanMessageContainers(
      std::vector<C_PuiSdNodeCanMessageContainer> & orc_CanMessageContainers,
      stw_opensyde_core::C_OSCXMLParserBase & orc_XMLParser);
   static stw_types::sint32 h_LoadCanMessageContainer(C_PuiSdNodeCanMessageContainer & orc_CanMessageContainer,
                                                      stw_opensyde_core::C_OSCXMLParserBase & orc_XMLParser);
   static stw_types::sint32 h_LoadCanMessages(std::vector<C_PuiSdNodeCanMessage> & orc_CanMessages,
                                              stw_opensyde_core::C_OSCXMLParserBase & orc_XMLParser);
   static stw_types::sint32 h_LoadCanMessage(C_PuiSdNodeCanMessage & orc_CanMessage,
                                             stw_opensyde_core::C_OSCXMLParserBase & orc_XMLParser);
   static void h_LoadCanSignals(std::vector<C_PuiSdNodeCanSignal> & orc_CanSignals,
                                stw_opensyde_core::C_OSCXMLParserBase & orc_XMLParser);
   static void h_LoadCanSignal(C_PuiSdNodeCanSignal & orc_CanSignal,
                               const stw_opensyde_core::C_OSCXMLParserBase & orc_XMLParser);
   static void h_SaveCanProtocols(const std::vector<C_PuiSdNodeCanProtocol> & orc_CanProtocols,
                                  stw_opensyde_core::C_OSCXMLParserBase & orc_XMLParser);
   static void h_SaveCanProtocol(const C_PuiSdNodeCanProtocol & orc_CanProtocol,
                                 stw_opensyde_core::C_OSCXMLParserBase & orc_XMLParser);
   static void h_SaveCanMessageContainers(const std::vector<C_PuiSdNodeCanMessageContainer> & orc_CanMessageContainers,
                                          stw_opensyde_core::C_OSCXMLParserBase & orc_XMLParser);
   static void h_SaveCanMessageContainer(const C_PuiSdNodeCanMessageContainer & orc_CanMessageContainer,
                                         stw_opensyde_core::C_OSCXMLParserBase & orc_XMLParser);
   static void h_SaveCanMessages(const std::vector<C_PuiSdNodeCanMessage> & orc_CanMessages,
                                 stw_opensyde_core::C_OSCXMLParserBase & orc_XMLParser);
   static void h_SaveCanMessage(const C_PuiSdNodeCanMessage & orc_CanMessage,
                                stw_opensyde_core::C_OSCXMLParserBase & orc_XMLParser);
   static void h_SaveCanSignals(const std::vector<C_PuiSdNodeCanSignal> & orc_CanSignals,
                                stw_opensyde_core::C_OSCXMLParserBase & orc_XMLParser);
   static void h_SaveCanSignal(const C_PuiSdNodeCanSignal & orc_CanSignal,
                               stw_opensyde_core::C_OSCXMLParserBase & orc_XMLParser);

   // Else
   static stw_types::sint32 h_LoadNodes(std::vector<C_PuiSdNode> & orc_Nodes,
                                        stw_opensyde_core::C_OSCXMLParserBase & orc_XMLParser);
   static void h_SaveNodes(const std::vector<C_PuiSdNode> & orc_Nodes,
                           stw_opensyde_core::C_OSCXMLParserBase & orc_XMLParser);
   static stw_types::sint32 h_LoadBuses(std::vector<C_PuiSdBus> & orc_Buses,
                                        stw_opensyde_core::C_OSCXMLParserBase & orc_XMLParser);
   static void h_SaveBuses(const std::vector<C_PuiSdBus> & orc_Buses,
                           stw_opensyde_core::C_OSCXMLParserBase & orc_XMLParser);
   static stw_types::sint32 h_LoadBusTextElements(std::vector<C_PuiSdTextElementBus> & orc_BusTextElements,
                                                  stw_opensyde_core::C_OSCXMLParserBase & orc_XMLParser);
   static void h_SaveBusTextElements(const std::vector<C_PuiSdTextElementBus> & orc_BusTextElements,
                                     stw_opensyde_core::C_OSCXMLParserBase & orc_XMLParser);

private:
   static stw_types::sint32 mh_LoadNode(C_PuiSdNode & orc_Node, stw_opensyde_core::C_OSCXMLParserBase & orc_XMLParser);
   static void mh_SaveNode(const C_PuiSdNode & orc_Node, stw_opensyde_core::C_OSCXMLParserBase & orc_XMLParser);
   static stw_types::sint32 mh_LoadBus(C_PuiSdBus & orc_Bus, stw_opensyde_core::C_OSCXMLParserBase & orc_XMLParser);
   static void mh_SaveBus(const C_PuiSdBus & orc_Bus, stw_opensyde_core::C_OSCXMLParserBase & orc_XMLParser);
   static stw_types::sint32 mh_LoadTextElement(C_PuiBsTextElement * const opc_TextElement,
                                               stw_opensyde_core::C_OSCXMLParserBase & orc_XMLParser);
   static void mh_SaveTextElement(const C_PuiBsTextElement * const opc_TextElement,
                                  stw_opensyde_core::C_OSCXMLParserBase & orc_XMLParser);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif
