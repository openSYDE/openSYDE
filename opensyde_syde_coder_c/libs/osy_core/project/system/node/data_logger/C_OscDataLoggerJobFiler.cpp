//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Filer for C_OscDataLoggerJob

   Filer for C_OscDataLoggerJob

   \copyright   Copyright 2024 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.hpp"

#include "TglFile.hpp"
#include "stwtypes.hpp"
#include "stwerrors.hpp"
#include "C_OscXmlParserLog.hpp"
#include "C_OscLoggingHandler.hpp"
#include "C_OscSystemFilerUtil.hpp"
#include "C_OscDataLoggerJobFiler.hpp"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw::tgl;
using namespace stw::errors;
using namespace stw::opensyde_core;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */
const uint16_t C_OscDataLoggerJobFiler::mhu16_FILE_VERSION_1 = 1;

/* -- Types --------------------------------------------------------------------------------------------------------- */

/* -- Global Variables ---------------------------------------------------------------------------------------------- */

/* -- Module Global Variables --------------------------------------------------------------------------------------- */

/* -- Module Global Function Prototypes ----------------------------------------------------------------------------- */

/* -- Implementation ------------------------------------------------------------------------------------------------ */

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Load data loggers

   \param[in,out]  orc_Config    Config
   \param[in]      orc_Path      Path

   \return
   C_NO_ERR    data read
   C_RANGE     specified data loggers file does not exist
   C_NOACT     specified file is present but structure is invalid (e.g. invalid XML file)
   C_CONFIG    data loggers content is invalid or incomplete
               data loggers could not be loaded
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_OscDataLoggerJobFiler::h_LoadFile(std::vector<C_OscDataLoggerJob> & orc_Config,
                                            const stw::scl::C_SclString & orc_Path)
{
   int32_t s32_Retval = C_NO_ERR;

   if (TglFileExists(orc_Path) == true)
   {
      C_OscXmlParserLog c_XmlParser;
      c_XmlParser.SetLogHeading("Loading data loggers data");
      s32_Retval = c_XmlParser.LoadFromFile(orc_Path);
      if (s32_Retval == C_NO_ERR)
      {
         if (c_XmlParser.SelectRoot() == "opensyde-data-loggers")
         {
            s32_Retval = h_LoadData(orc_Config, c_XmlParser);
         }
         else
         {
            osc_write_log_error("Loading data loggers data",
                                "Could not find \"opensyde-data-loggers\" node.");
            s32_Retval = C_CONFIG;
         }
      }
      else
      {
         osc_write_log_error("Loading data loggers data", "File \"" + orc_Path + "\" could not be opened.");
         s32_Retval = C_NOACT;
      }
   }
   else
   {
      osc_write_log_error("Loading data loggers data", "File \"" + orc_Path + "\" does not exist.");
      s32_Retval = C_RANGE;
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Save data loggers

   \param[in]  orc_Config  Config
   \param[in]  orc_Path    Path

   \return
   C_NO_ERR   data saved
   C_CONFIG   data invalid
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_OscDataLoggerJobFiler::h_SaveFile(const std::vector<C_OscDataLoggerJob> & orc_Config,
                                            const stw::scl::C_SclString & orc_Path)
{
   C_OscXmlParser c_XmlParser;
   int32_t s32_Retval = C_OscSystemFilerUtil::h_GetParserForNewFile(c_XmlParser, orc_Path,
                                                                    "opensyde-data-loggers");

   if (s32_Retval == C_NO_ERR)
   {
      //node
      C_OscDataLoggerJobFiler::h_SaveData(orc_Config, c_XmlParser);
      //Don't forget to save!
      if (c_XmlParser.SaveToFile(orc_Path) != C_NO_ERR)
      {
         osc_write_log_error("Saving data loggers data", "Could not create file for node.");
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
/*! \brief  Load data loggers

   \param[in,out]  orc_Config       Config
   \param[in,out]  orc_XmlParser    XML parser

   \return
   C_NO_ERR    data read
   C_CONFIG    Data loggers file content is invalid or incomplete
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_OscDataLoggerJobFiler::h_LoadData(std::vector<C_OscDataLoggerJob> & orc_Config,
                                            C_OscXmlParserBase & orc_XmlParser)
{
   int32_t s32_Retval = C_OscSystemFilerUtil::h_CheckVersion(orc_XmlParser, mhu16_FILE_VERSION_1, "file-version",
                                                             "Loading data loggers data");

   if (s32_Retval == C_NO_ERR)
   {
      s32_Retval = orc_XmlParser.SelectNodeChildError("jobs");

      orc_Config.clear();
      if (s32_Retval == C_NO_ERR)
      {
         uint32_t u32_ExpectedSize;
         s32_Retval = orc_XmlParser.GetAttributeUint32Error("length", u32_ExpectedSize);
         if (s32_Retval == C_NO_ERR)
         {
            stw::scl::C_SclString c_NodeName = orc_XmlParser.SelectNodeChild("job");
            if (c_NodeName == "job")
            {
               do
               {
                  C_OscDataLoggerJob c_Job;
                  s32_Retval = C_OscDataLoggerJobFiler::mh_LoadJobData(c_Job, orc_XmlParser);
                  if (s32_Retval == C_NO_ERR)
                  {
                     orc_Config.push_back(c_Job);
                  }

                  c_NodeName = orc_XmlParser.SelectNodeNext("job");
               }
               while ((c_NodeName == "job") && (s32_Retval == C_NO_ERR));
               tgl_assert(orc_XmlParser.SelectNodeParent() == "jobs");
            }
            if (u32_ExpectedSize != orc_Config.size())
            {
               stw::scl::C_SclString c_Tmp;
               c_Tmp.PrintFormatted("Unexpected data loggers count, expected: %u, got %u", u32_ExpectedSize,
                                    static_cast<uint32_t>(orc_Config.size()));
               orc_XmlParser.ReportErrorForAttributeContentAppendXmlContext("length", c_Tmp);
            }
         }
         orc_XmlParser.SelectNodeParent();
      }
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Save data loggers

   \param[in]      orc_Config       Config
   \param[in,out]  orc_XmlParser    XML parser
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OscDataLoggerJobFiler::h_SaveData(const std::vector<C_OscDataLoggerJob> & orc_Config,
                                         C_OscXmlParserBase & orc_XmlParser)
{
   //File version
   tgl_assert(orc_XmlParser.CreateAndSelectNodeChild("file-version") == "file-version");
   orc_XmlParser.SetNodeContent(stw::scl::C_SclString::IntToStr(mhu16_FILE_VERSION_1));
   //Return
   orc_XmlParser.SelectNodeParent();
   tgl_assert(orc_XmlParser.CreateAndSelectNodeChild("jobs") == "jobs");
   orc_XmlParser.SetAttributeUint32("length", static_cast<uint32_t>(orc_Config.size()));
   for (std::vector<C_OscDataLoggerJob>::const_iterator c_It = orc_Config.begin();
        c_It != orc_Config.end(); ++c_It)
   {
      tgl_assert(orc_XmlParser.CreateAndSelectNodeChild("job") == "job");
      C_OscDataLoggerJobFiler::mh_SaveJobData(*c_It, orc_XmlParser);
      tgl_assert(orc_XmlParser.SelectNodeParent() == "jobs");
   }
   //Return
   orc_XmlParser.SelectNodeParent();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Load data element id

   \param[in,out]  orc_Config       Config
   \param[in,out]  orc_XmlParser    Xml parser

   \return
   C_NO_ERR    data read
   C_CONFIG    Data loggers file content is invalid or incomplete
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_OscDataLoggerJobFiler::h_LoadDataElementId(C_OscNodeDataPoolListElementId & orc_Config,
                                                     C_OscXmlParserBase & orc_XmlParser)
{
   int32_t s32_Retval = orc_XmlParser.SelectNodeChildError("index");

   if (s32_Retval == C_NO_ERR)
   {
      s32_Retval = orc_XmlParser.GetAttributeUint32Error("node", orc_Config.u32_NodeIndex);
   }
   if (s32_Retval == C_NO_ERR)
   {
      s32_Retval = orc_XmlParser.GetAttributeUint32Error("data-pool", orc_Config.u32_DataPoolIndex);
   }
   if (s32_Retval == C_NO_ERR)
   {
      s32_Retval = orc_XmlParser.GetAttributeUint32Error("list", orc_Config.u32_ListIndex);
   }
   if (s32_Retval == C_NO_ERR)
   {
      s32_Retval = orc_XmlParser.GetAttributeUint32Error("element", orc_Config.u32_ElementIndex);
   }
   //Return
   orc_XmlParser.SelectNodeParent();
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Save data element id

   \param[in]      orc_Config       Config
   \param[in,out]  orc_XmlParser    Xml parser
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OscDataLoggerJobFiler::h_SaveDataElementId(const C_OscNodeDataPoolListElementId & orc_Config,
                                                  C_OscXmlParserBase & orc_XmlParser)
{
   orc_XmlParser.CreateAndSelectNodeChild("index");
   orc_XmlParser.SetAttributeUint32("node", orc_Config.u32_NodeIndex);
   orc_XmlParser.SetAttributeUint32("data-pool", orc_Config.u32_DataPoolIndex);
   orc_XmlParser.SetAttributeUint32("list", orc_Config.u32_ListIndex);
   orc_XmlParser.SetAttributeUint32("element", orc_Config.u32_ElementIndex);
   //Return
   orc_XmlParser.SelectNodeParent();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Load data element opt array id

   \param[in,out]  orc_Config       Config
   \param[in,out]  orc_XmlParser    Xml parser

   \return
   C_NO_ERR    data read
   C_CONFIG    Data loggers file content is invalid or incomplete
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_OscDataLoggerJobFiler::h_LoadDataElementOptArrayId(C_OscNodeDataPoolListElementOptArrayId & orc_Config,
                                                             C_OscXmlParserBase & orc_XmlParser)
{
   bool q_UseArrayElementIndex = false;
   uint32_t u32_ArrayElementIndex = 0UL;
   C_OscNodeDataPoolListElementId c_TmpId;
   int32_t s32_Retval = orc_XmlParser.GetAttributeBoolError("use-array-element-index", q_UseArrayElementIndex);

   if (s32_Retval == C_NO_ERR)
   {
      s32_Retval = orc_XmlParser.GetAttributeUint32Error("array-element-index", u32_ArrayElementIndex);
   }
   if (s32_Retval == C_NO_ERR)
   {
      s32_Retval = C_OscDataLoggerJobFiler::h_LoadDataElementId(c_TmpId, orc_XmlParser);
   }
   if (s32_Retval == C_NO_ERR)
   {
      orc_Config = C_OscNodeDataPoolListElementOptArrayId(c_TmpId, q_UseArrayElementIndex, u32_ArrayElementIndex);
      s32_Retval = orc_XmlParser.SelectNodeChildError("hal-channel-name");
   }
   if (s32_Retval == C_NO_ERR)
   {
      orc_Config.SetHalChannelName(orc_XmlParser.GetNodeContent());
      //Return
      orc_XmlParser.SelectNodeParent();
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Save data element opt array id

   \param[in]      orc_Config       Config
   \param[in,out]  orc_XmlParser    Xml parser
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OscDataLoggerJobFiler::h_SaveDataElementOptArrayId(const C_OscNodeDataPoolListElementOptArrayId & orc_Config,
                                                          C_OscXmlParserBase & orc_XmlParser)
{
   orc_XmlParser.SetAttributeBool("use-array-element-index", orc_Config.GetUseArrayElementIndex());
   orc_XmlParser.SetAttributeUint32("array-element-index", orc_Config.GetArrayElementIndex());
   C_OscDataLoggerJobFiler::h_SaveDataElementId(orc_Config, orc_XmlParser);
   orc_XmlParser.CreateNodeChild("hal-channel-name", orc_Config.GetHalChannelName());
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
int32_t C_OscDataLoggerJobFiler::h_LoadCommInterfaceId(C_OscSystemBus::E_Type & ore_Type,
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
void C_OscDataLoggerJobFiler::h_SaveCommInterfaceId(const C_OscSystemBus::E_Type oe_Type,
                                                    const uint8_t ou8_InterfaceNumber,
                                                    C_OscXmlParserBase & orc_XmlParser)
{
   orc_XmlParser.SetAttributeUint32("interface-number",
                                    static_cast<uint32_t>(ou8_InterfaceNumber));
   orc_XmlParser.CreateNodeChild("type",
                                 C_OscSystemFilerUtil::h_BusTypeEnumToString(oe_Type));
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Default constructor
*/
//----------------------------------------------------------------------------------------------------------------------
C_OscDataLoggerJobFiler::C_OscDataLoggerJobFiler()
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Load job data

   \param[in,out]  orc_Config       Config
   \param[in,out]  orc_XmlParser    XML parser

   \return
   C_NO_ERR    data read
   C_CONFIG    Data loggers file content is invalid or incomplete
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_OscDataLoggerJobFiler::mh_LoadJobData(C_OscDataLoggerJob & orc_Config, C_OscXmlParserBase & orc_XmlParser)
{
   int32_t s32_Retval = orc_XmlParser.GetAttributeBoolError("is-enabled", orc_Config.q_IsEnabled);

   if (s32_Retval == C_NO_ERR)
   {
      s32_Retval = mh_LoadJobProperties(orc_Config.c_Properties, orc_XmlParser);
   }

   if (s32_Retval == C_NO_ERR)
   {
      s32_Retval = mh_LoadConfiguredDataElements(orc_Config.c_ConfiguredDataElements, orc_XmlParser);
   }

   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Save job data

   \param[in]      orc_Config       Config
   \param[in,out]  orc_XmlParser    XML parser
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OscDataLoggerJobFiler::mh_SaveJobData(const C_OscDataLoggerJob & orc_Config, C_OscXmlParserBase & orc_XmlParser)
{
   orc_XmlParser.SetAttributeBool("is-enabled", orc_Config.q_IsEnabled);
   mh_SaveJobProperties(orc_Config.c_Properties, orc_XmlParser);
   mh_SaveConfiguredDataElements(orc_Config.c_ConfiguredDataElements, orc_XmlParser);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Load job properties

   \param[in,out]  orc_Config       Config
   \param[in,out]  orc_XmlParser    Xml parser

   \return
   C_NO_ERR    data read
   C_CONFIG    Data loggers file content is invalid or incomplete
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_OscDataLoggerJobFiler::mh_LoadJobProperties(C_OscDataLoggerJobProperties & orc_Config,
                                                      C_OscXmlParserBase & orc_XmlParser)
{
   int32_t s32_Retval = orc_XmlParser.SelectNodeChildError("properties");

   if (s32_Retval == C_NO_ERR)
   {
      s32_Retval = orc_XmlParser.GetAttributeUint32Error("max-log-file-size-mb",
                                                         orc_Config.u32_MaxLogFileSizeMb);
   }

   if (s32_Retval == C_NO_ERR)
   {
      s32_Retval = orc_XmlParser.SelectNodeChildError("name");
      if (s32_Retval == C_NO_ERR)
      {
         orc_Config.c_Name = orc_XmlParser.GetNodeContent();
         tgl_assert(orc_XmlParser.SelectNodeParent() == "properties");
      }
   }

   if (s32_Retval == C_NO_ERR)
   {
      s32_Retval = orc_XmlParser.SelectNodeChildError("comment");
      if (s32_Retval == C_NO_ERR)
      {
         orc_Config.c_Comment = orc_XmlParser.GetNodeContent();
         tgl_assert(orc_XmlParser.SelectNodeParent() == "properties");
      }
   }

   if (s32_Retval == C_NO_ERR)
   {
      s32_Retval = orc_XmlParser.SelectNodeChildError("log-file-format");
      if (s32_Retval == C_NO_ERR)
      {
         s32_Retval = mh_StringToLogFileType(orc_XmlParser.GetNodeContent(), orc_Config.e_LogFileFormat);
         tgl_assert(orc_XmlParser.SelectNodeParent() == "properties");
      }
   }

   if (s32_Retval == C_NO_ERR)
   {
      s32_Retval = orc_XmlParser.SelectNodeChildError("local-log-trigger");
      if (s32_Retval == C_NO_ERR)
      {
         s32_Retval = mh_StringToLocalLogTriggerType(orc_XmlParser.GetNodeContent(), orc_Config.e_LocalLogTrigger);
         tgl_assert(orc_XmlParser.SelectNodeParent() == "properties");
      }
   }
   if (s32_Retval == C_NO_ERR)
   {
      if (orc_XmlParser.SelectNodeChild("connected-interface") == "connected-interface")
      {
         s32_Retval = C_OscDataLoggerJobFiler::h_LoadCommInterfaceId(orc_Config.e_ConnectedInterfaceType,
                                                                     orc_Config.u8_ConnectedInterfaceNumber,
                                                                     orc_XmlParser, "connected-interface",
                                                                     "Loading data loggers data");
         tgl_assert(orc_XmlParser.SelectNodeParent() == "properties");
      }
      else
      {
         //Default
         orc_Config.e_ConnectedInterfaceType = C_OscSystemBus::eCAN;
         orc_Config.u8_ConnectedInterfaceNumber = 0U;
      }
   }
   tgl_assert(orc_XmlParser.SelectNodeParent() == "job");
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Save job properties

   \param[in]      orc_Config       Config
   \param[in,out]  orc_XmlParser    Xml parser
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OscDataLoggerJobFiler::mh_SaveJobProperties(const C_OscDataLoggerJobProperties & orc_Config,
                                                   C_OscXmlParserBase & orc_XmlParser)
{
   orc_XmlParser.CreateAndSelectNodeChild("properties");
   orc_XmlParser.SetAttributeUint32("max-log-file-size-mb", orc_Config.u32_MaxLogFileSizeMb);
   orc_XmlParser.CreateNodeChild("name", orc_Config.c_Name);
   orc_XmlParser.CreateNodeChild("comment", orc_Config.c_Comment);
   orc_XmlParser.CreateNodeChild("log-file-format", mh_LogFileTypeTypeToString(orc_Config.e_LogFileFormat));
   orc_XmlParser.CreateNodeChild("local-log-trigger", mh_LocalLogTriggerTypeToString(orc_Config.e_LocalLogTrigger));
   orc_XmlParser.CreateAndSelectNodeChild("connected-interface");
   C_OscDataLoggerJobFiler::h_SaveCommInterfaceId(orc_Config.e_ConnectedInterfaceType,
                                                  orc_Config.u8_ConnectedInterfaceNumber, orc_XmlParser);
   tgl_assert(orc_XmlParser.SelectNodeParent() == "properties");
   tgl_assert(orc_XmlParser.SelectNodeParent() == "job");
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Load configured data elements

   \param[in,out]  orc_Config       Config
   \param[in,out]  orc_XmlParser    Xml parser

   \return
   C_NO_ERR    data read
   C_CONFIG    Data loggers file content is invalid or incomplete
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_OscDataLoggerJobFiler::mh_LoadConfiguredDataElements(
   std::vector<C_OscDataLoggerDataElementReference> & orc_Config, C_OscXmlParserBase & orc_XmlParser)
{
   int32_t s32_Retval = orc_XmlParser.SelectNodeChildError("configured-data-elements");

   orc_Config.clear();
   if (s32_Retval == C_NO_ERR)
   {
      uint32_t u32_ExpectedSize;
      s32_Retval = orc_XmlParser.GetAttributeUint32Error("length", u32_ExpectedSize);
      if (s32_Retval == C_NO_ERR)
      {
         stw::scl::C_SclString c_NodeName = orc_XmlParser.SelectNodeChild("configured-data-element");
         if (c_NodeName == "configured-data-element")
         {
            do
            {
               C_OscDataLoggerDataElementReference c_Data;
               s32_Retval = C_OscDataLoggerJobFiler::mh_LoadConfiguredDataElement(c_Data, orc_XmlParser);
               if (s32_Retval == C_NO_ERR)
               {
                  orc_Config.push_back(c_Data);
               }

               c_NodeName = orc_XmlParser.SelectNodeNext("configured-data-element");
            }
            while ((c_NodeName == "configured-data-element") && (s32_Retval == C_NO_ERR));
            tgl_assert(orc_XmlParser.SelectNodeParent() == "configured-data-elements");
         }
         if (u32_ExpectedSize != orc_Config.size())
         {
            stw::scl::C_SclString c_Tmp;
            c_Tmp.PrintFormatted("Unexpected data logger data elements count, expected: %u, got %u", u32_ExpectedSize,
                                 static_cast<uint32_t>(orc_Config.size()));
            orc_XmlParser.ReportErrorForAttributeContentAppendXmlContext("length", c_Tmp);
         }
      }
      orc_XmlParser.SelectNodeParent();
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Save configured data elements

   \param[in]      orc_Config       Config
   \param[in,out]  orc_XmlParser    Xml parser
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OscDataLoggerJobFiler::mh_SaveConfiguredDataElements(
   const std::vector<C_OscDataLoggerDataElementReference> & orc_Config, C_OscXmlParserBase & orc_XmlParser)
{
   tgl_assert(orc_XmlParser.CreateAndSelectNodeChild("configured-data-elements") == "configured-data-elements");
   orc_XmlParser.SetAttributeUint32("length", static_cast<uint32_t>(orc_Config.size()));
   for (std::vector<C_OscDataLoggerDataElementReference>::const_iterator c_It = orc_Config.begin();
        c_It != orc_Config.end(); ++c_It)
   {
      tgl_assert(orc_XmlParser.CreateAndSelectNodeChild("configured-data-element") == "configured-data-element");
      C_OscDataLoggerJobFiler::mh_SaveConfiguredDataElement(*c_It, orc_XmlParser);
      tgl_assert(orc_XmlParser.SelectNodeParent() == "configured-data-elements");
   }
   //Return
   orc_XmlParser.SelectNodeParent();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Load configured data element

   \param[in,out]  orc_Config       Config
   \param[in,out]  orc_XmlParser    Xml parser

   \return
   C_NO_ERR    data read
   C_CONFIG    Data loggers file content is invalid or incomplete
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_OscDataLoggerJobFiler::mh_LoadConfiguredDataElement(C_OscDataLoggerDataElementReference & orc_Config,
                                                              C_OscXmlParserBase & orc_XmlParser)
{
   int32_t s32_Retval = orc_XmlParser.GetAttributeBoolError("use-custom-name", orc_Config.q_UseCustomName);

   if (s32_Retval == C_NO_ERR)
   {
      s32_Retval = orc_XmlParser.SelectNodeChildError("custom-name");
      if (s32_Retval == C_NO_ERR)
      {
         orc_Config.c_CustomName = orc_XmlParser.GetNodeContent();
         tgl_assert(orc_XmlParser.SelectNodeParent() == "configured-data-element");
      }
   }

   if (s32_Retval == C_NO_ERR)
   {
      s32_Retval = orc_XmlParser.SelectNodeChildError("data-pool-element");
      if (s32_Retval == C_NO_ERR)
      {
         s32_Retval = h_LoadDataElementOptArrayId(orc_Config.c_ConfiguredElementId, orc_XmlParser);
      }
      //Return
      orc_XmlParser.SelectNodeParent();
   }

   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Save configured data element

   \param[in]      orc_Config       Config
   \param[in,out]  orc_XmlParser    Xml parser
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OscDataLoggerJobFiler::mh_SaveConfiguredDataElement(const C_OscDataLoggerDataElementReference & orc_Config,
                                                           C_OscXmlParserBase & orc_XmlParser)
{
   orc_XmlParser.SetAttributeBool("use-custom-name", orc_Config.q_UseCustomName);
   orc_XmlParser.CreateNodeChild("custom-name", orc_Config.c_CustomName);
   orc_XmlParser.CreateAndSelectNodeChild("data-pool-element");
   h_SaveDataElementOptArrayId(orc_Config.c_ConfiguredElementId, orc_XmlParser);
   //Return
   orc_XmlParser.SelectNodeParent();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Log file type type to string

   \param[in]  ore_Type    Type

   \return
   Stringified log file type type
*/
//----------------------------------------------------------------------------------------------------------------------
stw::scl::C_SclString C_OscDataLoggerJobFiler::mh_LogFileTypeTypeToString(
   const C_OscDataLoggerJobProperties::E_LogFileFormat & ore_Type)
{
   stw::scl::C_SclString c_Retval;

   switch (ore_Type)
   {
   case C_OscDataLoggerJobProperties::eLFF_CSV:
      c_Retval = "csv";
      break;
   case C_OscDataLoggerJobProperties::eLFF_PARQUET:
      c_Retval = "parquet";
      break;
   default:
      c_Retval = "invalid";
      break;
   }
   return c_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  String to log file type

   \param[in]   orc_String    String
   \param[out]  ore_Type      Type

   \return
   C_NO_ERR   no error
   C_RANGE    String unknown
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_OscDataLoggerJobFiler::mh_StringToLogFileType(const stw::scl::C_SclString & orc_String,
                                                        C_OscDataLoggerJobProperties::E_LogFileFormat & ore_Type)
{
   int32_t s32_Retval = C_NO_ERR;

   if (orc_String == "csv")
   {
      ore_Type = C_OscDataLoggerJobProperties::eLFF_CSV;
   }
   else if (orc_String == "parquet")
   {
      ore_Type = C_OscDataLoggerJobProperties::eLFF_PARQUET;
   }
   else
   {
      s32_Retval = C_RANGE;
   }

   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Local log trigger type to string

   \param[in]  ore_Type    Type

   \return
   Stringified local log trigger type
*/
//----------------------------------------------------------------------------------------------------------------------
stw::scl::C_SclString C_OscDataLoggerJobFiler::mh_LocalLogTriggerTypeToString(
   const C_OscDataLoggerJobProperties::E_LocalLogTrigger & ore_Type)
{
   stw::scl::C_SclString c_Retval;

   switch (ore_Type)
   {
   case C_OscDataLoggerJobProperties::eLLT_ON_CHANGE:
      c_Retval = "on-change";
      break;
   case C_OscDataLoggerJobProperties::eLLT_INTERVAL:
      c_Retval = "interval";
      break;
   default:
      c_Retval = "invalid";
      break;
   }
   return c_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  String to local log trigger type

   \param[in]   orc_String    String
   \param[out]  ore_Type      Type

   \return
   C_NO_ERR   no error
   C_RANGE    String unknown
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_OscDataLoggerJobFiler::mh_StringToLocalLogTriggerType(const stw::scl::C_SclString & orc_String,
                                                                C_OscDataLoggerJobProperties::E_LocalLogTrigger & ore_Type)
{
   int32_t s32_Retval = C_NO_ERR;

   if (orc_String == "on-change")
   {
      ore_Type = C_OscDataLoggerJobProperties::eLLT_ON_CHANGE;
   }
   else if (orc_String == "interval")
   {
      ore_Type = C_OscDataLoggerJobProperties::eLLT_INTERVAL;
   }
   else
   {
      s32_Retval = C_RANGE;
   }

   return s32_Retval;
}
