//-----------------------------------------------------------------------------
/*!
   \file
   \brief       Class to find bus or node by string (header)

   Class to find bus or node by string (See .cpp file for full description)

   \implementation
   project     openSYDE
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     08.09.2016  STW/M.Echtler
   \endimplementation
*/
//-----------------------------------------------------------------------------
#ifndef C_PUIBSFINDNAMEHELPERH
#define C_PUIBSFINDNAMEHELPERH

/* -- Includes ------------------------------------------------------------- */
#include "CSCLString.h"

/* -- Namespace ------------------------------------------------------------ */
namespace stw_opensyde_gui_logic
{
/* -- Global Constants ----------------------------------------------------- */

/* -- Types ---------------------------------------------------------------- */

class C_PuiBsFindNameHelper
{
public:
   C_PuiBsFindNameHelper(const stw_scl::C_SCLString & orc_Cmp);
   bool operator ()(const stw_scl::C_SCLString * const opc_Cur) const;

private:
   const stw_scl::C_SCLString mc_Cmp; ///< String to compare all items to
};

/* -- Extern Global Variables ---------------------------------------------- */
}

#endif
