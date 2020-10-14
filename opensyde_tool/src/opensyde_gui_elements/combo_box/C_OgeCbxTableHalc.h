//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Styled combobox for HALC parameter configuration.
   \copyright   Copyright 2020 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_OGECBXTABLEHALC_H
#define C_OGECBXTABLEHALC_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "C_OgeCbxTableBase.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw_opensyde_gui_elements
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_OgeCbxTableHalc :
   public C_OgeCbxTableBase
{
   Q_OBJECT

public:
   explicit C_OgeCbxTableHalc(QWidget * const opc_Parent = NULL);
   ~C_OgeCbxTableHalc(void);

   //Avoid call
   C_OgeCbxTableHalc(const C_OgeCbxTableHalc &);
   C_OgeCbxTableHalc & operator =(const C_OgeCbxTableHalc &);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif
