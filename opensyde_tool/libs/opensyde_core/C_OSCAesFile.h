//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       openSYDE Core AES file encryption utility

   \class       stw_opensyde_core::C_OSCAesFile
   \brief       openSYDE Core zip/unzip file utilities

   Wrapper for the AES library by SergeyBel.
   Provides functions to encrypt/decrypt a file with AES.

   \copyright   Copyright 2021 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_OSCAESFILEH
#define C_OSCAESFILEH

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "stwtypes.h"
#include "CSCLString.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw_opensyde_core
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_OSCAesFile
{
public:
   static stw_types::sint32 h_EncryptFile(const stw_scl::C_SCLString & orc_Key,
                                          const stw_scl::C_SCLString & orc_InFilePath,
                                          const stw_scl::C_SCLString & orc_OutFilePath);

   static stw_types::sint32 h_DecryptFile(const stw_scl::C_SCLString & orc_Key,
                                          const stw_scl::C_SCLString & orc_InFilePath,
                                          const stw_scl::C_SCLString & orc_OutFilePath);

   static stw_types::sint32 h_CreateEncryptedZipFile(const stw_scl::C_SCLString & orc_FolderPathToZip,
                                                     const std::vector<stw_scl::C_SCLString> & orc_FilesToZip,
                                                     const stw_scl::C_SCLString & orc_PathForZipFile,
                                                     const stw_scl::C_SCLString & orc_Key,
                                                     stw_scl::C_SCLString * const opc_ErrorMessage);
   static stw_types::sint32 h_UnpackEncryptedZipFile(const stw_scl::C_SCLString & orc_PathOfZipFile,
                                                     const stw_scl::C_SCLString & orc_FolderPathToUnzip,
                                                     const stw_scl::C_SCLString & orc_Key,
                                                     stw_scl::C_SCLString * const opc_ErrorMessage);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}

#endif
