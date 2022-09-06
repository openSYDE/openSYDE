//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Filer for comm description files (implementation)

   Filer for comm description files

   \copyright   Copyright 2019 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.h"

#include "stwtypes.h"
#include "stwerrors.h"
#include "C_OSCNodeCommFiler.h"
#include "C_OSCLoggingHandler.h"
#include "C_OSCSystemFilerUtil.h"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw_scl;
using namespace stw_types;
using namespace stw_errors;
using namespace stw_opensyde_core;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

/* -- Global Variables ---------------------------------------------------------------------------------------------- */

/* -- Module Global Variables --------------------------------------------------------------------------------------- */

/* -- Module Global Function Prototypes ----------------------------------------------------------------------------- */

/* -- Implementation ------------------------------------------------------------------------------------------------ */

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Load comm definition file

   \param[out]  orc_NodeComProtocol    Data storage
   \param[in]   orc_FilePath           File path
   \param[in]   orc_NodeDataPools      Loaded datapools

   \return
   C_NO_ERR   data read
   C_CONFIG   content of file is invalid or incomplete
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_OSCNodeCommFiler::h_LoadNodeComProtocolFile(C_OSCCanProtocol & orc_NodeComProtocol,
                                                     const C_SCLString & orc_FilePath,
                                                     const std::vector<C_OSCNodeDataPool> & orc_NodeDataPools)
{
   C_OSCXMLParser c_XMLParser;
   sint32 s32_Retval = C_OSCSystemFilerUtil::h_GetParserForExistingFile(c_XMLParser, orc_FilePath,
                                                                        "opensyde-comm-core-definition");

   //File version
   if (c_XMLParser.SelectNodeChild("file-version") == "file-version")
   {
      uint16 u16_FileVersion = 0U;
      try
      {
         u16_FileVersion = static_cast<uint16>(c_XMLParser.GetNodeContent().ToInt());
      }
      catch (...)
      {
         osc_write_log_error("Loading node definition", "\"file-version\" could not be converted to a number.");
         s32_Retval = C_CONFIG;
      }

      //is the file version one we know ?
      if (s32_Retval == C_NO_ERR)
      {
         osc_write_log_info("Loading node definition", "Value of \"file-version\": " +
                            C_SCLString::IntToStr(u16_FileVersion));
         //Check file version
         if (u16_FileVersion != 1U)
         {
            osc_write_log_error("Loading node definition",
                                "Version defined by \"file-version\" is not supported.");
            s32_Retval = C_CONFIG;
         }
      }

      //Return
      c_XMLParser.SelectNodeParent();
   }
   else
   {
      osc_write_log_error("Loading node definition", "Could not find \"file-version\" node.");
      s32_Retval = C_CONFIG;
   }

   if (s32_Retval == C_NO_ERR)
   {
      if (c_XMLParser.SelectNodeChild("com-protocol") == "com-protocol")
      {
         s32_Retval = C_OSCNodeCommFiler::h_LoadNodeComProtocol(orc_NodeComProtocol, c_XMLParser, orc_NodeDataPools);
      }
      else
      {
         osc_write_log_error("Loading node definition", "Could not find \"com-protocol\" node.");
         s32_Retval = C_CONFIG;
      }
   }
   else
   {
      //More details are in log
      s32_Retval = C_CONFIG;
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Load node com protocol

   Load node data from XML file
   pre-condition: the passed XML parser has the active node set to "com-protocol"
   post-condition: the passed XML parser has the active node set to the same "com-protocol"

   \param[out]     orc_NodeComProtocol    data storage
   \param[in,out]  orc_XMLParser          XML with list active
   \param[in]      orc_NodeDataPools      Loaded datapools

   \return
   C_NO_ERR   data read
   C_CONFIG   content of file is invalid or incomplete
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_OSCNodeCommFiler::h_LoadNodeComProtocol(C_OSCCanProtocol & orc_NodeComProtocol,
                                                 C_OSCXMLParserBase & orc_XMLParser,
                                                 const std::vector<C_OSCNodeDataPool> & orc_NodeDataPools)
{
   sint32 s32_Retval = C_NO_ERR;

   //Get index from name
   if (orc_XMLParser.AttributeExists("data-pool-name") == true)
   {
      bool q_Found = false;
      const C_SCLString c_DatapoolName = orc_XMLParser.GetAttributeString("data-pool-name");
      for (uint32 u32_ItDatapool = 0UL; u32_ItDatapool < orc_NodeDataPools.size(); ++u32_ItDatapool)
      {
         const C_OSCNodeDataPool & rc_CurDatapool = orc_NodeDataPools[u32_ItDatapool];
         if (rc_CurDatapool.c_Name == c_DatapoolName)
         {
            orc_NodeComProtocol.u32_DataPoolIndex = u32_ItDatapool;
            q_Found = true;
            break;
         }
      }
      if (q_Found == false)
      {
         osc_write_log_error("Loading node definition",
                             "Could not find data pool with name \"" + c_DatapoolName + "\"");
         s32_Retval = C_CONFIG;
      }
   }
   else
   {
      osc_write_log_error("Loading node definition", "Could not find \"data-pool-name\" attribute.");
      s32_Retval = C_CONFIG;
   }

   if ((orc_XMLParser.SelectNodeChild("communication-protocol") == "communication-protocol") &&
       (s32_Retval == C_NO_ERR))
   {
      s32_Retval = h_StringToCommunicationProtocol(orc_XMLParser.GetNodeContent(), orc_NodeComProtocol.e_Type);
      //Return (don't check to allow reuse)
      orc_XMLParser.SelectNodeParent();
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
      //Return (don't check to allow reuse)
      orc_XMLParser.SelectNodeParent();
   }
   else
   {
      osc_write_log_error("Loading node definition", "Could not find \"com-message-containers\" node.");
      s32_Retval = C_CONFIG;
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Save comm definition

   Save comm definition to XML file

   \param[in]  orc_NodeComProtocol  data storage
   \param[in]  orc_FilePath         File path for xml
   \param[in]  orc_DatapoolName     Datapool name

   \return
   C_NO_ERR   data saved
   C_CONFIG   file could not be created
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_OSCNodeCommFiler::h_SaveNodeComProtocolFile(const C_OSCCanProtocol & orc_NodeComProtocol,
                                                     const C_SCLString & orc_FilePath,
                                                     const C_SCLString & orc_DatapoolName)
{
   C_OSCXMLParser c_XMLParser;
   sint32 s32_Retval = C_OSCSystemFilerUtil::h_GetParserForNewFile(c_XMLParser, orc_FilePath,
                                                                   "opensyde-comm-core-definition");

   if (s32_Retval == C_NO_ERR)
   {
      //Version
      c_XMLParser.CreateNodeChild("file-version", "1");
      tgl_assert(c_XMLParser.CreateAndSelectNodeChild("com-protocol") == "com-protocol");
      //node
      C_OSCNodeCommFiler::h_SaveNodeComProtocol(orc_NodeComProtocol, c_XMLParser, orc_DatapoolName);
      //Don't forget to save!
      if (c_XMLParser.SaveToFile(orc_FilePath) != C_NO_ERR)
      {
         osc_write_log_error("Saving node definition", "Could not create file for node.");
         s32_Retval = C_CONFIG;
      }
   }
   else
   {
      //More details are in log
      s32_Retval = C_CONFIG;
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Save node com protocol

   Save node to XML file
   pre-condition: the passed XML parser has the active node set to "com-protocol"
   post-condition: the passed XML parser has the active node set to the same "com-protocol"

   \param[in]      orc_NodeComProtocol    data storage
   \param[in,out]  orc_XMLParser          XML with list active
   \param[in]      orc_DatapoolName       Datapool name
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OSCNodeCommFiler::h_SaveNodeComProtocol(const C_OSCCanProtocol & orc_NodeComProtocol,
                                               C_OSCXMLParserBase & orc_XMLParser, const C_SCLString & orc_DatapoolName)
{
   orc_XMLParser.SetAttributeString("data-pool-name", orc_DatapoolName);
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

   \param[out]     orc_NodeComMessageContainers    data storage
   \param[in,out]  orc_XMLParser                   XML with list active

   \return
   C_NO_ERR   data read
   C_CONFIG   content of file is invalid or incomplete
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_OSCNodeCommFiler::h_LoadNodeComMessageContainers(
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

   \param[in]      orc_NodeComMessageContainers    data storage
   \param[in,out]  orc_XMLParser                   XML with list active
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OSCNodeCommFiler::h_SaveNodeComMessageContainers(
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

   \param[out]     orc_NodeComMessageContainer  data storage
   \param[in,out]  orc_XMLParser                XML with list active

   \return
   C_NO_ERR   data read
   C_CONFIG   content of file is invalid or incomplete
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_OSCNodeCommFiler::h_LoadNodeComMessageContainer(C_OSCCanMessageContainer & orc_NodeComMessageContainer,
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

   \param[in]      orc_NodeComMessageContainer  data storage
   \param[in,out]  orc_XMLParser                XML with list active
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OSCNodeCommFiler::h_SaveNodeComMessageContainer(const C_OSCCanMessageContainer & orc_NodeComMessageContainer,
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

   \param[out]     orc_NodeComMessages    data storage
   \param[in,out]  orc_XMLParser          XML with list active

   \return
   C_NO_ERR   data read
   C_CONFIG   content of file is invalid or incomplete
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_OSCNodeCommFiler::h_LoadNodeComMessages(std::vector<C_OSCCanMessage> & orc_NodeComMessages,
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

   \param[in]      orc_NodeComMessages    data storage
   \param[in,out]  orc_XMLParser          XML with list active
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OSCNodeCommFiler::h_SaveNodeComMessages(const std::vector<C_OSCCanMessage> & orc_NodeComMessages,
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

   \param[out]     orc_NodeComMessage  data storage
   \param[in,out]  orc_XMLParser       XML with list active

   \return
   C_NO_ERR   data read
   C_CONFIG   content of file is invalid or incomplete
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_OSCNodeCommFiler::h_LoadNodeComMessage(C_OSCCanMessage & orc_NodeComMessage,
                                                C_OSCXMLParserBase & orc_XMLParser)
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
   if (s32_Retval == C_NO_ERR)
   {
      s32_Retval = mh_LoadMessageCANOpenPart(orc_NodeComMessage, orc_XMLParser);
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Save node com message

   Save node to XML file
   pre-condition: the passed XML parser has the active node set to "com-message"
   post-condition: the passed XML parser has the active node set to the same "com-message"

   \param[in]      orc_NodeComMessage  data storage
   \param[in,out]  orc_XMLParser       XML with list active
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OSCNodeCommFiler::h_SaveNodeComMessage(const C_OSCCanMessage & orc_NodeComMessage,
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
   C_OSCNodeCommFiler::mh_SaveMessageCANOpenPart(orc_NodeComMessage, orc_XMLParser);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Load node com signals

   Load node data from XML file
   pre-condition: the passed XML parser has the active node set to "com-signals"
   post-condition: the passed XML parser has the active node set to the same "com-signals"

   \param[out]     orc_NodeComSignals  data storage
   \param[in,out]  orc_XMLParser       XML with list active

   \return
   C_NO_ERR   data read
   C_CONFIG   content of file is invalid or incomplete
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_OSCNodeCommFiler::h_LoadNodeComSignals(std::vector<C_OSCCanSignal> & orc_NodeComSignals,
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

   \param[in]      orc_NodeComSignals  data storage
   \param[in,out]  orc_XMLParser       XML with list active
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OSCNodeCommFiler::h_SaveNodeComSignals(const std::vector<C_OSCCanSignal> & orc_NodeComSignals,
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

   \param[out]     orc_NodeComSignal   data storage
   \param[in,out]  orc_XMLParser       XML with list active

   \return
   C_NO_ERR   data read
   C_CONFIG   content of file is invalid or incomplete
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_OSCNodeCommFiler::h_LoadNodeComSignal(C_OSCCanSignal & orc_NodeComSignal, C_OSCXMLParserBase & orc_XMLParser)
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
   if ((s32_Retval == C_NO_ERR) && (orc_XMLParser.SelectNodeChild("multiplexer-type") == "multiplexer-type"))
   {
      s32_Retval = mh_StringToCommunicationMuxType(orc_XMLParser.GetNodeContent(), orc_NodeComSignal.e_MultiplexerType);
      //Return
      tgl_assert(orc_XMLParser.SelectNodeParent() == "com-signal");
   }
   else
   {
      //Optional
      orc_NodeComSignal.e_MultiplexerType = C_OSCCanSignal::eMUX_DEFAULT;
   }
   if (orc_XMLParser.AttributeExists("multiplexer-value"))
   {
      orc_NodeComSignal.u16_MultiplexValue = static_cast<uint16>(orc_XMLParser.GetAttributeUint32("multiplexer-value"));
   }
   else
   {
      orc_NodeComSignal.u16_MultiplexValue = 0;
   }
   if (s32_Retval == C_NO_ERR)
   {
      s32_Retval = mh_LoadSignalCANOpenPart(orc_NodeComSignal, orc_XMLParser);
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Save node com signal

   Save node to XML file
   pre-condition: the passed XML parser has the active node set to "com-signal"
   post-condition: the passed XML parser has the active node set to the same "com-signal"

   \param[in]      orc_NodeComSignal   data storage
   \param[in,out]  orc_XMLParser       XML with list active
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OSCNodeCommFiler::h_SaveNodeComSignal(const C_OSCCanSignal & orc_NodeComSignal,
                                             C_OSCXMLParserBase & orc_XMLParser)
{
   orc_XMLParser.SetAttributeUint32("data-element-index", orc_NodeComSignal.u32_ComDataElementIndex);
   orc_XMLParser.SetAttributeUint32("bit-start", orc_NodeComSignal.u16_ComBitStart);
   orc_XMLParser.SetAttributeUint32("bit-length", orc_NodeComSignal.u16_ComBitLength);
   orc_XMLParser.SetAttributeSint32("multiplexer-value", orc_NodeComSignal.u16_MultiplexValue);
   orc_XMLParser.CreateNodeChild("byte-order", mh_CommunicationByteOrderToString(orc_NodeComSignal.e_ComByteOrder));
   orc_XMLParser.CreateNodeChild("multiplexer-type", mh_CommunicationMuxTypeToString(
                                    orc_NodeComSignal.e_MultiplexerType));
   orc_XMLParser.CreateAndSelectNodeChild("can-open-object-dictionary");
   mh_SaveSignalCANOpenPart(orc_NodeComSignal, orc_XMLParser);
   orc_XMLParser.SelectNodeParent();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Transform communication protocol type to string

   \param[in]  ore_CommunicationProtocol  Communication protocol type

   \return
   Stringified communication protocol type
*/
//----------------------------------------------------------------------------------------------------------------------
C_SCLString C_OSCNodeCommFiler::h_CommunicationProtocolToString(
   const C_OSCCanProtocol::E_Type & ore_CommunicationProtocol)
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
   case C_OSCCanProtocol::eCAN_OPEN:
      c_Retval = "can open";
      break;
   default:
      c_Retval = "invalid";
      break;
   }
   return c_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Transform string to communication protocol type

   \param[in]   orc_String    String to interpret
   \param[out]  ore_Type      Communication protocol type

   \return
   C_NO_ERR   no error
   C_RANGE    String unknown
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_OSCNodeCommFiler::h_StringToCommunicationProtocol(const C_SCLString & orc_String,
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
   else if (orc_String == "can open")
   {
      ore_Type = C_OSCCanProtocol::eCAN_OPEN;
   }
   else
   {
      osc_write_log_error("Loading node definition", "Invalid value for \"communication-protocol\":" + orc_String);
      s32_Retval = C_RANGE;
   }

   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get automatically generated file name

   \param[in]  orc_DatapoolName  Datapool name

   \return
   Automatically generated file name
*/
//----------------------------------------------------------------------------------------------------------------------
C_SCLString C_OSCNodeCommFiler::h_GetFileName(const C_SCLString & orc_DatapoolName)
{
   return "comm_" + C_OSCSystemFilerUtil::h_PrepareItemNameForFileName(orc_DatapoolName) + "_core.xml";
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Save node owner index

   \param[in]      orc_OwnerNodeIndex  Owner node index
   \param[in,out]  orc_XMLParser       XML parser
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OSCNodeCommFiler::h_SaveNodeOwnerIndex(const C_OSCCanInterfaceId & orc_OwnerNodeIndex,
                                              C_OSCXMLParserBase & orc_XMLParser)
{
   orc_XMLParser.CreateAndSelectNodeChild("node-owner-index");
   orc_XMLParser.SetAttributeUint32("node-index", orc_OwnerNodeIndex.u32_NodeIndex);
   orc_XMLParser.SetAttributeUint32("interface-id", orc_OwnerNodeIndex.u8_InterfaceNumber);
   //Return
   tgl_assert(orc_XMLParser.SelectNodeParent() == "node-owner-indices");
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Load node owner index

   \param[in,out]  orc_OwnerNodeIndex  Owner node index
   \param[in,out]  orc_XMLParser       XML parser

   \return
   C_NO_ERR   data read
   C_CONFIG   content of file is invalid or incomplete
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_OSCNodeCommFiler::h_LoadNodeOwnerIndex(C_OSCCanInterfaceId & orc_OwnerNodeIndex,
                                                const C_OSCXMLParserBase & orc_XMLParser)
{
   sint32 s32_Retval = orc_XMLParser.GetAttributeUint32Error("node-index", orc_OwnerNodeIndex.u32_NodeIndex);

   if (s32_Retval == C_NO_ERR)
   {
      uint32 u32_Value;
      s32_Retval = orc_XMLParser.GetAttributeUint32Error("interface-id", u32_Value);
      orc_OwnerNodeIndex.u8_InterfaceNumber = static_cast<uint8>(u32_Value);
   }

   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Save message CAN open part

   \param[in]      orc_NodeCommMessage    Node COMM message
   \param[in,out]  orc_XMLParser          XML parser
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OSCNodeCommFiler::mh_SaveMessageCANOpenPart(const C_OSCCanMessage & orc_NodeCommMessage,
                                                   C_OSCXMLParserBase & orc_XMLParser)
{
   orc_XMLParser.CreateAndSelectNodeChild("can-open");
   orc_XMLParser.SetAttributeBool("cob-id-includes-node-id", orc_NodeCommMessage.q_CanOpenManagerCobIdIncludesNodeID);
   orc_XMLParser.SetAttributeUint32("cob-id-offset", orc_NodeCommMessage.u32_CanOpenManagerCobIdOffset);
   orc_XMLParser.SetAttributeBool("message-active", orc_NodeCommMessage.q_CanOpenManagerMessageActive);
   orc_XMLParser.SetAttributeUint32("pdo-index", orc_NodeCommMessage.u16_CanOpenManagerPdoIndex);
   C_OSCNodeCommFiler::mh_SaveNodeOwnerIndices(orc_NodeCommMessage.c_CanOpenManagerOwnerNodeIndex, orc_XMLParser);
   //Return
   tgl_assert(orc_XMLParser.SelectNodeParent() == "com-message");
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Load message CAN open part

   \param[in,out]  orc_NodeCommMessage    Node comm message
   \param[in,out]  orc_XMLParser          XML parser

   \return
   C_NO_ERR   data read
   C_CONFIG   content of file is invalid or incomplete
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_OSCNodeCommFiler::mh_LoadMessageCANOpenPart(C_OSCCanMessage & orc_NodeCommMessage,
                                                     C_OSCXMLParserBase & orc_XMLParser)
{
   sint32 s32_Retval = C_NO_ERR;

   if (orc_XMLParser.SelectNodeChild("can-open") == "can-open")
   {
      s32_Retval = orc_XMLParser.GetAttributeBoolError("cob-id-includes-node-id",
                                                       orc_NodeCommMessage.q_CanOpenManagerCobIdIncludesNodeID);

      if (s32_Retval == C_NO_ERR)
      {
         s32_Retval = orc_XMLParser.GetAttributeUint32Error("cob-id-offset",
                                                            orc_NodeCommMessage.u32_CanOpenManagerCobIdOffset);
      }

      if (s32_Retval == C_NO_ERR)
      {
         s32_Retval = orc_XMLParser.GetAttributeBoolError("message-active",
                                                          orc_NodeCommMessage.q_CanOpenManagerMessageActive);
      }

      if (s32_Retval == C_NO_ERR)
      {
         uint32 u32_Value;
         s32_Retval = orc_XMLParser.GetAttributeUint32Error("pdo-index", u32_Value);
         if (s32_Retval == C_NO_ERR)
         {
            orc_NodeCommMessage.u16_CanOpenManagerPdoIndex = static_cast<uint16>(u32_Value);
         }
      }

      if (s32_Retval == C_NO_ERR)
      {
         s32_Retval = mh_LoadNodeOwnerIndices(orc_NodeCommMessage.c_CanOpenManagerOwnerNodeIndex, orc_XMLParser);
      }
      if (s32_Retval == C_NO_ERR)
      {
         //Return
         tgl_assert(orc_XMLParser.SelectNodeParent() == "com-message");
      }
   }

   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Save node owner indices

   \param[in]      orc_OwnerNodeIndex  Owner node index
   \param[in,out]  orc_XMLParser       XML parser
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OSCNodeCommFiler::mh_SaveNodeOwnerIndices(const C_OSCCanInterfaceId & orc_OwnerNodeIndex,
                                                 C_OSCXMLParserBase & orc_XMLParser)
{
   orc_XMLParser.CreateAndSelectNodeChild("node-owner-indices");
   orc_XMLParser.SetAttributeUint32("length", 1UL);
   C_OSCNodeCommFiler::h_SaveNodeOwnerIndex(orc_OwnerNodeIndex, orc_XMLParser);

   //Return
   tgl_assert(orc_XMLParser.SelectNodeParent() == "can-open");
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Load node owner indices

   \param[in,out]  orc_OwnerNodeIndex  Owner node index
   \param[in,out]  orc_XMLParser       XML parser

   \return
   C_NO_ERR   data read
   C_CONFIG   content of file is invalid or incomplete
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_OSCNodeCommFiler::mh_LoadNodeOwnerIndices(C_OSCCanInterfaceId & orc_OwnerNodeIndex,
                                                   C_OSCXMLParserBase & orc_XMLParser)
{
   sint32 s32_Retval = orc_XMLParser.SelectNodeChildError("node-owner-indices");

   if (s32_Retval == C_NO_ERR)
   {
      uint32 u32_ExpectedSize;
      s32_Retval = orc_XMLParser.GetAttributeUint32Error("length", u32_ExpectedSize);
      if (s32_Retval == C_NO_ERR)
      {
         stw_scl::C_SCLString c_NodeName = orc_XMLParser.SelectNodeChild("node-owner-index");
         if (c_NodeName == "node-owner-index")
         {
            do
            {
               C_OSCCanInterfaceId c_OwnerNodeIndex;
               s32_Retval = h_LoadNodeOwnerIndex(c_OwnerNodeIndex, orc_XMLParser);
               if (s32_Retval == C_NO_ERR)
               {
                  orc_OwnerNodeIndex = c_OwnerNodeIndex;
               }
               c_NodeName = orc_XMLParser.SelectNodeNext("node-owner-index");
            }
            while ((c_NodeName == "node-owner-index") && (s32_Retval == C_NO_ERR));
            //Return
            tgl_assert(orc_XMLParser.SelectNodeParent() == "node-owner-indices");
         }
      }
      if (s32_Retval == C_NO_ERR)
      {
         if (u32_ExpectedSize != 1)
         {
            C_SCLString c_Tmp;
            c_Tmp.PrintFormatted("Unexpected node owner indices count, expected: %i, got %i", u32_ExpectedSize,
                                 1);
            osc_write_log_warning("Load file", c_Tmp.c_str());
         }
         //Return
         tgl_assert(orc_XMLParser.SelectNodeParent() == "can-open");
      }
   }

   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Save signal CAN open part

   \param[in]      orc_NodeCommSignal  Node COMM signal
   \param[in,out]  orc_XMLParser       XML parser
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OSCNodeCommFiler::mh_SaveSignalCANOpenPart(const C_OSCCanSignal & orc_NodeCommSignal,
                                                  C_OSCXMLParserBase & orc_XMLParser)
{
   orc_XMLParser.SetAttributeUint32("index", orc_NodeCommSignal.u16_CanOpenManagerObjectDictionaryIndex);
   orc_XMLParser.SetAttributeUint32("sub-index", orc_NodeCommSignal.u8_CanOpenManagerObjectDictionarySubIndex);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Load signal CAN open part

   \param[in,out]  orc_NodeCommSignal  Node COMM signal
   \param[in,out]  orc_XMLParser       XML parser

   \return
   STW error codes

   \retval   C_NO_ERR   Detailed description
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_OSCNodeCommFiler::mh_LoadSignalCANOpenPart(C_OSCCanSignal & orc_NodeCommSignal,
                                                    C_OSCXMLParserBase & orc_XMLParser)
{
   sint32 s32_Retval = C_NO_ERR;

   if (orc_XMLParser.SelectNodeChild("can-open-object-dictionary") == "can-open-object-dictionary")
   {
      uint32 u32_Value;
      s32_Retval = orc_XMLParser.GetAttributeUint32Error("index", u32_Value);
      if (s32_Retval == C_NO_ERR)
      {
         orc_NodeCommSignal.u16_CanOpenManagerObjectDictionaryIndex = static_cast<uint16>(u32_Value);
         s32_Retval = orc_XMLParser.GetAttributeUint32Error("sub-index", u32_Value);
         if (s32_Retval == C_NO_ERR)
         {
            orc_NodeCommSignal.u8_CanOpenManagerObjectDictionarySubIndex = static_cast<uint8>(u32_Value);
         }
      }
      //Return
      tgl_assert(orc_XMLParser.SelectNodeParent() == "com-signal");
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Transform node data pool communication byte order type to string

   \param[in]  ore_CommunicationByteOrder    Node data pool communication byte order type

   \return
   Stringified node data pool communication byte order type
*/
//----------------------------------------------------------------------------------------------------------------------
C_SCLString C_OSCNodeCommFiler::mh_CommunicationByteOrderToString(
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
      break;
   }
   return c_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Transform string to node data pool communication byte order type

   \param[in]   orc_String    String to interpret
   \param[out]  ore_Type      Node data pool communication byte order type

   \return
   C_NO_ERR   no error
   C_RANGE    String unknown
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_OSCNodeCommFiler::mh_StringToCommunicationByteOrder(const C_SCLString & orc_String,
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
/*! \brief   Transform node data pool communication mux type to string

   \param[in]  ore_CommunicationByteOrder    Node data pool communication mux type

   \return
   Stringified node data pool communication mux type
*/
//----------------------------------------------------------------------------------------------------------------------
C_SCLString C_OSCNodeCommFiler::mh_CommunicationMuxTypeToString(
   const C_OSCCanSignal::E_MultiplexerType & ore_CommunicationByteOrder)
{
   C_SCLString c_Retval;

   switch (ore_CommunicationByteOrder)
   {
   case C_OSCCanSignal::eMUX_DEFAULT:
      c_Retval = "default";
      break;
   case C_OSCCanSignal::eMUX_MULTIPLEXER_SIGNAL:
      c_Retval = "multiplexer";
      break;
   case C_OSCCanSignal::eMUX_MULTIPLEXED_SIGNAL:
      c_Retval = "multiplexed";
      break;
   default:
      break;
   }
   return c_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Transform string to node data pool communication mux type

   \param[in]   orc_String    String to interpret
   \param[out]  ore_Type      Node data pool communication mux type

   \return
   C_NO_ERR   no error
   C_RANGE    String unknown
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_OSCNodeCommFiler::mh_StringToCommunicationMuxType(const C_SCLString & orc_String,
                                                           C_OSCCanSignal::E_MultiplexerType & ore_Type)
{
   sint32 s32_Retval = C_NO_ERR;

   if (orc_String == "default")
   {
      ore_Type = C_OSCCanSignal::eMUX_DEFAULT;
   }
   else if (orc_String == "multiplexer")
   {
      ore_Type = C_OSCCanSignal::eMUX_MULTIPLEXER_SIGNAL;
   }
   else if (orc_String == "multiplexed")
   {
      ore_Type = C_OSCCanSignal::eMUX_MULTIPLEXED_SIGNAL;
   }
   else
   {
      osc_write_log_error("Loading node definition", "Invalid value for \"com-message\".\"com-signals\".\nmultiplexer-type\":" +
                          orc_String);
      s32_Retval = C_RANGE;
   }

   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Transform node data pool communication message transmission method type to string

   \param[in]  ore_NodeComMessageTxMethod    Node data pool communication message transmission method type

   \return
   Stringified node data pool communication message transmission method type
*/
//----------------------------------------------------------------------------------------------------------------------
C_SCLString C_OSCNodeCommFiler::mh_NodeComMessageTxMethodToString(
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
   case C_OSCCanMessage::eTX_METHOD_CAN_OPEN_TYPE_254:
      c_Retval = "on-can-open-type-254";
      break;
   case C_OSCCanMessage::eTX_METHOD_CAN_OPEN_TYPE_255:
      c_Retval = "on-can-open-type-255";
      break;
   default:
      c_Retval = "invalid";
      break;
   }
   return c_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Transform string to node data pool communication message transmission method type

   \param[in]   orc_String    String to interpret
   \param[out]  ore_Type      Node data pool communication message transmission method type
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OSCNodeCommFiler::mh_StringToNodeComMessageTxMethod(const C_SCLString & orc_String,
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
   else if (orc_String == "on-can-open-type-254")
   {
      ore_Type = C_OSCCanMessage::eTX_METHOD_CAN_OPEN_TYPE_254;
   }
   else if (orc_String == "on-can-open-type-255")
   {
      ore_Type = C_OSCCanMessage::eTX_METHOD_CAN_OPEN_TYPE_255;
   }
   else
   {
      //best guess:
      ore_Type = C_OSCCanMessage::eTX_METHOD_ON_CHANGE;
      osc_write_log_warning("Loading project file", "Invalid transmission type \"" + orc_String + "\" in file. " \
                            "Assuming \"on_change\".");
   }
}
