//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Line edit variant to be integrated into a combo box (header)

   See cpp file for detailed description

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_OGELECOMBOBOX_HPP
#define C_OGELECOMBOBOX_HPP

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "C_OgeLeContextMenuBase.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_gui_elements
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_OgeLeComboBox :
   public C_OgeLeContextMenuBase
{
public:
   C_OgeLeComboBox(QWidget * const opc_Parent = NULL);

protected:
   void mousePressEvent(QMouseEvent * const opc_Event) override;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
