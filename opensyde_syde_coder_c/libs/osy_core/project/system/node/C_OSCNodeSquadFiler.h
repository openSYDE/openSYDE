//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       NodeSquad reader/writer
   \copyright   Copyright 2021 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_OSCNODESQUADFILER_H
#define C_OSCNODESQUADFILER_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "C_OSCNodeSquad.h"
#include "C_OSCXMLParser.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw_opensyde_core
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_OSCNodeSquadFiler
{
public:
   C_OSCNodeSquadFiler();

   static stw_types::sint32 h_LoadNodeGroups(std::vector<C_OSCNodeSquad> & orc_NodeGroups,
                                             C_OSCXMLParserBase & orc_XMLParser);
   static stw_types::sint32 h_LoadNodeGroup(C_OSCNodeSquad & orc_NodeGroup, C_OSCXMLParserBase & orc_XMLParser);
   static void h_SaveNodeGroups(const std::vector<C_OSCNodeSquad> & orc_NodeGroups, C_OSCXMLParserBase & orc_XMLParser);
   static void h_SaveNodeGroup(const C_OSCNodeSquad & orc_NodeGroup, C_OSCXMLParserBase & orc_XMLParser);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif
