//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       KEFEX DEF Project handling

   Implements reading DEF-style KEFEX projects.

   \copyright   Copyright 2009 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.hpp" //pre-compiled headers

#include "DiagLib_config.hpp" //diaglib configuration

#include "stwtypes.hpp"
#include "stwerrors.hpp"
#include "CKFXDEFProject.hpp"
#include "C_OscChecksummedIniFile.hpp"
#include "C_SclString.hpp"
#include "TglFile.hpp"
#include "TglUtils.hpp"
#include "DLLocalize.hpp"

//----------------------------------------------------------------------------------------------------------------------

using namespace stw::errors;
using namespace stw::diag_lib;
using namespace stw::scl;
using namespace stw::tgl;
using namespace stw::opensyde_core;

/* -- Defines ------------------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

//using unions is not nice but more portable than reinterpret_casting
union U_Union64
{
   int64_t s64_Value;
   float64_t f64_Value;
}; //lint !e1960  //cf. comment above

/* -- Global Variables ---------------------------------------------------------------------------------------------- */

/* -- Module Global Variables --------------------------------------------------------------------------------------- */
static const char_t macnc_RAM_FILE_EXTENSION[] = "ram";

/* -- Module Global Function Prototypes ----------------------------------------------------------------------------- */

/* -- Implementation ------------------------------------------------------------------------------------------------ */

C_KFXRAMNameIndex & C_KFXRAMNameIndex::operator =(const C_KFXRAMNameIndex & orc_Source)
{
   if (&orc_Source != this)
   {
      c_FileName  = orc_Source.c_FileName;
      u16_ListIndex = orc_Source.u16_ListIndex;
   }
   return (*this);
}

//----------------------------------------------------------------------------------------------------------------------

C_KFXRAMNameIndex::C_KFXRAMNameIndex()
{
   u16_ListIndex = 0;
}

//----------------------------------------------------------------------------------------------------------------------

C_KFXRAMNameIndex::C_KFXRAMNameIndex(const C_KFXRAMNameIndex & orc_Source)
{
   this->operator =(orc_Source);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief    Load data of one RAM variable list from corresponding .RAM file

   Open .RAM file and load variable descriptions for one list.

   Assumptions:
   - it has already been checked that the device name in the RAM file matches the project
   - file checksum has already been checked

   \param[in]     orc_FilePath       Full path to .ram-File
   \param[out]    orc_List           Variable List
   \param[out]    orc_ErrorText      additional text information on error

   \return
   C_NO_ERR           data loaded, results placed in oc_List   \n
   C_NOACT            no valid list name in file (is it a valid .ram file ?) \n
   C_CONFIG           invalid variable types in .ram file \n
   C_RANGE            invalid access right mode in .ram file \n
   C_RD_WR            file does not exist \n
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_KFXDEFProject::m_LoadRAMListFromFile(const C_SclString & orc_FilePath, C_KFXVariableListBase & orc_List,
                                               C_SclString & orc_ErrorText)
{
   uint16_t u16_NumVars;
   C_OscChecksummedIniFile * pc_IniFile;
   C_SclStringList c_StringList;
   C_KFXVariableBase * pt_Entry;
   C_SclString c_Section;
   C_SclString c_Directive;
   C_SclString c_Temp;

   bool q_Ok;
   int32_t j;
   int32_t k;
   int32_t s32_Index;

   if (TglFileExists(orc_FilePath) == false)
   {
      orc_ErrorText = orc_FilePath + ":\n" + TGL_LoadStr(STR_DS_ERR_DOES_NOT_EXIST);
      return C_RD_WR;
   }

   //read out RAM file
   try
   {
      pc_IniFile = new C_OscChecksummedIniFile(orc_FilePath);
   }
   catch (...)
   {
      tgl_assert(false);
      return C_OVERFLOW;
   }

   orc_List.VariableList.SetLength(0);

   u16_NumVars                  = pc_IniFile->ReadUint16("CONFIG", "NUMOFVARS", 0);
   orc_List.q_CheckSummed       = pc_IniFile->ReadBool("CONFIG", "CHECKSUMMED", false);
   orc_List.u32_CheckSumAddress = pc_IniFile->ReadInteger("CONFIG", "CRCADDRESS", 1024);
   orc_List.c_ListName          = pc_IniFile->ReadString("CONFIG", "LISTNAME", "");
   orc_List.u8_ListType         = pc_IniFile->ReadUint8("CONFIG", "VARIABLETYPE", KFX_VARIABLE_TYPE_RAM);
   if (orc_List.c_ListName == "")
   {
      orc_ErrorText = orc_FilePath + ":\n" + TGL_LoadStr(STR_DS_ERR_NO_VALID_NAME_IN_LIST);
      delete pc_IniFile;
      return C_NOACT;
   }

   orc_List.SetNumDefaults(pc_IniFile->ReadUint16("CONFIG", "NUMDEFAULTS", 1));

   orc_List.VariableList.SetLength(u16_NumVars); //much faster than "++" in a for loop
   for (j = 0; j < u16_NumVars; j++)
   {
      pt_Entry = &orc_List.VariableList[j];
      c_Section = "VARIABLE" + C_SclString::IntToStr(j + 1);

      c_StringList.Clear();
      pc_IniFile->ReadSectionValues(c_Section, &c_StringList);
      pt_Entry->c_Name = c_StringList.Values("NAME");
      c_Temp = c_StringList.Values("ADDRESS");
      pt_Entry->u32_Address = (c_Temp == "") ? 0UL : static_cast<uint32_t>(c_Temp.ToInt64());

      q_Ok = true;
      s32_Index = c_StringList.IndexOfName("TYPE_INDEX");
      if (s32_Index == -1) //TYPE_INDEX exists !! -> type is contained as an index
      {
         q_Ok = false;
      }

      k = KFX_DATA_TYPE_NOVAR;
      if (q_Ok == true)
      {
         try
         {
            k = c_StringList.ValueFromIndex(s32_Index).ToInt();
         }
         catch (...)
         {
            q_Ok = false;
         }
      }

      if (q_Ok == false)
      {
         q_Ok = true;
         //not numeric -> name of type (following very old conventions) !!! -> stay compatible to old files
         c_Temp = c_StringList.Values("TYPE").UpperCase().Trim();
         if (c_Temp == "BYTE")
         {
            k = KFX_DATA_TYPE_UINT8;
         }
         else if (c_Temp == "CHAR")
         {
            k = KFX_DATA_TYPE_SINT8;
         }
         else if (c_Temp == "INT")
         {
            k = KFX_DATA_TYPE_SINT16;
         }
         else if (c_Temp == "WORD")
         {
            k = KFX_DATA_TYPE_UINT16;
         }
         else if (c_Temp == "LONG")
         {
            k = KFX_DATA_TYPE_SINT32;
         }
         else if (c_Temp == "DWORD")
         {
            k = KFX_DATA_TYPE_UINT32;
         }
         else if ((c_Temp == "STRING") || (c_Temp == "ARRAY"))
         {
            //"array" -> compatible to even older files
            k = KFX_DATA_TYPE_ASINT8;
         }
         else if (c_Temp == "AOBYTE")
         {
            k = KFX_DATA_TYPE_AUINT8;
         }
         else if (c_Temp == "FLOAT")
         {
            k = KFX_DATA_TYPE_FLOAT32;
         }
         else
         {
            q_Ok = false;
         }
      }
      if ((k >= KFX_NUM_DATA_TYPES) || (k == KFX_DATA_TYPE_NOVAR))
      {
         q_Ok = false;
      }
      if (q_Ok == false)
      {
         orc_ErrorText = orc_FilePath + ":\n" + c_Temp + " " + TGL_LoadStr(STR_DS_INVALID_TYPE);
         delete pc_IniFile;
         return C_CONFIG;
      }
      pt_Entry->u8_Type = static_cast<uint8_t>(k);

      c_Temp = c_StringList.Values("SIZE");
      if ((c_Temp == "") || (c_Temp == "0"))
      {
         //possibly an old project file ...
         //if it's a numeric data type set size automatically ...
         switch (pt_Entry->u8_Type)
         {
         case KFX_DATA_TYPE_UINT8: //no break
         case KFX_DATA_TYPE_SINT8:
            pt_Entry->SetSize(1);
            break;
         case KFX_DATA_TYPE_SINT16: //no break
         case KFX_DATA_TYPE_UINT16: //no break
         case KFX_DATA_TYPE_CRC:
            pt_Entry->SetSize(2);
            break;
         case KFX_DATA_TYPE_SINT32: //no break
         case KFX_DATA_TYPE_UINT32:
            pt_Entry->SetSize(4);
            break;
         default:
            pt_Entry->SetSize(0);
            break;
         }
      }
      else
      {
         pt_Entry->SetSize(static_cast<uint32_t>(c_Temp.ToInt()));
      }
      c_Temp = c_StringList.Values("LOCATIONRAM").UpperCase();
      if ((c_Temp == "1") || (c_Temp == "TRUE"))
      {
         pt_Entry->q_LocationRAM = true;
      }
      else
      {
         pt_Entry->q_LocationRAM = false;
      }

      m_VarStringsToMinMax(c_StringList.Values("MIN"), c_StringList.Values("MAX"), *pt_Entry);

      for (k = 0; k < KFX_NUM_ACCESS_GROUPS; k++)
      {
         c_Directive = "ACCESS" + C_SclString::IntToStr(k);
         c_Temp = c_StringList.Values(c_Directive).UpperCase();
         if (c_Temp == "RO")
         {
            pt_Entry->au8_Access[k] = KFX_DATA_ACCESS_RO;
         }
         else if (c_Temp == "WO")
         {
            pt_Entry->au8_Access[k] = KFX_DATA_ACCESS_WO;
         }
         else if (c_Temp == "RW")
         {
            pt_Entry->au8_Access[k] = KFX_DATA_ACCESS_RW;
         }
         else if (c_Temp == "INV")
         {
            pt_Entry->au8_Access[k] = KFX_DATA_ACCESS_INVISIBLE;
         }
         else
         {
            orc_ErrorText = orc_FilePath + ":\n" + c_Temp + TGL_LoadStr(STR_DS_INVALID_ACC_TYPE);
            delete pc_IniFile;
            return C_RANGE;
         }
      }
      pt_Entry->c_Unit       = c_StringList.Values("UNIT");
      pt_Entry->SetNumericValue(0LL);
      pt_Entry->SetNumDefaults(orc_List.GetNumDefaults());

      c_Temp = c_StringList.Values("SCALINGFACTOR");
      if (c_Temp == "")
      {
         c_Temp = KFX_FACTOR_VALUE_100_PERCENT;
      }
      try
      {
         pt_Entry->s32_ScalingFactor = c_Temp.ToInt();
      }
      catch (...)
      {
         pt_Entry->s32_ScalingFactor = KFX_FACTOR_VALUE_100_PERCENT;
      }
      c_Temp = c_StringList.Values("SCALINGDIGITS");
      if (c_Temp == "")
      {
         c_Temp = 0;
      }
      try
      {
         pt_Entry->u8_ScalingDigits = static_cast<uint8_t>(c_Temp.ToInt());
      }
      catch (...)
      {
         pt_Entry->u8_ScalingDigits = 0;
      }

      pt_Entry->e_DefTransmissionType = TransTypeStringToEnum(c_StringList.Values("TRANSTYPE").UpperCase());
      try
      {
         pt_Entry->u16_DefInterval = static_cast<uint16_t>(c_StringList.Values("TRANSTIME").ToInt());
      }
      catch (...)
      {
         pt_Entry->u16_DefInterval = 200;
      }
      try
      {
         pt_Entry->u32_DefLowerHysteresis = static_cast<uint32_t>(c_StringList.Values("LOWERHYST").ToInt64());
      }
      catch (...)
      {
         pt_Entry->u32_DefLowerHysteresis = 10;
      }
      try
      {
         pt_Entry->u32_DefUpperHysteresis = static_cast<uint32_t>(c_StringList.Values("UPPERHYST").ToInt64());
      }
      catch (...)
      {
         pt_Entry->u32_DefUpperHysteresis = 10;
      }

      try
      {
         pt_Entry->u8_Class = static_cast<uint8_t>(c_StringList.Values("VAR_CLASS").ToInt());
      }
      catch (...)
      {
         pt_Entry->u8_Class = KFX_DATA_CLASS_SIGNAL;
      }
   }

   delete pc_IniFile;
   return C_NO_ERR;
}

//----------------------------------------------------------------------------------------------------------------------

void C_KFXDEFProject::m_VarStringsToMinMax(const C_SclString & orc_Min, const C_SclString & orc_Max,
                                           C_KFXVariableBase & orc_Variable)
{
   C_SclString c_Min = orc_Min;
   C_SclString c_Max = orc_Max;

   C_SclDynamicArray<C_SclString> c_TokensMin;
   C_SclDynamicArray<C_SclString> c_TokensMax;
   uint32_t u32_Index;
   U_Union64 u_Val64;
   uint8_t u8_Value;

   if (c_Min == "")
   {
      c_Min = "0";
   }
   if (c_Max == "")
   {
      c_Max = "0";
   }
   else if (orc_Variable.IsArrayType() == true)
   {
      //preset with absolute min/max values:
      orc_Variable.SetMinMaxToMaximum();

      //compatibility for projects without min/max for arrays: use maximum range of variable type:
      if ((c_Min != "0") && (c_Max != "0"))
      {
         //parse and set
         c_Min.Tokenize(";", c_TokensMin);
         c_Max.Tokenize(";", c_TokensMax);

         for (u32_Index = 0U; u32_Index < orc_Variable.GetSize(); u32_Index++)
         {
            if (c_Min.Length() > u32_Index)
            {
               try
               {
                  u8_Value = static_cast<uint8_t>(c_TokensMin[u32_Index].ToInt());
                  orc_Variable.GetMinReference()[u32_Index] = u8_Value;
               }
               catch (...)
               {
                  //leave it as it is ...
               }
            }
            if (c_Max.Length() > u32_Index)
            {
               try
               {
                  u8_Value = static_cast<uint8_t>(c_TokensMax[u32_Index].ToInt());
                  orc_Variable.GetMaxReference()[u32_Index] = u8_Value;
               }
               catch (...)
               {
                  //leave it as it is ...
               }
            }
         }
      }
   }
   else
   {
      //integer or float type (no array)
      try
      {
         u_Val64.s64_Value = c_Min.ToInt64();
         //keep compatibility: float min/max values are saved in int64 representation
         if (orc_Variable.IsFloatType() == true)
         {
            orc_Variable.SetFloatMin(u_Val64.f64_Value);
         }
         else
         {
            orc_Variable.SetNumericMin(u_Val64.s64_Value);
         }

         u_Val64.s64_Value = c_Max.ToInt64();
         if (orc_Variable.IsFloatType() == true)
         {
            orc_Variable.SetFloatMax(u_Val64.f64_Value);
         }
         else
         {
            orc_Variable.SetNumericMax(u_Val64.s64_Value);
         }
      }
      catch (...)
      {
         orc_Variable.SetNumericMin(0LL);
         orc_Variable.SetNumericMax(0LL);
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief    Convert transmission type string to enum value

   Convert transmission type textual description to corresponding enumeration.

   \param[in]     orc_Text           Transmission type as text

   \return
   Transmission type enum         if no valid transmission type is detected in oc_Text "eSRR" is returned
*/
//----------------------------------------------------------------------------------------------------------------------
E_TransmissionType C_KFXDEFProject::TransTypeStringToEnum(const C_SclString & orc_Text)
{
   E_TransmissionType e_Return;

   if (orc_Text == "TCRR")
   {
      e_Return = eTCRR;
   }
   else if (orc_Text == "ECRR")
   {
      e_Return = eECRR;
   }
   else if ((orc_Text == "TCRR(TS)") || (orc_Text == "TCRRTS"))
   {
      e_Return = eTCRRTS;
   }
   else
   {
      e_Return = eSRR; //default: SRR
   }

   return e_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief    Append data from .RAM files to variable table

   Clear VariableLists
   Search of all .ram files in oc_Directory belonging to the device oc_DeviceName.
   Read .RAM files and add the contents at the end of "VariableLists".

   \param[in]     orc_Directory    directory to seatch .ram files in
   \param[in]     orc_DeviceName   device name the .ram files must have.
   \param[in,out] orc_Lists        variable lists
   \param[out]    orc_ErrorText    additional text information on error
   \param[out]    orc_Warnings     warning texts (should be displayed by application even when returning C_NO_ERR)

   \return
   C_NO_ERR   -> lists appended           \n
   C_WARN     -> no lists found           \n
   C_CONFIG   -> list consistency error   \n
   C_RD_WR    -> error reading .ram file  \n
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_KFXDEFProject::LoadRAMFiles(const C_SclString & orc_Directory, const C_SclString & orc_DeviceName,
                                      C_KFXVariableLists & orc_Lists, C_SclString & orc_ErrorText,
                                      C_SclDynamicArray<C_SclString> & orc_Warnings)
{
   int32_t s32_Return;
   int32_t s32_Index;

   C_SclDynamicArray<C_KFXRAMNameIndex> c_Files;

   orc_Lists.SetLength(0);
   orc_Warnings.SetLength(0);

   //now search the work directory for all .RAM files which refer to this device !
   m_FindRelatedFiles(orc_Directory, orc_DeviceName, c_Files, orc_Warnings);
   if (c_Files.GetLength() == 0)
   {
      orc_ErrorText = orc_Directory + ":\n" + TGL_LoadStr(STR_DS_ERR_NO_RAM_LISTS_FOUND);
      return C_WARN;
   }

   //Sort by ListIndex
   s32_Return = m_SortRAMLists(c_Files, orc_ErrorText);
   if (s32_Return != C_NO_ERR)
   {
      return C_CONFIG;
   }

   orc_Lists.SetLength(c_Files.GetLength());

   for (s32_Index = 0; s32_Index < c_Files.GetLength(); s32_Index++)
   {
      //Read in the RAM-Files
      s32_Return = m_LoadRAMListFromFile(c_Files[s32_Index].c_FileName, orc_Lists[s32_Index], orc_ErrorText);
      if (s32_Return != C_NO_ERR)
      {
         return C_RD_WR;
      }
   }
   return C_NO_ERR;
}

//**************************************************************.FA*
//   .FUNCTION [ C_KFXDEFProject::m_SortRAMLists ]
//   .GROUP [ C_KFXDEFProject private functions ]
//--------------------------------------------------------------.F1-----------------------------------------------------
//   .DESCRIPTION
//   Sorts the given struct dependent of the "wListIndex".
//   - lowest listIndex at position 0 of the array
//   Also checks for consistency: list indexes must start with "0" and
//    there shall not be any gaps.
//----------------------------------------------------------------------------------------------------------------------
//   .PARAMETERS
//    Name              Comment
//    orc_Files         Reference to the list to change
//    orc_ErrorText     additional text information on error (out)
//----------------------------------------------------------------------------------------------------------------------
//   .RETURNVALUE
//   C_NO_ERR         sorted
//   C_CONFIG         consistency problem
//**************************************************************.FE*
int32_t C_KFXDEFProject::m_SortRAMLists(C_SclDynamicArray<C_KFXRAMNameIndex> & orc_Files, C_SclString & orc_ErrorText)
{
   int32_t s32_File;
   C_KFXRAMNameIndex c_Temp;

   tgl_assert(orc_Files.GetLength() != 0); //assume: check for 0 was already made ...
   if (orc_Files.GetLength() == 1)
   {
      //check for correct index if there's only one list ...
      if (orc_Files[0].u16_ListIndex != 0)
      {
         orc_ErrorText = orc_Files[0].c_FileName + ":\n" + TGL_LoadStr(STR_DS_ERR_PROJECT_INVALID);
         return C_CONFIG;
      }
      return C_NO_ERR;
   }

   //Sort
   for (s32_File = 0; s32_File < orc_Files.GetHigh(); s32_File++)
   {
      if (orc_Files[s32_File + 1].u16_ListIndex < orc_Files[s32_File].u16_ListIndex)
      {
         c_Temp = orc_Files[s32_File];
         orc_Files[s32_File] = orc_Files[s32_File + 1];
         orc_Files[s32_File + 1] = c_Temp;
         //Search again
         s32_File = -1;
      }
   }

   //check consistency
   // list indices must start with 0, have no gaps and no listindex used twice
   for (s32_File = 0; s32_File < orc_Files.GetLength(); s32_File++)
   {
      if (orc_Files[s32_File].u16_ListIndex < static_cast<uint16_t>(s32_File)) //two lists with the same index !
      {
         orc_ErrorText = orc_Files[s32_File].c_FileName + ":\n" + TGL_LoadStr(STR_DS_ERR_PROJECT_INVALID) +
                         "\n Two lists have the same list index:\n" +
                         orc_Files[s32_File - 1].c_FileName +
                         " and\n" + orc_Files[s32_File].c_FileName;
         return C_CONFIG;
      }
      else if (orc_Files[s32_File].u16_ListIndex > static_cast<uint16_t>(s32_File)) //probably missing file ?
      {
         orc_ErrorText = orc_Files[s32_File].c_FileName + ":\n" +
                         TGL_LoadStr(STR_DS_ERR_PROJECT_INVALID) +
                         "\nGap in list indexes (probably missing .ram file)";
         if (s32_File != 0)
         {
            orc_ErrorText += "\nbetween list files\n\"" + orc_Files[s32_File - 1].c_FileName + "\" and\n\"" +
                             orc_Files[s32_File].c_FileName + "\"";
         }
         else
         {
            orc_ErrorText += ".\nFirst list \n" + orc_Files[s32_File].c_FileName + " does not have index 0";
         }
         return C_CONFIG;
      }
      else
      {
         //nothing to do ...
      }
   }
   return C_NO_ERR;
}

//**************************************************************.FA*
//   .FUNCTION [ C_KFXDEFProject::FindRelatedFiles ]
//   .GROUP [ C_KFXDEFProject private functions ]
//--------------------------------------------------------------.F1-----------------------------------------------------
//   .DESCRIPTION
//    Get a list of all .ram files in the work directory which refer to the specified device
//----------------------------------------------------------------------------------------------------------------------
//   .PARAMETERS
//    Name             Comment
//   orc_DeviceName    Device name to check for
//----------------------------------------------------------------------------------------------------------------------
//   .RETURNVALUE
//**************************************************************.FE*
int32_t C_KFXDEFProject::m_FindRelatedFiles(const C_SclString & orc_Directory, const C_SclString & orc_DeviceName,
                                            C_SclDynamicArray<C_KFXRAMNameIndex> & orc_Files,
                                            C_SclDynamicArray<C_SclString> & orc_Warnings)
{
   C_SclString c_Device;
   C_SclString c_Dir;
   C_OscChecksummedIniFile * pc_IniFile;
   bool q_Return;
   int32_t s32_Index;

   orc_Files.SetLength(0);

   C_SclDynamicArray<C_TglFileSearchRecord> c_FoundFiles;

   c_Dir = TglFileIncludeTrailingDelimiter(orc_Directory);
   TglFileFind(c_Dir + "*." + macnc_RAM_FILE_EXTENSION, c_FoundFiles);
   for (s32_Index = 0; s32_Index < c_FoundFiles.GetLength(); s32_Index++)
   {
      pc_IniFile = new C_OscChecksummedIniFile(c_Dir + c_FoundFiles[s32_Index].c_FileName);
      //first check device match, then Checksum (performance reasons)
      c_Device = pc_IniFile->ReadString("CONFIG", "DEVICE", "");
      if (c_Device == orc_DeviceName)
      {
         q_Return = pc_IniFile->CheckCheckSum();
         if (q_Return == true)
         {
            //Collect the available RAM-Path,Name and listindexes
            orc_Files.IncLength();
            orc_Files[orc_Files.GetHigh()].c_FileName  = (c_Dir + c_FoundFiles[s32_Index].c_FileName);
            orc_Files[orc_Files.GetHigh()].u16_ListIndex = pc_IniFile->ReadUint16("CONFIG", "LISTINDEX", 0xFFFFU);
         }
         else
         {
            orc_Warnings.Insert(orc_Warnings.GetHigh(), "File " + c_FoundFiles[s32_Index].c_FileName +
                                " has an incorrect checksum. ");
         }
      }
      delete pc_IniFile;
   }
   return C_NO_ERR;
}

//**************************************************************.FA*
//   .FUNCTION [ C_KFXDEFProject::LoadComments ]
//   .GROUP [ C_KFXDEFProject public functions ]
//--------------------------------------------------------------.F1-----------------------------------------------------
//   .DESCRIPTION
//   Load comments from specified file.
//   Comments for variables that are not part of the project will be ignored.
//----------------------------------------------------------------------------------------------------------------------
//   .PARAMETERS
//    Bezeichner             Bedeutung
//    orc_FileName           Filename of .rec file
//----------------------------------------------------------------------------------------------------------------------
//   .RETURNVALUE
//   C_NO_ERR   -> no errors
//   C_NOACT    -> file does not exist
//   C_OVERFLOW -> file contains more than 5 languages
//   C_CONFIG   -> file does not refer to oc_DeviceName
//   C_RD_WR    -> invalid language name in .rec file
//**************************************************************.FE*
int32_t C_KFXDEFProject::LoadComments(const C_SclString & orc_FileName, const C_SclString & orc_DeviceName,
                                      C_KFXVariableLists & orc_VariableLists,
                                      C_SclString * const opc_CommentDescriptions)
{
   C_SclString c_Device;
   C_SclString c_Directive;
   C_SclString c_Section;
   C_SclString c_Text;
   C_SclString c_ListName;
   C_SclString c_VariableName;
   C_SclIniFile * pc_IniFile;
   C_SclStringList c_List;

   bool q_Found;
   int32_t i;
   uint32_t j;
   int32_t s32_List;
   int32_t k;
   int32_t l;
   uint16_t u16_NumLanguages;

   if (TglFileExists(orc_FileName) == false)
   {
      return C_NOACT;
   }

   //clear all existing comments
   for (s32_List = 0; s32_List < orc_VariableLists.GetLength(); s32_List++)
   {
      for (k = 0; k < orc_VariableLists[s32_List].VariableList.GetLength(); k++)
      {
         for (i = 0; i < KFX_DATA_MAX_NUM_LANGUAGES; i++)
         {
            orc_VariableLists[s32_List].VariableList[k].ac_Comments[i] = "";
         }
      }
   }

   for (i = 0; i < KFX_DATA_MAX_NUM_LANGUAGES; i++)
   {
      opc_CommentDescriptions[i] = "";
   }

   pc_IniFile = new C_SclIniFile(orc_FileName);
   c_Device = pc_IniFile->ReadString("CONFIG", "DEVICE", "");
   if (c_Device != orc_DeviceName)
   {
      delete pc_IniFile;
      return C_CONFIG;
   }

   u16_NumLanguages = pc_IniFile->ReadUint16("CONFIG", "NUMOFLANG", 0);
   if (u16_NumLanguages > KFX_DATA_MAX_NUM_LANGUAGES)
   {
      delete pc_IniFile;
      return C_OVERFLOW;
   }

   for (i = 0; i < u16_NumLanguages; i++)
   {
      c_Directive = "LANGNAME" + C_SclString::IntToStr(i + 1);
      c_Section = pc_IniFile->ReadString("CONFIG", c_Directive, "");
      if ((c_Section == "") || ((pc_IniFile->SectionExists(c_Section) == false)))
      {
         delete pc_IniFile;
         return C_RD_WR;
      }

      opc_CommentDescriptions[i] = c_Section;
      pc_IniFile->ReadSectionValues(c_Section, &c_List);
      for (j = 0; j < c_List.GetCount(); j++)
      {
         c_Text = c_List.Strings[j];
         c_Directive = c_Text.SubString(1, c_Text.Pos("=") - 1).Trim();
         (void)c_Text.Delete(1, c_Text.Pos("="));
         (void)c_Text.Trim();
         c_ListName = c_Directive;
         (void)c_ListName.Delete(c_ListName.Pos("."), c_ListName.Length());
         c_VariableName = c_Directive;
         (void)c_VariableName.Delete(1, c_VariableName.Pos("."));

         //find the list + variable index
         q_Found = false;
         for (k = 0; k < orc_VariableLists.GetLength(); k++)
         {
            if (orc_VariableLists[k].c_ListName.UpperCase() == c_ListName.UpperCase())
            {
               q_Found = true;
               break;
            }
         }
         if (q_Found == true)
         {
            q_Found = false;
            for (l = 0; l < orc_VariableLists[k].VariableList.GetLength(); l++)
            {
               if (orc_VariableLists[k].VariableList[l].c_Name.UpperCase() == c_VariableName.UpperCase())
               {
                  q_Found = true;
                  break;
               }
            }
            if (q_Found == true)
            {
               orc_VariableLists[k].VariableList[l].ac_Comments[i] = c_Text;
            }
         }
      }
   }
   delete pc_IniFile;
   return C_NO_ERR;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief    Load default set names from INI file

   Load names of default sets from opened INI file opc_IniFile.
   The names will be placed in orc_VariableLists.ac_DefaultNames.
   The default names are expected in the section "DEFAULT_SETS".

   \param[in]     opc_IniFile       opened INI file
   \param[out]    orc_VariableLists variable lists instance to place the names into
*/
//----------------------------------------------------------------------------------------------------------------------
void C_KFXDEFProject::LoadDefaultNames(C_OscChecksummedIniFile * const opc_IniFile,
                                       C_KFXVariableLists & orc_VariableLists)
{
   int32_t i;

   orc_VariableLists.ac_DefaultNames.SetLength(opc_IniFile->ReadUint16("DEFAULT_SETS", "DEFAULT_NAMES", 0));
   for (i = 0; i < orc_VariableLists.ac_DefaultNames.GetLength(); i++)
   {
      orc_VariableLists.ac_DefaultNames[i] =
         opc_IniFile->ReadString("DEFAULT_SETS", "NAMEDEFAULT" + C_SclString::IntToStr(i),
                                 "DEFAULT_" + C_SclString::IntToStr(i));
   }
}

//----------------------------------------------------------------------------------------------------------------------
