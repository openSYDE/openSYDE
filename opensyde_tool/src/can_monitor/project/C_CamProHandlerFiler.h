//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Project data file handler for can monitor (header)

   See cpp file for detailed description

   \copyright   Copyright 2018 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_CAMPROHANDLERFILER_H
#define C_CAMPROHANDLERFILER_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "stwtypes.h"
#include "C_OSCXMLParser.h"
#include "C_CamProHandler.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw_opensyde_gui_logic
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_CamProHandlerFiler
{
public:
   static void h_Save(const C_CamProHandler & orc_Handler, stw_opensyde_core::C_OSCXMLParserBase & orc_XMLParser);
   static stw_types::sint32 h_Load(C_CamProHandler & orc_Handler,
                                   stw_opensyde_core::C_OSCXMLParserBase & orc_XMLParser);

   static void h_SaveMessages(const std::vector<C_CamProMessageData> & orc_Messages,
                              stw_opensyde_core::C_OSCXMLParserBase & orc_XMLParser);
   static void h_SaveMessage(const C_CamProMessageData & orc_Message,
                             stw_opensyde_core::C_OSCXMLParserBase & orc_XMLParser);
   static stw_types::sint32 h_LoadMessages(std::vector<C_CamProMessageData> & orc_Messages,
                                           stw_opensyde_core::C_OSCXMLParserBase & orc_XMLParser);
   static stw_types::sint32 h_LoadMessage(C_CamProMessageData & orc_Message,
                                          stw_opensyde_core::C_OSCXMLParserBase & orc_XMLParser);

   static void h_SaveSettings(const C_CamProHandler & orc_Handler,
                              stw_opensyde_core::C_OSCXMLParserBase & orc_XMLParser);
   static stw_types::sint32 h_LoadSettings(C_CamProHandler & orc_Handler,
                                           stw_opensyde_core::C_OSCXMLParserBase & orc_XMLParser);

   static void h_SaveFilters(const std::vector<C_CamProFilterData> & orc_Filters,
                             stw_opensyde_core::C_OSCXMLParserBase & orc_XMLParser);
   static void h_SaveFilter(const C_CamProFilterData & orc_Filter,
                            stw_opensyde_core::C_OSCXMLParserBase & orc_XMLParser);
   static void h_SaveFilterItem(const C_CamProFilterItemData & orc_FilterItem,
                                stw_opensyde_core::C_OSCXMLParserBase & orc_XMLParser);
   static stw_types::sint32 h_LoadFilters(std::vector<C_CamProFilterData> & orc_Filters,
                                          stw_opensyde_core::C_OSCXMLParserBase & orc_XMLParser);
   static stw_types::sint32 h_LoadFilter(C_CamProFilterData & orc_Filter,
                                         stw_opensyde_core::C_OSCXMLParserBase & orc_XMLParser);
   static stw_types::sint32 h_LoadFilterItem(C_CamProFilterItemData & orc_FilterItem,
                                             const stw_opensyde_core::C_OSCXMLParserBase & orc_XMLParser);

   static void h_SaveDatabases(const std::vector<C_CamProDatabaseData> & orc_Databases,
                               stw_opensyde_core::C_OSCXMLParserBase & orc_XMLParser);
   static void h_SaveDatabase(const C_CamProDatabaseData & orc_Database,
                              stw_opensyde_core::C_OSCXMLParserBase & orc_XMLParser);
   static stw_types::sint32 h_LoadDatabases(std::vector<C_CamProDatabaseData> & orc_Databases,
                                            stw_opensyde_core::C_OSCXMLParserBase & orc_XMLParser);
   static stw_types::sint32 h_LoadDatabase(C_CamProDatabaseData & orc_Database,
                                           stw_opensyde_core::C_OSCXMLParserBase & orc_XMLParser);

   static void h_SaveLoggingConfig(const C_CamProLoggingData & orc_LoggingData,
                                   stw_opensyde_core::C_OSCXMLParserBase & orc_XMLParser);
   static stw_types::sint32 h_LoadLoggingConfig(C_CamProLoggingData & orc_LoggingData,
                                                stw_opensyde_core::C_OSCXMLParserBase & orc_XMLParser);

private:
   C_CamProHandlerFiler(void);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif
