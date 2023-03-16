//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Label for progress log heading (header)

   Label for progress log heading (note: main module description should be in .cpp file)

   \copyright   Copyright 2019 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_OGELABPROGRESSLOGHEADING_H
#define C_OGELABPROGRESSLOGHEADING_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <QLabel>

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_gui_elements
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */
class C_OgeLabProgressLogHeading :
   public QLabel
{
   Q_OBJECT

public:
   C_OgeLabProgressLogHeading(QWidget * const opc_Parent = NULL);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
