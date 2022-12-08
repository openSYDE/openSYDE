//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Data class for openSYDE server specific settings(implementation)

   Data class for openSYDE server specific settings

   \copyright   Copyright 2016 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.hpp"

#include "C_OscNodeOpenSydeServerSettings.hpp"

#include "C_SclChecksums.hpp"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */

using namespace stw::opensyde_core;

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
C_OscNodeOpenSydeServerSettings::C_OscNodeOpenSydeServerSettings(void)
{
   this->Initialize();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Initialize class content

   Clean up.
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OscNodeOpenSydeServerSettings::Initialize(void)
{
   u8_MaxClients = 1U;
   u8_MaxParallelTransmissions = 64U;
   s16_DpdDataBlockIndex = -1;
   u16_MaxMessageBufferTx = 585U;
   u16_MaxRoutingMessageBufferRx = 585U;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Calculates the hash value over all data

   The hash value is a 32 bit CRC value.

   \param[in,out] oru32_HashValue    Hash value with initial [in] value and result [out] value
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OscNodeOpenSydeServerSettings::CalcHash(uint32_t & oru32_HashValue) const
{
   stw::scl::C_SclChecksums::CalcCRC32(&this->u8_MaxClients, sizeof(this->u8_MaxClients), oru32_HashValue);
   stw::scl::C_SclChecksums::CalcCRC32(&this->u8_MaxParallelTransmissions, sizeof(this->u8_MaxParallelTransmissions),
                                       oru32_HashValue);
   stw::scl::C_SclChecksums::CalcCRC32(&this->s16_DpdDataBlockIndex, sizeof(this->s16_DpdDataBlockIndex),
                                       oru32_HashValue);
   stw::scl::C_SclChecksums::CalcCRC32(&this->u16_MaxMessageBufferTx, sizeof(this->u16_MaxMessageBufferTx),
                                       oru32_HashValue);
   stw::scl::C_SclChecksums::CalcCRC32(&this->u16_MaxRoutingMessageBufferRx,
                                       sizeof(this->u16_MaxRoutingMessageBufferRx),
                                       oru32_HashValue);
}
