//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Utility class for CAN specific operations (implementation)

   \copyright   Copyright 2018 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.hpp"

#include "stwtypes.hpp"
#include "C_OscCanUtil.hpp"
#include "C_OscNodeDataPoolContentUtil.hpp"
#include "TglUtils.hpp"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */

using namespace stw::opensyde_core;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

/* -- Global Variables ---------------------------------------------------------------------------------------------- */

/* -- Module Global Variables --------------------------------------------------------------------------------------- */

/* -- Module Global Function Prototypes ----------------------------------------------------------------------------- */

/* -- Implementation ------------------------------------------------------------------------------------------------ */

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Sets a new PGN and updates all depending elements

   \param[in]  ou32_Pgn     New PGN
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OscCanUtilJ1939PgInfo::SetPgn(const uint32_t ou32_Pgn)
{
   this->u32_Pgn = ou32_Pgn;

   // Extract the PGN
   this->u8_PduSpecific = static_cast<uint8_t>(this->u32_Pgn & 0x000000FFU);
   this->u8_PduFormat = static_cast<uint8_t>((this->u32_Pgn & 0x0000FF00U) >> 8U);
   this->u8_Dp = static_cast<uint8_t>((this->u32_Pgn & 0x00010000U) >> 16U);
   this->u8_Edp = static_cast<uint8_t>((this->u32_Pgn & 0x00020000U) >> 17U);

   this->q_HasDestinationAddress = (this->u8_PduFormat <= 0xEFU);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Default constructor
*/
//----------------------------------------------------------------------------------------------------------------------
C_OscCanUtil::C_OscCanUtil(void)
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Checks if signal fits into a CAN message with a specific DLC

   \param[in]  ou8_Dlc     Length of CAN message
   \param[in]  orc_Signal  Signal for checking

   \return
   true     Signal fits into message
   false    Signal does not fit into message
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_OscCanUtil::h_IsSignalInMessage(const uint8_t ou8_Dlc, const C_OscCanSignal & orc_Signal)
{
   bool q_Return = false;
   const uint16_t u16_LastBit = orc_Signal.GetDataBytesBitPosOfSignalBit(orc_Signal.u16_ComBitLength - 1U);
   const uint16_t u16_NeededBytes = (u16_LastBit / 8U) + 1U;

   if (u16_NeededBytes <= ou8_Dlc)
   {
      q_Return = true;
   }

   return q_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Gets value for signal out of CAN message data bytes

   Signals will be converted to little endian (Intel)

   \param[in]      orau8_CanDb         Data bytes of CAN message
   \param[in]      orc_Signal          Signal configuration used for extraction
   \param[in,out]  orc_DataPoolData    Data with extracted value (Size must be already set)
   \param[in]      oe_ContentType      Content type
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OscCanUtil::h_GetSignalValue(const uint8_t (&orau8_CanDb)[8], const C_OscCanSignal & orc_Signal,
                                    std::vector<uint8_t> & orc_DataPoolData,
                                    const C_OscNodeDataPoolContent::E_Type oe_ContentType)
{
   const uint16_t u16_StartByte = orc_Signal.u16_ComBitStart / 8U;
   const uint16_t u16_LengthBitOffset = orc_Signal.u16_ComBitLength % 8U;
   uint16_t u16_LengthByte = orc_Signal.u16_ComBitLength / 8U;

   if (u16_LengthBitOffset != 0U)
   {
      // Round up
      ++u16_LengthByte;
   }

   if (orc_Signal.e_ComByteOrder == C_OscCanSignal::eBYTE_ORDER_INTEL)
   {
      const uint16_t u16_LsbBitOffset = orc_Signal.u16_ComBitStart % 8U;

      if (u16_LsbBitOffset == 0U)
      {
         // Byte aligned data, copy the data
         (void)memcpy(&orc_DataPoolData[0], &orau8_CanDb[u16_StartByte], u16_LengthByte);
      }
      else
      {
         // Signal is not byte aligned
         uint16_t u16_CurByte;
         for (u16_CurByte = 0U; u16_CurByte < u16_LengthByte; ++u16_CurByte)
         {
            const uint16_t u16_MessageIndex = u16_StartByte + u16_CurByte;
            // This is the MSB part of the byte, right shifting to get it byte aligned
            orc_DataPoolData[u16_CurByte] = orau8_CanDb[u16_MessageIndex] >> u16_LsbBitOffset;
            if ((u16_MessageIndex + 1U) < 8U)
            {
               // If the byte is spread over two bytes of the message bytes
               // This is the MSB 'part' of the byte, left shifting over the LSB 'part'
               // Casting to 32 bit because of lint warning 701
               orc_DataPoolData[u16_CurByte] +=
                  static_cast<uint8_t>(static_cast<uint32_t>(orau8_CanDb[u16_MessageIndex + 1U]) <<
                                       (8U - u16_LsbBitOffset));
            }
         }
      }
   }
   else
   {
      // 'Empty' offset of the last bit to the previous byte
      const uint16_t u16_LsbBitOffset = orc_Signal.GetDataBytesBitPosOfSignalBit(orc_Signal.u16_ComBitLength - 1U) % 8U;
      uint16_t u16_CurByte;

      if (u16_LsbBitOffset == 0U)
      {
         // Byte aligned data, copy the data in other direction
         for (u16_CurByte = 0U; u16_CurByte < u16_LengthByte; ++u16_CurByte)
         {
            const uint16_t u16_DataIndex = (static_cast<uint16_t>(u16_LengthByte - u16_CurByte)) - 1U;
            orc_DataPoolData[u16_DataIndex] = orau8_CanDb[u16_StartByte + u16_CurByte];
         }
      }
      else
      {
         if ((u16_LsbBitOffset + orc_Signal.u16_ComBitLength) <= 8U)
         {
            // The signal is in only one message byte
            orc_DataPoolData[0] = orau8_CanDb[u16_StartByte] >> u16_LsbBitOffset;
         }
         else
         {
            // Get the byte count used in the message for the signal. It must not be the same as the size of the signal
            const uint16_t u16_LastBitDistance = (7U - (orc_Signal.u16_ComBitStart % 8U)) + orc_Signal.u16_ComBitLength;
            uint16_t u16_LengthInMessage = u16_LastBitDistance / 8U;

            if ((u16_LastBitDistance % 8U) != 0)
            {
               // Round up
               ++u16_LengthInMessage;
            }

            for (u16_CurByte = 0U; u16_CurByte < u16_LengthByte; ++u16_CurByte)
            {
               // Iterate through the message bytes in opposite direction
               const uint16_t u16_MessageIndex =
                  static_cast<uint16_t>(static_cast<uint16_t>(u16_LengthInMessage + u16_StartByte) - u16_CurByte) - 1U;
               tgl_assert(u16_MessageIndex < sizeof(orau8_CanDb));

               if (u16_MessageIndex < sizeof(orau8_CanDb))
               {
                  // This is the MSB part of the byte, right shifting to get it byte aligned
                  orc_DataPoolData[u16_CurByte] = orau8_CanDb[u16_MessageIndex] >> u16_LsbBitOffset;

                  if (u16_MessageIndex > 0U)
                  {
                     // If the byte is spread over two bytes of the message bytes
                     // This is the MSB 'part' of the byte, left shifting over the LSB 'part'
                     // Casting to 32 bit because of lint warning 701
                     orc_DataPoolData[u16_CurByte] +=
                        static_cast<uint8_t>(static_cast<uint32_t>(orau8_CanDb[u16_MessageIndex - 1U]) <<
                                             (8U - u16_LsbBitOffset));
                  }
               }
            }
         }
      }
   }

   if (u16_LengthBitOffset != 0U)
   {
      // Last byte of real signal depending of the signal length with the signed bit included
      const uint16_t u16_IndexLastSignalByte = u16_LengthByte - 1U;
      // Irrelevant bits could be still set 'above' the MSB
      orc_DataPoolData[u16_IndexLastSignalByte] &=
         static_cast<uint8_t>(~static_cast<uint8_t>(0xFFU << u16_LengthBitOffset));
      // Handle a signed value at the end
      if ((oe_ContentType == C_OscNodeDataPoolContent::eSINT8) ||
          (oe_ContentType == C_OscNodeDataPoolContent::eSINT16) ||
          (oe_ContentType == C_OscNodeDataPoolContent::eSINT32) ||
          (oe_ContentType == C_OscNodeDataPoolContent::eSINT64))
      {
         // The byte of the signal in the Datapool data
         // It is not always the same as the byte length of the signal
         const uint16_t u16_IndexLastDataTypeByte =
            static_cast<uint16_t>(C_OscNodeDataPoolContentUtil::h_GetDataTypeSizeInByte(oe_ContentType) - 1UL);
         // The signed bit mask for the signal based on the concrete size of the signal
         const uint8_t u8_SignedBitMask = static_cast<uint8_t>(0x01UL << (u16_LengthBitOffset - 1UL));

         // Check the type and its length and where the signed bit of the native type has to be

         // Check if it is a negative value
         if ((orc_DataPoolData[u16_IndexLastSignalByte] & u8_SignedBitMask) == u8_SignedBitMask)
         {
            uint16_t u16_NotUsedByteCounter;
            uint16_t u16_BitCounter;
            // The signed bit must be moved to MSB position and all "new" bits must be set to 1
            // Therefore the signed bit must not be moved explicit, all bits above (MSB side) the
            // previous signed bit must be set to 1
            for (u16_BitCounter = u16_LengthBitOffset; u16_BitCounter < 8U; ++u16_BitCounter)
            {
               orc_DataPoolData[u16_IndexLastSignalByte] |= static_cast<uint8_t>(0x01UL << u16_BitCounter);
            }

            // In case of the signed bit is not in the last byte (can happen in eSINT32 or eSINT64)
            // The bytes on the MSB side of the signed bit, which are not used, must be filled with 0xFF
            for (u16_NotUsedByteCounter = (u16_IndexLastSignalByte + 1UL);
                 u16_NotUsedByteCounter <= u16_IndexLastDataTypeByte;
                 ++u16_NotUsedByteCounter)
            {
               orc_DataPoolData[u16_NotUsedByteCounter] = 0xFFU;
            }
         }
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Gets value for signal out of CAN message data bytes

   \param[in]      orau8_CanDb   Data bytes of CAN message
   \param[in]      orc_Signal    Signal configuration used for extraction
   \param[in,out]  orc_Value     Extracted value and source for size of value
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OscCanUtil::h_GetSignalValue(const uint8_t (&orau8_CanDb)[8], const C_OscCanSignal & orc_Signal,
                                    C_OscNodeDataPoolContent & orc_Value)
{
   std::vector<uint8_t> c_Data;

   c_Data.resize(orc_Value.GetSizeByte(), 0U);
   // The result is in little endian. The function converts in case of Motorola format automatically.
   h_GetSignalValue(orau8_CanDb, orc_Signal, c_Data, orc_Value.GetType());

   orc_Value.SetValueFromLittleEndianBlob(c_Data);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Copy data bytes from data pool data to message payload data according to endianness.

   copied from: module: comm_stack/osy_com_utils.c function: m_fill_message_payload_from_data_pool_data

   \param[in,out]  orau8_CanDb   Current data bytes of CAN message
   \param[in]      orc_Signal    Current signal
   \param[in]      orc_Value     Data from data pool to write into the message
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OscCanUtil::h_SetSignalValue(uint8_t (&orau8_CanDb)[8], const C_OscCanSignal & orc_Signal,
                                    const C_OscNodeDataPoolContent & orc_Value)
{
   const uint8_t u8_StartByte = static_cast<uint8_t>(orc_Signal.u16_ComBitStart / 8U);
   const uint8_t u8_LengthBitOffset = static_cast<uint8_t>(orc_Signal.u16_ComBitLength % 8U);
   uint8_t u8_LengthByte = static_cast<uint8_t>(orc_Signal.u16_ComBitLength / 8U);

   std::vector<uint8_t> c_ValueData;

   //Get data in format as necessary
   orc_Value.GetValueAsLittleEndianBlob(c_ValueData);

   if (u8_LengthBitOffset != 0U)
   {
      // Round up
      ++u8_LengthByte;

      // Handle a signal with not standard length first
      // The byte of the signal in the Datapool data which has the bit for the signed value
      // It is not always the same as the byte length of the signal
      const uint16_t u16_LastDataTypeByte =
         static_cast<uint16_t>(C_OscNodeDataPoolContentUtil::h_GetDataTypeSizeInByte(orc_Value.GetType())) -
         1U;
      const uint8_t u8_LastSignalByte = u8_LengthByte - 1U;

      // clear data pool data
      std::vector<uint8_t> c_ValueDataCopy(8UL, 0UL);

      // Copy the Datapool data. Only the bytes which are necessary for the signal dependent of its defined length
      (void)memcpy(&c_ValueDataCopy[0], &c_ValueData[0], u8_LengthByte);

      // Handle a signed value
      if ((orc_Value.GetType() == C_OscNodeDataPoolContent::eSINT8) ||
          (orc_Value.GetType() == C_OscNodeDataPoolContent::eSINT16) ||
          (orc_Value.GetType() == C_OscNodeDataPoolContent::eSINT32) ||
          (orc_Value.GetType() == C_OscNodeDataPoolContent::eSINT64))
      {
         // Check if it is a negative value on the original Data
         // If the signal has less bytes than the Datapool element, it is possible that the byte with the signed
         // bit is not copied
         if (c_ValueData[u16_LastDataTypeByte] >= 0x80U)
         {
            // Removing of the signed bit is only relevant, if the byte is used of the signal
            if (u16_LastDataTypeByte == u8_LastSignalByte)
            {
               // Remove the signed bit of the native type
               c_ValueDataCopy[u16_LastDataTypeByte] ^= 0x80U;
            }

            // Add it on last signal bit position in the last signal byte
            c_ValueDataCopy[u8_LastSignalByte] |= static_cast<uint8_t>(0x01U << (u8_LengthBitOffset - 1U));
         }
      }

      // Remove the not used bits of the native type of the last used byte of the signal
      c_ValueDataCopy[u8_LastSignalByte] &= static_cast<uint8_t>(0xFFU >> (8U - u8_LengthBitOffset));

      // In case of a not byte aligned value, copy the data and update the pointer used by the previous algorithm
      c_ValueData = c_ValueDataCopy;
   }

   if (orc_Signal.e_ComByteOrder == C_OscCanSignal::eBYTE_ORDER_INTEL)
   {
      const uint8_t u8_LsbBitOffset = static_cast<uint8_t>(orc_Signal.u16_ComBitStart % 8U);

      if (u8_LsbBitOffset == 0U)
      {
         if (u8_LengthBitOffset == 0U)
         {
            // Byte aligned data on MSB and LSB side, copy the data
            (void)memcpy(&orau8_CanDb[u8_StartByte], &c_ValueData[0], u8_LengthByte);
         }
         else
         {
            const uint8_t u8_IndexLastByte = u8_LengthByte - 1U;
            // Byte aligned data only on LSB side, copy the data till the last byte
            if (u8_LengthByte > 1U)
            {
               (void)memcpy(&orau8_CanDb[u8_StartByte], &c_ValueData[0], u8_IndexLastByte);
            }
            // The last message byte can have multiple signals, which could be already assigned
            orau8_CanDb[u8_StartByte + u8_IndexLastByte] |= c_ValueData[u8_IndexLastByte];
         }
      }
      else
      {
         // Signal is not byte aligned
         uint8_t u8_CurByte;
         for (u8_CurByte = 0U; u8_CurByte < u8_LengthByte; ++u8_CurByte)
         {
            const uint8_t u8_MessageIndex = u8_StartByte + u8_CurByte;
            // This is the MSB part of the byte, left shifting to get it to the correct position
            orau8_CanDb[u8_MessageIndex] |= static_cast<uint8_t>(c_ValueData[u8_CurByte] << u8_LsbBitOffset);
            if ((u8_MessageIndex + 1U) < 8U)
            {
               // If the byte is spread over two bytes of the message bytes
               // This is the MSB 'part' of the byte, left shifting over the LSB 'part'
               orau8_CanDb[u8_MessageIndex + 1U] |= c_ValueData[u8_CurByte] >> (8U - u8_LsbBitOffset);
            }
         }
      }
   }
   else
   {
      // 'Empty' offset of the last bit to the previous byte
      const uint8_t u8_LsbBitOffset =
         static_cast<uint8_t>(orc_Signal.GetDataBytesBitPosOfSignalBit(orc_Signal.u16_ComBitLength - 1U) % 8U);
      uint8_t u8_CurByte;

      if (u8_LsbBitOffset == 0U)
      {
         // Byte aligned data on MSB and LSB side, assign the data in other direction
         // The first message byte can have multiple signals, which could be already assigned
         for (u8_CurByte = 0U; u8_CurByte < u8_LengthByte; ++u8_CurByte)
         {
            const uint8_t u8_DataIndex = (u8_LengthByte - u8_CurByte) - static_cast<uint8_t>(1U);
            orau8_CanDb[u8_StartByte + u8_CurByte] |= c_ValueData[u8_DataIndex];
         }
      }
      else
      {
         // Signal is not byte aligned
         if ((static_cast<uint16_t>(u8_LsbBitOffset) + orc_Signal.u16_ComBitLength) <= 8U)
         {
            // The signal is in only one message byte
            orau8_CanDb[u8_StartByte] |= static_cast<uint8_t>(c_ValueData[0] << u8_LsbBitOffset);
         }
         else
         {
            // Get the byte count used in the message for the signal. It must not be the same as the size of the signal
            const uint8_t u8_LastBitDistance =
               static_cast<uint8_t>((7U - (orc_Signal.u16_ComBitStart % 8U)) + orc_Signal.u16_ComBitLength);
            uint8_t u8_LengthInMessage = u8_LastBitDistance / 8U;

            if ((u8_LastBitDistance % 8U) != 0U)
            {
               // Round up
               ++u8_LengthInMessage;
            }

            for (u8_CurByte = 0U; u8_CurByte < u8_LengthByte; ++u8_CurByte)
            {
               // Iterate through the message bytes in opposite direction
               const uint8_t u8_MessageIndex = ((u8_LengthInMessage + u8_StartByte) - u8_CurByte) -
                                               static_cast<uint8_t>(1U);
               tgl_assert(u8_MessageIndex < sizeof(orau8_CanDb));

               // This is the MSB part of the byte, right shifting to get it byte aligned
               orau8_CanDb[u8_MessageIndex] |= static_cast<uint8_t>(c_ValueData[u8_CurByte] << u8_LsbBitOffset);
               if ((u8_MessageIndex > 0U) && (u8_MessageIndex < sizeof(orau8_CanDb)))
               {
                  // If the byte is spread over two bytes of the message bytes
                  // This is the MSB 'part' of the byte, left shifting over the LSB 'part'
                  orau8_CanDb[u8_MessageIndex - 1U] |= c_ValueData[u8_CurByte] >> (8U - u8_LsbBitOffset);
               }
            }
         }
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Gets the J1939 PG information from a CAN ID

   No error check or plausibility check here

   \param[in]       ou32_CanId     CAN identifier to extract J1939 PG information
   \param[out]      orc_PgInfo     Extracted PG information
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OscCanUtil::h_GetJ1939PgInfoFromCanId(const uint32_t ou32_CanId, C_OscCanUtilJ1939PgInfo & orc_PgInfo)
{
   orc_PgInfo.u8_SourceAddress = static_cast<uint8_t>(ou32_CanId & 0x000000FFU);
   orc_PgInfo.SetPgn((ou32_CanId & 0x03FFFF00U) >> 8U);
   orc_PgInfo.u8_Priority = static_cast<uint8_t>((ou32_CanId & 0x1C000000U) >> 26U);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Gets the CAN ID from a J1939 PG information

   No error check or plausibility check here

   \param[in]       orc_PgInfo     PG information for converting to CAN ID
   \param[out]      oru32_CanId    Generated CAN ID
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OscCanUtil::h_GetCanIdFromJ1939PgInfo(const C_OscCanUtilJ1939PgInfo & orc_PgInfo, uint32_t & oru32_CanId)
{
   oru32_CanId = static_cast<uint32_t>(orc_PgInfo.u8_SourceAddress);
   oru32_CanId |= ((orc_PgInfo.u32_Pgn << 8U) & 0x03FFFF00U);
   oru32_CanId |= ((static_cast<uint32_t>(orc_PgInfo.u8_Priority) << 26U) & 0x1C000000U);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Returns the PGN value for display purpose
 *
   Returns the modified PGN where the destination address is interpreted as 0x00

   \param[in]       ou32_Pgn     PGN value to be modified

   \return
   The modified PGN
*/
//----------------------------------------------------------------------------------------------------------------------
uint32_t C_OscCanUtil::h_GetVisiblePgn(const uint32_t ou32_Pgn)
{
   const uint8_t u8_PduFormat = static_cast<uint8_t>((ou32_Pgn & 0x0000FF00U) >> 8U);
   const bool q_HasDestinationAddress = (u8_PduFormat <= 0xEFU);

   uint32_t u32_VisiblePgn = ou32_Pgn;

   if (q_HasDestinationAddress == true) // PDU Format 1 (Point-to-Point)
   {
      u32_VisiblePgn = u32_VisiblePgn & 0x03FF00U;
   }
   return u32_VisiblePgn;
}
