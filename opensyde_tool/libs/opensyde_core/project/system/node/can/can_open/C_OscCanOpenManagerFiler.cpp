//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Filer for CANopen data classes

   Filer for CANopen data classes

   \copyright   Copyright 2022 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.hpp"

#include "TglFile.hpp"
#include "stwerrors.hpp"
#include "C_OscUtils.hpp"
#include "C_OscXmlParserLog.hpp"
#include "C_OscNodeCommFiler.hpp"
#include "C_OscLoggingHandler.hpp"
#include "C_OscSystemFilerUtil.hpp"
#include "C_OscNodeDataPoolFiler.hpp"
#include "C_OscCanOpenManagerFiler.hpp"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw::tgl;

using namespace stw::errors;
using namespace stw::opensyde_core;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */
const uint16_t C_OscCanOpenManagerFiler::mhu16_FILE_VERSION_1 = 1;

/* -- Types --------------------------------------------------------------------------------------------------------- */

/* -- Global Variables ---------------------------------------------------------------------------------------------- */

/* -- Module Global Variables --------------------------------------------------------------------------------------- */

/* -- Module Global Function Prototypes ----------------------------------------------------------------------------- */

/* -- Implementation ------------------------------------------------------------------------------------------------ */

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Default constructor
*/
//----------------------------------------------------------------------------------------------------------------------
C_OscCanOpenManagerFiler::C_OscCanOpenManagerFiler()
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
int32_t C_OscCanOpenManagerFiler::h_LoadFile(std::map<uint8_t, C_OscCanOpenManagerInfo> & orc_Config,
                                             const stw::scl::C_SclString & orc_Path,
                                             const stw::scl::C_SclString & orc_BasePath)
{
   int32_t s32_Retval = C_NO_ERR;

   if (TglFileExists(orc_Path) == true)
   {
      C_OscXmlParserLog c_XmlParser;
      c_XmlParser.SetLogHeading("Loading CANopen manager data");
      s32_Retval = c_XmlParser.LoadFromFile(orc_Path);
      if (s32_Retval == C_NO_ERR)
      {
         if (c_XmlParser.SelectRoot() == "opensyde-can-open-managers-config")
         {
            s32_Retval = h_LoadData(orc_Config, c_XmlParser, orc_BasePath);
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
int32_t C_OscCanOpenManagerFiler::h_SaveFile(const std::map<uint8_t,
                                                            C_OscCanOpenManagerInfo> & orc_Config,
                                             const stw::scl::C_SclString & orc_Path,
                                             const stw::scl::C_SclString & orc_BasePath,
                                             std::vector<stw::scl::C_SclString> * const opc_CreatedFiles,
                                             const std::map<uint32_t,
                                                            stw::scl::C_SclString> & orc_NodeIndicesToNameMap)
{
   C_OscXmlParser c_XmlParser;
   int32_t s32_Retval = C_OscSystemFilerUtil::h_GetParserForNewFile(c_XmlParser, orc_Path,
                                                                    "opensyde-can-open-managers-config");

   if (s32_Retval == C_NO_ERR)
   {
      //node
      s32_Retval = C_OscCanOpenManagerFiler::h_SaveData(orc_Config, c_XmlParser, orc_BasePath, opc_CreatedFiles,
                                                        orc_NodeIndicesToNameMap);
      if (s32_Retval == C_NO_ERR)
      {
         //Don't forget to save!
         if (c_XmlParser.SaveToFile(orc_Path) != C_NO_ERR)
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
   \param[in,out]  orc_XmlParser    XML parser
   \param[in]      orc_BasePath     Base path

   \return
   C_NO_ERR    data read
   C_CONFIG    CANopen manager file content is invalid or incomplete
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_OscCanOpenManagerFiler::h_LoadData(std::map<uint8_t, C_OscCanOpenManagerInfo> & orc_Config,
                                             C_OscXmlParserBase & orc_XmlParser,
                                             const stw::scl::C_SclString & orc_BasePath)
{
   int32_t s32_Retval = orc_XmlParser.SelectNodeChildError("can-open-managers");

   orc_Config.clear();
   if (s32_Retval == C_NO_ERR)
   {
      uint32_t u32_ExpectedSize;
      s32_Retval = orc_XmlParser.GetAttributeUint32Error("length", u32_ExpectedSize);
      if (s32_Retval == C_NO_ERR)
      {
         stw::scl::C_SclString c_NodeName = orc_XmlParser.SelectNodeChild("can-open-manager");
         if (c_NodeName == "can-open-manager")
         {
            do
            {
               uint32_t u32_Interface;
               s32_Retval = orc_XmlParser.GetAttributeUint32Error("interface", u32_Interface);
               if (s32_Retval == C_NO_ERR)
               {
                  C_OscCanOpenManagerInfo c_CanOpenManager;
                  s32_Retval = C_OscCanOpenManagerFiler::mh_LoadManagerData(c_CanOpenManager, orc_XmlParser,
                                                                            orc_BasePath);
                  if (s32_Retval == C_NO_ERR)
                  {
                     orc_Config[static_cast<uint8_t>(u32_Interface)] = c_CanOpenManager;
                  }
               }
               c_NodeName = orc_XmlParser.SelectNodeNext("can-open-manager");
            }
            while ((c_NodeName == "can-open-manager") && (s32_Retval == C_NO_ERR));
            tgl_assert(orc_XmlParser.SelectNodeParent() == "can-open-managers");
         }
         if (u32_ExpectedSize != orc_Config.size())
         {
            stw::scl::C_SclString c_Tmp;
            c_Tmp.PrintFormatted("Unexpected CANopen manager count, expected: %u, got %u", u32_ExpectedSize,
                                 static_cast<uint32_t>(orc_Config.size()));
            orc_XmlParser.ReportErrorForAttributeContentAppendXmlContext("length", c_Tmp);
         }
      }
      tgl_assert(orc_XmlParser.SelectNodeParent() == "opensyde-can-open-managers-config");
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Save CANopen manager configs

   \param[in]      orc_Config                Config
   \param[in,out]  orc_XmlParser             XML parser
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
int32_t C_OscCanOpenManagerFiler::h_SaveData(const std::map<uint8_t,
                                                            C_OscCanOpenManagerInfo> & orc_Config,
                                             C_OscXmlParserBase & orc_XmlParser,
                                             const stw::scl::C_SclString & orc_BasePath,
                                             std::vector<stw::scl::C_SclString> * const opc_CreatedFiles,
                                             const std::map<uint32_t,
                                                            stw::scl::C_SclString> & orc_NodeIndicesToNameMap)
{
   int32_t s32_Retval = C_NO_ERR;

   //File version
   tgl_assert(orc_XmlParser.CreateAndSelectNodeChild("file-version") == "file-version");
   orc_XmlParser.SetNodeContent(stw::scl::C_SclString::IntToStr(mhu16_FILE_VERSION_1));
   //Return
   orc_XmlParser.SelectNodeParent();
   tgl_assert(orc_XmlParser.CreateAndSelectNodeChild("can-open-managers") == "can-open-managers");
   orc_XmlParser.SetAttributeUint32("length", static_cast<uint32_t>(orc_Config.size()));
   for (std::map<uint8_t,
                 C_OscCanOpenManagerInfo>::const_iterator c_It = orc_Config.begin();
        (c_It != orc_Config.end()) && (s32_Retval == C_NO_ERR); ++c_It)
   {
      tgl_assert(orc_XmlParser.CreateAndSelectNodeChild("can-open-manager") == "can-open-manager");
      orc_XmlParser.SetAttributeUint32("interface", c_It->first);
      s32_Retval = C_OscCanOpenManagerFiler::mh_SaveManagerData(c_It->second, orc_XmlParser, orc_BasePath,
                                                                opc_CreatedFiles, orc_NodeIndicesToNameMap);
      tgl_assert(orc_XmlParser.SelectNodeParent() == "can-open-managers");
   }
   //Return
   orc_XmlParser.SelectNodeParent();
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Load manager data

   \param[in,out]  orc_Config       Config
   \param[in,out]  orc_XmlParser    XML parser
   \param[in]      orc_BasePath     Base path

   \return
   C_NO_ERR    data read
   C_CONFIG    CANopen manager file content is invalid or incomplete
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_OscCanOpenManagerFiler::mh_LoadManagerData(C_OscCanOpenManagerInfo & orc_Config,
                                                     C_OscXmlParserBase & orc_XmlParser,
                                                     const stw::scl::C_SclString & orc_BasePath)
{
   int32_t s32_Retval = C_OscCanOpenManagerFiler::mh_LoadManagerProperties(orc_Config, orc_XmlParser);

   if (s32_Retval == C_NO_ERR)
   {
      s32_Retval = C_OscCanOpenManagerFiler::mh_LoadManagerSubDevices(orc_Config.c_CanOpenDevices, orc_XmlParser,
                                                                      orc_BasePath);
   }

   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Save manager data

   \param[in]      orc_Config                Config
   \param[in,out]  orc_XmlParser             XML parser
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
int32_t C_OscCanOpenManagerFiler::mh_SaveManagerData(const C_OscCanOpenManagerInfo & orc_Config,
                                                     C_OscXmlParserBase & orc_XmlParser,
                                                     const stw::scl::C_SclString & orc_BasePath,
                                                     std::vector<stw::scl::C_SclString> * const opc_CreatedFiles,
                                                     const std::map<uint32_t,
                                                                    stw::scl::C_SclString> & orc_NodeIndicesToNameMap)
{
   C_OscCanOpenManagerFiler::mh_SaveManagerProperties(orc_Config, orc_XmlParser);
   return C_OscCanOpenManagerFiler::mh_SaveManagerSubDevices(orc_Config.c_CanOpenDevices, orc_XmlParser, orc_BasePath,
                                                             opc_CreatedFiles, orc_NodeIndicesToNameMap);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Load manager properties

   \param[in,out]  orc_Config       Config
   \param[in,out]  orc_XmlParser    XML parser

   \return
   C_NO_ERR    data read
   C_CONFIG    CANopen manager file content is invalid or incomplete
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_OscCanOpenManagerFiler::mh_LoadManagerProperties(C_OscCanOpenManagerInfo & orc_Config,
                                                           C_OscXmlParserBase & orc_XmlParser)
{
   int32_t s32_Retval = orc_XmlParser.SelectNodeChildError("properties");

   if (s32_Retval == C_NO_ERR)
   {
      uint32_t u32_Value;
      s32_Retval = orc_XmlParser.GetAttributeBoolError("use-opensyde-id", orc_Config.q_UseOpenSydeNodeId);

      if (s32_Retval == C_NO_ERR)
      {
         s32_Retval = orc_XmlParser.GetAttributeUint32Error("node-id-value", u32_Value);
         if (s32_Retval == C_NO_ERR)
         {
            orc_Config.u8_NodeIdValue = static_cast<uint8_t>(u32_Value);
         }
      }
      if (s32_Retval == C_NO_ERR)
      {
         s32_Retval = orc_XmlParser.GetAttributeUint32Error("global-sdo-timeout-ms", u32_Value);
         if (s32_Retval == C_NO_ERR)
         {
            orc_Config.u16_GlobalSdoTimeoutMs = static_cast<uint16_t>(u32_Value);
         }
      }
      if (s32_Retval == C_NO_ERR)
      {
         s32_Retval = orc_XmlParser.GetAttributeBoolError("autostart-can-open-manager",
                                                          orc_Config.q_AutostartCanOpenManager);
      }
      if (s32_Retval == C_NO_ERR)
      {
         s32_Retval = orc_XmlParser.GetAttributeBoolError("start-devices",
                                                          orc_Config.q_StartDevices);
      }
      if (s32_Retval == C_NO_ERR)
      {
         s32_Retval = orc_XmlParser.GetAttributeBoolError("NMT-start-all",
                                                          orc_Config.q_NmtStartAll);
      }
      if (s32_Retval == C_NO_ERR)
      {
         s32_Retval = orc_XmlParser.GetAttributeBoolError("enable-heartbeat-producing",
                                                          orc_Config.q_EnableHeartbeatProducing);
      }
      if (s32_Retval == C_NO_ERR)
      {
         s32_Retval = orc_XmlParser.GetAttributeUint32Error("heartbeat-producer-time-ms", u32_Value);
         if (s32_Retval == C_NO_ERR)
         {
            orc_Config.u16_HeartbeatProducerTimeMs = static_cast<uint16_t>(u32_Value);
         }
      }
      if (s32_Retval == C_NO_ERR)
      {
         s32_Retval = C_OscCanOpenManagerFiler::mh_LoadManagerSyncProperties(orc_Config, orc_XmlParser);
      }
      if (s32_Retval == C_NO_ERR)
      {
         s32_Retval = orc_XmlParser.SelectNodeChildError("nmt-error-behaviour");
         if (s32_Retval == C_NO_ERR)
         {
            const stw::scl::C_SclString c_Text = orc_XmlParser.GetNodeContent();
            s32_Retval = C_OscCanOpenManagerFiler::mh_StringToCanOpenManagerInfoType(c_Text,
                                                                                     orc_Config.e_NmtErrorBehaviour);
            if (s32_Retval == C_NO_ERR)
            {
               tgl_assert(orc_XmlParser.SelectNodeParent() == "properties");
            }
            else
            {
               orc_XmlParser.ReportErrorForNodeContentAppendXmlContext(
                  "Could not interpret \"" + c_Text + "\" as a valid NMT error behaviour type");
            }
         }
      }
      tgl_assert(orc_XmlParser.SelectNodeParent() == "can-open-manager");
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Save manager properties

   \param[in]      orc_Config       Config
   \param[in,out]  orc_XmlParser    XML parser
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OscCanOpenManagerFiler::mh_SaveManagerProperties(const C_OscCanOpenManagerInfo & orc_Config,
                                                        C_OscXmlParserBase & orc_XmlParser)
{
   orc_XmlParser.CreateAndSelectNodeChild("properties");
   orc_XmlParser.SetAttributeBool("use-opensyde-id", orc_Config.q_UseOpenSydeNodeId);
   orc_XmlParser.SetAttributeUint32("node-id-value", static_cast<uint32_t>(orc_Config.u8_NodeIdValue));
   orc_XmlParser.SetAttributeUint32("global-sdo-timeout-ms", static_cast<uint32_t>(orc_Config.u16_GlobalSdoTimeoutMs));
   orc_XmlParser.SetAttributeBool("autostart-can-open-manager", orc_Config.q_AutostartCanOpenManager);
   orc_XmlParser.SetAttributeBool("start-devices", orc_Config.q_StartDevices);
   orc_XmlParser.SetAttributeBool("NMT-start-all", orc_Config.q_NmtStartAll);
   orc_XmlParser.SetAttributeBool("enable-heartbeat-producing", orc_Config.q_EnableHeartbeatProducing);
   orc_XmlParser.SetAttributeUint32("heartbeat-producer-time-ms",
                                    static_cast<uint32_t>(orc_Config.u16_HeartbeatProducerTimeMs));
   orc_XmlParser.CreateNodeChild("nmt-error-behaviour",
                                 C_OscCanOpenManagerFiler::mh_CanOpenManagerInfoTypeToString(orc_Config.
                                                                                             e_NmtErrorBehaviour));
   C_OscCanOpenManagerFiler::mh_SaveManagerSyncProperties(orc_Config, orc_XmlParser);
   tgl_assert(orc_XmlParser.SelectNodeParent() == "can-open-manager");
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Load manager sync properties

   \param[in,out]  orc_Config       Config
   \param[in,out]  orc_XmlParser    XML parser

   \return
   C_NO_ERR    data read
   C_CONFIG    CANopen manager file content is invalid or incomplete
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_OscCanOpenManagerFiler::mh_LoadManagerSyncProperties(C_OscCanOpenManagerInfo & orc_Config,
                                                               C_OscXmlParserBase & orc_XmlParser)
{
   int32_t s32_Retval = C_NO_ERR;

   if (orc_XmlParser.SelectNodeChild("sync-message") == "sync-message")
   {
      s32_Retval = orc_XmlParser.GetAttributeBoolError("produce", orc_Config.q_ProduceSyncMessage);
      if (s32_Retval == C_NO_ERR)
      {
         s32_Retval = orc_XmlParser.GetAttributeUint32Error("cycle-period-us", orc_Config.u32_SyncCyclePeriodUs);
      }
      if (s32_Retval == C_NO_ERR)
      {
         s32_Retval = orc_XmlParser.GetAttributeUint32Error("window-length-us", orc_Config.u32_SyncWindowLengthUs);
      }
      if (s32_Retval == C_NO_ERR)
      {
         tgl_assert(orc_XmlParser.SelectNodeParent() == "properties");
      }
   }
   else
   {
      orc_Config.q_ProduceSyncMessage = false;
      orc_Config.u32_SyncCyclePeriodUs = 20000UL;
      orc_Config.u32_SyncWindowLengthUs = 10000UL;
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Save manager sync properties

   \param[in]      orc_Config       Config
   \param[in,out]  orc_XmlParser    XML parser
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OscCanOpenManagerFiler::mh_SaveManagerSyncProperties(const C_OscCanOpenManagerInfo & orc_Config,
                                                            C_OscXmlParserBase & orc_XmlParser)
{
   orc_XmlParser.CreateAndSelectNodeChild("sync-message");
   orc_XmlParser.SetAttributeBool("produce", orc_Config.q_ProduceSyncMessage);
   orc_XmlParser.SetAttributeUint32("cycle-period-us", orc_Config.u32_SyncCyclePeriodUs);
   orc_XmlParser.SetAttributeUint32("window-length-us", orc_Config.u32_SyncWindowLengthUs);
   tgl_assert(orc_XmlParser.SelectNodeParent() == "properties");
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Load manager sub devices

   \param[in,out]  orc_Config       Config
   \param[in,out]  orc_XmlParser    XML parser
   \param[in]      orc_BasePath     Base path

   \return
   C_NO_ERR    data read
   C_CONFIG    CANopen manager file content is invalid or incomplete
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_OscCanOpenManagerFiler::mh_LoadManagerSubDevices(std::map<C_OscCanInterfaceId,
                                                                    C_OscCanOpenManagerDeviceInfo> & orc_Config,
                                                           C_OscXmlParserBase & orc_XmlParser,
                                                           const stw::scl::C_SclString & orc_BasePath)
{
   int32_t s32_Retval = orc_XmlParser.SelectNodeChildError("can-open-devices");

   orc_Config.clear();
   if (s32_Retval == C_NO_ERR)
   {
      uint32_t u32_ExpectedSize;
      s32_Retval = orc_XmlParser.GetAttributeUint32Error("length", u32_ExpectedSize);
      if (s32_Retval == C_NO_ERR)
      {
         stw::scl::C_SclString c_NodeName = orc_XmlParser.SelectNodeChild("can-open-device");
         if (c_NodeName == "can-open-device")
         {
            do
            {
               s32_Retval = orc_XmlParser.SelectNodeChildError("interface-id");
               if (s32_Retval == C_NO_ERR)
               {
                  uint32_t u32_Value;
                  s32_Retval = orc_XmlParser.GetAttributeUint32Error("node-index", u32_Value);
                  if (s32_Retval == C_NO_ERR)
                  {
                     C_OscCanInterfaceId c_InterfaceId;
                     c_InterfaceId.u32_NodeIndex = u32_Value;
                     s32_Retval = orc_XmlParser.GetAttributeUint32Error("interface-id", u32_Value);
                     if (s32_Retval == C_NO_ERR)
                     {
                        C_OscCanOpenManagerDeviceInfo c_DeviceInfo;
                        c_InterfaceId.u8_InterfaceNumber = static_cast<uint8_t>(u32_Value);
                        tgl_assert(orc_XmlParser.SelectNodeParent() == "can-open-device");
                        s32_Retval = C_OscCanOpenManagerFiler::mh_LoadManagerSubDevice(c_DeviceInfo, orc_XmlParser,
                                                                                       orc_BasePath);
                        if (s32_Retval == C_NO_ERR)
                        {
                           orc_Config[c_InterfaceId] = c_DeviceInfo;
                        }
                     }
                  }
               }
               c_NodeName = orc_XmlParser.SelectNodeNext("can-open-device");
            }
            while ((c_NodeName == "can-open-device") && (s32_Retval == C_NO_ERR));
            tgl_assert(orc_XmlParser.SelectNodeParent() == "can-open-devices");
         }
         if (u32_ExpectedSize != orc_Config.size())
         {
            stw::scl::C_SclString c_Tmp;
            c_Tmp.PrintFormatted("Unexpected can open device count, expected: %u, got %u", u32_ExpectedSize,
                                 static_cast<uint32_t>(orc_Config.size()));
            orc_XmlParser.ReportErrorForAttributeContentAppendXmlContext("length", c_Tmp);
         }
      }
      tgl_assert(orc_XmlParser.SelectNodeParent() == "can-open-manager");
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Save manager sub devices

   \param[in]      orc_Config                Config
   \param[in,out]  orc_XmlParser             XML parser
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
int32_t C_OscCanOpenManagerFiler::mh_SaveManagerSubDevices(const std::map<C_OscCanInterfaceId,
                                                                          C_OscCanOpenManagerDeviceInfo> & orc_Config,
                                                           C_OscXmlParserBase & orc_XmlParser,
                                                           const stw::scl::C_SclString & orc_BasePath,
                                                           std::vector<stw::scl::C_SclString> * const opc_CreatedFiles,
                                                           const std::map<uint32_t,
                                                                          stw::scl::C_SclString> & orc_NodeIndicesToNameMap)
{
   int32_t s32_Retval = C_NO_ERR;

   orc_XmlParser.CreateAndSelectNodeChild("can-open-devices");
   orc_XmlParser.SetAttributeUint32("length", static_cast<uint32_t>(orc_Config.size()));
   for (std::map<C_OscCanInterfaceId,
                 C_OscCanOpenManagerDeviceInfo>::const_iterator c_It = orc_Config.begin();
        (c_It != orc_Config.end()) && (s32_Retval == C_NO_ERR);
        ++c_It)
   {
      const std::map<uint32_t,
                     stw::scl::C_SclString>::const_iterator c_FoundName = orc_NodeIndicesToNameMap.find(
         c_It->first.u32_NodeIndex);
      orc_XmlParser.CreateAndSelectNodeChild("can-open-device");
      orc_XmlParser.CreateAndSelectNodeChild("interface-id");
      orc_XmlParser.SetAttributeUint32("node-index", c_It->first.u32_NodeIndex);
      orc_XmlParser.SetAttributeUint32("interface-id", static_cast<uint32_t>(c_It->first.u8_InterfaceNumber));
      tgl_assert(orc_XmlParser.SelectNodeParent() == "can-open-device");
      if (c_FoundName != orc_NodeIndicesToNameMap.end())
      {
         s32_Retval = C_OscCanOpenManagerFiler::mh_SaveManagerSubDevice(c_It->second, orc_XmlParser, orc_BasePath,
                                                                        opc_CreatedFiles, c_FoundName->second,
                                                                        c_It->first.u8_InterfaceNumber);
      }
      else
      {
         s32_Retval = C_CONFIG;
         osc_write_log_error("saving canopen manager",
                             "could not find index " + stw::scl::C_SclString::IntToStr(
                                c_It->first.u32_NodeIndex) + " in parameter orc_NodeIndicesToNameMap");
      }
      tgl_assert(orc_XmlParser.SelectNodeParent() == "can-open-devices");
   }
   tgl_assert(orc_XmlParser.SelectNodeParent() == "can-open-manager");
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Load manager sub device

   \param[in,out]  orc_Config       Config
   \param[in,out]  orc_XmlParser    XML parser
   \param[in]      orc_BasePath     Base path

   \return
   C_NO_ERR    data read
   C_CONFIG    CANopen manager file content is invalid or incomplete
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_OscCanOpenManagerFiler::mh_LoadManagerSubDevice(C_OscCanOpenManagerDeviceInfo & orc_Config,
                                                          C_OscXmlParserBase & orc_XmlParser,
                                                          const stw::scl::C_SclString & orc_BasePath)
{
   int32_t s32_Retval = orc_XmlParser.SelectNodeChildError("properties");

   if (s32_Retval == C_NO_ERR)
   {
      uint32_t u32_Value;
      s32_Retval = orc_XmlParser.GetAttributeBoolError("device-optional", orc_Config.q_DeviceOptional);
      if (s32_Retval == C_NO_ERR)
      {
         s32_Retval = orc_XmlParser.GetAttributeBoolError("no-initialization", orc_Config.q_NoInitialization);
      }
      if (s32_Retval == C_NO_ERR)
      {
         s32_Retval =
            orc_XmlParser.GetAttributeBoolError("factory-settings-active", orc_Config.q_FactorySettingsActive);
      }
      if (s32_Retval == C_NO_ERR)
      {
         s32_Retval =
            orc_XmlParser.GetAttributeUint32Error("reset-node-object-dictionary-sub-index", u32_Value);
         orc_Config.u8_ResetNodeObjectDictionarySubIndex = static_cast<uint8_t>(u32_Value);
      }
      if (s32_Retval == C_NO_ERR)
      {
         s32_Retval =
            orc_XmlParser.GetAttributeBoolError("enable-heartbeat-producing", orc_Config.q_EnableHeartbeatProducing);
      }
      if (s32_Retval == C_NO_ERR)
      {
         s32_Retval = orc_XmlParser.GetAttributeUint32Error("heartbeat-producer-time-ms", u32_Value);
         orc_Config.u16_HeartbeatProducerTimeMs = static_cast<uint16_t>(u32_Value);
      }
      if (s32_Retval == C_NO_ERR)
      {
         s32_Retval = orc_XmlParser.GetAttributeBoolError("use-opensyde-id", orc_Config.q_UseOpenSydeNodeId);
      }
      if (s32_Retval == C_NO_ERR)
      {
         s32_Retval =
            orc_XmlParser.GetAttributeUint32Error("node-id-value", u32_Value);
         orc_Config.u8_NodeIdValue = static_cast<uint8_t>(u32_Value);
      }
      if (s32_Retval == C_NO_ERR)
      {
         s32_Retval =
            orc_XmlParser.GetAttributeBoolError("enable-heartbeat-consuming", orc_Config.q_EnableHeartbeatConsuming);
      }
      if (s32_Retval == C_NO_ERR)
      {
         s32_Retval = orc_XmlParser.GetAttributeUint32Error("heartbeat-consumer-time-ms", u32_Value);
         orc_Config.u16_HeartbeatConsumerTimeMs = static_cast<uint16_t>(u32_Value);
      }
      if (s32_Retval == C_NO_ERR)
      {
         s32_Retval =
            orc_XmlParser.GetAttributeBoolError("enable-heartbeat-consuming-auto-calculation",
                                                orc_Config.q_EnableHeartbeatConsumingAutoCalculation);
      }
      if (s32_Retval == C_NO_ERR)
      {
         s32_Retval = C_OscCanOpenManagerFiler::mh_LoadManagerSubDeviceEdsPart(orc_Config, orc_XmlParser, orc_BasePath);
      }
      if (s32_Retval == C_NO_ERR)
      {
         s32_Retval = C_OscCanOpenManagerFiler::mh_LoadManagerMappedSignals(orc_Config.c_EdsFileMappableSignals,
                                                                            orc_XmlParser);
      }
      tgl_assert(orc_XmlParser.SelectNodeParent() == "can-open-device");
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Save manager sub device

   \param[in]      orc_Config             Config
   \param[in,out]  orc_XmlParser          XML parser
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
int32_t C_OscCanOpenManagerFiler::mh_SaveManagerSubDevice(const C_OscCanOpenManagerDeviceInfo & orc_Config,
                                                          C_OscXmlParserBase & orc_XmlParser,
                                                          const stw::scl::C_SclString & orc_BasePath,
                                                          std::vector<stw::scl::C_SclString> * const opc_CreatedFiles,
                                                          const stw::scl::C_SclString & orc_NodeName,
                                                          const uint8_t ou8_InterfaceNumber)
{
   int32_t s32_Retval;

   orc_XmlParser.CreateAndSelectNodeChild("properties");
   orc_XmlParser.SetAttributeBool("device-optional", orc_Config.q_DeviceOptional);
   orc_XmlParser.SetAttributeBool("no-initialization", orc_Config.q_NoInitialization);
   orc_XmlParser.SetAttributeBool("factory-settings-active", orc_Config.q_FactorySettingsActive);
   orc_XmlParser.SetAttributeUint32("reset-node-object-dictionary-sub-index",
                                    static_cast<uint32_t>(orc_Config.u8_ResetNodeObjectDictionarySubIndex));
   orc_XmlParser.SetAttributeBool("enable-heartbeat-producing", orc_Config.q_EnableHeartbeatProducing);
   orc_XmlParser.SetAttributeUint32("heartbeat-producer-time-ms",
                                    static_cast<uint32_t>(orc_Config.u16_HeartbeatProducerTimeMs));
   orc_XmlParser.SetAttributeBool("use-opensyde-id", orc_Config.q_UseOpenSydeNodeId);
   orc_XmlParser.SetAttributeUint32("node-id-value",
                                    static_cast<uint32_t>(orc_Config.u8_NodeIdValue));
   orc_XmlParser.SetAttributeBool("enable-heartbeat-consuming", orc_Config.q_EnableHeartbeatConsuming);
   orc_XmlParser.SetAttributeUint32("heartbeat-consumer-time-ms",
                                    static_cast<uint32_t>(orc_Config.u16_HeartbeatConsumerTimeMs));
   orc_XmlParser.SetAttributeBool("enable-heartbeat-consuming-auto-calculation",
                                  orc_Config.q_EnableHeartbeatConsumingAutoCalculation);
   s32_Retval = C_OscCanOpenManagerFiler::mh_SaveManagerSubDeviceEdsPart(orc_Config, orc_XmlParser, orc_BasePath,
                                                                         opc_CreatedFiles, orc_NodeName,
                                                                         ou8_InterfaceNumber);
   C_OscCanOpenManagerFiler::mh_SaveManagerMappedSignals(orc_Config.c_EdsFileMappableSignals, orc_XmlParser);
   tgl_assert(orc_XmlParser.SelectNodeParent() == "can-open-device");
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Load manager sub device EDS part

   \param[in,out]  orc_Config       Config
   \param[in,out]  orc_XmlParser    XML parser
   \param[in]      orc_BasePath     Base path

   \return
   C_NO_ERR    data read
   C_CONFIG    CANopen manager file content is invalid or incomplete
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_OscCanOpenManagerFiler::mh_LoadManagerSubDeviceEdsPart(C_OscCanOpenManagerDeviceInfo & orc_Config,
                                                                 C_OscXmlParserBase & orc_XmlParser,
                                                                 const stw::scl::C_SclString & orc_BasePath)
{
   int32_t s32_Retval =
      orc_XmlParser.SelectNodeChildError("eds-file-name");

   if (s32_Retval == C_NO_ERR)
   {
      const stw::scl::C_SclString c_EdsFileName = orc_XmlParser.GetNodeContent();
      tgl_assert(orc_XmlParser.SelectNodeParent() == "properties");

      if (orc_BasePath.IsEmpty())
      {
         s32_Retval =
            orc_XmlParser.SelectNodeChildError("eds-file-content");
         if (s32_Retval == C_NO_ERR)
         {
            //Load EDS from string: not implemented
            tgl_assert(false);
            tgl_assert(orc_XmlParser.SelectNodeParent() == "properties");
         }
      }
      else
      {
         const stw::scl::C_SclString c_CompleteFileName =
            C_OscSystemFilerUtil::h_CombinePaths(orc_BasePath,
                                                 c_EdsFileName);
         if (TglFileExists(c_CompleteFileName))
         {
            s32_Retval = orc_Config.c_EdsFileContent.LoadFromFile(c_CompleteFileName);
            if (s32_Retval != C_NO_ERR)
            {
               orc_XmlParser.ReportErrorForNodeContentStartingWithXmlContext(
                  "Could not parse EDS file \"" + c_EdsFileName + "\" at path \"" + orc_BasePath + "\". Error: \"" +
                  orc_Config.c_EdsFileContent.GetLastErrorText() + "\"");
            }
         }
         else
         {
            s32_Retval = C_CONFIG;
            orc_XmlParser.ReportErrorForNodeContentStartingWithXmlContext(
               "Could not find file \"" + c_EdsFileName + "\" at path \"" + orc_BasePath + "\".");
         }
      }
   }
   if (s32_Retval == C_NO_ERR)
   {
      s32_Retval =
         orc_XmlParser.SelectNodeChildError("eds-original-file-name");
      orc_Config.c_EdsFileName = orc_XmlParser.GetNodeContent();
      tgl_assert(orc_XmlParser.SelectNodeParent() == "properties");
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Save manager sub device EDS part

   \param[in]      orc_Config             Config
   \param[in,out]  orc_XmlParser          XML parser
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
int32_t C_OscCanOpenManagerFiler::mh_SaveManagerSubDeviceEdsPart(const C_OscCanOpenManagerDeviceInfo & orc_Config,
                                                                 C_OscXmlParserBase & orc_XmlParser,
                                                                 const stw::scl::C_SclString & orc_BasePath,
                                                                 std::vector<stw::scl::C_SclString> * const opc_CreatedFiles, const stw::scl::C_SclString & orc_NodeName,
                                                                 const uint8_t ou8_InterfaceNumber)
{
   int32_t s32_Retval = C_NO_ERR;
   const stw::scl::C_SclString c_ItemPrefixUnprepared = orc_NodeName + "_can_" + stw::scl::C_SclString::IntToStr(
      ou8_InterfaceNumber) + "_";
   const stw::scl::C_SclString c_ItemPrefixPrepared = C_OscSystemFilerUtil::h_PrepareItemNameForFileName(
      c_ItemPrefixUnprepared);
   const stw::scl::C_SclString c_FileNameWithPrefix = c_ItemPrefixPrepared + orc_Config.c_EdsFileName;

   orc_XmlParser.CreateNodeChild("eds-file-name", c_FileNameWithPrefix);
   orc_XmlParser.CreateNodeChild("eds-original-file-name", orc_Config.c_EdsFileName);
   if (orc_BasePath.IsEmpty())
   {
      orc_XmlParser.CreateNodeChild("eds-file-content",
                                    orc_Config.c_EdsFileContent.c_InfoBlock.c_FileContentForSave.GetText());
   }
   else
   {
      const stw::scl::C_SclString c_CompleteFileName =
         C_OscSystemFilerUtil::h_CombinePaths(orc_BasePath, c_FileNameWithPrefix);
      //only use "\n" as separator; SaveStringToFile will add an \r anyways
      s32_Retval = C_OscSystemFilerUtil::h_SaveStringToFile(
         orc_Config.c_EdsFileContent.c_InfoBlock.c_FileContentForSave.GetText("\n"), c_CompleteFileName,
         "Saving CANopen manager data");
      if (opc_CreatedFiles != NULL)
      {
         opc_CreatedFiles->push_back(c_FileNameWithPrefix);
      }
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Load manager mapped signals

   \param[in,out]  orc_Config       Config
   \param[in,out]  orc_XmlParser    XML parser

   \return
   C_NO_ERR    data read
   C_CONFIG    CANopen manager file content is invalid or incomplete
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_OscCanOpenManagerFiler::mh_LoadManagerMappedSignals(
   std::vector<C_OscCanOpenManagerMappableSignal> & orc_Config, C_OscXmlParserBase & orc_XmlParser)
{
   int32_t s32_Retval = C_NO_ERR;

   orc_Config.clear();
   if (orc_XmlParser.SelectNodeChild("mappable-signals") == "mappable-signals")
   {
      uint32_t u32_ExpectedSize;
      s32_Retval = orc_XmlParser.GetAttributeUint32Error("length", u32_ExpectedSize);
      if (s32_Retval == C_NO_ERR)
      {
         stw::scl::C_SclString c_NodeName = orc_XmlParser.SelectNodeChild("mappable-signal");
         if (c_NodeName == "mappable-signal")
         {
            orc_Config.reserve(u32_ExpectedSize);
            do
            {
               C_OscCanOpenManagerMappableSignal c_Signal;
               s32_Retval = C_OscCanOpenManagerFiler::mh_LoadManagerMappedSignal(c_Signal, orc_XmlParser);
               if (s32_Retval == C_NO_ERR)
               {
                  orc_Config.push_back(c_Signal);
               }

               c_NodeName = orc_XmlParser.SelectNodeNext("mappable-signal");
            }
            while ((c_NodeName == "mappable-signal") && (s32_Retval == C_NO_ERR));
            tgl_assert(orc_XmlParser.SelectNodeParent() == "mappable-signals");
         }
         if (u32_ExpectedSize != orc_Config.size())
         {
            stw::scl::C_SclString c_Tmp;
            c_Tmp.PrintFormatted("Unexpected mappable signal count, expected: %u, got %u", u32_ExpectedSize,
                                 static_cast<uint32_t>(orc_Config.size()));
            orc_XmlParser.ReportErrorForAttributeContentAppendXmlContext("length", c_Tmp);
         }
      }
      tgl_assert(orc_XmlParser.SelectNodeParent() == "properties");
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Save manager mapped signals

   \param[in]      orc_Config       Config
   \param[in,out]  orc_XmlParser    XML parser
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OscCanOpenManagerFiler::mh_SaveManagerMappedSignals(
   const std::vector<C_OscCanOpenManagerMappableSignal> & orc_Config, C_OscXmlParserBase & orc_XmlParser)
{
   orc_XmlParser.CreateAndSelectNodeChild("mappable-signals");
   orc_XmlParser.SetAttributeUint32("length", static_cast<uint32_t>(orc_Config.size()));
   for (std::vector<C_OscCanOpenManagerMappableSignal>::const_iterator c_It = orc_Config.begin();
        c_It != orc_Config.end();
        ++c_It)
   {
      orc_XmlParser.CreateAndSelectNodeChild("mappable-signal");
      C_OscCanOpenManagerFiler::mh_SaveManagerMappedSignal(*c_It, orc_XmlParser);
      tgl_assert(orc_XmlParser.SelectNodeParent() == "mappable-signals");
   }
   tgl_assert(orc_XmlParser.SelectNodeParent() == "properties");
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Load manager mapped signal

   \param[in,out]  orc_Config       Config
   \param[in,out]  orc_XmlParser    XML parser

   \return
   C_NO_ERR    data read
   C_CONFIG    CANopen manager file content is invalid or incomplete
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_OscCanOpenManagerFiler::mh_LoadManagerMappedSignal(C_OscCanOpenManagerMappableSignal & orc_Config,
                                                             C_OscXmlParserBase & orc_XmlParser)
{
   int32_t s32_Retval = orc_XmlParser.GetAttributeBoolError("is-auto-min-max-used", orc_Config.q_AutoMinMaxUsed);

   if (s32_Retval == C_NO_ERR)
   {
      s32_Retval = orc_XmlParser.SelectNodeChildError("com-signal");
      if (s32_Retval == C_NO_ERR)
      {
         s32_Retval = C_OscNodeCommFiler::h_LoadNodeComSignal(orc_Config.c_SignalData, orc_XmlParser, true);
      }
      tgl_assert(orc_XmlParser.SelectNodeParent() == "mappable-signal");
   }
   if (s32_Retval == C_NO_ERR)
   {
      s32_Retval = orc_XmlParser.SelectNodeChildError("data-element");
      if (s32_Retval == C_NO_ERR)
      {
         s32_Retval = C_OscNodeDataPoolFiler::h_LoadDataPoolElement(orc_Config.c_DatapoolData, orc_XmlParser);
      }
      tgl_assert(orc_XmlParser.SelectNodeParent() == "mappable-signal");
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Save manager mapped signal

   \param[in]      orc_Config       Config
   \param[in,out]  orc_XmlParser    XML parser
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OscCanOpenManagerFiler::mh_SaveManagerMappedSignal(const C_OscCanOpenManagerMappableSignal & orc_Config,
                                                          C_OscXmlParserBase & orc_XmlParser)
{
   orc_XmlParser.SetAttributeBool("is-auto-min-max-used", orc_Config.q_AutoMinMaxUsed);
   orc_XmlParser.CreateAndSelectNodeChild("com-signal");
   C_OscNodeCommFiler::h_SaveNodeComSignal(orc_Config.c_SignalData, orc_XmlParser, C_OscCanProtocol::eCAN_OPEN);
   tgl_assert(orc_XmlParser.SelectNodeParent() == "mappable-signal");
   orc_XmlParser.CreateAndSelectNodeChild("data-element");
   C_OscNodeDataPoolFiler::h_SaveDataPoolElement(orc_Config.c_DatapoolData, orc_XmlParser, C_OscNodeDataPool::eCOM);
   tgl_assert(orc_XmlParser.SelectNodeParent() == "mappable-signal");
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Can open manager info type to string

   \param[in]  ore_Type    Type

   \return
   Stringified can open manager info type
*/
//----------------------------------------------------------------------------------------------------------------------
stw::scl::C_SclString C_OscCanOpenManagerFiler::mh_CanOpenManagerInfoTypeToString(
   const C_OscCanOpenManagerInfo::E_NmtErrorBehaviourType & ore_Type)
{
   stw::scl::C_SclString c_Retval;

   switch (ore_Type)
   {
   case C_OscCanOpenManagerInfo::eRESTART_ALL_DEVICES:
      c_Retval = "restart all devices";
      break;
   case C_OscCanOpenManagerInfo::eRESTART_FAILURE_DEVICE:
      c_Retval = "restart failure device";
      break;
   case C_OscCanOpenManagerInfo::eSTOP_ALL_DEVICES:
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
int32_t C_OscCanOpenManagerFiler::mh_StringToCanOpenManagerInfoType(const stw::scl::C_SclString & orc_String,
                                                                    C_OscCanOpenManagerInfo::E_NmtErrorBehaviourType & ore_Type)
{
   int32_t s32_Retval = C_NO_ERR;

   if (orc_String == "restart all devices")
   {
      ore_Type = C_OscCanOpenManagerInfo::eRESTART_ALL_DEVICES;
   }
   else if (orc_String == "restart failure device")
   {
      ore_Type = C_OscCanOpenManagerInfo::eRESTART_FAILURE_DEVICE;
   }
   else if (orc_String == "stop all devices")
   {
      ore_Type = C_OscCanOpenManagerInfo::eSTOP_ALL_DEVICES;
   }
   else
   {
      s32_Retval = C_RANGE;
   }

   return s32_Retval;
}
