//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Utility functions for system filers (implementation)

   Utility functions for system filers

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.h"

#include <cstdio>

#include "stwtypes.h"
#include "stwerrors.h"
#include "TGLFile.h"
#include "C_OSCUtils.h"
#include "C_OSCSystemFilerUtil.h"
#include "C_OSCLoggingHandler.h"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw_types;
using namespace stw_opensyde_core;
using namespace stw_scl;
using namespace stw_tgl;
using namespace stw_errors;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

/* -- Global Variables ---------------------------------------------------------------------------------------------- */

/* -- Module Global Variables --------------------------------------------------------------------------------------- */

/* -- Module Global Function Prototypes ----------------------------------------------------------------------------- */

/* -- Implementation ------------------------------------------------------------------------------------------------ */

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Convert bus type enum to string

   \param[in]   oe_Type   Bus type

   \return  string representation of oe_Type
*/
//----------------------------------------------------------------------------------------------------------------------
C_SCLString C_OSCSystemFilerUtil::h_BusTypeEnumToString(const C_OSCSystemBus::E_Type oe_Type)
{
   C_SCLString c_Retval;

   if (oe_Type == C_OSCSystemBus::eETHERNET)
   {
      c_Retval = "ethernet";
   }
   else
   {
      c_Retval = "can";
   }
   return c_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Convert bus type string to enum

   \param[in]  orc_Type   type string
   \param[out] ore_Type   Enum corresponding to type

   \return
   C_NO_ERR   no error
   C_RANGE    String unknown
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_OSCSystemFilerUtil::h_BusTypeStringToEnum(const stw_scl::C_SCLString & orc_Type,
                                                   C_OSCSystemBus::E_Type & ore_Type)
{
   sint32 s32_Retval = C_NO_ERR;

   if (orc_Type == "ethernet")
   {
      ore_Type = C_OSCSystemBus::eETHERNET;
   }
   else if (orc_Type == "can")
   {
      ore_Type = C_OSCSystemBus::eCAN;
   }
   else
   {
      osc_write_log_error("Loading System Definition", "Invalid value for bus.\"type\":" + orc_Type);
      s32_Retval = C_RANGE;
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get parser at root file node and handle all possible errors

   Warning: includes error logging

   \param[in,out] orc_FileXMLParser XML parser
   \param[in]     orc_Path          File path
   \param[in]     orc_RootNode      Root node name

   \return
   C_NO_ERR   data was read from file
   C_NOACT    could not read data from file
   C_RANGE    file not found
   C_CONFIG   root node not found
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_OSCSystemFilerUtil::h_GetParserForExistingFile(C_OSCXMLParser & orc_FileXMLParser,
                                                        const C_SCLString & orc_Path, const C_SCLString & orc_RootNode)
{
   sint32 s32_Retval = C_NO_ERR;

   if (TGL_FileExists(orc_Path))
   {
      s32_Retval = orc_FileXMLParser.LoadFromFile(orc_Path);
      if (s32_Retval == C_NO_ERR)
      {
         if (orc_FileXMLParser.SelectRoot() != orc_RootNode)
         {
            s32_Retval = C_CONFIG;
            osc_write_log_error("Loading files", "Unexpected content for file \"" + orc_Path + "\"");
         }
      }
      else
      {
         osc_write_log_error("Loading files", "Failure parsing file \"" + orc_Path + "\"");
      }
   }
   else
   {
      osc_write_log_error("Loading files", "Missing file \"" + orc_Path + "\"");
      s32_Retval = C_RANGE;
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get parser at root file node and handle all possible errors

   Warning: includes error logging

   \param[in,out] orc_FileXMLParser XML parser
   \param[in]     orc_Path          File path
   \param[in]     orc_RootNode      Root node name

   \return
   C_NO_ERR   XML handle was created
   C_NOACT    existing file could not be deleted
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_OSCSystemFilerUtil::h_GetParserForNewFile(C_OSCXMLParser & orc_FileXMLParser, const C_SCLString & orc_Path,
                                                   const C_SCLString & orc_RootNode)
{
   sint32 s32_Retval = C_NO_ERR;

   if (TGL_FileExists(orc_Path))
   {
      if (std::remove(orc_Path.c_str()) == 0)
      {
         s32_Retval = C_NO_ERR;
      }
      else
      {
         s32_Retval = C_NOACT;
         osc_write_log_error("Saving files", "Could not delete file \"" + orc_Path + "\"");
      }
   }
   orc_FileXMLParser.CreateAndSelectNodeChild(orc_RootNode);
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Create directory

   Warning: includes error logging

   \param[in] orc_Path Folder path

   \return
   C_NO_ERR   XML handle was created
   C_NOACT    folder could not be created
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_OSCSystemFilerUtil::h_CreateFolder(const C_SCLString & orc_Path)
{
   sint32 s32_Retval;

   if (TGL_DirectoryExists(orc_Path))
   {
      s32_Retval = C_NO_ERR;
   }
   else
   {
      if (TGL_CreateDirectory(orc_Path) == 0)
      {
         s32_Retval = C_NO_ERR;
      }
      else
      {
         s32_Retval = C_NOACT;
         osc_write_log_error("Saving files", "Could not create directory \"" + orc_Path + "\"");
      }
   }

   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Adapt item name for file name usage

   \param[in] orc_ItemName Item name

   \return
   Item name ready for file name usage
*/
//----------------------------------------------------------------------------------------------------------------------
C_SCLString C_OSCSystemFilerUtil::h_PrepareItemNameForFileName(const C_SCLString & orc_ItemName)
{
   return C_OSCUtils::h_NiceifyStringForFileName(orc_ItemName.LowerCase());
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Combine the base path and a relative sub folder path

   \param[in] orc_BasePathName      Base path name
   \param[in] orc_SubFolderFileName Sub folder directory and file name

   \return
   Full, combined path
*/
//----------------------------------------------------------------------------------------------------------------------
C_SCLString C_OSCSystemFilerUtil::h_CombinePaths(const C_SCLString & orc_BasePathName,
                                                 const C_SCLString & orc_SubFolderFileName)
{
   const C_SCLString c_BasePath = TGL_ExtractFilePath(orc_BasePathName);

   return c_BasePath + orc_SubFolderFileName;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Transform export scaling support type to string

   \param[in]  ore_Scaling   Flash loader type

   \return
   Stringified export scaling support type
*/
//----------------------------------------------------------------------------------------------------------------------
C_SCLString C_OSCSystemFilerUtil::h_CodeExportScalingTypeToString(
   const C_OSCNodeCodeExportSettings::E_Scaling & ore_Scaling)
{
   C_SCLString c_Retval;

   switch (ore_Scaling)
   {
   case C_OSCNodeCodeExportSettings::eFLOAT32:
      c_Retval = "float32";
      break;
   case C_OSCNodeCodeExportSettings::eFLOAT64:
      c_Retval = "float64";
      break;
   case C_OSCNodeCodeExportSettings::eNONE:
      c_Retval = "none";
      break;
   default:
      c_Retval = "invalid";
      break;
   }

   return c_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Transform string to code export scaling support type.

   \param[in]   orc_String    String to interpret
   \param[out]  ore_Scaling   Scaling support type

   \return
   C_NO_ERR   no error
   C_RANGE    String unknown
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_OSCSystemFilerUtil::h_StringToCodeExportScalingType(const C_SCLString & orc_String,
                                                             C_OSCNodeCodeExportSettings::E_Scaling & ore_Scaling)
{
   sint32 s32_Retval = C_NO_ERR;

   if (orc_String == "float32")
   {
      ore_Scaling = C_OSCNodeCodeExportSettings::eFLOAT32;
   }
   else if (orc_String == "float64")
   {
      ore_Scaling = C_OSCNodeCodeExportSettings::eFLOAT64;
   }
   else if (orc_String == "none")
   {
      ore_Scaling = C_OSCNodeCodeExportSettings::eNONE;
   }
   else
   {
      osc_write_log_error("Loading node definition",
                          "Invalid value for \"properties\".\"code-export-settings\".\"scaling-support\": " +
                          orc_String);
      s32_Retval = C_RANGE;
   }

   return s32_Retval;
}
