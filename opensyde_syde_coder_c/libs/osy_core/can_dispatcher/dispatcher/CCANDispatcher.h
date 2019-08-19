//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       CAN dispatcher class

   Header for CAN dispatcher class

   Provides a standardized reception mechanism for incoming CAN messages.
   Multiple clients can register in order to receive all incoming CAN traffic.

   \copyright   Copyright 2010 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef  CCANDISPATCHERH
#define  CCANDISPATCHERH

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <deque>
#include "stwtypes.h"
#include "CCANBase.h"
#include "TGLTasks.h"
#include "stw_can.h"
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

const stw_types::uint32 mu32_CAN_QUEUE_DEFAULT_MAX_SIZE = 2048U;

/* -- Types --------------------------------------------------------------------------------------------------------- */
///We wrap up a deque in order to provide thread safety.
//Also we limit the maximum size, to make sure we don't waste all of the heap if nobody clears the RX queue.
class STWCAN_PACKAGE C_CAN_RXQueue
{
private:
   std::deque <T_STWCAN_Msg_RX> mc_Messages;
   stw_types::uint32 mu32_MaxSize;
   stw_types::sint32 ms32_Status;
public:
   C_CAN_RXQueue(void);

   stw_types::sint32 Push(const T_STWCAN_Msg_RX & orc_Message);
   stw_types::sint32 Pop(T_STWCAN_Msg_RX & orc_Message);

   void SetMaxSize(const stw_types::uint32 ou32_MaxSize);
   stw_types::uint32 GetMaxSize(void) const;

   stw_types::uint32 GetSize(void) const;
   void Clear(void);

   stw_types::sint32 GetStatus(void);
};

//----------------------------------------------------------------------------------------------------------------------
///Reception filter configuration
//lint -sem(stw_can::C_CAN_RXFilter::PassAll,initializer)
class STWCAN_PACKAGE C_CAN_RXFilter
{
public:
   C_CAN_RXFilter(void);

   stw_types::uint32 u32_Code;   //pass = ((incoming_ID & mask) == code)
   stw_types::uint32 u32_Mask;

   bool q_XTD;           //false: 11bit; true: 29bit
   bool q_XTDMustMatch;
   bool q_RTR;           //false: plain; true: RTR ID
   bool q_RTRMustMatch;

   void PassAll(void);
   void PassOneID(const stw_types::uint32 ou32_ID, const bool oq_ExtID, const bool oq_RTR);

   bool DoesMessagePass(const T_STWCAN_Msg_RX & orc_Message) const;
};

//----------------------------------------------------------------------------------------------------------------------
///Information about one registered dispatch client
class STWCAN_PACKAGE C_CAN_DispatchClient
{
public:
   C_CAN_RXQueue c_RXQueue;
   C_CAN_RXFilter c_RXFilter;
   stw_types::uint16 u16_Handle; //for resyncing with pointer list
};

//----------------------------------------------------------------------------------------------------------------------
///Extends C_CAN_Base by queuing and filtering mechanisms
class STWCAN_PACKAGE C_CAN_Dispatcher : public C_CAN_Base
{
private:
   stw_scl::SCLDynamicArray <C_CAN_DispatchClient *> mc_ClientsByHandle;
   stw_scl::SCLDynamicArray <C_CAN_DispatchClient> mc_InstalledClients;

   void m_ResyncShortcutPointers(void);

   stw_tgl::C_TGLCriticalSection mc_CriticalSection;

protected:
   //-----------------------------------------------------------------------------
   /*!
      \brief   Read one incoming CAN message

      Functions shall read and return one incoming CAN message.
      Must be implemented by the CAN driver.

      \param[out]  orc_Message  read message

      \return
      C_NO_ERR   message read
      else       no message read or error
   */
   //-----------------------------------------------------------------------------
   virtual stw_types::sint32 m_CAN_Read_Msg(T_STWCAN_Msg_RX & orc_Message) = 0;

public:
   C_CAN_Dispatcher(void);
   C_CAN_Dispatcher(const stw_types::uint8 ou8_CommChannel);
   virtual ~C_CAN_Dispatcher(void);

   stw_types::sint32 DispatchIncoming(void);
   stw_types::sint32 RegisterClient(stw_types::uint16 & oru16_Handle, const C_CAN_RXFilter * const opc_RXFilter = NULL,
                                 const stw_types::uint32 & oru32_BufferSize = mu32_CAN_QUEUE_DEFAULT_MAX_SIZE);
   stw_types::sint32 RemoveClient(const stw_types::uint16 ou16_Handle);
   stw_types::sint32 SetRXFilter(const stw_types::uint16 ou16_Handle, const C_CAN_RXFilter & orc_RXFilter);

   stw_types::sint32 ReadFromQueue(const stw_types::uint16 ou16_Handle, T_STWCAN_Msg_RX & orc_Message);
   stw_types::sint32 ClearQueue(const stw_types::uint16 ou16_Handle);

   //lint -e{1411}  we hide the base class function on purpose here
   stw_types::sint32 CAN_Read_Msg(const stw_types::uint16 ou16_Handle, T_STWCAN_Msg_RX & orc_Message);
   //try to read from CAN driver and add message to all installed RX queues:
   virtual stw_types::sint32 CAN_Read_Msg(T_STWCAN_Msg_RX & orc_Message);
};

/* -- Global Variables ---------------------------------------------------------------------------------------------- */

/* -- Function Prototypes ------------------------------------------------------------------------------------------- */

/* -- Implementation ------------------------------------------------------------------------------------------------ */

}

#endif
