//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Database data structure (implementation)

   Database data structure containing e.g. name and enabled flag.

   \copyright   Copyright 2018 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.hpp"

#include "C_CamProDatabaseData.hpp"
#include "C_SclChecksums.hpp"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw::opensyde_gui_logic;
using namespace stw::opensyde_core;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

/* -- Global Variables ---------------------------------------------------------------------------------------------- */

/* -- Module Global Variables --------------------------------------------------------------------------------------- */

/* -- Module Global Function Prototypes ----------------------------------------------------------------------------- */

/* -- Implementation ------------------------------------------------------------------------------------------------ */

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default constructor of Database class.
*/
//----------------------------------------------------------------------------------------------------------------------
C_CamProDatabaseData::C_CamProDatabaseData(void)
{
   this->c_Name = "DefaultDatabaseName";
   this->q_Enabled = true;
   this->s32_BusIndex = -1;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Calculates the hash value over all data

   The hash value is a 32 bit CRC value.

   \param[in,out] oru32_HashValue    Hash value with init [in] value and result [out] value
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CamProDatabaseData::CalcHash(uint32_t & oru32_HashValue) const
{
   // Database properties (name, state)
   stw::scl::C_SclChecksums::CalcCRC32(this->c_Name.toStdString().c_str(), this->c_Name.length(), oru32_HashValue);
   stw::scl::C_SclChecksums::CalcCRC32(&this->q_Enabled, sizeof(this->q_Enabled), oru32_HashValue);
   stw::scl::C_SclChecksums::CalcCRC32(&this->s32_BusIndex, sizeof(this->s32_BusIndex), oru32_HashValue);
}
