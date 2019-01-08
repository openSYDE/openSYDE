//-----------------------------------------------------------------------------
/*!
   \internal
   \file
   \brief       Header view for tables with only one header (implementation)

   Header view for tables with only one header.

   \implementation
   project     openSYDE
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     29.05.2017  STW/M.Echtler
   \endimplementation
*/
//-----------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------- */
#include "precomp_headers.h"

#include "C_SdNdeSingleHeaderView.h"

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

   Set up GUI with all elements.

   \param[in]     ore_Orientation Orientation
   \param[in,out] opc_Parent      Optional pointer to parent

   \created     29.05.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
C_SdNdeSingleHeaderView::C_SdNdeSingleHeaderView(const Qt::Orientation & ore_Orientation, QWidget * const opc_Parent) :
   C_SdNdeDataPoolListTableHeaderView(ore_Orientation, opc_Parent)
{
}
