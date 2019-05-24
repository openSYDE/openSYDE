//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Label for tool tip heading (header)

   Label for tool tip heading (note: main module description should be in .cpp file)

   \copyright   Copyright 2016 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_OGELABHEADINGTOOLTIP_H
#define C_OGELABHEADINGTOOLTIP_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */

#include <QLabel>

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw_opensyde_gui_elements
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_OgeLabHeadingToolTip :
   public QLabel
{
   Q_OBJECT

public:
   C_OgeLabHeadingToolTip(QWidget * const opc_Parent = NULL);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif
