//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Export communication stack settings of an openSYDE node.

   See cpp file for detailed description

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_OSCEXPORTCOMMUNICATIONSTACKH
#define C_OSCEXPORTCOMMUNICATIONSTACKH

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <vector>
#include <map>

#include "stwtypes.hpp"

#include "C_SclStringList.hpp"
#include "C_OscNode.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_core
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_OscExportCommunicationStack
{
public:
   static stw::scl::C_SclString h_GetFileName(const uint8_t ou8_InterfaceIndex,
                                              const C_OscCanProtocol::E_Type & ore_ProtocolType);
   static stw::scl::C_SclString h_GetConfigurationName(const uint8_t ou8_InterfaceIndex,
                                                       const C_OscCanProtocol::E_Type & ore_ProtocolType);
   static int32_t h_CreateSourceCode(const stw::scl::C_SclString & orc_Path, const C_OscNode & orc_Node,
                                     const uint16_t ou16_ApplicationIndex, const uint8_t ou8_InterfaceIndex,
                                     const uint32_t ou32_DatapoolIndex, const C_OscCanProtocol::E_Type & ore_Protocol,
                                     const stw::scl::C_SclString & orc_ExportToolInfo = "");
   static uint16_t h_ConvertOverallCodeVersion(const uint16_t ou16_GenCodeVersion);

protected:
   static const bool mhq_IS_HEADER_FILE = false;
   static const bool mhq_IS_IMPLEMENTATION_FILE = true;

   static int32_t mh_CreateHeaderFile(const stw::scl::C_SclString & orc_ExportToolInfo,
                                      const stw::scl::C_SclString & orc_Path,
                                      const C_OscNodeApplication & orc_Applicaton,
                                      const C_OscCanProtocol & orc_ComProtocol, const uint8_t ou8_InterfaceIndex,
                                      const stw::scl::C_SclString & orc_ProjectId);
   static int32_t mh_CreateImplementationFile(const stw::scl::C_SclString & orc_ExportToolInfo,
                                              const stw::scl::C_SclString & orc_Path,
                                              const C_OscNodeApplication & orc_Applicaton,
                                              const C_OscCanProtocol & orc_ComProtocol,
                                              const C_OscNodeDataPool & orc_DataPool, const uint8_t ou8_InterfaceIndex,
                                              const stw::scl::C_SclString & orc_ProjectId);

   static void mh_AddHeader(const stw::scl::C_SclString & orc_ExportToolInfo, stw::scl::C_SclStringList & orc_Data,
                            const uint8_t ou8_InterfaceIndex, const C_OscCanProtocol::E_Type & ore_Protocol,
                            const bool oq_FileType);
   static void mh_AddCeIncludes(stw::scl::C_SclStringList & orc_Data, const C_OscNodeDataPool & orc_DataPool,
                                const uint8_t ou8_InterfaceIndex, const C_OscCanProtocol::E_Type & ore_Protocol,
                                const bool oq_NullRequired);
   static void mh_AddDefines(stw::scl::C_SclStringList & orc_Data, const C_OscCanMessageContainer & orc_ComMessage,
                             const uint8_t ou8_InterfaceIndex, const C_OscCanProtocol::E_Type & ore_Protocol,
                             const stw::scl::C_SclString & orc_ProjectId, const uint16_t ou16_GenCodeVersion,
                             const bool oq_FileType);
   static void mh_AddCeModuleGlobal(stw::scl::C_SclStringList & orc_Data, const bool oq_SafeData,
                                    const C_OscCanMessageContainer & orc_ComMessage, const uint8_t ou8_InterfaceIndex,
                                    const C_OscCanProtocol::E_Type & ore_Protocol, const uint16_t ou16_GenCodeVersion,
                                    const uint32_t ou32_TxListIndex, const uint32_t ou32_RxListIndex);
   static void mh_AddCeGlobalVariables(stw::scl::C_SclStringList & orc_Data,
                                       const stw::scl::C_SclString & orc_DataPoolName, const uint8_t ou8_InterfaceIndex,
                                       const C_OscCanProtocol::E_Type & ore_Protocol, const bool oq_TxMessagesPresent,
                                       const bool oq_RxMessagesPresent);
   static void mh_AddSignalDefinitions(stw::scl::C_SclStringList & orc_Data, const uint32_t ou32_SignalListIndex,
                                       const std::vector<C_OscCanMessage> & orc_Messages,
                                       const uint16_t ou16_GenCodeVersion);
   static void mh_AddMessageMuxDefinitions(stw::scl::C_SclStringList & orc_Data,
                                           const std::vector<C_OscCanMessage> & orc_Messages,
                                           const stw::scl::C_SclString & orc_TxRxString);
   static void mh_AddMessageDefinitions(stw::scl::C_SclStringList & orc_Data, const uint8_t ou8_InterfaceIndex,
                                        const C_OscCanProtocol::E_Type & ore_Protocol,
                                        const std::vector<C_OscCanMessage> & orc_Messages,
                                        const uint16_t ou16_GenCodeVersion, const bool oq_Tx);
   static uint32_t mh_CountMuxMessages(const std::vector<C_OscCanMessage> & orc_Messages);
   static void mh_ConvertSignalsToStrings(stw::scl::C_SclStringList & orc_Data,
                                          const std::vector<C_OscCanSignal> & orc_Signals,
                                          const uint32_t ou32_SignalListIndex, const bool oq_RemoveLastComma);
   static void mh_GroupSignalsByMuxValue(const C_OscCanMessage & orc_Message, const uint32_t ou32_MultiplexerIndex,
                                         std::map<int32_t,
                                                  std::vector<C_OscCanSignal> > & orc_MuxedSignalsPerValue,
                                         std::vector<C_OscCanSignal> & orc_NonMuxedSignals);
   static stw::scl::C_SclString mh_GetProtocolNameByType(const C_OscCanProtocol::E_Type & ore_Protocol);
   static stw::scl::C_SclString mh_GetByteOrderNameByType(const C_OscCanSignal::E_ByteOrderType & ore_ByteOrder);
   static stw::scl::C_SclString mh_GetTransmissionTriggerNameByType(
      const C_OscCanMessage::E_TxMethodType & ore_Trigger);
   static stw::scl::C_SclString mh_GetMagicName(const stw::scl::C_SclString & orc_ProjectId,
                                                const uint8_t ou8_InterfaceIndex,
                                                const C_OscCanProtocol::E_Type & ore_Protocol);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
