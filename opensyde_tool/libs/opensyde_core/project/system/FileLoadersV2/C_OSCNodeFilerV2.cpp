//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Node reader/writer (V2)

   Load / save node data from / to XML file

   \copyright   Copyright 2016 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.h"

#include "stwtypes.h"
#include "stwerrors.h"
#include "CSCLString.h"
#include "TGLUtils.h"
#include "C_OSCNodeFilerV2.h"
#include "C_OSCSystemFilerUtil.h"
#include "C_OSCNodeDataPoolFilerV2.h"
#include "C_OSCLoggingHandler.h"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw_types;
using namespace stw_errors;
using namespace stw_scl;
using namespace stw_opensyde_core;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

/* -- Global Variables ---------------------------------------------------------------------------------------------- */

/* -- Module Global Variables --------------------------------------------------------------------------------------- */

/* -- Module Global Function Prototypes ----------------------------------------------------------------------------- */

/* -- Implementation ------------------------------------------------------------------------------------------------ */

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Load node

   Load node data from XML file
   pre-condition: the passed XML parser has the active node set to "node"
   post-condition: the passed XML parser has the active node set to the same "node"

   \param[in]     ou16_XmlFormatVersion  version of XML format
   \param[out]    orc_Node      data storage
   \param[in,out] orc_XMLParser XML with node active

   \return
   C_NO_ERR   data read
   C_CONFIG   content of file is invalid or incomplete
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_OSCNodeFilerV2::h_LoadNode(const uint16 ou16_XmlFormatVersion, C_OSCNode & orc_Node,
                                    C_OSCXMLParserBase & orc_XMLParser)
{
   sint32 s32_Retval = C_NO_ERR;

   orc_Node.Initialize();

   if (orc_XMLParser.SelectNodeChild("core") == "core")
   {
      //Type
      if (orc_XMLParser.SelectNodeChild("type") == "type")
      {
         orc_Node.c_DeviceType = orc_XMLParser.GetNodeContent();
         //Return
         orc_XMLParser.SelectNodeParent(); //back up to core
      }
      s32_Retval = mh_LoadProperties(orc_Node.c_Properties, orc_XMLParser);
      if (s32_Retval == C_NO_ERR)
      {
         s32_Retval = mh_LoadApplications(orc_Node.c_Applications, orc_XMLParser);
         if (s32_Retval == C_NO_ERR)
         {
            if (orc_XMLParser.SelectNodeChild("com-protocols") == "com-protocols")
            {
               s32_Retval = h_LoadNodeComProtocols(orc_Node.c_ComProtocols, orc_XMLParser);
               if (s32_Retval == C_NO_ERR)
               {
                  //Return
                  tgl_assert(orc_XMLParser.SelectNodeParent() == "core"); //back up to core
               }
            }
            else
            {
               osc_write_log_error("Loading node definition", "Could not find \"com-protocols\" node.");
               s32_Retval = C_CONFIG;
            }
            if (s32_Retval == C_NO_ERR)
            {
               s32_Retval = mh_LoadDataPools(ou16_XmlFormatVersion, orc_Node, orc_XMLParser);
            }
         }
      }
      //Return
      if (s32_Retval == C_NO_ERR)
      {
         tgl_assert(orc_XMLParser.SelectNodeParent() == "node");
      }
   }
   else
   {
      osc_write_log_error("Loading node definition", "Could not find \"core\" node.");
      s32_Retval = C_CONFIG;
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Save node

   Save node to XML file
   pre-condition: the passed XML parser has the active node set to "node"
   post-condition: the passed XML parser has the active node set to the same "node"

   \param[in]     orc_Node      data storage
   \param[in,out] orc_XMLParser XML with node active
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OSCNodeFilerV2::h_SaveNode(const C_OSCNode & orc_Node, C_OSCXMLParserBase & orc_XMLParser)
{
   tgl_assert(orc_XMLParser.CreateAndSelectNodeChild("core") == "core");
   //Type
   orc_XMLParser.CreateNodeChild("type", orc_Node.c_DeviceType);
   mh_SaveProperties(orc_Node.c_Properties, orc_XMLParser);
   mh_SaveApplications(orc_Node.c_Applications, orc_XMLParser);
   orc_XMLParser.CreateAndSelectNodeChild("com-protocols");
   h_SaveNodeComProtocols(orc_Node.c_ComProtocols, orc_XMLParser);
   //Return
   tgl_assert(orc_XMLParser.SelectNodeParent() == "core");
   mh_SaveDataPools(orc_Node.c_DataPools, orc_XMLParser);

   //Return
   tgl_assert(orc_XMLParser.SelectNodeParent() == "node");
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Load node com protocols

   Load node data from XML file
   pre-condition: the passed XML parser has the active node set to "com-protocols"
   post-condition: the passed XML parser has the active node set to the same "com-protocols"

   \param[out]    orc_NodeComProtocols data storage
   \param[in,out] orc_XMLParser        XML with list active

   \return
   C_NO_ERR   data read
   C_CONFIG   content of file is invalid or incomplete
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_OSCNodeFilerV2::h_LoadNodeComProtocols(std::vector<C_OSCCanProtocol> & orc_NodeComProtocols,
                                                C_OSCXMLParserBase & orc_XMLParser)
{
   sint32 s32_Retval = C_NO_ERR;

   C_SCLString c_CurNodeComProtocol;
   uint32 u32_ExpectedSize = 0UL;
   const bool q_ExpectedSizeHere = orc_XMLParser.AttributeExists("length");

   //Check optional length
   if (q_ExpectedSizeHere == true)
   {
      u32_ExpectedSize = orc_XMLParser.GetAttributeUint32("length");
      orc_NodeComProtocols.reserve(u32_ExpectedSize);
   }

   c_CurNodeComProtocol = orc_XMLParser.SelectNodeChild("com-protocol");

   //Clear
   orc_NodeComProtocols.clear();
   if (c_CurNodeComProtocol == "com-protocol")
   {
      C_OSCCanProtocol c_CurComProtocol;
      do
      {
         if (s32_Retval == C_NO_ERR)
         {
            s32_Retval = h_LoadNodeComProtocol(c_CurComProtocol, orc_XMLParser);
         }

         //Append
         orc_NodeComProtocols.push_back(c_CurComProtocol);

         //Next
         c_CurNodeComProtocol = orc_XMLParser.SelectNodeNext("com-protocol");
      }
      while (c_CurNodeComProtocol == "com-protocol");
      //Return
      tgl_assert(orc_XMLParser.SelectNodeParent() == "com-protocols");
   }
   //Compare length
   if ((s32_Retval == C_NO_ERR) && (q_ExpectedSizeHere == true))
   {
      if (u32_ExpectedSize != orc_NodeComProtocols.size())
      {
         C_SCLString c_Tmp;
         c_Tmp.PrintFormatted("Unexpected comm protocol count, expected: %i, got %i", u32_ExpectedSize,
                              orc_NodeComProtocols.size());
         osc_write_log_warning("Load file", c_Tmp.c_str());
      }
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Save node com protocols

   Save node to XML file
   pre-condition: the passed XML parser has the active node set to "com-protocols"
   post-condition: the passed XML parser has the active node set to the same "com-protocols"

   \param[in]     orc_NodeComProtocols data storage
   \param[in,out] orc_XMLParser        XML with list active
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OSCNodeFilerV2::h_SaveNodeComProtocols(const std::vector<C_OSCCanProtocol> & orc_NodeComProtocols,
                                              C_OSCXMLParserBase & orc_XMLParser)
{
   orc_XMLParser.SetAttributeUint32("length", orc_NodeComProtocols.size());
   for (uint32 u32_ItComProtocol = 0; u32_ItComProtocol < orc_NodeComProtocols.size(); ++u32_ItComProtocol)
   {
      orc_XMLParser.CreateAndSelectNodeChild("com-protocol");
      h_SaveNodeComProtocol(orc_NodeComProtocols[u32_ItComProtocol], orc_XMLParser);
      //Return
      tgl_assert(orc_XMLParser.SelectNodeParent() == "com-protocols");
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Load node com protocol

   Load node data from XML file
   pre-condition: the passed XML parser has the active node set to "com-protocol"
   post-condition: the passed XML parser has the active node set to the same "com-protocol"

   \param[out]    orc_NodeComProtocol data storage
   \param[in,out] orc_XMLParser       XML with list active

   \return
   C_NO_ERR   data read
   C_CONFIG   content of file is invalid or incomplete
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_OSCNodeFilerV2::h_LoadNodeComProtocol(C_OSCCanProtocol & orc_NodeComProtocol,
                                               C_OSCXMLParserBase & orc_XMLParser)
{
   sint32 s32_Retval = C_NO_ERR;

   orc_NodeComProtocol.u32_DataPoolIndex = orc_XMLParser.GetAttributeUint32("data-pool-index");

   if (orc_XMLParser.SelectNodeChild("communication-protocol") == "communication-protocol")
   {
      s32_Retval = h_StringToCommunicationProtocol(orc_XMLParser.GetNodeContent(), orc_NodeComProtocol.e_Type);
      //Return
      tgl_assert(orc_XMLParser.SelectNodeParent() == "com-protocol");
   }
   else
   {
      osc_write_log_error("Loading node definition", "Could not find \"communication-protocol\" node.");
      s32_Retval = C_CONFIG;
   }

   if ((orc_XMLParser.SelectNodeChild("com-message-containers") == "com-message-containers") &&
       (s32_Retval == C_NO_ERR))
   {
      s32_Retval = h_LoadNodeComMessageContainers(orc_NodeComProtocol.c_ComMessages, orc_XMLParser);
      //Return
      tgl_assert(orc_XMLParser.SelectNodeParent() == "com-protocol");
   }
   else
   {
      osc_write_log_error("Loading node definition", "Could not find \"com-message-containers\" node.");
      s32_Retval = C_CONFIG;
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Save node com protocol

   Save node to XML file
   pre-condition: the passed XML parser has the active node set to "com-protocol"
   post-condition: the passed XML parser has the active node set to the same "com-protocol"

   \param[in]     orc_NodeComProtocol data storage
   \param[in,out] orc_XMLParser       XML with list active
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OSCNodeFilerV2::h_SaveNodeComProtocol(const C_OSCCanProtocol & orc_NodeComProtocol,
                                             C_OSCXMLParserBase & orc_XMLParser)
{
   orc_XMLParser.SetAttributeUint32("data-pool-index", orc_NodeComProtocol.u32_DataPoolIndex);
   orc_XMLParser.CreateNodeChild("communication-protocol",
                                 h_CommunicationProtocolToString(orc_NodeComProtocol.e_Type));
   //Messages
   orc_XMLParser.CreateAndSelectNodeChild("com-message-containers");
   h_SaveNodeComMessageContainers(orc_NodeComProtocol.c_ComMessages, orc_XMLParser);
   //Return
   tgl_assert(orc_XMLParser.SelectNodeParent() == "com-protocol");
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Load node com message containers

   Load node data from XML file
   pre-condition: the passed XML parser has the active node set to "com-message-containers"
   post-condition: the passed XML parser has the active node set to the same "com-message-containers"

   \param[out]    orc_NodeComMessageContainers data storage
   \param[in,out] orc_XMLParser                XML with list active

   \return
   C_NO_ERR   data read
   C_CONFIG   content of file is invalid or incomplete
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_OSCNodeFilerV2::h_LoadNodeComMessageContainers(
   std::vector<C_OSCCanMessageContainer> & orc_NodeComMessageContainers, C_OSCXMLParserBase & orc_XMLParser)
{
   sint32 s32_Retval = C_NO_ERR;

   C_SCLString c_CurNodeComMessageContainer;
   uint32 u32_ExpectedSize = 0UL;
   const bool q_ExpectedSizeHere = orc_XMLParser.AttributeExists("length");

   //Check optional length
   if (q_ExpectedSizeHere == true)
   {
      u32_ExpectedSize = orc_XMLParser.GetAttributeUint32("length");
      orc_NodeComMessageContainers.reserve(u32_ExpectedSize);
   }

   c_CurNodeComMessageContainer = orc_XMLParser.SelectNodeChild("com-message-container");

   //Clear
   orc_NodeComMessageContainers.clear();
   if (c_CurNodeComMessageContainer == "com-message-container")
   {
      C_OSCCanMessageContainer c_CurComMessageContainer;
      do
      {
         if (s32_Retval == C_NO_ERR)
         {
            s32_Retval = h_LoadNodeComMessageContainer(c_CurComMessageContainer, orc_XMLParser);
         }

         //Append
         orc_NodeComMessageContainers.push_back(c_CurComMessageContainer);

         //Next
         c_CurNodeComMessageContainer = orc_XMLParser.SelectNodeNext("com-message-container");
      }
      while (c_CurNodeComMessageContainer == "com-message-container");
      //Return
      tgl_assert(orc_XMLParser.SelectNodeParent() == "com-message-containers");
   }
   //Compare length
   if ((s32_Retval == C_NO_ERR) && (q_ExpectedSizeHere == true))
   {
      if (u32_ExpectedSize != orc_NodeComMessageContainers.size())
      {
         C_SCLString c_Tmp;
         c_Tmp.PrintFormatted("Unexpected comm message container count, expected: %i, got %i", u32_ExpectedSize,
                              orc_NodeComMessageContainers.size());
         osc_write_log_warning("Load file", c_Tmp.c_str());
      }
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Save node com message containers

   Save node to XML file
   pre-condition: the passed XML parser has the active node set to "com-message-containers"
   post-condition: the passed XML parser has the active node set to the same "com-message-containers"

   \param[in]     orc_NodeComMessageContainers data storage
   \param[in,out] orc_XMLParser                XML with list active
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OSCNodeFilerV2::h_SaveNodeComMessageContainers(
   const std::vector<C_OSCCanMessageContainer> & orc_NodeComMessageContainers, C_OSCXMLParserBase & orc_XMLParser)
{
   orc_XMLParser.SetAttributeUint32("length", orc_NodeComMessageContainers.size());
   for (uint32 u32_ItComMessageContainer = 0; u32_ItComMessageContainer < orc_NodeComMessageContainers.size();
        ++u32_ItComMessageContainer)
   {
      orc_XMLParser.CreateAndSelectNodeChild("com-message-container");
      h_SaveNodeComMessageContainer(orc_NodeComMessageContainers[u32_ItComMessageContainer], orc_XMLParser);
      //Return
      tgl_assert(orc_XMLParser.SelectNodeParent() == "com-message-containers");
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Load node com message container

   Load node data from XML file
   pre-condition: the passed XML parser has the active node set to "com-message-container"
   post-condition: the passed XML parser has the active node set to the same "com-message-container"

   \param[out]    orc_NodeComMessageContainer data storage
   \param[in,out] orc_XMLParser               XML with list active

   \return
   C_NO_ERR   data read
   C_CONFIG   content of file is invalid or incomplete
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_OSCNodeFilerV2::h_LoadNodeComMessageContainer(C_OSCCanMessageContainer & orc_NodeComMessageContainer,
                                                       C_OSCXMLParserBase & orc_XMLParser)
{
   sint32 s32_Retval = C_NO_ERR;

   orc_NodeComMessageContainer.q_IsComProtocolUsedByInterface =
      orc_XMLParser.GetAttributeBool("com-protocol-usage-flag");

   if (orc_XMLParser.SelectNodeChild("tx-messages") == "tx-messages")
   {
      s32_Retval = h_LoadNodeComMessages(orc_NodeComMessageContainer.c_TxMessages, orc_XMLParser);
      if (s32_Retval == C_NO_ERR)
      {
         //Return
         tgl_assert(orc_XMLParser.SelectNodeParent() == "com-message-container");
      }
   }
   else
   {
      osc_write_log_error("Loading node definition", "Could not find \"tx-messages\" node.");
      s32_Retval = C_CONFIG;
   }

   if ((orc_XMLParser.SelectNodeChild("rx-messages") == "rx-messages") && (s32_Retval == C_NO_ERR))
   {
      s32_Retval = h_LoadNodeComMessages(orc_NodeComMessageContainer.c_RxMessages, orc_XMLParser);
      if (s32_Retval == C_NO_ERR)
      {
         //Return
         tgl_assert(orc_XMLParser.SelectNodeParent() == "com-message-container");
      }
   }
   else
   {
      osc_write_log_error("Loading node definition", "Could not find \"rx-messages\" node.");
      s32_Retval = C_CONFIG;
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Save node com message container

   Save node to XML file
   pre-condition: the passed XML parser has the active node set to "com-message-container"
   post-condition: the passed XML parser has the active node set to the same "com-message-container"

   \param[in]     orc_NodeComMessageContainer data storage
   \param[in,out] orc_XMLParser               XML with list active
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OSCNodeFilerV2::h_SaveNodeComMessageContainer(const C_OSCCanMessageContainer & orc_NodeComMessageContainer,
                                                     C_OSCXMLParserBase & orc_XMLParser)
{
   orc_XMLParser.SetAttributeBool("com-protocol-usage-flag",
                                  orc_NodeComMessageContainer.q_IsComProtocolUsedByInterface);
   //Tx messages
   orc_XMLParser.CreateAndSelectNodeChild("tx-messages");
   h_SaveNodeComMessages(orc_NodeComMessageContainer.c_TxMessages, orc_XMLParser);
   //Return
   tgl_assert(orc_XMLParser.SelectNodeParent() == "com-message-container");
   //Rx messages
   orc_XMLParser.CreateAndSelectNodeChild("rx-messages");
   h_SaveNodeComMessages(orc_NodeComMessageContainer.c_RxMessages, orc_XMLParser);
   //Return
   tgl_assert(orc_XMLParser.SelectNodeParent() == "com-message-container");
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Load node com messages

   Load node data from XML file
   pre-condition: the passed XML parser has the active node set to unknown
   post-condition: the passed XML parser has the active node set to the same unknown

   \param[out]    orc_NodeComMessages data storage
   \param[in,out] orc_XMLParser       XML with list active

   \return
   C_NO_ERR   data read
   C_CONFIG   content of file is invalid or incomplete
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_OSCNodeFilerV2::h_LoadNodeComMessages(std::vector<C_OSCCanMessage> & orc_NodeComMessages,
                                               C_OSCXMLParserBase & orc_XMLParser)
{
   sint32 s32_Retval = C_NO_ERR;

   C_SCLString c_CurNodeComMessage;
   uint32 u32_ExpectedSize = 0UL;
   const bool q_ExpectedSizeHere = orc_XMLParser.AttributeExists("length");

   //Check optional length
   if (q_ExpectedSizeHere == true)
   {
      u32_ExpectedSize = orc_XMLParser.GetAttributeUint32("length");
      orc_NodeComMessages.reserve(u32_ExpectedSize);
   }

   c_CurNodeComMessage = orc_XMLParser.SelectNodeChild("com-message");

   //Clear
   orc_NodeComMessages.clear();
   if (c_CurNodeComMessage == "com-message")
   {
      do
      {
         C_OSCCanMessage c_CurComMessage;

         if (s32_Retval == C_NO_ERR)
         {
            s32_Retval = h_LoadNodeComMessage(c_CurComMessage, orc_XMLParser);
         }

         //Append
         orc_NodeComMessages.push_back(c_CurComMessage);

         //Next
         c_CurNodeComMessage = orc_XMLParser.SelectNodeNext("com-message");
      }
      while (c_CurNodeComMessage == "com-message");
      //Return
      orc_XMLParser.SelectNodeParent();
   }
   //Compare length
   if ((s32_Retval == C_NO_ERR) && (q_ExpectedSizeHere == true))
   {
      if (u32_ExpectedSize != orc_NodeComMessages.size())
      {
         C_SCLString c_Tmp;
         c_Tmp.PrintFormatted("Unexpected messages count, expected: %i, got %i", u32_ExpectedSize,
                              orc_NodeComMessages.size());
         osc_write_log_warning("Load file", c_Tmp.c_str());
      }
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Save node com messages

   Save node to XML file
   pre-condition: the passed XML parser has the active node set to unknown
   post-condition: the passed XML parser has the active node set to the same unknown

   \param[in]     orc_NodeComMessages data storage
   \param[in,out] orc_XMLParser       XML with list active
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OSCNodeFilerV2::h_SaveNodeComMessages(const std::vector<C_OSCCanMessage> & orc_NodeComMessages,
                                             C_OSCXMLParserBase & orc_XMLParser)
{
   orc_XMLParser.SetAttributeUint32("length", orc_NodeComMessages.size());
   for (uint32 u32_ItComMessage = 0; u32_ItComMessage < orc_NodeComMessages.size(); ++u32_ItComMessage)
   {
      orc_XMLParser.CreateAndSelectNodeChild("com-message");
      h_SaveNodeComMessage(orc_NodeComMessages[u32_ItComMessage], orc_XMLParser);
      //Return
      orc_XMLParser.SelectNodeParent();
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Load node com message

   Load node data from XML file
   pre-condition: the passed XML parser has the active node set to "com-message"
   post-condition: the passed XML parser has the active node set to the same "com-message"

   \param[out]    orc_NodeComMessage data storage
   \param[in,out] orc_XMLParser      XML with list active

   \return
   C_NO_ERR   data read
   C_CONFIG   content of file is invalid or incomplete
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_OSCNodeFilerV2::h_LoadNodeComMessage(C_OSCCanMessage & orc_NodeComMessage, C_OSCXMLParserBase & orc_XMLParser)
{
   sint32 s32_Retval = C_NO_ERR;

   orc_NodeComMessage.u32_CanId = orc_XMLParser.GetAttributeUint32("can-id");
   orc_NodeComMessage.q_IsExtended = orc_XMLParser.GetAttributeBool("is-extended");
   orc_NodeComMessage.u16_Dlc = static_cast<uint16>(orc_XMLParser.GetAttributeUint32("dlc"));
   orc_NodeComMessage.u32_CycleTimeMs = orc_XMLParser.GetAttributeUint32("cycle-time");
   orc_NodeComMessage.u16_DelayTimeMs =
      static_cast<uint16>(orc_XMLParser.GetAttributeUint32("minimum-cycle-time"));

   if (orc_XMLParser.AttributeExists("receive-timeout") == true)
   {
      orc_NodeComMessage.u32_TimeoutMs = orc_XMLParser.GetAttributeUint32("receive-timeout");
   }
   else
   {
      //Default
      orc_NodeComMessage.u32_TimeoutMs = (3UL * orc_NodeComMessage.u32_CycleTimeMs) + 10UL;
   }

   if (orc_XMLParser.SelectNodeChild("name") == "name")
   {
      orc_NodeComMessage.c_Name = orc_XMLParser.GetNodeContent();
      //Return
      tgl_assert(orc_XMLParser.SelectNodeParent() == "com-message");
   }
   else
   {
      osc_write_log_error("Loading node definition", "Could not find \"com-message\".\"name\" node.");
      s32_Retval = C_CONFIG;
   }
   if (orc_XMLParser.SelectNodeChild("comment") == "comment")
   {
      orc_NodeComMessage.c_Comment = orc_XMLParser.GetNodeContent();
      //Return
      tgl_assert(orc_XMLParser.SelectNodeParent() == "com-message");
   }
   if ((orc_XMLParser.SelectNodeChild("tx-method") == "tx-method") &&
       (s32_Retval == C_NO_ERR))
   {
      mh_StringToNodeComMessageTxMethod(orc_XMLParser.GetNodeContent(), orc_NodeComMessage.e_TxMethod);
      //Return
      tgl_assert(orc_XMLParser.SelectNodeParent() == "com-message");
   }
   else
   {
      osc_write_log_error("Loading node definition", "Could not find \"com-message\".\"tx-method\" node.");
      s32_Retval = C_CONFIG;
   }
   if ((orc_XMLParser.SelectNodeChild("com-signals") == "com-signals") &&
       (s32_Retval == C_NO_ERR))
   {
      s32_Retval = h_LoadNodeComSignals(orc_NodeComMessage.c_Signals, orc_XMLParser);
      if (s32_Retval == C_NO_ERR)
      {
         //Return
         tgl_assert(orc_XMLParser.SelectNodeParent() == "com-message");
      }
   }
   else
   {
      osc_write_log_error("Loading node definition", "Could not find \"com-message\".\"com-signals\" node.");
      s32_Retval = C_CONFIG;
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Save node com message

   Save node to XML file
   pre-condition: the passed XML parser has the active node set to "com-message"
   post-condition: the passed XML parser has the active node set to the same "com-message"

   \param[in]     orc_NodeComMessage data storage
   \param[in,out] orc_XMLParser      XML with list active
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OSCNodeFilerV2::h_SaveNodeComMessage(const C_OSCCanMessage & orc_NodeComMessage,
                                            C_OSCXMLParserBase & orc_XMLParser)
{
   orc_XMLParser.SetAttributeUint32("can-id", orc_NodeComMessage.u32_CanId);
   orc_XMLParser.SetAttributeBool("is-extended", orc_NodeComMessage.q_IsExtended);
   orc_XMLParser.SetAttributeUint32("dlc", orc_NodeComMessage.u16_Dlc);
   orc_XMLParser.SetAttributeUint32("cycle-time", orc_NodeComMessage.u32_CycleTimeMs);
   orc_XMLParser.SetAttributeUint32("minimum-cycle-time", orc_NodeComMessage.u16_DelayTimeMs);
   orc_XMLParser.SetAttributeUint32("receive-timeout", orc_NodeComMessage.u32_TimeoutMs);
   orc_XMLParser.CreateNodeChild("name", orc_NodeComMessage.c_Name);
   orc_XMLParser.CreateNodeChild("comment", orc_NodeComMessage.c_Comment);
   orc_XMLParser.CreateNodeChild("tx-method", mh_NodeComMessageTxMethodToString(orc_NodeComMessage.e_TxMethod));
   orc_XMLParser.CreateAndSelectNodeChild("com-signals");
   h_SaveNodeComSignals(orc_NodeComMessage.c_Signals, orc_XMLParser);
   //Return
   tgl_assert(orc_XMLParser.SelectNodeParent() == "com-message");
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Load node com signals

   Load node data from XML file
   pre-condition: the passed XML parser has the active node set to "com-signals"
   post-condition: the passed XML parser has the active node set to the same "com-signals"

   \param[out]    orc_NodeComSignals data storage
   \param[in,out] orc_XMLParser      XML with list active

   \return
   C_NO_ERR   data read
   C_CONFIG   content of file is invalid or incomplete
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_OSCNodeFilerV2::h_LoadNodeComSignals(std::vector<C_OSCCanSignal> & orc_NodeComSignals,
                                              C_OSCXMLParserBase & orc_XMLParser)
{
   sint32 s32_Retval = C_NO_ERR;

   C_SCLString c_CurNodeComSignal;
   uint32 u32_ExpectedSize = 0UL;
   const bool q_ExpectedSizeHere = orc_XMLParser.AttributeExists("length");

   //Check optional length
   if (q_ExpectedSizeHere == true)
   {
      u32_ExpectedSize = orc_XMLParser.GetAttributeUint32("length");
      orc_NodeComSignals.reserve(u32_ExpectedSize);
   }

   c_CurNodeComSignal = orc_XMLParser.SelectNodeChild("com-signal");

   //Clear
   orc_NodeComSignals.clear();
   if (c_CurNodeComSignal == "com-signal")
   {
      do
      {
         C_OSCCanSignal c_CurComSignal;

         if (s32_Retval == C_NO_ERR)
         {
            s32_Retval = h_LoadNodeComSignal(c_CurComSignal, orc_XMLParser);
         }

         //Append
         orc_NodeComSignals.push_back(c_CurComSignal);

         //Next
         c_CurNodeComSignal = orc_XMLParser.SelectNodeNext("com-signal");
      }
      while (c_CurNodeComSignal == "com-signal");
      //Return
      tgl_assert(orc_XMLParser.SelectNodeParent() == "com-signals");
   }
   //Compare length
   if ((s32_Retval == C_NO_ERR) && (q_ExpectedSizeHere == true))
   {
      if (u32_ExpectedSize != orc_NodeComSignals.size())
      {
         C_SCLString c_Tmp;
         c_Tmp.PrintFormatted("Unexpected comm signal count, expected: %i, got %i", u32_ExpectedSize,
                              orc_NodeComSignals.size());
         osc_write_log_warning("Load file", c_Tmp.c_str());
      }
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Save node com signals

   Save node to XML file
   pre-condition: the passed XML parser has the active node set to "com-signals"
   post-condition: the passed XML parser has the active node set to the same "com-signals"

   \param[in]     orc_NodeComSignals data storage
   \param[in,out] orc_XMLParser      XML with list active
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OSCNodeFilerV2::h_SaveNodeComSignals(const std::vector<C_OSCCanSignal> & orc_NodeComSignals,
                                            C_OSCXMLParserBase & orc_XMLParser)
{
   orc_XMLParser.SetAttributeUint32("length", orc_NodeComSignals.size());
   for (uint32 u32_ItComSignal = 0; u32_ItComSignal < orc_NodeComSignals.size(); ++u32_ItComSignal)
   {
      orc_XMLParser.CreateAndSelectNodeChild("com-signal");
      h_SaveNodeComSignal(orc_NodeComSignals[u32_ItComSignal], orc_XMLParser);
      //Return
      tgl_assert(orc_XMLParser.SelectNodeParent() == "com-signals");
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Load node com signal

   Load node data from XML file
   pre-condition: the passed XML parser has the active node set to "com-signal"
   post-condition: the passed XML parser has the active node set to the same "com-signal"

   \param[out]    orc_NodeComSignal data storage
   \param[in,out] orc_XMLParser     XML with list active

   \return
   C_NO_ERR   data read
   C_CONFIG   content of file is invalid or incomplete
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_OSCNodeFilerV2::h_LoadNodeComSignal(C_OSCCanSignal & orc_NodeComSignal, C_OSCXMLParserBase & orc_XMLParser)
{
   sint32 s32_Retval = C_NO_ERR;

   orc_NodeComSignal.u32_ComDataElementIndex = orc_XMLParser.GetAttributeUint32("data-element-index");
   orc_NodeComSignal.u16_ComBitStart = static_cast<uint16>(orc_XMLParser.GetAttributeUint32("bit-start"));
   orc_NodeComSignal.u16_ComBitLength = static_cast<uint16>(orc_XMLParser.GetAttributeUint32("bit-length"));

   if (orc_XMLParser.SelectNodeChild("byte-order") == "byte-order")
   {
      s32_Retval = mh_StringToCommunicationByteOrder(orc_XMLParser.GetNodeContent(), orc_NodeComSignal.e_ComByteOrder);
      //Return
      tgl_assert(orc_XMLParser.SelectNodeParent() == "com-signal");
   }
   else
   {
      osc_write_log_error("Loading node definition",
                          "Could not find \"com-message\".\"com-signals\".\nbyte-order\" node.");
      s32_Retval = C_CONFIG;
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Save node com signal

   Save node to XML file
   pre-condition: the passed XML parser has the active node set to "com-signal"
   post-condition: the passed XML parser has the active node set to the same "com-signal"

   \param[in]     orc_NodeComSignal data storage
   \param[in,out] orc_XMLParser     XML with list active
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OSCNodeFilerV2::h_SaveNodeComSignal(const C_OSCCanSignal & orc_NodeComSignal, C_OSCXMLParserBase & orc_XMLParser)
{
   orc_XMLParser.SetAttributeUint32("data-element-index", orc_NodeComSignal.u32_ComDataElementIndex);
   orc_XMLParser.SetAttributeUint32("bit-start", orc_NodeComSignal.u16_ComBitStart);
   orc_XMLParser.SetAttributeUint32("bit-length", orc_NodeComSignal.u16_ComBitLength);
   orc_XMLParser.CreateNodeChild("byte-order", mh_CommunicationByteOrderToString(orc_NodeComSignal.e_ComByteOrder));
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Transform communication protocol type to string

   \param[in] ore_CommunicationProtocol Communication protocol type

   \return
   Stringified communication protocol type
*/
//----------------------------------------------------------------------------------------------------------------------
C_SCLString C_OSCNodeFilerV2::h_CommunicationProtocolToString(const C_OSCCanProtocol::E_Type & ore_CommunicationProtocol)
{
   C_SCLString c_Retval;

   switch (ore_CommunicationProtocol)
   {
   case C_OSCCanProtocol::eLAYER2:
      c_Retval = "layer 2";
      break;
   case C_OSCCanProtocol::eECES:
      c_Retval = "eces";
      break;
   case C_OSCCanProtocol::eCAN_OPEN_SAFETY:
      c_Retval = "can open safety";
      break;
   default:
      c_Retval = "invalid";
      break;
   }
   return c_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Transform string to communication protocol type

   \param[in]  orc_String String to interpret
   \param[out] ore_Type   Communication protocol type

   \return
   C_NO_ERR   no error
   C_RANGE    String unknown
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_OSCNodeFilerV2::h_StringToCommunicationProtocol(const C_SCLString & orc_String,
                                                         C_OSCCanProtocol::E_Type & ore_Type)
{
   sint32 s32_Retval = C_NO_ERR;

   if (orc_String == "can open safety")
   {
      ore_Type = C_OSCCanProtocol::eCAN_OPEN_SAFETY;
   }
   else if (orc_String == "eces")
   {
      ore_Type = C_OSCCanProtocol::eECES;
   }
   else if (orc_String == "layer 2")
   {
      ore_Type = C_OSCCanProtocol::eLAYER2;
   }
   else
   {
      osc_write_log_error("Loading node definition", "Invalid value for \"communication-protocol\":" + orc_String);
      s32_Retval = C_RANGE;
   }

   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Load node properties

   Load node data from XML file
   pre-condition: the passed XML parser has the active node set to "core"
   post-condition: the passed XML parser has the active node set to the same "core"

   \param[out]    orc_NodeProperties data storage
   \param[in,out] orc_XMLParser      XML with core active

   \return
   C_NO_ERR   data read
   C_CONFIG   content of file is invalid or incomplete
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_OSCNodeFilerV2::mh_LoadProperties(C_OSCNodeProperties & orc_NodeProperties, C_OSCXMLParserBase & orc_XMLParser)
{
   sint32 s32_Retval = C_NO_ERR;

   if (orc_XMLParser.SelectNodeChild("properties") == "properties")
   {
      //Name
      if (orc_XMLParser.SelectNodeChild("name") == "name")
      {
         orc_NodeProperties.c_Name = orc_XMLParser.GetNodeContent();
         //Return
         tgl_assert(orc_XMLParser.SelectNodeParent() == "properties");
      }
      else
      {
         osc_write_log_error("Loading node definition", "Could not find \"properties\".\"name\" node.");
         s32_Retval = C_CONFIG;
      }
      //Comment
      if (orc_XMLParser.SelectNodeChild("comment") == "comment")
      {
         orc_NodeProperties.c_Comment = orc_XMLParser.GetNodeContent();
         //Return
         tgl_assert(orc_XMLParser.SelectNodeParent() == "properties");
      }
      //Diagnostic server
      if ((orc_XMLParser.SelectNodeChild("diagnostic-server") == "diagnostic-server") && (s32_Retval == C_NO_ERR))
      {
         s32_Retval =
            mh_StringToDiagnosticServer(orc_XMLParser.GetNodeContent(), orc_NodeProperties.e_DiagnosticServer);
         //Return
         tgl_assert(orc_XMLParser.SelectNodeParent() == "properties");
      }
      else
      {
         osc_write_log_error("Loading node definition", "Could not find \"properties\".\"diagnostic-server\" node.");
         s32_Retval = C_CONFIG;
      }
      //Flash loader
      if ((orc_XMLParser.SelectNodeChild("flash-loader") == "flash-loader") && (s32_Retval == C_NO_ERR))
      {
         s32_Retval = mh_StringToFlashLoader(orc_XMLParser.GetNodeContent(), orc_NodeProperties.e_FlashLoader);
         //Return
         tgl_assert(orc_XMLParser.SelectNodeParent() == "properties");
      }
      else
      {
         osc_write_log_error("Loading node definition", "Could not find \"properties\".\"flash-loader\" node.");
         s32_Retval = C_CONFIG;
      }
      //Communication interfaces
      if ((orc_XMLParser.SelectNodeChild("communication-interfaces") == "communication-interfaces") &&
          (s32_Retval == C_NO_ERR))
      {
         C_SCLString c_CurNode = orc_XMLParser.SelectNodeChild("communication-interface");
         if (c_CurNode == "communication-interface")
         {
            do
            {
               C_OSCNodeComInterfaceSettings c_ComInterface;

               c_ComInterface.u8_InterfaceNumber =
                  static_cast<uint8>(orc_XMLParser.GetAttributeUint32("interface-number"));
               c_ComInterface.u8_NodeID = static_cast<uint8>(orc_XMLParser.GetAttributeUint32("node-id"));
               c_ComInterface.q_IsUpdateEnabled = orc_XMLParser.GetAttributeBool("update-available");
               c_ComInterface.q_IsRoutingEnabled = orc_XMLParser.GetAttributeBool("routing-available");
               c_ComInterface.q_IsDiagnosisEnabled = orc_XMLParser.GetAttributeBool("diagnosis-available");
               //Type
               if ((orc_XMLParser.SelectNodeChild("type") == "type") && (s32_Retval == C_NO_ERR))
               {
                  s32_Retval = C_OSCSystemFilerUtil::h_BusTypeStringToEnum(
                     orc_XMLParser.GetNodeContent(), c_ComInterface.e_InterfaceType);
                  //Return
                  tgl_assert(orc_XMLParser.SelectNodeParent() == "communication-interface");
               }
               else
               {
                  osc_write_log_error("Loading node definition",
                                      "Could not find \"communication-interface\".\"type\" node.");
                  s32_Retval = C_CONFIG;
               }

               //IP address
               if ((orc_XMLParser.SelectNodeChild("ip-address") == "ip-address") && (s32_Retval == C_NO_ERR))
               {
                  c_ComInterface.c_Ip.au8_IpAddress[0] = static_cast<uint8>(orc_XMLParser.GetAttributeUint32("byte1"));
                  c_ComInterface.c_Ip.au8_IpAddress[1] = static_cast<uint8>(orc_XMLParser.GetAttributeUint32("byte2"));
                  c_ComInterface.c_Ip.au8_IpAddress[2] = static_cast<uint8>(orc_XMLParser.GetAttributeUint32("byte3"));
                  c_ComInterface.c_Ip.au8_IpAddress[3] = static_cast<uint8>(orc_XMLParser.GetAttributeUint32("byte4"));
                  //Return
                  tgl_assert(orc_XMLParser.SelectNodeParent() == "communication-interface");
               }
               else
               {
                  c_ComInterface.c_Ip.au8_IpAddress[0] = 0U;
                  c_ComInterface.c_Ip.au8_IpAddress[1] = 0U;
                  c_ComInterface.c_Ip.au8_IpAddress[2] = 0U;
                  c_ComInterface.c_Ip.au8_IpAddress[3] = 0U;
               }

               //net mask
               if ((orc_XMLParser.SelectNodeChild("net-mask") == "net-mask") && (s32_Retval == C_NO_ERR))
               {
                  c_ComInterface.c_Ip.au8_NetMask[0] = static_cast<uint8>(orc_XMLParser.GetAttributeUint32("byte1"));
                  c_ComInterface.c_Ip.au8_NetMask[1] = static_cast<uint8>(orc_XMLParser.GetAttributeUint32("byte2"));
                  c_ComInterface.c_Ip.au8_NetMask[2] = static_cast<uint8>(orc_XMLParser.GetAttributeUint32("byte3"));
                  c_ComInterface.c_Ip.au8_NetMask[3] = static_cast<uint8>(orc_XMLParser.GetAttributeUint32("byte4"));
                  //Return
                  tgl_assert(orc_XMLParser.SelectNodeParent() == "communication-interface");
               }
               else
               {
                  c_ComInterface.c_Ip.au8_NetMask[0] = 0U;
                  c_ComInterface.c_Ip.au8_NetMask[1] = 0U;
                  c_ComInterface.c_Ip.au8_NetMask[2] = 0U;
                  c_ComInterface.c_Ip.au8_NetMask[3] = 0U;
               }

               //default gateway
               if ((orc_XMLParser.SelectNodeChild("default-gateway") == "default-gateway") && (s32_Retval == C_NO_ERR))
               {
                  c_ComInterface.c_Ip.au8_DefaultGateway[0] =
                     static_cast<uint8>(orc_XMLParser.GetAttributeUint32("byte1"));
                  c_ComInterface.c_Ip.au8_DefaultGateway[1] =
                     static_cast<uint8>(orc_XMLParser.GetAttributeUint32("byte2"));
                  c_ComInterface.c_Ip.au8_DefaultGateway[2] =
                     static_cast<uint8>(orc_XMLParser.GetAttributeUint32("byte3"));
                  c_ComInterface.c_Ip.au8_DefaultGateway[3] =
                     static_cast<uint8>(orc_XMLParser.GetAttributeUint32("byte4"));
                  //Return
                  tgl_assert(orc_XMLParser.SelectNodeParent() == "communication-interface");
               }
               else
               {
                  c_ComInterface.c_Ip.au8_DefaultGateway[0] = 0U;
                  c_ComInterface.c_Ip.au8_DefaultGateway[1] = 0U;
                  c_ComInterface.c_Ip.au8_DefaultGateway[2] = 0U;
                  c_ComInterface.c_Ip.au8_DefaultGateway[3] = 0U;
               }
               //Bus
               if (orc_XMLParser.SelectNodeChild("bus") == "bus")
               {
                  c_ComInterface.q_IsBusConnected = orc_XMLParser.GetAttributeBool("connected");
                  c_ComInterface.u32_BusIndex = orc_XMLParser.GetAttributeUint32("bus-index");
                  //Return
                  tgl_assert(orc_XMLParser.SelectNodeParent() == "communication-interface");
               }
               else
               {
                  osc_write_log_error("Loading node definition",
                                      "Could not find \"communication-interface\".\"bus\" node.");
                  s32_Retval = C_CONFIG;
               }
               orc_NodeProperties.c_ComInterfaces.push_back(c_ComInterface);
               c_CurNode = orc_XMLParser.SelectNodeNext("communication-interface");
            }
            while (c_CurNode == "communication-interface");
            //Return
            tgl_assert(orc_XMLParser.SelectNodeParent() == "communication-interfaces");
         }
         //Return
         tgl_assert(orc_XMLParser.SelectNodeParent() == "properties");
      }
      //openSYDE server settings
      if (orc_XMLParser.SelectNodeChild("open-syde-server-settings") == "open-syde-server-settings")
      {
         if (orc_XMLParser.AttributeExists("max-clients") == true)
         {
            orc_NodeProperties.c_OpenSYDEServerSettings.u8_MaxClients =
               static_cast<uint8>(orc_XMLParser.GetAttributeUint32("max-clients"));
         }
         else
         {
            orc_NodeProperties.c_OpenSYDEServerSettings.u8_MaxClients = 1;
         }
         if (orc_XMLParser.AttributeExists("max-parallel-transmissions") == true)
         {
            orc_NodeProperties.c_OpenSYDEServerSettings.u8_MaxParallelTransmissions =
               static_cast<uint8>(orc_XMLParser.GetAttributeUint32("max-parallel-transmissions"));
         }
         else
         {
            orc_NodeProperties.c_OpenSYDEServerSettings.u8_MaxParallelTransmissions = 64;
         }
         if (orc_XMLParser.AttributeExists("application-index") == true)
         {
            orc_NodeProperties.c_OpenSYDEServerSettings.s16_DPDDataBlockIndex =
               static_cast<sint16>(orc_XMLParser.GetAttributeSint32("application-index"));
         }
         else
         {
            orc_NodeProperties.c_OpenSYDEServerSettings.s16_DPDDataBlockIndex = -1;
         }
         if (orc_XMLParser.AttributeExists("max-tx-message-buffer") == true)
         {
            orc_NodeProperties.c_OpenSYDEServerSettings.u16_MaxMessageBufferTx =
               static_cast<uint16>(orc_XMLParser.GetAttributeUint32("max-tx-message-buffer"));
         }
         else
         {
            orc_NodeProperties.c_OpenSYDEServerSettings.u16_MaxMessageBufferTx = 585U;
         }
         if (orc_XMLParser.AttributeExists("max-rx-routing-message-buffer") == true)
         {
            orc_NodeProperties.c_OpenSYDEServerSettings.u16_MaxRoutingMessageBufferRx =
               static_cast<uint16>(orc_XMLParser.GetAttributeUint32("max-rx-routing-message-buffer"));
         }
         else
         {
            orc_NodeProperties.c_OpenSYDEServerSettings.u16_MaxRoutingMessageBufferRx = 585U;
         }

         //Return
         tgl_assert(orc_XMLParser.SelectNodeParent() == "properties");
      }
      else
      {
         orc_NodeProperties.c_OpenSYDEServerSettings.Initialize();
      }

      if (s32_Retval == C_NO_ERR)
      {
         s32_Retval = mh_LoadStwFlashloaderOptions(orc_NodeProperties.c_STWFlashloaderSettings, orc_XMLParser);
      }
      //Return
      tgl_assert(orc_XMLParser.SelectNodeParent() == "core");
   }
   else
   {
      osc_write_log_error("Loading node definition", "Could not find \"properties\" node.");
      s32_Retval = C_CONFIG;
   }

   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Save node properties

   Save node to XML file
   pre-condition: the passed XML parser has the active node set to "core"
   post-condition: the passed XML parser has the active node set to the same "core"

   \param[in]     orc_NodeProperties data storage
   \param[in,out] orc_XMLParser      XML with core active
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OSCNodeFilerV2::mh_SaveProperties(const C_OSCNodeProperties & orc_NodeProperties,
                                         C_OSCXMLParserBase & orc_XMLParser)
{
   orc_XMLParser.CreateAndSelectNodeChild("properties");
   orc_XMLParser.CreateNodeChild("name", orc_NodeProperties.c_Name);
   orc_XMLParser.CreateNodeChild("comment", orc_NodeProperties.c_Comment);
   orc_XMLParser.CreateNodeChild("diagnostic-server",
                                 mh_DiagnosticServerToString(orc_NodeProperties.e_DiagnosticServer));
   orc_XMLParser.CreateNodeChild("flash-loader", mh_FlashLoaderToString(orc_NodeProperties.e_FlashLoader));
   //Com interfaces
   orc_XMLParser.CreateAndSelectNodeChild("communication-interfaces");
   for (uint32 u32_ItComInterface = 0; u32_ItComInterface < orc_NodeProperties.c_ComInterfaces.size();
        ++u32_ItComInterface)
   {
      const C_OSCNodeComInterfaceSettings & rc_CurComInterface = orc_NodeProperties.c_ComInterfaces[u32_ItComInterface];

      orc_XMLParser.CreateAndSelectNodeChild("communication-interface");
      orc_XMLParser.SetAttributeUint32("interface-number", static_cast<uint32>(rc_CurComInterface.u8_InterfaceNumber));
      orc_XMLParser.SetAttributeUint32("node-id", static_cast<uint32>(rc_CurComInterface.u8_NodeID));
      orc_XMLParser.SetAttributeBool("update-available", rc_CurComInterface.q_IsUpdateEnabled);
      orc_XMLParser.SetAttributeBool("routing-available", rc_CurComInterface.q_IsRoutingEnabled);
      orc_XMLParser.SetAttributeBool("diagnosis-available", rc_CurComInterface.q_IsDiagnosisEnabled);
      orc_XMLParser.CreateNodeChild("type",
                                    C_OSCSystemFilerUtil::h_BusTypeEnumToString(rc_CurComInterface.e_InterfaceType));
      if (rc_CurComInterface.e_InterfaceType == C_OSCSystemBus::eETHERNET)
      {
         orc_XMLParser.CreateAndSelectNodeChild("ip-address");
         orc_XMLParser.SetAttributeUint32("byte1", rc_CurComInterface.c_Ip.au8_IpAddress[0]);
         orc_XMLParser.SetAttributeUint32("byte2", rc_CurComInterface.c_Ip.au8_IpAddress[1]);
         orc_XMLParser.SetAttributeUint32("byte3", rc_CurComInterface.c_Ip.au8_IpAddress[2]);
         orc_XMLParser.SetAttributeUint32("byte4", rc_CurComInterface.c_Ip.au8_IpAddress[3]);
         //Return
         tgl_assert(orc_XMLParser.SelectNodeParent() == "communication-interface");
         orc_XMLParser.CreateAndSelectNodeChild("net-mask");
         orc_XMLParser.SetAttributeUint32("byte1", rc_CurComInterface.c_Ip.au8_NetMask[0]);
         orc_XMLParser.SetAttributeUint32("byte2", rc_CurComInterface.c_Ip.au8_NetMask[1]);
         orc_XMLParser.SetAttributeUint32("byte3", rc_CurComInterface.c_Ip.au8_NetMask[2]);
         orc_XMLParser.SetAttributeUint32("byte4", rc_CurComInterface.c_Ip.au8_NetMask[3]);
         //Return
         tgl_assert(orc_XMLParser.SelectNodeParent() == "communication-interface");
         orc_XMLParser.CreateAndSelectNodeChild("default-gateway");
         orc_XMLParser.SetAttributeUint32("byte1", rc_CurComInterface.c_Ip.au8_DefaultGateway[0]);
         orc_XMLParser.SetAttributeUint32("byte2", rc_CurComInterface.c_Ip.au8_DefaultGateway[1]);
         orc_XMLParser.SetAttributeUint32("byte3", rc_CurComInterface.c_Ip.au8_DefaultGateway[2]);
         orc_XMLParser.SetAttributeUint32("byte4", rc_CurComInterface.c_Ip.au8_DefaultGateway[3]);
         //Return
         tgl_assert(orc_XMLParser.SelectNodeParent() == "communication-interface");
      }
      //Create this section for compatibility reasons (had content and was required by previous openSYDE versions)
      orc_XMLParser.CreateAndSelectNodeChild("communication-protocol");
      //Return
      tgl_assert(orc_XMLParser.SelectNodeParent() == "communication-interface");
      orc_XMLParser.CreateAndSelectNodeChild("bus");
      orc_XMLParser.SetAttributeBool("connected", rc_CurComInterface.q_IsBusConnected);
      orc_XMLParser.SetAttributeUint32("bus-index", rc_CurComInterface.u32_BusIndex);
      //Return
      tgl_assert(orc_XMLParser.SelectNodeParent() == "communication-interface");
      //Return
      tgl_assert(orc_XMLParser.SelectNodeParent() == "communication-interfaces");
   }
   //Return
   tgl_assert(orc_XMLParser.SelectNodeParent() == "properties");

   //openSYDE server settings
   orc_XMLParser.CreateAndSelectNodeChild("open-syde-server-settings");
   orc_XMLParser.SetAttributeUint32("max-clients", orc_NodeProperties.c_OpenSYDEServerSettings.u8_MaxClients);
   orc_XMLParser.SetAttributeUint32("max-parallel-transmissions",
                                    orc_NodeProperties.c_OpenSYDEServerSettings.u8_MaxParallelTransmissions);
   orc_XMLParser.SetAttributeSint32("application-index",
                                    orc_NodeProperties.c_OpenSYDEServerSettings.s16_DPDDataBlockIndex);
   orc_XMLParser.SetAttributeUint32("max-tx-message-buffer",
                                    orc_NodeProperties.c_OpenSYDEServerSettings.u16_MaxMessageBufferTx);
   orc_XMLParser.SetAttributeUint32("max-rx-routing-message-buffer",
                                    orc_NodeProperties.c_OpenSYDEServerSettings.u16_MaxRoutingMessageBufferRx);
   //Return
   tgl_assert(orc_XMLParser.SelectNodeParent() == "properties");

   mh_SaveStwFlashloaderOptions(orc_NodeProperties.c_STWFlashloaderSettings, orc_XMLParser);

   //Return
   tgl_assert(orc_XMLParser.SelectNodeParent() == "core");
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Load node STW flashloader settings

   Load node data from XML file
   pre-condition: the passed XML parser has the active node set to "properties"
   post-condition: the passed XML parser has the active node set to the same "properties"

   \param[out]    orc_StwFlashloaderSettings data storage
   \param[in,out] orc_XMLParser              XML with core active

   \return
   C_NO_ERR   data read
   C_CONFIG   content of file is invalid or incomplete
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_OSCNodeFilerV2::mh_LoadStwFlashloaderOptions(C_OSCNodeStwFlashloaderSettings & orc_StwFlashloaderSettings,
                                                      C_OSCXMLParserBase & orc_XMLParser)
{
   sint32 s32_Retval = C_NO_ERR;

   if (orc_XMLParser.SelectNodeChild("stw-flashloader-settings") == "stw-flashloader-settings")
   {
      if (orc_XMLParser.SelectNodeChild("reset-message") == "reset-message")
      {
         if (orc_XMLParser.AttributeExists("active") == true)
         {
            orc_StwFlashloaderSettings.q_ResetMessageActive = orc_XMLParser.GetAttributeBool("active");
         }
         else
         {
            osc_write_log_error("Loading node definition",
                                "Could not find \"active\" attribute in \"reset-message\" node.");
            s32_Retval = C_CONFIG;
         }
         if (orc_XMLParser.AttributeExists("extended") == true)
         {
            orc_StwFlashloaderSettings.q_ResetMessageExtendedId = orc_XMLParser.GetAttributeBool("extended");
         }
         else
         {
            orc_StwFlashloaderSettings.q_ResetMessageExtendedId = false;
         }
         if (orc_XMLParser.AttributeExists("id") == true)
         {
            orc_StwFlashloaderSettings.u32_ResetMessageId = orc_XMLParser.GetAttributeUint32("id");
         }
         else
         {
            orc_StwFlashloaderSettings.u32_ResetMessageId = 0;
         }
         if (orc_XMLParser.AttributeExists("dlc") == true)
         {
            orc_StwFlashloaderSettings.u8_ResetMessageDlc = static_cast<uint8>(orc_XMLParser.GetAttributeUint32("dlc"));
         }
         else
         {
            orc_StwFlashloaderSettings.u8_ResetMessageDlc = 8;
         }
         orc_StwFlashloaderSettings.c_Data.clear();
         if (orc_XMLParser.SelectNodeChild("data-bytes") == "data-bytes")
         {
            C_SCLString c_CurNode = orc_XMLParser.SelectNodeChild("data-byte");
            if (c_CurNode == "data-byte")
            {
               do
               {
                  if (orc_XMLParser.AttributeExists("value") == true)
                  {
                     const uint8 u8_DataByte = static_cast<uint8>(orc_XMLParser.GetAttributeUint32("value"));
                     orc_StwFlashloaderSettings.c_Data.push_back(u8_DataByte);
                  }
                  else
                  {
                     osc_write_log_error("Loading node definition",
                                         "Could not find \"value\" attribute in \"data-byte\" node.");
                     s32_Retval = C_CONFIG;
                  }
                  c_CurNode = orc_XMLParser.SelectNodeNext("data-byte");
               }
               while ((c_CurNode == "data-byte") && (s32_Retval == C_NO_ERR));
               //Return
               tgl_assert(orc_XMLParser.SelectNodeParent() == "data-bytes");
            }
            //Return
            tgl_assert(orc_XMLParser.SelectNodeParent() == "reset-message");
         }
         //Return
         tgl_assert(orc_XMLParser.SelectNodeParent() == "stw-flashloader-settings");
      }
      else
      {
         osc_write_log_error("Loading node definition", "Could not find \"reset-message\" node.");
         s32_Retval = C_CONFIG;
      }
      //Return
      tgl_assert(orc_XMLParser.SelectNodeParent() == "properties");
   }
   else
   {
      osc_write_log_error("Loading node definition", "Could not find \"stw-flashloader-settings\" node.");
      s32_Retval = C_CONFIG;
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Save node STW flashloader settings

   Save node to XML file
   pre-condition: the passed XML parser has the active node set to "properties"
   post-condition: the passed XML parser has the active node set to the same "properties"

   \param[in]     orc_StwFlashloaderSettings data storage
   \param[in,out] orc_XMLParser              XML with core active
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OSCNodeFilerV2::mh_SaveStwFlashloaderOptions(const C_OSCNodeStwFlashloaderSettings & orc_StwFlashloaderSettings,
                                                    C_OSCXMLParserBase & orc_XMLParser)
{
   orc_XMLParser.CreateAndSelectNodeChild("stw-flashloader-settings");
   orc_XMLParser.CreateAndSelectNodeChild("reset-message");
   orc_XMLParser.SetAttributeBool("active", orc_StwFlashloaderSettings.q_ResetMessageActive);
   orc_XMLParser.SetAttributeBool("extended", orc_StwFlashloaderSettings.q_ResetMessageExtendedId);
   orc_XMLParser.SetAttributeUint32("id", orc_StwFlashloaderSettings.u32_ResetMessageId);
   orc_XMLParser.SetAttributeUint32("dlc", static_cast<uint32>(orc_StwFlashloaderSettings.u8_ResetMessageDlc));
   orc_XMLParser.CreateAndSelectNodeChild("data-bytes");
   for (uint32 u32_ItDataByte = 0; u32_ItDataByte < orc_StwFlashloaderSettings.c_Data.size(); ++u32_ItDataByte)
   {
      orc_XMLParser.CreateAndSelectNodeChild("data-byte");
      orc_XMLParser.SetAttributeUint32("value", static_cast<uint32>(orc_StwFlashloaderSettings.c_Data[u32_ItDataByte]));
      //Return
      tgl_assert(orc_XMLParser.SelectNodeParent() == "data-bytes");
   }
   //Return
   tgl_assert(orc_XMLParser.SelectNodeParent() == "reset-message");
   //Return
   tgl_assert(orc_XMLParser.SelectNodeParent() == "stw-flashloader-settings");
   //Return
   tgl_assert(orc_XMLParser.SelectNodeParent() == "properties");
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Load node applications

   Load node data from XML file
   pre-condition: the passed XML parser has the active node set to "core"
   post-condition: the passed XML parser has the active node set to the same "core"

   \param[out]    orc_NodeApplications data storage
   \param[in,out] orc_XMLParser        XML with core active

   \return
   C_NO_ERR   data read
   C_CONFIG   content of file is invalid or incomplete
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_OSCNodeFilerV2::mh_LoadApplications(std::vector<C_OSCNodeApplication> & orc_NodeApplications,
                                             C_OSCXMLParserBase & orc_XMLParser)
{
   sint32 s32_Retval = C_NO_ERR;

   if (orc_XMLParser.SelectNodeChild("applications") == "applications")
   {
      C_SCLString c_CurNode;
      uint32 u32_ExpectedSize = 0UL;
      const bool q_ExpectedSizeHere = orc_XMLParser.AttributeExists("length");

      //Check optional length
      if (q_ExpectedSizeHere == true)
      {
         u32_ExpectedSize = orc_XMLParser.GetAttributeUint32("length");
         orc_NodeApplications.reserve(u32_ExpectedSize);
      }

      c_CurNode = orc_XMLParser.SelectNodeChild("application");
      if (c_CurNode == "application")
      {
         do
         {
            C_OSCNodeApplication c_CurApplication;

            if (orc_XMLParser.AttributeExists("active") == true)
            {
               c_CurApplication.q_Active = orc_XMLParser.GetAttributeBool("active");
            }
            else
            {
               s32_Retval = C_CONFIG;
            }

            if (orc_XMLParser.AttributeExists("process-id") == true)
            {
               c_CurApplication.u8_ProcessId = static_cast<uint8>(orc_XMLParser.GetAttributeUint32("process-id"));
            }
            else
            {
               c_CurApplication.u8_ProcessId = 0U;
            }

            // Set generated code version to 1 (new feature not supported by V2; but default would be 0)
            c_CurApplication.u16_GenCodeVersion = 1U;

            //Type
            if ((s32_Retval == C_NO_ERR) && (orc_XMLParser.SelectNodeChild("type") == "type"))
            {
               C_OSCNodeApplication::h_StringToApplication(orc_XMLParser.GetNodeContent(), c_CurApplication.e_Type);
               //Return
               tgl_assert(orc_XMLParser.SelectNodeParent() == "application");
            }
            else
            {
               s32_Retval = C_CONFIG;
            }
            //Name
            if ((s32_Retval == C_NO_ERR) && (orc_XMLParser.SelectNodeChild("name") == "name"))
            {
               c_CurApplication.c_Name = orc_XMLParser.GetNodeContent();
               //Return
               tgl_assert(orc_XMLParser.SelectNodeParent() == "application");
            }
            else
            {
               s32_Retval = C_CONFIG;
            }

            //Comment
            if ((s32_Retval == C_NO_ERR) && (orc_XMLParser.SelectNodeChild("comment") == "comment"))
            {
               c_CurApplication.c_Comment = orc_XMLParser.GetNodeContent();
               //Return
               tgl_assert(orc_XMLParser.SelectNodeParent() == "application");
            }
            else
            {
               s32_Retval = C_CONFIG;
            }

            if ((s32_Retval == C_NO_ERR) && (orc_XMLParser.SelectNodeChild("project-path") == "project-path"))
            {
               c_CurApplication.c_ProjectPath = orc_XMLParser.GetNodeContent();
               //Return
               tgl_assert(orc_XMLParser.SelectNodeParent() == "application");
            }
            else
            {
               // No error, if child does not exist.
               c_CurApplication.c_ProjectPath = "";
            }

            if ((s32_Retval == C_NO_ERR) && (orc_XMLParser.SelectNodeChild("ide-call") == "ide-call"))
            {
               c_CurApplication.c_IDECall = orc_XMLParser.GetNodeContent();
               //Return
               tgl_assert(orc_XMLParser.SelectNodeParent() == "application");
            }
            else
            {
               // No error, if child does not exist.
               c_CurApplication.c_IDECall = "";
            }

            if ((s32_Retval == C_NO_ERR) &&
                (orc_XMLParser.SelectNodeChild("code-generator-path") == "code-generator-path"))
            {
               c_CurApplication.c_CodeGeneratorPath = orc_XMLParser.GetNodeContent();
               //Return
               tgl_assert(orc_XMLParser.SelectNodeParent() == "application");
            }
            else
            {
               // No error, if child does not exist.
               c_CurApplication.c_CodeGeneratorPath = "";
            }

            if ((s32_Retval == C_NO_ERR) && (orc_XMLParser.SelectNodeChild("generate-path") == "generate-path"))
            {
               c_CurApplication.c_GeneratePath = orc_XMLParser.GetNodeContent();
               //Return
               tgl_assert(orc_XMLParser.SelectNodeParent() == "application");
            }
            else
            {
               // No error, if child does not exist.
               c_CurApplication.c_GeneratePath = "";
            }

            if ((s32_Retval == C_NO_ERR) && (orc_XMLParser.SelectNodeChild("result-path") == "result-path"))
            {
               c_CurApplication.c_ResultPaths.resize(1);
               c_CurApplication.c_ResultPaths[0] = orc_XMLParser.GetNodeContent();
               //Return
               tgl_assert(orc_XMLParser.SelectNodeParent() == "application");
            }
            else
            {
               // No error, if child does not exist.
               c_CurApplication.c_ResultPaths.resize(1);
               c_CurApplication.c_ResultPaths[0] = "";
            }

            orc_NodeApplications.push_back(c_CurApplication);
            c_CurNode = orc_XMLParser.SelectNodeNext("application");
         }
         while ((c_CurNode == "application") && (s32_Retval == C_NO_ERR));
         //Return
         tgl_assert(orc_XMLParser.SelectNodeParent() == "applications");
      }
      //Compare length
      if ((s32_Retval == C_NO_ERR) && (q_ExpectedSizeHere == true))
      {
         if (u32_ExpectedSize != orc_NodeApplications.size())
         {
            C_SCLString c_Tmp;
            c_Tmp.PrintFormatted("Unexpected application count, expected: %i, got %i", u32_ExpectedSize,
                                 orc_NodeApplications.size());
            osc_write_log_warning("Load file", c_Tmp.c_str());
         }
      }
      //Return
      tgl_assert(orc_XMLParser.SelectNodeParent() == "core");
   }
   else
   {
      s32_Retval = C_CONFIG;
   }

   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Save node applications

   Save node to XML file
   pre-condition: the passed XML parser has the active node set to "core"
   post-condition: the passed XML parser has the active node set to the same "core"

   \param[in]     orc_NodeApplications data storage
   \param[in,out] orc_XMLParser        XML with core active
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OSCNodeFilerV2::mh_SaveApplications(const std::vector<C_OSCNodeApplication> & orc_NodeApplications,
                                           C_OSCXMLParserBase & orc_XMLParser)
{
   orc_XMLParser.CreateAndSelectNodeChild("applications");
   orc_XMLParser.SetAttributeUint32("length", orc_NodeApplications.size());
   for (uint32 u32_ItApplication = 0; u32_ItApplication < orc_NodeApplications.size(); ++u32_ItApplication)
   {
      const C_OSCNodeApplication & rc_CurApplication = orc_NodeApplications[u32_ItApplication];
      orc_XMLParser.CreateAndSelectNodeChild("application");
      orc_XMLParser.SetAttributeBool("active", rc_CurApplication.q_Active);
      orc_XMLParser.SetAttributeUint32("process-id", static_cast<uint32>(rc_CurApplication.u8_ProcessId));
      orc_XMLParser.CreateNodeChild("type", C_OSCNodeApplication::h_ApplicationToString(rc_CurApplication.e_Type));
      orc_XMLParser.CreateNodeChild("name", rc_CurApplication.c_Name);
      orc_XMLParser.CreateNodeChild("comment", rc_CurApplication.c_Comment);
      orc_XMLParser.CreateNodeChild("project-path", rc_CurApplication.c_ProjectPath);
      orc_XMLParser.CreateNodeChild("ide-call", rc_CurApplication.c_IDECall);
      orc_XMLParser.CreateNodeChild("code-generator-path", rc_CurApplication.c_CodeGeneratorPath);
      orc_XMLParser.CreateNodeChild("generate-path", rc_CurApplication.c_GeneratePath);
      orc_XMLParser.CreateNodeChild("result-path", rc_CurApplication.c_ResultPaths[0]);
      //Return
      tgl_assert(orc_XMLParser.SelectNodeParent() == "applications");
   }
   //Return
   tgl_assert(orc_XMLParser.SelectNodeParent() == "core");
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Load node data pools

   Load node data from XML file
   pre-condition: the passed XML parser has the active node set to "core"
   post-condition: the passed XML parser has the active node set to the same "core"

   \param[in]     ou16_XmlFormatVersion  version of XML format
   \param[out]    orc_Node      data storage
   \param[in,out] orc_XMLParser XML with core active

   \return
   C_NO_ERR   data read
   C_CONFIG   content of file is invalid or incomplete
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_OSCNodeFilerV2::mh_LoadDataPools(const uint16 ou16_XmlFormatVersion, C_OSCNode & orc_Node,
                                          C_OSCXMLParserBase & orc_XMLParser)
{
   sint32 s32_Retval = C_NO_ERR;

   //Clear
   orc_Node.c_DataPools.clear();
   if (orc_XMLParser.SelectNodeChild("data-pools") == "data-pools")
   {
      C_SCLString c_CurNode;
      uint32 u32_ExpectedSize = 0UL;
      const bool q_ExpectedSizeHere = orc_XMLParser.AttributeExists("length");

      //Check optional length
      if (q_ExpectedSizeHere == true)
      {
         u32_ExpectedSize = orc_XMLParser.GetAttributeUint32("length");
         orc_Node.c_DataPools.reserve(u32_ExpectedSize);
      }

      c_CurNode = orc_XMLParser.SelectNodeChild("data-pool");
      if (c_CurNode == "data-pool")
      {
         do
         {
            C_OSCNodeDataPool c_CurDataPool;

            s32_Retval =
               C_OSCNodeDataPoolFilerV2::h_LoadDataPool(ou16_XmlFormatVersion, c_CurDataPool, orc_XMLParser);
            if (s32_Retval != C_NO_ERR)
            {
               break;
            }
            orc_Node.c_DataPools.push_back(c_CurDataPool);
            c_CurNode = orc_XMLParser.SelectNodeNext("data-pool");
         }
         while (c_CurNode == "data-pool");

         if (s32_Retval == C_NO_ERR)
         {
            //Return
            tgl_assert(orc_XMLParser.SelectNodeParent() == "data-pools");
         }
      }
      if (s32_Retval == C_NO_ERR)
      {
         if (q_ExpectedSizeHere == true)
         {
            if (u32_ExpectedSize != orc_Node.c_DataPools.size())
            {
               C_SCLString c_Tmp;
               c_Tmp.PrintFormatted("Unexpected Datapool count, expected: %i, got %i", u32_ExpectedSize,
                                    orc_Node.c_DataPools.size());
               osc_write_log_warning("Load file", c_Tmp.c_str());
            }
         }
         //Return
         tgl_assert(orc_XMLParser.SelectNodeParent() == "core");
      }
   }
   else
   {
      osc_write_log_error("Loading Datapools", "Could not find \"data-pools\" node.");
      s32_Retval = C_CONFIG;
   }

   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Save node data pools

   Save node to XML file
   pre-condition: the passed XML parser has the active node set to "core"
   post-condition: the passed XML parser has the active node set to the same "core"

   \param[in]     orc_NodeDataPools data storage
   \param[in,out] orc_XMLParser     XML with core active
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OSCNodeFilerV2::mh_SaveDataPools(const std::vector<C_OSCNodeDataPool> & orc_NodeDataPools,
                                        C_OSCXMLParserBase & orc_XMLParser)
{
   orc_XMLParser.CreateAndSelectNodeChild("data-pools");
   orc_XMLParser.SetAttributeUint32("length", orc_NodeDataPools.size());
   for (uint32 u32_ItDataPool = 0; u32_ItDataPool < orc_NodeDataPools.size(); ++u32_ItDataPool)
   {
      orc_XMLParser.CreateAndSelectNodeChild("data-pool");
      C_OSCNodeDataPoolFilerV2::h_SaveDataPool(orc_NodeDataPools[u32_ItDataPool], orc_XMLParser);
      //Return
      tgl_assert(orc_XMLParser.SelectNodeParent() == "data-pools");
   }
   //Return
   tgl_assert(orc_XMLParser.SelectNodeParent() == "core");
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Transform diagnostic server type to string

   \param[in] ore_DiagnosticProtocol Diagnostic protocol type

   \return
   Stringified diagnostic server type
*/
//----------------------------------------------------------------------------------------------------------------------
C_SCLString C_OSCNodeFilerV2::mh_DiagnosticServerToString(
   const C_OSCNodeProperties::E_DiagnosticServerProtocol & ore_DiagnosticProtocol)
{
   C_SCLString c_Retval;

   switch (ore_DiagnosticProtocol)
   {
   case C_OSCNodeProperties::eDS_OPEN_SYDE:
      c_Retval = "open-syde";
      break;
   case C_OSCNodeProperties::eDS_KEFEX:
      c_Retval = "kefex";
      break;
   case C_OSCNodeProperties::eDS_NONE:
      c_Retval = "none";
      break;
   default:
      c_Retval = "invalid";
      break;
   }
   return c_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Transform string to diagnostic server type

   \param[in]  orc_String String to interpret
   \param[out] ore_Type   Diagnostic server type

   \return
   C_NO_ERR   no error
   C_RANGE    String unknown
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_OSCNodeFilerV2::mh_StringToDiagnosticServer(const C_SCLString & orc_String,
                                                     C_OSCNodeProperties::E_DiagnosticServerProtocol & ore_Type)
{
   sint32 s32_Retval = C_NO_ERR;

   if (orc_String == "kefex")
   {
      ore_Type = C_OSCNodeProperties::eDS_KEFEX;
   }
   else if (orc_String == "open-syde")
   {
      ore_Type = C_OSCNodeProperties::eDS_OPEN_SYDE;
   }
   else if (orc_String == "none")
   {
      ore_Type = C_OSCNodeProperties::eDS_NONE;
   }
   else
   {
      osc_write_log_error("Loading node definition", "Invalid value for \"properties\".\"diagnostic-server\":" +
                          orc_String);
      s32_Retval = C_RANGE;
   }

   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Transform flash loader type to string

   \param[in] ore_FlashLoader Flash loader type

   \return
   Stringified flash loader type
*/
//----------------------------------------------------------------------------------------------------------------------
C_SCLString C_OSCNodeFilerV2::mh_FlashLoaderToString(const C_OSCNodeProperties::E_FlashLoaderProtocol & ore_FlashLoader)
{
   C_SCLString c_Retval;

   switch (ore_FlashLoader)
   {
   case C_OSCNodeProperties::eFL_OPEN_SYDE:
      c_Retval = "open-syde";
      break;
   case C_OSCNodeProperties::eFL_STW:
      c_Retval = "stw";
      break;
   case C_OSCNodeProperties::eFL_NONE:
      c_Retval = "none";
      break;
   default:
      c_Retval = "invalid";
      break;
   }
   return c_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Transform string to flash loader type

   \param[in]  orc_String String to interpret
   \param[out] ore_Type   Flash loader type

   \return
   C_NO_ERR   no error
   C_RANGE    String unknown
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_OSCNodeFilerV2::mh_StringToFlashLoader(const C_SCLString & orc_String,
                                                C_OSCNodeProperties::E_FlashLoaderProtocol & ore_Type)
{
   sint32 s32_Retval = C_NO_ERR;

   if (orc_String == "stw")
   {
      ore_Type = C_OSCNodeProperties::eFL_STW;
   }
   else if (orc_String == "open-syde")
   {
      ore_Type = C_OSCNodeProperties::eFL_OPEN_SYDE;
   }
   else if (orc_String == "none")
   {
      ore_Type = C_OSCNodeProperties::eFL_NONE;
   }
   else
   {
      osc_write_log_error("Loading node definition",
                          "Invalid value for \"properties\".\"flash-loader\":" + orc_String);
      s32_Retval = C_RANGE;
   }

   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Transform node data pool communication byte order type to string

   \param[in] ore_CommunicationByteOrder Node data pool communication byte order type

   \return
   Stringified node data pool communication byte order type
*/
//----------------------------------------------------------------------------------------------------------------------
C_SCLString C_OSCNodeFilerV2::mh_CommunicationByteOrderToString(
   const C_OSCCanSignal::E_ByteOrderType & ore_CommunicationByteOrder)
{
   C_SCLString c_Retval;

   switch (ore_CommunicationByteOrder)
   {
   case C_OSCCanSignal::eBYTE_ORDER_INTEL:
      c_Retval = "intel";
      break;
   case C_OSCCanSignal::eBYTE_ORDER_MOTOROLA:
      c_Retval = "motorola";
      break;
   default:
      c_Retval = "invalid";
      break;
   }
   return c_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Transform string to node data pool communication byte order type

   \param[in]  orc_String String to interpret
   \param[out] ore_Type   Node data pool communication byte order type

   \return
   C_NO_ERR   no error
   C_RANGE    String unknown
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_OSCNodeFilerV2::mh_StringToCommunicationByteOrder(const C_SCLString & orc_String,
                                                           C_OSCCanSignal::E_ByteOrderType & ore_Type)
{
   sint32 s32_Retval = C_NO_ERR;

   if (orc_String == "intel")
   {
      ore_Type = C_OSCCanSignal::eBYTE_ORDER_INTEL;
   }
   else if (orc_String == "motorola")
   {
      ore_Type = C_OSCCanSignal::eBYTE_ORDER_MOTOROLA;
   }
   else
   {
      osc_write_log_error("Loading node definition", "Invalid value for \"com-message\".\"com-signals\".\nbyte-order\":" +
                          orc_String);
      s32_Retval = C_RANGE;
   }

   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Transform node data pool communication message transmission method type to string

   \param[in] ore_NodeComMessageTxMethod Node data pool communication message transmission method type

   \return
   Stringified node data pool communication message transmission method type
*/
//----------------------------------------------------------------------------------------------------------------------
C_SCLString C_OSCNodeFilerV2::mh_NodeComMessageTxMethodToString(
   const C_OSCCanMessage::E_TxMethodType & ore_NodeComMessageTxMethod)
{
   C_SCLString c_Retval;

   switch (ore_NodeComMessageTxMethod)
   {
   case C_OSCCanMessage::eTX_METHOD_CYCLIC:
      c_Retval = "cyclic";
      break;
   case C_OSCCanMessage::eTX_METHOD_ON_CHANGE:
      c_Retval = "on-data-change";
      break;
   case C_OSCCanMessage::eTX_METHOD_ON_EVENT:
      c_Retval = "on-application-event";
      break;
   default:
      c_Retval = "invalid";
      break;
   }
   return c_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Transform string to node data pool communication message transmission method type

   \param[in]  orc_String String to interpret
   \param[out] ore_Type   Node data pool communication message transmission method type
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OSCNodeFilerV2::mh_StringToNodeComMessageTxMethod(const C_SCLString & orc_String,
                                                         C_OSCCanMessage::E_TxMethodType & ore_Type)
{
   if (orc_String == "cyclic")
   {
      ore_Type = C_OSCCanMessage::eTX_METHOD_CYCLIC;
   }
   else if (orc_String == "on-data-change")
   {
      ore_Type = C_OSCCanMessage::eTX_METHOD_ON_CHANGE;
   }
   else if (orc_String == "on-application-event")
   {
      ore_Type = C_OSCCanMessage::eTX_METHOD_ON_EVENT;
   }
   else
   {
      //best guess:
      ore_Type = C_OSCCanMessage::eTX_METHOD_ON_CHANGE;
      osc_write_log_warning("Loading project file", "Invalid transmission type \"" + orc_String + "\" in file. " \
                            "Assuming \"on_change\".");
   }
}
