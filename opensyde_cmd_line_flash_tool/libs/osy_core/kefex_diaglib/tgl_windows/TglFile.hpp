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
#ifndef TGLFILEHPP
#define TGLFILEHPP

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "stwtypes.hpp"
#include "C_SclString.hpp"
#include "C_SclDynamicArray.hpp"

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

bool TglFileAgeString(const stw::scl::C_SclString & orc_FileName, stw::scl::C_SclString & orc_String);
int32_t TglFileSize(const stw::scl::C_SclString & orc_FileName);
bool TglDirectoryExists(const stw::scl::C_SclString & orc_Path);
bool TglFileExists(const stw::scl::C_SclString & orc_FileName);

int32_t TglFileFind(const stw::scl::C_SclString & orc_SearchPattern,
                    stw::scl::C_SclDynamicArray<C_TglFileSearchRecord> & orc_FoundFiles);

stw::scl::C_SclString TglFileIncludeTrailingDelimiter(const stw::scl::C_SclString & orc_Path);
stw::scl::C_SclString TglExtractFileExtension(const stw::scl::C_SclString & orc_Path);
stw::scl::C_SclString TglChangeFileExtension(const stw::scl::C_SclString & orc_Path,
                                             const stw::scl::C_SclString & orc_Extension);
stw::scl::C_SclString TglGetExePath(void);
stw::scl::C_SclString TglExtractFilePath(const stw::scl::C_SclString & orc_Path);
stw::scl::C_SclString TglExtractFileName(const stw::scl::C_SclString & orc_Path);
stw::scl::C_SclString TglExpandFileName(const stw::scl::C_SclString & orc_RelativePath,
                                        const stw::scl::C_SclString & orc_BasePath);
bool TglIsRelativePath(const stw::scl::C_SclString & orc_Path);

int32_t TglCreateDirectory(const stw::scl::C_SclString & orc_Directory);
int32_t TglRemoveDirectory(const stw::scl::C_SclString & orc_Directory, const bool oq_ContentOnly);

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
}

#endif
