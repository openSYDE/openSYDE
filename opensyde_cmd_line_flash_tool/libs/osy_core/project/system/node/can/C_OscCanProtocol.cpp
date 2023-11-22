//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Data class for a node CAN communication via a specific CAN communication protocol (implementation)

   Data class for a node CAN communication via a specific CAN communication protocol

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.hpp"

#include "stwerrors.hpp"
#include "C_OscCanProtocol.hpp"
#include "C_SclChecksums.hpp"
#include "TglUtils.hpp"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */

using namespace stw::opensyde_core;
using namespace stw::tgl;
using namespace stw::errors;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */
const std::vector<C_OscCanProtocol::E_Type> C_OscCanProtocol::hc_ALL_PROTOCOLS = C_OscCanProtocol::mh_GetAllProtocols();

/* -- Types --------------------------------------------------------------------------------------------------------- */

/* -- Global Variables ---------------------------------------------------------------------------------------------- */

/* -- Module Global Variables --------------------------------------------------------------------------------------- */

/* -- Module Global Function Prototypes ----------------------------------------------------------------------------- */

/* -- Implementation ------------------------------------------------------------------------------------------------ */

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default constructor
*/
//----------------------------------------------------------------------------------------------------------------------
C_OscCanProtocol::C_OscCanProtocol(void) :
   e_Type(eLAYER2),
   u32_DataPoolIndex(0)
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Calculates the hash value over all data

   The hash value is a 32 bit CRC value.
   It is not endian-safe, so it should only be used on the same system it is created on.

   \param[in,out]  oru32_HashValue  Hash value with init [in] value and result [out] value
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OscCanProtocol::CalcHash(uint32_t & oru32_HashValue) const
{
   stw::scl::C_SclChecksums::CalcCRC32(&this->e_Type, sizeof(this->e_Type), oru32_HashValue);
   stw::scl::C_SclChecksums::CalcCRC32(&this->u32_DataPoolIndex, sizeof(this->u32_DataPoolIndex), oru32_HashValue);

   for (uint32_t u32_Counter = 0U; u32_Counter < this->c_ComMessages.size(); ++u32_Counter)
   {
      this->c_ComMessages[u32_Counter].CalcHash(oru32_HashValue);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get all signals for one message

   \param[in]   orc_DataPool           Data pool with message signal data
   \param[in]   ou32_InterfaceIndex    Communication interface index
   \param[in]   ou32_MessageIndex      Message index
   \param[in]   oq_IsTx                true: Flag message is tx; false: rx
   \param[out]  orc_Signals            Contained signals

   \return
   C_NO_ERR Result valid
   C_RANGE  Either interface, message or signal not found
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_OscCanProtocol::GetAllSignalsForMessage(const C_OscNodeDataPool & orc_DataPool,
                                                  const uint32_t ou32_InterfaceIndex, const uint32_t ou32_MessageIndex,
                                                  const bool oq_IsTx,
                                                  std::vector<const C_OscNodeDataPoolListElement *> & orc_Signals) const
{
   const C_OscNodeDataPoolList * const pc_List = h_GetComListConst(orc_DataPool, ou32_InterfaceIndex, oq_IsTx);
   int32_t s32_Retval = C_NO_ERR;

   orc_Signals.clear();

   if (pc_List != NULL)
   {
      if (ou32_InterfaceIndex < this->c_ComMessages.size())
      {
         const C_OscCanMessageContainer & rc_ComMessageContainer = this->c_ComMessages[ou32_InterfaceIndex];
         const std::vector<C_OscCanMessage> & rc_ComMessages = rc_ComMessageContainer.GetMessagesConst(oq_IsTx);
         //Check consistency
         if (ou32_MessageIndex < rc_ComMessages.size())
         {
            const C_OscCanMessage & rc_Message = rc_ComMessages[ou32_MessageIndex];
            for (uint32_t u32_ItSignal = 0; u32_ItSignal < rc_Message.c_Signals.size(); ++u32_ItSignal)
            {
               const C_OscCanSignal & rc_Signal = rc_Message.c_Signals[u32_ItSignal];
               const uint32_t u32_ListSignalIndex = rc_Signal.u32_ComDataElementIndex;
               if (u32_ListSignalIndex < pc_List->c_Elements.size())
               {
                  orc_Signals.push_back(&pc_List->c_Elements[u32_ListSignalIndex]);
               }
               else
               {
                  s32_Retval = C_RANGE;
               }
            }
         }
         else
         {
            s32_Retval = C_RANGE;
         }
      }
      else
      {
         s32_Retval = C_RANGE;
      }
   }
   else
   {
      s32_Retval = C_RANGE;
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get specific com list Rx or Tx

   \param[in]  orc_DataPool         Data pool containing list
   \param[in]  ou32_InterfaceIndex  Interface index
   \param[in]  oq_IsTx              Flag if Tx list was requested (otherwise Rx)

   \return
   NULL No matching list found
   Else List matching the requested interface and type
*/
//----------------------------------------------------------------------------------------------------------------------
const C_OscNodeDataPoolList * C_OscCanProtocol::h_GetComListConst(const C_OscNodeDataPool & orc_DataPool,
                                                                  const uint32_t ou32_InterfaceIndex,
                                                                  const bool oq_IsTx)
{
   const C_OscNodeDataPoolList * pc_Retval = NULL;
   const int32_t s32_ListIndex = h_GetListIndex(orc_DataPool, ou32_InterfaceIndex, oq_IsTx);

   if (s32_ListIndex >= 0)
   {
      pc_Retval = &orc_DataPool.c_Lists[s32_ListIndex];
   }

   return pc_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get specific list index

   \param[in]  orc_DataPool         Data pool containing list
   \param[in]  ou32_InterfaceIndex  Interface index
   \param[in]  oq_IsTx              Flag if Tx list was requested (otherwise Rx)

   \return
     -1 No matching list found
   >= 0 List matching the requested interface and type
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_OscCanProtocol::h_GetListIndex(const C_OscNodeDataPool & orc_DataPool, const uint32_t ou32_InterfaceIndex,
                                         const bool oq_IsTx)
{
   int32_t s32_ResultIndex = -1;

   const uint32_t u32_ListIndex = ou32_InterfaceIndex * 2UL;

   if ((u32_ListIndex + 1UL) < static_cast<uint32_t>(orc_DataPool.c_Lists.size()))
   {
      const C_OscNodeDataPoolList & rc_List = orc_DataPool.c_Lists[u32_ListIndex];
      if (h_ListIsComTx(rc_List) == oq_IsTx)
      {
         s32_ResultIndex = static_cast<int32_t>(u32_ListIndex);
      }
      else
      {
         const C_OscNodeDataPoolList & rc_List2 = orc_DataPool.c_Lists[static_cast<size_t>(u32_ListIndex) + 1];
         if (h_ListIsComTx(rc_List2) == oq_IsTx)
         {
            s32_ResultIndex = static_cast<int32_t>(u32_ListIndex + 1U);
         }
      }
   }

   return s32_ResultIndex;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get specific com list Rx or Tx

   \param[in]  orc_DataPool         Data pool containing list
   \param[in]  ou32_InterfaceIndex  Interface index
   \param[in]  oq_IsTx              Flag if Tx list was requested (otherwise Rx)

   \return
   NULL No matching list found
   Else List matching the requested interface and type
*/
//----------------------------------------------------------------------------------------------------------------------
C_OscNodeDataPoolList * C_OscCanProtocol::h_GetComList(C_OscNodeDataPool & orc_DataPool,
                                                       const uint32_t ou32_InterfaceIndex, const bool oq_IsTx)
{
   C_OscNodeDataPoolList * pc_Retval = NULL;
   const uint32_t u32_ListIndex = ou32_InterfaceIndex * 2UL;

   if ((u32_ListIndex + 1UL) < static_cast<uint32_t>(orc_DataPool.c_Lists.size()))
   {
      const C_OscNodeDataPoolList & rc_List = orc_DataPool.c_Lists[u32_ListIndex];
      if (h_ListIsComTx(rc_List) == oq_IsTx)
      {
         pc_Retval = &orc_DataPool.c_Lists[u32_ListIndex];
      }
      else
      {
         const C_OscNodeDataPoolList & rc_List2 = orc_DataPool.c_Lists[static_cast<size_t>(u32_ListIndex) + 1];
         if (h_ListIsComTx(rc_List2) == oq_IsTx)
         {
            pc_Retval = &orc_DataPool.c_Lists[static_cast<size_t>(u32_ListIndex) + 1];
         }
      }
   }
   return pc_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get index of specific com list Rx or Tx

   \param[in]   orc_DataPool           Data pool containing list
   \param[in]   ou32_InterfaceIndex    Interface index
   \param[in]   oq_IsTx                Flag if Tx list was requested (otherwise Rx)
   \param[out]  oru32_ListIndex        List index

   \return
   C_NO_ERR Operation success
   C_RANGE  Operation failure: parameter invalid
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_OscCanProtocol::h_GetComListIndex(const C_OscNodeDataPool & orc_DataPool, const uint32_t ou32_InterfaceIndex,
                                            const bool oq_IsTx, uint32_t & oru32_ListIndex)
{
   int32_t s32_Retval = C_RANGE;
   const uint32_t u32_ListIndex = ou32_InterfaceIndex * 2UL;

   if ((u32_ListIndex + 1UL) < static_cast<uint32_t>(orc_DataPool.c_Lists.size()))
   {
      const C_OscNodeDataPoolList & rc_List = orc_DataPool.c_Lists[u32_ListIndex];
      s32_Retval = C_NO_ERR;
      if (h_ListIsComTx(rc_List) == oq_IsTx)
      {
         oru32_ListIndex = u32_ListIndex;
      }
      else
      {
         const C_OscNodeDataPoolList & rc_List2 = orc_DataPool.c_Lists[static_cast<size_t>(u32_ListIndex) + 1];
         if (h_ListIsComTx(rc_List2) == oq_IsTx)
         {
            oru32_ListIndex = u32_ListIndex + 1UL;
         }
      }
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get specific com list element

   \param[in]  orc_DataPool         Data pool containing list element
   \param[in]  ou32_InterfaceIndex  Interface index
   \param[in]  oq_IsTx              Flag if element is part of Tx list (otherwise Rx)
   \param[in]  ou32_MessageIndex    Message index
   \param[in]  ou32_SignalIndex     Signal index

   \return
   NULL No matching list element found
   Else List element matching the requested interface, type, message and signal
*/
//----------------------------------------------------------------------------------------------------------------------
const C_OscNodeDataPoolListElement * C_OscCanProtocol::GetComListElementConst(const C_OscNodeDataPool & orc_DataPool,
                                                                              const uint32_t ou32_InterfaceIndex,
                                                                              const bool oq_IsTx,
                                                                              const uint32_t ou32_MessageIndex,
                                                                              const uint32_t ou32_SignalIndex) const
{
   const C_OscNodeDataPoolListElement * pc_Retval = NULL;
   const C_OscNodeDataPoolList * const pc_List = h_GetComListConst(orc_DataPool, ou32_InterfaceIndex, oq_IsTx);

   if ((pc_List != NULL) && (ou32_InterfaceIndex < this->c_ComMessages.size()))
   {
      const C_OscCanMessageContainer & rc_MessageContainer = this->c_ComMessages[ou32_InterfaceIndex];
      const std::vector<C_OscCanMessage> & rc_Messages = rc_MessageContainer.GetMessagesConst(oq_IsTx);
      if (ou32_MessageIndex < rc_Messages.size())
      {
         const C_OscCanMessage & rc_Message = rc_Messages[ou32_MessageIndex];
         if (ou32_SignalIndex < rc_Message.c_Signals.size())
         {
            const C_OscCanSignal & rc_Signal = rc_Message.c_Signals[ou32_SignalIndex];
            if (rc_Signal.u32_ComDataElementIndex < pc_List->c_Elements.size())
            {
               pc_Retval = &pc_List->c_Elements[rc_Signal.u32_ComDataElementIndex];
            }
         }
      }
   }

   return pc_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get specific com list element

   \param[in]  orc_DataPool         Data pool containing list element
   \param[in]  ou32_InterfaceIndex  Interface index
   \param[in]  oq_IsTx              Flag if element is part of Tx list (otherwise Rx)
   \param[in]  ou32_MessageIndex    Message index
   \param[in]  ou32_SignalIndex     Signal index

   \return
   NULL No matching list element found
   Else List element matching the requested interface, type, message and signal
*/
//----------------------------------------------------------------------------------------------------------------------
C_OscNodeDataPoolListElement * C_OscCanProtocol::GetComListElement(C_OscNodeDataPool & orc_DataPool,
                                                                   const uint32_t ou32_InterfaceIndex,
                                                                   const bool oq_IsTx, const uint32_t ou32_MessageIndex,
                                                                   const uint32_t ou32_SignalIndex) const
{
   C_OscNodeDataPoolListElement * pc_Retval = NULL;
   C_OscNodeDataPoolList * const pc_List = h_GetComList(orc_DataPool, ou32_InterfaceIndex, oq_IsTx);

   if ((pc_List != NULL) && (ou32_InterfaceIndex < this->c_ComMessages.size()))
   {
      const C_OscCanMessageContainer & rc_MessageContainer = this->c_ComMessages[ou32_InterfaceIndex];
      const std::vector<C_OscCanMessage> & rc_Messages = rc_MessageContainer.GetMessagesConst(oq_IsTx);
      if (ou32_MessageIndex < rc_Messages.size())
      {
         const C_OscCanMessage & rc_Message = rc_Messages[ou32_MessageIndex];
         if (ou32_SignalIndex < rc_Message.c_Signals.size())
         {
            const C_OscCanSignal & rc_Signal = rc_Message.c_Signals[ou32_SignalIndex];
            if (rc_Signal.u32_ComDataElementIndex < pc_List->c_Elements.size())
            {
               pc_Retval = &pc_List->c_Elements[rc_Signal.u32_ComDataElementIndex];
            }
         }
      }
   }

   return pc_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Check if communication list is of tx type

   \param[in]  orc_List    list to check

   \return
   true  Tx
   false Rx
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_OscCanProtocol::h_ListIsComTx(const C_OscNodeDataPoolList & orc_List)
{
   bool q_Retval = false;

   if (orc_List.c_Name.Length() >= 2)
   {
      //Check second to last letter
      if (orc_List.c_Name[orc_List.c_Name.Length() - 1] == 'T')
      {
         q_Retval = true;
      }
   }
   return q_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get CAN message DLC offset for signal range check

   \param[in]  oe_Type  Current protocol type

   \return
   CAN message DLC offset for signal range check
*/
//----------------------------------------------------------------------------------------------------------------------
uint32_t C_OscCanProtocol::h_GetCanMessageValidSignalsDlcOffset(const E_Type oe_Type)
{
   uint32_t u32_Retval = 0UL;

   switch (oe_Type)
   {
   case C_OscCanProtocol::eECES:
      u32_Retval = 2UL;
      break;
   case C_OscCanProtocol::eLAYER2:
   case C_OscCanProtocol::eCAN_OPEN_SAFETY:
   case C_OscCanProtocol::eCAN_OPEN:
   case C_OscCanProtocol::eJ1939:
   default:
      u32_Retval = 0UL;
      break;
   }

   return u32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get CAN message signals gaps valid flag

   \param[in]  oe_Type  Current protocol type

   \retval   True    Gaps between signals in a CAN message are valid
   \retval   False   Gaps between signals in a CAN message are not valid
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_OscCanProtocol::h_GetCanMessageSignalGapsValid(const E_Type oe_Type)
{
   bool q_Retval;

   switch (oe_Type)
   {
   case C_OscCanProtocol::eCAN_OPEN:
      q_Retval = false;
      break;
   case C_OscCanProtocol::eLAYER2:
   case C_OscCanProtocol::eCAN_OPEN_SAFETY:
   case C_OscCanProtocol::eECES:
   case C_OscCanProtocol::eJ1939:
   default:
      q_Retval = true;
      break;
   }

   return q_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get CAN message signals byte alignment required flag

   \param[in]  oe_Type  Current protocol type

   \retval   True    Byte alignment in a CAN message is required
   \retval   False   Byte alignment in a CAN message is not required
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_OscCanProtocol::h_GetCanMessageSignalByteAlignmentRequired(const E_Type oe_Type)
{
   bool q_Retval;

   switch (oe_Type)
   {
   case C_OscCanProtocol::eCAN_OPEN:
      q_Retval = true;
      break;
   case C_OscCanProtocol::eLAYER2:
   case C_OscCanProtocol::eCAN_OPEN_SAFETY:
   case C_OscCanProtocol::eECES:
   case C_OscCanProtocol::eJ1939:
   default:
      q_Retval = false;
      break;
   }

   return q_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get CAN message signals required flag

   \param[in]  oe_Type  Current protocol type

   \retval   True    CAN message requires at least on signal
   \retval   False   CAN message requires no signals
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_OscCanProtocol::h_GetCanMessageSignalsRequired(const E_Type oe_Type)
{
   bool q_Retval;

   switch (oe_Type)
   {
   case C_OscCanProtocol::eCAN_OPEN:
      q_Retval = true;
      break;
   case C_OscCanProtocol::eLAYER2:
   case C_OscCanProtocol::eCAN_OPEN_SAFETY:
   case C_OscCanProtocol::eECES:
   case C_OscCanProtocol::eJ1939:
   default:
      q_Retval = false;
      break;
   }

   return q_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get all protocols

   \return
   All protocols
*/
//----------------------------------------------------------------------------------------------------------------------
std::vector<C_OscCanProtocol::E_Type> C_OscCanProtocol::mh_GetAllProtocols()
{
   std::vector<C_OscCanProtocol::E_Type> c_Retval;
   c_Retval.push_back(eLAYER2);
   c_Retval.push_back(eCAN_OPEN_SAFETY);
   c_Retval.push_back(eECES);
   c_Retval.push_back(eCAN_OPEN);
   c_Retval.push_back(eJ1939);
   //Check vector indices are same as enum values
   for (uint32_t u32_ItProt = 0UL; u32_ItProt < c_Retval.size(); ++u32_ItProt)
   {
      tgl_assert(static_cast<uint32_t>(c_Retval[u32_ItProt]) == u32_ItProt);
   }
   return c_Retval;
}
