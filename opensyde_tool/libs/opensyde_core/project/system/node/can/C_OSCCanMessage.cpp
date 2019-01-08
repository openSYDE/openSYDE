//-----------------------------------------------------------------------------
/*!
   \internal
   \file
   \brief       Data class for CAN communication message related, additional information (implementation)

   Data class for CAN communication message related, additional information

   \implementation
   project     openSYDE
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     31.03.2017  STW/M.Echtler
   \endimplementation
*/
//-----------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------- */
#include "precomp_headers.h"

#include "C_OSCCanMessage.h"
#include "CSCLChecksums.h"

/* -- Used Namespaces ------------------------------------------------------ */
using namespace stw_types;
using namespace stw_opensyde_core;

/* -- Module Global Constants ---------------------------------------------- */

/* -- Types ---------------------------------------------------------------- */

/* -- Global Variables ----------------------------------------------------- */

/* -- Module Global Variables ---------------------------------------------- */

/* -- Module Global Function Prototypes ------------------------------------ */

/* -- Implementation ------------------------------------------------------- */

//-----------------------------------------------------------------------------
/*!
   \brief   Default constructor

   \created     31.03.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
C_OSCCanMessage::C_OSCCanMessage(void) :
   c_Name("NewMessage"),
   c_Comment(""),
   u32_CanId(0x7FF),
   q_IsExtended(false),
   u16_Dlc(8),
   e_TxMethod(eTX_METHOD_CYCLIC),
   u32_CycleTimeMs(100),
   u16_DelayTimeMs(10),
   u32_TimeoutMs(310)
{
}

//-----------------------------------------------------------------------------
/*!
   \brief   Calculates the hash value over all data

   The hash value is a 32 bit CRC value.

   \param[in,out] oru32_HashValue    Hash value with unit [in] value and result [out] value

   \created     03.04.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_OSCCanMessage::CalcHash(uint32 & oru32_HashValue) const
{
   stw_scl::C_SCLChecksums::CalcCRC32(this->c_Name.c_str(), this->c_Name.Length(), oru32_HashValue);
   stw_scl::C_SCLChecksums::CalcCRC32(this->c_Comment.c_str(), this->c_Comment.Length(), oru32_HashValue);
   stw_scl::C_SCLChecksums::CalcCRC32(&this->u32_CanId, sizeof(this->u32_CanId), oru32_HashValue);
   stw_scl::C_SCLChecksums::CalcCRC32(&this->q_IsExtended, sizeof(this->q_IsExtended), oru32_HashValue);
   stw_scl::C_SCLChecksums::CalcCRC32(&this->u16_Dlc, sizeof(this->u16_Dlc), oru32_HashValue);
   stw_scl::C_SCLChecksums::CalcCRC32(&this->e_TxMethod, sizeof(this->e_TxMethod), oru32_HashValue);
   stw_scl::C_SCLChecksums::CalcCRC32(&this->u32_CycleTimeMs, sizeof(this->u32_CycleTimeMs), oru32_HashValue);
   stw_scl::C_SCLChecksums::CalcCRC32(&this->u16_DelayTimeMs, sizeof(this->u16_DelayTimeMs),
                                      oru32_HashValue);
   stw_scl::C_SCLChecksums::CalcCRC32(&this->u32_TimeoutMs, sizeof(this->u32_TimeoutMs),
                                      oru32_HashValue);

   for (uint32 u32_Counter = 0U; u32_Counter < this->c_Signals.size(); ++u32_Counter)
   {
      this->c_Signals[u32_Counter].CalcHash(oru32_HashValue);
   }
}

//-----------------------------------------------------------------------------
/*!
   \brief   Check error for specified signal

   \param[in]  opc_List                             Node data pool list containing signal data
                                                    (Optional as it is only required by some checks)
   \param[in]  oru32_SignalIndex                    Signal index
   \param[out] opq_LayoutConflict                   Layout conflict
   \param[out] opq_BorderConflict                   Border not usable as variable
   \param[out] opq_NameConflict                     Name conflict
   \param[out] opq_NameInvalid                      Name not usable as variable
   \param[out] opq_MinOverMax                       Minimum value over maximum value
   \param[out] opq_ValueBelowMin                    Init value below minimum
   \param[out] opq_ValueOverMax                     Init value over maximum
   \param[in]  ou32_CANMessageValidSignalsDLCOffset CAN message DLC offset for valid signal range check

   \created     23.02.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_OSCCanMessage::CheckErrorSignal(const C_OSCNodeDataPoolList * const opc_List, const uint32 & oru32_SignalIndex,
                                       bool * const opq_LayoutConflict, bool * const opq_BorderConflict,
                                       bool * const opq_NameConflict, bool * const opq_NameInvalid,
                                       bool * const opq_MinOverMax, bool * const opq_ValueBelowMin,
                                       bool * const opq_ValueOverMax,
                                       const uint32 ou32_CANMessageValidSignalsDLCOffset) const
{
   if (opq_LayoutConflict != NULL)
   {
      *opq_LayoutConflict = false;
   }
   if (opq_BorderConflict != NULL)
   {
      *opq_BorderConflict = false;
   }
   if (oru32_SignalIndex < this->c_Signals.size())
   {
      const C_OSCCanSignal & rc_CheckedSignal = this->c_Signals[oru32_SignalIndex];
      if (opq_LayoutConflict != NULL)
      {
         std::set<uint16> c_CheckedSetPositions;
         *opq_LayoutConflict = false;
         rc_CheckedSignal.GetDataBytesBitPositionsOfSignal(c_CheckedSetPositions);
         for (uint32 u32_ItSignal = 0; u32_ItSignal < this->c_Signals.size(); ++u32_ItSignal)
         {
            const C_OSCCanSignal & rc_CurrentSignal = c_Signals[u32_ItSignal];
            //Skip current signal
            if (u32_ItSignal != oru32_SignalIndex)
            {
               std::set<uint16> c_CurrentSetPositions;
               rc_CurrentSignal.GetDataBytesBitPositionsOfSignal(c_CurrentSetPositions);
               for (std::set<uint16>::const_iterator c_ItCurrentPosition = c_CurrentSetPositions.begin();
                    c_ItCurrentPosition != c_CurrentSetPositions.end();
                    ++c_ItCurrentPosition)
               {
                  for (std::set<uint16>::const_iterator c_ItCheckedPosition = c_CheckedSetPositions.begin();
                       c_ItCheckedPosition != c_CheckedSetPositions.end();
                       ++c_ItCheckedPosition)
                  {
                     if (*c_ItCurrentPosition == *c_ItCheckedPosition)
                     {
                        *opq_LayoutConflict = true;
                     }
                  }
               }
            }
         }
      }
      if (opq_BorderConflict != NULL)
      {
         if (static_cast<uint16>(ou32_CANMessageValidSignalsDLCOffset) <= this->u16_Dlc)
         {
            std::set<uint16> c_SetPositions;
            *opq_BorderConflict = false;
            //Check over end (after dlc)
            rc_CheckedSignal.GetDataBytesBitPositionsOfSignal(c_SetPositions);
            for (std::set<uint16>::const_iterator c_ItPosition = c_SetPositions.begin();
                 c_ItPosition != c_SetPositions.end(); ++c_ItPosition)
            {
               if (static_cast<sint16>(*c_ItPosition) >
                   (static_cast<sint16>((this->u16_Dlc - static_cast<uint16>(ou32_CANMessageValidSignalsDLCOffset)) *
                                        8U) -
                    1))
               {
                  *opq_BorderConflict = true;
               }
            }
         }
         else
         {
            *opq_BorderConflict = true;
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
            *opq_NameConflict = false;
            if (rc_CheckedSignal.u32_ComDataElementIndex < opc_List->c_Elements.size())
            {
               const C_OSCNodeDataPoolListElement & rc_CurrentElement =
                  opc_List->c_Elements[rc_CheckedSignal.u32_ComDataElementIndex];
               for (uint32 u32_ItSignal = 0; u32_ItSignal < this->c_Signals.size(); ++u32_ItSignal)
               {
                  const C_OSCCanSignal & rc_SignalData = c_Signals[u32_ItSignal];
                  //Skip current signal
                  if (u32_ItSignal != oru32_SignalIndex)
                  {
                     if (rc_SignalData.u32_ComDataElementIndex < opc_List->c_Elements.size())
                     {
                        const C_OSCNodeDataPoolListElement & rc_ListElement =
                           opc_List->c_Elements[rc_SignalData.u32_ComDataElementIndex];
                        if (rc_CurrentElement.c_Name.LowerCase() == rc_ListElement.c_Name.LowerCase())
                        {
                           *opq_NameConflict = true;
                        }
                     }
                  }
               }
            }
         }
      }
   }
}
