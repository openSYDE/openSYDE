//-----------------------------------------------------------------------------
/*!
   \internal
   \file
   \brief       Handler class for OSY data (implementation)

   Handler class for OSY data

   \implementation
   project     openSYDE
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     17.01.2019  STW/M.Echtler
   \endimplementation
*/
//-----------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------- */
#include "precomp_headers.h"

#include "stwtypes.h"
#include "stwerrors.h"
#include "C_CamDbOsy.h"

/* -- Used Namespaces ------------------------------------------------------ */
using namespace stw_types;
using namespace stw_errors;
using namespace stw_opensyde_core;
using namespace stw_opensyde_gui_logic;

/* -- Module Global Constants ---------------------------------------------- */

/* -- Types ---------------------------------------------------------------- */

/* -- Global Variables ----------------------------------------------------- */

/* -- Module Global Variables ---------------------------------------------- */

/* -- Module Global Function Prototypes ------------------------------------ */

/* -- Implementation ------------------------------------------------------- */

//-----------------------------------------------------------------------------
/*!
   \brief   Default constructor

   \created     17.01.2019  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
C_CamDbOsy::C_CamDbOsy() :
   mq_Active(true),
   mq_FoundAll(false),
   mc_Data(C_OSCSystemDefinition(), 0UL)
{
}

//-----------------------------------------------------------------------------
/*!
   \brief   Set active flag

   \param[in] oq_Active New active state

   \created     23.01.2019  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_CamDbOsy::SetActive(const bool oq_Active)
{
   this->mq_Active = oq_Active;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Set main data

   \param[in] orc_Data New data

   \created     17.01.2019  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_CamDbOsy::SetData(const stw_opensyde_core::C_OSCComMessageLoggerOsySysDefConfig & orc_Data)
{
   this->mc_Data = orc_Data;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Find all messages and store the fast access index for them

   Warning: this might take a while

   \created     21.01.2019  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_CamDbOsy::FindAllMessages(void)
{
   if (this->mq_FoundAll == false)
   {
      std::vector<uint32> c_NodeIndexes;
      std::vector<uint32> c_InterfaceIndexes;
      C_OSCCanMessageIdentificationIndices c_CurId;
      C_OSCNodeDataPoolListId c_CurListId;

      this->mc_Data.c_OsySysDef.GetNodeIndexesOfBus(this->mc_Data.u32_BusIndex, c_NodeIndexes, c_InterfaceIndexes);
      if (c_NodeIndexes.size() == c_InterfaceIndexes.size())
      {
         //All found nodes & interfaces
         for (uint32 u32_ItFoundItem = 0UL; u32_ItFoundItem < c_NodeIndexes.size();
              ++u32_ItFoundItem)
         {
            if (c_NodeIndexes[u32_ItFoundItem] < this->mc_Data.c_OsySysDef.c_Nodes.size())
            {
               //Found node
               const C_OSCNode & rc_Node = this->mc_Data.c_OsySysDef.c_Nodes[c_NodeIndexes[u32_ItFoundItem]];
               //Id
               c_CurListId.u32_NodeIndex = c_NodeIndexes[u32_ItFoundItem];
               c_CurId.u32_NodeIndex = c_NodeIndexes[u32_ItFoundItem];
               //All protocols
               for (uint8 u32_ItProt = 0U; u32_ItProt < rc_Node.c_ComProtocols.size();
                    ++u32_ItProt)
               {
                  const C_OSCCanProtocol & rc_Protocol = rc_Node.c_ComProtocols[u32_ItProt];
                  //Id
                  c_CurListId.u32_DataPoolIndex = rc_Protocol.u32_DataPoolIndex;
                  c_CurId.e_ComProtocol = rc_Protocol.e_Type;
                  if ((rc_Protocol.u32_DataPoolIndex < rc_Node.c_DataPools.size()) &&
                      (c_InterfaceIndexes[u32_ItFoundItem] < rc_Protocol.c_ComMessages.size()))
                  {
                     const C_OSCNodeDataPool & rc_Datapool = rc_Node.c_DataPools[rc_Protocol.u32_DataPoolIndex];
                     //Found container
                     const C_OSCCanMessageContainer & rc_Container =
                        rc_Protocol.c_ComMessages[c_InterfaceIndexes[u32_ItFoundItem]];
                     //Id
                     c_CurId.u32_InterfaceIndex = c_InterfaceIndexes[u32_ItFoundItem];
                     if (C_OSCCanProtocol::h_GetComListIndex(rc_Datapool, c_InterfaceIndexes[u32_ItFoundItem], false,
                                                             c_CurListId.u32_ListIndex) == C_NO_ERR)
                     {
                        //Each RX message
                        for (uint32 u32_ItMsg = 0UL; u32_ItMsg < rc_Container.c_RxMessages.size(); ++u32_ItMsg)
                        {
                           const C_OSCCanMessage & rc_Message = rc_Container.c_RxMessages[u32_ItMsg];
                           //Id
                           c_CurId.q_MessageIsTx = false;
                           c_CurId.u32_MessageIndex = u32_ItMsg;
                           this->mc_FoundMessagesId.insert(rc_Message.c_Name.c_str(), c_CurId);
                           this->mc_FoundMessagesListId.insert(rc_Message.c_Name.c_str(), c_CurListId);
                        }
                     }
                     if (C_OSCCanProtocol::h_GetComListIndex(rc_Datapool, c_InterfaceIndexes[u32_ItFoundItem], true,
                                                             c_CurListId.u32_ListIndex) == C_NO_ERR)
                     {
                        //Each TX message
                        for (uint32 u32_ItMsg = 0UL;
                             u32_ItMsg < rc_Container.c_TxMessages.size(); ++u32_ItMsg)
                        {
                           const C_OSCCanMessage & rc_Message = rc_Container.c_TxMessages[u32_ItMsg];
                           //Id
                           c_CurId.q_MessageIsTx = true;
                           c_CurId.u32_MessageIndex = u32_ItMsg;
                           this->mc_FoundMessagesId.insert(rc_Message.c_Name.c_str(), c_CurId);
                           this->mc_FoundMessagesListId.insert(rc_Message.c_Name.c_str(), c_CurListId);
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

//-----------------------------------------------------------------------------
/*!
   \brief   Search for a message with this ID, return the first one found

   \param[in]  ou32_Id     CAN ID to search for
   \param[out] orc_Message Found message name (only valid if C_NO_ERR)

   \return
   C_NO_ERR Found at least one matching message
   C_NOACT  No matching message found

   \created     22.01.2019  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
sint32 C_CamDbOsy::FindMessageById(const uint32 ou32_Id, QString & orc_Message) const
{
   sint32 s32_Retval = C_NOACT;

   std::vector<uint32> c_NodeIndexes;
   std::vector<uint32> c_InterfaceIndexes;

   this->mc_Data.c_OsySysDef.GetNodeIndexesOfBus(this->mc_Data.u32_BusIndex, c_NodeIndexes, c_InterfaceIndexes);
   if (c_NodeIndexes.size() == c_InterfaceIndexes.size())
   {
      //All found nodes & interfaces
      for (uint32 u32_ItFoundItem = 0UL; (u32_ItFoundItem < c_NodeIndexes.size()) && (s32_Retval == C_NOACT);
           ++u32_ItFoundItem)
      {
         if (c_NodeIndexes[u32_ItFoundItem] < this->mc_Data.c_OsySysDef.c_Nodes.size())
         {
            //Found node
            const C_OSCNode & rc_Node = this->mc_Data.c_OsySysDef.c_Nodes[c_NodeIndexes[u32_ItFoundItem]];
            //All protocols
            for (uint8 u32_ItProt = 0U; (u32_ItProt < rc_Node.c_ComProtocols.size()) && (s32_Retval == C_NOACT);
                 ++u32_ItProt)
            {
               const C_OSCCanProtocol & rc_Protocol = rc_Node.c_ComProtocols[u32_ItProt];
               if (c_InterfaceIndexes[u32_ItFoundItem] < rc_Protocol.c_ComMessages.size())
               {
                  //Found container
                  const C_OSCCanMessageContainer & rc_Container =
                     rc_Protocol.c_ComMessages[c_InterfaceIndexes[u32_ItFoundItem]];
                  //Each RX message
                  for (uint32 u32_ItMsg =
                          0UL; (u32_ItMsg < rc_Container.c_RxMessages.size()) && (s32_Retval == C_NOACT);
                       ++u32_ItMsg)
                  {
                     const C_OSCCanMessage & rc_Message = rc_Container.c_RxMessages[u32_ItMsg];
                     if (rc_Message.u32_CanId == ou32_Id)
                     {
                        //Found match
                        orc_Message = rc_Message.c_Name.c_str();
                        s32_Retval = C_NO_ERR;
                     }
                  }
                  //Each TX message
                  for (uint32 u32_ItMsg = 0UL;
                       (u32_ItMsg < rc_Container.c_TxMessages.size()) && (s32_Retval == C_NOACT); ++u32_ItMsg)
                  {
                     const C_OSCCanMessage & rc_Message = rc_Container.c_TxMessages[u32_ItMsg];
                     if (rc_Message.u32_CanId == ou32_Id)
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

//-----------------------------------------------------------------------------
/*!
   \brief   Find message in data base

   \param[in] orc_Message Message name to search for

   \return
   C_NO_ERR Found message
   C_NOACT  Message not found in file

   \created     17.01.2019  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
sint32 C_CamDbOsy::FindMessage(const QString & orc_Message)
{
   sint32 s32_Retval = C_NOACT;

   //Check if already searched once
   if ((this->mc_FoundMessagesId.contains(orc_Message) == true) &&
       (this->mc_FoundMessagesListId.contains(orc_Message) == true))
   {
      s32_Retval = C_NO_ERR;
   }
   else
   {
      std::vector<uint32> c_NodeIndexes;
      std::vector<uint32> c_InterfaceIndexes;
      C_OSCCanMessageIdentificationIndices c_CurId;
      C_OSCNodeDataPoolListId c_CurListId;

      this->mc_Data.c_OsySysDef.GetNodeIndexesOfBus(this->mc_Data.u32_BusIndex, c_NodeIndexes, c_InterfaceIndexes);
      if (c_NodeIndexes.size() == c_InterfaceIndexes.size())
      {
         //All found nodes & interfaces
         for (uint32 u32_ItFoundItem = 0UL; (u32_ItFoundItem < c_NodeIndexes.size()) && (s32_Retval == C_NOACT);
              ++u32_ItFoundItem)
         {
            if (c_NodeIndexes[u32_ItFoundItem] < this->mc_Data.c_OsySysDef.c_Nodes.size())
            {
               //Found node
               const C_OSCNode & rc_Node = this->mc_Data.c_OsySysDef.c_Nodes[c_NodeIndexes[u32_ItFoundItem]];
               //Id
               c_CurListId.u32_NodeIndex = c_NodeIndexes[u32_ItFoundItem];
               c_CurId.u32_NodeIndex = c_NodeIndexes[u32_ItFoundItem];
               //All protocols
               for (uint8 u32_ItProt = 0U; (u32_ItProt < rc_Node.c_ComProtocols.size()) && (s32_Retval == C_NOACT);
                    ++u32_ItProt)
               {
                  const C_OSCCanProtocol & rc_Protocol = rc_Node.c_ComProtocols[u32_ItProt];
                  //Id
                  c_CurListId.u32_DataPoolIndex = rc_Protocol.u32_DataPoolIndex;
                  c_CurId.e_ComProtocol = rc_Protocol.e_Type;
                  if ((rc_Protocol.u32_DataPoolIndex < rc_Node.c_DataPools.size()) &&
                      (c_InterfaceIndexes[u32_ItFoundItem] < rc_Protocol.c_ComMessages.size()))
                  {
                     const C_OSCNodeDataPool & rc_Datapool = rc_Node.c_DataPools[rc_Protocol.u32_DataPoolIndex];
                     //Found container
                     const C_OSCCanMessageContainer & rc_Container =
                        rc_Protocol.c_ComMessages[c_InterfaceIndexes[u32_ItFoundItem]];
                     //Id
                     c_CurId.u32_InterfaceIndex = c_InterfaceIndexes[u32_ItFoundItem];
                     //Each RX message
                     for (uint32 u32_ItMsg = 0UL; u32_ItMsg < rc_Container.c_RxMessages.size(); ++u32_ItMsg)
                     {
                        const C_OSCCanMessage & rc_Message = rc_Container.c_RxMessages[u32_ItMsg];
                        if (orc_Message.compare(rc_Message.c_Name.c_str()) == 0)
                        {
                           //Found match
                           //Id
                           c_CurId.q_MessageIsTx = false;
                           c_CurId.u32_MessageIndex = u32_ItMsg;
                           if (C_OSCCanProtocol::h_GetComListIndex(rc_Datapool, c_InterfaceIndexes[u32_ItFoundItem],
                                                                   c_CurId.q_MessageIsTx,
                                                                   c_CurListId.u32_ListIndex) == C_NO_ERR)
                           {
                              s32_Retval = C_NO_ERR;
                           }
                           break;
                        }
                     }
                     //Each TX message
                     for (uint32 u32_ItMsg = 0UL;
                          (u32_ItMsg < rc_Container.c_TxMessages.size()) && (s32_Retval == C_NOACT); ++u32_ItMsg)
                     {
                        const C_OSCCanMessage & rc_Message = rc_Container.c_TxMessages[u32_ItMsg];
                        if (orc_Message.compare(rc_Message.c_Name.c_str()) == 0)
                        {
                           //Found match
                           //Id
                           c_CurId.q_MessageIsTx = true;
                           c_CurId.u32_MessageIndex = u32_ItMsg;
                           if (C_OSCCanProtocol::h_GetComListIndex(rc_Datapool, c_InterfaceIndexes[u32_ItFoundItem],
                                                                   c_CurId.q_MessageIsTx,
                                                                   c_CurListId.u32_ListIndex) == C_NO_ERR)
                           {
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

//-----------------------------------------------------------------------------
/*!
   \brief   Get active state of this database

   \return
   Current active state

   \created     23.01.2019  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
bool C_CamDbOsy::GetActive(void) const
{
   return this->mq_Active;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Get all found messages

   \return
   All found messages

   \created     21.01.2019  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
const QMap<QString, C_OSCCanMessageIdentificationIndices> & C_CamDbOsy::GetFoundMessages(void) const
{
   return this->mc_FoundMessagesId;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Get OSC CAN message

   Requirement: this function can only return a valid index if the function FindMessage was at least called once

   \param[in] orc_Message Message name to look for

   \return
   NULL OSC CAN message not found
   Else Valid OSC CAN message

   \created     17.01.2019  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
const stw_opensyde_core::C_OSCCanMessage * C_CamDbOsy::GetOSCMessage(const QString & orc_Message) const
{
   const stw_opensyde_core::C_OSCCanMessage * pc_Retval = NULL;

   //Don't allow access to inactive file
   if (this->mq_Active)
   {
      const QMap<QString,
                 C_OSCCanMessageIdentificationIndices>::const_iterator c_It =
         this->mc_FoundMessagesId.find(orc_Message);

      if (c_It != this->mc_FoundMessagesId.end())
      {
         if (c_It->u32_NodeIndex < this->mc_Data.c_OsySysDef.c_Nodes.size())
         {
            const C_OSCNode & rc_Node = this->mc_Data.c_OsySysDef.c_Nodes[c_It->u32_NodeIndex];
            const C_OSCCanProtocol * const pc_Protocol = rc_Node.GetCANProtocolConst(c_It->e_ComProtocol);
            if (pc_Protocol != NULL)
            {
               if (c_It->u32_InterfaceIndex < pc_Protocol->c_ComMessages.size())
               {
                  const C_OSCCanMessageContainer & rc_Container = pc_Protocol->c_ComMessages[c_It->u32_InterfaceIndex];
                  const std::vector<C_OSCCanMessage> & rc_Messages = rc_Container.GetMessagesConst(c_It->q_MessageIsTx);
                  if (c_It->u32_MessageIndex < rc_Messages.size())
                  {
                     pc_Retval = &rc_Messages[c_It->u32_MessageIndex];
                  }
               }
            }
         }
      }
   }
   return pc_Retval;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Get OSC list

   Requirement: this function can only return a valid index if the function FindMessage was at least called once

   \param[in] orc_Message Message name to look for

   \return
   NULL OSC list not found
   Else Valid OSC list

   \created     17.01.2019  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
const stw_opensyde_core::C_OSCNodeDataPoolList * C_CamDbOsy::GetOSCList(const QString & orc_Message) const
{
   const stw_opensyde_core::C_OSCNodeDataPoolList * pc_Retval = NULL;

   //Don't allow access to inactive file
   if (this->mq_Active)
   {
      const QMap<QString,
                 C_OSCNodeDataPoolListId>::const_iterator c_It = this->mc_FoundMessagesListId.find(orc_Message);

      if (c_It != this->mc_FoundMessagesListId.end())
      {
         if (c_It->u32_NodeIndex < this->mc_Data.c_OsySysDef.c_Nodes.size())
         {
            const C_OSCNode & rc_Node = this->mc_Data.c_OsySysDef.c_Nodes[c_It->u32_NodeIndex];
            if (c_It->u32_DataPoolIndex < rc_Node.c_DataPools.size())
            {
               const C_OSCNodeDataPool & rc_DataPool = rc_Node.c_DataPools[c_It->u32_DataPoolIndex];
               if (c_It->u32_ListIndex < rc_DataPool.c_Lists.size())
               {
                  pc_Retval = &rc_DataPool.c_Lists[c_It->u32_ListIndex];
               }
            }
         }
      }
   }
   return pc_Retval;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Replace the bus index

   \param[in] ou32_BusIndex New bus index

   \return
   True  Is change
   False No change

   \created     22.01.2019  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
bool C_CamDbOsy::ReplaceOsyBusIndex(const uint32 ou32_BusIndex)
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
