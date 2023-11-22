//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Int 64 spin box field for table (header)

   See cpp file for detailed description

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_OGESPXINT64TABLE_HPP
#define C_OGESPXINT64TABLE_HPP

/* -- Includes ------------------------------------------------------------------------------------------------------ */

#include "C_OgeSpxInt64ToolTipBase.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_gui_elements
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_OgeSpxInt64Table :
   public C_OgeSpxInt64ToolTipBase
{
   Q_OBJECT

public:
   C_OgeSpxInt64Table(QWidget * const opc_Parent = NULL, const bool & orq_IsUnsigned = false);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
