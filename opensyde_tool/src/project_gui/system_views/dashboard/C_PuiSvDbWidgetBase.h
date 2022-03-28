//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       UI base class for dashboard widget data (header)

   See cpp file for detailed description

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_PUISVDBWIDGETBASE_H
#define C_PUISVDBWIDGETBASE_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <vector>

#include "stwtypes.h"

#include "C_PuiBsBox.h"
#include "C_PuiSvDbNodeDataElementConfig.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw_opensyde_gui_logic
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_PuiSvDbWidgetBase :
   public C_PuiBsBox
{
public:
   enum E_Style
   {
      eOPENSYDE,
      eOPENSYDE_2,
      eFLAT,
      eSKEUOMORPH
   };

   enum E_WriteMode
   {
      eWM_MANUAL,
      eWM_ON_CHANGE
   };

   C_PuiSvDbWidgetBase(void);

   virtual void CalcHash(stw_types::uint32 & oru32_HashValue) const override;
   virtual bool IsReadElement(void) const = 0;
   stw_types::sint32 GetIndex(const C_PuiSvDbNodeDataPoolListElementId & orc_Id, stw_types::uint32 & oru32_Index) const;
   virtual stw_types::sint32 RemoveElement(const stw_types::uint32 & oru32_Index);
   void RemoveAllReferencesToElementId(const C_PuiSvDbNodeDataPoolListElementId & orc_DataElementId);

   std::vector<C_PuiSvDbNodeDataElementConfig> c_DataPoolElementsConfig;
   E_WriteMode e_ElementWriteMode;
   E_Style e_DisplayStyle;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif
