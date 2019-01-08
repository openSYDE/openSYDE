//-----------------------------------------------------------------------------
/*!
   \internal
   \file
   \brief       Base class for graphics items with any data elements (implementation)

   Base class for graphics items with any data elements

   \implementation
   project     openSYDE
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     19.07.2017  STW/M.Echtler
   \endimplementation
*/
//-----------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------- */
#include "precomp_headers.h"

#include "stwtypes.h"
#include "C_PuiBsDataElement.h"

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

   \param[in] ors32_Index Index of connected data item

   \created     19.07.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
C_PuiBsDataElement::C_PuiBsDataElement(const sint32 & ors32_Index) :
   ms32_Index(ors32_Index)
{
}

//-----------------------------------------------------------------------------
/*!
   \brief   Default destructor

   Clean up.

   \created     19.07.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
C_PuiBsDataElement::~C_PuiBsDataElement(void)
{
}

//-----------------------------------------------------------------------------
/*!
   \brief   Get index

   \created     19.07.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
stw_types::sint32 C_PuiBsDataElement::GetIndex(void) const
{
   return this->ms32_Index;
}
