//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Core communication driver base class (implementation)

   Offers a interface to deliver CAN messages to all registered C_OscMessageLogger instances.
   Distributes all received messages to the registered C_OscMessageLogger instances.

   \copyright   Copyright 2018 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.hpp"

#include <cstring>

#include "stwerrors.hpp"

#include "C_OscComDriverBase.hpp"
#include "C_SclString.hpp"
#include "TglTime.hpp"
#include "C_OscLoggingHandler.hpp"
#include "C_OscComAutoSupport.hpp"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */

using namespace stw::errors;
using namespace stw::scl;
using namespace stw::can;
using namespace stw::opensyde_core;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

/* -- Global Variables ---------------------------------------------------------------------------------------------- */

/* -- Module Global Variables --------------------------------------------------------------------------------------- */

/* -- Module Global Function Prototypes ----------------------------------------------------------------------------- */

/* -- Implementation ------------------------------------------------------------------------------------------------ */

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default constructor
*/
//----------------------------------------------------------------------------------------------------------------------
C_OscComDriverBaseCanMessage::C_OscComDriverBaseCanMessage(void) :
   u32_TimeToSend(0U),
   u32_Interval(0U)
{
   this->c_Msg.u8_Align = 0U;
   this->c_Msg.u8_DLC = 0U;
   this->c_Msg.u8_RTR = 0U;
   this->c_Msg.u8_XTD = 0U;
   this->c_Msg.u32_ID = 0U;
   (void)std::memset(&this->c_Msg.au8_Data[0], 0, 8U);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Check if current equal to orc_Cmp

   \param[in] orc_Cmp Compared instance

   \return
   Current equal to orc_Cmp
   Else false
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_OscComDriverBaseCanMessage::operator ==(const C_OscComDriverBaseCanMessage & orc_Cmp) const
{
   bool q_Return = false;

   // Do not compare u32_TimeToSend, it has a temporary value
   if ((this->c_Msg.u32_ID == orc_Cmp.c_Msg.u32_ID) &&
       (this->c_Msg.u8_DLC == orc_Cmp.c_Msg.u8_DLC) &&
       (this->c_Msg.u8_RTR == orc_Cmp.c_Msg.u8_RTR) &&
       (this->c_Msg.u8_XTD == orc_Cmp.c_Msg.u8_XTD) &&
       (this->u32_Interval == orc_Cmp.u32_Interval))
   {
      uint8_t u8_Counter;
      q_Return = true;

      for (u8_Counter = 0U; u8_Counter < this->c_Msg.u8_DLC; ++u8_Counter)
      {
         // Check the data
         if (this->c_Msg.au8_Data[u8_Counter] != orc_Cmp.c_Msg.au8_Data[u8_Counter])
         {
            q_Return = false;
            break;
         }
      }
   }

   return q_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default constructor
*/
//----------------------------------------------------------------------------------------------------------------------
C_OscComDriverBase::C_OscComDriverBase(void) :
   mpc_CanDispatcher(NULL),
   mu16_DispatcherClientHandle(0U),
   mq_Started(false),
   mq_Paused(false),
   mu32_CanMessageBits(0U),
   ms32_CanBitrate(1000U),
   mu32_CanTxCounter(0U),
   mu32_CanTxErrors(0U)
{
   mpc_AutoSupportProtocol = new C_OscComAutoSupport();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default destructor

   Clean up.
*/
//----------------------------------------------------------------------------------------------------------------------
C_OscComDriverBase::~C_OscComDriverBase(void)
{
   this->mpc_CanDispatcher = NULL; //do not delete ! not owned by us
   delete this->mpc_AutoSupportProtocol;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Initialize all members

   \param[in]  opc_CanDispatcher       Pointer to concrete CAN dispatcher

   \return
   C_NO_ERR      Operation success
   C_COM         CAN initialization failed
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_OscComDriverBase::InitBase(C_CanDispatcher * const opc_CanDispatcher)
{
   int32_t s32_Return = C_NO_ERR;

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

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Registers a instance of C_OscComMessageLogger

   The instance will be delivered with all received CAN messages.

   \param[in]     opc_Logger         Pointer to CAN message logger
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OscComDriverBase::RegisterLogger(C_OscComMessageLogger * const opc_Logger)
{
   this->mc_Logger.push_back(opc_Logger);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Starts the logging

   The CAN dispatcher must be set and initialized already

   \param[in]  os32_Bitrate          CAN bitrate in kbit/s. Is used for the bus load calculation not the initialization

   \return
   C_NO_ERR                          CAN initialized and logging started
   C_CONFIG                          CAN dispatcher is not set
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_OscComDriverBase::StartLogging(const int32_t os32_Bitrate)
{
   int32_t s32_Return = C_CONFIG;

   if (this->mpc_CanDispatcher != NULL)
   {
      uint32_t u32_Counter;
      s32_Return = C_NO_ERR;

      this->mq_Started = true;
      this->mq_Paused = false;
      this->ms32_CanBitrate = os32_Bitrate;
      // Reset the counter
      this->mu32_CanMessageBits = 0U;
      this->mu32_CanTxCounter = 0U;
      this->mu32_CanTxErrors = 0U;

      // Inform all loggers about the start
      for (u32_Counter = 0U; u32_Counter < this->mc_Logger.size(); ++u32_Counter)
      {
         this->mc_Logger[u32_Counter]->Start();
      }
   }

   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Stops the logging
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OscComDriverBase::StopLogging(void)
{
   mpc_AutoSupportProtocol->ClearAutoSupportInfo();
   uint32_t u32_Counter;

   this->mq_Started = false;

   // Inform all logger about the stop and no bus load when stopped
   for (u32_Counter = 0U; u32_Counter < this->mc_Logger.size(); ++u32_Counter)
   {
      this->mc_Logger[u32_Counter]->Stop();
      this->mc_Logger[u32_Counter]->UpdateBusLoad(0U);
      this->mc_Logger[u32_Counter]->UpdateTxErrors(0U);
      this->mc_Logger[u32_Counter]->UpdateTxCounter(0U);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Continues the paused the logging

   Continues the logging.
   All active logger will log again.
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OscComDriverBase::ContinueLogging(void)
{
   uint32_t u32_Counter;

   this->mq_Paused = false;

   // Inform all logger about the continue
   for (u32_Counter = 0U; u32_Counter < this->mc_Logger.size(); ++u32_Counter)
   {
      this->mc_Logger[u32_Counter]->Continue();
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Pauses the logging.

   Pauses the logging of the entire logging.
   No registered logger will log.
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OscComDriverBase::PauseLogging(void)
{
   uint32_t u32_Counter;

   this->mq_Paused = true;

   // Inform all logger about the pause
   for (u32_Counter = 0U; u32_Counter < this->mc_Logger.size(); ++u32_Counter)
   {
      this->mc_Logger[u32_Counter]->Pause();
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Update the bitrate to adapt the calculation for the bus load

   \param[in]  os32_Bitrate          CAN bitrate in kbit/s. Is used for the bus load calculation not the initialization
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OscComDriverBase::UpdateBitrate(const int32_t os32_Bitrate)
{
   if (os32_Bitrate != this->ms32_CanBitrate)
   {
      this->ms32_CanBitrate = os32_Bitrate;
      // Reset the counter
      this->mu32_CanMessageBits = 0U;
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Clear rx messages
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OscComDriverBase::ClearRxMessages()
{
   if (this->mpc_CanDispatcher != NULL)
   {
      this->mpc_CanDispatcher->ClearQueue(this->mu16_DispatcherClientHandle);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Distributes the CAN message to all registered C_OscMessageLogger instances.

   Gets all received CAN messages and informs all logger about the received messages.
   This function must be called cyclic to have a continuous logging.

   Deriving classes can override m_HandleCanMessage to handle the CAN messages too.

   When making DistributeMessages and SendCanMessageQueued thread safe by overwriting to use sending CAN messages
   by separated threads.
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OscComDriverBase::DistributeMessages(void)
{
   if ((this->mpc_CanDispatcher != NULL) &&
       (this->mq_Started == true))
   {
      int32_t s32_Return;
      T_STWCAN_Msg_RX c_Msg;
      static uint32_t hu32_BusLoadTimeRefresh = 0U;
      uint32_t u32_BusLoadTimeDiff;
      uint32_t u32_LoggerCounter;

      // Send all relevant CAN messages and handle them by the logger too
      this->m_HandleCanMessagesForSending();

      //trigger dispatcher
      //ignore return value: we cannot be sure some other client did not check before us
      (void)mpc_CanDispatcher->DispatchIncoming();

      do
      {
         // Get the messages even if paused to clean the queue. The messages in the pause phase are not relevant
         s32_Return = this->mpc_CanDispatcher->ReadFromQueue(this->mu16_DispatcherClientHandle, c_Msg);

         if (s32_Return == C_NO_ERR)
         {
            if (c_Msg.u8_DLC <= 8)
            {
               this->m_HandleCanMessage(c_Msg, false);
            }
            else
            {
               //ignore invalid can message
               osc_write_log_error("Reading CAN message",
                                   "Ignored CAN message (ID: " + C_SclString::IntToStr(
                                      c_Msg.u32_ID) + ") due to invalid DLC (" + C_SclString::IntToStr(c_Msg.u8_DLC));
            }
         }
      }
      while (s32_Return == C_NO_ERR);

      // Check and update bus load
      u32_BusLoadTimeDiff = stw::tgl::TglGetTickCount() - hu32_BusLoadTimeRefresh;
      if (u32_BusLoadTimeDiff >= 1000U)
      {
         const uint32_t u32_MaxBitsSec = static_cast<uint32_t>(this->ms32_CanBitrate) * 1024U;
         // Scale to the real elapsed time
         const uint32_t u32_MaxBits =
            static_cast<uint32_t>((static_cast<uint64_t>(u32_MaxBitsSec) * static_cast<uint64_t>(u32_BusLoadTimeDiff)) /
                                  1000U);
         if (u32_MaxBits > 0U)
         {
            uint32_t u32_Load = (this->mu32_CanMessageBits * 100U) / u32_MaxBits;

            hu32_BusLoadTimeRefresh = stw::tgl::TglGetTickCount();

            if (u32_Load > 100U)
            {
               // Limit to 100%
               u32_Load = 100U;
            }

            // Inform all loggers about the bus load
            for (u32_LoggerCounter = 0U; u32_LoggerCounter < this->mc_Logger.size(); ++u32_LoggerCounter)
            {
               this->mc_Logger[u32_LoggerCounter]->UpdateBusLoad(static_cast<uint8_t>(u32_Load));
            }

            // Reset bit counter
            this->mu32_CanMessageBits = 0U;
         }
      }

      // Inform about Tx errors and counter
      for (u32_LoggerCounter = 0U; u32_LoggerCounter < this->mc_Logger.size(); ++u32_LoggerCounter)
      {
         this->mc_Logger[u32_LoggerCounter]->UpdateTxErrors(this->mu32_CanTxErrors);
         this->mc_Logger[u32_LoggerCounter]->UpdateTxCounter(this->mu32_CanTxCounter);
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Sending a CAN message by putting it into the queue

   The CAN messages will be handled by calling DistributeMessages

   When making DistributeMessages and SendCanMessageQueued thread safe by overwriting to use sending CAN messages
   by separated threads.

   \param[in]     orc_Msg        CAN message to send
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OscComDriverBase::SendCanMessageQueued(const T_STWCAN_Msg_TX & orc_Msg)
{
   this->mc_CanMessages.push_back(orc_Msg);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Sending a CAN message directly

   \param[in]     orc_Msg        CAN message to send

   \return
   C_NO_ERR    CAN message sent
   C_CONFIG    CAN dispatcher not initialized
   C_COM       Error on sending CAN message
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_OscComDriverBase::SendCanMessageDirect(T_STWCAN_Msg_TX & orc_Msg)
{
   orc_Msg.au8_Data[6] = mpc_AutoSupportProtocol->MessageCounter(orc_Msg.u32_ID, orc_Msg.au8_Data[6]);
   orc_Msg.au8_Data[7] =
      mpc_AutoSupportProtocol->SetCyclicRedundancyCheckCalculation(orc_Msg.u32_ID, orc_Msg.u8_DLC - 2,
                                                                   &orc_Msg.au8_Data[0]);

   int32_t s32_Return = C_CONFIG;

   if (this->mpc_CanDispatcher != NULL)
   {
      s32_Return = this->mpc_CanDispatcher->CAN_Send_Msg(orc_Msg);

      if (mpc_AutoSupportProtocol->SupportInvertedCanMessage(orc_Msg.u32_ID))
      {
         uint8_t au8_InvertedData[8U];

         C_OscComAutoSupport::h_InvertCanMessage(&orc_Msg.au8_Data[0], &au8_InvertedData[0]);
         T_STWCAN_Msg_TX orc_TransmitInvertedMsg;
         (void)std::memcpy(&orc_TransmitInvertedMsg.au8_Data[0], &au8_InvertedData[0], 8U);

         orc_TransmitInvertedMsg.u32_ID = orc_Msg.u32_ID + 1;
         orc_TransmitInvertedMsg.u8_DLC = orc_Msg.u8_DLC;
         orc_TransmitInvertedMsg.u8_XTD = orc_Msg.u8_XTD;
         orc_TransmitInvertedMsg.u8_RTR = orc_Msg.u8_RTR;
         orc_TransmitInvertedMsg.u8_Align = orc_Msg.u8_Align;
         this->mpc_CanDispatcher->CAN_Send_Msg(orc_TransmitInvertedMsg);
      }

      if (s32_Return == C_NO_ERR)
      {
         // Inform the logger about the sent message
         T_STWCAN_Msg_RX c_Msg;

         (void)std::memcpy(&c_Msg.au8_Data[0], &orc_Msg.au8_Data[0], 8U);
         c_Msg.u8_Align = orc_Msg.u8_Align;
         c_Msg.u8_DLC = orc_Msg.u8_DLC;
         c_Msg.u8_RTR = orc_Msg.u8_RTR;
         c_Msg.u8_XTD = orc_Msg.u8_XTD;
         c_Msg.u32_ID = orc_Msg.u32_ID;

         // The logger need the timestamp
         c_Msg.u64_TimeStamp = stw::tgl::TglGetTickCountUs();

         this->m_HandleCanMessage(c_Msg, true);

         if (mpc_AutoSupportProtocol->SupportInvertedCanMessage(orc_Msg.u32_ID))
         {
            uint8_t au8_InvertedData[8U];
            C_OscComAutoSupport::h_InvertCanMessage(&orc_Msg.au8_Data[0], &au8_InvertedData[0]);
            T_STWCAN_Msg_RX c_RecieveInvertedMsg;
            (void)std::memcpy(&c_RecieveInvertedMsg.au8_Data[0], &au8_InvertedData[0], 8U);

            c_RecieveInvertedMsg.u32_ID = orc_Msg.u32_ID + 1;
            c_RecieveInvertedMsg.u8_DLC = orc_Msg.u8_DLC;
            c_RecieveInvertedMsg.u8_XTD = orc_Msg.u8_XTD;
            c_RecieveInvertedMsg.u8_RTR = orc_Msg.u8_RTR;
            c_RecieveInvertedMsg.u8_Align = orc_Msg.u8_Align;
            c_RecieveInvertedMsg.u64_TimeStamp = stw::tgl::TglGetTickCountUs();
            this->m_HandleCanMessage(c_RecieveInvertedMsg, true);
         }

         // Count the message
         if (this->mu32_CanTxCounter < 0xFFFFFFFFU)
         {
            ++this->mu32_CanTxCounter;
         }
      }
      else
      {
         if (this->mu32_CanTxErrors < 0xFFFFFFFFU)
         {
            // Count the error
            ++this->mu32_CanTxErrors;
         }
         s32_Return = C_COM;
      }
   }

   if (s32_Return != C_NO_ERR)
   {
      osc_write_log_error("Sending CAN message", "Could not send CAN message. Error code: " +
                          C_SclString::IntToStr(s32_Return));
   }

   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Sends a CAN message with a specific configuration

   Interval is ignored and set to 0.
   Use AddCyclicCanMessage for registration of a cyclic CAN message.

   \param[in]     orc_MsgCfg                    CAN message configuration
   \param[in]      oq_SetAutoSupportMode     Is AutoSupport activated
   \param[in]      oe_ProtocolType           Current message Protocol type
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OscComDriverBase::SendCanMessage(C_OscComDriverBaseCanMessage & orc_MsgCfg, const bool oq_SetAutoSupportMode,
                                        const C_OscCanProtocol::E_Type oe_ProtocolType)
{
   mpc_AutoSupportProtocol->AutoSupportModeInfo(orc_MsgCfg.c_Msg.u32_ID, oq_SetAutoSupportMode, oe_ProtocolType);
   this->mc_CanMessageConfigs.push_back(orc_MsgCfg);
   this->mc_CanMessageConfigs.back().u32_Interval = 0U;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Registers a cyclic CAN message with a specific configuration

   \param[in]     orc_MsgCfg         CAN message configuration
   \param[in]      oq_SetAutoSupportMode     Is AutoSupport activated
   \param[in]      oe_ProtocolType           Current message Protocol type
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OscComDriverBase::AddCyclicCanMessage(const C_OscComDriverBaseCanMessage & orc_MsgCfg,
                                             const bool oq_SetAutoSupportMode,
                                             const C_OscCanProtocol::E_Type oe_ProtocolType)
{
   mpc_AutoSupportProtocol->AutoSupportModeInfo(orc_MsgCfg.c_Msg.u32_ID, oq_SetAutoSupportMode, oe_ProtocolType);
   this->mc_CanMessageConfigs.push_back(orc_MsgCfg);
   if (orc_MsgCfg.u32_Interval == 0U)
   {
      this->mc_CanMessageConfigs.back().u32_Interval = 1U;
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Removes a cyclic CAN message with a specific configuration

   \param[in]     orc_MsgCfg         CAN message configuration
   \param[in]      oq_SetAutoSupportMode     Is AutoSupport activated
   \param[in]      oe_ProtocolType           Current message Protocol type
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OscComDriverBase::RemoveCyclicCanMessage(const C_OscComDriverBaseCanMessage & orc_MsgCfg,
                                                const bool oq_SetAutoSupportMode,
                                                const C_OscCanProtocol::E_Type oe_ProtocolType)
{
   mpc_AutoSupportProtocol->AutoSupportModeInfo(orc_MsgCfg.c_Msg.u32_ID, false, oe_ProtocolType);
   C_OscComDriverBaseCanMessage c_MsgCfg = orc_MsgCfg;

   std::list<C_OscComDriverBaseCanMessage>::iterator c_ItConfig;

   if (c_MsgCfg.u32_Interval == 0U)
   {
      c_MsgCfg.u32_Interval = 1U;
   }

   for (c_ItConfig = this->mc_CanMessageConfigs.begin(); c_ItConfig != this->mc_CanMessageConfigs.end(); ++c_ItConfig)
   {
      if (((false == oq_SetAutoSupportMode) && (*c_ItConfig == c_MsgCfg)) ||
          ((true == oq_SetAutoSupportMode) && (oe_ProtocolType == C_OscCanProtocol::eCAN_OPEN_SAFETY) &&
           (*c_ItConfig == c_MsgCfg)) ||
          ((true == oq_SetAutoSupportMode) && (oe_ProtocolType == C_OscCanProtocol::eECES) &&
           ((c_ItConfig->c_Msg.u32_ID == orc_MsgCfg.c_Msg.u32_ID) &&
            (c_ItConfig->c_Msg.u8_Align == orc_MsgCfg.c_Msg.u8_Align) &&
            (c_ItConfig->c_Msg.u8_DLC == orc_MsgCfg.c_Msg.u8_DLC) &&
            (c_ItConfig->c_Msg.u8_RTR == orc_MsgCfg.c_Msg.u8_RTR) &&
            (c_ItConfig->c_Msg.u8_XTD == orc_MsgCfg.c_Msg.u8_XTD) &&
            (c_ItConfig->c_Msg.au8_Data[0] == orc_MsgCfg.c_Msg.au8_Data[0]) &&
            (c_ItConfig->c_Msg.au8_Data[1] == orc_MsgCfg.c_Msg.au8_Data[1]) &&
            (c_ItConfig->c_Msg.au8_Data[2] == orc_MsgCfg.c_Msg.au8_Data[2]) &&
            (c_ItConfig->c_Msg.au8_Data[3] == orc_MsgCfg.c_Msg.au8_Data[3]) &&
            (c_ItConfig->c_Msg.au8_Data[4] == orc_MsgCfg.c_Msg.au8_Data[4]) &&
            (c_ItConfig->c_Msg.au8_Data[5] == orc_MsgCfg.c_Msg.au8_Data[5]))))
      {
         this->mc_CanMessageConfigs.erase(c_ItConfig);
         break;
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Removes all cyclic CAN messages
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OscComDriverBase::RemoveAllCyclicCanMessages(void)
{
   this->mc_CanMessageConfigs.clear();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Prepare for shutting down class

   To be called by child classes on shutdown, before they destroy all owned class instances
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OscComDriverBase::PrepareForDestruction(void)
{
   if (this->mpc_CanDispatcher != NULL)
   {
      this->mpc_CanDispatcher->RemoveClient(this->mu16_DispatcherClientHandle);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  On Clear trace we reset message counters
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OscComDriverBase::ClearData()
{
   mpc_AutoSupportProtocol->ResetOnClearData();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  On Update of AutoSupportProtocol to send information for stop/start Message counter

   \param[in]       os32_CanId                Current message Can Id
   \param[in]      oq_SetAutoSupportMode     Is AutoSupport activated
   \param[in]      oe_ProtocolType           Current message Protocol type
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OscComDriverBase::UpdateAutoSupportProtocol(const int32_t os32_CanId, const bool oq_SetAutoSupportMode,
                                                   const C_OscCanProtocol::E_Type oe_ProtocolType)
{
   mpc_AutoSupportProtocol->AutoSupportModeInfo(os32_CanId, oq_SetAutoSupportMode, oe_ProtocolType);
   mpc_AutoSupportProtocol->ResetMessageCounters(os32_CanId, oq_SetAutoSupportMode, oe_ProtocolType);
   std::list<C_OscComDriverBaseCanMessage>::iterator c_ItConfig;
   for (c_ItConfig = this->mc_CanMessageConfigs.begin(); c_ItConfig != this->mc_CanMessageConfigs.end(); ++c_ItConfig)
   {
      if ((static_cast<int32_t>(c_ItConfig->c_Msg.u32_ID) == os32_CanId) &&
          (oe_ProtocolType == C_OscCanProtocol::eECES) && (oq_SetAutoSupportMode == false))
      {
         c_ItConfig->c_Msg.au8_Data[6] = 0;
         c_ItConfig->c_Msg.au8_Data[7] = 0;
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Distributes the CAN message to all registered C_OscMessageLogger instances.

   \param[in]     orc_Msg        Current CAN message
   \param[in]     oq_IsTx        Message was sent by C_OscComDriverBase itself
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OscComDriverBase::m_HandleCanMessage(const T_STWCAN_Msg_RX & orc_Msg, const bool oq_IsTx)
{
   if (this->mq_Paused == false)
   {
      uint32_t u32_Counter;

      // Inform all logger about the message
      for (u32_Counter = 0U; u32_Counter < this->mc_Logger.size(); ++u32_Counter)
      {
         this->mc_Logger[u32_Counter]->HandleCanMessage(orc_Msg, oq_IsTx);
      }

      this->mu32_CanMessageBits += C_OscComDriverBase::mh_GetCanMessageSizeInBits(orc_Msg);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Sending all CAN messages which are queued or registered for cyclic transmission
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OscComDriverBase::m_HandleCanMessagesForSending(void)
{
   std::list<stw::can::T_STWCAN_Msg_TX>::iterator c_ItCanMessage;
   std::list<C_OscComDriverBaseCanMessage>::iterator c_ItCanMessageConfig = this->mc_CanMessageConfigs.begin();

   // Send all queued CAN messages
   for (c_ItCanMessage = this->mc_CanMessages.begin(); c_ItCanMessage != this->mc_CanMessages.end();
        ++c_ItCanMessage)
   {
      this->SendCanMessageDirect(*c_ItCanMessage);
   }
   this->mc_CanMessages.clear();

   // Send all registered cyclic messages
   while (c_ItCanMessageConfig != this->mc_CanMessageConfigs.end())
   {
      const uint32_t u32_CurTimeStamp = stw::tgl::TglGetTickCount();

      if ((*c_ItCanMessageConfig).u32_TimeToSend <= u32_CurTimeStamp)
      {
         // It is time for sending the CAN message
         this->SendCanMessageDirect((*c_ItCanMessageConfig).c_Msg);

         if ((*c_ItCanMessageConfig).u32_Interval != 0)
         {
            // Update the next timestamp for the next sending
            (*c_ItCanMessageConfig).u32_TimeToSend = u32_CurTimeStamp + (*c_ItCanMessageConfig).u32_Interval;
            ++c_ItCanMessageConfig;
         }
         else
         {
            // No interval configured. Sending this CAN message only once and erase the configuration
            c_ItCanMessageConfig = this->mc_CanMessageConfigs.erase(c_ItCanMessageConfig);
         }
      }
      else
      {
         // Nothing to do, check next element
         ++c_ItCanMessageConfig;
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Calculates the CAN message size

   Assume average number of stuff bits
   Count in message even if it is filtered out
   We want to have the busload as close to the truth as possible

   Size CAN message with standard id: 47 Bit + payload + stuff bits
   Size CAN message with extended id: 67 Bit + payload + stuff bits

   The following table shows the maximum determined message lengths in bits with stuff bits
   Standard id:
   DLC | number of bits | number of stuff bits
   0   | 53             | 6
   1   | 59             | 4
   2   | 69             | 6
   3   | 79             | 8
   4   | 89             | 10
   5   | 98             | 11
   6   | 107            | 12
   7   | 117            | 14
   8   | 127            | 16
   Extended id:
   0   | 74             | 7
   1   | 83             | 8
   2   | 92             | 9
   3   | 101            | 10
   4   | 112            | 13
   5   | 122            | 15
   6   | 131            | 16
   7   | 140            | 17
   8   | 150            | 19

   CAN messages with extended id can have up to 4 stuff bits more than standard id with same DLC.
   In average we calculate with 3 more stuff bits for extended id.

   Additional to that we approach to the worst case of stuff bits by using the calculation DLC * 10 bits.

   Implementation of CANMon with adapted parameters.

   \param[in]     orc_Msg        Current CAN message

   \return
   CAN message size bits
*/
//----------------------------------------------------------------------------------------------------------------------
uint32_t C_OscComDriverBase::mh_GetCanMessageSizeInBits(const T_STWCAN_Msg_RX & orc_Msg)
{
   // Stuff bits dependent of DLC +
   // minimum size of CAN message with standard identifier +
   // Optional extended id with 18 bits for the extended id itself, SRR, additional reserved bit and the 3 stuff bits
   const uint8_t u8_ExtendedBits = ((orc_Msg.u8_XTD == 1U) ? 23U : 0U);

   return ((static_cast<uint32_t>(orc_Msg.u8_DLC) * 10U) + 47U + u8_ExtendedBits);
}
