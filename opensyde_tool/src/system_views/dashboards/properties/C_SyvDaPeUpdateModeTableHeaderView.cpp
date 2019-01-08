//-----------------------------------------------------------------------------
/*!
   \internal
   \file
   \brief       Header view for dashboard table widget (implementation)

   Header view for dashboard table widget

   \implementation
   project     openSYDE
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     13.09.2017  STW/M.Echtler
   \endimplementation
*/
//-----------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------- */
#include "precomp_headers.h"

#include "C_SyvDaPeUpdateModeTableHeaderView.h"

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

   \created     13.09.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
C_SyvDaPeUpdateModeTableHeaderView::C_SyvDaPeUpdateModeTableHeaderView(const Qt::Orientation & ore_Orientation,
                                                                       QWidget * const opc_Parent) :
   C_SdNdeDataPoolListTableHeaderView(ore_Orientation, opc_Parent)
{
}

//-----------------------------------------------------------------------------
/*!
   \brief   Get header size

   Here: set custom height

   \return
   Header size

   \created     13.09.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
QSize C_SyvDaPeUpdateModeTableHeaderView::sizeHint(void) const
{
   QSize c_Retval = QHeaderView::sizeHint();

   c_Retval.setHeight(50);

   return c_Retval;
}
