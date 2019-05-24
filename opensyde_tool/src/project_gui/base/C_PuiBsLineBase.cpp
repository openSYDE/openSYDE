//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       UI base class for segmented line based data: stores UI information (implementation)

   UI base class for segmented line based data: stores UI information

   \copyright   Copyright 2016 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.h"

#include "C_PuiBsLineBase.h"

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
*/
//----------------------------------------------------------------------------------------------------------------------
C_PuiBsLineBase::C_PuiBsLineBase(void) :
   c_UIColor(Qt::GlobalColor::cyan),
   s32_UIWidthPixels(3),
   f64_ZOrder(0.0)
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default destructor
*/
//----------------------------------------------------------------------------------------------------------------------
C_PuiBsLineBase::~C_PuiBsLineBase(void)
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Calculates the hash value over all data

   The hash value is a 32 bit CRC value.

   \param[in,out] oru32_HashValue    Hash value with init [in] value and result [out] value
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PuiBsLineBase::CalcHash(uint32 & oru32_HashValue) const
{
   sintn sn_Value;
   float64 f64_Value;
   uint32 u32_Counter;

   sn_Value = this->c_UIColor.red();
   stw_scl::C_SCLChecksums::CalcCRC32(&sn_Value, sizeof(sn_Value), oru32_HashValue);
   sn_Value = this->c_UIColor.green();
   stw_scl::C_SCLChecksums::CalcCRC32(&sn_Value, sizeof(sn_Value), oru32_HashValue);
   sn_Value = this->c_UIColor.blue();
   stw_scl::C_SCLChecksums::CalcCRC32(&sn_Value, sizeof(sn_Value), oru32_HashValue);
   sn_Value = this->c_UIColor.alpha();
   stw_scl::C_SCLChecksums::CalcCRC32(&sn_Value, sizeof(sn_Value), oru32_HashValue);

   stw_scl::C_SCLChecksums::CalcCRC32(&this->s32_UIWidthPixels, sizeof(this->s32_UIWidthPixels), oru32_HashValue);
   stw_scl::C_SCLChecksums::CalcCRC32(&this->f64_ZOrder, sizeof(this->f64_ZOrder), oru32_HashValue);

   for (u32_Counter = 0U; u32_Counter < this->c_UIInteractionPoints.size(); ++u32_Counter)
   {
      f64_Value = this->c_UIInteractionPoints[u32_Counter].x();
      stw_scl::C_SCLChecksums::CalcCRC32(&f64_Value, sizeof(f64_Value), oru32_HashValue);

      f64_Value = this->c_UIInteractionPoints[u32_Counter].y();
      stw_scl::C_SCLChecksums::CalcCRC32(&f64_Value, sizeof(f64_Value), oru32_HashValue);
   }
}
