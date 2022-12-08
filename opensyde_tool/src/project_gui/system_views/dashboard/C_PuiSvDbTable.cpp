//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       UI class for dashboard table data (implementation)

   UI class for dashboard table data

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.hpp"

#include "C_SclChecksums.hpp"
#include "C_PuiSvDbTable.hpp"

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
C_PuiSvDbTable::C_PuiSvDbTable(void) :
   C_PuiSvDbWidgetBase()
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Calculates the hash value over all data

   The hash value is a 32 bit CRC value.

   \param[in,out] oru32_HashValue    Hash value with init [in] value and result [out] value
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PuiSvDbTable::CalcHash(uint32_t & oru32_HashValue) const
{
   for (uint32_t u32_ItCol = 0; u32_ItCol < this->c_ColumnWidth.size(); ++u32_ItCol)
   {
      const int32_t & rs32_ColWidth = this->c_ColumnWidth[u32_ItCol];
      stw::scl::C_SclChecksums::CalcCRC32(&rs32_ColWidth, sizeof(rs32_ColWidth), oru32_HashValue);
   }
   C_PuiSvDbWidgetBase::CalcHash(oru32_HashValue);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Check if item is a read element

   \return
   True  Read element
   False Write element
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_PuiSvDbTable::IsReadElement(void) const
{
   return true;
}
