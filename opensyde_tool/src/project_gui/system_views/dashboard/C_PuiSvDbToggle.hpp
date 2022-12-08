//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       UI class for dashboard toggle data (header)

   See cpp file for detailed description

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_PUISVDBTOGGLE_H
#define C_PUISVDBTOGGLE_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "C_PuiSvDbWidgetBase.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_gui_logic
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_PuiSvDbToggle :
   public C_PuiSvDbWidgetBase
{
public:
   enum E_Type
   {
      eTYPE1,
      eTYPE2,
      eTYPE3
   };

   C_PuiSvDbToggle(void);

   void CalcHash(uint32_t & oru32_HashValue) const override;
   bool IsReadElement(void) const override;

   E_Type e_Type;
   bool q_State;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
