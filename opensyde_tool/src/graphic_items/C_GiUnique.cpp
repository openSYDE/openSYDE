//-----------------------------------------------------------------------------
/*!
   \internal
   \file
   \brief       Base class for all graphics items which require unique identification (implementation)

   Base class for all graphics items which require unique identification

   \implementation
   project     openSYDE
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     18.11.2016  STW/M.Echtler
   \endimplementation
*/
//-----------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------- */
#include "precomp_headers.h"

#include "C_GiUnique.h"

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

   \param[in] oru64_ID Unique ID

   \created     18.11.2016  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
C_GiUnique::C_GiUnique(const stw_types::uint64 & oru64_ID) :
   mu64_ID(oru64_ID)
{
}

//-----------------------------------------------------------------------------
/*!
   \brief   Default destructor

   \created     08.12.2016  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
C_GiUnique::~C_GiUnique(void)
{
}

//-----------------------------------------------------------------------------
/*!
   \brief   Check if unique ID matches

   \param[in] oru64_ID Unique ID to check for

   \return
   true:  match
   false: no match

   \created     21.11.2016  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
bool C_GiUnique::CheckMatch(const uint64 & oru64_ID) const
{
   return (this->mu64_ID == oru64_ID);
}

//-----------------------------------------------------------------------------
/*!
   \brief   Get ID

   \return
   Current ID

   \created     21.11.2016  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
stw_types::uint64 C_GiUnique::GetID() const
{
   return mu64_ID;
}
