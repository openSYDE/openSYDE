//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Translate L2 CAN message to L7 interpretation

   Translate L2 CAN message to J1939 protocol L7 interpretation

   \copyright   Copyright 2010 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef CCMONProtocolJ1939H
#define CCMONProtocolJ1939H

#include "stwtypes.hpp"
#include "C_CanMonProtocolBase.hpp"
#include "C_SclString.hpp"

#ifndef CMONPROTOCOL_PACKAGE
#ifdef __BORLANDC__
#define CMONPROTOCOL_PACKAGE __declspec(package)
#else
#define CMONPROTOCOL_PACKAGE
#endif
#endif

//----------------------------------------------------------------------------------------------------------------------

namespace stw
{
namespace cmon_protocol
{
//----------------------------------------------------------------------------------------------------------------------
///Interpretation of SAE J1939 protocol
class CMONPROTOCOL_PACKAGE C_CanMonProtocolJ1939 :
   public C_CanMonProtocolBase
{
public:
   C_CanMonProtocolJ1939(void);
   virtual stw::scl::C_SclString MessageToString(const stw::can::T_STWCAN_Msg_RX & orc_Msg) const;
   virtual stw::scl::C_SclString GetProtocolName(void) const;

private:
   stw::scl::C_SclString m_PgnToString(const uint32_t ou32_Pgn) const;
   uint32_t m_GetPgn(const uint8_t * const opu8_Data) const;
   stw::scl::C_SclString m_GetMessageSize(const stw::can::T_STWCAN_Msg_RX & orc_Msg) const;
   stw::scl::C_SclString m_GetData(const stw::can::T_STWCAN_Msg_RX & orc_Msg, const uint8_t ou8_StartIdx) const;
   stw::scl::C_SclString m_GetName(const stw::can::T_STWCAN_Msg_RX & orc_Msg) const;
};

//----------------------------------------------------------------------------------------------------------------------
}
}

#endif
