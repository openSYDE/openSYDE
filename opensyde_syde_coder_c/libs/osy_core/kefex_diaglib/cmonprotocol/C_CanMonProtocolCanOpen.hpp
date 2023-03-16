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

///CANopen protocol converter
class CMONPROTOCOL_PACKAGE C_CanMonProtocolCanOpen :
   public C_CanMonProtocolBase
{
public:
   virtual stw::scl::C_SclString MessageToString(const stw::can::T_STWCAN_Msg_RX & orc_Msg) const;
   virtual stw::scl::C_SclString GetProtocolName(void) const;
};

//----------------------------------------------------------------------------------------------------------------------
}
}
#endif
