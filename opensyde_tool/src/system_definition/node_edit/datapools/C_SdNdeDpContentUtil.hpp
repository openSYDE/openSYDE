//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Utilitiy functions for C_OscNodeDataPoolContent access (header)

   See cpp file for detailed description

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_SDNDEDPCONTENTUTIL_HPP
#define C_SDNDEDPCONTENTUTIL_HPP

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <QVariant>
#include "stwtypes.hpp"
#include "C_OscNodeDataPoolContent.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_gui_logic
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_SdNdeDpContentUtil
{
public:
   C_SdNdeDpContentUtil(void);

   static int32_t h_GetValueAsFloat64(const stw::opensyde_core::C_OscNodeDataPoolContent & orc_Content,
                                      float64_t & orf64_Output, const uint32_t ou32_Index);
   static void h_GetValuesAsFloat64(const stw::opensyde_core::C_OscNodeDataPoolContent & orc_Content,
                                    std::vector<float64_t> & orc_Output);
   static void h_GetAnyValueAsUint64(const stw::opensyde_core::C_OscNodeDataPoolContent & orc_Content,
                                     uint64_t & oru64_Output, const uint32_t ou32_Index);
   static void h_GetAnyValueAsSint64(const stw::opensyde_core::C_OscNodeDataPoolContent & orc_Content,
                                     int64_t & ors64_Output, const uint32_t ou32_Index);
   static void h_SetValueInContentS64(const int64_t os64_Value,
                                      stw::opensyde_core::C_OscNodeDataPoolContent & orc_Content,
                                      const uint32_t ou32_ArrayIndex = 0U);
   static void h_SetScaledValueInContent(const float64_t of64_ScaledValue,
                                         stw::opensyde_core::C_OscNodeDataPoolContent & orc_Content,
                                         const float64_t of64_Factor, const float64_t of64_Offset,
                                         const uint32_t ou32_ArrayIndex = 0U);
   static int32_t h_GetNumberOfAvailableSteps(const stw::opensyde_core::C_OscNodeDataPoolContent & orc_Min,
                                              const stw::opensyde_core::C_OscNodeDataPoolContent & orc_Max,
                                              uint64_t & oru64_Steps, const uint32_t ou32_Index = 0);
   static int32_t h_GetMinMaxAfterScaling(const stw::opensyde_core::C_OscNodeDataPoolContent & orc_Min,
                                          const stw::opensyde_core::C_OscNodeDataPoolContent & orc_Max,
                                          const float64_t of64_Factor, const float64_t of64_Offset,
                                          stw::opensyde_core::C_OscNodeDataPoolContent & orc_NewMin,
                                          stw::opensyde_core::C_OscNodeDataPoolContent & orc_NewMax,
                                          const uint32_t ou32_Index);
   static int32_t h_GetMinimalTypeAfterScaling(const stw::opensyde_core::C_OscNodeDataPoolContent & orc_Min,
                                               const stw::opensyde_core::C_OscNodeDataPoolContent & orc_Max,
                                               const float64_t of64_Factor, const float64_t of64_Offset,
                                               std::vector<stw::opensyde_core::C_OscNodeDataPoolContent::E_Type> & orc_Types);
   static stw::opensyde_core::C_OscNodeDataPoolContent::E_Type h_GetMinimumTypeForRange(const float64_t of64_Min,
                                                                                        const float64_t of64_Max,
                                                                                        const uint64_t ou64_Steps);
   static int32_t h_SimpleConvertToVariant(const stw::opensyde_core::C_OscNodeDataPoolContent & orc_Content,
                                           QVariant & orc_Variant);
   static int32_t h_SimpleConvertFromVariant(const QVariant & orc_Variant,
                                             stw::opensyde_core::C_OscNodeDataPoolContent & orc_Content);
   static void h_GetValuesAsScaledString(const stw::opensyde_core::C_OscNodeDataPoolContent & orc_Content,
                                         const float64_t of64_Factor, const float64_t of64_Offset,
                                         std::vector<QString> & orc_Output, const bool oq_AllowRangeAdaptation = true);
   static void h_GetValuesAsScaledCombinedString(const stw::opensyde_core::C_OscNodeDataPoolContent & orc_Content,
                                                 const float64_t of64_Factor, const float64_t of64_Offset,
                                                 QString & orc_Output, const bool oq_AllowRangeAdaptation = true);
   static QString h_CombineString(const std::vector<QString> & orc_Input);
   static void h_GetValueAsScaledString(const stw::opensyde_core::C_OscNodeDataPoolContent & orc_Content,
                                        const float64_t of64_Factor, const float64_t of64_Offset, QString & orc_Output,
                                        const uint32_t ou32_Index, const bool oq_AllowRangeAdaptation = true);
   static void h_InitMax(stw::opensyde_core::C_OscNodeDataPoolContent & orc_Content);
   static void h_InitMin(stw::opensyde_core::C_OscNodeDataPoolContent & orc_Content);
   static void h_InitMaxForSignal(stw::opensyde_core::C_OscNodeDataPoolContent & orc_Content,
                                  const uint16_t ou16_BitLength);
   static void h_InitMinForSignal(stw::opensyde_core::C_OscNodeDataPoolContent & orc_Content,
                                  const uint16_t ou16_BitLength);
   static QVariant h_ConvertContentToGeneric(const stw::opensyde_core::C_OscNodeDataPoolContent & orc_Input,
                                             const uint32_t & oru32_Index, const int32_t os32_Role  =
                                                static_cast<int32_t>(Qt::EditRole));
   static QVariant h_ConvertScaledContentToGeneric(const stw::opensyde_core::C_OscNodeDataPoolContent & orc_Input,
                                                   const float64_t of64_Factor, const float64_t of64_Offset,
                                                   const uint32_t & oru32_Index,
                                                   const bool oq_AllowRangeAdaptation = true, const int32_t os32_Role  =
                                                      static_cast<int32_t>(Qt::EditRole));
   static int32_t h_SetDataVariableFromGeneric(const QVariant & orc_Value,
                                               stw::opensyde_core::C_OscNodeDataPoolContent & orc_Output,
                                               const uint32_t & oru32_Index);
   static int32_t h_SetDataVariableFromGenericWithScaling(const QVariant & orc_Value,
                                                          stw::opensyde_core::C_OscNodeDataPoolContent & orc_Output,
                                                          const float64_t of64_Factor, const float64_t of64_Offset,
                                                          const uint32_t & oru32_Index);
   static QString h_GetAllContentAsString(const stw::opensyde_core::C_OscNodeDataPoolContent & orc_Input);
   static int32_t h_SetAllContentFromString(const QString & orc_Input,
                                            stw::opensyde_core::C_OscNodeDataPoolContent & orc_Output);
   static QString h_ConvertToString(const stw::opensyde_core::C_OscNodeDataPoolContent & orc_Data);
   static QString h_ConvertToString(const std::vector<int8_t> & orc_Data);
   static void h_SetString(const QString & orc_String, stw::opensyde_core::C_OscNodeDataPoolContent & orc_Output);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
