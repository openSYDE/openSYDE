//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Data class for data pool list UI part (implementation)

   Data class for data pool list UI part

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.hpp"

#include "C_PuiSdNodeDataPoolList.hpp"

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
C_PuiSdNodeDataPoolList::C_PuiSdNodeDataPoolList(void)
{
   c_DataPoolListElements.resize(1);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Calculates the hash value over all data

   The hash value is a 32 bit CRC value.

   \param[in,out] oru32_HashValue    Hash value with init [in] value and result [out] value
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PuiSdNodeDataPoolList::CalcHash(uint32_t & oru32_HashValue) const
{
   uint32_t u32_Counter;

   for (u32_Counter = 0U; u32_Counter < this->c_DataPoolListElements.size(); ++u32_Counter)
   {
      this->c_DataPoolListElements[u32_Counter].CalcHash(oru32_HashValue);
   }
}
