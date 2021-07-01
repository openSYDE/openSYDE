//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       ID to allow reidentification of HAL elements

   ID to allow reidentification of HAL elements

   \copyright   Copyright 2021 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.h"

#include "CSCLChecksums.h"
#include "C_PuiSvLastKnownHalElementId.h"

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
/*! \brief  Default constructor
*/
//----------------------------------------------------------------------------------------------------------------------
C_PuiSvLastKnownHalElementId::C_PuiSvLastKnownHalElementId() :
   u32_Crc(0UL)
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Default constructor

   \param[in]  ou32_Crc       Crc
   \param[in]  orc_HalDpName  Hal datapool name
*/
//----------------------------------------------------------------------------------------------------------------------
C_PuiSvLastKnownHalElementId::C_PuiSvLastKnownHalElementId(const uint32 ou32_Crc, const QString & orc_HalDpName)
{
   this->u32_Crc = ou32_Crc;
   this->c_HalDpName = orc_HalDpName;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Default destructor
*/
//----------------------------------------------------------------------------------------------------------------------
C_PuiSvLastKnownHalElementId::~C_PuiSvLastKnownHalElementId()
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Calculates the hash value over all data

   The hash value is a 32 bit CRC value.

   \param[in,out]  oru32_HashValue  Hash value with init [in] value and result [out] value
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PuiSvLastKnownHalElementId::CalcHash(uint32 & oru32_HashValue) const
{
   stw_scl::C_SCLChecksums::CalcCRC32(&this->u32_Crc, sizeof(this->u32_Crc), oru32_HashValue);
   stw_scl::C_SCLChecksums::CalcCRC32(this->c_HalDpName.toStdString().c_str(),
                                      this->c_HalDpName.length(), oru32_HashValue);
}
