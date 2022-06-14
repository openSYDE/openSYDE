//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Utility functions for system filers (header)

   See cpp file for detailed description

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_OSCSYSTEMFILERUTIL_H
#define C_OSCSYSTEMFILERUTIL_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "C_OSCXMLParser.h"
#include "C_OSCSystemBus.h"
#include "C_OSCNodeCodeExportSettings.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw_opensyde_core
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_OSCSystemFilerUtil
{
public:
   static stw_scl::C_SCLString h_BusTypeEnumToString(const C_OSCSystemBus::E_Type oe_Type);
   static stw_types::sint32 h_BusTypeStringToEnum(const stw_scl::C_SCLString & orc_Type,
                                                  C_OSCSystemBus::E_Type & ore_Type);
   static stw_scl::C_SCLString h_CodeExportScalingTypeToString(const C_OSCNodeCodeExportSettings::E_Scaling &
                                                               ore_Scaling);
   static stw_types::sint32 h_StringToCodeExportScalingType(const stw_scl::C_SCLString & orc_String,
                                                            C_OSCNodeCodeExportSettings::E_Scaling & ore_Scaling);
   static stw_types::sint32 h_GetParserForExistingFile(C_OSCXMLParser & orc_FileXMLParser,
                                                       const stw_scl::C_SCLString & orc_Path,
                                                       const stw_scl::C_SCLString & orc_RootNode);
   static stw_types::sint32 h_GetParserForNewFile(C_OSCXMLParser & orc_FileXMLParser,
                                                  const stw_scl::C_SCLString & orc_Path,
                                                  const stw_scl::C_SCLString & orc_RootNode);
   static stw_types::sint32 h_CreateFolder(const stw_scl::C_SCLString & orc_Path);
   static stw_scl::C_SCLString h_PrepareItemNameForFileName(const stw_scl::C_SCLString & orc_ItemName);
   static stw_scl::C_SCLString h_CombinePaths(const stw_scl::C_SCLString & orc_BasePathName,
                                              const stw_scl::C_SCLString & orc_SubFolderFileName);
   static stw_types::sint32 h_SaveStringToFile(const stw_scl::C_SCLString & orc_CompleteFileAsString,
                                               const stw_scl::C_SCLString & orc_CompleteFilePath,
                                               const stw_scl::C_SCLString & orc_LogHeading);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif
