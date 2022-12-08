//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       openSYDE protocol transport-protocol abstract class definition

   \class       stw::opensyde_core::C_OscProtocolDriverOsyTpBase
   \brief       openSYDE protocol transport-protocol abstract class definition

   Abstract base class defining function for packing, unpacking, sending, receiving services.
   All elements and functionality common to all transport protocols are contained here.
   Elements and functionality that is specific to the transport protocol needs to be implemented there.

   \copyright   Copyright 2016 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_OSCPROTOCOLDRIVEROSYTPBASE
#define C_OSCPROTOCOLDRIVEROSYTPBASE

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <vector>
#include <deque>
#include "stwtypes.hpp"
#include "TglTasks.hpp"
#include "C_SclString.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_core
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

///container for one openSYDE protocol service
class C_OscProtocolDriverOsyService
{
public:
   C_OscProtocolDriverOsyService() :
      q_CanTransferWithoutFlowControl(false)
   {
   }

   std::vector<uint8_t> c_Data;
   bool q_CanTransferWithoutFlowControl; // Flag if service shall use openSYDE Multi Frame transfer
};

//----------------------------------------------------------------------------------------------------------------------
///identifier of one node
class C_OscProtocolDriverOsyNode
{
public:
   static const uint8_t mhu8_MAX_BUS    = 0x0FU; ///< Maximum count of buses
   static const uint8_t mhu8_MAX_NODE   = 0x7FU; ///< Maximum count of nodes is mhu8_MAX_NODE - 1
   ///< and mhu8_MAX_NODE is reserved for broadcasts

   static const uint8_t mhu8_NODE_ID_BROADCASTS = 0x7FU;

   C_OscProtocolDriverOsyNode(void);
   C_OscProtocolDriverOsyNode(const uint8_t ou8_BusIdentifier, const uint8_t ou8_NodeIdentifier);
   uint8_t u8_BusIdentifier;
   uint8_t u8_NodeIdentifier;

   bool operator ==(const C_OscProtocolDriverOsyNode & orc_Cmp) const;
   bool operator !=(const C_OscProtocolDriverOsyNode & orc_Cmp) const;
   bool operator < (const C_OscProtocolDriverOsyNode & orc_Cmp) const;
};

//----------------------------------------------------------------------------------------------------------------------
///interface class
class C_OscProtocolDriverOsyTpBase
{
private:
   //not implemented -> prevent copying
   C_OscProtocolDriverOsyTpBase(const C_OscProtocolDriverOsyTpBase & orc_Source);
   //not implemented -> prevent assignment
   C_OscProtocolDriverOsyTpBase & operator = (const C_OscProtocolDriverOsyTpBase & orc_Source);

   uint16_t mu16_MaxServiceQueueSize;
   std::deque<C_OscProtocolDriverOsyService> mc_TxQueue;
   std::deque<C_OscProtocolDriverOsyService> mc_RxQueue;
   stw::tgl::C_TglCriticalSection mc_CsTxQueue; ///critical section protecting Tx Queue
   stw::tgl::C_TglCriticalSection mc_CsRxQueue; ///critical section protecting Rx Queue

protected:
   C_OscProtocolDriverOsyNode mc_ClientId; ///< our own client ID
   C_OscProtocolDriverOsyNode mc_ServerId; ///< ID of server node to communicate with
   uint32_t mu32_BroadcastTimeoutMs;       ///< timeout for collecting responses to broadcasts

   int32_t m_AddToTxQueue(const C_OscProtocolDriverOsyService & orc_Service);
   int32_t m_AddToRxQueue(const C_OscProtocolDriverOsyService & orc_Service);
   int32_t m_GetFromTxQueue(C_OscProtocolDriverOsyService & orc_Service);
   int32_t m_GetFromRxQueue(C_OscProtocolDriverOsyService & orc_Service);

public:
   ///common parameters for the different transport protocols
   static const uint8_t hu8_OSY_RESET_TYPE_KEY_OFF_ON           = 0x02U;
   static const uint8_t hu8_OSY_RESET_TYPE_RESET_TO_FLASHLOADER = 0x60U;
   static const uint16_t hu16_OSY_MAXIMUM_SERVICE_SIZE          = 4095U;

   explicit C_OscProtocolDriverOsyTpBase(const uint16_t ou16_MaxServiceQueueSize);
   virtual ~C_OscProtocolDriverOsyTpBase(void);

   virtual int32_t IsConnected(void);
   virtual int32_t ReConnect(void);
   virtual int32_t Disconnect(void);

   int32_t SendRequest(const C_OscProtocolDriverOsyService & orc_Request);
   int32_t ReadResponse(C_OscProtocolDriverOsyService & orc_Response);
   void ClearServiceQueues(void);

   void SetBroadcastTimeout(const uint32_t ou32_TimeoutMs);

   //-----------------------------------------------------------------------------
   /*!
      \brief   Perform cyclic communication tasks

      Perform cyclic communication tasks.
      - send services from Tx queue
      - read incoming services and place in Rx queue

      Configuration problems will be reported via the function return value.
      Ongoing communication problems will be written to the class's log text.

      \return
      C_NO_ERR   cycle finished
      C_CONFIG   no dispatcher installed
      C_COM      communication driver reported error
   */
   //-----------------------------------------------------------------------------
   virtual int32_t Cycle(void) = 0;
   virtual int32_t SetNodeIdentifiers(const C_OscProtocolDriverOsyNode & orc_ClientIdentifier,
                                      const C_OscProtocolDriverOsyNode & orc_ServerIdentifier);
   virtual void GetNodeIdentifiers(C_OscProtocolDriverOsyNode & orc_ClientIdentifier,
                                   C_OscProtocolDriverOsyNode & orc_ServerIdentifier);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
}

#endif
