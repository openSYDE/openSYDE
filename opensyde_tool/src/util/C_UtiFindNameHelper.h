//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Class to find bus or node by string (header)

   Class to find bus or node by string (See .cpp file for full description)

   \copyright   Copyright 2016 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_UTIFINDNAMEHELPERH
#define C_UTIFINDNAMEHELPERH

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "CSCLString.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw_opensyde_gui_logic
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_UtiFindNameHelper
{
public:
   C_UtiFindNameHelper(const stw_scl::C_SCLString & orc_Cmp = "");
   bool operator ()(const stw_scl::C_SCLString * const opc_Cur) const;

private:
   const stw_scl::C_SCLString mc_Cmp; ///< String to compare all items to
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}

#endif
