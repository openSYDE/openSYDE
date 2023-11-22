//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Base data class for write widgets

   Base data class for write widgets

   \copyright   Copyright 2023 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.hpp"

#include "stwtypes.hpp"
#include "C_SclChecksums.hpp"
#include "C_PuiSvDbWriteWidgetBase.hpp"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw::opensyde_gui_logic;

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
C_PuiSvDbWriteWidgetBase::C_PuiSvDbWriteWidgetBase() :
   C_PuiSvDbWidgetBase(),
   q_AutoWriteOnConnect(false),
   e_ElementWriteMode(eWM_MANUAL),
   e_InitialValueMode(eIVM_DISABLED)
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Calculates the hash value over all data

   The hash value is a 32 bit CRC value.

   \param[in,out]  oru32_HashValue  Hash value with init [in] value and result [out] value
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PuiSvDbWriteWidgetBase::CalcHash(uint32_t & oru32_HashValue) const
{
   stw::scl::C_SclChecksums::CalcCRC32(&this->q_AutoWriteOnConnect, sizeof(this->q_AutoWriteOnConnect),
                                       oru32_HashValue);
   stw::scl::C_SclChecksums::CalcCRC32(&this->e_ElementWriteMode, sizeof(this->e_ElementWriteMode), oru32_HashValue);
   stw::scl::C_SclChecksums::CalcCRC32(&this->e_InitialValueMode, sizeof(this->e_InitialValueMode), oru32_HashValue);

   this->c_InitialValue.CalcHash(oru32_HashValue);

   C_PuiSvDbWidgetBase::CalcHash(oru32_HashValue);
}
