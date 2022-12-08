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
#include "stwtypes.hpp"
#include "C_SclString.hpp"
#include "C_SclDateTime.hpp"
#include "C_SclIniFile.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_core
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_OscCanOpenEdsFileInfoBlock
{
public:
   C_OscCanOpenEdsFileInfoBlock();

   stw::scl::C_SclString c_FileName;         ///< File name
   uint8_t u8_FileVersion;                   ///< File version
   uint8_t u8_FileRevision;                  ///< File revision
   stw::scl::C_SclString c_EdsVersion;       ///< EDS version, in format X.y (3 chars)
   stw::scl::C_SclString c_Description;      ///< File description, max 243 characters
   stw::scl::C_SclString c_CreationTime;     ///< Creation time as hh:mm(AM|PM)
   stw::scl::C_SclString c_CreationDate;     ///< Creation date as mm-dd-yyyy
   stw::scl::C_SclString c_CreatedBy;        ///< Name or description of file creator, max 245 characters
   stw::scl::C_SclString c_ModificationTime; ///< Modification time as hh:mm(AM|PM)
   stw::scl::C_SclString c_ModificationDate; ///< Modification date as mm-dd-yyyy
   stw::scl::C_SclString c_ModifiedBy;       ///< Name or description of file creator, max 244 characters

   void CalcHash(uint32_t & oru32_HashValue) const;
   int32_t LoadFromIni(stw::scl::C_SclIniFile & orc_File, stw::scl::C_SclString & orc_LastError);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
