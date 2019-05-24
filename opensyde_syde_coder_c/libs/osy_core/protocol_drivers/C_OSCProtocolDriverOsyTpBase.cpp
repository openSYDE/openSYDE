//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       openSYDE protocol transport-protocol abstract class implementation

   For details cf. documentation in .h file.

   \copyright   Copyright 2016 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------- */
#include "precomp_headers.h"

#include <iostream>
#include "stwtypes.h"
#include "stwerrors.h"
#include "C_OSCProtocolDriverOsyTpBase.h"
#include "CSCLString.h"

/* -- Used Namespaces ------------------------------------------------------ */
using namespace stw_types;
using namespace stw_errors;
using namespace stw_opensyde_core;
using namespace stw_scl;

/* -- Module Global Constants ---------------------------------------------- */

/* -- Types ---------------------------------------------------------------- */

/* -- Global Variables ----------------------------------------------------- */

/* -- Module Global Variables ---------------------------------------------- */

/* -- Module Global Function Prototypes ------------------------------------ */

/* -- Implementation ------------------------------------------------------- */

//-----------------------------------------------------------------------------
/*! \brief   constructor

   Initialize with default values
*/
//-----------------------------------------------------------------------------
C_OSCProtocolDriverOsyNode::C_OSCProtocolDriverOsyNode(void) :
   u8_BusIdentifier(0U),
   u8_NodeIdentifier(0U)
{
}

//-----------------------------------------------------------------------------
/*! \brief   constructor

   Initialize with specified values

   \param[in] ou8_BusIdentifier    bus ID init value
   \param[in] ou8_NodeIdentifier   node ID init value
*/
//-----------------------------------------------------------------------------
C_OSCProtocolDriverOsyNode::C_OSCProtocolDriverOsyNode(const uint8 ou8_BusIdentifier, const uint8 ou8_NodeIdentifier) :
   u8_BusIdentifier(ou8_BusIdentifier),
   u8_NodeIdentifier(ou8_NodeIdentifier)
{
}

//-----------------------------------------------------------------------------
/*! \brief   Check if current equal to orc_Cmp

   \param[in] orc_Cmp Compared instance

   \return
   Current equal to orc_Cmp
   Else false
*/
//-----------------------------------------------------------------------------
bool C_OSCProtocolDriverOsyNode::operator ==(const C_OSCProtocolDriverOsyNode & orc_Cmp) const
{
   bool q_Return = true;

   if ((this->u8_BusIdentifier != orc_Cmp.u8_BusIdentifier) ||
       (this->u8_NodeIdentifier != orc_Cmp.u8_NodeIdentifier))
   {
      q_Return = false;
   }

   return q_Return;
}

//-----------------------------------------------------------------------------
/*! \brief   Check if current is not equal to orc_Cmp

   \param[in] orc_Cmp Compared instance

   \return
   Current not equal to orc_Cmp
   Else false
*/
//-----------------------------------------------------------------------------
bool C_OSCProtocolDriverOsyNode::operator !=(const C_OSCProtocolDriverOsyNode & orc_Cmp) const
{
   bool q_Return = true;

   if ((this->u8_BusIdentifier == orc_Cmp.u8_BusIdentifier) &&
       (this->u8_NodeIdentifier == orc_Cmp.u8_NodeIdentifier))
   {
      q_Return = false;
   }

   return q_Return;
}

//-----------------------------------------------------------------------------
/*! \brief    Less operator.

   \param[in] orc_Cmp Compared instance

   \return
   true     Current smaller than orc_Cmp
   false    Else
*/
//-----------------------------------------------------------------------------
bool C_OSCProtocolDriverOsyNode::operator <(const C_OSCProtocolDriverOsyNode & orc_Cmp) const
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

//-----------------------------------------------------------------------------
/*! \brief   add new service to TX queue

   Add specified service to TX queue for outgoing requests.
   During the operation the queue will be locked by a critical section.

   \param[in] orc_Service         service to add to queue

   \return
   C_NO_ERR    service added
   C_RANGE     service size out of range (maximum: 4095 bytes)
   C_OVERFLOW  TX queue is already full
   C_NOACT     could not add to queue (out of memory; should not happen in real life)
*/
//-----------------------------------------------------------------------------
sint32 C_OSCProtocolDriverOsyTpBase::m_AddToTxQueue(const C_OSCProtocolDriverOsyService & orc_Service)
{
   sint32 s32_Return = C_NO_ERR;

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

//-----------------------------------------------------------------------------
/*! \brief   add new service to RX queue

   Add specified service to RX queue for incoming responses.
   During the operation the queue will be locked by a critical section.

   \param[in] orc_Service         service to add to queue

   \return
   C_NO_ERR    service added
   C_RANGE     service size out of range
   C_OVERFLOW  RX queue is already full
   C_NOACT     could not add to queue (out of memory; should not happen in real life)
*/
//-----------------------------------------------------------------------------
sint32 C_OSCProtocolDriverOsyTpBase::m_AddToRxQueue(const C_OSCProtocolDriverOsyService & orc_Service)
{
   sint32 s32_Return = C_NO_ERR;

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

//-----------------------------------------------------------------------------
/*! \brief   get "oldest" entry from TX queue

   Get service from TX queue for outgoing requests.
   During the operation the queue will be locked by a critical section.

   \param[out] orc_Service         read service

   \return
   C_NO_ERR    service read; data in orc_Service
   C_NOACT     queue is empty
*/
//-----------------------------------------------------------------------------
sint32 C_OSCProtocolDriverOsyTpBase::m_GetFromTxQueue(C_OSCProtocolDriverOsyService & orc_Service)
{
   sint32 s32_Return = C_NO_ERR;

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

//-----------------------------------------------------------------------------
/*! \brief   get "oldest" entry from RX queue

   Get service from RX queue for incoming responses.
   During the operation the queue will be locked by a critical section.

   \param[out] orc_Service         read service

   \return
   C_NO_ERR    service read; data in orc_Service
   C_NOACT     queue is empty
*/
//-----------------------------------------------------------------------------
sint32 C_OSCProtocolDriverOsyTpBase::m_GetFromRxQueue(C_OSCProtocolDriverOsyService & orc_Service)
{
   sint32 s32_Return = C_NO_ERR;

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

//-----------------------------------------------------------------------------
/*! \brief   Clear RX and TX service queues

   Remove all elements from TX and TX service queues
   During the operation each of the queues will be locked by a critical section.
*/
//-----------------------------------------------------------------------------
void C_OSCProtocolDriverOsyTpBase::ClearServiceQueues(void)
{
   mc_CsRxQueue.Acquire();
   mc_RxQueue.clear();
   mc_CsRxQueue.Release();
   mc_CsTxQueue.Acquire();
   mc_TxQueue.clear();
   mc_CsTxQueue.Release();
}

//-----------------------------------------------------------------------------
/*! \brief   Set timeout for waiting for broadcast responses

   Set time period to wait when collecting responses to broadcast requests.

   \param[in]    ou32_TimeoutMs   number of ms to wait for responses
*/
//-----------------------------------------------------------------------------
void C_OSCProtocolDriverOsyTpBase::SetBroadcastTimeout(const uint32 ou32_TimeoutMs)
{
   mu32_BroadcastTimeoutMs = ou32_TimeoutMs;
}

//-----------------------------------------------------------------------------
/*! \brief   constructor

   Initialize class fields

   \param[in]  ou16_MaxServiceQueueSize  maximum number of service queue entries for TX and RX queue
*/
//-----------------------------------------------------------------------------
C_OSCProtocolDriverOsyTpBase::C_OSCProtocolDriverOsyTpBase(const uint16 ou16_MaxServiceQueueSize) :
   mu16_MaxServiceQueueSize(ou16_MaxServiceQueueSize),
   mu32_BroadcastTimeoutMs(1000U)
{
}

//-----------------------------------------------------------------------------
/*! \brief   destructor

   Clean up
*/
//-----------------------------------------------------------------------------
C_OSCProtocolDriverOsyTpBase::~C_OSCProtocolDriverOsyTpBase(void)
{
}

//-----------------------------------------------------------------------------
/*! \brief   Checks the connection of the TCP socket

   \return
   C_NO_ERR   is connected
   C_NOACT    is not connected
*/
//-----------------------------------------------------------------------------
sint32 C_OSCProtocolDriverOsyTpBase::IsConnected(void)
{
   return C_NO_ERR;
}

//-----------------------------------------------------------------------------
/*! \brief   re-connect to device

   Re-connect to device.
   e.g. after it was reset.
   No action required for connection-less protocols (e.g. CAN)

   \return
   C_NO_ERR    re-connection established (or not required)
   C_BUSY      re-connection failed
*/
//-----------------------------------------------------------------------------
sint32 C_OSCProtocolDriverOsyTpBase::ReConnect(void)
{
   return C_NO_ERR;
}

//-----------------------------------------------------------------------------
/*! \brief   Disconnect from server

   \return
   C_NO_ERR    disconnect established (or not required)
   C_NOACT     disconnect failed
*/
//-----------------------------------------------------------------------------
sint32 C_OSCProtocolDriverOsyTpBase::Disconnect(void)
{
   return C_NO_ERR;
}

//-----------------------------------------------------------------------------
/*! \brief   Send service request

   Add service request to TX queue for sending.
   Actual sending will not be performed here (use C_OSCProtocolDriverOsyTpBase::Cycle).

   \param[in]     orc_Request      service request to add to queue

   \return
   C_NO_ERR    service added
   C_RANGE     service size out of range
   C_OVERFLOW  TX queue is already full
   C_NOACT     could not add to queue (out of memory; should not happen in real life)
*/
//-----------------------------------------------------------------------------
sint32 C_OSCProtocolDriverOsyTpBase::SendRequest(const C_OSCProtocolDriverOsyService & orc_Request)
{
   return m_AddToTxQueue(orc_Request);
}

//-----------------------------------------------------------------------------
/*! \brief   Read service response from RX queue

   Get "oldest" incoming service request from RX queue.
   The function does not perform to actual reception from the CAN bus (use C_OSCProtocolDriverOsyTpBase::Cycle).

   \param[out]     orc_Response      incoming service response

   \return
   C_NO_ERR    service read; date in orc_Service
   C_NOACT     queue is empty
*/
//-----------------------------------------------------------------------------
sint32 C_OSCProtocolDriverOsyTpBase::ReadResponse(C_OSCProtocolDriverOsyService & orc_Response)
{
   return m_GetFromRxQueue(orc_Response);
}

//-----------------------------------------------------------------------------
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
//-----------------------------------------------------------------------------
sint32 C_OSCProtocolDriverOsyTpBase::SetNodeIdentifiers(const C_OSCProtocolDriverOsyNode & orc_ClientIdentifier,
                                                        const C_OSCProtocolDriverOsyNode & orc_ServerIdentifier)
{
   sint32 s32_Return = C_NO_ERR;

   //valid range ?
   if ((orc_ClientIdentifier.u8_BusIdentifier > C_OSCProtocolDriverOsyNode::mhu8_MAX_BUS) ||
       (orc_ClientIdentifier.u8_NodeIdentifier > C_OSCProtocolDriverOsyNode::mhu8_MAX_NODE) ||
       (orc_ServerIdentifier.u8_BusIdentifier > C_OSCProtocolDriverOsyNode::mhu8_MAX_BUS) ||
       (orc_ServerIdentifier.u8_NodeIdentifier > C_OSCProtocolDriverOsyNode::mhu8_MAX_NODE))
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
