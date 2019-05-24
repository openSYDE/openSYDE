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

#include "stwtypes.h"
#include "CCMONProtocolBase.h"
#include "CSCLString.h"

#ifndef CMONPROTOCOL_PACKAGE
#ifdef __BORLANDC__
#define CMONPROTOCOL_PACKAGE __declspec(package)
#else
#define CMONPROTOCOL_PACKAGE
#endif
#endif

//----------------------------------------------------------------------------------------------------------------------

namespace stw_cmon_protocol
{

//----------------------------------------------------------------------------------------------------------------------
///Interpretation of SAE J1939 protocol
class CMONPROTOCOL_PACKAGE C_CMONProtocolJ1939 : public C_CMONProtocolBase
{
public:
   C_CMONProtocolJ1939(void);
   virtual stw_scl::C_SCLString MessageToString(const stw_can::T_STWCAN_Msg_RX & orc_Msg) const;
   virtual stw_scl::C_SCLString GetProtocolName(void) const;
private:
   stw_scl::C_SCLString m_PgnToString(const stw_types::uint32 ou32_Pgn) const;
   stw_types::uint32 m_GetPgn(const stw_types::uint8 * const opu8_Data) const;
   stw_scl::C_SCLString m_GetMessageSize(const stw_can::T_STWCAN_Msg_RX & orc_Msg) const;
   stw_scl::C_SCLString m_GetData(const stw_can::T_STWCAN_Msg_RX & orc_Msg, const stw_types::uint8 ou8_StartIdx) const;
   stw_scl::C_SCLString m_GetName(const stw_can::T_STWCAN_Msg_RX & orc_Msg) const;
};

//----------------------------------------------------------------------------------------------------------------------

}

#endif
