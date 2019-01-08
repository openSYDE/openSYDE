//-----------------------------------------------------------------------------
/*!
   \file
   \brief       Check box with tool tip (header)

   See cpp file for detailed description

   \implementation
   project     openSYDE
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     26.03.2018  STW/M.Echtler
   \endimplementation
*/
//-----------------------------------------------------------------------------
#ifndef C_OGECHXTOOLTIPBASE_H
#define C_OGECHXTOOLTIPBASE_H

/* -- Includes ------------------------------------------------------------- */
#include <QCheckBox>
#include "C_OgeToolTipBase.h"

/* -- Namespace ------------------------------------------------------------ */
namespace stw_opensyde_gui_elements
{
/* -- Global Constants ----------------------------------------------------- */

/* -- Types ---------------------------------------------------------------- */

class C_OgeChxToolTipBase :
   public QCheckBox,
   public C_OgeToolTipBase
{
public:
   C_OgeChxToolTipBase(QWidget * const opc_Parent = NULL);

protected:
   // The naming of the Qt parameters can't be changed and are not compliant with the naming conventions
   //lint -save -e1960
   virtual bool event(QEvent * const opc_Event) override;
   //lint -restore
};

/* -- Extern Global Variables ---------------------------------------------- */
} //end of namespace

#endif
