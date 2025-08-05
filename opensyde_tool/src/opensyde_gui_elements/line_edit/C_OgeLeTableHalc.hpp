//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Styled lineedit for HALC parameter configuration.
   \copyright   Copyright 2020 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_OGELETABLEHALC_HPP
#define C_OGELETABLEHALC_HPP

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "C_TblEditLineEditBase.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_gui_elements
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_OgeLeTableHalc :
   public stw::opensyde_gui::C_TblEditLineEditBase
{
   Q_OBJECT

public:
   C_OgeLeTableHalc(QWidget * const opc_Parent = NULL);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
