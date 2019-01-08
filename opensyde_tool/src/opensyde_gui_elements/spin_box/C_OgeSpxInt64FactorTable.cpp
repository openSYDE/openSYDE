//-----------------------------------------------------------------------------
/*!
   \internal
   \file
   \brief       Int64 spin box which should avoid a zero value, table style (implementation)

   Int64 spin box which should avoid a zero value, table style.
   This class does not contain any functionality,
   but needs to exist, to have a unique group,
   to apply a specific stylesheet for.

   \implementation
   project     openSYDE
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     08.01.2018  STW/M.Echtler
   \endimplementation
*/
//-----------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------- */
#include "precomp_headers.h"

#include "C_OgeSpxInt64FactorTable.h"

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

   \created     08.01.2018  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
C_OgeSpxInt64FactorTable::C_OgeSpxInt64FactorTable(QWidget * const opc_Parent, const bool & orq_IsUnsigned) :
   C_OgeSpxInt64Factor(opc_Parent, orq_IsUnsigned)
{
}
