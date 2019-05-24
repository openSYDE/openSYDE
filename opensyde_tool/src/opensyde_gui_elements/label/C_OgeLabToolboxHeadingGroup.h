//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Label for toolbox group heading (header)

   Label for toolbox group heading (note: main module description should be in .cpp file)

   \copyright   Copyright 2016 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_OGELABTOOLBOXHEADINGGROUP_H
#define C_OGELABTOOLBOXHEADINGGROUP_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */

#include <QLabel>

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw_opensyde_gui_elements
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_OgeLabToolboxHeadingGroup :
   public QLabel
{
   Q_OBJECT

public:
   C_OgeLabToolboxHeadingGroup(QWidget * const opc_Parent = NULL);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif
