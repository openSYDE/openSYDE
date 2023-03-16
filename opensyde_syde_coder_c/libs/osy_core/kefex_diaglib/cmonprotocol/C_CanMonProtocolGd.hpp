//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Translate L2 CAN message to L7 interpretation

   Translate L2 CAN message to Generic Driver protocol L7 interpretation

   \copyright   Copyright 2007 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef CCMONProtocolGDH
#define CCMONProtocolGDH

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

///Interpretation of Generic Driver protocol
class CMONPROTOCOL_PACKAGE C_CanMonProtocolGd :
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
