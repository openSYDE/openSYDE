//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Node update information for parameter sets (implementation)

   Node update information for parameter sets

   \copyright   Copyright 2019 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.h"

#include "stwtypes.h"
#include "CSCLString.h"
#include "CSCLChecksums.h"
#include "C_PuiSvNodeUpdateParamInfo.h"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw_types;
using namespace stw_opensyde_gui_logic;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

/* -- Global Variables ---------------------------------------------------------------------------------------------- */

/* -- Module Global Variables --------------------------------------------------------------------------------------- */

/* -- Module Global Function Prototypes ----------------------------------------------------------------------------- */

/* -- Implementation ------------------------------------------------------------------------------------------------ */

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default constructor
*/
//----------------------------------------------------------------------------------------------------------------------
C_PuiSvNodeUpdateParamInfo::C_PuiSvNodeUpdateParamInfo(void) :
   mc_FilePath(""),
   mu32_LastKnownCrc(0UL)
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Calculates the hash value over all data

   The hash value is a 32 bit CRC value.

   \param[in,out] oru32_HashValue Hash value with init [in] value and result [out] value
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PuiSvNodeUpdateParamInfo::CalcHash(uint32 & oru32_HashValue) const
{
   const stw_scl::C_SCLString c_Data = this->mc_FilePath.toStdString().c_str();

   stw_scl::C_SCLChecksums::CalcCRC32(&this->mu32_LastKnownCrc, sizeof(this->mu32_LastKnownCrc), oru32_HashValue);
   stw_scl::C_SCLChecksums::CalcCRC32(c_Data.c_str(), c_Data.Length(), oru32_HashValue);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set file information

   \param[in] orc_FilePath      File path
   \param[in] ou32_LastKnownCrc Last known CRC value
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PuiSvNodeUpdateParamInfo::SetContent(const QString & orc_FilePath, const uint32 ou32_LastKnownCrc)
{
   this->mc_FilePath = orc_FilePath;
   this->mu32_LastKnownCrc = ou32_LastKnownCrc;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get current path

   \return
   Current path
*/
//----------------------------------------------------------------------------------------------------------------------
const QString & C_PuiSvNodeUpdateParamInfo::GetPath(void) const
{
   return this->mc_FilePath;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get last known CRC value

   \return
   Last known CRC value
*/
//----------------------------------------------------------------------------------------------------------------------
uint32 C_PuiSvNodeUpdateParamInfo::GetLastKnownCrc(void) const
{
   return this->mu32_LastKnownCrc;
}
