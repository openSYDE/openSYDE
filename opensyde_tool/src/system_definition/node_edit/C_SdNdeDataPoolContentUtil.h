//-----------------------------------------------------------------------------
/*!
   \file
   \brief       Utilitiy functions for C_OSCNodeDataPoolContent access (header)

   See cpp file for detailed description

   \implementation
   project     openSYDE
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     13.11.2017  STW/M.Echtler
   \endimplementation
*/
//-----------------------------------------------------------------------------
#ifndef C_SDNDEDATAPOOLCONTENTUTIL_H
#define C_SDNDEDATAPOOLCONTENTUTIL_H

/* -- Includes ------------------------------------------------------------- */
#include <QVariant>
#include "stwtypes.h"
#include "C_OSCNodeDataPoolContent.h"

/* -- Namespace ------------------------------------------------------------ */
namespace stw_opensyde_gui_logic
{
/* -- Global Constants ----------------------------------------------------- */

/* -- Types ---------------------------------------------------------------- */

class C_SdNdeDataPoolContentUtil
{
public:
   C_SdNdeDataPoolContentUtil(void);

   static stw_types::sint32 h_GetValueAsFloat64(const stw_opensyde_core::C_OSCNodeDataPoolContent & orc_Content,
                                                stw_types::float64 & orf64_Output);
   static void h_GetValuesAsFloat64(const stw_opensyde_core::C_OSCNodeDataPoolContent & orc_Content,
                                    std::vector<stw_types::float64> & orc_Output);
   static void h_GetAnyValueAsUint64(const stw_opensyde_core::C_OSCNodeDataPoolContent & orc_Content,
                                     stw_types::uint64 & oru64_Output, const stw_types::uint32 ou32_Index);
   static void h_GetAnyValueAsSint64(const stw_opensyde_core::C_OSCNodeDataPoolContent & orc_Content,
                                     stw_types::sint64 & ors64_Output, const stw_types::uint32 ou32_Index);
   static void h_SetValueInContent(const stw_types::float64 of64_Value,
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
   static void h_ZeroContent(stw_opensyde_core::C_OSCNodeDataPoolContent & orc_Content);
   static stw_types::sint32 h_SimpleConvertToVariant(const stw_opensyde_core::C_OSCNodeDataPoolContent & orc_Content,
                                                     QVariant & orc_Variant);
   static stw_types::sint32 h_SimpleConvertFromVariant(const QVariant & orc_Variant,
                                                       stw_opensyde_core::C_OSCNodeDataPoolContent & orc_Content);
   static void h_GetValuesAsScaledString(const stw_opensyde_core::C_OSCNodeDataPoolContent & orc_Content,
                                         const stw_types::float64 of64_Factor, const stw_types::float64 of64_Offset,
                                         std::vector<QString> & orc_Output);
   static void h_GetValuesAsScaledCombinedString(const stw_opensyde_core::C_OSCNodeDataPoolContent & orc_Content,
                                                 const stw_types::float64 of64_Factor,
                                                 const stw_types::float64 of64_Offset, QString & orc_Output);
   static QString h_CombineString(const std::vector<QString> & orc_Input);
   static void h_GetValueAsScaledString(const stw_opensyde_core::C_OSCNodeDataPoolContent & orc_Content,
                                        const stw_types::float64 of64_Factor, const stw_types::float64 of64_Offset,
                                        QString & orc_Output, const stw_types::uint32 ou32_Index);
   static void h_InitMax(stw_opensyde_core::C_OSCNodeDataPoolContent & orc_Content);
   static void h_InitMin(stw_opensyde_core::C_OSCNodeDataPoolContent & orc_Content);

   // enum indicates where a specific value should be set to
   enum E_SetValue
   {
      eLEAVE_VALUE, // if in range
      eTO_ZERO      //    -"-
   };

   // enum indicates where a specific value was set to
   enum E_ValueChangedTo
   {
      eNO_CHANGE,
      eZERO,
      eMIN,
      eMAX
   };

   // set a value in min and max range
   static stw_types::sint32 h_SetValueInMinMaxRange(const stw_opensyde_core::C_OSCNodeDataPoolContent & orc_Min,
                                                    const stw_opensyde_core::C_OSCNodeDataPoolContent & orc_Max,
                                                    stw_opensyde_core::C_OSCNodeDataPoolContent & orc_Value,
                                                    C_SdNdeDataPoolContentUtil::E_ValueChangedTo & ore_ValueChangedTo, const C_SdNdeDataPoolContentUtil::E_SetValue oe_SetValue =
                                                       E_SetValue::eLEAVE_VALUE);
   static QVariant h_ConvertContentToGeneric(const stw_opensyde_core::C_OSCNodeDataPoolContent & orc_Input,
                                             const stw_types::uint32 & oru32_Index);
   static QVariant h_ConvertScaledContentToGeneric(const stw_opensyde_core::C_OSCNodeDataPoolContent & orc_Input,
                                                   const stw_types::float64 of64_Factor,
                                                   const stw_types::float64 of64_Offset,
                                                   const stw_types::uint32 & oru32_Index);
   static stw_types::sint32 h_SetDataVariableFromGeneric(const QVariant & orc_Value,
                                                         stw_opensyde_core::C_OSCNodeDataPoolContent & orc_Output,
                                                         const stw_types::uint32 & oru32_Index);
   static stw_types::sint32 h_SetDataVariableFromGenericWithScaling(const QVariant & orc_Value,
                                                                    stw_opensyde_core::C_OSCNodeDataPoolContent & orc_Output, const stw_types::float64 of64_Factor, const stw_types::float64 of64_Offset, const stw_types::uint32 & oru32_Index);
   static QString h_GetAllContentAsString(const stw_opensyde_core::C_OSCNodeDataPoolContent & orc_Input);
   static stw_types::sint32 h_SetAllContentFromString(const QString & orc_Input,
                                                      stw_opensyde_core::C_OSCNodeDataPoolContent & orc_Output);

private:
   static const stw_types::float64 mhf64_Epsilon;
};

/* -- Extern Global Variables ---------------------------------------------- */
} //end of namespace

#endif
