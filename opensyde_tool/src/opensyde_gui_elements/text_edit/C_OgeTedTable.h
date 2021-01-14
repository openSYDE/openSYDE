//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Custom text edit for table (header)

   See cpp file for detailed description

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_OGETEDTABLE_H
#define C_OGETEDTABLE_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */

#include "C_OgeTedContextMenuBase.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw_opensyde_gui_elements
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_OgeTedTable :
   public C_OgeTedContextMenuBase
{
   Q_OBJECT

public:
   C_OgeTedTable(QWidget * const opc_Parent = NULL);

protected:
   virtual void focusInEvent(QFocusEvent * const opc_Event) override;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif
