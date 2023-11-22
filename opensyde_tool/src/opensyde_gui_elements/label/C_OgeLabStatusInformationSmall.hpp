//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Small label for status informations (header)

   (note: main module description should be in .cpp file)

   \copyright   Copyright 2016 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_OGELABSTATUSINFORMATIONSMALL_HPP
#define C_OGELABSTATUSINFORMATIONSMALL_HPP

/* -- Includes ------------------------------------------------------------------------------------------------------ */

#include "C_OgeLabToolTipBase.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_gui_elements
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_OgeLabStatusInformationSmall :
   public C_OgeLabToolTipBase
{
   Q_OBJECT

public:
   C_OgeLabStatusInformationSmall(QWidget * const opc_Parent = NULL);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
