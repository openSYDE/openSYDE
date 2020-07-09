//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Data class for code export settings (implementation)

   \copyright   Copyright 2020 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.h"

#include "stwtypes.h"
#include "C_OSCNodeCodeExportSettings.h"

#include "CSCLChecksums.h"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw_types;
using namespace stw_opensyde_core;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

/* -- Global Variables ---------------------------------------------------------------------------------------------- */

/* -- Module Global Variables --------------------------------------------------------------------------------------- */

/* -- Module Global Function Prototypes ----------------------------------------------------------------------------- */

/* -- Implementation ------------------------------------------------------------------------------------------------ */

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Default constructor/destructor
*/
//----------------------------------------------------------------------------------------------------------------------
C_OSCNodeCodeExportSettings::C_OSCNodeCodeExportSettings(void)
{
   this->Initialize();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Initialize
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OSCNodeCodeExportSettings::Initialize()
{
   e_ScalingSupport = eFLOAT32;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Calculates the hash value over all data

   The hash value is a 32 bit CRC value.

   \param[in,out] oru32_HashValue    Hash value with initial [in] value and result [out] value
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OSCNodeCodeExportSettings::CalcHash(stw_types::uint32 & oru32_HashValue) const
{
   stw_scl::C_SCLChecksums::CalcCRC32(&this->e_ScalingSupport, sizeof(this->e_ScalingSupport), oru32_HashValue);
}
