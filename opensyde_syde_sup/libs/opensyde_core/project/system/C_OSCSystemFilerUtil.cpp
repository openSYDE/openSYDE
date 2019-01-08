//-----------------------------------------------------------------------------
/*!
   \internal
   \file
   \brief       Utility functions for system filers (implementation)

   Utility functions for system filers

   \implementation
   project     openSYDE
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     10.01.2017  STW/M.Echtler
   \endimplementation
*/
//-----------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------- */
#include "precomp_headers.h"

#include "stwtypes.h"
#include "stwerrors.h"
#include "C_OSCSystemFilerUtil.h"
#include "C_OSCLoggingHandler.h"

/* -- Used Namespaces ------------------------------------------------------ */
using namespace stw_types;
using namespace stw_opensyde_core;
using namespace stw_scl;
using namespace stw_errors;

/* -- Module Global Constants ---------------------------------------------- */

/* -- Types ---------------------------------------------------------------- */

/* -- Global Variables ----------------------------------------------------- */

/* -- Module Global Variables ---------------------------------------------- */

/* -- Module Global Function Prototypes ------------------------------------ */

/* -- Implementation ------------------------------------------------------- */

//-----------------------------------------------------------------------------
/*!
   \brief   Default constructor

   \created     10.01.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
C_OSCSystemFilerUtil::C_OSCSystemFilerUtil()
{
}

//-----------------------------------------------------------------------------
/*!
   \brief   Convert bus type enum to string

   \param[in]   oe_Type   Bus type

   \return  string representation of oe_Type

   \created     06.09.2016  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
C_SCLString C_OSCSystemFilerUtil::mh_BusTypeEnumToString(const C_OSCSystemBus::E_Type oe_Type)
{
   C_SCLString c_Retval;

   if (oe_Type == C_OSCSystemBus::eETHERNET)
   {
      c_Retval = "ethernet";
   }
   else
   {
      c_Retval = "can";
   }
   return c_Retval;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Convert bus type string to enum

   \param[in]  orc_Type   type string
   \param[out] ore_Type   Enum corresponding to type

   \return
   C_NO_ERR   no error
   C_RANGE    String unknown

   \created     06.09.2016  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
sint32 C_OSCSystemFilerUtil::mh_BusTypeStringToEnum(const stw_scl::C_SCLString & orc_Type,
                                                    C_OSCSystemBus::E_Type & ore_Type)
{
   sint32 s32_Retval = C_NO_ERR;

   if (orc_Type == "ethernet")
   {
      ore_Type = C_OSCSystemBus::eETHERNET;
   }
   else if (orc_Type == "can")
   {
      ore_Type = C_OSCSystemBus::eCAN;
   }
   else
   {
      osc_write_log_error("Loading System Definition", "Invalid value for bus.\"type\":" + orc_Type);
      s32_Retval = C_RANGE;
   }
   return s32_Retval;
}
