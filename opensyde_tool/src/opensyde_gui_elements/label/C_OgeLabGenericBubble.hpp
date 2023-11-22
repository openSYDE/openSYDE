//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Generic label styled via setters with a bubble background
   \copyright   Copyright 2020 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_OGELABGENERICBUBBLE_HPP
#define C_OGELABGENERICBUBBLE_HPP

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "C_OgeLabGenericNoPaddingNoMargins.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_gui_elements
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_OgeLabGenericBubble :
   public C_OgeLabGenericNoPaddingNoMargins
{
   Q_OBJECT

public:
   explicit C_OgeLabGenericBubble(QWidget * const opc_Parent = NULL);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
