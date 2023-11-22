//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Label for widget heading sub title (header)

   Label for widget heading sub title (note: main module description should be in .cpp file)

   \copyright   Copyright 2016 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_OGELABHEADINGWIDGETSUBTITLE_HPP
#define C_OGELABHEADINGWIDGETSUBTITLE_HPP

/* -- Includes ------------------------------------------------------------------------------------------------------ */

#include <QLabel>

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_gui_elements
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_OgeLabHeadingWidgetSubTitle :
   public QLabel
{
   Q_OBJECT

public:
   C_OgeLabHeadingWidgetSubTitle(QWidget * const opc_Parent = NULL);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
