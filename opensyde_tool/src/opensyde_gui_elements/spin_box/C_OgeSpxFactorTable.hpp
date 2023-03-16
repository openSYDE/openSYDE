//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Factor spin box for table (header)

   See cpp file for detailed description

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_OGESPXFACTORTABLE_H
#define C_OGESPXFACTORTABLE_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */

#include "C_OgeSpxFactor.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_gui_elements
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_OgeSpxFactorTable :
   public C_OgeSpxFactor
{
   Q_OBJECT

public:
   C_OgeSpxFactorTable(QWidget * const opc_Parent = NULL);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
