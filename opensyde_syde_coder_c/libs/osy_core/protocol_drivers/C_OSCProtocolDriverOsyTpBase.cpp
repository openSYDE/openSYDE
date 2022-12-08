//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       openSYDE protocol transport-protocol abstract class implementation

   For details cf. documentation in .h file.

   \copyright   Copyright 2016 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.hpp"

#include <iostream>
#include "stwtypes.hpp"
#include "stwerrors.hpp"
#include "C_OscProtocolDriverOsyTpBase.hpp"
#include "C_SclString.hpp"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */

using namespace stw::errors;
using namespace stw::opensyde_core;
using namespace stw::scl;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

/* -- Global Variables ---------------------------------------------------------------------------------------------- */

/* -- Module Global Variables --------------------------------------------------------------------------------------- */

/* -- Module Global Function Prototypes ----------------------------------------------------------------------------- */

/* -- Implementation ------------------------------------------------------------------------------------------------ */

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   constructor

   Initialize with default values
*/
//----------------------------------------------------------------------------------------------------------------------
C_OscProtocolDriverOsyNode::C_OscProtocolDriverOsyNode(void) :
   u8_BusIdentifier(0U),
   u8_NodeIdentifier(0U)
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   constructor

   Initialize with specified values

   \param[in] ou8_BusIdentifier    bus ID init value
   \param[in] ou8_NodeIdentifier   node ID init value
*/
//----------------------------------------------------------------------------------------------------------------------
C_OscProtocolDriverOsyNode::C_OscProtocolDriverOsyNode(const uint8_t ou8_BusIdentifier,
                                                       const uint8_t ou8_NodeIdentifier) :
   u8_BusIdentifier(ou8_BusIdentifier),
   u8_NodeIdentifier(ou8_NodeIdentifier)
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Check if current equal to orc_Cmp

   \param[in] orc_Cmp Compared instance

   \return
   Current equal to orc_Cmp
   Else false
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_OscProtocolDriverOsyNode::operator ==(const C_OscProtocolDriverOsyNode & orc_Cmp) const
{
   bool q_Return = true;

   if ((this->u8_BusIdentifier != orc_Cmp.u8_BusIdentifier) ||
       (this->u8_NodeIdentifier != orc_Cmp.u8_NodeIdentifier))
   {
      q_Return = false;
   }

   return q_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Check if current is not equal to orc_Cmp

   \param[in] orc_Cmp Compared instance

   \return
   Current not equal to orc_Cmp
   Else false
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_OscProtocolDriverOsyNode::operator !=(const C_OscProtocolDriverOsyNode & orc_Cmp) const
{
   bool q_Return = true;

   if ((this->u8_BusIdentifier == orc_Cmp.u8_BusIdentifier) &&
       (this->u8_NodeIdentifier == orc_Cmp.u8_NodeIdentifier))
   {
      q_Return = false;
   }

   return q_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief    Less operator.

   \param[in] orc_Cmp Compared instance

   \return
   true     Current smaller than orc_Cmp
   false    Else
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_OscProtocolDriverOsyNode::operator <(const C_OscProtocolDriverOsyNode & orc_Cmp) const
{
   bool q_Return;

   if (this->u8_BusIdentifier > orc_Cmp.u8_BusIdentifier)
   {
      q_Return = false;
   }
   else if ((this->u8_BusIdentifier == orc_Cmp.u8_BusIdentifier) &&
            (this->u8_NodeIdentifier >= orc_Cmp.u8_NodeIdentifier))
   {
      q_Return = false;
   }
   else
   {
      q_Return = true;
   }

   return q_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   add new service to Tx queue

   Add specified service to Tx queue for outgoing requests.
   During the operation the queue will be locked by a critical section.

   \param[in] orc_Service         service to add to queue

   \return
   C_NO_ERR    service added
   C_RANGE     service size out of range (maximum: 4095 bytes)
   C_OVERFLOW  Tx queue is already full
   C_NOACT     could not add to queue (out of memory; should not happen in real life)
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_OscProtocolDriverOsyTpBase::m_AddToTxQueue(const C_OscProtocolDriverOsyService & orc_Service)
{
   int32_t s32_Return = C_NO_ERR;

   if (orc_Service.c_Data.size() > hu16_OSY_MAXIMUM_SERVICE_SIZE)
   {
      s32_Return = C_RANGE;
   }
   else
   {
      mc_CsTxQueue.Acquire();
      if (mc_TxQueue.size() >= mu16_MaxServiceQueueSize)
      {
         s32_Return = C_OVERFLOW;
      }
      else
      {
         try
         {
            mc_TxQueue.push_back(orc_Service);
         }
         catch (...)
         {
            s32_Return = C_NOACT; //probably out of memory
         }
      }
      mc_CsTxQueue.Release();
   }
   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   add new service to Rx queue

   Add specified service to Rx queue for incoming responses.
   During the operation the queue will be locked by a critical section.

   \param[in] orc_Service         service to add to queue

   \return
   C_NO_ERR    service added
   C_RANGE     service size out of range
   C_OVERFLOW  Rx queue is already full
   C_NOACT     could not add to queue (out of memory; should not happen in real life)
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_OscProtocolDriverOsyTpBase::m_AddToRxQueue(const C_OscProtocolDriverOsyService & orc_Service)
{
   int32_t s32_Return = C_NO_ERR;

   if (orc_Service.c_Data.size() > hu16_OSY_MAXIMUM_SERVICE_SIZE)
   {
      s32_Return = C_RANGE;
   }
   else
   {
      mc_CsRxQueue.Acquire();
      if (mc_RxQueue.size() >= mu16_MaxServiceQueueSize)
      {
         s32_Return = C_OVERFLOW;
      }
      else
      {
         try
         {
            mc_RxQueue.push_back(orc_Service);
         }
         catch (...)
         {
            s32_Return = C_NOACT; //probably out of memory
         }
      }
      mc_CsRxQueue.Release();
   }
   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   get "oldest" entry from Tx queue

   Get service from Tx queue for outgoing requests.
   During the operation the queue will be locked by a critical section.

   \param[out] orc_Service         read service

   \return
   C_NO_ERR    service read; data in orc_Service
   C_NOACT     queue is empty
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_OscProtocolDriverOsyTpBase::m_GetFromTxQueue(C_OscProtocolDriverOsyService & orc_Service)
{
   int32_t s32_Return = C_NO_ERR;

   mc_CsTxQueue.Acquire();
   if (mc_TxQueue.size() < 1U)
   {
      s32_Return = C_NOACT;
   }
   else
   {
      orc_Service = mc_TxQueue.front(); //get element from queue
      mc_TxQueue.pop_front();           //delete element from queue
   }
   mc_CsTxQueue.Release();
   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   get "oldest" entry from Rx queue

   Get service from Rx queue for incoming responses.
   During the operation the queue will be locked by a critical section.

   \param[out] orc_Service         read service

   \return
   C_NO_ERR    service read; data in orc_Service
   C_NOACT     queue is empty
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_OscProtocolDriverOsyTpBase::m_GetFromRxQueue(C_OscProtocolDriverOsyService & orc_Service)
{
   int32_t s32_Return = C_NO_ERR;

   mc_CsRxQueue.Acquire();
   if (mc_RxQueue.size() < 1U)
   {
      s32_Return = C_NOACT;
   }
   else
   {
      orc_Service = mc_RxQueue.front(); //get element from queue
      mc_RxQueue.pop_front();           //delete element from queue
   }
   mc_CsRxQueue.Release();
   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Clear Rx and Tx service queues

   Remove all elements from Tx and Tx service queues
   During the operation each of the queues will be locked by a critical section.
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OscProtocolDriverOsyTpBase::ClearServiceQueues(void)
{
   mc_CsRxQueue.Acquire();
   mc_RxQueue.clear();
   mc_CsRxQueue.Release();
   mc_CsTxQueue.Acquire();
   mc_TxQueue.clear();
   mc_CsTxQueue.Release();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set timeout for waiting for broadcast responses

   Set time period to wait when collecting responses to broadcast requests.

   \param[in]    ou32_TimeoutMs   number of ms to wait for responses
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OscProtocolDriverOsyTpBase::SetBroadcastTimeout(const uint32_t ou32_TimeoutMs)
{
   mu32_BroadcastTimeoutMs = ou32_TimeoutMs;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   constructor

   Initialize class fields

   \param[in]  ou16_MaxServiceQueueSize  maximum number of service queue entries for Tx and Rx queue
*/
//----------------------------------------------------------------------------------------------------------------------
C_OscProtocolDriverOsyTpBase::C_OscProtocolDriverOsyTpBase(const uint16_t ou16_MaxServiceQueueSize) :
   mu16_MaxServiceQueueSize(ou16_MaxServiceQueueSize),
   mu32_BroadcastTimeoutMs(1000U)
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   destructor

   Clean up
*/
//----------------------------------------------------------------------------------------------------------------------
C_OscProtocolDriverOsyTpBase::~C_OscProtocolDriverOsyTpBase(void)
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Checks the connection of the TCP socket

   \return
   C_NO_ERR   is connected
   C_NOACT    is not connected
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_OscProtocolDriverOsyTpBase::IsConnected(void)
{
   return C_NO_ERR;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   re-connect to device

   Re-connect to device.
   e.g. after it was reset.
   No action required for connection-less protocols (e.g. CAN)

   \return
   C_NO_ERR    re-connection established (or not required)
   C_BUSY      re-connection failed
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_OscProtocolDriverOsyTpBase::ReConnect(void)
{
   return C_NO_ERR;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Disconnect from server

   \return
   C_NO_ERR    disconnect established (or not required)
   C_NOACT     disconnect failed
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_OscProtocolDriverOsyTpBase::Disconnect(void)
{
   return C_NO_ERR;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Send service request

   Add service request to Tx queue for sending.
   Actual sending will not be performed here (use C_OscProtocolDriverOsyTpBase::Cycle).

   \param[in]     orc_Request      service request to add to queue

   \return
   C_NO_ERR    service added
   C_RANGE     service size out of range
   C_OVERFLOW  Tx queue is already full
   C_NOACT     could not add to queue (out of memory; should not happen in real life)
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_OscProtocolDriverOsyTpBase::SendRequest(const C_OscProtocolDriverOsyService & orc_Request)
{
   return m_AddToTxQueue(orc_Request);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Read service response from Rx queue

   Get "oldest" incoming service request from Rx queue.
   The function does not perform to actual reception from the CAN bus (use C_OscProtocolDriverOsyTpBase::Cycle).

   \param[out]     orc_Response      incoming service response

   \return
   C_NO_ERR    service read; date in orc_Service
   C_NOACT     queue is empty
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_OscProtocolDriverOsyTpBase::ReadResponse(C_OscProtocolDriverOsyService & orc_Response)
{
   return m_GetFromRxQueue(orc_Response);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Make server and client identifiers known to TP.

   * remember IDs

   If the specific TP needs to perform more actions when the IDs changes then
   - overload this function
   - call the base function in the overloaded one
   - implement the specifics in the derived class

   \param[in]  orc_ClientIdentifier   new client (i.e.: our own) identifier
   \param[in]  orc_ServerIdentifier   new server identifier

   \return
   C_NO_ERR   no problems
   C_RANGE    client and/or server identifier out of range
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_OscProtocolDriverOsyTpBase::SetNodeIdentifiers(const C_OscProtocolDriverOsyNode & orc_ClientIdentifier,
                                                         const C_OscProtocolDriverOsyNode & orc_ServerIdentifier)
{
   int32_t s32_Return = C_NO_ERR;

   //valid range ?
   if ((orc_ClientIdentifier.u8_BusIdentifier > C_OscProtocolDriverOsyNode::mhu8_MAX_BUS) ||
       (orc_ClientIdentifier.u8_NodeIdentifier > C_OscProtocolDriverOsyNode::mhu8_MAX_NODE) ||
       (orc_ServerIdentifier.u8_BusIdentifier > C_OscProtocolDriverOsyNode::mhu8_MAX_BUS) ||
       (orc_ServerIdentifier.u8_NodeIdentifier > C_OscProtocolDriverOsyNode::mhu8_MAX_NODE))
   {
      s32_Return = C_RANGE;
   }
   else
   {
      mc_ClientId = orc_ClientIdentifier;
      mc_ServerId = orc_ServerIdentifier;
   }

   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Read used server and client identifiers

   \param[out]  orc_ClientIdentifier   configured client (i.e.: our own) identifier
   \param[out]  orc_ServerIdentifier   configured server identifier
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OscProtocolDriverOsyTpBase::GetNodeIdentifiers(C_OscProtocolDriverOsyNode & orc_ClientIdentifier,
                                                      C_OscProtocolDriverOsyNode & orc_ServerIdentifier)
{
   orc_ClientIdentifier = mc_ClientId;
   orc_ServerIdentifier = mc_ServerId;
}
