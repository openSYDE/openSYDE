//-----------------------------------------------------------------------------
/*!
   \file
   \brief       openSYDE protocol transport-protocol abstract class definition

   \class       stw_opensyde_core::C_OSCProtocolDriverOsyTpBase
   \brief       openSYDE protocol transport-protocol abstract class definition

   Abstract base class defining function for packing, unpacking, sending, receiving services.
   All elements and functionality common to all transport protocols are contained here.
   Elements and functionality that is specific to the transport protocol needs to be implemented there.

   \implementation
   project     openSYDE
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     24.02.2016  STW/A.Stangl
   \endimplementation
*/
//-----------------------------------------------------------------------------
#ifndef C_OSCPROTOCOLDRIVEROSYTPBASE
#define C_OSCPROTOCOLDRIVEROSYTPBASE

/* -- Includes ------------------------------------------------------------- */
#include <vector>
#include <deque>
#include "stwtypes.h"
#include "TGLTasks.h"
#include "CSCLString.h"

/* -- Namespace ------------------------------------------------------------ */
namespace stw_opensyde_core
{
/* -- Global Constants ----------------------------------------------------- */

/* -- Types ---------------------------------------------------------------- */

///container for one openSYDE protocol service
class C_OSCProtocolDriverOsyService
{
public:
   C_OSCProtocolDriverOsyService() :
      q_CanTransferWithoutFlowControl(false)
   {
   }

   std::vector<stw_types::uint8> c_Data;
   bool q_CanTransferWithoutFlowControl; // Flag if service shall use openSYDE Multi Frame transfer
};

//-----------------------------------------------------------------------------
///identifier of one node
class C_OSCProtocolDriverOsyNode
{
public:
   static const stw_types::uint8 mhu8_MAX_BUS    = 0x0FU; ///< Maximum count of buses
   static const stw_types::uint8 mhu8_MAX_NODE   = 0x7FU; ///< Maximum count of nodes is mhu8_MAX_NODE - 1
   ///< and mhu8_MAX_NODE is reserved for broadcasts

   static const stw_types::uint8 mhu8_NODE_ID_BROADCASTS = 0x7FU;

   C_OSCProtocolDriverOsyNode(void);
   C_OSCProtocolDriverOsyNode(const stw_types::uint8 ou8_BusIdentifier, const stw_types::uint8 ou8_NodeIdentifier);
   stw_types::uint8 u8_BusIdentifier;
   stw_types::uint8 u8_NodeIdentifier;

   bool operator ==(const C_OSCProtocolDriverOsyNode & orc_Cmp) const;
   bool operator !=(const C_OSCProtocolDriverOsyNode & orc_Cmp) const;
   bool operator < (const C_OSCProtocolDriverOsyNode & orc_Cmp) const;
};

//-----------------------------------------------------------------------------
///interface class
class C_OSCProtocolDriverOsyTpBase
{
private:
   //not implemented -> prevent copying
   C_OSCProtocolDriverOsyTpBase(const C_OSCProtocolDriverOsyTpBase & orc_Source);
   //not implemented -> prevent assignment
   C_OSCProtocolDriverOsyTpBase & operator = (const C_OSCProtocolDriverOsyTpBase & orc_Source);

   stw_types::uint16 mu16_MaxServiceQueueSize;
   std::deque<C_OSCProtocolDriverOsyService> mc_TxQueue;
   std::deque<C_OSCProtocolDriverOsyService> mc_RxQueue;
   stw_tgl::C_TGLCriticalSection mc_CsTxQueue; ///critical section protecting TX Queue
   stw_tgl::C_TGLCriticalSection mc_CsRxQueue; ///critical section protecting RX Queue

protected:
   C_OSCProtocolDriverOsyNode mc_ClientId;    ///< our own client ID
   C_OSCProtocolDriverOsyNode mc_ServerId;    ///< ID of server node to communicate with
   stw_types::uint32 mu32_BroadcastTimeoutMs; ///< timeout for collecting responses to broadcasts

   stw_types::sint32 m_AddToTxQueue(const C_OSCProtocolDriverOsyService & orc_Service);
   stw_types::sint32 m_AddToRxQueue(const C_OSCProtocolDriverOsyService & orc_Service);
   stw_types::sint32 m_GetFromTxQueue(C_OSCProtocolDriverOsyService & orc_Service);
   stw_types::sint32 m_GetFromRxQueue(C_OSCProtocolDriverOsyService & orc_Service);

public:
   ///common parameters for the different transport protocols
   static const stw_types::uint8 hu8_OSY_RESET_TYPE_KEY_OFF_ON           = 0x02U;
   static const stw_types::uint8 hu8_OSY_RESET_TYPE_RESET_TO_FLASHLOADER = 0x60U;
   static const stw_types::uint16 hu16_OSY_MAXIMUM_SERVICE_SIZE          = 4095U;

   C_OSCProtocolDriverOsyTpBase(const stw_types::uint16 ou16_MaxServiceQueueSize);
   virtual ~C_OSCProtocolDriverOsyTpBase(void);

   virtual stw_types::sint32 IsConnected(void);
   virtual stw_types::sint32 ReConnect(void);
   virtual stw_types::sint32 Disconnect(void);

   stw_types::sint32 SendRequest(const C_OSCProtocolDriverOsyService & orc_Request);
   stw_types::sint32 ReadResponse(C_OSCProtocolDriverOsyService & orc_Response);
   void ClearServiceQueues(void);

   void SetBroadcastTimeout(const stw_types::uint32 ou32_TimeoutMs);

   virtual stw_types::sint32 Cycle(void) = 0;
   virtual stw_types::sint32 SetNodeIdentifiers(const C_OSCProtocolDriverOsyNode & orc_ClientIdentifier,
                                                const C_OSCProtocolDriverOsyNode & orc_ServerIdentifier);
};

/* -- Extern Global Variables ---------------------------------------------- */
}

#endif
