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
#include "stwtypes.hpp"
#include "C_OscXmlParser.hpp"
#include "C_CamProHandler.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_gui_logic
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_CamProHandlerFiler
{
public:
   static void h_Save(const C_CamProHandler & orc_Handler, stw::opensyde_core::C_OscXmlParserBase & orc_XmlParser);
   static int32_t h_Load(C_CamProHandler & orc_Handler, stw::opensyde_core::C_OscXmlParserBase & orc_XmlParser);

   static void h_SaveMessages(const std::vector<C_CamProMessageData> & orc_Messages,
                              stw::opensyde_core::C_OscXmlParserBase & orc_XmlParser);
   static void h_SaveMessage(const C_CamProMessageData & orc_Message,
                             stw::opensyde_core::C_OscXmlParserBase & orc_XmlParser);
   static int32_t h_LoadMessages(std::vector<C_CamProMessageData> & orc_Messages,
                                 stw::opensyde_core::C_OscXmlParserBase & orc_XmlParser);
   static int32_t h_LoadMessage(C_CamProMessageData & orc_Message,
                                stw::opensyde_core::C_OscXmlParserBase & orc_XmlParser);

   static void h_SaveSettings(const C_CamProHandler & orc_Handler,
                              stw::opensyde_core::C_OscXmlParserBase & orc_XmlParser);
   static int32_t h_LoadSettings(C_CamProHandler & orc_Handler, stw::opensyde_core::C_OscXmlParserBase & orc_XmlParser);

   static void h_SaveFilters(const std::vector<C_CamProFilterData> & orc_Filters,
                             stw::opensyde_core::C_OscXmlParserBase & orc_XmlParser);
   static void h_SaveFilter(const C_CamProFilterData & orc_Filter,
                            stw::opensyde_core::C_OscXmlParserBase & orc_XmlParser);
   static void h_SaveFilterItem(const C_CamProFilterItemData & orc_FilterItem,
                                stw::opensyde_core::C_OscXmlParserBase & orc_XmlParser);
   static int32_t h_LoadFilters(std::vector<C_CamProFilterData> & orc_Filters,
                                stw::opensyde_core::C_OscXmlParserBase & orc_XmlParser);
   static int32_t h_LoadFilter(C_CamProFilterData & orc_Filter, stw::opensyde_core::C_OscXmlParserBase & orc_XmlParser);
   static int32_t h_LoadFilterItem(C_CamProFilterItemData & orc_FilterItem,
                                   const stw::opensyde_core::C_OscXmlParserBase & orc_XmlParser);

   static void h_SaveDatabases(const std::vector<C_CamProDatabaseData> & orc_Databases,
                               stw::opensyde_core::C_OscXmlParserBase & orc_XmlParser);
   static void h_SaveDatabase(const C_CamProDatabaseData & orc_Database,
                              stw::opensyde_core::C_OscXmlParserBase & orc_XmlParser);
   static int32_t h_LoadDatabases(std::vector<C_CamProDatabaseData> & orc_Databases,
                                  stw::opensyde_core::C_OscXmlParserBase & orc_XmlParser);
   static int32_t h_LoadDatabase(C_CamProDatabaseData & orc_Database,
                                 stw::opensyde_core::C_OscXmlParserBase & orc_XmlParser);

   static void h_SaveLoggingConfig(const C_CamProLoggingData & orc_LoggingData,
                                   stw::opensyde_core::C_OscXmlParserBase & orc_XmlParser);
   static int32_t h_LoadLoggingConfig(C_CamProLoggingData & orc_LoggingData,
                                      stw::opensyde_core::C_OscXmlParserBase & orc_XmlParser);

private:
   C_CamProHandlerFiler(void);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
