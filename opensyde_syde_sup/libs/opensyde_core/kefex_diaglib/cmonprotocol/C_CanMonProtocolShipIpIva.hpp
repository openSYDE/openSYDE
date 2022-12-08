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

#include "stwtypes.hpp"
#include "C_CanMonProtocolBase.hpp"

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

///class for descriptions of SHIP-IP! services, only used internally
class C_CMONProtocolSIPPayload
{
public:
   stw::scl::C_SclString c_Name; ///< payload description (should include unit)
   bool q_Signed;                ///< signed / unsigned
   uint8_t u8_Size;              ///< size of value in bytes

   void Set(const stw::scl::C_SclString & orc_Name, const uint8_t ou8_Size, const bool oq_Signed);

   //for more complex stuff we could also add a pointer to a "decode" function returning the value as a string
   //e.g. return "RAM" or "EEPROM" for memory test types instead of bland numbers
   stw::scl::C_SclString Decode(const uint8_t * const opu8_Data, const bool oq_Decimal) const;
};

//----------------------------------------------------------------------------------------------------------------------

///class for descriptions of SHIP-IP! services, only used internally
class C_CMONProtocolSIPASPDescription
{
public:
   //index is implicit as they start from zero !
   stw::scl::C_SclString c_ShortName;
   uint16_t u16_Length; ///< total size of service in bytes

   stw::scl::C_SclDynamicArray<C_CMONProtocolSIPPayload> c_Payload;

   void Set(const stw::scl::C_SclString & orc_ShortName, const uint16_t ou16_Length);
};

//----------------------------------------------------------------------------------------------------------------------

///class for descriptions of SHIP-IP! services, only used internally
class C_CMONProtocolSIPASNDescription
{
public:
   //index is implicit as they start from zero !
   stw::scl::C_SclString c_ShortName;
   stw::scl::C_SclString c_ASPPrefix;                               ///< if c_ASPs.GetLength() == 0: meaning of ASP
                                                                    // (e.g. "CH" or "IDX")
   uint16_t u16_Length;                                             ///< if c_ASPs.GetLength() == 0: expected size of
                                                                    // APAY
   stw::scl::C_SclDynamicArray<C_CMONProtocolSIPPayload> c_Payload; //if c_ASPs.GetLength() == 0

   stw::scl::C_SclDynamicArray<C_CMONProtocolSIPASPDescription> c_ASPs;

   void Set(const stw::scl::C_SclString & orc_ShortName, const uint8_t ou8_NumASPs,
            const stw::scl::C_SclString & orc_ASPPrefix = "", const uint16_t ou16_Length = 0);
};

//----------------------------------------------------------------------------------------------------------------------

///class for descriptions of SHIP-IP! services, only used internally
class C_CMONProtocolSIPASADescription
{
public:
   //index is implicit as they start from zero !
   stw::scl::C_SclString c_ShortName;
   stw::scl::C_SclDynamicArray<C_CMONProtocolSIPASNDescription> c_ASNs;
};

//----------------------------------------------------------------------------------------------------------------------

///Interpretation of SHIP-IP!/IVA protocol:
class CMONPROTOCOL_PACKAGE C_CanMonProtocolShipIpIva :
   public C_CMONProtocolKEFEX_IVA
{
private:
   static bool hmq_ServiceTableInitialized;
   static void hm_InitServiceTable(void);
   static stw::scl::C_SclDynamicArray<C_CMONProtocolSIPASADescription> hmc_Services;

   stw::scl::C_SclString m_SIPDecodeData(const stw::scl::C_SclDynamicArray<C_CMONProtocolSIPPayload> & orc_Payload,
                                         const uint8_t ou8_NumBytesInPayload, const uint8_t * const opu8_Payload) const;

   stw::scl::C_SclString m_MessageToString11Bit(const stw::can::T_STWCAN_Msg_RX & orc_Msg) const;
   stw::scl::C_SclString m_IVAGetErrorFromCode(const uint16_t ou16_ErrorCode) const;
   stw::scl::C_SclString m_IVAServiceIndexToString(const uint16_t ou16_ServiceIndex) const;

   stw::scl::C_SclString m_SIPMessageToString(const stw::can::T_STWCAN_Msg_RX & orc_Msg) const;
   stw::scl::C_SclString m_SIP11MessageToString(const stw::can::T_STWCAN_Msg_RX & orc_Msg) const;
   stw::scl::C_SclString m_SIPDecodeSFFFReadWriteHeader(const uint8_t ou8_ASA, const uint8_t ou8_ASN,
                                                        const uint8_t ou8_ASP,
                                                        stw::scl::C_SclDynamicArray<C_CMONProtocolSIPPayload> ** const oppc_Payload)
   const;
   stw::scl::C_SclString m_SIPGetTPErrorFromCode(const uint8_t ou8_ErrorCode) const;
   stw::scl::C_SclString m_SIPGetAppErrorFromCode(const uint8_t ou8_ErrorCode) const;
   stw::scl::C_SclString m_SIPGetAccessType(const uint8_t ou8_AccessType) const;
   stw::scl::C_SclString m_SIPGetApplData(const uint8_t ou8_Byte1, const uint8_t ou8_Byte2,
                                          const uint8_t * const opu8_FollowingBytes,
                                          const uint8_t ou8_NumBytesInThisFrame, const bool oq_IsRequest) const;
   stw::scl::C_SclString m_SIPGetMemoryRWTypeAndIndex(const uint8_t ou8_MemType, const uint8_t ou8_MemIndex) const;
   stw::scl::C_SclString m_SIPGetODRWTypeAndIndex(const uint8_t ou8_ODType, const uint16_t ou16_VariableIndex) const;

public:
   virtual stw::scl::C_SclString MessageToString(const stw::can::T_STWCAN_Msg_RX & orc_Msg) const;
   virtual stw::scl::C_SclString GetProtocolName(void) const;
};

//----------------------------------------------------------------------------------------------------------------------
}
}

#endif
