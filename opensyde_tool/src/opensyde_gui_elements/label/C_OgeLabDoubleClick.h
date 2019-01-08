//-----------------------------------------------------------------------------
/*!
   \file
   \brief       Label that emits a signal on double click (header)

   See cpp file for detailed description

   \implementation
   project     openSYDE
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     22.08.2018  STW/G.Scupin
   \endimplementation
*/
//-----------------------------------------------------------------------------
#ifndef C_OGELABDOUBLECLICK_H
#define C_OGELABDOUBLECLICK_H

/* -- Includes ------------------------------------------------------------- */
#include "C_OgeLabGenericNoPaddingNoMargins.h"

/* -- Namespace ------------------------------------------------------------ */
namespace stw_opensyde_gui_elements
{
/* -- Global Constants ----------------------------------------------------- */

/* -- Types ---------------------------------------------------------------- */

class C_OgeLabDoubleClick :
   public C_OgeLabGenericNoPaddingNoMargins
{
   Q_OBJECT

public:
   explicit C_OgeLabDoubleClick(QWidget * const opc_Parent = NULL);

   //The signals keyword is necessary for Qt signal slot functionality
   //lint -save -e1736
Q_SIGNALS:
   //lint -restore
   void SigDoubleClicked();

protected:
   // The naming of the Qt parameters can't be changed and are not compliant with the naming conventions
   //lint -save -e1960
   virtual void mouseDoubleClickEvent(QMouseEvent * const opc_Event) override;
   //lint -restore
};

/* -- Extern Global Variables ---------------------------------------------- */
} //end of namespace

#endif
