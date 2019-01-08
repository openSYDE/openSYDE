//-----------------------------------------------------------------------------
/*!
   \internal
   \file
   \brief       Int 64 spin box field for table (implementation)

   Int 64 spin box field for table.
   This class does not contain any functionality,
   but needs to exist, to have a unique group,
   to apply a specific stylesheet for.

   \implementation
   project     openSYDE
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     30.03.2017  STW/M.Echtler
   \endimplementation
*/
//-----------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------- */
#include "precomp_headers.h"

#include "C_OgeSpxInt64Table.h"

/* -- Used Namespaces ------------------------------------------------------ */
using namespace stw_opensyde_gui_elements;

/* -- Module Global Constants ---------------------------------------------- */

/* -- Types ---------------------------------------------------------------- */

/* -- Global Variables ----------------------------------------------------- */

/* -- Module Global Variables ---------------------------------------------- */

/* -- Module Global Function Prototypes ------------------------------------ */

/* -- Implementation ------------------------------------------------------- */

//-----------------------------------------------------------------------------
/*!
   \brief   Default constructor

   Set up GUI with all elements.

   \param[in,out] opc_Parent     Optional pointer to parent
   \param[in]     orq_IsUnsigned Indicator if this spin box should handle unsigned 64 bit or signed 64 bit

   \created     30.03.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
C_OgeSpxInt64Table::C_OgeSpxInt64Table(QWidget * const opc_Parent, const bool & orq_IsUnsigned) :
   C_OgeSpxInt64ToolTipBase(opc_Parent, orq_IsUnsigned)
{
}
