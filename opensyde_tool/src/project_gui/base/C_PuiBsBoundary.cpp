//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       UI boundary data: stores UI information (implementation)

   UI boundary data: stores UI information

   \copyright   Copyright 2016 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.h"

#include "constants.h"
#include "C_PuiBsBoundary.h"

#include "CSCLChecksums.h"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw_types;
using namespace stw_opensyde_gui;
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
C_PuiBsBoundary::C_PuiBsBoundary() :
   C_PuiBsBox(),
   c_UIBorderColorBright(mc_STYLE_GUIDE_COLOR_7),
   c_UIBorderColorDark(mc_STYLE_GUIDE_COLOR_2),
   c_UIBackgroundColorBright(mc_STYLE_GUIDE_COLOR_12),
   c_UIBackgroundColorDark(mc_STYLE_GUIDE_COLOR_36),
   s32_UIBorderWidth(1)
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Calculates the hash value over all data

   The hash value is a 32 bit CRC value.

   \param[in,out] oru32_HashValue    Hash value with init [in] value and result [out] value
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PuiBsBoundary::CalcHash(uint32 & oru32_HashValue) const
{
   sintn sn_Value;

   sn_Value = this->c_UIBorderColorBright.red();
   stw_scl::C_SCLChecksums::CalcCRC32(&sn_Value, sizeof(sn_Value), oru32_HashValue);
   sn_Value = this->c_UIBorderColorBright.green();
   stw_scl::C_SCLChecksums::CalcCRC32(&sn_Value, sizeof(sn_Value), oru32_HashValue);
   sn_Value = this->c_UIBorderColorBright.blue();
   stw_scl::C_SCLChecksums::CalcCRC32(&sn_Value, sizeof(sn_Value), oru32_HashValue);
   sn_Value = this->c_UIBorderColorBright.alpha();
   stw_scl::C_SCLChecksums::CalcCRC32(&sn_Value, sizeof(sn_Value), oru32_HashValue);

   sn_Value = this->c_UIBorderColorDark.red();
   stw_scl::C_SCLChecksums::CalcCRC32(&sn_Value, sizeof(sn_Value), oru32_HashValue);
   sn_Value = this->c_UIBorderColorDark.green();
   stw_scl::C_SCLChecksums::CalcCRC32(&sn_Value, sizeof(sn_Value), oru32_HashValue);
   sn_Value = this->c_UIBorderColorDark.blue();
   stw_scl::C_SCLChecksums::CalcCRC32(&sn_Value, sizeof(sn_Value), oru32_HashValue);
   sn_Value = this->c_UIBorderColorDark.alpha();
   stw_scl::C_SCLChecksums::CalcCRC32(&sn_Value, sizeof(sn_Value), oru32_HashValue);

   sn_Value = this->c_UIBackgroundColorBright.red();
   stw_scl::C_SCLChecksums::CalcCRC32(&sn_Value, sizeof(sn_Value), oru32_HashValue);
   sn_Value = this->c_UIBackgroundColorBright.green();
   stw_scl::C_SCLChecksums::CalcCRC32(&sn_Value, sizeof(sn_Value), oru32_HashValue);
   sn_Value = this->c_UIBackgroundColorBright.blue();
   stw_scl::C_SCLChecksums::CalcCRC32(&sn_Value, sizeof(sn_Value), oru32_HashValue);
   sn_Value = this->c_UIBackgroundColorBright.alpha();
   stw_scl::C_SCLChecksums::CalcCRC32(&sn_Value, sizeof(sn_Value), oru32_HashValue);

   sn_Value = this->c_UIBackgroundColorDark.red();
   stw_scl::C_SCLChecksums::CalcCRC32(&sn_Value, sizeof(sn_Value), oru32_HashValue);
   sn_Value = this->c_UIBackgroundColorDark.green();
   stw_scl::C_SCLChecksums::CalcCRC32(&sn_Value, sizeof(sn_Value), oru32_HashValue);
   sn_Value = this->c_UIBackgroundColorDark.blue();
   stw_scl::C_SCLChecksums::CalcCRC32(&sn_Value, sizeof(sn_Value), oru32_HashValue);
   sn_Value = this->c_UIBackgroundColorDark.alpha();
   stw_scl::C_SCLChecksums::CalcCRC32(&sn_Value, sizeof(sn_Value), oru32_HashValue);

   stw_scl::C_SCLChecksums::CalcCRC32(&this->s32_UIBorderWidth, sizeof(this->s32_UIBorderWidth), oru32_HashValue);

   C_PuiBsBox::CalcHash(oru32_HashValue);
}
