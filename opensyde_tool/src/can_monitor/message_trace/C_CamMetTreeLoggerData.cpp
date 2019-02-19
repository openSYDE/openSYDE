//-----------------------------------------------------------------------------
/*!
   \internal
   \file
   \brief       Adaption of data class for logging CAN messages for message trace tree view (implementation)

   \implementation
   project     openSYDE
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     13.12.2018  STW/B.Bayer
   \endimplementation
*/
//-----------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------- */
#include "precomp_headers.h"

#include "C_CamMetTreeLoggerData.h"

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

   \created     03.09.2018  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
C_CamMetTreeLoggerData::C_CamMetTreeLoggerData(void) :
   C_OSCComMessageLoggerData(),
   sn_GrayOutValueMsg(0),
   sn_GrayOutValueData(0),
   u32_DataChangedTimeStamp(0U)
{
   // Handling of maximum 8 data bytes
   c_GrayOutValueDataBytes.resize(8);
   c_DataBytesChangedTimeStamps.resize(8);
}
