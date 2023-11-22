//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Data class for flashloader information

   Collection of information that can be read from the flashloader, containing
   flashloader versions, flash count, serial number, article number, fingerprint data, etc.

   \copyright   Copyright 2023 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.hpp"

#include "stwtypes.hpp"
//#include "C_SclString.hpp"
#include "C_OscComFlashloaderInformation.hpp"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw::scl;
using namespace stw::opensyde_core;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

/* -- Global Variables ---------------------------------------------------------------------------------------------- */

/* -- Module Global Variables --------------------------------------------------------------------------------------- */

/* -- Module Global Function Prototypes ----------------------------------------------------------------------------- */

/* -- Implementation ------------------------------------------------------------------------------------------------ */

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Default constructor
*/
//----------------------------------------------------------------------------------------------------------------------
C_OscComFlashloaderInformation::C_OscComFlashloaderInformation(void) :
   u32_FlashCount(0),
   u32_EcuArticleNumber(0),
   u16_MaxNumberOfBlockLength(0)
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Returns the Ecu serial number dependent of the extended serial number flag and the manufacturer format

   \return
   Formatted serial number string
*/
//----------------------------------------------------------------------------------------------------------------------
stw::scl::C_SclString C_OscComFlashloaderInformation::GetEcuSerialNumber(void) const
{
   return this->c_SerialNumber.GetSerialNumberAsFormattedString();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Returns the Ecu serial number description

   Formats:
   * Standard
   * Extended
   ** Manufacturer format

   \return
   String with description of the serial number format
*/
//----------------------------------------------------------------------------------------------------------------------
stw::scl::C_SclString C_OscComFlashloaderInformation::GetEcuSerialNumberFormatDescription(void) const
{
   C_SclString c_Return = "(Format: ";

   if (this->c_AvailableFeatures.q_ExtendedSerialNumberModeImplemented == false)
   {
      c_Return += "Standard";
   }
   else
   {
      c_Return += "Extended with Manufacturer Format " +
                  C_SclString::IntToStr(this->c_SerialNumber.u8_SerialNumberManufacturerFormat);
   }
   c_Return += ")";

   return c_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Convert information read from openSYDE server node to string list

   Can be used for a no-frills approach to get a textual representation of the information held by this class.

   \return
   Textual represenation of this class
*/
//----------------------------------------------------------------------------------------------------------------------
C_SclStringList C_OscComFlashloaderInformation::FlashloaderInformationToText(void) const
{
   C_SclStringList c_Text;
   C_SclString c_Line;

   c_Text.Clear();
   c_Line.PrintFormatted("Flashloader software version: V%d.%02dr%d",
                         au8_FlashloaderSoftwareVersion[0],
                         au8_FlashloaderSoftwareVersion[1],
                         au8_FlashloaderSoftwareVersion[2]);
   c_Text.Add(c_Line);
   c_Line.PrintFormatted("Flashloader protocol version: V%d.%02dr%d",
                         au8_FlashloaderProtocolVersion[0],
                         au8_FlashloaderProtocolVersion[1],
                         au8_FlashloaderProtocolVersion[2]);
   c_Text.Add(c_Line);
   c_Line.PrintFormatted("Protocol version: V%d.%02dr%d", au8_ProtocolVersion[0],
                         au8_ProtocolVersion[1],
                         au8_ProtocolVersion[2]);
   c_Text.Add(c_Line);
   c_Text.Add("Flash count: " + C_SclString::IntToStr(u32_FlashCount));
   c_Line = "Device serial number: " + GetEcuSerialNumber() + " " +
            GetEcuSerialNumberFormatDescription();
   c_Text.Add(c_Line);
   c_Text.Add("Device article number: " + C_SclString::IntToStr(u32_EcuArticleNumber));
   c_Text.Add("Device article version: " + c_EcuHardwareVersionNumber);
   c_Line.PrintFormatted("Flash fingerprint date: %02d-%02d-%02d (yy-mm-dd)",
                         au8_FlashFingerprintDate[0],
                         au8_FlashFingerprintDate[1],
                         au8_FlashFingerprintDate[2]);
   c_Text.Add(c_Line);
   c_Line.PrintFormatted("Flash fingerprint time: %02d:%02d:%02d",
                         au8_FlashFingerprintTime[0],
                         au8_FlashFingerprintTime[1],
                         au8_FlashFingerprintTime[2]);
   c_Text.Add(c_Line);
   c_Text.Add("Flash fingerprint username: " + c_FlashFingerprintUserName);
   c_Line.PrintFormatted("NVM writing available: %d",
                         (c_AvailableFeatures.q_FlashloaderCanWriteToNvm == true) ? 1 : 0);
   c_Text.Add(c_Line);
   c_Line.PrintFormatted("Security supported: %d",
                         (c_AvailableFeatures.q_SupportsSecurity == true) ? 1 : 0);
   c_Text.Add(c_Line);
   c_Line.PrintFormatted("Disabling debugger supported: %d",
                         (c_AvailableFeatures.q_SupportsDebuggerOff == true) ? 1 : 0);
   c_Text.Add(c_Line);
   c_Line.PrintFormatted("Enabling debugger supported: %d",
                         (c_AvailableFeatures.q_SupportsDebuggerOn == true) ? 1 : 0);
   c_Text.Add(c_Line);
   c_Line.PrintFormatted("Maximum block size information available: %d",
                         (c_AvailableFeatures.q_MaxNumberOfBlockLengthAvailable == true) ? 1 : 0);
   c_Text.Add(c_Line);
   if (c_AvailableFeatures.q_MaxNumberOfBlockLengthAvailable == true)
   {
      c_Line.PrintFormatted("Maximum block size: %d", u16_MaxNumberOfBlockLength);
      c_Text.Add(c_Line);
   }
   c_Line.PrintFormatted("Ethernet2Ethernet routing supported: %d",
                         (c_AvailableFeatures.q_EthernetToEthernetRoutingSupported == true) ? 1 : 0);
   c_Text.Add(c_Line);
   c_Line.PrintFormatted("FileBasedTransferExitResult available: %d",
                         (c_AvailableFeatures.q_FileBasedTransferExitResultAvailable == true) ? 1 : 0);
   c_Text.Add(c_Line);

   return c_Text;
}
