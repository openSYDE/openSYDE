//-----------------------------------------------------------------------------
/*!
   \internal
   \file
   \brief       Line edit variant to be integrated into a combo box (implementation)

   Line edit variant to be integrated into a combo box

   \implementation
   project     openSYDE
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     25.09.2017  STW/M.Echtler
   \endimplementation
*/
//-----------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------- */
#include "precomp_headers.h"

#include <QComboBox>
#include "C_OgeLeComboBox.h"
#include "C_OgeCbxText.h"

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

   \created     25.09.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
C_OgeLeComboBox::C_OgeLeComboBox(QWidget * const opc_Parent) :
   C_OgeLeContextMenuBase(opc_Parent)
{
}

//-----------------------------------------------------------------------------
/*!
   \brief   Overwritten mouse press event slot

   Here: Trigger combo box

   \param[in,out] opc_Event Event identification and information

   \created     25.09.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_OgeLeComboBox::mousePressEvent(QMouseEvent * const opc_Event)
{
   //lint -e{929}  false positive in PC-Lint: allowed by MISRA 5-2-2
   C_OgeCbxText * const pc_Parent = dynamic_cast<C_OgeCbxText *>(this->parent());

   C_OgeLeContextMenuBase::mousePressEvent(opc_Event);

   //Trigger combo box pop up
   if (pc_Parent != NULL)
   {
      pc_Parent->SuppressHide(); // suppress call to hidePopup (called because of "outside of combo box")
      pc_Parent->showPopup();
   }
}
