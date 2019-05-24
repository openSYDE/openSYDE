//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Label for progress text (under a progress bopperle) (header)

   See cpp file for detailed description

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_OGELABPROGRESSTEXT_H
#define C_OGELABPROGRESSTEXT_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <QLabel>

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw_opensyde_gui_elements
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_OgeLabProgressText :
   public QLabel
{
   Q_OBJECT

public:
   C_OgeLabProgressText(QWidget * const opc_Parent);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif
