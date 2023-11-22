//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       openSYDE Core zip/unzip utilities

   \class       stw::opensyde_core::C_OscZipFile
   \brief       openSYDE Core zip/unzip file utilities

   Wrapper for the miniz library.
   Provides functions to zip/unzip data to/from file

   \copyright   Copyright 2018 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_OSCZIPFILE_HPP
#define C_OSCZIPFILE_HPP

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <set>
#include "stwtypes.hpp"
#include "C_SclString.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_core
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_OscZipFile
{
public:
   static int32_t h_CreateZipFile(const stw::scl::C_SclString & orc_SourcePath,
                                  const std::set<stw::scl::C_SclString> & orc_SupFiles,
                                  const stw::scl::C_SclString & orc_ZipArchivePath,
                                  stw::scl::C_SclString * const opc_ErrorText = NULL);

   static int32_t h_UnpackZipFile(const stw::scl::C_SclString & orc_SourcePath,
                                  const stw::scl::C_SclString & orc_TargetUnzipPath,
                                  stw::scl::C_SclString * const opc_ErrorText = NULL);

   static void h_AppendFilesRelative(std::set<stw::scl::C_SclString> & orc_Set,
                                     const std::vector<stw::scl::C_SclString> & orc_Files,
                                     const stw::scl::C_SclString & orc_BasePath);

   static int32_t h_IsZipFile(const stw::scl::C_SclString & orc_FilePath);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
}

#endif
