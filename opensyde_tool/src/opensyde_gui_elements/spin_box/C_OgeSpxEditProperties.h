//-----------------------------------------------------------------------------
/*!
   \file
   \brief       Spin box field for editing properties (header)

   See cpp file for detailed description

   \implementation
   project     openSYDE
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     17.01.2017  STW/M.Echtler
   \endimplementation
*/
//-----------------------------------------------------------------------------
#ifndef C_OGESPXEDITPROPERTIES_H
#define C_OGESPXEDITPROPERTIES_H

/* -- Includes ------------------------------------------------------------- */

#include "C_OgeSpxToolTipBase.h"

/* -- Namespace ------------------------------------------------------------ */
namespace stw_opensyde_gui_elements
{
/* -- Global Constants ----------------------------------------------------- */

/* -- Types ---------------------------------------------------------------- */

class C_OgeSpxEditProperties :
   public C_OgeSpxToolTipBase
{
   Q_OBJECT

public:
   C_OgeSpxEditProperties(QWidget * const opc_Parent = NULL);

   //The signals keyword is necessary for Qt signal slot functionality
   //lint -save -e1736

Q_SIGNALS:
   //lint -restore
   void SigFocus(void);

protected:
   // The naming of the Qt parameters can't be changed and are not compliant with the naming conventions
   //lint -save -e1960
   virtual void focusInEvent(QFocusEvent * const opc_Event) override;
   virtual void mousePressEvent(QMouseEvent * const opc_Event) override;
   //lint -restore

private:
   bool mq_JustGotFocus;
};

/* -- Extern Global Variables ---------------------------------------------- */
} //end of namespace

#endif
