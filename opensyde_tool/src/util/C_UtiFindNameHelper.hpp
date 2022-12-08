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
#include "C_SclString.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_gui_logic
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_UtiFindNameHelper
{
public:
   C_UtiFindNameHelper(const stw::scl::C_SclString & orc_Cmp = "");
   bool operator ()(const stw::scl::C_SclString * const opc_Cur) const;

private:
   const stw::scl::C_SclString mc_Cmp; ///< String to compare all items to
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
}

#endif
