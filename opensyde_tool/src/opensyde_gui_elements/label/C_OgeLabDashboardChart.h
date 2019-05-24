//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Label for dashboard chart (header)

   See cpp file for detailed description

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_OGELABDASHBOARDCHART_H
#define C_OGELABDASHBOARDCHART_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <QLabel>

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw_opensyde_gui_elements
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_OgeLabDashboardChart :
   public QLabel
{
   Q_OBJECT

public:
   C_OgeLabDashboardChart(QWidget * const opc_Parent = NULL);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif
