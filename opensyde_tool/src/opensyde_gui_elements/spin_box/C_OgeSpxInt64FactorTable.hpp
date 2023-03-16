//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Int64 spin box which should avoid a zero value, table style (header)

   See cpp file for detailed description

   \copyright   Copyright 2018 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_OGESPXINT64FACTORTABLE_H
#define C_OGESPXINT64FACTORTABLE_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "C_OgeSpxInt64Factor.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_gui_elements
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_OgeSpxInt64FactorTable :
   public C_OgeSpxInt64Factor
{
   Q_OBJECT

public:
   C_OgeSpxInt64FactorTable(QWidget * const opc_Parent = NULL, const bool & orq_IsUnsigned = false);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
