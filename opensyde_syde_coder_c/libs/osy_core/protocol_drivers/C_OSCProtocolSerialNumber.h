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
#include "stwtypes.h"
#include "CSCLString.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw_opensyde_core
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_OSCProtocolSerialNumber
{
public:
   C_OSCProtocolSerialNumber(void);

   bool operator == (const C_OSCProtocolSerialNumber & orc_Cmp) const;
   bool operator <(const C_OSCProtocolSerialNumber & orc_Cmp) const;

   void SetPosSerialNumber(const stw_types::uint8 (&orau8_SerialNumber)[6]);
   stw_types::sint32 SetExtSerialNumber(const std::vector<stw_types::uint8> & orc_SerialNumber,
                                        const stw_types::uint8 ou8_SerialNumberManufacturerFormat);
   stw_types::sint32 SetExtSerialNumber(const stw_scl::C_SCLString & orc_SerialNumber,
                                        const stw_types::uint8 ou8_SerialNumberManufacturerFormat);
   std::vector<stw_types::uint8> GetSerialNumberAsRawData(void) const;
   stw_scl::C_SCLString GetSerialNumberAsFormattedString(void) const;
   stw_scl::C_SCLString GetSerialNumberAsPlainString(void) const;

   static bool h_SerialNumberFromStringToArray(const stw_scl::C_SCLString & orc_SerialNumber,
                                               stw_types::uint8(&orau8_SerialNumber)[6]);

   bool q_IsValid;         ///< flag if a valid serial number was already set
   bool q_ExtFormatUsed;   ///< flag which protocol communication was used. true for extended, false for standard
   bool q_FsnSerialNumber; ///< flag if serial number was set in FSN or POS format

   stw_types::uint8 u8_SerialNumberByteLength; ///< length of serial number

   // Value for a serial number in POS format
   stw_types::uint8 au8_SerialNumber[6]; ///< serial number of sender in POS format

   // Values used for a serial number in FSN format
   // Special case: u8_SerialNumberManufacturerFormat equals 0, the old STW standard format POS is used
   // despite of the used extended format.
   stw_scl::C_SCLString c_SerialNumberExt;             ///< serial number of sender in FSN format
   stw_types::uint8 u8_SerialNumberManufacturerFormat; ///< defines the used manufacturer format for the sn
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif
