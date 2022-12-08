//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Class for preparing CAN message data for showing on GUI (implementation)

   \copyright   Copyright 2018 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.hpp"

#include "stwtypes.hpp"
#include "stwerrors.hpp"

#include "TglUtils.hpp"

#include "C_SyvComMessageMonitor.hpp"
#include "C_CieImportDbc.hpp"
#include "C_OscLoggingHandler.hpp"
#include "C_SyvComMessageLoggerFileBlf.hpp"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw::errors;
using namespace stw::scl;
using namespace stw::can;
using namespace stw::opensyde_core;
using namespace stw::opensyde_gui_logic;

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
C_SyvComMessageMonitor::C_SyvComMessageMonitor(void) :
   C_OscComMessageLogger(),
   me_LoadingActivity(eNOT_ACTIVE),
   mc_Path(""),
   mu32_BusIndex(0U),
   ms32_Result(0),
   mu8_BusLoad(0U),
   mu32_TxMessages(0U),
   mu32_TxErrors(0U)
{
   mpc_LoadingThread = new C_SyvComDriverThread(&C_SyvComMessageMonitor::mh_ThreadFunc, this);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default destructor
*/
//----------------------------------------------------------------------------------------------------------------------
C_SyvComMessageMonitor::~C_SyvComMessageMonitor(void) noexcept
{
   if (this->mpc_LoadingThread != NULL)
   {
      try
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
      }
      catch (...)
      {
         //not much we can do here ...
      }
      delete mpc_LoadingThread;
      mpc_LoadingThread = NULL;
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Function to react on the start of the communication

   Reset filtered message counter
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvComMessageMonitor::Start(void)
{
   this->mc_CriticalSectionMsg.Acquire();
   // Erase queue in case of old messages
   this->mc_ReceivedMessages.clear();
   this->mc_CriticalSectionMsg.Release();

   this->mc_CriticalSectionConfig.Acquire();
   // Accessing to message filter counter. Need synchronization
   C_OscComMessageLogger::Start();
   this->mc_CriticalSectionConfig.Release();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Function to react on the stop of the communication

   Reset all CAN message counter
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvComMessageMonitor::Stop(void)
{
   this->mc_CriticalSectionCounter.Acquire();
   C_OscComMessageLogger::Stop();
   this->mc_CriticalSectionCounter.Release();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Starts thread to add openSYDE system definition

   \param[in]  orc_PathSystemDefinition   Path of system definition file (Must be .syde_sysdef)

   \return
   C_NO_ERR   started sequence
   C_BUSY     previously started sequence still going on
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_SyvComMessageMonitor::StartAddOsySysDef(const C_SclString & orc_PathSystemDefinition)
{
   int32_t s32_Return = C_NO_ERR;

   if (this->mpc_LoadingThread->isRunning() == true)
   {
      s32_Return = C_BUSY;
   }
   else
   {
      this->mc_Path = orc_PathSystemDefinition;
      this->mc_Busses.clear();
      this->me_LoadingActivity = eADD_OSY_SYSDEF_WITHOUT_BUSINDEX;
      this->mpc_LoadingThread->start();
   }
   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Starts thread to add openSYDE system definition

   \param[in]  orc_PathSystemDefinition   Path of system definition file (Must be .syde_sysdef)
   \param[in]  ou32_BusIndex              Bus index of CAN bus of system definition for monitoring

   \return
   C_NO_ERR   started sequence
   C_BUSY     previously started sequence still going on
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_SyvComMessageMonitor::StartAddOsySysDef(const C_SclString & orc_PathSystemDefinition,
                                                  const uint32_t ou32_BusIndex)
{
   int32_t s32_Return = C_NO_ERR;

   if (this->mpc_LoadingThread->isRunning() == true)
   {
      s32_Return = C_BUSY;
   }
   else
   {
      this->mc_Path = orc_PathSystemDefinition;
      this->mu32_BusIndex = ou32_BusIndex;
      this->mc_Busses.clear();
      this->me_LoadingActivity = eADD_OSY_SYSDEF_WITH_BUSINDEX;
      this->mpc_LoadingThread->start();
   }
   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Updates the set bus index of the specific system definition

   This function is thread safe.

   \param[in]  orc_PathSystemDefinition   Path of system definition file (Must be .syde_sysdef)
   \param[in]  ou32_BusIndex              Bus index of CAN bus of system definition for monitoring

   \return
   C_NO_ERR    Bus index for this system definition adapted
   C_NOACT     No system definition found with this path
   C_WARN      specified bus index was not found or is no CAN bus
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_SyvComMessageMonitor::SetOsySysDefBus(const C_SclString & orc_PathSystemDefinition,
                                                const uint32_t ou32_BusIndex)
{
   int32_t s32_Return;

   this->mc_CriticalSectionConfig.Acquire();
   s32_Return = C_OscComMessageLogger::SetOsySysDefBus(orc_PathSystemDefinition, ou32_BusIndex);
   this->mc_CriticalSectionConfig.Release();

   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Returns the entire loaded openSYDE system definition

   \param[in]      orc_PathSystemDefinition  Path and filename of openSYDE system definition file
   \param[in,out]  orc_SystemDefinition      Loaded system definition

   \return
   C_NO_ERR    openSYDE system definition returned
   C_RANGE     openSYDE system definition not found
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_SyvComMessageMonitor::GetOsySysDef(const C_SclString & orc_PathSystemDefinition,
                                             C_OscComMessageLoggerOsySysDefConfig & orc_SystemDefinition)
{
   int32_t s32_Return;

   this->mc_CriticalSectionConfig.Acquire();
   s32_Return = C_OscComMessageLogger::GetOsySysDef(orc_PathSystemDefinition, orc_SystemDefinition);
   this->mc_CriticalSectionConfig.Release();

   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Starts thread to add a DBC file

   \param[in]  orc_PathDbc    Path and filename of DBC file

   \return
   C_NO_ERR   started sequence
   C_BUSY     previously started sequence still going on
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_SyvComMessageMonitor::StartAddDbcFile(const C_SclString & orc_PathDbc)
{
   int32_t s32_Return = C_NO_ERR;

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

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Returns the entire loaded DBC definition

   \param[in]   orc_PathDbc         Path and filename of DBC file
   \param[out]  orc_DbcDefinition   Loaded DBC definition

   \return
   C_NO_ERR    DBC definition returned
   C_RANGE     DBC definition not found
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_SyvComMessageMonitor::GetDbcFile(const C_SclString & orc_PathDbc,
                                           C_CieConverter::C_CieCommDefinition & orc_DbcDefinition)
{
   int32_t s32_Return = C_RANGE;

   std::map<stw::scl::C_SclString, C_CieConverter::C_CieCommDefinition>::iterator c_ItDbc;

   this->mc_CriticalSectionConfig.Acquire();

   c_ItDbc = this->mc_DbcFiles.find(orc_PathDbc);
   if (c_ItDbc != this->mc_DbcFiles.end())
   {
      // Copy the DBC definition
      orc_DbcDefinition = c_ItDbc->second;
      s32_Return = C_NO_ERR;
   }

   this->mc_CriticalSectionConfig.Release();

   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Removes a database file

   This function is thread safe.

   \param[in]  orc_Path    Path and filename of database file

   \return
   C_NO_ERR    Database removed
   C_NOACT     No database found with this path
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_SyvComMessageMonitor::RemoveDatabase(const C_SclString & orc_Path)
{
   int32_t s32_Return = C_NOACT;

   std::map<stw::scl::C_SclString, C_CieConverter::C_CieCommDefinition>::iterator c_ItDbc;

   this->mc_CriticalSectionConfig.Acquire();
   c_ItDbc = this->mc_DbcFiles.find(orc_Path);
   if (c_ItDbc != this->mc_DbcFiles.end())
   {
      // Remove the entry
      this->mc_DbcFiles.erase(c_ItDbc);

      s32_Return = C_NO_ERR;
   }

   if (s32_Return == C_NOACT)
   {
      // No DBC file found, let try the base class
      s32_Return = C_OscComMessageLogger::RemoveDatabase(orc_Path);
   }
   this->mc_CriticalSectionConfig.Release();

   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Activate or deactivate a registered database

   \param[in]  orc_Path    Path and filename of database file
   \param[in]  oq_Active   Active flag

   \return
   C_NO_ERR    Database removed
   C_NOACT     No database found with this path
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_SyvComMessageMonitor::ActivateDatabase(const C_SclString & orc_Path, const bool oq_Active)
{
   int32_t s32_Return;

   //Logger handling
   this->mc_CriticalSectionConfig.Acquire();
   s32_Return = C_OscComMessageLogger::ActivateDatabase(orc_Path, oq_Active);
   this->mc_CriticalSectionConfig.Release();

   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Adds an ASC log file

   \param[in]  orc_FilePath                  Path with file name. File extension must be .asc
   \param[in]  oq_HexActive                  Mode for writing CAN Id and CAN data (hexadecimal or decimal)
   \param[in]  oq_RelativeTimeStampActive    Mode for writing CAN timestamp (relative or absolute)

   \return
   C_NO_ERR    File added successfully
   C_RD_WR     Error on creating file, folders or deleting old file
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_SyvComMessageMonitor::AddLogFileAsc(const C_SclString & orc_FilePath, const bool oq_HexActive,
                                              const bool oq_RelativeTimeStampActive)
{
   int32_t s32_Return;

   this->mc_CriticalSectionConfig.Acquire();
   s32_Return = C_OscComMessageLogger::AddLogFileAsc(orc_FilePath, oq_HexActive, oq_RelativeTimeStampActive);
   this->mc_CriticalSectionConfig.Release();

   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Adds an BLF log file

   \param[in]  orc_FilePath   Path with file name. File extension must be .blf

   \return
   C_NO_ERR    File added successfully
   C_RD_WR     Error on creating file, folders or deleting old file
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_SyvComMessageMonitor::AddLogFileBlf(const C_SclString & orc_FilePath)
{
   int32_t s32_Return;
   C_SyvComMessageLoggerFileBlf * const pc_File = new C_SyvComMessageLoggerFileBlf(orc_FilePath);

   s32_Return = pc_File->OpenFile();

   this->mc_CriticalSectionConfig.Acquire();
   this->mc_LoggingFiles.emplace(std::pair<C_SclString,
                                           C_OscComMessageLoggerFileBase * const>(orc_FilePath, pc_File));
   this->mc_CriticalSectionConfig.Release();

   //lint -e{429}  no memory leak of pc_File because of handling of instance in map mc_LoggingFiles
   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Remove an specific ASC log file

   The written file is not removed. The log file is removed from the list of active log files.

   \param[in]  orc_FilePath   Path with file name. File extension must be .asc

   \return
   C_NO_ERR    File removed
   C_NOACT     No file with this path registered
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_SyvComMessageMonitor::RemoveLogFile(const C_SclString & orc_FilePath)
{
   int32_t s32_Return;

   this->mc_CriticalSectionConfig.Acquire();
   s32_Return = C_OscComMessageLogger::RemoveLogFile(orc_FilePath);
   this->mc_CriticalSectionConfig.Release();

   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Remove all log files

   The written files are not removed. The log files are removed from the list of active log files.
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvComMessageMonitor::RemoveAllLogFiles(void)
{
   this->mc_CriticalSectionConfig.Acquire();
   C_OscComMessageLogger::RemoveAllLogFiles();
   this->mc_CriticalSectionConfig.Release();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Adds a new filter configuration

   This function is thread safe.

   \param[in]  orc_Filter  Filter configuration to add
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvComMessageMonitor::AddFilter(const C_OscComMessageLoggerFilter & orc_Filter)
{
   this->mc_CriticalSectionConfig.Acquire();
   C_OscComMessageLogger::AddFilter(orc_Filter);
   this->mc_CriticalSectionConfig.Release();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Removes a filter configuration

   This function is thread safe.

   \param[in]  orc_Filter  Filter configuration to remove
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvComMessageMonitor::RemoveFilter(const C_OscComMessageLoggerFilter & orc_Filter)
{
   this->mc_CriticalSectionConfig.Acquire();
   C_OscComMessageLogger::RemoveFilter(orc_Filter);
   this->mc_CriticalSectionConfig.Release();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Removes the entire filter configuration
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvComMessageMonitor::RemoveAllFilter(void)
{
   this->mc_CriticalSectionConfig.Acquire();
   C_OscComMessageLogger::RemoveAllFilter();
   this->mc_CriticalSectionConfig.Release();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Returns the current count of filtered CAN messages

   This function is thread safe

   \return
   Current count of filtered CAN messages
*/
//----------------------------------------------------------------------------------------------------------------------
uint32_t C_SyvComMessageMonitor::GetFilteredMessages(void) const
{
   uint32_t u32_FilteredMessages;

   this->mc_CriticalSectionConfig.Acquire();
   u32_FilteredMessages = C_OscComMessageLogger::GetFilteredMessages();
   this->mc_CriticalSectionConfig.Release();

   return u32_FilteredMessages;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   If necessary handle and prepare the CAN message for showing all necessary information

   This function is thread safe.

   \param[in]  orc_Msg  Current CAN message
   \param[in]  oq_IsTx  Message was sent of this application itself

   \return
   C_NO_ERR    CAN message logged
   C_NOACT     CAN message not relevant
   C_BUSY      Monitor is paused
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_SyvComMessageMonitor::HandleCanMessage(const T_STWCAN_Msg_RX & orc_Msg, const bool oq_IsTx)
{
   int32_t s32_Return;

   this->mc_CriticalSectionCounter.Acquire();
   s32_Return = C_OscComMessageLogger::HandleCanMessage(orc_Msg, oq_IsTx);
   this->mc_CriticalSectionCounter.Release();

   if (s32_Return == C_NO_ERR)
   {
      // Add the interpreted data to the list
      this->mc_CriticalSectionMsg.Acquire();
      this->mc_ReceivedMessages.push_back(this->m_GetHandledCanMessage());
      this->mc_CriticalSectionMsg.Release();
   }

   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Resets all CAN message counter
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvComMessageMonitor::ResetCounter(void)
{
   this->mc_CriticalSectionCounter.Acquire();
   C_OscComMessageLogger::ResetCounter();
   this->mc_CriticalSectionCounter.Release();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Function for getting the current bus load

   This function is thread safe

   \param[in]  ou8_BusLoad    Current CAN bus load in percentage
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvComMessageMonitor::UpdateBusLoad(const uint8_t ou8_BusLoad)
{
   this->mc_CriticalSectionMeta.Acquire();
   this->mu8_BusLoad = ou8_BusLoad;
   this->mc_CriticalSectionMeta.Release();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Function for getting the current count of all Tx messages

   This function is thread safe

   \param[in]  ou32_TxCount  Current number of all CAN Tx messages
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvComMessageMonitor::UpdateTxCounter(const uint32_t ou32_TxCount)
{
   this->mc_CriticalSectionMeta.Acquire();
   this->mu32_TxMessages = ou32_TxCount;
   this->mc_CriticalSectionMeta.Release();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Function for getting the current count of Tx errors

   This function is thread safe

   \param[in]  ou32_TxErrors  Current detected CAN Tx errors
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvComMessageMonitor::UpdateTxErrors(const uint32_t ou32_TxErrors)
{
   this->mc_CriticalSectionMeta.Acquire();
   this->mu32_TxErrors = ou32_TxErrors;
   this->mc_CriticalSectionMeta.Release();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Returns the current bus load

   This function is thread safe

   \return
   Current bus load in percentage
*/
//----------------------------------------------------------------------------------------------------------------------
uint8_t C_SyvComMessageMonitor::GetBusLoad(void) const
{
   uint8_t u8_BusLoad;

   this->mc_CriticalSectionMeta.Acquire();
   u8_BusLoad = this->mu8_BusLoad;
   this->mc_CriticalSectionMeta.Release();

   return u8_BusLoad;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Returns the current count of Tx messages

   This function is thread safe

   \return
   Current number of Tx messages
*/
//----------------------------------------------------------------------------------------------------------------------
uint32_t C_SyvComMessageMonitor::GetTxCount() const
{
   uint32_t u32_TxCount;

   this->mc_CriticalSectionMeta.Acquire();
   u32_TxCount = this->mu32_TxMessages;
   this->mc_CriticalSectionMeta.Release();

   return u32_TxCount;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Returns the current count of Tx errors

   This function is thread safe

   \return
   Current count of Tx errors
*/
//----------------------------------------------------------------------------------------------------------------------
uint32_t C_SyvComMessageMonitor::GetTxErrors(void) const
{
   uint32_t u32_TxErrors;

   this->mc_CriticalSectionMeta.Acquire();
   u32_TxErrors = this->mu32_TxErrors;
   this->mc_CriticalSectionMeta.Release();

   return u32_TxErrors;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get result of previously started service execution

   Can be used to extract the results of one service execution after it has finished.

   \param[out]  ors32_Result  result code of executed service function
                              for possible values see the DataDealer's function documentation

   \return
   C_NO_ERR       result code read
   C_BUSY         previously started polled communication still going on
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_SyvComMessageMonitor::GetResults(int32_t & ors32_Result) const
{
   int32_t s32_Return = C_NO_ERR;

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

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get result of previously started service execution

   Can be used to extract the results of one service execution after it has finished.

   \param[in,out]  orc_Busses    Busses

   \return
   C_NO_ERR       result code read
   C_BUSY         previously started polled communication still going on
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_SyvComMessageMonitor::GetResultBusses(std::vector<C_OscSystemBus> & orc_Busses) const
{
   int32_t s32_Return = C_NO_ERR;

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

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Gets the oldest message and removes it from the list

   \param[out]  orc_Message   Message from list
                              This function is thread safe.

   \return
   C_NO_ERR    Message copied
   C_NOACT     No message available
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_SyvComMessageMonitor::m_GetCanMessage(C_OscComMessageLoggerData & orc_Message)
{
   int32_t s32_Return = C_NOACT;

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

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Update of protocol string if the protocol was changed

   The current protocol setting of C_SyvComMessageMonitor will be used

   \param[in,out]  orc_MessageData  Message data for updating
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvComMessageMonitor::m_UpdateProtocolString(C_OscComMessageLoggerData & orc_MessageData) const
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

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Checks a specific CAN message for matching the filter configuration

   This function is thread safe.

   \param[in]  orc_Msg  Current CAN message

   \return
   true     CAN message is relevant
   false    CAN message is not relevant
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_SyvComMessageMonitor::m_CheckFilter(const T_STWCAN_Msg_RX & orc_Msg)
{
   bool q_Return;

   this->mc_CriticalSectionConfig.Acquire();
   q_Return = C_OscComMessageLogger::m_CheckFilter(orc_Msg);
   this->mc_CriticalSectionConfig.Release();

   return q_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Inserts openSYDE system definition to parsing configuration

   This function is thread safe.

   \param[in]  orc_PathSystemDefinition   Path of system definition file (Must be .syde_sysdef)
   \param[in]  orc_OsySysDef              Loaded openSYDE system definition
   \param[in]  ou32_BusIndex              Used CAN bus index
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvComMessageMonitor::m_InsertOsySysDef(const C_SclString & orc_PathSystemDefinition,
                                               const C_OscSystemDefinition & orc_OsySysDef,
                                               const uint32_t ou32_BusIndex)
{
   this->mc_CriticalSectionConfig.Acquire();
   C_OscComMessageLogger::m_InsertOsySysDef(orc_PathSystemDefinition, orc_OsySysDef, ou32_BusIndex);
   this->mc_CriticalSectionConfig.Release();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Checks if a matching CAN message is defined in at least one registered openSYDE system definition

   This function is thread safe.

   \param[in]  orc_Msg  CAN message for analyzing

   \return
   true     Matching CAN message found
   false    No matching CAN message found
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_SyvComMessageMonitor::m_CheckSysDef(const T_STWCAN_Msg_RX & orc_Msg)
{
   bool q_Return;

   this->mc_CriticalSectionConfig.Acquire();
   q_Return = C_OscComMessageLogger::m_CheckSysDef(orc_Msg);
   this->mc_CriticalSectionConfig.Release();

   return q_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Interprets the system definition message

   This function is thread safe.

   \param[in,out]  orc_MessageData  Message data target for the interpretation

   \return
   true     Matching CAN message exists
   false    No matching CAN message exists
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_SyvComMessageMonitor::m_InterpretSysDef(C_OscComMessageLoggerData & orc_MessageData) const
{
   bool q_Return;

   this->mc_CriticalSectionConfig.Acquire();
   q_Return = C_OscComMessageLogger::m_InterpretSysDef(orc_MessageData);
   this->mc_CriticalSectionConfig.Release();

   return q_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Checks if a matching CAN message is defined in a description / specification

   Using the both members c_ProtocolTextDec and c_ProtocolTextHex in orc_MessageData for the result.

   \param[in,out]  orc_MessageData  CAN message data class for analyzing and for output

   \return
   true     Matching CAN message found
   false    No matching CAN message found
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_SyvComMessageMonitor::m_CheckInterpretation(stw::opensyde_core::C_OscComMessageLoggerData & orc_MessageData)
{
   bool q_Return = false;
   const C_CieConverter::C_CieCanMessage * const pc_DbcMessage = this->m_CheckDbcFile(orc_MessageData.c_CanMsg);

   if (pc_DbcMessage != NULL)
   {
      q_Return = this->m_InterpretDbcFile(pc_DbcMessage, orc_MessageData);
   }
   return q_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Returns an interpreted string of the loaded DBC files

   This is the default implementation and returns an empty string

   \return
   Interpreted string if match protocol found
   Empty string if no match found
*/
//----------------------------------------------------------------------------------------------------------------------
C_SclString C_SyvComMessageMonitor::m_GetProtocolStringHexHook(void) const
{
   // TODO for logging
   return "";
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Returns an interpreted string of the loaded DBC files

   \return
   Interpreted string if match protocol found
   Empty string if no match found
*/
//----------------------------------------------------------------------------------------------------------------------
C_SclString C_SyvComMessageMonitor::m_GetProtocolStringDecHook(void) const
{
   // TODO for logging
   return "";
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Adds a DBC file for interpretation

   \param[in]  orc_PathDbc    File path to DBC file

   \return
   C_NO_ERR    required data from file successfully stored in orc_Definition and DBC file added to interpretation
   C_RANGE     orc_File is empty string or has a wrong extension
   C_CONFIG    orc_File does not point to a valid file
   C_RD_WR     error while reading file
   C_WARN      unknown parameter found -> default value set and warning reported
               it is a warning only, required data from file successfully stored in orc_Definition and
               DBC file added to interpretation
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_SyvComMessageMonitor::m_AddDbcFile(const C_SclString & orc_PathDbc)
{
   int32_t s32_Return = C_RANGE;

   if (orc_PathDbc.SubString(orc_PathDbc.Length() - 3U, 4U).LowerCase() == ".dbc")
   {
      C_CieConverter::C_CieCommDefinition c_DbcDefinition;
      C_SclStringList c_WarningMsgs;
      C_SclString c_ErrorMsg;

      s32_Return = C_CieImportDbc::h_ImportNetwork(orc_PathDbc, c_DbcDefinition, c_WarningMsgs, c_ErrorMsg, true);

      if ((s32_Return == C_NO_ERR) ||
          (s32_Return == C_WARN))
      {
         this->mc_CriticalSectionConfig.Acquire();
         this->mc_DbcFiles.emplace(std::pair<C_SclString, C_CieConverter::C_CieCommDefinition>(orc_PathDbc,
                                                                                               c_DbcDefinition));

         // Register the database in the activation flag map
         this->mc_DatabaseActiveFlags.emplace(std::pair<C_SclString, bool>(orc_PathDbc, true));

         this->mc_CriticalSectionConfig.Release();
      }
   }

   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Checks if a matching CAN message is defined in at least one registered DBC file

   This function is thread safe.

   \param[in]  orc_Msg  Current CAN message

   \return
   Pointer     Matching CAN message found and pointer to message is returned
   NULL        No matching CAN message found
*/
//----------------------------------------------------------------------------------------------------------------------
const C_CieConverter::C_CieCanMessage * C_SyvComMessageMonitor::m_CheckDbcFile(const T_STWCAN_Msg_RX & orc_Msg)
{
   const C_CieConverter::C_CieCanMessage * pc_DbcMessage = NULL;

   std::map<stw::scl::C_SclString, C_CieConverter::C_CieCommDefinition>::const_iterator c_ItDbc;

   this->mc_CriticalSectionConfig.Acquire();

   // Search for CAN message in all DBC files
   for (c_ItDbc = this->mc_DbcFiles.begin(); c_ItDbc != this->mc_DbcFiles.end(); ++c_ItDbc)
   {
      // Check if the database is active
      if (this->mc_DatabaseActiveFlags[c_ItDbc->first] == true)
      {
         uint32_t u32_NodeCounter;

         //Nodes
         for (u32_NodeCounter = 0U; u32_NodeCounter < c_ItDbc->second.c_Nodes.size(); ++u32_NodeCounter)
         {
            const C_CieConverter::C_CieNode & rc_Node = c_ItDbc->second.c_Nodes[u32_NodeCounter];
            uint32_t u32_MsgCounter;

            for (u32_MsgCounter = 0U; u32_MsgCounter < rc_Node.c_TxMessages.size(); ++u32_MsgCounter)
            {
               const C_CieConverter::C_CieCanMessage & rc_Msg = rc_Node.c_TxMessages[u32_MsgCounter].c_CanMessage;

               // No check of dlc here, it will be checked for each signal
               if ((orc_Msg.u32_ID == rc_Msg.u32_CanId) &&
                   ((orc_Msg.u8_XTD == 1U) == rc_Msg.q_IsExtended))
               {
                  // Matching CAN message found
                  pc_DbcMessage = &rc_Msg;
                  break;
               }
            }

            if (pc_DbcMessage == NULL)
            {
               // Not found yet, search in Rx messages
               for (u32_MsgCounter = 0U; u32_MsgCounter < rc_Node.c_RxMessages.size(); ++u32_MsgCounter)
               {
                  const C_CieConverter::C_CieCanMessage & rc_Msg = rc_Node.c_RxMessages[u32_MsgCounter].c_CanMessage;

                  // No check of dlc here, it will be checked for each signal
                  if ((orc_Msg.u32_ID == rc_Msg.u32_CanId) &&
                      ((orc_Msg.u8_XTD == 1U) == rc_Msg.q_IsExtended))
                  {
                     // Matching CAN message found
                     pc_DbcMessage = &rc_Msg;
                     break;
                  }
               }
            }

            if (pc_DbcMessage != NULL)
            {
               break;
            }
         }

         //Unmapped messages
         if (pc_DbcMessage == NULL)
         {
            for (uint32_t u32_ItMessage = 0U; u32_ItMessage < c_ItDbc->second.c_UnmappedMessages.size();
                 ++u32_ItMessage)
            {
               const C_CieConverter::C_CieCanMessage & rc_Msg =
                  c_ItDbc->second.c_UnmappedMessages[u32_ItMessage].c_CanMessage;

               // No check of dlc here, it will be checked for each signal
               if ((orc_Msg.u32_ID == rc_Msg.u32_CanId) &&
                   ((orc_Msg.u8_XTD == 1U) == rc_Msg.q_IsExtended))
               {
                  // Matching CAN message found
                  pc_DbcMessage = &rc_Msg;
                  break;
               }
            }
         }
      }
   }

   this->mc_CriticalSectionConfig.Release();

   return pc_DbcMessage;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Checks if a matching CAN message is defined in at least one registered DBC file and interprets the data

   This function is thread safe.

   \param[in]      opc_DbcMessage   Detected DBC message for current CAN message
   \param[in,out]  orc_MessageData  Message data target for the interpretation

   \return
   true     Matching CAN message exists
   false    No matching CAN message exists
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_SyvComMessageMonitor::m_InterpretDbcFile(const C_CieConverter::C_CieCanMessage * const opc_DbcMessage,
                                                C_OscComMessageLoggerData & orc_MessageData) const
{
   bool q_Return = false;

   this->mc_CriticalSectionConfig.Acquire();

   if (opc_DbcMessage != NULL)
   {
      uint32_t u32_Counter;
      bool q_MultiplexerFound = false;
      uint32_t u32_MultiplexerIndex = 0U;
      uint16_t u16_MultiplexerValue = 0U;

      orc_MessageData.c_Name = opc_DbcMessage->c_Name.c_str();

      // Check if a multiplexer signal exists
      for (u32_Counter = 0U; u32_Counter < opc_DbcMessage->c_Signals.size(); ++u32_Counter)
      {
         const C_CieConverter::C_CieCanSignal & rc_DbcSignal = opc_DbcMessage->c_Signals[u32_Counter];

         if (rc_DbcSignal.e_MultiplexerType == C_OscCanSignal::eMUX_MULTIPLEXER_SIGNAL)
         {
            mh_InterpretDbcFileCanSignal(orc_MessageData, rc_DbcSignal);

            if (orc_MessageData.c_Signals.size() > 0)
            {
               C_OscComMessageLoggerDataSignal & rc_Signal =
                  orc_MessageData.c_Signals[orc_MessageData.c_Signals.size() - 1];

               if (rc_Signal.q_DlcError == false)
               {
                  try
                  {
                     u16_MultiplexerValue = static_cast<uint16_t>(rc_Signal.c_RawValueDec.ToInt());
                     rc_Signal.c_Name += " (Multiplexer)";
                     q_MultiplexerFound = true;
                     u32_MultiplexerIndex = u32_Counter;
                  }
                  catch (...)
                  {
                  }
               }
            }
            break;
         }
      }

      // Interpret the signals
      for (u32_Counter = 0U; u32_Counter < opc_DbcMessage->c_Signals.size(); ++u32_Counter)
      {
         // Skip the multiplexer signal if one was found. It was added already.
         if ((q_MultiplexerFound == false) ||
             (u32_Counter != u32_MultiplexerIndex))
         {
            const C_CieConverter::C_CieCanSignal & rc_DbcSignal = opc_DbcMessage->c_Signals[u32_Counter];

            // Interpret all not multiplexed signals and all multiplexed signals with the matching multiplexer value
            if ((rc_DbcSignal.e_MultiplexerType == C_OscCanSignal::eMUX_DEFAULT) ||
                ((q_MultiplexerFound == true) &&
                 (u16_MultiplexerValue == rc_DbcSignal.u16_MultiplexValue)))
            {
               mh_InterpretDbcFileCanSignal(orc_MessageData, rc_DbcSignal);
            }
         }
      }

      orc_MessageData.SortSignals();

      if (orc_MessageData.c_CanMsg.u8_DLC != opc_DbcMessage->u16_Dlc)
      {
         // CAN message does not have the expected size
         orc_MessageData.q_CanDlcError = true;
      }

      q_Return = true;
   }

   this->mc_CriticalSectionConfig.Release();

   return q_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Interprets the DBC message signal

   \param[in,out]  orc_MessageData  Message data target for the interpretation
   \param[in]      orc_DbcSignal    CAN signal of DBC file
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvComMessageMonitor::mh_InterpretDbcFileCanSignal(C_OscComMessageLoggerData & orc_MessageData,
                                                          const C_CieConverter::C_CieCanSignal & orc_DbcSignal)
{
   C_OscComMessageLoggerDataSignal c_Signal;
   C_OscCanSignal c_OscSignal;

   c_Signal.c_Name = orc_DbcSignal.c_Element.c_Name.c_str();
   c_Signal.c_Unit = orc_DbcSignal.c_Element.c_Unit.c_str();
   c_Signal.c_Comment = orc_DbcSignal.c_Element.c_Comment.c_str();

   // Using of the openSYDE signal class for using common utility functions
   c_OscSignal.e_ComByteOrder = orc_DbcSignal.e_ComByteOrder;
   c_OscSignal.u16_ComBitLength = orc_DbcSignal.u16_ComBitLength;
   c_OscSignal.u16_ComBitStart = orc_DbcSignal.u16_ComBitStart;
   c_OscSignal.e_MultiplexerType = orc_DbcSignal.e_MultiplexerType;
   c_OscSignal.u16_MultiplexValue = orc_DbcSignal.u16_MultiplexValue;

   // Get the the minimum value for the correct type configuration
   mh_InterpretCanSignalValue(c_Signal, orc_MessageData.c_CanMsg.au8_Data, orc_MessageData.c_CanMsg.u8_DLC,
                              c_OscSignal,
                              orc_DbcSignal.c_Element.c_MinValue,
                              orc_DbcSignal.c_Element.f64_Factor,
                              orc_DbcSignal.c_Element.f64_Offset);

   if (orc_DbcSignal.c_ValueDescription.size() > 0)
   {
      // Check if a value description matches to the current value
      try
      {
         const uint32_t u32_Value = static_cast<uint32_t>(c_Signal.c_RawValueDec.ToInt64());
         const std::map<uint32_t, stw::scl::C_SclString>::const_iterator c_ItDescription =
            orc_DbcSignal.c_ValueDescription.find(u32_Value);

         if (c_ItDescription != orc_DbcSignal.c_ValueDescription.end())
         {
            c_Signal.c_Value = c_ItDescription->second;
         }
      }
      catch (...)
      {
      }
   }

   orc_MessageData.c_Signals.push_back(c_Signal);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Function for continuous calling by thread.
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvComMessageMonitor::mh_ThreadFunc(void * const opv_Instance)
{
   //lint -e{9079}  This class is the only one which registers itself at the caller of this function. It must match.
   C_SyvComMessageMonitor * const pc_Instance = reinterpret_cast<C_SyvComMessageMonitor *>(opv_Instance);

   tgl_assert(pc_Instance != NULL);
   if (pc_Instance != NULL)
   {
      pc_Instance->m_ThreadFunc();
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   The functions executed by the system's threading engine

   Calls the function configured via me_LoadingActivity
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvComMessageMonitor::m_ThreadFunc(void)
{
   switch (this->me_LoadingActivity)
   {
   case eADD_OSY_SYSDEF_WITHOUT_BUSINDEX:
      this->ms32_Result = this->AddOsySysDef(this->mc_Path, this->mc_Busses);
      break;
   case eADD_OSY_SYSDEF_WITH_BUSINDEX:
      this->ms32_Result = this->AddOsySysDef(this->mc_Path, this->mu32_BusIndex, this->mc_Busses);
      break;
   case eADD_DBC_FILE:
      this->ms32_Result = this->m_AddDbcFile(this->mc_Path);
      break;
   case eNOT_ACTIVE:
   default:
      // Nothing to do. Should not happen.
      break;
   }

   this->me_LoadingActivity = eNOT_ACTIVE;

   this->mpc_LoadingThread->requestInterruption();
}
