//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Translate L2 CAN message to textual interpretation

   \copyright   Copyright 2002 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef CCMONProtocolBASEH
#define CCMONProtocolBASEH

#include "stwtypes.h"
#include "CCMONProtocolTarget.h"
#include "CSCLString.h"
#include "CSCLIniFile.h"

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

//Base class for all protocol interpretations
//Provides basic functions to help converting a CAN message structure into text.
//Also provides an abstract interface for protocol-specific conversions.
class CMONPROTOCOL_PACKAGE C_CMONProtocolBase
{
protected:
   bool mq_Decimal;

   //general:
   stw_scl::C_SCLString m_GetValueDecHex(const stw_types::uint32 ou32_Value) const;
   stw_scl::C_SCLString m_GetByteAsStringFormat(const stw_types::uint8 ou8_Value) const;
   stw_scl::C_SCLString m_GetWordAsStringFormat(const stw_types::uint16 ou16_Value) const;
   static stw_types::uint32 m_BytesToDwordLowHigh(const stw_types::uint8 oau8_Bytes[4]);
   static stw_types::uint16 m_BytesToWordLowHigh(const stw_types::uint8 oau8_Bytes[2]);
   static stw_types::uint16 m_BytesToWordHighLow(const stw_types::uint8 oau8_Bytes[2]);
   static stw_types::uint32 m_BytesToDwordHighLow(const stw_types::uint8 oau8_Bytes[4]);
   static stw_scl::C_SCLString mh_SerialNumberToString(const stw_types::uint8 * const opu8_SerialNumber);

public:
   C_CMONProtocolBase(void);
   virtual ~C_CMONProtocolBase(void);

   ///actual conversion routine to be overloaded by inheriting class
   virtual stw_scl::C_SCLString MessageToString(const stw_can::T_STWCAN_Msg_RX & orc_Msg) const = 0;
   ///return string representation of protocol name
   virtual stw_scl::C_SCLString GetProtocolName(void) const = 0;

   virtual void SetDecimal(const bool oq_Decimal);

   //overload if we have something to save (use protocol abbreviation as prefix for directives !)
   virtual stw_types::sint32 SaveParamsToIni(stw_scl::C_SCLIniFile & orc_IniFile,
                                             const stw_scl::C_SCLString & orc_Section);
   virtual stw_types::sint32 LoadParamsFromIni(stw_scl::C_SCLIniFile & orc_IniFile,
                                               const stw_scl::C_SCLString & orc_Section);
};

//----------------------------------------------------------------------------------------------------------------------
//Common base for KEFEX-style variable-based protocols
///Adds functionality to link in an opened KEFEX project in order to decode variable indexes from CAN messages.
class CMONPROTOCOL_PACKAGE C_CMONProtocolKEFEX_IVA :
   public C_CMONProtocolBase
{
private:
   C_CMONProtocolKEFEX_IVA(const C_CMONProtocolKEFEX_IVA & orc_Souce);               //not implemented -> prevent
                                                                                     // copying
   C_CMONProtocolKEFEX_IVA & operator = (const C_CMONProtocolKEFEX_IVA & orc_Souce); //not implemented -> prevent
                                                                                     // assignment

protected:
   stw_types::uint16 mu16_KFXListOffset;
#ifdef CMONPROTOCOL_ALLOW_RAMVIEW_PROJECT_MAPPING
   const stw_diag_lib::C_KFXVariableLists * mpc_KFXLists;
#endif

   //used by KFX and IVA:
   stw_scl::C_SCLString m_KFXIndexToString(const stw_types::uint16 ou16_Index, bool oq_IsKEFEXVarIndex = true) const;
   stw_scl::C_SCLString m_KFXTextAndIndexToString(const stw_types::charn * const opcn_Text,
                                                  const stw_types::uint16 ou16_Index) const;

public:
   C_CMONProtocolKEFEX_IVA(void);
#ifdef CMONPROTOCOL_ALLOW_RAMVIEW_PROJECT_MAPPING
   void SetVariableInfo(const stw_diag_lib::C_KFXVariableLists * const opc_Lists,
                        const stw_types::uint16 ou16_ListOffset);
#endif
};

//----------------------------------------------------------------------------------------------------------------------
}

#endif
