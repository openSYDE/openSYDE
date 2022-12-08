//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Class for file info block handling of EDS/DCF files

   Class for file info block handling of EDS/DCF files

   \copyright   Copyright 2022 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.hpp"

#include "stwerrors.hpp"
#include "C_SclChecksums.hpp"
#include "C_OscCanOpenEdsFileInfoBlock.hpp"
#include "C_OscCanOpenEdsDeviceInfoBlock.hpp"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */

using namespace stw::errors;
using namespace stw::opensyde_core;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

/* -- Global Variables ---------------------------------------------------------------------------------------------- */

/* -- Module Global Variables --------------------------------------------------------------------------------------- */

/* -- Module Global Function Prototypes ----------------------------------------------------------------------------- */

/* -- Implementation ------------------------------------------------------------------------------------------------ */

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Default constructor
*/
//----------------------------------------------------------------------------------------------------------------------
C_OscCanOpenEdsFileInfoBlock::C_OscCanOpenEdsFileInfoBlock() :
   u8_FileVersion(0),
   u8_FileRevision(0)
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Calculates the hash value over all data

   The hash value is a 32 bit CRC value.

   \param[in,out]  oru32_HashValue  Hash value with unit [in] value and result [out] value
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OscCanOpenEdsFileInfoBlock::CalcHash(uint32_t & oru32_HashValue) const
{
   stw::scl::C_SclChecksums::CalcCRC32(this->c_FileName.c_str(), this->c_FileName.Length(), oru32_HashValue);
   stw::scl::C_SclChecksums::CalcCRC32(&this->u8_FileVersion, sizeof(this->u8_FileVersion),
                                       oru32_HashValue);
   stw::scl::C_SclChecksums::CalcCRC32(&this->u8_FileRevision, sizeof(this->u8_FileRevision),
                                       oru32_HashValue);
   stw::scl::C_SclChecksums::CalcCRC32(this->c_EdsVersion.c_str(), this->c_EdsVersion.Length(), oru32_HashValue);
   stw::scl::C_SclChecksums::CalcCRC32(this->c_Description.c_str(), this->c_Description.Length(), oru32_HashValue);
   stw::scl::C_SclChecksums::CalcCRC32(this->c_CreationTime.c_str(), this->c_CreationTime.Length(), oru32_HashValue);
   stw::scl::C_SclChecksums::CalcCRC32(this->c_CreationDate.c_str(), this->c_CreationDate.Length(), oru32_HashValue);
   stw::scl::C_SclChecksums::CalcCRC32(this->c_CreatedBy.c_str(), this->c_CreatedBy.Length(), oru32_HashValue);
   stw::scl::C_SclChecksums::CalcCRC32(this->c_ModificationTime.c_str(),
                                       this->c_ModificationTime.Length(), oru32_HashValue);
   stw::scl::C_SclChecksums::CalcCRC32(this->c_ModificationDate.c_str(),
                                       this->c_ModificationDate.Length(), oru32_HashValue);
   stw::scl::C_SclChecksums::CalcCRC32(this->c_ModifiedBy.c_str(), this->c_ModifiedBy.Length(), oru32_HashValue);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Load from ini

   \param[in,out]  orc_File         File
   \param[in,out]  orc_LastError    Last error

   \return
   STW error codes

   \retval   C_NO_ERR   Values read
   \retval   C_CONFIG   At least one value not found, for details see error message
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_OscCanOpenEdsFileInfoBlock::LoadFromIni(stw::scl::C_SclIniFile & orc_File,
                                                  stw::scl::C_SclString & orc_LastError)
{
   //lint -e{8062} Kept for later error reporting
   const int32_t s32_Retval = C_NO_ERR;
   const stw::scl::C_SclString c_SectionName = "FileInfo";

   orc_LastError = "";

   if (orc_File.SectionExists(c_SectionName))
   {
      //Maybe mandatory values
      this->c_FileName = orc_File.ReadString(c_SectionName, "FileName", "");
      this->u8_FileVersion = orc_File.ReadUint8(c_SectionName, "FileVersion", 0);
      this->u8_FileRevision = orc_File.ReadUint8(c_SectionName, "FileRevision", 0);
      this->c_Description = orc_File.ReadString(c_SectionName, "Description", "");
      this->c_CreationTime = orc_File.ReadString(c_SectionName, "CreationTime", "");
      this->c_CreationDate = orc_File.ReadString(c_SectionName, "CreationDate", "");
      this->c_CreatedBy = orc_File.ReadString(c_SectionName, "CreatedBy", "");
      //Optional values
      this->c_EdsVersion = orc_File.ReadString(c_SectionName, "EDSVersion", "3.0");
      this->c_ModificationTime = orc_File.ReadString(c_SectionName, "ModificationTime", "");
      this->c_ModificationDate = orc_File.ReadString(c_SectionName, "ModificationDate", "");
      this->c_ModifiedBy = orc_File.ReadString(c_SectionName, "ModifiedBy", "");
   }

   return s32_Retval;
}
