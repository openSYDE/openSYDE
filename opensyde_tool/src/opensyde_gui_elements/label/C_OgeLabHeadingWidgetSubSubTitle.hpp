//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Label for styling heading sub-sub-titles (header)

   See cpp file for detailed description

   \copyright   Copyright 2018 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_OGELABHEADINGWIDGETSUBSUBTITLE_H
#define C_OGELABHEADINGWIDGETSUBSUBTITLE_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <QLabel>

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_gui_elements
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_OgeLabHeadingWidgetSubSubTitle :
   public QLabel
{
   Q_OBJECT

public:
   explicit C_OgeLabHeadingWidgetSubSubTitle(QWidget * const opc_Parent = NULL);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
