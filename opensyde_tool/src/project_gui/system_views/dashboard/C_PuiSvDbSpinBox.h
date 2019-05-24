//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       UI class for dashboard spin box data (header)

   See cpp file for detailed description

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_PUISVDBSPINBOX_H
#define C_PUISVDBSPINBOX_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "C_PuiSvDbWidgetBase.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw_opensyde_gui_logic
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_PuiSvDbSpinBox :
   public C_PuiSvDbWidgetBase
{
public:
   enum E_Type
   {
      eTYPE1,
      eTYPE2
   };

   C_PuiSvDbSpinBox(void);

   virtual void CalcHash(stw_types::uint32 & oru32_HashValue) const override;
   virtual bool IsReadElement(void) const;

   E_Type e_Type;
   bool q_ShowUnit;
   stw_opensyde_core::C_OSCNodeDataPoolContent c_Value;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif
