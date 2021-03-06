//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       openSYDE Core zip/unzip utilities

   \class       stw_opensyde_core::C_OSCZipFile
   \brief       openSYDE Core zip/unzip file utilities

   Wrapper for the miniz library.
   Provides functions to zip/unzip data to/from file

   \copyright   Copyright 2018 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_OSCZIPFILEH
#define C_OSCZIPFILEH

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <set>
#include "stwtypes.h"
#include "CSCLString.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw_opensyde_core
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_OSCZipFile
{
public:
   static stw_types::sint32 h_CreateZipFile(const stw_scl::C_SCLString & orc_SourcePath,
                                            const std::set<stw_scl::C_SCLString> & orc_SupFiles,
                                            const stw_scl::C_SCLString & orc_ZipArchivePath,
                                            stw_scl::C_SCLString * const opc_ErrorText = NULL);

   static stw_types::sint32 h_UnpackZipFile(const stw_scl::C_SCLString & orc_SourcePath,
                                            const stw_scl::C_SCLString & orc_TargetUnzipPath,
                                            stw_scl::C_SCLString * const opc_ErrorText = NULL);

   static void h_AppendFilesRelative(std::set<stw_scl::C_SCLString> & orc_Set,
                                     const std::vector<stw_scl::C_SCLString> & orc_Files,
                                     const stw_scl::C_SCLString & orc_BasePath);

   static stw_types::sint32 h_IsZipFile(const stw_scl::C_SCLString & orc_FilePath);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}

#endif
