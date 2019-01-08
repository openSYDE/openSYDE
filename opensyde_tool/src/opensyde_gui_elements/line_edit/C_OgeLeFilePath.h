//-----------------------------------------------------------------------------
/*!
   \file
   \brief       Line edit field for file paths (header)

   Line edit field for file paths (note: main module description should be in .cpp file)

   \implementation
   project     openSYDE
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     09.07.2018  STW/D.Pohl
   \endimplementation
*/
//-----------------------------------------------------------------------------
#ifndef C_OGELEFILEPATH_H
#define C_OGELEFILEPATH_H

/* -- Includes ------------------------------------------------------------- */

#include "C_OgeLeProperties.h"

/* -- Namespace ------------------------------------------------------------ */
namespace stw_opensyde_gui_elements
{
/* -- Global Constants ----------------------------------------------------- */

/* -- Types ---------------------------------------------------------------- */

class C_OgeLeFilePath :
   public C_OgeLeProperties
{
   Q_OBJECT

public:
   C_OgeLeFilePath(QWidget * const opc_Parent = NULL);
   //lint -e{1511} Member hides non-virtual member --> does no harm and is rather desired
   void SetToolTipInformation(const QString & orc_FullFilePath);

   //The signals keyword is necessary for Qt signal slot functionality
   //lint -save -e1736
Q_SIGNALS:
   //lint -restore
   void SigFocus(void);
   void SigFocusOut(void);

protected:
   virtual void focusInEvent(QFocusEvent * const opc_Event);
   virtual void focusOutEvent(QFocusEvent * const opc_Event);
};

/* -- Extern Global Variables ---------------------------------------------- */
} //end of namespace

#endif
