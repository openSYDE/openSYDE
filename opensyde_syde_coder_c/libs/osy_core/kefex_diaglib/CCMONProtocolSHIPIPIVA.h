//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Translate L2 CAN message to L7 interpretation

   Translate L2 CAN message to SHIP-IP! resp. IVA protocol L7 interpretation

   \copyright   Copyright 2009 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef CCMONProtocolSHIPIPIVAH
#define CCMONProtocolSHIPIPIVAH

#include "stwtypes.h"
#include "CCMONProtocolBase.h"

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

///class for descriptions of SHIP-IP! services, only used internally
class C_CMONProtocolSIPPayload
{
public:
   stw_scl::C_SCLString c_Name; ///< payload description (should include unit)
   bool q_Signed;               ///< signed / unsigned
   stw_types::uint8 u8_Size;    ///< size of value in bytes

   void Set(const stw_scl::C_SCLString & orc_Name, const stw_types::uint8 ou8_Size, const bool oq_Signed);

   //for more complex stuff we could also add a pointer to a "decode" function returning the value as a string
   //e.g. return "RAM" or "EEPROM" for memory test types instead of bland numbers
   stw_scl::C_SCLString Decode(const stw_types::uint8 * const opu8_Data, const bool oq_Decimal) const;
};

//---------------------------------------------------------------------------

///class for descriptions of SHIP-IP! services, only used internally
class C_CMONProtocolSIPASPDescription
{
public:
   //index is implicit as they start from zero !
   stw_scl::C_SCLString c_ShortName;
   stw_types::uint16 u16_Length;  ///< total size of service in bytes

   stw_scl::SCLDynamicArray<C_CMONProtocolSIPPayload> c_Payload;

   void Set(const stw_scl::C_SCLString & orc_ShortName, const stw_types::uint16 ou16_Length);
};

//---------------------------------------------------------------------------

///class for descriptions of SHIP-IP! services, only used internally
class C_CMONProtocolSIPASNDescription
{
public:
   //index is implicit as they start from zero !
   stw_scl::C_SCLString c_ShortName;
   stw_scl::C_SCLString c_ASPPrefix;   ///< if c_ASPs.GetLength() == 0: meaning of ASP (e.g. "CH" or "IDX")
   stw_types::uint16 u16_Length;       ///< if c_ASPs.GetLength() == 0: expected size of APAY
   stw_scl::SCLDynamicArray<C_CMONProtocolSIPPayload> c_Payload; //if c_ASPs.GetLength() == 0

   stw_scl::SCLDynamicArray<C_CMONProtocolSIPASPDescription> c_ASPs;

   void Set(const stw_scl::C_SCLString & orc_ShortName, const stw_types::uint8 ou8_NumASPs,
            const stw_scl::C_SCLString & orc_ASPPrefix = "", const stw_types::uint16 ou16_Length = 0);
};

//---------------------------------------------------------------------------

///class for descriptions of SHIP-IP! services, only used internally
class C_CMONProtocolSIPASADescription
{
public:
   //index is implicit as they start from zero !
   stw_scl::C_SCLString c_ShortName;
   stw_scl::SCLDynamicArray<C_CMONProtocolSIPASNDescription> c_ASNs;
};

//---------------------------------------------------------------------------

///Interpretation of SHIP-IP!/IVA protocol:
class CMONPROTOCOL_PACKAGE C_CMONProtocolSHIP_IP_IVA : public C_CMONProtocolKEFEX_IVA
{
private:
   static bool hmq_ServiceTableInitialized;
   static void hm_InitServiceTable(void);
   static stw_scl::SCLDynamicArray<C_CMONProtocolSIPASADescription> hmc_Services;

   stw_scl::C_SCLString m_SIPDecodeData(const stw_scl::SCLDynamicArray<C_CMONProtocolSIPPayload> & orc_Payload,
                                        const stw_types::uint8 ou8_NumBytesInPayload,
                                        const stw_types::uint8 * const opu8_Payload) const;

   stw_scl::C_SCLString m_MessageToString11Bit(const stw_can::T_STWCAN_Msg_RX & orc_Msg) const;
   stw_scl::C_SCLString m_IVAGetErrorFromCode(const stw_types::uint16 ou16_ErrorCode) const;
   stw_scl::C_SCLString m_IVAServiceIndexToString(const stw_types::uint16 ou16_ServiceIndex) const;

   stw_scl::C_SCLString m_SIPMessageToString(const stw_can::T_STWCAN_Msg_RX & orc_Msg) const;
   stw_scl::C_SCLString m_SIP11MessageToString(const stw_can::T_STWCAN_Msg_RX & orc_Msg) const;
   stw_scl::C_SCLString m_SIPDecodeSFFFReadWriteHeader(const stw_types::uint8 ou8_ASA, const stw_types::uint8 ou8_ASN,
                                                       const stw_types::uint8 ou8_ASP,
                                                stw_scl::SCLDynamicArray<C_CMONProtocolSIPPayload> ** const oppc_Payload
                                                       ) const;
   stw_scl::C_SCLString m_SIPGetTPErrorFromCode(const stw_types::uint8 ou8_ErrorCode) const;
   stw_scl::C_SCLString m_SIPGetAppErrorFromCode(const stw_types::uint8 ou8_ErrorCode) const;
   stw_scl::C_SCLString m_SIPGetAccessType(const stw_types::uint8 ou8_AccessType) const;
   stw_scl::C_SCLString m_SIPGetApplData(const stw_types::uint8 ou8_Byte1, const stw_types::uint8 ou8_Byte2,
                                         const stw_types::uint8 * const opu8_FollowingBytes,
                                         const stw_types::uint8 ou8_NumBytesInThisFrame,
                                         const bool oq_IsRequest) const;
   stw_scl::C_SCLString m_SIPGetMemoryRWTypeAndIndex(const stw_types::uint8 ou8_MemType,
                                                     const stw_types::uint8 ou8_MemIndex) const;
   stw_scl::C_SCLString m_SIPGetODRWTypeAndIndex(const stw_types::uint8 ou8_ODType,
                                                 const stw_types::uint16 ou16_VariableIndex) const;

public:
   virtual stw_scl::C_SCLString MessageToString(const stw_can::T_STWCAN_Msg_RX & orc_Msg) const;
   virtual stw_scl::C_SCLString GetProtocolName(void) const;
};

//---------------------------------------------------------------------------

}

#endif
