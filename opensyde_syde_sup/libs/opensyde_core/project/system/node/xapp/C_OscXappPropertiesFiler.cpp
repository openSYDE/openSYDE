//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       X-app properties filer

   X-app properties filer

   \copyright   Copyright 2025 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.hpp"

#include "stwtypes.hpp"
#include "stwerrors.hpp"
#include "C_OscLoggingHandler.hpp"
#include "C_OscSystemFilerUtil.hpp"
#include "C_OscXappPropertiesFiler.hpp"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw::errors;
using namespace stw::opensyde_core;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */
const uint16_t C_OscXappPropertiesFiler::mhu16_FILE_VERSION_1 = 1;

/* -- Types --------------------------------------------------------------------------------------------------------- */

/* -- Global Variables ---------------------------------------------------------------------------------------------- */

/* -- Module Global Variables --------------------------------------------------------------------------------------- */

/* -- Module Global Function Prototypes ----------------------------------------------------------------------------- */

/* -- Implementation ------------------------------------------------------------------------------------------------ */

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Load X-App properties file

   \param[out]  orc_XappProperties  Data storage
   \param[in]   orc_FilePath        File path

   \return
   C_NO_ERR   data read
   C_CONFIG   content of file is invalid or incomplete
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_OscXappPropertiesFiler::h_LoadXappPropertiesFile(C_OscXappProperties & orc_XappProperties,
                                                           const stw::scl::C_SclString & orc_FilePath)
{
   C_OscXmlParserLog c_XmlParser;

   c_XmlParser.SetLogHeading("Loading X-App properties");
   int32_t s32_Retval = C_OscSystemFilerUtil::h_GetParserForExistingFile(c_XmlParser, orc_FilePath,
                                                                         "opensyde-x-app-properties");

   //File version
   if (s32_Retval == C_NO_ERR)
   {
      s32_Retval = C_OscSystemFilerUtil::h_CheckVersion(c_XmlParser, mhu16_FILE_VERSION_1, "file-version",
                                                        "Loading X-App properties");
   }
   if (s32_Retval == C_NO_ERR)
   {
      s32_Retval = C_OscXappPropertiesFiler::h_LoadXappProperties(orc_XappProperties, c_XmlParser);
   }
   else
   {
      //More details are in log
      s32_Retval = C_CONFIG;
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Load X-App properties

   \param[out]     orc_XappProperties  data storage
   \param[in,out]  orc_XmlParser       XML with node active

   \return
   C_NO_ERR   data read
   C_CONFIG   content of file is invalid or incomplete
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_OscXappPropertiesFiler::h_LoadXappProperties(C_OscXappProperties & orc_XappProperties,
                                                       C_OscXmlParserBase & orc_XmlParser)
{
   int32_t s32_Retval;

   orc_XappProperties.Initialize();

   s32_Retval = orc_XmlParser.SelectNodeChildError("properties");
   if (s32_Retval == C_NO_ERR)
   {
      s32_Retval =
         orc_XmlParser.GetAttributeUint32Error("polling-interval-ms", orc_XappProperties.u32_PollingIntervalMs);
   }
   if (s32_Retval == C_NO_ERR)
   {
      s32_Retval =
         orc_XmlParser.GetAttributeUint32Error("data-request-interval-ms",
                                               orc_XappProperties.u32_DataRequestIntervalMs);
   }
   if (s32_Retval == C_NO_ERR)
   {
      s32_Retval = orc_XmlParser.SelectNodeChildError("connected-interface");
      if (s32_Retval == C_NO_ERR)
      {
         s32_Retval = C_OscXappPropertiesFiler::h_LoadCommInterfaceId(orc_XappProperties.e_ConnectedInterfaceType,
                                                                      orc_XappProperties.u8_ConnectedInterfaceNumber,
                                                                      orc_XmlParser, "connected-interface",
                                                                      "Loading X-App properties");
         tgl_assert(orc_XmlParser.SelectNodeParent() == "properties");
      }
   }
   orc_XmlParser.SelectNodeParent();
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Save X-App properties file

   \param[in]      orc_XappProperties  data storage
   \param[in,out]  orc_FilePath        File path for xml

   \return
   C_NO_ERR   data saved
   C_CONFIG   file could not be created
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_OscXappPropertiesFiler::h_SaveXappPropertiesFile(const C_OscXappProperties & orc_XappProperties,
                                                           const stw::scl::C_SclString & orc_FilePath)
{
   C_OscXmlParser c_XmlParser;
   int32_t s32_Retval = C_OscSystemFilerUtil::h_GetParserForNewFile(c_XmlParser, orc_FilePath,
                                                                    "opensyde-x-app-properties");

   if (s32_Retval == C_NO_ERR)
   {
      //File version
      tgl_assert(c_XmlParser.CreateAndSelectNodeChild("file-version") == "file-version");
      c_XmlParser.SetNodeContent(stw::scl::C_SclString::IntToStr(mhu16_FILE_VERSION_1));
      //Return
      c_XmlParser.SelectNodeParent();
      //node
      C_OscXappPropertiesFiler::h_SaveXappProperties(orc_XappProperties, c_XmlParser);
      //Don't forget to save!
      if (c_XmlParser.SaveToFile(orc_FilePath) != C_NO_ERR)
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
/*! \brief  Save X-App properties

   \param[in]      orc_XappProperties  data storage
   \param[in,out]  orc_XmlParser       XML with node active
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OscXappPropertiesFiler::h_SaveXappProperties(const C_OscXappProperties & orc_XappProperties,
                                                    C_OscXmlParserBase & orc_XmlParser)
{
   orc_XmlParser.CreateAndSelectNodeChild("properties");
   orc_XmlParser.SetAttributeUint32("polling-interval-ms", orc_XappProperties.u32_PollingIntervalMs);
   orc_XmlParser.SetAttributeUint32("data-request-interval-ms", orc_XappProperties.u32_DataRequestIntervalMs);
   orc_XmlParser.CreateAndSelectNodeChild("connected-interface");
   C_OscXappPropertiesFiler::h_SaveCommInterfaceId(orc_XappProperties.e_ConnectedInterfaceType,
                                                   orc_XappProperties.u8_ConnectedInterfaceNumber, orc_XmlParser);
   //Return
   tgl_assert(orc_XmlParser.SelectNodeParent() == "properties");
   //Return
   orc_XmlParser.SelectNodeParent();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Load comm interface id

   \param[out]     ore_Type               Type
   \param[out]     oru8_InterfaceNumber   Interface number
   \param[in,out]  orc_XmlParser          Xml parser
   \param[in]      orc_ParentNodeName     Parent node name
   \param[in]      orc_UseCase            Use case

   \return
   C_NO_ERR   data read
   C_CONFIG   content of file is invalid or incomplete
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_OscXappPropertiesFiler::h_LoadCommInterfaceId(C_OscSystemBus::E_Type & ore_Type,
                                                        uint8_t & oru8_InterfaceNumber,
                                                        C_OscXmlParserBase & orc_XmlParser,
                                                        const stw::scl::C_SclString & orc_ParentNodeName,
                                                        const stw::scl::C_SclString & orc_UseCase)
{
   int32_t s32_Retval;

   oru8_InterfaceNumber =
      static_cast<uint8_t>(orc_XmlParser.GetAttributeUint32("interface-number"));
   //Type
   if (orc_XmlParser.SelectNodeChild("type") == "type")
   {
      s32_Retval = C_OscSystemFilerUtil::h_BusTypeStringToEnum(
         orc_XmlParser.GetNodeContent(), ore_Type);
      //Return
      tgl_assert(orc_XmlParser.SelectNodeParent() == orc_ParentNodeName);
   }
   else
   {
      osc_write_log_error(orc_UseCase,
                          "Could not find \"" + orc_ParentNodeName + "\".\"type\" node.");
      s32_Retval = C_CONFIG;
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Save comm interface id

   \param[in]      oe_Type                Type
   \param[in]      ou8_InterfaceNumber    Interface number
   \param[in,out]  orc_XmlParser          Xml parser
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OscXappPropertiesFiler::h_SaveCommInterfaceId(const C_OscSystemBus::E_Type oe_Type,
                                                     const uint8_t ou8_InterfaceNumber,
                                                     C_OscXmlParserBase & orc_XmlParser)
{
   orc_XmlParser.SetAttributeUint32("interface-number",
                                    static_cast<uint32_t>(ou8_InterfaceNumber));
   orc_XmlParser.CreateNodeChild("type",
                                 C_OscSystemFilerUtil::h_BusTypeEnumToString(oe_Type));
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get automatically generated file name

   \return
   Automatically generated file name
*/
//----------------------------------------------------------------------------------------------------------------------
stw::scl::C_SclString C_OscXappPropertiesFiler::h_GetFileName(void)
{
   return "x_app_properties.xml";
}
