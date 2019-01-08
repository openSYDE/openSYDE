//-----------------------------------------------------------------------------
/*!
   \internal
   \file
   \brief       short description (implementation)

   detailed description

   \implementation
   project     openSYDE
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     11.09.2017  STW/B.Bayer
   \endimplementation
*/
//-----------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------- */
#include "precomp_headers.h"

#include "stwtypes.h"
#include "CSCLChecksums.h"
#include "C_PuiSvDbNodeDataElementConfig.h"

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

   \created     11.09.2017  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
C_PuiSvDbNodeDataElementConfig::C_PuiSvDbNodeDataElementConfig(void) :
   c_ElementId(),
   c_ElementScaling(),
   c_DisplayName("")
{
}

//-----------------------------------------------------------------------------
/*!
   \brief   Calculates the hash value over all data

   The hash value is a 32 bit CRC value.

   \param[in,out] oru32_HashValue    Hash value with init [in] value and result [out] value

   \created     24.08.2017  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
void C_PuiSvDbNodeDataElementConfig::CalcHash(uint32 & oru32_HashValue) const
{
   this->c_ElementId.CalcHash(oru32_HashValue);
   this->c_ElementScaling.CalcHash(oru32_HashValue);
   stw_scl::C_SCLChecksums::CalcCRC32(this->c_DisplayName.toStdString().c_str(),
                                      this->c_DisplayName.length(), oru32_HashValue);
}
