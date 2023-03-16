//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       UI class for dashboard label data (implementation)

   UI class for dashboard label data

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.hpp"

#include "C_SclChecksums.hpp"
#include "C_PuiSvDbLabel.hpp"

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
C_PuiSvDbLabel::C_PuiSvDbLabel(void) :
   C_PuiSvDbWidgetBase(),
   c_Caption("VALUE-LABEL"),
   e_Type(eDEFAULT),
   q_ShowCaption(true),
   q_ShowUnit(true)
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Calculates the hash value over all data

   The hash value is a 32 bit CRC value.

   \param[in,out] oru32_HashValue    Hash value with init [in] value and result [out] value
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PuiSvDbLabel::CalcHash(uint32_t & oru32_HashValue) const
{
   stw::scl::C_SclChecksums::CalcCRC32(this->c_Caption.toStdString().c_str(), this->c_Caption.length(),
                                       oru32_HashValue);
   stw::scl::C_SclChecksums::CalcCRC32(&this->e_Type, sizeof(this->e_Type), oru32_HashValue);
   stw::scl::C_SclChecksums::CalcCRC32(&this->q_ShowCaption, sizeof(this->q_ShowCaption), oru32_HashValue);
   stw::scl::C_SclChecksums::CalcCRC32(&this->q_ShowUnit, sizeof(this->q_ShowUnit), oru32_HashValue);

   C_PuiSvDbWidgetBase::CalcHash(oru32_HashValue);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Check if item is a read element

   \return
   True  Read element
   False Write element
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_PuiSvDbLabel::IsReadElement(void) const
{
   return true;
}
