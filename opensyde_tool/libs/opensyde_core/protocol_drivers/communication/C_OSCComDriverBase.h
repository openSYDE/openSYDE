//-----------------------------------------------------------------------------
/*!
   \file
   \brief       Core communication driver base class (header)

   See cpp file for detailed description

   \implementation
   project     openSYDE
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     03.09.2018  STW/B.Bayer
   \endimplementation
*/
//-----------------------------------------------------------------------------
#ifndef C_OSCCOMDRIVERBASE_H
#define C_OSCCOMDRIVERBASE_H

/* -- Includes ------------------------------------------------------------- */

#include <vector>
#include <list>

#include "stwtypes.h"
#include "stw_can.h"
#include "CCANDispatcher.h"
#include "C_OSCComMessageLogger.h"

/* -- Namespace ------------------------------------------------------------ */
namespace stw_opensyde_core
{
/* -- Global Constants ----------------------------------------------------- */

/* -- Types ---------------------------------------------------------------- */

class C_OSCComDriverBaseCanMessage
{
public:
   C_OSCComDriverBaseCanMessage(void);
   bool operator ==(const C_OSCComDriverBaseCanMessage & orc_Cmp) const;

   stw_can::T_STWCAN_Msg_TX c_Msg;
   stw_types::uint32 u32_TimeToSend; // First time to send message in ms. Can be used for an offset
   stw_types::uint32 u32_Interval;   // Cyclic interval time in ms
};

class C_OSCComDriverBase
{
public:
   C_OSCComDriverBase(void);
   virtual ~C_OSCComDriverBase(void);

   stw_types::sint32 InitBase(stw_can::C_CAN_Dispatcher * const opc_CanDispatcher);

   void RegisterLogger(C_OSCComMessageLogger * const opc_Logger);

   virtual stw_types::sint32 StartLogging(const stw_types::sint32 os32_Bitrate);
   virtual void StopLogging(void);

   virtual void ContinueLogging(void);
   virtual void PauseLogging(void);

   virtual void UpdateBitrate(const stw_types::sint32 os32_Bitrate);

   virtual void DistributeMessages(void);
   virtual void SendCanMessageQueued(const stw_can::T_STWCAN_Msg_TX & orc_Msg);
   stw_types::sint32 SendCanMessageDirect(const stw_can::T_STWCAN_Msg_TX & orc_Msg);

   virtual void SendCanMessage(const C_OSCComDriverBaseCanMessage & orc_MsgCfg);
   virtual void AddCyclicCanMessage(const C_OSCComDriverBaseCanMessage & orc_MsgCfg);
   virtual void RemoveCyclicCanMessage(const C_OSCComDriverBaseCanMessage & orc_MsgCfg);
   virtual void RemoveAllCyclicCanMessages(void);

   virtual void PrepareForDestruction(void);

protected:
   virtual void m_HandleCanMessage(const stw_can::T_STWCAN_Msg_RX & orc_Msg, const bool oq_IsTx);

   stw_can::C_CAN_Dispatcher * mpc_CanDispatcher; ///< Holds created instance of CAN dispatcher

private:
   //Avoid call
   C_OSCComDriverBase(const C_OSCComDriverBase &);
   C_OSCComDriverBase & operator =(const C_OSCComDriverBase &);

   void m_HandleCanMessagesForSending(void);

   static stw_types::uint32 mh_GetCanMessageSizeInBits(const stw_can::T_STWCAN_Msg_RX & orc_Msg);

   // Handling the CAN message logging and monitoring
   std::vector<C_OSCComMessageLogger *> mc_Logger;
   stw_types::uint16 mu16_DispatcherClientHandle;
   bool mq_Started;
   bool mq_Paused;

   // Sending of CAN messages
   std::list<stw_can::T_STWCAN_Msg_TX> mc_CanMessages;
   // Sending of configured CAN messages
   std::list<C_OSCComDriverBaseCanMessage> mc_CanMessageConfigs;

   // Bus load information
   stw_types::uint32 mu32_CanMessageBits;
   stw_types::sint32 ms32_CanBitrate;

   stw_types::uint32 mu32_CanTxErrrors;
};

/* -- Extern Global Variables ---------------------------------------------- */
} //end of namespace

#endif
