//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Data class for a signal positioning in a CAN message (implementation)

   Data class for a signal positioning in a CAN message

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.h"

#include "C_OSCCanSignal.h"
#include "CSCLChecksums.h"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw_types;
using namespace stw_opensyde_core;

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
C_OSCCanSignal::C_OSCCanSignal(void) :
   e_ComByteOrder(eBYTE_ORDER_INTEL),
   u16_ComBitLength(8),
   u16_ComBitStart(0),
   u32_ComDataElementIndex(0),
   e_MultiplexerType(eMUX_DEFAULT),
   u16_MultiplexValue(0)
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Check if current not equal to orc_Cmp

   \param[in] orc_Cmp Compared instance

   \return
   Current not equal to orc_Cmp
   Else false
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_OSCCanSignal::operator !=(const C_OSCCanSignal & orc_Cmp) const
{
   bool q_Return = false;

   if ((this->u32_ComDataElementIndex != orc_Cmp.u32_ComDataElementIndex) ||
       (this->e_ComByteOrder != orc_Cmp.e_ComByteOrder) ||
       (this->u16_ComBitStart != orc_Cmp.u16_ComBitStart) ||
       (this->u16_ComBitLength != orc_Cmp.u16_ComBitLength))
   {
      q_Return = true;
   }

   return q_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Calculates the hash value over all data

   The hash value is a 32 bit CRC value.
   It is not endian-safe, so it should only be used on the same system it is created on.

   \param[in,out] oru32_HashValue    Hash value with init [in] value and result [out] value
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OSCCanSignal::CalcHash(uint32 & oru32_HashValue) const
{
   stw_scl::C_SCLChecksums::CalcCRC32(&this->e_ComByteOrder, sizeof(this->e_ComByteOrder), oru32_HashValue);
   stw_scl::C_SCLChecksums::CalcCRC32(&this->e_MultiplexerType, sizeof(this->e_MultiplexerType), oru32_HashValue);
   stw_scl::C_SCLChecksums::CalcCRC32(&this->u16_ComBitLength, sizeof(this->u16_ComBitLength), oru32_HashValue);
   stw_scl::C_SCLChecksums::CalcCRC32(&this->u16_ComBitStart, sizeof(this->u16_ComBitStart), oru32_HashValue);
   stw_scl::C_SCLChecksums::CalcCRC32(&this->u32_ComDataElementIndex, sizeof(this->u32_ComDataElementIndex),
                                      oru32_HashValue);
   stw_scl::C_SCLChecksums::CalcCRC32(&this->u16_MultiplexValue, sizeof(this->u16_MultiplexValue), oru32_HashValue);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Calculates the position of a signal bit in the data bytes

   \param[in]     ou16_SignalBitPosition   Signal bit position starting at 0

   \return
   Data bytes bit position
*/
//----------------------------------------------------------------------------------------------------------------------
uint16 C_OSCCanSignal::GetDataBytesBitPosOfSignalBit(const uint16 ou16_SignalBitPosition) const
{
   return this->GetDataBytesBitPosOfSignalBit(this->u16_ComBitStart, ou16_SignalBitPosition);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Calculates the position of a signal bit in the data bytes

   \param[in]     ou16_StartBit            New start bit for the signal
   \param[in]     ou16_SignalBitPosition   Signal bit position starting at 0

   \return
   Data bytes bit position
*/
//----------------------------------------------------------------------------------------------------------------------
uint16 C_OSCCanSignal::GetDataBytesBitPosOfSignalBit(const uint16 ou16_StartBit,
                                                     const uint16 ou16_SignalBitPosition) const
{
   uint16 u16_DataBytesBitPos;

   if (this->e_ComByteOrder == C_OSCCanSignal::eBYTE_ORDER_INTEL)
   {
      // Intel byte order has an 1:1 mapping
      u16_DataBytesBitPos = (ou16_StartBit + ou16_SignalBitPosition);
   }
   else
   {
      // start bit is the only one bit with the "correct" index and the start value for calculation at the same time
      u16_DataBytesBitPos = ou16_StartBit;

      if (ou16_SignalBitPosition > 0U)
      {
         uint16 u16_Counter;

         // calculate the position
         for (u16_Counter = 1U; u16_Counter <= ou16_SignalBitPosition; ++u16_Counter)
         {
            if ((u16_DataBytesBitPos % 8U) == 0U)
            {
               // the end of the "upper" byte reached. jump to start of the next byte.
               u16_DataBytesBitPos += 15U;
            }
            else
            {
               // byte order is backwards
               --u16_DataBytesBitPos;
            }
         }
      }
   }

   return u16_DataBytesBitPos;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Calculates the positions of all signal bits in the data bytes

   \param[out]     orc_SetPositions   Signal bit positions
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OSCCanSignal::GetDataBytesBitPositionsOfSignal(std::set<uint16> & orc_SetPositions) const
{
   uint16 u16_Counter;
   uint16 u16_DataBytesBitPos = this->u16_ComBitStart;

   // start bit is the only one bit with the "correct" index and the start value for calculation at the same time
   orc_SetPositions.insert(u16_DataBytesBitPos);

   for (u16_Counter = 1U; u16_Counter < this->u16_ComBitLength; ++u16_Counter)
   {
      if (this->e_ComByteOrder == C_OSCCanSignal::eBYTE_ORDER_INTEL)
      {
         // Intel byte order has an 1:1 mapping
         ++u16_DataBytesBitPos;
      }
      else
      {
         if ((u16_DataBytesBitPos % 8U) == 0U)
         {
            // the end of the "upper" byte reached. jump to start of the next byte.
            u16_DataBytesBitPos += 15U;
         }
         else
         {
            // byte order is backwards
            --u16_DataBytesBitPos;
         }
      }

      orc_SetPositions.insert(u16_DataBytesBitPos);
   }
}
