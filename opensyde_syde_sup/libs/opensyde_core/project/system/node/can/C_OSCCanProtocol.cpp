//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Data class for a node CAN communication via a specific CAN communication protocol (implementation)

   Data class for a node CAN communication via a specific CAN communication protocol

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.h"

#include "stwerrors.h"
#include "C_OSCCanProtocol.h"
#include "CSCLChecksums.h"
#include "TGLUtils.h"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw_types;
using namespace stw_opensyde_core;
using namespace stw_tgl;
using namespace stw_errors;

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
C_OSCCanProtocol::C_OSCCanProtocol(void) :
   e_Type(eLAYER2),
   u32_DataPoolIndex(0)
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Calculates the hash value over all data

   The hash value is a 32 bit CRC value.
   It is not endian-safe, so it should only be used on the same system it is created on.

   \param[in,out] oru32_HashValue    Hash value with init [in] value and result [out] value
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OSCCanProtocol::CalcHash(uint32 & oru32_HashValue) const
{
   stw_scl::C_SCLChecksums::CalcCRC32(&this->e_Type, sizeof(this->e_Type), oru32_HashValue);
   stw_scl::C_SCLChecksums::CalcCRC32(&this->u32_DataPoolIndex, sizeof(this->u32_DataPoolIndex), oru32_HashValue);

   for (uint32 u32_Counter = 0U; u32_Counter < this->c_ComMessages.size(); ++u32_Counter)
   {
      this->c_ComMessages[u32_Counter].CalcHash(oru32_HashValue);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get all signals for one message

   \param[in]  orc_DataPool          Data pool with message signal data
   \param[in]  ou32_InterfaceIndex   Communication interface index
   \param[in]  ou32_MessageIndex     Message index
   \param[in]  oq_IsTx               true: Flag message is tx; false: rx
   \param[out] orc_Signals           Contained signals

   \return
   C_NO_ERR Result valid
   C_RANGE  Either interface, message or signal not found
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_OSCCanProtocol::GetAllSignalsForMessage(const C_OSCNodeDataPool & orc_DataPool,
                                                 const uint32 ou32_InterfaceIndex, const uint32 ou32_MessageIndex,
                                                 const bool oq_IsTx,
                                                 std::vector<const C_OSCNodeDataPoolListElement *> & orc_Signals) const
{
   const C_OSCNodeDataPoolList * const pc_List = h_GetComListConst(orc_DataPool, ou32_InterfaceIndex, oq_IsTx);
   sint32 s32_Retval = C_NO_ERR;

   orc_Signals.clear();

   if (pc_List != NULL)
   {
      if (ou32_InterfaceIndex < this->c_ComMessages.size())
      {
         const C_OSCCanMessageContainer & rc_ComMessageContainer = this->c_ComMessages[ou32_InterfaceIndex];
         const std::vector<C_OSCCanMessage> & rc_ComMessages = rc_ComMessageContainer.GetMessagesConst(oq_IsTx);
         //Check consistency
         if (ou32_MessageIndex < rc_ComMessages.size())
         {
            const C_OSCCanMessage & rc_Message = rc_ComMessages[ou32_MessageIndex];
            for (uint32 u32_ItSignal = 0; u32_ItSignal < rc_Message.c_Signals.size(); ++u32_ItSignal)
            {
               const C_OSCCanSignal & rc_Signal = rc_Message.c_Signals[u32_ItSignal];
               const uint32 u32_ListSignalIndex = rc_Signal.u32_ComDataElementIndex;
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

   \param[in] orc_DataPool         Data pool containing list
   \param[in] ou32_InterfaceIndex  Interface index
   \param[in] oq_IsTx              Flag if Tx list was requested (otherwise Rx)

   \return
   NULL No matching list found
   Else List matching the requested interface and type
*/
//----------------------------------------------------------------------------------------------------------------------
const C_OSCNodeDataPoolList * C_OSCCanProtocol::h_GetComListConst(const C_OSCNodeDataPool & orc_DataPool,
                                                                  const uint32 ou32_InterfaceIndex, const bool oq_IsTx)
{
   const C_OSCNodeDataPoolList * pc_Retval = NULL;
   const sint32 s32_ListIndex = h_GetListIndex(orc_DataPool, ou32_InterfaceIndex, oq_IsTx);

   if (s32_ListIndex >= 0)
   {
      pc_Retval = &orc_DataPool.c_Lists[s32_ListIndex];
   }

   return pc_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get specific list index

   \param[in] orc_DataPool         Data pool containing list
   \param[in] ou32_InterfaceIndex  Interface index
   \param[in] oq_IsTx              Flag if Tx list was requested (otherwise Rx)

   \return
     -1 No matching list found
   >= 0 List matching the requested interface and type
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_OSCCanProtocol::h_GetListIndex(const C_OSCNodeDataPool & orc_DataPool, const uint32 ou32_InterfaceIndex,
                                        const bool oq_IsTx)
{
   sint32 s32_ResultIndex = -1;

   const uint32 u32_ListIndex = ou32_InterfaceIndex * 2UL;

   if ((u32_ListIndex + 1UL) < orc_DataPool.c_Lists.size())
   {
      const C_OSCNodeDataPoolList & rc_List = orc_DataPool.c_Lists[u32_ListIndex];
      if (h_ListIsComTx(rc_List) == oq_IsTx)
      {
         s32_ResultIndex = static_cast<sint32>(u32_ListIndex);
      }
      else
      {
         const C_OSCNodeDataPoolList & rc_List2 = orc_DataPool.c_Lists[static_cast<size_t>(u32_ListIndex + 1)];
         if (h_ListIsComTx(rc_List2) == oq_IsTx)
         {
            s32_ResultIndex = static_cast<sint32>(u32_ListIndex + 1U);
         }
      }
   }

   return s32_ResultIndex;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get specific com list Rx or Tx

   \param[in] orc_DataPool         Data pool containing list
   \param[in] ou32_InterfaceIndex  Interface index
   \param[in] oq_IsTx              Flag if Tx list was requested (otherwise Rx)

   \return
   NULL No matching list found
   Else List matching the requested interface and type
*/
//----------------------------------------------------------------------------------------------------------------------
C_OSCNodeDataPoolList * C_OSCCanProtocol::h_GetComList(C_OSCNodeDataPool & orc_DataPool,
                                                       const uint32 ou32_InterfaceIndex, const bool oq_IsTx)
{
   C_OSCNodeDataPoolList * pc_Retval = NULL;
   const uint32 u32_ListIndex = ou32_InterfaceIndex * 2UL;

   if ((u32_ListIndex + 1UL) < orc_DataPool.c_Lists.size())
   {
      const C_OSCNodeDataPoolList & rc_List = orc_DataPool.c_Lists[u32_ListIndex];
      if (h_ListIsComTx(rc_List) == oq_IsTx)
      {
         pc_Retval = &orc_DataPool.c_Lists[u32_ListIndex];
      }
      else
      {
         const C_OSCNodeDataPoolList & rc_List2 = orc_DataPool.c_Lists[static_cast<size_t>(u32_ListIndex + 1)];
         if (h_ListIsComTx(rc_List2) == oq_IsTx)
         {
            pc_Retval = &orc_DataPool.c_Lists[static_cast<size_t>(u32_ListIndex + 1)];
         }
      }
   }
   return pc_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get index of specific com list Rx or Tx

   \param[in]  orc_DataPool         Data pool containing list
   \param[in]  ou32_InterfaceIndex  Interface index
   \param[in]  oq_IsTx              Flag if Tx list was requested (otherwise Rx)
   \param[out] oru32_ListIndex      List index

   \return
   C_NO_ERR Operation success
   C_RANGE  Operation failure: parameter invalid
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_OSCCanProtocol::h_GetComListIndex(const C_OSCNodeDataPool & orc_DataPool, const uint32 ou32_InterfaceIndex,
                                           const bool oq_IsTx, uint32 & oru32_ListIndex)
{
   sint32 s32_Retval = C_RANGE;
   const uint32 u32_ListIndex = ou32_InterfaceIndex * 2UL;

   if ((u32_ListIndex + 1UL) < orc_DataPool.c_Lists.size())
   {
      const C_OSCNodeDataPoolList & rc_List = orc_DataPool.c_Lists[u32_ListIndex];
      s32_Retval = C_NO_ERR;
      if (h_ListIsComTx(rc_List) == oq_IsTx)
      {
         oru32_ListIndex = u32_ListIndex;
      }
      else
      {
         const C_OSCNodeDataPoolList & rc_List2 = orc_DataPool.c_Lists[static_cast<size_t>(u32_ListIndex + 1)];
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

   \param[in] orc_DataPool         Data pool containing list element
   \param[in] ou32_InterfaceIndex  Interface index
   \param[in] oq_IsTx              Flag if element is part of Tx list (otherwise Rx)
   \param[in] ou32_MessageIndex    Message index
   \param[in] ou32_SignalIndex     Signal index

   \return
   NULL No matching list element found
   Else List element matching the requested interface, type, message and signal
*/
//----------------------------------------------------------------------------------------------------------------------
const C_OSCNodeDataPoolListElement * C_OSCCanProtocol::GetComListElementConst(const C_OSCNodeDataPool & orc_DataPool,
                                                                              const uint32 ou32_InterfaceIndex,
                                                                              const bool oq_IsTx,
                                                                              const uint32 ou32_MessageIndex,
                                                                              const uint32 ou32_SignalIndex) const
{
   const C_OSCNodeDataPoolListElement * pc_Retval = NULL;
   const C_OSCNodeDataPoolList * const pc_List = h_GetComListConst(orc_DataPool, ou32_InterfaceIndex, oq_IsTx);

   if ((pc_List != NULL) && (ou32_InterfaceIndex < this->c_ComMessages.size()))
   {
      const C_OSCCanMessageContainer & rc_MessageContainer = this->c_ComMessages[ou32_InterfaceIndex];
      const std::vector<C_OSCCanMessage> & rc_Messages = rc_MessageContainer.GetMessagesConst(oq_IsTx);
      if (ou32_MessageIndex < rc_Messages.size())
      {
         const C_OSCCanMessage & rc_Message = rc_Messages[ou32_MessageIndex];
         if (ou32_SignalIndex < rc_Message.c_Signals.size())
         {
            const C_OSCCanSignal & rc_Signal = rc_Message.c_Signals[ou32_SignalIndex];
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

   \param[in] orc_DataPool         Data pool containing list element
   \param[in] ou32_InterfaceIndex  Interface index
   \param[in] oq_IsTx              Flag if element is part of Tx list (otherwise Rx)
   \param[in] ou32_MessageIndex    Message index
   \param[in] ou32_SignalIndex     Signal index

   \return
   NULL No matching list element found
   Else List element matching the requested interface, type, message and signal
*/
//----------------------------------------------------------------------------------------------------------------------
C_OSCNodeDataPoolListElement * C_OSCCanProtocol::GetComListElement(C_OSCNodeDataPool & orc_DataPool,
                                                                   const uint32 ou32_InterfaceIndex, const bool oq_IsTx,
                                                                   const uint32 ou32_MessageIndex,
                                                                   const uint32 ou32_SignalIndex) const
{
   C_OSCNodeDataPoolListElement * pc_Retval = NULL;
   C_OSCNodeDataPoolList * const pc_List = h_GetComList(orc_DataPool, ou32_InterfaceIndex, oq_IsTx);

   if ((pc_List != NULL) && (ou32_InterfaceIndex < this->c_ComMessages.size()))
   {
      const C_OSCCanMessageContainer & rc_MessageContainer = this->c_ComMessages[ou32_InterfaceIndex];
      const std::vector<C_OSCCanMessage> & rc_Messages = rc_MessageContainer.GetMessagesConst(oq_IsTx);
      if (ou32_MessageIndex < rc_Messages.size())
      {
         const C_OSCCanMessage & rc_Message = rc_Messages[ou32_MessageIndex];
         if (ou32_SignalIndex < rc_Message.c_Signals.size())
         {
            const C_OSCCanSignal & rc_Signal = rc_Message.c_Signals[ou32_SignalIndex];
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

   \param[in]   orc_List    list to check

   \return
   true  Tx
   false Rx
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_OSCCanProtocol::h_ListIsComTx(const C_OSCNodeDataPoolList & orc_List)
{
   bool q_Retval = false;

   if (orc_List.c_Name.Length() >= 2)
   {
      //Check second to last letter
      if (orc_List.c_Name[static_cast<sintn>(orc_List.c_Name.Length() - 1)] == 'T')
      {
         q_Retval = true;
      }
   }
   return q_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get CAN message DLC offset for signal range check

   \param[in] oe_Type Current protocol type

   \return
   CAN message DLC offset for signal range check
*/
//----------------------------------------------------------------------------------------------------------------------
uint32 C_OSCCanProtocol::h_GetCANMessageValidSignalsDLCOffset(const E_Type oe_Type)
{
   uint32 u32_Retval = 0UL;

   switch (oe_Type)
   {
   case C_OSCCanProtocol::eECES:
      u32_Retval = 2UL;
      break;
   case C_OSCCanProtocol::eLAYER2:
   case C_OSCCanProtocol::eCAN_OPEN_SAFETY:
   default:
      u32_Retval = 0UL;
      break;
   }

   return u32_Retval;
}
