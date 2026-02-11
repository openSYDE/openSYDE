//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Helper class to check trigger expressions
   \copyright   Copyright 2025 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_SDNDEDALTRIGGERCHECKHELPER_HPP
#define C_SDNDEDALTRIGGERCHECKHELPER_HPP

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <string>

#include <QString>

#include "expression.hpp"
#include <data/monitor/lexer.hpp>

#include "C_OscNode.hpp"
#include "C_SclString.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_gui_logic
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_SdNdeDalTriggerCheckHelper
{
public:
   static bool h_Check(const uint32_t ou32_NodeIndex, const uint32_t ou32_DataLoggerIndex,
                       const stw::scl::C_SclString & orc_Expression, std::string * const opc_ErrorDetails,
                       bool * const opq_AreVariablesValid, bool * const opq_IsSyntaxValid);

private:
   C_SdNdeDalTriggerCheckHelper();

   static bool mh_ParseTokens(const stw::scl::C_SclString & orc_Expression,
                              std::vector<data::monitor::Token> & orc_Tokens, std::string * const opc_ErrorDetails);
   static bool mh_ReplaceChannelTokens(std::vector<data::monitor::Token> & orc_Tokens,
                                       std::string * const opc_ErrorDetails);
   static data::monitor::ChannelDataContainer mh_InitializeDataContainer(void);
   static bool mh_CheckSyntax(const std::vector<data::monitor::Token> & orc_Tokens,
                              std::string * const opc_ErrorDetails);
   static bool mh_CheckTokens(const uint32_t ou32_NodeIndex, const uint32_t ou32_DataLoggerIndex,
                              const std::vector<data::monitor::Token> & orc_Tokens,
                              std::string * const opc_ErrorDetails);
   static bool mh_CheckChannelDataLogger(const uint32_t ou32_NodeIndex, const uint32_t ou32_DataLoggerIndex,
                                         const std::vector<std::string> & orc_ChannelComponents,
                                         std::string * const opc_ErrorDetails);
   static QString mh_GetNamespace(const std::vector<std::string> & orc_ChannelComponents);
   static bool mh_CheckOnlyNumbers(const std::string & orc_String);
   static bool mh_CheckChannel(const uint32_t ou32_NodeIndex, const std::vector<std::string> & orc_ChannelComponents,
                               std::string * const opc_ErrorDetails);
   static bool mh_CheckChannelDatapool(const stw::opensyde_core::C_OscNode & orc_Node,
                                       const std::vector<std::string> & orc_ChannelComponents,
                                       std::string * const opc_ErrorDetails);
   static bool mh_CheckChannelHalType(const stw::opensyde_core::C_OscNode & orc_Node,
                                      const stw::opensyde_core::C_OscNodeDataPool & orc_Datapool,
                                      const std::vector<std::string> & orc_ChannelComponents,
                                      std::string * const opc_ErrorDetails);
   static bool mh_ConvertHalListToType(const stw::scl::C_SclString & orc_ListName,
                                       stw::opensyde_core::C_OscHalcDefDomain::E_VariableSelector & ore_Type);
   static bool mh_CheckChannelList(const stw::opensyde_core::C_OscNodeDataPool & orc_Datapool,
                                   const std::vector<std::string> & orc_ChannelComponents,
                                   std::string * const opc_ErrorDetails);
   static bool mh_CheckChannelList(const stw::opensyde_core::C_OscNodeDataPool & orc_Datapool,
                                   const std::string & orc_ListName, const std::string & orc_ElementName,
                                   const bool oq_UseArrayIndex, const uint32_t ou32_ArrayIndex,
                                   std::string * const opc_ErrorDetails);
   static bool mh_CheckChannelElement(const stw::opensyde_core::C_OscNodeDataPoolList & orc_List,
                                      const std::string & orc_ElementName, const bool oq_UseArrayIndex,
                                      const uint32_t ou32_ArrayIndex, std::string * const opc_ErrorDetails);
   static bool mh_CheckChannelElementArray(const stw::opensyde_core::C_OscNodeDataPoolListElement & orc_Element,
                                           const uint32_t ou32_ArrayIndex, std::string * const opc_ErrorDetails);

   static const uint32_t mhu32_DUMMY_CHANNEL_ID;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
