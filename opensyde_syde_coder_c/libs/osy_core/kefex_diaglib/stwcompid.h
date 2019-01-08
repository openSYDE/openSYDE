//-----------------------------------------------------------------------------
/*!
   \file        stwcompid.h
   \brief       Utility functions concerning the STW company-ID

   This module provides functionality to convert the STW company-ID
    between ASCII and binary representation.
   It is written in plain ANSI-C and can thus be used in most C / C++
    projects.

   \implementation
   project
   copyright   STW (c) 1999-200x
   license     use only under terms of contract / confidential

   created     03.11.2008  STW/A.Stangl
   \endimplementation

   \internal
   \history
   Date(dd.mm.yyyy)  Author        Description
   12.11.2008        STW/A.Stangl  added conditional package definitions for Borland C++ Builder
   03.11.2008        STW/A.Stangl  file created
   \endhistory
*/
//-----------------------------------------------------------------------------
#ifndef STWCOMPIDH
#define STWCOMPIDH

/* -- Includes ------------------------------------------------------------ */
#include "stwtypes.h"

#ifdef __cplusplus

namespace stw_company_id
{

/* -- Defines ------------------------------------------------------------- */
//maybe this will be a part of a Borland library:
#ifndef STWCOMPID_PACKAGE
#ifdef __BORLANDC__
#define STWCOMPID_PACKAGE __declspec(package)
#else
#define STWCOMPID_PACKAGE
#endif
#endif

/* -- Types --------------------------------------------------------------- */

/* -- Global Variables ---------------------------------------------------- */

/* -- Function Prototypes ------------------------------------------------- */
extern "C"
{

extern stw_types::sintn STWCOMPID_PACKAGE CID_string_to_bytes(const stw_types::charn * const opcn_String,
                                                              stw_types::uint8 * const opu8_Bytes,
                                                              stw_types::uint8 * const opu8_NumBytes);
extern stw_types::sintn STWCOMPID_PACKAGE CID_bytes_to_string(const stw_types::uint8 * const opu8_Bytes,
                                                              const stw_types::uint8 ou8_NumBytes,
                                                              stw_types::charn * const opcn_String);
}

}
#else
//plain C
//We could also achieve this via some typedefs however this would be harder to read and maintain.
extern sintn CID_string_to_bytes(const charn * const opcn_String, uint8 * const opu8_Bytes,
                                 uint8 * const opu8_NumBytes);
extern sintn CID_bytes_to_string(const uint8 * const opu8_Bytes, const uint8 ou8_NumBytes,
                                 charn * const opcn_String);
#endif

/* -- Implementation ------------------------------------------------------ */

#endif
