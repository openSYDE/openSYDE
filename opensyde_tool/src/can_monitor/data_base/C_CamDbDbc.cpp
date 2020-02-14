//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Handler class for DBC data (implementation)

   Handler class for DBC data

   \copyright   Copyright 2019 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.h"

#include "stwtypes.h"
#include "stwerrors.h"
#include "C_CamDbDbc.h"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw_types;
using namespace stw_errors;
using namespace stw_opensyde_gui_logic;

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

   \param[in] oq_Active New active state
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CamDbDbc::SetActive(const bool oq_Active)
{
   this->mq_Active = oq_Active;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set main data

   \param[in] orc_Data New data
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CamDbDbc::SetData(const C_CieConverter::C_CIECommDefinition & orc_Data)
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
      for (uint32 u32_ItNode = 0UL; u32_ItNode < this->mc_Data.c_Nodes.size();
           ++u32_ItNode)
      {
         const C_CieConverter::C_CIENode & rc_Node = this->mc_Data.c_Nodes[u32_ItNode];
         //Id
         c_Id.u32_NodeIndex = u32_ItNode;
         //Each Rx message
         for (uint32 u32_ItMessage = 0UL; u32_ItMessage < rc_Node.c_RxMessages.size(); ++u32_ItMessage)
         {
            const C_CieConverter::C_CIENodeMessage & rc_Message = rc_Node.c_RxMessages[u32_ItMessage];
            //Id
            c_Id.q_MessageIsTx = false;
            c_Id.u32_MessageIndex = u32_ItMessage;
            //Remember for future access
            this->mc_FoundMessagesNodes.insert(rc_Message.c_CanMessage.c_Name.c_str(), c_Id);
         }
         //Each Tx message
         for (uint32 u32_ItMessage = 0UL; u32_ItMessage < rc_Node.c_TxMessages.size(); ++u32_ItMessage)
         {
            const C_CieConverter::C_CIENodeMessage & rc_Message = rc_Node.c_TxMessages[u32_ItMessage];
            //Id
            c_Id.q_MessageIsTx = true;
            c_Id.u32_MessageIndex = u32_ItMessage;
            //Remember for future access
            this->mc_FoundMessagesNodes.insert(rc_Message.c_CanMessage.c_Name.c_str(), c_Id);
         }
      }
      //Unmapped messages
      for (uint32 u32_ItMessage = 0UL; u32_ItMessage < this->mc_Data.c_UnmappedMessages.size(); ++u32_ItMessage)
      {
         const C_CieConverter::C_CIENodeMessage & rc_Message = this->mc_Data.c_UnmappedMessages[u32_ItMessage];
         //Remember for future access
         this->mc_FoundMessagesUnmapped.insert(rc_Message.c_CanMessage.c_Name.c_str(), u32_ItMessage);
      }
      this->mq_FoundAll = true;
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Search for a message with this ID, return the first one found

   \param[in]  ou32_Id     CAN ID to search for
   \param[out] orc_Message Found message name (only valid if C_NO_ERR)

   \return
   C_NO_ERR Found at least one matching message
   C_NOACT  No matching message found
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_CamDbDbc::FindMessageById(const uint32 ou32_Id, QString & orc_Message) const
{
   sint32 s32_Retval = C_NOACT;

   //Each node
   for (uint32 u32_ItNode = 0UL; (u32_ItNode < this->mc_Data.c_Nodes.size()) && (s32_Retval == C_NOACT);
        ++u32_ItNode)
   {
      const C_CieConverter::C_CIENode & rc_Node = this->mc_Data.c_Nodes[u32_ItNode];
      //Each Rx message
      for (uint32 u32_ItMessage = 0UL; (u32_ItMessage < rc_Node.c_RxMessages.size()) && (s32_Retval == C_NOACT);
           ++u32_ItMessage)
      {
         const C_CieConverter::C_CIENodeMessage & rc_Message = rc_Node.c_RxMessages[u32_ItMessage];
         if (rc_Message.c_CanMessage.u32_CanId == ou32_Id)
         {
            //Found match
            orc_Message = rc_Message.c_CanMessage.c_Name.c_str();
            s32_Retval = C_NO_ERR;
         }
      }
      //Each Tx message
      for (uint32 u32_ItMessage = 0UL; (u32_ItMessage < rc_Node.c_TxMessages.size()) && (s32_Retval == C_NOACT);
           ++u32_ItMessage)
      {
         const C_CieConverter::C_CIENodeMessage & rc_Message = rc_Node.c_TxMessages[u32_ItMessage];
         if (rc_Message.c_CanMessage.u32_CanId == ou32_Id)
         {
            //Found match
            orc_Message = rc_Message.c_CanMessage.c_Name.c_str();
            s32_Retval = C_NO_ERR;
         }
      }
   }
   //Unmapped messages
   for (uint32 u32_ItMessage = 0UL; u32_ItMessage < this->mc_Data.c_UnmappedMessages.size(); ++u32_ItMessage)
   {
      const C_CieConverter::C_CIENodeMessage & rc_Message = this->mc_Data.c_UnmappedMessages[u32_ItMessage];
      if (rc_Message.c_CanMessage.u32_CanId == ou32_Id)
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

   \param[in] orc_Message Message name to search for

   \return
   C_NO_ERR Found message
   C_NOACT  Message not found in file
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_CamDbDbc::FindMessage(const QString & orc_Message)
{
   sint32 s32_Retval = C_NOACT;

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
      for (uint32 u32_ItNode = 0UL; (u32_ItNode < this->mc_Data.c_Nodes.size()) && (s32_Retval == C_NOACT);
           ++u32_ItNode)
      {
         const C_CieConverter::C_CIENode & rc_Node = this->mc_Data.c_Nodes[u32_ItNode];
         //Id
         c_Id.u32_NodeIndex = u32_ItNode;
         //Each Rx message
         for (uint32 u32_ItMessage = 0UL; u32_ItMessage < rc_Node.c_RxMessages.size(); ++u32_ItMessage)
         {
            const C_CieConverter::C_CIENodeMessage & rc_Message = rc_Node.c_RxMessages[u32_ItMessage];
            if (orc_Message.compare(rc_Message.c_CanMessage.c_Name.c_str()) == 0)
            {
               //Found
               s32_Retval = C_NO_ERR;
               //Id
               c_Id.q_MessageIsTx = false;
               c_Id.u32_MessageIndex = u32_ItMessage;
               break;
            }
         }
         //Each Tx message
         for (uint32 u32_ItMessage = 0UL; (u32_ItMessage < rc_Node.c_TxMessages.size()) && (s32_Retval == C_NOACT);
              ++u32_ItMessage)
         {
            const C_CieConverter::C_CIENodeMessage & rc_Message = rc_Node.c_TxMessages[u32_ItMessage];
            if (orc_Message.compare(rc_Message.c_CanMessage.c_Name.c_str()) == 0)
            {
               //Found
               s32_Retval = C_NO_ERR;
               //Id
               c_Id.q_MessageIsTx = true;
               c_Id.u32_MessageIndex = u32_ItMessage;
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
         for (uint32 u32_ItMessage = 0UL; u32_ItMessage < this->mc_Data.c_UnmappedMessages.size(); ++u32_ItMessage)
         {
            const C_CieConverter::C_CIENodeMessage & rc_Message = this->mc_Data.c_UnmappedMessages[u32_ItMessage];
            if (orc_Message.compare(rc_Message.c_CanMessage.c_Name.c_str()) == 0)
            {
               //Found
               s32_Retval = C_NO_ERR;
               //Id
               this->mc_FoundMessagesUnmapped.insert(rc_Message.c_CanMessage.c_Name.c_str(), u32_ItMessage);
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
   c_Retval.reserve(this->mc_FoundMessagesNodes.size() + this->mc_FoundMessagesUnmapped.size());
   for (QList<QString>::const_iterator c_It = c_Keys.begin(); c_It != c_Keys.end(); ++c_It)
   {
      c_Retval.push_back(*c_It);
   }
   //Unmapped
   c_Keys = this->mc_FoundMessagesUnmapped.keys();
   for (QList<QString>::const_iterator c_It = c_Keys.begin(); c_It != c_Keys.end(); ++c_It)
   {
      c_Retval.push_back(*c_It);
   }
   return c_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get DBC message

   Requirement: this function can only return a valid index if the function FindMessage was at least called once

   \param[in] orc_Message Message name to look for

   \return
   NULL DBC message not found
   Else Valid DBC message
*/
//----------------------------------------------------------------------------------------------------------------------
const C_CieConverter::C_CIECanMessage * C_CamDbDbc::GetDBCMessage(const QString & orc_Message) const
{
   const C_CieConverter::C_CIECanMessage * pc_Retval = NULL;

   //Don't allow access to inactive file
   if (this->mq_Active)
   {
      const QMap<QString, C_CamDbDbcMessageId>::const_iterator c_It = this->mc_FoundMessagesNodes.find(orc_Message);

      if (c_It != this->mc_FoundMessagesNodes.end())
      {
         if (c_It->u32_NodeIndex < this->mc_Data.c_Nodes.size())
         {
            const C_CieConverter::C_CIENode & rc_Node = this->mc_Data.c_Nodes[c_It->u32_NodeIndex];
            if (c_It->q_MessageIsTx == false)
            {
               if (c_It->u32_MessageIndex < rc_Node.c_RxMessages.size())
               {
                  const C_CieConverter::C_CIENodeMessage & rc_Message = rc_Node.c_RxMessages[c_It->u32_MessageIndex];
                  pc_Retval = &rc_Message.c_CanMessage;
               }
            }
            else
            {
               if (c_It->u32_MessageIndex < rc_Node.c_TxMessages.size())
               {
                  const C_CieConverter::C_CIENodeMessage & rc_Message = rc_Node.c_TxMessages[c_It->u32_MessageIndex];
                  pc_Retval = &rc_Message.c_CanMessage;
               }
            }
         }
      }
      else
      {
         const QMap<QString, uint32>::const_iterator c_It = this->mc_FoundMessagesUnmapped.find(orc_Message);

         if (c_It != this->mc_FoundMessagesUnmapped.end())
         {
            if (c_It.value() < this->mc_Data.c_UnmappedMessages.size())
            {
               const C_CieConverter::C_CIENodeMessage & rc_Message = this->mc_Data.c_UnmappedMessages[c_It.value()];
               pc_Retval = &rc_Message.c_CanMessage;
            }
         }
      }
   }
   return pc_Retval;
}
