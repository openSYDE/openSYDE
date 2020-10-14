//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       DiagLib Target Glue Layer: File functions

   DiagLib Target Glue Layer module containing file functions:
   - file timestamps
   - file size
   - file exists

   \copyright   Copyright 2009 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef TGLFILEH
#define TGLFILEH

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "stwtypes.h"
#include "CSCLString.h"
#include "SCLDynamicArray.h"

#ifndef TGL_PACKAGE
#ifdef __BORLANDC__
#define TGL_PACKAGE __declspec(package)
#else
#define TGL_PACKAGE
#endif
#endif

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw_tgl
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

///Information about found files
struct TGL_FileSearchRecord
{
   stw_scl::C_SCLString c_FileName; ///< name of found file (with full path)
   //for now we only need the name ...
};

bool TGL_PACKAGE TGL_FileAgeString(const stw_scl::C_SCLString & orc_FileName, stw_scl::C_SCLString & orc_String);
stw_types::sint32 TGL_PACKAGE TGL_FileSize(const stw_scl::C_SCLString & orc_FileName);
bool TGL_PACKAGE TGL_DirectoryExists(const stw_scl::C_SCLString & orc_Path);
bool TGL_PACKAGE TGL_FileExists(const stw_scl::C_SCLString & orc_FileName);

stw_types::sint32 TGL_PACKAGE TGL_FileFind(const stw_scl::C_SCLString & orc_SearchPattern,
                                           stw_scl::SCLDynamicArray<TGL_FileSearchRecord> & orc_FoundFiles);

stw_scl::C_SCLString TGL_PACKAGE TGL_FileIncludeTrailingDelimiter(const stw_scl::C_SCLString & orc_Path);
stw_scl::C_SCLString TGL_PACKAGE TGL_ExtractFileExtension(const stw_scl::C_SCLString & orc_Path);
stw_scl::C_SCLString TGL_PACKAGE TGL_ChangeFileExtension(const stw_scl::C_SCLString & orc_Path,
                                                         const stw_scl::C_SCLString & orc_Extension);
stw_scl::C_SCLString TGL_PACKAGE TGL_GetExePath(void);
stw_scl::C_SCLString TGL_PACKAGE TGL_ExtractFilePath(const stw_scl::C_SCLString & orc_Path);
stw_scl::C_SCLString TGL_PACKAGE TGL_ExtractFileName(const stw_scl::C_SCLString & orc_Path);
stw_scl::C_SCLString TGL_PACKAGE TGL_ExpandFileName(const stw_scl::C_SCLString & orc_RelativePath,
                                                    const stw_scl::C_SCLString & orc_BasePath);
stw_types::sint32 TGL_PACKAGE TGL_CreateDirectory(const stw_scl::C_SCLString & orc_Directory);
stw_types::sint32 TGL_PACKAGE TGL_RemoveDirectory(const stw_scl::C_SCLString & orc_Directory,
                                                  const bool oq_ContentOnly);

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}

#endif
