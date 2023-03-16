//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Logging data structure (implementation)

   Logging data structure containing e.g. directory, file name and flags.

   \copyright   Copyright 2019 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.hpp"

#include "C_CamProLoggingData.hpp"
#include "C_SclChecksums.hpp"
#include "C_Uti.hpp"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw::opensyde_gui_logic;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

/* -- Global Variables ---------------------------------------------------------------------------------------------- */

/* -- Module Global Variables --------------------------------------------------------------------------------------- */

/* -- Module Global Function Prototypes ----------------------------------------------------------------------------- */

/* -- Implementation ------------------------------------------------------------------------------------------------ */

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default constructor.
*/
//----------------------------------------------------------------------------------------------------------------------
C_CamProLoggingData::C_CamProLoggingData(void) :
   c_Directory(""),
   c_FileName(""),
   q_Enabled(false),
   e_OverwriteMode(eADD_TIMESTAMP),
   e_FileFormat(eASC)
{
   this->Clear();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Calculates the hash value over all data

   The hash value is a 32 bit CRC value.

   \param[in,out] oru32_HashValue    Hash value with init [in] value and result [out] value.
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CamProLoggingData::CalcHash(uint32_t & oru32_HashValue) const
{
   stw::scl::C_SclChecksums::CalcCRC32(this->c_FileName.toStdString().c_str(), this->c_FileName.length(),
                                       oru32_HashValue);
   stw::scl::C_SclChecksums::CalcCRC32(this->c_Directory.toStdString().c_str(), this->c_Directory.length(),
                                       oru32_HashValue);
   stw::scl::C_SclChecksums::CalcCRC32(&this->q_Enabled, sizeof(this->q_Enabled), oru32_HashValue);
   stw::scl::C_SclChecksums::CalcCRC32(&this->e_OverwriteMode, sizeof(this->e_OverwriteMode), oru32_HashValue);
   stw::scl::C_SclChecksums::CalcCRC32(&this->e_FileFormat, sizeof(this->e_FileFormat), oru32_HashValue);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Reset all member variables.
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CamProLoggingData::Clear(void)
{
   c_Directory = "";
   c_FileName = "CANlog";
   q_Enabled = false;
   e_OverwriteMode = eADD_TIMESTAMP;
   e_FileFormat = eASC;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Convert enum to string.

   \param[in]     oe_Format        format type

   \return
   format as string
*/
//----------------------------------------------------------------------------------------------------------------------
QString C_CamProLoggingData::h_FormatEnumToString(const C_CamProLoggingData::E_Format oe_Format)
{
   QString c_Return;

   if (oe_Format == eBLF)
   {
      c_Return = "blf";
   }
   else
   {
      c_Return = "asc";
   }

   return c_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Convert string to enum.

   \param[in]     orc_Format        format string

   \return
   format as enum
*/
//----------------------------------------------------------------------------------------------------------------------
C_CamProLoggingData::E_Format C_CamProLoggingData::h_FormatStringToEnum(const QString & orc_Format)
{
   E_Format e_Return;

   if (orc_Format == "blf")
   {
      e_Return = eBLF;
   }
   else
   {
      e_Return = eASC;
   }

   return e_Return;
}
