//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Group box with top blue border (header)

   See cpp file for detailed description

   \copyright   Copyright 2018 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_CAMOGEGBXMESSAGESIGNALSBORDER_H
#define C_CAMOGEGBXMESSAGESIGNALSBORDER_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <QGroupBox>

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_gui_elements
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_CamOgeGbxMessageSignalsBorder :
   public QGroupBox
{
   Q_OBJECT

public:
   explicit C_CamOgeGbxMessageSignalsBorder(QWidget * const opc_Parent = NULL);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
