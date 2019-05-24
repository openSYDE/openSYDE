//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Header for C_CAN_DLL class.

   Header to class implementing an interface to communication driver DLLs in the STW-CAN-DLL
   format.

   \copyright   Copyright 2008 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef CCANDLLH
#define CCANDLLH

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <windows.h>
#include "stwtypes.h"   //!<STW typedefs
#include "stw_can.h"    //!<STW CAN DLL types

namespace stw_can
{

/* -- Defines ------------------------------------------------------------------------------------------------------- */
#ifndef STWCAN_PACKAGE
#ifdef __BORLANDC__
//maybe we want this module to be put into a VCL package ...
#define STWCAN_PACKAGE __declspec(package)
#else
#define STWCAN_PACKAGE
#endif
#endif

/* -- Types --------------------------------------------------------------------------------------------------------- */
//function pointer types:
typedef stw_types::sint32 (WINAPI *PR_CAN_INIT)(void);
typedef stw_types::sint32 (WINAPI *PR_CAN_EXIT)(void);
typedef stw_types::sint32 (WINAPI *PR_CAN_RESET)(void);
typedef stw_types::sint32 (WINAPI *PR_CAN_BITRATE)(const stw_types::sint32 os32_Bitrate);
typedef stw_types::sint32 (WINAPI *PR_CAN_READ_MSG)(STW_CAN_MSG * const opt_Msg);
typedef stw_types::sint32 (WINAPI *PR_CAN_SEND_MSG)(const STW_CAN_MSG * const opt_Msg);
typedef stw_types::sint32 (WINAPI *PR_CAN_SEND_RTR)(const STW_CAN_MSG * const opt_Msg);
typedef void              (WINAPI *PR_CAN_INTERFACESETUP)(const HWND opv_Handle);
typedef stw_types::sint32 (WINAPI *PR_CAN_STATUS)(STW_CAN_STATUS * const opt_Status);
typedef stw_types::sint32 (WINAPI *PR_CAN_DLL_INFO)(STW_CAN_DLL_INFO * const opt_Info);
typedef stw_types::sint32 (WINAPI *PR_CAN_SEND_EXTMSG)(const STW_CAN_EXTMSG * const opt_Msg);
typedef stw_types::sint32 (WINAPI *PR_CAN_READ_EXTMSG)(STW_CAN_EXTMSG * const opt_Msg);
typedef stw_types::sint32 (WINAPI *PR_CAN_SEND_EXTRTR)(const STW_CAN_EXTMSG * const opt_Msg);
typedef stw_types::sint32 (WINAPI *PR_CAN_INIT_ONE_ID)(const stw_types::sint32 os32_Bitrate,
                                                       const stw_types::uint32 ou32_ID);

//functions only available for stream based communication interfaces:
typedef stw_types::sint32 (WINAPI *PR_SER_GET_TX_BUF_COUNT)(stw_types::uint32 * const opu32_NumBytes,
                                                            stw_types::uint32 * const opu32_MaxBufSize);
typedef stw_types::sint32 (WINAPI *PR_SER_GET_RX_BUF_COUNT)(stw_types::uint32 * const opu32_NumBytes,
                                                            stw_types::uint32 * const opu32_MaxBufSize);
typedef stw_types::sint32 (WINAPI *PR_SER_SEND_BYTES)(const stw_types::uint8 * const opu8_Data,
                                                      const stw_types::uint32 ou32_NumBytes);
typedef stw_types::sint32 (WINAPI *PR_SER_READ_BYTES)(stw_types::uint8 * const opu8_Data,
                                                      stw_types::uint32 * const opu32_NumBytes);

//special functions for dial-up based communication interfaces:
typedef stw_types::sint32 (WINAPI *PR_CANTAPI_CONNECT)(const stw_types::uint8 * opu8_Number,
                                                       const stw_types::uint32 ou32_TimeOut);
typedef stw_types::sint32 (WINAPI *PR_CANTAPI_DISCONNECT)(const stw_types::uint32 ou32_TimeOut);

//"ext" function pointer types:
typedef stw_types::sint32 (WINAPI * PR_CANext_Init)(const stw_types::uint8 ou8_Channel);
typedef stw_types::sint32 (WINAPI * PR_CANext_Exit)(const stw_types::uint8 ou8_Channel);
typedef stw_types::sint32 (WINAPI * PR_CANext_Bitrate)(const stw_types::uint8 ou8_Channel,
                                                       const stw_types::sint32 os32_Bitrate);
typedef stw_types::sint32 (WINAPI * PR_CANext_Read_Msg)(const stw_types::uint8 ou8_Channel,
                                                        T_STWCAN_Msg_RX * const opt_Message);
typedef stw_types::sint32 (WINAPI * PR_CANext_Send_Msg)(const stw_types::uint8 ou8_Channel,
                                                        const T_STWCAN_Msg_TX * const opt_Message);
typedef stw_types::sint32 (WINAPI * PR_CANext_InterfaceSetup)(const stw_types::uint8 ou8_Channel);
typedef stw_types::sint32 (WINAPI * PR_CANext_Status)(const stw_types::uint8 ou8_Channel,
                                                      T_STWCAN_Status * const opt_Status);
typedef stw_types::sint32 (WINAPI * PR_CANext_Init_One_ID)(const stw_types::uint8 ou8_Channel,
                                                           const stw_types::sint32 os32_Bitrate,
                                                           const stw_types::uint32 ou32_ID);
typedef stw_types::sint32 (WINAPI * PR_CANext_DLL_Info)(const stw_types::uint8 ou8_Channel,
                                                        T_STWCAN_DLLInfo * const opt_Info);
typedef stw_types::sint32 (WINAPI * PR_CANext_Get_Num_Channels)(void);
typedef stw_types::sint32 (WINAPI * PR_CANext_Get_System_Time)(stw_types::uint64 * const opu64_TimeUs);
typedef stw_types::sint32 (WINAPI * PR_CANext_Get_Num_Supported_Bitrates)
                                                               (stw_types::uint32 * const opu32_MultiplicationFactor);
typedef stw_types::sint32 (WINAPI * PR_CANext_Get_Supported_Bitrate)(const stw_types::uint16 ou16_BitrateIndex,
                                                                     stw_types::uint32 * const opu32_Bitrate);

// special functions for stw_tcp.dll:
typedef stw_types::sint32 (WINAPI * PR_CANtcp_Read_Device_List_From_Server)(const stw_types::uint8 ou8_Channel,
                                                                           stw_types::uint32 * const opu32_NoOfDevices,
                                                                           stw_types::uint32 * const opu32_MaxLen);
typedef stw_types::sint32 (WINAPI * PR_CANtcp_Get_Device_Name)(const stw_types::uint8 ou8_Channel,
                                                               const stw_types::uint32 ou32_Index,
                                                               stw_types::charn * const opcn_DeviceName);

/* -- Global Variables ---------------------------------------------------------------------------------------------- */

/* -- Function Prototypes ------------------------------------------------------------------------------------------- */

/* -- Implementation ------------------------------------------------------------------------------------------------ */

//lint -sem(stw_can::C_CAN_DLL::m_Init,initializer)
//lint -sem(stw_can::C_CAN_DLL::m_Exit,cleanup)
///Interface class to use STW CAN DLL functions
class STWCAN_PACKAGE C_CAN_DLL
{
private:
   CRITICAL_SECTION mt_Lock;

   PR_CAN_INIT        mpr_CAN_Init;
   PR_CAN_EXIT        mpr_CAN_Exit;
   PR_CAN_RESET       mpr_CAN_Reset;
   PR_CAN_BITRATE     mpr_CAN_Bitrate;
   PR_CAN_READ_MSG    mpr_CAN_Read_Msg;
   PR_CAN_SEND_MSG    mpr_CAN_Send_Msg;
   PR_CAN_SEND_RTR    mpr_CAN_Send_RTR;
   PR_CAN_INTERFACESETUP mpr_CAN_InterfaceSetup;
   PR_CAN_STATUS      mpr_CAN_Status;
   PR_CAN_DLL_INFO    mpr_CAN_DLL_Info;
   PR_CAN_SEND_EXTMSG mpr_CAN_Send_extMsg;
   PR_CAN_READ_EXTMSG mpr_CAN_Read_extMsg;
   PR_CAN_SEND_EXTRTR mpr_CAN_Send_extRTR;
   PR_CAN_INIT_ONE_ID mpr_CAN_Init_One_ID;

   PR_SER_GET_TX_BUF_COUNT mpr_SER_GET_TX_BUF_COUNT;
   PR_SER_GET_RX_BUF_COUNT mpr_SER_GET_RX_BUF_COUNT;
   PR_SER_SEND_BYTES       mpr_SER_SEND_BYTES;
   PR_SER_READ_BYTES       mpr_SER_READ_BYTES;

   PR_CANTAPI_CONNECT    mpr_CANTAPI_CONNECT;
   PR_CANTAPI_DISCONNECT mpr_CANTAPI_DISCONNECT;

   PR_CANext_Init              mpr_CANext_Init;
   PR_CANext_Exit              mpr_CANext_Exit;
   PR_CANext_Bitrate           mpr_CANext_Bitrate;
   PR_CANext_Read_Msg          mpr_CANext_Read_Msg;
   PR_CANext_Send_Msg          mpr_CANext_Send_Msg;
   PR_CANext_InterfaceSetup    mpr_CANext_InterfaceSetup;
   PR_CANext_Status            mpr_CANext_Status;
   PR_CANext_Init_One_ID       mpr_CANext_Init_One_ID;
   PR_CANext_DLL_Info          mpr_CANext_DLL_Info;
   PR_CANext_Get_Num_Channels  mpr_CANext_Get_Num_Channels;
   PR_CANext_Get_System_Time   mpr_CANext_Get_System_Time;
   PR_CANext_Get_Num_Supported_Bitrates mpr_CANext_Get_Num_Supported_Bitrates;
   PR_CANext_Get_Supported_Bitrate      mpr_CANext_Get_Supported_Bitrate;

   PR_CANtcp_Read_Device_List_From_Server   mpr_CANtcp_Read_Device_List_From_Server;
   PR_CANtcp_Get_Device_Name                mpr_CANtcp_Get_Device_Name;

   stw_types::sint32 m_Init(const stw_types::charn * const opcn_DLLPath);
   void m_Exit(void);
   void m_ClearFunctionPointers(void);

public:
   C_CAN_DLL(const C_CAN_DLL & orc_Souce);              //not implemented -> prevent copying
   C_CAN_DLL& operator = (const C_CAN_DLL & orc_Souce); //not implemented -> prevent assignment

   C_CAN_DLL(const stw_types::charn * const opcn_DLLPath);
   ~C_CAN_DLL();

   stw_types::sint32 CAN_Init(void);
   stw_types::sint32 CAN_Exit(void);
   stw_types::sint32 CAN_Reset(void);
   stw_types::sint32 CAN_Bitrate(const stw_types::sint32 os32_BitrateKBitS);
   stw_types::sint32 CAN_Read_Msg(STW_CAN_MSG & orc_Message);
   stw_types::sint32 CAN_Send_Msg(const STW_CAN_MSG & orc_Message);
   stw_types::sint32 CAN_Send_RTR(const STW_CAN_MSG & orc_Message);
   stw_types::sint32 CAN_InterfaceSetup(const HWND opv_Handle);
   stw_types::sint32 CAN_Status(STW_CAN_STATUS & orc_Status);
   stw_types::sint32 CAN_DLL_Info(STW_CAN_DLL_INFO & orc_Info);
   stw_types::sint32 CAN_Read_extMsg(STW_CAN_EXTMSG & orc_Message);
   stw_types::sint32 CAN_Send_extMsg(const STW_CAN_EXTMSG & orc_Message);
   stw_types::sint32 CAN_Send_extRTR(const STW_CAN_EXTMSG & orc_Message);
   stw_types::sint32 CAN_Init_One_ID(const stw_types::sint32 os32_BitrateKBitS, const stw_types::uint32 ou32_ID);

   stw_types::sint32 SER_Get_TX_Buf_Count(stw_types::uint32 & oru32_NumBytes,
                                          stw_types::uint32 & oru32_MaxBufSize);
   stw_types::sint32 SER_Send_Bytes(const stw_types::uint8 * const opu8_Data, const stw_types::uint32 ou32_NumBytes);
   stw_types::sint32 SER_Get_RX_Buf_Count(stw_types::uint32 & oru32_NumBytes,
                                          stw_types::uint32 & oru32_MaxBufSize);
   stw_types::sint32 SER_Read_Bytes(stw_types::uint8 * const opu8_Data, stw_types::uint32 & oru32_NumBytes);
   stw_types::sint32 CANTAPI_Connect(const stw_types::uint8 * const opu8_Number, const stw_types::uint32 ou32_TimeOut);
   stw_types::sint32 CANTAPI_Disconnect(const stw_types::uint32 ou32_TimeOut);

   //"ext" functions:
   stw_types::sint32 CANext_Init(const stw_types::uint8 ou8_Channel);
   stw_types::sint32 CANext_Exit(const stw_types::uint8 ou8_Channel);
   stw_types::sint32 CANext_Bitrate(const stw_types::uint8 ou8_Channel, const stw_types::sint32 os32_BitrateKBitS);
   stw_types::sint32 CANext_Read_Msg(const stw_types::uint8 ou8_Channel, T_STWCAN_Msg_RX & orc_Message);
   stw_types::sint32 CANext_Send_Msg(const stw_types::uint8 ou8_Channel, const T_STWCAN_Msg_TX & orc_Message);
   stw_types::sint32 CANext_InterfaceSetup(const stw_types::uint8 ou8_Channel);
   stw_types::sint32 CANext_Status(const stw_types::uint8 ou8_Channel, T_STWCAN_Status & orc_Status);
   stw_types::sint32 CANext_Init_One_ID(const stw_types::uint8 ou8_Channel, const stw_types::sint32 os32_BitrateKBitS,
                                        const stw_types::uint32 ou32_ID);
   stw_types::sint32 CANext_DLL_Info(const stw_types::uint8 ou8_Channel, T_STWCAN_DLLInfo & orc_Info);
   stw_types::sint32 CANext_Get_Num_Channels(void);
   stw_types::sint32 CANext_Get_System_Time(stw_types::uint64 & oru64_SystemTimeUs);
   stw_types::sint32 CANext_Get_Num_Supported_Bitrates(stw_types::uint32 & oru32_MultiplicationFactor);
   stw_types::sint32 CANext_Get_Supported_Bitrate(const stw_types::uint16 ou16_BitrateIndex,
                                                  stw_types::uint32 & oru32_Bitrate);

   stw_types::sint32 CANtcp_Read_Device_List_From_Server(const stw_types::uint8 ou8_Channel,
                                                         stw_types::uint32 & oru32_NoOfDevices,
                                                         stw_types::uint32 & oru32_MaxLen);
   stw_types::sint32 CANtcp_Get_Device_Name(const stw_types::uint8 ou8_Channel, const stw_types::uint32 ou32_Index,
                                           stw_types::charn * const opcn_DeviceName);

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

#endif //CanDll_Header
