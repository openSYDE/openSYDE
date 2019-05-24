//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Data class for data set related, additional information (implementation)

   Data class for data set related, additional information

   \copyright   Copyright 2016 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------- */
#include "precomp_headers.h"

#include "C_OSCNodeDataPoolDataSet.h"

#include "CSCLChecksums.h"

/* -- Used Namespaces ------------------------------------------------------ */
using namespace stw_types;
using namespace stw_opensyde_core;

/* -- Module Global Constants ---------------------------------------------- */

/* -- Types ---------------------------------------------------------------- */

/* -- Global Variables ----------------------------------------------------- */

/* -- Module Global Variables ---------------------------------------------- */

/* -- Module Global Function Prototypes ------------------------------------ */

/* -- Implementation ------------------------------------------------------- */

//-----------------------------------------------------------------------------
/*! \brief   Default constructor
*/
//-----------------------------------------------------------------------------
C_OSCNodeDataPoolDataSet::C_OSCNodeDataPoolDataSet(void) :
   c_Name("DefaultDataSetName"),
   c_Comment("")
{
}

//-----------------------------------------------------------------------------
/*! \brief   Calculates the hash value over all data

   The hash value is a 32 bit CRC value.

   \param[in,out] oru32_HashValue    Hash value with initial [in] value and result [out] value
*/
//-----------------------------------------------------------------------------
void C_OSCNodeDataPoolDataSet::CalcHash(uint32 & oru32_HashValue) const
{
   stw_scl::C_SCLChecksums::CalcCRC32(this->c_Name.c_str(), this->c_Name.Length(), oru32_HashValue);
   stw_scl::C_SCLChecksums::CalcCRC32(this->c_Comment.c_str(), this->c_Comment.Length(), oru32_HashValue);
}
