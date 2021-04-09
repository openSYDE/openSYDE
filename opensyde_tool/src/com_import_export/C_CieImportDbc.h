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
#include "CSCLString.h"
#include "CSCLStringList.h"
#include "stwtypes.h"
#include "C_CieConverter.h"
#include "C_OSCNodeDataPoolContent.h"

#include "DBC.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw_opensyde_gui_logic
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_CieImportDbc
{
private:
   static const stw_scl::C_SCLString mhc_SendType;
   static const stw_scl::C_SCLString mhc_CycleTime;
   static Vector::DBC::AttributeDefinition mhc_AttributeSendType;
   static stw_scl::C_SCLString mhc_DefaultSendTypeValue;
   static const stw_scl::C_SCLString mhc_InitialValue;
   static stw_types::float32 mhf32_DefaultInitialValue;
   static bool mhq_DefaultValueDefined;
   static stw_scl::C_SCLStringList mhc_WarningMessages; // global warnings e.g. why some messages could not be imported
   static stw_scl::C_SCLString mhc_ErrorMessage;        // description of error which caused the import to fail

   static stw_types::sint32 mh_ReadFile(const stw_scl::C_SCLString & orc_File, Vector::DBC::Network & orc_Network);
   static void mh_GetNode(const Vector::DBC::Node & orc_DbcNode, C_CieConverter::C_CIENode & orc_Node);
   static stw_types::sint32 mh_GetMessage(const Vector::DBC::Message & orc_DbcMessage,
                                          C_CieConverter::C_CIENode & orc_Node);
   static stw_types::sint32 mh_PrepareMessage(const Vector::DBC::Message & orc_DbcMessage,
                                              C_CieConverter::C_CIENodeMessage & orc_Message);
   static stw_types::sint32 mh_ConvertAndAddMessage(const Vector::DBC::Message & orc_DbcMessage,
                                                    std::vector<C_CieConverter::C_CIENodeMessage> & orc_Messages);
   static stw_types::sint32 mh_GetSignal(const Vector::DBC::Signal & orc_DbcSignal, bool & orq_SignalAdapted,
                                         C_CieConverter::C_CIENodeMessage & orc_Message);
   static void mh_VerifySignalValueTable(C_CieConverter::C_CIECanSignal & orc_DbcSignal);
   static stw_types::sint32 mh_GetSignalValues(const Vector::DBC::Signal & orc_DbcSignal,
                                               const bool oq_MultiplexerSignal, bool & orq_SignalAdapted,
                                               C_CieConverter::C_CIEDataPoolElement & orc_Element,
                                               stw_scl::C_SCLStringList & orc_WarningMessages);
   static stw_types::sint32 mh_GetAttributeDefinitions(const Vector::DBC::Network & orc_DbcNetwork);
   static void mh_GetTransmission(const Vector::DBC::Message & orc_DbcMessage,
                                  C_CieConverter::C_CIENodeMessage & orc_Message);
   static stw_types::sint32 mh_CheckRange(const stw_types::float64 of64_Value,
                                          const stw_opensyde_core::C_OSCNodeDataPoolContent::E_Type oe_Datatype);
   static stw_scl::C_SCLString mh_ReEscapeCriticalSymbols(const stw_scl::C_SCLString & orc_String);

public:
   static stw_types::sint32 h_ImportNetwork(const stw_scl::C_SCLString & orc_File,
                                            C_CieConverter::C_CIECommDefinition & orc_Definition,
                                            stw_scl::C_SCLStringList & orc_WarningMessages,
                                            stw_scl::C_SCLString & orc_ErrorMessage, const bool oq_AddUnmappedMessages);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif
