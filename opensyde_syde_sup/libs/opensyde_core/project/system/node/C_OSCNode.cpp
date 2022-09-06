//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Data container for node data

   Data container for node data

   \copyright   Copyright 2016 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.h"

#include <map>

#include "stwerrors.h"

#include "C_OSCNode.h"
#include "CSCLChecksums.h"
#include "TGLUtils.h"
#include "C_OSCUtils.h"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw_types;
using namespace stw_errors;
using namespace stw_opensyde_core;
using namespace stw_tgl;

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
C_OSCNode::C_OSCNode(void)
{
   this->Initialize();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Default destructor

   Clean up.
*/
//----------------------------------------------------------------------------------------------------------------------
C_OSCNode::~C_OSCNode(void)
{
   pc_DeviceDefinition = NULL;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Initialize class content

   Clean up.
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OSCNode::Initialize(void)
{
   pc_DeviceDefinition = NULL;
   u32_SubDeviceIndex = 0UL;
   c_DeviceType = "Custom";
   c_Properties.Initialize();
   c_DataPools.resize(0);
   q_DatapoolAutoNvMStartAddress = true;
   c_Applications.resize(0);
   c_ComProtocols.resize(0);
   c_HALCConfig.Clear();
   c_CanOpenManagers.clear();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Insert data pool at specified position

   \param[in]  ou32_DataPoolIndex   Data pool index
   \param[in]  orc_DataPool         Data pool data

   \return
   C_NO_ERR Operation success
   C_RANGE  Operation failure: parameter invalid
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_OSCNode::InsertDataPool(const uint32 ou32_DataPoolIndex, const C_OSCNodeDataPool & orc_DataPool)
{
   sint32 s32_Retval = C_NO_ERR;

   if (ou32_DataPoolIndex <= this->c_DataPools.size())
   {
      this->c_DataPools.insert(this->c_DataPools.begin() + ou32_DataPoolIndex, orc_DataPool);
      //Sync data pool index with com protocol
      for (uint32 u32_ItComProtocol = 0; u32_ItComProtocol < this->c_ComProtocols.size(); ++u32_ItComProtocol)
      {
         C_OSCCanProtocol & rc_Protocol = this->c_ComProtocols[u32_ItComProtocol];
         if (rc_Protocol.u32_DataPoolIndex >= ou32_DataPoolIndex)
         {
            ++rc_Protocol.u32_DataPoolIndex;
         }
      }
   }
   else
   {
      s32_Retval = C_RANGE;
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Delete data pool at specified position

   \param[in]  ou32_DataPoolIndex   Data pool index

   \return
   C_NO_ERR Operation success
   C_RANGE  Operation failure: parameter invalid
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_OSCNode::DeleteDataPool(const uint32 ou32_DataPoolIndex)
{
   sint32 s32_Retval = C_NO_ERR;

   if (ou32_DataPoolIndex <= this->c_DataPools.size())
   {
      this->c_DataPools.erase(this->c_DataPools.begin() + ou32_DataPoolIndex);
      //Sync data pool index with com protocol
      for (uint32 u32_ItComProtocol = 0; u32_ItComProtocol < this->c_ComProtocols.size(); ++u32_ItComProtocol)
      {
         C_OSCCanProtocol & rc_Protocol = this->c_ComProtocols[u32_ItComProtocol];
         if (rc_Protocol.u32_DataPoolIndex >= ou32_DataPoolIndex)
         {
            --rc_Protocol.u32_DataPoolIndex;
         }
      }
   }
   else
   {
      s32_Retval = C_RANGE;
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Move datapool in node

   \param[in]  ou32_Start     Start index
   \param[in]  ou32_Target    Target index

   \return
   C_NO_ERR OK
   C_RANGE  Something out of range
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_OSCNode::MoveDataPool(const uint32 ou32_Start, const uint32 ou32_Target)
{
   sint32 s32_Return = C_RANGE;

   if ((ou32_Start < this->c_DataPools.size()) && (ou32_Target < this->c_DataPools.size()))
   {
      //Copy
      const C_OSCNodeDataPool c_Data = this->c_DataPools[ou32_Start];
      C_OSCCanProtocol * pc_Protocol;
      //Can protocol handling
      if (c_Data.e_Type == C_OSCNodeDataPool::eCOM)
      {
         pc_Protocol = this->GetRelatedCANProtocol(ou32_Start);
      }
      else
      {
         pc_Protocol = NULL;
      }
      //Erase
      this->DeleteDataPool(ou32_Start);
      //Insert
      this->InsertDataPool(ou32_Target, c_Data);
      //Can protocol handling
      if (pc_Protocol != NULL)
      {
         pc_Protocol->u32_DataPoolIndex = ou32_Target;
      }

      s32_Return = C_NO_ERR;
   }

   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Insert message at specified position

   \param[in]  oe_ComProtocol       Com protocol
   \param[in]  ou32_InterfaceIndex  Interface index
   \param[in]  ou32_DatapoolIndex   Datapool index
   \param[in]  oq_MessageIsTx       Flag if message is tx
   \param[in]  ou32_MessageIndex    Message index
   \param[in]  orc_Message          Message data
   \param[in]  orc_SignalData       Signal data

   \return
   C_NO_ERR Operation success
   C_RANGE  Operation failure: parameter invalid
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_OSCNode::InsertMessage(const C_OSCCanProtocol::E_Type oe_ComProtocol, const uint32 ou32_InterfaceIndex,
                                const uint32 ou32_DatapoolIndex, const bool oq_MessageIsTx,
                                const uint32 ou32_MessageIndex, const C_OSCCanMessage & orc_Message,
                                const std::vector<C_OSCNodeDataPoolListElement> & orc_SignalData)
{
   sint32 s32_Retval = C_NO_ERR;
   C_OSCCanProtocol * const pc_Protocol = this->GetCANProtocol(oe_ComProtocol, ou32_DatapoolIndex);
   C_OSCNodeDataPool * const pc_DataPool = this->GetComDataPool(oe_ComProtocol, ou32_DatapoolIndex);

   if ((pc_Protocol != NULL) && (pc_DataPool != NULL))
   {
      C_OSCNodeDataPoolList * const pc_ListData = C_OSCCanProtocol::h_GetComList(*pc_DataPool, ou32_InterfaceIndex,
                                                                                 oq_MessageIsTx);
      if ((pc_ListData != NULL) && (ou32_InterfaceIndex < pc_Protocol->c_ComMessages.size()))
      {
         C_OSCCanMessageContainer & rc_MessageContainer = pc_Protocol->c_ComMessages[ou32_InterfaceIndex];
         std::vector<C_OSCCanMessage> & rc_Messages = rc_MessageContainer.GetMessages(oq_MessageIsTx);
         const uint32 u32_DataPoolListElementIndex = rc_MessageContainer.GetMessageSignalDataStartIndex(
            oq_MessageIsTx, ou32_MessageIndex);
         if (ou32_MessageIndex <= rc_Messages.size())
         {
            rc_Messages.insert(rc_Messages.begin() + ou32_MessageIndex, orc_Message);
         }
         else
         {
            s32_Retval = C_RANGE;
         }

         //Smaller equal because appending is also a valid option
         if (u32_DataPoolListElementIndex <= pc_ListData->c_Elements.size())
         {
            for (uint32 u32_ItSignal = 0; u32_ItSignal < orc_SignalData.size(); ++u32_ItSignal)
            {
               C_OSCNodeDataPoolListElement c_Copy = orc_SignalData[u32_ItSignal];
               //Depending on safety flag set appropriate access mode
               if (pc_DataPool->q_IsSafety == true)
               {
                  c_Copy.e_Access = C_OSCNodeDataPoolListElement::eACCESS_RO;
               }
               else
               {
                  c_Copy.e_Access = C_OSCNodeDataPoolListElement::eACCESS_RW;
               }
               pc_ListData->c_Elements.insert(
                  pc_ListData->c_Elements.begin() + u32_DataPoolListElementIndex + u32_ItSignal, c_Copy);
            }
         }
         else
         {
            s32_Retval = C_RANGE;
         }

         //Recalculate affected indices
         rc_MessageContainer.ReCalcDataElementIndices();
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
/*! \brief  Set message at specified position

   \param[in]  oe_ComProtocol       Com protocol
   \param[in]  ou32_InterfaceIndex  Interface index
   \param[in]  ou32_DatapoolIndex   Datapool index
   \param[in]  oq_MessageIsTx       Flag if message is tx
   \param[in]  ou32_MessageIndex    Message index
   \param[in]  orc_Message          Message data
   \param[in]  oq_NewMessageIsTx    true: the message is a Tx message
                                    false: the message is an Rx message
   \param[in]  orc_SignalData       Signal data

   \return
   C_NO_ERR Operation success
   C_RANGE  Operation failure: parameter invalid
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_OSCNode::SetMessage(const C_OSCCanProtocol::E_Type oe_ComProtocol, const uint32 ou32_InterfaceIndex,
                             const uint32 ou32_DatapoolIndex, const bool oq_MessageIsTx, const uint32 ou32_MessageIndex,
                             const C_OSCCanMessage & orc_Message, const bool oq_NewMessageIsTx,
                             const std::vector<C_OSCNodeDataPoolListElement> & orc_SignalData)
{
   sint32 s32_Retval = C_NO_ERR;

   C_OSCCanProtocol * const pc_Protocol = this->GetCANProtocol(oe_ComProtocol, ou32_DatapoolIndex);

   if (pc_Protocol != NULL)
   {
      if (ou32_InterfaceIndex < pc_Protocol->c_ComMessages.size())
      {
         C_OSCCanMessageContainer & rc_MessageContainer = pc_Protocol->c_ComMessages[ou32_InterfaceIndex];
         std::vector<C_OSCCanMessage> & rc_Messages = rc_MessageContainer.GetMessages(oq_MessageIsTx);
         if (oq_MessageIsTx == oq_NewMessageIsTx)
         {
            //Standard set (Move would handle both cases but vector resizing might have a serious performance impact)
            if (ou32_MessageIndex < rc_Messages.size())
            {
               C_OSCNodeDataPool * const pc_DataPool = this->GetComDataPool(oe_ComProtocol, ou32_DatapoolIndex);
               rc_Messages[ou32_MessageIndex] = orc_Message;
               //Handle data pool
               if (pc_DataPool != NULL)
               {
                  C_OSCNodeDataPoolList * const pc_List =
                     C_OSCCanProtocol::h_GetComList(*pc_DataPool, ou32_InterfaceIndex, oq_MessageIsTx);
                  if (pc_List != NULL)
                  {
                     const uint32 u32_DataPoolListElementIndex = rc_MessageContainer.GetMessageSignalDataStartIndex(
                        oq_MessageIsTx, ou32_MessageIndex);
                     //Get previous signal size
                     const uint32 u32_PreviousSignalSize = rc_Messages[ou32_MessageIndex].c_Signals.size();
                     //Delete old signals
                     for (uint32 u32_ItSignal = u32_PreviousSignalSize; u32_ItSignal > 0; --u32_ItSignal)
                     {
                        const uint32 u32_Add = (u32_DataPoolListElementIndex + u32_ItSignal) - 1UL;
                        pc_List->c_Elements.erase(pc_List->c_Elements.begin() + u32_Add);
                     }
                     //Insert new signals
                     for (uint32 u32_ItSignal = 0; u32_ItSignal < orc_SignalData.size(); ++u32_ItSignal)
                     {
                        pc_List->c_Elements.insert(
                           pc_List->c_Elements.begin() + u32_DataPoolListElementIndex + u32_ItSignal,
                           orc_SignalData[u32_ItSignal]);
                     }
                  }
               }
            }
         }
         else
         {
            //Move
            s32_Retval = this->DeleteMessage(oe_ComProtocol, ou32_InterfaceIndex, ou32_DatapoolIndex,
                                             oq_MessageIsTx, ou32_MessageIndex);

            if (s32_Retval == C_NO_ERR)
            {
               const std::vector<C_OSCCanMessage> & rc_MatchingMessages = rc_MessageContainer.GetMessages(
                  oq_NewMessageIsTx);
               const uint32 u32_NewMessageIndex = rc_MatchingMessages.size();
               s32_Retval = this->InsertMessage(oe_ComProtocol, ou32_InterfaceIndex, ou32_DatapoolIndex,
                                                oq_NewMessageIsTx,
                                                u32_NewMessageIndex, orc_Message,
                                                orc_SignalData);
            }
         }
         //One way or the other recalculate data indices
         rc_MessageContainer.ReCalcDataElementIndices();
      }
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Delete message at specified position

   \param[in]  oe_ComProtocol       Com protocol
   \param[in]  ou32_InterfaceIndex  Interface index
   \param[in]  ou32_DatapoolIndex   Datapool index
   \param[in]  oq_MessageIsTx       Flag if message is tx
   \param[in]  ou32_MessageIndex    Message index

   \return
   C_NO_ERR Operation success
   C_RANGE  Operation failure: parameter invalid
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_OSCNode::DeleteMessage(const C_OSCCanProtocol::E_Type oe_ComProtocol, const uint32 ou32_InterfaceIndex,
                                const uint32 ou32_DatapoolIndex, const bool oq_MessageIsTx,
                                const uint32 ou32_MessageIndex)
{
   sint32 s32_Retval = C_NO_ERR;
   C_OSCCanProtocol * const pc_Protocol = this->GetCANProtocol(oe_ComProtocol, ou32_DatapoolIndex);
   C_OSCNodeDataPool * const pc_DataPool = this->GetComDataPool(oe_ComProtocol, ou32_DatapoolIndex);

   if ((pc_Protocol != NULL) && (pc_DataPool != NULL))
   {
      C_OSCNodeDataPoolList * const pc_ListData =
         C_OSCCanProtocol::h_GetComList(*pc_DataPool, ou32_InterfaceIndex, oq_MessageIsTx);
      if ((pc_ListData != NULL) && (ou32_InterfaceIndex < pc_Protocol->c_ComMessages.size()))
      {
         C_OSCCanMessageContainer & rc_MessageContainer = pc_Protocol->c_ComMessages[ou32_InterfaceIndex];
         std::vector<C_OSCCanMessage> & rc_Messages = rc_MessageContainer.GetMessages(oq_MessageIsTx);
         const uint32 u32_DataPoolListElementIndex =
            rc_MessageContainer.GetMessageSignalDataStartIndex(oq_MessageIsTx, ou32_MessageIndex);
         if (ou32_MessageIndex < rc_Messages.size())
         {
            const C_OSCCanMessage & rc_Message = rc_Messages[ou32_MessageIndex];
            if (rc_Message.c_Signals.size() > 0)
            {
               if (u32_DataPoolListElementIndex < pc_ListData->c_Elements.size())
               {
                  for (uint32 u32_ItSignal = rc_Message.c_Signals.size(); u32_ItSignal > 0UL; --u32_ItSignal)
                  {
                     const uint32 u32_Add = (u32_DataPoolListElementIndex + u32_ItSignal) - 1UL;
                     pc_ListData->c_Elements.erase(pc_ListData->c_Elements.begin() + u32_Add);
                  }
               }
               else
               {
                  s32_Retval = C_RANGE;
               }
            }

            //Delete message
            rc_Messages.erase(rc_Messages.begin() + ou32_MessageIndex);

            //Recalculate affected indices
            rc_MessageContainer.ReCalcDataElementIndices();
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
/*! \brief  Insert signal at specified position

   \param[in]  oe_ComProtocol       Com protocol
   \param[in]  ou32_InterfaceIndex  Interface index
   \param[in]  ou32_DatapoolIndex   Datapool index
   \param[in]  oq_MessageIsTx       Flag if message is tx
   \param[in]  ou32_MessageIndex    Message index
   \param[in]  ou32_SignalIndex     Signal index
   \param[in]  orc_Signal           Signal data for message positioning
   \param[in]  orc_SignalData       Signal data for data pool

   \return
   C_NO_ERR Operation success
   C_RANGE  Operation failure: parameter invalid
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_OSCNode::InsertSignal(const C_OSCCanProtocol::E_Type oe_ComProtocol, const uint32 ou32_InterfaceIndex,
                               const uint32 ou32_DatapoolIndex, const bool oq_MessageIsTx,
                               const uint32 ou32_MessageIndex, const uint32 ou32_SignalIndex,
                               const C_OSCCanSignal & orc_Signal, const C_OSCNodeDataPoolListElement & orc_SignalData)
{
   sint32 s32_Retval = C_NO_ERR;
   C_OSCCanProtocol * const pc_Protocol = this->GetCANProtocol(oe_ComProtocol, ou32_DatapoolIndex);
   C_OSCNodeDataPool * const pc_DataPool = this->GetComDataPool(oe_ComProtocol, ou32_DatapoolIndex);

   if ((pc_Protocol != NULL) && (pc_DataPool != NULL))
   {
      C_OSCNodeDataPoolList * const pc_ListData =
         C_OSCCanProtocol::h_GetComList(*pc_DataPool, ou32_InterfaceIndex, oq_MessageIsTx);
      if ((pc_ListData != NULL) && (ou32_InterfaceIndex < pc_Protocol->c_ComMessages.size()))
      {
         C_OSCCanMessageContainer & rc_MessageContainer = pc_Protocol->c_ComMessages[ou32_InterfaceIndex];
         std::vector<C_OSCCanMessage> & rc_Messages = rc_MessageContainer.GetMessages(oq_MessageIsTx);
         if (ou32_MessageIndex < rc_Messages.size())
         {
            const uint32 u32_DataPoolListElementIndex = rc_MessageContainer.GetMessageSignalDataStartIndex(
               oq_MessageIsTx, ou32_MessageIndex) + ou32_SignalIndex;
            C_OSCCanMessage & rc_Message = rc_Messages[ou32_MessageIndex];

            //Smaller equal because appending is also a valid option
            if ((u32_DataPoolListElementIndex <= pc_ListData->c_Elements.size()) &&
                (ou32_SignalIndex <= rc_Message.c_Signals.size()))
            {
               C_OSCNodeDataPoolListElement c_Copy = orc_SignalData;

               //Message
               rc_Message.c_Signals.insert(rc_Message.c_Signals.begin() + ou32_SignalIndex, orc_Signal);

               //Depending on safety flag set appropriate access mode
               if (pc_DataPool->q_IsSafety == true)
               {
                  c_Copy.e_Access = C_OSCNodeDataPoolListElement::eACCESS_RO;
               }
               else
               {
                  c_Copy.e_Access = C_OSCNodeDataPoolListElement::eACCESS_RW;
               }

               //Data pool
               pc_ListData->c_Elements.insert(
                  pc_ListData->c_Elements.begin() + u32_DataPoolListElementIndex,
                  c_Copy);

               //Recalculate affected indices
               rc_MessageContainer.ReCalcDataElementIndices();
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
   }
   else
   {
      s32_Retval = C_RANGE;
   }

   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Set signal at specified position

   \param[in]  oe_ComProtocol       Com protocol
   \param[in]  ou32_InterfaceIndex  Interface index
   \param[in]  ou32_DatapoolIndex   Datapool index
   \param[in]  oq_MessageIsTx       Flag if message is tx
   \param[in]  ou32_MessageIndex    Message index
   \param[in]  ou32_SignalIndex     Signal index
   \param[in]  orc_Signal           Signal data for message positioning
   \param[in]  orc_SignalData       Signal data for data pool

   \return
   C_NO_ERR Operation success
   C_RANGE  Operation failure: parameter invalid
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_OSCNode::SetSignal(const C_OSCCanProtocol::E_Type oe_ComProtocol, const uint32 ou32_InterfaceIndex,
                            const uint32 ou32_DatapoolIndex, const bool oq_MessageIsTx, const uint32 ou32_MessageIndex,
                            const uint32 ou32_SignalIndex, const C_OSCCanSignal & orc_Signal,
                            const C_OSCNodeDataPoolListElement & orc_SignalData)
{
   sint32 s32_Retval = C_NO_ERR;
   C_OSCCanProtocol * const pc_Protocol = this->GetCANProtocol(oe_ComProtocol, ou32_DatapoolIndex);
   C_OSCNodeDataPool * const pc_DataPool = this->GetComDataPool(oe_ComProtocol, ou32_DatapoolIndex);

   if ((pc_Protocol != NULL) && (pc_DataPool != NULL))
   {
      C_OSCNodeDataPoolList * const pc_ListData =
         C_OSCCanProtocol::h_GetComList(*pc_DataPool, ou32_InterfaceIndex, oq_MessageIsTx);
      if ((pc_ListData != NULL) && (ou32_InterfaceIndex < pc_Protocol->c_ComMessages.size()))
      {
         C_OSCCanMessageContainer & rc_MessageContainer = pc_Protocol->c_ComMessages[ou32_InterfaceIndex];
         std::vector<C_OSCCanMessage> & rc_Messages = rc_MessageContainer.GetMessages(oq_MessageIsTx);
         if (ou32_MessageIndex < rc_Messages.size())
         {
            const uint32 u32_DataPoolListElementIndex = rc_MessageContainer.GetMessageSignalDataStartIndex(
               oq_MessageIsTx, ou32_MessageIndex) + ou32_SignalIndex;
            C_OSCCanMessage & rc_Message = rc_Messages[ou32_MessageIndex];

            if ((u32_DataPoolListElementIndex < pc_ListData->c_Elements.size()) &&
                (ou32_SignalIndex < rc_Message.c_Signals.size()))
            {
               //Data pool
               pc_ListData->c_Elements[u32_DataPoolListElementIndex] = orc_SignalData;

               //Message
               rc_Message.c_Signals[ou32_SignalIndex] = orc_Signal;
               //Just in case
               rc_Message.c_Signals[ou32_SignalIndex].u32_ComDataElementIndex = u32_DataPoolListElementIndex;
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
   }
   else
   {
      s32_Retval = C_RANGE;
   }

   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Set signal at specified position

   \param[in]  oe_ComProtocol       Com protocol
   \param[in]  ou32_InterfaceIndex  Interface index
   \param[in]  ou32_DatapoolIndex   Datapool index
   \param[in]  oq_MessageIsTx       Flag if message is tx
   \param[in]  ou32_MessageIndex    Message index
   \param[in]  ou32_SignalIndex     Signal index
   \param[in]  orc_Signal           Signal data for message positioning

   \return
   C_NO_ERR Operation success
   C_RANGE  Operation failure: parameter invalid
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_OSCNode::SetSignalPosition(const C_OSCCanProtocol::E_Type oe_ComProtocol, const uint32 ou32_InterfaceIndex,
                                    const uint32 ou32_DatapoolIndex, const bool oq_MessageIsTx,
                                    const uint32 ou32_MessageIndex, const uint32 ou32_SignalIndex,
                                    const C_OSCCanSignal & orc_Signal)
{
   sint32 s32_Retval = C_NO_ERR;
   C_OSCCanProtocol * const pc_Protocol = this->GetCANProtocol(oe_ComProtocol, ou32_DatapoolIndex);

   if (pc_Protocol != NULL)
   {
      if (ou32_InterfaceIndex < pc_Protocol->c_ComMessages.size())
      {
         C_OSCCanMessageContainer & rc_MessageContainer = pc_Protocol->c_ComMessages[ou32_InterfaceIndex];
         std::vector<C_OSCCanMessage> & rc_Messages = rc_MessageContainer.GetMessages(oq_MessageIsTx);
         if (ou32_MessageIndex < rc_Messages.size())
         {
            const uint32 u32_DataPoolListElementIndex = rc_MessageContainer.GetMessageSignalDataStartIndex(
               oq_MessageIsTx, ou32_MessageIndex) + ou32_SignalIndex;
            C_OSCCanMessage & rc_Message = rc_Messages[ou32_MessageIndex];

            if (ou32_SignalIndex < rc_Message.c_Signals.size())
            {
               //Message
               rc_Message.c_Signals[ou32_SignalIndex] = orc_Signal;
               //Just in case
               rc_Message.c_Signals[ou32_SignalIndex].u32_ComDataElementIndex = u32_DataPoolListElementIndex;
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
   }
   else
   {
      s32_Retval = C_RANGE;
   }

   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Set signal at specified position

   \param[in]  oe_ComProtocol       Com protocol
   \param[in]  ou32_InterfaceIndex  Interface index
   \param[in]  ou32_DatapoolIndex   Datapool index
   \param[in]  oq_MessageIsTx       Flag if message is tx
   \param[in]  ou32_MessageIndex    Message index
   \param[in]  ou32_SignalIndex     Signal index
   \param[in]  ou16_MultiplexValue  New multiplex value

   \return
   C_NO_ERR Operation success
   C_RANGE  Operation failure: parameter invalid
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_OSCNode::SetSignalMUXValue(const C_OSCCanProtocol::E_Type oe_ComProtocol, const uint32 ou32_InterfaceIndex,
                                    const uint32 ou32_DatapoolIndex, const bool oq_MessageIsTx,
                                    const uint32 ou32_MessageIndex, const uint32 ou32_SignalIndex,
                                    const stw_types::uint16 ou16_MultiplexValue)
{
   sint32 s32_Retval = C_NO_ERR;
   C_OSCCanProtocol * const pc_Protocol = this->GetCANProtocol(oe_ComProtocol, ou32_DatapoolIndex);

   if (pc_Protocol != NULL)
   {
      if (ou32_InterfaceIndex < pc_Protocol->c_ComMessages.size())
      {
         C_OSCCanMessageContainer & rc_MessageContainer = pc_Protocol->c_ComMessages[ou32_InterfaceIndex];
         std::vector<C_OSCCanMessage> & rc_Messages = rc_MessageContainer.GetMessages(oq_MessageIsTx);
         if (ou32_MessageIndex < rc_Messages.size())
         {
            C_OSCCanMessage & rc_Message = rc_Messages[ou32_MessageIndex];

            if (ou32_SignalIndex < rc_Message.c_Signals.size())
            {
               //Message
               C_OSCCanSignal & rc_Signal = rc_Message.c_Signals[ou32_SignalIndex];
               rc_Signal.u16_MultiplexValue = ou16_MultiplexValue;
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
   }
   else
   {
      s32_Retval = C_RANGE;
   }

   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Delete signal at specified position

   \param[in]  oe_ComProtocol       Com protocol
   \param[in]  ou32_InterfaceIndex  Interface index
   \param[in]  ou32_DatapoolIndex   Datapool index
   \param[in]  oq_MessageIsTx       Flag if message is tx
   \param[in]  ou32_MessageIndex    Message index
   \param[in]  ou32_SignalIndex     Signal index

   \return
   C_NO_ERR Operation success
   C_RANGE  Operation failure: parameter invalid
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_OSCNode::DeleteSignal(const C_OSCCanProtocol::E_Type oe_ComProtocol, const uint32 ou32_InterfaceIndex,
                               const uint32 ou32_DatapoolIndex, const bool oq_MessageIsTx,
                               const uint32 ou32_MessageIndex, const uint32 ou32_SignalIndex)
{
   sint32 s32_Retval = C_NO_ERR;
   C_OSCCanProtocol * const pc_Protocol = this->GetCANProtocol(oe_ComProtocol, ou32_DatapoolIndex);
   C_OSCNodeDataPool * const pc_DataPool = this->GetComDataPool(oe_ComProtocol, ou32_DatapoolIndex);

   if ((pc_Protocol != NULL) && (pc_DataPool != NULL))
   {
      C_OSCNodeDataPoolList * const pc_ListData = C_OSCCanProtocol::h_GetComList(*pc_DataPool, ou32_InterfaceIndex,
                                                                                 oq_MessageIsTx);
      if ((pc_ListData != NULL) && (ou32_InterfaceIndex < pc_Protocol->c_ComMessages.size()))
      {
         C_OSCCanMessageContainer & rc_MessageContainer = pc_Protocol->c_ComMessages[ou32_InterfaceIndex];
         std::vector<C_OSCCanMessage> & rc_Messages = rc_MessageContainer.GetMessages(oq_MessageIsTx);
         if (ou32_MessageIndex < rc_Messages.size())
         {
            const uint32 u32_DataPoolListElementIndex = rc_MessageContainer.GetMessageSignalDataStartIndex(
               oq_MessageIsTx, ou32_MessageIndex) + ou32_SignalIndex;
            C_OSCCanMessage & rc_Message = rc_Messages[ou32_MessageIndex];

            if ((u32_DataPoolListElementIndex < pc_ListData->c_Elements.size()) &&
                (ou32_SignalIndex < rc_Message.c_Signals.size()))
            {
               //Message
               rc_Message.c_Signals.erase(rc_Message.c_Signals.begin() + ou32_SignalIndex);

               //Data pool
               pc_ListData->c_Elements.erase(pc_ListData->c_Elements.begin() + u32_DataPoolListElementIndex);

               //Recalculate affected indices
               rc_MessageContainer.ReCalcDataElementIndices();
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
   }
   else
   {
      s32_Retval = C_RANGE;
   }

   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Calculates the hash value over all data

   The hash value is a 32 bit CRC value.
   It is not endian-safe, so it should only be used on the same system it is created on.

   \param[in,out]  oru32_HashValue  Hash value with initial [in] value and result [out] value
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OSCNode::CalcHash(uint32 & oru32_HashValue) const
{
   uint32 u32_Counter;

   stw_scl::C_SCLChecksums::CalcCRC32(this->c_DeviceType.c_str(), this->c_DeviceType.Length(), oru32_HashValue);
   stw_scl::C_SCLChecksums::CalcCRC32(&this->q_DatapoolAutoNvMStartAddress, sizeof(this->q_DatapoolAutoNvMStartAddress),
                                      oru32_HashValue);

   this->c_Properties.CalcHash(oru32_HashValue);

   for (u32_Counter = 0U; u32_Counter < this->c_ComProtocols.size(); ++u32_Counter)
   {
      this->c_ComProtocols[u32_Counter].CalcHash(oru32_HashValue);
   }

   for (u32_Counter = 0U; u32_Counter < this->c_DataPools.size(); ++u32_Counter)
   {
      this->c_DataPools[u32_Counter].CalcHash(oru32_HashValue);
   }

   for (u32_Counter = 0U; u32_Counter < this->c_Applications.size(); ++u32_Counter)
   {
      this->c_Applications[u32_Counter].CalcHash(oru32_HashValue);
   }

   this->c_HALCConfig.CalcHash(oru32_HashValue);
   for (std::map<stw_types::uint8, C_OSCCanOpenManagerInfo>::const_iterator c_It = this->c_CanOpenManagers.begin();
        c_It != this->c_CanOpenManagers.end(); ++c_It)
   {
      const uint8 u8_Value = c_It->first;
      stw_scl::C_SCLChecksums::CalcCRC32(&u8_Value, sizeof(u8_Value), oru32_HashValue);
      c_It->second.CalcHash(oru32_HashValue);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get datapool index by type specific datapool index

   \param[in]  oe_DataPoolType         Type of datapool
   \param[in]  ou32_DataPoolTypeIndex  Type specific index of datapool

   \return
   if datapool is found: zero based index
   if datapool is not found: -1
*/
//----------------------------------------------------------------------------------------------------------------------
stw_types::sint32 C_OSCNode::GetDataPoolIndex(const C_OSCNodeDataPool::E_Type oe_DataPoolType,
                                              const stw_types::uint32 ou32_DataPoolTypeIndex) const
{
   sint32 s32_Retval = -1;
   uint32 u32_Index;
   uint32 u32_TypeIndex = 0U;

   for (u32_Index = 0U; u32_Index < this->c_DataPools.size(); ++u32_Index)
   {
      if (this->c_DataPools[u32_Index].e_Type == oe_DataPoolType)
      {
         // count all datapools of the same type
         if (u32_TypeIndex == ou32_DataPoolTypeIndex)
         {
            s32_Retval = u32_Index;
            break;
         }
         ++u32_TypeIndex;
      }
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get Datapool type specific index from Datapool index

   \param[in]  ou32_DataPoolIndex   Data pool index

   \return
   if datapool is found: zero based index
   if datapool is not found or parameter out of range: -1
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_OSCNode::GetDataPoolTypeIndex(const uint32 ou32_DataPoolIndex) const
{
   sint32 s32_Retval = -1;

   if (ou32_DataPoolIndex < this->c_DataPools.size())
   {
      const C_OSCNodeDataPool & rc_Dp = this->c_DataPools[ou32_DataPoolIndex];

      uint32 u32_Counter = 0UL;
      for (uint32 u32_ItDp = 0UL; u32_ItDp < ou32_DataPoolIndex; ++u32_ItDp)
      {
         const C_OSCNodeDataPool & rc_OtherDp = this->c_DataPools[u32_ItDp];
         if (rc_OtherDp.e_Type == rc_Dp.e_Type)
         {
            ++u32_Counter;
         }
      }

      s32_Retval = u32_Counter;
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get number of Datapools of specified type

   \param[in]  oe_DataPoolType   Data pool type

   \return
   Number of Datapools of specified type
*/
//----------------------------------------------------------------------------------------------------------------------
uint32 C_OSCNode::GetDataPoolCount(const C_OSCNodeDataPool::E_Type oe_DataPoolType) const
{
   uint32 u32_Retval = 0U;
   uint32 u32_Index;

   for (u32_Index = 0U; u32_Index < this->c_DataPools.size(); ++u32_Index)
   {
      if (this->c_DataPools[u32_Index].e_Type == oe_DataPoolType)
      {
         ++u32_Retval;
      }
   }
   return u32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Compare node names for greater

   Nodes are compared by name.
   Primary sorting criteria: Name length: Shortest first
   Secondary sorting criteria: First difference in alphabetic ordering

   \param[in]  orc_Node1   Node 1
   \param[in]  orc_Node2   Node 2

   \return
   true:  Node 1 smaller than Node 2
   false: Else
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_OSCNode::h_CompareNameGreater(const C_OSCNode & orc_Node1, const C_OSCNode & orc_Node2)
{
   bool q_Retval;

   if (orc_Node1.c_Properties.c_Name.Length() == orc_Node2.c_Properties.c_Name.Length())
   {
      q_Retval = (orc_Node1.c_Properties.c_Name < orc_Node2.c_Properties.c_Name);
   }
   else
   {
      q_Retval = orc_Node1.c_Properties.c_Name.Length() < orc_Node2.c_Properties.c_Name.Length();
   }
   return q_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get number of bytes occupied by lists in NVM data pools

   \return
   Number of bytes occupied by lists in NVM data pools
*/
//----------------------------------------------------------------------------------------------------------------------
uint32 C_OSCNode::GetListsSize(void) const
{
   uint32 u32_Retval = 0;

   for (uint32 u32_ItDataPool = 0; u32_ItDataPool < this->c_DataPools.size(); ++u32_ItDataPool)
   {
      const C_OSCNodeDataPool & rc_NodeDataPool = this->c_DataPools[u32_ItDataPool];
      if ((rc_NodeDataPool.e_Type == C_OSCNodeDataPool::eNVM) ||
          (rc_NodeDataPool.e_Type == C_OSCNodeDataPool::eHALC_NVM))
      {
         u32_Retval += rc_NodeDataPool.GetListsSize();
      }
   }
   return u32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get number of bytes occupied by NVM data pools

   \return
   Number of bytes occupied by NVM data pools
*/
//----------------------------------------------------------------------------------------------------------------------
uint32 C_OSCNode::GetDataPoolsSize(void) const
{
   uint32 u32_Retval = 0;

   for (uint32 u32_ItDataPool = 0; u32_ItDataPool < this->c_DataPools.size(); ++u32_ItDataPool)
   {
      const C_OSCNodeDataPool & rc_NodeDataPool = this->c_DataPools[u32_ItDataPool];
      if ((rc_NodeDataPool.e_Type == C_OSCNodeDataPool::eNVM) ||
          (rc_NodeDataPool.e_Type == C_OSCNodeDataPool::eHALC_NVM))
      {
         u32_Retval += rc_NodeDataPool.u32_NvMSize;
      }
   }
   return u32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get node data pool absolute index

   \param[in]  ou32_DataPoolIndex   Node data pool index

   \return
   Node data pool absolute index
*/
//----------------------------------------------------------------------------------------------------------------------
uint32 C_OSCNode::GetDataPoolAbsoluteAddress(const uint32 ou32_DataPoolIndex) const
{
   uint32 u32_Retval = 0;

   for (uint32 u32_ItDataPool = 0; (u32_ItDataPool < this->c_DataPools.size()) && (u32_ItDataPool < ou32_DataPoolIndex);
        ++u32_ItDataPool)
   {
      const C_OSCNodeDataPool & rc_NodeDataPool = this->c_DataPools[u32_ItDataPool];
      if ((rc_NodeDataPool.e_Type == C_OSCNodeDataPool::eNVM) ||
          (rc_NodeDataPool.e_Type == C_OSCNodeDataPool::eHALC_NVM))
      {
         u32_Retval += rc_NodeDataPool.u32_NvMSize;
      }
   }
   return u32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get node data pool list absolute index

   \param[in]  ou32_DataPoolIndex   Node data pool index
   \param[in]  ou32_ListIndex       Node data pool list index

   \return
   Node data pool list absolute index
*/
//----------------------------------------------------------------------------------------------------------------------
uint32 C_OSCNode::GetListAbsoluteAddress(const uint32 ou32_DataPoolIndex, const uint32 ou32_ListIndex) const
{
   uint32 u32_Retval = GetDataPoolAbsoluteAddress(ou32_DataPoolIndex);

   if (ou32_DataPoolIndex < this->c_DataPools.size())
   {
      const C_OSCNodeDataPool & rc_NodeDataPool = this->c_DataPools[ou32_DataPoolIndex];

      for (uint32 u32_ItList = 0; (u32_ItList < rc_NodeDataPool.c_Lists.size()) && (u32_ItList < ou32_ListIndex);
           ++u32_ItList)
      {
         const C_OSCNodeDataPoolList & rc_NodeDataPoolList = rc_NodeDataPool.c_Lists[u32_ItList];
         u32_Retval += rc_NodeDataPoolList.u32_NvMSize;
      }
   }
   return u32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get node data pool list element absolute index

   \param[in]  ou32_DataPoolIndex   Node data pool index
   \param[in]  ou32_ListIndex       Node data pool list index
   \param[in]  ou32_ElementIndex    Node data pool list element index

   \return
   Node data pool list element absolute index
*/
//----------------------------------------------------------------------------------------------------------------------
uint32 C_OSCNode::GetElementAbsoluteAddress(const uint32 ou32_DataPoolIndex, const uint32 ou32_ListIndex,
                                            const uint32 ou32_ElementIndex) const
{
   uint32 u32_Retval = GetListAbsoluteAddress(ou32_DataPoolIndex, ou32_ListIndex);

   if (ou32_DataPoolIndex < this->c_DataPools.size())
   {
      const C_OSCNodeDataPool & rc_NodeDataPool = this->c_DataPools[ou32_DataPoolIndex];
      if (ou32_ListIndex < rc_NodeDataPool.c_Lists.size())
      {
         const C_OSCNodeDataPoolList & rc_NodeDataPoolList = rc_NodeDataPool.c_Lists[ou32_ListIndex];

         if (rc_NodeDataPoolList.q_NvMCRCActive == true)
         {
            u32_Retval += 2;
         }
         for (uint32 u32_ItElement = 0;
              (u32_ItElement < rc_NodeDataPoolList.c_Elements.size()) && (u32_ItElement < ou32_ElementIndex);
              ++u32_ItElement)
         {
            const C_OSCNodeDataPoolListElement & rc_NodeDataPoolListElement =
               rc_NodeDataPoolList.c_Elements[u32_ItElement];
            u32_Retval += rc_NodeDataPoolListElement.GetSizeByte();
         }
      }
   }

   return u32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get first HAL data pool of specified type

   Returns the first found HAL datapool (HALC or HALC_NVM) of specified type and ignores all other datapool copies.

   \param[in]  oq_SafeDatapool   Flag to differ HAL safe datapool and HAL nonsafe datapool

   \return
   NULL No matching HAL data pool found
   Else Pointer to HAL data pool of node
*/
//----------------------------------------------------------------------------------------------------------------------
const C_OSCNodeDataPool * C_OSCNode::GetHalDataPoolConst(const bool oq_SafeDatapool) const
{
   const C_OSCNodeDataPool * pc_Retval = NULL;

   for (uint32 u32_ItDatapools = 0; u32_ItDatapools < this->c_DataPools.size(); ++u32_ItDatapools)
   {
      const C_OSCNodeDataPool & rc_Datapool = this->c_DataPools[u32_ItDatapools];
      if (((rc_Datapool.e_Type == C_OSCNodeDataPool::eHALC) || (rc_Datapool.e_Type == C_OSCNodeDataPool::eHALC_NVM)) &&
          (rc_Datapool.q_IsSafety == oq_SafeDatapool))
      {
         pc_Retval = &this->c_DataPools[u32_ItDatapools];
         break;
      }
   }

   return pc_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get communication data pool for specified protocol

   \param[in]  oe_ComProtocol       Communication protocol
   \param[in]  ou32_DataPoolIndex   Datapool index

   \return
   NULL No matching data pool found for communication protocol
   Else Pointer to communication data pool of node
*/
//----------------------------------------------------------------------------------------------------------------------
const C_OSCNodeDataPool * C_OSCNode::GetComDataPoolConst(const C_OSCCanProtocol::E_Type oe_ComProtocol,
                                                         const uint32 ou32_DataPoolIndex) const
{
   const C_OSCNodeDataPool * pc_Retval = NULL;

   for (uint32 u32_ItCANProtocol = 0; (u32_ItCANProtocol < this->c_ComProtocols.size()) && (pc_Retval == NULL);
        ++u32_ItCANProtocol)
   {
      const C_OSCCanProtocol & rc_CANProtocol = this->c_ComProtocols[u32_ItCANProtocol];
      if (rc_CANProtocol.e_Type == oe_ComProtocol)
      {
         if ((rc_CANProtocol.u32_DataPoolIndex == ou32_DataPoolIndex) &&
             (rc_CANProtocol.u32_DataPoolIndex < this->c_DataPools.size()))
         {
            pc_Retval = &this->c_DataPools[rc_CANProtocol.u32_DataPoolIndex];
            break;
         }
      }
   }
   return pc_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get communication data pool for specified protocol

   \param[in]  oe_ComProtocol       Communication protocol
   \param[in]  ou32_DataPoolIndex   Datapool index

   \return
   NULL No matching data pool found for communication protocol
   Else Pointer to communication data pool of node
*/
//----------------------------------------------------------------------------------------------------------------------
C_OSCNodeDataPool * C_OSCNode::GetComDataPool(const C_OSCCanProtocol::E_Type oe_ComProtocol,
                                              const uint32 ou32_DataPoolIndex)
{
   C_OSCNodeDataPool * pc_Retval = NULL;

   for (uint32 u32_ItCANProtocol = 0; (u32_ItCANProtocol < this->c_ComProtocols.size()) && (pc_Retval == NULL);
        ++u32_ItCANProtocol)
   {
      const C_OSCCanProtocol & rc_CANProtocol = this->c_ComProtocols[u32_ItCANProtocol];
      if (rc_CANProtocol.e_Type == oe_ComProtocol)
      {
         if ((rc_CANProtocol.u32_DataPoolIndex == ou32_DataPoolIndex) &&
             (rc_CANProtocol.u32_DataPoolIndex < this->c_DataPools.size()))
         {
            pc_Retval = &this->c_DataPools[rc_CANProtocol.u32_DataPoolIndex];
            break;
         }
      }
   }
   return pc_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get all communication data pools for specified protocol

   \param[in]  oe_ComProtocol    Communication protocol type

   \return
   Empty vector            No matching data found for communication protocol
   Pointer to protocols    Pointers to communication datapools of node
*/
//----------------------------------------------------------------------------------------------------------------------
std::vector<const C_OSCNodeDataPool *> C_OSCNode::GetComDatapoolsConst(const C_OSCCanProtocol::E_Type oe_ComProtocol)
const
{
   std::vector<const C_OSCNodeDataPool *> c_Return;

   for (uint32 u32_ItCANProtocol = 0; u32_ItCANProtocol < this->c_ComProtocols.size(); ++u32_ItCANProtocol)
   {
      const C_OSCCanProtocol & rc_CANProtocol = this->c_ComProtocols[u32_ItCANProtocol];
      if (rc_CANProtocol.e_Type == oe_ComProtocol)
      {
         if (rc_CANProtocol.u32_DataPoolIndex < this->c_DataPools.size())
         {
            c_Return.push_back(&this->c_DataPools[rc_CANProtocol.u32_DataPoolIndex]);
         }
      }
   }

   return c_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get all communication data pools for specified protocol

   \param[in]  oe_ComProtocol    Communication protocol type

   \return
   Empty vector            No matching data found for communication protocol
   Pointer to protocols    Pointers to communication datapools of node
*/
//----------------------------------------------------------------------------------------------------------------------
std::vector<C_OSCNodeDataPool *> C_OSCNode::GetComDataPools(const C_OSCCanProtocol::E_Type oe_ComProtocol)
{
   std::vector<C_OSCNodeDataPool *> c_Return;

   for (uint32 u32_ItCANProtocol = 0; u32_ItCANProtocol < this->c_ComProtocols.size(); ++u32_ItCANProtocol)
   {
      const C_OSCCanProtocol & rc_CANProtocol = this->c_ComProtocols[u32_ItCANProtocol];
      if (rc_CANProtocol.e_Type == oe_ComProtocol)
      {
         if (rc_CANProtocol.u32_DataPoolIndex < this->c_DataPools.size())
         {
            c_Return.push_back(&this->c_DataPools[rc_CANProtocol.u32_DataPoolIndex]);
         }
      }
   }

   return c_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get protocol for specified communication protocol and specified Datapools

   \param[in]  oe_ComProtocol       Communication protocol
   \param[in]  ou32_DataPoolIndex   Datapool index

   \return
   NULL No matching data found for communication protocol
   Else Pointer to communication data of node
*/
//----------------------------------------------------------------------------------------------------------------------
const C_OSCCanProtocol * C_OSCNode::GetCANProtocolConst(const C_OSCCanProtocol::E_Type oe_ComProtocol,
                                                        const uint32 ou32_DataPoolIndex)
const
{
   const C_OSCCanProtocol * pc_Retval = this->GetRelatedCANProtocolConst(ou32_DataPoolIndex);

   if ((pc_Retval != NULL) &&
       (pc_Retval->e_Type != oe_ComProtocol))
   {
      // Protocol does not match
      pc_Retval = NULL;
   }

   return pc_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get protocol for specified communication protocol and specified Datapools

   \param[in]  oe_ComProtocol       Communication protocol
   \param[in]  ou32_DataPoolIndex   Datapool index

   \return
   NULL No matching data found for communication protocol
   Else Pointer to communication data of node
*/
//----------------------------------------------------------------------------------------------------------------------
C_OSCCanProtocol * C_OSCNode::GetCANProtocol(const C_OSCCanProtocol::E_Type oe_ComProtocol,
                                             const uint32 ou32_DataPoolIndex)
{
   C_OSCCanProtocol * pc_Retval =  this->GetRelatedCANProtocol(ou32_DataPoolIndex);

   if ((pc_Retval != NULL) &&
       (pc_Retval->e_Type != oe_ComProtocol))
   {
      // Protocol does not match
      pc_Retval = NULL;
   }

   return pc_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get all protocols for specified communication protocol

   \param[in]  oe_ComProtocol    Communication protocol

   \return
   Empty vector            No matching data found for communication protocol
   Pointer to protocols    Pointers to communication data of node
*/
//----------------------------------------------------------------------------------------------------------------------
std::vector<const C_OSCCanProtocol *> C_OSCNode::GetCANProtocolsConst(const C_OSCCanProtocol::E_Type oe_ComProtocol)
const
{
   std::vector<const C_OSCCanProtocol *> c_Return;

   for (uint32 u32_ItCanProtocol = 0; u32_ItCanProtocol < this->c_ComProtocols.size(); ++u32_ItCanProtocol)
   {
      const C_OSCCanProtocol & rc_Protocol = this->c_ComProtocols[u32_ItCanProtocol];
      if (rc_Protocol.e_Type == oe_ComProtocol)
      {
         c_Return.push_back(&this->c_ComProtocols[u32_ItCanProtocol]);
      }
   }

   return c_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get all protocols for specified communication protocol

   \param[in]  oe_ComProtocol    Communication protocol

   \return
   Empty vector            No matching data found for communication protocol
   Pointer to protocols    Pointers to communication data of node
*/
//----------------------------------------------------------------------------------------------------------------------
std::vector<C_OSCCanProtocol *> C_OSCNode::GetCANProtocols(const C_OSCCanProtocol::E_Type oe_ComProtocol)
{
   std::vector<C_OSCCanProtocol *> c_Return;

   for (uint32 u32_ItCanProtocol = 0; u32_ItCanProtocol < this->c_ComProtocols.size(); ++u32_ItCanProtocol)
   {
      const C_OSCCanProtocol & rc_Protocol = this->c_ComProtocols[u32_ItCanProtocol];
      if (rc_Protocol.e_Type == oe_ComProtocol)
      {
         c_Return.push_back(&this->c_ComProtocols[u32_ItCanProtocol]);
      }
   }

   return c_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Find com protocol for com data pool

   \param[in]  ou32_DataPoolIndex   Data pool index

   \return
   NULL No matching data found for communication protocol
   Else Pointer to communication data of node
*/
//----------------------------------------------------------------------------------------------------------------------
const C_OSCCanProtocol * C_OSCNode::GetRelatedCANProtocolConst(const uint32 ou32_DataPoolIndex) const
{
   const C_OSCCanProtocol * pc_Retval = NULL;

   for (uint32 u32_ItCanProtocol = 0; u32_ItCanProtocol < this->c_ComProtocols.size(); ++u32_ItCanProtocol)
   {
      const C_OSCCanProtocol & rc_Protocol = this->c_ComProtocols[u32_ItCanProtocol];
      if (rc_Protocol.u32_DataPoolIndex == ou32_DataPoolIndex)
      {
         pc_Retval = &this->c_ComProtocols[u32_ItCanProtocol];
         break;
      }
   }

   return pc_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Find com protocol for com data pool

   \param[in]  ou32_DataPoolIndex   Data pool index

   \return
   NULL No matching data found for communication protocol
   Else Pointer to communication data of node
*/
//----------------------------------------------------------------------------------------------------------------------
C_OSCCanProtocol * C_OSCNode::GetRelatedCANProtocol(const uint32 ou32_DataPoolIndex)
{
   C_OSCCanProtocol * pc_Retval = NULL;

   for (uint32 u32_ItCanProtocol = 0; u32_ItCanProtocol < this->c_ComProtocols.size(); ++u32_ItCanProtocol)
   {
      const C_OSCCanProtocol & rc_Protocol = this->c_ComProtocols[u32_ItCanProtocol];
      if (rc_Protocol.u32_DataPoolIndex == ou32_DataPoolIndex)
      {
         pc_Retval = &this->c_ComProtocols[u32_ItCanProtocol];
         break;
      }
   }

   return pc_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Check whether the number of lists and elements is within supported range

   Goes through the data pool lists and element and check whether the number are within the supported range.
   The valid range is limited by the openSYDE communication protocol.

   \param[in]   ou32_DataPoolIndex                    Data pool index
   \param[out]  orq_InvalidNumberOfListsOrElements    true: number of lists or elements of at least one list out of range
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OSCNode::CheckErrorDataPoolNumListsAndElements(const uint32 ou32_DataPoolIndex,
                                                      bool & orq_InvalidNumberOfListsOrElements) const
{
   orq_InvalidNumberOfListsOrElements = true;
   if (ou32_DataPoolIndex < this->c_DataPools.size())
   {
      const C_OSCNodeDataPool & rc_CheckedDataPool = this->c_DataPools[ou32_DataPoolIndex];

      orq_InvalidNumberOfListsOrElements = false;

      //number of lists ok ?
      if (rc_CheckedDataPool.c_Lists.size() > hu32_MAX_NUMBER_OF_LISTS_PER_DATA_POOL)
      {
         //no
         orq_InvalidNumberOfListsOrElements = true;
      }
      else
      {
         //number of elements ok ?
         for (uint32 u32_Index = 0U; u32_Index < rc_CheckedDataPool.c_Lists.size(); u32_Index++)
         {
            if (rc_CheckedDataPool.c_Lists[u32_Index].c_Elements.size() > hu32_MAX_NUMBER_OF_ELEMENTS_PER_LIST)
            {
               orq_InvalidNumberOfListsOrElements = true;
               break;
            }
         }
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Check error for data pool

   Checks for:
   * number of lists within range
   * number of elements per list within range
   * naming: data pool name is invalid
   * naming conflict: data pool has same name as another data pool
   * naming conflict: multiple lists have the same name
   * naming conflict: mulsiple elements within one list have the same name

   \param[in]   ou32_DataPoolIndex                    Data pool index
   \param[out]  opq_NameConflict                      Name conflict
   \param[out]  opq_NameInvalid                       Name not usable as variable
   \param[out]  opq_IsErrorInListOrMessage            true: conflict in any list or message was detected
   \param[out]  opq_InvalidNumberOfListsOrElements    true: invalid number of lists or elements
   \param[out]  opc_InvalidListIndices                Optional storage for list of invalid list indices
                                                      (see opq_IsErrorInListOrMessage)
                                                      If COMM: interface index
                                                      Else: list index
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OSCNode::CheckErrorDataPool(const uint32 ou32_DataPoolIndex, bool * const opq_NameConflict,
                                   bool * const opq_NameInvalid, bool * const opq_IsErrorInListOrMessage,
                                   bool * const opq_InvalidNumberOfListsOrElements,
                                   std::vector<uint32> * const opc_InvalidListIndices) const
{
   if (ou32_DataPoolIndex < this->c_DataPools.size())
   {
      const C_OSCNodeDataPool & rc_CheckedDataPool = this->c_DataPools[ou32_DataPoolIndex];

      //Is the data pool name valid ?
      if (opq_NameInvalid != NULL)
      {
         if (C_OSCUtils::h_CheckValidCName(rc_CheckedDataPool.c_Name) == false)
         {
            *opq_NameInvalid = true;
         }
         else
         {
            *opq_NameInvalid = false;
         }
      }
      //Is the data pool name used more than once ?
      if (opq_NameConflict != NULL)
      {
         *opq_NameConflict = false;
         for (uint32 u32_ItElement = 0; u32_ItElement < this->c_DataPools.size(); ++u32_ItElement)
         {
            if (u32_ItElement != ou32_DataPoolIndex)
            {
               const C_OSCNodeDataPool & rc_CurrentDataPool = this->c_DataPools[u32_ItElement];
               if (rc_CheckedDataPool.c_Name.LowerCase() == rc_CurrentDataPool.c_Name.LowerCase())
               {
                  *opq_NameConflict = true;
                  break;
               }
            }
         }
      }
      //Lists
      if (opq_IsErrorInListOrMessage != NULL)
      {
         //Init
         *opq_IsErrorInListOrMessage = false;

         //Are number of lists and elements within range ?
         if (opq_InvalidNumberOfListsOrElements != NULL)
         {
            CheckErrorDataPoolNumListsAndElements(ou32_DataPoolIndex, *opq_InvalidNumberOfListsOrElements);
         }

         if (rc_CheckedDataPool.e_Type == C_OSCNodeDataPool::eCOM)
         {
            const C_OSCCanProtocol * const pc_Protocol = this->GetRelatedCANProtocolConst(ou32_DataPoolIndex);
            if (pc_Protocol != NULL)
            {
               static std::map<std::vector<uint32>, bool> hc_PreviousResults;
               for (uint32 u32_ItContainer = 0; u32_ItContainer < pc_Protocol->c_ComMessages.size(); ++u32_ItContainer)
               {
                  uint32 u32_TxListIndex;
                  uint32 u32_RxListIndex;
                  if ((C_OSCCanProtocol::h_GetComListIndex(rc_CheckedDataPool, u32_ItContainer, true,
                                                           u32_TxListIndex) == C_NO_ERR) &&
                      (C_OSCCanProtocol::h_GetComListIndex(rc_CheckedDataPool, u32_ItContainer, false,
                                                           u32_RxListIndex) == C_NO_ERR))
                  {
                     //Get Hash for all relevant data
                     const uint32 u32_HashCon = this->m_GetContainerHash(ou32_DataPoolIndex, u32_ItContainer);
                     const uint32 u32_HashTx = this->m_GetListHash(ou32_DataPoolIndex, u32_TxListIndex);
                     const uint32 u32_HashRx = this->m_GetListHash(ou32_DataPoolIndex, u32_RxListIndex);
                     std::vector<uint32> c_Hashes;
                     std::map<std::vector<uint32>, bool>::const_iterator c_It;
                     c_Hashes.push_back(u32_HashCon);
                     c_Hashes.push_back(u32_HashTx);
                     c_Hashes.push_back(u32_HashRx);
                     //Check if check was already performed in the past
                     c_It = hc_PreviousResults.find(c_Hashes);
                     if (c_It == hc_PreviousResults.end())
                     {
                        if ((u32_TxListIndex < rc_CheckedDataPool.c_Lists.size()) &&
                            (u32_RxListIndex < rc_CheckedDataPool.c_Lists.size()))
                        {
                           const C_OSCCanMessageContainer & rc_Container = pc_Protocol->c_ComMessages[u32_ItContainer];
                           const bool q_Error = rc_Container.CheckLocalError(
                              rc_CheckedDataPool.c_Lists[u32_TxListIndex],
                              rc_CheckedDataPool.c_Lists[u32_RxListIndex],
                              C_OSCCanProtocol::h_GetCANMessageValidSignalsDLCOffset(pc_Protocol->e_Type),
                              C_OSCCanProtocol::h_GetCANMessageSignalGapsValid(pc_Protocol->e_Type),
                              C_OSCCanProtocol::h_GetCANMessageSignalByteAlignmentRequired(pc_Protocol->e_Type),
                              C_OSCCanProtocol::h_GetCANMessageSignalsRequired(pc_Protocol->e_Type));
                           if (q_Error == true)
                           {
                              *opq_IsErrorInListOrMessage = true;
                              if (opc_InvalidListIndices != NULL)
                              {
                                 opc_InvalidListIndices->push_back(u32_ItContainer);
                              }
                              //Append for possible reusing this result
                              hc_PreviousResults[c_Hashes] = true;
                           }
                           else
                           {
                              //Append for possible reusing this result
                              hc_PreviousResults[c_Hashes] = false;
                           }
                        }
                        else
                        {
                           *opq_IsErrorInListOrMessage = true;
                           if (opc_InvalidListIndices != NULL)
                           {
                              opc_InvalidListIndices->push_back(u32_ItContainer);
                           }
                           //Append for possible reusing this result
                           hc_PreviousResults[c_Hashes] = true;
                        }
                     }
                     else
                     {
                        //Do not reset error
                        *opq_IsErrorInListOrMessage = (*opq_IsErrorInListOrMessage) || (c_It->second);
                        if ((opc_InvalidListIndices != NULL) && (c_It->second))
                        {
                           opc_InvalidListIndices->push_back(u32_ItContainer);
                        }
                     }
                  }
                  else
                  {
                     *opq_IsErrorInListOrMessage = true;
                  }
               }
            }
         }
         else
         {
            std::map<stw_scl::C_SCLString, uint32> c_PreviousNames;
            static std::map<uint32, bool> hc_PreviousResults;
            bool q_CheckSize;
            bool q_NameInvalid;
            bool q_UsageInvalid;
            bool q_OutOfDataPool;
            bool q_DataSetsInvalid;
            bool q_ElementsInvalid;

            if ((rc_CheckedDataPool.e_Type == C_OSCNodeDataPool::eNVM) ||
                (rc_CheckedDataPool.e_Type == C_OSCNodeDataPool::eHALC_NVM))
            {
               q_CheckSize = true;
            }
            else
            {
               q_CheckSize = false;
            }

            for (uint32 u32_ItList = 0;
                 (u32_ItList < rc_CheckedDataPool.c_Lists.size()) &&
                 ((*opq_IsErrorInListOrMessage == false) || (opc_InvalidListIndices != NULL));
                 ++u32_ItList)
            {
               //Overarching checks
               const C_OSCNodeDataPoolList & rc_List = rc_CheckedDataPool.c_Lists[u32_ItList];
               const std::map<stw_scl::C_SCLString,
                              uint32>::const_iterator c_ItList = c_PreviousNames.find(rc_List.c_Name.LowerCase());
               if (c_ItList != c_PreviousNames.end())
               {
                  *opq_IsErrorInListOrMessage = true;
                  if (opc_InvalidListIndices != NULL)
                  {
                     bool q_Added = false;
                     //Only add element once!
                     for (uint32 u32_It = 0UL; u32_It < opc_InvalidListIndices->size(); ++u32_It)
                     {
                        if ((*opc_InvalidListIndices)[u32_It] == c_ItList->second)
                        {
                           q_Added = true;
                           break;
                        }
                     }
                     //Add conflicting other item
                     if (q_Added == false)
                     {
                        opc_InvalidListIndices->push_back(c_ItList->second);
                     }
                     //Add itself
                     opc_InvalidListIndices->push_back(u32_ItList);
                  }
               }
               else
               {
                  //Get Hash for all relevant data
                  const uint32 u32_Hash = this->m_GetListHash(ou32_DataPoolIndex, u32_ItList);
                  //Check if check was already performed in the past
                  const std::map<uint32, bool>::const_iterator c_ItErr = hc_PreviousResults.find(u32_Hash);
                  //Append new name
                  c_PreviousNames[rc_List.c_Name.LowerCase()] = u32_ItList;
                  //Element specific checks
                  if (c_ItErr == hc_PreviousResults.end())
                  {
                     q_NameInvalid = false;
                     q_UsageInvalid = false;
                     q_OutOfDataPool = false;
                     q_DataSetsInvalid = false;
                     q_ElementsInvalid = false;
                     rc_CheckedDataPool.CheckErrorList(u32_ItList, NULL, &q_NameInvalid, &q_UsageInvalid,
                                                       &q_OutOfDataPool, &q_DataSetsInvalid, &q_ElementsInvalid, NULL,
                                                       NULL);
                     if (q_CheckSize == false)
                     {
                        q_UsageInvalid = false;
                        q_OutOfDataPool = false;
                     }
                     if (((((q_NameInvalid == true) || (q_UsageInvalid == true)) ||
                           (q_ElementsInvalid == true)) || (q_OutOfDataPool == true)) || (q_DataSetsInvalid == true))
                     {
                        *opq_IsErrorInListOrMessage = true;
                        if (opc_InvalidListIndices != NULL)
                        {
                           opc_InvalidListIndices->push_back(u32_ItList);
                        }
                        //Append for possible reusing this result
                        hc_PreviousResults[u32_Hash] = true;
                     }
                     else
                     {
                        //Append for possible reusing this result
                        hc_PreviousResults[u32_Hash] = false;
                     }
                  }
                  else
                  {
                     //Do not reset error
                     *opq_IsErrorInListOrMessage = (*opq_IsErrorInListOrMessage) || (c_ItErr->second);
                     if ((opc_InvalidListIndices != NULL) && (c_ItErr->second))
                     {
                        opc_InvalidListIndices->push_back(u32_ItList);
                     }
                  }
               }
            }
         }
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Check if message id valid

   \param[in]   ou32_InterfaceIndex       Interface index
   \param[in]   orc_MessageId             Message id
   \param[out]  orq_Valid                 Flag if valid
   \param[in]   ope_SkipComProtocol       Optional parameter to skip one index
                                          (Only used if all other optional skip parameters are used as well)
                                          (Use-case: skip current message to avoid conflict with itself)
   \param[in]   opu32_SkipInterfaceIndex  Optional parameter to skip one index
                                          (Only used if all other optional skip parameters are used as well)
                                          (Use-case: skip current message to avoid conflict with itself)
   \param[in]   opq_SkipMessageIsTxFlag   Optional parameter to skip one index
                                          (Only used if all other optional skip parameters are used as well)
                                          (Use-case: skip current message to avoid conflict with itself)
   \param[in]   opu32_SkipMessageIndex    Optional parameter to skip one index
                                          (Only used if all other optional skip parameters are used as well)
                                          (Use-case: skip current message to avoid conflict with itself)
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OSCNode::CheckMessageId(const uint32 ou32_InterfaceIndex, const C_OSCCanMessageUniqueId & orc_MessageId,
                               bool & orq_Valid, const C_OSCCanProtocol::E_Type * const ope_SkipComProtocol,
                               const uint32 * const opu32_SkipInterfaceIndex,
                               const bool * const opq_SkipMessageIsTxFlag,
                               const uint32 * const opu32_SkipMessageIndex) const
{
   std::vector<const C_OSCCanMessage *> c_Messages;
   m_GetAllMessages(ou32_InterfaceIndex, c_Messages, ope_SkipComProtocol, opu32_SkipInterfaceIndex,
                    opq_SkipMessageIsTxFlag, opu32_SkipMessageIndex);
   orq_Valid = true;
   for (uint32 u32_ItMessage = 0; u32_ItMessage < c_Messages.size(); ++u32_ItMessage)
   {
      const C_OSCCanMessage * const pc_CurMessage = c_Messages[u32_ItMessage];
      if (pc_CurMessage != NULL)
      {
         //No skip necessary as they should be already filtered
         if ((pc_CurMessage->u32_CanId == orc_MessageId.u32_CanId) &&
             (pc_CurMessage->q_IsExtended == orc_MessageId.q_IsExtended))
         {
            orq_Valid = false;
            break;
         }
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Check if message name valid (invalid if duplicate or invalid for use as variable)

   \param[in]   ou32_InterfaceIndex       Interface index
   \param[in]   orc_MessageName           Message name
   \param[out]  orq_Valid                 Flag if valid (invalid if duplicate or invalid for use as variable)
   \param[in]   ope_SkipComProtocol       Optional parameter to skip one index
                                          (Only used if all other optional skip parameters are used as well)
                                          (Use-case: skip current message to avoid conflict with itself)
   \param[in]   opu32_SkipInterfaceIndex  Optional parameter to skip one index
                                          (Only used if all other optional skip parameters are used as well)
                                          (Use-case: skip current message to avoid conflict with itself)
   \param[in]   opq_SkipMessageIsTxFlag   Optional parameter to skip one index
                                          (Only used if all other optional skip parameters are used as well)
                                          (Use-case: skip current message to avoid conflict with itself)
   \param[in]   opu32_SkipMessageIndex    Optional parameter to skip one index
                                          (Only used if all other optional skip parameters are used as well)
                                          (Use-case: skip current message to avoid conflict with itself)
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OSCNode::CheckMessageName(const uint32 ou32_InterfaceIndex, const stw_scl::C_SCLString & orc_MessageName,
                                 bool & orq_Valid, const C_OSCCanProtocol::E_Type * const ope_SkipComProtocol,
                                 const uint32 * const opu32_SkipInterfaceIndex,
                                 const bool * const opq_SkipMessageIsTxFlag,
                                 const uint32 * const opu32_SkipMessageIndex) const
{
   if (C_OSCUtils::h_CheckValidCName(orc_MessageName) == true)
   {
      std::vector<const C_OSCCanMessage *> c_Messages;
      m_GetAllMessages(ou32_InterfaceIndex, c_Messages, ope_SkipComProtocol, opu32_SkipInterfaceIndex,
                       opq_SkipMessageIsTxFlag, opu32_SkipMessageIndex);
      orq_Valid = true;
      for (uint32 u32_ItMessage = 0; u32_ItMessage < c_Messages.size(); ++u32_ItMessage)
      {
         const C_OSCCanMessage * const pc_CurMessage = c_Messages[u32_ItMessage];
         if (pc_CurMessage != NULL)
         {
            //No skip necessary as they should be already filtered
            if (pc_CurMessage->c_Name == orc_MessageName)
            {
               orq_Valid = false;
            }
         }
      }
   }
   else
   {
      orq_Valid = false;
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Check application process ID valid

   \param[in]   ou32_ApplicationIndex  Application index (ID)
   \param[out]  orq_Valid              Valid check result (should only be used if the function returned C_NO_ERR)

   \return
   C_NO_ERR Operation success
   C_RANGE  Operation failure: parameter invalid
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_OSCNode::CheckApplicationProcessIdValid(const uint32 ou32_ApplicationIndex, bool & orq_Valid) const
{
   sint32 s32_Retval = C_NO_ERR;

   orq_Valid = true;
   if (ou32_ApplicationIndex < this->c_Applications.size())
   {
      const C_OSCNodeApplication & rc_CheckedApplication = this->c_Applications[ou32_ApplicationIndex];
      //Only check programmable applications
      if (rc_CheckedApplication.e_Type == C_OSCNodeApplication::ePROGRAMMABLE_APPLICATION)
      {
         for (uint32 u32_ItApp = 0; u32_ItApp < this->c_Applications.size(); ++u32_ItApp)
         {
            if (u32_ItApp != ou32_ApplicationIndex)
            {
               const C_OSCNodeApplication & rc_Application = this->c_Applications[u32_ItApp];
               //Only check programmable applications
               if (rc_Application.e_Type == C_OSCNodeApplication::ePROGRAMMABLE_APPLICATION)
               {
                  if (rc_Application.u8_ProcessId == rc_CheckedApplication.u8_ProcessId)
                  {
                     orq_Valid = false;
                  }
               }
            }
         }
      }
   }
   else
   {
      s32_Retval = C_RANGE;
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Check HALC domain configuration valid

   \param[out]     opq_ConfigInvalid         Config invalid
   \param[in,out]  opc_InvalidDomainIndices  Invalid domain indices
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OSCNode::CheckHalcConfigValid(bool * const opq_ConfigInvalid,
                                     std::vector<uint32> * const opc_InvalidDomainIndices) const
{
   this->c_HALCConfig.CheckConfigValid(opq_ConfigInvalid, opc_InvalidDomainIndices);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Recalculate all com protocol data pool indices

   Based on assumption data pools are sorted as can protocols
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OSCNode::ReCalcCanProtocolDataPoolIndices(void)
{
   for (uint32 u32_ItCANProtocol = 0; u32_ItCANProtocol < this->c_ComProtocols.size();
        ++u32_ItCANProtocol)
   {
      C_OSCCanProtocol & rc_CANProtocol = this->c_ComProtocols[u32_ItCANProtocol];
      uint32 u32_ComDataPoolNumber = 0;
      //Find corresponding data pool
      for (uint32 u32_ItDataPool = 0; u32_ItDataPool < this->c_DataPools.size();
           ++u32_ItDataPool)
      {
         const C_OSCNodeDataPool & rc_DataPool = this->c_DataPools[u32_ItDataPool];
         if (rc_DataPool.e_Type == C_OSCNodeDataPool::eCOM)
         {
            if (u32_ItCANProtocol == u32_ComDataPoolNumber)
            {
               rc_CANProtocol.u32_DataPoolIndex = u32_ItDataPool;
               break;
            }
            ++u32_ComDataPoolNumber;
         }
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Check if any interface has update support

   \return
   True  Update available
   False No update available
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_OSCNode::IsAnyUpdateAvailable(void) const
{
   bool q_Retval = false;

   if (this->pc_DeviceDefinition != NULL)
   {
      for (uint32 u32_ItInterface = 0;
           (u32_ItInterface < this->c_Properties.c_ComInterfaces.size()) && (q_Retval == false); ++u32_ItInterface)
      {
         const C_OSCNodeComInterfaceSettings & rc_CurInterface = this->c_Properties.c_ComInterfaces[u32_ItInterface];
         tgl_assert(this->u32_SubDeviceIndex < this->pc_DeviceDefinition->c_SubDevices.size());
         if (this->pc_DeviceDefinition->c_SubDevices[this->u32_SubDeviceIndex].IsUpdateAvailable(rc_CurInterface.
                                                                                                 e_InterfaceType) ==
             true)
         {
            q_Retval = true;
         }
      }
   }
   return q_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Checks if the node supports routing

   Special case: Hybrid nodes can be used with openSYDE or KEFEX server,
   but routing is available with the openSYDE server only.

   \param[in]  oe_Type  Interface type

   \return
   true  Routing is available
   false Routing is not available
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_OSCNode::IsRoutingAvailable(const C_OSCSystemBus::E_Type oe_Type) const
{
   bool q_Return = false;

   if ((this->pc_DeviceDefinition != NULL) &&
       (this->c_Properties.e_DiagnosticServer == C_OSCNodeProperties::eDS_OPEN_SYDE))
   {
      tgl_assert(this->u32_SubDeviceIndex < this->pc_DeviceDefinition->c_SubDevices.size());
      q_Return = this->pc_DeviceDefinition->c_SubDevices[this->u32_SubDeviceIndex].IsRoutingAvailable(oe_Type);
   }

   return q_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Recalculate data pool addresses
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OSCNode::RecalculateAddress(void)
{
   uint32 u32_Offset = 0UL;

   for (uint32 u32_ItDataPool = 0UL; u32_ItDataPool < this->c_DataPools.size(); ++u32_ItDataPool)
   {
      C_OSCNodeDataPool & rc_DataPool = this->c_DataPools[u32_ItDataPool];
      if ((rc_DataPool.e_Type == C_OSCNodeDataPool::eNVM) ||
          (rc_DataPool.e_Type == C_OSCNodeDataPool::eHALC_NVM))
      {
         if (this->q_DatapoolAutoNvMStartAddress == true)
         {
            // Updating the Datapool start address only if the auto mode is activated
            // This configuration affects the Datapool start address only. The lists and elements are calculated
            // automatically always
            rc_DataPool.u32_NvMStartAddress = u32_Offset;
         }
         rc_DataPool.RecalculateAddress();
         u32_Offset += rc_DataPool.u32_NvMSize;
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Utility: get specific data pool element

   Returns pointer to specific data pool element.
   Note that the validity of the returned pointer is only guaranteed as long as the layout of the data pool is not
    changed.

   \param[in]  ou32_DataPoolIndex   data pool index
   \param[in]  ou32_ListIndex       list index
   \param[in]  ou32_ElementIndex    element index

   \return
   Pointer to specified data pool element; NULL if one of the parameters is out of range
*/
//----------------------------------------------------------------------------------------------------------------------
C_OSCNodeDataPoolListElement * C_OSCNode::GetDataPoolListElement(const uint32 ou32_DataPoolIndex,
                                                                 const uint32 ou32_ListIndex,
                                                                 const uint32 ou32_ElementIndex)
{
   C_OSCNodeDataPoolListElement * pc_Element = NULL;

   if ((ou32_DataPoolIndex < this->c_DataPools.size()) &&
       (ou32_ListIndex < this->c_DataPools[ou32_DataPoolIndex].c_Lists.size()) &&
       (ou32_ElementIndex < this->c_DataPools[ou32_DataPoolIndex].c_Lists[ou32_ListIndex].c_Elements.size()))
   {
      pc_Element = &this->c_DataPools[ou32_DataPoolIndex].c_Lists[ou32_ListIndex].c_Elements[ou32_ElementIndex];
   }
   return pc_Element;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get all messages

   \param[in]   ou32_InterfaceIndex       Interface index
   \param[out]  orc_Messages              Output messages
   \param[in]   ope_SkipComProtocol       Optional parameter to skip one index
                                          (Only used if all other optional skip parameters are used as well)
                                          (Use-case: skip current message to avoid conflict with itself)
   \param[in]   opu32_SkipInterfaceIndex  Optional parameter to skip one index
                                          (Only used if all other optional skip parameters are used as well)
                                          (Use-case: skip current message to avoid conflict with itself)
   \param[in]   opq_SkipMessageIsTxFlag   Optional parameter to skip one index
                                          (Only used if all other optional skip parameters are used as well)
                                          (Use-case: skip current message to avoid conflict with itself)
   \param[in]   opu32_SkipMessageIndex    Optional parameter to skip one index
                                          (Only used if all other optional skip parameters are used as well)
                                          (Use-case: skip current message to avoid conflict with itself)
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OSCNode::m_GetAllMessages(const uint32 ou32_InterfaceIndex, std::vector<const C_OSCCanMessage *> & orc_Messages,
                                 const C_OSCCanProtocol::E_Type * const ope_SkipComProtocol,
                                 const uint32 * const opu32_SkipInterfaceIndex,
                                 const bool * const opq_SkipMessageIsTxFlag,
                                 const uint32 * const opu32_SkipMessageIndex) const
{
   //Clear
   orc_Messages.clear();

   //L2
   m_AppendAllProtocolMessages(ou32_InterfaceIndex, C_OSCCanProtocol::eLAYER2, orc_Messages,
                               ope_SkipComProtocol, opu32_SkipInterfaceIndex, opq_SkipMessageIsTxFlag,
                               opu32_SkipMessageIndex);
   //ECeS
   m_AppendAllProtocolMessages(ou32_InterfaceIndex, C_OSCCanProtocol::eECES, orc_Messages,
                               ope_SkipComProtocol, opu32_SkipInterfaceIndex, opq_SkipMessageIsTxFlag,
                               opu32_SkipMessageIndex);
   //ECoS
   m_AppendAllProtocolMessages(ou32_InterfaceIndex, C_OSCCanProtocol::eCAN_OPEN_SAFETY, orc_Messages,
                               ope_SkipComProtocol, opu32_SkipInterfaceIndex, opq_SkipMessageIsTxFlag,
                               opu32_SkipMessageIndex);
   //CAN open manager
   m_AppendAllProtocolMessages(ou32_InterfaceIndex, C_OSCCanProtocol::eCAN_OPEN, orc_Messages,
                               ope_SkipComProtocol, opu32_SkipInterfaceIndex, opq_SkipMessageIsTxFlag,
                               opu32_SkipMessageIndex);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Append all messages for one com protocol

   \param[in]   ou32_InterfaceIndex       Interface index
   \param[in]   oe_ComProtocol            Communication protocol
   \param[out]  orc_Messages              Output messages
   \param[in]   ope_SkipComProtocol       Optional parameter to skip one index
                                          (Only used if all other optional skip parameters are used as well)
                                          (Use-case: skip current message to avoid conflict with itself)
   \param[in]   opu32_SkipInterfaceIndex  Optional parameter to skip one index
                                          (Only used if all other optional skip parameters are used as well)
                                          (Use-case: skip current message to avoid conflict with itself)
   \param[in]   opq_SkipMessageIsTxFlag   Optional parameter to skip one index
                                          (Only used if all other optional skip parameters are used as well)
                                          (Use-case: skip current message to avoid conflict with itself)
   \param[in]   opu32_SkipMessageIndex    Optional parameter to skip one index
                                          (Only used if all other optional skip parameters are used as well)
                                          (Use-case: skip current message to avoid conflict with itself)
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OSCNode::m_AppendAllProtocolMessages(const uint32 ou32_InterfaceIndex,
                                            const C_OSCCanProtocol::E_Type oe_ComProtocol,
                                            std::vector<const C_OSCCanMessage *> & orc_Messages,
                                            const C_OSCCanProtocol::E_Type * const ope_SkipComProtocol,
                                            const uint32 * const opu32_SkipInterfaceIndex,
                                            const bool * const opq_SkipMessageIsTxFlag,
                                            const uint32 * const opu32_SkipMessageIndex) const
{
   for (uint32 u32_ItCANProtocol = 0; u32_ItCANProtocol < this->c_ComProtocols.size(); ++u32_ItCANProtocol)
   {
      const C_OSCCanProtocol & rc_CANProtocol = this->c_ComProtocols[u32_ItCANProtocol];
      if (rc_CANProtocol.e_Type == oe_ComProtocol)
      {
         if (ou32_InterfaceIndex < rc_CANProtocol.c_ComMessages.size())
         {
            const C_OSCCanMessageContainer & rc_MessageContainer =
               rc_CANProtocol.c_ComMessages[ou32_InterfaceIndex];
            for (uint8 u8_ItToggle = 0; u8_ItToggle < 2; ++u8_ItToggle)
            {
               const bool q_IsTx = (u8_ItToggle % 2) == 0;
               const std::vector<C_OSCCanMessage> & rc_Messages = rc_MessageContainer.GetMessagesConst(q_IsTx);
               for (uint32 u32_ItMessage = 0; u32_ItMessage < rc_Messages.size(); ++u32_ItMessage)
               {
                  //Check if skip necessary
                  bool q_Skip = false;
                  if ((((ope_SkipComProtocol != NULL) && (opu32_SkipInterfaceIndex != NULL)) &&
                       (opq_SkipMessageIsTxFlag != NULL)) && (opu32_SkipMessageIndex != NULL))
                  {
                     //Match
                     if ((((*ope_SkipComProtocol == oe_ComProtocol) && (*opu32_SkipMessageIndex == u32_ItMessage)) &&
                          (*opu32_SkipInterfaceIndex == ou32_InterfaceIndex)) &&
                         (((*opq_SkipMessageIsTxFlag == true) && (u8_ItToggle == 0)) ||
                          ((*opq_SkipMessageIsTxFlag == false) && (u8_ItToggle == 1))))
                     {
                        q_Skip = true;
                     }
                  }
                  if (q_Skip == false)
                  {
                     orc_Messages.push_back(&(rc_Messages[u32_ItMessage]));
                  }
               }
            }
         }
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get hash for list

   \param[in]  ou32_DataPoolIndex   Data pool index
   \param[in]  ou32_ListIndex       List index

   \return
   Hash for list
*/
//----------------------------------------------------------------------------------------------------------------------
uint32 C_OSCNode::m_GetListHash(const uint32 ou32_DataPoolIndex, const uint32 ou32_ListIndex) const
{
   uint32 u32_Retval = 0xFFFFFFFFUL;

   if (ou32_DataPoolIndex < this->c_DataPools.size())
   {
      const C_OSCNodeDataPool & rc_Dp = this->c_DataPools[ou32_DataPoolIndex];
      if (ou32_ListIndex < rc_Dp.c_Lists.size())
      {
         const C_OSCNodeDataPoolList & rc_List = rc_Dp.c_Lists[ou32_ListIndex];
         rc_List.CalcHash(u32_Retval);
      }
   }
   return u32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get hash for container

   \param[in]  ou32_DataPoolIndex   Data pool index
   \param[in]  ou32_ContainerIndex  Container index

   \return
   Hash for container
*/
//----------------------------------------------------------------------------------------------------------------------
uint32 C_OSCNode::m_GetContainerHash(const uint32 ou32_DataPoolIndex, const uint32 ou32_ContainerIndex) const
{
   uint32 u32_Retval = 0xFFFFFFFFUL;
   const C_OSCCanProtocol * const pc_Protocol = this->GetRelatedCANProtocolConst(ou32_DataPoolIndex);

   if (pc_Protocol != NULL)
   {
      if (ou32_ContainerIndex < pc_Protocol->c_ComMessages.size())
      {
         const C_OSCCanMessageContainer & rc_Container = pc_Protocol->c_ComMessages[ou32_ContainerIndex];
         rc_Container.CalcHash(u32_Retval);
      }
   }
   return u32_Retval;
}
