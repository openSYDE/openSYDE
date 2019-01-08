//-----------------------------------------------------------------------------
/*!
   \internal
   \file
   \brief       Class for preparing CAN message data for showing on GUI (implementation)

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

#include "stwtypes.h"
#include "stwerrors.h"

#include "TGLUtils.h"

#include "C_SyvComMessageMonitor.h"
#include "C_CieImportDbc.h"
#include "C_OSCCanUtil.h"
#include "C_OSCLoggingHandler.h"

/* -- Used Namespaces ------------------------------------------------------ */
using namespace stw_types;
using namespace stw_errors;
using namespace stw_scl;
using namespace stw_can;
using namespace stw_opensyde_core;
using namespace stw_opensyde_gui_logic;

/* -- Module Global Constants ---------------------------------------------- */

/* -- Types ---------------------------------------------------------------- */

/* -- Global Variables ----------------------------------------------------- */

/* -- Module Global Variables ---------------------------------------------- */

/* -- Module Global Function Prototypes ------------------------------------ */

/* -- Implementation ------------------------------------------------------- */

//-----------------------------------------------------------------------------
/*!
   \brief   Default constructor

   \created     20.09.2018  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
C_SyvComMessageMonitorDataSignal::C_SyvComMessageMonitorDataSignal(void) :
   c_Name(""),
   c_Value(""),
   c_Unit(""),
   c_RawValueDec(""),
   c_RawValueHex(""),
   c_Comment(""),
   q_DlcError(false)
{
}

//-----------------------------------------------------------------------------
/*!
   \brief   Default constructor

   \created     03.09.2018  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
C_SyvComMessageMonitorData::C_SyvComMessageMonitorData(void) :
   c_TimeStampRelative(""),
   c_TimeStampAbsolute(""),
   c_CanIdDec(""),
   c_CanIdHex(""),
   c_CanDlc(""),
   c_CanDataDec(""),
   c_CanDataHex(""),
   c_Name(""),
   c_ProtocolTextDec(""),
   c_ProtocolTextHex(""),
   c_Counter(""),
   q_IsTx(false)
{
}

//-----------------------------------------------------------------------------
/*!
   \brief   Default constructor

   \created     03.09.2018  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
C_SyvComMessageMonitor::C_SyvComMessageMonitor(void) :
   C_OSCComMessageLogger(),
   me_LoadingActivity(eNOT_ACTIVE),
   mc_Path(""),
   mc_PathDeviceDefinitions(""),
   mu32_BusIndex(0U),
   ms32_Result(0),
   mu64_LastTimeStamp(0U),
   mpc_DbcMessage(NULL)
{
   // Resize the vector for all potential CAN standard ids
   this->mc_MsgCounterStandardId.resize(0x800U, 0U);

   mpc_LoadingThread = new C_SyvComDriverThread(&C_SyvComMessageMonitor::mh_ThreadFunc, this);
}

//-----------------------------------------------------------------------------
/*!
   \brief   Default destructor

   \created     07.09.2018  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
C_SyvComMessageMonitor::~C_SyvComMessageMonitor(void)
{
   if (this->mpc_LoadingThread != NULL)
   {
      if (this->mpc_LoadingThread->isRunning() == true)
      {
         this->mpc_LoadingThread->requestInterruption();

         if (this->mpc_LoadingThread->wait(2000U) == false)
         {
            // Not finished yet
            osc_write_log_warning("Closing message monitor",
                                  "Waiting time for stopping loading thread was not enough");
         }
      }
      delete mpc_LoadingThread;
      mpc_LoadingThread = NULL;
   }
}

//-----------------------------------------------------------------------------
/*!
   \brief   Optional function to react on the stop of the communication

   Reset all CAN message counter

   \created     25.09.2018  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
void C_SyvComMessageMonitor::Stop(void)
{
   std::fill(this->mc_MsgCounterStandardId.begin(), this->mc_MsgCounterStandardId.end(), 0U);
   this->mc_MsgCounterExtendedId.clear();
}

//-----------------------------------------------------------------------------
/*!
   \brief   Starts thread to add openSYDE system definition

   \return
   C_NO_ERR   started sequence
   C_BUSY     previously started sequence still going on

   \created     18.12.2017  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
sint32 C_SyvComMessageMonitor::StartAddOsySysDef(const C_SCLString & orc_PathSystemDefinition,
                                                 const C_SCLString & orc_PathDeviceDefinitions)
{
   sint32 s32_Return = C_NO_ERR;

   if (this->mpc_LoadingThread->isRunning() == true)
   {
      s32_Return = C_BUSY;
   }
   else
   {
      this->mc_Path = orc_PathSystemDefinition;
      this->mc_PathDeviceDefinitions = orc_PathDeviceDefinitions;
      this->mc_Busses.clear();
      this->me_LoadingActivity = eADD_OSY_SYSDEF_WITHOUT_BUSINDEX;
      this->mpc_LoadingThread->start();
   }
   return s32_Return;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Starts thread to add openSYDE system definition

   \return
   C_NO_ERR   started sequence
   C_BUSY     previously started sequence still going on

   \created     18.12.2017  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
sint32 C_SyvComMessageMonitor::StartAddOsySysDef(const C_SCLString & orc_PathSystemDefinition,
                                                 const C_SCLString & orc_PathDeviceDefinitions,
                                                 const uint32 ou32_BusIndex)
{
   sint32 s32_Return = C_NO_ERR;

   if (this->mpc_LoadingThread->isRunning() == true)
   {
      s32_Return = C_BUSY;
   }
   else
   {
      this->mc_Path = orc_PathSystemDefinition;
      this->mc_PathDeviceDefinitions = orc_PathDeviceDefinitions;
      this->mu32_BusIndex = ou32_BusIndex;
      this->me_LoadingActivity = eADD_OSY_SYSDEF_WITH_BUSINDEX;
      this->mpc_LoadingThread->start();
   }
   return s32_Return;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Updates the set bus index of the specific system definition

   This function is thread safe.

   \param[in]     orc_PathSystemDefinition       Path of system definition file (Must be .syde_sysdef)
   \param[in]     ou32_BusIndex                  Bus index of CAN bus of system definition for monitoring

   \return
   C_NO_ERR    Bus index for this system definition adapted
   C_NOACT     No system definition found with this path

   \created     18.09.2018  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
sint32 C_SyvComMessageMonitor::SetOsySysDefBus(const C_SCLString & orc_PathSystemDefinition, const uint32 ou32_BusIndex)
{
   sint32 s32_Return;

   this->mc_CriticalSectionConfig.Acquire();
   s32_Return = C_OSCComMessageLogger::SetOsySysDefBus(orc_PathSystemDefinition, ou32_BusIndex);
   this->mc_CriticalSectionConfig.Release();

   return s32_Return;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Removes an openSYDE system definition

   This function is thread safe.

   \param[in]     orc_PathSystemDefinition       Path of system definition file as identification

   \return
   C_NO_ERR    System definition removed
   C_NOACT     No system definition found with this path

   \created     17.09.2018  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
sint32 C_SyvComMessageMonitor::RemoveOsySysDef(const C_SCLString & orc_PathSystemDefinition)
{
   sint32 s32_Return;

   this->mc_CriticalSectionConfig.Acquire();
   s32_Return = C_OSCComMessageLogger::RemoveOsySysDef(orc_PathSystemDefinition);
   this->mc_CriticalSectionConfig.Release();

   return s32_Return;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Starts thread to add a DBC file

   \return
   C_NO_ERR   started sequence
   C_BUSY     previously started sequence still going on

   \created     21.09.2018  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
sint32 C_SyvComMessageMonitor::StartAddDbcFile(const C_SCLString & orc_PathDbc)
{
   sint32 s32_Return = C_NO_ERR;

   if (this->mpc_LoadingThread->isRunning() == true)
   {
      s32_Return = C_BUSY;
   }
   else
   {
      this->mc_Path = orc_PathDbc;
      this->me_LoadingActivity = eADD_DBC_FILE;
      this->mpc_LoadingThread->start();
   }
   return s32_Return;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Removes a DBC file

   This function is thread safe.

   \return
   C_NO_ERR    System definition removed
   C_NOACT     No system definition found with this path

   \created     21.09.2018  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
sint32 C_SyvComMessageMonitor::RemoveDbcFile(const C_SCLString & orc_PathDbc)
{
   sint32 s32_Return = C_NOACT;

   this->mc_CriticalSectionConfig.Acquire();
   std::map<stw_scl::C_SCLString, C_CieConverter::C_CIECommDefinition>::iterator c_ItDbc = this->mc_DbcFiles.find(
      orc_PathDbc);

   if (c_ItDbc != this->mc_DbcFiles.end())
   {
      // Remove the entry
      this->mc_DbcFiles.erase(c_ItDbc);

      s32_Return = C_NO_ERR;
   }
   this->mc_CriticalSectionConfig.Release();

   return s32_Return;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Adds a new filter configuration

   This function is thread safe.

   \param[in]  orc_Filter     Filter configuration to add

   \created     05.09.2018  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
void C_SyvComMessageMonitor::AddFilter(const C_OSCComMessageLoggerFilter & orc_Filter)
{
   this->mc_CriticalSectionConfig.Acquire();
   C_OSCComMessageLogger::AddFilter(orc_Filter);
   this->mc_CriticalSectionConfig.Release();
}

//-----------------------------------------------------------------------------
/*!
   \brief   Removes a filter configuration

   This function is thread safe.

   \param[in]  orc_Filter     Filter configuration to remove

   \created     05.09.2018  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
void C_SyvComMessageMonitor::RemoveFilter(const C_OSCComMessageLoggerFilter & orc_Filter)
{
   this->mc_CriticalSectionConfig.Acquire();
   C_OSCComMessageLogger::RemoveFilter(orc_Filter);
   this->mc_CriticalSectionConfig.Release();
}

//-----------------------------------------------------------------------------
/*!
   \brief   If necessary handle and prepare the CAN message for showing all necessary information

   This function is thread safe.

   \param[in]     orc_Msg        Current CAN message
   \param[in]     oq_IsTx        Message was sent of this application itself

   \return
   C_NO_ERR    CAN message logged
   C_NOACT     CAN message not relevant
   C_BUSY      Monitor is paused

   \created     03.09.2018  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
sint32 C_SyvComMessageMonitor::HandleCanMessage(const T_STWCAN_Msg_RX & orc_Msg, const bool oq_IsTx)
{
   const sint32 s32_Return = C_OSCComMessageLogger::HandleCanMessage(orc_Msg, oq_IsTx);

   if (s32_Return == C_NO_ERR)
   {
      uint8 u8_DbCounter;
      C_SyvComMessageMonitorData c_NewMessage;

      // Save the L2 message. Needed for the interpretation partly
      c_NewMessage.c_CanMsg = orc_Msg;

      // Prepare the data for the ui
      c_NewMessage.c_CanIdDec = QString::number(orc_Msg.u32_ID);
      c_NewMessage.c_CanIdHex = QString::number(orc_Msg.u32_ID, 16).toUpper();
      c_NewMessage.c_CanDlc = QString::number(orc_Msg.u8_DLC);
      c_NewMessage.q_IsTx = oq_IsTx;

      for (u8_DbCounter = 0U; u8_DbCounter < orc_Msg.u8_DLC; ++u8_DbCounter)
      {
         c_NewMessage.c_CanDataDec += QString("%1").arg(orc_Msg.au8_Data[u8_DbCounter], 3, 10, QChar(' '));
         c_NewMessage.c_CanDataHex += QString("%1").arg(orc_Msg.au8_Data[u8_DbCounter], 2, 16, QChar('0')).toUpper();

         if (u8_DbCounter < (orc_Msg.u8_DLC - 1U))
         {
            c_NewMessage.c_CanDataDec += " ";
            c_NewMessage.c_CanDataHex += " ";
         }
      }
      // Timestamps
      c_NewMessage.c_TimeStampAbsolute = this->m_GetTimestampAsString(orc_Msg.u64_TimeStamp).c_str();
      c_NewMessage.c_TimeStampRelative =
         this->m_GetTimestampAsString(orc_Msg.u64_TimeStamp - this->mu64_LastTimeStamp).c_str();
      // Save the timestamp for the next message to calculate the relative timestamp
      this->mu64_LastTimeStamp = orc_Msg.u64_TimeStamp;

      // Check for openSYDE system definition interpretation
      if (this->m_InterpretSysDef(c_NewMessage) == false)
      {
         // Check for DBC interpretation
         if (this->m_InterpretDbcFile(c_NewMessage) == false)
         {
            // No interpretation found, check for classical protocol interpretation
            c_NewMessage.c_ProtocolTextHex = this->m_GetProtocolStringHex(orc_Msg).c_str();
            if (c_NewMessage.c_ProtocolTextHex != "")
            {
               // Only necessary if a protocol was found in the hex variant
               c_NewMessage.c_ProtocolTextDec = this->m_GetProtocolStringDec(orc_Msg).c_str();
            }
         }
      }

      // Update the counter
      if (orc_Msg.u8_XTD == 0U)
      {
         if (orc_Msg.u32_ID <= 0x7FF)
         {
            // Standard id
            ++this->mc_MsgCounterStandardId[orc_Msg.u32_ID];
            c_NewMessage.c_Counter = QString::number(this->mc_MsgCounterStandardId[orc_Msg.u32_ID]);
         }
      }
      else
      {
         std::map<stw_types::uint32, stw_types::uint32>::iterator c_ItCounter =
            this->mc_MsgCounterExtendedId.find(orc_Msg.u32_ID);

         if (c_ItCounter == this->mc_MsgCounterExtendedId.end())
         {
            // New message
            this->mc_MsgCounterExtendedId.insert(std::pair<uint32, uint32>(orc_Msg.u32_ID, 1U));
            c_NewMessage.c_Counter = "1";
         }
         else
         {
            ++c_ItCounter->second;
            c_NewMessage.c_Counter = QString::number(c_ItCounter->second);
         }
      }

      // Add the data to the list
      this->mc_CriticalSectionMsg.Acquire();
      this->mc_ReceivedMessages.push_back(c_NewMessage);
      this->mc_CriticalSectionMsg.Release();
   }

   return s32_Return;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Get result of previously started service execution

   Can be used to extract the results of one service execution after it has finished.

   \param[out]  ors32_Result       result code of executed service function
                                   for possible values see the DataDealer's function documentation

   \return
   C_NO_ERR       result code read
   C_BUSY         previously started polled communication still going on

   \created     19.09.2018  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
sint32 C_SyvComMessageMonitor::GetResults(sint32 & ors32_Result) const
{
   sint32 s32_Return = C_NO_ERR;

   if (this->mpc_LoadingThread->isRunning() == true)
   {
      s32_Return = C_BUSY;
   }
   else
   {
      ors32_Result = this->ms32_Result;
   }

   return s32_Return;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Get result of previously started service execution

   Can be used to extract the results of one service execution after it has finished.

   \param[out]  ors32_Result       result code of executed service function
                                   for possible values see the DataDealer's function documentation

   \return
   C_NO_ERR       result code read
   C_BUSY         previously started polled communication still going on

   \created     19.09.2018  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
sint32 C_SyvComMessageMonitor::GetResultBusses(std::vector<C_OSCSystemBus> & orc_Busses) const
{
   sint32 s32_Return = C_NO_ERR;

   if (this->mpc_LoadingThread->isRunning() == true)
   {
      s32_Return = C_BUSY;
   }
   else
   {
      orc_Busses = this->mc_Busses;
   }

   return s32_Return;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Gets the oldest message and removes it from the list

   \param[out]    orc_Message       Message from list
   This function is thread safe.

   \return
   C_NO_ERR    Message copied
   C_NOACT     No message available

   \created     05.09.2018  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
sint32 C_SyvComMessageMonitor::m_GetCanMessage(C_SyvComMessageMonitorData & orc_Message)
{
   sint32 s32_Return = C_NOACT;

   this->mc_CriticalSectionMsg.Acquire();
   if (this->mc_ReceivedMessages.size() > 0)
   {
      // Get oldest message in the list and remove it from the list
      orc_Message = this->mc_ReceivedMessages.first();
      this->mc_ReceivedMessages.removeFirst();

      s32_Return = C_NO_ERR;
   }
   this->mc_CriticalSectionMsg.Release();

   return s32_Return;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Gets all received CAN messages and clears the messages from the list

   The container orc_Messages will be cleared first.
   This function is thread safe.

   \param[out]    orc_Messages     Container for messages

   \created     05.09.2018  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
void C_SyvComMessageMonitor::m_GetCanMessages(QVector<C_SyvComMessageMonitorData> & orc_Messages)
{
   QList<C_SyvComMessageMonitorData>::const_iterator c_ItMessage;
   uint32 u32_Counter = 0U;

   orc_Messages.clear();

   this->mc_CriticalSectionMsg.Acquire();

   orc_Messages.resize(this->mc_ReceivedMessages.size());

   for (c_ItMessage = this->mc_ReceivedMessages.begin(); c_ItMessage != this->mc_ReceivedMessages.end(); ++c_ItMessage)
   {
      // Copy the message
      orc_Messages[u32_Counter] = *c_ItMessage;
      ++u32_Counter;
   }

   // Remove all messages
   this->mc_ReceivedMessages.clear();

   this->mc_CriticalSectionMsg.Release();
}

//-----------------------------------------------------------------------------
/*!
   \brief   Update of protocol string if the protocol was changed

   The current protocol setting of C_SyvComMessageMonitor will be used

   \param[in,out] orc_MessageData    Message data for updating

   \created     25.09.2018  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
void C_SyvComMessageMonitor::m_UpdateProtocolString(C_SyvComMessageMonitorData & orc_MessageData) const
{
   // If the name is not empty, the message was already interpreted by DBC or openSYDE system definition interpretation
   if (orc_MessageData.c_Name == "")
   {
      // No interpretation found, check for classical protocol interpretation
      orc_MessageData.c_ProtocolTextHex = this->m_GetProtocolStringHex(orc_MessageData.c_CanMsg).c_str();
      if (orc_MessageData.c_ProtocolTextHex != "")
      {
         // Only necessary if a protocol was found in the hex variant
         orc_MessageData.c_ProtocolTextDec = this->m_GetProtocolStringDec(orc_MessageData.c_CanMsg).c_str();
      }
      else
      {
         orc_MessageData.c_ProtocolTextDec = "";
      }
   }
}

//-----------------------------------------------------------------------------
/*!
   \brief   Checks a specific CAN message for matching the filter configuration

   This function is thread safe.

   \param[in]     orc_Msg        Current CAN message

   \return
   true     CAN message is relevant
   false    CAN message is not relevant

   \created     05.09.2018  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
bool C_SyvComMessageMonitor::m_CheckFilter(const T_STWCAN_Msg_RX & orc_Msg) const
{
   bool q_Return;

   this->mc_CriticalSectionConfig.Acquire();
   q_Return = C_OSCComMessageLogger::m_CheckFilter(orc_Msg);
   this->mc_CriticalSectionConfig.Release();

   return q_Return;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Inserts openSYDE system definition to parsing configuration

   This function is thread safe.

   \param[in]     orc_PathSystemDefinition   Path of system definition file (Must be .syde_sysdef)
   \param[in]     orc_OsySysDef              Loaded openSYDE system definition
   \param[in]     ou32_BusIndex              Used CAN bus index

   \created     19.09.2018  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
void C_SyvComMessageMonitor::m_InsertOsySysDef(const C_SCLString & orc_PathSystemDefinition,
                                               const C_OSCSystemDefinition & orc_OsySysDef, const uint32 ou32_BusIndex)
{
   this->mc_CriticalSectionConfig.Acquire();
   C_OSCComMessageLogger::m_InsertOsySysDef(orc_PathSystemDefinition, orc_OsySysDef, ou32_BusIndex);
   this->mc_CriticalSectionConfig.Release();
}

//-----------------------------------------------------------------------------
/*!
   \brief   Checks if a matching CAN message is defined in at least one registered openSYDE system definition

   This function is thread safe.

   \param[in]     orc_Msg             CAN message for analyzing

   \return
   true     Matching CAN message found
   false    No matching CAN message found

   \created     17.09.2018  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
bool C_SyvComMessageMonitor::m_CheckSysDef(const T_STWCAN_Msg_RX & orc_Msg)
{
   bool q_Return;

   this->mc_CriticalSectionConfig.Acquire();
   q_Return = C_OSCComMessageLogger::m_CheckSysDef(orc_Msg);
   this->mc_CriticalSectionConfig.Release();

   return q_Return;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Checks if a matching CAN message is defined in a description / specification

   \param[in]     orc_Msg             CAN message for analyzing

   \created     21.09.2018  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
void C_SyvComMessageMonitor::m_CheckInterpretation(const T_STWCAN_Msg_RX & orc_Msg)
{
   this->m_CheckDbcFile(orc_Msg);
}

//-----------------------------------------------------------------------------
/*!
   \brief   Returns an interpreted string of the loaded DBC files

   This is the default implementation and returns an empty string

   \return
   Interpreted string if match protocol found
   Empty string if no match found

   \created     21.09.2018  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
C_SCLString C_SyvComMessageMonitor::m_GetProtocolStringHexHook(void) const
{
   // TODO for logging
   return "";
}

//-----------------------------------------------------------------------------
/*!
   \brief   Returns an interpreted string of the loaded DBC files

   \return
   Interpreted string if match protocol found
   Empty string if no match found

   \created     21.09.2018  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
C_SCLString C_SyvComMessageMonitor::m_GetProtocolStringDecHook(void) const
{
   // TODO for logging
   return "";
}

//-----------------------------------------------------------------------------
/*!
   \brief   Interprets the system definition message

   This function is thread safe.

   \param[in,out] orc_MessageData    Message data target for the interpretation

   \return
   true     Matching CAN message exists
   false    No matching CAN message exists

   \created     21.09.2018  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
bool C_SyvComMessageMonitor::m_InterpretSysDef(C_SyvComMessageMonitorData & orc_MessageData) const
{
   bool q_Return = false;

   this->mc_CriticalSectionConfig.Acquire();

   if ((this->mpc_OsySysDefMessage != NULL) &&
       (this->mpc_OsySysDefDataPoolList != NULL))
   {
      uint32 u32_Counter;

      orc_MessageData.c_Name = this->mpc_OsySysDefMessage->c_Name.c_str();

      // Interpret the signals
      for (u32_Counter = 0U; u32_Counter < this->mpc_OsySysDefMessage->c_Signals.size(); ++u32_Counter)
      {
         const C_OSCCanSignal & rc_OscSignal = this->mpc_OsySysDefMessage->c_Signals[u32_Counter];

         if (rc_OscSignal.u32_ComDataElementIndex < this->mpc_OsySysDefDataPoolList->c_Elements.size())
         {
            C_SyvComMessageMonitorDataSignal c_Signal;
            const C_OSCNodeDataPoolListElement & rc_OscElement =
               this->mpc_OsySysDefDataPoolList->c_Elements[rc_OscSignal.u32_ComDataElementIndex];

            c_Signal.c_Name = rc_OscElement.c_Name.c_str();
            c_Signal.c_Unit = rc_OscElement.c_Unit.c_str();
            c_Signal.c_Comment = rc_OscElement.c_Comment.c_str();

            // Get the current value for the correct type configuration
            mh_InterpretCanSignalValue(c_Signal, orc_MessageData.c_CanMsg.au8_Data, orc_MessageData.c_CanMsg.u8_DLC,
                                       rc_OscSignal, rc_OscElement.c_Value, rc_OscElement.f64_Factor,
                                       rc_OscElement.f64_Offset);

            orc_MessageData.c_Signals.push_back(c_Signal);
         }
      }

      q_Return = true;
   }

   this->mc_CriticalSectionConfig.Release();

   return q_Return;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Adds a DBC file for interpretation

   \param[in]     orc_PathDbc      File path to DBC file

   \return
   C_NO_ERR    required data from file successfully stored in orc_Definition
   C_RANGE     orc_File is empty string
   C_CONFIG    orc_File does not point to a valid file
   C_RD_WR     error while reading file
   C_WARN      unknown parameter found -> default value set and error reported

   \created     21.09.2018  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
sint32 C_SyvComMessageMonitor::m_AddDbcFile(const C_SCLString & orc_PathDbc)
{
   sint32 s32_Return;

   C_CieConverter::C_CIECommDefinition c_DbcDefinition;
   C_SCLStringList c_WarningMsgs;
   C_SCLString c_ErrorMsg;

   s32_Return = C_CieImportDbc::h_ImportNetwork(orc_PathDbc, c_DbcDefinition, c_WarningMsgs, c_ErrorMsg);

   if (s32_Return == C_NO_ERR)
   {
      this->mc_CriticalSectionConfig.Acquire();
      this->mc_DbcFiles.insert(std::pair<C_SCLString, C_CieConverter::C_CIECommDefinition>(orc_PathDbc,
                                                                                           c_DbcDefinition));
      this->mc_CriticalSectionConfig.Release();
   }

   return s32_Return;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Checks if a matching CAN message is defined in at least one registered DBC file and interprets the data

   This function is thread safe.

   \param[in]     orc_Msg        Current CAN message
   \param[in,out] orc_MessageData    Message data target for the interpretation

   \return
   possible return value(s) and description

   \created     21.09.2018  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
void C_SyvComMessageMonitor::m_CheckDbcFile(const T_STWCAN_Msg_RX & orc_Msg)
{
   std::map<stw_scl::C_SCLString, C_CieConverter::C_CIECommDefinition>::const_iterator c_ItDbc;

   this->mc_CriticalSectionConfig.Acquire();

   this->mpc_DbcMessage = NULL;

   // Search for CAN message in all DBC files
   for (c_ItDbc = this->mc_DbcFiles.begin(); c_ItDbc != this->mc_DbcFiles.end(); ++c_ItDbc)
   {
      uint32 u32_NodeCounter;

      for (u32_NodeCounter = 0U; u32_NodeCounter < c_ItDbc->second.c_Nodes.size(); ++u32_NodeCounter)
      {
         const C_CieConverter::C_CIENode & rc_Node = c_ItDbc->second.c_Nodes[u32_NodeCounter];
         uint32 u32_MsgCounter;

         for (u32_MsgCounter = 0U; u32_MsgCounter < rc_Node.c_TxMessages.size(); ++u32_MsgCounter)
         {
            const C_CieConverter::C_CIECanMessage & rc_Msg = rc_Node.c_TxMessages[u32_MsgCounter].c_CanMessage;

            // No check of dlc here, it will be checked for each signal
            if ((orc_Msg.u32_ID == rc_Msg.u32_CanId) &&
                ((orc_Msg.u8_XTD == 1U) == rc_Msg.q_IsExtended))
            {
               // Matching CAN message found
               this->mpc_DbcMessage = &rc_Msg;
               break;
            }
         }

         if (this->mpc_DbcMessage == NULL)
         {
            // Not found yet, search in RX messages
            for (u32_MsgCounter = 0U; u32_MsgCounter < rc_Node.c_RxMessages.size(); ++u32_MsgCounter)
            {
               const C_CieConverter::C_CIECanMessage & rc_Msg = rc_Node.c_RxMessages[u32_MsgCounter].c_CanMessage;

               // No check of dlc here, it will be checked for each signal
               if ((orc_Msg.u32_ID == rc_Msg.u32_CanId) &&
                   ((orc_Msg.u8_XTD == 1U) == rc_Msg.q_IsExtended))
               {
                  // Matching CAN message found
                  this->mpc_DbcMessage = &rc_Msg;
                  break;
               }
            }
         }

         if (this->mpc_DbcMessage != NULL)
         {
            break;
         }
      }
   }

   this->mc_CriticalSectionConfig.Release();
}

//-----------------------------------------------------------------------------
/*!
   \brief   Checks if a matching CAN message is defined in at least one registered DBC file and interprets the data

   This function is thread safe.

   \param[in]     orc_Msg        Current CAN message
   \param[in,out] orc_MessageData    Message data target for the interpretation

   \return
   possible return value(s) and description

   \created     21.09.2018  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
bool C_SyvComMessageMonitor::m_InterpretDbcFile(C_SyvComMessageMonitorData & orc_MessageData) const
{
   bool q_Return = false;

   this->mc_CriticalSectionConfig.Acquire();

   if (this->mpc_DbcMessage != NULL)
   {
      uint32 u32_SignalCounter;
      orc_MessageData.c_Name = this->mpc_DbcMessage->c_Name.c_str();

      // Interpret the signals
      for (u32_SignalCounter = 0U; u32_SignalCounter < this->mpc_DbcMessage->c_Signals.size(); ++u32_SignalCounter)
      {
         C_SyvComMessageMonitorDataSignal c_Signal;
         C_OSCCanSignal c_OscSignal;
         const C_CieConverter::C_CIECanSignal & rc_DbcSignal = this->mpc_DbcMessage->c_Signals[u32_SignalCounter];

         c_Signal.c_Name = rc_DbcSignal.c_Element.c_Name.c_str();
         c_Signal.c_Unit = rc_DbcSignal.c_Element.c_Unit.c_str();
         c_Signal.c_Comment = rc_DbcSignal.c_Element.c_Comment.c_str();

         // Using of the openSYDE signal class for using common utility functions
         c_OscSignal.e_ComByteOrder = rc_DbcSignal.e_ComByteOrder;
         c_OscSignal.u16_ComBitLength = rc_DbcSignal.u16_ComBitLength;
         c_OscSignal.u16_ComBitStart = rc_DbcSignal.u16_ComBitStart;

         // Get the the minimum value for the correct type configuration
         mh_InterpretCanSignalValue(c_Signal, orc_MessageData.c_CanMsg.au8_Data, orc_MessageData.c_CanMsg.u8_DLC,
                                    c_OscSignal,
                                    rc_DbcSignal.c_Element.c_MinValue,
                                    rc_DbcSignal.c_Element.f64_Factor,
                                    rc_DbcSignal.c_Element.f64_Offset);

         orc_MessageData.c_Signals.push_back(c_Signal);
      }

      q_Return = true;
   }

   this->mc_CriticalSectionConfig.Release();

   return q_Return;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Interprets the received CAN signal value

   \param[in,out]     orc_Signal        Signal interpretation
   \param[in]         orau8_CanDb       RAW CAN message data
   \param[in]         ou8_CanDlc        DLC of CAN message
   \param[in]         orc_OscSignal     openSYDE CAN signal configuration
   \param[in]         orc_OscValue      Datapool element content for type configuration
   \param[in]         of64_Factor       Factor for resulting value
   \param[in]         of64_Offset       Offset for resulting value

   \created     26.09.2018  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
void C_SyvComMessageMonitor::mh_InterpretCanSignalValue(C_SyvComMessageMonitorDataSignal & orc_Signal,
                                                        const uint8(&orau8_CanDb)[8], const uint8 ou8_CanDlc,
                                                        const C_OSCCanSignal & orc_OscSignal,
                                                        const C_OSCNodeDataPoolContent & orc_OscValue,
                                                        const float64 of64_Factor, const float64 of64_Offset)
{
   const bool q_SignalFits = C_OSCCanUtil::h_IsSignalInMessage(ou8_CanDlc, orc_OscSignal);

   if (q_SignalFits == true)
   {
      uint8 u8_RawDataCounter;
      std::vector<uint8> c_SignalRawData;
      std::string c_StringValue;
      C_OSCNodeDataPoolContent c_OscValue = orc_OscValue;
      uint64 u64_Value = 0U;

      // Get the signal as raw value in the first step
      c_SignalRawData.resize(c_OscValue.GetSizeByte());
      C_OSCCanUtil::h_GetSignalValue(orau8_CanDb, orc_OscSignal, c_SignalRawData);
      // And the interpreted value
      c_OscValue.SetValueFromLittleEndianBlob(c_SignalRawData);

      // Raw values
      // Signal data is converted to little endian by h_GetSignalValue
      for (u8_RawDataCounter = 0U; u8_RawDataCounter < c_SignalRawData.size(); ++u8_RawDataCounter)
      {
         u64_Value += static_cast<uint64>(c_SignalRawData[u8_RawDataCounter] << (u8_RawDataCounter * 8U));
      }

      orc_Signal.c_RawValueDec = QString::number(u64_Value, 10);
      orc_Signal.c_RawValueHex = QString::number(u64_Value, 16).toUpper();

      // Interpreted value
      c_OscValue.GetValueAsScaledString(of64_Factor, of64_Offset, c_StringValue, 0U);
      orc_Signal.c_Value = c_StringValue.c_str();
   }
   else
   {
      // Signal is outside of the set data bytes (DLC is not big enough)
      orc_Signal.c_RawValueDec = "N/A";
      orc_Signal.c_RawValueHex = "N/A";
      orc_Signal.c_Value = "N/A";

      orc_Signal.q_DlcError = true;
   }

   orc_Signal.c_OscSignal = orc_OscSignal;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Function for continuous calling by thread.

   \created     19.09.2018  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
void C_SyvComMessageMonitor::mh_ThreadFunc(void * const opv_Instance)
{
   //lint -e{925}  This class is the only one which registers itself at the caller of this function. It must match.
   C_SyvComMessageMonitor * const pc_Instance = reinterpret_cast<C_SyvComMessageMonitor * const>(opv_Instance);

   tgl_assert(pc_Instance != NULL);
   if (pc_Instance != NULL)
   {
      pc_Instance->m_ThreadFunc();
   }
}

//-----------------------------------------------------------------------------
/*!
   \brief   The functions executed by the system's threading engine

   Calls the function configured via me_LoadingActivity

   \created     19.09.2018  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
void C_SyvComMessageMonitor::m_ThreadFunc(void)
{
   switch (this->me_LoadingActivity)
   {
   case eNOT_ACTIVE:
      // Nothing to do. Should not happen.
      break;
   case eADD_OSY_SYSDEF_WITHOUT_BUSINDEX:
      this->ms32_Result = this->AddOsySysDef(this->mc_Path, this->mc_PathDeviceDefinitions,
                                             this->mc_Busses);
      break;
   case eADD_OSY_SYSDEF_WITH_BUSINDEX:
      this->ms32_Result = this->AddOsySysDef(this->mc_Path, this->mc_PathDeviceDefinitions,
                                             this->mu32_BusIndex);
      break;
   case eADD_DBC_FILE:
      this->ms32_Result = this->m_AddDbcFile(this->mc_Path);
      break;
   }

   this->me_LoadingActivity = eNOT_ACTIVE;

   this->mpc_LoadingThread->requestInterruption();
}
