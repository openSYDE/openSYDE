//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Custom group box grayed (header)

   Custom group box grayed (note: main module description should be in .cpp file)

   \copyright   Copyright 2016 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_OGEGBXGRAYED_H
#define C_OGEGBXGRAYED_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */

#include <QGroupBox>

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_gui_elements
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_OgeGbxGrayed :
   public QGroupBox
{
   Q_OBJECT

public:
   C_OgeGbxGrayed(QWidget * const opc_Parent = NULL);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
