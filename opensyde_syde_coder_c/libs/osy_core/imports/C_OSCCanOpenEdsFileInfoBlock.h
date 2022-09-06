//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Class for file info block handling of EDS/DCF files
   \copyright   Copyright 2022 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_OSCCANOPENEDSFILEINFOBLOCK_H
#define C_OSCCANOPENEDSFILEINFOBLOCK_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "stwtypes.h"
#include "CSCLString.h"
#include "CSCLDateTime.h"
#include "CSCLIniFile.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw_opensyde_core
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_OSCCanOpenEdsFileInfoBlock
{
public:
   C_OSCCanOpenEdsFileInfoBlock();

   stw_scl::C_SCLString c_FileName;         ///< File name
   stw_types::uint8 u8_FileVersion;         ///< File version
   stw_types::uint8 u8_FileRevision;        ///< File revision
   stw_scl::C_SCLString c_EDSVersion;       ///< EDS version, in format X.y (3 chars)
   stw_scl::C_SCLString c_Description;      ///< File description, max 243 characters
   stw_scl::C_SCLString c_CreationTime;     ///< Creation time as hh:mm(AM|PM)
   stw_scl::C_SCLString c_CreationDate;     ///< Creation date as mm-dd-yyyy
   stw_scl::C_SCLString c_CreatedBy;        ///< Name or description of file creator, max 245 characters
   stw_scl::C_SCLString c_ModificationTime; ///< Modification time as hh:mm(AM|PM)
   stw_scl::C_SCLString c_ModificationDate; ///< Modification date as mm-dd-yyyy
   stw_scl::C_SCLString c_ModifiedBy;       ///< Name or description of file creator, max 244 characters

   void CalcHash(stw_types::uint32 & oru32_HashValue) const;
   stw_types::sint32 LoadFromIni(stw_scl::C_SCLIniFile & orc_File, stw_scl::C_SCLString & orc_LastError);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif
