//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Data class for CAN communication message related, additional information UI part (implementation)

   Data class for CAN communication message related, additional information UI part

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.h"

#include "CSCLChecksums.h"
#include "C_PuiSdNodeCanMessage.h"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
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
C_PuiSdNodeCanMessage::C_PuiSdNodeCanMessage(void) :
   q_UseAutoReceiveTimeout(true)
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Calculates the hash value over all data

   The hash value is a 32 bit CRC value.

   \param[in,out] oru32_HashValue    Hash value with init [in] value and result [out] value
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PuiSdNodeCanMessage::CalcHash(stw_types::uint32 & oru32_HashValue) const
{
   stw_scl::C_SCLChecksums::CalcCRC32(&q_UseAutoReceiveTimeout, sizeof(q_UseAutoReceiveTimeout), oru32_HashValue);
}
