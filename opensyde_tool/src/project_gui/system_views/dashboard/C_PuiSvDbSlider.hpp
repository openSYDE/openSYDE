//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       UI class for dashboard slider data (header)

   See cpp file for detailed description

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_PUISVDBSLIDER_HPP
#define C_PUISVDBSLIDER_HPP

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "C_PuiSvDbWriteWidgetBase.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_gui_logic
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_PuiSvDbSlider :
   public C_PuiSvDbWriteWidgetBase
{
public:
   enum E_Type
   {
      eTYPE_SMALL_COLOR_1,
      eTYPE_BIG_COLOR_1,
      eTYPE_SMALL_COLOR_2,
      eTYPE_BIG_COLOR_2
   };

   C_PuiSvDbSlider(void);

   void CalcHash(uint32_t & oru32_HashValue) const override;
   bool IsReadElement(void) const override;

   E_Type e_Type;
   bool q_ShowMinMax;
   stw::opensyde_core::C_OscNodeDataPoolContent c_Value;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
