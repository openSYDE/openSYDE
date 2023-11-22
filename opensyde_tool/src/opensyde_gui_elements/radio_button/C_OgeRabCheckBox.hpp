//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Radio button check box style (header)

   See cpp file for detailed description

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_OGERABCHECKBOX_HPP
#define C_OGERABCHECKBOX_HPP

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <QRadioButton>

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_gui_elements
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_OgeRabCheckBox :
   public QRadioButton
{
   Q_OBJECT

public:
   C_OgeRabCheckBox(QWidget * const opc_Parent = NULL);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
