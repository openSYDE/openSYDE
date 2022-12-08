//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Helper class for formatter to have the information about an invalid formatter config on initialization
   \copyright   Copyright 2022 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_PUISVDBDATAELEMENTDISPLAYFORMATTERCONFIG_H
#define C_PUISVDBDATAELEMENTDISPLAYFORMATTERCONFIG_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "C_PuiSvDbDataElementDisplayFormatter.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_gui_logic
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_PuiSvDbDataElementDisplayFormatterConfig :
   public C_PuiSvDbDataElementDisplayFormatter
{
public:
   C_PuiSvDbDataElementDisplayFormatterConfig(void);

   void SetDisplayFormatter(const C_PuiSvDbDataElementDisplayFormatter & orc_Formatter);
   void SetType(const E_TypeCategory oe_Type);

   QString GetSingleValueContentFormatted(const C_PuiSvDbDataElementContent & orc_Value, const uint32_t ou32_Index,
                                          const C_PuiSvDbDataElementScaling & orc_Scaling,
                                          float64_t * const opf64_UnscaledValueAsFloat) const;
   std::vector<QString> GetValuesContentFormatted(const C_PuiSvDbDataElementContent & orc_Value,
                                                  const C_PuiSvDbDataElementScaling & orc_Scaling,
                                                  std::vector<float64_t> & orc_UnscaledValueAsFloat) const;

   bool q_IsValid;
   E_TypeCategory e_TypeCategory;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
