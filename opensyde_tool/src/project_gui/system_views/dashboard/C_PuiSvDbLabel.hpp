//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       UI class for dashboard label data (header)

   See cpp file for detailed description

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_PUISVDBLABEL_H
#define C_PUISVDBLABEL_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <QString>
#include "C_PuiSvDbWidgetBase.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_gui_logic
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_PuiSvDbLabel :
   public C_PuiSvDbWidgetBase
{
public:
   enum E_Type
   {
      eDEFAULT,
      eTRANSPARENT
   };

   C_PuiSvDbLabel(void);

   void CalcHash(uint32_t & oru32_HashValue) const override;
   bool IsReadElement(void) const override;

   QString c_Caption;
   E_Type e_Type;
   bool q_ShowCaption;
   bool q_ShowUnit;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
