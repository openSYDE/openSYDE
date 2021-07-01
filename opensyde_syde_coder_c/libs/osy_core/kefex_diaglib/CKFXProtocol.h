#ifndef CKFXPROTOCOLH
#define CKFXPROTOCOLH

#include <cstdio>

#include "DiagLib_config.h"
#include "stwtypes.h"
#include "CKFXCommConfiguration.h"
#include "TGLTasks.h"
#include "CCANDispatcher.h"
#ifndef DIAGLIB_KEFEX_PROTOCOL_NO_LOGGING
#include "CCMONProtocol.h" //for protocolling HLP to text file
#ifdef CMONPROTOCOL_ALLOW_RAMVIEW_PROJECT_MAPPING
#include "CKFXVariableLists.h" //for protocolling HLP to text file
#endif
#endif

//---------------------------------------------------------------------------

namespace stw_diag_lib
{
//---------------------------------------------------------------------------

const stw_types::uint8 KFX_PROTOCOL_SRR                          = 0x01U;
const stw_types::uint8 KFX_PROTOCOL_ECRR                         = 0x02U;
const stw_types::uint8 KFX_PROTOCOL_TCRR                         = 0x03U;
const stw_types::uint8 KFX_PROTOCOL_TCRR_TIMESTAMPED             = 0x04U;
const stw_types::uint8 KFX_PROTOCOL_ABORT_INDIVIDUAL_RESPONSE    = 0x0AU;
const stw_types::uint8 KFX_PROTOCOL_ABORT_INDIVIDUAL_RESPONSE_HS = 0x0BU;
const stw_types::uint8 KFX_PROTOCOL_ABORT_ALL_RESPONSES          = 0x0CU;
const stw_types::uint8 KFX_PROTOCOL_ABORT_ALL_RESPONSES_HS       = 0x0DU;
const stw_types::uint8 KFX_PROTOCOL_IMMEDIATE_WRITE_HS           = 0x0FU;
const stw_types::uint8 KFX_PROTOCOL_LOGON_HS                     = 0x11U;
const stw_types::uint8 KFX_PROTOCOL_LOGOFF_HS                    = 0x15U;
const stw_types::uint8 KFX_PROTOCOL_SERVICE_READ                 = 0x16U;
const stw_types::uint8 KFX_PROTOCOL_SERVICE_WRITE                = 0x17U;
const stw_types::uint8 KFX_PROTOCOL_STARTUP_INDICATION           = 0x1AU;
const stw_types::uint8 KFX_PROTOCOL_READ_EEPROM                  = 0x20U;
const stw_types::uint8 KFX_PROTOCOL_READ_FLASH_EEPROM            = 0x21U;
const stw_types::uint8 KFX_PROTOCOL_READ_SRAM                    = 0x22U;
const stw_types::uint8 KFX_PROTOCOL_READ_EPROM                   = 0x23U;
const stw_types::uint8 KFX_PROTOCOL_WRITE_EEPROM_HS              = 0x27U;
const stw_types::uint8 KFX_PROTOCOL_WRITE_FLASH_EEPROM_HS        = 0x29U;
const stw_types::uint8 KFX_PROTOCOL_WRITE_SRAM_HS                = 0x2BU;
const stw_types::uint8 KFX_PROTOCOL_UPDATE_TASK                  = 0x2EU;
const stw_types::uint8 KFX_PROTOCOL_UPDATE_TASK_HS               = 0x2FU;
const stw_types::uint8 KFX_PROTOCOL_WRITE_EEPROM_SSL             = 0x30U;
const stw_types::uint8 KFX_PROTOCOL_IWR_FF                       = 0x40U;
const stw_types::uint8 KFX_PROTOCOL_IWR_FC                       = 0x40U;
const stw_types::uint8 KFX_PROTOCOL_IWR_CF                       = 0x41U;
const stw_types::uint8 KFX_PROTOCOL_SRR_FF                       = 0x42U;
const stw_types::uint8 KFX_PROTOCOL_SRR_FC                       = 0x42U;
const stw_types::uint8 KFX_PROTOCOL_SRR_CF                       = 0x43U;

//----------------------------------------------------------------------------------------------------------------------

///KEFEX communication protocol response
class C_KFXProcotolResponse
{
public:
   stw_types::uint8 u8_Service; ///< ID of service
   stw_types::uint32 u32_IndexOrAddress;
   stw_types::uint8 au8_Data[7]; ///< Value or error code
   stw_types::uint8 u8_Size;     ///< number of bytes of dwValue in use
   bool q_Overrun;
   bool q_NewValue;
   bool q_Error;

   stw_types::uint16 GetU16FromDataIntel(const stw_types::uint8 ou8_StartIndex) const;
   stw_types::uint32 GetU32FromDataIntel(const stw_types::uint8 ou8_StartIndex) const;
};

//----------------------------------------------------------------------------------------------------------------------

//function prototypes
//do not use __closure to stay independent of Borland C++
typedef void (* PR_KFXProtECUResetCallBack)(void * const opv_InstancePointer);
typedef void (* PR_KFXProtNewCyclicTransmissionReceived)(void * const opv_Instance, const stw_types::uint32 ou32_Index,
                                                         const stw_types::sint64 os64_Value,
                                                         const stw_types::uint32 ou32_TimeStamp,
                                                         const bool oq_IsTimeStamped,
                                                         const bool oq_Error);

//---------------------------------------------------------------------------

///KEFEX communication protocol driver
class STW_DIAGLIB_EXPORT C_KFXProtocol
{
private:
   C_KFXProtocol(const C_KFXProtocol & orc_Source);               ///< not implemented -> prevent copying
   C_KFXProtocol & operator = (const C_KFXProtocol & orc_Source); ///< not implemented -> prevent assignment

protected:
   //for protocolling to file:
#ifndef DIAGLIB_KEFEX_PROTOCOL_NO_LOGGING
   stw_cmon_protocol::C_CMONProtocols mc_Protocol;
   std::FILE * mpt_DebugFileHandle;
   void m_ProtocolCheckInit(void);
   void m_ProtocolSend(const bool oq_OK, const stw_can::T_STWCAN_Msg_TX & orc_Msg);
   void m_ProtocolReceive(const stw_can::T_STWCAN_Msg_RX & orc_Msg);
#endif

   PR_KFXProtECUResetCallBack mpr_ResetCallBack;
   void * mpv_ResetCallBackInstance;

   stw_types::uint32 mu32_SendIDRQ;
   stw_types::uint32 mu32_ReceiveIDRSP;

   stw_types::sint32 m_EvalResponses(const stw_types::uint16 ou16_TimeOut, const stw_types::uint8 ou8_ExpectedService,
                                     const bool oq_CheckExpectedService, C_KFXProcotolResponse & orc_Response);

   void m_SetLastResponse(const stw_types::uint8 ou8_Service, const stw_types::uint32 ou32_Index,
                          const stw_types::uint8 * const opu8_Data, const stw_types::uint8 ou8_Size,
                          const bool oq_Error);
   void m_GetLastResponse(C_KFXProcotolResponse & orc_Response);

   stw_types::sint32 m_CalcHysteresis(const stw_types::uint32 ou32_AbsoluteValue, stw_types::uint8 * const opu8_Base,
                                      stw_types::uint8 * const opu8_Value, const bool oq_ForceBase) const;

   C_KFXProcotolResponse mc_LastResponse;

   stw_types::sint32 m_SendMessage(stw_can::T_STWCAN_Msg_TX & orc_Msg, const bool oq_SetXtdOff = true);

   bool m_MessageReceived(const stw_can::T_STWCAN_Msg_RX & orc_MSG);

   stw_can::C_CAN_Dispatcher * mpc_Dispatcher;
   stw_types::uint16 mu16_CANDispatcherHandle;
   C_KFXCommConfiguration * mpt_Config;
   bool mq_SleepBetweenPolling;

   stw_types::sint32 m_SendSRRRequest(const stw_types::uint16 ou16_VariableIndex);

public:
   C_KFXProtocol(C_KFXCommConfiguration * const opt_CommConfig, const bool oq_SleepBetweenPolling = false);
   ~C_KFXProtocol();

   void SetCommDispatcher(stw_can::C_CAN_Dispatcher * const opc_Dispatcher);

   //Read responses and parse all incoming cyclic data
   void EvaluateAllResponses(void);
   //Read responses for a maximum of wTimeout ms but return immediately if
   // one service of type bService is received
   stw_types::sint32 EvaluateResponses(const stw_types::uint8 ou8_Service, C_KFXProcotolResponse & orc_Response);

   stw_types::sint32 ConfigChanged(void);

   stw_types::sint32 SendLogonRequest(const stw_types::uint16 ou16_TypeAddressCRC,
                                      const stw_types::uint8 ou8_ProjectIndex, const stw_types::uint16 ou16_DataVersion,
                                      const stw_types::uint16 ou16_NumOfVariables);
   stw_types::sint32 SendLogoffRequest(void);
   stw_types::sint32 SendServiceReadRequest(const stw_types::uint16 ou16_VariableIndex);
   stw_types::sint32 SendServiceWriteRequest(const stw_types::uint16 ou16_VariableIndex,
                                             const stw_types::uint32 ou32_Value);

   stw_types::sint32 SendSRRRequest(const stw_types::uint16 ou16_VariableIndex);
   stw_types::sint32 SendECRRRequest(const stw_types::uint16 ou16_VariableIndex,
                                     const stw_types::uint16 ou16_MaxWaitTime,
                                     const stw_types::uint8 ou8_UpperHysteresis,
                                     const stw_types::uint8 ou8_LowerHysteresis,
                                     const stw_types::uint8 ou8_HysteresisBase);
   stw_types::sint32 SendECRRRequestAbsolute(const stw_types::uint16 ou16_VariableIndex,
                                             const stw_types::uint16 ou16_MaxWaitTime,
                                             const stw_types::uint32 ou32_UpperHysteresis,
                                             const stw_types::uint32 ou32_LowerHysteresis);
   stw_types::sint32 SendTCRRRequest(const stw_types::uint16 ou16_VariableIndex, const bool oq_TimeStamped,
                                     const stw_types::uint16 ou16_Interval);

   stw_types::sint32 SendAbortIndividualResponseRequest(const stw_types::uint16 ou16_VariableIndex,
                                                        const bool oq_Handshake);
   stw_types::sint32 SendAbortAllResponsesRequest(const bool oq_Handshake);
   stw_types::sint32 SendImmediateWriteRequest(const stw_types::uint16 ou16_VariableIndex,
                                               const stw_types::uint32 ou32_Value);
   stw_types::sint32 SendReadDataWordRequest(const stw_types::uint8 ou8_Type, const stw_types::uint32 ou32_Address);
   stw_types::sint32 SendReadDataByteRequest(const stw_types::uint8 ou8_Type, const stw_types::uint32 ou32_Address);
   stw_types::sint32 SendWriteDataWordRequest(const stw_types::uint8 ou8_Type, const stw_types::uint32 ou32_Address,
                                              const stw_types::uint8 oau8_Values[2]);
   stw_types::sint32 SendWriteDataByteRequest(const stw_types::uint8 ou8_Type, const stw_types::uint32 ou32_Address,
                                              const stw_types::uint8 ou8_Value);
   stw_types::sint32 SendWriteEepromSslStartRequest(const stw_types::uint16 ou16_NumFollowingWriteServices);
   stw_types::sint32 SendWriteEepromSslEndRequest(const stw_types::uint16 ou16_NumRequestedWriteServices,
                                                  const stw_types::uint16 ou16_CRCOverSentPayload);

   stw_types::sint32 SendUpdateTaskRequest(const stw_types::uint16 ou16_TaskIndex, const bool oq_Handshake);

   stw_types::sint32 SegmentedSRRTransfer(const stw_types::uint16 ou16_Index,
                                          const stw_types::uint32 ou32_NumBytesExpected,
                                          stw_types::uint8 * const opu8_Data,
                                          stw_types::uint16 * const opu16_Error = NULL);
   stw_types::sint32 SegmentedIWRTransfer(const stw_types::uint16 ou16_Index, const stw_types::uint32 ou32_NumBytes,
                                          const stw_types::uint8 * opu8_Data);

   //StartupIndication will be directly forwarded to application if polled
   void InstallECUResetHandler(const PR_KFXProtECUResetCallBack opr_Callback, void * const opv_InstancePointer);

   //protocol is still undocumented: not recommended to use ...
   stw_types::uint8 u8_CreateCommProtocol; //0 -> off; 1 -> KFX + L2; 2 -> KFX
   void ResetCommProtocol(void);

#ifdef CMONPROTOCOL_ALLOW_RAMVIEW_PROJECT_MAPPING
   void SetKEFEXVariableInfo(const C_KFXVariableLists * const opt_KFXLists, const stw_types::uint16 ou16_ListOffset);
#endif

   PR_KFXProtNewCyclicTransmissionReceived mpr_OnNewCyclicTransmissionReceived;
   void * mpv_CyclicCallbackInstance;
};

//---------------------------------------------------------------------------
}

#endif
