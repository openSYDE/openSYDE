//-----------------------------------------------------------------------------
/*!
   \file
   \brief       openSYDE Core zip/unzip utilities

   \class       stw_opensyde_core::C_OSCZipFile
   \brief       openSYDE Core zip/unzip file utilities

   Wrapper for the miniz library.
   Provides functions to zip/unzip data to/from file

   \implementation
   project     openSYDE
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     05.10.2018  STW/A.Stangl (refactored from pre-existing code)
   \endimplementation
*/
//-----------------------------------------------------------------------------
#ifndef C_OSCZIPFILEH
#define C_OSCZIPFILEH

/* -- Includes ------------------------------------------------------------- */
#include <set>
#include "stwtypes.h"
#include "CSCLString.h"

/* -- Namespace ------------------------------------------------------------ */
namespace stw_opensyde_core
{
/* -- Global Constants ----------------------------------------------------- */

/* -- Types ---------------------------------------------------------------- */

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
};

/* -- Extern Global Variables ---------------------------------------------- */
}

#endif
