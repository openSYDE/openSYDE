//-----------------------------------------------------------------------------
/*!
   \internal
   \file
   \brief       File handler for device definition file data

   Handle device definition loading and saving

   \implementation
   project     openSYDE
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     06.09.2016  STW/A.Stangl
   \endimplementation
*/
//-----------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------- */
#include "precomp_headers.h"

#include "stwtypes.h"
#include "stwerrors.h"
#include "CSCLString.h"
#include "TGLFile.h"
#include "TGLUtils.h"
#include "C_OSCDeviceDefinitionFiler.h"
#include "C_OSCXMLParser.h"
#include "C_OSCLoggingHandler.h"

/* -- Used Namespaces ------------------------------------------------------ */
using namespace stw_types;
using namespace stw_errors;
using namespace stw_scl;
using namespace stw_tgl;
using namespace stw_opensyde_core;

/* -- Module Global Constants ---------------------------------------------- */

const uint16 C_OSCDeviceDefinitionFiler::mhu16_FILE_VERSION = 0x0001U;

/* -- Types ---------------------------------------------------------------- */

/* -- Global Variables ----------------------------------------------------- */

/* -- Module Global Variables ---------------------------------------------- */

/* -- Module Global Function Prototypes ------------------------------------ */

/* -- Implementation ------------------------------------------------------- */

//-----------------------------------------------------------------------------

void C_OSCDeviceDefinitionFiler::mh_ParseOpenSydeAvailability(const C_OSCXMLParser & orc_Parser,
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

//-----------------------------------------------------------------------------
/*!
   \brief   Gets the openSYDE flashloader parameter

   The parent "opensyde" in "protocols-flashloader" must be selected

   \param[in]     orc_Parser                    XML Parser
   \param[out]    orc_RequestDownloadTimeout    Parameter for Request Download Timeout
   \param[out]    orc_TransferDataTimeout       Parameter for Transfer Data Timeout

   \created     22.01.2018  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
void C_OSCDeviceDefinitionFiler::mh_ParseOpenSydeFlashloaderParameter(const C_OSCXMLParser & orc_Parser,
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

//-----------------------------------------------------------------------------

void C_OSCDeviceDefinitionFiler::mh_ParseSTWFlashloaderAvailability(const C_OSCXMLParser & orc_Parser,
                                                                    bool & orq_ProtocolSupportedCan)
{
   //no check for existence of entries: fall back to "not supported" in this case
   bool q_Support;
   bool q_Can;

   q_Support  = orc_Parser.GetAttributeBool("support");
   q_Can      = orc_Parser.GetAttributeBool("can");
   orq_ProtocolSupportedCan = (q_Support && q_Can);
}

//-----------------------------------------------------------------------------
/*!
   \brief   Load device definition

   Load data from specified file and place it in device definition instance

   \param[out]    orc_DeviceDefinition    device definition information read from file
   \param[in]     orc_Path                path to file

   \return
   C_NO_ERR   data read and placed into device definition instance
   C_RANGE    specified file does not exist
   C_NOACT    specified file is invalid (invalid XML file)
   C_CONFIG   content of file is invalid or incomplete

   \created     06.09.2016  STW/A.Stangl
*/
//-----------------------------------------------------------------------------
sint32 C_OSCDeviceDefinitionFiler::h_Load(C_OSCDeviceDefinition & orc_DeviceDefinition,
                                          const stw_scl::C_SCLString & orc_Path)
{
   sint32 s32_Return = C_NO_ERR;

   if (TGL_FileExists(orc_Path) == false)
   {
      osc_write_log_error("Loading device definition", "File not found: \"" + orc_Path + "\".");
      s32_Return = C_RANGE;
   }
   else
   {
      C_OSCXMLParser c_XML;
      //"empty" device definition to have a clearly defined status:
      orc_DeviceDefinition.Clear();

      s32_Return = c_XML.LoadFromFile(orc_Path); //open XML file
      if (s32_Return == C_NO_ERR)
      {
         C_SCLString c_Text;
         uint16 u16_Index;
         uint32 u32_Value;
         std::vector<C_OSCXMLAttribute> c_Attributes;
         //Check if root node exists:
         c_Text = c_XML.SelectRoot();

         if (c_Text != "opensyde-device-definition")
         {
            osc_write_log_error("Loading device definition", "XML node \"opensyde-device-definition\" not found.");
            s32_Return = C_CONFIG;
         }
         else
         {
            c_Text = c_XML.SelectNodeChild("file-version");
            if (c_Text != "file-version")
            {
               osc_write_log_error("Loading device definition", "XML node \"file-version\" not found.");
               s32_Return = C_CONFIG;
            }
         }
         if (s32_Return == C_NO_ERR)
         {
            //no special handling required yet based on version
            c_XML.SelectNodeParent(); //back to parent ...
            c_Text = c_XML.SelectNodeChild("device-name");
            if (c_Text != "device-name")
            {
               osc_write_log_error("Loading device definition", "XML node \"device-name\" not found.");
               s32_Return = C_CONFIG;
            }
         }
         if (s32_Return == C_NO_ERR)
         {
            orc_DeviceDefinition.c_DeviceName = c_XML.GetNodeContent();
            //Optional alias
            c_XML.SelectNodeParent(); //back to parent ...
            if (c_XML.SelectNodeChild("device-name-alias") == "device-name-alias")
            {
               orc_DeviceDefinition.c_DeviceNameAlias = c_XML.GetNodeContent();
               c_XML.SelectNodeParent(); //back to parent ...
            }
            c_Text = c_XML.SelectNodeChild("device-description");
            if (c_Text != "device-description")
            {
               osc_write_log_error("Loading device definition", "XML node \"device-description\" not found.");
               s32_Return = C_CONFIG;
            }
         }
         if (s32_Return == C_NO_ERR)
         {
            orc_DeviceDefinition.c_DeviceDescription = c_XML.GetNodeContent();

            c_XML.SelectNodeParent(); //back to parent ...
            c_Text = c_XML.SelectNodeChild("programming-properties");
            if (c_Text == "programming-properties")
            {
               if (c_XML.AttributeExists("is-programmable") == true)
               {
                  orc_DeviceDefinition.q_ProgrammingSupport = c_XML.GetAttributeBool("is-programmable");
               }
               else
               {
                  osc_write_log_error("Loading device definition", "XML attribute \"is-programmable\" not found.");
                  s32_Return = C_CONFIG;
               }
               c_XML.SelectNodeParent(); //back to parent ...
            }
            else
            {
               orc_DeviceDefinition.q_ProgrammingSupport = false;
            }

            c_Text = c_XML.SelectNodeChild("image");
            if (c_Text != "image")
            {
               osc_write_log_error("Loading device definition", "XML node \"image\" not found.");
               s32_Return = C_CONFIG;
            }
         }
         if (s32_Return == C_NO_ERR)
         {
            orc_DeviceDefinition.c_ImagePath = c_XML.GetNodeContent();
            c_XML.SelectNodeParent(); //back to parent ...
            c_Text = c_XML.SelectNodeChild("bus-systems-available");
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
            u32_Value = c_XML.GetAttributeUint32("can");
            if (u32_Value > 255)
            {
               osc_write_log_error("Loading device definition", "XML node \"can\" contains invalid value.");
               s32_Return = C_CONFIG; //not a valid number
            }
            if (s32_Return == C_NO_ERR)
            {
               orc_DeviceDefinition.u8_NumCanBusses = static_cast<uint8>(u32_Value);
               u32_Value = c_XML.GetAttributeUint32("ethernet");
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
            c_XML.SelectNodeParent(); //back to parent ...
            c_Text = c_XML.SelectNodeChild("can-bitrates-support");
            if (c_Text != "can-bitrates-support")
            {
               osc_write_log_error("Loading device definition", "XML node \"can-bitrates-support\" not found.");
               s32_Return = C_CONFIG;
            }
            if (s32_Return == C_NO_ERR)
            {
               //get bitrates
               c_Text = c_XML.SelectNodeChild("can-bitrate");
               if (c_Text == "can-bitrate")
               {
                  do
                  {
                     if (c_XML.AttributeExists("value") == true)
                     {
                        const uint16 u16_Bitrate = static_cast<uint16>(c_XML.GetAttributeUint32("value"));
                        orc_DeviceDefinition.c_SupportedBitrates.push_back(u16_Bitrate);
                     }
                     else
                     {
                        osc_write_log_error("Loading device definition",
                                            "XML node \"can-bitrate\" attribute \"value\" not found.");
                        s32_Return = C_CONFIG;
                     }
                     c_Text = c_XML.SelectNodeNext("can-bitrate");
                  }
                  while ((c_Text == "can-bitrate") && (s32_Return == C_NO_ERR));
                  c_XML.SelectNodeParent(); //back to parent of parent ...
               }
            }
         }

         if (s32_Return == C_NO_ERR)
         {
            c_Text = c_XML.SelectNodeParent(); //back to parent of parent ...
            tgl_assert(c_Text == "opensyde-device-definition");

            c_Text = c_XML.SelectNodeChild("protocols-diagnostics");
            if (c_Text != "protocols-diagnostics")
            {
               osc_write_log_error("Loading device definition", "XML node \"protocols-diagnostics\" not found.");
               s32_Return = C_CONFIG;
            }
            else
            {
               //get sub-node
               c_Text = c_XML.SelectNodeChild("kefex");
               if (c_Text != "kefex")
               {
                  osc_write_log_error("Loading device definition",
                                      "XML node \"protocols-diagnostics\".\"kefex\" not found.");
                  s32_Return = C_CONFIG;
               }
            }
         }
         if (s32_Return == C_NO_ERR)
         {
            c_Attributes = c_XML.GetAttributes();
            for (u16_Index = 0U; u16_Index < c_Attributes.size(); u16_Index++)
            {
               if (c_Attributes[u16_Index].c_Name == "support")
               {
                  orc_DeviceDefinition.q_DiagnosticProtocolKefex = (c_Attributes[u16_Index].c_Value == "1");
               }
               else
               {
                  //unknown attribute; nothing we can do with it
               }
            }
            c_XML.SelectNodeParent(); //back to parent ...
            c_Text = c_XML.SelectNodeChild("opensyde");
            if (c_Text != "opensyde")
            {
               osc_write_log_error("Loading device definition",
                                   "XML node \"protocols-diagnostics\".\"opensyde\" not found.");
               s32_Return = C_CONFIG;
            }
         }
         if (s32_Return == C_NO_ERR)
         {
            mh_ParseOpenSydeAvailability(c_XML, orc_DeviceDefinition.q_DiagnosticProtocolOpenSydeCan,
                                         orc_DeviceDefinition.q_DiagnosticProtocolOpenSydeEthernet);

            c_XML.SelectNodeParent(); //back to parent ...
            c_XML.SelectNodeParent(); //back to parent of parent ...

            c_Text = c_XML.SelectNodeChild("protocols-flashloader");
            if (c_Text != "protocols-flashloader")
            {
               osc_write_log_error("Loading device definition", "XML node \"protocols-flashloader\" not found.");
               s32_Return = C_CONFIG;
            }
            else
            {
               //get sub-node
               c_Text = c_XML.SelectNodeChild("stw-flashloader");
               if (c_Text != "stw-flashloader")
               {
                  osc_write_log_error("Loading device definition",
                                      "XML node \"protocols-flashloader\".\"stw-flashloader\" not found.");
                  s32_Return = C_CONFIG;
               }
            }
         }
         if (s32_Return == C_NO_ERR)
         {
            mh_ParseSTWFlashloaderAvailability(c_XML, orc_DeviceDefinition.q_FlashloaderStwCan);
            c_XML.SelectNodeParent(); //back to parent ...
            c_Text = c_XML.SelectNodeChild("opensyde");
            if (c_Text != "opensyde")
            {
               osc_write_log_error("Loading device definition",
                                   "XML node \"protocols-flashloader\".\"opensyde\" not found.");
               s32_Return = C_CONFIG;
            }
         }
         if (s32_Return == C_NO_ERR)
         {
            mh_ParseOpenSydeAvailability(c_XML, orc_DeviceDefinition.q_FlashloaderOpenSydeCan,
                                         orc_DeviceDefinition.q_FlashloaderOpenSydeEthernet);

            mh_ParseOpenSydeFlashloaderParameter(c_XML,
                                                 orc_DeviceDefinition.u32_FlashloaderOpenSydeRequestDownloadTimeout,
                                                 orc_DeviceDefinition.u32_FlashloaderOpenSydeTransferDataTimeout,
                                                 orc_DeviceDefinition.q_FlashloaderOpenSydeIsFileBased);

            c_XML.SelectNodeParent(); //back to parent ...
            c_XML.SelectNodeParent(); //back to parent of parent ...

            c_Text = c_XML.SelectNodeChild("memory");
            if (c_Text != "memory")
            {
               osc_write_log_error("Loading device definition", "XML node \"memory\" not found.");
               s32_Return = C_CONFIG;
            }
         }
         if (s32_Return == C_NO_ERR)
         {
            c_Text = c_XML.SelectNodeChild("user-eeprom");
            if (c_Text != "user-eeprom")
            {
               osc_write_log_error("Loading device definition", "XML node \"memory\".\"user-eeprom\" not found.");
               s32_Return = C_CONFIG;
            }
            else
            {
               orc_DeviceDefinition.u32_UserEepromSizeBytes = c_XML.GetAttributeUint32("sizebytes");
            }
         }
      }
      else
      {
         osc_write_log_error("Loading device definition", "Could not open XML file \"" + orc_Path + "\".");
         s32_Return = C_NOACT;
      }
   }
   return s32_Return;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Save device definition

   Save device definition data into specified file.
   Will overwrite the file if it already exists.

   \param[in]    orc_DeviceDefinition    device definition information to write to file
   \param[in]    orc_Path                path to file

   \return
   C_NO_ERR   data written to file
   C_RD_WR    could not erase pre-existing file before saving
   C_RD_WR    could not write to file (e.g. missing write permissions; missing folder)

   \created     06.09.2016  STW/A.Stangl
*/
//-----------------------------------------------------------------------------
sint32 C_OSCDeviceDefinitionFiler::h_Save(const C_OSCDeviceDefinition & orc_DeviceDefinition,
                                          const C_SCLString & orc_Path)
{
   sint32 s32_Return = C_NO_ERR;

   if (TGL_FileExists(orc_Path) == true)
   {
      //erase it:
      sintn sn_Return;
      sn_Return = std::remove(orc_Path.c_str());
      if (sn_Return != 0)
      {
         osc_write_log_error("Saving device definition", "Could not erase pre-existing file \"" + orc_Path + "\".");
         s32_Return = C_RD_WR;
      }
   }
   if (s32_Return == C_NO_ERR)
   {
      uint32 u32_Counter;
      C_OSCXMLParser c_XML;
      c_XML.CreateNodeChild("opensyde-device-definition"); //root node
      c_XML.SelectRoot();
      c_XML.CreateNodeChild("file-version", "0x" + C_SCLString::IntToHex(mhu16_FILE_VERSION, 4U));
      c_XML.CreateNodeChild("device-name", orc_DeviceDefinition.c_DeviceName);
      c_XML.CreateNodeChild("device-name-alias", orc_DeviceDefinition.c_DeviceNameAlias);
      c_XML.CreateNodeChild("device-description", orc_DeviceDefinition.c_DeviceDescription);
      c_XML.CreateAndSelectNodeChild("programming-properties");
      c_XML.SetAttributeBool("is-programmable", orc_DeviceDefinition.q_ProgrammingSupport);
      c_XML.SelectNodeParent();
      c_XML.CreateNodeChild("image", orc_DeviceDefinition.c_ImagePath);
      c_XML.CreateAndSelectNodeChild("bus-systems-available");
      c_XML.SetAttributeUint32("can", orc_DeviceDefinition.u8_NumCanBusses);
      c_XML.SetAttributeUint32("ethernet", orc_DeviceDefinition.u8_NumEthernetBusses);
      c_XML.SelectNodeParent();
      c_XML.CreateAndSelectNodeChild("can-bitrates-support");
      for (u32_Counter = 0U; u32_Counter < orc_DeviceDefinition.c_SupportedBitrates.size(); ++u32_Counter)
      {
         c_XML.CreateAndSelectNodeChild("can-bitrate");
         c_XML.SetAttributeString("value", orc_DeviceDefinition.c_SupportedBitrates[u32_Counter]);
         c_XML.SelectNodeParent();
      }
      c_XML.SelectNodeParent();
      c_XML.CreateAndSelectNodeChild("protocols-diagnostics");
      c_XML.CreateAndSelectNodeChild("kefex");
      c_XML.SetAttributeBool("support", orc_DeviceDefinition.q_DiagnosticProtocolKefex);
      c_XML.SelectNodeParent();
      c_XML.CreateAndSelectNodeChild("opensyde");
      c_XML.SetAttributeBool("support",
                             (orc_DeviceDefinition.q_DiagnosticProtocolOpenSydeCan ||
                              orc_DeviceDefinition.q_DiagnosticProtocolOpenSydeEthernet));
      c_XML.SetAttributeBool("can", orc_DeviceDefinition.q_DiagnosticProtocolOpenSydeCan);
      c_XML.SetAttributeBool("ethernet", orc_DeviceDefinition.q_DiagnosticProtocolOpenSydeEthernet);
      c_XML.SelectNodeParent();
      c_XML.SelectNodeParent();

      c_XML.CreateAndSelectNodeChild("protocols-flashloader");
      c_XML.CreateAndSelectNodeChild("stw-flashloader");
      c_XML.SetAttributeBool("support", orc_DeviceDefinition.q_FlashloaderStwCan);
      c_XML.SetAttributeBool("can", orc_DeviceDefinition.q_FlashloaderStwCan);
      c_XML.SelectNodeParent();
      c_XML.CreateAndSelectNodeChild("opensyde");
      c_XML.SetAttributeBool("support",
                             (orc_DeviceDefinition.q_FlashloaderOpenSydeCan ||
                              orc_DeviceDefinition.q_FlashloaderOpenSydeEthernet));
      c_XML.SetAttributeBool("can", orc_DeviceDefinition.q_FlashloaderOpenSydeCan);
      c_XML.SetAttributeBool("ethernet", orc_DeviceDefinition.q_FlashloaderOpenSydeEthernet);
      c_XML.SetAttributeUint32("requestdownloadtimeout",
                               orc_DeviceDefinition.u32_FlashloaderOpenSydeRequestDownloadTimeout);
      c_XML.SetAttributeUint32("transferdatatimeout",
                               orc_DeviceDefinition.u32_FlashloaderOpenSydeTransferDataTimeout);
      c_XML.SetAttributeBool("is-file-based", orc_DeviceDefinition.q_FlashloaderOpenSydeIsFileBased);
      c_XML.SelectNodeParent();
      c_XML.SelectNodeParent();
      c_XML.CreateAndSelectNodeChild("memory");
      c_XML.CreateAndSelectNodeChild("user-eeprom");
      c_XML.SetAttributeUint32("sizebytes", orc_DeviceDefinition.u32_UserEepromSizeBytes);
      s32_Return = c_XML.SaveToFile(orc_Path);
      if (s32_Return != C_NO_ERR)
      {
         osc_write_log_error("Saving Device definition", "Could not write to file \"" + orc_Path + "\".");
         s32_Return = C_RD_WR;
      }
   }
   return s32_Return;
}
