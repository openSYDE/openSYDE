//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Styled combobox for HALC parameter configuration.
   \copyright   Copyright 2020 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_OGECBXTABLEHALC_HPP
#define C_OGECBXTABLEHALC_HPP

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "C_OgeCbxTableBase.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_gui_elements
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_OgeCbxTableHalc :
   public C_OgeCbxTableBase
{
   Q_OBJECT

public:
   explicit C_OgeCbxTableHalc(QWidget * const opc_Parent = NULL);
   ~C_OgeCbxTableHalc(void) override;

private:
   //Avoid call
   C_OgeCbxTableHalc(const C_OgeCbxTableHalc &);
   C_OgeCbxTableHalc & operator =(const C_OgeCbxTableHalc &) &;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
