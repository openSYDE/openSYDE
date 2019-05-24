//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       UI bus: stores additional UI information (implementation)

   UI bus: stores additional UI information.

   \copyright   Copyright 2016 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.h"

#include "C_PuiSdBus.h"

#include "CSCLChecksums.h"

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

   Initialize default values
*/
//----------------------------------------------------------------------------------------------------------------------
C_PuiSdBus::C_PuiSdBus() :
   C_PuiBsLineBase()
{
   s32_UIWidthPixels = 6;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Calculates the hash value over all data

   The hash value is a 32 bit CRC value.

   \param[in,out] oru32_HashValue    Hash value with init [in] value and result [out] value
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PuiSdBus::CalcHash(uint32 & oru32_HashValue) const
{
   sintn sn_Value;

   sn_Value = this->c_UIColorMiddleLine.red();
   stw_scl::C_SCLChecksums::CalcCRC32(&sn_Value, sizeof(sn_Value), oru32_HashValue);
   sn_Value = this->c_UIColorMiddleLine.green();
   stw_scl::C_SCLChecksums::CalcCRC32(&sn_Value, sizeof(sn_Value), oru32_HashValue);
   sn_Value = this->c_UIColorMiddleLine.blue();
   stw_scl::C_SCLChecksums::CalcCRC32(&sn_Value, sizeof(sn_Value), oru32_HashValue);
   sn_Value = this->c_UIColorMiddleLine.alpha();
   stw_scl::C_SCLChecksums::CalcCRC32(&sn_Value, sizeof(sn_Value), oru32_HashValue);

   C_PuiBsLineBase::CalcHash(oru32_HashValue);
}
