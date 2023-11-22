//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Custom combo box with lines (header)

   Custom combo box with lines (note: main module description should be in .cpp file)

   \copyright   Copyright 2016 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_OGECBXLINE_HPP
#define C_OGECBXLINE_HPP

/* -- Includes ------------------------------------------------------------------------------------------------------ */

#include "C_OgeCbxIconOnly.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_gui_elements
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_OgeCbxLine :
   public C_OgeCbxIconOnly
{
   Q_OBJECT

public:
   C_OgeCbxLine(QWidget * const opc_Parent = NULL);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
