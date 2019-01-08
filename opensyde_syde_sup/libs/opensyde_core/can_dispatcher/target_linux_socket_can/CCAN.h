//-----------------------------------------------------------------------------
/*!
   \file        ccan.h
   \brief       Header for C_CAN class.

   Header to class implementing an high level interface to MPC5200 Linux SocketCAN communication driver.

   \implementation
   project     KEFEX
   copyright   STW (c) 2012-2012
   license     use only under terms of contract / confidential

   created     2012-03-09  STW/T.Heilig
   \endimplementation

   \internal
*/
//-----------------------------------------------------------------------------
#ifndef CCANH
#define CCANH

/* -- Includes ------------------------------------------------------------ */
#include <sys/time.h>

#include "stwtypes.h"
#include "CCANDispatcher.h"
#include "CSCLString.h"

namespace stw_can
{
/* -- Defines ------------------------------------------------------------- */
#ifndef STWCAN_PACKAGE
#define STWCAN_PACKAGE
#endif

/* -- Types --------------------------------------------------------------- */

class STWCAN_PACKAGE C_CAN :
   public C_CAN_Dispatcher
{
private:
   C_CAN(const C_CAN & orc_Souce);               //not implemented -> prevent copying
   C_CAN & operator = (const C_CAN & orc_Souce); //not implemented -> prevent assignment
   stw_types::sintn msn_Socket;

   // ms32_RxTimeout: Timeout parameter for poll() in m_CAN_Read_Msg.
   // -1: block until message received; 0: return immediately; >0: timeout in ms
   stw_types::sint32 ms32_RxTimeout;
   stw_types::uint32 mu32_RXID;
   stw_scl::C_SCLString mc_CanIfName;

   void m_InitClass(const stw_types::uint8 ou8_CommChannel);

protected:
   //function from Dispatcher (mandatory to implement)
   //actual reading from CAN driver
   virtual stw_types::sint32 m_CAN_Read_Msg(T_STWCAN_Msg_RX & orc_Message);

public:
   C_CAN(void);
   C_CAN(const stw_types::uint8 ou8_CommChannel); // e.g. ou8_CommChannel=0 -> can0
   virtual ~C_CAN(void);

   virtual stw_types::sint32 CAN_Init(void);
   virtual stw_types::sint32 CAN_Init(const stw_scl::C_SCLString & orc_InterfaceName,
                                      const stw_types::sint32 os32_RxTimeout = 0);
   // Bitrate is set by interface settings outside the application, thus value of os32_Bitrate is ignored
   virtual stw_types::sint32 CAN_Init(const stw_types::sint32 os32_Bitrate);

   virtual stw_types::sint32 CAN_Exit(void);
   virtual stw_types::sint32 CAN_Reset(void);
   virtual stw_types::sint32 CAN_Send_Msg(const T_STWCAN_Msg_TX & orc_Message);
   virtual stw_types::sint32 CAN_Get_System_Time(stw_types::uint64 & oru64_SystemTime) const;

   stw_types::sint32 CAN_Read_Msg_Timeout(const stw_types::uint32 ou32_MaxWaitTimeMS, T_STWCAN_Msg_RX & orc_Message);

   //setter/getter functions
   void SetDLLName(const stw_scl::C_SCLString & orc_DLLName);
   stw_scl::C_SCLString GetDLLName(void) const;

   void SetLimitRXID(const stw_types::uint32 ou32_LimitRXID);
   stw_types::uint32 GetLimitRXID(void) const;
};

/* -- Global Variables ---------------------------------------------------- */
// Error codes for compatibility with Windows implementation, might never be set here
const stw_types::sint32 CAN_COMP_ERR_DLL_NOT_OPENED       =   -201;
const stw_types::sint32 CAN_COMP_ERR_DLL_ALREADY_CLOSED   =   -203;
const stw_types::sint32 CAN_COMP_ERR_DLL_ALREADY_OPENED   =   -204;
const stw_types::sint32 CAN_COMP_ERR_DLL_INIT             =   -205;
const stw_types::sint32 CAN_COMP_ERR_DLL_FORMAT           =   -206;

const stw_types::uint32 CAN_RX_ID_INVALID = 0xFFFFFFFFUL;

/* -- Function Prototypes ------------------------------------------------- */

/* -- Implementation ------------------------------------------------------ */
}

#endif
