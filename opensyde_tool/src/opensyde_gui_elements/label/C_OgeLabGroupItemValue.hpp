//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Label for group item value(header)

   Label for group item value(note: main module description should be in .cpp file)

   \copyright   Copyright 2016 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_OGELABGROUPITEMVALUE_HPP
#define C_OGELABGROUPITEMVALUE_HPP

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "C_OgeLabContextMenuBase.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_gui_elements
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_OgeLabGroupItemValue :
   public C_OgeLabContextMenuBase
{
   Q_OBJECT

public:
   C_OgeLabGroupItemValue(QWidget * const opc_Parent = NULL);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
