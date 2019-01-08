//-----------------------------------------------------------------------------
/*!
   \file
   \brief       Class for logging CAN messages (header)

   See cpp file for detailed description

   \implementation
   project     openSYDE
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     03.09.2018  STW/B.Bayer
   \endimplementation
*/
//-----------------------------------------------------------------------------
#ifndef C_OSCCOMMESSAGELOGGER_H
#define C_OSCCOMMESSAGELOGGER_H

/* -- Includes ------------------------------------------------------------- */
#include <vector>
#include <map>

#include "stwtypes.h"
#include "CSCLString.h"
#include "stw_can.h"

#include "C_OSCComLoggerProtocols.h"
#include "C_OSCSystemDefinition.h"
#include "C_OSCComMessageLoggerOsySysDefConfig.h"

/* -- Namespace ------------------------------------------------------------ */
namespace stw_opensyde_core
{
/* -- Global Constants ----------------------------------------------------- */

/* -- Types ---------------------------------------------------------------- */

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

   void Continue(void);
   void Pause(void);
   virtual void Stop(void);

   virtual void SetProtocol(const stw_cmon_protocol::e_CMONL7Protocols oe_Protocol);
   stw_types::sint32 AddOsySysDef(const stw_scl::C_SCLString & orc_PathSystemDefinition,
                                  const stw_scl::C_SCLString & orc_PathDeviceDefinitions,
                                  std::vector<C_OSCSystemBus> & orc_Busses);
   stw_types::sint32 AddOsySysDef(const stw_scl::C_SCLString & orc_PathSystemDefinition,
                                  const stw_scl::C_SCLString & orc_PathDeviceDefinitions,
                                  const stw_types::uint32 ou32_BusIndex);
   virtual stw_types::sint32 SetOsySysDefBus(const stw_scl::C_SCLString & orc_PathSystemDefinition,
                                             const stw_types::uint32 ou32_BusIndex);
   virtual stw_types::sint32 RemoveOsySysDef(const stw_scl::C_SCLString & orc_PathSystemDefinition);

   virtual void AddFilter(const C_OSCComMessageLoggerFilter & orc_Filter);
   virtual void RemoveFilter(const C_OSCComMessageLoggerFilter & orc_Filter);

   virtual stw_types::sint32 HandleCanMessage(const stw_can::T_STWCAN_Msg_RX & orc_Msg, const bool oq_IsTx);

protected:
   virtual bool m_CheckFilter(const stw_can::T_STWCAN_Msg_RX & orc_Msg) const;

   // CANmon based protocol interpretation
   stw_scl::C_SCLString m_GetProtocolStringHex(const stw_can::T_STWCAN_Msg_RX & orc_Msg) const;
   stw_scl::C_SCLString m_GetProtocolStringDec(const stw_can::T_STWCAN_Msg_RX & orc_Msg) const;

   // openSYDE system definition based interpretation
   virtual void m_InsertOsySysDef(const stw_scl::C_SCLString & orc_PathSystemDefinition,
                                  const C_OSCSystemDefinition & orc_OsySysDef, const stw_types::uint32 ou32_BusIndex);
   virtual bool m_CheckSysDef(const stw_can::T_STWCAN_Msg_RX & orc_Msg);
   stw_scl::C_SCLString m_GetOsySysDefStringHex(void) const;
   stw_scl::C_SCLString m_GetOsySysDefStringDec(void) const;
   stw_scl::C_SCLString m_GetTimestampAsString(const stw_types::uint64 ou64_TimeStamp) const;

   // Functions for integrating other CAN message interpretations
   virtual void m_CheckInterpretation(const stw_can::T_STWCAN_Msg_RX & orc_Msg);
   virtual stw_scl::C_SCLString m_GetProtocolStringHexHook(void) const;
   virtual stw_scl::C_SCLString m_GetProtocolStringDecHook(void) const;

   const C_OSCCanMessage * mpc_OsySysDefMessage;
   const C_OSCNodeDataPoolList * mpc_OsySysDefDataPoolList;

   stw_cmon_protocol::e_CMONL7Protocols me_Protocol;
   bool mq_Paused;

private:
   //Avoid call
   C_OSCComMessageLogger(const C_OSCComMessageLogger &);
   C_OSCComMessageLogger & operator =(const C_OSCComMessageLogger &);

   C_OSCComLoggerProtocols mc_ProtocolHex;
   C_OSCComLoggerProtocols mc_ProtocolDec;
   std::vector<C_OSCComMessageLoggerFilter> mc_CanFilterConfig;

   std::map<stw_scl::C_SCLString, C_OSCComMessageLoggerOsySysDefConfig> mc_OsySysDefs;
};

/* -- Extern Global Variables ---------------------------------------------- */
} //end of namespace

#endif
