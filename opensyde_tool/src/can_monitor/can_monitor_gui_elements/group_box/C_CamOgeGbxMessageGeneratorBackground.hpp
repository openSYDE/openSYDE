//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Group box with white background and top border (header)

   See cpp file for detailed description

   \copyright   Copyright 2018 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_CAMOGEGBXMESSAGEGENERATORBACKGROUND_H
#define C_CAMOGEGBXMESSAGEGENERATORBACKGROUND_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <QGroupBox>

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_gui_elements
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_CamOgeGbxMessageGeneratorBackground :
   public QGroupBox
{
   Q_OBJECT

public:
   C_CamOgeGbxMessageGeneratorBackground(QWidget * const opc_Parent = NULL);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
