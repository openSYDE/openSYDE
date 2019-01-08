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

class C_OSCComDriverBase
{
public:
   C_OSCComDriverBase(void);
   virtual ~C_OSCComDriverBase(void);

   stw_types::sint32 InitBase(stw_can::C_CAN_Dispatcher * const opc_CanDispatcher);

   void RegisterLogger(C_OSCComMessageLogger * const opc_Logger);

   stw_types::sint32 StartLogging(const bool oq_InitCan);
   void StopLogging(const bool oq_ExitCan);

   void ContinueLogging(void);
   void PauseLogging(void);

   virtual void DistributeMessages(void);
   virtual void SendCanMessageQueued(const stw_can::T_STWCAN_Msg_TX & orc_Msg);
   stw_types::sint32 SendCanMessage(const stw_can::T_STWCAN_Msg_TX & orc_Msg);

   virtual void PrepareForDestruction(void);

protected:
   virtual void m_HandleCanMessage(const stw_can::T_STWCAN_Msg_RX & orc_Msg, const bool oq_IsTx);

   stw_can::C_CAN_Dispatcher * mpc_CanDispatcher; ///< Holds created instance of CAN dispatcher

private:
   //Avoid call
   C_OSCComDriverBase(const C_OSCComDriverBase &);
   C_OSCComDriverBase & operator =(const C_OSCComDriverBase &);

   // Handling the CAN message logging and monitoring
   std::vector<C_OSCComMessageLogger *> mc_Logger;
   stw_types::uint16 mu16_DispatcherClientHandle;
   bool mq_Started;
   bool mq_Paused;

   // Sending of CAN messages
   std::list<stw_can::T_STWCAN_Msg_TX> mc_CanMessages;
};

/* -- Extern Global Variables ---------------------------------------------- */
} //end of namespace

#endif
