//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Utilitiy functions for C_OSCNodeDataPoolContent access (header)

   See cpp file for detailed description

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_SDNDEDPCONTENTUTIL_H
#define C_SDNDEDPCONTENTUTIL_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <QVariant>
#include "stwtypes.h"
#include "C_OSCNodeDataPoolContent.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw_opensyde_gui_logic
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_SdNdeDpContentUtil
{
public:
   C_SdNdeDpContentUtil(void);

   static stw_types::sint32 h_GetValueAsFloat64(const stw_opensyde_core::C_OSCNodeDataPoolContent & orc_Content,
                                                stw_types::float64 & orf64_Output, const stw_types::uint32 ou32_Index);
   static void h_GetValuesAsFloat64(const stw_opensyde_core::C_OSCNodeDataPoolContent & orc_Content,
                                    std::vector<stw_types::float64> & orc_Output);
   static void h_GetAnyValueAsUint64(const stw_opensyde_core::C_OSCNodeDataPoolContent & orc_Content,
                                     stw_types::uint64 & oru64_Output, const stw_types::uint32 ou32_Index);
   static void h_GetAnyValueAsSint64(const stw_opensyde_core::C_OSCNodeDataPoolContent & orc_Content,
                                     stw_types::sint64 & ors64_Output, const stw_types::uint32 ou32_Index);
   static void h_SetValueInContentS64(const stw_types::sint64 os64_Value,
                                      stw_opensyde_core::C_OSCNodeDataPoolContent & orc_Content,
                                      const stw_types::uint32 ou32_ArrayIndex = 0U);
   static void h_SetScaledValueInContent(const stw_types::float64 of64_ScaledValue,
                                         stw_opensyde_core::C_OSCNodeDataPoolContent & orc_Content,
                                         const stw_types::float64 of64_Factor, const stw_types::float64 of64_Offset,
                                         const stw_types::uint32 ou32_ArrayIndex = 0U);
   static stw_types::sint32 h_GetNumberOfAvailableSteps(const stw_opensyde_core::C_OSCNodeDataPoolContent & orc_Min,
                                                        const stw_opensyde_core::C_OSCNodeDataPoolContent & orc_Max,
                                                        stw_types::uint64 & oru64_Steps,
                                                        const stw_types::uint32 ou32_Index = 0);
   static stw_types::sint32 h_GetMinMaxAfterScaling(const stw_opensyde_core::C_OSCNodeDataPoolContent & orc_Min,
                                                    const stw_opensyde_core::C_OSCNodeDataPoolContent & orc_Max,
                                                    const stw_types::float64 of64_Factor,
                                                    const stw_types::float64 of64_Offset,
                                                    stw_opensyde_core::C_OSCNodeDataPoolContent & orc_NewMin,
                                                    stw_opensyde_core::C_OSCNodeDataPoolContent & orc_NewMax,
                                                    const stw_types::uint32 ou32_Index);
   static stw_types::sint32 h_GetMinimalTypeAfterScaling(const stw_opensyde_core::C_OSCNodeDataPoolContent & orc_Min,
                                                         const stw_opensyde_core::C_OSCNodeDataPoolContent & orc_Max,
                                                         const stw_types::float64 of64_Factor,
                                                         const stw_types::float64 of64_Offset,
                                                         std::vector<stw_opensyde_core::C_OSCNodeDataPoolContent::E_Type> & orc_Types);
   static stw_opensyde_core::C_OSCNodeDataPoolContent::E_Type h_GetMinimumTypeForRange(
      const stw_types::float64 of64_Min, const stw_types::float64 of64_Max, const stw_types::uint64 ou64_Steps);
   static stw_types::sint32 h_SimpleConvertToVariant(const stw_opensyde_core::C_OSCNodeDataPoolContent & orc_Content,
                                                     QVariant & orc_Variant);
   static stw_types::sint32 h_SimpleConvertFromVariant(const QVariant & orc_Variant,
                                                       stw_opensyde_core::C_OSCNodeDataPoolContent & orc_Content);
   static void h_GetValuesAsScaledString(const stw_opensyde_core::C_OSCNodeDataPoolContent & orc_Content,
                                         const stw_types::float64 of64_Factor, const stw_types::float64 of64_Offset,
                                         std::vector<QString> & orc_Output, const bool oq_AllowRangeAdaptation = true);
   static void h_GetValuesAsScaledCombinedString(const stw_opensyde_core::C_OSCNodeDataPoolContent & orc_Content,
                                                 const stw_types::float64 of64_Factor,
                                                 const stw_types::float64 of64_Offset, QString & orc_Output,
                                                 const bool oq_AllowRangeAdaptation = true);
   static QString h_CombineString(const std::vector<QString> & orc_Input);
   static void h_GetValueAsScaledString(const stw_opensyde_core::C_OSCNodeDataPoolContent & orc_Content,
                                        const stw_types::float64 of64_Factor, const stw_types::float64 of64_Offset,
                                        QString & orc_Output, const stw_types::uint32 ou32_Index,
                                        const bool oq_AllowRangeAdaptation = true);
   static void h_InitMax(stw_opensyde_core::C_OSCNodeDataPoolContent & orc_Content);
   static void h_InitMin(stw_opensyde_core::C_OSCNodeDataPoolContent & orc_Content);
   static void h_InitMaxForSignal(stw_opensyde_core::C_OSCNodeDataPoolContent & orc_Content,
                                  const stw_types::uint16 ou16_BitLength);
   static void h_InitMinForSignal(stw_opensyde_core::C_OSCNodeDataPoolContent & orc_Content,
                                  const stw_types::uint16 ou16_BitLength);
   static QVariant h_ConvertContentToGeneric(const stw_opensyde_core::C_OSCNodeDataPoolContent & orc_Input,
                                             const stw_types::uint32 & oru32_Index, const stw_types::sintn osn_Role  =
                                                static_cast<stw_types::sintn>(Qt::EditRole));
   static QVariant h_ConvertScaledContentToGeneric(const stw_opensyde_core::C_OSCNodeDataPoolContent & orc_Input,
                                                   const stw_types::float64 of64_Factor,
                                                   const stw_types::float64 of64_Offset,
                                                   const stw_types::uint32 & oru32_Index,
                                                   const bool oq_AllowRangeAdaptation = true, const stw_types::sintn osn_Role  =
                                                      static_cast<stw_types::sintn>(Qt::EditRole));
   static stw_types::sint32 h_SetDataVariableFromGeneric(const QVariant & orc_Value,
                                                         stw_opensyde_core::C_OSCNodeDataPoolContent & orc_Output,
                                                         const stw_types::uint32 & oru32_Index);
   static stw_types::sint32 h_SetDataVariableFromGenericWithScaling(const QVariant & orc_Value,
                                                                    stw_opensyde_core::C_OSCNodeDataPoolContent & orc_Output, const stw_types::float64 of64_Factor, const stw_types::float64 of64_Offset, const stw_types::uint32 & oru32_Index);
   static QString h_GetAllContentAsString(const stw_opensyde_core::C_OSCNodeDataPoolContent & orc_Input);
   static stw_types::sint32 h_SetAllContentFromString(const QString & orc_Input,
                                                      stw_opensyde_core::C_OSCNodeDataPoolContent & orc_Output);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif
