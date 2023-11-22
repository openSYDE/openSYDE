//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Custom combo box with arrow heads (header)

   Custom combo box (note: main module description should be in .cpp file)

   \copyright   Copyright 2016 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_OGECBXARROW_HPP
#define C_OGECBXARROW_HPP

/* -- Includes ------------------------------------------------------------------------------------------------------ */

#include "C_OgeCbxIconOnly.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_gui_elements
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_OgeCbxArrow :
   public C_OgeCbxIconOnly
{
   Q_OBJECT

public:
   C_OgeCbxArrow(QWidget * const opc_Parent = NULL);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
