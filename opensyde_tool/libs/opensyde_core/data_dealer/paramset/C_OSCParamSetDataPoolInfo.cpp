//-----------------------------------------------------------------------------
/*!
   \internal
   \file
   \brief       Data class, containing information about the data pools in a parameter set file (implementation)

   Data class, containing information about the data pools in a parameter set file

   \implementation
   project     openSYDE
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     17.10.2017  STW/M.Echtler
   \endimplementation
*/
//-----------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------- */
#include "precomp_headers.h"

#include "C_OSCParamSetDataPoolInfo.h"

/* -- Used Namespaces ------------------------------------------------------ */
using namespace stw_opensyde_core;

/* -- Module Global Constants ---------------------------------------------- */

/* -- Types ---------------------------------------------------------------- */

/* -- Global Variables ----------------------------------------------------- */

/* -- Module Global Variables ---------------------------------------------- */

/* -- Module Global Function Prototypes ------------------------------------ */

/* -- Implementation ------------------------------------------------------- */

//-----------------------------------------------------------------------------
/*!
   \brief   Default constructor

   \created     17.10.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
C_OSCParamSetDataPoolInfo::C_OSCParamSetDataPoolInfo(void) :
   u32_DataPoolCrc(0)
{
   au8_Version[0] = 0;
   au8_Version[1] = 0;
   au8_Version[2] = 0;
}
