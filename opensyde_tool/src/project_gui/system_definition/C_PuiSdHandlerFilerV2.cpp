//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       UI topology filer (V2 format) (implementation)

   UI topology filer (V2 format)

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.h"

#include <QByteArray>
#include <QBuffer>
#include "stwtypes.h"
#include "stwerrors.h"
#include "TGLUtils.h"
#include "C_OSCLoggingHandler.h"
#include "C_PuiSdHandlerFilerV2.h"
#include "C_PuiBsElementsFiler.h"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw_types;
using namespace stw_errors;
using namespace stw_tgl;
using namespace stw_scl;
using namespace stw_opensyde_gui_logic;
using namespace stw_opensyde_core;

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
C_PuiSdHandlerFilerV2::C_PuiSdHandlerFilerV2(void)
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Load data pools

   \param[in,out] orc_DataPools Data pool elements (Cleared if necessary)
   \param[in,out] orc_XMLParser XML parser with the "current" element set to the "gui" element

   \return
   C_NO_ERR    information loaded
   C_CONFIG    error loading information
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_PuiSdHandlerFilerV2::h_LoadDataPools(std::vector<C_PuiSdNodeDataPool> & orc_DataPools,
                                              C_OSCXMLParserBase & orc_XMLParser)
{
   sint32 s32_Retval = C_NO_ERR;

   //Clear last data pools
   orc_DataPools.clear();
   if (orc_XMLParser.SelectNodeChild("data-pools") == "data-pools")
   {
      C_SCLString c_CurrentDataPoolNode;
      uint32 u32_ExpectedSize = 0UL;
      const bool q_ExpectedSizeHere = orc_XMLParser.AttributeExists("length");

      //Check optional length
      if (q_ExpectedSizeHere == true)
      {
         u32_ExpectedSize = orc_XMLParser.GetAttributeUint32("length");
         orc_DataPools.reserve(u32_ExpectedSize);
      }

      c_CurrentDataPoolNode = orc_XMLParser.SelectNodeChild("data-pool");
      if (c_CurrentDataPoolNode == "data-pool")
      {
         uint32 u32_DataPoolIndex;
         do
         {
            u32_DataPoolIndex = orc_XMLParser.GetAttributeUint32("index");
            if (u32_DataPoolIndex >= orc_DataPools.size())
            {
               orc_DataPools.resize(static_cast<std::vector<C_PuiSdNodeDataPool>::size_type>(u32_DataPoolIndex + 1UL));
            }
            s32_Retval = h_LoadDataPool(orc_DataPools[u32_DataPoolIndex], orc_XMLParser);
            //Next
            c_CurrentDataPoolNode = orc_XMLParser.SelectNodeNext("data-pool");
         }
         while ((s32_Retval == C_NO_ERR) && (c_CurrentDataPoolNode == "data-pool"));
         //Return
         tgl_assert(orc_XMLParser.SelectNodeParent() == "data-pools");
      }
      //Compare length
      if ((s32_Retval == C_NO_ERR) && (q_ExpectedSizeHere == true))
      {
         if (u32_ExpectedSize != orc_DataPools.size())
         {
            C_SCLString c_Tmp;
            c_Tmp.PrintFormatted("Unexpected UI Datapool count, expected: %i, got %i", u32_ExpectedSize,
                                 orc_DataPools.size());
            osc_write_log_warning("Load file", c_Tmp.c_str());
         }
      }
      //Return
      tgl_assert(orc_XMLParser.SelectNodeParent() == "gui");
   }
   else
   {
      s32_Retval = C_CONFIG;
   }

   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Load data pool

   \param[in,out] orc_DataPool  Data pool element
   \param[in,out] orc_XMLParser XML parser with the "current" element set to the "data-pool" element

   \return
   C_NO_ERR    information loaded
   C_CONFIG    error loading information
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_PuiSdHandlerFilerV2::h_LoadDataPool(C_PuiSdNodeDataPool & orc_DataPool, C_OSCXMLParserBase & orc_XMLParser)
{
   sint32 s32_Retval = C_NO_ERR;

   if (orc_XMLParser.SelectNodeChild("lists") == "lists")
   {
      s32_Retval = h_LoadDataPoolLists(orc_DataPool.c_DataPoolLists, orc_XMLParser);
      //Return
      tgl_assert(orc_XMLParser.SelectNodeParent() == "data-pool");
   }
   else
   {
      s32_Retval = C_CONFIG;
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Load data pool lists

   \param[in,out] orc_DataPoolLists Data pool lists element
   \param[in,out] orc_XMLParser     XML parser with the "current" element set to the "lists" element

   \return
   C_NO_ERR    information loaded
   C_CONFIG    error loading information
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_PuiSdHandlerFilerV2::h_LoadDataPoolLists(std::vector<C_PuiSdNodeDataPoolList> & orc_DataPoolLists,
                                                  C_OSCXMLParserBase & orc_XMLParser)
{
   sint32 s32_Retval = C_NO_ERR;

   C_SCLString c_CurrentDataPoolListNode;
   uint32 u32_ExpectedSize = 0UL;
   const bool q_ExpectedSizeHere = orc_XMLParser.AttributeExists("length");

   //Check optional length
   if (q_ExpectedSizeHere == true)
   {
      u32_ExpectedSize = orc_XMLParser.GetAttributeUint32("length");
      orc_DataPoolLists.reserve(u32_ExpectedSize);
   }

   c_CurrentDataPoolListNode = orc_XMLParser.SelectNodeChild("list");

   orc_DataPoolLists.clear();
   if (c_CurrentDataPoolListNode == "list")
   {
      uint32 u32_DataPoolListIndex;
      do
      {
         u32_DataPoolListIndex = orc_XMLParser.GetAttributeUint32("index");
         if (u32_DataPoolListIndex >= orc_DataPoolLists.size())
         {
            orc_DataPoolLists.resize(static_cast<std::vector<C_PuiSdNodeDataPool>::size_type>(u32_DataPoolListIndex +
                                                                                              1UL));
         }
         if (s32_Retval == C_NO_ERR)
         {
            s32_Retval = h_LoadDataPoolList(orc_DataPoolLists[u32_DataPoolListIndex], orc_XMLParser);
         }
         //Next
         c_CurrentDataPoolListNode = orc_XMLParser.SelectNodeNext("list");
      }
      while (c_CurrentDataPoolListNode == "list");
      //Return
      tgl_assert(orc_XMLParser.SelectNodeParent() == "lists");
   }
   //Compare length
   if ((s32_Retval == C_NO_ERR) && (q_ExpectedSizeHere == true))
   {
      if (u32_ExpectedSize != orc_DataPoolLists.size())
      {
         C_SCLString c_Tmp;
         c_Tmp.PrintFormatted("Unexpected UI list count, expected: %i, got %i", u32_ExpectedSize,
                              orc_DataPoolLists.size());
         osc_write_log_warning("Load file", c_Tmp.c_str());
      }
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Load data pool list

   \param[in,out] orc_DataPoolList Data pool list element
   \param[in,out] orc_XMLParser    XML parser with the "current" element set to the "list" element

   \return
   C_NO_ERR    information loaded
   C_CONFIG    error loading information
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_PuiSdHandlerFilerV2::h_LoadDataPoolList(C_PuiSdNodeDataPoolList & orc_DataPoolList,
                                                 C_OSCXMLParserBase & orc_XMLParser)
{
   sint32 s32_Retval = C_NO_ERR;

   if (orc_XMLParser.SelectNodeChild("data-elements") == "data-elements")
   {
      h_LoadDataPoolListElements(orc_DataPoolList.c_DataPoolListElements, orc_XMLParser);
      //Return
      tgl_assert(orc_XMLParser.SelectNodeParent() == "list");
   }
   else
   {
      s32_Retval = C_CONFIG;
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Load data pool list elements

   \param[in,out] orc_DataPoolListElements Data pool list elements
   \param[in,out] orc_XMLParser            XML parser with the "current" element set to the "data-elements" element

   \return
   C_NO_ERR    information loaded
   C_CONFIG    error loading information
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_PuiSdHandlerFilerV2::h_LoadDataPoolListElements(
   std::vector<C_PuiSdNodeDataPoolListElement> & orc_DataPoolListElements, C_OSCXMLParserBase & orc_XMLParser)
{
   sint32 s32_Retval = C_NO_ERR;

   C_SCLString c_CurrentDataPoolListElementNode;
   uint32 u32_ExpectedSize = 0UL;
   const bool q_ExpectedSizeHere = orc_XMLParser.AttributeExists("length");

   //Check optional length
   if (q_ExpectedSizeHere == true)
   {
      u32_ExpectedSize = orc_XMLParser.GetAttributeUint32("length");
      orc_DataPoolListElements.reserve(u32_ExpectedSize);
   }

   c_CurrentDataPoolListElementNode = orc_XMLParser.SelectNodeChild("data-element");

   orc_DataPoolListElements.clear();
   if (c_CurrentDataPoolListElementNode == "data-element")
   {
      uint32 u32_DataPoolListElementIndex;
      do
      {
         u32_DataPoolListElementIndex = orc_XMLParser.GetAttributeUint32("index");
         if (u32_DataPoolListElementIndex >= orc_DataPoolListElements.size())
         {
            orc_DataPoolListElements.resize(static_cast<std::vector<C_PuiSdNodeDataPool>::size_type>(
                                               u32_DataPoolListElementIndex + 1UL));
         }
         h_LoadDataPoolListElement(orc_DataPoolListElements[u32_DataPoolListElementIndex],
                                   orc_XMLParser);

         //Next
         c_CurrentDataPoolListElementNode = orc_XMLParser.SelectNodeNext("data-element");
      }
      while (c_CurrentDataPoolListElementNode == "data-element");
      //Return
      tgl_assert(orc_XMLParser.SelectNodeParent() == "data-elements");
   }
   //Compare length
   if (q_ExpectedSizeHere == true)
   {
      if (u32_ExpectedSize != orc_DataPoolListElements.size())
      {
         C_SCLString c_Tmp;
         c_Tmp.PrintFormatted("Unexpected UI data element count, expected: %i, got %i", u32_ExpectedSize,
                              orc_DataPoolListElements.size());
         osc_write_log_warning("Load file", c_Tmp.c_str());
      }
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Load data pool list element

   \param[in,out] orc_DataPoolListElement Data pool list element
   \param[in,out] orc_XMLParser           XML parser with the "current" element set to the "data-element" element

   \return
   C_NO_ERR    information loaded
   C_CONFIG    error loading information
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PuiSdHandlerFilerV2::h_LoadDataPoolListElement(C_PuiSdNodeDataPoolListElement & orc_DataPoolListElement,
                                                      const C_OSCXMLParserBase & orc_XMLParser)
{
   orc_DataPoolListElement.q_AutoMinMaxActive = orc_XMLParser.GetAttributeBool("auto_min_max_active");
   orc_DataPoolListElement.q_InterpretAsString = orc_XMLParser.GetAttributeBool("interpret_as_string");
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Save data pools

   \param[in]     orc_DataPools Data pool elements
   \param[in,out] orc_XMLParser XML parser with the "current" element set to the "gui" element
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PuiSdHandlerFilerV2::h_SaveDataPools(const std::vector<C_PuiSdNodeDataPool> & orc_DataPools,
                                            C_OSCXMLParserBase & orc_XMLParser)
{
   orc_XMLParser.CreateAndSelectNodeChild("data-pools");
   orc_XMLParser.SetAttributeUint32("length", orc_DataPools.size());
   for (uint32 u32_ItDataPool = 0; u32_ItDataPool < orc_DataPools.size(); ++u32_ItDataPool)
   {
      orc_XMLParser.CreateAndSelectNodeChild("data-pool");
      orc_XMLParser.SetAttributeUint32("index", u32_ItDataPool);
      h_SaveDataPool(orc_DataPools[u32_ItDataPool], orc_XMLParser);
      //Return
      tgl_assert(orc_XMLParser.SelectNodeParent() == "data-pools");
   }
   //Return
   tgl_assert(orc_XMLParser.SelectNodeParent() == "gui");
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Save data pool

   \param[in]     orc_DataPool  Data pool element
   \param[in,out] orc_XMLParser XML parser with the "current" element set to the "data-pool" element
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PuiSdHandlerFilerV2::h_SaveDataPool(const C_PuiSdNodeDataPool & orc_DataPool, C_OSCXMLParserBase & orc_XMLParser)
{
   orc_XMLParser.CreateAndSelectNodeChild("lists");

   h_SaveDataPoolLists(orc_DataPool.c_DataPoolLists, orc_XMLParser);

   //Return
   tgl_assert(orc_XMLParser.SelectNodeParent() == "data-pool");
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Save data pool lists

   \param[in]     orc_DataPoolLists Data pool lists
   \param[in,out] orc_XMLParser     XML parser with the "current" element set to the "lists" element
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PuiSdHandlerFilerV2::h_SaveDataPoolLists(const std::vector<C_PuiSdNodeDataPoolList> & orc_DataPoolLists,
                                                C_OSCXMLParserBase & orc_XMLParser)
{
   orc_XMLParser.SetAttributeUint32("length", orc_DataPoolLists.size());
   for (uint32 u32_ItDataPoolList = 0; u32_ItDataPoolList < orc_DataPoolLists.size();
        ++u32_ItDataPoolList)
   {
      orc_XMLParser.CreateAndSelectNodeChild("list");
      orc_XMLParser.SetAttributeUint32("index", u32_ItDataPoolList);
      h_SaveDataPoolList(orc_DataPoolLists[u32_ItDataPoolList], orc_XMLParser);
      //Return
      tgl_assert(orc_XMLParser.SelectNodeParent() == "lists");
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Save data pool list

   \param[in]     orc_DataPoolList Data pool list element
   \param[in,out] orc_XMLParser    XML parser with the "current" element set to the "list" element
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PuiSdHandlerFilerV2::h_SaveDataPoolList(const C_PuiSdNodeDataPoolList & orc_DataPoolList,
                                               C_OSCXMLParserBase & orc_XMLParser)
{
   orc_XMLParser.CreateAndSelectNodeChild("data-elements");
   h_SaveDataPoolListElements(orc_DataPoolList.c_DataPoolListElements, orc_XMLParser);
   //Return
   tgl_assert(orc_XMLParser.SelectNodeParent() == "list");
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Save data pool list elements

   \param[in]     orc_DataPoolListElements Data pool list elements
   \param[in,out] orc_XMLParser            XML parser with the "current" element set to the "data-elements" element
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PuiSdHandlerFilerV2::h_SaveDataPoolListElements(
   const std::vector<C_PuiSdNodeDataPoolListElement> & orc_DataPoolListElements, C_OSCXMLParserBase & orc_XMLParser)
{
   orc_XMLParser.SetAttributeUint32("length", orc_DataPoolListElements.size());
   for (uint32 u32_ItDataPoolListElement = 0;
        u32_ItDataPoolListElement < orc_DataPoolListElements.size();
        ++u32_ItDataPoolListElement)
   {
      orc_XMLParser.CreateAndSelectNodeChild("data-element");
      orc_XMLParser.SetAttributeUint32("index", u32_ItDataPoolListElement);
      h_SaveDataPoolListElement(orc_DataPoolListElements[u32_ItDataPoolListElement], orc_XMLParser);
      //Return
      tgl_assert(orc_XMLParser.SelectNodeParent() == "data-elements");
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Save data pool list element

   \param[in]     orc_DataPoolListElement Data pool list element
   \param[in,out] orc_XMLParser           XML parser with the "current" element set to the "data-element" element
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PuiSdHandlerFilerV2::h_SaveDataPoolListElement(const C_PuiSdNodeDataPoolListElement & orc_DataPoolListElement,
                                                      C_OSCXMLParserBase & orc_XMLParser)
{
   orc_XMLParser.SetAttributeBool("auto_min_max_active", orc_DataPoolListElement.q_AutoMinMaxActive);
   orc_XMLParser.SetAttributeBool("interpret_as_string", orc_DataPoolListElement.q_InterpretAsString);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Load can protocols

   \param[in,out] orc_CanProtocols Can protocols (Cleared if necessary)
   \param[in,out] orc_XMLParser    XML parser with the "current" element set to the "gui" element

   \return
   C_NO_ERR    information loaded
   C_CONFIG    error loading information
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_PuiSdHandlerFilerV2::h_LoadCanProtocols(std::vector<C_PuiSdNodeCanProtocol> & orc_CanProtocols,
                                                 C_OSCXMLParserBase & orc_XMLParser)
{
   sint32 s32_Retval = C_NO_ERR;

   //Clear last can protocols
   orc_CanProtocols.clear();
   if (orc_XMLParser.SelectNodeChild("com-protocols") == "com-protocols")
   {
      C_SCLString c_CurrentCanProtocolNode;
      uint32 u32_ExpectedSize = 0UL;
      const bool q_ExpectedSizeHere = orc_XMLParser.AttributeExists("length");

      //Check optional length
      if (q_ExpectedSizeHere == true)
      {
         u32_ExpectedSize = orc_XMLParser.GetAttributeUint32("length");
         orc_CanProtocols.reserve(u32_ExpectedSize);
      }

      c_CurrentCanProtocolNode = orc_XMLParser.SelectNodeChild("com-protocol");
      if (c_CurrentCanProtocolNode == "com-protocol")
      {
         do
         {
            if (s32_Retval == C_NO_ERR)
            {
               C_PuiSdNodeCanProtocol c_CanProtocol;
               s32_Retval = h_LoadCanProtocol(c_CanProtocol, orc_XMLParser);
               if (s32_Retval == C_NO_ERR)
               {
                  orc_CanProtocols.push_back(c_CanProtocol);
               }
            }
            //Next
            c_CurrentCanProtocolNode = orc_XMLParser.SelectNodeNext("com-protocol");
         }
         while (c_CurrentCanProtocolNode == "com-protocol");
         //Return
         tgl_assert(orc_XMLParser.SelectNodeParent() == "com-protocols");
      }
      //Compare length
      if ((s32_Retval == C_NO_ERR) && (q_ExpectedSizeHere == true))
      {
         if (u32_ExpectedSize != orc_CanProtocols.size())
         {
            C_SCLString c_Tmp;
            c_Tmp.PrintFormatted("Unexpected UI protocol count, expected: %i, got %i", u32_ExpectedSize,
                                 orc_CanProtocols.size());
            osc_write_log_warning("Load file", c_Tmp.c_str());
         }
      }
      //Return
      tgl_assert(orc_XMLParser.SelectNodeParent() == "gui");
   }
   else
   {
      s32_Retval = C_CONFIG;
   }

   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Load can protocol

   \param[in,out] orc_CanProtocol Can protocol element
   \param[in,out] orc_XMLParser   XML parser with the "current" element set to the "com-protocol" element

   \return
   C_NO_ERR    information loaded
   C_CONFIG    error loading information
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_PuiSdHandlerFilerV2::h_LoadCanProtocol(C_PuiSdNodeCanProtocol & orc_CanProtocol,
                                                C_OSCXMLParserBase & orc_XMLParser)
{
   sint32 s32_Retval = C_NO_ERR;

   if (orc_XMLParser.SelectNodeChild("com-message-containers") == "com-message-containers")
   {
      s32_Retval = h_LoadCanMessageContainers(orc_CanProtocol.c_ComMessages, orc_XMLParser);
      //Return
      tgl_assert(orc_XMLParser.SelectNodeParent() == "com-protocol");
   }
   else
   {
      s32_Retval = C_CONFIG;
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Load can message containers

   \param[in,out] orc_CanMessageContainers Can message containers
   \param[in,out] orc_XMLParser            XML parser with the "current" element set
                                           to the "com-message-containers" element

   \return
   C_NO_ERR    information loaded
   C_CONFIG    error loading information
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_PuiSdHandlerFilerV2::h_LoadCanMessageContainers(
   std::vector<C_PuiSdNodeCanMessageContainer> & orc_CanMessageContainers, C_OSCXMLParserBase & orc_XMLParser)
{
   sint32 s32_Retval = C_NO_ERR;

   C_SCLString c_CurrentCanMessageContainerNode;
   uint32 u32_ExpectedSize = 0UL;
   const bool q_ExpectedSizeHere = orc_XMLParser.AttributeExists("length");

   //Check optional length
   if (q_ExpectedSizeHere == true)
   {
      u32_ExpectedSize = orc_XMLParser.GetAttributeUint32("length");
      orc_CanMessageContainers.reserve(u32_ExpectedSize);
   }

   c_CurrentCanMessageContainerNode = orc_XMLParser.SelectNodeChild("com-message-container");

   orc_CanMessageContainers.clear();
   if (c_CurrentCanMessageContainerNode == "com-message-container")
   {
      do
      {
         if (s32_Retval == C_NO_ERR)
         {
            C_PuiSdNodeCanMessageContainer c_CanMessageContainer;
            s32_Retval =
               h_LoadCanMessageContainer(c_CanMessageContainer, orc_XMLParser);
            if (s32_Retval == C_NO_ERR)
            {
               orc_CanMessageContainers.push_back(c_CanMessageContainer);
            }
         }
         //Next
         c_CurrentCanMessageContainerNode = orc_XMLParser.SelectNodeNext("com-message-container");
      }
      while (c_CurrentCanMessageContainerNode == "com-message-container");
      //Return
      tgl_assert(orc_XMLParser.SelectNodeParent() == "com-message-containers");
   }
   //Compare length
   if ((s32_Retval == C_NO_ERR) && (q_ExpectedSizeHere == true))
   {
      if (u32_ExpectedSize != orc_CanMessageContainers.size())
      {
         C_SCLString c_Tmp;
         c_Tmp.PrintFormatted("Unexpected data element count, expected: %i, got %i", u32_ExpectedSize,
                              orc_CanMessageContainers.size());
         osc_write_log_warning("Load file", c_Tmp.c_str());
      }
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Load can message container

   \param[in,out] orc_CanMessageContainer Can message container element
   \param[in,out] orc_XMLParser           XML parser with the "current" element set
                                          to the "com-message-container " element

   \return
   C_NO_ERR    information loaded
   C_CONFIG    error loading information
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_PuiSdHandlerFilerV2::h_LoadCanMessageContainer(C_PuiSdNodeCanMessageContainer & orc_CanMessageContainer,
                                                        C_OSCXMLParserBase & orc_XMLParser)
{
   sint32 s32_Retval;

   if (orc_XMLParser.SelectNodeChild("tx-messages") == "tx-messages")
   {
      s32_Retval = h_LoadCanMessages(orc_CanMessageContainer.c_TxMessages, orc_XMLParser);
      //Return
      tgl_assert(orc_XMLParser.SelectNodeParent() == "com-message-container");
   }
   else
   {
      s32_Retval = C_CONFIG;
   }
   if ((orc_XMLParser.SelectNodeChild("rx-messages") == "rx-messages") && (s32_Retval == C_NO_ERR))
   {
      s32_Retval = h_LoadCanMessages(orc_CanMessageContainer.c_RxMessages, orc_XMLParser);
      //Return
      tgl_assert(orc_XMLParser.SelectNodeParent() == "com-message-container");
   }
   else
   {
      s32_Retval = C_CONFIG;
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Load can messages

   \param[in,out] orc_CanMessages Can messages
   \param[in,out] orc_XMLParser   XML parser with the "current" element set to unknown element

   \return
   C_NO_ERR    information loaded
   C_CONFIG    error loading information
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_PuiSdHandlerFilerV2::h_LoadCanMessages(std::vector<C_PuiSdNodeCanMessage> & orc_CanMessages,
                                                C_OSCXMLParserBase & orc_XMLParser)
{
   sint32 s32_Retval = C_NO_ERR;

   C_SCLString c_CurrentCanMessageNode;
   uint32 u32_ExpectedSize = 0UL;
   const bool q_ExpectedSizeHere = orc_XMLParser.AttributeExists("length");

   //Check optional length
   if (q_ExpectedSizeHere == true)
   {
      u32_ExpectedSize = orc_XMLParser.GetAttributeUint32("length");
      orc_CanMessages.reserve(u32_ExpectedSize);
   }

   c_CurrentCanMessageNode = orc_XMLParser.SelectNodeChild("com-message");

   orc_CanMessages.clear();
   if (c_CurrentCanMessageNode == "com-message")
   {
      do
      {
         if (s32_Retval == C_NO_ERR)
         {
            C_PuiSdNodeCanMessage c_CanMessage;
            s32_Retval = h_LoadCanMessage(c_CanMessage, orc_XMLParser);
            if (s32_Retval == C_NO_ERR)
            {
               orc_CanMessages.push_back(c_CanMessage);
            }
         }
         //Next
         c_CurrentCanMessageNode = orc_XMLParser.SelectNodeNext("com-message");
      }
      while (c_CurrentCanMessageNode == "com-message");
      //Return
      orc_XMLParser.SelectNodeParent();
   }
   //Compare length
   if ((s32_Retval == C_NO_ERR) && (q_ExpectedSizeHere == true))
   {
      if (u32_ExpectedSize != orc_CanMessages.size())
      {
         C_SCLString c_Tmp;
         c_Tmp.PrintFormatted("Unexpected UI messages count, expected: %i, got %i", u32_ExpectedSize,
                              orc_CanMessages.size());
         osc_write_log_warning("Load file", c_Tmp.c_str());
      }
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Load can message

   \param[in,out] orc_CanMessage Can message element
   \param[in,out] orc_XMLParser  XML parser with the "current" element set to the "com-message" element

   \return
   C_NO_ERR    information loaded
   C_CONFIG    error loading information
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_PuiSdHandlerFilerV2::h_LoadCanMessage(C_PuiSdNodeCanMessage & orc_CanMessage,
                                               C_OSCXMLParserBase & orc_XMLParser)
{
   sint32 s32_Retval = C_NO_ERR;

   //Attributes
   if (orc_XMLParser.AttributeExists("use-auto-receive-timeout") == true)
   {
      orc_CanMessage.q_UseAutoReceiveTimeout = orc_XMLParser.GetAttributeBool("use-auto-receive-timeout");
   }
   else
   {
      orc_CanMessage.q_UseAutoReceiveTimeout = true;
   }

   //Signals
   if (orc_XMLParser.SelectNodeChild("com-signals") == "com-signals")
   {
      h_LoadCanSignals(orc_CanMessage.c_Signals, orc_XMLParser);
      //Return
      tgl_assert(orc_XMLParser.SelectNodeParent() == "com-message");
   }
   else
   {
      s32_Retval = C_CONFIG;
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Load can signals

   \param[in,out] orc_CanSignals Can signals
   \param[in,out] orc_XMLParser  XML parser with the "current" element set to the "com-signals" element

   \return
   C_NO_ERR    information loaded
   C_CONFIG    error loading information
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PuiSdHandlerFilerV2::h_LoadCanSignals(std::vector<C_PuiSdNodeCanSignal> & orc_CanSignals,
                                             C_OSCXMLParserBase & orc_XMLParser)
{
   C_SCLString c_CurrentDataPoolListElementNode;
   uint32 u32_ExpectedSize = 0UL;
   const bool q_ExpectedSizeHere = orc_XMLParser.AttributeExists("length");

   //Check optional length
   if (q_ExpectedSizeHere == true)
   {
      u32_ExpectedSize = orc_XMLParser.GetAttributeUint32("length");
      orc_CanSignals.reserve(u32_ExpectedSize);
   }

   c_CurrentDataPoolListElementNode = orc_XMLParser.SelectNodeChild("com-signal");

   if (c_CurrentDataPoolListElementNode == "com-signal")
   {
      do
      {
         C_PuiSdNodeCanSignal c_CanSignal;
         h_LoadCanSignal(c_CanSignal, orc_XMLParser);
         orc_CanSignals.push_back(c_CanSignal);

         //Next
         c_CurrentDataPoolListElementNode = orc_XMLParser.SelectNodeNext("com-signal");
      }
      while (c_CurrentDataPoolListElementNode == "com-signal");
      //Return
      tgl_assert(orc_XMLParser.SelectNodeParent() == "com-signals");
   }
   //Compare length
   if (q_ExpectedSizeHere == true)
   {
      if (u32_ExpectedSize != orc_CanSignals.size())
      {
         C_SCLString c_Tmp;
         c_Tmp.PrintFormatted("Unexpected data element count, expected: %i, got %i", u32_ExpectedSize,
                              orc_CanSignals.size());
         osc_write_log_warning("Load file", c_Tmp.c_str());
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Load can signal element

   \param[in,out] orc_CanSignal Can signal element
   \param[in,out] orc_XMLParser XML parser with the "current" element set to the "com-signal" element

   \return
   C_NO_ERR    information loaded
   C_CONFIG    error loading information
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PuiSdHandlerFilerV2::h_LoadCanSignal(C_PuiSdNodeCanSignal & orc_CanSignal,
                                            const C_OSCXMLParserBase & orc_XMLParser)
{
   orc_CanSignal.u8_ColorIndex = static_cast<uint8>(orc_XMLParser.GetAttributeUint32("color-index"));
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Save can protocols

   \param[in]     orc_CanProtocols Can protocols
   \param[in,out] orc_XMLParser    XML parser with the "current" element set to the "gui" element
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PuiSdHandlerFilerV2::h_SaveCanProtocols(const std::vector<C_PuiSdNodeCanProtocol> & orc_CanProtocols,
                                               C_OSCXMLParserBase & orc_XMLParser)
{
   orc_XMLParser.CreateAndSelectNodeChild("com-protocols");
   orc_XMLParser.SetAttributeUint32("length", orc_CanProtocols.size());
   for (uint32 u32_ItCanProtocol = 0; u32_ItCanProtocol < orc_CanProtocols.size(); ++u32_ItCanProtocol)
   {
      orc_XMLParser.CreateAndSelectNodeChild("com-protocol");
      h_SaveCanProtocol(orc_CanProtocols[u32_ItCanProtocol], orc_XMLParser);
      //Return
      tgl_assert(orc_XMLParser.SelectNodeParent() == "com-protocols");
   }
   //Return
   tgl_assert(orc_XMLParser.SelectNodeParent() == "gui");
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Save can protocol

   \param[in]     orc_CanProtocol Can protocol
   \param[in,out] orc_XMLParser   XML parser with the "current" element set to the "com-protocol" element
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PuiSdHandlerFilerV2::h_SaveCanProtocol(const C_PuiSdNodeCanProtocol & orc_CanProtocol,
                                              C_OSCXMLParserBase & orc_XMLParser)
{
   orc_XMLParser.CreateAndSelectNodeChild("com-message-containers");

   h_SaveCanMessageContainers(orc_CanProtocol.c_ComMessages, orc_XMLParser);

   //Return
   tgl_assert(orc_XMLParser.SelectNodeParent() == "com-protocol");
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Save can message containers

   \param[in]     orc_CanMessageContainers Can message containers
   \param[in,out] orc_XMLParser            XML parser with the "current" element set
                                           to the "com-message-containers" element
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PuiSdHandlerFilerV2::h_SaveCanMessageContainers(
   const std::vector<C_PuiSdNodeCanMessageContainer> & orc_CanMessageContainers, C_OSCXMLParserBase & orc_XMLParser)
{
   orc_XMLParser.SetAttributeUint32("length", orc_CanMessageContainers.size());
   for (uint32 u32_ItCanMessageContainer = 0; u32_ItCanMessageContainer < orc_CanMessageContainers.size();
        ++u32_ItCanMessageContainer)
   {
      orc_XMLParser.CreateAndSelectNodeChild("com-message-container");
      h_SaveCanMessageContainer(orc_CanMessageContainers[u32_ItCanMessageContainer], orc_XMLParser);
      //Return
      tgl_assert(orc_XMLParser.SelectNodeParent() == "com-message-containers");
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Save can message container element

   \param[in]     orc_CanMessageContainer Can message container element
   \param[in,out] orc_XMLParser           XML parser with the "current" element set
                                          to the "com-message-container" element
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PuiSdHandlerFilerV2::h_SaveCanMessageContainer(const C_PuiSdNodeCanMessageContainer & orc_CanMessageContainer,
                                                      C_OSCXMLParserBase & orc_XMLParser)
{
   orc_XMLParser.CreateAndSelectNodeChild("tx-messages");
   h_SaveCanMessages(orc_CanMessageContainer.c_TxMessages, orc_XMLParser);
   //Return
   tgl_assert(orc_XMLParser.SelectNodeParent() == "com-message-container");
   orc_XMLParser.CreateAndSelectNodeChild("rx-messages");
   h_SaveCanMessages(orc_CanMessageContainer.c_RxMessages, orc_XMLParser);
   //Return
   tgl_assert(orc_XMLParser.SelectNodeParent() == "com-message-container");
}
//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Save can messages

   \param[in]     orc_CanMessages Can messages
   \param[in,out] orc_XMLParser   XML parser with the "current" element set to unknown element
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PuiSdHandlerFilerV2::h_SaveCanMessages(const std::vector<C_PuiSdNodeCanMessage> & orc_CanMessages,
                                              C_OSCXMLParserBase & orc_XMLParser)
{
   orc_XMLParser.SetAttributeUint32("length", orc_CanMessages.size());
   for (uint32 u32_ItMessage = 0; u32_ItMessage < orc_CanMessages.size();
        ++u32_ItMessage)
   {
      orc_XMLParser.CreateAndSelectNodeChild("com-message");
      h_SaveCanMessage(orc_CanMessages[u32_ItMessage], orc_XMLParser);
      //Return
      orc_XMLParser.SelectNodeParent();
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Save can message element

   \param[in]     orc_CanMessage Can message element
   \param[in,out] orc_XMLParser  XML parser with the "current" element set to the "com-message" element
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PuiSdHandlerFilerV2::h_SaveCanMessage(const C_PuiSdNodeCanMessage & orc_CanMessage,
                                             C_OSCXMLParserBase & orc_XMLParser)
{
   //Attributes
   orc_XMLParser.SetAttributeBool("use-auto-receive-timeout", orc_CanMessage.q_UseAutoReceiveTimeout);
   //Signals
   orc_XMLParser.CreateAndSelectNodeChild("com-signals");
   h_SaveCanSignals(orc_CanMessage.c_Signals, orc_XMLParser);
   //Return
   tgl_assert(orc_XMLParser.SelectNodeParent() == "com-message");
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Save can signals

   \param[in]     orc_CanSignals Can signals
   \param[in,out] orc_XMLParser  XML parser with the "current" element set to the "com-signals" element
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PuiSdHandlerFilerV2::h_SaveCanSignals(const std::vector<C_PuiSdNodeCanSignal> & orc_CanSignals,
                                             C_OSCXMLParserBase & orc_XMLParser)
{
   orc_XMLParser.SetAttributeUint32("length", orc_CanSignals.size());
   for (uint32 u32_ItDataPoolListElement = 0;
        u32_ItDataPoolListElement < orc_CanSignals.size();
        ++u32_ItDataPoolListElement)
   {
      orc_XMLParser.CreateAndSelectNodeChild("com-signal");
      h_SaveCanSignal(orc_CanSignals[u32_ItDataPoolListElement], orc_XMLParser);
      //Return
      tgl_assert(orc_XMLParser.SelectNodeParent() == "com-signals");
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Save can signal element

   \param[in]     orc_CanSignal Can signal element
   \param[in,out] orc_XMLParser XML parser with the "current" element set to the "com-signal" element
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PuiSdHandlerFilerV2::h_SaveCanSignal(const C_PuiSdNodeCanSignal & orc_CanSignal,
                                            C_OSCXMLParserBase & orc_XMLParser)
{
   orc_XMLParser.SetAttributeUint32("color-index", orc_CanSignal.u8_ColorIndex);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Load UI nodes

   Load UI node information from XML structure.
   Also uses base class function to load basic information.
   Postcondition: XMLParser has the node "nodes" selected.

   \param[out]     orc_Nodes      UI node data containers
   \param[in,out]  orc_XMLParser  XMLParser with the "current" element set to the "nodes" element
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_PuiSdHandlerFilerV2::h_LoadNodes(std::vector<C_PuiSdNode> & orc_Nodes, C_OSCXMLParserBase & orc_XMLParser)
{
   sint32 s32_Retval = C_NO_ERR;
   C_SCLString c_SelectedNode;
   uint32 u32_ExpectedSize = 0UL;
   const bool q_ExpectedSizeHere = orc_XMLParser.AttributeExists("length");

   //Check optional length
   if (q_ExpectedSizeHere == true)
   {
      u32_ExpectedSize = orc_XMLParser.GetAttributeUint32("length");
      orc_Nodes.reserve(u32_ExpectedSize);
   }

   c_SelectedNode = orc_XMLParser.SelectNodeChild("node");

   orc_Nodes.clear();
   if (c_SelectedNode == "node")
   {
      do
      {
         C_PuiSdNode c_Node;
         s32_Retval = mh_LoadNode(c_Node, orc_XMLParser);
         if (s32_Retval == C_NO_ERR)
         {
            orc_Nodes.push_back(c_Node);
         }

         //Next
         c_SelectedNode = orc_XMLParser.SelectNodeNext("node");
      }
      while (c_SelectedNode == "node");
      //Return
      tgl_assert(orc_XMLParser.SelectNodeParent() == "nodes");
   }
   //Compare length
   if ((s32_Retval == C_NO_ERR) && (q_ExpectedSizeHere == true))
   {
      if (u32_ExpectedSize != orc_Nodes.size())
      {
         C_SCLString c_Tmp;
         c_Tmp.PrintFormatted("Unexpected UI nodes count, expected: %i, got %i", u32_ExpectedSize,
                              orc_Nodes.size());
         osc_write_log_warning("Load file", c_Tmp.c_str());
      }
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Save UI nodes

   Save UI node information to XML structure
   Also uses base class function to save basic information.
   Postcondition: XMLParser has the node "nodes" selected.

   \param[in]      orc_Nodes      UI node data containers
   \param[in,out]  orc_XMLParser  XMLParser with the "current" element set to the "nodes" element
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PuiSdHandlerFilerV2::h_SaveNodes(const std::vector<C_PuiSdNode> & orc_Nodes, C_OSCXMLParserBase & orc_XMLParser)
{
   if (orc_Nodes.size() > 0)
   {
      orc_XMLParser.SetAttributeUint32("length", orc_Nodes.size());
      tgl_assert(orc_XMLParser.SelectNodeChild("node") == "node"); //first node ...
      for (uint32 u32_Index = 0U; u32_Index < orc_Nodes.size(); u32_Index++)
      {
         C_PuiSdHandlerFilerV2::mh_SaveNode(orc_Nodes[u32_Index], orc_XMLParser);
         if (u32_Index < (orc_Nodes.size() - 1UL))
         {
            tgl_assert(orc_XMLParser.SelectNodeNext("node") == "node"); //next node
         }
      }
      tgl_assert(orc_XMLParser.SelectNodeParent() == "nodes"); //back up
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Load GUI buses

   Load GUI-specific information about bus.
   Also uses base class function to load basic information.

   \param[in,out]  orc_Buses      UI bus data containers
   \param[in,out]  orc_XMLParser  XML parser with the "current" element set to the "buses" element

   \return
   C_NO_ERR    information loaded
   C_CONFIG    error loading information
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_PuiSdHandlerFilerV2::h_LoadBuses(std::vector<C_PuiSdBus> & orc_Buses, C_OSCXMLParserBase & orc_XMLParser)
{
   sint32 s32_Retval = C_NO_ERR;
   C_SCLString c_SelectedNode;
   uint32 u32_ExpectedSize = 0UL;
   const bool q_ExpectedSizeHere = orc_XMLParser.AttributeExists("length");

   //Check optional length
   if (q_ExpectedSizeHere == true)
   {
      u32_ExpectedSize = orc_XMLParser.GetAttributeUint32("length");
      orc_Buses.reserve(u32_ExpectedSize);
   }

   c_SelectedNode = orc_XMLParser.SelectNodeChild("bus");

   orc_Buses.clear();
   if (c_SelectedNode == "bus")
   {
      do
      {
         C_PuiSdBus c_Bus;
         s32_Retval = C_PuiSdHandlerFilerV2::mh_LoadBus(c_Bus, orc_XMLParser);
         if (s32_Retval == C_NO_ERR)
         {
            orc_Buses.push_back(c_Bus);
         }

         //Next
         c_SelectedNode = orc_XMLParser.SelectNodeNext("bus");
      }
      while (c_SelectedNode == "bus");
      //Return
      tgl_assert(orc_XMLParser.SelectNodeParent() == "buses");
   }
   //Compare length
   if ((s32_Retval == C_NO_ERR) && (q_ExpectedSizeHere == true))
   {
      if (u32_ExpectedSize != orc_Buses.size())
      {
         C_SCLString c_Tmp;
         c_Tmp.PrintFormatted("Unexpected bus count, expected: %i, got %i", u32_ExpectedSize,
                              orc_Buses.size());
         osc_write_log_warning("Load file", c_Tmp.c_str());
      }
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Save GUI buses

   Save GUI-specific information about bus.
   Also uses base class function to save basic information.

   \param[in]      orc_Buses      UI bus data containers
   \param[in,out]  orc_XMLParser  XML parser with the "current" element set to the "buses" element

   \return
   C_NO_ERR    information loaded
   C_CONFIG    error loading information
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PuiSdHandlerFilerV2::h_SaveBuses(const std::vector<C_PuiSdBus> & orc_Buses, C_OSCXMLParserBase & orc_XMLParser)
{
   if (orc_Buses.size() > 0)
   {
      orc_XMLParser.SetAttributeUint32("length", orc_Buses.size());
      tgl_assert(orc_XMLParser.SelectNodeChild("bus") == "bus"); //first bus ...
      for (uint32 u32_Index = 0U; u32_Index < orc_Buses.size(); u32_Index++)
      {
         C_PuiSdHandlerFilerV2::mh_SaveBus(orc_Buses[u32_Index], orc_XMLParser);
         if (u32_Index < (orc_Buses.size() - 1UL))
         {
            orc_XMLParser.SelectNodeNext("bus"); //next bus
         }
      }
      tgl_assert(orc_XMLParser.SelectNodeParent() == "buses"); //back up
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Load text elements

   \param[in,out] orc_BusTextElements Text element data elements
   \param[in,out] orc_XMLParser       XML parser with the "current" element set to the "bus-text-elements" element

   \return
   C_NO_ERR    information loaded
   C_CONFIG    error loading information
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_PuiSdHandlerFilerV2::h_LoadBusTextElements(std::vector<C_PuiSdTextElementBus> & orc_BusTextElements,
                                                    C_OSCXMLParserBase & orc_XMLParser)
{
   sint32 s32_Retval = C_NO_ERR;
   C_SCLString c_SelectedNode;
   uint32 u32_ExpectedSize = 0UL;
   const bool q_ExpectedSizeHere = orc_XMLParser.AttributeExists("length");

   //Check optional length
   if (q_ExpectedSizeHere == true)
   {
      u32_ExpectedSize = orc_XMLParser.GetAttributeUint32("length");
      orc_BusTextElements.reserve(u32_ExpectedSize);
   }

   c_SelectedNode = orc_XMLParser.SelectNodeChild("bus-text-element");

   orc_BusTextElements.clear();
   if (c_SelectedNode == "bus-text-element")
   {
      do
      {
         C_PuiSdTextElementBus c_TextElement;
         s32_Retval = C_PuiSdHandlerFilerV2::mh_LoadTextElement(&c_TextElement, orc_XMLParser);
         if (s32_Retval == C_NO_ERR)
         {
            orc_BusTextElements.push_back(c_TextElement);
         }

         //Next
         c_SelectedNode = orc_XMLParser.SelectNodeNext("bus-text-element");
      }
      while (c_SelectedNode == "bus-text-element");
      //Return
      tgl_assert(orc_XMLParser.SelectNodeParent() == "bus-text-elements");
   }
   //Compare length
   if ((s32_Retval == C_NO_ERR) && (q_ExpectedSizeHere == true))
   {
      if (u32_ExpectedSize != orc_BusTextElements.size())
      {
         C_SCLString c_Tmp;
         c_Tmp.PrintFormatted("Unexpected bus text element count, expected: %i, got %i", u32_ExpectedSize,
                              orc_BusTextElements.size());
         osc_write_log_warning("Load file", c_Tmp.c_str());
      }
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Save text elements

   \param[in]     orc_BusTextElements Text element data elements
   \param[in,out] orc_XMLParser       XML parser with the "current" element set to the "bus-text-elements" element
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PuiSdHandlerFilerV2::h_SaveBusTextElements(const std::vector<C_PuiSdTextElementBus> & orc_BusTextElements,
                                                  C_OSCXMLParserBase & orc_XMLParser)
{
   orc_XMLParser.SetAttributeUint32("length", orc_BusTextElements.size());
   for (uint32 u32_Index = 0U; u32_Index < orc_BusTextElements.size(); ++u32_Index)
   {
      orc_XMLParser.CreateAndSelectNodeChild("bus-text-element");
      C_PuiSdHandlerFilerV2::mh_SaveTextElement(&orc_BusTextElements[u32_Index], orc_XMLParser);
      tgl_assert(orc_XMLParser.SelectNodeParent() == "bus-text-elements"); //back up
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Load UI node information

   Load UI node information from XML structure.
   Also uses base class function to load basic information.
   Postcondition: XMLParser has the node "node" selected.

   \param[out]     orc_Node       UI node data container
   \param[in,out]  orc_XMLParser  XMLParser with the "current" element set to the "node" element
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_PuiSdHandlerFilerV2::mh_LoadNode(C_PuiSdNode & orc_Node, C_OSCXMLParserBase & orc_XMLParser)
{
   sint32 s32_Return = C_NO_ERR;

   //GUI information is stored in "gui" subnode
   if (orc_XMLParser.SelectNodeChild("gui") == "gui")
   {
      if (orc_XMLParser.SelectNodeChild("box") == "box")
      {
         s32_Return = C_PuiBsElementsFiler::h_LoadBoxBase(orc_Node, orc_XMLParser);
         orc_XMLParser.SelectNodeParent(); //back up to "gui"
      }
      if (s32_Return == C_NO_ERR)
      {
         s32_Return = h_LoadCanProtocols(orc_Node.c_UICanProtocols, orc_XMLParser);
         if (s32_Return == C_NO_ERR)
         {
            s32_Return = h_LoadDataPools(orc_Node.c_UIDataPools, orc_XMLParser);

            if ((s32_Return == C_NO_ERR) && (orc_XMLParser.SelectNodeChild("busconnections") == "busconnections"))
            {
               orc_Node.c_UIBusConnections.clear();

               //go through list:
               if (orc_XMLParser.SelectNodeChild("connection") == "connection")
               {
                  do
                  {
                     //check type and busnumber just to verify the sequence matches the one in core:
                     C_PuiSdNodeConnectionId c_Connection;
                     C_SCLString c_Text;
                     c_Text = orc_XMLParser.GetAttributeString("type");
                     if (c_Text == "ethernet")
                     {
                        c_Connection.e_InterfaceType = C_OSCSystemBus::eETHERNET;
                     }
                     else if (c_Text == "can")
                     {
                        c_Connection.e_InterfaceType = C_OSCSystemBus::eCAN;
                     }
                     else
                     {
                        s32_Return = C_CONFIG;
                     }
                     if (s32_Return == C_NO_ERR)
                     {
                        c_Connection.u8_InterfaceNumber =
                           static_cast<uint8>(orc_XMLParser.GetAttributeUint32("busnumber"));

                        {
                           C_PuiSdNodeConnection c_UIConnection;

                           //Set ID
                           c_UIConnection.c_ConnectionID = c_Connection;

                           //sequence OK; now get points:
                           if (orc_XMLParser.SelectNodeChild("interaction-points") == "interaction-points")
                           {
                              //file through children:
                              c_Text = orc_XMLParser.SelectNodeChild("interaction-point");

                              if (c_Text == "interaction-point")
                              {
                                 do
                                 {
                                    QPointF c_Point(orc_XMLParser.GetAttributeFloat64(
                                                       "x"), orc_XMLParser.GetAttributeFloat64(
                                                       "y"));
                                    c_UIConnection.c_UINodeConnectionInteractionPoints.push_back(c_Point);
                                    c_Text = orc_XMLParser.SelectNodeNext("interaction-point");
                                 }
                                 while (c_Text == "interaction-point");
                                 tgl_assert(orc_XMLParser.SelectNodeParent() == "interaction-points");
                              }

                              orc_Node.c_UIBusConnections.push_back(c_UIConnection);
                              tgl_assert(orc_XMLParser.SelectNodeParent() == "connection");
                           }
                           else
                           {
                              s32_Return = C_CONFIG;
                           }
                        }
                     }
                  }
                  while ((s32_Return == C_NO_ERR) && (orc_XMLParser.SelectNodeNext("connection") == "connection"));
                  //any more connections ?

                  tgl_assert(orc_XMLParser.SelectNodeParent() == "busconnections"); //back to busconnections
               }
               orc_XMLParser.SelectNodeParent(); //back to gui
               orc_XMLParser.SelectNodeParent(); //back to node
            }
            else
            {
               s32_Return = C_CONFIG;
            }
         }
      }
   }
   else
   {
      s32_Return = C_CONFIG;
   }

   //also load information for base class:
   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Save UI node information

   Save UI node information to XML structure
   Also uses base class function to save basic information.
   Postcondition: XMLParser has the node "node" selected.

   \param[in]      orc_Node       UI node data container
   \param[in,out]  orc_XMLParser  XMLParser with the "current" element set to the "node" element
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PuiSdHandlerFilerV2::mh_SaveNode(const C_PuiSdNode & orc_Node, C_OSCXMLParserBase & orc_XMLParser)
{
   //GUI information is stored in "gui" subnode
   orc_XMLParser.CreateAndSelectNodeChild("gui");

   orc_XMLParser.CreateAndSelectNodeChild("box");
   C_PuiBsElementsFiler::h_SaveBoxBase(orc_Node, orc_XMLParser);
   orc_XMLParser.SelectNodeParent();

   h_SaveCanProtocols(orc_Node.c_UICanProtocols, orc_XMLParser);
   h_SaveDataPools(orc_Node.c_UIDataPools, orc_XMLParser);

   orc_XMLParser.CreateAndSelectNodeChild("busconnections");
   for (uint16 u16_Bus = 0U; u16_Bus < orc_Node.c_UIBusConnections.size(); u16_Bus++)
   {
      const C_PuiSdNodeConnection & rc_Bus = orc_Node.c_UIBusConnections[u16_Bus];

      orc_XMLParser.CreateAndSelectNodeChild("connection");

      if (rc_Bus.c_ConnectionID.e_InterfaceType == C_OSCSystemBus::eCAN)
      {
         orc_XMLParser.SetAttributeString("type", "can");
      }
      else if (rc_Bus.c_ConnectionID.e_InterfaceType == C_OSCSystemBus::eETHERNET)
      {
         orc_XMLParser.SetAttributeString("type", "ethernet");
      }
      else
      {
         //weird ...
      }
      orc_XMLParser.SetAttributeUint32("busnumber", rc_Bus.c_ConnectionID.u8_InterfaceNumber);

      orc_XMLParser.CreateAndSelectNodeChild("interaction-points");

      for (uint16 u16_Point = 0U;
           u16_Point < rc_Bus.c_UINodeConnectionInteractionPoints.size(); u16_Point++)
      {
         orc_XMLParser.CreateAndSelectNodeChild("interaction-point");
         orc_XMLParser.SetAttributeFloat64("x", rc_Bus.c_UINodeConnectionInteractionPoints[u16_Point].x());
         orc_XMLParser.SetAttributeFloat64("y", rc_Bus.c_UINodeConnectionInteractionPoints[u16_Point].y());
         orc_XMLParser.SelectNodeParent(); //back to interaction-points
      }

      tgl_assert(orc_XMLParser.SelectNodeParent() == "connection");     //back to connection
      tgl_assert(orc_XMLParser.SelectNodeParent() == "busconnections"); //back to busconnections
   }

   tgl_assert(orc_XMLParser.SelectNodeParent() == "gui");  //back to "gui"
   tgl_assert(orc_XMLParser.SelectNodeParent() == "node"); //back to "node"
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Load GUI bus information

   Load GUI-specific information about bus.
   Also uses base class function to load basic information.

   \param[in,out]  orc_Bus        UI bus data container
   \param[in,out]  orc_XMLParser  XML parser with the "current" element set to the "bus" element

   \return
   C_NO_ERR    information loaded
   C_CONFIG    error loading information
*/
//----------------------------------------------------------------------------------------------------------------------
stw_types::sint32 C_PuiSdHandlerFilerV2::mh_LoadBus(C_PuiSdBus & orc_Bus, C_OSCXMLParserBase & orc_XMLParser)
{
   sint32 s32_Return = C_NO_ERR;

   //GUI information is stored in "gui" subnode
   if (orc_XMLParser.SelectNodeChild("gui") == "gui")
   {
      if (orc_XMLParser.SelectNodeChild("line") == "line")
      {
         s32_Return = C_PuiBsElementsFiler::h_LoadLineBase(orc_Bus, orc_XMLParser);
      }
      else
      {
         s32_Return = C_CONFIG;
      }
      if (orc_XMLParser.SelectNodeNext("color-middle-line") == "color-middle-line")
      {
         C_PuiBsElementsFiler::h_LoadColor(orc_Bus.c_UIColorMiddleLine, orc_XMLParser);
      }
      else
      {
         s32_Return = C_CONFIG;
      }

      orc_XMLParser.SelectNodeParent(); //back to gui
      orc_XMLParser.SelectNodeParent(); //back to bus
   }
   else
   {
      s32_Return = C_CONFIG;
   }

   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Save GUI bus information

   Save GUI-specific information about bus.
   Also uses base class function to save basic information.

   \param[in]      orc_Bus        UI bus data container
   \param[in,out]  orc_XMLParser  XML parser with the "current" element set to the "bus" element

   \return
   C_NO_ERR    information loaded
   C_CONFIG    error loading information
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PuiSdHandlerFilerV2::mh_SaveBus(const C_PuiSdBus & orc_Bus, C_OSCXMLParserBase & orc_XMLParser)
{
   //GUI information is stored in "gui" subnode
   orc_XMLParser.CreateAndSelectNodeChild("gui");
   orc_XMLParser.CreateAndSelectNodeChild("line");
   C_PuiBsElementsFiler::h_SaveLineBase(orc_Bus, orc_XMLParser);
   orc_XMLParser.SelectNodeParent(); //back to "gui"

   orc_XMLParser.CreateAndSelectNodeChild("color-middle-line");
   C_PuiBsElementsFiler::h_SaveColor(orc_Bus.c_UIColorMiddleLine, orc_XMLParser);

   orc_XMLParser.SelectNodeParent(); //back to "gui"
   orc_XMLParser.SelectNodeParent(); //back to "bus"
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Load text element attributes

   \param[in,out] opc_TextElement Text element data element
   \param[in,out] orc_XMLParser   XML parser with the "current" element set to the "text-element" element

   \return
   C_NO_ERR    information loaded
   C_CONFIG    error loading information
*/
//----------------------------------------------------------------------------------------------------------------------
stw_types::sint32 C_PuiSdHandlerFilerV2::mh_LoadTextElement(C_PuiBsTextElement * const opc_TextElement,
                                                            stw_opensyde_core::C_OSCXMLParserBase & orc_XMLParser)
{
   sint32 s32_Return;
   //lint -e{929}  false positive in PC-Lint: allowed by MISRA 5-2-2
   C_PuiSdTextElementBus * const opc_BusTextElement =
      dynamic_cast<C_PuiSdTextElementBus * const>(opc_TextElement);

   s32_Return = C_PuiBsElementsFiler::h_LoadTextElement(opc_TextElement, orc_XMLParser);
   //Check bus
   if (opc_BusTextElement != NULL)
   {
      opc_BusTextElement->u32_BusIndex = orc_XMLParser.GetAttributeUint32("bus-index");
   }
   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Save text element

   \param[in]     opc_TextElement Text element data element
   \param[in,out] orc_XMLParser   XML parser with the "current" element set to the "text-element" element
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PuiSdHandlerFilerV2::mh_SaveTextElement(const C_PuiBsTextElement * const opc_TextElement,
                                               stw_opensyde_core::C_OSCXMLParserBase & orc_XMLParser)
{
   //lint -e{929}  false positive in PC-Lint: allowed by MISRA 5-2-2
   const C_PuiSdTextElementBus * const opc_BusTextElement =
      dynamic_cast<const C_PuiSdTextElementBus * const>(opc_TextElement);

   C_PuiBsElementsFiler::h_SaveTextElement(opc_TextElement, orc_XMLParser);
   //Check bus
   if (opc_BusTextElement != NULL)
   {
      orc_XMLParser.SetAttributeUint32("bus-index", opc_BusTextElement->u32_BusIndex);
   }
}
