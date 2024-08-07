//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Class for info block handling of EDS/DCF files

   Class for info block handling of EDS/DCF files

   \copyright   Copyright 2022 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.hpp"

#include "TglFile.hpp"
#include "stwerrors.hpp"
#include "C_OscUtils.hpp"
#include "C_SclChecksums.hpp"
#include "C_OscCanOpenEdsInfoBlock.hpp"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw::tgl;

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
C_OscCanOpenEdsInfoBlock::C_OscCanOpenEdsInfoBlock() :
   u32_FileHash(0)
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Calculates the hash value over all data

   The hash value is a 32 bit CRC value.

   \param[in,out]  oru32_HashValue  Hash value with unit [in] value and result [out] value
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OscCanOpenEdsInfoBlock::CalcHash(uint32_t & oru32_HashValue) const
{
   stw::scl::C_SclChecksums::CalcCRC32(&this->u32_FileHash, sizeof(this->u32_FileHash),
                                       oru32_HashValue);
   this->c_FileInfo.CalcHash(oru32_HashValue);
   this->c_DeviceInfo.CalcHash(oru32_HashValue);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Load from file

   \param[in]      orc_FilePath     File path
   \param[in,out]  orc_LastError    Last error

   \return
   STW error codes

   \retval   C_NO_ERR   Values read
   \retval   C_RANGE    File not found
   \retval   C_CONFIG   At least one value not found, for details see error message
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_OscCanOpenEdsInfoBlock::LoadFromFile(const stw::scl::C_SclString & orc_FilePath,
                                               stw::scl::C_SclString & orc_LastError)
{
   int32_t s32_Retval = C_NO_ERR;

   if (TglFileExists(orc_FilePath))
   {
      stw::scl::C_SclIniFile c_IniFile(orc_FilePath);
      C_OscUtils::h_FileToString(orc_FilePath, this->c_FileContentForSave);
      this->m_InitHash();
      s32_Retval = this->c_FileInfo.LoadFromIni(c_IniFile, orc_LastError);

      if (s32_Retval == C_NO_ERR)
      {
         s32_Retval = this->c_DeviceInfo.LoadFromIni(c_IniFile, orc_LastError);
      }
   }
   else
   {
      s32_Retval = C_RANGE;
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get granularity

   \return
   Granularity
*/
//----------------------------------------------------------------------------------------------------------------------
uint8_t C_OscCanOpenEdsInfoBlock::GetGranularity() const
{
   return this->c_DeviceInfo.GetGranularity();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Init hash
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OscCanOpenEdsInfoBlock::m_InitHash()
{
   this->u32_FileHash = 0xFFFFFFFFU;

   stw::scl::C_SclChecksums::CalcCRC32(this->c_FileContentForSave.c_str(),
                                       this->c_FileContentForSave.Length(), this->u32_FileHash);
}
