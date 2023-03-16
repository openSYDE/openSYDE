//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       UI base class for segmented line based data: stores UI information (implementation)

   UI base class for segmented line based data: stores UI information

   \copyright   Copyright 2016 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.hpp"

#include "C_PuiBsLineBase.hpp"

#include "C_SclChecksums.hpp"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw::opensyde_gui_logic;

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
   c_UiColor(Qt::GlobalColor::cyan),
   s32_UiWidthPixels(3),
   f64_ZetOrder(0.0)
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
void C_PuiBsLineBase::CalcHash(uint32_t & oru32_HashValue) const
{
   int32_t s32_Value;
   float64_t f64_Value;
   uint32_t u32_Counter;

   s32_Value = this->c_UiColor.red();
   stw::scl::C_SclChecksums::CalcCRC32(&s32_Value, sizeof(s32_Value), oru32_HashValue);
   s32_Value = this->c_UiColor.green();
   stw::scl::C_SclChecksums::CalcCRC32(&s32_Value, sizeof(s32_Value), oru32_HashValue);
   s32_Value = this->c_UiColor.blue();
   stw::scl::C_SclChecksums::CalcCRC32(&s32_Value, sizeof(s32_Value), oru32_HashValue);
   s32_Value = this->c_UiColor.alpha();
   stw::scl::C_SclChecksums::CalcCRC32(&s32_Value, sizeof(s32_Value), oru32_HashValue);

   stw::scl::C_SclChecksums::CalcCRC32(&this->s32_UiWidthPixels, sizeof(this->s32_UiWidthPixels), oru32_HashValue);
   //lint -e{9110} //we do not really use the bit representation; we just assume it is "stable" for this type
   stw::scl::C_SclChecksums::CalcCRC32(&this->f64_ZetOrder, sizeof(this->f64_ZetOrder), oru32_HashValue);

   for (u32_Counter = 0U; u32_Counter < this->c_UiInteractionPoints.size(); ++u32_Counter)
   {
      f64_Value = this->c_UiInteractionPoints[u32_Counter].x();
      //lint -e{9110} //we do not really use the bit representation; we just assume it is "stable" for this type
      stw::scl::C_SclChecksums::CalcCRC32(&f64_Value, sizeof(f64_Value), oru32_HashValue);

      f64_Value = this->c_UiInteractionPoints[u32_Counter].y();
      //lint -e{9110} //we do not really use the bit representation; we just assume it is "stable" for this type
      stw::scl::C_SclChecksums::CalcCRC32(&f64_Value, sizeof(f64_Value), oru32_HashValue);
   }
}
