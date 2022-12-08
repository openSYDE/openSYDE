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

#include "stwtypes.hpp"
#include "C_SclString.hpp"
#include "stw_can.hpp"

#include "C_OscComLoggerProtocols.hpp"
#include "C_OscSystemDefinition.hpp"
#include "C_OscComMessageLoggerOsySysDefConfig.hpp"
#include "C_OscComMessageLoggerData.hpp"
#include "C_OscComMessageLoggerFileBase.hpp"
#include "C_OscCanSignal.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_core
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_OscComMessageLoggerFilter
{
public:
   C_OscComMessageLoggerFilter(void);

   bool operator ==(const C_OscComMessageLoggerFilter & orc_Cmp) const;

   uint8_t u8_ExtendedId; ///< 0 -> 11bit ID; 1 -> 29bit ID
   uint32_t u32_StartId;  ///< First CAN ID which is relevant for the filter configuration
   uint32_t u32_EndId;    ///< Last CAN ID which is relevant for the filter configuration
   bool q_PassFilter;     ///< true -> Filter for passing; false -> Filter for blocking configured CAN IDs
};

class C_OscComMessageLogger
{
public:
   C_OscComMessageLogger(void);
   virtual ~C_OscComMessageLogger(void);

   virtual void Continue(void);
   virtual void Pause(void);
   virtual void Stop(void);
   virtual void Start(void);

   // openSYDE system definition handling
   virtual void SetProtocol(const stw::cmon_protocol::e_CanMonL7Protocols oe_Protocol);
   int32_t AddOsySysDef(const stw::scl::C_SclString & orc_PathSystemDefinition,
                        std::vector<C_OscSystemBus> & orc_Buses);
   int32_t AddOsySysDef(const stw::scl::C_SclString & orc_PathSystemDefinition, const uint32_t ou32_BusIndex,
                        std::vector<C_OscSystemBus> & orc_Buses);
   virtual int32_t SetOsySysDefBus(const stw::scl::C_SclString & orc_PathSystemDefinition,
                                   const uint32_t ou32_BusIndex);
   virtual int32_t GetOsySysDef(const stw::scl::C_SclString & orc_PathSystemDefinition,
                                C_OscComMessageLoggerOsySysDefConfig & orc_SystemDefinition);

   // Generic database handling
   virtual int32_t RemoveDatabase(const stw::scl::C_SclString & orc_Path);
   virtual int32_t ActivateDatabase(const stw::scl::C_SclString & orc_Path, const bool oq_Active);

   // Logging handling
   virtual int32_t AddLogFileAsc(const stw::scl::C_SclString & orc_FilePath, const bool oq_HexActive,
                                 const bool oq_RelativeTimeStampActive);
   virtual int32_t RemoveLogFile(const stw::scl::C_SclString & orc_FilePath);
   virtual void RemoveAllLogFiles(void);

   // Filter handling
   virtual void AddFilter(const C_OscComMessageLoggerFilter & orc_Filter);
   virtual void RemoveFilter(const C_OscComMessageLoggerFilter & orc_Filter);
   virtual void RemoveAllFilter(void);
   virtual uint32_t GetFilteredMessages(void) const;

   // CAN bus handling
   virtual int32_t HandleCanMessage(const stw::can::T_STWCAN_Msg_RX & orc_Msg, const bool oq_IsTx);
   virtual void ResetCounter(void);
   virtual void UpdateBusLoad(const uint8_t ou8_BusLoad);
   virtual void UpdateTxErrors(const uint32_t ou32_TxErrors);
   virtual void UpdateTxCounter(const uint32_t ou32_TxCount);

protected:
   const C_OscComMessageLoggerData & m_GetHandledCanMessage(void) const;

   virtual bool m_CheckFilter(const stw::can::T_STWCAN_Msg_RX & orc_Msg);

   // CANmon based protocol interpretation
   stw::scl::C_SclString m_GetProtocolStringHex(const stw::can::T_STWCAN_Msg_RX & orc_Msg) const;
   stw::scl::C_SclString m_GetProtocolStringDec(const stw::can::T_STWCAN_Msg_RX & orc_Msg) const;

   // openSYDE system definition based interpretation
   virtual void m_InsertOsySysDef(const stw::scl::C_SclString & orc_PathSystemDefinition,
                                  const C_OscSystemDefinition & orc_OsySysDef, const uint32_t ou32_BusIndex);
   virtual bool m_CheckSysDef(const stw::can::T_STWCAN_Msg_RX & orc_Msg);
   virtual bool m_InterpretSysDef(stw::opensyde_core::C_OscComMessageLoggerData & orc_MessageData) const;
   stw::scl::C_SclString m_GetOsySysDefStringHex(void) const;
   stw::scl::C_SclString m_GetOsySysDefStringDec(void) const;

   // Functions for integrating other CAN message interpretations
   virtual bool m_CheckInterpretation(C_OscComMessageLoggerData & orc_MessageData);

   static void mh_InterpretCanSignalValue(stw::opensyde_core::C_OscComMessageLoggerDataSignal & orc_Signal,
                                          const uint8_t(&orau8_CanDb)[8], const uint8_t ou8_CanDlc,
                                          const stw::opensyde_core::C_OscCanSignal & orc_OscSignal,
                                          const stw::opensyde_core::C_OscNodeDataPoolContent & orc_OscValue,
                                          const float64_t of64_Factor, const float64_t of64_Offset);

   virtual stw::scl::C_SclString m_GetProtocolStringHexHook(void) const;
   virtual stw::scl::C_SclString m_GetProtocolStringDecHook(void) const;

   // Generic database (for example openSYDE system definitions) activation flag register
   std::map<stw::scl::C_SclString, bool> mc_DatabaseActiveFlags;

   const C_OscCanMessage * mpc_OsySysDefMessage;
   const C_OscNodeDataPoolList * mpc_OsySysDefDataPoolList;

   stw::cmon_protocol::e_CanMonL7Protocols me_Protocol;
   bool mq_Paused;

   // Logging
   std::map<stw::scl::C_SclString, C_OscComMessageLoggerFileBase * const> mc_LoggingFiles;

private:
   //Avoid call
   C_OscComMessageLogger(const C_OscComMessageLogger &);
   C_OscComMessageLogger & operator =(const C_OscComMessageLogger &);

   void m_ConvertCanMessage(const stw::can::T_STWCAN_Msg_RX & orc_Msg, const bool oq_IsTx);
   void m_InterpretSysDefCanSignal(C_OscComMessageLoggerData & orc_MessageData,
                                   const C_OscCanSignal & orc_OscSignal) const;
   void m_ResetCounter(void);
   static void mh_PostProcessSysDef(stw::opensyde_core::C_OscSystemDefinition & orc_SystemDefinition);
   static void mh_AddSpecialEcesSignals(C_OscNode & orc_Node, const C_OscCanMessageIdentificationIndices & orc_Id,
                                        const uint32_t ou32_SignalIndex);

   C_OscComMessageLoggerData mc_HandledCanMessage;
   uint64_t mu64_FirstTimeStampStart;
   uint64_t mu64_FirstTimeStampDayOfTime;
   uint64_t mu64_LastTimeStamp;

   static const uint64_t mhu64_MAX_TIMESTAMP_DAY_OF_TIME;

   C_OscComLoggerProtocols mc_ProtocolHex;
   C_OscComLoggerProtocols mc_ProtocolDec;

   // Database interpretation
   std::map<stw::scl::C_SclString, C_OscComMessageLoggerOsySysDefConfig> mc_OsySysDefs;

   // Filtering
   std::vector<C_OscComMessageLoggerFilter> mc_CanFilterConfig;
   uint32_t mu32_FilteredMessages; //number of messages that did not pass the filter

   // Message counting
   std::vector<uint32_t> mc_MsgCounterStandardId;
   std::map<uint32_t, uint32_t> mc_MsgCounterExtendedId;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
