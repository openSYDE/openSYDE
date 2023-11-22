//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Base data class for write widgets
   \copyright   Copyright 2023 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_PUISVDBWRITEWIDGETBASE_HPP
#define C_PUISVDBWRITEWIDGETBASE_HPP

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "C_PuiSvDbWidgetBase.hpp"
#include "C_OscNodeDataPoolContent.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_gui_logic
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_PuiSvDbWriteWidgetBase :
   public C_PuiSvDbWidgetBase
{
public:
   C_PuiSvDbWriteWidgetBase();

   void CalcHash(uint32_t & oru32_HashValue) const override;

   enum E_WriteMode
   {
      eWM_MANUAL,
      eWM_ON_CHANGE
   };

   enum E_InitialValueModeType
   {
      eIVM_DISABLED,
      eIVM_SET_CONSTANT_VALUE,
      eIVM_READ_SERVER_VALUE
   };

   bool q_AutoWriteOnConnect;
   E_WriteMode e_ElementWriteMode;
   E_InitialValueModeType e_InitialValueMode;
   stw::opensyde_core::C_OscNodeDataPoolContent c_InitialValue;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
