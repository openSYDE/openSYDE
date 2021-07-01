//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Project data file handler for can monitor (implementation)

   Project data file handler for can monitor

   \copyright   Copyright 2018 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.h"

#include "stwtypes.h"
#include "stwerrors.h"
#include "C_OSCLoggingHandler.h"
#include "C_CamProHandlerFiler.h"
#include "C_Uti.h"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw_scl;
using namespace stw_types;
using namespace stw_errors;
using namespace stw_opensyde_core;
using namespace stw_opensyde_gui_logic;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

/* -- Global Variables ---------------------------------------------------------------------------------------------- */

/* -- Module Global Variables --------------------------------------------------------------------------------------- */

/* -- Module Global Function Prototypes ----------------------------------------------------------------------------- */

/* -- Implementation ------------------------------------------------------------------------------------------------ */

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Handle technical save operations for project

   \param[in]      orc_Handler      Data
   \param[in,out]  orc_XMLParser    XML parser to store in
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CamProHandlerFiler::h_Save(const C_CamProHandler & orc_Handler, C_OSCXMLParserBase & orc_XMLParser)
{
   orc_XMLParser.CreateAndSelectNodeChild("opensyde-can-monitor-configuration");

   orc_XMLParser.CreateNodeChild("file-version", "1");

   // settings
   orc_XMLParser.CreateAndSelectNodeChild("settings");
   h_SaveSettings(orc_Handler, orc_XMLParser);
   //Return
   tgl_assert(orc_XMLParser.SelectNodeParent() == "opensyde-can-monitor-configuration");

   // messages
   orc_XMLParser.CreateAndSelectNodeChild("messages");
   orc_XMLParser.SetAttributeBool("cyclic-active", orc_Handler.GetCyclicMessageTransmitActive());
   h_SaveMessages(orc_Handler.GetMessages(), orc_XMLParser);
   //Return
   tgl_assert(orc_XMLParser.SelectNodeParent() == "opensyde-can-monitor-configuration");
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Handle technical load operations for project

   \param[out]     orc_Handler      Data
   \param[in,out]  orc_XMLParser    XML parser to load from

   \return
   C_NO_ERR Operation success
   C_CONFIG Operation failure: configuration invalid
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_CamProHandlerFiler::h_Load(C_CamProHandler & orc_Handler, C_OSCXMLParserBase & orc_XMLParser)
{
   sint32 s32_Retval = C_CONFIG;

   if (orc_XMLParser.SelectRoot() == "opensyde-can-monitor-configuration")
   {
      // load settings section
      if (orc_XMLParser.SelectNodeChild("settings") == "settings")
      {
         s32_Retval = h_LoadSettings(orc_Handler, orc_XMLParser);
         //Return
         tgl_assert(orc_XMLParser.SelectNodeParent() == "opensyde-can-monitor-configuration");
      }

      // load messages
      if (orc_XMLParser.SelectNodeChild("messages") == "messages")
      {
         std::vector<C_CamProMessageData> c_Messages;
         if (orc_XMLParser.AttributeExists("cyclic-active"))
         {
            orc_Handler.SetCyclicMessageTransmitActive(orc_XMLParser.GetAttributeBool("cyclic-active"));
         }
         else
         {
            orc_Handler.SetCyclicMessageTransmitActive(true);
         }
         s32_Retval = h_LoadMessages(c_Messages, orc_XMLParser);
         orc_Handler.SetMessages(c_Messages);
         //Return
         tgl_assert(orc_XMLParser.SelectNodeParent() == "opensyde-can-monitor-configuration");
      }
   }

   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Save messages

   \param[in]      orc_Messages     Data
   \param[in,out]  orc_XMLParser    XML parser to load from
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CamProHandlerFiler::h_SaveMessages(const std::vector<C_CamProMessageData> & orc_Messages,
                                          C_OSCXMLParserBase & orc_XMLParser)
{
   orc_XMLParser.SetAttributeUint32("length", orc_Messages.size());
   for (uint32 u32_ItMessage = 0UL; u32_ItMessage < orc_Messages.size(); ++u32_ItMessage)
   {
      orc_XMLParser.CreateAndSelectNodeChild("message");
      C_CamProHandlerFiler::h_SaveMessage(orc_Messages[u32_ItMessage], orc_XMLParser);
      //Return
      orc_XMLParser.SelectNodeParent();
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Save message

   \param[in]      orc_Message      Data
   \param[in,out]  orc_XMLParser    XML parser to load from
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CamProHandlerFiler::h_SaveMessage(const C_CamProMessageData & orc_Message, C_OSCXMLParserBase & orc_XMLParser)
{
   orc_XMLParser.SetAttributeBool("has-valid-hash", orc_Message.q_ContainsValidHash);
   orc_XMLParser.SetAttributeUint32("hash", orc_Message.u32_Hash);
   orc_XMLParser.SetAttributeUint32("ID", orc_Message.u32_Id);
   orc_XMLParser.SetAttributeUint32("DLC", orc_Message.u16_Dlc);
   orc_XMLParser.SetAttributeBool("XTD", orc_Message.q_IsExtended);
   orc_XMLParser.SetAttributeBool("RTR", orc_Message.q_IsRTR);
   orc_XMLParser.SetAttributeUint32("DB1", orc_Message.c_Bytes[0]);
   orc_XMLParser.SetAttributeUint32("DB2", orc_Message.c_Bytes[1]);
   orc_XMLParser.SetAttributeUint32("DB3", orc_Message.c_Bytes[2]);
   orc_XMLParser.SetAttributeUint32("DB4", orc_Message.c_Bytes[3]);
   orc_XMLParser.SetAttributeUint32("DB5", orc_Message.c_Bytes[4]);
   orc_XMLParser.SetAttributeUint32("DB6", orc_Message.c_Bytes[5]);
   orc_XMLParser.SetAttributeUint32("DB7", orc_Message.c_Bytes[6]);
   orc_XMLParser.SetAttributeUint32("DB8", orc_Message.c_Bytes[7]);
   orc_XMLParser.SetAttributeBool("do-cyclic-trigger", orc_Message.q_DoCyclicTrigger);
   orc_XMLParser.SetAttributeUint32("interval", orc_Message.u32_CyclicTriggerTime);
   orc_XMLParser.SetAttributeUint32("key-press-offset", orc_Message.u32_KeyPressOffset);
   orc_XMLParser.CreateNodeChild("name", orc_Message.c_Name);
   orc_XMLParser.CreateNodeChild("key", orc_Message.c_Key);
   orc_XMLParser.CreateNodeChild("database", orc_Message.c_DataBaseFilePath);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Load messages

   \param[out]     orc_Messages     Data
   \param[in,out]  orc_XMLParser    XML parser to load from

   \return
   C_NO_ERR Operation success
   C_CONFIG Operation failure: configuration invalid
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_CamProHandlerFiler::h_LoadMessages(std::vector<C_CamProMessageData> & orc_Messages,
                                            C_OSCXMLParserBase & orc_XMLParser)
{
   sint32 s32_Retval = C_NO_ERR;
   C_SCLString c_CurrentDataPoolNode;
   uint32 u32_ExpectedSize = 0UL;
   const bool q_ExpectedSizeHere = orc_XMLParser.AttributeExists("length");

   orc_Messages.clear();

   //Check optional length
   if (q_ExpectedSizeHere == true)
   {
      u32_ExpectedSize = orc_XMLParser.GetAttributeUint32("length");
      orc_Messages.reserve(u32_ExpectedSize);
   }

   c_CurrentDataPoolNode = orc_XMLParser.SelectNodeChild("message");
   if (c_CurrentDataPoolNode == "message")
   {
      do
      {
         C_CamProMessageData c_Message;
         s32_Retval = h_LoadMessage(c_Message, orc_XMLParser);
         orc_Messages.push_back(c_Message);
         //Next
         c_CurrentDataPoolNode = orc_XMLParser.SelectNodeNext("message");
      }
      while ((s32_Retval == C_NO_ERR) && (c_CurrentDataPoolNode == "message"));
      //Return
      orc_XMLParser.SelectNodeParent();
   }
   //Compare length
   if ((s32_Retval == C_NO_ERR) && (q_ExpectedSizeHere == true))
   {
      if (u32_ExpectedSize != orc_Messages.size())
      {
         C_SCLString c_Tmp;
         c_Tmp.PrintFormatted("Unexpected messages count, expected: %i, got %i", u32_ExpectedSize,
                              orc_Messages.size());
         osc_write_log_warning("Load file", c_Tmp.c_str());
      }
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Load message

   \param[out]     orc_Message      Data
   \param[in,out]  orc_XMLParser    XML parser to load from

   \return
   C_NO_ERR Operation success
   C_CONFIG Operation failure: configuration invalid
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_CamProHandlerFiler::h_LoadMessage(C_CamProMessageData & orc_Message, C_OSCXMLParserBase & orc_XMLParser)
{
   sint32 s32_Retval = C_NO_ERR;

   if (orc_XMLParser.AttributeExists("has-valid-hash") == true)
   {
      orc_Message.q_ContainsValidHash = orc_XMLParser.GetAttributeBool("has-valid-hash");
   }
   if (orc_XMLParser.AttributeExists("hash") == true)
   {
      orc_Message.u32_Hash = orc_XMLParser.GetAttributeUint32("hash");
   }
   if (orc_XMLParser.AttributeExists("ID") == true)
   {
      orc_Message.u32_Id = orc_XMLParser.GetAttributeUint32("ID");
   }
   else
   {
      s32_Retval = C_CONFIG;
   }
   if (orc_XMLParser.AttributeExists("DLC") == true)
   {
      orc_Message.u16_Dlc = static_cast<uint8>(orc_XMLParser.GetAttributeUint32("DLC"));
   }
   else
   {
      s32_Retval = C_CONFIG;
   }
   if (orc_XMLParser.AttributeExists("XTD") == true)
   {
      orc_Message.q_IsExtended = orc_XMLParser.GetAttributeBool("XTD");
   }
   else
   {
      s32_Retval = C_CONFIG;
   }
   if (orc_XMLParser.AttributeExists("RTR") == true)
   {
      orc_Message.q_IsRTR = orc_XMLParser.GetAttributeBool("RTR");
   }
   else
   {
      s32_Retval = C_CONFIG;
   }
   if (orc_XMLParser.AttributeExists("DB1") == true)
   {
      orc_Message.c_Bytes[0] = static_cast<uint8>(orc_XMLParser.GetAttributeUint32("DB1"));
   }
   else
   {
      s32_Retval = C_CONFIG;
   }
   if (orc_XMLParser.AttributeExists("DB2") == true)
   {
      orc_Message.c_Bytes[1] = static_cast<uint8>(orc_XMLParser.GetAttributeUint32("DB2"));
   }
   else
   {
      s32_Retval = C_CONFIG;
   }
   if (orc_XMLParser.AttributeExists("DB3") == true)
   {
      orc_Message.c_Bytes[2] = static_cast<uint8>(orc_XMLParser.GetAttributeUint32("DB3"));
   }
   else
   {
      s32_Retval = C_CONFIG;
   }
   if (orc_XMLParser.AttributeExists("DB4") == true)
   {
      orc_Message.c_Bytes[3] = static_cast<uint8>(orc_XMLParser.GetAttributeUint32("DB4"));
   }
   else
   {
      s32_Retval = C_CONFIG;
   }
   if (orc_XMLParser.AttributeExists("DB5") == true)
   {
      orc_Message.c_Bytes[4] = static_cast<uint8>(orc_XMLParser.GetAttributeUint32("DB5"));
   }
   else
   {
      s32_Retval = C_CONFIG;
   }
   if (orc_XMLParser.AttributeExists("DB6") == true)
   {
      orc_Message.c_Bytes[5] = static_cast<uint8>(orc_XMLParser.GetAttributeUint32("DB6"));
   }
   else
   {
      s32_Retval = C_CONFIG;
   }
   if (orc_XMLParser.AttributeExists("DB7") == true)
   {
      orc_Message.c_Bytes[6] = static_cast<uint8>(orc_XMLParser.GetAttributeUint32("DB7"));
   }
   else
   {
      s32_Retval = C_CONFIG;
   }
   if (orc_XMLParser.AttributeExists("DB8") == true)
   {
      orc_Message.c_Bytes[7] = static_cast<uint8>(orc_XMLParser.GetAttributeUint32("DB8"));
   }
   else
   {
      s32_Retval = C_CONFIG;
   }
   if (orc_XMLParser.AttributeExists("do-cyclic-trigger") == true)
   {
      orc_Message.q_DoCyclicTrigger = orc_XMLParser.GetAttributeBool("do-cyclic-trigger");
   }
   else
   {
      s32_Retval = C_CONFIG;
   }
   if (orc_XMLParser.AttributeExists("interval") == true)
   {
      orc_Message.u32_CyclicTriggerTime = orc_XMLParser.GetAttributeUint32("interval");
   }
   else
   {
      s32_Retval = C_CONFIG;
   }
   if (orc_XMLParser.AttributeExists("key-press-offset") == true)
   {
      orc_Message.u32_KeyPressOffset = orc_XMLParser.GetAttributeUint32("key-press-offset");
   }
   else
   {
      s32_Retval = C_CONFIG;
   }

   if (orc_XMLParser.SelectNodeChild("name") == "name")
   {
      orc_Message.c_Name = orc_XMLParser.GetNodeContent();
      //Return
      tgl_assert(orc_XMLParser.SelectNodeParent() == "message");
   }
   else
   {
      s32_Retval = C_CONFIG;
   }
   if (orc_XMLParser.SelectNodeChild("key") == "key")
   {
      orc_Message.c_Key = orc_XMLParser.GetNodeContent();
      //Return
      tgl_assert(orc_XMLParser.SelectNodeParent() == "message");
   }
   else
   {
      s32_Retval = C_CONFIG;
   }
   if (orc_XMLParser.SelectNodeChild("database") == "database")
   {
      orc_Message.c_DataBaseFilePath = orc_XMLParser.GetNodeContent();
      //Return
      tgl_assert(orc_XMLParser.SelectNodeParent() == "message");
   }
   else
   {
      s32_Retval = C_CONFIG;
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Save settings section.

   \param[in]      orc_Handler      Data
   \param[in,out]  orc_XMLParser    XML parser to store in
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CamProHandlerFiler::h_SaveSettings(const C_CamProHandler & orc_Handler, C_OSCXMLParserBase & orc_XMLParser)
{
   // CAN DLL configuration
   orc_XMLParser.CreateAndSelectNodeChild("can-dll-path");
   orc_XMLParser.SetAttributeSint32("type", static_cast<sint32>(orc_Handler.GetCANDllType()));
   orc_XMLParser.SetNodeContent(orc_Handler.GetCustomCANDllPath().toStdString().c_str());

   //Return
   orc_XMLParser.SelectNodeParent();

   // Filters
   orc_XMLParser.CreateAndSelectNodeChild("filters");
   orc_XMLParser.SetAttributeBool("active", orc_Handler.GetFilterWidgetEnabled());
   C_CamProHandlerFiler::h_SaveFilters(orc_Handler.GetFilters(), orc_XMLParser);
   //Return
   orc_XMLParser.SelectNodeParent();

   // Databases
   orc_XMLParser.CreateAndSelectNodeChild("databases");
   C_CamProHandlerFiler::h_SaveDatabases(orc_Handler.GetDatabases(), orc_XMLParser);
   //Return
   orc_XMLParser.SelectNodeParent();

   // Logging
   orc_XMLParser.CreateAndSelectNodeChild("logging");
   C_CamProHandlerFiler::h_SaveLoggingConfig(orc_Handler.GetLoggingData(), orc_XMLParser);
   //Return
   orc_XMLParser.SelectNodeParent();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Load settings section.

   \param[out]     orc_Handler      Data
   \param[in,out]  orc_XMLParser    XML parser to load from

   \return
   C_NO_ERR Operation success
   C_CONFIG Operation failure: configuration invalid
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_CamProHandlerFiler::h_LoadSettings(C_CamProHandler & orc_Handler, C_OSCXMLParserBase & orc_XMLParser)
{
   sint32 s32_Return = C_NO_ERR;

   // CAN DLL path
   if (orc_XMLParser.SelectNodeChild("can-dll-path") == "can-dll-path")
   {
      if (orc_XMLParser.AttributeExists("type"))
      {
         orc_Handler.SetCANDllType(static_cast<C_CamProHandler::E_CANDllType>(orc_XMLParser.GetAttributeSint32("type")));
      }
      else
      {
         s32_Return = C_CONFIG;
      }

      orc_Handler.SetCustomCANDllPath(orc_XMLParser.GetNodeContent().c_str());

      //Return
      orc_XMLParser.SelectNodeParent();
   }
   else
   {
      s32_Return = C_CONFIG;
   }

   // Filters
   if ((s32_Return == C_NO_ERR) && (orc_XMLParser.SelectNodeChild("filters") == "filters"))
   {
      if (orc_XMLParser.AttributeExists("active"))
      {
         orc_Handler.SetFilterWidgetEnabled(orc_XMLParser.GetAttributeBool("active"));

         std::vector<C_CamProFilterData> c_Filters;
         s32_Return = h_LoadFilters(c_Filters, orc_XMLParser);
         orc_Handler.SetFilters(c_Filters);

         //Return
         orc_XMLParser.SelectNodeParent();
      }
      else
      {
         s32_Return = C_CONFIG;
      }
   }
   else
   {
      s32_Return = C_CONFIG;
   }

   // Databases
   if ((s32_Return == C_NO_ERR) && (orc_XMLParser.SelectNodeChild("databases") == "databases"))
   {
      std::vector<C_CamProDatabaseData> c_Databases;
      s32_Return = h_LoadDatabases(c_Databases, orc_XMLParser);
      orc_Handler.SetDatabases(c_Databases);

      //Return
      orc_XMLParser.SelectNodeParent();
   }
   else
   {
      s32_Return = C_CONFIG;
   }

   // Logging
   if ((s32_Return == C_NO_ERR) && (orc_XMLParser.SelectNodeChild("logging") == "logging"))
   {
      C_CamProLoggingData c_LoggingData;
      s32_Return = h_LoadLoggingConfig(c_LoggingData, orc_XMLParser);
      orc_Handler.SetLoggingData(c_LoggingData);

      //Return
      orc_XMLParser.SelectNodeParent();
   }
   else
   {
      s32_Return = C_CONFIG;
   }

   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Save filters.

   \param[in]      orc_Filters      Filters data
   \param[in,out]  orc_XMLParser    XML parser to load from
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CamProHandlerFiler::h_SaveFilters(const std::vector<C_CamProFilterData> & orc_Filters,
                                         C_OSCXMLParserBase & orc_XMLParser)
{
   orc_XMLParser.SetAttributeUint32("length", orc_Filters.size());
   for (uint32 u32_ItFilter = 0UL; u32_ItFilter < orc_Filters.size(); ++u32_ItFilter)
   {
      orc_XMLParser.CreateAndSelectNodeChild("filter");
      C_CamProHandlerFiler::h_SaveFilter(orc_Filters[u32_ItFilter], orc_XMLParser);
      //Return
      orc_XMLParser.SelectNodeParent();
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Save filter.

   \param[in]      orc_Filter       Filter data
   \param[in,out]  orc_XMLParser    XML parser to load from
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CamProHandlerFiler::h_SaveFilter(const C_CamProFilterData & orc_Filter, C_OSCXMLParserBase & orc_XMLParser)
{
   orc_XMLParser.SetAttributeBool("active", orc_Filter.q_Enabled);
   orc_XMLParser.CreateNodeChild("name", orc_Filter.c_Name.toStdString().c_str());
   orc_XMLParser.CreateNodeChild("comment", orc_Filter.c_Comment.toStdString().c_str());
   orc_XMLParser.CreateAndSelectNodeChild("filter-items");
   orc_XMLParser.SetAttributeUint32("length", orc_Filter.c_FilterItems.size());

   // filter items
   for (sint32 s32_ItFilterItem = 0UL; s32_ItFilterItem < orc_Filter.c_FilterItems.size(); ++s32_ItFilterItem)
   {
      orc_XMLParser.CreateAndSelectNodeChild("filter-item");
      C_CamProHandlerFiler::h_SaveFilterItem(orc_Filter.c_FilterItems[s32_ItFilterItem], orc_XMLParser);
      //Return
      orc_XMLParser.SelectNodeParent();
   }

   //Return
   orc_XMLParser.SelectNodeParent();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Save filter item.

   \param[in]      orc_FilterItem   Filter item data
   \param[in,out]  orc_XMLParser    XML parser to load from
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CamProHandlerFiler::h_SaveFilterItem(const C_CamProFilterItemData & orc_FilterItem,
                                            C_OSCXMLParserBase & orc_XMLParser)
{
   orc_XMLParser.SetAttributeBool("active", orc_FilterItem.q_Enabled);
   orc_XMLParser.SetAttributeSint32("filter-type", static_cast<sint32>(orc_FilterItem.e_Type));
   orc_XMLParser.SetAttributeBool("pass-filter", orc_FilterItem.q_PassFilter);
   orc_XMLParser.SetAttributeUint32("start-ID", orc_FilterItem.u32_StartId);
   orc_XMLParser.SetAttributeUint32("end-ID", orc_FilterItem.u32_EndId);
   orc_XMLParser.SetAttributeBool("extended-ID", static_cast<bool>(orc_FilterItem.u8_ExtendedId));
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Load filters.

   \param[out]     orc_Filters      Filters data
   \param[in,out]  orc_XMLParser    XML parser to load from

   \return
   C_NO_ERR Operation success
   C_CONFIG Operation failure: configuration invalid
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_CamProHandlerFiler::h_LoadFilters(std::vector<C_CamProFilterData> & orc_Filters,
                                           C_OSCXMLParserBase & orc_XMLParser)
{
   sint32 s32_Retval = C_NO_ERR;
   C_SCLString c_CurrentFilter;
   uint32 u32_ExpectedSize = 0UL;
   const bool q_ExpectedSizeHere = orc_XMLParser.AttributeExists("length");

   orc_Filters.clear();

   //Check optional length
   if (q_ExpectedSizeHere == true)
   {
      u32_ExpectedSize = orc_XMLParser.GetAttributeUint32("length");
      orc_Filters.reserve(u32_ExpectedSize);
   }

   c_CurrentFilter = orc_XMLParser.SelectNodeChild("filter");
   if (c_CurrentFilter == "filter")
   {
      do
      {
         C_CamProFilterData c_Filter;
         s32_Retval = h_LoadFilter(c_Filter, orc_XMLParser);
         orc_Filters.push_back(c_Filter);
         //Next
         c_CurrentFilter = orc_XMLParser.SelectNodeNext("filter");
      }
      while ((s32_Retval == C_NO_ERR) && (c_CurrentFilter == "filter"));
      //Return
      orc_XMLParser.SelectNodeParent();
   }

   //Compare length
   if ((s32_Retval == C_NO_ERR) && (q_ExpectedSizeHere == true))
   {
      if (u32_ExpectedSize != orc_Filters.size())
      {
         C_SCLString c_Tmp;
         c_Tmp.PrintFormatted("Unexpected filters count, expected: %i, got %i", u32_ExpectedSize,
                              orc_Filters.size());
         osc_write_log_warning("Load file", c_Tmp.c_str());
      }
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Load filter.

   \param[out]     orc_Filter       Filter data
   \param[in,out]  orc_XMLParser    XML parser to load from

   \return
   C_NO_ERR Operation success
   C_CONFIG Operation failure: configuration invalid
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_CamProHandlerFiler::h_LoadFilter(C_CamProFilterData & orc_Filter, C_OSCXMLParserBase & orc_XMLParser)
{
   sint32 s32_Retval = C_NO_ERR;
   C_SCLString c_CurrentFilter;
   uint32 u32_ExpectedSize = 0UL;
   bool q_ExpectedSizeHere = false;

   if (orc_XMLParser.AttributeExists("active") == true)
   {
      orc_Filter.q_Enabled = orc_XMLParser.GetAttributeBool("active");
   }
   else
   {
      s32_Retval = C_CONFIG;
   }

   if (orc_XMLParser.SelectNodeChild("name") == "name")
   {
      orc_Filter.c_Name = orc_XMLParser.GetNodeContent().c_str();
      //Return
      tgl_assert(orc_XMLParser.SelectNodeParent() == "filter");
   }
   else
   {
      s32_Retval = C_CONFIG;
   }

   if (orc_XMLParser.SelectNodeChild("comment") == "comment")
   {
      orc_Filter.c_Comment = orc_XMLParser.GetNodeContent().c_str();
      //Return
      tgl_assert(orc_XMLParser.SelectNodeParent() == "filter");
   }
   else
   {
      s32_Retval = C_CONFIG;
   }

   if (orc_XMLParser.SelectNodeChild("filter-items") == "filter-items")
   {
      q_ExpectedSizeHere = orc_XMLParser.AttributeExists("length");

      //Check optional length
      if (q_ExpectedSizeHere == true)
      {
         u32_ExpectedSize = orc_XMLParser.GetAttributeUint32("length");
         orc_Filter.c_FilterItems.reserve(u32_ExpectedSize);
      }

      c_CurrentFilter = orc_XMLParser.SelectNodeChild("filter-item");
      if (c_CurrentFilter == "filter-item")
      {
         do
         {
            C_CamProFilterItemData c_FilterItem;
            s32_Retval = h_LoadFilterItem(c_FilterItem, orc_XMLParser);
            orc_Filter.c_FilterItems.push_back(c_FilterItem);
            //Next
            c_CurrentFilter = orc_XMLParser.SelectNodeNext("filter-item");
         }
         while ((s32_Retval == C_NO_ERR) && (c_CurrentFilter == "filter-item"));
         //Return
         orc_XMLParser.SelectNodeParent();
      }
      //Return
      tgl_assert(orc_XMLParser.SelectNodeParent() == "filter");
   }
   else
   {
      s32_Retval = C_CONFIG;
   }
   //Compare length
   if ((s32_Retval == C_NO_ERR) && (q_ExpectedSizeHere == true))
   {
      if (u32_ExpectedSize != static_cast<uint32>(orc_Filter.c_FilterItems.size()))
      {
         C_SCLString c_Tmp;
         c_Tmp.PrintFormatted("Unexpected filter items count, expected: %i, got %i", u32_ExpectedSize,
                              orc_Filter.c_FilterItems.size());
         osc_write_log_warning("Load file", c_Tmp.c_str());
      }
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Load filter.

   \param[out]     orc_FilterItem   Filter item data
   \param[in,out]  orc_XMLParser    XML parser to load from

   \return
   C_NO_ERR Operation success
   C_CONFIG Operation failure: configuration invalid
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_CamProHandlerFiler::h_LoadFilterItem(C_CamProFilterItemData & orc_FilterItem,
                                              const C_OSCXMLParserBase & orc_XMLParser)
{
   sint32 s32_Retval = C_NO_ERR;

   if (orc_XMLParser.AttributeExists("active") == true)
   {
      orc_FilterItem.q_Enabled = orc_XMLParser.GetAttributeBool("active");
   }
   else
   {
      s32_Retval = C_CONFIG;
   }

   if (orc_XMLParser.AttributeExists("filter-type") == true)
   {
      orc_FilterItem.e_Type =
         static_cast<C_CamProFilterItemData::E_Type>(orc_XMLParser.GetAttributeSint32("filter-type"));
   }
   else
   {
      s32_Retval = C_CONFIG;
   }

   if (orc_XMLParser.AttributeExists("pass-filter") == true)
   {
      orc_FilterItem.q_PassFilter = orc_XMLParser.GetAttributeBool("pass-filter");
   }
   else
   {
      s32_Retval = C_CONFIG;
   }

   if (orc_XMLParser.AttributeExists("start-ID") == true)
   {
      orc_FilterItem.u32_StartId = orc_XMLParser.GetAttributeUint32("start-ID");
   }
   else
   {
      s32_Retval = C_CONFIG;
   }

   if (orc_XMLParser.AttributeExists("end-ID") == true)
   {
      orc_FilterItem.u32_EndId = orc_XMLParser.GetAttributeUint32("end-ID");
   }
   else
   {
      s32_Retval = C_CONFIG;
   }

   if (orc_XMLParser.AttributeExists("extended-ID") == true)
   {
      // extended ID is traditionally uint8, but in fact it is either 0 or 1, i.e. bool
      orc_FilterItem.u8_ExtendedId = static_cast<uint8>(orc_XMLParser.GetAttributeBool("extended-ID"));
   }
   else
   {
      s32_Retval = C_CONFIG;
   }

   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Save databases.

   \param[in]      orc_Databases    Databases data
   \param[in,out]  orc_XMLParser    XML parser to load from
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CamProHandlerFiler::h_SaveDatabases(const std::vector<C_CamProDatabaseData> & orc_Databases,
                                           C_OSCXMLParserBase & orc_XMLParser)
{
   orc_XMLParser.SetAttributeUint32("length", orc_Databases.size());
   for (uint32 u32_ItDatabase = 0UL; u32_ItDatabase < orc_Databases.size(); ++u32_ItDatabase)
   {
      orc_XMLParser.CreateAndSelectNodeChild("database");
      C_CamProHandlerFiler::h_SaveDatabase(orc_Databases[u32_ItDatabase], orc_XMLParser);
      //Return
      orc_XMLParser.SelectNodeParent();
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Save filter.

   \param[in]      orc_Database     Database data
   \param[in,out]  orc_XMLParser    XML parser to load from
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CamProHandlerFiler::h_SaveDatabase(const C_CamProDatabaseData & orc_Database, C_OSCXMLParserBase & orc_XMLParser)
{
   orc_XMLParser.SetAttributeBool("active", orc_Database.q_Enabled);
   orc_XMLParser.SetAttributeSint32("bus-index", orc_Database.s32_BusIndex);
   orc_XMLParser.CreateNodeChild("name", orc_Database.c_Name.toStdString().c_str());
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Load databases.

   \param[out]     orc_Databases    Databases data
   \param[in,out]  orc_XMLParser    XML parser to load from

   \return
   C_NO_ERR Operation success
   C_CONFIG Operation failure: configuration invalid
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_CamProHandlerFiler::h_LoadDatabases(std::vector<C_CamProDatabaseData> & orc_Databases,
                                             C_OSCXMLParserBase & orc_XMLParser)
{
   sint32 s32_Retval = C_NO_ERR;
   C_SCLString c_CurrentDatabase;
   uint32 u32_ExpectedSize = 0UL;
   const bool q_ExpectedSizeHere = orc_XMLParser.AttributeExists("length");

   orc_Databases.clear();

   //Check optional length
   if (q_ExpectedSizeHere == true)
   {
      u32_ExpectedSize = orc_XMLParser.GetAttributeUint32("length");
      orc_Databases.reserve(u32_ExpectedSize);
   }

   c_CurrentDatabase = orc_XMLParser.SelectNodeChild("database");
   if (c_CurrentDatabase == "database")
   {
      do
      {
         C_CamProDatabaseData c_Database;
         s32_Retval = h_LoadDatabase(c_Database, orc_XMLParser);
         orc_Databases.push_back(c_Database);
         //Next
         c_CurrentDatabase = orc_XMLParser.SelectNodeNext("database");
      }
      while ((s32_Retval == C_NO_ERR) && (c_CurrentDatabase == "database"));
      //Return
      orc_XMLParser.SelectNodeParent();
   }

   //Compare length
   if ((s32_Retval == C_NO_ERR) && (q_ExpectedSizeHere == true))
   {
      if (u32_ExpectedSize != orc_Databases.size())
      {
         C_SCLString c_Tmp;
         c_Tmp.PrintFormatted("Unexpected databases count, expected: %i, got %i", u32_ExpectedSize,
                              orc_Databases.size());
         osc_write_log_warning("Load file", c_Tmp.c_str());
      }
   }

   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Load database.

   \param[out]     orc_Database     Database data
   \param[in,out]  orc_XMLParser    XML parser to load from

   \return
   C_NO_ERR Operation success
   C_CONFIG Operation failure: configuration invalid
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_CamProHandlerFiler::h_LoadDatabase(C_CamProDatabaseData & orc_Database, C_OSCXMLParserBase & orc_XMLParser)
{
   sint32 s32_Retval = C_NO_ERR;

   if (orc_XMLParser.AttributeExists("active") == true)
   {
      orc_Database.q_Enabled = orc_XMLParser.GetAttributeBool("active");
   }
   else
   {
      s32_Retval = C_CONFIG;
   }

   if (orc_XMLParser.AttributeExists("bus-index") == true)
   {
      orc_Database.s32_BusIndex = orc_XMLParser.GetAttributeSint32("bus-index");
   }
   else
   {
      s32_Retval = C_CONFIG;
   }

   if (orc_XMLParser.SelectNodeChild("name") == "name")
   {
      orc_Database.c_Name = orc_XMLParser.GetNodeContent().c_str();
      //Return
      tgl_assert(orc_XMLParser.SelectNodeParent() == "database");
   }
   else
   {
      s32_Retval = C_CONFIG;
   }

   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Save settings section.

   \param[in]      orc_LoggingData  Logging data
   \param[in,out]  orc_XMLParser    XML parser to store in
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CamProHandlerFiler::h_SaveLoggingConfig(const C_CamProLoggingData & orc_LoggingData,
                                               C_OSCXMLParserBase & orc_XMLParser)
{
   orc_XMLParser.SetAttributeBool("active", orc_LoggingData.q_Enabled);
   orc_XMLParser.SetAttributeSint32("overwrite-mode", static_cast<sint32>(orc_LoggingData.e_OverwriteMode));
   orc_XMLParser.SetAttributeString("format", C_CamProLoggingData::h_FormatEnumToString(
                                       orc_LoggingData.e_FileFormat).toStdString().c_str());
   orc_XMLParser.CreateNodeChild("directory", orc_LoggingData.c_Directory.toStdString().c_str());
   orc_XMLParser.CreateNodeChild("file-name", orc_LoggingData.c_FileName.toStdString().c_str());
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Load logging configuration.

   \param[out]     orc_LoggingData  Logging data
   \param[in,out]  orc_XMLParser    XML parser to load from

   \return
   C_NO_ERR Operation success
   C_CONFIG Operation failure: configuration invalid
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_CamProHandlerFiler::h_LoadLoggingConfig(C_CamProLoggingData & orc_LoggingData,
                                                 C_OSCXMLParserBase & orc_XMLParser)
{
   sint32 s32_Retval = C_NO_ERR;

   if (orc_XMLParser.AttributeExists("active") == true)
   {
      orc_LoggingData.q_Enabled = orc_XMLParser.GetAttributeBool("active");
   }
   else
   {
      s32_Retval = C_CONFIG;
   }

   if (orc_XMLParser.AttributeExists("overwrite-mode") == true)
   {
      orc_LoggingData.e_OverwriteMode =
         static_cast<C_CamProLoggingData::E_OverwriteMode>(orc_XMLParser.GetAttributeSint32("overwrite-mode"));
   }
   else
   {
      s32_Retval = C_CONFIG;
   }

   if (orc_XMLParser.AttributeExists("format") == true)
   {
      orc_LoggingData.e_FileFormat =
         C_CamProLoggingData::h_FormatStringToEnum(orc_XMLParser.GetAttributeString("format").c_str());
   }
   else
   {
      s32_Retval = C_CONFIG;
   }

   if (orc_XMLParser.SelectNodeChild("directory") == "directory")
   {
      orc_LoggingData.c_Directory = orc_XMLParser.GetNodeContent().c_str();
      //Return
      tgl_assert(orc_XMLParser.SelectNodeParent() == "logging");
   }
   else
   {
      s32_Retval = C_CONFIG;
   }

   if (orc_XMLParser.SelectNodeChild("file-name") == "file-name")
   {
      orc_LoggingData.c_FileName = orc_XMLParser.GetNodeContent().c_str();
      //Return
      tgl_assert(orc_XMLParser.SelectNodeParent() == "logging");
   }
   else
   {
      s32_Retval = C_CONFIG;
   }

   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default constructor
*/
//----------------------------------------------------------------------------------------------------------------------
C_CamProHandlerFiler::C_CamProHandlerFiler(void)
{
}
