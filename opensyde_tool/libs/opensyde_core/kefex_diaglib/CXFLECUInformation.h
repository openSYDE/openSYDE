//-----------------------------------------------------------------------------
/*!
   \file
   \brief       header for class C_XFLECUInformation

   Handling of device information block.

   \implementation
   project     KEFEX
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     21.08.2015  STW/A.Stangl
   \endimplementation
*/
//-----------------------------------------------------------------------------
#ifndef CXFLECUINFORMATIONH
#define CXFLECUINFORMATIONH

/* -- Includes ------------------------------------------------------------- */
#include "stwtypes.h"
#include "CSCLString.h"
#include "CSCLStringList.h"

/* -- Namespace ------------------------------------------------------------ */
namespace stw_diag_lib
{

/* -- Global Constants ----------------------------------------------------- */

//contants for device information block
const stw_types::uint8 XFL_DEVICE_INFO_MAGIC_LENGTH_V1 = 7U;
const stw_types::uint8 XFL_DEVICE_INFO_MAGIC_LENGTH_V2 = 8U;
extern const stw_types::charn XFL_DEVICE_INFO_MAGIC_V1[XFL_DEVICE_INFO_MAGIC_LENGTH_V1 + 1U]; /// +1 for \0
extern const stw_types::charn XFL_DEVICE_INFO_MAGIC_V2[XFL_DEVICE_INFO_MAGIC_LENGTH_V2]; /// \0 already included

/* -- Types ---------------------------------------------------------------- */

///Device information block
class C_XFLECUInformation
{
private:
   stw_scl::C_SCLString m_GetNonTerminatedString(const stw_types::charn * opcn_Chars,
                                                 const stw_types::uint8 ou8_MaxLength) const;

   stw_types::sint32 m_ParsePayload(const stw_types::uint8 * const opu8_Data,
                                    const stw_types::uint16 ou16_NumBytesAvailable,
                                    const stw_types::uint8 ou8_BlockVersion);
public:

   //sizes of content fields by version (1st element: V1, 2nd element: V2, 3rd element: V3)
   static const stw_types::uint8 hau8_LENGTHS_DEVICE_ID[3];
   static const stw_types::uint8 hau8_LENGTHS_DATE[3];
   static const stw_types::uint8 hau8_LENGTHS_TIME[3];
   static const stw_types::uint8 hau8_LENGTHS_PROJECT_NAME[3];
   static const stw_types::uint8 hau8_LENGTHS_PROJECT_VERSION[3];

   C_XFLECUInformation(void);

   void ClearContents(void);
   stw_types::uint16 GetMaxSizeOnECU(void) const;

   stw_types::sint32 ParseFromBLOB(const stw_types::uint8 * const opu8_Data,
                                   const stw_types::uint16 ou16_NumBytesAvailable);

   stw_types::charn acn_Magic[XFL_DEVICE_INFO_MAGIC_LENGTH_V2]; //Maximum length ...
   stw_types::uint8 u8_StructVersion;       ///< = 1
   stw_types::uint8 u8_ContentMap;          ///< Bit0 = contains device ID  \n
                                            ///< Bit1 = contains date+time \n
                                            ///< Bit2 = contains project name \n
                                            ///< Bit3 = contains project version \n
                                            ///< Bit4 = contains additional info
   //the lenghts of the char-arrays are set to accomodate the longest possible texts of all defined versions
   stw_types::charn acn_DeviceID[29];       ///< ASCII device ID e.g. "ESX2" (optional; recommended)
   stw_types::charn acn_Date[12];           ///< date of compilation(format: __DATE__ ANSI C macro) (optional)
   stw_types::charn acn_Time[9];            ///< time of compilation (format: __TIME__ ANSI C macro) (optional)
   stw_types::charn acn_ProjectName[33];    ///< short description of project (optional)
   stw_types::charn acn_ProjectVersion[17]; ///< version number of project; (optional)
   stw_types::uint8 u8_LenAdditionalInfo;
   stw_types::charn acn_AdditionalInfo[255]; ///< does not hurt on PC side, if we simply use the theoretical max. of 255 bytes

   stw_scl::C_SCLString GetDeviceID(void) const;
   stw_scl::C_SCLString GetDate(void) const;
   stw_scl::C_SCLString GetTime(void) const;
   stw_scl::C_SCLString GetProjectName(void) const;
   stw_scl::C_SCLString GetProjectVersion(void) const;
   stw_scl::C_SCLString GetAdditionalInfo(void) const;

   void AddInfoToList(stw_scl::C_SCLStringList & orc_List) const;

   stw_types::sint32 GetInfoLevel(void) const;
   stw_scl::C_SCLString GetInfoLevelAsString(void) const;

   bool ContainsDeviceID(void) const;
   bool ContainsDateAndTime(void) const;
   bool ContainsProjectName(void) const;
   bool ContainsProjectVersion(void) const;
   bool ContainsAdditionalInfo(void) const;
};

/* -- Extern Global Variables ---------------------------------------------- */

} //end of namespace

#endif
