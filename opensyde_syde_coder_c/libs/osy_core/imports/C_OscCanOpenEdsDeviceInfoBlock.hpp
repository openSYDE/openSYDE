//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Class for device info block handling of EDS/DCF files
   \copyright   Copyright 2022 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_OSCCANOPENEDSDEVICEINFOBLOCK_H
#define C_OSCCANOPENEDSDEVICEINFOBLOCK_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "stwtypes.hpp"
#include "C_SclString.hpp"
#include "C_SclIniFile.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_core
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_OscCanOpenEdsDeviceInfoBlock
{
public:
   C_OscCanOpenEdsDeviceInfoBlock();

   stw::scl::C_SclString c_VendorName;     ///< Vendor name, max 244 characters
   stw::scl::C_SclString c_VendorNumber;   ///< Vendor number, also see object dictionary index 1018, sub-index 1
   stw::scl::C_SclString c_ProductName;    ///< Product name, max 243 characters
   stw::scl::C_SclString c_ProductNumber;  ///< Product number, also see object dictionary index 1018, sub-index 2
   stw::scl::C_SclString c_RevisionNumber; ///< Revision number, also see object dictionary index 1018, sub-index 3
   stw::scl::C_SclString c_OrderCode;      ///< Product order code, max 245 characters
   bool q_BaudRate10;                      ///< Support of baud rate 10 kbit/s
   bool q_BaudRate20;                      ///< Support of baud rate 20 kbit/s
   bool q_BaudRate50;                      ///< Support of baud rate 50 kbit/s
   bool q_BaudRate125;                     ///< Support of baud rate 125 kbit/s
   bool q_BaudRate250;                     ///< Support of baud rate 250 kbit/s
   bool q_BaudRate500;                     ///< Support of baud rate 500 kbit/s
   bool q_BaudRate800;                     ///< Support of baud rate 800 kbit/s
   bool q_BaudRate1000;                    ///< Support of baud rate 1000 kbit/s
   bool q_SimpleBootUpMaster;              ///< Simple boot up master functionality
   bool q_SimpleBootUpSlave;               ///< Simple boot up slave functionality
   uint8_t u8_Granularity;                 ///< Minimum size of a mappable object in bits, allowed for the PDO mapping
                                           // on
   ///< this CANopen device
   stw::scl::C_SclString c_DynamicChannelsSupported; ///< The value indicates the facility of dynamic variable
   ///< generation. If the value is unequal to "0", the additional section DynamicChannels exists
   bool q_GroupMessaging;  ///< The value indicates, if multiplexed PDOs configuration is supported
   uint16_t u16_NrOfRxPdo; ///< Number of supported receive PDOs
   uint16_t u16_NrOfTxPdo; ///< Number of supported transmit PDOs
   bool q_LssSupported;    ///< The value indicates information whether LSS functionality is supported.

   void CalcHash(uint32_t & oru32_HashValue) const;
   int32_t LoadFromIni(stw::scl::C_SclIniFile & orc_File, stw::scl::C_SclString & orc_LastError);

   uint8_t GetGranularity(void) const;

   static int32_t h_LoadStringValueFromIniFile(stw::scl::C_SclIniFile & orc_File,
                                               const stw::scl::C_SclString & orc_SectionName,
                                               const stw::scl::C_SclString & orc_KeyName,
                                               stw::scl::C_SclString & orc_OutputValue,
                                               stw::scl::C_SclString & orc_ErrorMessage,
                                               const stw::scl::C_SclString & orc_DefaultValue = "");
   static int32_t h_LoadU8ValueFromIniFile(stw::scl::C_SclIniFile & orc_File,
                                           const stw::scl::C_SclString & orc_SectionName,
                                           const stw::scl::C_SclString & orc_KeyName, uint8_t & oru8_OutputValue,
                                           stw::scl::C_SclString & orc_ErrorMessage,
                                           const uint8_t ou8_DefaultValue = 0);
   static int32_t h_LoadU16ValueFromIniFile(stw::scl::C_SclIniFile & orc_File,
                                            const stw::scl::C_SclString & orc_SectionName,
                                            const stw::scl::C_SclString & orc_KeyName, uint16_t & oru16_OutputValue,
                                            stw::scl::C_SclString & orc_ErrorMessage,
                                            const uint16_t ou16_DefaultValue = 0);
   static int32_t h_LoadBoolValueFromIniFile(stw::scl::C_SclIniFile & orc_File,
                                             const stw::scl::C_SclString & orc_SectionName,
                                             const stw::scl::C_SclString & orc_KeyName, bool & orq_OutputValue,
                                             stw::scl::C_SclString & orc_ErrorMessage,
                                             const bool oq_DefaultValue = false);
   static void h_ReportMissingSectionError(const stw::scl::C_SclString & orc_SectionName,
                                           stw::scl::C_SclString & orc_ErrorMessage);
   static void h_ReportMissingKeyError(const stw::scl::C_SclString & orc_SectionName,
                                       const stw::scl::C_SclString & orc_KeyName,
                                       stw::scl::C_SclString & orc_ErrorMessage);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
