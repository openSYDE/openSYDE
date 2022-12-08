//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Translate L2 CAN message to L7 interpretation

   Translate L2 CAN message to STW flashloader protocol L7 interpretation

   \copyright   Copyright 2003 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef CCMONProtocolXFLH
#define CCMONProtocolXFLH

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

///Interpretation of STW Flashloader protocol
class CMONPROTOCOL_PACKAGE C_CanMonProtocolXfl :
   public C_CanMonProtocolBase
{
private:
   uint32_t mu32_XFLSendID;

public:
   C_CanMonProtocolXfl(void);

   virtual stw::scl::C_SclString MessageToString(const stw::can::T_STWCAN_Msg_RX & orc_Msg) const;
   virtual stw::scl::C_SclString GetProtocolName(void) const;

   virtual int32_t SaveParamsToIni(stw::scl::C_SclIniFile & orc_IniFile, const stw::scl::C_SclString & orc_Section);
   virtual int32_t LoadParamsFromIni(stw::scl::C_SclIniFile & orc_IniFile, const stw::scl::C_SclString & orc_Section);

   uint32_t GetSendID(void) const;
   void SetSendID(const uint32_t ou32_Send);
};

//----------------------------------------------------------------------------------------------------------------------
}
}

#endif
