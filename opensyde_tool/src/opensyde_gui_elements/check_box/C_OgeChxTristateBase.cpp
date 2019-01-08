//-----------------------------------------------------------------------------
/*!
   \internal
   \file
   \brief       Custom tristate check box base (Handle check box indicator resize) (implementation)

   Custom tristate check box base.
   Handle check box indicator resize.

   \implementation
   project     openSYDE
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     14.02.2017  STW/M.Echtler
   \endimplementation
*/
//-----------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------- */
#include "precomp_headers.h"

#include <QResizeEvent>
#include "C_OgeChxTristateBase.h"

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

   \created     14.02.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
C_OgeChxTristateBase::C_OgeChxTristateBase(QWidget * const opc_Parent) :
   C_OgeChxToolTipBase(opc_Parent)
{
}

//-----------------------------------------------------------------------------
/*!
   \brief   Overwritten resize event slot

   Here: Resize indicator to widget size

   \param[in,out] opc_Event Event identification and information

   \created     14.02.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_OgeChxTristateBase::resizeEvent(QResizeEvent * const opc_Event)
{
   const QString c_StyleSheet = "stw_opensyde_gui_elements--C_OgeChxTristateBase::indicator {"
                                "width:" + QString::number(opc_Event->size().width()) + "px;"
                                "height:" + QString::number(opc_Event->size().height()) + "px;"
                                "}";

   this->setStyleSheet(c_StyleSheet);
   C_OgeChxToolTipBase::resizeEvent(opc_Event);
}
