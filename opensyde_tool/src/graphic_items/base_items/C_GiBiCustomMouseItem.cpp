//-----------------------------------------------------------------------------
/*!
   \internal
   \file
   \brief       Base class for changeable mouse cursor on hover over (implementation)

   Base class for changeable mouse cursor on hover over

   \implementation
   project     openSYDE
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     20.09.2016  STW/M.Echtler
   \endimplementation
*/
//-----------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------- */
#include "precomp_headers.h"

#include "C_GiBiCustomMouseItem.h"

/* -- Used Namespaces ------------------------------------------------------ */

using namespace stw_opensyde_gui;

/* -- Module Global Constants ---------------------------------------------- */

/* -- Types ---------------------------------------------------------------- */

/* -- Global Variables ----------------------------------------------------- */

/* -- Module Global Variables ---------------------------------------------- */

/* -- Module Global Function Prototypes ------------------------------------ */

/* -- Implementation ------------------------------------------------------- */

//-----------------------------------------------------------------------------
/*!
   \brief   Default constructor

   \created     20.09.2016  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
C_GiBiCustomMouseItem::C_GiBiCustomMouseItem(void)
{
   this->mc_DefaultCursor = QCursor(Qt::ArrowCursor);
}

//-----------------------------------------------------------------------------
/*!
   \brief   Default destructor

   \created     19.10.2016  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
C_GiBiCustomMouseItem::~C_GiBiCustomMouseItem(void)
{
}
