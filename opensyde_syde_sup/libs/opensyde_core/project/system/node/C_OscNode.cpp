//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Data container for node data

   Data container for node data

   \copyright   Copyright 2016 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.hpp"

#include <map>

#include "stwerrors.hpp"

#include "C_OscNode.hpp"
#include "C_SclChecksums.hpp"
#include "TglUtils.hpp"
#include "C_OscUtils.hpp"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */

using namespace stw::errors;
using namespace stw::opensyde_core;
using namespace stw::tgl;

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
C_OscNode::C_OscNode(void)
{
   this->Initialize();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Default destructor

   Clean up.
*/
//----------------------------------------------------------------------------------------------------------------------
C_OscNode::~C_OscNode(void)
{
   pc_DeviceDefinition = NULL;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Initialize class content

   Clean up.
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OscNode::Initialize(void)
{
   pc_DeviceDefinition = NULL;
   u32_SubDeviceIndex = 0UL;
   c_DeviceType = "Custom";
   c_Properties.Initialize();
   c_DataPools.resize(0);
   q_DatapoolAutoNvmStartAddress = true;
   c_Applications.resize(0);
   c_ComProtocols.resize(0);
   c_HalcConfig.Clear();
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
int32_t C_OscNode::InsertDataPool(const uint32_t ou32_DataPoolIndex, const C_OscNodeDataPool & orc_DataPool)
{
   int32_t s32_Retval = C_NO_ERR;

   if (ou32_DataPoolIndex <= this->c_DataPools.size())
   {
      this->c_DataPools.insert(this->c_DataPools.begin() + ou32_DataPoolIndex, orc_DataPool);
      //Sync data pool index with com protocol
      for (uint32_t u32_ItComProtocol = 0; u32_ItComProtocol < this->c_ComProtocols.size(); ++u32_ItComProtocol)
      {
         C_OscCanProtocol & rc_Protocol = this->c_ComProtocols[u32_ItComProtocol];
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
int32_t C_OscNode::DeleteDataPool(const uint32_t ou32_DataPoolIndex)
{
   int32_t s32_Retval = C_NO_ERR;

   if (ou32_DataPoolIndex <= this->c_DataPools.size())
   {
      this->c_DataPools.erase(this->c_DataPools.begin() + ou32_DataPoolIndex);
      //Sync data pool index with com protocol
      for (uint32_t u32_ItComProtocol = 0; u32_ItComProtocol < this->c_ComProtocols.size(); ++u32_ItComProtocol)
      {
         C_OscCanProtocol & rc_Protocol = this->c_ComProtocols[u32_ItComProtocol];
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
int32_t C_OscNode::MoveDataPool(const uint32_t ou32_Start, const uint32_t ou32_Target)
{
   int32_t s32_Return = C_RANGE;

   if ((ou32_Start < this->c_DataPools.size()) && (ou32_Target < this->c_DataPools.size()))
   {
      //Copy
      const C_OscNodeDataPool c_Data = this->c_DataPools[ou32_Start];
      C_OscCanProtocol * pc_Protocol;
      //Can protocol handling
      if (c_Data.e_Type == C_OscNodeDataPool::eCOM)
      {
         pc_Protocol = this->GetRelatedCanProtocol(ou32_Start);
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
int32_t C_OscNode::InsertMessage(const C_OscCanProtocol::E_Type oe_ComProtocol, const uint32_t ou32_InterfaceIndex,
                                 const uint32_t ou32_DatapoolIndex, const bool oq_MessageIsTx,
                                 const uint32_t ou32_MessageIndex, const C_OscCanMessage & orc_Message,
                                 const std::vector<C_OscNodeDataPoolListElement> & orc_SignalData)
{
   int32_t s32_Retval = C_NO_ERR;
   C_OscCanProtocol * const pc_Protocol = this->GetCanProtocol(oe_ComProtocol, ou32_DatapoolIndex);
   C_OscNodeDataPool * const pc_DataPool = this->GetComDataPool(oe_ComProtocol, ou32_DatapoolIndex);

   if ((pc_Protocol != NULL) && (pc_DataPool != NULL))
   {
      C_OscNodeDataPoolList * const pc_ListData = C_OscCanProtocol::h_GetComList(*pc_DataPool, ou32_InterfaceIndex,
                                                                                 oq_MessageIsTx);
      if ((pc_ListData != NULL) && (ou32_InterfaceIndex < pc_Protocol->c_ComMessages.size()))
      {
         C_OscCanMessageContainer & rc_MessageContainer = pc_Protocol->c_ComMessages[ou32_InterfaceIndex];
         std::vector<C_OscCanMessage> & rc_Messages = rc_MessageContainer.GetMessages(oq_MessageIsTx);
         const uint32_t u32_DataPoolListElementIndex = rc_MessageContainer.GetMessageSignalDataStartIndex(
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
            for (uint32_t u32_ItSignal = 0; u32_ItSignal < orc_SignalData.size(); ++u32_ItSignal)
            {
               C_OscNodeDataPoolListElement c_Copy = orc_SignalData[u32_ItSignal];
               //Depending on safety flag set appropriate access mode
               if (pc_DataPool->q_IsSafety == true)
               {
                  c_Copy.e_Access = C_OscNodeDataPoolListElement::eACCESS_RO;
               }
               else
               {
                  c_Copy.e_Access = C_OscNodeDataPoolListElement::eACCESS_RW;
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
int32_t C_OscNode::SetMessage(const C_OscCanProtocol::E_Type oe_ComProtocol, const uint32_t ou32_InterfaceIndex,
                              const uint32_t ou32_DatapoolIndex, const bool oq_MessageIsTx,
                              const uint32_t ou32_MessageIndex, const C_OscCanMessage & orc_Message,
                              const bool oq_NewMessageIsTx,
                              const std::vector<C_OscNodeDataPoolListElement> & orc_SignalData)
{
   int32_t s32_Retval = C_NO_ERR;

   C_OscCanProtocol * const pc_Protocol = this->GetCanProtocol(oe_ComProtocol, ou32_DatapoolIndex);

   if (pc_Protocol != NULL)
   {
      if (ou32_InterfaceIndex < pc_Protocol->c_ComMessages.size())
      {
         C_OscCanMessageContainer & rc_MessageContainer = pc_Protocol->c_ComMessages[ou32_InterfaceIndex];
         std::vector<C_OscCanMessage> & rc_Messages = rc_MessageContainer.GetMessages(oq_MessageIsTx);
         if (oq_MessageIsTx == oq_NewMessageIsTx)
         {
            //Standard set (Move would handle both cases but vector resizing might have a serious performance impact)
            if (ou32_MessageIndex < rc_Messages.size())
            {
               C_OscNodeDataPool * const pc_DataPool = this->GetComDataPool(oe_ComProtocol, ou32_DatapoolIndex);
               rc_Messages[ou32_MessageIndex] = orc_Message;
               //Handle data pool
               if (pc_DataPool != NULL)
               {
                  C_OscNodeDataPoolList * const pc_List =
                     C_OscCanProtocol::h_GetComList(*pc_DataPool, ou32_InterfaceIndex, oq_MessageIsTx);
                  if (pc_List != NULL)
                  {
                     const uint32_t u32_DataPoolListElementIndex = rc_MessageContainer.GetMessageSignalDataStartIndex(
                        oq_MessageIsTx, ou32_MessageIndex);
                     //Get previous signal size
                     const uint32_t u32_PreviousSignalSize =
                        static_cast<uint32_t>(rc_Messages[ou32_MessageIndex].c_Signals.size());
                     //Delete old signals
                     for (uint32_t u32_ItSignal = u32_PreviousSignalSize; u32_ItSignal > 0; --u32_ItSignal)
                     {
                        const uint32_t u32_Add = (u32_DataPoolListElementIndex + u32_ItSignal) - 1UL;
                        pc_List->c_Elements.erase(pc_List->c_Elements.begin() + u32_Add);
                     }
                     //Insert new signals
                     for (uint32_t u32_ItSignal = 0; u32_ItSignal < orc_SignalData.size(); ++u32_ItSignal)
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
               const std::vector<C_OscCanMessage> & rc_MatchingMessages = rc_MessageContainer.GetMessages(
                  oq_NewMessageIsTx);
               const uint32_t u32_NewMessageIndex = static_cast<uint32_t>(rc_MatchingMessages.size());
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
int32_t C_OscNode::DeleteMessage(const C_OscCanProtocol::E_Type oe_ComProtocol, const uint32_t ou32_InterfaceIndex,
                                 const uint32_t ou32_DatapoolIndex, const bool oq_MessageIsTx,
                                 const uint32_t ou32_MessageIndex)
{
   int32_t s32_Retval = C_NO_ERR;
   C_OscCanProtocol * const pc_Protocol = this->GetCanProtocol(oe_ComProtocol, ou32_DatapoolIndex);
   C_OscNodeDataPool * const pc_DataPool = this->GetComDataPool(oe_ComProtocol, ou32_DatapoolIndex);

   if ((pc_Protocol != NULL) && (pc_DataPool != NULL))
   {
      C_OscNodeDataPoolList * const pc_ListData =
         C_OscCanProtocol::h_GetComList(*pc_DataPool, ou32_InterfaceIndex, oq_MessageIsTx);
      if ((pc_ListData != NULL) && (ou32_InterfaceIndex < pc_Protocol->c_ComMessages.size()))
      {
         C_OscCanMessageContainer & rc_MessageContainer = pc_Protocol->c_ComMessages[ou32_InterfaceIndex];
         std::vector<C_OscCanMessage> & rc_Messages = rc_MessageContainer.GetMessages(oq_MessageIsTx);
         const uint32_t u32_DataPoolListElementIndex =
            rc_MessageContainer.GetMessageSignalDataStartIndex(oq_MessageIsTx, ou32_MessageIndex);
         if (ou32_MessageIndex < rc_Messages.size())
         {
            const C_OscCanMessage & rc_Message = rc_Messages[ou32_MessageIndex];
            if (rc_Message.c_Signals.size() > 0)
            {
               if (u32_DataPoolListElementIndex < pc_ListData->c_Elements.size())
               {
                  for (uint32_t u32_ItSignal = static_cast<uint32_t>(rc_Message.c_Signals.size()); u32_ItSignal > 0UL;
                       --u32_ItSignal)
                  {
                     const uint32_t u32_Add = (u32_DataPoolListElementIndex + u32_ItSignal) - 1UL;
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
int32_t C_OscNode::InsertSignal(const C_OscCanProtocol::E_Type oe_ComProtocol, const uint32_t ou32_InterfaceIndex,
                                const uint32_t ou32_DatapoolIndex, const bool oq_MessageIsTx,
                                const uint32_t ou32_MessageIndex, const uint32_t ou32_SignalIndex,
                                const C_OscCanSignal & orc_Signal, const C_OscNodeDataPoolListElement & orc_SignalData)
{
   int32_t s32_Retval = C_NO_ERR;
   C_OscCanProtocol * const pc_Protocol = this->GetCanProtocol(oe_ComProtocol, ou32_DatapoolIndex);
   C_OscNodeDataPool * const pc_DataPool = this->GetComDataPool(oe_ComProtocol, ou32_DatapoolIndex);

   if ((pc_Protocol != NULL) && (pc_DataPool != NULL))
   {
      C_OscNodeDataPoolList * const pc_ListData =
         C_OscCanProtocol::h_GetComList(*pc_DataPool, ou32_InterfaceIndex, oq_MessageIsTx);
      if ((pc_ListData != NULL) && (ou32_InterfaceIndex < pc_Protocol->c_ComMessages.size()))
      {
         C_OscCanMessageContainer & rc_MessageContainer = pc_Protocol->c_ComMessages[ou32_InterfaceIndex];
         std::vector<C_OscCanMessage> & rc_Messages = rc_MessageContainer.GetMessages(oq_MessageIsTx);
         if (ou32_MessageIndex < rc_Messages.size())
         {
            const uint32_t u32_DataPoolListElementIndex = rc_MessageContainer.GetMessageSignalDataStartIndex(
               oq_MessageIsTx, ou32_MessageIndex) + ou32_SignalIndex;
            C_OscCanMessage & rc_Message = rc_Messages[ou32_MessageIndex];

            //Smaller equal because appending is also a valid option
            if ((u32_DataPoolListElementIndex <= pc_ListData->c_Elements.size()) &&
                (ou32_SignalIndex <= rc_Message.c_Signals.size()))
            {
               C_OscNodeDataPoolListElement c_Copy = orc_SignalData;

               //Message
               rc_Message.c_Signals.insert(rc_Message.c_Signals.begin() + ou32_SignalIndex, orc_Signal);

               //Depending on safety flag set appropriate access mode
               if (pc_DataPool->q_IsSafety == true)
               {
                  c_Copy.e_Access = C_OscNodeDataPoolListElement::eACCESS_RO;
               }
               else
               {
                  c_Copy.e_Access = C_OscNodeDataPoolListElement::eACCESS_RW;
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
int32_t C_OscNode::SetSignal(const C_OscCanProtocol::E_Type oe_ComProtocol, const uint32_t ou32_InterfaceIndex,
                             const uint32_t ou32_DatapoolIndex, const bool oq_MessageIsTx,
                             const uint32_t ou32_MessageIndex, const uint32_t ou32_SignalIndex,
                             const C_OscCanSignal & orc_Signal, const C_OscNodeDataPoolListElement & orc_SignalData)
{
   int32_t s32_Retval = C_NO_ERR;
   C_OscCanProtocol * const pc_Protocol = this->GetCanProtocol(oe_ComProtocol, ou32_DatapoolIndex);
   C_OscNodeDataPool * const pc_DataPool = this->GetComDataPool(oe_ComProtocol, ou32_DatapoolIndex);

   if ((pc_Protocol != NULL) && (pc_DataPool != NULL))
   {
      C_OscNodeDataPoolList * const pc_ListData =
         C_OscCanProtocol::h_GetComList(*pc_DataPool, ou32_InterfaceIndex, oq_MessageIsTx);
      if ((pc_ListData != NULL) && (ou32_InterfaceIndex < pc_Protocol->c_ComMessages.size()))
      {
         C_OscCanMessageContainer & rc_MessageContainer = pc_Protocol->c_ComMessages[ou32_InterfaceIndex];
         std::vector<C_OscCanMessage> & rc_Messages = rc_MessageContainer.GetMessages(oq_MessageIsTx);
         if (ou32_MessageIndex < rc_Messages.size())
         {
            const uint32_t u32_DataPoolListElementIndex = rc_MessageContainer.GetMessageSignalDataStartIndex(
               oq_MessageIsTx, ou32_MessageIndex) + ou32_SignalIndex;
            C_OscCanMessage & rc_Message = rc_Messages[ou32_MessageIndex];

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
int32_t C_OscNode::SetSignalPosition(const C_OscCanProtocol::E_Type oe_ComProtocol, const uint32_t ou32_InterfaceIndex,
                                     const uint32_t ou32_DatapoolIndex, const bool oq_MessageIsTx,
                                     const uint32_t ou32_MessageIndex, const uint32_t ou32_SignalIndex,
                                     const C_OscCanSignal & orc_Signal)
{
   int32_t s32_Retval = C_NO_ERR;
   C_OscCanProtocol * const pc_Protocol = this->GetCanProtocol(oe_ComProtocol, ou32_DatapoolIndex);

   if (pc_Protocol != NULL)
   {
      if (ou32_InterfaceIndex < pc_Protocol->c_ComMessages.size())
      {
         C_OscCanMessageContainer & rc_MessageContainer = pc_Protocol->c_ComMessages[ou32_InterfaceIndex];
         std::vector<C_OscCanMessage> & rc_Messages = rc_MessageContainer.GetMessages(oq_MessageIsTx);
         if (ou32_MessageIndex < rc_Messages.size())
         {
            const uint32_t u32_DataPoolListElementIndex = rc_MessageContainer.GetMessageSignalDataStartIndex(
               oq_MessageIsTx, ou32_MessageIndex) + ou32_SignalIndex;
            C_OscCanMessage & rc_Message = rc_Messages[ou32_MessageIndex];

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
int32_t C_OscNode::SetSignalMuxValue(const C_OscCanProtocol::E_Type oe_ComProtocol, const uint32_t ou32_InterfaceIndex,
                                     const uint32_t ou32_DatapoolIndex, const bool oq_MessageIsTx,
                                     const uint32_t ou32_MessageIndex, const uint32_t ou32_SignalIndex,
                                     const uint16_t ou16_MultiplexValue)
{
   int32_t s32_Retval = C_NO_ERR;
   C_OscCanProtocol * const pc_Protocol = this->GetCanProtocol(oe_ComProtocol, ou32_DatapoolIndex);

   if (pc_Protocol != NULL)
   {
      if (ou32_InterfaceIndex < pc_Protocol->c_ComMessages.size())
      {
         C_OscCanMessageContainer & rc_MessageContainer = pc_Protocol->c_ComMessages[ou32_InterfaceIndex];
         std::vector<C_OscCanMessage> & rc_Messages = rc_MessageContainer.GetMessages(oq_MessageIsTx);
         if (ou32_MessageIndex < rc_Messages.size())
         {
            C_OscCanMessage & rc_Message = rc_Messages[ou32_MessageIndex];

            if (ou32_SignalIndex < rc_Message.c_Signals.size())
            {
               //Message
               C_OscCanSignal & rc_Signal = rc_Message.c_Signals[ou32_SignalIndex];
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
int32_t C_OscNode::DeleteSignal(const C_OscCanProtocol::E_Type oe_ComProtocol, const uint32_t ou32_InterfaceIndex,
                                const uint32_t ou32_DatapoolIndex, const bool oq_MessageIsTx,
                                const uint32_t ou32_MessageIndex, const uint32_t ou32_SignalIndex)
{
   int32_t s32_Retval = C_NO_ERR;
   C_OscCanProtocol * const pc_Protocol = this->GetCanProtocol(oe_ComProtocol, ou32_DatapoolIndex);
   C_OscNodeDataPool * const pc_DataPool = this->GetComDataPool(oe_ComProtocol, ou32_DatapoolIndex);

   if ((pc_Protocol != NULL) && (pc_DataPool != NULL))
   {
      C_OscNodeDataPoolList * const pc_ListData = C_OscCanProtocol::h_GetComList(*pc_DataPool, ou32_InterfaceIndex,
                                                                                 oq_MessageIsTx);
      if ((pc_ListData != NULL) && (ou32_InterfaceIndex < pc_Protocol->c_ComMessages.size()))
      {
         C_OscCanMessageContainer & rc_MessageContainer = pc_Protocol->c_ComMessages[ou32_InterfaceIndex];
         std::vector<C_OscCanMessage> & rc_Messages = rc_MessageContainer.GetMessages(oq_MessageIsTx);
         if (ou32_MessageIndex < rc_Messages.size())
         {
            const uint32_t u32_DataPoolListElementIndex = rc_MessageContainer.GetMessageSignalDataStartIndex(
               oq_MessageIsTx, ou32_MessageIndex) + ou32_SignalIndex;
            C_OscCanMessage & rc_Message = rc_Messages[ou32_MessageIndex];

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
void C_OscNode::CalcHash(uint32_t & oru32_HashValue) const
{
   uint32_t u32_Counter;

   stw::scl::C_SclChecksums::CalcCRC32(this->c_DeviceType.c_str(), this->c_DeviceType.Length(), oru32_HashValue);
   stw::scl::C_SclChecksums::CalcCRC32(&this->q_DatapoolAutoNvmStartAddress,
                                       sizeof(this->q_DatapoolAutoNvmStartAddress),
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

   this->c_HalcConfig.CalcHash(oru32_HashValue);
   for (std::map<uint8_t, C_OscCanOpenManagerInfo>::const_iterator c_It = this->c_CanOpenManagers.begin();
        c_It != this->c_CanOpenManagers.end(); ++c_It)
   {
      const uint8_t u8_Value = c_It->first;
      stw::scl::C_SclChecksums::CalcCRC32(&u8_Value, sizeof(u8_Value), oru32_HashValue);
      c_It->second.CalcHash(oru32_HashValue);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get all HEX applications

   This returns a vector of all applications that are not of type ePARAMETER_SET_HALC, i.e. those that on system
   commissioning side are handled as HEX files.

   \return
   Vector of HEX applications (i.e. all but parameter set applications)
*/
//----------------------------------------------------------------------------------------------------------------------
std::vector<C_OscNodeApplication> C_OscNode::GetHexApplications(void) const
{
   std::vector<C_OscNodeApplication> c_HexApps;

   for (uint32_t u32_Index = 0U; u32_Index < this->c_Applications.size(); ++u32_Index)
   {
      if (this->c_Applications[u32_Index].e_Type != C_OscNodeApplication::ePARAMETER_SET_HALC)
      {
         c_HexApps.push_back(this->c_Applications[u32_Index]);
      }
   }

   return c_HexApps;
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
int32_t C_OscNode::GetDataPoolIndex(const C_OscNodeDataPool::E_Type oe_DataPoolType,
                                    const uint32_t ou32_DataPoolTypeIndex) const
{
   int32_t s32_Retval = -1;
   uint32_t u32_Index;
   uint32_t u32_TypeIndex = 0U;

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
int32_t C_OscNode::GetDataPoolTypeIndex(const uint32_t ou32_DataPoolIndex) const
{
   int32_t s32_Retval = -1;

   if (ou32_DataPoolIndex < this->c_DataPools.size())
   {
      const C_OscNodeDataPool & rc_Dp = this->c_DataPools[ou32_DataPoolIndex];

      uint32_t u32_Counter = 0UL;
      for (uint32_t u32_ItDp = 0UL; u32_ItDp < ou32_DataPoolIndex; ++u32_ItDp)
      {
         const C_OscNodeDataPool & rc_OtherDp = this->c_DataPools[u32_ItDp];
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
uint32_t C_OscNode::GetDataPoolCount(const C_OscNodeDataPool::E_Type oe_DataPoolType) const
{
   uint32_t u32_Retval = 0U;
   uint32_t u32_Index;

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
bool C_OscNode::h_CompareNameGreater(const C_OscNode & orc_Node1, const C_OscNode & orc_Node2)
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
uint32_t C_OscNode::GetListsSize(void) const
{
   uint32_t u32_Retval = 0;

   for (uint32_t u32_ItDataPool = 0; u32_ItDataPool < this->c_DataPools.size(); ++u32_ItDataPool)
   {
      const C_OscNodeDataPool & rc_NodeDataPool = this->c_DataPools[u32_ItDataPool];
      if ((rc_NodeDataPool.e_Type == C_OscNodeDataPool::eNVM) ||
          (rc_NodeDataPool.e_Type == C_OscNodeDataPool::eHALC_NVM))
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
uint32_t C_OscNode::GetDataPoolsSize(void) const
{
   uint32_t u32_Retval = 0;

   for (uint32_t u32_ItDataPool = 0; u32_ItDataPool < this->c_DataPools.size(); ++u32_ItDataPool)
   {
      const C_OscNodeDataPool & rc_NodeDataPool = this->c_DataPools[u32_ItDataPool];
      if ((rc_NodeDataPool.e_Type == C_OscNodeDataPool::eNVM) ||
          (rc_NodeDataPool.e_Type == C_OscNodeDataPool::eHALC_NVM))
      {
         u32_Retval += rc_NodeDataPool.u32_NvmSize;
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
uint32_t C_OscNode::GetDataPoolAbsoluteAddress(const uint32_t ou32_DataPoolIndex) const
{
   uint32_t u32_Retval = 0;

   for (uint32_t u32_ItDataPool = 0;
        (u32_ItDataPool < this->c_DataPools.size()) && (u32_ItDataPool < ou32_DataPoolIndex);
        ++u32_ItDataPool)
   {
      const C_OscNodeDataPool & rc_NodeDataPool = this->c_DataPools[u32_ItDataPool];
      if ((rc_NodeDataPool.e_Type == C_OscNodeDataPool::eNVM) ||
          (rc_NodeDataPool.e_Type == C_OscNodeDataPool::eHALC_NVM))
      {
         u32_Retval += rc_NodeDataPool.u32_NvmSize;
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
uint32_t C_OscNode::GetListAbsoluteAddress(const uint32_t ou32_DataPoolIndex, const uint32_t ou32_ListIndex) const
{
   uint32_t u32_Retval = GetDataPoolAbsoluteAddress(ou32_DataPoolIndex);

   if (ou32_DataPoolIndex < this->c_DataPools.size())
   {
      const C_OscNodeDataPool & rc_NodeDataPool = this->c_DataPools[ou32_DataPoolIndex];

      for (uint32_t u32_ItList = 0; (u32_ItList < rc_NodeDataPool.c_Lists.size()) && (u32_ItList < ou32_ListIndex);
           ++u32_ItList)
      {
         const C_OscNodeDataPoolList & rc_NodeDataPoolList = rc_NodeDataPool.c_Lists[u32_ItList];
         u32_Retval += rc_NodeDataPoolList.u32_NvmSize;
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
uint32_t C_OscNode::GetElementAbsoluteAddress(const uint32_t ou32_DataPoolIndex, const uint32_t ou32_ListIndex,
                                              const uint32_t ou32_ElementIndex) const
{
   uint32_t u32_Retval = GetListAbsoluteAddress(ou32_DataPoolIndex, ou32_ListIndex);

   if (ou32_DataPoolIndex < this->c_DataPools.size())
   {
      const C_OscNodeDataPool & rc_NodeDataPool = this->c_DataPools[ou32_DataPoolIndex];
      if (ou32_ListIndex < rc_NodeDataPool.c_Lists.size())
      {
         const C_OscNodeDataPoolList & rc_NodeDataPoolList = rc_NodeDataPool.c_Lists[ou32_ListIndex];

         if (rc_NodeDataPoolList.q_NvmCrcActive == true)
         {
            u32_Retval += 2;
         }
         for (uint32_t u32_ItElement = 0;
              (u32_ItElement < rc_NodeDataPoolList.c_Elements.size()) && (u32_ItElement < ou32_ElementIndex);
              ++u32_ItElement)
         {
            const C_OscNodeDataPoolListElement & rc_NodeDataPoolListElement =
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
const C_OscNodeDataPool * C_OscNode::GetHalDataPoolConst(const bool oq_SafeDatapool) const
{
   const C_OscNodeDataPool * pc_Retval = NULL;

   for (uint32_t u32_ItDatapools = 0; u32_ItDatapools < this->c_DataPools.size(); ++u32_ItDatapools)
   {
      const C_OscNodeDataPool & rc_Datapool = this->c_DataPools[u32_ItDatapools];
      if (((rc_Datapool.e_Type == C_OscNodeDataPool::eHALC) || (rc_Datapool.e_Type == C_OscNodeDataPool::eHALC_NVM)) &&
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
const C_OscNodeDataPool * C_OscNode::GetComDataPoolConst(const C_OscCanProtocol::E_Type oe_ComProtocol,
                                                         const uint32_t ou32_DataPoolIndex) const
{
   const C_OscNodeDataPool * pc_Retval = NULL;

   for (uint32_t u32_ItCanProtocol = 0; (u32_ItCanProtocol < this->c_ComProtocols.size()) && (pc_Retval == NULL);
        ++u32_ItCanProtocol)
   {
      const C_OscCanProtocol & rc_CanProtocol = this->c_ComProtocols[u32_ItCanProtocol];
      if (rc_CanProtocol.e_Type == oe_ComProtocol)
      {
         if ((rc_CanProtocol.u32_DataPoolIndex == ou32_DataPoolIndex) &&
             (rc_CanProtocol.u32_DataPoolIndex < this->c_DataPools.size()))
         {
            pc_Retval = &this->c_DataPools[rc_CanProtocol.u32_DataPoolIndex];
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
C_OscNodeDataPool * C_OscNode::GetComDataPool(const C_OscCanProtocol::E_Type oe_ComProtocol,
                                              const uint32_t ou32_DataPoolIndex)
{
   C_OscNodeDataPool * pc_Retval = NULL;

   for (uint32_t u32_ItCanProtocol = 0; (u32_ItCanProtocol < this->c_ComProtocols.size()) && (pc_Retval == NULL);
        ++u32_ItCanProtocol)
   {
      const C_OscCanProtocol & rc_CanProtocol = this->c_ComProtocols[u32_ItCanProtocol];
      if (rc_CanProtocol.e_Type == oe_ComProtocol)
      {
         if ((rc_CanProtocol.u32_DataPoolIndex == ou32_DataPoolIndex) &&
             (rc_CanProtocol.u32_DataPoolIndex < this->c_DataPools.size()))
         {
            pc_Retval = &this->c_DataPools[rc_CanProtocol.u32_DataPoolIndex];
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
std::vector<const C_OscNodeDataPool *> C_OscNode::GetComDatapoolsConst(const C_OscCanProtocol::E_Type oe_ComProtocol)
const
{
   std::vector<const C_OscNodeDataPool *> c_Return;

   for (uint32_t u32_ItCanProtocol = 0; u32_ItCanProtocol < this->c_ComProtocols.size(); ++u32_ItCanProtocol)
   {
      const C_OscCanProtocol & rc_CanProtocol = this->c_ComProtocols[u32_ItCanProtocol];
      if (rc_CanProtocol.e_Type == oe_ComProtocol)
      {
         if (rc_CanProtocol.u32_DataPoolIndex < this->c_DataPools.size())
         {
            c_Return.push_back(&this->c_DataPools[rc_CanProtocol.u32_DataPoolIndex]);
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
std::vector<C_OscNodeDataPool *> C_OscNode::GetComDataPools(const C_OscCanProtocol::E_Type oe_ComProtocol)
{
   std::vector<C_OscNodeDataPool *> c_Return;

   for (uint32_t u32_ItCanProtocol = 0; u32_ItCanProtocol < this->c_ComProtocols.size(); ++u32_ItCanProtocol)
   {
      const C_OscCanProtocol & rc_CanProtocol = this->c_ComProtocols[u32_ItCanProtocol];
      if (rc_CanProtocol.e_Type == oe_ComProtocol)
      {
         if (rc_CanProtocol.u32_DataPoolIndex < this->c_DataPools.size())
         {
            c_Return.push_back(&this->c_DataPools[rc_CanProtocol.u32_DataPoolIndex]);
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
const C_OscCanProtocol * C_OscNode::GetCanProtocolConst(const C_OscCanProtocol::E_Type oe_ComProtocol,
                                                        const uint32_t ou32_DataPoolIndex)
const
{
   const C_OscCanProtocol * pc_Retval = this->GetRelatedCanProtocolConst(ou32_DataPoolIndex);

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
C_OscCanProtocol * C_OscNode::GetCanProtocol(const C_OscCanProtocol::E_Type oe_ComProtocol,
                                             const uint32_t ou32_DataPoolIndex)
{
   C_OscCanProtocol * pc_Retval =  this->GetRelatedCanProtocol(ou32_DataPoolIndex);

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
std::vector<const C_OscCanProtocol *> C_OscNode::GetCanProtocolsConst(const C_OscCanProtocol::E_Type oe_ComProtocol)
const
{
   std::vector<const C_OscCanProtocol *> c_Return;

   for (uint32_t u32_ItCanProtocol = 0; u32_ItCanProtocol < this->c_ComProtocols.size(); ++u32_ItCanProtocol)
   {
      const C_OscCanProtocol & rc_Protocol = this->c_ComProtocols[u32_ItCanProtocol];
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
std::vector<C_OscCanProtocol *> C_OscNode::GetCanProtocols(const C_OscCanProtocol::E_Type oe_ComProtocol)
{
   std::vector<C_OscCanProtocol *> c_Return;

   for (uint32_t u32_ItCanProtocol = 0; u32_ItCanProtocol < this->c_ComProtocols.size(); ++u32_ItCanProtocol)
   {
      const C_OscCanProtocol & rc_Protocol = this->c_ComProtocols[u32_ItCanProtocol];
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
const C_OscCanProtocol * C_OscNode::GetRelatedCanProtocolConst(const uint32_t ou32_DataPoolIndex) const
{
   const C_OscCanProtocol * pc_Retval = NULL;

   for (uint32_t u32_ItCanProtocol = 0; u32_ItCanProtocol < this->c_ComProtocols.size(); ++u32_ItCanProtocol)
   {
      const C_OscCanProtocol & rc_Protocol = this->c_ComProtocols[u32_ItCanProtocol];
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
C_OscCanProtocol * C_OscNode::GetRelatedCanProtocol(const uint32_t ou32_DataPoolIndex)
{
   C_OscCanProtocol * pc_Retval = NULL;

   for (uint32_t u32_ItCanProtocol = 0; u32_ItCanProtocol < this->c_ComProtocols.size(); ++u32_ItCanProtocol)
   {
      const C_OscCanProtocol & rc_Protocol = this->c_ComProtocols[u32_ItCanProtocol];
      if (rc_Protocol.u32_DataPoolIndex == ou32_DataPoolIndex)
      {
         pc_Retval = &this->c_ComProtocols[u32_ItCanProtocol];
         break;
      }
   }

   return pc_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Checks a specific CAN protocol on all CAN interfaces for errors

   For all protocol types:
   Allowed signals for each protocol, Datapool and direction (TX/RX) is 2048.

   For CANopen only:
   Allowed active PDOS (Messages) for each direction (TPDO/RPDO) is 512

   \param[in]   oe_ComProtocol                  Communication protocol
   \param[in]   oq_ComProtocolUsedByInterface   Flag for communication protocol bus connection to check for
                                                true: Check only protocol instances which are connected to a bus
                                                false: Check only protocol instances which are not connected to a bus
   \param[out]  orq_InvalidMaxRxSignalCount     The number of RX signals of this protocol with at least one Datapool is to high
   \param[out]  orq_InvalidMaxTxSignalCount     The number of TX signals of this protocol with at least one Datapool is to high
   \param[out]  orq_InvalidCoRxPdoCount         The number of active RPDOs (CANopen Manager viewpoint) of CANopen is to high
   \param[out]  orq_InvalidCoTxPdoCount         The number of active TPDOs (CANopen Manager viewpoint) of CANopen is to high
   \param[out]  orq_InvalidMinSignalCount       The number of signals of this protocol with at least one Datapool is too low
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OscNode::CheckErrorCanProtocol(const C_OscCanProtocol::E_Type oe_ComProtocol,
                                      const bool oq_ComProtocolUsedByInterface, bool & orq_InvalidMaxRxSignalCount,
                                      bool & orq_InvalidMaxTxSignalCount, bool & orq_InvalidCoRxPdoCount,
                                      bool & orq_InvalidCoTxPdoCount, bool & orq_InvalidMinSignalCount) const
{
   uint32_t u32_InterfaceCounter;

   orq_InvalidMaxRxSignalCount = false;
   orq_InvalidMaxTxSignalCount = false;
   orq_InvalidCoTxPdoCount = false;
   orq_InvalidCoRxPdoCount = false;
   orq_InvalidMinSignalCount = false;

   // Check all interfaces
   for (u32_InterfaceCounter = 0U;
        u32_InterfaceCounter < this->c_Properties.c_ComInterfaces.size();
        ++u32_InterfaceCounter)
   {
      if (this->c_Properties.c_ComInterfaces[u32_InterfaceCounter].e_InterfaceType ==
          C_OscSystemBus::eCAN)
      {
         bool q_TempCommMaxRxSignalCountInvalid = false;
         bool q_TempCommMaxTxSignalCountInvalid = false;
         bool q_TempCoRxPdoCountInvalid = false;
         bool q_TempCoTxPdoCountInvalid = false;
         bool q_TempCommMinSignalCountInvalid = false;

         this->CheckErrorCanProtocol(u32_InterfaceCounter, oe_ComProtocol,
                                     oq_ComProtocolUsedByInterface,
                                     q_TempCommMaxRxSignalCountInvalid, q_TempCommMaxTxSignalCountInvalid,
                                     q_TempCoRxPdoCountInvalid, q_TempCoTxPdoCountInvalid,
                                     q_TempCommMinSignalCountInvalid);

         // Do not overwrite already detected errors
         if (q_TempCommMaxRxSignalCountInvalid == true)
         {
            orq_InvalidMaxRxSignalCount = true;
         }
         if (q_TempCommMaxTxSignalCountInvalid == true)
         {
            orq_InvalidMaxTxSignalCount = true;
         }
         if (q_TempCommMinSignalCountInvalid == true)
         {
            orq_InvalidMinSignalCount = true;
         }

         if (q_TempCoRxPdoCountInvalid == true)
         {
            orq_InvalidCoRxPdoCount = true;
         }
         if (q_TempCoTxPdoCountInvalid == true)
         {
            orq_InvalidCoTxPdoCount = true;
         }
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Checks a specific CAN protocol for errors

   For all protocol types:
   Allowed signals for each protocol, Datapool and direction (TX/RX) is 2048.

   For CANopen only:
   Allowed active PDOS (Messages) for each direction (TPDO/RPDO) is 512

   \param[in]   ou32_InterfaceIndex             Interface index
   \param[in]   oe_ComProtocol                  Communication protocol
   \param[in]   oq_ComProtocolUsedByInterface   Flag for communication protocol bus connection to check for
                                                true: Check only protocol instances which are connected to a bus
                                                false: Check only protocol instances which are not connected to a bus
   \param[out]  orq_InvalidMaxRxSignalCount     The number of RX signals of this protocol with at least one Datapool is to high
   \param[out]  orq_InvalidMaxTxSignalCount     The number of TX signals of this protocol with at least one Datapool is to high
   \param[out]  orq_InvalidCoRxPdoCount         The number of active RPDOs (CANopen Manager viewpoint) of CANopen is to high
   \param[out]  orq_InvalidCoTxPdoCount         The number of active TPDOs (CANopen Manager viewpoint) of CANopen is to high
   \param[out]  orq_InvalidMinSignalCount       The number of signals of this protocol with at least one Datapool is too low
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OscNode::CheckErrorCanProtocol(const uint32_t ou32_InterfaceIndex, const C_OscCanProtocol::E_Type oe_ComProtocol,
                                      const bool oq_ComProtocolUsedByInterface, bool & orq_InvalidMaxRxSignalCount,
                                      bool & orq_InvalidMaxTxSignalCount, bool & orq_InvalidCoRxPdoCount,
                                      bool & orq_InvalidCoTxPdoCount, bool & orq_InvalidMinSignalCount) const
{
   const std::vector<const C_OscCanProtocol *> c_Protocols = this->GetCanProtocolsConst(oe_ComProtocol);
   uint32_t u32_Counter;

   orq_InvalidMaxRxSignalCount = false;
   orq_InvalidMaxTxSignalCount = false;
   orq_InvalidCoTxPdoCount = false;
   orq_InvalidCoRxPdoCount = false;
   orq_InvalidMinSignalCount = false;

   for (u32_Counter = 0U; u32_Counter < c_Protocols.size(); ++u32_Counter)
   {
      // Check all instances of the matching protocol instances
      const C_OscCanProtocol * const pc_Prot = c_Protocols[u32_Counter];
      if ((pc_Prot != NULL) &&
          (pc_Prot->e_Type == oe_ComProtocol) &&
          (ou32_InterfaceIndex < pc_Prot->c_ComMessages.size()) &&
          (oq_ComProtocolUsedByInterface == pc_Prot->c_ComMessages[ou32_InterfaceIndex].q_IsComProtocolUsedByInterface))
      {
         // Check Rx and RPDOs
         mh_CheckErrorCanProtocolDirection(pc_Prot->c_ComMessages[ou32_InterfaceIndex].c_RxMessages,
                                           oe_ComProtocol,
                                           orq_InvalidMaxRxSignalCount, orq_InvalidCoRxPdoCount,
                                           orq_InvalidMinSignalCount);

         // Check Tx and TPDOs
         mh_CheckErrorCanProtocolDirection(pc_Prot->c_ComMessages[ou32_InterfaceIndex].c_TxMessages,
                                           oe_ComProtocol,
                                           orq_InvalidMaxTxSignalCount, orq_InvalidCoTxPdoCount,
                                           orq_InvalidMinSignalCount);
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Check whether the number of lists and elements is within supported range

   Goes through the data pool lists and element and check whether the number are within the supported range.
   The valid range is limited by the openSYDE communication protocol.

   \param[in]   ou32_DataPoolIndex           Data pool index
   \param[out]  orq_TooFewListsOrElements    true: number of lists or elements of at least one list too low
   \param[out]  orq_TooManyListsOrElements   true: number of lists or elements of at least one list too high
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OscNode::CheckErrorDataPoolNumListsAndElements(const uint32_t ou32_DataPoolIndex,
                                                      bool & orq_TooFewListsOrElements,
                                                      bool & orq_TooManyListsOrElements) const
{
   orq_TooFewListsOrElements = true;
   orq_TooManyListsOrElements = true;
   if (ou32_DataPoolIndex < this->c_DataPools.size())
   {
      const C_OscNodeDataPool & rc_CheckedDataPool = this->c_DataPools[ou32_DataPoolIndex];

      orq_TooManyListsOrElements = false;

      //number of lists ok ?
      if (rc_CheckedDataPool.c_Lists.size() > hu32_MAX_NUMBER_OF_LISTS_PER_DATA_POOL)
      {
         //no
         orq_TooManyListsOrElements = true;
      }
      else
      {
         //number of elements ok ?
         for (uint32_t u32_Index = 0U; u32_Index < rc_CheckedDataPool.c_Lists.size(); u32_Index++)
         {
            if (rc_CheckedDataPool.c_Lists[u32_Index].c_Elements.size() > hu32_MAX_NUMBER_OF_ELEMENTS_PER_LIST)
            {
               orq_TooManyListsOrElements = true;
               break;
            }
         }
      }

      orq_TooFewListsOrElements = this->m_CheckErrorTooFewElements(ou32_DataPoolIndex);
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
   * naming conflict: multiple elements within one list have the same name

   \param[in]   ou32_DataPoolIndex           Data pool index
   \param[out]  opq_NameConflict             Name conflict
   \param[out]  opq_NameInvalid              Name not usable as variable
   \param[out]  opq_IsErrorInListOrMessage   true: conflict in any list or message was detected
   \param[out]  opq_TooFewListsOrElements    true: too few lists or elements
   \param[out]  opq_TooManyListsOrElements   true: too many lists or elements
   \param[out]  opc_InvalidListIndices       Optional storage for list of invalid list indices
                                             (see opq_IsErrorInListOrMessage)
                                             If COMM: interface index
                                             Else: list index
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OscNode::CheckErrorDataPool(const uint32_t ou32_DataPoolIndex, bool * const opq_NameConflict,
                                   bool * const opq_NameInvalid, bool * const opq_IsErrorInListOrMessage,
                                   bool * const opq_TooFewListsOrElements, bool * const opq_TooManyListsOrElements,
                                   std::vector<uint32_t> * const opc_InvalidListIndices) const
{
   if (ou32_DataPoolIndex < this->c_DataPools.size())
   {
      const C_OscNodeDataPool & rc_CheckedDataPool = this->c_DataPools[ou32_DataPoolIndex];

      //Is the data pool name valid ?
      if (opq_NameInvalid != NULL)
      {
         if (C_OscUtils::h_CheckValidCeName(rc_CheckedDataPool.c_Name) == false)
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
         for (uint32_t u32_ItElement = 0; u32_ItElement < this->c_DataPools.size(); ++u32_ItElement)
         {
            if (u32_ItElement != ou32_DataPoolIndex)
            {
               const C_OscNodeDataPool & rc_CurrentDataPool = this->c_DataPools[u32_ItElement];
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
         if ((opq_TooManyListsOrElements != NULL) || (opq_TooFewListsOrElements != NULL))
         {
            bool q_TooFew;
            bool q_TooMany;
            CheckErrorDataPoolNumListsAndElements(ou32_DataPoolIndex, q_TooFew, q_TooMany);
            if (opq_TooFewListsOrElements != NULL)
            {
               *opq_TooFewListsOrElements = q_TooFew;
            }
            if (opq_TooManyListsOrElements != NULL)
            {
               *opq_TooManyListsOrElements = q_TooMany;
            }
         }

         if (rc_CheckedDataPool.e_Type == C_OscNodeDataPool::eCOM)
         {
            const C_OscCanProtocol * const pc_Protocol = this->GetRelatedCanProtocolConst(ou32_DataPoolIndex);
            if (pc_Protocol != NULL)
            {
               static std::map<std::vector<uint32_t>, bool> hc_PreviousResults;
               for (uint32_t u32_ItContainer = 0; u32_ItContainer < pc_Protocol->c_ComMessages.size();
                    ++u32_ItContainer)
               {
                  uint32_t u32_TxListIndex;
                  uint32_t u32_RxListIndex;
                  if ((C_OscCanProtocol::h_GetComListIndex(rc_CheckedDataPool, u32_ItContainer, true,
                                                           u32_TxListIndex) == C_NO_ERR) &&
                      (C_OscCanProtocol::h_GetComListIndex(rc_CheckedDataPool, u32_ItContainer, false,
                                                           u32_RxListIndex) == C_NO_ERR))
                  {
                     // Parameter oq_CanOpenPdoSyncValid is true in case of no CANopen protocol
                     // to avoid a check for this scenario
                     bool q_CanOpenPdoSyncValid = true;
                     //Get Hash for all relevant data
                     const uint32_t u32_HashCon = this->m_GetContainerHash(ou32_DataPoolIndex, u32_ItContainer);
                     const uint32_t u32_HashTx = this->m_GetListHash(ou32_DataPoolIndex, u32_TxListIndex);
                     const uint32_t u32_HashRx = this->m_GetListHash(ou32_DataPoolIndex, u32_RxListIndex);
                     std::vector<uint32_t> c_Hashes;
                     std::map<std::vector<uint32_t>, bool>::const_iterator c_It;
                     c_Hashes.push_back(u32_HashCon);
                     c_Hashes.push_back(u32_HashTx);
                     c_Hashes.push_back(u32_HashRx);

                     // Special case CANopen
                     if (pc_Protocol->e_Type == C_OscCanProtocol::eCAN_OPEN)
                     {
                        // Get the CANopen Manager
                        tgl_assert(u32_ItContainer < this->c_Properties.c_ComInterfaces.size());
                        if (u32_ItContainer < this->c_Properties.c_ComInterfaces.size())
                        {
                           const std::map<uint8_t,
                                          C_OscCanOpenManagerInfo>::const_iterator c_ItCoManager =
                              this->c_CanOpenManagers.find(this->c_Properties.c_ComInterfaces[
                                                              u32_ItContainer].u8_InterfaceNumber);

                           // Check if it is the CANopen Manager of this bus
                           if (c_ItCoManager != this->c_CanOpenManagers.end())
                           {
                              uint32_t u32_HashManager;
                              const C_OscCanOpenManagerInfo & rc_Manager = c_ItCoManager->second;
                              // When PDO SYNC message is produces, a configured TX method with PDO SYNC
                              // is valid
                              q_CanOpenPdoSyncValid = rc_Manager.q_ProduceSyncMessage;

                              // Due to impacts on the error detection in case of changes of CANopen Manger
                              // configuration, the hash of the CANopen Manager is relevant too
                              rc_Manager.CalcHash(u32_HashManager);
                              c_Hashes.push_back(u32_HashManager);
                           }
                        }
                     }

                     //Check if check was already performed in the past
                     c_It = hc_PreviousResults.find(c_Hashes);
                     if (c_It == hc_PreviousResults.end())
                     {
                        if ((u32_TxListIndex < rc_CheckedDataPool.c_Lists.size()) &&
                            (u32_RxListIndex < rc_CheckedDataPool.c_Lists.size()))
                        {
                           const C_OscCanMessageContainer & rc_Container = pc_Protocol->c_ComMessages[u32_ItContainer];

                           const bool q_Error = rc_Container.CheckLocalError(
                              rc_CheckedDataPool.c_Lists[u32_TxListIndex],
                              rc_CheckedDataPool.c_Lists[u32_RxListIndex],
                              C_OscCanProtocol::h_GetCanMessageValidSignalsDlcOffset(pc_Protocol->e_Type),
                              C_OscCanProtocol::h_GetCanMessageSignalGapsValid(pc_Protocol->e_Type),
                              C_OscCanProtocol::h_GetCanMessageSignalByteAlignmentRequired(pc_Protocol->e_Type),
                              C_OscCanProtocol::h_GetCanMessageSignalsRequired(pc_Protocol->e_Type),
                              q_CanOpenPdoSyncValid);
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
            std::map<stw::scl::C_SclString, uint32_t> c_PreviousNames;
            static std::map<uint32_t, bool> hc_PreviousResults;
            bool q_CheckSize;
            bool q_NameInvalid;
            bool q_UsageInvalid;
            bool q_OutOfDataPool;
            bool q_DataSetsInvalid;
            bool q_ElementsInvalid;

            if ((rc_CheckedDataPool.e_Type == C_OscNodeDataPool::eNVM) ||
                (rc_CheckedDataPool.e_Type == C_OscNodeDataPool::eHALC_NVM))
            {
               q_CheckSize = true;
            }
            else
            {
               q_CheckSize = false;
            }

            for (uint32_t u32_ItList = 0;
                 (u32_ItList < rc_CheckedDataPool.c_Lists.size()) &&
                 ((*opq_IsErrorInListOrMessage == false) || (opc_InvalidListIndices != NULL));
                 ++u32_ItList)
            {
               //Overarching checks
               const C_OscNodeDataPoolList & rc_List = rc_CheckedDataPool.c_Lists[u32_ItList];
               const std::map<stw::scl::C_SclString,
                              uint32_t>::const_iterator c_ItList = c_PreviousNames.find(rc_List.c_Name.LowerCase());
               if (c_ItList != c_PreviousNames.end())
               {
                  *opq_IsErrorInListOrMessage = true;
                  if (opc_InvalidListIndices != NULL)
                  {
                     bool q_Added = false;
                     //Only add element once!
                     for (uint32_t u32_It = 0UL; u32_It < opc_InvalidListIndices->size(); ++u32_It)
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
                  const uint32_t u32_Hash = this->m_GetListHash(ou32_DataPoolIndex, u32_ItList);
                  //Check if check was already performed in the past
                  const std::map<uint32_t, bool>::const_iterator c_ItErr = hc_PreviousResults.find(u32_Hash);
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
void C_OscNode::CheckMessageId(const uint32_t ou32_InterfaceIndex, const C_OscCanMessageUniqueId & orc_MessageId,
                               bool & orq_Valid, const C_OscCanProtocol::E_Type * const ope_SkipComProtocol,
                               const uint32_t * const opu32_SkipInterfaceIndex,
                               const bool * const opq_SkipMessageIsTxFlag,
                               const uint32_t * const opu32_SkipMessageIndex) const
{
   std::vector<const C_OscCanMessage *> c_Messages;
   m_GetAllMessages(ou32_InterfaceIndex, c_Messages, ope_SkipComProtocol, opu32_SkipInterfaceIndex,
                    opq_SkipMessageIsTxFlag, opu32_SkipMessageIndex);
   orq_Valid = true;
   for (uint32_t u32_ItMessage = 0; u32_ItMessage < c_Messages.size(); ++u32_ItMessage)
   {
      const C_OscCanMessage * const pc_CurMessage = c_Messages[u32_ItMessage];
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
void C_OscNode::CheckMessageName(const uint32_t ou32_InterfaceIndex, const stw::scl::C_SclString & orc_MessageName,
                                 bool & orq_Valid, const C_OscCanProtocol::E_Type * const ope_SkipComProtocol,
                                 const uint32_t * const opu32_SkipInterfaceIndex,
                                 const bool * const opq_SkipMessageIsTxFlag,
                                 const uint32_t * const opu32_SkipMessageIndex) const
{
   if (C_OscUtils::h_CheckValidCeName(orc_MessageName) == true)
   {
      std::vector<const C_OscCanMessage *> c_Messages;
      m_GetAllMessages(ou32_InterfaceIndex, c_Messages, ope_SkipComProtocol, opu32_SkipInterfaceIndex,
                       opq_SkipMessageIsTxFlag, opu32_SkipMessageIndex);
      orq_Valid = true;
      for (uint32_t u32_ItMessage = 0; u32_ItMessage < c_Messages.size(); ++u32_ItMessage)
      {
         const C_OscCanMessage * const pc_CurMessage = c_Messages[u32_ItMessage];
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
int32_t C_OscNode::CheckApplicationProcessIdValid(const uint32_t ou32_ApplicationIndex, bool & orq_Valid) const
{
   int32_t s32_Retval = C_NO_ERR;

   orq_Valid = true;
   if (ou32_ApplicationIndex < this->c_Applications.size())
   {
      const C_OscNodeApplication & rc_CheckedApplication = this->c_Applications[ou32_ApplicationIndex];
      //Only check programmable applications
      if (rc_CheckedApplication.e_Type == C_OscNodeApplication::ePROGRAMMABLE_APPLICATION)
      {
         for (uint32_t u32_ItApp = 0; u32_ItApp < this->c_Applications.size(); ++u32_ItApp)
         {
            if (u32_ItApp != ou32_ApplicationIndex)
            {
               const C_OscNodeApplication & rc_Application = this->c_Applications[u32_ItApp];
               //Only check programmable applications
               if (rc_Application.e_Type == C_OscNodeApplication::ePROGRAMMABLE_APPLICATION)
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
void C_OscNode::CheckHalcConfigValid(bool * const opq_ConfigInvalid,
                                     std::vector<uint32_t> * const opc_InvalidDomainIndices) const
{
   this->c_HalcConfig.CheckConfigValid(opq_ConfigInvalid, opc_InvalidDomainIndices);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Recalculate all com protocol data pool indices

   Based on assumption data pools are sorted as can protocols
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OscNode::ReCalcCanProtocolDataPoolIndices(void)
{
   for (uint32_t u32_ItCanProtocol = 0; u32_ItCanProtocol < this->c_ComProtocols.size();
        ++u32_ItCanProtocol)
   {
      C_OscCanProtocol & rc_CanProtocol = this->c_ComProtocols[u32_ItCanProtocol];
      uint32_t u32_ComDataPoolNumber = 0;
      //Find corresponding data pool
      for (uint32_t u32_ItDataPool = 0; u32_ItDataPool < this->c_DataPools.size();
           ++u32_ItDataPool)
      {
         const C_OscNodeDataPool & rc_DataPool = this->c_DataPools[u32_ItDataPool];
         if (rc_DataPool.e_Type == C_OscNodeDataPool::eCOM)
         {
            if (u32_ItCanProtocol == u32_ComDataPoolNumber)
            {
               rc_CanProtocol.u32_DataPoolIndex = u32_ItDataPool;
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
bool C_OscNode::IsAnyUpdateAvailable(void) const
{
   bool q_Retval = false;

   if (this->pc_DeviceDefinition != NULL)
   {
      for (uint32_t u32_ItInterface = 0;
           (u32_ItInterface < this->c_Properties.c_ComInterfaces.size()) && (q_Retval == false); ++u32_ItInterface)
      {
         const C_OscNodeComInterfaceSettings & rc_CurInterface = this->c_Properties.c_ComInterfaces[u32_ItInterface];
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
bool C_OscNode::IsRoutingAvailable(const C_OscSystemBus::E_Type oe_Type) const
{
   bool q_Return = false;

   if ((this->pc_DeviceDefinition != NULL) &&
       (this->c_Properties.e_DiagnosticServer == C_OscNodeProperties::eDS_OPEN_SYDE))
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
void C_OscNode::RecalculateAddress(void)
{
   uint32_t u32_Offset = 0UL;

   for (uint32_t u32_ItDataPool = 0UL; u32_ItDataPool < this->c_DataPools.size(); ++u32_ItDataPool)
   {
      C_OscNodeDataPool & rc_DataPool = this->c_DataPools[u32_ItDataPool];
      if ((rc_DataPool.e_Type == C_OscNodeDataPool::eNVM) ||
          (rc_DataPool.e_Type == C_OscNodeDataPool::eHALC_NVM))
      {
         if (this->q_DatapoolAutoNvmStartAddress == true)
         {
            // Updating the Datapool start address only if the auto mode is activated
            // This configuration affects the Datapool start address only. The lists and elements are calculated
            // automatically always
            rc_DataPool.u32_NvmStartAddress = u32_Offset;
         }
         rc_DataPool.RecalculateAddress();
         u32_Offset += rc_DataPool.u32_NvmSize;
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Count all local messages

   \return
   Total number of local messages
*/
//----------------------------------------------------------------------------------------------------------------------
uint32_t C_OscNode::CountAllLocalMessages(void) const
{
   uint32_t u32_MessageCount = 0UL;

   for (uint32_t u32_ItProt = 0UL; u32_ItProt < this->c_ComProtocols.size(); ++u32_ItProt)
   {
      const C_OscCanProtocol & rc_Prot = this->c_ComProtocols[u32_ItProt];
      for (uint32_t u32_ItCont = 0UL; u32_ItCont < rc_Prot.c_ComMessages.size(); ++u32_ItCont)
      {
         const C_OscCanMessageContainer & rc_Cont = rc_Prot.c_ComMessages[u32_ItCont];
         u32_MessageCount += static_cast<uint32_t>(rc_Cont.c_RxMessages.size());
         u32_MessageCount += static_cast<uint32_t>(rc_Cont.c_TxMessages.size());
      }
   }
   return u32_MessageCount;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Handle name max char limit

   \param[in]      ou32_NameMaxCharLimit  Name max char limit
   \param[in,out]  opc_ChangedItems       Changed items
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OscNode::HandleNameMaxCharLimit(const uint32_t ou32_NameMaxCharLimit,
                                       std::list<C_OscSystemNameMaxCharLimitChangeReportItem> * const opc_ChangedItems)
{
   for (uint32_t u32_ItDp = 0UL; u32_ItDp < this->c_DataPools.size(); ++u32_ItDp)
   {
      C_OscNodeDataPool & rc_Dp = this->c_DataPools[u32_ItDp];
      rc_Dp.HandleNameMaxCharLimit(ou32_NameMaxCharLimit, opc_ChangedItems);
   }
   for (uint32_t u32_ItProt = 0UL; u32_ItProt < this->c_ComProtocols.size(); ++u32_ItProt)
   {
      C_OscCanProtocol & rc_Prot = this->c_ComProtocols[u32_ItProt];
      rc_Prot.HandleNameMaxCharLimit(ou32_NameMaxCharLimit, opc_ChangedItems);
   }
   this->c_HalcConfig.HandleNameMaxCharLimit(ou32_NameMaxCharLimit, opc_ChangedItems);
   for (uint32_t u32_ItDatablock = 0UL; u32_ItDatablock < this->c_Applications.size(); ++u32_ItDatablock)
   {
      C_OscNodeApplication & rc_Datablock = this->c_Applications[u32_ItDatablock];
      C_OscSystemNameMaxCharLimitChangeReportItem::h_HandleNameMaxCharLimitItem(ou32_NameMaxCharLimit,
                                                                                "node-datablock-name",
                                                                                rc_Datablock.c_Name,
                                                                                opc_ChangedItems);
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
C_OscNodeDataPoolListElement * C_OscNode::GetDataPoolListElement(const uint32_t ou32_DataPoolIndex,
                                                                 const uint32_t ou32_ListIndex,
                                                                 const uint32_t ou32_ElementIndex)
{
   C_OscNodeDataPoolListElement * pc_Element = NULL;

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
void C_OscNode::m_GetAllMessages(const uint32_t ou32_InterfaceIndex,
                                 std::vector<const C_OscCanMessage *> & orc_Messages,
                                 const C_OscCanProtocol::E_Type * const ope_SkipComProtocol,
                                 const uint32_t * const opu32_SkipInterfaceIndex,
                                 const bool * const opq_SkipMessageIsTxFlag,
                                 const uint32_t * const opu32_SkipMessageIndex) const
{
   //Clear
   orc_Messages.clear();

   //L2
   m_AppendAllProtocolMessages(ou32_InterfaceIndex, C_OscCanProtocol::eLAYER2, orc_Messages,
                               ope_SkipComProtocol, opu32_SkipInterfaceIndex, opq_SkipMessageIsTxFlag,
                               opu32_SkipMessageIndex);
   //ECeS
   m_AppendAllProtocolMessages(ou32_InterfaceIndex, C_OscCanProtocol::eECES, orc_Messages,
                               ope_SkipComProtocol, opu32_SkipInterfaceIndex, opq_SkipMessageIsTxFlag,
                               opu32_SkipMessageIndex);
   //ECoS
   m_AppendAllProtocolMessages(ou32_InterfaceIndex, C_OscCanProtocol::eCAN_OPEN_SAFETY, orc_Messages,
                               ope_SkipComProtocol, opu32_SkipInterfaceIndex, opq_SkipMessageIsTxFlag,
                               opu32_SkipMessageIndex);
   //CAN open manager
   m_AppendAllProtocolMessages(ou32_InterfaceIndex, C_OscCanProtocol::eCAN_OPEN, orc_Messages,
                               ope_SkipComProtocol, opu32_SkipInterfaceIndex, opq_SkipMessageIsTxFlag,
                               opu32_SkipMessageIndex);

   //J1939
   m_AppendAllProtocolMessages(ou32_InterfaceIndex, C_OscCanProtocol::eJ1939, orc_Messages,
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
void C_OscNode::m_AppendAllProtocolMessages(const uint32_t ou32_InterfaceIndex,
                                            const C_OscCanProtocol::E_Type oe_ComProtocol,
                                            std::vector<const C_OscCanMessage *> & orc_Messages,
                                            const C_OscCanProtocol::E_Type * const ope_SkipComProtocol,
                                            const uint32_t * const opu32_SkipInterfaceIndex,
                                            const bool * const opq_SkipMessageIsTxFlag,
                                            const uint32_t * const opu32_SkipMessageIndex) const
{
   for (uint32_t u32_ItCanProtocol = 0; u32_ItCanProtocol < this->c_ComProtocols.size(); ++u32_ItCanProtocol)
   {
      const C_OscCanProtocol & rc_CanProtocol = this->c_ComProtocols[u32_ItCanProtocol];
      if (rc_CanProtocol.e_Type == oe_ComProtocol)
      {
         if (ou32_InterfaceIndex < rc_CanProtocol.c_ComMessages.size())
         {
            const C_OscCanMessageContainer & rc_MessageContainer =
               rc_CanProtocol.c_ComMessages[ou32_InterfaceIndex];
            for (uint8_t u8_ItToggle = 0; u8_ItToggle < 2; ++u8_ItToggle)
            {
               const bool q_IsTx = (u8_ItToggle % 2) == 0;
               const std::vector<C_OscCanMessage> & rc_Messages = rc_MessageContainer.GetMessagesConst(q_IsTx);
               for (uint32_t u32_ItMessage = 0; u32_ItMessage < rc_Messages.size(); ++u32_ItMessage)
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
uint32_t C_OscNode::m_GetListHash(const uint32_t ou32_DataPoolIndex, const uint32_t ou32_ListIndex) const
{
   uint32_t u32_Retval = 0xFFFFFFFFUL;

   if (ou32_DataPoolIndex < this->c_DataPools.size())
   {
      const C_OscNodeDataPool & rc_Dp = this->c_DataPools[ou32_DataPoolIndex];
      if (ou32_ListIndex < rc_Dp.c_Lists.size())
      {
         const C_OscNodeDataPoolList & rc_List = rc_Dp.c_Lists[ou32_ListIndex];
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
uint32_t C_OscNode::m_GetContainerHash(const uint32_t ou32_DataPoolIndex, const uint32_t ou32_ContainerIndex) const
{
   uint32_t u32_Retval = 0xFFFFFFFFUL;
   const C_OscCanProtocol * const pc_Protocol = this->GetRelatedCanProtocolConst(ou32_DataPoolIndex);

   if (pc_Protocol != NULL)
   {
      if (ou32_ContainerIndex < pc_Protocol->c_ComMessages.size())
      {
         const C_OscCanMessageContainer & rc_Container = pc_Protocol->c_ComMessages[ou32_ContainerIndex];
         rc_Container.CalcHash(u32_Retval);
      }
   }
   return u32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Checks a specific CAN protocol direction message container for errors

   For all protocol types:
   Allowed signals for each protocol, Datapool and direction (TX/RX) is 2048.

   For CANopen only:
   Allowed active PDOS (Messages) for each direction (TPDO/RPDO) is 512

   \param[in]   orc_Messages                 All messages of one specific direction of one specific protocol
   \param[in]   oe_ComProtocol               Communication protocol
   \param[out]  orq_InvalidMaxSignalCount    The number of signals of this messages is too high
   \param[out]  orq_InvalidCoPdoCount        The number of active PDOs (CANopen Manager viewpoint) of CANopen is to high
   \param[out]  orq_InvalidMinSignalCount    The number of signals of this messages is too low
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OscNode::mh_CheckErrorCanProtocolDirection(const std::vector<C_OscCanMessage> & orc_Messages,
                                                  const C_OscCanProtocol::E_Type oe_ComProtocol,
                                                  bool & orq_InvalidMaxSignalCount, bool & orq_InvalidCoPdoCount,
                                                  bool & orq_InvalidMinSignalCount)
{
   uint32_t u32_MessageCounter;
   uint32_t u32_SignalCount = 0U;
   uint32_t u32_ActivePdoCounter = 0U;

   for (u32_MessageCounter = 0U;
        u32_MessageCounter < orc_Messages.size();
        ++u32_MessageCounter)
   {
      const C_OscCanMessage & rc_Msg = orc_Messages[u32_MessageCounter];
      // Check signal count, which is relevant for all protocol types
      // For each direction and interface is a Datapool list associated. The maximum number of list entries
      // is the limit for the count of signals
      u32_SignalCount += static_cast<uint32_t>(rc_Msg.c_Signals.size());
      if (u32_SignalCount > hu32_MAX_NUMBER_OF_ELEMENTS_PER_LIST)
      {
         orq_InvalidMaxSignalCount = true;
      }

      // Check active PDO count, which is only relevant for CANopen
      if (oe_ComProtocol == C_OscCanProtocol::eCAN_OPEN)
      {
         if (rc_Msg.q_CanOpenManagerMessageActive == true)
         {
            ++u32_ActivePdoCounter;
            if (u32_ActivePdoCounter > hu32_MAX_NUMBER_OF_ACTIVE_PDO_PER_DIRECTION)
            {
               orq_InvalidCoPdoCount = true;
            }
         }
      }
   }
   if ((u32_SignalCount == 0UL) && (orc_Messages.size() > 0UL))
   {
      orq_InvalidMinSignalCount = true;
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Check error too few elements

   \param[in]  ou32_DataPoolIndex   Data pool index

   \return
   Flags

   \retval   True    Too few element error
   \retval   False   No error
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_OscNode::m_CheckErrorTooFewElements(const uint32_t ou32_DataPoolIndex) const
{
   bool orq_TooFewListsOrElements = false;

   if (ou32_DataPoolIndex < this->c_DataPools.size())
   {
      const C_OscNodeDataPool & rc_CheckedDataPool = this->c_DataPools[ou32_DataPoolIndex];
      const C_OscCanProtocol * const pc_Protocol = this->GetRelatedCanProtocolConst(ou32_DataPoolIndex);
      if (pc_Protocol != NULL)
      {
         //number of lists ok ?
         if (rc_CheckedDataPool.c_Lists.size() == 0UL)
         {
            //no
            orq_TooFewListsOrElements = true;
         }
         else
         {
            for (uint32_t u32_ItContainer = 0UL; u32_ItContainer < pc_Protocol->c_ComMessages.size(); ++u32_ItContainer)
            {
               const C_OscCanMessageContainer & rc_Container = pc_Protocol->c_ComMessages[u32_ItContainer];
               if (rc_Container.CheckMinSignalError())
               {
                  orq_TooFewListsOrElements = true;
               }
            }
         }
      }
   }
   return orq_TooFewListsOrElements;
}
