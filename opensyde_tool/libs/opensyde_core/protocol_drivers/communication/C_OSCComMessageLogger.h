//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Class for logging CAN messages (header)

   See cpp file for detailed description

   \copyright   Copyright 2018 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_OSCCOMMESSAGELOGGER_H
#define C_OSCCOMMESSAGELOGGER_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <vector>
#include <map>

#include "stwtypes.h"
#include "CSCLString.h"
#include "stw_can.h"

#include "C_OSCComLoggerProtocols.h"
#include "C_OSCSystemDefinition.h"
#include "C_OSCComMessageLoggerOsySysDefConfig.h"
#include "C_OSCComMessageLoggerData.h"
#include "C_OSCComMessageLoggerFileBase.h"
#include "C_OSCCanSignal.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw_opensyde_core
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_OSCComMessageLoggerFilter
{
public:
   C_OSCComMessageLoggerFilter(void);

   bool operator ==(const C_OSCComMessageLoggerFilter & orc_Cmp) const;

   stw_types::uint8 u8_ExtendedId; ///< 0 -> 11bit ID; 1 -> 29bit ID
   stw_types::uint32 u32_StartId;  ///< First CAN ID which is relevant for the filter configuration
   stw_types::uint32 u32_EndId;    ///< Last CAN ID which is relevant for the filter configuration
   bool q_PassFilter;              ///< true -> Filter for passing; false -> Filter for blocking configured CAN IDs
};

class C_OSCComMessageLogger
{
public:
   C_OSCComMessageLogger(void);
   virtual ~C_OSCComMessageLogger(void);

   virtual void Continue(void);
   virtual void Pause(void);
   virtual void Stop(void);
   virtual void Start(void);

   // openSYDE system definition handling
   virtual void SetProtocol(const stw_cmon_protocol::e_CMONL7Protocols oe_Protocol);
   stw_types::sint32 AddOsySysDef(const stw_scl::C_SCLString & orc_PathSystemDefinition,
                                  std::vector<C_OSCSystemBus> & orc_Busses);
   stw_types::sint32 AddOsySysDef(const stw_scl::C_SCLString & orc_PathSystemDefinition,
                                  const stw_types::uint32 ou32_BusIndex, std::vector<C_OSCSystemBus> & orc_Busses);
   virtual stw_types::sint32 SetOsySysDefBus(const stw_scl::C_SCLString & orc_PathSystemDefinition,
                                             const stw_types::uint32 ou32_BusIndex);
   virtual stw_types::sint32 GetOsySysDef(const stw_scl::C_SCLString & orc_PathSystemDefinition,
                                          C_OSCComMessageLoggerOsySysDefConfig & orc_SystemDefinition);

   // Generic database handling
   virtual stw_types::sint32 RemoveDatabase(const stw_scl::C_SCLString & orc_Path);
   virtual stw_types::sint32 ActivateDatabase(const stw_scl::C_SCLString & orc_Path, const bool oq_Active);

   // Logging handling
   virtual stw_types::sint32 AddLogFileAsc(const stw_scl::C_SCLString & orc_FilePath, const bool oq_HexActive,
                                           const bool oq_RelativeTimeStampActive);
   virtual stw_types::sint32 RemoveLogFile(const stw_scl::C_SCLString & orc_FilePath);
   virtual void RemoveAllLogFiles(void);

   // Filter handling
   virtual void AddFilter(const C_OSCComMessageLoggerFilter & orc_Filter);
   virtual void RemoveFilter(const C_OSCComMessageLoggerFilter & orc_Filter);
   virtual void RemoveAllFilter(void);
   virtual stw_types::uint32 GetFilteredMessages(void) const;

   // CAN bus handling
   virtual stw_types::sint32 HandleCanMessage(const stw_can::T_STWCAN_Msg_RX & orc_Msg, const bool oq_IsTx);
   virtual void ResetCounter(void);
   virtual void UpdateBusLoad(const stw_types::uint8 ou8_BusLoad);
   virtual void UpdateTxErrors(const stw_types::uint32 ou32_TxErrors);

protected:
   const C_OSCComMessageLoggerData & m_GetHandledCanMessage(void) const;

   virtual bool m_CheckFilter(const stw_can::T_STWCAN_Msg_RX & orc_Msg);

   // CANmon based protocol interpretation
   stw_scl::C_SCLString m_GetProtocolStringHex(const stw_can::T_STWCAN_Msg_RX & orc_Msg) const;
   stw_scl::C_SCLString m_GetProtocolStringDec(const stw_can::T_STWCAN_Msg_RX & orc_Msg) const;

   // openSYDE system definition based interpretation
   virtual void m_InsertOsySysDef(const stw_scl::C_SCLString & orc_PathSystemDefinition,
                                  const C_OSCSystemDefinition & orc_OsySysDef, const stw_types::uint32 ou32_BusIndex);
   virtual bool m_CheckSysDef(const stw_can::T_STWCAN_Msg_RX & orc_Msg);
   virtual bool m_InterpretSysDef(stw_opensyde_core::C_OSCComMessageLoggerData & orc_MessageData) const;
   stw_scl::C_SCLString m_GetOsySysDefStringHex(void) const;
   stw_scl::C_SCLString m_GetOsySysDefStringDec(void) const;
   stw_scl::C_SCLString m_GetTimestampAsString(const stw_types::uint64 ou64_TimeStamp) const;

   // Functions for integrating other CAN message interpretations
   virtual bool m_CheckInterpretation(C_OSCComMessageLoggerData & orc_MessageData);

   static void mh_InterpretCanSignalValue(stw_opensyde_core::C_OSCComMessageLoggerDataSignal & orc_Signal,
                                          const stw_types::uint8(&orau8_CanDb)[8], const stw_types::uint8 ou8_CanDlc,
                                          const stw_opensyde_core::C_OSCCanSignal & orc_OscSignal,
                                          const stw_opensyde_core::C_OSCNodeDataPoolContent & orc_OscValue,
                                          const stw_types::float64 of64_Factor, const stw_types::float64 of64_Offset);

   virtual stw_scl::C_SCLString m_GetProtocolStringHexHook(void) const;
   virtual stw_scl::C_SCLString m_GetProtocolStringDecHook(void) const;

   // Generic database (for example openSYDE system definitions) activation flag register
   std::map<stw_scl::C_SCLString, bool> mc_DatabaseActiveFlags;

   const C_OSCCanMessage * mpc_OsySysDefMessage;
   const C_OSCNodeDataPoolList * mpc_OsySysDefDataPoolList;

   stw_cmon_protocol::e_CMONL7Protocols me_Protocol;
   bool mq_Paused;

   // Logging
   std::map<stw_scl::C_SCLString, C_OSCComMessageLoggerFileBase * const> mc_LoggingFiles;

private:
   //Avoid call
   C_OSCComMessageLogger(const C_OSCComMessageLogger &);
   C_OSCComMessageLogger & operator =(const C_OSCComMessageLogger &);

   void m_ConvertCanMessage(const stw_can::T_STWCAN_Msg_RX & orc_Msg, const bool oq_IsTx);
   void m_ResetCounter(void);

   C_OSCComMessageLoggerData mc_HandledCanMessage;
   stw_types::uint64 mu64_FirstTimeStamp;
   stw_types::uint64 mu64_LastTimeStamp;

   C_OSCComLoggerProtocols mc_ProtocolHex;
   C_OSCComLoggerProtocols mc_ProtocolDec;

   // Database interpretation
   std::map<stw_scl::C_SCLString, C_OSCComMessageLoggerOsySysDefConfig> mc_OsySysDefs;

   // Filtering
   std::vector<C_OSCComMessageLoggerFilter> mc_CanFilterConfig;
   stw_types::uint32 mu32_FilteredMessages;

   // Message counting
   std::vector<stw_types::uint32> mc_MsgCounterStandardId;
   std::map<stw_types::uint32, stw_types::uint32> mc_MsgCounterExtendedId;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif
