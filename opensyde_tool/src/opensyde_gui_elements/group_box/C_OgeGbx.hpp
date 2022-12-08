//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Custom group box (header)

   Custom group box (note: main module description should be in .cpp file)

   \copyright   Copyright 2016 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_OGEGBX_H
#define C_OGEGBX_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */

#include <QGroupBox>

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_gui_elements
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_OgeGbx :
   public QGroupBox
{
   Q_OBJECT

public:
   C_OgeGbx(QWidget * const opc_Parent = NULL);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
