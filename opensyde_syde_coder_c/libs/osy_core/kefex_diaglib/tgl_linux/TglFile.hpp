//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       DiagLib Target Glue Layer: File functions

   DiagLib Target Glue Layer module containing file functions:
   - file timestamps
   - file size
   - file exists

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef TGLFILEHPP
#define TGLFILEHPP

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "stwtypes.hpp"
#include "C_SclString.hpp"
#include "C_SclDynamicArray.hpp"

#ifndef TGL_PACKAGE
#ifdef __BORLANDC__
#define TGL_PACKAGE __declspec(package)
#else
#define TGL_PACKAGE
#endif
#endif

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace tgl
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

///Information about found files
class C_TglFileSearchRecord
{
public:
   stw::scl::C_SclString c_FileName; ///< name of found file (with full path)
   //for now we only need the name ...
};

bool TGL_PACKAGE TglFileAgeString(const stw::scl::C_SclString & orc_FileName, stw::scl::C_SclString & orc_String);
int32_t TGL_PACKAGE TglFileSize(const stw::scl::C_SclString & orc_FileName);
bool TGL_PACKAGE TglDirectoryExists(const stw::scl::C_SclString & orc_Path);
bool TGL_PACKAGE TglFileExists(const stw::scl::C_SclString & orc_FileName);

int32_t TGL_PACKAGE TglFileFind(const stw::scl::C_SclString & orc_SearchPattern,
                                stw::scl::C_SclDynamicArray<C_TglFileSearchRecord> & orc_FoundFiles);

stw::scl::C_SclString TGL_PACKAGE TglFileIncludeTrailingDelimiter(const stw::scl::C_SclString & orc_Path);
stw::scl::C_SclString TGL_PACKAGE TglExtractFileExtension(const stw::scl::C_SclString & orc_Path);
stw::scl::C_SclString TGL_PACKAGE TglChangeFileExtension(const stw::scl::C_SclString & orc_Path,
                                                         const stw::scl::C_SclString & orc_Extension);
stw::scl::C_SclString TGL_PACKAGE TglGetExePath(void);
stw::scl::C_SclString TGL_PACKAGE TglExtractFilePath(const stw::scl::C_SclString & orc_Path);
stw::scl::C_SclString TGL_PACKAGE TglExtractFileName(const stw::scl::C_SclString & orc_Path);
stw::scl::C_SclString TGL_PACKAGE TglExpandFileName(const stw::scl::C_SclString & orc_RelativePath,
                                                    const stw::scl::C_SclString & orc_BasePath);
int32_t TGL_PACKAGE TglCreateDirectory(const stw::scl::C_SclString & orc_Directory);
int32_t TGL_PACKAGE TglRemoveDirectory(const stw::scl::C_SclString & orc_Directory, const bool oq_ContentOnly);

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
}

#endif
