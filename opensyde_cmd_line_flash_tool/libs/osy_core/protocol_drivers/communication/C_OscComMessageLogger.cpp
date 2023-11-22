//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Class for logging CAN messages (implementation)

   Handles:
   - CAN message filter (CAN-Id, protocol relevance)
   - Converting CAN message to protocol specific information
   - Writing to file

   \copyright   Copyright 2018 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.hpp"

#include <algorithm>

#include "stwerrors.hpp"

#include "TglTime.hpp"
#include "TglFile.hpp"

#include "C_OscLoggingHandler.hpp"
#include "C_OscComMessageLogger.hpp"
#include "C_OscSystemDefinitionFiler.hpp"
#include "C_CanMonProtocol.hpp"
#include "C_OscCanUtil.hpp"
#include "C_OscComMessageLoggerFileAsc.hpp"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */

using namespace stw::errors;
using namespace stw::can;
using namespace stw::scl;
using namespace stw::opensyde_core;
using namespace stw::cmon_protocol;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */
const uint64_t C_OscComMessageLogger::mhu64_MAX_TIMESTAMP_DAY_OF_TIME =
   ((((24ULL * 60ULL) * 60ULL) * 1000ULL) * 1000ULL);

const stw::scl::C_SclString C_OscComMessageLogger::mhc_ECES_MESSAGE_COUNTER = "ECeS_Message_Counter";
const stw::scl::C_SclString C_OscComMessageLogger::mhc_ECES_CHECKSUM = "ECeS_Checksum";
const uint32_t C_OscComMessageLogger::mhu32_ECES_MAX_MESSAGE_COUNTER = 255;

/* -- Types --------------------------------------------------------------------------------------------------------- */

/* -- Global Variables ---------------------------------------------------------------------------------------------- */

/* -- Module Global Variables --------------------------------------------------------------------------------------- */

/* -- Module Global Function Prototypes ----------------------------------------------------------------------------- */

/* -- Implementation ------------------------------------------------------------------------------------------------ */

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Default constructor
*/
//----------------------------------------------------------------------------------------------------------------------
C_OscComMessageLoggerFilter::C_OscComMessageLoggerFilter(void) :
   u8_ExtendedId(0U),
   u32_StartId(0U),
   u32_EndId(0U),
   q_PassFilter(false)
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Check if current equal to orc_Cmp

   \param[in] orc_Cmp Compared instance

   \return
   Current equal to orc_Cmp
   Else false
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_OscComMessageLoggerFilter::operator ==(const C_OscComMessageLoggerFilter & orc_Cmp) const
{
   bool q_Return = false;

   if ((this->u8_ExtendedId == orc_Cmp.u8_ExtendedId) &&
       (this->q_PassFilter == orc_Cmp.q_PassFilter) &&
       (this->u32_StartId == orc_Cmp.u32_StartId) &&
       (this->u32_EndId == orc_Cmp.u32_EndId))
   {
      q_Return = true;
   }

   return q_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Default constructor
*/
//----------------------------------------------------------------------------------------------------------------------
C_OscComMessageLogger::C_OscComMessageLogger(void) :
   mpc_OsySysDefMessage(NULL),
   mpc_OsySysDefDataPoolList(NULL),
   me_Protocol(stw::cmon_protocol::eCMON_L7_PROTOCOL_NONE),
   mq_Paused(false),
   mu64_FirstTimeStampStart(0U),
   mu64_FirstTimeStampDayOfTime(0U),
   mu64_LastTimeStamp(0U),
   mu32_FilteredMessages(0U),
   mpc_AutoSupportProtocol(new C_OscComAutoSupport())
{
   // Resize the vector for all potential CAN standard ids
   this->mc_MsgCounterStandardId.resize(0x800U, 0U);

   this->mc_ProtocolHex.SetDecimalMode(false);
   this->mc_ProtocolDec.SetDecimalMode(true);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Default destructor
*/
//----------------------------------------------------------------------------------------------------------------------
C_OscComMessageLogger::~C_OscComMessageLogger(void)
{
   try
   {
      C_OscComMessageLogger::RemoveAllLogFiles();
   }
   catch (...)
   {
   }
   mpc_OsySysDefMessage = NULL;
   mpc_OsySysDefDataPoolList = NULL;

   delete this->mpc_AutoSupportProtocol;
   this->mpc_AutoSupportProtocol = NULL;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Continues the paused the logging

   Continues the logging of this logger.
   The delivering instance of C_OscComDriverBase must be running logging too.
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OscComMessageLogger::Continue(void)
{
   this->mq_Paused = false;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Pauses the logging.

   Pauses the logging of this logger.
   The delivering instance of C_OscComDriverBase will not be changed by this pause.
   Other registered logger will still log.
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OscComMessageLogger::Pause(void)
{
   this->mq_Paused = true;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Function to react on the stop of the communication

   Reset all CAN message counter
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OscComMessageLogger::Stop(void)
{
   this->m_ResetCounter();
   this->m_ResetEcesMessages();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Function to react on the stop of the communication

   Reset filtered message counter and set the first timestamp for calculation of absolute timestamp relative to start
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OscComMessageLogger::Start(void)
{
   stw::tgl::C_TglDateTime c_Time;

   this->mq_Paused = false;
   this->mu32_FilteredMessages = 0U;

   stw::tgl::TglGetDateTimeNow(c_Time);

   // Init all reference timestamps on each start
   // Save first timestamp for starting at current time
   // Using the same function as the CAN message timestamps bases on
   this->mu64_FirstTimeStampStart = stw::tgl::TglGetTickCountUs();
   this->mu64_LastTimeStamp = this->mu64_FirstTimeStampStart;

   // Calculate the start time beginning at midnight too, for showing the absolute timestamp as time of day
   // In seconds
   this->mu64_FirstTimeStampDayOfTime =
      (((static_cast<uint64_t>(c_Time.mu8_Hour) * 60ULL) * 60ULL) +
       (static_cast<uint64_t>(c_Time.mu8_Minute) * 60ULL)) +
      static_cast<uint64_t>(c_Time.mu8_Second);
   // the ms
   this->mu64_FirstTimeStampDayOfTime *= 1000ULL;
   this->mu64_FirstTimeStampDayOfTime += static_cast<uint64_t>(c_Time.mu16_MilliSeconds);
   // in us
   this->mu64_FirstTimeStampDayOfTime *= 1000ULL;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Sets the protocol for interpreting

   \param[in] oe_Protocol Set protocol type
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OscComMessageLogger::SetProtocol(const e_CanMonL7Protocols oe_Protocol)
{
   C_SclString c_ProtocolName;

   std::map<stw::scl::C_SclString, C_OscComMessageLoggerFileBase * const>::iterator c_ItFile;

   this->me_Protocol = oe_Protocol;
   this->mc_ProtocolHex.SetProtocolMode(oe_Protocol);
   this->mc_ProtocolDec.SetProtocolMode(oe_Protocol);

   // Update the protocol names of all log files
   this->mc_ProtocolDec.GetProtocolName(this->me_Protocol, c_ProtocolName);
   for (c_ItFile = this->mc_LoggingFiles.begin(); c_ItFile != this->mc_LoggingFiles.end(); ++c_ItFile)
   {
      c_ItFile->second->SetProtocolName(c_ProtocolName);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Adds an openSYDE system definition for analyzing

   \param[in]  orc_PathSystemDefinition Path of system definition file (Must be .syde_sysdef)
   \param[out] orc_Buses                All CAN buses of system definition

   \return
   C_NO_ERR    data read
   C_RANGE     specified system definition file does not exist or has a wrong extension
   C_NOACT     specified file is present but structure is invalid (e.g. invalid XML file)
   C_CONFIG    system definition file content is invalid or incomplete
               device definition file could not be loaded
   C_OVERFLOW  node in system definition references a device not part of the device definitions
   C_COM       no CAN bus in system definition
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_OscComMessageLogger::AddOsySysDef(const C_SclString & orc_PathSystemDefinition,
                                            std::vector<C_OscSystemBus> & orc_Buses)
{
   return this->AddOsySysDef(orc_PathSystemDefinition, 0xFFFFFFFFUL, orc_Buses);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Adds an openSYDE system definition for analyzing

   \param[in]  orc_PathSystemDefinition Path of system definition file (Must be .syde_sysdef)
   \param[in]  ou32_BusIndex            Bus index of CAN bus of system definition for monitoring
   \param[out] orc_Buses               All CAN buses of system definition

   \return
   C_NO_ERR    data read
   C_RANGE     specified system definition file does not exist or has a wrong extension
   C_NOACT     specified file is present but structure is invalid (e.g. invalid XML file)
   C_CONFIG    system definition file content is invalid or incomplete
               device definition file could not be loaded
   C_OVERFLOW  node in system definition references a device not part of the device definitions
   C_COM       no CAN bus in system definition
   C_WARN      specified bus index was not found or is no CAN bus
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_OscComMessageLogger::AddOsySysDef(const C_SclString & orc_PathSystemDefinition, const uint32_t ou32_BusIndex,
                                            std::vector<C_OscSystemBus> & orc_Buses)
{
   const C_SclString c_FileExtension = stw::tgl::TglExtractFileExtension(orc_PathSystemDefinition).LowerCase();
   int32_t s32_Return = C_RANGE;

   if (c_FileExtension == ".syde_sysdef")
   {
      C_OscSystemDefinition c_SysDef;
      // Load without device definitions
      // Optional parameters to skip contents of h_LoadSystemDefinitionFile are not used,
      // because we are not in SYDEsup or SYDE Coder C context (#61996) and we want all system definition data here.
      s32_Return =
         C_OscSystemDefinitionFiler::h_LoadSystemDefinitionFile(c_SysDef, orc_PathSystemDefinition, "", false);
      if (s32_Return == C_NO_ERR)
      {
         uint32_t u32_BusCounter;
         uint32_t u32_FirstBusIndex = 0U;
         bool q_CanBusFound = false;
         bool q_ConcreteCanBusFound = false;

         // Return the CAN buses
         for (u32_BusCounter = 0U; u32_BusCounter < c_SysDef.c_Buses.size(); ++u32_BusCounter)
         {
            // Return all busses to make sure the index still works
            orc_Buses.push_back(c_SysDef.c_Buses[u32_BusCounter]);

            // Only CAN buses are relevant
            if (c_SysDef.c_Buses[u32_BusCounter].e_Type == C_OscSystemBus::eCAN)
            {
               if (q_CanBusFound == false)
               {
                  q_CanBusFound = true;
                  // And save the first CAN bus as initial bus if no concrete bus was set or found
                  u32_FirstBusIndex = u32_BusCounter;
               }

               if ((ou32_BusIndex != 0xFFFFFFFFUL) &&
                   (u32_BusCounter == ou32_BusIndex))
               {
                  // Concrete set bus was found
                  q_ConcreteCanBusFound = true;
               }
            }
         }

         if (q_CanBusFound == true)
         {
            uint32_t u32_UsedBusIndex;
            if (q_ConcreteCanBusFound == true)
            {
               // Set bus index was found, use it
               u32_UsedBusIndex = ou32_BusIndex;
            }
            else
            {
               if (ou32_BusIndex != 0xFFFFFFFFUL)
               {
                  // An concrete bus index was set, but not found
                  s32_Return = C_WARN;
               }

               // Use the first found CAN bus instead
               u32_UsedBusIndex = u32_FirstBusIndex;
            }

            // Adapt ECeS messages
            mh_PostProcessSysDef(c_SysDef);
            // Save the system definition
            this->m_InsertOsySysDef(orc_PathSystemDefinition, c_SysDef, u32_UsedBusIndex);
         }
         else
         {
            s32_Return = C_COM;
         }
      }
   }

   if (s32_Return != C_NO_ERR)
   {
      osc_write_log_error("Loading System Definition", "Could not load System Definition. Error code: " +
                          C_SclString::IntToStr(s32_Return));
   }

   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Updates the set bus index of the specific system definition

   \param[in] orc_PathSystemDefinition Path of system definition file (Must be .syde_sysdef)
   \param[in] ou32_BusIndex            Bus index of CAN bus of system definition for monitoring

   \return
   C_NO_ERR    Bus index for this system definition adapted
   C_NOACT     No system definition found with this path
   C_WARN      specified bus index was not found or is no CAN bus
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_OscComMessageLogger::SetOsySysDefBus(const C_SclString & orc_PathSystemDefinition,
                                               const uint32_t ou32_BusIndex)
{
   int32_t s32_Return = C_NOACT;

   const std::map<stw::scl::C_SclString,
                  C_OscComMessageLoggerOsySysDefConfig>::iterator c_ItSysDef = this->mc_OsySysDefs.find(
      orc_PathSystemDefinition);

   if (c_ItSysDef != this->mc_OsySysDefs.end())
   {
      // Check if the bus index is correct
      if ((ou32_BusIndex < c_ItSysDef->second.c_OsySysDef.c_Buses.size()) &&
          (c_ItSysDef->second.c_OsySysDef.c_Buses[ou32_BusIndex].e_Type == C_OscSystemBus::eCAN))
      {
         c_ItSysDef->second.u32_BusIndex = ou32_BusIndex;
         s32_Return = C_NO_ERR;
      }
      else
      {
         s32_Return = C_WARN;
      }
   }

   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Returns the entire loaded openSYDE system definition

   \param[in]  orc_PathSystemDefinition Path and filename of openSYDE system definition file
   \param[out] orc_SystemDefinition     Loaded system definition

   \return
   C_NO_ERR    openSYDE system definition returned
   C_RANGE     openSYDE system definition not found
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_OscComMessageLogger::GetOsySysDef(const C_SclString & orc_PathSystemDefinition,
                                            C_OscComMessageLoggerOsySysDefConfig & orc_SystemDefinition)
{
   int32_t s32_Return = C_RANGE;

   std::map<stw::scl::C_SclString, C_OscComMessageLoggerOsySysDefConfig>::iterator c_ItSysDef;

   c_ItSysDef = this->mc_OsySysDefs.find(orc_PathSystemDefinition);
   if (c_ItSysDef != this->mc_OsySysDefs.end())
   {
      // Copy the DBC definition
      orc_SystemDefinition = c_ItSysDef->second;
      s32_Return = C_NO_ERR;
   }

   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Removes a database file

   \param[in] orc_Path Path and filename of database file

   \return
   C_NO_ERR    Database removed
   C_NOACT     No database found with this path
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_OscComMessageLogger::RemoveDatabase(const C_SclString & orc_Path)
{
   int32_t s32_Return = C_NOACT;

   const std::map<stw::scl::C_SclString,
                  C_OscComMessageLoggerOsySysDefConfig>::iterator c_ItSysDef = this->mc_OsySysDefs.find(
      orc_Path);

   if (c_ItSysDef != this->mc_OsySysDefs.end())
   {
      this->mc_ProtocolDec.RemoveOsySysDef(&(c_ItSysDef->second));
      this->mc_ProtocolHex.RemoveOsySysDef(&(c_ItSysDef->second));

      // Remove the entry
      this->mc_OsySysDefs.erase(c_ItSysDef);

      s32_Return = C_NO_ERR;
   }

   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Activate or deactivate a registered database

   \param[in] orc_Path  Path and filename of database file
   \param[in] oq_Active Flag if database is active

   \return
   C_NO_ERR    Database removed
   C_NOACT     No database found with this path
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_OscComMessageLogger::ActivateDatabase(const C_SclString & orc_Path, const bool oq_Active)
{
   int32_t s32_Return = C_NOACT;

   const std::map<stw::scl::C_SclString, bool>::iterator c_ItFlag = this->mc_DatabaseActiveFlags.find(orc_Path);

   if (c_ItFlag != this->mc_DatabaseActiveFlags.end())
   {
      const std::map<stw::scl::C_SclString, C_OscComMessageLoggerOsySysDefConfig>::iterator c_ItSysDef =
         this->mc_OsySysDefs.find(orc_Path);

      c_ItFlag->second = oq_Active;
      s32_Return = C_NO_ERR;

      // Check if it is an openSYDE System Definition
      if (c_ItSysDef != this->mc_OsySysDefs.end())
      {
         // The configuration of the layer 7 interpretation must be adapted too
         if (oq_Active == true)
         {
            this->mc_ProtocolDec.AddOsySysDef(&(c_ItSysDef->second));
            this->mc_ProtocolHex.AddOsySysDef(&(c_ItSysDef->second));
         }
         else
         {
            this->mc_ProtocolDec.RemoveOsySysDef(&(c_ItSysDef->second));
            this->mc_ProtocolHex.RemoveOsySysDef(&(c_ItSysDef->second));
         }
      }
   }

   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Adds an ASC log file

   \param[in] orc_FilePath               Path with file name. File extension must be .asc
   \param[in] oq_HexActive               Mode for writing CAN Id and CAN data (hexadecimal or decimal)
   \param[in] oq_RelativeTimeStampActive Mode for writing CAN timestamp (relative or absolute)

   \return
   C_NO_ERR    File added successfully
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_OscComMessageLogger::AddLogFileAsc(const C_SclString & orc_FilePath, const bool oq_HexActive,
                                             const bool oq_RelativeTimeStampActive)
{
   int32_t s32_Return;
   C_OscComMessageLoggerFileAsc * pc_File;
   C_SclString c_ProtocolName;

   this->mc_ProtocolDec.GetProtocolName(this->me_Protocol, c_ProtocolName);
   pc_File = new C_OscComMessageLoggerFileAsc(orc_FilePath, c_ProtocolName, oq_HexActive, oq_RelativeTimeStampActive);
   s32_Return = pc_File->OpenFile();

   this->mc_LoggingFiles.insert(std::pair<C_SclString,
                                          C_OscComMessageLoggerFileBase * const>(orc_FilePath, pc_File));

   return s32_Return; //lint !e429  //no memory leak of pc_File because of handling of instance in map mc_LoggingFiles
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Remove an specific log file

   The written file is not removed. The log file is removed from the list of active log files.

   \param[in] orc_FilePath Path with file name. File extension must be .asc

   \return
   C_NO_ERR    File removed
   C_NOACT     No file with this path registered
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_OscComMessageLogger::RemoveLogFile(const C_SclString & orc_FilePath)
{
   int32_t s32_Return = C_NOACT;

   std::map<stw::scl::C_SclString, C_OscComMessageLoggerFileBase * const>::iterator c_ItFile;

   c_ItFile = this->mc_LoggingFiles.find(orc_FilePath);

   if (c_ItFile != this->mc_LoggingFiles.end())
   {
      s32_Return = C_NO_ERR;

      delete c_ItFile->second;
      this->mc_LoggingFiles.erase(c_ItFile);
   }

   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Remove all log files

   The written files are not removed. The log files are removed from the list of active log files.
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OscComMessageLogger::RemoveAllLogFiles(void)
{
   std::map<stw::scl::C_SclString, C_OscComMessageLoggerFileBase * const>::iterator c_ItFile;

   for (c_ItFile = this->mc_LoggingFiles.begin(); c_ItFile != this->mc_LoggingFiles.end(); ++c_ItFile)
   {
      delete c_ItFile->second;
   }
   this->mc_LoggingFiles.clear();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Adds a new filter configuration

   \param[in] orc_Filter Filter configuration to add
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OscComMessageLogger::AddFilter(const C_OscComMessageLoggerFilter & orc_Filter)
{
   this->mc_CanFilterConfig.push_back(orc_Filter);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Removes a filter configuration

   \param[in] orc_Filter Filter configuration to remove
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OscComMessageLogger::RemoveFilter(const C_OscComMessageLoggerFilter & orc_Filter)
{
   uint32_t u32_Counter;

   for (u32_Counter = 0U; u32_Counter < this->mc_CanFilterConfig.size(); ++u32_Counter)
   {
      if (orc_Filter == this->mc_CanFilterConfig[u32_Counter])
      {
         // Filter found, remove it
         this->mc_CanFilterConfig.erase(this->mc_CanFilterConfig.begin() + u32_Counter);
         break;
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Removes the entire filter configuration
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OscComMessageLogger::RemoveAllFilter(void)
{
   this->mc_CanFilterConfig.clear();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Returns the current count of filtered CAN messages

   Filtered messages refers to the number of messages that did not pass active filter(s).
   This function is thread safe

   \return
   Current count of filtered CAN messages
*/
//----------------------------------------------------------------------------------------------------------------------
uint32_t C_OscComMessageLogger::GetFilteredMessages(void) const
{
   return this->mu32_FilteredMessages;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  If necessary handle and log the CAN message

   Check if CAN message must be logged.
   Check CAN message for configured protocols.
   Fills an instance of C_OscComMessageLoggerData which can get by calling GetHandledCanMessage.
   This base class implementation must be called first.

   \param[in] orc_Msg Current CAN message
   \param[in] oq_IsTx Message was sent of this application itself

   \return
   C_NO_ERR    CAN message logged
   C_NOACT     CAN message not relevant
   C_BUSY      Logger is paused
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_OscComMessageLogger::HandleCanMessage(const T_STWCAN_Msg_RX & orc_Msg, const bool oq_IsTx)
{
   int32_t s32_Return = C_BUSY;

   if (this->mq_Paused == false)
   {
      if (this->m_CheckFilter(orc_Msg) == true)
      {
         std::map<stw::scl::C_SclString, C_OscComMessageLoggerFileBase * const>::const_iterator c_ItFileLogger;
         bool q_OpenSydeInterpretationFound = false;

         // Parse message and fill C_OscComMessageLoggerData for all further steps.
         this->m_ConvertCanMessage(orc_Msg, oq_IsTx);

         // Check the registered openSYDE system definitions.
         // The result is in mpc_OsySysDefMessage and mpc_OsySysDefDataPoolList
         if (this->m_CheckSysDef(orc_Msg) == true)
         {
            q_OpenSydeInterpretationFound = this->m_InterpretSysDef(this->mc_HandledCanMessage);
         }

         if (q_OpenSydeInterpretationFound == true)
         {
            // Check for ECoS message
            if (this->m_CheckIfEcosMessage(orc_Msg) == true)
            {
               this->m_SaveEcosMessage();
            }
            // Check and handle for ECeS message
            else
            {
               this->m_CheckAndHandleEcesMessage();
            }
         }
         else
         {
            // No message in openSYDE system definitions found, check and let interpret other
            if (this->m_CheckInterpretation(this->mc_HandledCanMessage) == false)
            {
               // No interpretation found, check for classical protocol interpretation
               this->mc_HandledCanMessage.c_ProtocolTextHex = this->m_GetProtocolStringHex(orc_Msg).c_str();
               if (this->mc_HandledCanMessage.c_ProtocolTextHex != "")
               {
                  // Only necessary if a protocol was found in the hex variant
                  this->mc_HandledCanMessage.c_ProtocolTextDec = this->m_GetProtocolStringDec(orc_Msg).c_str();
               }

               // Message not found in any other place. Check and process for ECoS inverted message
               else
               {
                  this->m_HandleEcosInvertedMessage();
               }
            }
         }

         // File logging
         for (c_ItFileLogger = this->mc_LoggingFiles.begin(); c_ItFileLogger != this->mc_LoggingFiles.end();
              ++c_ItFileLogger)
         {
            c_ItFileLogger->second->AddMessageToFile(this->mc_HandledCanMessage);
         }

         s32_Return = C_NO_ERR;
      }
      else
      {
         // Message is not relevant for this logger because of filter configuration
         s32_Return = C_NOACT;
      }
   }

   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Resets all CAN message counter
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OscComMessageLogger::ResetCounter(void)
{
   this->m_ResetCounter();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Optional function for getting the current bus load

   This implementation does nothing with the value. This function must be overridden to use the bus load value

   \param[in] ou8_BusLoad Current CAN bus load in percentage
*/
//----------------------------------------------------------------------------------------------------------------------
//lint -e{9175}  //intentionally no functionality in default implementation
void C_OscComMessageLogger::UpdateBusLoad(const uint8_t ou8_BusLoad)
{
   (void)ou8_BusLoad;
   // Nothing to do here
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Optional function for getting the current detected CAN Tx errors

   This implementation does nothing with the value. This function must be overridden to use the detected CAN Tx errors

   \param[in] ou32_TxErrors Current detected CAN Tx errors
*/
//----------------------------------------------------------------------------------------------------------------------
//lint -e{9175}  //intentionally no functionality in default implementation
void C_OscComMessageLogger::UpdateTxErrors(const uint32_t ou32_TxErrors)
{
   (void)ou32_TxErrors;
   // Nothing to do here
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Optional function for getting the current counted CAN Tx messages

   This implementation does nothing with the value. This function must be overridden to use the counted CAN Tx messages.

   \param[in] ou32_TxCount Current counted CAN Tx messages
*/
//----------------------------------------------------------------------------------------------------------------------
//lint -e{9175}  //intentionally no functionality in default implementation
void C_OscComMessageLogger::UpdateTxCounter(const uint32_t ou32_TxCount)
{
   (void)ou32_TxCount;
   // Nothing to do here
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Returns the current handled CAN message after call of HandleCanMessage

   \return
   Current interpreted CAN message
*/
//----------------------------------------------------------------------------------------------------------------------
const C_OscComMessageLoggerData & C_OscComMessageLogger::m_GetHandledCanMessage(void) const
{
   return this->mc_HandledCanMessage;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Checks a specific CAN message for matching the filter configuration

   \param[in] orc_Msg Current CAN message

   \return
   true     CAN message is relevant
   false    CAN message is not relevant
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_OscComMessageLogger::m_CheckFilter(const T_STWCAN_Msg_RX & orc_Msg)
{
   bool q_Return = false;

   if (this->mc_CanFilterConfig.size() > 0)
   {
      uint32_t u32_FilterCounter;
      bool q_Blocked = false;
      bool q_PassFilterAvailable = false;

      // Check all block filters first
      for (u32_FilterCounter = 0U; u32_FilterCounter < this->mc_CanFilterConfig.size(); ++u32_FilterCounter)
      {
         if (this->mc_CanFilterConfig[u32_FilterCounter].q_PassFilter == false)
         {
            if ((orc_Msg.u8_XTD == this->mc_CanFilterConfig[u32_FilterCounter].u8_ExtendedId) &&
                (orc_Msg.u32_ID >= this->mc_CanFilterConfig[u32_FilterCounter].u32_StartId) &&
                (orc_Msg.u32_ID <= this->mc_CanFilterConfig[u32_FilterCounter].u32_EndId))
            {
               // Message is not relevant. No further checks necessary.
               q_Blocked = true;
               break;
            }
         }
         else
         {
            // At least one pass filter is configured
            // The pass filters will be checked separately if not blocked
            q_PassFilterAvailable = true;
         }
      }

      if (q_Blocked == false)
      {
         if (q_PassFilterAvailable == true)
         {
            // Check only if the CAN message was not already blocked explicitly and at least one pass filter
            // was configured
            for (u32_FilterCounter = 0U; u32_FilterCounter < this->mc_CanFilterConfig.size(); ++u32_FilterCounter)
            {
               if ((orc_Msg.u8_XTD == this->mc_CanFilterConfig[u32_FilterCounter].u8_ExtendedId) &&
                   (orc_Msg.u32_ID >= this->mc_CanFilterConfig[u32_FilterCounter].u32_StartId) &&
                   (orc_Msg.u32_ID <= this->mc_CanFilterConfig[u32_FilterCounter].u32_EndId))
               {
                  // This pass filter matched. CAN message is not blocked and relevant.
                  q_Return = true;
                  break;
               }
            }
         }
         else
         {
            // The message is not blocked and no pass filter configured.
            // That means that all messages, which are not blocked are relevant.
            q_Return = true;
         }
      }
   }
   else
   {
      // No filter configured, all messages are welcome
      q_Return = true;
   }

   if ((q_Return == false) &&
       (this->mu32_FilteredMessages < 0xFFFFFFFFU))
   {
      // Count the filtered message
      ++this->mu32_FilteredMessages;
   }

   return q_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Returns the CAN message as protocol specific interpreted string with values as hex

   \param[in] orc_Msg Current CAN message

   \return
   CAN message converted to protocol string
   Empty string if no protocol is configured or the CAN message does not match to the configured protocol
*/
//----------------------------------------------------------------------------------------------------------------------
C_SclString C_OscComMessageLogger::m_GetProtocolStringHex(const T_STWCAN_Msg_RX & orc_Msg) const
{
   C_SclString c_Result = "";

   if (this->me_Protocol != stw::cmon_protocol::eCMON_L7_PROTOCOL_NONE)
   {
      c_Result = this->mc_ProtocolHex.MessageToStringProtocolOnly(orc_Msg);
   }

   return c_Result;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Returns the CAN message as protocol specific interpreted string with values as decimal

   \param[in] orc_Msg Current CAN message

   \return
   CAN message converted to protocol string
   Empty string if no protocol is configured or the CAN message does not match to the configured protocol
*/
//----------------------------------------------------------------------------------------------------------------------
C_SclString C_OscComMessageLogger::m_GetProtocolStringDec(const T_STWCAN_Msg_RX & orc_Msg) const
{
   C_SclString c_Result = "";

   if (this->me_Protocol != stw::cmon_protocol::eCMON_L7_PROTOCOL_NONE)
   {
      c_Result = this->mc_ProtocolDec.MessageToStringProtocolOnly(orc_Msg);
   }

   return c_Result;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Inserts openSYDE system definition to parsing configuration

   Splitting of parts of function AddOsySysDef into this one because of performance.
   Loading of file needs a lot more time and shall not be part of the synchronized parts.
   Deriving classes can override this function to use synchronization.

   \param[in] orc_PathSystemDefinition Path of system definition file (Must be .syde_sysdef)
   \param[in] orc_OsySysDef            Loaded openSYDE system definition
   \param[in] ou32_BusIndex            Used CAN bus index
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OscComMessageLogger::m_InsertOsySysDef(const C_SclString & orc_PathSystemDefinition,
                                              const C_OscSystemDefinition & orc_OsySysDef, const uint32_t ou32_BusIndex)
{
   const C_OscComMessageLoggerOsySysDefConfig c_SysDefConfig(orc_OsySysDef, ou32_BusIndex);

   std::map<stw::scl::C_SclString, C_OscComMessageLoggerOsySysDefConfig>::iterator c_ItNewConfig;

   c_ItNewConfig = this->mc_OsySysDefs.insert(std::pair<C_SclString,
                                                        C_OscComMessageLoggerOsySysDefConfig>(orc_PathSystemDefinition,
                                                                                              c_SysDefConfig)).first;
   // Register the database in the activation flag map
   this->mc_DatabaseActiveFlags.insert(std::pair<C_SclString, bool>(orc_PathSystemDefinition, true));

   this->mc_ProtocolDec.AddOsySysDef(&(c_ItNewConfig->second));
   this->mc_ProtocolHex.AddOsySysDef(&(c_ItNewConfig->second));
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Checks if a matching CAN message is defined in at least one registered openSYDE system definition

   The result will be saved to mpc_OsySysDefMessage and mpc_OsySysDefDataPoolList.
   If mpc_OsySysDefMessage and mpc_OsySysDefDataPoolList is NULL, no CAN message found in any system definition.

   \param[in] orc_Msg CAN message for analyzing

   \return
   true     Matching CAN message found
   false    No matching CAN message found
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_OscComMessageLogger::m_CheckSysDef(const T_STWCAN_Msg_RX & orc_Msg)
{
   bool q_Return = false;

   std::map<stw::scl::C_SclString, C_OscComMessageLoggerOsySysDefConfig>::const_iterator c_ItSysDef;

   this->mpc_OsySysDefMessage = NULL;
   this->mpc_OsySysDefDataPoolList = NULL;

   for (c_ItSysDef = this->mc_OsySysDefs.begin(); c_ItSysDef != this->mc_OsySysDefs.end(); ++c_ItSysDef)
   {
      // Check if the database is active
      if (this->mc_DatabaseActiveFlags[c_ItSysDef->first] == true)
      {
         const C_OscSystemDefinition & rc_OsySysDef = c_ItSysDef->second.c_OsySysDef;
         uint32_t u32_NodeCounter;

         // Search all nodes which are connected to to the CAN bus
         for (u32_NodeCounter = 0U; u32_NodeCounter < rc_OsySysDef.c_Nodes.size(); ++u32_NodeCounter)
         {
            const C_OscNode & rc_Node = rc_OsySysDef.c_Nodes[u32_NodeCounter];
            uint32_t u32_IntfCounter;
            bool q_IntfFound = false;

            // Search an interface which is connected to the bus
            for (u32_IntfCounter = 0U; u32_IntfCounter < rc_Node.c_Properties.c_ComInterfaces.size(); ++u32_IntfCounter)
            {
               if ((rc_Node.c_Properties.c_ComInterfaces[u32_IntfCounter].GetBusConnected() == true) &&
                   (rc_Node.c_Properties.c_ComInterfaces[u32_IntfCounter].u32_BusIndex ==
                    c_ItSysDef->second.u32_BusIndex))
               {
                  // Com Interface found
                  q_IntfFound = true;
                  break;
               }
            }

            if (q_IntfFound == true)
            {
               // Check all messages of the node for this interface on this bus
               uint32_t u32_ProtCounter;

               for (u32_ProtCounter = 0U; u32_ProtCounter < rc_Node.c_ComProtocols.size(); ++u32_ProtCounter)
               {
                  const C_OscCanProtocol & rc_CanProt = rc_Node.c_ComProtocols[u32_ProtCounter];

                  tgl_assert(u32_IntfCounter < rc_CanProt.c_ComMessages.size());
                  if (u32_IntfCounter < rc_CanProt.c_ComMessages.size())
                  {
                     const std::vector<C_OscCanMessage> & rc_CanMsgContainerTx =
                        rc_CanProt.c_ComMessages[u32_IntfCounter].c_TxMessages;
                     bool q_SearchAlsoInRxMessages = true; // flag to improve performance
                     uint32_t u32_CanMsgCounter;

                     for (u32_CanMsgCounter = 0U; u32_CanMsgCounter < rc_CanMsgContainerTx.size(); ++u32_CanMsgCounter)
                     {
                        const C_OscCanMessage & rc_OscMsg = rc_CanMsgContainerTx[u32_CanMsgCounter];

                        // No check of dlc here, it will be checked for each signal
                        if ((orc_Msg.u32_ID == rc_OscMsg.u32_CanId) &&
                            ((orc_Msg.u8_XTD == 1U) == rc_OscMsg.q_IsExtended))
                        {
                           // Message matches
                           this->mpc_OsySysDefMessage = &rc_OscMsg;
                           q_SearchAlsoInRxMessages = false; // found message, no need to search in Rx messages anymore

                           // Get the associated list
                           tgl_assert(rc_CanProt.u32_DataPoolIndex < rc_Node.c_DataPools.size());
                           if (rc_CanProt.u32_DataPoolIndex < rc_Node.c_DataPools.size())
                           {
                              this->mpc_OsySysDefDataPoolList =
                                 C_OscCanProtocol::h_GetComListConst(rc_Node.c_DataPools[rc_CanProt.u32_DataPoolIndex],
                                                                     u32_IntfCounter, true);
                              tgl_assert(this->mpc_OsySysDefDataPoolList != NULL);

                              q_Return = true;
                           }

                           break;
                        }
                     }

                     // Only in case of CANopen protocol we want to visualize TXPDO messages (sent from server to
                     // client, RX messages from client view) and their signals in trace because this safes us a lot
                     // of performance.
                     // Trigger of this feature is: https://redmine.sensor-technik.de/issues/78633
                     if ((rc_CanProt.e_Type == C_OscCanProtocol::eCAN_OPEN) && (q_SearchAlsoInRxMessages == true))
                     {
                        const std::vector<C_OscCanMessage> & rc_CanMsgContainerRx =
                           rc_CanProt.c_ComMessages[u32_IntfCounter].c_RxMessages;
                        uint32_t u32_CanMsgCounterRx;

                        for (u32_CanMsgCounterRx = 0U; u32_CanMsgCounterRx < rc_CanMsgContainerRx.size();
                             ++u32_CanMsgCounterRx)
                        {
                           const C_OscCanMessage & rc_OscMsg = rc_CanMsgContainerRx[u32_CanMsgCounterRx];

                           // No check of dlc here, it will be checked for each signal
                           if ((orc_Msg.u32_ID == rc_OscMsg.u32_CanId) &&
                               ((orc_Msg.u8_XTD == 1U) == rc_OscMsg.q_IsExtended))
                           {
                              // Message matches
                              this->mpc_OsySysDefMessage = &rc_OscMsg;

                              // Get the associated list
                              tgl_assert(rc_CanProt.u32_DataPoolIndex < rc_Node.c_DataPools.size());
                              if (rc_CanProt.u32_DataPoolIndex < rc_Node.c_DataPools.size())
                              {
                                 this->mpc_OsySysDefDataPoolList =
                                    C_OscCanProtocol::h_GetComListConst(
                                       rc_Node.c_DataPools[rc_CanProt.u32_DataPoolIndex],
                                       u32_IntfCounter, false);
                                 tgl_assert(this->mpc_OsySysDefDataPoolList != NULL);

                                 q_Return = true;
                              }

                              break;
                           }
                        }
                     }
                  }

                  if (this->mpc_OsySysDefMessage != NULL)
                  {
                     break;
                  }
               }
            }

            if (this->mpc_OsySysDefMessage != NULL)
            {
               break;
            }
         }

         if (this->mpc_OsySysDefMessage != NULL)
         {
            break;
         }
      }
   }

   return q_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Interprets the system definition message

   \param[in,out] orc_MessageData Message data target for the interpretation

   \return
   true     Matching CAN message exists
   false    No matching CAN message exists
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_OscComMessageLogger::m_InterpretSysDef(C_OscComMessageLoggerData & orc_MessageData) const
{
   bool q_Return = false;

   if ((this->mpc_OsySysDefMessage != NULL) &&
       (this->mpc_OsySysDefDataPoolList != NULL))
   {
      uint32_t u32_Counter;
      bool q_MultiplexerFound = false;
      uint32_t u32_MultiplexerIndex = 0U;
      uint16_t u16_MultiplexValue = 0U;

      orc_MessageData.c_Name = this->mpc_OsySysDefMessage->c_Name.c_str();

      // Check if a multiplexer signal exists
      for (u32_Counter = 0U; u32_Counter < this->mpc_OsySysDefMessage->c_Signals.size(); ++u32_Counter)
      {
         const C_OscCanSignal & rc_OscSignal = this->mpc_OsySysDefMessage->c_Signals[u32_Counter];

         if (rc_OscSignal.e_MultiplexerType == C_OscCanSignal::eMUX_MULTIPLEXER_SIGNAL)
         {
            this->m_InterpretSysDefCanSignal(orc_MessageData, rc_OscSignal);

            if (orc_MessageData.c_Signals.size() > 0)
            {
               C_OscComMessageLoggerDataSignal & rc_Signal =
                  orc_MessageData.c_Signals[orc_MessageData.c_Signals.size() - 1];

               if (rc_Signal.q_DlcError == false)
               {
                  try
                  {
                     u16_MultiplexValue = static_cast<uint16_t>(rc_Signal.c_RawValueDec.ToInt());
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
      for (u32_Counter = 0U; u32_Counter < this->mpc_OsySysDefMessage->c_Signals.size(); ++u32_Counter)
      {
         // Skip the multiplexer signal if one was found. It was added already.
         if ((q_MultiplexerFound == false) ||
             (u32_Counter != u32_MultiplexerIndex))
         {
            const C_OscCanSignal & rc_OscSignal = this->mpc_OsySysDefMessage->c_Signals[u32_Counter];

            // Interpret all not multiplexed signals and all multiplexed signals with the matching multiplexer value
            if ((rc_OscSignal.e_MultiplexerType == C_OscCanSignal::eMUX_DEFAULT) ||
                ((q_MultiplexerFound == true) &&
                 (u16_MultiplexValue == rc_OscSignal.u16_MultiplexValue)))
            {
               this->m_InterpretSysDefCanSignal(orc_MessageData, rc_OscSignal);
            }
         }
      }

      orc_MessageData.SortSignals();

      if (orc_MessageData.c_CanMsg.u8_DLC != static_cast<uint8_t>(this->mpc_OsySysDefMessage->u16_Dlc))
      {
         // CAN message does not have the expected size
         orc_MessageData.q_CanDlcError = true;
      }

      q_Return = true;
   }

   return q_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Returns the interpreted string of the found CAN message interpretation of an openSYDE system definition

   The result in mpc_OsySysDefMessage and mpc_OsySysDefDataPoolList will be used.
   All values are shown as hexadecimal value.

   \return
   Interpreted string if mpc_OsySysDefMessage and mpc_OsySysDefDataPoolList is not NULL
   Empty string if mpc_OsySysDefMessage or mpc_OsySysDefDataPoolList is NULL
*/
//----------------------------------------------------------------------------------------------------------------------
C_SclString C_OscComMessageLogger::m_GetOsySysDefStringHex(void) const
{
   C_SclString c_Return = "";

   if ((this->mpc_OsySysDefDataPoolList != NULL) &&
       (this->mpc_OsySysDefMessage != NULL))
   {
      // TODO for logging
      c_Return = this->mpc_OsySysDefMessage->c_Name;
   }

   return c_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Returns the interpreted string of the found CAN message interpretation of an openSYDE system definition

   The result in mpc_OsySysDefMessage and mpc_OsySysDefDataPoolList will be used.
   All values are shown as decimal value.

   \return
   Interpreted string if mpc_OsySysDefMessage and mpc_OsySysDefDataPoolList is not NULL
   Empty string if mpc_OsySysDefMessage or mpc_OsySysDefDataPoolList is NULL
*/
//----------------------------------------------------------------------------------------------------------------------
C_SclString C_OscComMessageLogger::m_GetOsySysDefStringDec(void) const
{
   C_SclString c_Return = "";

   if ((this->mpc_OsySysDefDataPoolList != NULL) &&
       (this->mpc_OsySysDefMessage != NULL))
   {
      // TODO for logging
      c_Return = this->mpc_OsySysDefMessage->c_Name;
   }

   return c_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Checks and interprets if a matching CAN message is defined in a description / specification

   Using the both members c_ProtocolTextDec and c_ProtocolTextHex in orc_MessageData for the result.

   \param[in,out] orc_MessageData CAN message data class for analyzing and for output

   \return
   true     Matching CAN message found
   false    No matching CAN message found
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_OscComMessageLogger::m_CheckInterpretation(stw::opensyde_core::C_OscComMessageLoggerData & orc_MessageData)
{
   // Nothing to do here.
   // Deriving class must override this function to use an own CAN message description or specification.
   static_cast<void>(orc_MessageData);

   return false;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Interprets the received CAN signal value

   \param[in,out]     orc_Signal        Signal interpretation
   \param[in]         orau8_CanDb       RAW CAN message data
   \param[in]         ou8_CanDlc        DLC of CAN message
   \param[in]         orc_OscSignal     openSYDE CAN signal configuration
   \param[in]         orc_OscValue      Datapool element content for type configuration
   \param[in]         of64_Factor       Factor for resulting value
   \param[in]         of64_Offset       Offset for resulting value
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OscComMessageLogger::mh_InterpretCanSignalValue(C_OscComMessageLoggerDataSignal & orc_Signal,
                                                       const uint8_t(&orau8_CanDb)[8], const uint8_t ou8_CanDlc,
                                                       const C_OscCanSignal & orc_OscSignal,
                                                       const C_OscNodeDataPoolContent & orc_OscValue,
                                                       const float64_t of64_Factor, const float64_t of64_Offset)
{
   const bool q_SignalFits = C_OscCanUtil::h_IsSignalInMessage(ou8_CanDlc, orc_OscSignal);

   if (q_SignalFits == true)
   {
      uint8_t u8_RawDataCounter;
      std::vector<uint8_t> c_SignalRawData;
      std::string c_StringValue;
      C_OscNodeDataPoolContent c_OscValue = orc_OscValue;
      uint64_t u64_Value = 0U;

      // Get the signal as raw value in the first step
      c_SignalRawData.resize(c_OscValue.GetSizeByte());
      C_OscCanUtil::h_GetSignalValue(orau8_CanDb, orc_OscSignal, c_SignalRawData, orc_OscValue.GetType());
      // And the interpreted value
      c_OscValue.SetValueFromLittleEndianBlob(c_SignalRawData);

      // Raw values
      // Signal data is converted to little endian by h_GetSignalValue
      for (u8_RawDataCounter = 0U; u8_RawDataCounter < c_SignalRawData.size(); ++u8_RawDataCounter)
      {
         const uint64_t u64_ShiftValue = static_cast<uint64_t>(u8_RawDataCounter) * 8U;
         u64_Value += static_cast<uint64_t>(c_SignalRawData[u8_RawDataCounter]) << u64_ShiftValue;
      }

      if ((orc_OscValue.GetType() == C_OscNodeDataPoolContent::eFLOAT32) ||
          (orc_OscValue.GetType() == C_OscNodeDataPoolContent::eFLOAT64))
      {
         // In case of a float value, the raw value does not make any sense in byte form.
         // Use the value without scaling as raw value.
         c_OscValue.GetValueAsScaledString(1.0, 0.0, c_StringValue, 0U, true, true);
         orc_Signal.c_RawValueDec = c_StringValue.c_str();
         orc_Signal.c_RawValueHex = c_StringValue.c_str();
      }
      else
      {
         orc_Signal.c_RawValueDec = C_SclString::IntToStr(u64_Value);
         orc_Signal.c_RawValueHex = C_SclString::IntToHex(static_cast<int64_t>(u64_Value), 1).UpperCase();
      }

      // Interpreted value
      c_OscValue.GetValueAsScaledString(of64_Factor, of64_Offset, c_StringValue, 0U, true, true);
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

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Returns an interpreted string of a protocol

   This is the default implementation and returns an empty string

   \return
   Interpreted string if match protocol found
   Empty string if no match found
*/
//----------------------------------------------------------------------------------------------------------------------
C_SclString C_OscComMessageLogger::m_GetProtocolStringHexHook(void) const
{
   return "";
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Returns an interpreted string of a protocol

   This is the default implementation and returns an empty string

   \return
   Interpreted string if match protocol found
   Empty string if no match found
*/
//----------------------------------------------------------------------------------------------------------------------
C_SclString C_OscComMessageLogger::m_GetProtocolStringDecHook(void) const
{
   return "";
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Fills instance mc_HandledCanMessage of C_OscComMessageLoggerData dependent of the current message

   \param[in] orc_Msg Current CAN message
   \param[in] oq_IsTx Message was sent of this application itself
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OscComMessageLogger::m_ConvertCanMessage(const T_STWCAN_Msg_RX & orc_Msg, const bool oq_IsTx)
{
   uint8_t u8_DbCounter;

   // Clear not handled data in this function
   this->mc_HandledCanMessage.c_Name = "";
   this->mc_HandledCanMessage.c_ProtocolTextHex = "";
   this->mc_HandledCanMessage.c_ProtocolTextDec = "";
   this->mc_HandledCanMessage.c_Signals.clear();
   this->mc_HandledCanMessage.q_CanDlcError = false;
   this->mc_HandledCanMessage.c_Status = "";

   // Save the L2 message. Needed for the interpretation partly
   this->mc_HandledCanMessage.c_CanMsg = orc_Msg;

   // Prepare the data for the ui
   this->mc_HandledCanMessage.c_CanIdDec = C_SclString::IntToStr(orc_Msg.u32_ID);
   this->mc_HandledCanMessage.c_CanIdHex = C_SclString::IntToHex(orc_Msg.u32_ID, 1).UpperCase();
   if (orc_Msg.u8_XTD > 0U)
   {
      // Extended Id
      this->mc_HandledCanMessage.c_CanIdDec += "x";
      this->mc_HandledCanMessage.c_CanIdHex += "x";
   }
   this->mc_HandledCanMessage.c_CanDlc = C_SclString::IntToStr(orc_Msg.u8_DLC);
   this->mc_HandledCanMessage.q_IsTx = oq_IsTx;

   this->mc_HandledCanMessage.c_CanDataDec = "";
   this->mc_HandledCanMessage.c_CanDataHex = "";
   for (u8_DbCounter = 0U; u8_DbCounter < orc_Msg.u8_DLC; ++u8_DbCounter)
   {
      if (orc_Msg.au8_Data[u8_DbCounter] < 100)
      {
         this->mc_HandledCanMessage.c_CanDataDec += " ";
         if (orc_Msg.au8_Data[u8_DbCounter] < 10)
         {
            this->mc_HandledCanMessage.c_CanDataDec += " ";
         }
      }
      this->mc_HandledCanMessage.c_CanDataDec += C_SclString::IntToStr(orc_Msg.au8_Data[u8_DbCounter]);
      this->mc_HandledCanMessage.c_CanDataHex += C_SclString::IntToHex(orc_Msg.au8_Data[u8_DbCounter], 2).UpperCase();

      if (u8_DbCounter < (orc_Msg.u8_DLC - 1U))
      {
         this->mc_HandledCanMessage.c_CanDataDec += " ";
         this->mc_HandledCanMessage.c_CanDataHex += " ";
      }
   }

   // It is possible that communication between a tool on the same CAN bus on the virtual side and a real
   // hardware communication partner cause a problem in the CAN message order.
   // The internal send CAN messages can be seen by other tools on the same internal network earlier than
   // the other CAN bus nodes. The timestamp is set by the CAN hardware part (in case of PEAK).
   // The problem: The timestamp can be later than the timestamp of some CAN messages received from CAN bus nodes,
   // but are in the receive queue before the CAN bus nodes.
   // So we must prevent possible negative values.

   // Absolute
   if (orc_Msg.u64_TimeStamp > this->mu64_FirstTimeStampStart)
   {
      this->mc_HandledCanMessage.u64_TimeStampAbsoluteStart = orc_Msg.u64_TimeStamp - this->mu64_FirstTimeStampStart;
      // Calculating the us by starting at midnight to get the timestamp based on time of day
      // Days will not be counted and shown and will be dropped
      this->mc_HandledCanMessage.u64_TimeStampAbsoluteTimeOfDay =
         (mu64_FirstTimeStampDayOfTime + this->mc_HandledCanMessage.u64_TimeStampAbsoluteStart) %
         mhu64_MAX_TIMESTAMP_DAY_OF_TIME;
   }
   else
   {
      this->mc_HandledCanMessage.u64_TimeStampAbsoluteStart = 0ULL;
   }

   // Relative
   if (orc_Msg.u64_TimeStamp > this->mu64_LastTimeStamp)
   {
      this->mc_HandledCanMessage.u64_TimeStampRelative = orc_Msg.u64_TimeStamp - this->mu64_LastTimeStamp;
   }
   else
   {
      this->mc_HandledCanMessage.u64_TimeStampRelative = 0ULL;
   }

   this->mc_HandledCanMessage.c_TimeStampAbsoluteStart =
      C_OscComMessageLoggerData::h_GetTimestampAsString(this->mc_HandledCanMessage.u64_TimeStampAbsoluteStart);
   this->mc_HandledCanMessage.c_TimeStampAbsoluteTimeOfDay =
      C_OscComMessageLoggerData::h_GetTimestampAsString(this->mc_HandledCanMessage.u64_TimeStampAbsoluteTimeOfDay);
   this->mc_HandledCanMessage.c_TimeStampRelative =
      C_OscComMessageLoggerData::h_GetTimestampAsString(this->mc_HandledCanMessage.u64_TimeStampRelative);
   // Save the timestamp for the next message to calculate the relative timestamp
   this->mu64_LastTimeStamp = orc_Msg.u64_TimeStamp;

   // Update the counter
   if (orc_Msg.u8_XTD == 0U)
   {
      if (orc_Msg.u32_ID <= 0x7FF)
      {
         // Standard id
         this->mc_MsgCounterStandardId[orc_Msg.u32_ID] = this->mc_MsgCounterStandardId[orc_Msg.u32_ID] + 1U;
         this->mc_HandledCanMessage.c_Counter = C_SclString::IntToStr(this->mc_MsgCounterStandardId[orc_Msg.u32_ID]);
      }
      else
      {
         // Should not happen
         this->mc_HandledCanMessage.c_Counter = "0";
      }
   }
   else
   {
      const std::map<uint32_t, uint32_t>::iterator c_ItCounter =
         this->mc_MsgCounterExtendedId.find(orc_Msg.u32_ID);

      if (c_ItCounter == this->mc_MsgCounterExtendedId.end())
      {
         // New message
         this->mc_MsgCounterExtendedId.insert(std::pair<uint32_t, uint32_t>(orc_Msg.u32_ID, 1U));
         this->mc_HandledCanMessage.c_Counter = "1";
      }
      else
      {
         c_ItCounter->second = c_ItCounter->second + 1;
         this->mc_HandledCanMessage.c_Counter = C_SclString::IntToStr(c_ItCounter->second);
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Interprets the system definition message signal

   \param[in,out] orc_MessageData Message data target for the interpretation
   \param[in]     orc_OscSignal   CAN signal of system definition
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OscComMessageLogger::m_InterpretSysDefCanSignal(C_OscComMessageLoggerData & orc_MessageData,
                                                       const C_OscCanSignal & orc_OscSignal) const
{
   if ((this->mpc_OsySysDefDataPoolList != NULL) &&
       (orc_OscSignal.u32_ComDataElementIndex < this->mpc_OsySysDefDataPoolList->c_Elements.size()))
   {
      C_OscComMessageLoggerDataSignal c_Signal;
      const C_OscNodeDataPoolListElement & rc_OscElement =
         this->mpc_OsySysDefDataPoolList->c_Elements[orc_OscSignal.u32_ComDataElementIndex];

      c_Signal.c_Name = rc_OscElement.c_Name.c_str();
      c_Signal.c_Unit = rc_OscElement.c_Unit.c_str();
      c_Signal.c_Comment = rc_OscElement.c_Comment.c_str();

      // Get the current value for the correct type configuration
      mh_InterpretCanSignalValue(c_Signal, orc_MessageData.c_CanMsg.au8_Data, orc_MessageData.c_CanMsg.u8_DLC,
                                 orc_OscSignal, rc_OscElement.c_Value, rc_OscElement.f64_Factor,
                                 rc_OscElement.f64_Offset);

      orc_MessageData.c_Signals.push_back(c_Signal);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Resets all CAN message counter
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OscComMessageLogger::m_ResetCounter(void)
{
   //lint -e522 //false positive; call to std::fill has side effects
   std::fill(this->mc_MsgCounterStandardId.begin(), this->mc_MsgCounterStandardId.end(), 0U);
   this->mc_MsgCounterExtendedId.clear();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Post process a loaded openSYDE system definition.

   More precisely, manipulate all messages of protocol ECeS by adding two signals for message counter and checksum.

   \param[in,out]  orc_SystemDefinition   System definition
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OscComMessageLogger::mh_PostProcessSysDef(C_OscSystemDefinition & orc_SystemDefinition)
{
   // indices
   uint32_t u32_ItNodes;
   uint32_t u32_ItInterfaces;
   uint32_t u32_ItProtocols;
   uint32_t u32_ItMessages;
   C_OscCanMessageIdentificationIndices c_Id;

   // for all nodes
   for (u32_ItNodes = 0; u32_ItNodes < orc_SystemDefinition.c_Nodes.size(); u32_ItNodes++)
   {
      C_OscNode & rc_Node = orc_SystemDefinition.c_Nodes[u32_ItNodes];
      c_Id.u32_NodeIndex = u32_ItNodes;

      // for all interfaces of this node
      for (u32_ItInterfaces = 0; u32_ItInterfaces < rc_Node.c_Properties.c_ComInterfaces.size(); u32_ItInterfaces++)
      {
         c_Id.u32_InterfaceIndex = u32_ItInterfaces;

         // for all protocols of this node
         for (u32_ItProtocols = 0; u32_ItProtocols < rc_Node.c_ComProtocols.size(); u32_ItProtocols++)
         {
            const C_OscCanProtocol & rc_Protocol = rc_Node.c_ComProtocols[u32_ItProtocols];

            if (rc_Protocol.e_Type == C_OscCanProtocol::eECES)
            {
               c_Id.u32_DatapoolIndex = rc_Protocol.u32_DataPoolIndex;
               c_Id.e_ComProtocol = rc_Protocol.e_Type;

               // one container for each interface
               if (u32_ItInterfaces < rc_Protocol.c_ComMessages.size())
               {
                  const C_OscCanMessageContainer & rc_Container = rc_Protocol.c_ComMessages[u32_ItInterfaces];

                  // Tx messages
                  for (u32_ItMessages = 0; u32_ItMessages < rc_Container.c_TxMessages.size(); u32_ItMessages++)
                  {
                     const C_OscCanMessage & rc_Message = rc_Container.c_TxMessages[u32_ItMessages];
                     c_Id.q_MessageIsTx = true;
                     c_Id.u32_MessageIndex = u32_ItMessages;
                     mh_AddSpecialEcesSignals(rc_Node, c_Id, static_cast<uint32_t>(rc_Message.c_Signals.size()));
                  }
                  // Rx messages
                  for (u32_ItMessages = 0; u32_ItMessages < rc_Container.c_RxMessages.size(); u32_ItMessages++)
                  {
                     const C_OscCanMessage & rc_Message = rc_Container.c_RxMessages[u32_ItMessages];
                     c_Id.q_MessageIsTx = false;
                     c_Id.u32_MessageIndex = u32_ItMessages;
                     mh_AddSpecialEcesSignals(rc_Node, c_Id, static_cast<uint32_t>(rc_Message.c_Signals.size()));
                  }
               }
            }
         }
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Add two signals incl. their Datapool elements for message counter and checksum to a message.

   \param[in,out]  orc_Node            Node
   \param[in]      orc_Id              Message identification indices
   \param[in]      ou32_SignalIndex    Signal index
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OscComMessageLogger::mh_AddSpecialEcesSignals(C_OscNode & orc_Node,
                                                     const C_OscCanMessageIdentificationIndices & orc_Id,
                                                     const uint32_t ou32_SignalIndex)
{
   // create signals incl. datapool element
   C_OscCanSignal c_MessageCounterSig;
   C_OscCanSignal c_ChecksumSig;
   C_OscNodeDataPoolListElement c_MessageCounterElement;
   C_OscNodeDataPoolListElement c_ChecksumElement;

   c_MessageCounterSig.e_ComByteOrder = C_OscCanSignal::eBYTE_ORDER_INTEL;
   c_MessageCounterSig.u16_ComBitLength = 8;
   c_MessageCounterSig.u16_ComBitStart = 48;
   c_MessageCounterElement.c_Name = mhc_ECES_MESSAGE_COUNTER;
   c_MessageCounterElement.c_Comment =
      "Automatically generated signal to represent message counter signal of the ECeS Message.";

   c_ChecksumSig.e_ComByteOrder = C_OscCanSignal::eBYTE_ORDER_INTEL;
   c_ChecksumSig.u16_ComBitLength = 8;
   c_ChecksumSig.u16_ComBitStart = 56;
   c_ChecksumElement.c_Name = mhc_ECES_CHECKSUM;
   c_ChecksumElement.c_Comment = "Automatically generated signal to represent CRC checksum signal of the ECeS Message.";

   // insert signals
   int32_t s32_Result;
   s32_Result = orc_Node.InsertSignal(orc_Id.e_ComProtocol, orc_Id.u32_InterfaceIndex, orc_Id.u32_DatapoolIndex,
                                      orc_Id.q_MessageIsTx, orc_Id.u32_MessageIndex, ou32_SignalIndex,
                                      c_MessageCounterSig, c_MessageCounterElement);
   tgl_assert(s32_Result == C_NO_ERR);
   s32_Result = orc_Node.InsertSignal(orc_Id.e_ComProtocol, orc_Id.u32_InterfaceIndex, orc_Id.u32_DatapoolIndex,
                                      orc_Id.q_MessageIsTx, orc_Id.u32_MessageIndex, ou32_SignalIndex + 1,
                                      c_ChecksumSig, c_ChecksumElement);
   tgl_assert(s32_Result == C_NO_ERR);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Check if received message has protocol ECeS and process it accordingly

      Special signals ("ECeS_Message_Counter" and "ECeS_Checksum") indicate that it is an ECeS message
      Validate both their values and update message status
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OscComMessageLogger::m_CheckAndHandleEcesMessage()
{
   // Look for special signals that indicate an ECeS message
   for (uint32_t u32_Counter = 0U; u32_Counter < this->mc_HandledCanMessage.c_Signals.size(); ++u32_Counter)
   {
      C_OscComMessageLoggerDataSignal & rc_Signal = this->mc_HandledCanMessage.c_Signals[u32_Counter];

      // Look for special signal "ECeS_Message_Counter" in the message received
      if (rc_Signal.c_Name.AnsiCompareIc(mhc_ECES_MESSAGE_COUNTER) == 0)
      {
         // Search for the unique ECeS message (based on CAN Id) in the saved ECeS messages
         const std::map<uint32_t,  C_SclString>::iterator c_Iterator = this->mc_EcesMessages.find(
            this->mpc_OsySysDefMessage->u32_CanId);

         this->mc_HandledCanMessage.c_Status = "Counter OK. ";
         C_SclString c_DifferenceValue = "";

         // Validate message counter with previous message with same CAN Id
         if (c_Iterator != this->mc_EcesMessages.end())
         {
            // compare the message counter values (ideally the counter of the received message should be +1)
            int32_t s32_CounterDiff = rc_Signal.c_Value.ToInt() - c_Iterator->second.ToInt();

            // When max value of counter is reached, ensure that the increment is valid
            // E.g if max value = 255, next counter value = 0. Hence s32_CounterDiff = -255.
            // In order that the counting continues further, set the difference to 1
            if (std::abs(s32_CounterDiff) == static_cast<int32_t>(mhu32_ECES_MAX_MESSAGE_COUNTER))
            {
               s32_CounterDiff = 1;
            }

            // if message counter is incorrect
            if (s32_CounterDiff != 1)
            {
               this->mc_HandledCanMessage.c_Status = "Counter Invalid. ";
            }

            c_DifferenceValue = C_SclString::IntToStr(s32_CounterDiff);

            // Insert a "+" sign for positive difference value
            if (s32_CounterDiff >= 1)
            {
               c_DifferenceValue = c_DifferenceValue.Insert("+", 0);
            }
            this->mc_HandledCanMessage.c_Status += "Diff: " + c_DifferenceValue +
                                                   "; Prev. Counter = " + c_Iterator->second + ". ";

            // remove the old message
            this->mc_EcesMessages.erase(this->mpc_OsySysDefMessage->u32_CanId);
         }

         this->mc_EcesMessages.insert(std::pair<uint32_t, C_SclString>(this->mpc_OsySysDefMessage->u32_CanId,
                                                                       rc_Signal.c_Value));
      }

      // Look for special signal "ECeS_Checksum" in the message received
      else if (rc_Signal.c_Name.AnsiCompareIc(mhc_ECES_CHECKSUM) == 0)
      {
         const uint8_t u8_OrigCrcValue = static_cast<uint8_t>(this->mc_HandledCanMessage.c_CanMsg.au8_Data[7]);
         const uint8_t u8_CalculatedCrcValue = C_OscComAutoSupport::h_GetCyclicRedundancyCheckCalculation(6,
                                                                                                          this->mc_HandledCanMessage.c_CanMsg.au8_Data);
         // Validate checksum
         if (u8_OrigCrcValue == u8_CalculatedCrcValue)
         {
            this->mc_HandledCanMessage.c_Status += "CRC OK.";
         }
         else
         {
            this->mc_HandledCanMessage.c_Status += "CRC Invalid.";
         }
      }
      // added to remove static warning
      else
      {
         // do nothing
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Reset ECeS messages
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OscComMessageLogger::m_ResetEcesMessages()
{
   this->mc_EcesMessages.clear();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Save the last seen ECoS message received

      When a new ECoS message is received, the old one is overwritten
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OscComMessageLogger::m_SaveEcosMessage()
{
   const T_STWCAN_Msg_RX & rc_Msg =  this->mc_HandledCanMessage.c_CanMsg;

   std::vector<uint8_t> c_MessageData;

   // Insert message data into vector
   c_MessageData.insert(c_MessageData.end(), &rc_Msg.au8_Data[0], &rc_Msg.au8_Data[rc_Msg.u8_DLC]);

   // mc_EcosMessage details saved: CAN Id, message name, message data
   this->mc_EcosMessage.u32_CanId = rc_Msg.u32_ID;
   this->mc_EcosMessage.c_MessageName = this->mc_HandledCanMessage.c_Name;
   this->mc_EcosMessage.c_MessageData = c_MessageData;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Checks if the received message has protocol ECoS

   Searches in all loaded databases and finds if received message has protocol ECoS

   \param[in]       orc_Msg    CAN message received

   \return
   true if it is an ECoS message
   false otherwise
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_OscComMessageLogger::m_CheckIfEcosMessage(const stw::can::T_STWCAN_Msg_RX & orc_Msg)
{
   bool q_Return = false;

   std::map<stw::scl::C_SclString, C_OscComMessageLoggerOsySysDefConfig>::const_iterator c_ItSysDef;

   for (c_ItSysDef = this->mc_OsySysDefs.begin(); c_ItSysDef != this->mc_OsySysDefs.end(); ++c_ItSysDef)
   {
      // Check if the database is active
      if (this->mc_DatabaseActiveFlags[c_ItSysDef->first] == true)
      {
         const C_OscSystemDefinition & rc_OsySysDef = c_ItSysDef->second.c_OsySysDef;
         uint32_t u32_NodeCounter;

         // Search all nodes which are connected to to the CAN bus
         for (u32_NodeCounter = 0U; u32_NodeCounter < rc_OsySysDef.c_Nodes.size(); ++u32_NodeCounter)
         {
            const C_OscNode & rc_Node = rc_OsySysDef.c_Nodes[u32_NodeCounter];
            uint32_t u32_IntfCounter;
            bool q_IntfFound = false;

            // Search an interface which is connected to the bus
            for (u32_IntfCounter = 0U; u32_IntfCounter < rc_Node.c_Properties.c_ComInterfaces.size(); ++u32_IntfCounter)
            {
               if ((rc_Node.c_Properties.c_ComInterfaces[u32_IntfCounter].GetBusConnected() == true) &&
                   (rc_Node.c_Properties.c_ComInterfaces[u32_IntfCounter].u32_BusIndex ==
                    c_ItSysDef->second.u32_BusIndex))
               {
                  // Com Interface found
                  q_IntfFound = true;
                  break;
               }
            }
            if (q_IntfFound == true)
            {
               // Check all messages of the node for this interface on this bus
               uint32_t u32_ProtCounter;

               for (u32_ProtCounter = 0U; u32_ProtCounter < rc_Node.c_ComProtocols.size(); ++u32_ProtCounter)
               {
                  const C_OscCanProtocol & rc_CanProt = rc_Node.c_ComProtocols[u32_ProtCounter];

                  // Search for ECoS messages only
                  if (rc_CanProt.e_Type == C_OscCanProtocol::eCAN_OPEN_SAFETY)
                  {
                     tgl_assert(u32_IntfCounter < rc_CanProt.c_ComMessages.size());
                     if (u32_IntfCounter < rc_CanProt.c_ComMessages.size())
                     {
                        // Search TX messages
                        const std::vector<C_OscCanMessage> & rc_CanMsgContainerTx =
                           rc_CanProt.c_ComMessages[u32_IntfCounter].c_TxMessages;
                        bool q_SearchAlsoInRxMessages = true; // flag to improve performance
                        uint32_t u32_CanMsgCounter;

                        for (u32_CanMsgCounter = 0U; u32_CanMsgCounter < rc_CanMsgContainerTx.size();
                             ++u32_CanMsgCounter)
                        {
                           const C_OscCanMessage & rc_OscMsg = rc_CanMsgContainerTx[u32_CanMsgCounter];

                           // if CAN ID and extended bit match
                           if ((orc_Msg.u32_ID == rc_OscMsg.u32_CanId) &&
                               ((orc_Msg.u8_XTD == 1U) == rc_OscMsg.q_IsExtended))
                           {
                              // Found message, no need to search in Rx messages
                              q_SearchAlsoInRxMessages = false;
                              q_Return = true;
                              break;
                           }
                        }

                        // Search RX messages
                        if (q_SearchAlsoInRxMessages == true)
                        {
                           const std::vector<C_OscCanMessage> & rc_CanMsgContainerRx =
                              rc_CanProt.c_ComMessages[u32_IntfCounter].c_RxMessages;
                           uint32_t u32_CanMsgCounterRx;

                           for (u32_CanMsgCounterRx = 0U; u32_CanMsgCounterRx <
                                rc_CanMsgContainerRx.size();
                                ++u32_CanMsgCounterRx)
                           {
                              const C_OscCanMessage & rc_OscMsg =
                                 rc_CanMsgContainerRx[u32_CanMsgCounterRx];

                              // No check of dlc here, it will be checked for each signal
                              if ((orc_Msg.u32_ID == rc_OscMsg.u32_CanId) &&
                                  ((orc_Msg.u8_XTD == 1U) == rc_OscMsg.q_IsExtended))
                              {
                                 q_Return = true;
                                 break;
                              }
                           } // end for Rx
                        }    // end if q_SearchAlsoInRxMessages
                     } // end if messages for protocol exist
                  }    // end if protocol is eCAN_OPEN_SAFETY
               } // end for c_ComProtocols
            }    // end if Com interface found
         } // end for c_Nodes
      }    // end if database is active
   } // end for mc_OsySysDefs
   return q_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Resets ECeS messages
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OscComMessageLogger::ResetEcesMessages(void)
{
   this->m_ResetEcesMessages();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Handle ECoS inverted frame

  Process the message sent after an ECoS message (contains inverted data).
  The inverted frame has:
      - Name field empty
      _ CAN Id +1 to ECoS message CAN Id
      - Data inverted
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OscComMessageLogger::m_HandleEcosInvertedMessage()
{
   // Check if an ECoS message has been previously received.
   // If yes, verify CAN Id, data size and if inverted data exists
   if (this->mc_EcosMessage.c_MessageData.size() > 0)
   {
      uint8_t au8_InvertedData[8U];
      std::vector<uint8_t> c_InvertedData;
      this->mc_HandledCanMessage.c_Status = "";

      const uint8_t u8_DataSize = static_cast<uint8_t>(this->mc_EcosMessage.c_MessageData.size());

      // Compare CAN ID and DLC (data length code) of received messsage to that of last received ECoS message.
      if (((this->mc_HandledCanMessage.c_CanMsg.u32_ID - 1) == (this->mc_EcosMessage.u32_CanId)) &&
          (this->mc_HandledCanMessage.c_CanMsg.u8_DLC == u8_DataSize))
      {
         // Set message name
         this->mc_HandledCanMessage.c_Name = (this->mc_EcosMessage.c_MessageName) + " (Inverted Frame)";

         // Invert message data of the received CAN message
         C_OscComAutoSupport::h_InvertCanMessage(this->mc_HandledCanMessage.c_CanMsg.au8_Data,
                                                 au8_InvertedData);

         c_InvertedData.insert(c_InvertedData.end(), &au8_InvertedData[0],
                               &au8_InvertedData[this->mc_HandledCanMessage.c_CanMsg.u8_DLC]);

         // Compare data
         if (c_InvertedData == this->mc_EcosMessage.c_MessageData)
         {
            this->mc_HandledCanMessage.c_Status = "Inverted data OK";
         }
         else
         {
            this->mc_HandledCanMessage.c_Status = "Incorrect inverted data";
         }
      }
   }
}
