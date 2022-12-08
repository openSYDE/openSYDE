//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Export CANopen configuration for CANopen manager.

   See cpp file for detailed description
   \copyright   Copyright 2022 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_OSCEXPORTCANOPENCONFIG_HPP
#define C_OSCEXPORTCANOPENCONFIG_HPP

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "C_SclStringList.hpp"
#include "C_OscNode.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_core
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_OscExportCanOpenConciseEntry
{
public:
   C_OscExportCanOpenConciseEntry(void);
   C_OscExportCanOpenConciseEntry(const uint16_t ou16_Index, const uint8_t ou8_SubIndex,
                                  const std::vector<uint8_t> & orc_Payload, const stw::scl::C_SclString & orc_Comment);
   virtual ~C_OscExportCanOpenConciseEntry(void);

   void SetConciseEntry(const uint16_t ou16_Index, const uint8_t ou8_SubIndex, const std::vector<uint8_t> & orc_Payload,
                        const stw::scl::C_SclString & orc_Comment);
   void SetConciseEntry(const uint16_t ou16_Index, const uint8_t ou8_SubIndex, const uint8_t ou8_Value,
                        const stw::scl::C_SclString & orc_Comment);
   void SetConciseEntry(const uint16_t ou16_Index, const uint8_t ou8_SubIndex, const uint16_t ou16_Value,
                        const stw::scl::C_SclString & orc_Comment);
   void SetConciseEntry(const uint16_t ou16_Index, const uint8_t ou8_SubIndex, const uint32_t ou32_Value,
                        const stw::scl::C_SclString & orc_Comment);

   stw::scl::C_SclString GetConciseString(void) const;
   static stw::scl::C_SclString h_GetNumOfEntriesString(const uint32_t ou32_NumOfEntries);
   static uint32_t h_GetConciseArraySize(std::vector<C_OscExportCanOpenConciseEntry> & orc_ConciseEntries);

   uint16_t u16_Index;
   uint8_t u8_SubIndex;
   std::vector<uint8_t> c_Payload;
   stw::scl::C_SclString c_Comment;
};

class C_OscExportCanOpenConfig
{
public:
   static stw::scl::C_SclString h_GetFileName(const uint8_t ou8_InterfaceIndex);
   static uint16_t h_ConvertOverallCodeVersion(const uint16_t ou16_GenCodeVersion);
   static int32_t h_CreateSourceCode(const stw::scl::C_SclString & orc_Path, const C_OscNode & orc_Node,
                                     const uint16_t ou16_ApplicationIndex, const uint8_t ou8_InterfaceIndex,
                                     const uint32_t ou32_DatapoolIndex,
                                     const stw::scl::C_SclString & orc_ExportToolInfo);

protected:
   static const bool mhq_IS_HEADER_FILE = false;
   static const bool mhq_IS_IMPLEMENTATION_FILE = true;

   static int32_t mh_CreateHeaderFile(const stw::scl::C_SclString & orc_ExportInfoTool,
                                      const stw::scl::C_SclString & orc_Path, const C_OscNode & orc_Node,
                                      const C_OscCanMessageContainer & orc_MsgContainer,
                                      const C_OscNodeApplication & orc_Application, const uint8_t ou8_InterfaceIndex,
                                      const stw::scl::C_SclString & orc_ProjectId);

   static int32_t mh_CreateImplementationFile(const stw::scl::C_SclString & orc_ExportInfoTool,
                                              const stw::scl::C_SclString & orc_Path, const C_OscNode & orc_Node,
                                              const C_OscCanMessageContainer & orc_MsgContainer,
                                              const C_OscNodeApplication & orc_Application,
                                              const C_OscNodeDataPool & orc_Datapool, const uint8_t ou8_InterfaceIndex,
                                              const stw::scl::C_SclString & orc_ProjectId);

   static void mh_AddHeader(const stw::scl::C_SclString & orc_ExportToolInfo, stw::scl::C_SclStringList & orc_Data,
                            const uint8_t ou8_InterfaceIndex, const bool oq_FileType);
   static void mh_AddIncludes(stw::scl::C_SclStringList & orc_Data, const C_OscNodeDataPool & orc_Datapool,
                              const uint8_t ou8_InterfaceIndex, const bool oq_IncludeDataPool);
   static void mh_AddDefines(stw::scl::C_SclStringList & orc_Data, const C_OscNode & orc_Node,
                             const C_OscCanMessageContainer & orc_MsgContainer, const uint8_t ou8_InterfaceIndex,
                             const stw::scl::C_SclString & orc_ProjectId, const uint16_t ou16_GenCodeVersion,
                             const bool oq_FileType);
   static void mh_AddGlobalVariables(stw::scl::C_SclStringList & orc_Data,
                                     const stw::scl::C_SclString & orc_DatapoolName, const C_OscNode & orc_Node,
                                     const C_OscCanMessageContainer & orc_MsgContainer,
                                     const uint8_t ou8_InterfaceIndex, const C_OscNodeDataPoolList & orc_DatapoolTxList,
                                     const C_OscNodeDataPoolList & orc_DatapoolRxList, const uint32_t ou32_TxListIndex,
                                     const uint32_t ou32_RxListIndex, const uint16_t ou16_GenCodeVersion);
   static void mh_AddSignalDefinitions(stw::scl::C_SclStringList & orc_Data,
                                       const C_OscNodeDataPoolList & orc_DatapoolList,
                                       const uint32_t ou32_SignalListIndex,
                                       const std::vector<C_OscCanMessage> & orc_Messages);
   static void mh_AddPdoDefinitions(stw::scl::C_SclStringList & orc_Data,
                                    const std::vector<C_OscCanMessage> & orc_Messages, const uint8_t ou8_InterfaceIndex,
                                    const bool oq_IsTx, const bool oq_RemoveLastComma);
   static std::vector<uint32_t> mh_AddDeviceSpecificConciseData(stw::scl::C_SclStringList & orc_Data,
                                                                const C_OscNode & orc_Node,
                                                                const C_OscCanMessageContainer & orc_MsgContainer,
                                                                const uint8_t ou8_InterfaceIndex);
   static void mh_AddDeviceSettings(stw::scl::C_SclStringList & orc_Data, const std::vector<uint32_t> & orc_ConcSizes,
                                    const C_OscCanOpenManagerInfo & orc_ManInfo, const uint8_t ou8_InterfaceIndex,
                                    const uint16_t ou16_GenCodeVersion);
   static void mh_AddManagerConfig(stw::scl::C_SclStringList & orc_Data, const stw::scl::C_SclString & orc_DatapoolName,
                                   const C_OscCanOpenManagerInfo & orc_ManInfo, const uint32_t ou32_ManInstance,
                                   const uint8_t ou8_InterfaceIndex, const bool oq_TxExists, const bool oq_RxExists,
                                   const bool oq_ReferenceDataPool, const uint16_t ou16_GenCodeVersion);
   static void mh_CollectDeviceSpecificConciseData(std::vector<C_OscExportCanOpenConciseEntry> & orc_ConciseEntries,
                                                   const C_OscCanOpenManagerDeviceInfo & orc_DeviceInfo,
                                                   const uint8_t ou8_ManagerId, const uint32_t ou32_CyclePeriod,
                                                   const uint32_t ou32_WindowLength);
   static void mh_CollectPdoConciseData(std::vector<C_OscExportCanOpenConciseEntry> & orc_ConciseEntries,
                                        const C_OscCanOpenManagerDeviceInfo & orc_DeviceInfo,
                                        const uint32_t ou32_NodeIndex,
                                        const C_OscCanMessageContainer & orc_MsgContainer, const bool oq_IsTx);
   static void mh_CreateManSettings(stw::scl::C_SclStringList & orc_Data, const C_OscCanOpenManagerInfo & orc_ManInfo,
                                    const uint16_t ou16_GenCodeVersion);
   static void mh_CreatePdoConfig(stw::scl::C_SclStringList & orc_Data, const stw::scl::C_SclString & orc_DatapoolName,
                                  const uint8_t ou8_InterfaceIndex, const bool oq_TxExists, const bool oq_RxExists,
                                  const bool oq_ReferenceDataPool);
   static void mh_ConvertSignalsToStrings(stw::scl::C_SclStringList & orc_Data,
                                          const C_OscNodeDataPoolList & orc_DatapoolList,
                                          const std::vector<C_OscCanSignal> & orc_Signals,
                                          const uint32_t ou32_SignalListIndex, const bool oq_RemoveLastComma);
   static stw::scl::C_SclString mh_GetMagicName(const stw::scl::C_SclString & orc_ProjectId,
                                                const uint8_t ou8_InterfaceIndex);
   static void mh_GetDefineNmlStart(stw::scl::C_SclString & orc_String, const bool oq_StartDevices,
                                    const bool oq_NmtStartAll);
   static void mh_GetDefineNmtStop(stw::scl::C_SclString & orc_String,
                                   const C_OscCanOpenManagerInfo::E_NmtErrorBehaviourType oe_NmtBehavior);
   static uint32_t mh_GetTotalNumOfPdoSignals(const C_OscCanMessageContainer & orc_MsgContainer, const bool oq_IsTx);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
