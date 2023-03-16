//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Representation of node serial number in all variations

   It can handle FSN and POS format.
   Utility functions offer conversions into formatted strings and back.
   It is used for all interfaces which need a serial number.

   \copyright   Copyright 2021 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.hpp"

#include <cstring>
#include "stwerrors.hpp"
#include "TglUtils.hpp"
#include "C_OscLoggingHandler.hpp"
#include "C_OscUtils.hpp"
#include "C_OscProtocolSerialNumber.hpp"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */

using namespace stw::errors;
using namespace stw::scl;
using namespace stw::opensyde_core;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

/* -- Global Variables ---------------------------------------------------------------------------------------------- */

/* -- Module Global Variables --------------------------------------------------------------------------------------- */

/* -- Module Global Function Prototypes ----------------------------------------------------------------------------- */

/* -- Implementation ------------------------------------------------------------------------------------------------ */

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default constructor
*/
//----------------------------------------------------------------------------------------------------------------------
C_OscProtocolSerialNumber::C_OscProtocolSerialNumber(void) :
   q_IsValid(false),
   q_ExtFormatUsed(false),
   q_FsnSerialNumber(false),
   u8_SerialNumberByteLength(0U),
   u8_SerialNumberManufacturerFormat(0U)
{
   memset(&this->au8_SerialNumber[0], 0U, 6);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief    Equal operator.

   \param[in] orc_Cmp Compared instance

   \return
   true     Current equals orc_Cmp
   false    Else
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_OscProtocolSerialNumber::operator ==(const C_OscProtocolSerialNumber & orc_Cmp) const
{
   bool q_Return = false;

   if ((this->q_IsValid == orc_Cmp.q_IsValid) &&
       (this->q_ExtFormatUsed == orc_Cmp.q_ExtFormatUsed) &&
       (this->q_FsnSerialNumber == orc_Cmp.q_FsnSerialNumber) &&
       (this->u8_SerialNumberByteLength == orc_Cmp.u8_SerialNumberByteLength) &&
       (this->c_SerialNumberExt == orc_Cmp.c_SerialNumberExt) &&
       (this->u8_SerialNumberManufacturerFormat == orc_Cmp.u8_SerialNumberManufacturerFormat))
   {
      if (memcmp(&this->au8_SerialNumber[0], &orc_Cmp.au8_SerialNumber[0], 6) == 0)
      {
         q_Return = true;
      }
   }

   return q_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Check if current smaller than orc_Cmp

   Only the serial number relevant values are checked.
   q_ExtFormatUsed and q_FsnSerialNumber are not used.

   \param[in] orc_Cmp Compared instance

   \return
   Current smaller than orc_Cmp
   Else false
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_OscProtocolSerialNumber::operator <(const C_OscProtocolSerialNumber & orc_Cmp) const
{
   bool q_Return = false;

   if ((this->q_IsValid == true) &&
       (this->q_IsValid == orc_Cmp.q_IsValid))
   {
      const int x_Return = //lint !e970 !e8080  //using type to match library interface
                           std::memcmp(&this->au8_SerialNumber[0], &orc_Cmp.au8_SerialNumber[0],
                                       sizeof(this->au8_SerialNumber));

      if (x_Return < 0)
      {
         q_Return = true;
      }
      else if (x_Return == 0)
      {
         if (this->u8_SerialNumberManufacturerFormat < orc_Cmp.u8_SerialNumberManufacturerFormat)
         {
            q_Return = true;
         }
         else if (this->u8_SerialNumberManufacturerFormat == orc_Cmp.u8_SerialNumberManufacturerFormat)
         {
            if (this->c_SerialNumberExt < orc_Cmp.c_SerialNumberExt)
            {
               q_Return = true;
            }
         }
         else
         {
            // Nothing to do
         }
      }
      else
      {
         // Nothing to do
      }
   }
   else if (q_IsValid == false)
   {
      q_Return = true;
   }
   else
   {
      // Nothing to do
   }

   return q_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Sets a POS serial number and all other relevant members

   \param[in]    orau8_SerialNumber   POS serial number
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OscProtocolSerialNumber::SetPosSerialNumber(const uint8_t (&orau8_SerialNumber)[6])
{
   (void)std::memcpy(&this->au8_SerialNumber[0], &orau8_SerialNumber[0], 6U);
   this->q_ExtFormatUsed = false;
   this->q_IsValid = true;
   this->u8_SerialNumberByteLength = 6U;
   this->q_FsnSerialNumber = false;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Sets an extended serial number and all other relevant members

   If manufacturer id is 0, the serial number will be handled as POS serial number.
   In all other cases, it will be handled as FSN serial number

   \param[in]    orc_SerialNumber                     Extended serial number (1 to 29 bytes allowed)
   \param[in]    ou8_SerialNumberManufacturerFormat   manufacturer format of serial number

   \return
   C_NO_ERR    Serial number set
   C_RANGE     Serial number has zero length or is to long
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_OscProtocolSerialNumber::SetExtSerialNumber(const std::vector<uint8_t> & orc_SerialNumber,
                                                      const uint8_t ou8_SerialNumberManufacturerFormat)
{
   int32_t s32_Return = C_NO_ERR;

   if ((ou8_SerialNumberManufacturerFormat == 0U) &&
       (orc_SerialNumber.size() == 6))
   {
      // Special case. Handle it as POS serial number
      (void)std::memcpy(&this->au8_SerialNumber[0], &orc_SerialNumber[0], 6U);
      this->q_FsnSerialNumber = false;
   }
   else if ((ou8_SerialNumberManufacturerFormat > 0U) &&
            (orc_SerialNumber.size() > 0) && (orc_SerialNumber.size() <= 29))
   {
      //extract text:
      std::vector<char_t> c_Text;
      c_Text.resize(orc_SerialNumber.size() + 1); // plus 1 for termination
      (void)std::memcpy(&c_Text[0], &orc_SerialNumber[0], orc_SerialNumber.size());
      c_Text[c_Text.size() - 1] = '\0'; //add termination
      this->c_SerialNumberExt = static_cast<C_SclString>(&c_Text[0]);
      this->q_FsnSerialNumber = true;
   }
   else
   {
      s32_Return = C_RANGE;
   }

   if (s32_Return == C_NO_ERR)
   {
      this->q_IsValid = true;
      this->q_ExtFormatUsed = true;
      this->u8_SerialNumberManufacturerFormat = ou8_SerialNumberManufacturerFormat;
      this->u8_SerialNumberByteLength = static_cast<uint8_t>(orc_SerialNumber.size());
   }

   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Sets an extended serial number and all other relevant members

   If manufacturer id is 0, the serial number will be handled as POS serial number.
   In all other cases, it will be handled as FSN serial number

   \param[in]    orc_SerialNumber                     FSN: Extended serial number (1 to 29 bytes allowed)
                                                      POS: Formatted serial number by h_PosSerialNumberToString
                                                           Formation will be reversed
   \param[in]    ou8_SerialNumberManufacturerFormat   manufacturer format of serial number

   \return
   C_NO_ERR    Serial number set
   C_RANGE     Serial number has zero length or is to long
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_OscProtocolSerialNumber::SetExtSerialNumber(const stw::scl::C_SclString & orc_SerialNumber,
                                                      const uint8_t ou8_SerialNumberManufacturerFormat)
{
   int32_t s32_Return = C_NO_ERR;

   if (ou8_SerialNumberManufacturerFormat == 0U)
   {
      this->u8_SerialNumberByteLength = 6U;
      this->q_FsnSerialNumber = false;

      if (h_SerialNumberFromStringToArray(orc_SerialNumber, this->au8_SerialNumber) == false)
      {
         // String is not compatible to POS
         s32_Return = C_RANGE;
      }
   }
   else
   {
      if ((orc_SerialNumber.Length() > 0) && (orc_SerialNumber.Length() <= 29))
      {
         this->c_SerialNumberExt = orc_SerialNumber;
         this->u8_SerialNumberByteLength = static_cast<uint8_t>(orc_SerialNumber.Length());
         this->q_FsnSerialNumber = true;
      }
      else
      {
         s32_Return = C_RANGE;
      }
   }

   if (s32_Return == C_NO_ERR)
   {
      this->q_IsValid = true;
      this->q_ExtFormatUsed = true;
      this->u8_SerialNumberManufacturerFormat = ou8_SerialNumberManufacturerFormat;
   }

   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Returns a vector with uint8 filled with the serial number

   \return
   Serial number in uint8 vector
*/
//----------------------------------------------------------------------------------------------------------------------
std::vector<uint8_t> C_OscProtocolSerialNumber::GetSerialNumberAsRawData(void) const
{
   std::vector<uint8_t> c_SerialNumber;

   if (this->q_FsnSerialNumber == true)
   {
      c_SerialNumber.resize(this->c_SerialNumberExt.Length());
      // Use the FSN string serial number
      memcpy(&c_SerialNumber[0], &this->c_SerialNumberExt.c_str()[0], this->c_SerialNumberExt.Length());
      tgl_assert(this->c_SerialNumberExt.Length() == this->u8_SerialNumberByteLength);
   }
   else
   {
      c_SerialNumber.resize(6);
      // Use the POS serial number
      memcpy(&c_SerialNumber[0], &this->au8_SerialNumber[0], 6);
      tgl_assert(this->u8_SerialNumberByteLength == 6U);
   }

   return c_SerialNumber;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Returns the serial number as formatted string

   In case of POS format, the serial number will be converted by h_PosSerialNumberToString.
   In case of FSN format, the serial number will be converted by h_FsnSerialNumberToString if a specific handling
   for the manufacturer format exists.

   \return
   Formatted serial number string
*/
//----------------------------------------------------------------------------------------------------------------------
stw::scl::C_SclString C_OscProtocolSerialNumber::GetSerialNumberAsFormattedString(void) const
{
   C_SclString c_Return;

   if (this->q_FsnSerialNumber == false)
   {
      c_Return = C_OscUtils::h_PosSerialNumberToString(&this->au8_SerialNumber[0]);
   }
   else
   {
      c_Return = C_OscUtils::h_FsnSerialNumberToString(this->u8_SerialNumberManufacturerFormat,
                                                       this->c_SerialNumberExt);
   }

   return c_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Returns the serial number as simplest possible string

   In case of POS, the serial number is converted by h_PosSerialNumberToString. It can be reversed by
   h_SerialNumberFromStringToArray.

   In case of FSN, the serial number is returned without any change.

   \return
   Serial number as string
*/
//----------------------------------------------------------------------------------------------------------------------
C_SclString C_OscProtocolSerialNumber::GetSerialNumberAsPlainString(void) const
{
   C_SclString c_Return;

   if (this->q_FsnSerialNumber == true)
   {
      c_Return = this->c_SerialNumberExt;
   }
   else
   {
      c_Return = C_OscUtils::h_PosSerialNumberToString(&this->au8_SerialNumber[0]);
   }

   return c_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Conversion of a formatted or string version of a POS serial number to 6 Byte array

   \param[in]       orc_SerialNumber     Detailed input parameter description
   \param[out]      orau8_SerialNumber   Detailed output parameter description

   \retval   true    Conversion successful
   \retval   false   String does not match
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_OscProtocolSerialNumber::h_SerialNumberFromStringToArray(const C_SclString & orc_SerialNumber,
                                                                uint8_t (&orau8_SerialNumber)[6])
{
   bool q_Return = false;
   C_SclString c_CompleteString;

   //format up to and including 2019. E.g: 05.123456.1001
   if (orc_SerialNumber.Length() == 14)
   {
      C_SclDynamicArray<C_SclString> c_Tokens;
      //Get all numbers
      orc_SerialNumber.Tokenize(".", c_Tokens);
      if (c_Tokens.GetLength() == 3)
      {
         //Combine all numbers
         for (uint32_t u32_ItCounter = 0U; u32_ItCounter < static_cast<uint32_t>(c_Tokens.GetLength()); ++u32_ItCounter)
         {
            c_CompleteString += c_Tokens[u32_ItCounter];
         }
      }
   }
   //format from 2020. E.g: 200012345678
   else if (orc_SerialNumber.Length() == 12)
   {
      //no convert needed
      c_CompleteString = orc_SerialNumber;
   }
   else
   {
      //invalid format, should never happen
      C_SclString c_Error;
      c_Error.PrintFormatted("Invalid serial number format. String: \"%s\".", c_CompleteString.c_str());
      osc_write_log_error("Convert serial number string to array", c_Error);
   }

   //get bytes
   if (c_CompleteString.Length() == 12)
   {
      uint32_t u32_ItByte = 0U;
      q_Return = true;
      //For each 2 numbers assign one byte
      for (uint32_t u32_ItChar = 1U; u32_ItChar < c_CompleteString.Length(); u32_ItChar += 2)
      {
         C_SclString c_SubString = c_CompleteString[u32_ItChar];
         c_SubString += c_CompleteString[u32_ItChar + 1];
         c_SubString = "0x" + c_SubString;
         orau8_SerialNumber[u32_ItByte] = static_cast<uint8_t>(c_SubString.ToInt());
         //Next byte
         ++u32_ItByte;
      }
   }

   return q_Return;
}
