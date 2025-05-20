//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       openSYDE Core AES file encryption utility

   \class       stw::opensyde_core::C_OscAesFile
   \brief       openSYDE Core zip/unzip file utilities

   Wrapper for the AES library by SergeyBel.
   Provides functions to encrypt/decrypt a file with AES.

   \copyright   Copyright 2021 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_OSCAESFILE_HPP
#define C_OSCAESFILE_HPP

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

class C_OscAesFile
{
public:
   static int32_t h_EncryptFile(const stw::scl::C_SclString & orc_Key, const stw::scl::C_SclString & orc_InFilePath,
                                const stw::scl::C_SclString & orc_OutFilePath);

   static int32_t h_DecryptFile(const stw::scl::C_SclString & orc_Key, const stw::scl::C_SclString & orc_InFilePath,
                                const stw::scl::C_SclString & orc_OutFilePath);

   static int32_t h_CreateEncryptedZipFile(const stw::scl::C_SclString & orc_FolderPathToZip,
                                           const std::set<stw::scl::C_SclString> & orc_SupFiles,
                                           const stw::scl::C_SclString & orc_PathForZipFile,
                                           const stw::scl::C_SclString & orc_Key,
                                           stw::scl::C_SclString * const opc_ErrorMessage);
   static int32_t h_UnpackEncryptedZipFile(const stw::scl::C_SclString & orc_PathOfZipFile,
                                           const stw::scl::C_SclString & orc_FolderPathToUnzip,
                                           const stw::scl::C_SclString & orc_Key,
                                           stw::scl::C_SclString * const opc_ErrorMessage);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
}

#endif
