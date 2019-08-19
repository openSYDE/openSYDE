//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Translate L2 CAN message to L7 interpretation

   Translate L2 CAN message to CANopen protocol L7 interpretation

   \copyright   Copyright 2006 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef CCMONProtocolCANOPENH
#define CCMONProtocolCANOPENH

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

///CANopen protocol converter
class CMONPROTOCOL_PACKAGE C_CMONProtocolCANopen : public C_CMONProtocolBase
{
public:
   virtual stw_scl::C_SCLString MessageToString(const stw_can::T_STWCAN_Msg_RX & orc_Msg) const;
   virtual stw_scl::C_SCLString GetProtocolName(void) const;
};
//----------------------------------------------------------------------------------------------------------------------

}

#endif
