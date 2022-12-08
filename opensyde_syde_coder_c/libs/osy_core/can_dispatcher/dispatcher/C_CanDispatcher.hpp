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
#ifndef CCANDISPATCHERHPP
#define CCANDISPATCHERHPP

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <deque>
#include "stwtypes.hpp"
#include "C_CanBase.hpp"
#include "TglTasks.hpp"
#include "stw_can.hpp"
#include "C_SclDynamicArray.hpp"

namespace stw
{
namespace can
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

const uint32_t mu32_CAN_QUEUE_DEFAULT_MAX_SIZE = 2048U;

/* -- Types --------------------------------------------------------------------------------------------------------- */
///We wrap up a deque in order to provide thread safety.
//Also we limit the maximum size, to make sure we don't waste all of the heap if nobody clears the RX queue.
class STWCAN_PACKAGE C_CanRxQueue
{
private:
   std::deque<T_STWCAN_Msg_RX> mc_Messages;
   uint32_t mu32_MaxSize;
   int32_t ms32_Status;

public:
   C_CanRxQueue(void);

   int32_t Push(const T_STWCAN_Msg_RX & orc_Message);
   int32_t Pop(T_STWCAN_Msg_RX & orc_Message);

   void SetMaxSize(const uint32_t ou32_MaxSize);
   uint32_t GetMaxSize(void) const;

   uint32_t GetSize(void) const;
   void Clear(void);

   int32_t GetStatus(void);
};

//----------------------------------------------------------------------------------------------------------------------
///Reception filter configuration
//lint -sem(stw::can::C_CanRxFilter::PassAll,initializer)
class STWCAN_PACKAGE C_CanRxFilter
{
public:
   C_CanRxFilter(void);

   uint32_t u32_Code; //pass = ((incoming_ID & mask) == code)
   uint32_t u32_Mask;

   bool q_XTD; //false: 11bit; true: 29bit
   bool q_XTDMustMatch;
   bool q_RTR; //false: plain; true: RTR ID
   bool q_RTRMustMatch;

   void PassAll(void);
   void PassOneID(const uint32_t ou32_ID, const bool oq_ExtID, const bool oq_RTR);

   bool DoesMessagePass(const T_STWCAN_Msg_RX & orc_Message) const;
};

//----------------------------------------------------------------------------------------------------------------------
///Information about one registered dispatch client
class STWCAN_PACKAGE C_CanDispatchClient
{
public:
   C_CanRxQueue c_RXQueue;
   C_CanRxFilter c_RXFilter;
   uint16_t u16_Handle; //for resyncing with pointer list
};

//----------------------------------------------------------------------------------------------------------------------
///Extends C_CAN_Base by queuing and filtering mechanisms
class STWCAN_PACKAGE C_CanDispatcher :
   public C_CanBase
{
private:
   stw::scl::C_SclDynamicArray<C_CanDispatchClient *> mc_ClientsByHandle;
   stw::scl::C_SclDynamicArray<C_CanDispatchClient> mc_InstalledClients;

   void m_ResyncShortcutPointers(void);

   stw::tgl::C_TglCriticalSection mc_CriticalSection;

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
   virtual int32_t m_CAN_Read_Msg(T_STWCAN_Msg_RX & orc_Message) = 0;

public:
   C_CanDispatcher(void);
   C_CanDispatcher(const uint8_t ou8_CommChannel);
   virtual ~C_CanDispatcher(void);

   int32_t DispatchIncoming(void);
   int32_t RegisterClient(uint16_t & oru16_Handle, const C_CanRxFilter * const opc_RXFilter = NULL,
                          const uint32_t & oru32_BufferSize = mu32_CAN_QUEUE_DEFAULT_MAX_SIZE);
   int32_t RemoveClient(const uint16_t ou16_Handle);
   int32_t SetRXFilter(const uint16_t ou16_Handle, const C_CanRxFilter & orc_RXFilter);

   int32_t ReadFromQueue(const uint16_t ou16_Handle, T_STWCAN_Msg_RX & orc_Message);
   int32_t ClearQueue(const uint16_t ou16_Handle);

   //we hide the base class function on purpose here
   int32_t CAN_Read_Msg(const uint16_t ou16_Handle, T_STWCAN_Msg_RX & orc_Message); //lint !e1411
   //try to read from CAN driver and add message to all installed RX queues:
   virtual int32_t CAN_Read_Msg(T_STWCAN_Msg_RX & orc_Message);
};

/* -- Global Variables ---------------------------------------------------------------------------------------------- */

/* -- Function Prototypes ------------------------------------------------------------------------------------------- */

/* -- Implementation ------------------------------------------------------------------------------------------------ */
}
}
#endif
