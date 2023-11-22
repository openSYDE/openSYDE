//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Helper class for formatter to have the information about an invalid formatter config on initialization

   \copyright   Copyright 2022 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.hpp"

#include "stwtypes.hpp"
#include "TglUtils.hpp"
#include "C_OscUtils.hpp"
#include "C_GtGetText.hpp"
#include "C_SdNdeDpContentUtil.hpp"
#include "C_PuiSvDbDataElementDisplayFormatterConfig.hpp"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw::opensyde_core;
using namespace stw::opensyde_gui_logic;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

/* -- Global Variables ---------------------------------------------------------------------------------------------- */

/* -- Module Global Variables --------------------------------------------------------------------------------------- */

/* -- Module Global Function Prototypes ----------------------------------------------------------------------------- */

/* -- Implementation ------------------------------------------------------------------------------------------------ */

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default constructor
*/
//----------------------------------------------------------------------------------------------------------------------
C_PuiSvDbDataElementDisplayFormatterConfig::C_PuiSvDbDataElementDisplayFormatterConfig(void) :
   C_PuiSvDbDataElementDisplayFormatter(),
   q_IsValid(false),
   e_TypeCategory(C_PuiSvDbDataElementDisplayFormatterConfig::eINTEGER)
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Sets the base class configuration

   \param[in]       orc_Formatter     Base class with formatter configuration
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PuiSvDbDataElementDisplayFormatterConfig::SetDisplayFormatter(
   const C_PuiSvDbDataElementDisplayFormatter & orc_Formatter)
{
   this->q_IsActive = orc_Formatter.q_IsActive;
   this->c_FormatterString = orc_Formatter.c_FormatterString;
   this->q_IsValid = false;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Sets the current type for the element and checks if the current formatter configuration is valid

   Must be called before calling GetValueFormatted

   \param[in]       oe_Type     Current type of the element
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PuiSvDbDataElementDisplayFormatterConfig::SetType(
   const C_PuiSvDbDataElementDisplayFormatter::E_TypeCategory oe_Type)
{
   this->e_TypeCategory = oe_Type;
   this->q_IsValid = this->IsFormatterCompatible(oe_Type);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get the value of Dp content as formatted and scaled string

   If scaling is activated, an integer based value will be handled and converted as float before formatting

   \param[in]     orc_Value                       Dp content to convert, format and scale
   \param[in]     ou32_Index                      Array index in content (0 in case of no array)
   \param[in]     orc_Scaling                     Scaling configuration (Factor 1.0 and Offset 0.0 for deactivation)
   \param[out]    opf64_UnscaledValueAsFloat      Optional float with unscaled value
   \param[out]    opf64_ScaledValueAsFloat        Optional float with scaled value

   \return
   Formatted and scaled string
*/
//----------------------------------------------------------------------------------------------------------------------
QString C_PuiSvDbDataElementDisplayFormatterConfig::GetSingleValueContentFormatted(
   const C_PuiSvDbDataElementContent & orc_Value, const uint32_t ou32_Index,
   const C_PuiSvDbDataElementScaling & orc_Scaling, float64_t * const opf64_UnscaledValueAsFloat,
   float64_t * const opf64_ScaledValueAsFloat) const
{
   QString c_Return;
   float64_t f64_Temp = 0.0;

   if (((opf64_UnscaledValueAsFloat != NULL) || (opf64_ScaledValueAsFloat != NULL)) &&
       (this->e_TypeCategory != C_PuiSvDbDataElementDisplayFormatter::eSTRING))
   {
      // When needed, get the unscaled float value. this is independent
      C_SdNdeDpContentUtil::h_GetValueAsFloat64(orc_Value, f64_Temp, ou32_Index);

      if (opf64_UnscaledValueAsFloat != NULL)
      {
         *opf64_UnscaledValueAsFloat = f64_Temp;
      }

      if (opf64_ScaledValueAsFloat != NULL)
      {
         *opf64_ScaledValueAsFloat = C_OscUtils::h_GetValueScaled(f64_Temp, orc_Scaling.f64_Factor,
                                                                  orc_Scaling.f64_Offset);
      }
   }

   if (this->q_IsActive == false)
   {
      // Formatter deactivated. Convert in conventional way
      if (this->e_TypeCategory != C_PuiSvDbDataElementDisplayFormatter::eSTRING)
      {
         // Integer or float values
         // Scale the value
         C_SdNdeDpContentUtil::h_GetValueAsScaledString(orc_Value, orc_Scaling.f64_Factor, orc_Scaling.f64_Offset,
                                                        c_Return, ou32_Index);
      }
      else
      {
         // String should not happen on single value read element
         tgl_assert(false);
      }
   }
   else if (this->q_IsValid == true)
   {
      switch (this->e_TypeCategory)
      {
      case C_PuiSvDbDataElementDisplayFormatter::eINTEGER:
         {
            int64_t s64_Temp;

            // Get the value always as 64 bit value
            C_SdNdeDpContentUtil::h_GetAnyValueAsSint64(orc_Value, s64_Temp, ou32_Index);

            // Let do the formatter its work
            c_Return = this->GetValueFormatted(s64_Temp);
         }
         break;
      case C_PuiSvDbDataElementDisplayFormatter::eFLOAT:
         if ((opf64_UnscaledValueAsFloat == NULL) && (opf64_ScaledValueAsFloat == NULL))
         {
            // Value was not read as float already
            C_SdNdeDpContentUtil::h_GetValueAsFloat64(orc_Value, f64_Temp, ou32_Index);
         }

         // Scale the value
         if (opf64_ScaledValueAsFloat == NULL)
         {
            f64_Temp = C_OscUtils::h_GetValueScaled(f64_Temp, orc_Scaling.f64_Factor, orc_Scaling.f64_Offset);
         }
         else
         {
            // Was already scaled
            f64_Temp = *opf64_ScaledValueAsFloat;
         }

         // Let do the formatter its work
         c_Return = this->GetValueFormatted(f64_Temp);
         break;
      case C_PuiSvDbDataElementDisplayFormatter::eSTRING:
         // String should not happen on single value read element
         tgl_assert(false);
         break;
      default:
         break;
      }
   }
   else
   {
      c_Return = C_GtGetText::h_GetText("Invalid Formatter");
   }

   return c_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get the values of Dp content as formatted and scaled strings

   If scaling is activated, an integer based value will be handled and converted as float before formatting

   If it is an array, the size of the vector is > 1
   If it is no array, the size of the vector is 1
   If an error occurred, the size of the vector is 0

   Special case: If the element is configured as string (sint8 array): The return value has one string and the
   orc_UnscaledValueAsFloat array has one value with 0.0

   \param[in]     orc_Value                       Dp content to convert, format and scale
   \param[in]     orc_Scaling                     Scaling configuration (Factor 1.0 and Offset 0.0 for deactivation)
   \param[out]    orc_UnscaledValueAsFloat        Optional floats with unscaled values

   \return
   Formatted and scaled strings
*/
//----------------------------------------------------------------------------------------------------------------------
std::vector<QString> C_PuiSvDbDataElementDisplayFormatterConfig::GetValuesContentFormatted(
   const C_PuiSvDbDataElementContent & orc_Value, const C_PuiSvDbDataElementScaling & orc_Scaling,
   std::vector<float64_t> & orc_UnscaledValueAsFloat) const
{
   std::vector<QString> c_Return;

   orc_UnscaledValueAsFloat.clear();

   if (this->e_TypeCategory != C_PuiSvDbDataElementDisplayFormatter::eSTRING)
   {
      // When needed, get the unscaled float value. this is independent
      C_SdNdeDpContentUtil::h_GetValuesAsFloat64(orc_Value, orc_UnscaledValueAsFloat);
   }

   if (this->q_IsActive == false)
   {
      // Formatter deactivated. Convert in conventional way
      if (this->e_TypeCategory != C_PuiSvDbDataElementDisplayFormatter::eSTRING)
      {
         // Integer or float values
         // Scale the value
         C_SdNdeDpContentUtil::h_GetValuesAsScaledString(orc_Value, orc_Scaling.f64_Factor, orc_Scaling.f64_Offset,
                                                         c_Return);
      }
      else
      {
         // Special case string: No float as unscaled value
         std::vector<int8_t> c_VecValues;
         c_VecValues = orc_Value.GetValueArrS8();
         c_Return.push_back(C_SdNdeDpContentUtil::h_ConvertToString(c_VecValues));
      }
   }
   else if (this->q_IsValid == true)
   {
      uint32_t u32_ArrayCounter;
      switch (this->e_TypeCategory)
      {
      case C_PuiSvDbDataElementDisplayFormatter::eINTEGER:

         for (u32_ArrayCounter = 0U; u32_ArrayCounter < orc_Value.GetArraySize(); ++u32_ArrayCounter)
         {
            int64_t s64_Temp;
            // Get the value always as 64 bit value
            C_SdNdeDpContentUtil::h_GetAnyValueAsSint64(orc_Value, s64_Temp, u32_ArrayCounter);

            // Let do the formatter its work
            c_Return.push_back(this->GetValueFormatted(s64_Temp));
         }
         break;
      case C_PuiSvDbDataElementDisplayFormatter::eFLOAT:

         // Scale the value

         for (u32_ArrayCounter = 0U; u32_ArrayCounter < orc_Value.GetArraySize(); ++u32_ArrayCounter)
         {
            const float64_t f64_Temp = C_OscUtils::h_GetValueScaled(orc_UnscaledValueAsFloat[u32_ArrayCounter],
                                                                    orc_Scaling.f64_Factor, orc_Scaling.f64_Offset);
            // Let do the formatter its work
            c_Return.push_back(this->GetValueFormatted(f64_Temp));
         }
         break;
      case C_PuiSvDbDataElementDisplayFormatter::eSTRING:
         {
            // Special case string: No float as unscaled value
            QString c_Temp;
            std::vector<int8_t> c_VecValues;
            c_VecValues = orc_Value.GetValueArrS8();

            c_Temp = C_SdNdeDpContentUtil::h_ConvertToString(c_VecValues);
            c_Return.push_back(this->GetValueFormatted(c_Temp));
         }
         break;
      default:
         break;
      }
   }
   else
   {
      if (this->e_TypeCategory != C_PuiSvDbDataElementDisplayFormatter::eSTRING)
      {
         uint32_t u32_ArrayCounter;
         for (u32_ArrayCounter = 0U; u32_ArrayCounter < orc_Value.GetArraySize(); ++u32_ArrayCounter)
         {
            // For each array element an error return string
            c_Return.emplace_back(C_GtGetText::h_GetText("Invalid Formatter"));
         }
      }
      else
      {
         // Special case string: Only one string as return value
         c_Return.emplace_back(C_GtGetText::h_GetText("Invalid Formatter"));
      }
   }

   if (orc_UnscaledValueAsFloat.size() < c_Return.size())
   {
      orc_UnscaledValueAsFloat.resize(c_Return.size(), 0.0);
   }

   return c_Return;
}
