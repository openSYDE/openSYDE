//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Styled multi combobox for HALC parameter configuration

   \copyright   Copyright 2020 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_OGECBXMULTISELECTTABLEHALC_HPP
#define C_OGECBXMULTISELECTTABLEHALC_HPP

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "C_OgeCbxMultiSelect.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_gui_elements
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_OgeCbxMultiSelectTableHalc :
   public C_OgeCbxMultiSelect
{
   Q_OBJECT

public:
   explicit C_OgeCbxMultiSelectTableHalc(QWidget * const opc_Parent = NULL);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
