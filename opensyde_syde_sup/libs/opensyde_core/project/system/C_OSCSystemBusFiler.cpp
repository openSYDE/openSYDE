//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Bus reader/writer

   Load / save bus data from / to XML file

   \copyright   Copyright 2016 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.hpp"

#include "stwtypes.hpp"
#include "stwerrors.hpp"
#include "C_OscSystemBusFiler.hpp"
#include "C_SclString.hpp"
#include "TglUtils.hpp"
#include "C_OscSystemFilerUtil.hpp"
#include "C_OscLoggingHandler.hpp"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw::opensyde_core;
using namespace stw::scl;

using namespace stw::errors;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

/* -- Global Variables ---------------------------------------------------------------------------------------------- */

/* -- Module Global Variables --------------------------------------------------------------------------------------- */

/* -- Module Global Function Prototypes ----------------------------------------------------------------------------- */

/* -- Implementation ------------------------------------------------------------------------------------------------ */

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Load bus

   Load bus data.
   pre-condition: the passed XML parser has the active node set to "bus"
   post-condition: the passed XML parser has the active node set to the same "bus"

   \param[out]    orc_Bus        Bus data
   \param[in,out] orc_XmlParser  XML parser

   \return
   C_NO_ERR   data read
   C_CONFIG   content of file is invalid or incomplete (content of orc_Bus is undefined)
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_OscSystemBusFiler::h_LoadBus(C_OscSystemBus & orc_Bus, C_OscXmlParserBase & orc_XmlParser)
{
   int32_t s32_Retval = C_NO_ERR;

   //Name
   if (orc_XmlParser.SelectNodeChild("name") == "name")
   {
      orc_Bus.c_Name = orc_XmlParser.GetNodeContent();
      //Return
      tgl_assert(orc_XmlParser.SelectNodeParent() == "bus");
   }
   else
   {
      osc_write_log_error("Loading bus definition", "Could not find \"bus\".\"name\" node.");
      s32_Retval = C_CONFIG;
   }
   //Comment
   if (s32_Retval == C_NO_ERR)
   {
      if (orc_XmlParser.SelectNodeChild("comment") == "comment")
      {
         orc_Bus.c_Comment = orc_XmlParser.GetNodeContent();
         //Return
         tgl_assert(orc_XmlParser.SelectNodeParent() == "bus");
      }
      //Type
      if (orc_XmlParser.SelectNodeChild("type") == "type")
      {
         s32_Retval = C_OscSystemFilerUtil::h_BusTypeStringToEnum(orc_XmlParser.GetNodeContent(), orc_Bus.e_Type);
         if (s32_Retval != C_NO_ERR)
         {
            osc_write_log_error("Loading bus definition", "Could not find \"bus\".\"type\" node.");
            s32_Retval = C_CONFIG;
         }
         //Return
         tgl_assert(orc_XmlParser.SelectNodeParent() == "bus");
      }
      else
      {
         osc_write_log_error("Loading bus definition", "Could not find \"bus\".\"type\" node.");
         s32_Retval = C_CONFIG;
      }
   }
   //Bitrate
   if (s32_Retval == C_NO_ERR)
   {
      if (orc_XmlParser.SelectNodeChild("bitrate") == "bitrate")
      {
         try
         {
            orc_Bus.u64_BitRate = orc_XmlParser.GetAttributeSint64("number");
         }
         catch (...)
         {
            osc_write_log_error("Loading bus definition", "Invalid value for \"bitrate\".\"number\".");
            orc_Bus.u64_BitRate = 0ULL;
            s32_Retval = C_CONFIG;
         }
         //Return
         tgl_assert(orc_XmlParser.SelectNodeParent() == "bus");
      }
      else
      {
         osc_write_log_error("Loading bus definition", "Could not find \"bus\".\"bitrate\" node.");
         s32_Retval = C_CONFIG;
      }
   }
   //Bus id
   if (s32_Retval == C_NO_ERR)
   {
      if (orc_XmlParser.SelectNodeChild("bus-id") == "bus-id")
      {
         try
         {
            orc_Bus.u8_BusId = static_cast<uint8_t>(orc_XmlParser.GetAttributeUint32("number"));
         }
         catch (...)
         {
            osc_write_log_error("Loading bus definition", "Invalid value for \"bus-id\".\"number\".");
            orc_Bus.u8_BusId = 0;
            s32_Retval = C_CONFIG;
         }
         //Return
         tgl_assert(orc_XmlParser.SelectNodeParent() == "bus");
      }
      else
      {
         osc_write_log_error("Loading bus definition", "Could not find \"bus\".\"bus-id\" node.");
         s32_Retval = C_CONFIG;
      }
   }
   //Rx delta time
   if (s32_Retval == C_NO_ERR)
   {
      if (orc_XmlParser.SelectNodeChild("rx-delta-time") == "rx-delta-time")
      {
         try
         {
            orc_Bus.u16_RxTimeoutOffsetMs = static_cast<uint16_t>(orc_XmlParser.GetAttributeUint32("number"));
         }
         catch (...)
         {
            osc_write_log_error("Loading bus definition", "Invalid value for \"rx-delta-time\".\"number\".");
            orc_Bus.u16_RxTimeoutOffsetMs = 0U;
            s32_Retval = C_CONFIG;
         }
         //Return
         tgl_assert(orc_XmlParser.SelectNodeParent() == "bus");
      }
      else
      {
         osc_write_log_error("Loading bus definition", "Could not find \"bus\".\"rx-delta-time\" node.");
         s32_Retval = C_CONFIG;
      }
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Save bus

   Save bus data to XML file
   pre-condition: the passed XML parser has the active node set to "bus"
   post-condition: the passed XML parser has the active node set to the same "bus"

   \param[in]     orc_Bus        Bus data to store
   \param[in,out] orc_XmlParser  XML with bus active
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OscSystemBusFiler::h_SaveBus(const C_OscSystemBus & orc_Bus, C_OscXmlParserBase & orc_XmlParser)
{
   const C_SclString c_BitRate(orc_Bus.u64_BitRate);
   const C_SclString c_RxTimeout(orc_Bus.u16_RxTimeoutOffsetMs);

   //Name
   tgl_assert(orc_XmlParser.CreateAndSelectNodeChild("name") == "name");
   orc_XmlParser.SetNodeContent(orc_Bus.c_Name);
   //Return
   tgl_assert(orc_XmlParser.SelectNodeParent() == "bus");
   //Comment
   orc_XmlParser.CreateNodeChild("comment", orc_Bus.c_Comment);
   //Type
   tgl_assert(orc_XmlParser.CreateAndSelectNodeChild("type") == "type");
   orc_XmlParser.SetNodeContent(C_OscSystemFilerUtil::h_BusTypeEnumToString(orc_Bus.e_Type));
   //Return
   tgl_assert(orc_XmlParser.SelectNodeParent() == "bus");
   //Bitrate
   tgl_assert(orc_XmlParser.CreateAndSelectNodeChild("bitrate") == "bitrate");
   orc_XmlParser.SetAttributeString("number", c_BitRate);
   //Return
   tgl_assert(orc_XmlParser.SelectNodeParent() == "bus");
   //Bus id
   tgl_assert(orc_XmlParser.CreateAndSelectNodeChild("bus-id") == "bus-id");
   orc_XmlParser.SetAttributeUint32("number", orc_Bus.u8_BusId);
   //Return
   tgl_assert(orc_XmlParser.SelectNodeParent() == "bus");
   //Bitrate
   tgl_assert(orc_XmlParser.CreateAndSelectNodeChild("rx-delta-time") == "rx-delta-time");
   orc_XmlParser.SetAttributeString("number", c_RxTimeout);
   //Return
   tgl_assert(orc_XmlParser.SelectNodeParent() == "bus");
}
