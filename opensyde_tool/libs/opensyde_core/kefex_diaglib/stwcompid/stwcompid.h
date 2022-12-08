//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Utility functions concerning the STW company-ID

   This module provides functionality to convert the STW company-ID
    between ASCII and binary representation.
   It is written in plain ANSI-C and can thus be used in most C / C++
    projects.

   \copyright   Copyright 2008 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef STWCOMPIDH
#define STWCOMPIDH

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "stwtypes.h"

/* -- Types --------------------------------------------------------------------------------------------------------- */

/* -- Global Variables ---------------------------------------------------------------------------------------------- */

/* -- Function Prototypes ------------------------------------------------------------------------------------------- */
#ifdef __cplusplus
extern "C" {
#endif
extern sintn CID_string_to_bytes(const charn * const opcn_String, uint8 * const opu8_Bytes,
                                 uint8 * const opu8_NumBytes);
extern sintn CID_bytes_to_string(const uint8 * const opu8_Bytes, const uint8 ou8_NumBytes, charn * const opcn_String);
#ifdef __cplusplus
}
#endif

/* -- Implementation ------------------------------------------------------------------------------------------------ */

#endif
