//-----------------------------------------------------------------------------
/*!
   \file
   \brief       Custom tristate check box base (Handle check box indicator resize) (header)

   See cpp file for detailed description

   \implementation
   project     openSYDE
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     14.02.2017  STW/M.Echtler
   \endimplementation
*/
//-----------------------------------------------------------------------------
#ifndef C_OGECHXTRISTATEBASE_H
#define C_OGECHXTRISTATEBASE_H

/* -- Includes ------------------------------------------------------------- */

#include "C_OgeChxToolTipBase.h"

/* -- Namespace ------------------------------------------------------------ */
namespace stw_opensyde_gui_elements
{
/* -- Global Constants ----------------------------------------------------- */

/* -- Types ---------------------------------------------------------------- */

class C_OgeChxTristateBase :
   public C_OgeChxToolTipBase
{
   Q_OBJECT

public:
   C_OgeChxTristateBase(QWidget * const opc_Parent = NULL);

protected:
   virtual void resizeEvent(QResizeEvent * const opc_Event) override;
};

/* -- Extern Global Variables ---------------------------------------------- */
} //end of namespace

#endif
