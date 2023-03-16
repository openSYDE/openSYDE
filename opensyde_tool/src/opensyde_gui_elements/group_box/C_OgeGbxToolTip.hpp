//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Label for tool tip group box (header)

   Label for tool tip group box (note: main module description should be in .cpp file)

   \copyright   Copyright 2016 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_OGEGBXTOOLTIP_H
#define C_OGEGBXTOOLTIP_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */

#include <QGroupBox>

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_gui_elements
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_OgeGbxToolTip :
   public QGroupBox
{
   Q_OBJECT

public:
   C_OgeGbxToolTip(QWidget * const opc_Parent = NULL);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
