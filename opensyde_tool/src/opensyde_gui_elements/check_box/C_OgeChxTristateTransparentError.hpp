//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Custom transparent tristate error check box (header)

   See cpp file for detailed description

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_OGECHXTRISTATETRANSPARENTERROR_H
#define C_OGECHXTRISTATETRANSPARENTERROR_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */

#include <QCheckBox>

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_gui_elements
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_OgeChxTristateTransparentError :
   public QCheckBox
{
   Q_OBJECT

public:
   C_OgeChxTristateTransparentError(QWidget * const opc_Parent = NULL);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
