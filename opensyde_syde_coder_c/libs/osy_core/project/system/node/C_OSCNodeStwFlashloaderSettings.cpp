//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       short description (implementation)

   detailed description

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.h"

#include "C_OSCNodeStwFlashloaderSettings.h"

#include "CSCLChecksums.h"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw_types;
using namespace stw_opensyde_core;

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
C_OSCNodeStwFlashloaderSettings::C_OSCNodeStwFlashloaderSettings(void)
{
   this->Initialize();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Initialize class content

   Clean up.
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OSCNodeStwFlashloaderSettings::Initialize(void)
{
   q_ResetMessageActive = false;
   q_ResetMessageExtendedId = false;
   u32_ResetMessageId = 0U;
   u8_ResetMessageDlc = 8U;
   c_Data.resize(8, 0);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Calculates the hash value over all data

   The hash value is a 32 bit CRC value.

   \param[in,out] oru32_HashValue    Hash value with initial [in] value and result [out] value
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OSCNodeStwFlashloaderSettings::CalcHash(uint32 & oru32_HashValue) const
{
   stw_scl::C_SCLChecksums::CalcCRC32(&this->q_ResetMessageActive, sizeof(this->q_ResetMessageActive), oru32_HashValue);
   stw_scl::C_SCLChecksums::CalcCRC32(&this->q_ResetMessageExtendedId, sizeof(this->q_ResetMessageExtendedId),
                                      oru32_HashValue);
   stw_scl::C_SCLChecksums::CalcCRC32(&this->u32_ResetMessageId, sizeof(this->u32_ResetMessageId), oru32_HashValue);
   stw_scl::C_SCLChecksums::CalcCRC32(&this->u8_ResetMessageDlc, sizeof(this->u8_ResetMessageDlc), oru32_HashValue);
   stw_scl::C_SCLChecksums::CalcCRC32(&this->c_Data[0], static_cast<uint32>(this->c_Data.size()), oru32_HashValue);
}
