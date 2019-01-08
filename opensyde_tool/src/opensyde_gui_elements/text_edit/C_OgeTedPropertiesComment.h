//-----------------------------------------------------------------------------
/*!
   \file
   \brief       Text edit field for property comment (header)

   Text edit field for property comment (note: main module description should be in .cpp file)

   \implementation
   project     openSYDE
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     16.01.2017  STW/S.Singer
   \endimplementation
*/
//-----------------------------------------------------------------------------
#ifndef C_OGETEDPROPERTIESCOMMENT_H
#define C_OGETEDPROPERTIESCOMMENT_H

/* -- Includes ------------------------------------------------------------- */
#include <QFocusEvent>
#include "C_OgeTedToolTipBase.h"

/* -- Namespace ------------------------------------------------------------ */
namespace stw_opensyde_gui_elements
{
/* -- Global Constants ----------------------------------------------------- */

/* -- Types ---------------------------------------------------------------- */

class C_OgeTedPropertiesComment :
   public C_OgeTedToolTipBase
{
   Q_OBJECT

public:
   C_OgeTedPropertiesComment(QWidget * const opc_Parent = NULL);

protected:
   // The naming of the Qt parameters can't be changed and are not compliant with the naming conventions
   //lint -save -e1960
   virtual void focusOutEvent(QFocusEvent * const opc_Event) override;
   //lint -restore
};

/* -- Extern Global Variables ---------------------------------------------- */
} //end of namespace

#endif
