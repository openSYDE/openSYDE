//-----------------------------------------------------------------------------
/*!
   \file
   \brief       Export data to Vector DBC file (header)

   See cpp file for detailed description

   \implementation
   project     openSYDE
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     27.04.2018  STW/D.Pohl
   \endimplementation
*/
//-----------------------------------------------------------------------------
#ifndef C_CIEEXPORTDBC_H
#define C_CIEEXPORTDBC_H

/* -- Includes ------------------------------------------------------------- */
#include "stwtypes.h"
#include "stwerrors.h"
#include "CSCLString.h"
#include "CSCLStringList.h"
#include "C_CieConverter.h"
#include "C_OSCNodeDataPoolContent.h"

/* -- Namespace ------------------------------------------------------------ */
namespace stw_opensyde_gui_logic
{
/* -- Global Constants ----------------------------------------------------- */

/* -- Types ---------------------------------------------------------------- */

class C_CieExportDbc
{
public:
   static stw_types::sint32 h_ExportNetwork(const stw_scl::C_SCLString & orc_File,
                                            const C_CieConverter::C_CIECommDefinition & orc_Definition,
                                            stw_scl::C_SCLStringList & orc_WarningMessages,
                                            stw_scl::C_SCLString & orc_ErrorMessage);

   static stw_types::sint32 h_GetNodeMapping(std::map<stw_scl::C_SCLString, stw_scl::C_SCLString> & orc_NodeMapping);

   class C_ExportStatistic
   {
   public:
      stw_types::uint32 u32_NumOfMessages;
      stw_types::uint32 u32_NumOfSignals;
   };

   static stw_types::sint32 h_GetExportStatistic(C_ExportStatistic & orc_ExportStatistic);

private:
   static stw_scl::C_SCLStringList mhc_WarningMessages; // global warnings e.g. why some messages could not be exported
   static stw_scl::C_SCLString mhc_ErrorMessage;        // description of error which caused the export to fail

   static stw_types::sint32 mh_SetNodes(const std::vector<C_CieConverter::C_CIENode> & orc_CIENodes,
                                        std::map<std::string, Vector::DBC::Node> & orc_DBCNodes);
   static stw_types::sint32 mh_SetMessages(const std::vector<C_CieConverter::C_CIENode> & orc_CIENodes,
                                           std::map<stw_types::uintn, Vector::DBC::Message> & orc_DBCMessages);
   static stw_types::sint32 mh_SetSignals(const std::vector<C_CieConverter::C_CIECanSignal> & orc_CIESignals,
                                          const std::vector<C_CieConverter::C_CIENode> & orc_CIENodes,
                                          Vector::DBC::Message & orc_DBCMessage);
   static stw_types::sint32 mh_SetSignalValues(const C_CieConverter::C_CIEDataPoolElement & orc_Element,
                                               Vector::DBC::Signal & orc_DbcSignal);
   static stw_types::sint32 mh_SetTransmission(const C_CieConverter::C_CIENodeMessage & orc_Message,
                                               Vector::DBC::Message & orc_DbcMessage);
   static stw_types::sint32 mh_CheckDbcFileStatus(const Vector::DBC::Status & orc_Status);
   static void mh_SetNewSymbols(std::list<std::string> & orc_NewSymbols);
   static void mh_SetAttributeDefaults(std::map<std::string, Vector::DBC::Attribute> & orc_AttributeDefaults);
   static void mh_SetAttributeDefinitions(std::map<std::string,
                                                   Vector::DBC::AttributeDefinition> & orc_AttributeDefinitions);
   static stw_scl::C_SCLString mh_NiceifyStringForDbcSymbol(const stw_scl::C_SCLString & orc_String);
   static stw_scl::C_SCLString mh_EscapeCriticalSymbols(const stw_scl::C_SCLString & orc_String);


   static const std::string mhc_SigInitialValue;
   static const std::string mhc_MsgCycleTime;
   static const std::string mhc_MsgSendType;
   static std::map<stw_scl::C_SCLString, stw_scl::C_SCLString> mhc_NodeMapping;
   static bool mhq_ValidDbcExport;
   static C_ExportStatistic mhc_ExportStatistic;
};

/* -- Extern Global Variables ---------------------------------------------- */
} //end of namespace

#endif
