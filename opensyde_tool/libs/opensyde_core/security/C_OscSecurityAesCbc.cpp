//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       openSYDE Core AES text encryption utility

   see header in .h file for details.

   \copyright   Copyright 2025 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.hpp"

#include "AES.h"
#include "stwerrors.hpp"
#include "C_OscSecurityAesCbc.hpp"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw::errors;
using namespace stw::opensyde_core;
using namespace std;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

/* -- Global Variables ---------------------------------------------------------------------------------------------- */

/* -- Module Global Variables --------------------------------------------------------------------------------------- */

/* -- Module Global Function Prototypes ----------------------------------------------------------------------------- */

/* -- Implementation ------------------------------------------------------------------------------------------------ */

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Encrypt given array with AES-128 using CBC mode and PKCS#7 padding

   The algorithm only can work with blocks of 16 bytes. So we apply PKCS#7 algorithm to fill up.
   The library we use supports padding, but only with zeros, not with PKCS#7. So we do this manually.

   Steps:
   * append padding values with PKCS#7 algorithm
   * perform encryption
   * write to output array

   \param[in]   orau8_Key           128bit key to use for encryption
   \param[in]   orau8_InitVector    128bit initialization vector
   \param[in]   orc_Input           Input data
   \param[out]  orc_Output          Output data

   \return
   C_NO_ERR    success
   C_CONFIG    output size changed while encryption with library (very unexpected and reason unknown)
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_OscSecurityAesCbc::h_Encrypt(const uint8_t (&orau8_Key)[hu32_KEY_LENGTH],
                                       const uint8_t (&orau8_InitVector)[hu32_IV_LENGTH],
                                       const vector<uint8_t> & orc_Input, vector<uint8_t> & orc_Output)
{
   int32_t s32_Return = C_NO_ERR;

   const uint32_t u32_InputSize = static_cast<uint32_t>(orc_Input.size());
   const uint8_t u8_Pkcs7Size = static_cast<uint8_t>(16U - (u32_InputSize % 16U));
   const uint32_t u32_PaddedInputSize = u32_InputSize + static_cast<uint32_t>(u8_Pkcs7Size);

   vector<uint8_t> c_PaddedInput = orc_Input;
   uint8_t au8_KeyCopy[hu32_KEY_LENGTH];
   uint8_t au8_InitVectCopy[hu32_KEY_LENGTH];

   uint8_t * pu8_EncryptedData;
   unsigned int x_EncryptedSize; //lint !e8080 !e970  //using type to match library interface
   AES c_Aes(128);

   // use copies because library parameters are unfortunately not const
   memcpy(au8_KeyCopy, orau8_Key, hu32_KEY_LENGTH);
   memcpy(au8_InitVectCopy, orau8_InitVector, hu32_IV_LENGTH);

   //add PKCS#7 values:
   c_PaddedInput.resize(u32_PaddedInputSize);
   for (uint8_t u8_ByteIndex = 0U; u8_ByteIndex < u8_Pkcs7Size; u8_ByteIndex++)
   {
      c_PaddedInput[static_cast<size_t>(u32_InputSize) + u8_ByteIndex] = u8_Pkcs7Size;
   }

   // do the encryption
   pu8_EncryptedData = c_Aes.EncryptCBC(
      &c_PaddedInput[0],
      static_cast<unsigned int>(u32_PaddedInputSize), //lint !e970  //using type to match library interface
      &au8_KeyCopy[0],
      &au8_InitVectCopy[0],
      x_EncryptedSize);

   //size should not have changed
   if (x_EncryptedSize != u32_PaddedInputSize)
   {
      s32_Return = C_CONFIG;
   }
   else
   {
      //write to output array
      orc_Output.assign(pu8_EncryptedData, &pu8_EncryptedData[x_EncryptedSize]);
   }
   //clean up memory allocated by AES library:
   delete[] pu8_EncryptedData;

   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Decrypt given text with AES-128 using CBC mode and PKCS#7 padding

   The size of the text must be a multiple of 16bytes resp. 32hex characters.
   The text must have been written with PKCS#7 algorithm.

   Steps:
   * perform decryption
   * size decrypted data according to PKCS#7 algorithm

   \param[in]   orau8_Key           128bit key to use for decryption
   \param[in]   orau8_InitVector    128bit initialization vector
   \param[in]   orc_Input           Encrypted input string (assumed to be a hex string))
   \param[out]  orc_Output          Decrypted output string

   \return
   C_NO_ERR    success
   C_CONFIG    input text length is no multiple of 16bytes
   C_CHECKSUM  input text is invalid; PKCS#7 value is > 16 or > file size (checked after decryption)
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_OscSecurityAesCbc::h_Decrypt(const uint8_t (&orau8_Key)[hu32_KEY_LENGTH],
                                       const uint8_t (&orau8_InitVector)[hu32_IV_LENGTH],
                                       const vector<uint8_t> & orc_Input, vector<uint8_t> & orc_Output)
{
   int32_t s32_Return = C_NO_ERR;
   const uint32_t u32_InputSize = static_cast<uint32_t>(orc_Input.size());

   // check inputs: input text correctly padded to 16 bytes resp. 32 hex characters?
   if ((u32_InputSize % 16U) != 0U)
   {
      s32_Return = C_CONFIG;
   }
   else
   {
      uint8_t * pu8_DecryptedData;
      uint8_t u8_Pkcs7Value;
      AES c_Aes(128);
      uint8_t au8_KeyCopy[hu32_KEY_LENGTH];
      uint8_t au8_InitVectCopy[hu32_KEY_LENGTH];
      vector<uint8_t> c_InputCopy;

      // use copies because library parameters are unfortunately not const
      memcpy(au8_KeyCopy, orau8_Key, hu32_KEY_LENGTH);
      memcpy(au8_InitVectCopy, orau8_InitVector, hu32_IV_LENGTH);
      c_InputCopy = orc_Input;

      // do the decryption
      pu8_DecryptedData = c_Aes.DecryptCBC(
         &c_InputCopy[0],
         static_cast<unsigned int>(u32_InputSize), //lint !e970  //using type to match library interface
         &au8_KeyCopy[0],
         &au8_InitVectCopy[0]);

      //get padding length of PKCS#7 padding
      u8_Pkcs7Value = pu8_DecryptedData[u32_InputSize - 1];
      if ((u8_Pkcs7Value > 16) || (u8_Pkcs7Value > u32_InputSize))
      {
         //this is unexpected; possible reasons: incorrect key; not a text encrypted with AES + PKCS#7
         s32_Return = C_CHECKSUM;
      }
      else
      {
         //write to output array
         orc_Output.assign(pu8_DecryptedData, &pu8_DecryptedData[static_cast<uint32_t>(u32_InputSize - u8_Pkcs7Value)]);
      }

      //clean up memory allocated by AES library:
      delete[] pu8_DecryptedData;
   }

   return s32_Return;
}
