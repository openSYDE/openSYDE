//-----------------------------------------------------------------------------
/*!
   \internal
   \file
   \brief       Class for logging CAN messages (implementation)

   Handles:
   - CAN message filter (CAN-Id, protocol relevance)
   - Converting CAN message to protocol specific information
   - Writing to file

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

#include "stwerrors.h"

#include "C_OSCLoggingHandler.h"
#include "C_OSCComMessageLogger.h"
#include "C_OSCSystemDefinitionFiler.h"
#include "CCMONProtocol.h"

/* -- Used Namespaces ------------------------------------------------------ */
using namespace stw_types;
using namespace stw_errors;
using namespace stw_can;
using namespace stw_scl;
using namespace stw_opensyde_core;
using namespace stw_cmon_protocol;

/* -- Module Global Constants ---------------------------------------------- */

/* -- Types ---------------------------------------------------------------- */

/* -- Global Variables ----------------------------------------------------- */

/* -- Module Global Variables ---------------------------------------------- */

/* -- Module Global Function Prototypes ------------------------------------ */

/* -- Implementation ------------------------------------------------------- */

//-----------------------------------------------------------------------------
/*!
   \brief   Default constructor

   \created     06.09.2018  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
C_OSCComMessageLoggerFilter::C_OSCComMessageLoggerFilter(void) :
   u8_ExtendedId(0U),
   u32_StartId(0U),
   u32_EndId(0U),
   q_PassFilter(true)
{
}

//-----------------------------------------------------------------------------
/*!
   \brief   Check if current equal to orc_Cmp

   \param[in] orc_Cmp Compared instance

   \return
   Current equal to orc_Cmp
   Else false

   \created     07.09.2018  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
bool C_OSCComMessageLoggerFilter::operator ==(const C_OSCComMessageLoggerFilter & orc_Cmp) const
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

//-----------------------------------------------------------------------------
/*!
   \brief   Default constructor

   \created     03.09.2018  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
C_OSCComMessageLogger::C_OSCComMessageLogger(void) :
   mpc_OsySysDefMessage(NULL),
   mpc_OsySysDefDataPoolList(NULL),
   me_Protocol(stw_cmon_protocol::CMONL7ProtocolNone),
   mq_Paused(false)
{
   this->mc_ProtocolHex.SetDecimalMode(false);
   this->mc_ProtocolDec.SetDecimalMode(true);
}

//-----------------------------------------------------------------------------
/*!
   \brief   Default destructor

   \created     07.09.2018  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
C_OSCComMessageLogger::~C_OSCComMessageLogger(void)
{
   //lint -e{1540} Never took ownership of mpc_OsySysDefMessage and mpc_OsySysDefDataPoolList
}

//-----------------------------------------------------------------------------
/*!
   \brief   Continues the paused the logging

   Continues the logging of this logger.
   The delivering instance of C_OSCComDriverBase must be running logging too.

   \created     05.09.2018  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
void C_OSCComMessageLogger::Continue(void)
{
   this->mq_Paused = false;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Pauses the logging.

   Pauses the logging of this logger.
   The delivering instance of C_OSCComDriverBase will not be changed by this pause.
   Other registered logger will still log.

   \created     05.09.2018  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
void C_OSCComMessageLogger::Pause(void)
{
   this->mq_Paused = true;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Optional function to react on the stop of the communication

   Base class does nothing here.

   \created     25.09.2018  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
void C_OSCComMessageLogger::Stop(void)
{
}

//-----------------------------------------------------------------------------
/*!
   \brief   Sets the protocol for interpreting

   \param[in]     oe_Protocol     Set protocol type

   \created     05.09.2018  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
void C_OSCComMessageLogger::SetProtocol(const e_CMONL7Protocols oe_Protocol)
{
   this->me_Protocol = oe_Protocol;
   this->mc_ProtocolHex.SetProtocolMode(oe_Protocol);
   this->mc_ProtocolDec.SetProtocolMode(oe_Protocol);
}

//-----------------------------------------------------------------------------
/*!
   \brief   Adds an openSYDE system definition for analyzing

   \param[in]     orc_PathSystemDefinition       Path of system definition file (Must be .syde_sysdef)
   \param[in]     orc_PathDeviceDefinitions      Path to device definition description files
   \param[out]    orc_Busses                     All CAN buses of system definition

   \return
   C_NO_ERR    data read
   C_RANGE     specified system definition file does not exist
   C_NOACT     specified file is present but structure is invalid (e.g. invalid XML file)
   C_CONFIG    system definition file content is invalid or incomplete
               device definition file could not be loaded
   C_OVERFLOW  node in system definition references a device not part of the device definitions
   C_COM       no CAN bus in system definition

   \created     17.09.2018  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
sint32 C_OSCComMessageLogger::AddOsySysDef(const C_SCLString & orc_PathSystemDefinition,
                                           const C_SCLString & orc_PathDeviceDefinitions,
                                           std::vector<C_OSCSystemBus> & orc_Busses)
{
   C_OSCSystemDefinition c_SysDef;
   sint32 s32_Return;

   s32_Return = C_OSCSystemDefinitionFiler::h_LoadSystemDefinitionFile(c_SysDef, orc_PathSystemDefinition,
                                                                       orc_PathDeviceDefinitions);
   if (s32_Return == C_NO_ERR)
   {
      uint32 u32_BusCounter;
      uint32 u32_FirstBusIndex = 0U;
      bool q_CanBusFound = false;

      // Return the CAN buses
      for (u32_BusCounter = 0U; u32_BusCounter < c_SysDef.c_Buses.size(); ++u32_BusCounter)
      {
         // Only CAN buses are relevant
         if (c_SysDef.c_Buses[u32_BusCounter].e_Type == C_OSCSystemBus::eCAN)
         {
            orc_Busses.push_back(c_SysDef.c_Buses[u32_BusCounter]);
            if (q_CanBusFound == false)
            {
               q_CanBusFound = true;
               // And save the first CAN bus as initial bus
               u32_FirstBusIndex = u32_BusCounter;
            }
         }
      }

      if (q_CanBusFound == true)
      {
         // Save the system definition
         this->m_InsertOsySysDef(orc_PathSystemDefinition, c_SysDef, u32_FirstBusIndex);
      }
      else
      {
         s32_Return = C_COM;
      }
   }

   if (s32_Return != C_NO_ERR)
   {
      osc_write_log_error("Loading System Definition", "Could not load System Definition. Error code: " +
                          C_SCLString::IntToStr(s32_Return));
   }

   return s32_Return;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Adds an openSYDE system definition for analyzing

   \param[in]     orc_PathSystemDefinition       Path of system definition file (Must be .syde_sysdef)
   \param[in]     orc_PathDeviceDefinitions      Path to device definition description files
   \param[in]     ou32_BusIndex                  Bus index of CAN bus of system definition for monitoring

   \return
   C_NO_ERR    data read
   C_RANGE     specified system definition file does not exist
   C_NOACT     specified file is present but structure is invalid (e.g. invalid XML file)
   C_CONFIG    system definition file content is invalid or incomplete
               device definition file could not be loaded
   C_OVERFLOW  node in system definition references a device not part of the device definitions
   C_COM       no CAN bus in system definition

   \created     18.09.2018  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
sint32 C_OSCComMessageLogger::AddOsySysDef(const C_SCLString & orc_PathSystemDefinition,
                                           const C_SCLString & orc_PathDeviceDefinitions, const uint32 ou32_BusIndex)
{
   C_OSCSystemDefinition c_SysDef;
   sint32 s32_Return;

   s32_Return = C_OSCSystemDefinitionFiler::h_LoadSystemDefinitionFile(c_SysDef, orc_PathSystemDefinition,
                                                                       orc_PathDeviceDefinitions);
   if (s32_Return == C_NO_ERR)
   {
      if ((ou32_BusIndex < c_SysDef.c_Buses.size()) &&
          (c_SysDef.c_Buses[ou32_BusIndex].e_Type == C_OSCSystemBus::eCAN))
      {
         // Save the system definition
         this->m_InsertOsySysDef(orc_PathSystemDefinition, c_SysDef, ou32_BusIndex);
      }
      else
      {
         // Bus configuration invalid
         s32_Return = C_COM;
      }
   }

   return s32_Return;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Updates the set bus index of the specific system definition

   \param[in]     orc_PathSystemDefinition       Path of system definition file (Must be .syde_sysdef)
   \param[in]     ou32_BusIndex                  Bus index of CAN bus of system definition for monitoring

   \return
   C_NO_ERR    Bus index for this system definition adapted
   C_NOACT     No system definition found with this path

   \created     18.09.2018  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
sint32 C_OSCComMessageLogger::SetOsySysDefBus(const C_SCLString & orc_PathSystemDefinition, const uint32 ou32_BusIndex)
{
   sint32 s32_Return = C_NOACT;

   std::map<stw_scl::C_SCLString, C_OSCComMessageLoggerOsySysDefConfig>::iterator c_ItSysDef = this->mc_OsySysDefs.find(
      orc_PathSystemDefinition);

   if (c_ItSysDef != this->mc_OsySysDefs.end())
   {
      c_ItSysDef->second.u32_BusIndex = ou32_BusIndex;

      s32_Return = C_NO_ERR;
   }

   return s32_Return;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Removes an openSYDE system definition

   \param[in]     orc_PathSystemDefinition       Path of system definition file as identification

   \return
   C_NO_ERR    System definition removed
   C_NOACT     No system definition found with this path

   \created     17.09.2018  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
sint32 C_OSCComMessageLogger::RemoveOsySysDef(const C_SCLString & orc_PathSystemDefinition)
{
   sint32 s32_Return = C_NOACT;

   std::map<stw_scl::C_SCLString, C_OSCComMessageLoggerOsySysDefConfig>::iterator c_ItSysDef = this->mc_OsySysDefs.find(
      orc_PathSystemDefinition);

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

//-----------------------------------------------------------------------------
/*!
   \brief   Adds a new filter configuration

   \param[in]  orc_Filter     Filter configuration to add

   \created     05.09.2018  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
void C_OSCComMessageLogger::AddFilter(const C_OSCComMessageLoggerFilter & orc_Filter)
{
   this->mc_CanFilterConfig.push_back(orc_Filter);
}

//-----------------------------------------------------------------------------
/*!
   \brief   Removes a filter configuration

   \param[in]  orc_Filter     Filter configuration to remove

   \created     05.09.2018  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
void C_OSCComMessageLogger::RemoveFilter(const C_OSCComMessageLoggerFilter & orc_Filter)
{
   uintn un_Counter;

   for (un_Counter = 0U; un_Counter < this->mc_CanFilterConfig.size(); ++un_Counter)
   {
      if (orc_Filter == this->mc_CanFilterConfig[un_Counter])
      {
         // Filter found, remove it
         this->mc_CanFilterConfig.erase(this->mc_CanFilterConfig.begin() + un_Counter);
         break;
      }
   }
}

//-----------------------------------------------------------------------------
/*!
   \brief   If necessary handle and log the CAN message

   Check if CAN message must be logged.
   Check CAN message for configured protocols.

   \param[in]     orc_Msg        Current CAN message
   \param[in]     oq_IsTx        Message was sent of this application itself

   \return
   C_NO_ERR    CAN message logged
   C_NOACT     CAN message not relevant
   C_BUSY      Logger is paused

   \created     03.09.2018  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
sint32 C_OSCComMessageLogger::HandleCanMessage(const T_STWCAN_Msg_RX & orc_Msg, const bool oq_IsTx)
{
   sint32 s32_Return = C_BUSY;

   if (this->mq_Paused == false)
   {
      if (this->m_CheckFilter(orc_Msg) == true)
      {
         // Check the registered openSYDE system definitions.
         // The result is in mpc_OsySysDefMessage and mpc_OsySysDefDataPoolList
         if (this->m_CheckSysDef(orc_Msg) == false)
         {
            // Check for other interpretations of deriving classes
            this->m_CheckInterpretation(orc_Msg);
         }

         // TODO: Handling file logging here
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

//-----------------------------------------------------------------------------
/*!
   \brief   Checks a specific CAN message for matching the filter configuration

   \param[in]     orc_Msg        Current CAN message

   \return
   true     CAN message is relevant
   false    CAN message is not relevant

   \created     05.09.2018  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
bool C_OSCComMessageLogger::m_CheckFilter(const T_STWCAN_Msg_RX & orc_Msg) const
{
   bool q_Return = false;

   if (this->mc_CanFilterConfig.size() > 0)
   {
      uintn un_FilterCounter;
      bool q_Blocked = false;
      bool q_PassFilterAvailable = false;

      // Check all block filters first
      for (un_FilterCounter = 0U; un_FilterCounter < this->mc_CanFilterConfig.size(); ++un_FilterCounter)
      {
         if (this->mc_CanFilterConfig[un_FilterCounter].q_PassFilter == false)
         {
            if ((orc_Msg.u8_XTD == this->mc_CanFilterConfig[un_FilterCounter].u8_ExtendedId) &&
                (orc_Msg.u32_ID >= this->mc_CanFilterConfig[un_FilterCounter].u32_StartId) &&
                (orc_Msg.u32_ID <= this->mc_CanFilterConfig[un_FilterCounter].u32_EndId))
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
            for (un_FilterCounter = 0U; un_FilterCounter < this->mc_CanFilterConfig.size(); ++un_FilterCounter)
            {
               if ((orc_Msg.u8_XTD == this->mc_CanFilterConfig[un_FilterCounter].u8_ExtendedId) &&
                   (orc_Msg.u32_ID >= this->mc_CanFilterConfig[un_FilterCounter].u32_StartId) &&
                   (orc_Msg.u32_ID <= this->mc_CanFilterConfig[un_FilterCounter].u32_EndId))
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

   return q_Return;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Returns the CAN message as protocol specific interpreted string with values as hex

   \param[in]     orc_Msg        Current CAN message

   \return
   CAN message converted to protocol string
   Empty string if no protocol is configured or the CAN message does not match to the configured protocol

   \created     05.09.2018  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
C_SCLString C_OSCComMessageLogger::m_GetProtocolStringHex(const T_STWCAN_Msg_RX & orc_Msg) const
{
   C_SCLString c_Result = "";

   if (this->me_Protocol != stw_cmon_protocol::CMONL7ProtocolNone)
   {
      c_Result = this->mc_ProtocolHex.MessageToStringProtocolOnly(orc_Msg);
   }

   return c_Result;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Returns the CAN message as protocol specific interpreted string with values as decimal

   \param[in]     orc_Msg        Current CAN message

   \return
   CAN message converted to protocol string
   Empty string if no protocol is configured or the CAN message does not match to the configured protocol

   \created     05.09.2018  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
C_SCLString C_OSCComMessageLogger::m_GetProtocolStringDec(const T_STWCAN_Msg_RX & orc_Msg) const
{
   C_SCLString c_Result = "";

   if (this->me_Protocol != stw_cmon_protocol::CMONL7ProtocolNone)
   {
      c_Result = this->mc_ProtocolDec.MessageToStringProtocolOnly(orc_Msg);
   }

   return c_Result;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Inserts openSYDE system definition to parsing configuration

   Splitting of parts of function AddOsySysDef into this one because of performance.
   Loading of file needs a lot more time and shall not be part of the synchronized parts.
   Deriving classes can override this function to use synchronization.

   \param[in]     orc_PathSystemDefinition   Path of system definition file (Must be .syde_sysdef)
   \param[in]     orc_OsySysDef              Loaded openSYDE system definition
   \param[in]     ou32_BusIndex              Used CAN bus index

   \created     19.09.2018  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
void C_OSCComMessageLogger::m_InsertOsySysDef(const C_SCLString & orc_PathSystemDefinition,
                                              const C_OSCSystemDefinition & orc_OsySysDef, const uint32 ou32_BusIndex)
{
   C_OSCComMessageLoggerOsySysDefConfig c_SysDefConfig(orc_OsySysDef, ou32_BusIndex);

   std::map<stw_scl::C_SCLString, C_OSCComMessageLoggerOsySysDefConfig>::iterator c_ItNewConfig;

   c_ItNewConfig = this->mc_OsySysDefs.insert(std::pair<C_SCLString,
                                                        C_OSCComMessageLoggerOsySysDefConfig>(orc_PathSystemDefinition,
                                                                                              c_SysDefConfig)).first;
   this->mc_ProtocolDec.AddOsySysDef(&(c_ItNewConfig->second));
   this->mc_ProtocolHex.AddOsySysDef(&(c_ItNewConfig->second));
}

//-----------------------------------------------------------------------------
/*!
   \brief   Checks if a matching CAN message is defined in at least one registered openSYDE system definition

   The result will be saved to mpc_OsySysDefMessage and mpc_OsySysDefDataPoolList.
   If mpc_OsySysDefMessage and mpc_OsySysDefDataPoolList is NULL, no CAN message found in any system definition.

   \param[in]     orc_Msg             CAN message for analyzing

   \return
   true     Matching CAN message found
   false    No matching CAN message found

   \created     17.09.2018  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
bool C_OSCComMessageLogger::m_CheckSysDef(const T_STWCAN_Msg_RX & orc_Msg)
{
   bool q_Return = false;

   std::map<stw_scl::C_SCLString, C_OSCComMessageLoggerOsySysDefConfig>::const_iterator c_ItSysDef;

   this->mpc_OsySysDefMessage = NULL;
   this->mpc_OsySysDefDataPoolList = NULL;

   for (c_ItSysDef = this->mc_OsySysDefs.begin(); c_ItSysDef != this->mc_OsySysDefs.end(); ++c_ItSysDef)
   {
      const C_OSCSystemDefinition & rc_OsySysDef = c_ItSysDef->second.c_OsySysDef;
      uint32 u32_NodeCounter;

      // Search all nodes which are connected to to the CAN bus
      for (u32_NodeCounter = 0U; u32_NodeCounter < rc_OsySysDef.c_Nodes.size(); ++u32_NodeCounter)
      {
         const C_OSCNode & rc_Node = rc_OsySysDef.c_Nodes[u32_NodeCounter];
         uint32 u32_IntfCounter;
         bool q_IntfFound = false;

         // Search an interface which is connected to the bus
         for (u32_IntfCounter = 0U; u32_IntfCounter < rc_Node.c_Properties.c_ComInterfaces.size(); ++u32_IntfCounter)
         {
            if (rc_Node.c_Properties.c_ComInterfaces[u32_IntfCounter].u32_BusIndex == c_ItSysDef->second.u32_BusIndex)
            {
               // Com Interface found
               q_IntfFound = true;
               break;
            }
         }

         if (q_IntfFound == true)
         {
            // Check all messages of the node for this interface on this bus
            uint32 u32_ProtCounter;

            for (u32_ProtCounter = 0U; u32_ProtCounter < rc_Node.c_ComProtocols.size(); ++u32_ProtCounter)
            {
               tgl_assert(u32_IntfCounter < rc_Node.c_ComProtocols.size());
               if (u32_IntfCounter < rc_Node.c_ComProtocols.size())
               {
                  const C_OSCCanProtocol & rc_CanProt = rc_Node.c_ComProtocols[u32_ProtCounter];
                  const std::vector<C_OSCCanMessage> & rc_CanMsgContainerTx =
                     rc_CanProt.c_ComMessages[u32_IntfCounter].c_TxMessages;
                  uint32 u32_CanMsgCounter;

                  for (u32_CanMsgCounter = 0U; u32_CanMsgCounter < rc_CanMsgContainerTx.size(); ++u32_CanMsgCounter)
                  {
                     const C_OSCCanMessage & rc_OscMsg = rc_CanMsgContainerTx[u32_CanMsgCounter];

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
                              C_OSCCanProtocol::h_GetComListConst(rc_Node.c_DataPools[rc_CanProt.u32_DataPoolIndex],
                                                                  u32_IntfCounter, true);
                           tgl_assert(this->mpc_OsySysDefDataPoolList != NULL);

                           q_Return = true;
                        }

                        break;
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

   return q_Return;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Returns the interpreted string of the found CAN message interpretation of a openSYDE system definition

   The result in mpc_OsySysDefMessage and mpc_OsySysDefDataPoolList will be used.
   All values are shown as hexadecimal value.

   \return
   Interpreted string if mpc_OsySysDefMessage and mpc_OsySysDefDataPoolList is not NULL
   Empty string if mpc_OsySysDefMessage or mpc_OsySysDefDataPoolList is NULL

   \created     20.09.2018  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
C_SCLString C_OSCComMessageLogger::m_GetOsySysDefStringHex(void) const
{
   C_SCLString c_Return = "";

   if ((this->mpc_OsySysDefDataPoolList != NULL) &&
       (this->mpc_OsySysDefMessage != NULL))
   {
      // TODO for logging
      c_Return = this->mpc_OsySysDefMessage->c_Name;
   }

   return c_Return;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Returns the interpreted string of the found CAN message interpretation of a openSYDE system definition

   The result in mpc_OsySysDefMessage and mpc_OsySysDefDataPoolList will be used.
   All values are shown as decimal value.

   \return
   Interpreted string if mpc_OsySysDefMessage and mpc_OsySysDefDataPoolList is not NULL
   Empty string if mpc_OsySysDefMessage or mpc_OsySysDefDataPoolList is NULL

   \created     20.09.2018  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
C_SCLString C_OSCComMessageLogger::m_GetOsySysDefStringDec(void) const
{
   C_SCLString c_Return = "";

   if ((this->mpc_OsySysDefDataPoolList != NULL) &&
       (this->mpc_OsySysDefMessage != NULL))
   {
      // TODO for logging
      c_Return = this->mpc_OsySysDefMessage->c_Name;
   }

   return c_Return;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Returns the timestamp as string

   Using C_CMONProtocols implementation

   \param[in]     ou64_TimeStamp   Timestamp in us

   \return
   Formated timestamp

   \created     10.10.2018  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
C_SCLString C_OSCComMessageLogger::m_GetTimestampAsString(const uint64 ou64_TimeStamp) const
{
   return C_CMONProtocols::FormatTimeStamp(ou64_TimeStamp, true);
}

//-----------------------------------------------------------------------------
/*!
   \brief   Checks if a matching CAN message is defined in a description / specification

   \param[in]     orc_Msg             CAN message for analyzing

   \created     21.09.2018  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
void C_OSCComMessageLogger::m_CheckInterpretation(const T_STWCAN_Msg_RX & orc_Msg)
{
   // Nothing to do here.
   // Deriving class must override this function to use an own CAN message description or specification.
   static_cast<void>(orc_Msg);
}

//-----------------------------------------------------------------------------
/*!
   \brief   Returns an interpreted string of a protocol

   This is the default implementation and returns an empty string

   \return
   Interpreted string if match protocol found
   Empty string if no match found

   \created     21.09.2018  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
C_SCLString C_OSCComMessageLogger::m_GetProtocolStringHexHook(void) const
{
   return "";
}

//-----------------------------------------------------------------------------
/*!
   \brief   Returns an interpreted string of a protocol

   This is the default implementation and returns an empty string

   \return
   Interpreted string if match protocol found
   Empty string if no match found

   \created     21.09.2018  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
C_SCLString C_OSCComMessageLogger::m_GetProtocolStringDecHook(void) const
{
   return "";
}
