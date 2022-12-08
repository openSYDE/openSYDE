#ifndef CKFXPROTOCOLH
#define CKFXPROTOCOLH

#include <cstdio>

#include "DiagLib_config.hpp"
#include "stwtypes.hpp"
#include "CKFXCommConfiguration.hpp"
#include "TglTasks.hpp"
#include "C_CanDispatcher.hpp"
#ifndef DIAGLIB_KEFEX_PROTOCOL_NO_LOGGING
#include "C_CanMonProtocol.hpp" //for protocolling HLP to text file
#ifdef CMONPROTOCOL_ALLOW_RAMVIEW_PROJECT_MAPPING
#include "CKFXVariableLists.h" //for protocolling HLP to text file
#endif
#endif

//---------------------------------------------------------------------------

namespace stw
{
namespace diag_lib
{
//---------------------------------------------------------------------------

const uint8_t KFX_PROTOCOL_SRR                          = 0x01U;
const uint8_t KFX_PROTOCOL_ECRR                         = 0x02U;
const uint8_t KFX_PROTOCOL_TCRR                         = 0x03U;
const uint8_t KFX_PROTOCOL_TCRR_TIMESTAMPED             = 0x04U;
const uint8_t KFX_PROTOCOL_ABORT_INDIVIDUAL_RESPONSE    = 0x0AU;
const uint8_t KFX_PROTOCOL_ABORT_INDIVIDUAL_RESPONSE_HS = 0x0BU;
const uint8_t KFX_PROTOCOL_ABORT_ALL_RESPONSES          = 0x0CU;
const uint8_t KFX_PROTOCOL_ABORT_ALL_RESPONSES_HS       = 0x0DU;
const uint8_t KFX_PROTOCOL_IMMEDIATE_WRITE_HS           = 0x0FU;
const uint8_t KFX_PROTOCOL_LOGON_HS                     = 0x11U;
const uint8_t KFX_PROTOCOL_LOGOFF_HS                    = 0x15U;
const uint8_t KFX_PROTOCOL_SERVICE_READ                 = 0x16U;
const uint8_t KFX_PROTOCOL_SERVICE_WRITE                = 0x17U;
const uint8_t KFX_PROTOCOL_STARTUP_INDICATION           = 0x1AU;
const uint8_t KFX_PROTOCOL_READ_EEPROM                  = 0x20U;
const uint8_t KFX_PROTOCOL_READ_FLASH_EEPROM            = 0x21U;
const uint8_t KFX_PROTOCOL_READ_SRAM                    = 0x22U;
const uint8_t KFX_PROTOCOL_READ_EPROM                   = 0x23U;
const uint8_t KFX_PROTOCOL_WRITE_EEPROM_HS              = 0x27U;
const uint8_t KFX_PROTOCOL_WRITE_FLASH_EEPROM_HS        = 0x29U;
const uint8_t KFX_PROTOCOL_WRITE_SRAM_HS                = 0x2BU;
const uint8_t KFX_PROTOCOL_UPDATE_TASK                  = 0x2EU;
const uint8_t KFX_PROTOCOL_UPDATE_TASK_HS               = 0x2FU;
const uint8_t KFX_PROTOCOL_WRITE_EEPROM_SSL             = 0x30U;
const uint8_t KFX_PROTOCOL_IWR_FF                       = 0x40U;
const uint8_t KFX_PROTOCOL_IWR_FC                       = 0x40U;
const uint8_t KFX_PROTOCOL_IWR_CF                       = 0x41U;
const uint8_t KFX_PROTOCOL_SRR_FF                       = 0x42U;
const uint8_t KFX_PROTOCOL_SRR_FC                       = 0x42U;
const uint8_t KFX_PROTOCOL_SRR_CF                       = 0x43U;

//----------------------------------------------------------------------------------------------------------------------

///KEFEX communication protocol response
class C_KFXProcotolResponse
{
public:
   uint8_t u8_Service; ///< ID of service
   uint32_t u32_IndexOrAddress;
   uint8_t au8_Data[7]; ///< Value or error code
   uint8_t u8_Size;     ///< number of bytes of dwValue in use
   bool q_Overrun;
   bool q_NewValue;
   bool q_Error;

   uint16_t GetU16FromDataIntel(const uint8_t ou8_StartIndex) const;
   uint32_t GetU32FromDataIntel(const uint8_t ou8_StartIndex) const;
};

//----------------------------------------------------------------------------------------------------------------------

//function prototypes
//do not use __closure to stay independent of Borland C++
typedef void (* PR_KFXProtECUResetCallBack)(void * const opv_InstancePointer);
typedef void (* PR_KFXProtNewCyclicTransmissionReceived)(void * const opv_Instance, const uint32_t ou32_Index,
                                                         const int64_t os64_Value, const uint32_t ou32_TimeStamp,
                                                         const bool oq_IsTimeStamped, const bool oq_Error);

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
   stw::cmon_protocol::C_CMONProtocols mc_Protocol;
   std::FILE * mpt_DebugFileHandle;
   void m_ProtocolCheckInit(void);
   void m_ProtocolSend(const bool oq_OK, const stw::can::T_STWCAN_Msg_TX & orc_Msg);
   void m_ProtocolReceive(const stw::can::T_STWCAN_Msg_RX & orc_Msg);
#endif

   PR_KFXProtECUResetCallBack mpr_ResetCallBack;
   void * mpv_ResetCallBackInstance;

   uint32_t mu32_SendIDRQ;
   uint32_t mu32_ReceiveIDRSP;

   int32_t m_EvalResponses(const uint16_t ou16_TimeOut, const uint8_t ou8_ExpectedService,
                           const bool oq_CheckExpectedService, C_KFXProcotolResponse & orc_Response);

   void m_SetLastResponse(const uint8_t ou8_Service, const uint32_t ou32_Index, const uint8_t * const opu8_Data,
                          const uint8_t ou8_Size, const bool oq_Error);
   void m_GetLastResponse(C_KFXProcotolResponse & orc_Response);

   int32_t m_CalcHysteresis(const uint32_t ou32_AbsoluteValue, uint8_t * const opu8_Base, uint8_t * const opu8_Value,
                            const bool oq_ForceBase) const;

   C_KFXProcotolResponse mc_LastResponse;

   int32_t m_SendMessage(stw::can::T_STWCAN_Msg_TX & orc_Msg, const bool oq_SetXtdOff = true);

   bool m_MessageReceived(const stw::can::T_STWCAN_Msg_RX & orc_MSG);

   stw::can::C_CanDispatcher * mpc_Dispatcher;
   uint16_t mu16_CANDispatcherHandle;
   C_KFXCommConfiguration * mpt_Config;
   bool mq_SleepBetweenPolling;

   int32_t m_SendSRRRequest(const uint16_t ou16_VariableIndex);

public:
   C_KFXProtocol(C_KFXCommConfiguration * const opt_CommConfig, const bool oq_SleepBetweenPolling = false);
   ~C_KFXProtocol();

   void SetCommDispatcher(stw::can::C_CanDispatcher * const opc_Dispatcher);

   //Read responses and parse all incoming cyclic data
   void EvaluateAllResponses(void);
   //Read responses for a maximum of wTimeout ms but return immediately if
   // one service of type bService is received
   int32_t EvaluateResponses(const uint8_t ou8_Service, C_KFXProcotolResponse & orc_Response);

   int32_t ConfigChanged(void);

   int32_t SendLogonRequest(const uint16_t ou16_TypeAddressCRC, const uint8_t ou8_ProjectIndex,
                            const uint16_t ou16_DataVersion, const uint16_t ou16_NumOfVariables);
   int32_t SendLogoffRequest(void);
   int32_t SendServiceReadRequest(const uint16_t ou16_VariableIndex);
   int32_t SendServiceWriteRequest(const uint16_t ou16_VariableIndex, const uint32_t ou32_Value);

   int32_t SendSRRRequest(const uint16_t ou16_VariableIndex);
   int32_t SendECRRRequest(const uint16_t ou16_VariableIndex, const uint16_t ou16_MaxWaitTime,
                           const uint8_t ou8_UpperHysteresis, const uint8_t ou8_LowerHysteresis,
                           const uint8_t ou8_HysteresisBase);
   int32_t SendECRRRequestAbsolute(const uint16_t ou16_VariableIndex, const uint16_t ou16_MaxWaitTime,
                                   const uint32_t ou32_UpperHysteresis, const uint32_t ou32_LowerHysteresis);
   int32_t SendTCRRRequest(const uint16_t ou16_VariableIndex, const bool oq_TimeStamped, const uint16_t ou16_Interval);

   int32_t SendAbortIndividualResponseRequest(const uint16_t ou16_VariableIndex, const bool oq_Handshake);
   int32_t SendAbortAllResponsesRequest(const bool oq_Handshake);
   int32_t SendImmediateWriteRequest(const uint16_t ou16_VariableIndex, const uint32_t ou32_Value);
   int32_t SendReadDataWordRequest(const uint8_t ou8_Type, const uint32_t ou32_Address);
   int32_t SendReadDataByteRequest(const uint8_t ou8_Type, const uint32_t ou32_Address);
   int32_t SendWriteDataWordRequest(const uint8_t ou8_Type, const uint32_t ou32_Address, const uint8_t oau8_Values[2]);
   int32_t SendWriteDataByteRequest(const uint8_t ou8_Type, const uint32_t ou32_Address, const uint8_t ou8_Value);
   int32_t SendWriteEepromSslStartRequest(const uint16_t ou16_NumFollowingWriteServices);
   int32_t SendWriteEepromSslEndRequest(const uint16_t ou16_NumRequestedWriteServices,
                                        const uint16_t ou16_CRCOverSentPayload);

   int32_t SendUpdateTaskRequest(const uint16_t ou16_TaskIndex, const bool oq_Handshake);

   int32_t SegmentedSRRTransfer(const uint16_t ou16_Index, const uint32_t ou32_NumBytesExpected,
                                uint8_t * const opu8_Data, uint16_t * const opu16_Error = NULL);
   int32_t SegmentedIWRTransfer(const uint16_t ou16_Index, const uint32_t ou32_NumBytes, const uint8_t * opu8_Data);

   //StartupIndication will be directly forwarded to application if polled
   void InstallECUResetHandler(const PR_KFXProtECUResetCallBack opr_Callback, void * const opv_InstancePointer);

   //protocol is still undocumented: not recommended to use ...
   uint8_t u8_CreateCommProtocol; //0 -> off; 1 -> KFX + L2; 2 -> KFX
   void ResetCommProtocol(void);

#ifdef CMONPROTOCOL_ALLOW_RAMVIEW_PROJECT_MAPPING
   void SetKEFEXVariableInfo(const C_KFXVariableLists * const opt_KFXLists, const uint16_t ou16_ListOffset);
#endif

   PR_KFXProtNewCyclicTransmissionReceived mpr_OnNewCyclicTransmissionReceived;
   void * mpv_CyclicCallbackInstance;
};

//---------------------------------------------------------------------------
}
}

#endif
