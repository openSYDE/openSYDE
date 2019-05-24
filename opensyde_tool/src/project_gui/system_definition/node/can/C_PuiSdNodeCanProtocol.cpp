//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Data class for a node CAN communication via a specific CAN communication protocol UI part (implementation)

   Data class for a node CAN communication via a specific CAN communication protocol UI part

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.h"

#include "C_PuiSdNodeCanProtocol.h"

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
C_PuiSdNodeCanProtocol::C_PuiSdNodeCanProtocol(void)
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Calculates the hash value over all data

   The hash value is a 32 bit CRC value.

   \param[in,out] oru32_HashValue    Hash value with init [in] value and result [out] value
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PuiSdNodeCanProtocol::CalcHash(stw_types::uint32 & oru32_HashValue) const
{
   for (stw_types::uint32 u32_ItContainer = 0; u32_ItContainer < this->c_ComMessages.size(); ++u32_ItContainer)
   {
      const C_PuiSdNodeCanMessageContainer & rc_Container = this->c_ComMessages[u32_ItContainer];
      rc_Container.CalcHash(oru32_HashValue);
   }
}
