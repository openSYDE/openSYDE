//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Export data to Vector DBC file (header)

   See cpp file for detailed description

   \copyright   Copyright 2018 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_CIEEXPORTDBC_H
#define C_CIEEXPORTDBC_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "stwtypes.hpp"
#include "stwerrors.hpp"
#include "C_SclString.hpp"
#include "C_SclStringList.hpp"
#include "C_CieConverter.hpp"
#include "C_OscNodeDataPoolContent.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_gui_logic
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_CieExportDbc
{
public:
   static int32_t h_ExportNetwork(const stw::scl::C_SclString & orc_File,
                                  const C_CieConverter::C_CieCommDefinition & orc_Definition,
                                  stw::scl::C_SclStringList & orc_WarningMessages,
                                  stw::scl::C_SclString & orc_ErrorMessage);

   static int32_t h_GetNodeMapping(std::map<stw::scl::C_SclString, stw::scl::C_SclString> & orc_NodeMapping);

   class C_ExportStatistic
   {
   public:
      uint32_t u32_NumOfMessages;
      uint32_t u32_NumOfSignals;
   };

   static int32_t h_GetExportStatistic(C_ExportStatistic & orc_ExportStatistic);

private:
   static stw::scl::C_SclStringList mhc_WarningMessages; // global warnings e.g. why some messages could not be exported
   static stw::scl::C_SclString mhc_ErrorMessage;        // description of error which caused the export to fail

   static int32_t mh_SetNodes(const std::vector<C_CieConverter::C_CieNode> & orc_CieNodes, std::map<std::string,
                                                                                                    Vector::DBC::Node> & orc_DbcNodes);
   static int32_t mh_SetMessages(const std::vector<C_CieConverter::C_CieNode> & orc_CieNodes, std::map<uint32_t,
                                                                                                       Vector::DBC::Message> & orc_DbcMessages);
   static int32_t mh_SetSignals(const std::vector<C_CieConverter::C_CieCanSignal> & orc_CieSignals,
                                const std::vector<C_CieConverter::C_CieNode> & orc_CieNodes,
                                Vector::DBC::Message & orc_DbcMessage);
   static int32_t mh_SetSignalValues(const C_CieConverter::C_CieDataPoolElement & orc_Element,
                                     Vector::DBC::Signal & orc_DbcSignal);
   static int32_t mh_SetTransmission(const C_CieConverter::C_CieNodeMessage & orc_Message,
                                     Vector::DBC::Message & orc_DbcMessage);
   static int32_t mh_CheckDbcFileStatus(const Vector::DBC::Status & ore_Status);
   static void mh_SetNewSymbols(std::list<std::string> & orc_NewSymbols);
   static void mh_SetAttributeDefaults(std::map<std::string, Vector::DBC::Attribute> & orc_AttributeDefaults);
   static void mh_SetAttributeDefinitions(std::map<std::string,
                                                   Vector::DBC::AttributeDefinition> & orc_AttributeDefinitions);
   static stw::scl::C_SclString mh_NiceifyStringForDbcSymbol(const stw::scl::C_SclString & orc_String);
   static stw::scl::C_SclString mh_EscapeCriticalSymbols(const stw::scl::C_SclString & orc_String);

   static const std::string mhc_SIG_INITIAL_VALUE;
   static const std::string mhc_MSG_CYCLE_TIME;
   static const std::string mhc_MSG_SEND_TYPE;
   static std::map<stw::scl::C_SclString, stw::scl::C_SclString> mhc_NodeMapping;
   static bool mhq_ValidDbcExport;
   static C_ExportStatistic mhc_ExportStatistic;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
