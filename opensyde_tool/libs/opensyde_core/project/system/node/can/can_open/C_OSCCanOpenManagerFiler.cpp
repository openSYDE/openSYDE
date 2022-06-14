//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Filer for CANopen data classes

   Filer for CANopen data classes

   \copyright   Copyright 2022 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.h"

#include "TGLFile.h"
#include "stwerrors.h"
#include "C_OSCUtils.h"
#include "C_OSCXMLParserLog.h"
#include "C_OSCLoggingHandler.h"
#include "C_OSCSystemFilerUtil.h"
#include "C_OSCCanOpenManagerFiler.h"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw_tgl;
using namespace stw_types;
using namespace stw_errors;
using namespace stw_opensyde_core;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */
const stw_types::uint16 C_OSCCanOpenManagerFiler::mhu16_FILE_VERSION_1 = 1;

/* -- Types --------------------------------------------------------------------------------------------------------- */

/* -- Global Variables ---------------------------------------------------------------------------------------------- */

/* -- Module Global Variables --------------------------------------------------------------------------------------- */

/* -- Module Global Function Prototypes ----------------------------------------------------------------------------- */

/* -- Implementation ------------------------------------------------------------------------------------------------ */

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Default constructor
*/
//----------------------------------------------------------------------------------------------------------------------
C_OSCCanOpenManagerFiler::C_OSCCanOpenManagerFiler()
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Load CANopen manager configs

   \param[in,out]  orc_Config    Config
   \param[in]      orc_Path      Path
   \param[in]      orc_BasePath  Base path

   \return
   C_NO_ERR    data read
   C_RANGE     specified system definition file does not exist
   C_NOACT     specified file is present but structure is invalid (e.g. invalid XML file)
   C_CONFIG    system definition file content is invalid or incomplete
               device definition file could not be loaded
*/
//----------------------------------------------------------------------------------------------------------------------
stw_types::sint32 C_OSCCanOpenManagerFiler::h_LoadFile(std::map<stw_types::uint8, C_OSCCanOpenManagerInfo> & orc_Config,
                                                       const stw_scl::C_SCLString & orc_Path,
                                                       const stw_scl::C_SCLString & orc_BasePath)
{
   sint32 s32_Retval = C_NO_ERR;

   if (TGL_FileExists(orc_Path) == true)
   {
      C_OSCXMLParserLog c_XMLParser;
      c_XMLParser.SetLogHeading("Loading CANopen manager data");
      s32_Retval = c_XMLParser.LoadFromFile(orc_Path);
      if (s32_Retval == C_NO_ERR)
      {
         if (c_XMLParser.SelectRoot() == "opensyde-can-open-managers-config")
         {
            s32_Retval = h_LoadData(orc_Config, c_XMLParser, orc_BasePath);
         }
         else
         {
            osc_write_log_error("Loading CANopen manager data",
                                "Could not find \"opensyde-can-open-managers-config\" node.");
            s32_Retval = C_CONFIG;
         }
      }
      else
      {
         osc_write_log_error("Loading CANopen manager data", "File \"" + orc_Path + "\" could not be opened.");
         s32_Retval = C_NOACT;
      }
   }
   else
   {
      osc_write_log_error("Loading CANopen manager data", "File \"" + orc_Path + "\" does not exist.");
      s32_Retval = C_RANGE;
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Save CANopen manager configs

   \param[in]      orc_Config                Config
   \param[in]      orc_Path                  Path
   \param[in]      orc_BasePath              Base path
   \param[in,out]  opc_CreatedFiles          Created files
   \param[in]      orc_NodeIndicesToNameMap  Node indices to name map

   \return
   C_NO_ERR   data saved
   C_CONFIG   data invalid
   C_RD_WR    could not erase pre-existing file before saving
   C_RD_WR    could not write to file (e.g. missing write permissions; missing folder)
*/
//----------------------------------------------------------------------------------------------------------------------
stw_types::sint32 C_OSCCanOpenManagerFiler::h_SaveFile(const std::map<stw_types::uint8,
                                                                      C_OSCCanOpenManagerInfo> & orc_Config,
                                                       const stw_scl::C_SCLString & orc_Path,
                                                       const stw_scl::C_SCLString & orc_BasePath,
                                                       std::vector<stw_scl::C_SCLString> * const opc_CreatedFiles,
                                                       const std::map<uint32,
                                                                      stw_scl::C_SCLString> & orc_NodeIndicesToNameMap)
{
   C_OSCXMLParser c_XMLParser;
   sint32 s32_Retval = C_OSCSystemFilerUtil::h_GetParserForNewFile(c_XMLParser, orc_Path,
                                                                   "opensyde-can-open-managers-config");

   if (s32_Retval == C_NO_ERR)
   {
      //node
      s32_Retval = C_OSCCanOpenManagerFiler::h_SaveData(orc_Config, c_XMLParser, orc_BasePath, opc_CreatedFiles,
                                                        orc_NodeIndicesToNameMap);
      if (s32_Retval == C_NO_ERR)
      {
         //Don't forget to save!
         if (c_XMLParser.SaveToFile(orc_Path) != C_NO_ERR)
         {
            osc_write_log_error("Saving CANopen manager data", "Could not create file for node.");
            s32_Retval = C_CONFIG;
         }
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
/*! \brief  Load CANopen manager configs

   \param[in,out]  orc_Config       Config
   \param[in,out]  orc_XMLParser    XML parser
   \param[in]      orc_BasePath     Base path

   \return
   C_NO_ERR    data read
   C_CONFIG    CANopen manager file content is invalid or incomplete
*/
//----------------------------------------------------------------------------------------------------------------------
stw_types::sint32 C_OSCCanOpenManagerFiler::h_LoadData(std::map<stw_types::uint8, C_OSCCanOpenManagerInfo> & orc_Config,
                                                       C_OSCXMLParserBase & orc_XMLParser,
                                                       const stw_scl::C_SCLString & orc_BasePath)
{
   sint32 s32_Retval = orc_XMLParser.SelectNodeChildError("can-open-managers");

   orc_Config.clear();
   if (s32_Retval == C_NO_ERR)
   {
      uint32 u32_ExpectedSize;
      s32_Retval = orc_XMLParser.GetAttributeUint32Error("length", u32_ExpectedSize);
      if (s32_Retval == C_NO_ERR)
      {
         stw_scl::C_SCLString c_NodeName = orc_XMLParser.SelectNodeChild("can-open-manager");
         if (c_NodeName == "can-open-manager")
         {
            do
            {
               uint32 u32_Interface;
               s32_Retval = orc_XMLParser.GetAttributeUint32Error("interface", u32_Interface);
               if (s32_Retval == C_NO_ERR)
               {
                  C_OSCCanOpenManagerInfo c_CANOpenManager;
                  s32_Retval = C_OSCCanOpenManagerFiler::mh_LoadManagerData(c_CANOpenManager, orc_XMLParser,
                                                                            orc_BasePath);
                  if (s32_Retval == C_NO_ERR)
                  {
                     orc_Config[static_cast<uint8>(u32_Interface)] = c_CANOpenManager;
                  }
               }
               c_NodeName = orc_XMLParser.SelectNodeNext("can-open-manager");
            }
            while ((c_NodeName == "can-open-manager") && (s32_Retval == C_NO_ERR));
            tgl_assert(orc_XMLParser.SelectNodeParent() == "can-open-managers");
         }
         if (u32_ExpectedSize != orc_Config.size())
         {
            stw_scl::C_SCLString c_Tmp;
            c_Tmp.PrintFormatted("Unexpected CANopen manager count, expected: %i, got %i", u32_ExpectedSize,
                                 orc_Config.size());
            orc_XMLParser.ReportErrorForAttributeContentAppendXMLContext("length", c_Tmp);
         }
         if (s32_Retval == C_NO_ERR)
         {
            tgl_assert(orc_XMLParser.SelectNodeParent() == "opensyde-can-open-managers-config");
         }
      }
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Save CANopen manager configs

   \param[in]      orc_Config                Config
   \param[in,out]  orc_XMLParser             XML parser
   \param[in]      orc_BasePath              Base path
   \param[in,out]  opc_CreatedFiles          Created files
   \param[in]      orc_NodeIndicesToNameMap  Node indices to name map

   \return
   STW error codes

   \retval   C_NO_ERR   data saved
   \retval   C_CONFIG   data invalid
   \retval   C_RD_WR    could not erase pre-existing file before saving
   \retval   C_RD_WR    could not write to file (e.g. missing write permissions; missing folder)
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_OSCCanOpenManagerFiler::h_SaveData(const std::map<stw_types::uint8,
                                                           C_OSCCanOpenManagerInfo> & orc_Config,
                                            C_OSCXMLParserBase & orc_XMLParser,
                                            const stw_scl::C_SCLString & orc_BasePath,
                                            std::vector<stw_scl::C_SCLString> * const opc_CreatedFiles,
                                            const std::map<uint32,
                                                           stw_scl::C_SCLString> & orc_NodeIndicesToNameMap)
{
   sint32 s32_Retval = C_NO_ERR;

   //File version
   tgl_assert(orc_XMLParser.CreateAndSelectNodeChild("file-version") == "file-version");
   orc_XMLParser.SetNodeContent(stw_scl::C_SCLString::IntToStr(mhu16_FILE_VERSION_1));
   //Return
   orc_XMLParser.SelectNodeParent();
   tgl_assert(orc_XMLParser.CreateAndSelectNodeChild("can-open-managers") == "can-open-managers");
   orc_XMLParser.SetAttributeUint32("length", orc_Config.size());
   for (std::map<stw_types::uint8,
                 C_OSCCanOpenManagerInfo>::const_iterator c_It = orc_Config.begin();
        (c_It != orc_Config.end()) && (s32_Retval == C_NO_ERR); ++c_It)
   {
      tgl_assert(orc_XMLParser.CreateAndSelectNodeChild("can-open-manager") == "can-open-manager");
      orc_XMLParser.SetAttributeUint32("interface", c_It->first);
      s32_Retval = C_OSCCanOpenManagerFiler::mh_SaveManagerData(c_It->second, orc_XMLParser, orc_BasePath,
                                                                opc_CreatedFiles, orc_NodeIndicesToNameMap);
      tgl_assert(orc_XMLParser.SelectNodeParent() == "can-open-managers");
   }
   //Return
   orc_XMLParser.SelectNodeParent();
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Load manager data

   \param[in,out]  orc_Config       Config
   \param[in,out]  orc_XMLParser    XML parser
   \param[in]      orc_BasePath     Base path

   \return
   C_NO_ERR    data read
   C_CONFIG    CANopen manager file content is invalid or incomplete
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_OSCCanOpenManagerFiler::mh_LoadManagerData(C_OSCCanOpenManagerInfo & orc_Config,
                                                    C_OSCXMLParserBase & orc_XMLParser,
                                                    const stw_scl::C_SCLString & orc_BasePath)
{
   sint32 s32_Retval = C_OSCCanOpenManagerFiler::mh_LoadManagerProperties(orc_Config, orc_XMLParser);

   if (s32_Retval == C_NO_ERR)
   {
      s32_Retval = C_OSCCanOpenManagerFiler::mh_LoadManagerSubDevices(orc_Config.c_CanOpenDevices, orc_XMLParser,
                                                                      orc_BasePath);
   }

   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Save manager data

   \param[in]      orc_Config                Config
   \param[in,out]  orc_XMLParser             XML parser
   \param[in]      orc_BasePath              Base path
   \param[in,out]  opc_CreatedFiles          Created files
   \param[in]      orc_NodeIndicesToNameMap  Node indices to name map

   \return
   STW error codes

   \retval   C_NO_ERR   data saved
   \retval   C_CONFIG   data invalid
   \retval   C_RD_WR    could not erase pre-existing file before saving
   \retval   C_RD_WR    could not write to file (e.g. missing write permissions; missing folder)
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_OSCCanOpenManagerFiler::mh_SaveManagerData(const C_OSCCanOpenManagerInfo & orc_Config,
                                                    C_OSCXMLParserBase & orc_XMLParser,
                                                    const stw_scl::C_SCLString & orc_BasePath,
                                                    std::vector<stw_scl::C_SCLString> * const opc_CreatedFiles,
                                                    const std::map<uint32,
                                                                   stw_scl::C_SCLString> & orc_NodeIndicesToNameMap)
{
   C_OSCCanOpenManagerFiler::mh_SaveManagerProperties(orc_Config, orc_XMLParser);
   return C_OSCCanOpenManagerFiler::mh_SaveManagerSubDevices(orc_Config.c_CanOpenDevices, orc_XMLParser, orc_BasePath,
                                                             opc_CreatedFiles, orc_NodeIndicesToNameMap);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Load manager properties

   \param[in,out]  orc_Config       Config
   \param[in,out]  orc_XMLParser    XML parser

   \return
   C_NO_ERR    data read
   C_CONFIG    CANopen manager file content is invalid or incomplete
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_OSCCanOpenManagerFiler::mh_LoadManagerProperties(C_OSCCanOpenManagerInfo & orc_Config,
                                                          C_OSCXMLParserBase & orc_XMLParser)
{
   sint32 s32_Retval = orc_XMLParser.SelectNodeChildError("properties");

   if (s32_Retval == C_NO_ERR)
   {
      uint32 u32_Value;
      s32_Retval = orc_XMLParser.GetAttributeBoolError("use-opensyde-id", orc_Config.q_UseOpenSYDENodeID);

      if (s32_Retval == C_NO_ERR)
      {
         s32_Retval = orc_XMLParser.GetAttributeUint32Error("node-id-value", u32_Value);
         if (s32_Retval == C_NO_ERR)
         {
            orc_Config.u8_NodeIDValue = static_cast<uint8>(u32_Value);
         }
      }
      if (s32_Retval == C_NO_ERR)
      {
         s32_Retval = orc_XMLParser.GetAttributeUint32Error("global-sdo-timeout-ms", u32_Value);
         if (s32_Retval == C_NO_ERR)
         {
            orc_Config.u16_GlobalSDOTimeoutMs = static_cast<uint16>(u32_Value);
         }
      }
      if (s32_Retval == C_NO_ERR)
      {
         s32_Retval = orc_XMLParser.GetAttributeBoolError("autostart-can-open-manager",
                                                          orc_Config.q_AutostartCanOpenManager);
      }
      if (s32_Retval == C_NO_ERR)
      {
         s32_Retval = orc_XMLParser.GetAttributeBoolError("start-devices",
                                                          orc_Config.q_StartDevices);
      }
      if (s32_Retval == C_NO_ERR)
      {
         s32_Retval = orc_XMLParser.GetAttributeBoolError("NMT-start-all",
                                                          orc_Config.q_NMTStartAll);
      }
      if (s32_Retval == C_NO_ERR)
      {
         s32_Retval = orc_XMLParser.GetAttributeBoolError("enable-heartbeat-producing",
                                                          orc_Config.q_EnableHeartbeatProducing);
      }
      if (s32_Retval == C_NO_ERR)
      {
         s32_Retval = orc_XMLParser.GetAttributeUint32Error("heartbeat-producer-time-ms", u32_Value);
         if (s32_Retval == C_NO_ERR)
         {
            orc_Config.u16_HeartbeatProducerTimeMs = static_cast<uint16>(u32_Value);
         }
      }
      if (s32_Retval == C_NO_ERR)
      {
         s32_Retval = orc_XMLParser.SelectNodeChildError("nmt-error-behaviour");
         if (s32_Retval == C_NO_ERR)
         {
            const stw_scl::C_SCLString c_Text = orc_XMLParser.GetNodeContent();
            s32_Retval = C_OSCCanOpenManagerFiler::mh_StringToCanOpenManagerInfoType(c_Text,
                                                                                     orc_Config.e_NMTErrorBehaviour);
            if (s32_Retval == C_NO_ERR)
            {
               tgl_assert(orc_XMLParser.SelectNodeParent() == "properties");
            }
            else
            {
               orc_XMLParser.ReportErrorForNodeContentAppendXMLContext(
                  "Could not interpret \"" + c_Text + "\" as a valid NMT error behaviour type");
            }
         }
      }
      if (s32_Retval == C_NO_ERR)
      {
         tgl_assert(orc_XMLParser.SelectNodeParent() == "can-open-manager");
      }
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Save manager properties

   \param[in]      orc_Config       Config
   \param[in,out]  orc_XMLParser    XML parser
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OSCCanOpenManagerFiler::mh_SaveManagerProperties(const C_OSCCanOpenManagerInfo & orc_Config,
                                                        C_OSCXMLParserBase & orc_XMLParser)
{
   orc_XMLParser.CreateAndSelectNodeChild("properties");
   orc_XMLParser.SetAttributeBool("use-opensyde-id", orc_Config.q_UseOpenSYDENodeID);
   orc_XMLParser.SetAttributeUint32("node-id-value", static_cast<uint32>(orc_Config.u8_NodeIDValue));
   orc_XMLParser.SetAttributeUint32("global-sdo-timeout-ms", static_cast<uint32>(orc_Config.u16_GlobalSDOTimeoutMs));
   orc_XMLParser.SetAttributeBool("autostart-can-open-manager", orc_Config.q_AutostartCanOpenManager);
   orc_XMLParser.SetAttributeBool("start-devices", orc_Config.q_StartDevices);
   orc_XMLParser.SetAttributeBool("NMT-start-all", orc_Config.q_NMTStartAll);
   orc_XMLParser.SetAttributeBool("enable-heartbeat-producing", orc_Config.q_EnableHeartbeatProducing);
   orc_XMLParser.SetAttributeUint32("heartbeat-producer-time-ms",
                                    static_cast<uint32>(orc_Config.u16_HeartbeatProducerTimeMs));
   orc_XMLParser.CreateNodeChild("nmt-error-behaviour",
                                 C_OSCCanOpenManagerFiler::mh_CanOpenManagerInfoTypeToString(orc_Config.
                                                                                             e_NMTErrorBehaviour));
   tgl_assert(orc_XMLParser.SelectNodeParent() == "can-open-manager");
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Load manager sub devices

   \param[in,out]  orc_Config       Config
   \param[in,out]  orc_XMLParser    XML parser
   \param[in]      orc_BasePath     Base path

   \return
   C_NO_ERR    data read
   C_CONFIG    CANopen manager file content is invalid or incomplete
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_OSCCanOpenManagerFiler::mh_LoadManagerSubDevices(std::map<C_OSCCanInterfaceId,
                                                                   C_OSCCanOpenManagerDeviceInfo> & orc_Config,
                                                          C_OSCXMLParserBase & orc_XMLParser,
                                                          const stw_scl::C_SCLString & orc_BasePath)
{
   sint32 s32_Retval = orc_XMLParser.SelectNodeChildError("can-open-devices");

   orc_Config.clear();
   if (s32_Retval == C_NO_ERR)
   {
      uint32 u32_ExpectedSize;
      s32_Retval = orc_XMLParser.GetAttributeUint32Error("length", u32_ExpectedSize);
      if (s32_Retval == C_NO_ERR)
      {
         stw_scl::C_SCLString c_NodeName = orc_XMLParser.SelectNodeChild("can-open-device");
         if (c_NodeName == "can-open-device")
         {
            do
            {
               s32_Retval = orc_XMLParser.SelectNodeChildError("interface-id");
               if (s32_Retval == C_NO_ERR)
               {
                  uint32 u32_Value;
                  s32_Retval = orc_XMLParser.GetAttributeUint32Error("node-index", u32_Value);
                  if (s32_Retval == C_NO_ERR)
                  {
                     C_OSCCanInterfaceId c_InterfaceId;
                     c_InterfaceId.u32_NodeIndex = u32_Value;
                     s32_Retval = orc_XMLParser.GetAttributeUint32Error("interface-id", u32_Value);
                     if (s32_Retval == C_NO_ERR)
                     {
                        C_OSCCanOpenManagerDeviceInfo c_DeviceInfo;
                        c_InterfaceId.u8_InterfaceNumber = static_cast<uint8>(u32_Value);
                        tgl_assert(orc_XMLParser.SelectNodeParent() == "can-open-device");
                        s32_Retval = C_OSCCanOpenManagerFiler::mh_LoadManagerSubDevice(c_DeviceInfo, orc_XMLParser,
                                                                                       orc_BasePath);
                        if (s32_Retval == C_NO_ERR)
                        {
                           orc_Config[c_InterfaceId] = c_DeviceInfo;
                        }
                     }
                  }
               }
               c_NodeName = orc_XMLParser.SelectNodeNext("can-open-device");
            }
            while ((c_NodeName == "can-open-device") && (s32_Retval == C_NO_ERR));
            if (s32_Retval == C_NO_ERR)
            {
               tgl_assert(orc_XMLParser.SelectNodeParent() == "can-open-devices");
            }
         }
         if (u32_ExpectedSize != orc_Config.size())
         {
            stw_scl::C_SCLString c_Tmp;
            c_Tmp.PrintFormatted("Unexpected can open device count, expected: %i, got %i", u32_ExpectedSize,
                                 orc_Config.size());
            orc_XMLParser.ReportErrorForAttributeContentAppendXMLContext("length", c_Tmp);
         }
         if (s32_Retval == C_NO_ERR)
         {
            tgl_assert(orc_XMLParser.SelectNodeParent() == "can-open-manager");
         }
      }
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Save manager sub devices

   \param[in]      orc_Config                Config
   \param[in,out]  orc_XMLParser             XML parser
   \param[in]      orc_BasePath              Base path
   \param[in,out]  opc_CreatedFiles          Created files
   \param[in]      orc_NodeIndicesToNameMap  Node indices to name map

   \return
   STW error codes

   \retval   C_NO_ERR   data saved
   \retval   C_CONFIG   data invalid
   \retval   C_RD_WR    could not erase pre-existing file before saving
   \retval   C_RD_WR    could not write to file (e.g. missing write permissions; missing folder)
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_OSCCanOpenManagerFiler::mh_SaveManagerSubDevices(const std::map<C_OSCCanInterfaceId,
                                                                         C_OSCCanOpenManagerDeviceInfo> & orc_Config,
                                                          C_OSCXMLParserBase & orc_XMLParser,
                                                          const stw_scl::C_SCLString & orc_BasePath,
                                                          std::vector<stw_scl::C_SCLString> * const opc_CreatedFiles,
                                                          const std::map<uint32,
                                                                         stw_scl::C_SCLString> & orc_NodeIndicesToNameMap)
{
   sint32 s32_Retval = C_NO_ERR;

   orc_XMLParser.CreateAndSelectNodeChild("can-open-devices");
   orc_XMLParser.SetAttributeUint32("length", orc_Config.size());
   for (std::map<C_OSCCanInterfaceId,
                 C_OSCCanOpenManagerDeviceInfo>::const_iterator c_It = orc_Config.begin();
        (c_It != orc_Config.end()) && (s32_Retval == C_NO_ERR);
        ++c_It)
   {
      const std::map<uint32,
                     stw_scl::C_SCLString>::const_iterator c_FoundName = orc_NodeIndicesToNameMap.find(
         c_It->first.u32_NodeIndex);
      orc_XMLParser.CreateAndSelectNodeChild("can-open-device");
      orc_XMLParser.CreateAndSelectNodeChild("interface-id");
      orc_XMLParser.SetAttributeUint32("node-index", c_It->first.u32_NodeIndex);
      orc_XMLParser.SetAttributeUint32("interface-id", static_cast<uint32>(c_It->first.u8_InterfaceNumber));
      tgl_assert(orc_XMLParser.SelectNodeParent() == "can-open-device");
      if (c_FoundName != orc_NodeIndicesToNameMap.end())
      {
         s32_Retval = C_OSCCanOpenManagerFiler::mh_SaveManagerSubDevice(c_It->second, orc_XMLParser, orc_BasePath,
                                                                        opc_CreatedFiles, c_FoundName->second,
                                                                        c_It->first.u8_InterfaceNumber);
      }
      else
      {
         s32_Retval = C_CONFIG;
         osc_write_log_error("saving canopen manager",
                             "could not find index " + stw_scl::C_SCLString::IntToStr(
                                c_It->first.u32_NodeIndex) + " in parameter orc_NodeIndicesToNameMap");
      }
      tgl_assert(orc_XMLParser.SelectNodeParent() == "can-open-devices");
   }
   tgl_assert(orc_XMLParser.SelectNodeParent() == "can-open-manager");
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Load manager sub device

   \param[in,out]  orc_Config       Config
   \param[in,out]  orc_XMLParser    XML parser
   \param[in]      orc_BasePath     Base path

   \return
   C_NO_ERR    data read
   C_CONFIG    CANopen manager file content is invalid or incomplete
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_OSCCanOpenManagerFiler::mh_LoadManagerSubDevice(C_OSCCanOpenManagerDeviceInfo & orc_Config,
                                                         C_OSCXMLParserBase & orc_XMLParser,
                                                         const stw_scl::C_SCLString & orc_BasePath)
{
   sint32 s32_Retval = orc_XMLParser.SelectNodeChildError("properties");

   if (s32_Retval == C_NO_ERR)
   {
      uint32 u32_Value;
      s32_Retval = orc_XMLParser.GetAttributeBoolError("device-optional", orc_Config.q_DeviceOptional);
      if (s32_Retval == C_NO_ERR)
      {
         s32_Retval = orc_XMLParser.GetAttributeBoolError("no-initialization", orc_Config.q_NoInitialization);
      }
      if (s32_Retval == C_NO_ERR)
      {
         s32_Retval =
            orc_XMLParser.GetAttributeBoolError("factory-settings-active", orc_Config.q_FactorySettingsActive);
      }
      if (s32_Retval == C_NO_ERR)
      {
         s32_Retval =
            orc_XMLParser.GetAttributeUint32Error("reset-node-object-dictionary-sub-index", u32_Value);
         orc_Config.u8_ResetNodeObjectDictionarySubIndex = static_cast<uint8>(u32_Value);
      }
      if (s32_Retval == C_NO_ERR)
      {
         s32_Retval =
            orc_XMLParser.GetAttributeBoolError("enable-heartbeat-producing", orc_Config.q_EnableHeartbeatProducing);
      }
      if (s32_Retval == C_NO_ERR)
      {
         s32_Retval =
            orc_XMLParser.GetAttributeUint32Error("heartbeat-producer-time-ms", u32_Value);
         orc_Config.u16_HeartbeatProducerTimeMs = static_cast<uint16>(u32_Value);
      }
      if (s32_Retval == C_NO_ERR)
      {
         s32_Retval =
            orc_XMLParser.GetAttributeBoolError("use-opensyde-id", orc_Config.q_UseOpenSYDENodeID);
      }
      if (s32_Retval == C_NO_ERR)
      {
         s32_Retval =
            orc_XMLParser.GetAttributeUint32Error("node-id-value", u32_Value);
         orc_Config.u8_NodeIDValue = static_cast<uint8>(u32_Value);
      }
      if (s32_Retval == C_NO_ERR)
      {
         s32_Retval =
            orc_XMLParser.GetAttributeBoolError("enable-heartbeat-consuming", orc_Config.q_EnableHeartbeatConsuming);
      }
      if (s32_Retval == C_NO_ERR)
      {
         s32_Retval =
            orc_XMLParser.GetAttributeUint32Error("heartbeat-consumer-time-ms", u32_Value);
         orc_Config.u16_HeartbeatConsumerTimeMs = static_cast<uint16>(u32_Value);
      }
      if (s32_Retval == C_NO_ERR)
      {
         s32_Retval =
            orc_XMLParser.GetAttributeBoolError("enable-heartbeat-consuming-auto-calculation",
                                                orc_Config.q_EnableHeartbeatConsumingAutoCalculation);
      }
      if (s32_Retval == C_NO_ERR)
      {
         s32_Retval = C_OSCCanOpenManagerFiler::mh_LoadManagerSubDeviceEDSPart(orc_Config, orc_XMLParser, orc_BasePath);
      }
      if (s32_Retval == C_NO_ERR)
      {
         tgl_assert(orc_XMLParser.SelectNodeParent() == "can-open-device");
      }
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Save manager sub device

   \param[in]      orc_Config             Config
   \param[in,out]  orc_XMLParser          XML parser
   \param[in]      orc_BasePath           Base path
   \param[in,out]  opc_CreatedFiles       Created files
   \param[in]      orc_NodeName           Node name
   \param[in]      ou8_InterfaceNumber    Interface number

   \return
   STW error codes

   \retval   C_NO_ERR   data saved
   \retval   C_RD_WR    could not erase pre-existing file before saving
   \retval   C_RD_WR    could not write to file (e.g. missing write permissions; missing folder)
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_OSCCanOpenManagerFiler::mh_SaveManagerSubDevice(const C_OSCCanOpenManagerDeviceInfo & orc_Config,
                                                         C_OSCXMLParserBase & orc_XMLParser,
                                                         const stw_scl::C_SCLString & orc_BasePath,
                                                         std::vector<stw_scl::C_SCLString> * const opc_CreatedFiles,
                                                         const stw_scl::C_SCLString & orc_NodeName,
                                                         const stw_types::uint8 ou8_InterfaceNumber)
{
   sint32 s32_Retval;

   orc_XMLParser.CreateAndSelectNodeChild("properties");
   orc_XMLParser.SetAttributeBool("device-optional", orc_Config.q_DeviceOptional);
   orc_XMLParser.SetAttributeBool("no-initialization", orc_Config.q_NoInitialization);
   orc_XMLParser.SetAttributeBool("factory-settings-active", orc_Config.q_FactorySettingsActive);
   orc_XMLParser.SetAttributeUint32("reset-node-object-dictionary-sub-index",
                                    static_cast<uint32>(orc_Config.u8_ResetNodeObjectDictionarySubIndex));
   orc_XMLParser.SetAttributeBool("enable-heartbeat-producing", orc_Config.q_EnableHeartbeatProducing);
   orc_XMLParser.SetAttributeUint32("heartbeat-producer-time-ms",
                                    static_cast<uint32>(orc_Config.u16_HeartbeatProducerTimeMs));
   orc_XMLParser.SetAttributeBool("use-opensyde-id", orc_Config.q_UseOpenSYDENodeID);
   orc_XMLParser.SetAttributeUint32("node-id-value",
                                    static_cast<uint32>(orc_Config.u8_NodeIDValue));
   orc_XMLParser.SetAttributeBool("enable-heartbeat-consuming", orc_Config.q_EnableHeartbeatConsuming);
   orc_XMLParser.SetAttributeUint32("heartbeat-consumer-time-ms",
                                    static_cast<uint32>(orc_Config.u16_HeartbeatConsumerTimeMs));
   orc_XMLParser.SetAttributeBool("enable-heartbeat-consuming-auto-calculation",
                                  orc_Config.q_EnableHeartbeatConsumingAutoCalculation);
   s32_Retval = C_OSCCanOpenManagerFiler::mh_SaveManagerSubDeviceEDSPart(orc_Config, orc_XMLParser, orc_BasePath,
                                                                         opc_CreatedFiles, orc_NodeName,
                                                                         ou8_InterfaceNumber);
   tgl_assert(orc_XMLParser.SelectNodeParent() == "can-open-device");
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Load manager sub device EDS part

   \param[in,out]  orc_Config       Config
   \param[in,out]  orc_XMLParser    XML parser
   \param[in]      orc_BasePath     Base path

   \return
   C_NO_ERR    data read
   C_CONFIG    CANopen manager file content is invalid or incomplete
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_OSCCanOpenManagerFiler::mh_LoadManagerSubDeviceEDSPart(C_OSCCanOpenManagerDeviceInfo & orc_Config,
                                                                C_OSCXMLParserBase & orc_XMLParser,
                                                                const stw_scl::C_SCLString & orc_BasePath)
{
   sint32 s32_Retval =
      orc_XMLParser.SelectNodeChildError("eds-file-name");

   if (s32_Retval == C_NO_ERR)
   {
      const stw_scl::C_SCLString c_EDSFileName = orc_XMLParser.GetNodeContent();
      tgl_assert(orc_XMLParser.SelectNodeParent() == "properties");

      if (orc_BasePath.IsEmpty())
      {
         s32_Retval =
            orc_XMLParser.SelectNodeChildError("eds-file-content");
         if (s32_Retval == C_NO_ERR)
         {
            //Load EDS from string: not implemented
            tgl_assert(false);
            tgl_assert(orc_XMLParser.SelectNodeParent() == "properties");
         }
      }
      else
      {
         const stw_scl::C_SCLString c_CompleteFileName =
            C_OSCSystemFilerUtil::h_CombinePaths(orc_BasePath,
                                                 c_EDSFileName);
         if (TGL_FileExists(c_CompleteFileName))
         {
            s32_Retval = orc_Config.c_EDSFileContent.LoadFromFile(c_CompleteFileName);
            if (s32_Retval != C_NO_ERR)
            {
               orc_XMLParser.ReportErrorForNodeContentStartingWithXMLContext(
                  "Could not parse EDS file \"" + c_EDSFileName + "\" at path \"" + orc_BasePath + "\". Error: \"" +
                  orc_Config.c_EDSFileContent.GetLastErrorText() + "\"");
            }
         }
         else
         {
            s32_Retval = C_CONFIG;
            orc_XMLParser.ReportErrorForNodeContentStartingWithXMLContext(
               "Could not find file \"" + c_EDSFileName + "\" at path \"" + orc_BasePath + "\".");
         }
      }
   }
   if (s32_Retval == C_NO_ERR)
   {
      s32_Retval =
         orc_XMLParser.SelectNodeChildError("eds-original-file-name");
      orc_Config.c_EDSFileName = orc_XMLParser.GetNodeContent();
      tgl_assert(orc_XMLParser.SelectNodeParent() == "properties");
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Save manager sub device EDS part

   \param[in]      orc_Config             Config
   \param[in,out]  orc_XMLParser          XML parser
   \param[in]      orc_BasePath           Base path
   \param[in,out]  opc_CreatedFiles       Created files
   \param[in]      orc_NodeName           Node name
   \param[in]      ou8_InterfaceNumber    Interface number

   \return
   STW error codes

   \retval   C_NO_ERR   data saved
   \retval   C_RD_WR    could not erase pre-existing file before saving
   \retval   C_RD_WR    could not write to file (e.g. missing write permissions; missing folder)
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_OSCCanOpenManagerFiler::mh_SaveManagerSubDeviceEDSPart(const C_OSCCanOpenManagerDeviceInfo & orc_Config,
                                                                C_OSCXMLParserBase & orc_XMLParser,
                                                                const stw_scl::C_SCLString & orc_BasePath,
                                                                std::vector<stw_scl::C_SCLString> * const opc_CreatedFiles, const stw_scl::C_SCLString & orc_NodeName,
                                                                const stw_types::uint8 ou8_InterfaceNumber)
{
   sint32 s32_Retval = C_NO_ERR;
   const stw_scl::C_SCLString c_ItemPrefixUnprepared = orc_NodeName + "_can_" + stw_scl::C_SCLString::IntToStr(
      ou8_InterfaceNumber) + "_";
   const stw_scl::C_SCLString c_ItemPrefixPrepared = C_OSCSystemFilerUtil::h_PrepareItemNameForFileName(
      c_ItemPrefixUnprepared);
   const stw_scl::C_SCLString c_FileNameWithPrefix = c_ItemPrefixPrepared + orc_Config.c_EDSFileName;

   orc_XMLParser.CreateNodeChild("eds-file-name", c_FileNameWithPrefix);
   orc_XMLParser.CreateNodeChild("eds-original-file-name", orc_Config.c_EDSFileName);
   if (orc_BasePath.IsEmpty())
   {
      orc_XMLParser.CreateNodeChild("eds-file-content",
                                    orc_Config.c_EDSFileContent.c_InfoBlock.c_FileContentForSave);
   }
   else
   {
      const stw_scl::C_SCLString c_CompleteFileName =
         C_OSCSystemFilerUtil::h_CombinePaths(orc_BasePath, c_FileNameWithPrefix);
      s32_Retval = C_OSCSystemFilerUtil::h_SaveStringToFile(
         orc_Config.c_EDSFileContent.c_InfoBlock.c_FileContentForSave, c_CompleteFileName,
         "Saving CANopen manager data");
      if (opc_CreatedFiles != NULL)
      {
         opc_CreatedFiles->push_back(c_CompleteFileName);
      }
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Can open manager info type to string

   \param[in]  ore_Type    Type

   \return
   Stringified can open manager info type
*/
//----------------------------------------------------------------------------------------------------------------------
stw_scl::C_SCLString C_OSCCanOpenManagerFiler::mh_CanOpenManagerInfoTypeToString(
   const C_OSCCanOpenManagerInfo::E_NMTErrorBehaviourType & ore_Type)
{
   stw_scl::C_SCLString c_Retval;

   switch (ore_Type)
   {
   case C_OSCCanOpenManagerInfo::eRESTART_ALL_DEVICES:
      c_Retval = "restart all devices";
      break;
   case C_OSCCanOpenManagerInfo::eRESTART_FAILURE_DEVICE:
      c_Retval = "restart failure device";
      break;
   case C_OSCCanOpenManagerInfo::eSTOP_ALL_DEVICES:
      c_Retval = "stop all devices";
      break;
   default:
      c_Retval = "invalid";
      break;
   }
   return c_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  String to can open manager info type

   \param[in]   orc_String    String
   \param[out]  ore_Type      Type

   \return
   C_NO_ERR   no error
   C_RANGE    String unknown
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_OSCCanOpenManagerFiler::mh_StringToCanOpenManagerInfoType(const stw_scl::C_SCLString & orc_String,
                                                                   C_OSCCanOpenManagerInfo::E_NMTErrorBehaviourType & ore_Type)
{
   sint32 s32_Retval = C_NO_ERR;

   if (orc_String == "restart all devices")
   {
      ore_Type = C_OSCCanOpenManagerInfo::eRESTART_ALL_DEVICES;
   }
   else if (orc_String == "restart failure device")
   {
      ore_Type = C_OSCCanOpenManagerInfo::eRESTART_FAILURE_DEVICE;
   }
   else if (orc_String == "stop all devices")
   {
      ore_Type = C_OSCCanOpenManagerInfo::eSTOP_ALL_DEVICES;
   }
   else
   {
      s32_Retval = C_RANGE;
   }

   return s32_Retval;
}
