//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       ECU information block handling

   Provides functions to handle ECU information blocks:
   - deserialize
   - extract information

   \copyright   Copyright 2015 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.h" //pre-compiled headers
#ifdef __BORLANDC__          //putting the pragmas in the config-header will not work
#pragma hdrstop
#pragma package(smart_init)
#endif

#include <cstring>
#include "stwtypes.h"
#include "stwerrors.h"
#include "CXFLECUInformation.h"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw_types;
using namespace stw_errors;
using namespace stw_scl;
using namespace stw_diag_lib;

//magic for device info block in hex file
const charn stw_diag_lib::XFL_DEVICE_INFO_MAGIC_V1[XFL_DEVICE_INFO_MAGIC_LENGTH_V1 + 1U] = "Lx_?z2.";
const charn stw_diag_lib::XFL_DEVICE_INFO_MAGIC_V2[XFL_DEVICE_INFO_MAGIC_LENGTH_V2] = "Lx_?zg."; /// \0 already included

const stw_types::uint8 C_XFLECUInformation::hau8_LENGTHS_DEVICE_ID[3]       =
{
   16U, 17U, 29U
};
const stw_types::uint8 C_XFLECUInformation::hau8_LENGTHS_DATE[3]            =
{
   11U, 12U, 12U
};
const stw_types::uint8 C_XFLECUInformation::hau8_LENGTHS_TIME[3]            =
{
   8U,  9U,  9U
};
const stw_types::uint8 C_XFLECUInformation::hau8_LENGTHS_PROJECT_NAME[3]    =
{
   15U, 16U, 33U
};
const stw_types::uint8 C_XFLECUInformation::hau8_LENGTHS_PROJECT_VERSION[3] =
{
   10U, 11U, 17U
};

/* -- Module Global Constants --------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

/* -- Global Variables ---------------------------------------------------------------------------------------------- */

/* -- Module Global Variables --------------------------------------------------------------------------------------- */

/* -- Module Global Function Prototypes ----------------------------------------------------------------------------- */

/* -- Implementation ------------------------------------------------------------------------------------------------ */

C_XFLECUInformation::C_XFLECUInformation(void) :
   u8_StructVersion(0U),
   u8_ContentMap(0U),
   u8_LenAdditionalInfo(0U)
{
   this->ClearContents();
}

//----------------------------------------------------------------------------------------------------------------------

void C_XFLECUInformation::ClearContents(void)
{
   (void)std::memset(&acn_Magic[0], 0, sizeof(acn_Magic));
   (void)std::memset(&acn_DeviceID[0], 0, sizeof(acn_DeviceID));
   (void)std::memset(&acn_Date[0], 0, sizeof(acn_Date));
   (void)std::memset(&acn_Time[0], 0, sizeof(acn_Time));
   (void)std::memset(&acn_ProjectName[0], 0, sizeof(acn_ProjectName));
   (void)std::memset(&acn_ProjectVersion[0], 0, sizeof(acn_ProjectVersion));
   (void)std::memset(&acn_AdditionalInfo[0], 0, sizeof(acn_AdditionalInfo));
}

//----------------------------------------------------------------------------------------------------------------------

uint16 C_XFLECUInformation::GetMaxSizeOnECU(void) const
{
   //the data has to be "packed" on ECU side
   uint16 u16_Size = 0U;

   u16_Size = static_cast<uint16>(u16_Size + sizeof(acn_Magic));
   u16_Size = static_cast<uint16>(u16_Size + sizeof(u8_StructVersion));
   u16_Size = static_cast<uint16>(u16_Size + sizeof(u8_ContentMap));
   u16_Size = static_cast<uint16>(u16_Size + sizeof(acn_DeviceID));
   u16_Size = static_cast<uint16>(u16_Size + sizeof(acn_Date));
   u16_Size = static_cast<uint16>(u16_Size + sizeof(acn_Time));
   u16_Size = static_cast<uint16>(u16_Size + sizeof(acn_ProjectName));
   u16_Size = static_cast<uint16>(u16_Size + sizeof(acn_ProjectVersion));
   u16_Size = static_cast<uint16>(u16_Size + sizeof(u8_LenAdditionalInfo));
   u16_Size = static_cast<uint16>(u16_Size + sizeof(acn_AdditionalInfo));
   return u16_Size;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Parse ECU information payload data from BLOB

   Tries to parse the payload of an ECU information block.
   Supports V1, V2, V3 of the block definition.

   \param[in]     opu8_Data                 data to parse, starting with the payload
                                              (first byte after version resp. content map byte)
   \param[in]     ou16_NumBytesAvailable    number of bytes available in opu8_Data

   \return
   C_NO_ERR           no errors, information stored in class members
   else               error parsing
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_XFLECUInformation::m_ParsePayload(const uint8 * const opu8_Data, const uint16 ou16_NumBytesAvailable,
                                           const uint8 ou8_BlockVersion)
{
   uint16 u16_Index = 0U;
   uint16 u16_Length;
   sint32 s32_Return = C_NO_ERR;
   uint8 u8_VersionIndex = 0U;

   switch (ou8_BlockVersion)
   {
   case 1U:
      u8_VersionIndex = 0U;
      break;
   case 2U:
      u8_VersionIndex = 1U;
      break;
   case 3U:
      u8_VersionIndex = 2U;
      break;
   default:
      s32_Return = C_RANGE;
      break;
   }

   if ((s32_Return == C_NO_ERR) && (this->ContainsDeviceID() == true))
   {
      u16_Length = hau8_LENGTHS_DEVICE_ID[u8_VersionIndex];
      if ((ou16_NumBytesAvailable - u16_Index)  <= u16_Length)
      {
         s32_Return = C_OVERFLOW;
      }
      else
      {
         this->acn_DeviceID[sizeof(acn_DeviceID) - 1] = '\0';
         (void)memcpy(&this->acn_DeviceID[0], &opu8_Data[u16_Index], u16_Length);
         u16_Index = static_cast<uint16>(u16_Index + u16_Length);
      }
   }

   if ((s32_Return == C_NO_ERR) && (this->ContainsDateAndTime() == true))
   {
      //extract date:
      u16_Length = hau8_LENGTHS_DATE[u8_VersionIndex];
      if ((ou16_NumBytesAvailable - u16_Index)  <= u16_Length)
      {
         s32_Return = C_OVERFLOW;
      }
      else
      {
         this->acn_Date[sizeof(acn_Date) - 1] = '\0';
         (void)memcpy(&this->acn_Date[0], &opu8_Data[u16_Index], u16_Length);
         u16_Index = static_cast<uint16>(u16_Index + u16_Length);
      }

      //extract time:
      u16_Length = hau8_LENGTHS_TIME[u8_VersionIndex];
      if ((ou16_NumBytesAvailable - u16_Index)  <= u16_Length)
      {
         s32_Return = C_OVERFLOW;
      }
      else
      {
         this->acn_Time[sizeof(acn_Time) - 1] = '\0';
         (void)memcpy(&this->acn_Time[0], &opu8_Data[u16_Index], u16_Length);
         u16_Index = static_cast<uint16>(u16_Index + u16_Length);
      }
   }

   if ((s32_Return == C_NO_ERR) && (this->ContainsProjectName() == true))
   {
      u16_Length = hau8_LENGTHS_PROJECT_NAME[u8_VersionIndex];
      if ((ou16_NumBytesAvailable - u16_Index)  <= u16_Length)
      {
         s32_Return = C_OVERFLOW;
      }
      else
      {
         this->acn_ProjectName[sizeof(acn_ProjectName) - 1] = '\0';
         (void)memcpy(&this->acn_ProjectName[0], &opu8_Data[u16_Index], u16_Length);
         u16_Index = static_cast<uint16>(u16_Index + u16_Length);
      }
   }

   if ((s32_Return == C_NO_ERR) && (this->ContainsProjectName() == true))
   {
      if (this->ContainsProjectVersion() == true)
      {
         u16_Length = hau8_LENGTHS_PROJECT_VERSION[u8_VersionIndex];
         if ((ou16_NumBytesAvailable - u16_Index)  <= u16_Length)
         {
            s32_Return = C_OVERFLOW;
         }
         else
         {
            this->acn_ProjectVersion[sizeof(acn_ProjectVersion) - 1] = '\0';
            (void)memcpy(&this->acn_ProjectVersion[0], &opu8_Data[u16_Index], u16_Length);
            u16_Index = static_cast<uint16>(u16_Index + u16_Length);
         }
      }
   }

   if ((s32_Return == C_NO_ERR) && (this->ContainsAdditionalInfo() == true))
   {
      u16_Length = 1;
      if ((ou16_NumBytesAvailable - u16_Index) <= u16_Length)
      {
         s32_Return = C_OVERFLOW;
      }
      else
      {
         this->u8_LenAdditionalInfo = opu8_Data[u16_Index];
         u16_Index = static_cast<uint16>(u16_Index + u16_Length);
      }

      u16_Length = this->u8_LenAdditionalInfo;
      if ((ou16_NumBytesAvailable - u16_Index)  < u16_Length)
      {
         s32_Return = C_OVERFLOW;
      }
      else
      {
         (void)memcpy(&this->acn_AdditionalInfo[0], &opu8_Data[u16_Index], this->u8_LenAdditionalInfo);
         //            u16_Index = static_cast<uint16>(u16_Index + u16_Length);
      }
   }
   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Parse ECU information data from BLOB

   Tries to parse an ECU information block.
   Supports V1, V2, V3 of the block definition.

   \param[in]     opu8_Data                 data to parse, starting with magic pattern
   \param[in]     ou16_NumBytesAvailable    number of bytes available in opu8_Data (must be >= than the area
                                             occupied by the information block itseld)

   \return
   C_NO_ERR           no errors, information stored in class members
   else               error parsing
*/
//----------------------------------------------------------------------------------------------------------------------
stw_types::sint32 C_XFLECUInformation::ParseFromBLOB(const uint8 * const opu8_Data, const uint16 ou16_NumBytesAvailable)
{
   C_XFLECUInformation c_Temp;
   sint32 s32_Return = C_NO_ERR;
   bool q_V2OrV3Detected = false;
   uint16 u16_Length;
   uint16 u16_Index = 0U;

   //We need at least the magic + version + content map (or at least a few bytes of valid data for V3)
   if (ou16_NumBytesAvailable < (XFL_DEVICE_INFO_MAGIC_LENGTH_V1 + 2))
   {
      s32_Return = C_OVERFLOW;
   }

   if (s32_Return == C_NO_ERR)
   {
      //don't compare block type indicator ...
      s32_Return = memcmp(&opu8_Data[0], XFL_DEVICE_INFO_MAGIC_V1, XFL_DEVICE_INFO_MAGIC_LENGTH_V1 - 1);
      if (s32_Return != 0)
      {
         //try V2/V3:
         s32_Return = memcmp(&opu8_Data[0], XFL_DEVICE_INFO_MAGIC_V2, XFL_DEVICE_INFO_MAGIC_LENGTH_V2 - 2);
         if (s32_Return == 0)
         {
            //final byte zero ?
            if (opu8_Data[XFL_DEVICE_INFO_MAGIC_LENGTH_V2 - 1] != 0U)
            {
               s32_Return = -1;
            }
            else
            {
               q_V2OrV3Detected = true;
            }
         }
      }
      if (s32_Return != 0)
      {
         s32_Return = C_CONFIG;
      }
      else
      {
         s32_Return = C_NO_ERR;

         c_Temp.acn_Magic[XFL_DEVICE_INFO_MAGIC_LENGTH_V1] = '\0';
         u16_Length = (static_cast<uint16>(XFL_DEVICE_INFO_MAGIC_LENGTH_V1) + ((q_V2OrV3Detected == true) ? 1 : 0));
         memcpy(&c_Temp.acn_Magic[0], &opu8_Data[0], u16_Length);
         u16_Index = u16_Length;
      }
   }

   if (s32_Return == C_NO_ERR)
   {
      c_Temp.u8_StructVersion = opu8_Data[u16_Index];
      if ((c_Temp.u8_StructVersion != 1U) && (c_Temp.u8_StructVersion != 2U) && (c_Temp.u8_StructVersion != 3U))
      {
         s32_Return = C_CONFIG; //nothing we can handle ...
      }

      u16_Index += 1U; //skip version

      switch (c_Temp.u8_StructVersion)
      {
      case 1: //no break
      case 2:
         c_Temp.u8_ContentMap = opu8_Data[u16_Index];
         u16_Index += 1U; //skip content map
         break;
      case 3:
         c_Temp.u8_ContentMap = 0x1FU; //all data mandatory here
         break;
      default:
         s32_Return = C_CONFIG; //nothing we can handle ...
         break;
      }
   }

   if (s32_Return == C_NO_ERR)
   {
      s32_Return = c_Temp.m_ParsePayload(&opu8_Data[u16_Index], ou16_NumBytesAvailable - u16_Index,
                                         c_Temp.u8_StructVersion);
   }

   if (s32_Return == C_NO_ERR)
   {
      (*this) = c_Temp;
   }
   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------

C_SCLString C_XFLECUInformation::m_GetNonTerminatedString(const charn * const opcn_Chars,
                                                          const uint8 ou8_MaxLength) const
{
   C_SCLString c_Help;
   charn * pcn_Text;

   pcn_Text = new charn[static_cast<uintn>(ou8_MaxLength) + 1];
   pcn_Text[ou8_MaxLength] = '\0';
   (void)memcpy(pcn_Text, opcn_Chars, ou8_MaxLength);
   c_Help = pcn_Text;
   delete[] pcn_Text;
   c_Help = c_Help.TrimRight();
   return c_Help;
}

//----------------------------------------------------------------------------------------------------------------------

void C_XFLECUInformation::AddInfoToList(C_SCLStringList & orc_List) const
{
   (void)orc_List.Add("Block type:      " + GetInfoLevelAsString());
   (void)orc_List.Add("Version:         " + C_SCLString::IntToStr(u8_StructVersion));

   if (ContainsDeviceID() == true)
   {
      (void)orc_List.Add("DeviceID:        " + GetDeviceID());
   }
   else
   {
      (void)orc_List.Add("DeviceID:        undefined");
   }
   if (ContainsDateAndTime() == true)
   {
      (void)orc_List.Add("Date:            " + GetDate());
      (void)orc_List.Add("Time:            " + GetTime());
   }
   else
   {
      (void)orc_List.Add("Date and Time:   undefined");
   }

   if (ContainsProjectName() == true)
   {
      (void)orc_List.Add("Project name:    " + GetProjectName());
   }
   else
   {
      (void)orc_List.Add("Project name:    undefined");
   }

   if (ContainsProjectVersion() == true)
   {
      (void)orc_List.Add("Project version: " + GetProjectVersion());
   }
   else
   {
      (void)orc_List.Add("Project version: undefined");
   }

   if (ContainsAdditionalInfo() == true)
   {
      (void)orc_List.Add("Additional info: " + GetAdditionalInfo());
   }
   else
   {
      (void)orc_List.Add("Additional info: undefined");
   }
}

//----------------------------------------------------------------------------------------------------------------------

C_SCLString C_XFLECUInformation::GetInfoLevelAsString(void) const
{
   sint32 s32_Return;
   C_SCLString c_Text;

   s32_Return = GetInfoLevel();
   switch (s32_Return)
   {
   case 0:
      c_Text = "Application";
      break;
   case 1:
      c_Text = "BIOS";
      break;
   case 2:
      c_Text = "Driver Library";
      break;
   case 3:
      c_Text = "Level3";
      break;
   case 4:
      c_Text = "Level4";
      break;
   case 5:
      c_Text = "Level5";
      break;
   case 6:
      c_Text = "Level6";
      break;
   case 7:
      c_Text = "Level7";
      break;
   case 8:
      c_Text = "Level8";
      break;
   case 9:
      c_Text = "Level9";
      break;
   default:
      c_Text = "Undefined";
      break;
   }
   return c_Text;
}

//----------------------------------------------------------------------------------------------------------------------

sint32 C_XFLECUInformation::GetInfoLevel(void) const
{
   //last character decides:
   sint32 s32_Return;

   switch (acn_Magic[XFL_DEVICE_INFO_MAGIC_LENGTH_V1 - 1U])
   {
   case '.':
      s32_Return = 0;
      break;
   case '1':
      s32_Return = 1;
      break;
   case '2':
      s32_Return = 2;
      break;
   case '3':
      s32_Return = 3;
      break;
   case '4':
      s32_Return = 4;
      break;
   case '5':
      s32_Return = 5;
      break;
   case '6':
      s32_Return = 6;
      break;
   case '7':
      s32_Return = 7;
      break;
   case '8':
      s32_Return = 8;
      break;
   case '9':
      s32_Return = 9;
      break;
   default:
      s32_Return = C_RANGE;
      break;
   }
   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------

bool C_XFLECUInformation::ContainsDeviceID(void) const
{
   return ((u8_ContentMap & 0x01U) == 0x01U) ? true : false;
}

//----------------------------------------------------------------------------------------------------------------------

bool C_XFLECUInformation::ContainsDateAndTime(void) const
{
   return ((u8_ContentMap & 0x02U) == 0x02U) ? true : false;
}

//----------------------------------------------------------------------------------------------------------------------

bool C_XFLECUInformation::ContainsProjectName(void) const
{
   return ((u8_ContentMap & 0x04U) == 0x04U) ? true : false;
}

//----------------------------------------------------------------------------------------------------------------------

bool C_XFLECUInformation::ContainsProjectVersion(void) const
{
   return ((u8_ContentMap & 0x08U) == 0x08U) ? true : false;
}

//----------------------------------------------------------------------------------------------------------------------

bool C_XFLECUInformation::ContainsAdditionalInfo(void) const
{
   return ((u8_ContentMap & 0x10U) == 0x10U) ? true : false;
}

//----------------------------------------------------------------------------------------------------------------------

C_SCLString C_XFLECUInformation::GetDeviceID(void) const
{
   return m_GetNonTerminatedString(&acn_DeviceID[0], static_cast<uint8>(sizeof(acn_DeviceID)));
}

//----------------------------------------------------------------------------------------------------------------------

C_SCLString C_XFLECUInformation::GetDate(void) const
{
   return m_GetNonTerminatedString(&acn_Date[0], static_cast<uint8>(sizeof(acn_Date)));
}

//----------------------------------------------------------------------------------------------------------------------

C_SCLString C_XFLECUInformation::GetTime(void) const
{
   return m_GetNonTerminatedString(&acn_Time[0], static_cast<uint8>(sizeof(acn_Time)));
}

//----------------------------------------------------------------------------------------------------------------------

C_SCLString C_XFLECUInformation::GetProjectName(void) const
{
   return m_GetNonTerminatedString(&acn_ProjectName[0], static_cast<uint8>(sizeof(acn_ProjectName)));
}

//----------------------------------------------------------------------------------------------------------------------

C_SCLString C_XFLECUInformation::GetProjectVersion(void) const
{
   return m_GetNonTerminatedString(&acn_ProjectVersion[0], static_cast<uint8>(sizeof(acn_ProjectVersion)));
}

//----------------------------------------------------------------------------------------------------------------------

C_SCLString C_XFLECUInformation::GetAdditionalInfo(void) const
{
   return m_GetNonTerminatedString(&acn_AdditionalInfo[0], u8_LenAdditionalInfo);
}

//----------------------------------------------------------------------------------------------------------------------
