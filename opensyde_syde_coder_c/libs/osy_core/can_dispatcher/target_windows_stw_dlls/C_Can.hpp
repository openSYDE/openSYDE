//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Header for C_CAN class.

   Header to class implementing a high level interface to communication driver DLLs in the STW-CAN-DLL
   format.

   \copyright   Copyright 2002 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef CCANHPP
#define CCANHPP

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "stwtypes.hpp"
#include "C_CanDll.hpp"
#include "C_CanDispatcher.hpp"
#include "C_SclString.hpp"

#include "C_SclDynamicArray.hpp"

namespace stw
{
namespace can
{
/* -- Defines ------------------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */
///high level interface to STW CANDLLs
class C_Can :
   public C_CanDispatcher
{
private:
   stw::scl::C_SclString mc_DllName;
   C_CanDll * mpc_Can;
   bool mq_DLLOpened;
   uint32_t mu32_RxId;

   int32_t m_ReadMsgFromDLL(T_STWCAN_Msg_RX & orc_Message) const;
   int32_t m_SendMsgToDLL(const T_STWCAN_Msg_TX & orc_Message) const;

protected:
   //function from Dispatcher (mandatory to implement)
   //actual reading from CAN driver
   virtual int32_t m_CAN_Read_Msg(T_STWCAN_Msg_RX & orc_Message);

public:
   C_Can(const C_Can & orc_Souce);               //not implemented -> prevent copying
   C_Can & operator = (const C_Can & orc_Souce); //not implemented -> prevent assignment
   C_Can(void);
   C_Can(const uint8_t ou8_CommChannel);
   virtual ~C_Can(void);

   //functions from base (mandatory to implement):
   virtual int32_t CAN_Init(void); //use bitrate currently configured in CAN driver
   virtual int32_t CAN_Init(const int32_t os32_BitrateKBitS);
   virtual int32_t CAN_Exit(void);
   virtual int32_t CAN_Reset(void);
   virtual int32_t CAN_Send_Msg(const T_STWCAN_Msg_TX & orc_Message);
   virtual int32_t CAN_Get_System_Time(uint64_t & oru64_SystemTimeUs) const;

   //additional functions for Windows STW CAN DLLs
   //DLL handling:
   int32_t DLL_Open(void);
   int32_t DLL_Open(const stw::scl::C_SclString & orc_FileName);
   int32_t DLL_Close(void);

   int32_t CAN_Read_Msg_Timeout(const uint32_t ou32_MaxWaitTimeMs, T_STWCAN_Msg_RX & orc_Message) const;

   int32_t CAN_InteractiveSetup(void) const;

   int32_t CAN_Status(T_STWCAN_Status & orc_Status) const;
   int32_t CAN_DLL_Info(T_STWCAN_DLLInfo & orc_Info) const;
   int32_t CAN_Get_Supported_Bitrates(stw::scl::C_SclString & orc_Unit,
                                      stw::scl::C_SclDynamicArray<uint32_t> & orc_Bitrates,
                                      uint32_t & oru32_MultiplicationFactor) const;

   //setter/getter functions
   void SetDLLName(const stw::scl::C_SclString & orc_DLLName);
   stw::scl::C_SclString GetDLLName(void) const;

   void SetLimitRXID(const uint32_t ou32_LimitRXID);
   uint32_t GetLimitRXID(void) const;
};

/* -- Global Variables ---------------------------------------------------------------------------------------------- */
//error codes:
const int32_t CAN_COMP_ERR_DLL_NOT_OPENED       =   -201;
const int32_t CAN_COMP_ERR_DLL_ALREADY_CLOSED   =   -203;
const int32_t CAN_COMP_ERR_DLL_ALREADY_OPENED   =   -204;
const int32_t CAN_COMP_ERR_DLL_INIT             =   -205;
const int32_t CAN_COMP_ERR_DLL_FORMAT           =   -206;

const uint32_t CAN_RX_ID_INVALID = 0xFFFFFFFFUL;

/* -- Function Prototypes ------------------------------------------------------------------------------------------- */

/* -- Implementation ------------------------------------------------------------------------------------------------ */
}
}
#endif
