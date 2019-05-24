//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Custom group box grayed with edit (header)

   See cpp file for detailed description

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_OGEGBXGRAYEDEDIT_H
#define C_OGEGBXGRAYEDEDIT_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <QGroupBox>

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw_opensyde_gui_elements
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_OgeGbxGrayedEdit :
   public QGroupBox
{
   Q_OBJECT

public:
   C_OgeGbxGrayedEdit(QWidget * const opc_Parent = NULL);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif
