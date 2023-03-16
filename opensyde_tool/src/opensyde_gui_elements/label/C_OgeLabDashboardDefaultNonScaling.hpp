//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Default label for dashboard, not scaling (header)

   See cpp file for detailed description

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_OGELABDASHBOARDDEFAULTNONSCALING_H
#define C_OGELABDASHBOARDDEFAULTNONSCALING_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <QLabel>

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_gui_elements
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_OgeLabDashboardDefaultNonScaling :
   public QLabel
{
   Q_OBJECT

public:
   C_OgeLabDashboardDefaultNonScaling(QWidget * const opc_Parent = NULL);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
