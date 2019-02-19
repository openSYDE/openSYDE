//-----------------------------------------------------------------------------
/*!
   \internal
   \file
   \brief       View (display) component for filter items table view (implementation)

   View (display) component for filter items table view.

   \implementation
   project     openSYDE
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     07.12.2018  STW/G.Landsgesell
   \endimplementation
*/
//-----------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------- */
#include "precomp_headers.h"

#include <QHeaderView>

#include "C_CamMosFilterTableView.h"

/* -- Used Namespaces ------------------------------------------------------ */
using namespace stw_types;
using namespace stw_opensyde_gui;
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

   Set up GUI with all elements.

   \param[in,out] opc_Parent Optional pointer to parent

   \created     07.12.2018  STW/G.Landsgesell
*/
//-----------------------------------------------------------------------------
C_CamMosFilterTableView::C_CamMosFilterTableView(QWidget * const opc_Parent) :
   C_TblViewToolTipBase(opc_Parent)
{
   this->setItemDelegate(&this->mc_Delegate);

   // model is set externally (see C_CamMosFilterPopup)

   //Configure
   this->setShowGrid(false);
   this->verticalHeader()->setDefaultSectionSize(20);

   //Deactivate
   this->verticalHeader()->setVisible(false);

   this->setFocusPolicy(Qt::NoFocus);
   this->setSelectionBehavior(QAbstractItemView::SelectRows);
   this->setSelectionMode(QAbstractItemView::SingleSelection);

   this->setMinimumHeight(100);
   this->setMaximumHeight(100);
}
