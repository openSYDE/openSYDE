//-----------------------------------------------------------------------------
/*!
   \internal
   \file
   \brief       UI bus: stores additional UI information (implementation)

   UI bus: stores additional UI information.

   \implementation
   project     openSYDE
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     08.09.2016  STW/M.Echtler
   \endimplementation
*/
//-----------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------- */
#include "precomp_headers.h"

#include "C_PuiSdBus.h"

#include "CSCLChecksums.h"

/* -- Used Namespaces ------------------------------------------------------ */
using namespace stw_types;
using namespace stw_opensyde_gui_logic;

/* -- Module Global Constants ---------------------------------------------- */

/* -- Types ---------------------------------------------------------------- */

/* -- Global Variables ----------------------------------------------------- */

/* -- Module Global Variables ---------------------------------------------- */

/* -- Module Global Function Prototypes ------------------------------------ */

/* -- Implementation ------------------------------------------------------- */

//-----------------------------------------------------------------------------
/*!
   \brief   Default constructor

   Initialize default values

   \created     08.09.2016  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
C_PuiSdBus::C_PuiSdBus() :
   C_PuiBsLineBase()
{
   s32_UIWidthPixels = 6;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Calculates the hash value over all data

   The hash value is a 32 bit CRC value.

   \param[in,out] oru32_HashValue    Hash value with init [in] value and result [out] value

   \created     22.03.2017  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
void C_PuiSdBus::CalcHash(uint32 & oru32_HashValue) const
{
   sintn sn_Value;

   sn_Value = this->c_UIColorMiddleLine.red();
   stw_scl::C_SCLChecksums::CalcCRC32(&sn_Value, sizeof(sn_Value), oru32_HashValue);
   sn_Value = this->c_UIColorMiddleLine.green();
   stw_scl::C_SCLChecksums::CalcCRC32(&sn_Value, sizeof(sn_Value), oru32_HashValue);
   sn_Value = this->c_UIColorMiddleLine.blue();
   stw_scl::C_SCLChecksums::CalcCRC32(&sn_Value, sizeof(sn_Value), oru32_HashValue);
   sn_Value = this->c_UIColorMiddleLine.alpha();
   stw_scl::C_SCLChecksums::CalcCRC32(&sn_Value, sizeof(sn_Value), oru32_HashValue);

   C_PuiBsLineBase::CalcHash(oru32_HashValue);
}
