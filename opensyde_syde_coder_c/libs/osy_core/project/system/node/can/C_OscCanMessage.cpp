//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Data class for CAN communication message related, additional information (implementation)

   Data class for CAN communication message related, additional information

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.hpp"

#include <limits>
#include <map>

#include "C_OscCanMessage.hpp"
#include "C_SclChecksums.hpp"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */

using namespace stw::opensyde_core;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

/* -- Global Variables ---------------------------------------------------------------------------------------------- */

/* -- Module Global Variables --------------------------------------------------------------------------------------- */

/* -- Module Global Function Prototypes ----------------------------------------------------------------------------- */

/* -- Implementation ------------------------------------------------------------------------------------------------ */

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Default constructor
*/
//----------------------------------------------------------------------------------------------------------------------
C_OscCanMessage::C_OscCanMessage(void) :
   c_Name("NewMessage"),
   c_Comment(""),
   u32_CanId(0x7FF),
   q_IsExtended(false),
   u16_Dlc(8),
   e_TxMethod(eTX_METHOD_CYCLIC),
   u32_CycleTimeMs(100),
   u16_DelayTimeMs(10),
   u32_TimeoutMs(310),
   q_CanOpenManagerCobIdIncludesNodeId(true),
   u32_CanOpenManagerCobIdOffset(0),
   q_CanOpenManagerMessageActive(true),
   u16_CanOpenManagerPdoIndex(0),
   u8_CanOpenTxMethodAdditionalInfo(0)
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Check if current not equal to orc_Cmp

   \param[in]  orc_Cmp  Compared instance

   \return
   Current not equal to orc_Cmp
   Else false
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_OscCanMessage::operator !=(const C_OscCanMessage & orc_Cmp) const
{
   bool q_Return = false;

   if ((this->c_Name          != orc_Cmp.c_Name) ||
       (this->c_Comment       != orc_Cmp.c_Comment) ||
       (this->u32_CanId       != orc_Cmp.u32_CanId) ||
       (this->q_IsExtended    != orc_Cmp.q_IsExtended) ||
       (this->u16_Dlc         != orc_Cmp.u16_Dlc) ||
       (this->e_TxMethod      != orc_Cmp.e_TxMethod) ||
       (this->u32_CycleTimeMs != orc_Cmp.u32_CycleTimeMs) ||
       (this->u16_DelayTimeMs != orc_Cmp.u16_DelayTimeMs) ||
       (this->u32_TimeoutMs   != orc_Cmp.u32_TimeoutMs) ||
       (this->c_CanOpenManagerOwnerNodeIndex   != orc_Cmp.c_CanOpenManagerOwnerNodeIndex) ||
       (this->q_CanOpenManagerCobIdIncludesNodeId   != orc_Cmp.q_CanOpenManagerCobIdIncludesNodeId) ||
       (this->u32_CanOpenManagerCobIdOffset   != orc_Cmp.u32_CanOpenManagerCobIdOffset) ||
       (this->q_CanOpenManagerMessageActive   != orc_Cmp.q_CanOpenManagerMessageActive) ||
       (this->u16_CanOpenManagerPdoIndex   != orc_Cmp.u16_CanOpenManagerPdoIndex) ||
       (this->u8_CanOpenTxMethodAdditionalInfo   != orc_Cmp.u8_CanOpenTxMethodAdditionalInfo) ||
       (this->c_Signals       != orc_Cmp.c_Signals))
   {
      q_Return = true;
   }

   return q_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Calculates the hash value over all data

   The hash value is a 32 bit CRC value.

   \param[in,out]  oru32_HashValue     Hash value with unit [in] value and result [out] value
   \param[in]      oq_R20Compatible    Flag to calculate the hash of only elements present in R20
                                       to allow compatibility with existing hash values from R20 release
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OscCanMessage::CalcHash(uint32_t & oru32_HashValue, const bool oq_R20Compatible) const
{
   stw::scl::C_SclChecksums::CalcCRC32(this->c_Name.c_str(), this->c_Name.Length(), oru32_HashValue);
   stw::scl::C_SclChecksums::CalcCRC32(this->c_Comment.c_str(), this->c_Comment.Length(), oru32_HashValue);
   stw::scl::C_SclChecksums::CalcCRC32(&this->u32_CanId, sizeof(this->u32_CanId), oru32_HashValue);
   stw::scl::C_SclChecksums::CalcCRC32(&this->q_IsExtended, sizeof(this->q_IsExtended), oru32_HashValue);
   stw::scl::C_SclChecksums::CalcCRC32(&this->u16_Dlc, sizeof(this->u16_Dlc), oru32_HashValue);
   stw::scl::C_SclChecksums::CalcCRC32(&this->e_TxMethod, sizeof(this->e_TxMethod), oru32_HashValue);
   stw::scl::C_SclChecksums::CalcCRC32(&this->u32_CycleTimeMs, sizeof(this->u32_CycleTimeMs), oru32_HashValue);
   stw::scl::C_SclChecksums::CalcCRC32(&this->u16_DelayTimeMs, sizeof(this->u16_DelayTimeMs),
                                       oru32_HashValue);
   stw::scl::C_SclChecksums::CalcCRC32(&this->u32_TimeoutMs, sizeof(this->u32_TimeoutMs),
                                       oru32_HashValue);

   for (uint32_t u32_Counter = 0U; u32_Counter < this->c_Signals.size(); ++u32_Counter)
   {
      this->c_Signals[u32_Counter].CalcHash(oru32_HashValue, oq_R20Compatible);
   }
   if (oq_R20Compatible == false)
   {
      this->c_CanOpenManagerOwnerNodeIndex.CalcHash(oru32_HashValue);

      stw::scl::C_SclChecksums::CalcCRC32(&this->q_CanOpenManagerCobIdIncludesNodeId,
                                          sizeof(this->q_CanOpenManagerCobIdIncludesNodeId),
                                          oru32_HashValue);
      stw::scl::C_SclChecksums::CalcCRC32(&this->u32_CanOpenManagerCobIdOffset,
                                          sizeof(this->u32_CanOpenManagerCobIdOffset),
                                          oru32_HashValue);
      stw::scl::C_SclChecksums::CalcCRC32(&this->q_CanOpenManagerMessageActive,
                                          sizeof(this->q_CanOpenManagerMessageActive),
                                          oru32_HashValue);
      stw::scl::C_SclChecksums::CalcCRC32(&this->u16_CanOpenManagerPdoIndex,
                                          sizeof(this->u16_CanOpenManagerPdoIndex),
                                          oru32_HashValue);
      stw::scl::C_SclChecksums::CalcCRC32(&this->u8_CanOpenTxMethodAdditionalInfo,
                                          sizeof(this->u8_CanOpenTxMethodAdditionalInfo),
                                          oru32_HashValue);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Check error for specified signal

   \param[in]  opc_List                               Node data pool list containing signal data
                                                      (Optional as it is only required by some checks)
   \param[in]  oru32_SignalIndex                      Signal index
   \param[in]  ou32_CanMessageValidSignalsDlcOffset   CAN message DLC offset for valid signal range check
   \param[in]  oq_CanMessageSignalGapsValid           Flag if gaps between signals are valid or handled as errors
   \param[in]  oq_ByteAlignmentRequired               Flag if byte alignment is required and if the signal does not
                                                      fit into byte alignment it will be handled as error

   \return
   True  Error
   False No error
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_OscCanMessage::CheckErrorSignal(const C_OscNodeDataPoolList * const opc_List, const uint32_t & oru32_SignalIndex,
                                       const uint32_t ou32_CanMessageValidSignalsDlcOffset,
                                       const bool oq_CanMessageSignalGapsValid,
                                       const bool oq_ByteAlignmentRequired) const
{
   bool q_Retval = false;

   if (this->q_CanOpenManagerMessageActive == true)
   {
      bool q_LayoutConflict;
      bool q_BorderConflict;
      bool q_GapConflict;
      bool q_ByteAlignmentLengthConflict;
      bool q_ByteAlignmentStartbitConflict;
      bool q_NameConflict;
      bool q_NoMultiplexerButMultiplexed;
      bool q_MultiplexedValueOutOfRange;
      //Get Hash for all relevant data
      const std::vector<uint32_t> c_Hashes = this->m_GetSignalHashes(opc_List, oru32_SignalIndex);
      static std::map<std::vector<uint32_t>, bool> hc_PreviousResults;
      //Check if check was already performed in the past
      const std::map<std::vector<uint32_t>, bool>::const_iterator c_It = hc_PreviousResults.find(c_Hashes);

      if (c_It == hc_PreviousResults.end())
      {
         bool q_NameInvalid;
         bool q_MinOverMax;
         bool q_ValueBelowMin;
         bool q_ValueOverMax;
         this->CheckErrorSignalDetailed(opc_List, oru32_SignalIndex, &q_LayoutConflict, &q_BorderConflict,
                                        &q_GapConflict,
                                        &q_ByteAlignmentLengthConflict, &q_ByteAlignmentStartbitConflict,
                                        &q_NameConflict, &q_NameInvalid, &q_MinOverMax, &q_ValueBelowMin,
                                        &q_ValueOverMax, &q_NoMultiplexerButMultiplexed, &q_MultiplexedValueOutOfRange,
                                        ou32_CanMessageValidSignalsDlcOffset,
                                        oq_CanMessageSignalGapsValid,
                                        oq_ByteAlignmentRequired);
         if ((q_MultiplexedValueOutOfRange == false) &&
             (q_NoMultiplexerButMultiplexed == false) &&
             (q_LayoutConflict == false) &&
             (q_BorderConflict == false) &&
             (q_GapConflict == false) &&
             (q_ByteAlignmentLengthConflict == false) &&
             (q_ByteAlignmentStartbitConflict == false) &&
             (q_NameConflict == false) &&
             (q_NameInvalid == false) &&
             (q_MinOverMax == false) &&
             (q_ValueBelowMin == false) &&
             (q_ValueOverMax == false))
         {
            q_Retval = false;
         }
         else
         {
            q_Retval = true;
         }
         //Append for possible reusing this result (without conflict checks)
         if ((q_NameInvalid == false) &&
             (q_MinOverMax == false) &&
             (q_ValueBelowMin == false) &&
             (q_ValueOverMax == false))
         {
            hc_PreviousResults[c_Hashes] = false;
         }
         else
         {
            hc_PreviousResults[c_Hashes] = true;
         }
      }
      else
      {
         //Do separate conflict checks, then reuse previous non conflict value
         this->CheckErrorSignalDetailed(opc_List, oru32_SignalIndex, &q_LayoutConflict, &q_BorderConflict,
                                        &q_GapConflict,
                                        &q_ByteAlignmentLengthConflict, &q_ByteAlignmentStartbitConflict,
                                        &q_NameConflict,
                                        NULL, NULL,
                                        NULL, NULL, &q_NoMultiplexerButMultiplexed, &q_MultiplexedValueOutOfRange,
                                        ou32_CanMessageValidSignalsDlcOffset,
                                        oq_CanMessageSignalGapsValid,
                                        oq_ByteAlignmentRequired);
         if ((q_NameConflict == true) ||
             (q_LayoutConflict == true) ||
             (q_NoMultiplexerButMultiplexed == true) ||
             (q_MultiplexedValueOutOfRange == true) ||
             (q_BorderConflict == true) ||
             (q_GapConflict == true) ||
             (q_ByteAlignmentLengthConflict == true) ||
             (q_ByteAlignmentStartbitConflict == true))
         {
            q_Retval = true;
         }
         else
         {
            q_Retval = c_It->second;
         }
      }
   }

   return q_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Check detailed error for specified signal

   \param[in]   opc_List                              Node data pool list containing signal data
                                                      (Optional as it is only required by some checks)
   \param[in]   oru32_SignalIndex                     Signal index
   \param[out]  opq_LayoutConflict                    Layout conflict
   \param[out]  opq_BorderConflict                    Border not usable as variable
   \param[out]  opq_GapConflict                       Gap between signals found and
                                                      oq_CanMessageSignalGapsValid is false
   \param[out]  opq_ByteAlignmentLengthConflict       Length of signal is not compatible to byte alignment and
                                                      oq_ByteAlignmentRequired is true
   \param[out]  opq_ByteAlignmentStartbitConflict     Start bit of signal is not compatible to byte alignment and
                                                      oq_ByteAlignmentRequired is true
   \param[out]  opq_NameConflict                      Name conflict
   \param[out]  opq_NameInvalid                       Name not usable as variable
   \param[out]  opq_MinOverMax                        Minimum value over maximum value
   \param[out]  opq_ValueBelowMin                     Init value below minimum
   \param[out]  opq_ValueOverMax                      Init value over maximum
   \param[out]  opq_NoMultiplexerButMultiplexed       Multiplexed signal(s) but no multiplexer
   \param[out]  opq_MultiplexedValueOutOfRange        Multiplexed signal multiplexer value out of range of multiplexer
   \param[in]   ou32_CanMessageValidSignalsDlcOffset  CAN message DLC offset for valid signal range check
   \param[in]   oq_CanMessageSignalGapsValid          Flag if gaps between signals are valid or handled as errors
   \param[in]   oq_ByteAlignmentRequired              Flag if byte alignment is required and if the signal does not
                                                      fit into byte alignment it will be handled as error
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OscCanMessage::CheckErrorSignalDetailed(const C_OscNodeDataPoolList * const opc_List,
                                               const uint32_t & oru32_SignalIndex, bool * const opq_LayoutConflict,
                                               bool * const opq_BorderConflict, bool * const opq_GapConflict,
                                               bool * const opq_ByteAlignmentLengthConflict,
                                               bool * const opq_ByteAlignmentStartbitConflict,
                                               bool * const opq_NameConflict, bool * const opq_NameInvalid,
                                               bool * const opq_MinOverMax, bool * const opq_ValueBelowMin,
                                               bool * const opq_ValueOverMax,
                                               bool * const opq_NoMultiplexerButMultiplexed,
                                               bool * const opq_MultiplexedValueOutOfRange,
                                               const uint32_t ou32_CanMessageValidSignalsDlcOffset,
                                               const bool oq_CanMessageSignalGapsValid,
                                               const bool oq_ByteAlignmentRequired) const
{
   if (opq_LayoutConflict != NULL)
   {
      *opq_LayoutConflict = false;
   }
   if (opq_BorderConflict != NULL)
   {
      *opq_BorderConflict = false;
   }
   if (opq_GapConflict != NULL)
   {
      *opq_GapConflict = false;
   }
   if (opq_ByteAlignmentLengthConflict != NULL)
   {
      *opq_ByteAlignmentLengthConflict = false;
   }
   if (opq_ByteAlignmentStartbitConflict != NULL)
   {
      *opq_ByteAlignmentStartbitConflict = false;
   }
   if (opq_NameConflict != NULL)
   {
      *opq_NameConflict = false;
   }
   if (opq_NameInvalid != NULL)
   {
      *opq_NameInvalid = false;
   }
   if (opq_MinOverMax != NULL)
   {
      *opq_MinOverMax = false;
   }
   if (opq_ValueBelowMin != NULL)
   {
      *opq_ValueBelowMin = false;
   }
   if (opq_ValueOverMax != NULL)
   {
      *opq_ValueOverMax = false;
   }
   if (opq_NoMultiplexerButMultiplexed != NULL)
   {
      *opq_NoMultiplexerButMultiplexed = false;
   }
   if (opq_MultiplexedValueOutOfRange != NULL)
   {
      *opq_MultiplexedValueOutOfRange = false;
   }

   if (this->q_CanOpenManagerMessageActive == true)
   {
      if (oru32_SignalIndex < this->c_Signals.size())
      {
         const C_OscCanSignal & rc_CheckedSignal = this->c_Signals[oru32_SignalIndex];
         if (opq_LayoutConflict != NULL)
         {
            std::set<uint16_t> c_CheckedSetPositions;
            rc_CheckedSignal.GetDataBytesBitPositionsOfSignal(c_CheckedSetPositions);
            for (uint32_t u32_ItSignal = 0; u32_ItSignal < this->c_Signals.size(); ++u32_ItSignal)
            {
               const C_OscCanSignal & rc_CurrentSignal = c_Signals[u32_ItSignal];
               //Skip current signal
               if (u32_ItSignal != oru32_SignalIndex)
               {
                  //Check relevant conflict
                  //Basically: Not relevant if both are multiplexed and have different multiplexer values
                  if ((((rc_CheckedSignal.e_MultiplexerType == C_OscCanSignal::eMUX_MULTIPLEXED_SIGNAL) &&
                        (rc_CurrentSignal.e_MultiplexerType == C_OscCanSignal::eMUX_MULTIPLEXED_SIGNAL)) &&
                       (rc_CheckedSignal.u16_MultiplexValue != rc_CurrentSignal.u16_MultiplexValue)) == false)
                  {
                     std::set<uint16_t> c_CurrentSetPositions;
                     rc_CurrentSignal.GetDataBytesBitPositionsOfSignal(c_CurrentSetPositions);
                     for (std::set<uint16_t>::const_iterator c_ItCurrentPosition = c_CurrentSetPositions.begin();
                          c_ItCurrentPosition != c_CurrentSetPositions.end();
                          ++c_ItCurrentPosition)
                     {
                        for (std::set<uint16_t>::const_iterator c_ItCheckedPosition = c_CheckedSetPositions.begin();
                             c_ItCheckedPosition != c_CheckedSetPositions.end();
                             ++c_ItCheckedPosition)
                        {
                           if (*c_ItCurrentPosition == *c_ItCheckedPosition)
                           {
                              //Conflict
                              *opq_LayoutConflict = true;
                              break;
                           }
                        }
                        if ((*opq_LayoutConflict) == true)
                        {
                           break;
                        }
                     }
                  }
               }
            }
         }
         if (opq_BorderConflict != NULL)
         {
            if (static_cast<uint16_t>(ou32_CanMessageValidSignalsDlcOffset) <= this->u16_Dlc)
            {
               std::set<uint16_t> c_SetPositions;
               //Check over end (after dlc)
               rc_CheckedSignal.GetDataBytesBitPositionsOfSignal(c_SetPositions);
               for (std::set<uint16_t>::const_iterator c_ItPosition = c_SetPositions.begin();
                    c_ItPosition != c_SetPositions.end(); ++c_ItPosition)
               {
                  const uint16_t u16_MaxPosition = static_cast<uint16_t>
                                                   (((this->u16_Dlc - ou32_CanMessageValidSignalsDlcOffset) * 8U) - 1U);
                  if ((*c_ItPosition) > u16_MaxPosition)
                  {
                     *opq_BorderConflict = true;
                     break;
                  }
               }
            }
            else
            {
               *opq_BorderConflict = true;
            }
         }
         if ((opq_GapConflict != NULL)  &&
             (oq_CanMessageSignalGapsValid == false))
         {
            bool q_NoCheckNecessary = false;
            uint16_t u16_RelevantBitPos;

            *opq_GapConflict = true;
            if (rc_CheckedSignal.e_ComByteOrder == C_OscCanSignal::eBYTE_ORDER_INTEL)
            {
               if (rc_CheckedSignal.u16_ComBitStart > 0U)
               {
                  // Intel byte order is the previous bit the relevant bit
                  u16_RelevantBitPos = rc_CheckedSignal.u16_ComBitStart - 1U;
               }
               else
               {
                  // In the other case no gap possible
                  q_NoCheckNecessary = true;
                  *opq_GapConflict = false;
               }
            }
            else
            {
               const uint16_t u16_ComBitStartModulo = rc_CheckedSignal.u16_ComBitStart % 8U;
               if ((u16_ComBitStartModulo + 1U) <= rc_CheckedSignal.u16_ComBitLength)
               {
                  // Needs all lower bits of the byte, so the first bit before the message is in the byte before
                  if (rc_CheckedSignal.u16_ComBitStart >= 8U)
                  {
                     u16_RelevantBitPos =
                        static_cast<uint16_t>(rc_CheckedSignal.u16_ComBitStart - u16_ComBitStartModulo) -
                        1U;
                  }
                  else
                  {
                     //But it is the first byte, no gap before possible
                     q_NoCheckNecessary = true;
                     *opq_GapConflict = false;
                  }
               }
               else
               {
                  // Needs not the entire byte, the first bit before the message is next to the end of the message
                  u16_RelevantBitPos = rc_CheckedSignal.u16_ComBitStart - rc_CheckedSignal.u16_ComBitLength;
               }
            }

            if (q_NoCheckNecessary == false)
            {
               // The signal shall not have a gap before its start bit
               // Comparing with the other signals
               for (uint32_t u32_ItSignal = 0; u32_ItSignal < this->c_Signals.size(); ++u32_ItSignal)
               {
                  const C_OscCanSignal & rc_CurrentSignal = c_Signals[u32_ItSignal];
                  //Skip current signal
                  if (u32_ItSignal != oru32_SignalIndex)
                  {
                     if (rc_CurrentSignal.IsBitPosPartOfSignal(u16_RelevantBitPos) == true)
                     {
                        // At least one signal is on this position
                        // In case of multiple elements on this position, an other part of
                        // this error check will control for it
                        *opq_GapConflict = false;
                        break;
                     }
                  }
               }
            }
         }
         // Byte alignment specific check
         if (oq_ByteAlignmentRequired == true)
         {
            if (opq_ByteAlignmentLengthConflict != NULL)
            {
               // Only standard sizes are supported in case of necessary byte alignment
               if ((rc_CheckedSignal.u16_ComBitLength == 8U) ||
                   (rc_CheckedSignal.u16_ComBitLength == 16U) ||
                   (rc_CheckedSignal.u16_ComBitLength == 32U) ||
                   (rc_CheckedSignal.u16_ComBitLength == 64U))
               {
                  *opq_ByteAlignmentLengthConflict = false;
               }
               else
               {
                  *opq_ByteAlignmentLengthConflict = true;
               }
            }
            if (opq_ByteAlignmentStartbitConflict != NULL)
            {
               if (rc_CheckedSignal.e_ComByteOrder == C_OscCanSignal::eBYTE_ORDER_INTEL)
               {
                  // Intel byte order needs the first bit of the byte as start bit
                  *opq_ByteAlignmentStartbitConflict = ((rc_CheckedSignal.u16_ComBitStart % 8U) != 0);
               }
               else
               {
                  // Motorola byte order needs the last bit of the byte as start bit
                  *opq_ByteAlignmentStartbitConflict = ((rc_CheckedSignal.u16_ComBitStart % 8U) != 7);
               }
            }
         }

         if (opc_List != NULL)
         {
            //Name & min & max
            opc_List->CheckErrorElement(rc_CheckedSignal.u32_ComDataElementIndex, NULL, opq_NameInvalid,
                                        opq_MinOverMax, NULL, NULL);
            //Init
            opc_List->CheckErrorDataSetValue(rc_CheckedSignal.u32_ComDataElementIndex, 0, opq_ValueBelowMin,
                                             opq_ValueOverMax, NULL);

            //Name conflict (only for data elements which are part of the signal
            if (opq_NameConflict != NULL)
            {
               if (rc_CheckedSignal.u32_ComDataElementIndex < opc_List->c_Elements.size())
               {
                  const C_OscNodeDataPoolListElement & rc_CurrentElement =
                     opc_List->c_Elements[rc_CheckedSignal.u32_ComDataElementIndex];
                  for (uint32_t u32_ItSignal = 0; u32_ItSignal < this->c_Signals.size(); ++u32_ItSignal)
                  {
                     const C_OscCanSignal & rc_SignalData = c_Signals[u32_ItSignal];
                     //Skip current signal
                     if (u32_ItSignal != oru32_SignalIndex)
                     {
                        if (rc_SignalData.u32_ComDataElementIndex < opc_List->c_Elements.size())
                        {
                           const C_OscNodeDataPoolListElement & rc_ListElement =
                              opc_List->c_Elements[rc_SignalData.u32_ComDataElementIndex];
                           if (rc_CurrentElement.c_Name.LowerCase() == rc_ListElement.c_Name.LowerCase())
                           {
                              *opq_NameConflict = true;
                              break;
                           }
                        }
                     }
                  }
               }
            }
         }
         if (opq_NoMultiplexerButMultiplexed != NULL)
         {
            //Check contains multiplexer
            if (C_OscCanMessage::h_ContainsMultiplexer(this->c_Signals) == false)
            {
               //Check is multiplexed
               if (rc_CheckedSignal.e_MultiplexerType == C_OscCanSignal::eMUX_MULTIPLEXED_SIGNAL)
               {
                  *opq_NoMultiplexerButMultiplexed = true;
               }
            }
         }
         if (opq_MultiplexedValueOutOfRange != NULL)
         {
            //Only relevant for multiplexed signals
            if (rc_CheckedSignal.e_MultiplexerType == C_OscCanSignal::eMUX_MULTIPLEXED_SIGNAL)
            {
               //Find multiplexer
               for (uint32_t u32_ItSignal = 0; u32_ItSignal < this->c_Signals.size(); ++u32_ItSignal)
               {
                  //Skip current signal
                  if (u32_ItSignal != oru32_SignalIndex)
                  {
                     const C_OscCanSignal & rc_SignalData = c_Signals[u32_ItSignal];
                     if (rc_SignalData.e_MultiplexerType == C_OscCanSignal::eMUX_MULTIPLEXER_SIGNAL)
                     {
                        //Check range
                        const uint64_t u64_MaxValue =
                           (rc_SignalData.u16_ComBitLength >= 64U) ?
                           std::numeric_limits<uint64_t>::max() :
                           ((static_cast<uint64_t>(1ULL) << rc_SignalData.u16_ComBitLength) - 1ULL);
                        if (rc_CheckedSignal.u16_MultiplexValue > u64_MaxValue)
                        {
                           *opq_MultiplexedValueOutOfRange = true;
                        }
                        //Should only be one
                        break;
                     }
                  }
               }
            }
         }
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Is transmission type a cyclic type

   \return
   Flags

   \retval   True    Transmission type is a cyclic type
   \retval   False   Transmission type is not a cyclic type
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_OscCanMessage::IsTransmissionTypeOfCyclicType() const
{
   return C_OscCanMessage::h_IsTransmissionTypeOfCyclicType(this->e_TxMethod);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Is transmission type a cyclic type

   \param[in]  oe_Type  Transmission type

   \return
   Flags

   \retval   True    Transmission type is a cyclic type
   \retval   False   Transmission type is not a cyclic type
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_OscCanMessage::h_IsTransmissionTypeOfCyclicType(const C_OscCanMessage::E_TxMethodType oe_Type)
{
   return ((oe_Type == eTX_METHOD_CYCLIC) || ((oe_Type == eTX_METHOD_CAN_OPEN_TYPE_254)) ||
           (oe_Type == eTX_METHOD_CAN_OPEN_TYPE_255));
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Gets the information if the CAN message is multiplexed

   A multiplexer signal must be defined to be a multiplexed CAN message.
   This multiplexer must be unique, therefore the first found multiplexer is also the only multiplexer.

   \param[out]  opu32_MultiplexerIndex    Multiplexer index

   \retval   true    CAN message is multiplexed, has a multiplexer signal
   \retval   false   CAN message is not multiplexed
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_OscCanMessage::IsMultiplexed(uint32_t * const opu32_MultiplexerIndex) const
{
   return C_OscCanMessage::h_ContainsMultiplexer(this->c_Signals, opu32_MultiplexerIndex);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Check if any of the specified signals is a multiplexer signal

   \param[in]   orc_Signals               Signals to check
   \param[out]  opu32_MultiplexerIndex    Multiplexer index

   \retval   true    Contains multiplexer signal
   \retval   false   No multiplexer signal found
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_OscCanMessage::h_ContainsMultiplexer(const std::vector<C_OscCanSignal> & orc_Signals,
                                            uint32_t * const opu32_MultiplexerIndex)
{
   bool q_Return = false;
   uint32_t u32_SignalCounter;

   for (u32_SignalCounter = 0U; u32_SignalCounter < orc_Signals.size(); ++u32_SignalCounter)
   {
      const C_OscCanSignal & rc_Signal = orc_Signals[u32_SignalCounter];
      if (rc_Signal.e_MultiplexerType == C_OscCanSignal::eMUX_MULTIPLEXER_SIGNAL)
      {
         if (opu32_MultiplexerIndex != NULL)
         {
            *opu32_MultiplexerIndex = u32_SignalCounter;
         }
         q_Return = true;
         break;
      }
   }

   return q_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Returns all possible multiplexer values

   \param[out]  orc_Values    All multiplexer values
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OscCanMessage::GetMultiplexerValues(std::set<uint16_t> & orc_Values) const
{
   orc_Values.clear();
   uint32_t u32_SignalCounter;

   for (u32_SignalCounter = 0U; u32_SignalCounter < this->c_Signals.size(); ++u32_SignalCounter)
   {
      if (this->c_Signals[u32_SignalCounter].e_MultiplexerType == C_OscCanSignal::eMUX_MULTIPLEXED_SIGNAL)
      {
         orc_Values.insert(this->c_Signals[u32_SignalCounter].u16_MultiplexValue);
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get hashes for signal

   \param[in]  opc_List             Node data pool list containing signal data
   \param[in]  oru32_SignalIndex    Signal index

   \return
   Hashes for signal
*/
//----------------------------------------------------------------------------------------------------------------------
std::vector<uint32_t> C_OscCanMessage::m_GetSignalHashes(const C_OscNodeDataPoolList * const opc_List,
                                                         const uint32_t & oru32_SignalIndex) const
{
   std::vector<uint32_t> c_Retval;
   if (opc_List != NULL)
   {
      if (oru32_SignalIndex < this->c_Signals.size())
      {
         const C_OscCanSignal & rc_Signal = this->c_Signals[oru32_SignalIndex];
         uint32_t u32_HashSig = 0xFFFFFFFFUL;
         rc_Signal.CalcHash(u32_HashSig);
         if (rc_Signal.u32_ComDataElementIndex < opc_List->c_Elements.size())
         {
            const C_OscNodeDataPoolListElement & rc_Element = opc_List->c_Elements[rc_Signal.u32_ComDataElementIndex];
            uint32_t u32_HashSigCo = 0xFFFFFFFFUL;
            rc_Element.CalcHash(u32_HashSigCo);
            c_Retval.push_back(u32_HashSig);
            c_Retval.push_back(u32_HashSigCo);
         }
      }
   }
   return c_Retval;
}
