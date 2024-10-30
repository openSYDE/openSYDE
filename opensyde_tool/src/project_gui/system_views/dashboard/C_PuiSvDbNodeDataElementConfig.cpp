//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       short description (implementation)

   detailed description

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.hpp"

#include "stwtypes.hpp"
#include "C_SclChecksums.hpp"
#include "C_PuiSvDbNodeDataElementConfig.hpp"

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
C_PuiSvDbNodeDataElementConfig::C_PuiSvDbNodeDataElementConfig(void) :
   c_ElementId(),
   c_ElementScaling(),
   c_DisplayFormatter(),
   c_DisplayName("")
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   destructor

   clean up ...
*/
//----------------------------------------------------------------------------------------------------------------------
C_PuiSvDbNodeDataElementConfig::~C_PuiSvDbNodeDataElementConfig(void) noexcept
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Calculates the hash value over all data

   The hash value is a 32 bit CRC value.

   \param[in,out]  oru32_HashValue  Hash value with init [in] value and result [out] value
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PuiSvDbNodeDataElementConfig::CalcHash(uint32_t & oru32_HashValue) const
{
   this->c_ElementId.CalcHash(oru32_HashValue);
   this->c_ElementScaling.CalcHash(oru32_HashValue);
   this->c_DisplayFormatter.CalcHash(oru32_HashValue);
   stw::scl::C_SclChecksums::CalcCRC32(this->c_DisplayName.toStdString().c_str(),
                                       this->c_DisplayName.length(), oru32_HashValue);
}
