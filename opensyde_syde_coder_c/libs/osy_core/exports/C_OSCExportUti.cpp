//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Utility class for C code export.

   Handles generic code structures of export classes for Datapool, COMM and HALC export.

   \copyright   Copyright 2019 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.h"

#include "stwtypes.h"
#include "stwerrors.h"
#include "TGLFile.h"
#include "C_OSCExportUti.h"

#include "C_OSCLoggingHandler.h"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw_types;
using namespace stw_errors;
using namespace stw_scl;
using namespace stw_tgl;
using namespace stw_opensyde_core;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

/* -- Global Variables ---------------------------------------------------------------------------------------------- */

/* -- Module Global Variables --------------------------------------------------------------------------------------- */

/* -- Module Global Function Prototypes ----------------------------------------------------------------------------- */

/* -- Implementation ------------------------------------------------------------------------------------------------ */

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Create 120 characters long section separator.

   \param[in]  orc_SectionName   Name of the following code section (e.g. Includes, Global Variables, ...)

   \return
   Section separator as string
*/
//----------------------------------------------------------------------------------------------------------------------
C_SCLString C_OSCExportUti::h_GetSectionSeparator(const C_SCLString & orc_SectionName)
{
   C_SCLString c_Return;

   c_Return = "/* -- ";
   c_Return += orc_SectionName;
   c_Return += " ";

   for (uint32 u32_UpfillCounter = 117 - c_Return.Length(); u32_UpfillCounter > 0; u32_UpfillCounter--)
   {
      c_Return += "-";
   }

   c_Return += " */"; // last 3 characters to fill up 120

   return c_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get 120 characters long class header separator

   \return
   Class header separator
*/
//----------------------------------------------------------------------------------------------------------------------
C_SCLString C_OSCExportUti::h_GetHeaderSeparator(void)
{
   const C_SCLString c_Return =
      "//--------------------------------------------------------------------------------------------------------------"
      "--------";

   return c_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Create string with information that the file was generated.

   \param[in]  orc_ExportToolInfo   Export tool info

   \return
   generation information string
*/
//----------------------------------------------------------------------------------------------------------------------
C_SCLString C_OSCExportUti::h_GetCreationToolInfo(const C_SCLString & orc_ExportToolInfo)
{
   return "   This file was generated by openSYDE " + orc_ExportToolInfo + ".";
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Add start for extern C structure.

   \param[in]  orc_Data    File data to append structure to
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OSCExportUti::h_AddExternCStart(C_SCLStringList & orc_Data)
{
   orc_Data.Append("#ifdef __cplusplus");
   orc_Data.Append("extern \"C\" {");
   orc_Data.Append("#endif");
   orc_Data.Append("");
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Add end for extern C structure.

   \param[in]  orc_Data    File data to append structure to
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OSCExportUti::h_AddExternCEnd(C_SCLStringList & orc_Data)
{
   orc_Data.Append("#ifdef __cplusplus");
   orc_Data.Append("} /* end of extern \"C\" */");
   orc_Data.Append("#endif");
   orc_Data.Append("");
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Add define with project ID to ensure file consistency between .c and .h file.

   \param[out]  orc_Data         File data as string list
   \param[in]   orc_MagicName    Magic name including project ID
   \param[in]   oq_HeaderFile    Flag if .c or .h file (true: header file)
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OSCExportUti::h_AddProjectIdDef(C_SCLStringList & orc_Data, const C_SCLString & orc_MagicName,
                                       const bool oq_HeaderFile)
{
   if (oq_HeaderFile == true)
   {
      orc_Data.Append("///unique ID to ensure consistency between .h and .c files");
      orc_Data.Append("#define " + orc_MagicName + " void " + orc_MagicName.LowerCase() + "(void) {}");
      orc_Data.Append("");
   }
   else
   {
      orc_Data.Append("///ensure file consistency (if compilation fails here the .h file does not match this .c file)");
      orc_Data.Append(orc_MagicName);
      orc_Data.Append("");
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Add function prototype section with magic name

   \param[out]  orc_Data         File data as string list
   \param[in]   orc_MagicName    Magic name including project ID
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OSCExportUti::h_AddProjIdFunctionPrototype(C_SCLStringList & orc_Data, const C_SCLString & orc_MagicName)
{
   orc_Data.Append(C_OSCExportUti::h_GetSectionSeparator("Function Prototypes"));
   orc_Data.Append("///unique ID to ensure consistency between .h and .c files");
   orc_Data.Append("extern void " + orc_MagicName.LowerCase() + "(void);");
   orc_Data.Append("");
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Store assembled data in file

   \param[in]  orc_Data       File data as string list
   \param[in]  orc_Path       Directory path for created file excluding file name
   \param[in]  orc_FileName   File name excluding file extension
   \param[in]  oq_HeaderFile  Flag if .c or .h file (true: header file)

   \return
   C_NO_ERR Operation success
   C_RD_WR  Operation failure: cannot store file
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_OSCExportUti::h_SaveToFile(stw_scl::C_SCLStringList & orc_Data, const stw_scl::C_SCLString & orc_Path,
                                    const stw_scl::C_SCLString & orc_FileName, const bool oq_HeaderFile)
{
   sint32 s32_Retval = C_NO_ERR;
   C_SCLString c_PathAndFilename;

   // get file path: path + filename + extension
   // add path + add filename + extension
   c_PathAndFilename = orc_Path + "/" + orc_FileName;

   if (oq_HeaderFile == true)
   {
      c_PathAndFilename += ".h";
   }
   else
   {
      c_PathAndFilename += ".c";
   }

   // store into file
   try
   {
      orc_Data.SaveToFile(c_PathAndFilename);
   }
   catch (...)
   {
      osc_write_log_error("Creating source code", "Could not write to file \"" + c_PathAndFilename + "\"");
      s32_Retval = C_RD_WR;
   }

   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Add .c and .h file paths to file paths list.

   \param[in,out]  orc_FilePaths    List of file paths
   \param[in]      orc_Path         Base path of files to add
   \param[in]      orc_FileName     File base name of files to add
   \param[in]      oq_SourceCode    Flag if source code (.c and .h) or parameter set image (.syde_psi)
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OSCExportUti::h_CollectFilePaths(std::vector<C_SCLString> & orc_FilePaths, const C_SCLString & orc_Path,
                                        const C_SCLString & orc_FileName, const bool oq_SourceCode)
{
   C_SCLString c_FileName;

   c_FileName = TGL_FileIncludeTrailingDelimiter(orc_Path) + orc_FileName;
   if (oq_SourceCode == true)
   {
      orc_FilePaths.push_back(c_FileName + ".h");
      orc_FilePaths.push_back(c_FileName + ".c");
   }
   else
   {
      orc_FilePaths.push_back(c_FileName); // file suffix already included in this case
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get type prefix according to data type

   \param[in]  oe_Type     data type (uint8, sint8, ...)
   \param[in]  oq_IsArray  data type is an array

   \return
   data type as variable prefix string
*/
//----------------------------------------------------------------------------------------------------------------------
C_SCLString C_OSCExportUti::h_GetTypePrefix(const C_OSCNodeDataPoolContent::E_Type oe_Type, const bool oq_IsArray)
{
   C_SCLString c_Prefix;

   if (oq_IsArray == true)
   {
      c_Prefix = "a";
   }
   else
   {
      c_Prefix = "";
   }

   switch (oe_Type)
   {
   case C_OSCNodeDataPoolContent::eUINT8: // Data type unsigned 8 bit integer
      c_Prefix += "u8";
      break;
   case C_OSCNodeDataPoolContent::eUINT16: // Data type unsigned 16 bit integer
      c_Prefix += "u16";
      break;
   case C_OSCNodeDataPoolContent::eUINT32: // Data type unsigned 32 bit integer
      c_Prefix += "u32";
      break;
   case C_OSCNodeDataPoolContent::eUINT64: // Data type unsigned 64 bit integer
      c_Prefix += "u64";
      break;
   case C_OSCNodeDataPoolContent::eSINT8: // Data type signed 8 bit integer
      c_Prefix += "s8";
      break;
   case C_OSCNodeDataPoolContent::eSINT16: // Data type signed 16 bit integer
      c_Prefix += "s16";
      break;
   case C_OSCNodeDataPoolContent::eSINT32: // Data type signed 32 bit integer
      c_Prefix += "s32";
      break;
   case C_OSCNodeDataPoolContent::eSINT64: // Data type signed 64 bit integer
      c_Prefix += "s64";
      break;
   case C_OSCNodeDataPoolContent::eFLOAT32: // Data type 32 bit floating point
      c_Prefix += "f32";
      break;
   case C_OSCNodeDataPoolContent::eFLOAT64: // Data type 64 bit floating point
      c_Prefix += "f64";
      break;
   default:
      break;
   }

   return c_Prefix;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get C variable name for a Datapool list element.

   \param[in]  orc_Name       Name
   \param[in]  oq_IsArray     Flag if array
   \param[in]  oe_Type        Content type
   \param[in]  orc_ArrayPos   Array position

   \return
   C variable name for given Datapool list element
*/
//----------------------------------------------------------------------------------------------------------------------
C_SCLString C_OSCExportUti::h_GetElementCName(const C_SCLString & orc_Name, const bool oq_IsArray,
                                              const C_OSCNodeDataPoolContent::E_Type oe_Type,
                                              const C_SCLString & orc_ArrayPos)
{
   C_SCLString c_Return = C_OSCExportUti::h_GetTypePrefix(oe_Type, oq_IsArray) + "_" + orc_Name;

   if (oq_IsArray == true)
   {
      c_Return += "[" + orc_ArrayPos + "]";
   }

   return c_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Convert 32-bit float to string with %g formatter and maximum representable precision.

   Always adds a decimal point.

   \param[in]  of32_Value     Float value
   \param[in]  opq_InfOrNan   Flag if conversion returned "inf" or "nan"

   \return
   Value converted to string
*/
//----------------------------------------------------------------------------------------------------------------------
C_SCLString C_OSCExportUti::h_FloatToStrG(const float32 of32_Value, bool * const opq_InfOrNan)
{
   C_SCLString c_Return;
   bool q_InfOrNan;

   c_Return.StringPrintFormatted("%.*g", 9, static_cast<float64>(of32_Value));
   // do not use '#' option of print formatted, as it also adds trailing zeroes

   q_InfOrNan = h_CheckInfOrNan(c_Return);

   if (opq_InfOrNan != NULL)
   {
      *opq_InfOrNan = q_InfOrNan;
   }

   if (q_InfOrNan == false)
   {
      h_AddDecimalPointIfNone(c_Return);
   }

   return c_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Convert 64-bit float to string with %g formatter and maximum representable precision

   Always adds a decimal point.

   \param[in]  of64_Value     Float value
   \param[in]  opq_InfOrNan   Flag if conversion returned "inf" or "nan"

   \return
   Value converted to string
*/
//----------------------------------------------------------------------------------------------------------------------
C_SCLString C_OSCExportUti::h_FloatToStrG(const float64 of64_Value, bool * const opq_InfOrNan)
{
   C_SCLString c_Return;
   bool q_InfOrNan;

   c_Return.StringPrintFormatted("%.*g", 17, of64_Value);
   // do not use '#' option of print formatted, as it also adds trailing zeroes

   q_InfOrNan = h_CheckInfOrNan(c_Return);

   if (opq_InfOrNan != NULL)
   {
      *opq_InfOrNan = q_InfOrNan;
   }

   if (q_InfOrNan == false)
   {
      h_AddDecimalPointIfNone(c_Return);
   }

   return c_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Check if a string contains inf or nan (case independent)

   \param[in]  orc_String  String

   \retval   true   string contains "inf" or "nan"
   \retval   false  string does not contain "inf" or "nan"
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_OSCExportUti::h_CheckInfOrNan(const C_SCLString & orc_String)
{
   bool q_Return;

   if ((orc_String.LowerCase().Pos("inf") > 0) || (orc_String.LowerCase().Pos("nan") > 0))
   {
      q_Return = true;
   }
   else
   {
      q_Return = false;
   }

   return q_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Add decimal point to a float string if there is none.

   Use this if float was converted with %g, but without '#' option as trailing zeroes shall be omitted.

   Examples:
      5        -> 5.0
      5e+20    -> 5.0e+20
      123.45   -> 123.45

   \param[in,out]  orc_FloatString  Float string
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OSCExportUti::h_AddDecimalPointIfNone(C_SCLString & orc_FloatString)
{
   if (orc_FloatString.Pos('.') == 0)
   {
      if (orc_FloatString.Pos('e') > 0)
      {
         orc_FloatString = orc_FloatString.Insert(".0", orc_FloatString.Pos('e'));
      }
      else
      {
         orc_FloatString += ".0";
      }
   }
}
