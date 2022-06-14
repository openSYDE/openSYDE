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
#include "stwtypes.h"
#include "CSCLString.h"
#include "CSCLIniFile.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw_opensyde_core
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_OSCCanOpenEdsDeviceInfoBlock
{
public:
   C_OSCCanOpenEdsDeviceInfoBlock();

   stw_scl::C_SCLString c_VendorName;     ///< Vendor name, max 244 characters
   stw_scl::C_SCLString c_VendorNumber;   ///< Vendor number, also see object dictionary index 1018, sub-index 1
   stw_scl::C_SCLString c_ProductName;    ///< Product name, max 243 characters
   stw_scl::C_SCLString c_ProductNumber;  ///< Product number, also see object dictionary index 1018, sub-index 2
   stw_scl::C_SCLString c_RevisionNumber; ///< Revision number, also see object dictionary index 1018, sub-index 3
   stw_scl::C_SCLString c_OrderCode;      ///< Product order code, max 245 characters
   bool q_BaudRate10;                     ///< Support of baud rate 10 kbit/s
   bool q_BaudRate20;                     ///< Support of baud rate 20 kbit/s
   bool q_BaudRate50;                     ///< Support of baud rate 50 kbit/s
   bool q_BaudRate125;                    ///< Support of baud rate 125 kbit/s
   bool q_BaudRate250;                    ///< Support of baud rate 250 kbit/s
   bool q_BaudRate500;                    ///< Support of baud rate 500 kbit/s
   bool q_BaudRate800;                    ///< Support of baud rate 800 kbit/s
   bool q_BaudRate1000;                   ///< Support of baud rate 1000 kbit/s
   bool q_SimpleBootUpMaster;             ///< Simple boot up master functionality
   bool q_SimpleBootUpSlave;              ///< Simple boot up slave functionality
   stw_types::uint8 u8_Granularity;       ///< Minimum size of a mappable object in bits, allowed for the PDO mapping on
   ///< this CANopen device
   stw_scl::C_SCLString c_DynamicChannelsSupported; ///< The value indicates the facility of dynamic variable
   ///< generation. If the value is unequal to "0", the additional section DynamicChannels exists
   bool q_GroupMessaging;           ///< The value indicates, if multiplexed PDOs configuration is supported
   stw_types::uint16 u16_NrOfRxPDO; ///< Number of supported receive PDOs
   stw_types::uint16 u16_NrOfTxPDO; ///< Number of supported transmit PDOs
   bool q_LSSSupported;             ///< The value indicates information whether LSS functionality is supported.

   void CalcHash(stw_types::uint32 & oru32_HashValue) const;
   stw_types::sint32 LoadFromIni(stw_scl::C_SCLIniFile & orc_File, stw_scl::C_SCLString & orc_LastError);

   stw_types::uint8 GetGranularity(void) const;

   static stw_types::sint32 h_LoadStringValueFromIniFile(stw_scl::C_SCLIniFile & orc_File,
                                                         const stw_scl::C_SCLString & orc_SectionName,
                                                         const stw_scl::C_SCLString & orc_KeyName,
                                                         stw_scl::C_SCLString & orc_OutputValue,
                                                         stw_scl::C_SCLString & orc_ErrorMessage,
                                                         const stw_scl::C_SCLString & orc_DefaultValue = "");
   static stw_types::sint32 h_LoadU8ValueFromIniFile(stw_scl::C_SCLIniFile & orc_File,
                                                     const stw_scl::C_SCLString & orc_SectionName,
                                                     const stw_scl::C_SCLString & orc_KeyName,
                                                     stw_types::uint8 & oru8_OutputValue,
                                                     stw_scl::C_SCLString & orc_ErrorMessage,
                                                     const stw_types::uint8 ou8_DefaultValue = 0);
   static stw_types::sint32 h_LoadU16ValueFromIniFile(stw_scl::C_SCLIniFile & orc_File,
                                                      const stw_scl::C_SCLString & orc_SectionName,
                                                      const stw_scl::C_SCLString & orc_KeyName,
                                                      stw_types::uint16 & oru16_OutputValue,
                                                      stw_scl::C_SCLString & orc_ErrorMessage,
                                                      const stw_types::uint16 ou16_DefaultValue = 0);
   static stw_types::sint32 h_LoadBoolValueFromIniFile(stw_scl::C_SCLIniFile & orc_File,
                                                       const stw_scl::C_SCLString & orc_SectionName,
                                                       const stw_scl::C_SCLString & orc_KeyName, bool & orq_OutputValue,
                                                       stw_scl::C_SCLString & orc_ErrorMessage,
                                                       const bool oq_DefaultValue = false);
   static void h_ReportMissingSectionError(const stw_scl::C_SCLString & orc_SectionName,
                                           stw_scl::C_SCLString & orc_ErrorMessage);
   static void h_ReportMissingKeyError(const stw_scl::C_SCLString & orc_SectionName,
                                       const stw_scl::C_SCLString & orc_KeyName,
                                       stw_scl::C_SCLString & orc_ErrorMessage);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif
