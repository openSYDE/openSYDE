//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Class for text brower with context menu styled as a simple label
   \copyright   Copyright 2023 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_OGETEBLABEL_H
#define C_OGETEBLABEL_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "C_OgeTebContextMenuBase.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_gui_elements
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_OgeTebLabel :
   public C_OgeTebContextMenuBase
{
   Q_OBJECT

public:
   explicit C_OgeTebLabel(QWidget * const opc_Parent = NULL);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
