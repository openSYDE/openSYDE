//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       header for class C_XFLECUInformation

   Handling of device information block.

   \copyright   Copyright 2015 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef CXFLECUINFORMATIONH
#define CXFLECUINFORMATIONH

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "stwtypes.hpp"
#include "C_SclString.hpp"
#include "C_SclStringList.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace diag_lib
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

//contants for device information block
const uint8_t XFL_DEVICE_INFO_MAGIC_LENGTH_V1 = 7U;
const uint8_t XFL_DEVICE_INFO_MAGIC_LENGTH_V2 = 8U;
extern const char_t XFL_DEVICE_INFO_MAGIC_V1[XFL_DEVICE_INFO_MAGIC_LENGTH_V1 + 1U]; /// +1 for \0
extern const char_t XFL_DEVICE_INFO_MAGIC_V2[XFL_DEVICE_INFO_MAGIC_LENGTH_V2];      /// \0 already included

/* -- Types --------------------------------------------------------------------------------------------------------- */

///Device information block
class C_XFLECUInformation
{
private:
   stw::scl::C_SclString m_GetNonTerminatedString(const char_t * opcn_Chars, const uint8_t ou8_MaxLength) const;

   int32_t m_ParsePayload(const uint8_t * const opu8_Data, const uint16_t ou16_NumBytesAvailable,
                          const uint8_t ou8_BlockVersion);

public:
   //sizes of content fields by version (1st element: V1, 2nd element: V2, 3rd element: V3)
   static const uint8_t hau8_LENGTHS_DEVICE_ID[3];
   static const uint8_t hau8_LENGTHS_DATE[3];
   static const uint8_t hau8_LENGTHS_TIME[3];
   static const uint8_t hau8_LENGTHS_PROJECT_NAME[3];
   static const uint8_t hau8_LENGTHS_PROJECT_VERSION[3];

   C_XFLECUInformation(void);

   void ClearContents(void);
   uint16_t GetMaxSizeOnECU(void) const;

   int32_t ParseFromBLOB(const uint8_t * const opu8_Data, const uint16_t ou16_NumBytesAvailable);

   char_t acn_Magic[XFL_DEVICE_INFO_MAGIC_LENGTH_V2]; //Maximum length ...
   uint8_t u8_StructVersion;                          ///< = 1
   uint8_t u8_ContentMap;                             ///< Bit0 = contains device ID  \n
   ///< Bit1 = contains date+time \n
   ///< Bit2 = contains project name \n
   ///< Bit3 = contains project version \n
   ///< Bit4 = contains additional info
   //the lenghts of the char-arrays are set to accomodate the longest possible texts of all defined versions
   char_t acn_DeviceID[29];       ///< ASCII device ID e.g. "ESX2" (optional; recommended)
   char_t acn_Date[12];           ///< date of compilation(format: __DATE__ ANSI C macro) (optional)
   char_t acn_Time[9];            ///< time of compilation (format: __TIME__ ANSI C macro) (optional)
   char_t acn_ProjectName[33];    ///< short description of project (optional)
   char_t acn_ProjectVersion[17]; ///< version number of project; (optional)
   uint8_t u8_LenAdditionalInfo;
   char_t acn_AdditionalInfo[255]; ///< does not hurt on PC side, if we simply use the theoretical max. of 255 bytes

   stw::scl::C_SclString GetDeviceID(void) const;
   stw::scl::C_SclString GetDate(void) const;
   stw::scl::C_SclString GetTime(void) const;
   stw::scl::C_SclString GetProjectName(void) const;
   stw::scl::C_SclString GetProjectVersion(void) const;
   stw::scl::C_SclString GetAdditionalInfo(void) const;

   void AddInfoToList(stw::scl::C_SclStringList & orc_List) const;

   int32_t GetInfoLevel(void) const;
   stw::scl::C_SclString GetInfoLevelAsString(void) const;

   bool ContainsDeviceID(void) const;
   bool ContainsDateAndTime(void) const;
   bool ContainsProjectName(void) const;
   bool ContainsProjectVersion(void) const;
   bool ContainsAdditionalInfo(void) const;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
