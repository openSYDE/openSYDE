//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Translate L2 CAN message to L7 interpretation

   Translate L2 CAN message to KEFEX protocol L7 interpretation

   \copyright   Copyright 2003 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef CCMONPROTOCOLKEFEXHPP
#define CCMONPROTOCOLKEFEXHPP

#include "stwtypes.hpp"
#include "C_CanMonProtocolBase.hpp"
#include "C_SclString.hpp"

//----------------------------------------------------------------------------------------------------------------------

namespace stw
{
namespace cmon_protocol
{
//----------------------------------------------------------------------------------------------------------------------
///Interpretation of KEFEX protocol
class C_CanMonProtocolKefex :
   public C_CanMonProtocolKefexIva
{
private:
   uint16_t mu16_KfxBaseID;

   stw::scl::C_SclString m_MessageToStringKefex(const stw::can::T_STWCAN_Msg_RX & orc_Msg);
   stw::scl::C_SclString m_KfxIndexAndErrorToString(const char_t * const opcn_Text, const uint16_t ou16_Index,
                                                    const uint16_t ou16_Error,
                                                    const bool oq_IsKefexVarIndex = true) const;
   stw::scl::C_SclString m_KfxTextAndValueToString(const char_t * const opcn_Text, const uint32_t ou32_Value) const;

public:
   C_CanMonProtocolKefex(void);

   virtual stw::scl::C_SclString MessageToString(const stw::can::T_STWCAN_Msg_RX & orc_Msg) const;
   virtual stw::scl::C_SclString GetProtocolName(void) const;

   virtual int32_t SaveParamsToIni(stw::scl::C_SclIniFile & orc_IniFile, const stw::scl::C_SclString & orc_Section);
   virtual int32_t LoadParamsFromIni(stw::scl::C_SclIniFile & orc_IniFile, const stw::scl::C_SclString & orc_Section);

   uint16_t GetBaseId(void) const;
   void SetBaseId(const uint16_t ou16_BaseId);
};

//----------------------------------------------------------------------------------------------------------------------
}
}

#endif
