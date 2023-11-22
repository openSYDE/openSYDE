//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Utility functions concerning the STW company-ID

   This module provides functionality to convert the STW company-ID
    between ASCII and binary representation.
   It is written in plain ANSI-C and can thus be used in most C / C++
    projects.

   project
   \copyright   Copyright 2008 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <string.h>

#include "stwtypes.h"
#include "stwerrors.h"
#include "stwcompid.h"

/* -- Defines ------------------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

/* -- Global Variables ---------------------------------------------------------------------------------------------- */

/* -- Module Global Variables --------------------------------------------------------------------------------------- */

/* -- Module Global Function Prototypes ----------------------------------------------------------------------------- */

/* -- Implementation ------------------------------------------------------------------------------------------------ */

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Convert company-ID string to byte-array to send to server

   Convert 2, 3, 5 character company-ID to 2-byte array to send to server.
   Checks for valid content.
   2 and 5 character IDs are simply copied into the array.
   3 character IDs are packed.

   \param[in]     opcn_String      Company-ID as string (must be zero-terminated)
   \param[out]    opu8_Bytes       2/5 Bytes: binary company-ID (the caller is responsible for providing sufficient \n
                                                                 memory)
   \param[out]    opu8_NumBytes    Number of bytes used in opu8_Data

   \return
   C_NO_ERR   converted company-id
   C_CONFIG   invalid company-id
*/
//----------------------------------------------------------------------------------------------------------------------
sintn CID_string_to_bytes(const charn * const opcn_String, uint8 * const opu8_Bytes, uint8 * const opu8_NumBytes)
{
   sint32 s32_Index;
   uint8 au8_Help[3];
   uint16 u16_CID;
   uint8 u8_Check;
   sintn sn_Length;
   sintn sn_Return = C_NO_ERR;

   sn_Length = (sintn)strlen(opcn_String);
   switch (sn_Length)
   {
   case 2:
   case 5:
      //check for valid characters: (0..9; A..Z; uppercase)
      for (s32_Index = 0; s32_Index < sn_Length; s32_Index++)
      {
         u8_Check = opcn_String[s32_Index];
         if (((u8_Check >= 'A') && (u8_Check <= 'Z')) || ((u8_Check >= '0') && (u8_Check <= '9')) ||
             (u8_Check == '*')) //no idea what the '*' is about :-)
         {
         }
         else
         {
            return C_CONFIG;
         }
      }

      //simply copy over ...
      (void)memcpy(opu8_Bytes, opcn_String, sn_Length);
      if (opu8_NumBytes != NULL)
      {
         *opu8_NumBytes = (uint8)sn_Length;
      }
      break;
   case 3:
      //check for valid characters: (1..6; A..Z; uppercase)
      for (s32_Index = 0; s32_Index < 3; s32_Index++)
      {
         u8_Check = opcn_String[s32_Index];
         if (((u8_Check >= 'A') && (u8_Check <= 'Z')) || ((u8_Check >= '1') && (u8_Check <= '6')))
         {
         }
         else
         {
            sn_Return = C_CONFIG;
         }
      }
      if (sn_Return == C_NO_ERR)
      {
         //encode into the two bytes:
         //mapping: 5bit, 32characters, 0 = A .. 25 = Z; 26 = 1 .. 31 = 6
         for (s32_Index = 0; s32_Index < 3; s32_Index++)
         {
            au8_Help[s32_Index] = opcn_String[s32_Index];
            if ((au8_Help[s32_Index] >= 'A') && (au8_Help[s32_Index] <= 'Z'))
            {
               //character
               au8_Help[s32_Index] = (uint8)(au8_Help[s32_Index] - 'A');
            }
            else
            {
               //number
               au8_Help[s32_Index] = (uint8)((au8_Help[s32_Index] - '1') + 26);
            }
         }

         //now put the 16 bit value together
         u16_CID = (uint16)(0x8000U + //set flag to signal "binary" format
                            (((uint16)(au8_Help[0])) << 10U) +
                            (((uint16)(au8_Help[1])) << 5U) +
                            ((uint16)(au8_Help[2])));
         //spec: little endian
         opu8_Bytes[0] = (uint8)u16_CID;
         opu8_Bytes[1] = (uint8)(u16_CID >> 8U);
         if (opu8_NumBytes != NULL)
         {
            *opu8_NumBytes = 2;
         }
      }
      break;
   default:
      sn_Return = C_CONFIG;
      break;
   }
   return sn_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Convert company-ID byte-array to a displayable string

   Convert 2, 3 or 5 character company-ID from binary form to an AnsiString
   No check for valid ASCII character is performed.

   \param[in]     ou8_NumBytes   length of binary company-ID (2 or 5)
   \param[in]     opu8_Bytes     data (array of 2 or 5 bytes) (the caller is responsible for providing enough data)
   \param[out]    opcn_String    zero-terminated string with displayable ID (buffer must be >= 6 characters)

   \return
   C_NO_ERR   converted company-id
   C_CONFIG   invalid length
*/
//----------------------------------------------------------------------------------------------------------------------
sintn CID_bytes_to_string(const uint8 * const opu8_Bytes, const uint8 ou8_NumBytes, charn * const opcn_String)
{
   uint8 au8_Bytes[3];
   uint16 u16_CID;
   uint8 u8_Index;
   sintn sn_Return = C_NO_ERR;

   (void)memset(opcn_String, 0, 6);
   switch (ou8_NumBytes)
   {
   case 2:
      //either a real 2 character ID, or an encoded 3 character one
      //spec: little endian
      u16_CID = (uint16)(((uint16)opu8_Bytes[1] << 8U) + opu8_Bytes[0]);
      if ((u16_CID & 0x8000U) == 0x8000U)
      {
         //encoded 3 character ID -> decode
         au8_Bytes[0] = (uint8)((u16_CID >> 10U) & 0x1F);
         au8_Bytes[1] = (uint8)((u16_CID >> 5U)  & 0x1F);
         au8_Bytes[2] = (uint8)(u16_CID         & 0x1F);

         for (u8_Index = 0; u8_Index < 3; u8_Index++)
         {
            if (au8_Bytes[u8_Index] < 26) //A..Z
            {
               opcn_String[u8_Index] = (charn)(au8_Bytes[u8_Index] + 'A');
            }
            else //1..6
            {
               opcn_String[u8_Index] = (charn)(au8_Bytes[u8_Index] + ('1' - 26));
            }
         }
      }
      else
      {
         //regular 2 character company ID
         //simply copy over:
         (void)memcpy(opcn_String, opu8_Bytes, 2);
      }
      break;
   case 5:
      //simply copy over:
      (void)memcpy(opcn_String, opu8_Bytes, 5);
      break;
   default:
      sn_Return = C_CONFIG;
      break;
   }
   return sn_Return;
}

//----------------------------------------------------------------------------------------------------------------------
