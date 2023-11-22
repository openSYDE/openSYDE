//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Custom text edit for table (header)

   See cpp file for detailed description

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_OGETEDTABLE_HPP
#define C_OGETEDTABLE_HPP

/* -- Includes ------------------------------------------------------------------------------------------------------ */

#include "C_OgeTedContextMenuBase.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_gui_elements
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_OgeTedTable :
   public C_OgeTedContextMenuBase
{
   Q_OBJECT

public:
   C_OgeTedTable(QWidget * const opc_Parent = NULL);

   //The signals keyword is necessary for Qt signal slot functionality
   //lint -save -e1736

Q_SIGNALS:
   //lint -restore
   void SigConfirmed(void) const;

protected:
   void focusInEvent(QFocusEvent * const opc_Event) override;
   void keyPressEvent(QKeyEvent * const opc_KeyEvent) override;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
