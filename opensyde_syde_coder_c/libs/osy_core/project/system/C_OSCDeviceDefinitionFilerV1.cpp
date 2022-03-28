//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       File handler for device definition file data (V1)

   Handle device definition loading and saving

   \copyright   Copyright 2021 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.h"

#include "stwtypes.h"
#include "stwerrors.h"
#include "CSCLString.h"
#include "TGLFile.h"
#include "TGLUtils.h"
#include "C_OSCDeviceDefinitionFilerV1.h"
#include "C_OSCXMLParser.h"
#include "C_OSCLoggingHandler.h"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw_types;
using namespace stw_errors;
using namespace stw_scl;
using namespace stw_tgl;
using namespace stw_opensyde_core;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */

const uint16 C_OSCDeviceDefinitionFilerV1::mhu16_FILE_VERSION = 0x0001U;

/* -- Types --------------------------------------------------------------------------------------------------------- */

/* -- Global Variables ---------------------------------------------------------------------------------------------- */

/* -- Module Global Variables --------------------------------------------------------------------------------------- */

/* -- Module Global Function Prototypes ----------------------------------------------------------------------------- */

/* -- Implementation ------------------------------------------------------------------------------------------------ */

//----------------------------------------------------------------------------------------------------------------------

void C_OSCDeviceDefinitionFilerV1::mh_ParseOpenSydeAvailability(const C_OSCXMLParser & orc_Parser,
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
void C_OSCDeviceDefinitionFilerV1::mh_ParseOpenSydeFlashloaderParameter(const C_OSCXMLParser & orc_Parser,
                                                                        uint32 & oru32_RequestDownloadTimeout,
                                                                        uint32 & oru32_TransferDataTimeout,
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

void C_OSCDeviceDefinitionFilerV1::mh_ParseSTWFlashloaderAvailability(const C_OSCXMLParser & orc_Parser,
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
sint32 C_OSCDeviceDefinitionFilerV1::h_Load(C_OSCDeviceDefinition & orc_DeviceDefinition, C_OSCXMLParser & orc_Parser,
                                            const stw_scl::C_SCLString & orc_Path)
{
   sint32 s32_Return = C_NO_ERR;

   uint32 u32_Value;

   stw_scl::C_SCLString c_Text;
   C_OSCSubDeviceDefinition c_SubDevice;

   c_Text = orc_Parser.SelectNodeChild("device-name");
   if (c_Text != "device-name")
   {
      osc_write_log_error("Loading device definition", "XML node \"device-name\" not found.");
      s32_Return = C_CONFIG;
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
      if (orc_Parser.SelectNodeChild("other-accepted-names") == "other-accepted-names")
      {
         c_Text = orc_Parser.SelectNodeChild("other-accepted-name");
         if (c_Text == "other-accepted-name")
         {
            do
            {
               const stw_scl::C_SCLString c_Tmp = orc_Parser.GetNodeContent();
               c_SubDevice.c_OtherAcceptedNames.push_back(c_Tmp);
               c_Text = orc_Parser.SelectNodeNext("other-accepted-name");
            }
            while (c_Text == "other-accepted-name");
            orc_Parser.SelectNodeParent(); //back to parent of parent ...
         }
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
      c_Text = orc_Parser.SelectNodeChild("programming-properties");
      if (c_Text == "programming-properties")
      {
         if (orc_Parser.AttributeExists("is-programmable") == true)
         {
            c_SubDevice.q_ProgrammingSupport = orc_Parser.GetAttributeBool("is-programmable");
         }
         else
         {
            osc_write_log_error("Loading device definition", "XML attribute \"is-programmable\" not found.");
            s32_Return = C_CONFIG;
         }
         orc_Parser.SelectNodeParent(); //back to parent ...
      }
      else
      {
         c_SubDevice.q_ProgrammingSupport = false;
      }

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
      orc_DeviceDefinition.c_ImagePath = TGL_ExpandFileName(orc_DeviceDefinition.c_ImagePath,
                                                            TGL_ExtractFilePath(orc_Path));
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
         orc_DeviceDefinition.u8_NumCanBusses = static_cast<uint8>(u32_Value);
         u32_Value = orc_Parser.GetAttributeUint32("ethernet");
         if (u32_Value > 255)
         {
            osc_write_log_error("Loading device definition", "XML node \"ethernet\" contains invalid value.");
            s32_Return = C_CONFIG; //not a number
         }
         if (s32_Return == C_NO_ERR)
         {
            orc_DeviceDefinition.u8_NumEthernetBusses = static_cast<uint8>(u32_Value);
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
                  const uint16 u16_Bitrate = static_cast<uint16>(orc_Parser.GetAttributeUint32("value"));
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
      }
   }

   if (s32_Return == C_NO_ERR)
   {
      c_Text = orc_Parser.SelectNodeParent(); //back to parent of parent ...
      tgl_assert(c_Text == "opensyde-device-definition");

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
            const std::vector<C_OSCXMLAttribute> c_Attributes = orc_Parser.GetAttributes();
            for (uint16 u16_Index = 0U; u16_Index < c_Attributes.size(); u16_Index++)
            {
               if (c_Attributes[u16_Index].c_Name == "support")
               {
                  c_SubDevice.q_DiagnosticProtocolKefex = (c_Attributes[u16_Index].c_Value == "1");
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
            mh_ParseOpenSydeAvailability(orc_Parser, c_SubDevice.q_DiagnosticProtocolOpenSydeCan,
                                         c_SubDevice.q_DiagnosticProtocolOpenSydeEthernet);

            c_Text = orc_Parser.SelectNodeParent(); //back to parent ...
            tgl_assert(c_Text == "protocols-diagnostics");
         }
         c_Text = orc_Parser.SelectNodeParent(); //back to parent of parent ...
         tgl_assert(c_Text == "opensyde-device-definition");
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
               u32_Value = orc_Parser.GetAttributeUint32("value");

               // Due to compatibility use the value of the old format for all Flashloader reset wait times
               c_SubDevice.u32_FlashloaderResetWaitTimeNoChangesCan = u32_Value;
               c_SubDevice.u32_FlashloaderResetWaitTimeNoChangesEthernet = u32_Value;
               c_SubDevice.u32_FlashloaderResetWaitTimeNoFundamentalChangesCan = u32_Value;
               c_SubDevice.u32_FlashloaderResetWaitTimeNoFundamentalChangesEthernet = u32_Value;
               c_SubDevice.u32_FlashloaderResetWaitTimeFundamentalChangesCan = u32_Value;
               c_SubDevice.u32_FlashloaderResetWaitTimeFundamentalChangesEthernet = u32_Value;

               osc_write_log_info("Loading device definition",
                                  "Due to compatibility all flashloader reset wait times set to the"
                                  " same configuration value (" + C_SCLString::IntToStr(u32_Value) +
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
               c_SubDevice.u32_FlashloaderResetWaitTimeNoChangesCan = orc_Parser.GetAttributeUint32("value");
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
                                     C_SCLString::IntToStr(c_SubDevice.
                                                           u32_FlashloaderResetWaitTimeNoChangesCan) +
                                     " ms) for XML file \"" + orc_Path + "\" used.");
               }
            }

            c_Text = orc_Parser.SelectNodeChild("no-changes-ethernet");
            if (c_Text == "no-changes-ethernet")
            {
               c_SubDevice.u32_FlashloaderResetWaitTimeNoChangesEthernet = orc_Parser.GetAttributeUint32(
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
                                     C_SCLString::IntToStr(c_SubDevice.
                                                           u32_FlashloaderResetWaitTimeNoChangesEthernet) +
                                     " ms) for XML file \"" + orc_Path + "\" used.");
               }
            }

            c_Text = orc_Parser.SelectNodeChild("no-fundamental-com-changes-can");
            if (c_Text == "no-fundamental-com-changes-can")
            {
               c_SubDevice.u32_FlashloaderResetWaitTimeNoFundamentalChangesCan =
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
                                     C_SCLString::IntToStr(c_SubDevice.
                                                           u32_FlashloaderResetWaitTimeNoFundamentalChangesCan) +
                                     " ms) for XML file \"" + orc_Path + "\" used.");
               }
            }

            c_Text = orc_Parser.SelectNodeChild("no-fundamental-com-changes-ethernet");
            if (c_Text == "no-fundamental-com-changes-ethernet")
            {
               c_SubDevice.u32_FlashloaderResetWaitTimeNoFundamentalChangesEthernet =
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
                                     C_SCLString::IntToStr(c_SubDevice.
                                                           u32_FlashloaderResetWaitTimeNoFundamentalChangesEthernet) +
                                     " ms) for XML file \"" + orc_Path + "\" used.");
               }
            }

            c_Text = orc_Parser.SelectNodeChild("fundamental-com-changes-can");
            if (c_Text == "fundamental-com-changes-can")
            {
               c_SubDevice.u32_FlashloaderResetWaitTimeFundamentalChangesCan = orc_Parser.GetAttributeUint32(
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
                                     C_SCLString::IntToStr(c_SubDevice.
                                                           u32_FlashloaderResetWaitTimeFundamentalChangesCan) +
                                     " ms) for XML file \"" + orc_Path + "\" used.");
               }
            }

            c_Text = orc_Parser.SelectNodeChild("fundamental-com-changes-ethernet");
            if (c_Text == "fundamental-com-changes-ethernet")
            {
               c_SubDevice.u32_FlashloaderResetWaitTimeFundamentalChangesEthernet =
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
                                     C_SCLString::IntToStr(c_SubDevice.
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
            mh_ParseSTWFlashloaderAvailability(orc_Parser, c_SubDevice.q_FlashloaderStwCan);
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
            mh_ParseOpenSydeAvailability(orc_Parser, c_SubDevice.q_FlashloaderOpenSydeCan,
                                         c_SubDevice.q_FlashloaderOpenSydeEthernet);

            mh_ParseOpenSydeFlashloaderParameter(orc_Parser,
                                                 c_SubDevice.u32_FlashloaderOpenSydeRequestDownloadTimeout,
                                                 c_SubDevice.u32_FlashloaderOpenSydeTransferDataTimeout,
                                                 c_SubDevice.q_FlashloaderOpenSydeIsFileBased);

            c_Text = orc_Parser.SelectNodeParent(); //back to parent ...
            tgl_assert(c_Text == "protocols-flashloader");
         }
         c_Text = orc_Parser.SelectNodeParent(); //back to parent of parent ...
         tgl_assert(c_Text == "opensyde-device-definition");
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
            c_SubDevice.u32_UserEepromSizeBytes = orc_Parser.GetAttributeUint32("sizebytes");
         }
      }
   }
   orc_DeviceDefinition.c_SubDevices.push_back(c_SubDevice);
   return s32_Return;
}
