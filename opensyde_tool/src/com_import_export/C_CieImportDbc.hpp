//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Importing data from Vector DBC file (implementation)

   cf. .cpp file header for details

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_CIEIMPORTDBCH
#define C_CIEIMPORTDBCH

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "C_SclString.hpp"
#include "C_SclStringList.hpp"
#include "stwtypes.hpp"
#include "C_CieConverter.hpp"
#include "C_OscNodeDataPoolContent.hpp"

#include "DBC.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_gui_logic
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_CieImportDbc
{
private:
   static const stw::scl::C_SclString mhc_SEND_TYPE;
   static const stw::scl::C_SclString mhc_CYCLE_TIME;
   static Vector::DBC::AttributeDefinition mhc_AttributeSendType;
   static stw::scl::C_SclString mhc_DefaultSendTypeValue;
   static const stw::scl::C_SclString mhc_INITIAL_VALUE;
   static float32_t mhf32_DefaultInitialValue;
   static bool mhq_DefaultValueDefined;
   static stw::scl::C_SclStringList mhc_WarningMessages; // global warnings e.g. why some messages could not be imported
   static stw::scl::C_SclString mhc_ErrorMessage;        // description of error which caused the import to fail

   static int32_t mh_ReadFile(const stw::scl::C_SclString & orc_File, Vector::DBC::Network & orc_Network);
   static void mh_GetNode(const Vector::DBC::Node & orc_DbcNode, C_CieConverter::C_CieNode & orc_Node);
   static int32_t mh_GetMessage(const Vector::DBC::Message & orc_DbcMessage, C_CieConverter::C_CieNode & orc_Node);
   static int32_t mh_PrepareMessage(const Vector::DBC::Message & orc_DbcMessage,
                                    C_CieConverter::C_CieNodeMessage & orc_Message);
   static int32_t mh_ConvertAndAddMessage(const Vector::DBC::Message & orc_DbcMessage,
                                          std::vector<C_CieConverter::C_CieNodeMessage> & orc_Messages);
   static int32_t mh_GetSignal(const Vector::DBC::Signal & orc_DbcSignal, bool & orq_SignalAdapted,
                               C_CieConverter::C_CieNodeMessage & orc_Message);
   static void mh_VerifySignalValueTable(C_CieConverter::C_CieCanSignal & orc_DbcSignal);
   static int32_t mh_GetSignalValues(const Vector::DBC::Signal & orc_DbcSignal, const bool oq_MultiplexerSignal,
                                     bool & orq_SignalAdapted, C_CieConverter::C_CieDataPoolElement & orc_Element,
                                     stw::scl::C_SclStringList & orc_WarningMessages);
   static int32_t mh_GetAttributeDefinitions(const Vector::DBC::Network & orc_DbcNetwork);
   static void mh_GetTransmission(const Vector::DBC::Message & orc_DbcMessage,
                                  C_CieConverter::C_CieNodeMessage & orc_Message);
   static int32_t mh_CheckRange(const float64_t of64_Value,
                                const stw::opensyde_core::C_OscNodeDataPoolContent::E_Type oe_Datatype);
   static stw::scl::C_SclString mh_ReEscapeCriticalSymbols(const stw::scl::C_SclString & orc_String);

public:
   static int32_t h_ImportNetwork(const stw::scl::C_SclString & orc_File,
                                  C_CieConverter::C_CieCommDefinition & orc_Definition,
                                  stw::scl::C_SclStringList & orc_WarningMessages,
                                  stw::scl::C_SclString & orc_ErrorMessage, const bool oq_AddUnmappedMessages);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
