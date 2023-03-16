//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Navigation heading with special style (and background)
   \copyright   Copyright 2019 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_OGELABNAVIGATIONSUBHEADING_H
#define C_OGELABNAVIGATIONSUBHEADING_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <QLabel>

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_gui_elements
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_OgeLabNavigationSubHeading :
   public QLabel
{
   Q_OBJECT

public:
   explicit C_OgeLabNavigationSubHeading(QWidget * const opc_Parent = NULL);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
