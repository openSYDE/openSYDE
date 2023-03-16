//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Header for C_CAN_DLL class.

   Header to class implementing an interface to communication driver DLLs in the STW-CAN-DLL
   format.

   \copyright   Copyright 2008 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef CCANDLLHPP
#define CCANDLLHPP

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <windows.h>
#include "stwtypes.hpp" //!<STW typedefs
#include "stw_can.hpp"  //!<STW CAN DLL types

namespace stw
{
namespace can
{
/* -- Defines ------------------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */
//function pointer types:
typedef int32_t (WINAPI * PR_CAN_INIT)(void);
typedef int32_t (WINAPI * PR_CAN_EXIT)(void);
typedef int32_t (WINAPI * PR_CAN_RESET)(void);
typedef int32_t (WINAPI * PR_CAN_BITRATE)(const int32_t os32_Bitrate);
typedef int32_t (WINAPI * PR_CAN_READ_MSG)(STW_CAN_MSG * const opt_Msg);
typedef int32_t (WINAPI * PR_CAN_SEND_MSG)(const STW_CAN_MSG * const opt_Msg);
typedef int32_t (WINAPI * PR_CAN_SEND_RTR)(const STW_CAN_MSG * const opt_Msg);
typedef void (WINAPI * PR_CAN_INTERFACESETUP)(const HWND opv_Handle);
typedef int32_t (WINAPI * PR_CAN_STATUS)(STW_CAN_STATUS * const opt_Status);
typedef int32_t (WINAPI * PR_CAN_DLL_INFO)(STW_CAN_DLL_INFO * const opt_Info);
typedef int32_t (WINAPI * PR_CAN_SEND_EXTMSG)(const STW_CAN_EXTMSG * const opt_Msg);
typedef int32_t (WINAPI * PR_CAN_READ_EXTMSG)(STW_CAN_EXTMSG * const opt_Msg);
typedef int32_t (WINAPI * PR_CAN_SEND_EXTRTR)(const STW_CAN_EXTMSG * const opt_Msg);
typedef int32_t (WINAPI * PR_CAN_INIT_ONE_ID)(const int32_t os32_Bitrate,
                                              const uint32_t ou32_Id);

//functions only available for stream based communication interfaces:
typedef int32_t (WINAPI * PR_SER_GET_TX_BUF_COUNT)(uint32_t * const opu32_NumBytes,
                                                   uint32_t * const opu32_MaxBufSize);
typedef int32_t (WINAPI * PR_SER_GET_RX_BUF_COUNT)(uint32_t * const opu32_NumBytes,
                                                   uint32_t * const opu32_MaxBufSize);
typedef int32_t (WINAPI * PR_SER_SEND_BYTES)(const uint8_t * const opu8_Data,
                                             const uint32_t ou32_NumBytes);
typedef int32_t (WINAPI * PR_SER_READ_BYTES)(uint8_t * const opu8_Data,
                                             uint32_t * const opu32_NumBytes);

//special functions for dial-up based communication interfaces:
typedef int32_t (WINAPI * PR_CANTAPI_CONNECT)(const uint8_t * opu8_Number,
                                              const uint32_t ou32_TimeOut);
typedef int32_t (WINAPI * PR_CANTAPI_DISCONNECT)(const uint32_t ou32_TimeOut);

//"ext" function pointer types:
typedef int32_t (WINAPI * PR_CANext_Init)(const uint8_t ou8_Channel);
typedef int32_t (WINAPI * PR_CANext_Exit)(const uint8_t ou8_Channel);
typedef int32_t (WINAPI * PR_CANext_Bitrate)(const uint8_t ou8_Channel,
                                             const int32_t os32_Bitrate);
typedef int32_t (WINAPI * PR_CANext_Read_Msg)(const uint8_t ou8_Channel,
                                              T_STWCAN_Msg_RX * const opt_Message);
typedef int32_t (WINAPI * PR_CANext_Send_Msg)(const uint8_t ou8_Channel,
                                              const T_STWCAN_Msg_TX * const opt_Message);
typedef int32_t (WINAPI * PR_CANext_InterfaceSetup)(const uint8_t ou8_Channel);
typedef int32_t (WINAPI * PR_CANext_Status)(const uint8_t ou8_Channel,
                                            T_STWCAN_Status * const opt_Status);
typedef int32_t (WINAPI * PR_CANext_Init_One_ID)(const uint8_t ou8_Channel,
                                                 const int32_t os32_Bitrate,
                                                 const uint32_t ou32_Id);
typedef int32_t (WINAPI * PR_CANext_DLL_Info)(const uint8_t ou8_Channel,
                                              T_STWCAN_DLLInfo * const opt_Info);
typedef int32_t (WINAPI * PR_CANext_Get_Num_Channels)(void);
typedef int32_t (WINAPI * PR_CANext_Get_System_Time)(uint64_t * const opu64_TimeUs);
typedef int32_t (WINAPI * PR_CANext_Get_Num_Supported_Bitrates)
   (uint32_t * const opu32_MultiplicationFactor);
typedef int32_t (WINAPI * PR_CANext_Get_Supported_Bitrate)(const uint16_t ou16_BitrateIndex,
                                                           uint32_t * const opu32_Bitrate);

// special functions for stw_tcp.dll:
typedef int32_t (WINAPI * PR_CANtcp_Read_Device_List_From_Server)(const uint8_t ou8_Channel,
                                                                  uint32_t * const
                                                                  opu32_NoOfDevices,
                                                                  uint32_t * const opu32_MaxLen);
typedef int32_t (WINAPI * PR_CANtcp_Get_Device_Name)(const uint8_t ou8_Channel,
                                                     const uint32_t ou32_Index,
                                                     char_t * const opcn_DeviceName);

/* -- Global Variables ---------------------------------------------------------------------------------------------- */

/* -- Function Prototypes ------------------------------------------------------------------------------------------- */

/* -- Implementation ------------------------------------------------------------------------------------------------ */

//lint -sem(stw::can::C_CAN_DLL::m_Init,initializer)
//lint -sem(stw::can::C_CAN_DLL::m_Exit,cleanup)
///Interface class to use STW CAN DLL functions
class C_CanDll
{
private:
   CRITICAL_SECTION mt_Lock;

   PR_CAN_INIT mpr_CAN_Init;
   PR_CAN_EXIT mpr_CAN_Exit;
   PR_CAN_RESET mpr_CAN_Reset;
   PR_CAN_BITRATE mpr_CAN_Bitrate;
   PR_CAN_READ_MSG mpr_CAN_Read_Msg;
   PR_CAN_SEND_MSG mpr_CAN_Send_Msg;
   PR_CAN_SEND_RTR mpr_CAN_Send_RTR;
   PR_CAN_INTERFACESETUP mpr_CAN_InterfaceSetup;
   PR_CAN_STATUS mpr_CAN_Status;
   PR_CAN_DLL_INFO mpr_CAN_DLL_Info;
   PR_CAN_SEND_EXTMSG mpr_CAN_Send_extMsg;
   PR_CAN_READ_EXTMSG mpr_CAN_Read_extMsg;
   PR_CAN_SEND_EXTRTR mpr_CAN_Send_extRTR;
   PR_CAN_INIT_ONE_ID mpr_CAN_Init_One_ID;

   PR_SER_GET_TX_BUF_COUNT mpr_SER_GET_TX_BUF_COUNT;
   PR_SER_GET_RX_BUF_COUNT mpr_SER_GET_RX_BUF_COUNT;
   PR_SER_SEND_BYTES mpr_SER_SEND_BYTES;
   PR_SER_READ_BYTES mpr_SER_READ_BYTES;

   PR_CANTAPI_CONNECT mpr_CANTAPI_CONNECT;
   PR_CANTAPI_DISCONNECT mpr_CANTAPI_DISCONNECT;

   PR_CANext_Init mpr_CANext_Init;
   PR_CANext_Exit mpr_CANext_Exit;
   PR_CANext_Bitrate mpr_CANext_Bitrate;
   PR_CANext_Read_Msg mpr_CANext_Read_Msg;
   PR_CANext_Send_Msg mpr_CANext_Send_Msg;
   PR_CANext_InterfaceSetup mpr_CANext_InterfaceSetup;
   PR_CANext_Status mpr_CANext_Status;
   PR_CANext_Init_One_ID mpr_CANext_Init_One_ID;
   PR_CANext_DLL_Info mpr_CANext_DLL_Info;
   PR_CANext_Get_Num_Channels mpr_CANext_Get_Num_Channels;
   PR_CANext_Get_System_Time mpr_CANext_Get_System_Time;
   PR_CANext_Get_Num_Supported_Bitrates mpr_CANext_Get_Num_Supported_Bitrates;
   PR_CANext_Get_Supported_Bitrate mpr_CANext_Get_Supported_Bitrate;

   PR_CANtcp_Read_Device_List_From_Server mpr_CANtcp_Read_Device_List_From_Server;
   PR_CANtcp_Get_Device_Name mpr_CANtcp_Get_Device_Name;

   int32_t m_Init(const char_t * const opcn_DllPath);
   void m_Exit(void);
   void m_ClearFunctionPointers(void);

public:
   C_CanDll(const C_CanDll & orc_Souce);               //not implemented -> prevent copying
   C_CanDll & operator = (const C_CanDll & orc_Souce); //not implemented -> prevent assignment

   C_CanDll(const char_t * const opcn_DllPath);
   ~C_CanDll();

   int32_t CAN_Init(void);
   int32_t CAN_Exit(void);
   int32_t CAN_Reset(void);
   int32_t CAN_Bitrate(const int32_t os32_BitrateKBitS);
   int32_t CAN_Read_Msg(STW_CAN_MSG & orc_Message);
   int32_t CAN_Send_Msg(const STW_CAN_MSG & orc_Message);
   int32_t CAN_Send_RTR(const STW_CAN_MSG & orc_Message);
   int32_t CAN_InterfaceSetup(const HWND opv_Handle);
   int32_t CAN_Status(STW_CAN_STATUS & orc_Status);
   int32_t CAN_DLL_Info(STW_CAN_DLL_INFO & orc_Info);
   int32_t CAN_Read_extMsg(STW_CAN_EXTMSG & orc_Message);
   int32_t CAN_Send_extMsg(const STW_CAN_EXTMSG & orc_Message);
   int32_t CAN_Send_extRTR(const STW_CAN_EXTMSG & orc_Message);
   int32_t CAN_Init_One_ID(const int32_t os32_BitrateKBitS, const uint32_t ou32_ID);

   int32_t SER_Get_TX_Buf_Count(uint32_t & oru32_NumBytes, uint32_t & oru32_MaxBufSize);
   int32_t SER_Send_Bytes(const uint8_t * const opu8_Data, const uint32_t ou32_NumBytes);
   int32_t SER_Get_RX_Buf_Count(uint32_t & oru32_NumBytes, uint32_t & oru32_MaxBufSize);
   int32_t SER_Read_Bytes(uint8_t * const opu8_Data, uint32_t & oru32_NumBytes);
   int32_t CANTAPI_Connect(const uint8_t * const opu8_Number, const uint32_t ou32_TimeOut);
   int32_t CANTAPI_Disconnect(const uint32_t ou32_TimeOut);

   //"ext" functions:
   int32_t CANext_Init(const uint8_t ou8_Channel);
   int32_t CANext_Exit(const uint8_t ou8_Channel);
   int32_t CANext_Bitrate(const uint8_t ou8_Channel, const int32_t os32_BitrateKBitS);
   int32_t CANext_Read_Msg(const uint8_t ou8_Channel, T_STWCAN_Msg_RX & orc_Message);
   int32_t CANext_Send_Msg(const uint8_t ou8_Channel, const T_STWCAN_Msg_TX & orc_Message);
   int32_t CANext_InterfaceSetup(const uint8_t ou8_Channel);
   int32_t CANext_Status(const uint8_t ou8_Channel, T_STWCAN_Status & orc_Status);
   int32_t CANext_Init_One_ID(const uint8_t ou8_Channel, const int32_t os32_BitrateKBitS, const uint32_t ou32_ID);
   int32_t CANext_DLL_Info(const uint8_t ou8_Channel, T_STWCAN_DLLInfo & orc_Info);
   int32_t CANext_Get_Num_Channels(void);
   int32_t CANext_Get_System_Time(uint64_t & oru64_SystemTimeUs);
   int32_t CANext_Get_Num_Supported_Bitrates(uint32_t & oru32_MultiplicationFactor);
   int32_t CANext_Get_Supported_Bitrate(const uint16_t ou16_BitrateIndex, uint32_t & oru32_Bitrate);

   int32_t CANtcp_Read_Device_List_From_Server(const uint8_t ou8_Channel, uint32_t & oru32_NoOfDevices,
                                               uint32_t & oru32_MaxLen);
   int32_t CANtcp_Get_Device_Name(const uint8_t ou8_Channel, const uint32_t ou32_Index, char_t * const opcn_DeviceName);

   //utilities to convert between different message structure types
   static void STWCANMSG_to_CANMSGRX(T_STWCAN_Msg_RX & orc_Target, const STW_CAN_MSG & orc_Source);
   static void STWCANEXTMSG_to_CANMSGRX(T_STWCAN_Msg_RX & orc_Target, const STW_CAN_EXTMSG & orc_Source);
   static void CANMSGTX_to_STWCANMSG(STW_CAN_MSG & orc_Target, const T_STWCAN_Msg_TX & orc_Source);
   static void CANMSGTX_to_STWCANEXTMSG(STW_CAN_EXTMSG & orc_Target, const T_STWCAN_Msg_TX & orc_Source);

   bool mq_ExtFunctionsAvailable;
   bool mq_ModemFunctionsAvailable;
   bool mq_StreamFunctionsAvailable; //does not signal availability of "supported_bitrate" functions !
   bool mq_BitrateInformationFunctionsAvailable;
   bool mq_TCPFunctionsAvailable;

   HINSTANCE mpv_DLL;
};
}
}
#endif
