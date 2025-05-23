//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Spin box group style for table (header)

   See cpp file for detailed description

   \copyright   Copyright 2019 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_CAMOGEWISPINBOXGROUPTABLE_HPP
#define C_CAMOGEWISPINBOXGROUPTABLE_HPP

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "C_OgeWiSpinBoxGroup.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_gui_elements
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_CamOgeWiSpinBoxGroupTable :
   public C_OgeWiSpinBoxGroup
{
   Q_OBJECT

public:
   explicit C_CamOgeWiSpinBoxGroupTable(QWidget * const opc_Parent = NULL);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
