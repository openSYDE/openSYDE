//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Handler class for DBC data (implementation)

   Handler class for DBC data

   \copyright   Copyright 2019 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.hpp"

#include "stwtypes.hpp"
#include "stwerrors.hpp"
#include "C_CamDbDbc.hpp"
#include "C_CamGenSigUtil.hpp"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw::errors;
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
C_CamDbDbc::C_CamDbDbc() :
   mq_Active(true),
   mq_FoundAll(false)
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set active flag

   \param[in]  oq_Active   New active state
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CamDbDbc::SetActive(const bool oq_Active)
{
   this->mq_Active = oq_Active;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set main data

   \param[in]  orc_Data    New data
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CamDbDbc::SetData(const C_CieConverter::C_CieCommDefinition & orc_Data)
{
   this->mc_Data = orc_Data;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Find all messages and store the fast access index for them

   Warning: this might take a while
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CamDbDbc::FindAllMessages(void)
{
   if (this->mq_FoundAll == false)
   {
      C_CamDbDbcMessageId c_Id;

      //Each node
      for (uint32_t u32_ItNode = 0UL; u32_ItNode < this->mc_Data.c_Nodes.size();
           ++u32_ItNode)
      {
         const C_CieConverter::C_CieNode & rc_Node = this->mc_Data.c_Nodes[u32_ItNode];
         //Id
         c_Id.u32_NodeIndex = u32_ItNode;
         //Each Rx message
         for (uint32_t u32_ItMessage = 0UL; u32_ItMessage < rc_Node.c_RxMessages.size(); ++u32_ItMessage)
         {
            const C_CieConverter::C_CieNodeMessage & rc_Message = rc_Node.c_RxMessages[u32_ItMessage];
            //Use first occurance
            if (!this->mc_FoundMessagesNodes.contains(rc_Message.c_CanMessage.c_Name.c_str()))
            {
               //Id
               c_Id.q_MessageIsTx = false;
               c_Id.u32_MessageIndex = u32_ItMessage;
               c_Id.u32_Hash = C_CamGenSigUtil::h_CalcMessageHash(rc_Message.c_CanMessage);
               //Remember for future access
               this->mc_FoundMessagesNodes.insert(rc_Message.c_CanMessage.c_Name.c_str(), c_Id);
            }
         }
         //Each Tx message
         for (uint32_t u32_ItMessage = 0UL; u32_ItMessage < rc_Node.c_TxMessages.size(); ++u32_ItMessage)
         {
            const C_CieConverter::C_CieNodeMessage & rc_Message = rc_Node.c_TxMessages[u32_ItMessage];
            //Use first occurance
            if (!this->mc_FoundMessagesNodes.contains(rc_Message.c_CanMessage.c_Name.c_str()))
            {
               //Id
               c_Id.q_MessageIsTx = true;
               c_Id.u32_MessageIndex = u32_ItMessage;
               c_Id.u32_Hash = C_CamGenSigUtil::h_CalcMessageHash(rc_Message.c_CanMessage);
               //Remember for future access
               this->mc_FoundMessagesNodes.insert(rc_Message.c_CanMessage.c_Name.c_str(), c_Id);
            }
         }
      }
      //Unmapped messages
      for (uint32_t u32_ItMessage = 0UL; u32_ItMessage < this->mc_Data.c_UnmappedMessages.size(); ++u32_ItMessage)
      {
         const C_CieConverter::C_CieNodeMessage & rc_Message = this->mc_Data.c_UnmappedMessages[u32_ItMessage];
         //Use first occurance
         if (!this->mc_FoundMessagesUnmapped.contains(rc_Message.c_CanMessage.c_Name.c_str()))
         {
            C_CamDbDbcUnmappedMessageId c_UnmappedId;
            c_UnmappedId.u32_Index = u32_ItMessage;
            c_UnmappedId.u32_Hash = C_CamGenSigUtil::h_CalcMessageHash(rc_Message.c_CanMessage);
            //Remember for future access
            this->mc_FoundMessagesUnmapped.insert(rc_Message.c_CanMessage.c_Name.c_str(), c_UnmappedId);
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
int32_t C_CamDbDbc::FindMessageById(const uint32_t ou32_Id, const bool oq_IsExtended, QString & orc_Message) const
{
   int32_t s32_Retval = C_NOACT;

   //Each node
   for (uint32_t u32_ItNode = 0UL; (u32_ItNode < this->mc_Data.c_Nodes.size()) && (s32_Retval == C_NOACT);
        ++u32_ItNode)
   {
      const C_CieConverter::C_CieNode & rc_Node = this->mc_Data.c_Nodes[u32_ItNode];
      //Each Rx message
      for (uint32_t u32_ItMessage = 0UL; (u32_ItMessage < rc_Node.c_RxMessages.size()) && (s32_Retval == C_NOACT);
           ++u32_ItMessage)
      {
         const C_CieConverter::C_CieNodeMessage & rc_Message = rc_Node.c_RxMessages[u32_ItMessage];
         if ((rc_Message.c_CanMessage.u32_CanId == ou32_Id) && (rc_Message.c_CanMessage.q_IsExtended == oq_IsExtended))
         {
            //Found match
            orc_Message = rc_Message.c_CanMessage.c_Name.c_str();
            s32_Retval = C_NO_ERR;
         }
      }
      //Each Tx message
      for (uint32_t u32_ItMessage = 0UL; (u32_ItMessage < rc_Node.c_TxMessages.size()) && (s32_Retval == C_NOACT);
           ++u32_ItMessage)
      {
         const C_CieConverter::C_CieNodeMessage & rc_Message = rc_Node.c_TxMessages[u32_ItMessage];
         if ((rc_Message.c_CanMessage.u32_CanId == ou32_Id) && (rc_Message.c_CanMessage.q_IsExtended == oq_IsExtended))
         {
            //Found match
            orc_Message = rc_Message.c_CanMessage.c_Name.c_str();
            s32_Retval = C_NO_ERR;
         }
      }
   }
   //Unmapped messages
   for (uint32_t u32_ItMessage = 0UL; u32_ItMessage < this->mc_Data.c_UnmappedMessages.size(); ++u32_ItMessage)
   {
      const C_CieConverter::C_CieNodeMessage & rc_Message = this->mc_Data.c_UnmappedMessages[u32_ItMessage];
      if ((rc_Message.c_CanMessage.u32_CanId == ou32_Id) && (rc_Message.c_CanMessage.q_IsExtended == oq_IsExtended))
      {
         //Found match
         orc_Message = rc_Message.c_CanMessage.c_Name.c_str();
         s32_Retval = C_NO_ERR;
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
int32_t C_CamDbDbc::FindMessage(const QString & orc_Message)
{
   int32_t s32_Retval = C_NOACT;

   //Check if already searched once
   if ((this->mc_FoundMessagesNodes.contains(orc_Message) == true) ||
       (this->mc_FoundMessagesUnmapped.contains(orc_Message) == true))
   {
      s32_Retval = C_NO_ERR;
   }
   else
   {
      C_CamDbDbcMessageId c_Id;

      //Each node
      for (uint32_t u32_ItNode = 0UL; (u32_ItNode < this->mc_Data.c_Nodes.size()) && (s32_Retval == C_NOACT);
           ++u32_ItNode)
      {
         const C_CieConverter::C_CieNode & rc_Node = this->mc_Data.c_Nodes[u32_ItNode];
         //Id
         c_Id.u32_NodeIndex = u32_ItNode;
         //Each Rx message
         for (uint32_t u32_ItMessage = 0UL; u32_ItMessage < rc_Node.c_RxMessages.size(); ++u32_ItMessage)
         {
            const C_CieConverter::C_CieNodeMessage & rc_Message = rc_Node.c_RxMessages[u32_ItMessage];
            if (orc_Message.compare(rc_Message.c_CanMessage.c_Name.c_str()) == 0)
            {
               //Found
               s32_Retval = C_NO_ERR;
               //Id
               c_Id.q_MessageIsTx = false;
               c_Id.u32_MessageIndex = u32_ItMessage;
               c_Id.u32_Hash = C_CamGenSigUtil::h_CalcMessageHash(rc_Message.c_CanMessage);
               break;
            }
         }
         //Each Tx message
         for (uint32_t u32_ItMessage = 0UL; (u32_ItMessage < rc_Node.c_TxMessages.size()) && (s32_Retval == C_NOACT);
              ++u32_ItMessage)
         {
            const C_CieConverter::C_CieNodeMessage & rc_Message = rc_Node.c_TxMessages[u32_ItMessage];
            if (orc_Message.compare(rc_Message.c_CanMessage.c_Name.c_str()) == 0)
            {
               //Found
               s32_Retval = C_NO_ERR;
               //Id
               c_Id.q_MessageIsTx = true;
               c_Id.u32_MessageIndex = u32_ItMessage;
               c_Id.u32_Hash = C_CamGenSigUtil::h_CalcMessageHash(rc_Message.c_CanMessage);
               break;
            }
         }
      }
      if (s32_Retval == C_NO_ERR)
      {
         //Remember for future access
         this->mc_FoundMessagesNodes.insert(orc_Message, c_Id);
      }
      else
      {
         //Unmapped messages
         for (uint32_t u32_ItMessage = 0UL; u32_ItMessage < this->mc_Data.c_UnmappedMessages.size(); ++u32_ItMessage)
         {
            const C_CieConverter::C_CieNodeMessage & rc_Message = this->mc_Data.c_UnmappedMessages[u32_ItMessage];
            if (orc_Message.compare(rc_Message.c_CanMessage.c_Name.c_str()) == 0)
            {
               C_CamDbDbcUnmappedMessageId c_UnmappedId;
               //Found
               s32_Retval = C_NO_ERR;
               c_UnmappedId.u32_Index = u32_ItMessage;
               c_UnmappedId.u32_Hash = C_CamGenSigUtil::h_CalcMessageHash(rc_Message.c_CanMessage);
               //Id
               this->mc_FoundMessagesUnmapped.insert(rc_Message.c_CanMessage.c_Name.c_str(), c_UnmappedId);
               break;
            }
         }
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
bool C_CamDbDbc::GetActive(void) const
{
   return this->mq_Active;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get all found messages

   \return
   All found messages
*/
//----------------------------------------------------------------------------------------------------------------------
const std::vector<QString> C_CamDbDbc::GetFoundMessages(void) const
{
   std::vector<QString> c_Retval;
   QList<QString> c_Keys = this->mc_FoundMessagesNodes.keys();
   //Nodes
   c_Retval.reserve(static_cast<uint>(this->mc_FoundMessagesNodes.size() + this->mc_FoundMessagesUnmapped.size()));
   for (QList<QString>::const_iterator c_It = c_Keys.cbegin(); c_It != c_Keys.cend(); ++c_It)
   {
      c_Retval.push_back(*c_It);
   }
   //Unmapped
   c_Keys = this->mc_FoundMessagesUnmapped.keys();
   for (QList<QString>::const_iterator c_It = c_Keys.cbegin(); c_It != c_Keys.cend(); ++c_It)
   {
      c_Retval.push_back(*c_It);
   }
   return c_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get DBC message

   Requirement: this function can only return a valid index if the function FindMessage was at least called once

   \param[in]  orc_Message    Message name to look for
   \param[in]  oq_UseHash     Use hash
   \param[in]  ou32_Hash      Hash

   \return
   NULL DBC message not found
   Else Valid DBC message
*/
//----------------------------------------------------------------------------------------------------------------------
const C_CieConverter::C_CieCanMessage * C_CamDbDbc::GetDbcMessage(const QString & orc_Message, const bool oq_UseHash,
                                                                  const uint32_t ou32_Hash) const
{
   const C_CieConverter::C_CieCanMessage * pc_Retval = NULL;

   //Don't allow access to inactive file
   if (this->mq_Active)
   {
      const QMap<QString, C_CamDbDbcMessageId>::const_iterator c_ItMessage = this->mc_FoundMessagesNodes.find(
         orc_Message);

      if (c_ItMessage != this->mc_FoundMessagesNodes.end())
      {
         if ((oq_UseHash == false) || (ou32_Hash == c_ItMessage->u32_Hash))
         {
            if (c_ItMessage->u32_NodeIndex < this->mc_Data.c_Nodes.size())
            {
               const C_CieConverter::C_CieNode & rc_Node = this->mc_Data.c_Nodes[c_ItMessage->u32_NodeIndex];
               if (c_ItMessage->q_MessageIsTx == false)
               {
                  if (c_ItMessage->u32_MessageIndex < rc_Node.c_RxMessages.size())
                  {
                     const C_CieConverter::C_CieNodeMessage & rc_Message =
                        rc_Node.c_RxMessages[c_ItMessage->u32_MessageIndex];
                     pc_Retval = &rc_Message.c_CanMessage;
                  }
               }
               else
               {
                  if (c_ItMessage->u32_MessageIndex < rc_Node.c_TxMessages.size())
                  {
                     const C_CieConverter::C_CieNodeMessage & rc_Message =
                        rc_Node.c_TxMessages[c_ItMessage->u32_MessageIndex];
                     pc_Retval = &rc_Message.c_CanMessage;
                  }
               }
            }
         }
      }
      else
      {
         const QMap<QString, C_CamDbDbcUnmappedMessageId>::const_iterator c_It = this->mc_FoundMessagesUnmapped.find(
            orc_Message);

         if (c_It != this->mc_FoundMessagesUnmapped.end())
         {
            if ((oq_UseHash == false) || (ou32_Hash == c_It->u32_Hash))
            {
               if (c_It->u32_Index < this->mc_Data.c_UnmappedMessages.size())
               {
                  const C_CieConverter::C_CieNodeMessage & rc_Message =
                     this->mc_Data.c_UnmappedMessages[c_It->u32_Index];
                  pc_Retval = &rc_Message.c_CanMessage;
               }
            }
         }
      }
   }
   return pc_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Check hash for message

   \param[in]  orc_Message    Message
   \param[in]  ou32_Hash      Hash

   \retval   true   Valid
   \retval   false  Invalid
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_CamDbDbc::CheckHashForMessage(const QString & orc_Message, const uint32_t ou32_Hash) const
{
   bool q_Retval = false;

   const QMap<QString, C_CamDbDbcMessageId>::const_iterator c_ItMapped = this->mc_FoundMessagesNodes.find(orc_Message);
   const QMap<QString, C_CamDbDbcUnmappedMessageId>::const_iterator c_ItUnmapped =
      this->mc_FoundMessagesUnmapped.find(orc_Message);

   if (c_ItMapped != this->mc_FoundMessagesNodes.end())
   {
      if (c_ItMapped->u32_Hash == ou32_Hash)
      {
         q_Retval = true;
      }
   }
   if (c_ItUnmapped != this->mc_FoundMessagesUnmapped.end())
   {
      if (c_ItUnmapped->u32_Hash == ou32_Hash)
      {
         q_Retval = true;
      }
   }
   return q_Retval;
}
