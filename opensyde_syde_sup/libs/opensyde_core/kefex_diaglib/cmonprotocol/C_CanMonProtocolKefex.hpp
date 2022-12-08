//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Translate L2 CAN message to L7 interpretation

   Translate L2 CAN message to KEFEX protocol L7 interpretation

   \copyright   Copyright 2003 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef CCMONProtocolKEFEXH
#define CCMONProtocolKEFEXH

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
///Interpretation of KEFEX protocol
class CMONPROTOCOL_PACKAGE C_CanMonProtocolKefex :
   public C_CMONProtocolKEFEX_IVA
{
private:
   uint16_t mu16_KFXBaseID;

   stw::scl::C_SclString m_MessageToStringKEFEX(const stw::can::T_STWCAN_Msg_RX & orc_Msg);
   stw::scl::C_SclString m_KFXIndexAndErrorToString(const char_t * const opcn_Text, const uint16_t ou16_Index,
                                                    const uint16_t ou16_Error,
                                                    const bool oq_IsKEFEXVarIndex = true) const;
   stw::scl::C_SclString m_KFXTextAndValueToString(const char_t * const opcn_Text, const uint32_t ou32_Value) const;

public:
   C_CanMonProtocolKefex(void);

   virtual stw::scl::C_SclString MessageToString(const stw::can::T_STWCAN_Msg_RX & orc_Msg) const;
   virtual stw::scl::C_SclString GetProtocolName(void) const;

   virtual int32_t SaveParamsToIni(stw::scl::C_SclIniFile & orc_IniFile, const stw::scl::C_SclString & orc_Section);
   virtual int32_t LoadParamsFromIni(stw::scl::C_SclIniFile & orc_IniFile, const stw::scl::C_SclString & orc_Section);

   uint16_t GetBaseID(void) const;
   void SetBaseID(const uint16_t ou16_BaseID);
};

//----------------------------------------------------------------------------------------------------------------------
}
}

#endif
