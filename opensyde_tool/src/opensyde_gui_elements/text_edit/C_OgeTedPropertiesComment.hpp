//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Text edit field for property comment (header)

   Text edit field for property comment (note: main module description should be in .cpp file)

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_OGETEDPROPERTIESCOMMENT_H
#define C_OGETEDPROPERTIESCOMMENT_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <QFocusEvent>
#include "C_OgeTedToolTipBase.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_gui_elements
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_OgeTedPropertiesComment :
   public C_OgeTedToolTipBase
{
   Q_OBJECT

public:
   C_OgeTedPropertiesComment(QWidget * const opc_Parent = NULL);

   //The signals keyword is necessary for Qt signal slot functionality
   //lint -save -e1736
Q_SIGNALS:
   //lint -restore
   void SigEditingFinished(void) const;  ///< emitted on focus out (return/enter key press mean "new line" in text
                                         // edits)
   void SigCommentConfirmed(void) const; ///< emitted on return/enter key press

protected:
   void focusOutEvent(QFocusEvent * const opc_Event) override;
   void keyPressEvent(QKeyEvent * const opc_KeyEvent) override;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
