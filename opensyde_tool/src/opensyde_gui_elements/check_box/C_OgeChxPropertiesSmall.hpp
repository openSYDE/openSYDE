//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Custom properties check box (small)
   \copyright   Copyright 2022 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_OGECHXPROPERTIESSMALL_H
#define C_OGECHXPROPERTIESSMALL_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "C_OgeChxProperties.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_gui_elements
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_OgeChxPropertiesSmall :
   public C_OgeChxProperties
{
   Q_OBJECT

public:
   explicit C_OgeChxPropertiesSmall(QWidget * const opc_Parent = NULL);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
