//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Translate L2 CAN message to L7 interpretation

   Translate L2 CAN message to STW flashloader protocol L7 interpretation

   \copyright   Copyright 2003 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef CCMONPROTOCOLXFLHPP
#define CCMONPROTOCOLXFLHPP

#include "stwtypes.hpp"
#include "C_CanMonProtocolBase.hpp"
#include "C_SclString.hpp"

//----------------------------------------------------------------------------------------------------------------------

namespace stw
{
namespace cmon_protocol
{
//----------------------------------------------------------------------------------------------------------------------

///Interpretation of STW Flashloader protocol
class C_CanMonProtocolXfl :
   public C_CanMonProtocolBase
{
private:
   uint32_t mu32_XFLSendId;

public:
   C_CanMonProtocolXfl(void);

   virtual stw::scl::C_SclString MessageToString(const stw::can::T_STWCAN_Msg_RX & orc_Msg) const;
   virtual stw::scl::C_SclString GetProtocolName(void) const;

   virtual int32_t SaveParamsToIni(stw::scl::C_SclIniFile & orc_IniFile, const stw::scl::C_SclString & orc_Section);
   virtual int32_t LoadParamsFromIni(stw::scl::C_SclIniFile & orc_IniFile, const stw::scl::C_SclString & orc_Section);

   uint32_t GetSendId(void) const;
   void SetSendId(const uint32_t ou32_SendId);
};

//----------------------------------------------------------------------------------------------------------------------
}
}

#endif
