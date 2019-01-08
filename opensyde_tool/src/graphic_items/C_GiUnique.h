//-----------------------------------------------------------------------------
/*!
   \file
   \brief       Base class for all graphics items which require unique identification (header)

   See cpp file for detailed description

   \implementation
   project     openSYDE
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     18.11.2016  STW/M.Echtler
   \endimplementation
*/
//-----------------------------------------------------------------------------
#ifndef C_GIUNIQUE_H
#define C_GIUNIQUE_H

/* -- Includes ------------------------------------------------------------- */
#include "stwtypes.h"

/* -- Namespace ------------------------------------------------------------ */
namespace stw_opensyde_gui_logic
{
/* -- Global Constants ----------------------------------------------------- */

/* -- Types ---------------------------------------------------------------- */

class C_GiUnique
{
public:
   C_GiUnique(const stw_types::uint64 & oru64_ID);
   virtual ~C_GiUnique(void);
   bool CheckMatch(const stw_types::uint64 & oru64_ID) const;

   stw_types::uint64 GetID() const;

private:
   const stw_types::uint64 mu64_ID;
};

/* -- Extern Global Variables ---------------------------------------------- */
} //end of namespace

#endif
