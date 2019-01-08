//-----------------------------------------------------------------------------
/*!
   \internal
   \file
   \brief       Core communication driver base class (implementation)

   Offers a interface to deliver CAN messages to all registered C_OSCMessageLogger instances.
   Distributes all received messages to the registered C_OSCMessageLogger instances.

   \implementation
   project     openSYDE
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     03.09.2018  STW/B.Bayer
   \endimplementation
*/
//-----------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------- */
#include "precomp_headers.h"

#include <cstring>

#include "stwerrors.h"

#include "C_OSCComDriverBase.h"
#include "CSCLString.h"
#include "TGLTime.h"
#include "C_OSCLoggingHandler.h"

/* -- Used Namespaces ------------------------------------------------------ */
using namespace stw_types;
using namespace stw_errors;
using namespace stw_scl;
using namespace stw_can;
using namespace stw_opensyde_core;

/* -- Module Global Constants ---------------------------------------------- */

/* -- Types ---------------------------------------------------------------- */

/* -- Global Variables ----------------------------------------------------- */

/* -- Module Global Variables ---------------------------------------------- */

/* -- Module Global Function Prototypes ------------------------------------ */

/* -- Implementation ------------------------------------------------------- */

//-----------------------------------------------------------------------------
/*!
   \brief   Default constructor

   \created     03.09.2018  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
C_OSCComDriverBase::C_OSCComDriverBase(void) :
   mpc_CanDispatcher(NULL),
   mu16_DispatcherClientHandle(0U),
   mq_Started(false),
   mq_Paused(false)
{
}

//-----------------------------------------------------------------------------
/*!
   \brief   Default destructor

   Clean up.

   \created     03.09.2018  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
C_OSCComDriverBase::~C_OSCComDriverBase(void)
{
   this->mpc_CanDispatcher = NULL; //do not delete ! not owned by us

   //lint -e{1740}  no memory leak because the ownership of mpc_CanDispatcher was never transferred to this class
}

//-----------------------------------------------------------------------------
/*!
   \brief   Initialize all members

   \param[in]  opc_CanDispatcher       Pointer to concrete CAN dispatcher

   \return
   C_NO_ERR      Operation success
   C_COM         CAN initialization failed

   \created     04.09.2018  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
sint32 C_OSCComDriverBase::InitBase(C_CAN_Dispatcher * const opc_CanDispatcher)
{
   sint32 s32_Return = C_NO_ERR;

   this->mpc_CanDispatcher = opc_CanDispatcher;

   if (this->mpc_CanDispatcher != NULL)
   {
      s32_Return = this->mpc_CanDispatcher->RegisterClient(this->mu16_DispatcherClientHandle);

      if (s32_Return != C_NO_ERR)
      {
         s32_Return = C_COM;
      }
   }

   return s32_Return;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Registers a instance of C_OSCComMessageLogger

   The instance will be delivered with all received CAN messages.

   \param[in]     opc_Logger         Pointer to CAN message logger

   \created     03.09.2018  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
void C_OSCComDriverBase::RegisterLogger(C_OSCComMessageLogger * const opc_Logger)
{
   this->mc_Logger.push_back(opc_Logger);
}

//-----------------------------------------------------------------------------
/*!
   \brief   Starts the logging

   If CAN should not be initialized here, the Flag oq_InitCan must be set to false.
   But the dispatcher must be set.

   \param[in]  oq_InitCan            Flag for initializing CAN

   \return
   C_NO_ERR                          CAN initialized and logging started
   C_COM                             Initialization of CAN failed
   C_CONFIG                          Channel other than 0 configured but not supported by loaded DLL

   \created     25.09.2018  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
sint32 C_OSCComDriverBase::StartLogging(const bool oq_InitCan)
{
   sint32 s32_Return = C_CONFIG;

   if (this->mpc_CanDispatcher != NULL)
   {
      if (oq_InitCan == true)
      {
         s32_Return = this->mpc_CanDispatcher->CAN_Init();
      }
      else
      {
         s32_Return = C_NO_ERR;
      }

      if (s32_Return != C_NO_ERR)
      {
         s32_Return = C_COM;
      }
      else
      {
         this->mq_Started = true;
         this->mq_Paused = false;
      }
   }

   return s32_Return;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Stops the logging

   \param[in]  oq_ExitCan            Flag for stopping CAN

   \created     25.09.2018  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
void C_OSCComDriverBase::StopLogging(const bool oq_ExitCan)
{
   uintn un_Counter;

   this->mq_Started = false;

   if ((oq_ExitCan == true) &&
       (this->mpc_CanDispatcher != NULL))
   {
      this->mpc_CanDispatcher->CAN_Exit();
   }

   // Inform all logger about the stop
   for (un_Counter = 0U; un_Counter < this->mc_Logger.size(); ++un_Counter)
   {
      this->mc_Logger[un_Counter]->Stop();
   }
}

//-----------------------------------------------------------------------------
/*!
   \brief   Continues the paused the logging

   Continues the logging.
   All active logger will log again.

   \created     05.09.2018  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
void C_OSCComDriverBase::ContinueLogging(void)
{
   this->mq_Paused = false;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Pauses the logging.

   Pauses the logging of the entire logging.
   No registered logger will log.

   \created     05.09.2018  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
void C_OSCComDriverBase::PauseLogging(void)
{
   this->mq_Paused = true;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Distributes the CAN message to all registered C_OSCMessageLogger instances.

   Gets all received CAN messages and informs all logger about the received messages.
   This function must be called cyclic to have a continuous logging.

   Deriving classes can override m_HandleCanMessage to handle the CAN messages too.

   When making DistributeMessages and SendCanMessageQueued thread safe by overwriting to use sending CAN messages
   by separated threads.

   \created     03.09.2018  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
void C_OSCComDriverBase::DistributeMessages(void)
{
   if ((this->mpc_CanDispatcher != NULL) &&
       (this->mq_Started == true))
   {
      sint32 s32_Return;
      T_STWCAN_Msg_RX t_Msg;
      std::list<stw_can::T_STWCAN_Msg_TX>::iterator c_ItCanMessage;

      // Send all queued CAN messages and handle them by the logger too
      for (c_ItCanMessage = this->mc_CanMessages.begin(); c_ItCanMessage != this->mc_CanMessages.end();
           ++c_ItCanMessage)
      {
         this->SendCanMessage(*c_ItCanMessage);
      }
      this->mc_CanMessages.clear();

      //trigger dispatcher
      //ignore return value: we cannot be sure some other client did not check before us
      (void)mpc_CanDispatcher->DispatchIncoming();

      do
      {
         // Get the messages even if paused to clean the queue. The messages in the pause phase are not relevant
         s32_Return = this->mpc_CanDispatcher->ReadFromQueue(this->mu16_DispatcherClientHandle, t_Msg);

         if (s32_Return == C_NO_ERR)
         {
            this->m_HandleCanMessage(t_Msg, false);
         }
      }
      while (s32_Return == C_NO_ERR);
   }
}

//-----------------------------------------------------------------------------
/*!
   \brief   Sending a CAN message by putting it into the queue

   The CAN messages will be handled by calling DistributeMessages

   When making DistributeMessages and SendCanMessageQueued thread safe by overwriting to use sending CAN messages
   by separated threads.

   \param[in]     orc_Msg        CAN message to send

   \created     08.10.2018  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
void C_OSCComDriverBase::SendCanMessageQueued(const T_STWCAN_Msg_TX & orc_Msg)
{
   this->mc_CanMessages.push_back(orc_Msg);
}

//-----------------------------------------------------------------------------
/*!
   \brief   Sending a CAN message directly

   \param[in]     orc_Msg        CAN message to send

   \return
   C_NO_ERR    CAN message sent
   C_CONFIG    CAN dispatcher not initialized
   C_COM       Error on sending CAN message

   \created     08.10.2018  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
sint32 C_OSCComDriverBase::SendCanMessage(const T_STWCAN_Msg_TX & orc_Msg)
{
   sint32 s32_Return = C_CONFIG;

   if (this->mpc_CanDispatcher != NULL)
   {
      s32_Return = this->mpc_CanDispatcher->CAN_Send_Msg(orc_Msg);

      if (s32_Return == C_NO_ERR)
      {
         // Inform the logger about the sent message
         T_STWCAN_Msg_RX c_Msg;

         (void)std::memcpy(c_Msg.au8_Data, orc_Msg.au8_Data, 8U);
         c_Msg.u8_Align = orc_Msg.u8_Align;
         c_Msg.u8_DLC = orc_Msg.u8_DLC;
         c_Msg.u8_RTR = orc_Msg.u8_RTR;
         c_Msg.u8_XTD = orc_Msg.u8_XTD;
         c_Msg.u32_ID = orc_Msg.u32_ID;

         // The logger need the timestamp
         c_Msg.u64_TimeStamp = stw_tgl::TGL_GetTickCountUS();

         this->m_HandleCanMessage(c_Msg, true);
      }
      else
      {
         s32_Return = C_COM;
      }
   }

   if (s32_Return != C_NO_ERR)
   {
      osc_write_log_error("Sending CAN message", "Could not send CAN message. Error code: " +
                          C_SCLString::IntToStr(s32_Return));
   }

   return s32_Return;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Prepare for shutting down class

   To be called by child classes on shutdown, before they destroy all owned class instances

   \created     04.09.2018  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
void C_OSCComDriverBase::PrepareForDestruction(void)
{
   if (this->mpc_CanDispatcher != NULL)
   {
      this->mpc_CanDispatcher->RemoveClient(this->mu16_DispatcherClientHandle);
   }
}

//-----------------------------------------------------------------------------
/*!
   \brief   Distributes the CAN message to all registered C_OSCMessageLogger instances.

   \param[in]     orc_Msg        Current CAN message
   \param[in]     oq_IsTx        Message was sent by C_OSCComDriverBase itself

   \created     07.09.2018  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
void C_OSCComDriverBase::m_HandleCanMessage(const T_STWCAN_Msg_RX & orc_Msg, const bool oq_IsTx)
{
   if (this->mq_Paused == false)
   {
      uintn un_Counter;

      // Inform all logger about the message
      for (un_Counter = 0U; un_Counter < this->mc_Logger.size(); ++un_Counter)
      {
         this->mc_Logger[un_Counter]->HandleCanMessage(orc_Msg, oq_IsTx);
      }
   }
}
