//-----------------------------------------------------------------------------
/*!
   \file
   \brief       Load/save system definition data from/to file

   (See .cpp file for full description)

   \implementation
   project     openSYDE
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     02.09.2016  STW/M.Echtler
   \endimplementation
*/
//-----------------------------------------------------------------------------
#ifndef C_OSCSYSTEMDEFINITIONFILERH
#define C_OSCSYSTEMDEFINITIONFILERH

/* -- Includes ------------------------------------------------------------- */
#include "stwtypes.h"
#include "CSCLString.h"
#include "C_OSCNode.h"
#include "C_OSCNodeFiler.h"
#include "C_OSCSystemBus.h"
#include "C_OSCSystemBusFiler.h"
#include "C_OSCSystemDefinition.h"
#include "C_OSCXMLParser.h"

/* -- Namespace ------------------------------------------------------------ */
namespace stw_opensyde_core
{
/* -- Global Constants ----------------------------------------------------- */

/* -- Types ---------------------------------------------------------------- */

class C_OSCSystemDefinitionFiler
{
public:
   static stw_types::sint32 h_LoadSystemDefinitionFile(C_OSCSystemDefinition & orc_SystemDefinition,
                                                       const stw_scl::C_SCLString & orc_PathSystemDefinition,
                                                       const stw_scl::C_SCLString & orc_PathDeviceDefinitions);
   static stw_types::sint32 h_SaveSystemDefinitionFile(const C_OSCSystemDefinition & orc_SystemDefinition,
                                                       const stw_scl::C_SCLString & orc_Path);
   static stw_types::sint32 h_LoadSystemDefinitionString(C_OSCSystemDefinition & orc_SystemDefinition,
                                                         const stw_scl::C_SCLString & orc_Content,
                                                         const stw_scl::C_SCLString & orc_PathDeviceDefinitions);
   static void h_SaveSystemDefinitionString(const C_OSCSystemDefinition & orc_SystemDefinition,
                                            stw_scl::C_SCLString & orc_Content);
   static stw_types::sint32 h_LoadNodes(const stw_types::uint16 ou16_XmlFormatVersion,
                                        std::vector<C_OSCNode> & orc_Nodes, C_OSCXMLParserBase & orc_XMLParser,
                                        const C_OSCDeviceManager & orc_DeviceDefinitions);
   static stw_types::sint32 h_LoadBuses(std::vector<C_OSCSystemBus> & orc_Buses, C_OSCXMLParserBase & orc_XMLParser);
   static void h_SaveNodes(const std::vector<C_OSCNode> & orc_Nodes, C_OSCXMLParserBase & orc_XMLParser);
   static void h_SaveBuses(const std::vector<C_OSCSystemBus> & orc_Buses, C_OSCXMLParserBase & orc_XMLParser);

   ///known file versions
   static const stw_types::uint16 hu16_FILE_VERSION_1 = 1U;
   static const stw_types::uint16 hu16_FILE_VERSION_2 = 2U;
   static const stw_types::uint16 hu16_FILE_VERSION_LATEST = hu16_FILE_VERSION_2;

private:
   static stw_types::sint32 mh_LoadSystemDefinition(C_OSCSystemDefinition & orc_SystemDefinition,
                                                    C_OSCXMLParserBase & orc_XMLParser,
                                                    const stw_scl::C_SCLString & orc_PathDeviceDefinitions);
   static void mh_SaveSystemDefinition(const C_OSCSystemDefinition & orc_SystemDefinition,
                                       C_OSCXMLParserBase & orc_XMLParser);
};

/* -- Extern Global Variables ---------------------------------------------- */
}

#endif
