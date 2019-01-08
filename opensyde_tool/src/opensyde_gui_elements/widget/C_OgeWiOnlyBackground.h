//-----------------------------------------------------------------------------
/*!
   \file
   \brief       Widget with a simple background (header)

   See cpp file for detailed description

   \implementation
   project     openSYDE
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     13.02.2018  STW/B.Bayer
   \endimplementation
*/
//-----------------------------------------------------------------------------
#ifndef C_OGEWIONLYBACKGROUND_H
#define C_OGEWIONLYBACKGROUND_H

/* -- Includes ------------------------------------------------------------- */
#include "stwtypes.h"
#include "C_OgeWiWithToolTip.h"

/* -- Namespace ------------------------------------------------------------ */
namespace stw_opensyde_gui_elements
{
/* -- Global Constants ----------------------------------------------------- */

/* -- Types ---------------------------------------------------------------- */

class C_OgeWiOnlyBackground :
   public C_OgeWiWithToolTip
{
   Q_OBJECT

public:
   C_OgeWiOnlyBackground(QWidget * const opc_Parent = NULL);
   void SetBackgroundColor(const stw_types::sintn osn_Value);

protected:
   // The naming of the Qt parameters can't be changed and are not compliant with the naming conventions
   //lint -save -e1960
   virtual void paintEvent(QPaintEvent * const opc_Event) override;
   //lint -restore
};

/* -- Extern Global Variables ---------------------------------------------- */
} //end of namespace

#endif
