//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Translate L2 CAN message to L7 interpretation

   Translate L2 CAN message to openSYDE protocol L7 interpretation

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef CCMONPROTOCOLOPENSYDEH
#define CCMONPROTOCOLOPENSYDEH

/* -- Includes ------------------------------------------------------------- */

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

/* -- Namespace ------------------------------------------------------------ */
namespace stw_cmon_protocol
{

/* -- Global Constants ----------------------------------------------------- */

/* -- Types ---------------------------------------------------------------- */
///Interpretation of openSYDE protocol
class CMONPROTOCOL_PACKAGE C_CMONProtocolOpenSYDE : public C_CMONProtocolBase
{
protected:
   struct T_NodeIdentifier
   {
      stw_types::uint8 u8_Subnet;
      stw_types::uint8 u8_NodeId;
   };

   struct T_CanAddressInformation     ///< disassembled CAN identifier
   {
      T_NodeIdentifier t_NodeIdSource; ///< local CAN address and subnet
      T_NodeIdentifier t_NodeIdTarget; ///< target CAN address and subnet
      stw_types::uint8 u8_AddressMode; ///< fixed addressing mode physical/functional
      stw_types::uint8 u8_RoutingMode; ///< routing configured or not
   };

   virtual stw_scl::C_SCLString m_AddressInformationToText(
      const T_CanAddressInformation & ort_CanAddressInformation) const;
   virtual stw_scl::C_SCLString m_DataPoolIdentifierToText(const stw_types::uint32 ou32_DataPoolIdentifier,
      const bool oq_IsResponse,
      const T_CanAddressInformation & ort_CanAddressInformation) const;

private:
   static stw_types::sint32 mh_DisassembleCanId(const stw_types::uint32 ou32_CanId,
                                                T_CanAddressInformation & or_CanAddressInformation);

   static stw_scl::C_SCLString mh_ThreeByteVersionToString(const stw_types::uint8 * const opu8_Version);
   stw_scl::C_SCLString m_RawDataToString(const stw_types::uint8 ou8_NumBytes,
                                                 const stw_types::uint8 * const opu8_Data) const;
   stw_scl::C_SCLString m_GetSupposedlyAsciiText(const stw_types::uint8 ou8_NumBytes,
                                                 const stw_types::uint8 * const opu8_Data) const;
   stw_scl::C_SCLString m_ServiceIdToText(const stw_types::uint8 ou8_ServiceData,
                                          const bool oq_IsNegativeResponse = false) const;
   stw_scl::C_SCLString m_SessionIdToText(const stw_types::uint8 ou8_SessionId) const;
   stw_scl::C_SCLString m_ServiceDataToText(const stw_types::uint8 * const opu8_ServiceData,
                                            const stw_types::uint8 ou8_ServiceSize,
                                            const T_CanAddressInformation & ort_CanAddressInformation) const;
   stw_scl::C_SCLString m_NegativeResponseCodeToText(const stw_types::uint8 ou8_NrCode) const;
   stw_scl::C_SCLString m_DataIdentifierAndDataToText(const stw_types::uint16 ou16_DataIdentifier,
                                                      const bool oq_IsResponse,
                                                      const stw_types::uint8 ou8_PayloadSize,
                                                      const stw_types::uint8 * const opu8_Payload) const;
   stw_scl::C_SCLString m_RoutineDataToText(const stw_types::uint16 ou16_RoutineIdentifier,
                                            const bool oq_IsResponse,
                                            const stw_types::uint8 ou8_DataSize,
                                            const stw_types::uint8 * const opu8_Data) const;

public:
   C_CMONProtocolOpenSYDE(void);

   virtual stw_scl::C_SCLString MessageToString(const stw_can::T_STWCAN_Msg_RX & orc_Msg) const;
   virtual stw_scl::C_SCLString GetProtocolName(void) const;

};

/* -- Extern Global Variables ---------------------------------------------- */

} //end of namespace

#endif
