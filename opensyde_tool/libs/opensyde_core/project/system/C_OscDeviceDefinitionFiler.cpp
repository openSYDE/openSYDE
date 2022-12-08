//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       File handler for device definition file data

   Handle device definition loading and saving

   \copyright   Copyright 2016 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.hpp"

#include "stwtypes.hpp"
#include "stwerrors.hpp"
#include "C_SclString.hpp"
#include "TglFile.hpp"
#include "TglUtils.hpp"
#include "C_OscUtils.hpp"
#include "C_OscDeviceDefinitionFiler.hpp"
#include "C_OscDeviceDefinitionFilerV1.hpp"
#include "C_OscXmlParserLog.hpp"
#include "C_OscLoggingHandler.hpp"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */

using namespace stw::errors;
using namespace stw::scl;
using namespace stw::tgl;
using namespace stw::opensyde_core;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */

const uint16_t C_OscDeviceDefinitionFiler::mhu16_FILE_VERSION = 0x0002U;

/* -- Types --------------------------------------------------------------------------------------------------------- */

/* -- Global Variables ---------------------------------------------------------------------------------------------- */

/* -- Module Global Variables --------------------------------------------------------------------------------------- */

/* -- Module Global Function Prototypes ----------------------------------------------------------------------------- */

/* -- Implementation ------------------------------------------------------------------------------------------------ */

//----------------------------------------------------------------------------------------------------------------------

void C_OscDeviceDefinitionFiler::mh_ParseOpenSydeAvailability(const C_OscXmlParser & orc_Parser,
                                                              bool & orq_ProtocolSupportedCan,
                                                              bool & orq_ProtocolSupportedEthernet)
{
   bool q_Support;
   bool q_Can;
   bool q_Ethernet;

   //no check for existence of entries: fall back to "not supported" in this case
   q_Support  = orc_Parser.GetAttributeBool("support");
   q_Can      = orc_Parser.GetAttributeBool("can");
   q_Ethernet = orc_Parser.GetAttributeBool("ethernet");
   orq_ProtocolSupportedCan = (q_Support && q_Can);
   orq_ProtocolSupportedEthernet = (q_Support && q_Ethernet);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Gets the openSYDE flashloader parameter

   The parent "opensyde" in "protocols-flashloader" must be selected

   \param[in]   orc_Parser                      XML Parser
   \param[out]  oru32_RequestDownloadTimeout    Parameter for Request Download Timeout
   \param[out]  oru32_TransferDataTimeout       Parameter for Transfer Data Timeout
   \param[out]  orq_IsFileBased                 Flag if file based
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OscDeviceDefinitionFiler::mh_ParseOpenSydeFlashloaderParameter(const C_OscXmlParser & orc_Parser,
                                                                      uint32_t & oru32_RequestDownloadTimeout,
                                                                      uint32_t & oru32_TransferDataTimeout,
                                                                      bool & orq_IsFileBased)
{
   if (orc_Parser.AttributeExists("requestdownloadtimeout") == true)
   {
      oru32_RequestDownloadTimeout = orc_Parser.GetAttributeUint32("requestdownloadtimeout");
   }
   else
   {
      oru32_RequestDownloadTimeout = 20000U;
   }
   if (orc_Parser.AttributeExists("transferdatatimeout") == true)
   {
      oru32_TransferDataTimeout = orc_Parser.GetAttributeUint32("transferdatatimeout");
   }
   else
   {
      oru32_TransferDataTimeout = 2000U;
   }
   if (orc_Parser.AttributeExists("is-file-based") == true)
   {
      orq_IsFileBased = orc_Parser.GetAttributeBool("is-file-based");
   }
   else
   {
      orq_IsFileBased = false;
   }
}

//----------------------------------------------------------------------------------------------------------------------

void C_OscDeviceDefinitionFiler::mh_ParseStwFlashloaderAvailability(const C_OscXmlParser & orc_Parser,
                                                                    bool & orq_ProtocolSupportedCan)
{
   //no check for existence of entries: fall back to "not supported" in this case
   bool q_Support;
   bool q_Can;

   q_Support  = orc_Parser.GetAttributeBool("support");
   q_Can      = orc_Parser.GetAttributeBool("can");
   orq_ProtocolSupportedCan = (q_Support && q_Can);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Load device definition

   Load data from specified file and place it in device definition instance

   \param[out]  orc_DeviceDefinition   device definition information read from file
   \param[in]   orc_Parser             XML Parser
   \param[in]   orc_Path               path to file

   \return
   C_NO_ERR   data read and placed into device definition instance
   C_CONFIG   content of file is invalid or incomplete
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_OscDeviceDefinitionFiler::mh_Load(C_OscDeviceDefinition & orc_DeviceDefinition, C_OscXmlParser & orc_Parser,
                                            const C_SclString & orc_Path)
{
   int32_t s32_Return;

   uint32_t u32_Value;

   stw::scl::C_SclString c_Text;

   s32_Return = orc_Parser.SelectNodeChildError("global");
   if (s32_Return == C_NO_ERR)
   {
      c_Text = orc_Parser.SelectNodeChild("device-name");
      if (c_Text != "device-name")
      {
         osc_write_log_error("Loading device definition", "XML node \"device-name\" not found.");
         s32_Return = C_CONFIG;
      }
   }

   if (s32_Return == C_NO_ERR)
   {
      orc_DeviceDefinition.c_DeviceName = orc_Parser.GetNodeContent();
      //Optional alias
      orc_Parser.SelectNodeParent(); //back to parent ...
      if (orc_Parser.SelectNodeChild("device-name-alias") == "device-name-alias")
      {
         orc_DeviceDefinition.c_DeviceNameAlias = orc_Parser.GetNodeContent();
         orc_Parser.SelectNodeParent(); //back to parent ...
      }
      c_Text = orc_Parser.SelectNodeChild("device-description");
      if (c_Text != "device-description")
      {
         osc_write_log_error("Loading device definition", "XML node \"device-description\" not found.");
         s32_Return = C_CONFIG;
      }
   }
   if (s32_Return == C_NO_ERR)
   {
      orc_DeviceDefinition.c_DeviceDescription = orc_Parser.GetNodeContent();

      orc_Parser.SelectNodeParent(); //back to parent ...

      c_Text = orc_Parser.SelectNodeChild("image");
      if (c_Text != "image")
      {
         osc_write_log_error("Loading device definition", "XML node \"image\" not found.");
         s32_Return = C_CONFIG;
      }
   }
   if (s32_Return == C_NO_ERR)
   {
      orc_DeviceDefinition.c_ImagePath = orc_Parser.GetNodeContent();
      orc_Parser.SelectNodeParent(); //back to parent ...
      c_Text = orc_Parser.SelectNodeChild("bus-systems-available");
      if (c_Text != "bus-systems-available")
      {
         osc_write_log_error("Loading device definition", "XML node \"bus-systems-available\" not found.");
         s32_Return = C_CONFIG;
      }
      //expand the potentially relative image path to an absolute path
      // we will need it later to open the image in the UI
      orc_DeviceDefinition.c_ImagePath = TglExpandFileName(orc_DeviceDefinition.c_ImagePath,
                                                           TglExtractFilePath(orc_Path));
      // also store file path
      // it is needed for creating service update package (see #24474)
      orc_DeviceDefinition.c_FilePath = orc_Path;
   }
   if (s32_Return == C_NO_ERR)
   {
      u32_Value = orc_Parser.GetAttributeUint32("can");
      if (u32_Value > 255)
      {
         osc_write_log_error("Loading device definition", "XML node \"can\" contains invalid value.");
         s32_Return = C_CONFIG; //not a valid number
      }
      if (s32_Return == C_NO_ERR)
      {
         orc_DeviceDefinition.u8_NumCanBusses = static_cast<uint8_t>(u32_Value);
         u32_Value = orc_Parser.GetAttributeUint32("ethernet");
         if (u32_Value > 255)
         {
            osc_write_log_error("Loading device definition", "XML node \"ethernet\" contains invalid value.");
            s32_Return = C_CONFIG; //not a number
         }
         if (s32_Return == C_NO_ERR)
         {
            orc_DeviceDefinition.u8_NumEthernetBusses = static_cast<uint8_t>(u32_Value);
         }
      }
   }

   if (s32_Return == C_NO_ERR)
   {
      orc_Parser.SelectNodeParent(); //back to parent ...
      c_Text = orc_Parser.SelectNodeChild("can-bitrates-support");
      if (c_Text != "can-bitrates-support")
      {
         osc_write_log_error("Loading device definition", "XML node \"can-bitrates-support\" not found.");
         s32_Return = C_CONFIG;
      }
      if (s32_Return == C_NO_ERR)
      {
         //get bitrates
         c_Text = orc_Parser.SelectNodeChild("can-bitrate");
         if (c_Text == "can-bitrate")
         {
            do
            {
               if (orc_Parser.AttributeExists("value") == true)
               {
                  const uint16_t u16_Bitrate = static_cast<uint16_t>(orc_Parser.GetAttributeUint32("value"));
                  orc_DeviceDefinition.c_SupportedBitrates.push_back(u16_Bitrate);
               }
               else
               {
                  osc_write_log_error("Loading device definition",
                                      "XML node \"can-bitrate\" attribute \"value\" not found.");
                  s32_Return = C_CONFIG;
               }
               c_Text = orc_Parser.SelectNodeNext("can-bitrate");
            }
            while ((c_Text == "can-bitrate") && (s32_Return == C_NO_ERR));
            orc_Parser.SelectNodeParent(); //back to parent of parent ...
         }
         c_Text = orc_Parser.SelectNodeParent(); //back to parent of parent ...
         tgl_assert(c_Text == "global");
      }
      if (s32_Return == C_NO_ERR)
      {
         if (orc_Parser.SelectNodeChild("manufacturer-string") == "manufacturer-string")
         {
            s32_Return = orc_Parser.SelectNodeChildError("id");
            if (s32_Return == C_NO_ERR)
            {
               s32_Return = orc_Parser.GetAttributeUint32Error("value", u32_Value);
               if (s32_Return == C_NO_ERR)
               {
                  orc_DeviceDefinition.u8_ManufacturerId = static_cast<uint8_t>(u32_Value);
               }
               else
               {
                  orc_DeviceDefinition.u8_ManufacturerId = 0U;
               }
               c_Text = orc_Parser.SelectNodeParent(); //back to parent of parent ...
               tgl_assert(c_Text == "manufacturer-string");
            }
            if (s32_Return == C_NO_ERR)
            {
               s32_Return = orc_Parser.SelectNodeChildError("display-value");
               if (s32_Return == C_NO_ERR)
               {
                  orc_DeviceDefinition.c_ManufacturerDisplayValue = orc_Parser.GetNodeContent();
                  c_Text = orc_Parser.SelectNodeParent(); //back to parent of parent ...
                  tgl_assert(c_Text == "manufacturer-string");
               }
            }
            c_Text = orc_Parser.SelectNodeParent(); //back to parent of parent ...
            tgl_assert(c_Text == "global");
         }
         if (orc_DeviceDefinition.u8_ManufacturerId == 0U)
         {
            orc_DeviceDefinition.c_ManufacturerDisplayValue = C_OscDeviceDefinition::hc_DEFAULT_COMPANY_NAME;
         }
      }
   }

   if (s32_Return == C_NO_ERR)
   {
      c_Text = orc_Parser.SelectNodeParent(); //back to parent of parent ...
      tgl_assert(c_Text == "opensyde-device-definition");
      s32_Return = orc_Parser.SelectNodeChildError("sub-devices");
      if (s32_Return == C_NO_ERR)
      {
         s32_Return = orc_Parser.SelectNodeChildError("sub-device");
         if (s32_Return == C_NO_ERR)
         {
            do
            {
               C_OscSubDeviceDefinition c_SubDevice;
               s32_Return = C_OscDeviceDefinitionFiler::mh_LoadSubDevice(c_SubDevice, orc_Parser, orc_DeviceDefinition,
                                                                         orc_Path);
               orc_DeviceDefinition.c_SubDevices.push_back(c_SubDevice);
               c_Text = orc_Parser.SelectNodeNext("sub-device");
            }
            while ((c_Text == "sub-device") && (s32_Return == C_NO_ERR));
         }
      }
   }
   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Load sub device definition

   Load data from specified file and place it in sub device definition instance

   \param[out]  orc_SubDeviceDefinition   Sub device definition information read from file
   \param[in]   orc_Parser                XML Parser
   \param[in]   orc_DeviceDefinition      device definition information read from file
   \param[in]   orc_Path                  path to file

   \return
   C_NO_ERR   data read and placed into device definition instance
   C_CONFIG   content of file is invalid or incomplete
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_OscDeviceDefinitionFiler::mh_LoadSubDevice(C_OscSubDeviceDefinition & orc_SubDeviceDefinition,
                                                     C_OscXmlParser & orc_Parser,
                                                     const C_OscDeviceDefinition & orc_DeviceDefinition,
                                                     const C_SclString & orc_Path)
{
   int32_t s32_Return = C_NO_ERR;

   stw::scl::C_SclString c_Text;

   if (orc_Parser.SelectNodeChild("sub-device-name") == "sub-device-name")
   {
      orc_SubDeviceDefinition.c_SubDeviceName = orc_Parser.GetNodeContent();
      c_Text = orc_Parser.SelectNodeParent(); //back to parent ...
      tgl_assert(c_Text == "sub-device");
   }
   orc_SubDeviceDefinition.c_ConnectedInterfaces.clear();
   if (orc_Parser.SelectNodeChild("connected-interfaces") == "connected-interfaces")
   {
      s32_Return = orc_Parser.SelectNodeChildError("interface");
      if (s32_Return == C_NO_ERR)
      {
         do
         {
            s32_Return = orc_Parser.GetAttributeStringError("name", c_Text);
            if (s32_Return == C_NO_ERR)
            {
               bool q_Tmp;
               s32_Return = orc_Parser.GetAttributeBoolError("connected", q_Tmp);
               if (s32_Return == C_NO_ERR)
               {
                  orc_SubDeviceDefinition.c_ConnectedInterfaces[c_Text.LowerCase()] = q_Tmp;
               }
            }
            c_Text = orc_Parser.SelectNodeNext("interface");
         }
         while ((c_Text == "interface") && (s32_Return == C_NO_ERR));
         c_Text = orc_Parser.SelectNodeParent(); //back to parent ...
         tgl_assert(c_Text == "connected-interfaces");
      }
      c_Text = orc_Parser.SelectNodeParent(); //back to parent ...
      tgl_assert(c_Text == "sub-device");
   }

   if (orc_Parser.SelectNodeChild("other-accepted-names") == "other-accepted-names")
   {
      c_Text = orc_Parser.SelectNodeChild("other-accepted-name");
      if (c_Text == "other-accepted-name")
      {
         do
         {
            const stw::scl::C_SclString c_Tmp = orc_Parser.GetNodeContent();
            orc_SubDeviceDefinition.c_OtherAcceptedNames.push_back(c_Tmp);
            c_Text = orc_Parser.SelectNodeNext("other-accepted-name");
         }
         while (c_Text == "other-accepted-name");
         orc_Parser.SelectNodeParent(); //back to parent of parent ...
      }
      orc_Parser.SelectNodeParent(); //back to parent ...
   }
   c_Text = orc_Parser.SelectNodeChild("programming-properties");
   if (c_Text == "programming-properties")
   {
      if (orc_Parser.AttributeExists("is-programmable") == true)
      {
         orc_SubDeviceDefinition.q_ProgrammingSupport = orc_Parser.GetAttributeBool("is-programmable");
      }
      else
      {
         osc_write_log_error("Loading device definition",
                             "XML attribute \"is-programmable\" not found.");
         s32_Return = C_CONFIG;
      }
      orc_Parser.SelectNodeParent(); //back to parent ...
   }
   else
   {
      orc_SubDeviceDefinition.q_ProgrammingSupport = false;
   }
   c_Text = orc_Parser.SelectNodeChild("protocols-diagnostics");
   if (c_Text != "protocols-diagnostics")
   {
      //Optional: Use default values
   }
   else
   {
      //get sub-node
      c_Text = orc_Parser.SelectNodeChild("kefex");
      if (c_Text != "kefex")
      {
         //Optional: Use default values
      }
      else
      {
         const std::vector<C_OscXmlAttribute> c_Attributes = orc_Parser.GetAttributes();
         for (uint16_t u16_Index = 0U; u16_Index < c_Attributes.size(); u16_Index++)
         {
            if (c_Attributes[u16_Index].c_Name == "support")
            {
               orc_SubDeviceDefinition.q_DiagnosticProtocolKefex = (c_Attributes[u16_Index].c_Value == "1");
            }
            else
            {
               //unknown attribute; nothing we can do with it
            }
         }
         c_Text = orc_Parser.SelectNodeParent(); //back to parent ...
         tgl_assert(c_Text == "protocols-diagnostics");
      }
      c_Text = orc_Parser.SelectNodeChild("opensyde");
      if (c_Text != "opensyde")
      {
         //Optional: Use default values
      }
      else
      {
         mh_ParseOpenSydeAvailability(orc_Parser, orc_SubDeviceDefinition.q_DiagnosticProtocolOpenSydeCan,
                                      orc_SubDeviceDefinition.q_DiagnosticProtocolOpenSydeEthernet);

         c_Text = orc_Parser.SelectNodeParent(); //back to parent ...
         tgl_assert(c_Text == "protocols-diagnostics");
      }
      c_Text = orc_Parser.SelectNodeParent(); //back to parent of parent ...
      tgl_assert(c_Text == "sub-device");
   }

   c_Text = orc_Parser.SelectNodeChild("protocols-flashloader");
   if (c_Text != "protocols-flashloader")
   {
      //Optional: Use default values
   }
   else
   {
      //get sub-node
      c_Text = orc_Parser.SelectNodeChild("flashloader-reset-wait-times");
      if (c_Text != "flashloader-reset-wait-times")
      {
         //check due to compatibility for the old variant
         //get sub-node
         c_Text = orc_Parser.SelectNodeChild("flashloader-reset-wait-time");
         if (c_Text != "flashloader-reset-wait-time")
         {
            //Optional: Use default values
            osc_write_log_info("Loading device definition",
                               "Default values for flashloader reset wait times"
                               " for XML file \"" + orc_Path + "\" were used.");
         }
         else
         {
            const uint32_t u32_Value = orc_Parser.GetAttributeUint32("value");

            // Due to compatibility use the value of the old format for all Flashloader reset wait times
            orc_SubDeviceDefinition.u32_FlashloaderResetWaitTimeNoChangesCan = u32_Value;
            orc_SubDeviceDefinition.u32_FlashloaderResetWaitTimeNoChangesEthernet = u32_Value;
            orc_SubDeviceDefinition.u32_FlashloaderResetWaitTimeNoFundamentalChangesCan = u32_Value;
            orc_SubDeviceDefinition.u32_FlashloaderResetWaitTimeNoFundamentalChangesEthernet = u32_Value;
            orc_SubDeviceDefinition.u32_FlashloaderResetWaitTimeFundamentalChangesCan = u32_Value;
            orc_SubDeviceDefinition.u32_FlashloaderResetWaitTimeFundamentalChangesEthernet = u32_Value;

            osc_write_log_info("Loading device definition",
                               "Due to compatibility all flashloader reset wait times set to the"
                               " same configuration value (" + C_SclString::IntToStr(u32_Value) +
                               " ms) for XML file \"" + orc_Path + "\".");

            c_Text = orc_Parser.SelectNodeParent(); //back to parent ...
            tgl_assert(c_Text == "protocols-flashloader");
         }
      }
      else
      {
         // If elements were not found, the default value is used
         c_Text = orc_Parser.SelectNodeChild("no-changes-can");
         if (c_Text == "no-changes-can")
         {
            orc_SubDeviceDefinition.u32_FlashloaderResetWaitTimeNoChangesCan =
               orc_Parser.GetAttributeUint32("value");
            c_Text = orc_Parser.SelectNodeParent(); //back to parent ...
            tgl_assert(c_Text == "flashloader-reset-wait-times");
         }
         else
         {
            if (orc_DeviceDefinition.u8_NumCanBusses > 0U)
            {
               osc_write_log_info("Loading device definition",
                                  "Default value for flashloader reset wait time "
                                  "u32_FlashloaderResetWaitTimeNoChangesCan (" +
                                  C_SclString::IntToStr(orc_SubDeviceDefinition.
                                                        u32_FlashloaderResetWaitTimeNoChangesCan) +
                                  " ms) for XML file \"" + orc_Path + "\" used.");
            }
         }

         c_Text = orc_Parser.SelectNodeChild("no-changes-ethernet");
         if (c_Text == "no-changes-ethernet")
         {
            orc_SubDeviceDefinition.u32_FlashloaderResetWaitTimeNoChangesEthernet = orc_Parser.GetAttributeUint32(
               "value");
            c_Text = orc_Parser.SelectNodeParent(); //back to parent ...
            tgl_assert(c_Text == "flashloader-reset-wait-times");
         }
         else
         {
            if (orc_DeviceDefinition.u8_NumEthernetBusses > 0U)
            {
               osc_write_log_info("Loading device definition",
                                  "Default value for flashloader reset wait time "
                                  "u32_FlashloaderResetWaitTimeNoChangesEthernet (" +
                                  C_SclString::IntToStr(orc_SubDeviceDefinition.
                                                        u32_FlashloaderResetWaitTimeNoChangesEthernet) +
                                  " ms) for XML file \"" + orc_Path + "\" used.");
            }
         }

         c_Text = orc_Parser.SelectNodeChild("no-fundamental-com-changes-can");
         if (c_Text == "no-fundamental-com-changes-can")
         {
            orc_SubDeviceDefinition.u32_FlashloaderResetWaitTimeNoFundamentalChangesCan =
               orc_Parser.GetAttributeUint32("value");
            c_Text = orc_Parser.SelectNodeParent(); //back to parent ...
            tgl_assert(c_Text == "flashloader-reset-wait-times");
         }
         else
         {
            if (orc_DeviceDefinition.u8_NumCanBusses > 0U)
            {
               osc_write_log_info("Loading device definition",
                                  "Default value for flashloader reset wait time "
                                  "u32_FlashloaderResetWaitTimeNoFundamentalChangesCan (" +
                                  C_SclString::IntToStr(orc_SubDeviceDefinition.
                                                        u32_FlashloaderResetWaitTimeNoFundamentalChangesCan) +
                                  " ms) for XML file \"" + orc_Path + "\" used.");
            }
         }

         c_Text = orc_Parser.SelectNodeChild("no-fundamental-com-changes-ethernet");
         if (c_Text == "no-fundamental-com-changes-ethernet")
         {
            orc_SubDeviceDefinition.u32_FlashloaderResetWaitTimeNoFundamentalChangesEthernet =
               orc_Parser.GetAttributeUint32("value");
            c_Text = orc_Parser.SelectNodeParent(); //back to parent ...
            tgl_assert(c_Text == "flashloader-reset-wait-times");
         }
         else
         {
            if (orc_DeviceDefinition.u8_NumEthernetBusses > 0U)
            {
               osc_write_log_info("Loading device definition",
                                  "Default value for flashloader reset wait time "
                                  "u32_FlashloaderResetWaitTimeNoFundamentalChangesEthernet (" +
                                  C_SclString::IntToStr(orc_SubDeviceDefinition.
                                                        u32_FlashloaderResetWaitTimeNoFundamentalChangesEthernet) +
                                  " ms) for XML file \"" + orc_Path + "\" used.");
            }
         }

         c_Text = orc_Parser.SelectNodeChild("fundamental-com-changes-can");
         if (c_Text == "fundamental-com-changes-can")
         {
            orc_SubDeviceDefinition.u32_FlashloaderResetWaitTimeFundamentalChangesCan =
               orc_Parser.GetAttributeUint32(
                  "value");
            c_Text = orc_Parser.SelectNodeParent(); //back to parent ...
            tgl_assert(c_Text == "flashloader-reset-wait-times");
         }
         else
         {
            if (orc_DeviceDefinition.u8_NumCanBusses > 0U)
            {
               osc_write_log_info("Loading device definition",
                                  "Default value for flashloader reset wait time "
                                  "u32_FlashloaderResetWaitTimeFundamentalChangesCan (" +
                                  C_SclString::IntToStr(orc_SubDeviceDefinition.
                                                        u32_FlashloaderResetWaitTimeFundamentalChangesCan) +
                                  " ms) for XML file \"" + orc_Path + "\" used.");
            }
         }

         c_Text = orc_Parser.SelectNodeChild("fundamental-com-changes-ethernet");
         if (c_Text == "fundamental-com-changes-ethernet")
         {
            orc_SubDeviceDefinition.u32_FlashloaderResetWaitTimeFundamentalChangesEthernet =
               orc_Parser.GetAttributeUint32("value");
            c_Text = orc_Parser.SelectNodeParent(); //back to parent ...
            tgl_assert(c_Text == "flashloader-reset-wait-times");
         }
         else
         {
            if (orc_DeviceDefinition.u8_NumEthernetBusses > 0U)
            {
               osc_write_log_info("Loading device definition",
                                  "Default value for flashloader reset wait time "
                                  "u32_FlashloaderResetWaitTimeFundamentalChangesEthernet (" +
                                  C_SclString::IntToStr(orc_SubDeviceDefinition.
                                                        u32_FlashloaderResetWaitTimeFundamentalChangesEthernet) +
                                  " ms) for XML file \"" + orc_Path + "\" used.");
            }
         }

         c_Text = orc_Parser.SelectNodeParent(); //back to parent ...
         tgl_assert(c_Text == "protocols-flashloader");
      }

      //get sub-node
      c_Text = orc_Parser.SelectNodeChild("stw-flashloader");
      if (c_Text != "stw-flashloader")
      {
         //Optional: Use default values
      }
      else
      {
         mh_ParseStwFlashloaderAvailability(orc_Parser, orc_SubDeviceDefinition.q_FlashloaderStwCan);
         c_Text = orc_Parser.SelectNodeParent(); //back to parent ...
         tgl_assert(c_Text == "protocols-flashloader");
      }
      c_Text = orc_Parser.SelectNodeChild("opensyde");
      if (c_Text != "opensyde")
      {
         //Optional: Use default values
      }
      else
      {
         mh_ParseOpenSydeAvailability(orc_Parser, orc_SubDeviceDefinition.q_FlashloaderOpenSydeCan,
                                      orc_SubDeviceDefinition.q_FlashloaderOpenSydeEthernet);

         mh_ParseOpenSydeFlashloaderParameter(orc_Parser,
                                              orc_SubDeviceDefinition.u32_FlashloaderOpenSydeRequestDownloadTimeout,
                                              orc_SubDeviceDefinition.u32_FlashloaderOpenSydeTransferDataTimeout,
                                              orc_SubDeviceDefinition.q_FlashloaderOpenSydeIsFileBased);

         c_Text = orc_Parser.SelectNodeParent(); //back to parent ...
         tgl_assert(c_Text == "protocols-flashloader");
      }
      c_Text = orc_Parser.SelectNodeParent(); //back to parent of parent ...
      tgl_assert(c_Text == "sub-device");
   }

   c_Text = orc_Parser.SelectNodeChild("memory");
   if (c_Text != "memory")
   {
      //Optional: Use default values
   }
   else
   {
      c_Text = orc_Parser.SelectNodeChild("user-eeprom");
      if (c_Text != "user-eeprom")
      {
         //Optional: Use default values
      }
      else
      {
         orc_SubDeviceDefinition.u32_UserEepromSizeBytes = orc_Parser.GetAttributeUint32("sizebytes");
      }
      c_Text = orc_Parser.SelectNodeParent(); //back to parent of parent ...
      tgl_assert(c_Text == "memory");
      c_Text = orc_Parser.SelectNodeParent(); //back to parent of parent ...
      tgl_assert(c_Text == "sub-device");
   }

   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Save sub device

   \param[in]      orc_SubDeviceDefinition   Sub device definition
   \param[in,out]  orc_Parser                XML Parser
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OscDeviceDefinitionFiler::mh_SaveSubDevice(const C_OscSubDeviceDefinition & orc_SubDeviceDefinition,
                                                  C_OscXmlParser & orc_Parser)
{
   orc_Parser.CreateAndSelectNodeChild("sub-device");
   orc_Parser.CreateNodeChild("sub-device-name", orc_SubDeviceDefinition.c_SubDeviceName);
   orc_Parser.CreateAndSelectNodeChild("connected-interfaces");
   for (std::map<stw::scl::C_SclString, bool>::const_iterator c_It =
           orc_SubDeviceDefinition.c_ConnectedInterfaces.begin();
        c_It != orc_SubDeviceDefinition.c_ConnectedInterfaces.end(); ++c_It)
   {
      orc_Parser.CreateAndSelectNodeChild("interface");
      orc_Parser.SetAttributeString("name", c_It->first);
      orc_Parser.SetAttributeBool("connected", c_It->second);
      orc_Parser.SelectNodeParent();
   }
   orc_Parser.SelectNodeParent();
   orc_Parser.CreateAndSelectNodeChild("other-accepted-names");
   for (uint32_t u32_It = 0UL; u32_It < orc_SubDeviceDefinition.c_OtherAcceptedNames.size();
        ++u32_It)
   {
      orc_Parser.CreateNodeChild("other-accepted-name",
                                 orc_SubDeviceDefinition.c_OtherAcceptedNames[u32_It]);
   }
   orc_Parser.SelectNodeParent();
   orc_Parser.CreateAndSelectNodeChild("programming-properties");
   orc_Parser.SetAttributeBool("is-programmable", orc_SubDeviceDefinition.q_ProgrammingSupport);
   orc_Parser.SelectNodeParent();
   orc_Parser.CreateAndSelectNodeChild("protocols-diagnostics");
   orc_Parser.CreateAndSelectNodeChild("kefex");
   orc_Parser.SetAttributeBool("support", orc_SubDeviceDefinition.q_DiagnosticProtocolKefex);
   orc_Parser.SelectNodeParent();
   orc_Parser.CreateAndSelectNodeChild("opensyde");
   orc_Parser.SetAttributeBool("support",
                               (orc_SubDeviceDefinition.q_DiagnosticProtocolOpenSydeCan ||
                                orc_SubDeviceDefinition.q_DiagnosticProtocolOpenSydeEthernet));
   orc_Parser.SetAttributeBool("can", orc_SubDeviceDefinition.q_DiagnosticProtocolOpenSydeCan);
   orc_Parser.SetAttributeBool("ethernet",
                               orc_SubDeviceDefinition.q_DiagnosticProtocolOpenSydeEthernet);
   orc_Parser.SelectNodeParent();
   orc_Parser.SelectNodeParent();

   orc_Parser.CreateAndSelectNodeChild("protocols-flashloader");
   orc_Parser.CreateAndSelectNodeChild("flashloader-reset-wait-times");
   orc_Parser.CreateAndSelectNodeChild("no-changes-can");
   orc_Parser.SetAttributeUint32("value",
                                 orc_SubDeviceDefinition.u32_FlashloaderResetWaitTimeNoChangesCan);
   orc_Parser.SelectNodeParent();
   orc_Parser.CreateAndSelectNodeChild("no-changes-ethernet");
   orc_Parser.SetAttributeUint32("value",
                                 orc_SubDeviceDefinition.u32_FlashloaderResetWaitTimeNoChangesEthernet);
   orc_Parser.SelectNodeParent();
   orc_Parser.CreateAndSelectNodeChild("no-fundamental-com-changes-can");
   orc_Parser.SetAttributeUint32("value",
                                 orc_SubDeviceDefinition.u32_FlashloaderResetWaitTimeNoFundamentalChangesCan);
   orc_Parser.SelectNodeParent();
   orc_Parser.CreateAndSelectNodeChild("no-fundamental-com-changes-ethernet");
   orc_Parser.SetAttributeUint32("value",
                                 orc_SubDeviceDefinition.u32_FlashloaderResetWaitTimeNoFundamentalChangesEthernet);
   orc_Parser.SelectNodeParent();
   orc_Parser.CreateAndSelectNodeChild("fundamental-com-changes-can");
   orc_Parser.SetAttributeUint32("value",
                                 orc_SubDeviceDefinition.u32_FlashloaderResetWaitTimeFundamentalChangesCan);
   orc_Parser.SelectNodeParent();
   orc_Parser.CreateAndSelectNodeChild("fundamental-com-changes-ethernet");
   orc_Parser.SetAttributeUint32("value",
                                 orc_SubDeviceDefinition.u32_FlashloaderResetWaitTimeFundamentalChangesEthernet);
   orc_Parser.SelectNodeParent();
   orc_Parser.SelectNodeParent();
   orc_Parser.CreateAndSelectNodeChild("stw-flashloader");
   orc_Parser.SetAttributeBool("support", orc_SubDeviceDefinition.q_FlashloaderStwCan);
   orc_Parser.SetAttributeBool("can", orc_SubDeviceDefinition.q_FlashloaderStwCan);
   orc_Parser.SelectNodeParent();
   orc_Parser.CreateAndSelectNodeChild("opensyde");
   orc_Parser.SetAttributeBool("support",
                               (orc_SubDeviceDefinition.q_FlashloaderOpenSydeCan ||
                                orc_SubDeviceDefinition.q_FlashloaderOpenSydeEthernet));
   orc_Parser.SetAttributeBool("can", orc_SubDeviceDefinition.q_FlashloaderOpenSydeCan);
   orc_Parser.SetAttributeBool("ethernet", orc_SubDeviceDefinition.q_FlashloaderOpenSydeEthernet);
   orc_Parser.SetAttributeUint32("requestdownloadtimeout",
                                 orc_SubDeviceDefinition.u32_FlashloaderOpenSydeRequestDownloadTimeout);
   orc_Parser.SetAttributeUint32("transferdatatimeout",
                                 orc_SubDeviceDefinition.u32_FlashloaderOpenSydeTransferDataTimeout);
   orc_Parser.SetAttributeBool("is-file-based",
                               orc_SubDeviceDefinition.q_FlashloaderOpenSydeIsFileBased);
   orc_Parser.SelectNodeParent();
   orc_Parser.SelectNodeParent();
   orc_Parser.CreateAndSelectNodeChild("memory");
   orc_Parser.CreateAndSelectNodeChild("user-eeprom");
   orc_Parser.SetAttributeUint32("sizebytes", orc_SubDeviceDefinition.u32_UserEepromSizeBytes);
   orc_Parser.SelectNodeParent();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Handle connected interfaces

   \param[in,out]  orc_DeviceDefinition   Device definition

   \return
   Type of return values, e.g. STW error codes

   \retval   Return value 1   Detailed description of 1st return value
   \retval   Return value 2   Detailed description of 2nd return value
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_OscDeviceDefinitionFiler::mh_HandleConnectedInterfaces(C_OscDeviceDefinition & orc_DeviceDefinition)
{
   int32_t s32_Return = C_NO_ERR;

   if (orc_DeviceDefinition.c_SubDevices.size() <= 1L)
   {
      //Auto fill
      for (uint32_t u32_ItSubDev = 0UL; u32_ItSubDev < orc_DeviceDefinition.c_SubDevices.size(); ++u32_ItSubDev)
      {
         C_OscSubDeviceDefinition & rc_SubDevice = orc_DeviceDefinition.c_SubDevices[u32_ItSubDev];
         for (uint32_t u32_ItCan = 0UL; u32_ItCan < static_cast<uint32_t>(orc_DeviceDefinition.u8_NumCanBusses);
              ++u32_ItCan)
         {
            rc_SubDevice.c_ConnectedInterfaces["can" + stw::scl::C_SclString::IntToStr(u32_ItCan + 1U)] = true;
         }
         for (uint32_t u32_ItEth = 0UL; u32_ItEth < static_cast<uint32_t>(orc_DeviceDefinition.u8_NumEthernetBusses);
              ++u32_ItEth)
         {
            rc_SubDevice.c_ConnectedInterfaces["eth" + stw::scl::C_SclString::IntToStr(u32_ItEth + 1U)] = true;
         }
      }
   }
   else
   {
      //Check
      for (uint32_t u32_ItSubDev = 0UL;
           (u32_ItSubDev < orc_DeviceDefinition.c_SubDevices.size()) && (s32_Return == C_NO_ERR); ++u32_ItSubDev)
      {
         const C_OscSubDeviceDefinition & rc_SubDevice = orc_DeviceDefinition.c_SubDevices[u32_ItSubDev];
         if (rc_SubDevice.c_SubDeviceName.IsEmpty())
         {
            s32_Return = C_CONFIG;
            osc_write_log_error("Loading device definition", "sub device name empty in multiple sub device setting");
         }
         for (uint32_t u32_ItCan = 0UL;
              (u32_ItCan < static_cast<uint32_t>(orc_DeviceDefinition.u8_NumCanBusses)) && (s32_Return == C_NO_ERR);
              ++u32_ItCan)
         {
            const stw::scl::C_SclString c_Interface = "can" + stw::scl::C_SclString::IntToStr(u32_ItCan + 1U);
            if (rc_SubDevice.c_ConnectedInterfaces.count(c_Interface) != 1UL)
            {
               s32_Return = C_CONFIG;
               osc_write_log_error("Loading device definition",
                                   "entry for \"" + c_Interface + "\" missing in node \"connected-interfaces\"");
            }
         }
         for (uint32_t u32_ItEth = 0UL;
              (u32_ItEth < static_cast<uint32_t>(orc_DeviceDefinition.u8_NumEthernetBusses)) &&
              (s32_Return == C_NO_ERR);
              ++u32_ItEth)
         {
            const stw::scl::C_SclString c_Interface = "eth" + stw::scl::C_SclString::IntToStr(u32_ItEth + 1U);
            if (rc_SubDevice.c_ConnectedInterfaces.count(c_Interface) != 1UL)
            {
               s32_Return = C_CONFIG;
               osc_write_log_error("Loading device definition",
                                   "entry for \"" + c_Interface + "\" missing in node \"connected-interfaces\"");
            }
         }
      }
   }
   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Check content errors

   \param[in]  orc_DeviceDefinition    Device definition

   \return
   C_NO_ERR   no error detected
   C_CONFIG   error detected
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_OscDeviceDefinitionFiler::mh_CheckContentErrors(const C_OscDeviceDefinition & orc_DeviceDefinition)
{
   int32_t s32_Retval;

   s32_Retval = mh_CheckNotConnectedInterface(orc_DeviceDefinition);
   if (s32_Retval == C_NO_ERR)
   {
      s32_Retval = mh_CheckNotConnectedDevice(orc_DeviceDefinition);
   }
   if (s32_Retval == C_NO_ERR)
   {
      s32_Retval = mh_CheckDeviceName(orc_DeviceDefinition);
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Check not connected interface

   \param[in]  orc_DeviceDefinition    Device definition

   \return
   C_NO_ERR   no error detected
   C_CONFIG   error detected
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_OscDeviceDefinitionFiler::mh_CheckNotConnectedInterface(const C_OscDeviceDefinition & orc_DeviceDefinition)
{
   int32_t s32_Retval = mh_CheckNotConnectedInterfaceByType(orc_DeviceDefinition, C_OscSystemBus::eCAN,
                                                            orc_DeviceDefinition.u8_NumCanBusses);

   if (s32_Retval == C_NO_ERR)
   {
      s32_Retval = mh_CheckNotConnectedInterfaceByType(orc_DeviceDefinition, C_OscSystemBus::eETHERNET,
                                                       orc_DeviceDefinition.u8_NumEthernetBusses);
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Check not connected interface by type

   \param[in]  orc_DeviceDefinition          Device definition
   \param[in]  oe_Type                       Type
   \param[in]  ou8_NumAvailableInterfaces    Number of available interfaces

   \return
   C_NO_ERR   no error detected
   C_CONFIG   error detected
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_OscDeviceDefinitionFiler::mh_CheckNotConnectedInterfaceByType(
   const C_OscDeviceDefinition & orc_DeviceDefinition, const C_OscSystemBus::E_Type oe_Type,
   const uint8_t ou8_NumAvailableInterfaces)
{
   int32_t s32_Retval = C_NO_ERR;

   for (uint8_t u8_ItInterface = 0U; u8_ItInterface < ou8_NumAvailableInterfaces; ++u8_ItInterface)
   {
      bool q_IsUsed = false;
      for (uint32_t u32_ItSubDevice = 0UL; u32_ItSubDevice < orc_DeviceDefinition.c_SubDevices.size();
           ++u32_ItSubDevice)
      {
         const C_OscSubDeviceDefinition & rc_SubDevice = orc_DeviceDefinition.c_SubDevices[u32_ItSubDevice];
         if (rc_SubDevice.IsConnected(oe_Type, u8_ItInterface))
         {
            q_IsUsed = true;
            break;
         }
      }
      if (!q_IsUsed)
      {
         const stw::scl::C_SclString c_Interface = C_OscSubDeviceDefinition::h_GetInterfaceNameLower(oe_Type,
                                                                                                     u8_ItInterface);
         s32_Retval = C_CONFIG;
         osc_write_log_error("Loading device definition", "Could not find usage for interface " + c_Interface);
      }
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Check not connected device

   \param[in]  orc_DeviceDefinition    Device definition

   \return
   C_NO_ERR   no error detected
   C_CONFIG   error detected
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_OscDeviceDefinitionFiler::mh_CheckNotConnectedDevice(const C_OscDeviceDefinition & orc_DeviceDefinition)
{
   int32_t s32_Retval = C_NO_ERR;

   for (uint32_t u32_ItSubDevice = 0UL; u32_ItSubDevice < orc_DeviceDefinition.c_SubDevices.size(); ++u32_ItSubDevice)
   {
      const C_OscSubDeviceDefinition & rc_SubDevice = orc_DeviceDefinition.c_SubDevices[u32_ItSubDevice];
      if (!C_OscDeviceDefinitionFiler::mh_CheckNotConnectedDeviceByType(rc_SubDevice, C_OscSystemBus::eCAN,
                                                                        orc_DeviceDefinition.u8_NumCanBusses))
      {
         if (!C_OscDeviceDefinitionFiler::mh_CheckNotConnectedDeviceByType(rc_SubDevice, C_OscSystemBus::eETHERNET,
                                                                           orc_DeviceDefinition.u8_NumEthernetBusses))
         {
            s32_Retval = C_CONFIG;
            osc_write_log_error("Loading device definition",
                                "Could not find any connected interface for node \"" + rc_SubDevice.c_SubDeviceName +
                                "\"");
         }
      }
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Check not connected device by type

   \param[in]  orc_SubDeviceDefinition       Sub device definition
   \param[in]  oe_Type                       Type
   \param[in]  ou8_NumAvailableInterfaces    Number of available interfaces

   \return
   true  is connected
   false is not connected
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_OscDeviceDefinitionFiler::mh_CheckNotConnectedDeviceByType(
   const C_OscSubDeviceDefinition & orc_SubDeviceDefinition, const C_OscSystemBus::E_Type oe_Type,
   const uint8_t ou8_NumAvailableInterfaces)
{
   bool q_IsConnected = false;

   for (uint8_t u8_ItInterface = 0U; u8_ItInterface < ou8_NumAvailableInterfaces; ++u8_ItInterface)
   {
      if (orc_SubDeviceDefinition.IsConnected(oe_Type, u8_ItInterface))
      {
         q_IsConnected = true;
         break;
      }
   }
   return q_IsConnected;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Check device name

   \param[in]  orc_DeviceDefinition    Device definition

   \return
   C_NO_ERR   no error detected
   C_CONFIG   error detected
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_OscDeviceDefinitionFiler::mh_CheckDeviceName(const C_OscDeviceDefinition & orc_DeviceDefinition)
{
   int32_t s32_Retval = C_NO_ERR;

   for (uint32_t u32_ItSubDevice = 0UL; u32_ItSubDevice < orc_DeviceDefinition.c_SubDevices.size(); ++u32_ItSubDevice)
   {
      const C_OscSubDeviceDefinition & rc_SubDevice = orc_DeviceDefinition.c_SubDevices[u32_ItSubDevice];
      if (!C_OscUtils::h_CheckValidCeName(rc_SubDevice.c_SubDeviceName))
      {
         s32_Retval = C_CONFIG;
         osc_write_log_error("Loading device definition",
                             "Invalid device name \"" + rc_SubDevice.c_SubDeviceName + "\" (non C compliant)");
      }
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Load device definition

   Load data from specified file and place it in device definition instance

   \param[out]  orc_DeviceDefinition   device definition information read from file
   \param[in]   orc_Path               path to file

   \return
   C_NO_ERR   data read and placed into device definition instance
   C_RANGE    specified file does not exist
   C_NOACT    specified file is invalid (invalid XML file)
   C_CONFIG   content of file is invalid or incomplete
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_OscDeviceDefinitionFiler::h_Load(C_OscDeviceDefinition & orc_DeviceDefinition,
                                           const stw::scl::C_SclString & orc_Path)
{
   int32_t s32_Return = C_NO_ERR;

   if (TglFileExists(orc_Path) == false)
   {
      osc_write_log_error("Loading device definition", "File not found: \"" + orc_Path + "\".");
      s32_Return = C_RANGE;
   }
   else
   {
      C_OscXmlParserLog c_Xml;
      //"empty" device definition to have a clearly defined status:
      orc_DeviceDefinition.Clear();

      c_Xml.SetLogHeading("Loading device definition");

      s32_Return = c_Xml.LoadFromFile(orc_Path); //open XML file
      if (s32_Return == C_NO_ERR)
      {
         C_SclString c_Text;
         //Check if root node exists:
         c_Text = c_Xml.SelectRoot();

         if (c_Text != "opensyde-device-definition")
         {
            osc_write_log_error("Loading device definition", "XML node \"opensyde-device-definition\" not found.");
            s32_Return = C_CONFIG;
         }
         else
         {
            if (c_Xml.SelectNodeChild("file-version") == "file-version")
            {
               uint16_t u16_FileVersion = 0U;
               try
               {
                  u16_FileVersion = static_cast<uint16_t>(c_Xml.GetNodeContent().ToInt());
               }
               catch (...)
               {
                  osc_write_log_error("Loading device definition",
                                      "\"file-version\" could not be converted to a number.");
                  s32_Return = C_CONFIG;
               }

               //is the file version one we know ?
               if (s32_Return == C_NO_ERR)
               {
                  osc_write_log_info("Loading device definition", "Value of \"file-version\": " +
                                     C_SclString::IntToStr(u16_FileVersion));

                  //Return
                  c_Xml.SelectNodeParent();
                  //Check file version
                  if (u16_FileVersion == mhu16_FILE_VERSION)
                  {
                     s32_Return = C_OscDeviceDefinitionFiler::mh_Load(orc_DeviceDefinition, c_Xml, orc_Path);
                  }
                  else if (u16_FileVersion == 1)
                  {
                     s32_Return = C_OscDeviceDefinitionFilerV1::h_Load(orc_DeviceDefinition, c_Xml, orc_Path);
                  }
                  else
                  {
                     osc_write_log_error("Loading device definition",
                                         "Version defined by \"file-version\" is not supported.");
                     s32_Return = C_CONFIG;
                  }
               }
            }
            else
            {
               osc_write_log_error("Loading device definition", "XML node \"file-version\" not found.");
               s32_Return = C_CONFIG;
            }
         }
      }
      else
      {
         osc_write_log_error("Loading device definition", "Could not open XML file \"" + orc_Path + "\".");
         s32_Return = C_NOACT;
      }
   }
   if (s32_Return == C_NO_ERR)
   {
      s32_Return = C_OscDeviceDefinitionFiler::mh_HandleConnectedInterfaces(orc_DeviceDefinition);
   }
   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Save device definition

   Save device definition data into specified file.
   Will overwrite the file if it already exists.

   \param[in]  orc_DeviceDefinition    device definition information to write to file
   \param[in]  orc_Path                path to file

   \return
   C_NO_ERR   data written to file
   C_RANGE    device definition invalid
   C_RD_WR    could not erase pre-existing file before saving
   C_RD_WR    could not write to file (e.g. missing write permissions; missing folder)
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_OscDeviceDefinitionFiler::h_Save(const C_OscDeviceDefinition & orc_DeviceDefinition,
                                           const C_SclString & orc_Path)
{
   int32_t s32_Return = C_NO_ERR;

   if (TglFileExists(orc_Path) == true)
   {
      //erase it:
      int x_Return; //lint !e970 !e8080  //using type to match library interface
      x_Return = std::remove(orc_Path.c_str());
      if (x_Return != 0)
      {
         osc_write_log_error("Saving device definition", "Could not erase pre-existing file \"" + orc_Path + "\".");
         s32_Return = C_RD_WR;
      }
   }
   if (s32_Return == C_NO_ERR)
   {
      uint32_t u32_Counter;
      C_OscXmlParser c_Xml;
      c_Xml.CreateNodeChild("opensyde-device-definition"); //root node
      c_Xml.SelectRoot();
      c_Xml.CreateNodeChild("file-version", "0x" + C_SclString::IntToHex(mhu16_FILE_VERSION, 4U));

      c_Xml.CreateAndSelectNodeChild("global");
      c_Xml.CreateNodeChild("device-name", orc_DeviceDefinition.c_DeviceName);
      c_Xml.CreateNodeChild("device-name-alias", orc_DeviceDefinition.c_DeviceNameAlias);
      c_Xml.CreateNodeChild("device-description", orc_DeviceDefinition.c_DeviceDescription);
      c_Xml.CreateNodeChild("image", orc_DeviceDefinition.c_ImagePath);
      c_Xml.CreateAndSelectNodeChild("bus-systems-available");
      c_Xml.SetAttributeUint32("can", orc_DeviceDefinition.u8_NumCanBusses);
      c_Xml.SetAttributeUint32("ethernet", orc_DeviceDefinition.u8_NumEthernetBusses);
      c_Xml.SelectNodeParent();
      c_Xml.CreateAndSelectNodeChild("can-bitrates-support");
      for (u32_Counter = 0U; u32_Counter < orc_DeviceDefinition.c_SupportedBitrates.size(); ++u32_Counter)
      {
         c_Xml.CreateAndSelectNodeChild("can-bitrate");
         c_Xml.SetAttributeUint32("value", orc_DeviceDefinition.c_SupportedBitrates[u32_Counter]);
         c_Xml.SelectNodeParent();
      }
      c_Xml.SelectNodeParent();
      c_Xml.CreateAndSelectNodeChild("manufacturer-string");
      c_Xml.CreateAndSelectNodeChild("id");
      c_Xml.SetAttributeUint32("value", orc_DeviceDefinition.u8_ManufacturerId);
      c_Xml.SelectNodeParent();
      c_Xml.CreateNodeChild("display-value", orc_DeviceDefinition.c_ManufacturerDisplayValue);
      c_Xml.SelectNodeParent();
      c_Xml.SelectNodeParent();
      c_Xml.CreateAndSelectNodeChild("sub-devices");
      for (uint32_t u32_ItSubDevice = 0UL; u32_ItSubDevice < orc_DeviceDefinition.c_SubDevices.size();
           ++u32_ItSubDevice)
      {
         const C_OscSubDeviceDefinition & rc_CurSubDevice = orc_DeviceDefinition.c_SubDevices[u32_ItSubDevice];
         C_OscDeviceDefinitionFiler::mh_SaveSubDevice(rc_CurSubDevice, c_Xml);
      }
      c_Xml.SelectNodeParent();

      s32_Return = c_Xml.SaveToFile(orc_Path);
      if (s32_Return != C_NO_ERR)
      {
         osc_write_log_error("Saving Device definition", "Could not write to file \"" + orc_Path + "\".");
         s32_Return = C_RD_WR;
      }
   }
   else
   {
      osc_write_log_error("Saving Device definition", "No sub devices found.");
      s32_Return = C_RD_WR;
   }

   return s32_Return;
}
