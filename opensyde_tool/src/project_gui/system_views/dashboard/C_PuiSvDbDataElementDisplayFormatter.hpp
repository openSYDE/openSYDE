//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Store display formatter settings and handle functionality
   \copyright   Copyright 2022 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_PUISVDBDATAELEMENTDISPLAYFORMATTER_H
#define C_PUISVDBDATAELEMENTDISPLAYFORMATTER_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <QString>

#include "stwtypes.hpp"

#include "C_PuiSvDbDataElementContent.hpp"
#include "C_PuiSvDbDataElementScaling.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_gui_logic
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_PuiSvDbDataElementDisplayFormatter
{
public:
   enum E_TypeCategory
   {
      eINTEGER,
      eFLOAT,
      eSTRING
   };

   C_PuiSvDbDataElementDisplayFormatter();
   virtual ~C_PuiSvDbDataElementDisplayFormatter();

   virtual void CalcHash(uint32_t & oru32_HashValue) const;

   bool IsFormatterCompatible(const E_TypeCategory oe_Category) const;

   QString GetValueFormatted(const int64_t os64_Value) const;
   QString GetValueFormatted(const float64_t of64_Value) const;
   QString GetValueFormatted(const QString & orc_Value) const;

   static E_TypeCategory h_GetTypeCategory(const C_PuiSvDbDataElementContent & orc_Content,
                                           const C_PuiSvDbDataElementScaling & orc_Scaling,
                                           const bool oq_InterpretedAsString);

   bool q_IsActive;
   QString c_FormatterString;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
