//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Bus reader/writer

   Load / save bus data from / to XML file

   \copyright   Copyright 2016 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.h"

#include "stwtypes.h"
#include "stwerrors.h"
#include "C_OSCSystemBusFilerV2.h"
#include "CSCLString.h"
#include "TGLUtils.h"
#include "C_OSCSystemFilerUtil.h"
#include "C_OSCLoggingHandler.h"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw_opensyde_core;
using namespace stw_scl;
using namespace stw_types;
using namespace stw_errors;

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
   \param[in,out] orc_XMLParser  XML parser

   \return
   C_NO_ERR   data read
   C_CONFIG   content of file is invalid or incomplete
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_OSCSystemBusFilerV2::h_LoadBus(C_OSCSystemBus & orc_Bus, C_OSCXMLParserBase & orc_XMLParser)
{
   sint32 s32_Retval = C_NO_ERR;

   if (orc_XMLParser.SelectNodeChild("core") == "core")
   {
      //Name
      if (orc_XMLParser.SelectNodeChild("name") == "name")
      {
         orc_Bus.c_Name = orc_XMLParser.GetNodeContent();
         //Return
         tgl_assert(orc_XMLParser.SelectNodeParent() == "core");
      }
      else
      {
         osc_write_log_error("Loading bus definition", "Could not find \"core\".\"name\" node.");
         s32_Retval = C_CONFIG;
      }
      //Comment
      if (orc_XMLParser.SelectNodeChild("comment") == "comment")
      {
         orc_Bus.c_Comment = orc_XMLParser.GetNodeContent();
         //Return
         tgl_assert(orc_XMLParser.SelectNodeParent() == "core");
      }
      //Type
      if (orc_XMLParser.SelectNodeChild("type") == "type")
      {
         s32_Retval = C_OSCSystemFilerUtil::mh_BusTypeStringToEnum(orc_XMLParser.GetNodeContent(), orc_Bus.e_Type);
         if (s32_Retval != C_NO_ERR)
         {
            osc_write_log_error("Loading bus definition", "Could not find \"core\".\"type\" node.");
            s32_Retval = C_CONFIG;
         }
         //Return
         tgl_assert(orc_XMLParser.SelectNodeParent() == "core");
      }
      else
      {
         osc_write_log_error("Loading bus definition", "Could not find \"core\".\"type\" node.");
         s32_Retval = C_CONFIG;
      }
      //Bitrate
      if (orc_XMLParser.SelectNodeChild("bitrate") == "bitrate")
      {
         try
         {
            orc_Bus.u64_BitRate = orc_XMLParser.GetAttributeSint64("number");
         }
         catch (...)
         {
            osc_write_log_error("Loading bus definition", "Invalid value for \"bitrate\".\"number\".");
            orc_Bus.u64_BitRate = 0ULL;
            s32_Retval = C_CONFIG;
         }
         //Return
         tgl_assert(orc_XMLParser.SelectNodeParent() == "core");
      }
      else
      {
         osc_write_log_error("Loading bus definition", "Could not find \"core\".\"bitrate\" node.");
         s32_Retval = C_CONFIG;
      }
      //Bus id
      if (orc_XMLParser.SelectNodeChild("bus-id") == "bus-id")
      {
         try
         {
            orc_Bus.u8_BusID = static_cast<uint8>(orc_XMLParser.GetAttributeUint32("number"));
         }
         catch (...)
         {
            osc_write_log_error("Loading bus definition", "Invalid value for \"bus-id\".\"number\".");
            orc_Bus.u8_BusID = 0;
            s32_Retval = C_CONFIG;
         }
         //Return
         tgl_assert(orc_XMLParser.SelectNodeParent() == "core");
      }
      else
      {
         osc_write_log_error("Loading bus definition", "Could not find \"core\".\"bus-id\" node.");
         s32_Retval = C_CONFIG;
      }
      //Rx delta time
      if (orc_XMLParser.SelectNodeChild("rx-delta-time") == "rx-delta-time")
      {
         try
         {
            orc_Bus.u16_RxTimeoutOffsetMs = static_cast<uint16>(orc_XMLParser.GetAttributeUint32("number"));
         }
         catch (...)
         {
            osc_write_log_error("Loading bus definition", "Invalid value for \"rx-delta-time\".\"number\".");
            orc_Bus.u16_RxTimeoutOffsetMs = 0U;
            s32_Retval = C_CONFIG;
         }
         //Return
         tgl_assert(orc_XMLParser.SelectNodeParent() == "core");
      }
      else
      {
         osc_write_log_error("Loading bus definition", "Could not find \"core\".\"rx-delta-time\" node.");
         s32_Retval = C_CONFIG;
      }
      //Return
      tgl_assert(orc_XMLParser.SelectNodeParent() == "bus");
   }
   else
   {
      osc_write_log_error("Loading bus definition", "Could not find \"core\" node.");
      s32_Retval = C_CONFIG;
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Save bus

   Save bus data to XML file
   pre-condition: the passed XML parser has the active node set to "bus"
   post-condition: the passed XML parser has the active node set to the same "bus"

   \param[in]     orc_Bus        Bus data to store
   \param[in,out] orc_XMLParser  XML with bus active
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OSCSystemBusFilerV2::h_SaveBus(const C_OSCSystemBus & orc_Bus, C_OSCXMLParserBase & orc_XMLParser)
{
   const C_SCLString c_BitRate(orc_Bus.u64_BitRate);
   const C_SCLString c_RxTimeout(orc_Bus.u16_RxTimeoutOffsetMs);

   tgl_assert(orc_XMLParser.CreateAndSelectNodeChild("core") == "core");
   //Name
   tgl_assert(orc_XMLParser.CreateAndSelectNodeChild("name") == "name");
   orc_XMLParser.SetNodeContent(orc_Bus.c_Name);
   //Return
   tgl_assert(orc_XMLParser.SelectNodeParent() == "core");
   //Comment
   orc_XMLParser.CreateNodeChild("comment", orc_Bus.c_Comment);
   //Type
   tgl_assert(orc_XMLParser.CreateAndSelectNodeChild("type") == "type");
   orc_XMLParser.SetNodeContent(C_OSCSystemFilerUtil::mh_BusTypeEnumToString(orc_Bus.e_Type));
   //Return
   tgl_assert(orc_XMLParser.SelectNodeParent() == "core");
   //Bitrate
   tgl_assert(orc_XMLParser.CreateAndSelectNodeChild("bitrate") == "bitrate");
   orc_XMLParser.SetAttributeString("number", c_BitRate);
   //Return
   tgl_assert(orc_XMLParser.SelectNodeParent() == "core");
   //Bus id
   tgl_assert(orc_XMLParser.CreateAndSelectNodeChild("bus-id") == "bus-id");
   orc_XMLParser.SetAttributeUint32("number", orc_Bus.u8_BusID);
   //Return
   tgl_assert(orc_XMLParser.SelectNodeParent() == "core");
   //Bitrate
   tgl_assert(orc_XMLParser.CreateAndSelectNodeChild("rx-delta-time") == "rx-delta-time");
   orc_XMLParser.SetAttributeString("number", c_RxTimeout);
   //Return
   tgl_assert(orc_XMLParser.SelectNodeParent() == "core");
   //Return
   tgl_assert(orc_XMLParser.SelectNodeParent() == "bus");
}
