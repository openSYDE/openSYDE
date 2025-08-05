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
#include "C_SclIniFile.hpp"
#include "C_SclStringList.hpp"
#include "C_OscCanOpenEdsInfoBlock.hpp"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw::tgl;
using namespace stw::scl;
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

   \param[in]      orc_File         Opened eds file to load from
   \param[in,out]  orc_LastError    Last error

   \return
   STW error codes

   \retval   C_NO_ERR   Values read
   \retval   C_CONFIG   At least one value not found, for details see error message
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_OscCanOpenEdsInfoBlock::LoadFromFile(C_SclIniFile & orc_File, C_SclString & orc_LastError)
{
   int32_t s32_Retval;

   orc_File.GetFileAsStringList(this->c_FileContentForSave);
   this->m_InitHash();
   s32_Retval = this->c_FileInfo.LoadFromIni(orc_File, orc_LastError);
   if (s32_Retval == C_NO_ERR)
   {
      s32_Retval = this->c_DeviceInfo.LoadFromIni(orc_File, orc_LastError);
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

   for (int32_t s32_Line = 0; s32_Line < this->c_FileContentForSave.Strings.GetLength(); s32_Line++)
   {
      stw::scl::C_SclChecksums::CalcCRC32(this->c_FileContentForSave.Strings[s32_Line].c_str(),
                                          this->c_FileContentForSave.Strings[s32_Line].Length(),
                                          this->u32_FileHash);
   }
}
