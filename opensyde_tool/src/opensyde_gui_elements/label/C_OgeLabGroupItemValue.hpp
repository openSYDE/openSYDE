//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Label for group item value(header)

   Label for group item value(note: main module description should be in .cpp file)

   \copyright   Copyright 2016 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_OGELABGROUPITEMVALUE_H
#define C_OGELABGROUPITEMVALUE_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "C_OgeLabToolTipBase.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_gui_elements
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_OgeLabGroupItemValue :
   public C_OgeLabToolTipBase
{
   Q_OBJECT

public:
   C_OgeLabGroupItemValue(QWidget * const opc_Parent = NULL);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
