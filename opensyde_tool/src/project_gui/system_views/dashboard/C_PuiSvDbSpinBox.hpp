//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       UI class for dashboard spin box data (header)

   See cpp file for detailed description

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_PUISVDBSPINBOX_HPP
#define C_PUISVDBSPINBOX_HPP

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "C_PuiSvDbWriteWidgetBase.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_gui_logic
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_PuiSvDbSpinBox :
   public C_PuiSvDbWriteWidgetBase
{
public:
   enum E_Type
   {
      eTYPE1,
      eTYPE2
   };

   C_PuiSvDbSpinBox(void);

   void CalcHash(uint32_t & oru32_HashValue) const override;
   bool IsReadElement(void) const override;

   E_Type e_Type;
   bool q_ShowUnit;
   stw::opensyde_core::C_OscNodeDataPoolContent c_Value;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
