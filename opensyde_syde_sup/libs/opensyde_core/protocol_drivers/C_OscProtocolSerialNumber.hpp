//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Representation of node serial number in all variations

   See cpp file for detailed description

   \copyright   Copyright 2021 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_OSCPROTOCOLSERIALNUMBER_H
#define C_OSCPROTOCOLSERIALNUMBER_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "stwtypes.hpp"
#include "C_SclString.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_core
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_OscProtocolSerialNumber
{
public:
   C_OscProtocolSerialNumber(void);

   bool operator == (const C_OscProtocolSerialNumber & orc_Cmp) const;
   bool operator <(const C_OscProtocolSerialNumber & orc_Cmp) const;

   void SetPosSerialNumber(const uint8_t (&orau8_SerialNumber)[6]);
   int32_t SetExtSerialNumber(const std::vector<uint8_t> & orc_SerialNumber,
                              const uint8_t ou8_SerialNumberManufacturerFormat);
   int32_t SetExtSerialNumber(const stw::scl::C_SclString & orc_SerialNumber,
                              const uint8_t ou8_SerialNumberManufacturerFormat);
   std::vector<uint8_t> GetSerialNumberAsRawData(void) const;
   stw::scl::C_SclString GetSerialNumberAsFormattedString(void) const;
   stw::scl::C_SclString GetSerialNumberAsPlainString(void) const;

   static bool h_SerialNumberFromStringToArray(const stw::scl::C_SclString & orc_SerialNumber,
                                               uint8_t(&orau8_SerialNumber)[6]);

   bool q_IsValid;         ///< flag if a valid serial number was already set
   bool q_ExtFormatUsed;   ///< flag which protocol communication was used. true for extended, false for standard
   bool q_FsnSerialNumber; ///< flag if serial number was set in FSN or POS format

   uint8_t u8_SerialNumberByteLength; ///< length of serial number

   // Value for a serial number in POS format
   uint8_t au8_SerialNumber[6]; ///< serial number of sender in POS format

   // Values used for a serial number in FSN format
   // Special case: u8_SerialNumberManufacturerFormat equals 0, the old STW standard format POS is used
   // despite of the used extended format.
   stw::scl::C_SclString c_SerialNumberExt;   ///< serial number of sender in FSN format
   uint8_t u8_SerialNumberManufacturerFormat; ///< defines the used manufacturer format for the sn
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
