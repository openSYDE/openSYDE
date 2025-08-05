//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Header for C_CAN class.

   Header to class implementing an high level interface to MPC5200 Linux SocketCAN communication driver.

   \copyright   Copyright 2012 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef CCANHPP
#define CCANHPP

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <sys/time.h>

#include "stwtypes.hpp"
#include "C_CanDispatcher.hpp"
#include "C_SclString.hpp"

namespace stw::can
{
/* -- Defines ------------------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_Can :
   public C_CanDispatcher
{
   //names of inherited functions are specified by base class:
   //lint -estring(8001,*::CAN_Init'*,*::CAN_Exit'*,*::CAN_Reset'*,*::CAN_Read_Msg'*,*::CAN_Send_Msg'*)
   //lint -estring(8001,*::CAN_Get_System_Time'*,*::m_CAN_Read_Msg'*)
   //lint -estring(8010,*::m_CAN_Read_Msg'*)

private:
   C_Can(const C_Can & orc_Souce);               //not implemented -> prevent copying
   C_Can & operator = (const C_Can & orc_Souce); //not implemented -> prevent assignment
   int32_t ms32_Socket;

   // ms32_RxTimeout: Timeout parameter for poll() in m_CAN_Read_Msg.
   // -1: block until message received; 0: return immediately; >0: timeout in ms
   int32_t ms32_RxTimeout;
   uint32_t mu32_RxId;
   stw::scl::C_SclString mc_CanIfName;

   void m_InitClass(const uint8_t ou8_CommChannel);

protected:
   //function from Dispatcher (mandatory to implement)
   //actual reading from CAN driver
   virtual int32_t m_CAN_Read_Msg(T_STWCAN_Msg_RX & orc_Message);
   int32_t m_CanReadMsgTimeout(const uint32_t ou32_MaxWaitTimeMs, T_STWCAN_Msg_RX & orc_Message) const;

public:
   C_Can(void);
   C_Can(const uint8_t ou8_CommChannel); // e.g. ou8_CommChannel=0 -> can0
   virtual ~C_Can(void);

   //functions from base (mandatory to implement):
   virtual int32_t CAN_Init(void);
   // Bitrate is set by interface settings outside the application, thus value of os32_Bitrate is ignored
   virtual int32_t CAN_Init(const int32_t os32_BitrateKBitS);

   virtual int32_t CAN_Exit(void);
   virtual int32_t CAN_Reset(void);
   virtual int32_t CAN_Send_Msg(const T_STWCAN_Msg_TX & orc_Message);
   virtual int32_t CAN_Get_System_Time(uint64_t & oru64_SystemTimeUs) const;

   //additional functions for Linux socket CAN driver
   int32_t CAN_Init(const stw::scl::C_SclString & orc_InterfaceName, const int32_t os32_RxTimeout = 0);

   //lint -e{8001} //keep name so this class can serve as a stand-in replacement for pre-existing Windows applications
   void SetLimitRXID(const uint32_t ou32_LimitRxId);
   //lint -e{8001} //keep name so this class can serve as a stand-in replacement for pre-existing Windows applications
   uint32_t GetLimitRXID(void) const;
};

/* -- Global Variables ---------------------------------------------------------------------------------------------- */
// Error codes for compatibility with Windows implementation, might never be set here
//lint -save -e8002 -e8011 -e8037 -e8062 //keep names to stay compatible with possibly existing applications
const int32_t CAN_COMP_ERR_DLL_NOT_OPENED       =   -201;
const int32_t CAN_COMP_ERR_DLL_ALREADY_CLOSED   =   -203;
const int32_t CAN_COMP_ERR_DLL_ALREADY_OPENED   =   -204;
const int32_t CAN_COMP_ERR_DLL_INIT             =   -205;
const int32_t CAN_COMP_ERR_DLL_FORMAT           =   -206;

const uint32_t CAN_RX_ID_INVALID = 0xFFFFFFFFUL;
//lint -restore

/* -- Function Prototypes ------------------------------------------------------------------------------------------- */

/* -- Implementation ------------------------------------------------------------------------------------------------ */
}

#endif
