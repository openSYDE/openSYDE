//-----------------------------------------------------------------------------
/*!
   \internal
   \file
   \brief       UI class for dashboard progress bar data (implementation)

   UI class for dashboard progress bar data

   \implementation
   project     openSYDE
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     21.08.2017  STW/M.Echtler
   \endimplementation
*/
//-----------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------- */
#include "precomp_headers.h"

#include "CSCLChecksums.h"
#include "C_PuiSvDbProgressBar.h"

/* -- Used Namespaces ------------------------------------------------------ */
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

   \created     21.08.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
C_PuiSvDbProgressBar::C_PuiSvDbProgressBar() :
   C_PuiSvDbWidgetBase(),
   e_Type(eTYPE_1),
   e_Alignment(eTOP),
   q_ShowMinMax(true),
   q_ShowUnit(false)
{
}

//-----------------------------------------------------------------------------
/*!
   \brief   Calculates the hash value over all data

   The hash value is a 32 bit CRC value.

   \param[in,out] oru32_HashValue    Hash value with init [in] value and result [out] value

   \created     29.01.2018  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_PuiSvDbProgressBar::CalcHash(stw_types::uint32 & oru32_HashValue) const
{
   stw_scl::C_SCLChecksums::CalcCRC32(&this->e_Type, sizeof(this->e_Type), oru32_HashValue);
   stw_scl::C_SCLChecksums::CalcCRC32(&this->e_Alignment, sizeof(this->e_Alignment), oru32_HashValue);
   stw_scl::C_SCLChecksums::CalcCRC32(&this->q_ShowMinMax, sizeof(this->q_ShowMinMax), oru32_HashValue);
   stw_scl::C_SCLChecksums::CalcCRC32(&this->q_ShowUnit, sizeof(this->q_ShowUnit), oru32_HashValue);
   C_PuiSvDbWidgetBase::CalcHash(oru32_HashValue);
}

//-----------------------------------------------------------------------------
/*!
   \brief   Check if item is a read element

   \return
   True  Read element
   False Write element

   \created     27.09.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
bool C_PuiSvDbProgressBar::IsReadElement(void) const
{
   return true;
}
