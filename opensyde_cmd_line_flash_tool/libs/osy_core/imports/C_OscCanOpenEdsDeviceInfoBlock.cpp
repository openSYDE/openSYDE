//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Class for device info block handling of EDS/DCF files

   Class for device info block handling of EDS/DCF files

   \copyright   Copyright 2022 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.hpp"

#include "stwerrors.hpp"
#include "C_SclChecksums.hpp"
#include "C_OscCanOpenEdsDeviceInfoBlock.hpp"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */

using namespace stw::errors;
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
C_OscCanOpenEdsDeviceInfoBlock::C_OscCanOpenEdsDeviceInfoBlock() :
   q_BaudRate10(false),
   q_BaudRate20(false),
   q_BaudRate50(false),
   q_BaudRate125(false),
   q_BaudRate250(false),
   q_BaudRate500(false),
   q_BaudRate800(false),
   q_BaudRate1000(false),
   q_SimpleBootUpMaster(false),
   q_SimpleBootUpSlave(false),
   u8_Granularity(0),
   q_GroupMessaging(false),
   u16_NrOfRxPdo(0),
   u16_NrOfTxPdo(0),
   q_LssSupported(false)
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Calculates the hash value over all data

   The hash value is a 32 bit CRC value.

   \param[in,out]  oru32_HashValue  Hash value with unit [in] value and result [out] value
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OscCanOpenEdsDeviceInfoBlock::CalcHash(uint32_t & oru32_HashValue) const
{
   stw::scl::C_SclChecksums::CalcCRC32(this->c_VendorName.c_str(), this->c_VendorName.Length(), oru32_HashValue);
   stw::scl::C_SclChecksums::CalcCRC32(this->c_VendorNumber.c_str(), this->c_VendorNumber.Length(), oru32_HashValue);
   stw::scl::C_SclChecksums::CalcCRC32(this->c_ProductName.c_str(), this->c_ProductName.Length(), oru32_HashValue);
   stw::scl::C_SclChecksums::CalcCRC32(this->c_ProductNumber.c_str(), this->c_ProductNumber.Length(), oru32_HashValue);
   stw::scl::C_SclChecksums::CalcCRC32(this->c_RevisionNumber.c_str(), this->c_RevisionNumber.Length(),
                                       oru32_HashValue);
   stw::scl::C_SclChecksums::CalcCRC32(this->c_OrderCode.c_str(), this->c_OrderCode.Length(), oru32_HashValue);
   stw::scl::C_SclChecksums::CalcCRC32(&this->q_BaudRate10, sizeof(this->q_BaudRate10),
                                       oru32_HashValue);
   stw::scl::C_SclChecksums::CalcCRC32(&this->q_BaudRate20, sizeof(this->q_BaudRate20),
                                       oru32_HashValue);
   stw::scl::C_SclChecksums::CalcCRC32(&this->q_BaudRate50, sizeof(this->q_BaudRate50),
                                       oru32_HashValue);
   stw::scl::C_SclChecksums::CalcCRC32(&this->q_BaudRate125, sizeof(this->q_BaudRate125),
                                       oru32_HashValue);
   stw::scl::C_SclChecksums::CalcCRC32(&this->q_BaudRate250, sizeof(this->q_BaudRate250),
                                       oru32_HashValue);
   stw::scl::C_SclChecksums::CalcCRC32(&this->q_BaudRate500, sizeof(this->q_BaudRate500),
                                       oru32_HashValue);
   stw::scl::C_SclChecksums::CalcCRC32(&this->q_BaudRate800, sizeof(this->q_BaudRate800),
                                       oru32_HashValue);
   stw::scl::C_SclChecksums::CalcCRC32(&this->q_BaudRate1000, sizeof(this->q_BaudRate1000),
                                       oru32_HashValue);
   stw::scl::C_SclChecksums::CalcCRC32(&this->q_SimpleBootUpMaster, sizeof(this->q_SimpleBootUpMaster),
                                       oru32_HashValue);
   stw::scl::C_SclChecksums::CalcCRC32(&this->q_SimpleBootUpSlave, sizeof(this->q_SimpleBootUpSlave),
                                       oru32_HashValue);
   stw::scl::C_SclChecksums::CalcCRC32(&this->u8_Granularity, sizeof(this->u8_Granularity),
                                       oru32_HashValue);
   stw::scl::C_SclChecksums::CalcCRC32(this->c_DynamicChannelsSupported.c_str(),
                                       this->c_DynamicChannelsSupported.Length(), oru32_HashValue);
   stw::scl::C_SclChecksums::CalcCRC32(&this->q_GroupMessaging, sizeof(this->q_GroupMessaging),
                                       oru32_HashValue);
   stw::scl::C_SclChecksums::CalcCRC32(&this->u16_NrOfRxPdo, sizeof(this->u16_NrOfRxPdo),
                                       oru32_HashValue);
   stw::scl::C_SclChecksums::CalcCRC32(&this->u16_NrOfTxPdo, sizeof(this->u16_NrOfTxPdo),
                                       oru32_HashValue);
   stw::scl::C_SclChecksums::CalcCRC32(&this->q_LssSupported, sizeof(this->q_LssSupported),
                                       oru32_HashValue);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Load from ini

   \param[in,out]  orc_File         File
   \param[in,out]  orc_LastError    Last error

   \return
   STW error codes

   \retval   C_NO_ERR   Values read
   \retval   C_CONFIG   At least one value not found, for details see error message
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_OscCanOpenEdsDeviceInfoBlock::LoadFromIni(stw::scl::C_SclIniFile & orc_File,
                                                    stw::scl::C_SclString & orc_LastError)
{
   //lint -e{8062} Kept for later error reporting
   const int32_t s32_Retval = C_NO_ERR;
   const stw::scl::C_SclString c_SectionName = "DeviceInfo";

   orc_LastError = "";

   if (orc_File.SectionExists(c_SectionName))
   {
      //Maybe mandatory values
      this->c_VendorName = orc_File.ReadString(c_SectionName, "VendorName", "");
      this->c_ProductName = orc_File.ReadString(c_SectionName, "ProductName", "");
      this->q_BaudRate10 = orc_File.ReadBool(c_SectionName, "BaudRate_10", false);
      this->q_BaudRate20 = orc_File.ReadBool(c_SectionName, "BaudRate_20", false);
      this->q_BaudRate50 = orc_File.ReadBool(c_SectionName, "BaudRate_50", false);
      this->q_BaudRate125 = orc_File.ReadBool(c_SectionName, "BaudRate_125", false);
      this->q_BaudRate250 = orc_File.ReadBool(c_SectionName, "BaudRate_250", false);
      this->q_BaudRate500 = orc_File.ReadBool(c_SectionName, "BaudRate_500", false);
      this->q_BaudRate800 = orc_File.ReadBool(c_SectionName, "BaudRate_800", false);
      this->q_BaudRate1000 = orc_File.ReadBool(c_SectionName, "BaudRate_1000", false);
      this->q_SimpleBootUpMaster = orc_File.ReadBool(c_SectionName, "SimpleBootUpMaster", false);
      this->q_SimpleBootUpSlave = orc_File.ReadBool(c_SectionName, "SimpleBootUpSlave", false);
      this->u8_Granularity = orc_File.ReadUint8(c_SectionName, "Granularity", 0);
      this->c_DynamicChannelsSupported = orc_File.ReadString(c_SectionName, "DynamicChannelsSupported", "0");
      this->c_RevisionNumber = orc_File.ReadString(c_SectionName, "RevisionNumber", "");
      //Optional
      this->c_RevisionNumber = orc_File.ReadString(c_SectionName, "RevisionNumber", "");
      this->q_GroupMessaging = orc_File.ReadBool(c_SectionName, "GroupMessaging", false);
      this->u16_NrOfRxPdo = orc_File.ReadUint16(c_SectionName, "NrOfRxPDO", 0);
      this->u16_NrOfTxPdo = orc_File.ReadUint16(c_SectionName, "NrOfTxPDO", 0);
      this->q_LssSupported = orc_File.ReadBool(c_SectionName, "LSS_Supported", false);
      this->c_VendorNumber = orc_File.ReadString(c_SectionName, "VendorNumber", "");
      this->c_ProductNumber = orc_File.ReadString(c_SectionName, "ProductNumber", "");
      this->c_OrderCode = orc_File.ReadString(c_SectionName, "OrderCode", "");
   }

   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get granularity

   \return
   Granularity
*/
//----------------------------------------------------------------------------------------------------------------------
uint8_t C_OscCanOpenEdsDeviceInfoBlock::GetGranularity() const
{
   return this->u8_Granularity;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Load string value from ini file

   \param[in,out]  orc_File            File
   \param[in]      orc_SectionName     Section name
   \param[in]      orc_KeyName         Key name
   \param[in,out]  orc_OutputValue     Output value
   \param[in,out]  orc_ErrorMessage    Error message
   \param[in]      orc_DefaultValue    Default value

   \return
   STW error codes

   \retval   C_NO_ERR   Value read
   \retval   C_CONFIG   Value not found, for details see error message
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_OscCanOpenEdsDeviceInfoBlock::h_LoadStringValueFromIniFile(stw::scl::C_SclIniFile & orc_File,
                                                                     const stw::scl::C_SclString & orc_SectionName,
                                                                     const stw::scl::C_SclString & orc_KeyName,
                                                                     stw::scl::C_SclString & orc_OutputValue,
                                                                     stw::scl::C_SclString & orc_ErrorMessage,
                                                                     const stw::scl::C_SclString & orc_DefaultValue)
{
   int32_t s32_Retval = C_NO_ERR;

   if (orc_File.ValueExists(orc_SectionName, orc_KeyName))
   {
      orc_OutputValue = orc_File.ReadString(orc_SectionName, orc_KeyName, orc_DefaultValue);
   }
   else
   {
      orc_OutputValue = orc_DefaultValue;
      s32_Retval = C_CONFIG;
      C_OscCanOpenEdsDeviceInfoBlock::h_ReportMissingKeyError(orc_SectionName, orc_KeyName, orc_ErrorMessage);
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Load u8 value from ini file

   \param[in]      orc_File            File
   \param[in]      orc_SectionName     Section name
   \param[in]      orc_KeyName         Key name
   \param[out]     oru8_OutputValue    Output value
   \param[in,out]  orc_ErrorMessage    Error message
   \param[in]      ou8_DefaultValue    Default value

   \return
   STW error codes

   \retval   C_NO_ERR   Value read
   \retval   C_CONFIG   Value not found, for details see error message
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_OscCanOpenEdsDeviceInfoBlock::h_LoadU8ValueFromIniFile(stw::scl::C_SclIniFile & orc_File,
                                                                 const stw::scl::C_SclString & orc_SectionName,
                                                                 const stw::scl::C_SclString & orc_KeyName,
                                                                 uint8_t & oru8_OutputValue,
                                                                 stw::scl::C_SclString & orc_ErrorMessage,
                                                                 const uint8_t ou8_DefaultValue)
{
   int32_t s32_Retval = C_NO_ERR;

   if (orc_File.ValueExists(orc_SectionName, orc_KeyName))
   {
      oru8_OutputValue = orc_File.ReadUint8(orc_SectionName, orc_KeyName, ou8_DefaultValue);
   }
   else
   {
      oru8_OutputValue = ou8_DefaultValue;
      s32_Retval = C_CONFIG;
      C_OscCanOpenEdsDeviceInfoBlock::h_ReportMissingKeyError(orc_SectionName, orc_KeyName, orc_ErrorMessage);
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Load u16 value from ini file

   \param[in,out]  orc_File            File
   \param[in]      orc_SectionName     Section name
   \param[in]      orc_KeyName         Key name
   \param[out]     oru16_OutputValue   Output value
   \param[in,out]  orc_ErrorMessage    Error message
   \param[in]      ou16_DefaultValue   Default value

   \return
   STW error codes

   \retval   C_NO_ERR   Value read
   \retval   C_CONFIG   Value not found, for details see error message
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_OscCanOpenEdsDeviceInfoBlock::h_LoadU16ValueFromIniFile(stw::scl::C_SclIniFile & orc_File,
                                                                  const stw::scl::C_SclString & orc_SectionName,
                                                                  const stw::scl::C_SclString & orc_KeyName,
                                                                  uint16_t & oru16_OutputValue,
                                                                  stw::scl::C_SclString & orc_ErrorMessage,
                                                                  const uint16_t ou16_DefaultValue)
{
   int32_t s32_Retval = C_NO_ERR;

   if (orc_File.ValueExists(orc_SectionName, orc_KeyName))
   {
      oru16_OutputValue = orc_File.ReadUint16(orc_SectionName, orc_KeyName, ou16_DefaultValue);
   }
   else
   {
      oru16_OutputValue = ou16_DefaultValue;
      s32_Retval = C_CONFIG;
      C_OscCanOpenEdsDeviceInfoBlock::h_ReportMissingKeyError(orc_SectionName, orc_KeyName, orc_ErrorMessage);
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Load bool value from ini file

   \param[in,out]  orc_File            File
   \param[in]      orc_SectionName     Section name
   \param[in]      orc_KeyName         Key name
   \param[out]     orq_OutputValue     Output value
   \param[in,out]  orc_ErrorMessage    Error message
   \param[in]      oq_DefaultValue     Default value

   \return
   STW error codes

   \retval   C_NO_ERR   Value read
   \retval   C_CONFIG   Value not found, for details see error message
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_OscCanOpenEdsDeviceInfoBlock::h_LoadBoolValueFromIniFile(stw::scl::C_SclIniFile & orc_File,
                                                                   const stw::scl::C_SclString & orc_SectionName,
                                                                   const stw::scl::C_SclString & orc_KeyName,
                                                                   bool & orq_OutputValue,
                                                                   stw::scl::C_SclString & orc_ErrorMessage,
                                                                   const bool oq_DefaultValue)
{
   int32_t s32_Retval = C_NO_ERR;

   if (orc_File.ValueExists(orc_SectionName, orc_KeyName))
   {
      orq_OutputValue = orc_File.ReadBool(orc_SectionName, orc_KeyName, oq_DefaultValue);
   }
   else
   {
      orq_OutputValue = oq_DefaultValue;
      s32_Retval = C_CONFIG;
      C_OscCanOpenEdsDeviceInfoBlock::h_ReportMissingKeyError(orc_SectionName, orc_KeyName, orc_ErrorMessage);
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Report missing section error

   \param[in]      orc_SectionName     Section name
   \param[in,out]  orc_ErrorMessage    Error message
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OscCanOpenEdsDeviceInfoBlock::h_ReportMissingSectionError(const stw::scl::C_SclString & orc_SectionName,
                                                                 stw::scl::C_SclString & orc_ErrorMessage)
{
   orc_ErrorMessage = "Error: Could not find section \"" + orc_SectionName + "\".";
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Report missing key error

   \param[in]      orc_SectionName     Section name
   \param[in]      orc_KeyName         Key name
   \param[in,out]  orc_ErrorMessage    Error message
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OscCanOpenEdsDeviceInfoBlock::h_ReportMissingKeyError(const stw::scl::C_SclString & orc_SectionName,
                                                             const stw::scl::C_SclString & orc_KeyName,
                                                             stw::scl::C_SclString & orc_ErrorMessage)
{
   orc_ErrorMessage = "Error: Could not find key \"" + orc_KeyName + "\" in section \"" + orc_SectionName + "\".";
}
