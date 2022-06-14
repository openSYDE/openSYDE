//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Export CANopen configuration for CANopen manager.

   See cpp file for detailed description
   \copyright   Copyright 2022 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_OSCEXPORTCANOPENCONFIG_H
#define C_OSCEXPORTCANOPENCONFIG_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "CSCLStringList.h"
#include "C_OSCNode.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw_opensyde_core
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_OSCExportCanOpenConciseEntry
{
public:
   C_OSCExportCanOpenConciseEntry(void);
   C_OSCExportCanOpenConciseEntry(const stw_types::uint16 ou16_Index, const stw_types::uint8 ou8_SubIndex,
                                  const stw_types::uint32 ou32_PayloadSize,
                                  const std::vector<stw_types::uint8> & orc_Payload,
                                  const stw_scl::C_SCLString & orc_Comment);
   virtual ~C_OSCExportCanOpenConciseEntry(void);

   void SetConciseEntry(const stw_types::uint16 ou16_Index, const stw_types::uint8 ou8_SubIndex,
                        const stw_types::uint32 ou32_PayloadSize, const std::vector<stw_types::uint8> & orc_Payload,
                        const stw_scl::C_SCLString & orc_Comment);

   stw_scl::C_SCLString GetConciseString(void) const;
   static stw_scl::C_SCLString h_GetNumOfEntriesString(const stw_types::uint32 ou32_NumOfEntries);
   static stw_types::uint32 h_GetConciseArraySize(std::vector<C_OSCExportCanOpenConciseEntry> & orc_ConciseEntries);

   stw_types::uint16 u16_Index;
   stw_types::uint8 u8_SubIndex;
   stw_types::uint32 u32_PayloadSize;
   std::vector<stw_types::uint8> c_Payload;
   stw_scl::C_SCLString c_Comment;
};

class C_OSCExportCanOpenConfig
{
public:
   static stw_scl::C_SCLString h_GetFileName(const stw_types::uint8 ou8_InterfaceIndex);
   static stw_types::uint16 h_ConvertOverallCodeVersion(const stw_types::uint16 ou16_GenCodeVersion);
   static stw_types::sint32 h_CreateSourceCode(const stw_scl::C_SCLString & orc_Path, const C_OSCNode & orc_Node,
                                               const stw_types::uint16 ou16_ApplicationIndex,
                                               const stw_types::uint8 ou8_InterfaceIndex,
                                               const stw_types::uint32 ou32_DatapoolIndex,
                                               const stw_scl::C_SCLString & orc_ExportToolInfo);

protected:
   static const bool mhq_IS_HEADER_FILE = false;
   static const bool mhq_IS_IMPLEMENTATION_FILE = true;

   static const stw_types::uint16 mhu16_OD_INDEX_RESTORE_PARAMETERS = 0x1011U;
   static const stw_types::uint16 mhu16_OD_INDEX_HEARTBEAT_CONSUMER = 0x1016U;
   static const stw_types::uint16 mhu16_OD_INDEX_HEARTBEAT_PRODUCER = 0x1017U;
   static const stw_types::uint16 mhu16_OD_INDEX_FIRST_RX_PDO = 0x1400U;
   static const stw_types::uint16 mhu16_OD_INDEX_FIRST_TX_PDO = 0x1800U;
   static const stw_types::uint16 mhu16_OD_INDEX_FIRST_RX_PDO_MAPPING = 0x1600U;
   static const stw_types::uint16 mhu16_OD_INDEX_FIRST_TX_PDO_MAPPING = 0x1A00U;

   static stw_types::sint32 mh_CreateHeaderFile(const stw_scl::C_SCLString & orc_ExportInfoTool,
                                                const stw_scl::C_SCLString & orc_Path, const C_OSCNode & orc_Node,
                                                const C_OSCCanMessageContainer & orc_MsgContainer,
                                                const C_OSCNodeApplication & orc_Application,
                                                const stw_types::uint8 ou8_InterfaceIndex,
                                                const stw_scl::C_SCLString & orc_ProjectId);

   static stw_types::sint32 mh_CreateImplementationFile(const stw_scl::C_SCLString & orc_ExportInfoTool,
                                                        const stw_scl::C_SCLString & orc_Path,
                                                        const C_OSCNode & orc_Node,
                                                        const C_OSCCanMessageContainer & orc_MsgContainer,
                                                        const C_OSCNodeApplication & orc_Application,
                                                        const C_OSCNodeDataPool & orc_Datapool,
                                                        const stw_types::uint8 ou8_InterfaceIndex,
                                                        const stw_scl::C_SCLString & orc_ProjectId);

   static void mh_AddHeader(const stw_scl::C_SCLString & orc_ExportToolInfo, stw_scl::C_SCLStringList & orc_Data,
                            const stw_types::uint8 ou8_InterfaceIndex, const bool oq_FileType);
   static void mh_AddIncludes(stw_scl::C_SCLStringList & orc_Data, const C_OSCNodeDataPool & orc_Datapool,
                              const stw_types::uint8 ou8_InterfaceIndex, const bool oq_NullRequired);
   static void mh_AddDefines(stw_scl::C_SCLStringList & orc_Data, const C_OSCNode & orc_Node,
                             const C_OSCCanMessageContainer & orc_MsgContainer,
                             const stw_types::uint8 ou8_InterfaceIndex, const stw_scl::C_SCLString & orc_ProjectId,
                             const stw_types::uint16 ou16_GenCodeVersion, const bool oq_FileType);
   static void mh_AddGlobalVariables(stw_scl::C_SCLStringList & orc_Data, const stw_scl::C_SCLString & orc_DatapoolName,
                                     const C_OSCNode & orc_Node, const C_OSCCanMessageContainer & orc_MsgContainer,
                                     const stw_types::uint8 ou8_InterfaceIndex,
                                     const C_OSCNodeDataPoolList & orc_DatapoolTxList,
                                     const C_OSCNodeDataPoolList & orc_DatapoolRxList,
                                     const stw_types::uint32 ou32_TxListIndex,
                                     const stw_types::uint32 ou32_RxListIndex);
   static void mh_AddSignalDefinitions(stw_scl::C_SCLStringList & orc_Data,
                                       const C_OSCNodeDataPoolList & orc_DatapoolList,
                                       const stw_types::uint32 ou32_SignalListIndex,
                                       const std::vector<C_OSCCanMessage> & orc_Messages);
   static void mh_AddPdoDefinitions(stw_scl::C_SCLStringList & orc_Data,
                                    const std::vector<C_OSCCanMessage> & orc_Messages,
                                    const stw_types::uint8 ou8_InterfaceIndex, const bool oq_IsTx,
                                    const bool oq_RemoveLastComma);
   static std::vector<stw_types::uint32> mh_AddDeviceSpecificConciseData(stw_scl::C_SCLStringList & orc_Data,
                                                                         const C_OSCNode & orc_Node,
                                                                         const C_OSCCanMessageContainer & orc_MsgContainer, const stw_types::uint8 ou8_InterfaceIndex);
   static void mh_AddDeviceSettings(stw_scl::C_SCLStringList & orc_Data,
                                    const std::vector<stw_types::uint32> & orc_ConcSizes,
                                    const C_OSCCanOpenManagerInfo & orc_ManInfo,
                                    const stw_types::uint8 ou8_InterfaceIndex, const bool oq_RemoveLastComma);
   static void mh_AddManagerConfig(stw_scl::C_SCLStringList & orc_Data, const stw_scl::C_SCLString & orc_DatapoolName,
                                   const C_OSCCanOpenManagerInfo & orc_ManInfo,
                                   const stw_types::uint32 ou32_ManInstance, const stw_types::uint8 ou8_InterfaceIndex,
                                   const bool oq_TxExists, const bool oq_RxExists);
   static void mh_CollectDeviceSpecificConciseData(std::vector<C_OSCExportCanOpenConciseEntry> & orc_ConciseEntries,
                                                   const C_OSCCanOpenManagerDeviceInfo & orc_DeviceInfo);
   static void mh_CollectPdoConciseData(std::vector<C_OSCExportCanOpenConciseEntry> & orc_ConciseEntries,
                                        const C_OSCCanOpenManagerDeviceInfo & orc_DeviceInfo,
                                        const stw_types::uint32 ou32_NodeIndex,
                                        const C_OSCCanMessageContainer & orc_MsgContainer, const bool oq_IsTx);
   static void mh_CreateManSettings(stw_scl::C_SCLStringList & orc_Data, const C_OSCCanOpenManagerInfo & orc_ManInfo);
   static void mh_CreatePdoConfig(stw_scl::C_SCLStringList & orc_Data, const stw_scl::C_SCLString & orc_DatapoolName,
                                  const stw_types::uint8 ou8_InterfaceIndex, const bool oq_TxExists,
                                  const bool oq_RxExists);
   static void mh_ConvertSignalsToStrings(stw_scl::C_SCLStringList & orc_Data,
                                          const C_OSCNodeDataPoolList & orc_DatapoolList,
                                          const std::vector<C_OSCCanSignal> & orc_Signals,
                                          const stw_types::uint32 ou32_SignalListIndex, const bool oq_RemoveLastComma);
   static stw_scl::C_SCLString mh_GetMagicName(const stw_scl::C_SCLString & orc_ProjectId,
                                               const stw_types::uint8 ou8_InterfaceIndex);
   static void mh_GetDefineNMTStart(stw_scl::C_SCLString & orc_String, const bool oq_StartDevices,
                                    const bool oq_NMTStartAll);
   static void mh_GetDefineNMTStop(stw_scl::C_SCLString & orc_String,
                                   const C_OSCCanOpenManagerInfo::E_NMTErrorBehaviourType oe_NMTBehavior);
   static void mh_GetRestoreValueComment(stw_scl::C_SCLString & orc_Comment, const stw_types::uint8 ou8_Value);
   static stw_types::uint32 mh_GetTotalNumOfPdoSignals(const C_OSCCanMessageContainer & orc_MsgContainer,
                                                       const bool oq_IsTx);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif
