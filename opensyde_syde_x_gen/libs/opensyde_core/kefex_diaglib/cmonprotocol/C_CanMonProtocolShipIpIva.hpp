//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Translate L2 CAN message to L7 interpretation

   Translate L2 CAN message to SHIP-IP! resp. IVA protocol L7 interpretation

   \copyright   Copyright 2009 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef CCMONPROTOCOLSHIPIPIVAHPP
#define CCMONPROTOCOLSHIPIPIVAHPP

#include "stwtypes.hpp"
#include "C_CanMonProtocolBase.hpp"

//----------------------------------------------------------------------------------------------------------------------

namespace stw
{
namespace cmon_protocol
{
//----------------------------------------------------------------------------------------------------------------------

///class for descriptions of SHIP-IP! services, only used internally
class C_CanMonProtocolSipPayload
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
class C_CanMonProtocolSipAspDescription
{
public:
   //index is implicit as they start from zero !
   stw::scl::C_SclString c_ShortName;
   uint16_t u16_Length; ///< total size of service in bytes

   stw::scl::C_SclDynamicArray<C_CanMonProtocolSipPayload> c_Payload;

   void Set(const stw::scl::C_SclString & orc_ShortName, const uint16_t ou16_Length);
};

//----------------------------------------------------------------------------------------------------------------------

///class for descriptions of SHIP-IP! services, only used internally
class C_CanMonProtocolSipAsnDescription
{
public:
   //index is implicit as they start from zero !
   stw::scl::C_SclString c_ShortName;
   stw::scl::C_SclString c_AspPrefix;                                 ///< if c_ASPs.GetLength() == 0: meaning of ASP
                                                                      // (e.g. "CH" or "IDX")
   uint16_t u16_Length;                                               ///< if c_ASPs.GetLength() == 0: expected size of
                                                                      // APAY
   stw::scl::C_SclDynamicArray<C_CanMonProtocolSipPayload> c_Payload; ///< if c_ASPs.GetLength() == 0
   stw::scl::C_SclDynamicArray<C_CanMonProtocolSipAspDescription> c_Asps;

   void Set(const stw::scl::C_SclString & orc_ShortName, const uint8_t ou8_NumAsps,
            const stw::scl::C_SclString & orc_AspPrefix = "", const uint16_t ou16_Length = 0);
};

//----------------------------------------------------------------------------------------------------------------------

///class for descriptions of SHIP-IP! services, only used internally
class C_CanMonProtocolSipAsaDescription
{
public:
   //index is implicit as they start from zero !
   stw::scl::C_SclString c_ShortName;
   stw::scl::C_SclDynamicArray<C_CanMonProtocolSipAsnDescription> c_Asns;
};

//----------------------------------------------------------------------------------------------------------------------

///Interpretation of SHIP-IP!/IVA protocol:
class C_CanMonProtocolShipIpIva :
   public C_CanMonProtocolKefexIva
{
private:
   static bool hmq_ServiceTableInitialized;
   static void hm_InitServiceTable(void);
   static stw::scl::C_SclDynamicArray<C_CanMonProtocolSipAsaDescription> hmc_Services;

   stw::scl::C_SclString m_SipDecodeData(const stw::scl::C_SclDynamicArray<C_CanMonProtocolSipPayload> & orc_Payload,
                                         const uint8_t ou8_NumBytesInPayload, const uint8_t * const opu8_Payload) const;

   stw::scl::C_SclString m_MessageToString11Bit(const stw::can::T_STWCAN_Msg_RX & orc_Msg) const;
   stw::scl::C_SclString m_IvaGetErrorFromCode(const uint16_t ou16_ErrorCode) const;
   stw::scl::C_SclString m_IvaServiceIndexToString(const uint16_t ou16_ServiceIndex) const;

   stw::scl::C_SclString m_SipMessageToString(const stw::can::T_STWCAN_Msg_RX & orc_Msg) const;
   stw::scl::C_SclString m_Sip11MessageToString(const stw::can::T_STWCAN_Msg_RX & orc_Msg) const;
   stw::scl::C_SclString m_SipDecodeSfFfReadWriteHeader(const uint8_t ou8_Asa, const uint8_t ou8_Asn,
                                                        const uint8_t ou8_Asp,
                                                        stw::scl::C_SclDynamicArray<C_CanMonProtocolSipPayload> ** const oppc_Payload)
   const;
   stw::scl::C_SclString m_SipGetTpErrorFromCode(const uint8_t ou8_ErrorCode) const;
   stw::scl::C_SclString m_SipGetAppErrorFromCode(const uint8_t ou8_ErrorCode) const;
   stw::scl::C_SclString m_SipGetAccessType(const uint8_t ou8_AccessType) const;
   stw::scl::C_SclString m_SipGetApplData(const uint8_t ou8_Byte1, const uint8_t ou8_Byte2,
                                          const uint8_t * const opu8_FollowingBytes,
                                          const uint8_t ou8_NumBytesInThisFrame, const bool oq_IsRequest) const;
   stw::scl::C_SclString m_SipGetMemoryRwTypeAndIndex(const uint8_t ou8_MemType, const uint8_t ou8_MemIndex) const;
   stw::scl::C_SclString m_SipGetOdRwTypeAndIndex(const uint8_t ou8_OdType, const uint16_t ou16_VariableIndex) const;

public:
   virtual stw::scl::C_SclString MessageToString(const stw::can::T_STWCAN_Msg_RX & orc_Msg) const;
   virtual stw::scl::C_SclString GetProtocolName(void) const;
};

//----------------------------------------------------------------------------------------------------------------------
}
}

#endif
