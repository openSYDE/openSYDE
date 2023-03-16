//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Handler class for OSY data (implementation)

   Handler class for OSY data

   \copyright   Copyright 2019 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.hpp"

#include "stwtypes.hpp"
#include "stwerrors.hpp"
#include "C_CamDbOsy.hpp"
#include "C_CamGenSigUtil.hpp"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw::errors;
using namespace stw::opensyde_core;
using namespace stw::opensyde_gui_logic;

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
C_CamDbOsy::C_CamDbOsy() :
   mq_Active(true),
   mq_FoundAll(false),
   mc_Data(C_OscSystemDefinition(), 0UL)
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set active flag

   \param[in]  oq_Active   New active state
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CamDbOsy::SetActive(const bool oq_Active)
{
   this->mq_Active = oq_Active;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set main data

   \param[in]  orc_Data    New data
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CamDbOsy::SetData(const stw::opensyde_core::C_OscComMessageLoggerOsySysDefConfig & orc_Data)
{
   this->mc_Data = orc_Data;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Find all messages and store the fast access index for them

   Warning: this might take a while
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CamDbOsy::FindAllMessages(void)
{
   if (this->mq_FoundAll == false)
   {
      std::vector<uint32_t> c_NodeIndexes;
      std::vector<uint32_t> c_InterfaceIndexes;
      C_CamDbOsyMessageId c_CurId;
      C_CamDbOsyListId c_CurListId;

      //Clear
      this->mc_FoundMessagesId.clear();
      this->mc_FoundMessagesListId.clear();

      this->mc_Data.c_OsySysDef.GetNodeIndexesOfBus(this->mc_Data.u32_BusIndex, c_NodeIndexes, c_InterfaceIndexes);
      if (c_NodeIndexes.size() == c_InterfaceIndexes.size())
      {
         //All found nodes & interfaces
         for (uint32_t u32_ItFoundItem = 0UL; u32_ItFoundItem < c_NodeIndexes.size();
              ++u32_ItFoundItem)
         {
            if (c_NodeIndexes[u32_ItFoundItem] < this->mc_Data.c_OsySysDef.c_Nodes.size())
            {
               //Found node
               const C_OscNode & rc_Node = this->mc_Data.c_OsySysDef.c_Nodes[c_NodeIndexes[u32_ItFoundItem]];
               //Id
               c_CurListId.c_Id.u32_NodeIndex = c_NodeIndexes[u32_ItFoundItem];
               c_CurId.c_Id.u32_NodeIndex = c_NodeIndexes[u32_ItFoundItem];
               //All protocols
               for (uint8_t u8_ItProt = 0U; u8_ItProt < rc_Node.c_ComProtocols.size(); ++u8_ItProt)
               {
                  const C_OscCanProtocol & rc_Protocol = rc_Node.c_ComProtocols[u8_ItProt];
                  //Id
                  c_CurListId.c_Id.u32_DataPoolIndex = rc_Protocol.u32_DataPoolIndex;
                  c_CurId.c_Id.e_ComProtocol = rc_Protocol.e_Type;
                  c_CurId.c_Id.u32_DatapoolIndex = rc_Protocol.u32_DataPoolIndex;
                  if ((rc_Protocol.u32_DataPoolIndex < rc_Node.c_DataPools.size()) &&
                      (c_InterfaceIndexes[u32_ItFoundItem] < rc_Protocol.c_ComMessages.size()))
                  {
                     const C_OscNodeDataPool & rc_Datapool = rc_Node.c_DataPools[rc_Protocol.u32_DataPoolIndex];
                     //Found container
                     const C_OscCanMessageContainer & rc_Container =
                        rc_Protocol.c_ComMessages[c_InterfaceIndexes[u32_ItFoundItem]];
                     //Id
                     c_CurId.c_Id.u32_InterfaceIndex = c_InterfaceIndexes[u32_ItFoundItem];
                     if ((C_OscCanProtocol::h_GetComListIndex(rc_Datapool, c_InterfaceIndexes[u32_ItFoundItem], false,
                                                              c_CurListId.c_Id.u32_ListIndex) == C_NO_ERR) &&
                         (c_CurListId.c_Id.u32_ListIndex < rc_Datapool.c_Lists.size()))
                     {
                        //Each Rx message
                        for (uint32_t u32_ItMsg = 0UL; u32_ItMsg < rc_Container.c_RxMessages.size(); ++u32_ItMsg)
                        {
                           const C_OscCanMessage & rc_Message = rc_Container.c_RxMessages[u32_ItMsg];
                           //Use first occurance
                           if (!this->mc_FoundMessagesId.contains(rc_Message.c_Name.c_str()))
                           {
                              const uint32_t u32_Hash =
                                 C_CamGenSigUtil::h_CalcMessageHash(rc_Message,
                                                                    rc_Datapool.c_Lists[c_CurListId.c_Id.u32_ListIndex]);
                              //Id
                              c_CurId.c_Id.q_MessageIsTx = false;
                              c_CurId.c_Id.u32_MessageIndex = u32_ItMsg;
                              c_CurId.u32_Hash = u32_Hash;
                              c_CurListId.u32_Hash = u32_Hash;
                              this->mc_FoundMessagesId.insert(rc_Message.c_Name.c_str(), c_CurId);
                              this->mc_FoundMessagesListId.insert(rc_Message.c_Name.c_str(), c_CurListId);
                           }
                        }
                     }
                     if ((C_OscCanProtocol::h_GetComListIndex(rc_Datapool, c_InterfaceIndexes[u32_ItFoundItem], true,
                                                              c_CurListId.c_Id.u32_ListIndex) == C_NO_ERR) &&
                         (c_CurListId.c_Id.u32_ListIndex < rc_Datapool.c_Lists.size()))
                     {
                        //Each Tx message
                        for (uint32_t u32_ItMsg = 0UL;
                             u32_ItMsg < rc_Container.c_TxMessages.size(); ++u32_ItMsg)
                        {
                           const C_OscCanMessage & rc_Message = rc_Container.c_TxMessages[u32_ItMsg];
                           //Use first occurance
                           if (!this->mc_FoundMessagesId.contains(rc_Message.c_Name.c_str()))
                           {
                              const uint32_t u32_Hash =
                                 C_CamGenSigUtil::h_CalcMessageHash(rc_Message,
                                                                    rc_Datapool.c_Lists[c_CurListId.c_Id.u32_ListIndex]);
                              //Id
                              c_CurId.c_Id.q_MessageIsTx = true;
                              c_CurId.c_Id.u32_MessageIndex = u32_ItMsg;
                              c_CurId.u32_Hash = u32_Hash;
                              c_CurListId.u32_Hash = u32_Hash;
                              this->mc_FoundMessagesId.insert(rc_Message.c_Name.c_str(), c_CurId);
                              this->mc_FoundMessagesListId.insert(rc_Message.c_Name.c_str(), c_CurListId);
                           }
                        }
                     }
                  }
               }
            }
         }
      }
      this->mq_FoundAll = true;
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Search for a message with this ID, return the first one found

   \param[in]   ou32_Id          CAN ID to search for
   \param[in]   oq_IsExtended    Is extended
   \param[out]  orc_Message      Found message name (only valid if C_NO_ERR)

   \return
   C_NO_ERR Found at least one matching message
   C_NOACT  No matching message found
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_CamDbOsy::FindMessageById(const uint32_t ou32_Id, const bool oq_IsExtended, QString & orc_Message) const
{
   int32_t s32_Retval = C_NOACT;

   std::vector<uint32_t> c_NodeIndexes;
   std::vector<uint32_t> c_InterfaceIndexes;

   this->mc_Data.c_OsySysDef.GetNodeIndexesOfBus(this->mc_Data.u32_BusIndex, c_NodeIndexes, c_InterfaceIndexes);
   if (c_NodeIndexes.size() == c_InterfaceIndexes.size())
   {
      //All found nodes & interfaces
      for (uint32_t u32_ItFoundItem = 0UL; (u32_ItFoundItem < c_NodeIndexes.size()) && (s32_Retval == C_NOACT);
           ++u32_ItFoundItem)
      {
         if (c_NodeIndexes[u32_ItFoundItem] < this->mc_Data.c_OsySysDef.c_Nodes.size())
         {
            //Found node
            const C_OscNode & rc_Node = this->mc_Data.c_OsySysDef.c_Nodes[c_NodeIndexes[u32_ItFoundItem]];
            //All protocols
            for (uint8_t u8_ItProt = 0U; (u8_ItProt < rc_Node.c_ComProtocols.size()) && (s32_Retval == C_NOACT);
                 ++u8_ItProt)
            {
               const C_OscCanProtocol & rc_Protocol = rc_Node.c_ComProtocols[u8_ItProt];
               if (c_InterfaceIndexes[u32_ItFoundItem] < rc_Protocol.c_ComMessages.size())
               {
                  //Found container
                  const C_OscCanMessageContainer & rc_Container =
                     rc_Protocol.c_ComMessages[c_InterfaceIndexes[u32_ItFoundItem]];
                  //Each Rx message
                  for (uint32_t u32_ItMsg =
                          0UL; (u32_ItMsg < rc_Container.c_RxMessages.size()) && (s32_Retval == C_NOACT);
                       ++u32_ItMsg)
                  {
                     const C_OscCanMessage & rc_Message = rc_Container.c_RxMessages[u32_ItMsg];
                     if ((rc_Message.u32_CanId == ou32_Id) && (rc_Message.q_IsExtended == oq_IsExtended))
                     {
                        //Found match
                        orc_Message = rc_Message.c_Name.c_str();
                        s32_Retval = C_NO_ERR;
                     }
                  }
                  //Each Tx message
                  for (uint32_t u32_ItMsg = 0UL;
                       (u32_ItMsg < rc_Container.c_TxMessages.size()) && (s32_Retval == C_NOACT); ++u32_ItMsg)
                  {
                     const C_OscCanMessage & rc_Message = rc_Container.c_TxMessages[u32_ItMsg];
                     if ((rc_Message.u32_CanId == ou32_Id) && (rc_Message.q_IsExtended == oq_IsExtended))
                     {
                        //Found match
                        orc_Message = rc_Message.c_Name.c_str();
                        s32_Retval = C_NO_ERR;
                     }
                  }
               }
            }
         }
      }
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Find message in data base

   \param[in]  orc_Message    Message name to search for

   \return
   C_NO_ERR Found message
   C_NOACT  Message not found in file
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_CamDbOsy::FindMessage(const QString & orc_Message)
{
   int32_t s32_Retval = C_NOACT;

   //Check if already searched once
   if ((this->mc_FoundMessagesId.contains(orc_Message) == true) &&
       (this->mc_FoundMessagesListId.contains(orc_Message) == true))
   {
      s32_Retval = C_NO_ERR;
   }
   else
   {
      std::vector<uint32_t> c_NodeIndexes;
      std::vector<uint32_t> c_InterfaceIndexes;
      C_CamDbOsyMessageId c_CurId;
      C_CamDbOsyListId c_CurListId;

      this->mc_Data.c_OsySysDef.GetNodeIndexesOfBus(this->mc_Data.u32_BusIndex, c_NodeIndexes, c_InterfaceIndexes);
      if (c_NodeIndexes.size() == c_InterfaceIndexes.size())
      {
         //All found nodes & interfaces
         for (uint32_t u32_ItFoundItem = 0UL; (u32_ItFoundItem < c_NodeIndexes.size()) && (s32_Retval == C_NOACT);
              ++u32_ItFoundItem)
         {
            if (c_NodeIndexes[u32_ItFoundItem] < this->mc_Data.c_OsySysDef.c_Nodes.size())
            {
               //Found node
               const C_OscNode & rc_Node = this->mc_Data.c_OsySysDef.c_Nodes[c_NodeIndexes[u32_ItFoundItem]];
               //Id
               c_CurListId.c_Id.u32_NodeIndex = c_NodeIndexes[u32_ItFoundItem];
               c_CurId.c_Id.u32_NodeIndex = c_NodeIndexes[u32_ItFoundItem];
               //All protocols
               for (uint8_t u8_ItProt = 0U; (u8_ItProt < rc_Node.c_ComProtocols.size()) && (s32_Retval == C_NOACT);
                    ++u8_ItProt)
               {
                  const C_OscCanProtocol & rc_Protocol = rc_Node.c_ComProtocols[u8_ItProt];
                  //Id
                  c_CurListId.c_Id.u32_DataPoolIndex = rc_Protocol.u32_DataPoolIndex;
                  c_CurId.c_Id.e_ComProtocol = rc_Protocol.e_Type;
                  c_CurId.c_Id.u32_DatapoolIndex = rc_Protocol.u32_DataPoolIndex;
                  if ((rc_Protocol.u32_DataPoolIndex < rc_Node.c_DataPools.size()) &&
                      (c_InterfaceIndexes[u32_ItFoundItem] < rc_Protocol.c_ComMessages.size()))
                  {
                     const C_OscNodeDataPool & rc_Datapool = rc_Node.c_DataPools[rc_Protocol.u32_DataPoolIndex];
                     //Found container
                     const C_OscCanMessageContainer & rc_Container =
                        rc_Protocol.c_ComMessages[c_InterfaceIndexes[u32_ItFoundItem]];
                     //Id
                     c_CurId.c_Id.u32_InterfaceIndex = c_InterfaceIndexes[u32_ItFoundItem];
                     //Each Rx message
                     for (uint32_t u32_ItMsg = 0UL; u32_ItMsg < rc_Container.c_RxMessages.size(); ++u32_ItMsg)
                     {
                        const C_OscCanMessage & rc_Message = rc_Container.c_RxMessages[u32_ItMsg];
                        if (orc_Message.compare(rc_Message.c_Name.c_str()) == 0)
                        {
                           //Found match
                           //Id
                           c_CurId.c_Id.q_MessageIsTx = false;
                           c_CurId.c_Id.u32_MessageIndex = u32_ItMsg;
                           if ((C_OscCanProtocol::h_GetComListIndex(rc_Datapool, c_InterfaceIndexes[u32_ItFoundItem],
                                                                    c_CurId.c_Id.q_MessageIsTx,
                                                                    c_CurListId.c_Id.u32_ListIndex) == C_NO_ERR) &&
                               (c_CurListId.c_Id.u32_ListIndex < rc_Datapool.c_Lists.size()))
                           {
                              const uint32_t u32_Hash =
                                 C_CamGenSigUtil::h_CalcMessageHash(rc_Message,
                                                                    rc_Datapool.c_Lists[c_CurListId.c_Id.u32_ListIndex]);
                              c_CurId.u32_Hash = u32_Hash;
                              c_CurListId.u32_Hash = u32_Hash;
                              s32_Retval = C_NO_ERR;
                           }
                           break;
                        }
                     }
                     //Each Tx message
                     for (uint32_t u32_ItMsg = 0UL;
                          (u32_ItMsg < rc_Container.c_TxMessages.size()) && (s32_Retval == C_NOACT); ++u32_ItMsg)
                     {
                        const C_OscCanMessage & rc_Message = rc_Container.c_TxMessages[u32_ItMsg];
                        if (orc_Message.compare(rc_Message.c_Name.c_str()) == 0)
                        {
                           //Found match
                           //Id
                           c_CurId.c_Id.q_MessageIsTx = true;
                           c_CurId.c_Id.u32_MessageIndex = u32_ItMsg;
                           if ((C_OscCanProtocol::h_GetComListIndex(rc_Datapool, c_InterfaceIndexes[u32_ItFoundItem],
                                                                    c_CurId.c_Id.q_MessageIsTx,
                                                                    c_CurListId.c_Id.u32_ListIndex) == C_NO_ERR) &&
                               (c_CurListId.c_Id.u32_ListIndex < rc_Datapool.c_Lists.size()))
                           {
                              const uint32_t u32_Hash =
                                 C_CamGenSigUtil::h_CalcMessageHash(rc_Message,
                                                                    rc_Datapool.c_Lists[c_CurListId.c_Id.u32_ListIndex]);
                              c_CurId.u32_Hash = u32_Hash;
                              c_CurListId.u32_Hash = u32_Hash;
                              s32_Retval = C_NO_ERR;
                           }
                           break;
                        }
                     }
                  }
               }
            }
         }
      }
      //Check if found
      if (s32_Retval == C_NO_ERR)
      {
         //Remember found message index for future fast access
         this->mc_FoundMessagesId.insert(orc_Message, c_CurId);
         this->mc_FoundMessagesListId.insert(orc_Message, c_CurListId);
      }
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get active state of this database

   \return
   Current active state
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_CamDbOsy::GetActive(void) const
{
   return this->mq_Active;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get all found messages

   \return
   All found messages
*/
//----------------------------------------------------------------------------------------------------------------------
const QMap<QString, C_CamDbOsyMessageId> & C_CamDbOsy::GetFoundMessages(void) const
{
   return this->mc_FoundMessagesId;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get OSC CAN message

   Requirement: this function can only return a valid index if the function FindMessage was at least called once

   \param[in]   orc_Message         Message name to look for
   \param[in]   oq_UseHash          Use hash
   \param[in]   ou32_Hash           Hash
   \param[out]  ope_ProtocolType    Protocol type

   \return
   NULL OSC CAN message not found
   Else Valid OSC CAN message
*/
//----------------------------------------------------------------------------------------------------------------------
const stw::opensyde_core::C_OscCanMessage * C_CamDbOsy::GetOscMessage(const QString & orc_Message,
                                                                      const bool oq_UseHash, const uint32_t ou32_Hash,
                                                                      C_OscCanProtocol::E_Type * const ope_ProtocolType)
const
{
   const stw::opensyde_core::C_OscCanMessage * pc_Retval = NULL;

   //Don't allow access to inactive file
   if (this->mq_Active)
   {
      const QMap<QString,
                 C_CamDbOsyMessageId>::const_iterator c_It =
         this->mc_FoundMessagesId.find(orc_Message);

      if (c_It != this->mc_FoundMessagesId.end())
      {
         if ((oq_UseHash == false) || (ou32_Hash == c_It->u32_Hash))
         {
            if (c_It->c_Id.u32_NodeIndex < this->mc_Data.c_OsySysDef.c_Nodes.size())
            {
               const C_OscNode & rc_Node = this->mc_Data.c_OsySysDef.c_Nodes[c_It->c_Id.u32_NodeIndex];
               const C_OscCanProtocol * const pc_Protocol = rc_Node.GetCanProtocolConst(c_It->c_Id.e_ComProtocol,
                                                                                        c_It->c_Id.u32_DatapoolIndex);
               if (pc_Protocol != NULL)
               {
                  if (c_It->c_Id.u32_InterfaceIndex < pc_Protocol->c_ComMessages.size())
                  {
                     const C_OscCanMessageContainer & rc_Container =
                        pc_Protocol->c_ComMessages[c_It->c_Id.u32_InterfaceIndex];
                     const std::vector<C_OscCanMessage> & rc_Messages = rc_Container.GetMessagesConst(
                        c_It->c_Id.q_MessageIsTx);
                     if (c_It->c_Id.u32_MessageIndex < rc_Messages.size())
                     {
                        pc_Retval = &rc_Messages[c_It->c_Id.u32_MessageIndex];
                        if (ope_ProtocolType != NULL)
                        {
                           *ope_ProtocolType = c_It->c_Id.e_ComProtocol;
                        }
                     }
                  }
               }
            }
         }
      }
   }
   return pc_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get OSC list

   Requirement: this function can only return a valid index if the function FindMessage was at least called once

   \param[in]  orc_Message    Message name to look for
   \param[in]  oq_UseHash     Use hash
   \param[in]  ou32_Hash      Hash

   \return
   NULL OSC list not found
   Else Valid OSC list
*/
//----------------------------------------------------------------------------------------------------------------------
const stw::opensyde_core::C_OscNodeDataPoolList * C_CamDbOsy::GetOscList(const QString & orc_Message,
                                                                         const bool oq_UseHash,
                                                                         const uint32_t ou32_Hash) const
{
   const stw::opensyde_core::C_OscNodeDataPoolList * pc_Retval = NULL;

   //Don't allow access to inactive file
   if (this->mq_Active)
   {
      const QMap<QString,
                 C_CamDbOsyListId>::const_iterator c_It = this->mc_FoundMessagesListId.find(orc_Message);

      if (c_It != this->mc_FoundMessagesListId.end())
      {
         if ((oq_UseHash == false) || (ou32_Hash == c_It->u32_Hash))
         {
            if (c_It->c_Id.u32_NodeIndex < this->mc_Data.c_OsySysDef.c_Nodes.size())
            {
               const C_OscNode & rc_Node = this->mc_Data.c_OsySysDef.c_Nodes[c_It->c_Id.u32_NodeIndex];
               if (c_It->c_Id.u32_DataPoolIndex < rc_Node.c_DataPools.size())
               {
                  const C_OscNodeDataPool & rc_DataPool = rc_Node.c_DataPools[c_It->c_Id.u32_DataPoolIndex];
                  if (c_It->c_Id.u32_ListIndex < rc_DataPool.c_Lists.size())
                  {
                     pc_Retval = &rc_DataPool.c_Lists[c_It->c_Id.u32_ListIndex];
                  }
               }
            }
         }
      }
   }
   return pc_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Replace the bus index

   \param[in]  ou32_BusIndex  New bus index

   \return
   True  Is change
   False No change
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_CamDbOsy::ReplaceOsyBusIndex(const uint32_t ou32_BusIndex)
{
   bool q_Retval;

   if (this->mc_Data.u32_BusIndex == ou32_BusIndex)
   {
      //Signal no change
      q_Retval = false;
   }
   else
   {
      this->mc_Data.u32_BusIndex = ou32_BusIndex;
      //Clean up everything (known locations will likely change)
      this->mq_FoundAll = false;
      this->mc_FoundMessagesId.clear();
      this->mc_FoundMessagesListId.clear();
      //Signal change
      q_Retval = true;
   }
   return q_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Check hash for message

   \param[in]  orc_Message    Message
   \param[in]  ou32_Hash      Hash

   \retval   true   Valid
   \retval   false  Invalid
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_CamDbOsy::CheckHashForMessage(const QString & orc_Message, const uint32_t ou32_Hash) const
{
   bool q_Retval = false;

   const QMap<QString, C_CamDbOsyMessageId>::const_iterator c_It = this->mc_FoundMessagesId.find(orc_Message);

   if (c_It != this->mc_FoundMessagesId.end())
   {
      if (c_It->u32_Hash == ou32_Hash)
      {
         q_Retval = true;
      }
   }
   return q_Retval;
}
