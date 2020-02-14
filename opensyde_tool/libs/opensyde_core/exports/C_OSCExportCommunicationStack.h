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

#include "stwtypes.h"

#include "CSCLStringList.h"
#include "C_OSCNode.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw_opensyde_core
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_OSCExportCommunicationStack
{
public:
   C_OSCExportCommunicationStack(void);
   virtual ~C_OSCExportCommunicationStack(void);

   static stw_scl::C_SCLString h_GetFileName(const stw_types::uint8 ou8_InterfaceIndex,
                                             const C_OSCCanProtocol::E_Type & ore_ProtocolType);
   static stw_scl::C_SCLString h_GetConfigurationName(const stw_types::uint8 ou8_InterfaceIndex,
                                                      const C_OSCCanProtocol::E_Type & ore_ProtocolType);
   static stw_types::sint32 h_CreateSourceCode(const stw_scl::C_SCLString & orc_Path, const C_OSCNode & orc_Node,
                                               const stw_types::uint16 ou16_ApplicationIndex,
                                               const stw_types::uint8 ou8_InterfaceIndex,
                                               const stw_types::uint32 ou32_DatapoolIndex,
                                               const C_OSCCanProtocol::E_Type & ore_Protocol,
                                               const stw_scl::C_SCLString & orc_ExportToolInfo = "");
   static stw_types::uint16 h_ConvertOverallCodeVersion(const stw_types::uint16 ou16_GenCodeVersion);

protected:
   static const bool mhq_IS_HEADER_FILE = false;
   static const bool mhq_IS_IMPLEMENTATION_FILE = true;

   static stw_types::sint32 mh_CreateHeaderFile(const stw_scl::C_SCLString & orc_ExportToolInfo,
                                                const stw_scl::C_SCLString & orc_Path,
                                                const C_OSCNodeApplication & orc_Applicaton,
                                                const C_OSCCanProtocol & orc_ComProtocol,
                                                const stw_types::uint8 ou8_InterfaceIndex,
                                                const stw_scl::C_SCLString & orc_ProjectId);
   static stw_types::sint32 mh_CreateImplementationFile(const stw_scl::C_SCLString & orc_ExportToolInfo,
                                                        const stw_scl::C_SCLString & orc_Path,
                                                        const C_OSCNodeApplication & orc_Applicaton,
                                                        const C_OSCCanProtocol & orc_ComProtocol,
                                                        const C_OSCNodeDataPool & orc_DataPool,
                                                        const stw_types::uint8 ou8_InterfaceIndex,
                                                        const stw_scl::C_SCLString & orc_ProjectId);

   static void mh_AddHeader(const stw_scl::C_SCLString & orc_ExportToolInfo, stw_scl::C_SCLStringList & orc_Data,
                            const stw_types::uint8 ou8_InterfaceIndex, const C_OSCCanProtocol::E_Type & ore_Protocol,
                            const bool oq_FileType);
   static void mh_AddCIncludes(stw_scl::C_SCLStringList & orc_Data, const stw_scl::C_SCLString & orc_DataPoolName,
                               const stw_types::uint8 ou8_InterfaceIndex, const C_OSCCanProtocol::E_Type & ore_Protocol,
                               const bool oq_NullRequired);
   static void mh_AddDefines(stw_scl::C_SCLStringList & orc_Data, const C_OSCCanMessageContainer & orc_ComMessage,
                             const stw_types::uint8 ou8_InterfaceIndex, const C_OSCCanProtocol::E_Type & ore_Protocol,
                             const stw_scl::C_SCLString & orc_ProjectId, const stw_types::uint16 ou16_GenCodeVersion,
                             const bool oq_FileType);
   static void mh_AddCModuleGlobal(stw_scl::C_SCLStringList & orc_Data, const bool oq_SafeData,
                                   const C_OSCCanMessageContainer & orc_ComMessage,
                                   const stw_types::uint8 ou8_InterfaceIndex,
                                   const C_OSCCanProtocol::E_Type & ore_Protocol,
                                   const stw_types::uint16 ou16_GenCodeVersion,
                                   const stw_types::uint32 ou32_TxListIndex, const stw_types::uint32 ou32_RxListIndex);
   static void mh_AddCGlobalVariables(stw_scl::C_SCLStringList & orc_Data,
                                      const stw_scl::C_SCLString & orc_DataPoolName,
                                      const stw_types::uint8 ou8_InterfaceIndex,
                                      const C_OSCCanProtocol::E_Type & ore_Protocol, const bool oq_TxMessagesPresent,
                                      const bool oq_RxMessagesPresent);
   static void mh_AddSignalDefinitions(stw_scl::C_SCLStringList & orc_Data,
                                       const stw_types::uint32 ou32_SignalListIndex,
                                       const std::vector<C_OSCCanMessage> & orc_Messages,
                                       const stw_types::uint16 ou16_GenCodeVersion);
   static void mh_AddMessageMuxDefinitions(stw_scl::C_SCLStringList & orc_Data,
                                           const std::vector<C_OSCCanMessage> & orc_Messages,
                                           const stw_scl::C_SCLString & orc_TxRxString);
   static void mh_AddMessageDefinitions(stw_scl::C_SCLStringList & orc_Data, const stw_types::uint8 ou8_InterfaceIndex,
                                        const C_OSCCanProtocol::E_Type & ore_Protocol,
                                        const std::vector<C_OSCCanMessage> & orc_Messages,
                                        const stw_types::uint16 ou16_GenCodeVersion, const bool oq_Tx);
   static stw_types::uint32 mh_CountMuxMessages(const std::vector<C_OSCCanMessage> & orc_Messages);
   static void mh_ConvertSignalsToStrings(stw_scl::C_SCLStringList & orc_Data,
                                          const std::vector<C_OSCCanSignal> & orc_Signals,
                                          const stw_types::uint32 ou32_SignalListIndex, const bool oq_RemoveLastComma);
   static void mh_GroupSignalsByMuxValue(const C_OSCCanMessage & orc_Message,
                                         const stw_types::uint32 u32_MultiplexerIndex, std::map<stw_types::sint32,
                                                                                                std::vector<C_OSCCanSignal> > & orc_SignalsPerValue);
   static stw_scl::C_SCLString mh_GetProtocolNameByType(const C_OSCCanProtocol::E_Type & ore_Protocol);
   static stw_scl::C_SCLString mh_GetByteOrderNameByType(const C_OSCCanSignal::E_ByteOrderType & ore_ByteOrder);
   static stw_scl::C_SCLString mh_GetTransmissionTriggerNameByType(const C_OSCCanMessage::E_TxMethodType & ore_Trigger);
   static stw_scl::C_SCLString mh_GetMagicName(const stw_scl::C_SCLString & orc_ProjectId,
                                               const stw_types::uint8 ou8_InterfaceIndex,
                                               const C_OSCCanProtocol::E_Type & ore_Protocol);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif
