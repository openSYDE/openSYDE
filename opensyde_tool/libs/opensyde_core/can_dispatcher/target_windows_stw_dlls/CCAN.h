//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Header for C_CAN class.

   Header to class implementing an high level interface to communication driver DLLs in the STW-CAN-DLL
   format.

   \copyright   Copyright 2002 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef CCANH
#define CCANH

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "stwtypes.h"
#include "CCANDLL.h"
#include "CCANDispatcher.h"
#include "CSCLString.h"

#include "SCLDynamicArray.h"

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
///high level interface to STW CANDLLs
class STWCAN_PACKAGE C_CAN : public C_CAN_Dispatcher
{
private:
   stw_scl::C_SCLString mc_DLLName;
   C_CAN_DLL * mpc_CAN;
   bool mq_DLLOpened;
   stw_types::uint32 mu32_RXID;

   stw_types::sint32 m_ReadMsgFromDLL(T_STWCAN_Msg_RX & orc_Message) const;
   stw_types::sint32 m_SendMsgToDLL(const T_STWCAN_Msg_TX & orc_Message) const;

protected:
   //function from Dispatcher (mandatory to implement)
   //actual reading from CAN driver
   virtual stw_types::sint32 m_CAN_Read_Msg(T_STWCAN_Msg_RX & orc_Message);

public:
   C_CAN(const C_CAN & orc_Souce);              //not implemented -> prevent copying
   C_CAN& operator = (const C_CAN & orc_Souce); //not implemented -> prevent assignment

   C_CAN(void);
   C_CAN(const stw_types::uint8 ou8_CommChannel);
   virtual ~C_CAN(void);

   //functions from base (mandatory to implement):
   virtual stw_types::sint32 CAN_Init(void);   //use bitrate currently configured in CAN driver
   virtual stw_types::sint32 CAN_Init(const stw_types::sint32 os32_BitrateKBitS);
   virtual stw_types::sint32 CAN_Exit(void);
   virtual stw_types::sint32 CAN_Reset(void);
   virtual stw_types::sint32 CAN_Send_Msg(const T_STWCAN_Msg_TX & orc_Message);
   virtual stw_types::sint32 CAN_Get_System_Time(stw_types::uint64 & oru64_SystemTimeUs) const;

   //optional SER streaming functions (do not exist in an "ext" variation yet -> channel member will not be used !!)
   virtual stw_types::sint32 SER_Get_TX_Buf_Count(stw_types::uint32 & oru32_NumBytes,
                                                  stw_types::uint32 & oru32_MaxBufSize) const;
   virtual stw_types::sint32 SER_Send_Bytes(const stw_types::uint8 * opu8_Data, const stw_types::uint32 ou32_NumBytes);
   virtual stw_types::sint32 SER_Get_RX_Buf_Count(stw_types::uint32 & oru32_NumBytes,
                                                  stw_types::uint32 & oru32_MaxBufSize) const;
   virtual stw_types::sint32 SER_Read_Bytes(stw_types::uint8 * const opu8_Data, stw_types::uint32 & oru32_NumBytes);

   //additional functions for Windows STW CAN DLLs
   //DLL handling:
   stw_types::sint32 DLL_Open(void);
   stw_types::sint32 DLL_Open(const stw_scl::C_SCLString & orc_FileName);
   stw_types::sint32 DLL_Close(void);

   stw_types::sint32 CAN_Read_Msg_Timeout(const stw_types::uint32 ou32_MaxWaitTimeMS, T_STWCAN_Msg_RX & orc_Message)
                                          const;

   stw_types::sint32 CAN_InteractiveSetup(void) const;

   stw_types::sint32 CAN_Status(T_STWCAN_Status & orc_Status) const;
   stw_types::sint32 CAN_DLL_Info(T_STWCAN_DLLInfo & orc_Info) const;
   stw_types::sint32 CAN_Get_Supported_Bitrates(stw_scl::C_SCLString & orc_Unit,
                                                stw_scl::SCLDynamicArray<stw_types::uint32> & orc_Bitrates,
                                                stw_types::uint32 & oru32_MultiplicationFactor) const;

   //CANtapi functions do not exist in an "ext" variation yet -> channel member will not be used !!
   stw_types::sint32 CANTAPI_Connect(const stw_types::uint8 * const opu8_Number, const stw_types::uint32 ou32_TimeOut);
   stw_types::sint32 CANTAPI_Disconnect(const stw_types::uint32 ou32_TimeOut);

   //setter/getter functions
   void SetDLLName(const stw_scl::C_SCLString & orc_DLLName);
   stw_scl::C_SCLString GetDLLName(void) const;

   void SetLimitRXID(const stw_types::uint32 ou32_LimitRXID);
   stw_types::uint32 GetLimitRXID(void) const;
};

/* -- Global Variables ---------------------------------------------------------------------------------------------- */
//error codes:
const stw_types::sint32 CAN_COMP_ERR_DLL_NOT_OPENED       =   -201;
const stw_types::sint32 CAN_COMP_ERR_DLL_ALREADY_CLOSED   =   -203;
const stw_types::sint32 CAN_COMP_ERR_DLL_ALREADY_OPENED   =   -204;
const stw_types::sint32 CAN_COMP_ERR_DLL_INIT             =   -205;
const stw_types::sint32 CAN_COMP_ERR_DLL_FORMAT           =   -206;

const stw_types::uint32 CAN_RX_ID_INVALID = 0xFFFFFFFFUL;

/* -- Function Prototypes ------------------------------------------------------------------------------------------- */

/* -- Implementation ------------------------------------------------------------------------------------------------ */

}

#endif
