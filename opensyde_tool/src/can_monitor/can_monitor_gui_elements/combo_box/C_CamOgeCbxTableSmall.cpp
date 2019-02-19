//-----------------------------------------------------------------------------
/*!
   \internal
   \file
   \brief       Combo box styled for smaller widget usage (implementation)

   Combo box styled for smaller widget usage.
   This class does not contain any functionality,
   but needs to exist, to have a unique group,
   to apply a specific stylesheet for.

   \implementation
   project     openSYDE
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     30.01.2019  STW/M.Echtler
   \endimplementation
*/
//-----------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------- */
#include "precomp_headers.h"

#include "C_OgeCbxIconDelegate.h"
#include "C_CamOgeCbxTableSmall.h"

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

   \param[in,out] opc_Parent Optional pointer to parent

   \created     30.01.2019  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
C_CamOgeCbxTableSmall::C_CamOgeCbxTableSmall(QWidget * const opc_Parent) :
   C_OgeCbxResizingView(opc_Parent)
{
   //this code allows to handle the QAbstractItemView::item in stylesheets
   C_OgeCbxIconDelegate * const pc_ItemDelegate = new C_OgeCbxIconDelegate();

   this->setItemDelegate(pc_ItemDelegate);
   //lint -e{429}  no memory leak because of the parent all elements and the Qt memory management
}

//-----------------------------------------------------------------------------
/*!
   \brief   Default destructor

   \created     30.01.2019  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
C_CamOgeCbxTableSmall::~C_CamOgeCbxTableSmall(void)
{
}
