#include "precomp_headers.hpp" //pre-compiled headers

#include "DiagLib_config.hpp" //diaglib configuration

#include "stwtypes.hpp"
#include "stwerrors.hpp"
#include "CKFXProjectOptions.hpp"
#include "C_SclIniFile.hpp"

//--------------------------------------------------------------

using namespace stw::errors;
using namespace stw::scl;
using namespace stw::diag_lib;

//--------------------------------------------------------------

C_KFXProjectMetaInfo::C_KFXProjectMetaInfo(void)
{
}

//--------------------------------------------------------------

C_KFXProjectMetaInfo::~C_KFXProjectMetaInfo(void)
{
}

//--------------------------------------------------------------

C_KFXProjectMetaInfo::C_KFXProjectMetaInfo(const C_KFXProjectMetaInfo & orc_Source)
{
   (*this) = orc_Source;
}

//--------------------------------------------------------------

C_KFXProjectMetaInfo & C_KFXProjectMetaInfo::operator =(const C_KFXProjectMetaInfo & orc_Source)
{
   if (&orc_Source != this)
   {
      this->c_Text = orc_Source.c_Text;
   }
   return (*this);
}

//--------------------------------------------------------------

//load (usually from .def file)
void C_KFXProjectMetaInfo::LoadFromINI(C_SclIniFile & orc_IniFile, const C_SclString & orc_Section)
{
   uint32_t u32_NumOfLines;
   uint32_t u32_Index;

   u32_NumOfLines = orc_IniFile.ReadInteger(orc_Section, "META_NUM_LINES", 0);

   for (u32_Index = 0U; u32_Index < u32_NumOfLines; u32_Index++)
   {
      (void)this->c_Text.Add(orc_IniFile.ReadString(orc_Section, "META_LINE_" + C_SclString::IntToStr(u32_Index), ""));
   }
}

//--------------------------------------------------------------

bool C_KFXProjectMetaInfo::operator !=(C_KFXProjectMetaInfo & orc_Source)
{
   return (this->c_Text.GetText() != orc_Source.c_Text.GetText());
}

//--------------------------------------------------------------

C_KFXProjectOptions::C_KFXProjectOptions()
{
   uint8_t u8_Index;

   u16_MaxDesignatorLength = 200U;
   c_DeviceName     = "";
   u16_ProjectIndex = 0U;
   u16_ListOffset   = 0U;
   u16_DataVersion  = 0U;
   for (u8_Index = 0U; u8_Index < KFX_DATA_MAX_NUM_LANGUAGES; u8_Index++)
   {
      ac_CommentDescriptions[u8_Index] = "";
   }
   for (u8_Index = 0U; u8_Index < KFX_NUM_ACCESS_GROUPS; u8_Index++)
   {
      aq_UserMayWriteKDX[u8_Index] = false;
   }
}

//--------------------------------------------------------------

bool C_KFXProjectOptions::operator !=(C_KFXProjectOptions & orc_Source)
{
   bool q_Diff;
   uint32_t u32_Index;

   q_Diff =            (orc_Source.u16_MaxDesignatorLength != u16_MaxDesignatorLength);
   q_Diff = q_Diff || (orc_Source.u16_ProjectIndex        != u16_ProjectIndex);
   q_Diff = q_Diff || (orc_Source.u16_ListOffset          != u16_ListOffset);
   q_Diff = q_Diff || (orc_Source.u16_DataVersion         != u16_DataVersion);
   q_Diff = q_Diff || (orc_Source.c_DeviceName            != c_DeviceName);
   for (u32_Index = 0U; u32_Index < KFX_DATA_MAX_NUM_LANGUAGES; u32_Index++)
   {
      q_Diff = q_Diff || (orc_Source.ac_CommentDescriptions[u32_Index] != ac_CommentDescriptions[u32_Index]);
   }
   for (u32_Index = 0U; u32_Index < KFX_NUM_ACCESS_GROUPS; u32_Index++)
   {
      q_Diff = q_Diff || (orc_Source.aq_UserMayWriteKDX[u32_Index] != aq_UserMayWriteKDX[u32_Index]);
   }
   q_Diff = q_Diff || (orc_Source.c_MetaInfo.operator !=(c_MetaInfo));
   return q_Diff;
}

//--------------------------------------------------------------

void C_KFXProjectOptions::LoadConfigFromIni(C_SclIniFile & orc_IniFile)
{
   uint8_t u8_Index;

   u16_MaxDesignatorLength  = orc_IniFile.ReadUint16("EDIT",   "DESIGNATOR_MAX_LENGTH", 200U);
   c_DeviceName             = orc_IniFile.ReadString("CONFIG", "DEVICENAME",            "");
   u16_DataVersion          = orc_IniFile.ReadUint16("CONFIG", "RAMDATAVERSION",        0U);
   u16_ListOffset           = orc_IniFile.ReadUint16("CONFIG", "LISTOFFSET",            0U);
   u16_ProjectIndex         = orc_IniFile.ReadUint16("CONFIG", "PROJECTINDEX",          0U);
   for (u8_Index = 0U; u8_Index < KFX_NUM_ACCESS_GROUPS; u8_Index++)
   {
      aq_UserMayWriteKDX[u8_Index]  = orc_IniFile.ReadBool("CONFIG",
                                                           "KDX_WRITE_PERMITTED" + C_SclString::IntToStr(u8_Index),
                                                           false);
   }
   c_MetaInfo.LoadFromINI(orc_IniFile, "META_DATA");
   //ac_CommentDescriptions are not handled by this class
}

//--------------------------------------------------------------
