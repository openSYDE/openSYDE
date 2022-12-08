//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Line edit field for toolbar search (header)

   Line edit field for toolbar search (note: main module description should be in .cpp file)

   \copyright   Copyright 2016 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_OGELETOOLBARSEARCH_H
#define C_OGELETOOLBARSEARCH_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */

#include "C_OgeLeContextMenuBase.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_gui_elements
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_OgeLeToolBarSearch :
   public C_OgeLeContextMenuBase
{
   Q_OBJECT

public:
   C_OgeLeToolBarSearch(QWidget * const opc_Parent = NULL);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
