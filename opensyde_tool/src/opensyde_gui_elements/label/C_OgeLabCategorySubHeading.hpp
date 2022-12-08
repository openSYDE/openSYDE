//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Label for category sub heading (header)

   Label for widget heading (note: main module description should be in .cpp file)

   \copyright   Copyright 2016 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_OGELABCATEGORYSUBHEADING_H
#define C_OGELABCATEGORYSUBHEADING_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */

#include <QLabel>

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_gui_elements
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_OgeLabCategorySubHeading :
   public QLabel
{
   Q_OBJECT

public:
   C_OgeLabCategorySubHeading(QWidget * const opc_Parent = NULL);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
