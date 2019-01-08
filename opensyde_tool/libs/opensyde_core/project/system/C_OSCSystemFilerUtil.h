//-----------------------------------------------------------------------------
/*!
   \file
   \brief       Utility functions for system filers (header)

   See cpp file for detailed description

   \implementation
   project     openSYDE
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     10.01.2017  STW/M.Echtler
   \endimplementation
*/
//-----------------------------------------------------------------------------
#ifndef C_OSCSYSTEMFILERUTIL_H
#define C_OSCSYSTEMFILERUTIL_H

/* -- Includes ------------------------------------------------------------- */

#include "C_OSCSystemBus.h"

/* -- Namespace ------------------------------------------------------------ */
namespace stw_opensyde_core
{
/* -- Global Constants ----------------------------------------------------- */

/* -- Types ---------------------------------------------------------------- */

class C_OSCSystemFilerUtil
{
public:
   C_OSCSystemFilerUtil();
   static stw_scl::C_SCLString mh_BusTypeEnumToString(const C_OSCSystemBus::E_Type oe_Type);
   static stw_types::sint32 mh_BusTypeStringToEnum(const stw_scl::C_SCLString & orc_Type,
                                                   C_OSCSystemBus::E_Type & ore_Type);
};

/* -- Extern Global Variables ---------------------------------------------- */
} //end of namespace

#endif
