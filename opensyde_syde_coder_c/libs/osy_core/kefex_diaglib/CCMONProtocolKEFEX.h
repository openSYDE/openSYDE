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

//---------------------------------------------------------------------------

namespace stw_cmon_protocol
{

//---------------------------------------------------------------------------
///Interpretation of KEFEX protocol
class CMONPROTOCOL_PACKAGE C_CMONProtocolKEFEX : public C_CMONProtocolKEFEX_IVA
{
private:
   stw_types::uint16 mu16_KFXBaseID;

   stw_scl::C_SCLString m_MessageToStringKEFEX(const stw_can::T_STWCAN_Msg_RX & orc_Msg);
   stw_scl::C_SCLString m_KFXIndexAndErrorToString(const stw_types::charn * const opcn_Text,
                                                   const stw_types::uint16 ou16_Index,
                                                   const stw_types::uint16 ou16_Error,
                                                   const bool oq_IsKEFEXVarIndex = true) const;
   stw_scl::C_SCLString m_KFXTextAndValueToString(const stw_types::charn * const opcn_Text,
                                                    const stw_types::uint32 ou32_Value) const;

public:
   C_CMONProtocolKEFEX(void);

   virtual stw_scl::C_SCLString MessageToString(const stw_can::T_STWCAN_Msg_RX & orc_Msg) const;
   virtual stw_scl::C_SCLString GetProtocolName(void) const;

   virtual stw_types::sint32 SaveParamsToIni(stw_scl::C_SCLIniFile & orc_IniFile,
                                             const stw_scl::C_SCLString & orc_Section);
   virtual stw_types::sint32 LoadParamsFromIni(stw_scl::C_SCLIniFile & orc_IniFile,
                                               const stw_scl::C_SCLString & orc_Section);

   stw_types::uint16 GetBaseID(void) const;
   void SetBaseID(const stw_types::uint16 ou16_BaseID);
};

//---------------------------------------------------------------------------

}

#endif
