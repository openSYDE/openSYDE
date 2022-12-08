//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Project data file handler for can monitor (implementation)

   Project data file handler for can monitor

   \copyright   Copyright 2018 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.hpp"

#include "stwtypes.hpp"
#include "stwerrors.hpp"
#include "C_OscLoggingHandler.hpp"
#include "C_CamProHandlerFiler.hpp"
#include "C_Uti.hpp"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw::scl;
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
/*! \brief   Handle technical save operations for project

   \param[in]      orc_Handler      Data
   \param[in,out]  orc_XmlParser    XML parser to store in
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CamProHandlerFiler::h_Save(const C_CamProHandler & orc_Handler, C_OscXmlParserBase & orc_XmlParser)
{
   orc_XmlParser.CreateAndSelectNodeChild("opensyde-can-monitor-configuration");

   orc_XmlParser.CreateNodeChild("file-version", "1");

   // settings
   orc_XmlParser.CreateAndSelectNodeChild("settings");
   h_SaveSettings(orc_Handler, orc_XmlParser);
   //Return
   tgl_assert(orc_XmlParser.SelectNodeParent() == "opensyde-can-monitor-configuration");

   // messages
   orc_XmlParser.CreateAndSelectNodeChild("messages");
   orc_XmlParser.SetAttributeBool("cyclic-active", orc_Handler.GetCyclicMessageTransmitActive());
   h_SaveMessages(orc_Handler.GetMessages(), orc_XmlParser);
   //Return
   tgl_assert(orc_XmlParser.SelectNodeParent() == "opensyde-can-monitor-configuration");
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Handle technical load operations for project

   \param[out]     orc_Handler      Data
   \param[in,out]  orc_XmlParser    XML parser to load from

   \return
   C_NO_ERR Operation success
   C_CONFIG Operation failure: configuration invalid
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_CamProHandlerFiler::h_Load(C_CamProHandler & orc_Handler, C_OscXmlParserBase & orc_XmlParser)
{
   int32_t s32_Retval = C_CONFIG;

   if (orc_XmlParser.SelectRoot() == "opensyde-can-monitor-configuration")
   {
      // load settings section
      if (orc_XmlParser.SelectNodeChild("settings") == "settings")
      {
         s32_Retval = h_LoadSettings(orc_Handler, orc_XmlParser);
         //Return
         tgl_assert(orc_XmlParser.SelectNodeParent() == "opensyde-can-monitor-configuration");
      }

      // load messages
      if (orc_XmlParser.SelectNodeChild("messages") == "messages")
      {
         std::vector<C_CamProMessageData> c_Messages;
         if (orc_XmlParser.AttributeExists("cyclic-active"))
         {
            orc_Handler.SetCyclicMessageTransmitActive(orc_XmlParser.GetAttributeBool("cyclic-active"));
         }
         else
         {
            orc_Handler.SetCyclicMessageTransmitActive(true);
         }
         s32_Retval = h_LoadMessages(c_Messages, orc_XmlParser);
         orc_Handler.SetMessages(c_Messages);
         //Return
         tgl_assert(orc_XmlParser.SelectNodeParent() == "opensyde-can-monitor-configuration");
      }
   }

   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Save messages

   \param[in]      orc_Messages     Data
   \param[in,out]  orc_XmlParser    XML parser to load from
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CamProHandlerFiler::h_SaveMessages(const std::vector<C_CamProMessageData> & orc_Messages,
                                          C_OscXmlParserBase & orc_XmlParser)
{
   orc_XmlParser.SetAttributeUint32("length", orc_Messages.size());
   for (uint32_t u32_ItMessage = 0UL; u32_ItMessage < orc_Messages.size(); ++u32_ItMessage)
   {
      orc_XmlParser.CreateAndSelectNodeChild("message");
      C_CamProHandlerFiler::h_SaveMessage(orc_Messages[u32_ItMessage], orc_XmlParser);
      //Return
      orc_XmlParser.SelectNodeParent();
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Save message

   \param[in]      orc_Message      Data
   \param[in,out]  orc_XmlParser    XML parser to load from
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CamProHandlerFiler::h_SaveMessage(const C_CamProMessageData & orc_Message, C_OscXmlParserBase & orc_XmlParser)
{
   orc_XmlParser.SetAttributeBool("has-valid-hash", orc_Message.q_ContainsValidHash);
   orc_XmlParser.SetAttributeUint32("hash", orc_Message.u32_Hash);
   orc_XmlParser.SetAttributeUint32("ID", orc_Message.u32_Id);
   orc_XmlParser.SetAttributeUint32("DLC", orc_Message.u16_Dlc);
   orc_XmlParser.SetAttributeBool("XTD", orc_Message.q_IsExtended);
   orc_XmlParser.SetAttributeBool("RTR", orc_Message.q_IsRtr);
   orc_XmlParser.SetAttributeUint32("DB1", orc_Message.c_Bytes[0]);
   orc_XmlParser.SetAttributeUint32("DB2", orc_Message.c_Bytes[1]);
   orc_XmlParser.SetAttributeUint32("DB3", orc_Message.c_Bytes[2]);
   orc_XmlParser.SetAttributeUint32("DB4", orc_Message.c_Bytes[3]);
   orc_XmlParser.SetAttributeUint32("DB5", orc_Message.c_Bytes[4]);
   orc_XmlParser.SetAttributeUint32("DB6", orc_Message.c_Bytes[5]);
   orc_XmlParser.SetAttributeUint32("DB7", orc_Message.c_Bytes[6]);
   orc_XmlParser.SetAttributeUint32("DB8", orc_Message.c_Bytes[7]);
   orc_XmlParser.SetAttributeBool("do-cyclic-trigger", orc_Message.q_DoCyclicTrigger);
   orc_XmlParser.SetAttributeUint32("interval", orc_Message.u32_CyclicTriggerTime);
   orc_XmlParser.SetAttributeUint32("key-press-offset", orc_Message.u32_KeyPressOffset);
   orc_XmlParser.CreateNodeChild("name", orc_Message.c_Name);
   orc_XmlParser.CreateNodeChild("key", orc_Message.c_Key);
   orc_XmlParser.CreateNodeChild("database", orc_Message.c_DataBaseFilePath);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Load messages

   \param[out]     orc_Messages     Data
   \param[in,out]  orc_XmlParser    XML parser to load from

   \return
   C_NO_ERR Operation success
   C_CONFIG Operation failure: configuration invalid
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_CamProHandlerFiler::h_LoadMessages(std::vector<C_CamProMessageData> & orc_Messages,
                                             C_OscXmlParserBase & orc_XmlParser)
{
   int32_t s32_Retval = C_NO_ERR;
   C_SclString c_CurrentDataPoolNode;
   uint32_t u32_ExpectedSize = 0UL;
   const bool q_ExpectedSizeHere = orc_XmlParser.AttributeExists("length");

   orc_Messages.clear();

   //Check optional length
   if (q_ExpectedSizeHere == true)
   {
      u32_ExpectedSize = orc_XmlParser.GetAttributeUint32("length");
      orc_Messages.reserve(u32_ExpectedSize);
   }

   c_CurrentDataPoolNode = orc_XmlParser.SelectNodeChild("message");
   if (c_CurrentDataPoolNode == "message")
   {
      do
      {
         C_CamProMessageData c_Message;
         s32_Retval = h_LoadMessage(c_Message, orc_XmlParser);
         orc_Messages.push_back(c_Message);
         //Next
         c_CurrentDataPoolNode = orc_XmlParser.SelectNodeNext("message");
      }
      while ((s32_Retval == C_NO_ERR) && (c_CurrentDataPoolNode == "message"));
      //Return
      orc_XmlParser.SelectNodeParent();
   }
   //Compare length
   if ((s32_Retval == C_NO_ERR) && (q_ExpectedSizeHere == true))
   {
      if (u32_ExpectedSize != orc_Messages.size())
      {
         C_SclString c_Tmp;
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
   \param[in,out]  orc_XmlParser    XML parser to load from

   \return
   C_NO_ERR Operation success
   C_CONFIG Operation failure: configuration invalid
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_CamProHandlerFiler::h_LoadMessage(C_CamProMessageData & orc_Message, C_OscXmlParserBase & orc_XmlParser)
{
   int32_t s32_Retval = C_NO_ERR;

   if (orc_XmlParser.AttributeExists("has-valid-hash") == true)
   {
      orc_Message.q_ContainsValidHash = orc_XmlParser.GetAttributeBool("has-valid-hash");
   }
   if (orc_XmlParser.AttributeExists("hash") == true)
   {
      orc_Message.u32_Hash = orc_XmlParser.GetAttributeUint32("hash");
   }
   if (orc_XmlParser.AttributeExists("ID") == true)
   {
      orc_Message.u32_Id = orc_XmlParser.GetAttributeUint32("ID");
   }
   else
   {
      s32_Retval = C_CONFIG;
   }
   if (orc_XmlParser.AttributeExists("DLC") == true)
   {
      orc_Message.u16_Dlc = static_cast<uint8_t>(orc_XmlParser.GetAttributeUint32("DLC"));
   }
   else
   {
      s32_Retval = C_CONFIG;
   }
   if (orc_XmlParser.AttributeExists("XTD") == true)
   {
      orc_Message.q_IsExtended = orc_XmlParser.GetAttributeBool("XTD");
   }
   else
   {
      s32_Retval = C_CONFIG;
   }
   if (orc_XmlParser.AttributeExists("RTR") == true)
   {
      orc_Message.q_IsRtr = orc_XmlParser.GetAttributeBool("RTR");
   }
   else
   {
      s32_Retval = C_CONFIG;
   }
   if (orc_XmlParser.AttributeExists("DB1") == true)
   {
      orc_Message.c_Bytes[0] = static_cast<uint8_t>(orc_XmlParser.GetAttributeUint32("DB1"));
   }
   else
   {
      s32_Retval = C_CONFIG;
   }
   if (orc_XmlParser.AttributeExists("DB2") == true)
   {
      orc_Message.c_Bytes[1] = static_cast<uint8_t>(orc_XmlParser.GetAttributeUint32("DB2"));
   }
   else
   {
      s32_Retval = C_CONFIG;
   }
   if (orc_XmlParser.AttributeExists("DB3") == true)
   {
      orc_Message.c_Bytes[2] = static_cast<uint8_t>(orc_XmlParser.GetAttributeUint32("DB3"));
   }
   else
   {
      s32_Retval = C_CONFIG;
   }
   if (orc_XmlParser.AttributeExists("DB4") == true)
   {
      orc_Message.c_Bytes[3] = static_cast<uint8_t>(orc_XmlParser.GetAttributeUint32("DB4"));
   }
   else
   {
      s32_Retval = C_CONFIG;
   }
   if (orc_XmlParser.AttributeExists("DB5") == true)
   {
      orc_Message.c_Bytes[4] = static_cast<uint8_t>(orc_XmlParser.GetAttributeUint32("DB5"));
   }
   else
   {
      s32_Retval = C_CONFIG;
   }
   if (orc_XmlParser.AttributeExists("DB6") == true)
   {
      orc_Message.c_Bytes[5] = static_cast<uint8_t>(orc_XmlParser.GetAttributeUint32("DB6"));
   }
   else
   {
      s32_Retval = C_CONFIG;
   }
   if (orc_XmlParser.AttributeExists("DB7") == true)
   {
      orc_Message.c_Bytes[6] = static_cast<uint8_t>(orc_XmlParser.GetAttributeUint32("DB7"));
   }
   else
   {
      s32_Retval = C_CONFIG;
   }
   if (orc_XmlParser.AttributeExists("DB8") == true)
   {
      orc_Message.c_Bytes[7] = static_cast<uint8_t>(orc_XmlParser.GetAttributeUint32("DB8"));
   }
   else
   {
      s32_Retval = C_CONFIG;
   }
   if (orc_XmlParser.AttributeExists("do-cyclic-trigger") == true)
   {
      orc_Message.q_DoCyclicTrigger = orc_XmlParser.GetAttributeBool("do-cyclic-trigger");
   }
   else
   {
      s32_Retval = C_CONFIG;
   }
   if (orc_XmlParser.AttributeExists("interval") == true)
   {
      orc_Message.u32_CyclicTriggerTime = orc_XmlParser.GetAttributeUint32("interval");
   }
   else
   {
      s32_Retval = C_CONFIG;
   }
   if (orc_XmlParser.AttributeExists("key-press-offset") == true)
   {
      orc_Message.u32_KeyPressOffset = orc_XmlParser.GetAttributeUint32("key-press-offset");
   }
   else
   {
      s32_Retval = C_CONFIG;
   }

   if (orc_XmlParser.SelectNodeChild("name") == "name")
   {
      orc_Message.c_Name = orc_XmlParser.GetNodeContent();
      //Return
      tgl_assert(orc_XmlParser.SelectNodeParent() == "message");
   }
   else
   {
      s32_Retval = C_CONFIG;
   }
   if (orc_XmlParser.SelectNodeChild("key") == "key")
   {
      orc_Message.c_Key = orc_XmlParser.GetNodeContent();
      //Return
      tgl_assert(orc_XmlParser.SelectNodeParent() == "message");
   }
   else
   {
      s32_Retval = C_CONFIG;
   }
   if (orc_XmlParser.SelectNodeChild("database") == "database")
   {
      orc_Message.c_DataBaseFilePath = orc_XmlParser.GetNodeContent();
      //Return
      tgl_assert(orc_XmlParser.SelectNodeParent() == "message");
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
   \param[in,out]  orc_XmlParser    XML parser to store in
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CamProHandlerFiler::h_SaveSettings(const C_CamProHandler & orc_Handler, C_OscXmlParserBase & orc_XmlParser)
{
   // CAN DLL configuration
   orc_XmlParser.CreateAndSelectNodeChild("can-dll-path");
   orc_XmlParser.SetAttributeSint32("type", static_cast<int32_t>(orc_Handler.GetCanDllType()));
   orc_XmlParser.SetNodeContent(orc_Handler.GetCustomCanDllPath().toStdString().c_str());

   //Return
   orc_XmlParser.SelectNodeParent();

   // Filters
   orc_XmlParser.CreateAndSelectNodeChild("filters");
   orc_XmlParser.SetAttributeBool("active", orc_Handler.GetFilterWidgetEnabled());
   C_CamProHandlerFiler::h_SaveFilters(orc_Handler.GetFilters(), orc_XmlParser);
   //Return
   orc_XmlParser.SelectNodeParent();

   // Databases
   orc_XmlParser.CreateAndSelectNodeChild("databases");
   C_CamProHandlerFiler::h_SaveDatabases(orc_Handler.GetDatabases(), orc_XmlParser);
   //Return
   orc_XmlParser.SelectNodeParent();

   // Logging
   orc_XmlParser.CreateAndSelectNodeChild("logging");
   C_CamProHandlerFiler::h_SaveLoggingConfig(orc_Handler.GetLoggingData(), orc_XmlParser);
   //Return
   orc_XmlParser.SelectNodeParent();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Load settings section.

   \param[out]     orc_Handler      Data
   \param[in,out]  orc_XmlParser    XML parser to load from

   \return
   C_NO_ERR Operation success
   C_CONFIG Operation failure: configuration invalid
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_CamProHandlerFiler::h_LoadSettings(C_CamProHandler & orc_Handler, C_OscXmlParserBase & orc_XmlParser)
{
   int32_t s32_Return = C_NO_ERR;

   // CAN DLL path
   if (orc_XmlParser.SelectNodeChild("can-dll-path") == "can-dll-path")
   {
      if (orc_XmlParser.AttributeExists("type"))
      {
         orc_Handler.SetCanDllType(static_cast<C_CamProHandler::E_CanDllType>(orc_XmlParser.GetAttributeSint32("type")));
      }
      else
      {
         s32_Return = C_CONFIG;
      }

      orc_Handler.SetCustomCanDllPath(orc_XmlParser.GetNodeContent().c_str());

      //Return
      orc_XmlParser.SelectNodeParent();
   }
   else
   {
      s32_Return = C_CONFIG;
   }

   // Filters
   if ((s32_Return == C_NO_ERR) && (orc_XmlParser.SelectNodeChild("filters") == "filters"))
   {
      if (orc_XmlParser.AttributeExists("active"))
      {
         orc_Handler.SetFilterWidgetEnabled(orc_XmlParser.GetAttributeBool("active"));

         std::vector<C_CamProFilterData> c_Filters;
         s32_Return = h_LoadFilters(c_Filters, orc_XmlParser);
         orc_Handler.SetFilters(c_Filters);

         //Return
         orc_XmlParser.SelectNodeParent();
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
   if ((s32_Return == C_NO_ERR) && (orc_XmlParser.SelectNodeChild("databases") == "databases"))
   {
      std::vector<C_CamProDatabaseData> c_Databases;
      s32_Return = h_LoadDatabases(c_Databases, orc_XmlParser);
      orc_Handler.SetDatabases(c_Databases);

      //Return
      orc_XmlParser.SelectNodeParent();
   }
   else
   {
      s32_Return = C_CONFIG;
   }

   // Logging
   if ((s32_Return == C_NO_ERR) && (orc_XmlParser.SelectNodeChild("logging") == "logging"))
   {
      C_CamProLoggingData c_LoggingData;
      s32_Return = h_LoadLoggingConfig(c_LoggingData, orc_XmlParser);
      orc_Handler.SetLoggingData(c_LoggingData);

      //Return
      orc_XmlParser.SelectNodeParent();
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
   \param[in,out]  orc_XmlParser    XML parser to load from
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CamProHandlerFiler::h_SaveFilters(const std::vector<C_CamProFilterData> & orc_Filters,
                                         C_OscXmlParserBase & orc_XmlParser)
{
   orc_XmlParser.SetAttributeUint32("length", orc_Filters.size());
   for (uint32_t u32_ItFilter = 0UL; u32_ItFilter < orc_Filters.size(); ++u32_ItFilter)
   {
      orc_XmlParser.CreateAndSelectNodeChild("filter");
      C_CamProHandlerFiler::h_SaveFilter(orc_Filters[u32_ItFilter], orc_XmlParser);
      //Return
      orc_XmlParser.SelectNodeParent();
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Save filter.

   \param[in]      orc_Filter       Filter data
   \param[in,out]  orc_XmlParser    XML parser to load from
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CamProHandlerFiler::h_SaveFilter(const C_CamProFilterData & orc_Filter, C_OscXmlParserBase & orc_XmlParser)
{
   orc_XmlParser.SetAttributeBool("active", orc_Filter.q_Enabled);
   orc_XmlParser.CreateNodeChild("name", orc_Filter.c_Name.toStdString().c_str());
   orc_XmlParser.CreateNodeChild("comment", orc_Filter.c_Comment.toStdString().c_str());
   orc_XmlParser.CreateAndSelectNodeChild("filter-items");
   orc_XmlParser.SetAttributeUint32("length", orc_Filter.c_FilterItems.size());

   // filter items
   for (int32_t s32_ItFilterItem = 0UL; s32_ItFilterItem < orc_Filter.c_FilterItems.size(); ++s32_ItFilterItem)
   {
      orc_XmlParser.CreateAndSelectNodeChild("filter-item");
      C_CamProHandlerFiler::h_SaveFilterItem(orc_Filter.c_FilterItems[s32_ItFilterItem], orc_XmlParser);
      //Return
      orc_XmlParser.SelectNodeParent();
   }

   //Return
   orc_XmlParser.SelectNodeParent();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Save filter item.

   \param[in]      orc_FilterItem   Filter item data
   \param[in,out]  orc_XmlParser    XML parser to load from
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CamProHandlerFiler::h_SaveFilterItem(const C_CamProFilterItemData & orc_FilterItem,
                                            C_OscXmlParserBase & orc_XmlParser)
{
   orc_XmlParser.SetAttributeBool("active", orc_FilterItem.q_Enabled);
   orc_XmlParser.SetAttributeSint32("filter-type", static_cast<int32_t>(orc_FilterItem.e_Type));
   orc_XmlParser.SetAttributeBool("pass-filter", orc_FilterItem.q_PassFilter);
   orc_XmlParser.SetAttributeUint32("start-ID", orc_FilterItem.u32_StartId);
   orc_XmlParser.SetAttributeUint32("end-ID", orc_FilterItem.u32_EndId);
   orc_XmlParser.SetAttributeBool("extended-ID", static_cast<bool>(orc_FilterItem.u8_ExtendedId));
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Load filters.

   \param[out]     orc_Filters      Filters data
   \param[in,out]  orc_XmlParser    XML parser to load from

   \return
   C_NO_ERR Operation success
   C_CONFIG Operation failure: configuration invalid
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_CamProHandlerFiler::h_LoadFilters(std::vector<C_CamProFilterData> & orc_Filters,
                                            C_OscXmlParserBase & orc_XmlParser)
{
   int32_t s32_Retval = C_NO_ERR;
   C_SclString c_CurrentFilter;
   uint32_t u32_ExpectedSize = 0UL;
   const bool q_ExpectedSizeHere = orc_XmlParser.AttributeExists("length");

   orc_Filters.clear();

   //Check optional length
   if (q_ExpectedSizeHere == true)
   {
      u32_ExpectedSize = orc_XmlParser.GetAttributeUint32("length");
      orc_Filters.reserve(u32_ExpectedSize);
   }

   c_CurrentFilter = orc_XmlParser.SelectNodeChild("filter");
   if (c_CurrentFilter == "filter")
   {
      do
      {
         C_CamProFilterData c_Filter;
         s32_Retval = h_LoadFilter(c_Filter, orc_XmlParser);
         orc_Filters.push_back(c_Filter);
         //Next
         c_CurrentFilter = orc_XmlParser.SelectNodeNext("filter");
      }
      while ((s32_Retval == C_NO_ERR) && (c_CurrentFilter == "filter"));
      //Return
      orc_XmlParser.SelectNodeParent();
   }

   //Compare length
   if ((s32_Retval == C_NO_ERR) && (q_ExpectedSizeHere == true))
   {
      if (u32_ExpectedSize != orc_Filters.size())
      {
         C_SclString c_Tmp;
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
   \param[in,out]  orc_XmlParser    XML parser to load from

   \return
   C_NO_ERR Operation success
   C_CONFIG Operation failure: configuration invalid
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_CamProHandlerFiler::h_LoadFilter(C_CamProFilterData & orc_Filter, C_OscXmlParserBase & orc_XmlParser)
{
   int32_t s32_Retval = C_NO_ERR;
   C_SclString c_CurrentFilter;
   uint32_t u32_ExpectedSize = 0UL;
   bool q_ExpectedSizeHere = false;

   if (orc_XmlParser.AttributeExists("active") == true)
   {
      orc_Filter.q_Enabled = orc_XmlParser.GetAttributeBool("active");
   }
   else
   {
      s32_Retval = C_CONFIG;
   }

   if (orc_XmlParser.SelectNodeChild("name") == "name")
   {
      orc_Filter.c_Name = orc_XmlParser.GetNodeContent().c_str();
      //Return
      tgl_assert(orc_XmlParser.SelectNodeParent() == "filter");
   }
   else
   {
      s32_Retval = C_CONFIG;
   }

   if (orc_XmlParser.SelectNodeChild("comment") == "comment")
   {
      orc_Filter.c_Comment = orc_XmlParser.GetNodeContent().c_str();
      //Return
      tgl_assert(orc_XmlParser.SelectNodeParent() == "filter");
   }
   else
   {
      s32_Retval = C_CONFIG;
   }

   if (orc_XmlParser.SelectNodeChild("filter-items") == "filter-items")
   {
      q_ExpectedSizeHere = orc_XmlParser.AttributeExists("length");

      //Check optional length
      if (q_ExpectedSizeHere == true)
      {
         u32_ExpectedSize = orc_XmlParser.GetAttributeUint32("length");
         orc_Filter.c_FilterItems.reserve(u32_ExpectedSize);
      }

      c_CurrentFilter = orc_XmlParser.SelectNodeChild("filter-item");
      if (c_CurrentFilter == "filter-item")
      {
         do
         {
            C_CamProFilterItemData c_FilterItem;
            s32_Retval = h_LoadFilterItem(c_FilterItem, orc_XmlParser);
            orc_Filter.c_FilterItems.push_back(c_FilterItem);
            //Next
            c_CurrentFilter = orc_XmlParser.SelectNodeNext("filter-item");
         }
         while ((s32_Retval == C_NO_ERR) && (c_CurrentFilter == "filter-item"));
         //Return
         orc_XmlParser.SelectNodeParent();
      }
      //Return
      tgl_assert(orc_XmlParser.SelectNodeParent() == "filter");
   }
   else
   {
      s32_Retval = C_CONFIG;
   }
   //Compare length
   if ((s32_Retval == C_NO_ERR) && (q_ExpectedSizeHere == true))
   {
      if (u32_ExpectedSize != static_cast<uint32_t>(orc_Filter.c_FilterItems.size()))
      {
         C_SclString c_Tmp;
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
   \param[in,out]  orc_XmlParser    XML parser to load from

   \return
   C_NO_ERR Operation success
   C_CONFIG Operation failure: configuration invalid
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_CamProHandlerFiler::h_LoadFilterItem(C_CamProFilterItemData & orc_FilterItem,
                                               const C_OscXmlParserBase & orc_XmlParser)
{
   int32_t s32_Retval = C_NO_ERR;

   if (orc_XmlParser.AttributeExists("active") == true)
   {
      orc_FilterItem.q_Enabled = orc_XmlParser.GetAttributeBool("active");
   }
   else
   {
      s32_Retval = C_CONFIG;
   }

   if (orc_XmlParser.AttributeExists("filter-type") == true)
   {
      orc_FilterItem.e_Type =
         static_cast<C_CamProFilterItemData::E_Type>(orc_XmlParser.GetAttributeSint32("filter-type"));
   }
   else
   {
      s32_Retval = C_CONFIG;
   }

   if (orc_XmlParser.AttributeExists("pass-filter") == true)
   {
      orc_FilterItem.q_PassFilter = orc_XmlParser.GetAttributeBool("pass-filter");
   }
   else
   {
      s32_Retval = C_CONFIG;
   }

   if (orc_XmlParser.AttributeExists("start-ID") == true)
   {
      orc_FilterItem.u32_StartId = orc_XmlParser.GetAttributeUint32("start-ID");
   }
   else
   {
      s32_Retval = C_CONFIG;
   }

   if (orc_XmlParser.AttributeExists("end-ID") == true)
   {
      orc_FilterItem.u32_EndId = orc_XmlParser.GetAttributeUint32("end-ID");
   }
   else
   {
      s32_Retval = C_CONFIG;
   }

   if (orc_XmlParser.AttributeExists("extended-ID") == true)
   {
      // extended ID is traditionally uint8_t, but in fact it is either 0 or 1, i.e. bool
      orc_FilterItem.u8_ExtendedId = static_cast<uint8_t>(orc_XmlParser.GetAttributeBool("extended-ID"));
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
   \param[in,out]  orc_XmlParser    XML parser to load from
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CamProHandlerFiler::h_SaveDatabases(const std::vector<C_CamProDatabaseData> & orc_Databases,
                                           C_OscXmlParserBase & orc_XmlParser)
{
   orc_XmlParser.SetAttributeUint32("length", orc_Databases.size());
   for (uint32_t u32_ItDatabase = 0UL; u32_ItDatabase < orc_Databases.size(); ++u32_ItDatabase)
   {
      orc_XmlParser.CreateAndSelectNodeChild("database");
      C_CamProHandlerFiler::h_SaveDatabase(orc_Databases[u32_ItDatabase], orc_XmlParser);
      //Return
      orc_XmlParser.SelectNodeParent();
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Save filter.

   \param[in]      orc_Database     Database data
   \param[in,out]  orc_XmlParser    XML parser to load from
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CamProHandlerFiler::h_SaveDatabase(const C_CamProDatabaseData & orc_Database, C_OscXmlParserBase & orc_XmlParser)
{
   orc_XmlParser.SetAttributeBool("active", orc_Database.q_Enabled);
   orc_XmlParser.SetAttributeSint32("bus-index", orc_Database.s32_BusIndex);
   orc_XmlParser.CreateNodeChild("name", orc_Database.c_Name.toStdString().c_str());
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Load databases.

   \param[out]     orc_Databases    Databases data
   \param[in,out]  orc_XmlParser    XML parser to load from

   \return
   C_NO_ERR Operation success
   C_CONFIG Operation failure: configuration invalid
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_CamProHandlerFiler::h_LoadDatabases(std::vector<C_CamProDatabaseData> & orc_Databases,
                                              C_OscXmlParserBase & orc_XmlParser)
{
   int32_t s32_Retval = C_NO_ERR;
   C_SclString c_CurrentDatabase;
   uint32_t u32_ExpectedSize = 0UL;
   const bool q_ExpectedSizeHere = orc_XmlParser.AttributeExists("length");

   orc_Databases.clear();

   //Check optional length
   if (q_ExpectedSizeHere == true)
   {
      u32_ExpectedSize = orc_XmlParser.GetAttributeUint32("length");
      orc_Databases.reserve(u32_ExpectedSize);
   }

   c_CurrentDatabase = orc_XmlParser.SelectNodeChild("database");
   if (c_CurrentDatabase == "database")
   {
      do
      {
         C_CamProDatabaseData c_Database;
         s32_Retval = h_LoadDatabase(c_Database, orc_XmlParser);
         orc_Databases.push_back(c_Database);
         //Next
         c_CurrentDatabase = orc_XmlParser.SelectNodeNext("database");
      }
      while ((s32_Retval == C_NO_ERR) && (c_CurrentDatabase == "database"));
      //Return
      orc_XmlParser.SelectNodeParent();
   }

   //Compare length
   if ((s32_Retval == C_NO_ERR) && (q_ExpectedSizeHere == true))
   {
      if (u32_ExpectedSize != orc_Databases.size())
      {
         C_SclString c_Tmp;
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
   \param[in,out]  orc_XmlParser    XML parser to load from

   \return
   C_NO_ERR Operation success
   C_CONFIG Operation failure: configuration invalid
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_CamProHandlerFiler::h_LoadDatabase(C_CamProDatabaseData & orc_Database, C_OscXmlParserBase & orc_XmlParser)
{
   int32_t s32_Retval = C_NO_ERR;

   if (orc_XmlParser.AttributeExists("active") == true)
   {
      orc_Database.q_Enabled = orc_XmlParser.GetAttributeBool("active");
   }
   else
   {
      s32_Retval = C_CONFIG;
   }

   if (orc_XmlParser.AttributeExists("bus-index") == true)
   {
      orc_Database.s32_BusIndex = orc_XmlParser.GetAttributeSint32("bus-index");
   }
   else
   {
      s32_Retval = C_CONFIG;
   }

   if (orc_XmlParser.SelectNodeChild("name") == "name")
   {
      orc_Database.c_Name = orc_XmlParser.GetNodeContent().c_str();
      //Return
      tgl_assert(orc_XmlParser.SelectNodeParent() == "database");
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
   \param[in,out]  orc_XmlParser    XML parser to store in
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CamProHandlerFiler::h_SaveLoggingConfig(const C_CamProLoggingData & orc_LoggingData,
                                               C_OscXmlParserBase & orc_XmlParser)
{
   orc_XmlParser.SetAttributeBool("active", orc_LoggingData.q_Enabled);
   orc_XmlParser.SetAttributeSint32("overwrite-mode", static_cast<int32_t>(orc_LoggingData.e_OverwriteMode));
   orc_XmlParser.SetAttributeString("format", C_CamProLoggingData::h_FormatEnumToString(
                                       orc_LoggingData.e_FileFormat).toStdString().c_str());
   orc_XmlParser.CreateNodeChild("directory", orc_LoggingData.c_Directory.toStdString().c_str());
   orc_XmlParser.CreateNodeChild("file-name", orc_LoggingData.c_FileName.toStdString().c_str());
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Load logging configuration.

   \param[out]     orc_LoggingData  Logging data
   \param[in,out]  orc_XmlParser    XML parser to load from

   \return
   C_NO_ERR Operation success
   C_CONFIG Operation failure: configuration invalid
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_CamProHandlerFiler::h_LoadLoggingConfig(C_CamProLoggingData & orc_LoggingData,
                                                  C_OscXmlParserBase & orc_XmlParser)
{
   int32_t s32_Retval = C_NO_ERR;

   if (orc_XmlParser.AttributeExists("active") == true)
   {
      orc_LoggingData.q_Enabled = orc_XmlParser.GetAttributeBool("active");
   }
   else
   {
      s32_Retval = C_CONFIG;
   }

   if (orc_XmlParser.AttributeExists("overwrite-mode") == true)
   {
      orc_LoggingData.e_OverwriteMode =
         static_cast<C_CamProLoggingData::E_OverwriteMode>(orc_XmlParser.GetAttributeSint32("overwrite-mode"));
   }
   else
   {
      s32_Retval = C_CONFIG;
   }

   if (orc_XmlParser.AttributeExists("format") == true)
   {
      orc_LoggingData.e_FileFormat =
         C_CamProLoggingData::h_FormatStringToEnum(orc_XmlParser.GetAttributeString("format").c_str());
   }
   else
   {
      s32_Retval = C_CONFIG;
   }

   if (orc_XmlParser.SelectNodeChild("directory") == "directory")
   {
      orc_LoggingData.c_Directory = orc_XmlParser.GetNodeContent().c_str();
      //Return
      tgl_assert(orc_XmlParser.SelectNodeParent() == "logging");
   }
   else
   {
      s32_Retval = C_CONFIG;
   }

   if (orc_XmlParser.SelectNodeChild("file-name") == "file-name")
   {
      orc_LoggingData.c_FileName = orc_XmlParser.GetNodeContent().c_str();
      //Return
      tgl_assert(orc_XmlParser.SelectNodeParent() == "logging");
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
