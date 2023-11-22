//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       UI class for dashboard progress bar data (header)

   See cpp file for detailed description

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_PUISVDBPROGRESSBAR_HPP
#define C_PUISVDBPROGRESSBAR_HPP

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "C_PuiSvDbWidgetBase.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_gui_logic
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_PuiSvDbProgressBar :
   public C_PuiSvDbWidgetBase
{
public:
   enum E_Type
   {
      eTYPE_1,
      eTYPE_2,
      eTYPE_3
   };

   enum E_Alignment
   {
      eTOP,
      eRIGHT,
      eBOTTOM,
      eLEFT
   };

   C_PuiSvDbProgressBar();

   void CalcHash(uint32_t & oru32_HashValue) const override;
   bool IsReadElement(void) const override;

   E_Type e_Type;
   E_Alignment e_Alignment;
   bool q_ShowMinMax;
   bool q_ShowUnit;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
