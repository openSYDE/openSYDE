//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Data pool content utility functions

   Data pool content utility functions

   \copyright   Copyright 2019 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.h"

#include <cmath>
#include <limits>

#include "stwerrors.h"
#include "C_OSCNodeDataPoolContentUtil.h"
#include "TGLUtils.h"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw_types;
using namespace stw_errors;
using namespace stw_opensyde_core;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */
const float64 C_OSCNodeDataPoolContentUtil::mhf64_EPSILON = 1e-5; // to check for a 6 digit precision (openSYDE default)

/* -- Types --------------------------------------------------------------------------------------------------------- */

/* -- Global Variables ---------------------------------------------------------------------------------------------- */

/* -- Module Global Variables --------------------------------------------------------------------------------------- */

/* -- Module Global Function Prototypes ----------------------------------------------------------------------------- */

/* -- Implementation ------------------------------------------------------------------------------------------------ */

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Set input content values to zero

   \param[in,out] orc_Content Content
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OSCNodeDataPoolContentUtil::h_ZeroContent(C_OSCNodeDataPoolContent & orc_Content)
{
   if (orc_Content.GetArray() == false)
   {
      switch (orc_Content.GetType())
      {
      case C_OSCNodeDataPoolContent::eUINT8:
         orc_Content.SetValueU8(0);
         break;
      case C_OSCNodeDataPoolContent::eUINT16:
         orc_Content.SetValueU16(0);
         break;
      case C_OSCNodeDataPoolContent::eUINT32:
         orc_Content.SetValueU32(0UL);
         break;
      case C_OSCNodeDataPoolContent::eUINT64:
         orc_Content.SetValueU64(0ULL);
         break;
      case C_OSCNodeDataPoolContent::eSINT8:
         orc_Content.SetValueS8(0);
         break;
      case C_OSCNodeDataPoolContent::eSINT16:
         orc_Content.SetValueS16(0);
         break;
      case C_OSCNodeDataPoolContent::eSINT32:
         orc_Content.SetValueS32(0L);
         break;
      case C_OSCNodeDataPoolContent::eSINT64:
         orc_Content.SetValueS64(0LL);
         break;
      case C_OSCNodeDataPoolContent::eFLOAT32:
         orc_Content.SetValueF32(0.0F);
         break;
      case C_OSCNodeDataPoolContent::eFLOAT64:
         orc_Content.SetValueF64(0.0);
         break;
      default:
         break;
      }
   }
   else
   {
      for (uint32 u32_ItElem = 0; u32_ItElem < orc_Content.GetArraySize(); ++u32_ItElem)
      {
         switch (orc_Content.GetType())
         {
         case C_OSCNodeDataPoolContent::eUINT8:
            orc_Content.SetValueAU8Element(0, u32_ItElem);
            break;
         case C_OSCNodeDataPoolContent::eUINT16:
            orc_Content.SetValueAU16Element(0, u32_ItElem);
            break;
         case C_OSCNodeDataPoolContent::eUINT32:
            orc_Content.SetValueAU32Element(0UL, u32_ItElem);
            break;
         case C_OSCNodeDataPoolContent::eUINT64:
            orc_Content.SetValueAU64Element(0ULL, u32_ItElem);
            break;
         case C_OSCNodeDataPoolContent::eSINT8:
            orc_Content.SetValueAS8Element(0, u32_ItElem);
            break;
         case C_OSCNodeDataPoolContent::eSINT16:
            orc_Content.SetValueAS16Element(0, u32_ItElem);
            break;
         case C_OSCNodeDataPoolContent::eSINT32:
            orc_Content.SetValueAS32Element(0L, u32_ItElem);
            break;
         case C_OSCNodeDataPoolContent::eSINT64:
            orc_Content.SetValueAS64Element(0LL, u32_ItElem);
            break;
         case C_OSCNodeDataPoolContent::eFLOAT32:
            orc_Content.SetValueAF32Element(0.0F, u32_ItElem);
            break;
         case C_OSCNodeDataPoolContent::eFLOAT64:
            orc_Content.SetValueAF64Element(0.0, u32_ItElem);
            break;
         default:
            break;
         }
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Sets a float value in the content class

   The of64_Value value will be casted to the current type of the orc_Content class.
   If the orc_Content holds an array, the value will be set to the position ou32_ArrayIndex.

   Hint: for integer datatypes an offset for rounding is added, so that the decimal place is
         not lost by casting.

   \param[in]     of64_Value      Value to set and cast
   \param[in,out] orc_Content     Content class with configured type and array configuration
   \param[in]     ou32_ArrayIndex Optional index for array position if content class has an array
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OSCNodeDataPoolContentUtil::h_SetValueInContent(const float64 of64_Value, C_OSCNodeDataPoolContent & orc_Content,
                                                       const uint32 ou32_ArrayIndex)
{
   bool q_ValueAssigned = false;
   //Use this instead of +0.5 because this also works with negative numbers
   const float64 f64_IntegerValue = std::floor((of64_Value * 10.0) + 0.5) / 10.0;

   //Handle maximum values for integer type
   switch (orc_Content.GetType())
   {
   case C_OSCNodeDataPoolContent::eUINT8:
      if (of64_Value <= static_cast<float64>(std::numeric_limits<uint8>::min()))
      {
         if (orc_Content.GetArray() == true)
         {
            orc_Content.SetValueAU8Element(std::numeric_limits<uint8>::min(), ou32_ArrayIndex);
         }
         else
         {
            orc_Content.SetValueU8(std::numeric_limits<uint8>::min());
         }
         q_ValueAssigned = true;
      }
      if (f64_IntegerValue >= static_cast<float64>(std::numeric_limits<uint8>::max()))
      {
         if (orc_Content.GetArray() == true)
         {
            orc_Content.SetValueAU8Element(std::numeric_limits<uint8>::max(), ou32_ArrayIndex);
         }
         else
         {
            orc_Content.SetValueU8(std::numeric_limits<uint8>::max());
         }
         q_ValueAssigned = true;
      }
      break;
   case C_OSCNodeDataPoolContent::eUINT16:
      if (of64_Value <= static_cast<float64>(std::numeric_limits<uint16>::min()))
      {
         if (orc_Content.GetArray() == true)
         {
            orc_Content.SetValueAU16Element(std::numeric_limits<uint16>::min(), ou32_ArrayIndex);
         }
         else
         {
            orc_Content.SetValueU16(std::numeric_limits<uint16>::min());
         }
         q_ValueAssigned = true;
      }
      if (f64_IntegerValue >= static_cast<float64>(std::numeric_limits<uint16>::max()))
      {
         if (orc_Content.GetArray() == true)
         {
            orc_Content.SetValueAU16Element(std::numeric_limits<uint16>::max(), ou32_ArrayIndex);
         }
         else
         {
            orc_Content.SetValueU16(std::numeric_limits<uint16>::max());
         }
         q_ValueAssigned = true;
      }
      break;
   case C_OSCNodeDataPoolContent::eUINT32:
      if (of64_Value <= static_cast<float64>(std::numeric_limits<uint32>::min()))
      {
         if (orc_Content.GetArray() == true)
         {
            orc_Content.SetValueAU32Element(std::numeric_limits<uint32>::min(), ou32_ArrayIndex);
         }
         else
         {
            orc_Content.SetValueU32(std::numeric_limits<uint32>::min());
         }
         q_ValueAssigned = true;
      }
      if (f64_IntegerValue >= static_cast<float64>(std::numeric_limits<uint32>::max()))
      {
         if (orc_Content.GetArray() == true)
         {
            orc_Content.SetValueAU32Element(std::numeric_limits<uint32>::max(), ou32_ArrayIndex);
         }
         else
         {
            orc_Content.SetValueU32(std::numeric_limits<uint32>::max());
         }
         q_ValueAssigned = true;
      }
      break;
   case C_OSCNodeDataPoolContent::eUINT64:
      if (of64_Value <= static_cast<float64>(std::numeric_limits<uint64>::min()))
      {
         if (orc_Content.GetArray() == true)
         {
            orc_Content.SetValueAU64Element(std::numeric_limits<uint64>::min(), ou32_ArrayIndex);
         }
         else
         {
            orc_Content.SetValueU64(std::numeric_limits<uint64>::min());
         }
         q_ValueAssigned = true;
      }
      if (f64_IntegerValue >= static_cast<float64>(std::numeric_limits<uint64>::max()))
      {
         if (orc_Content.GetArray() == true)
         {
            orc_Content.SetValueAU64Element(std::numeric_limits<uint64>::max(), ou32_ArrayIndex);
         }
         else
         {
            orc_Content.SetValueU64(std::numeric_limits<uint64>::max());
         }
         q_ValueAssigned = true;
      }
      break;
   case C_OSCNodeDataPoolContent::eSINT8:
      if (of64_Value <= static_cast<float64>(std::numeric_limits<sint8>::min()))
      {
         if (orc_Content.GetArray() == true)
         {
            orc_Content.SetValueAS8Element(std::numeric_limits<sint8>::min(), ou32_ArrayIndex);
         }
         else
         {
            orc_Content.SetValueS8(std::numeric_limits<sint8>::min());
         }
         q_ValueAssigned = true;
      }
      if (f64_IntegerValue >= static_cast<float64>(std::numeric_limits<sint8>::max()))
      {
         if (orc_Content.GetArray() == true)
         {
            orc_Content.SetValueAS8Element(std::numeric_limits<sint8>::max(), ou32_ArrayIndex);
         }
         else
         {
            orc_Content.SetValueS8(std::numeric_limits<sint8>::max());
         }
         q_ValueAssigned = true;
      }
      break;
   case C_OSCNodeDataPoolContent::eSINT16:
      if (of64_Value <= static_cast<float64>(std::numeric_limits<sint16>::min()))
      {
         if (orc_Content.GetArray() == true)
         {
            orc_Content.SetValueAS16Element(std::numeric_limits<sint16>::min(), ou32_ArrayIndex);
         }
         else
         {
            orc_Content.SetValueS16(std::numeric_limits<sint16>::min());
         }
         q_ValueAssigned = true;
      }
      if (f64_IntegerValue >= static_cast<float64>(std::numeric_limits<sint16>::max()))
      {
         if (orc_Content.GetArray() == true)
         {
            orc_Content.SetValueAS16Element(std::numeric_limits<sint16>::max(), ou32_ArrayIndex);
         }
         else
         {
            orc_Content.SetValueS16(std::numeric_limits<sint16>::max());
         }
         q_ValueAssigned = true;
      }
      break;
   case C_OSCNodeDataPoolContent::eSINT32:
      if (of64_Value <= static_cast<float64>(std::numeric_limits<sint32>::min()))
      {
         if (orc_Content.GetArray() == true)
         {
            orc_Content.SetValueAS32Element(std::numeric_limits<sint32>::min(), ou32_ArrayIndex);
         }
         else
         {
            orc_Content.SetValueS32(std::numeric_limits<sint32>::min());
         }
         q_ValueAssigned = true;
      }
      if (f64_IntegerValue >= static_cast<float64>(std::numeric_limits<sint32>::max()))
      {
         if (orc_Content.GetArray() == true)
         {
            orc_Content.SetValueAS32Element(std::numeric_limits<sint32>::max(), ou32_ArrayIndex);
         }
         else
         {
            orc_Content.SetValueS32(std::numeric_limits<sint32>::max());
         }
         q_ValueAssigned = true;
      }
      break;
   case C_OSCNodeDataPoolContent::eSINT64:
      if (of64_Value <= static_cast<float64>(std::numeric_limits<sint64>::min()))
      {
         if (orc_Content.GetArray() == true)
         {
            orc_Content.SetValueAS64Element(std::numeric_limits<sint64>::min(), ou32_ArrayIndex);
         }
         else
         {
            orc_Content.SetValueS64(std::numeric_limits<sint64>::min());
         }
         q_ValueAssigned = true;
      }
      if (f64_IntegerValue >= static_cast<float64>(std::numeric_limits<sint64>::max()))
      {
         if (orc_Content.GetArray() == true)
         {
            orc_Content.SetValueAS64Element(std::numeric_limits<sint64>::max(), ou32_ArrayIndex);
         }
         else
         {
            orc_Content.SetValueS64(std::numeric_limits<sint64>::max());
         }
         q_ValueAssigned = true;
      }
      break;
   case C_OSCNodeDataPoolContent::eFLOAT32:
   case C_OSCNodeDataPoolContent::eFLOAT64:
   default:
      //Not an integer type; nothing todo
      break;
   }

   if (q_ValueAssigned == false)
   {
      if (orc_Content.GetArray() == true)
      {
         switch (orc_Content.GetType())
         {
         case C_OSCNodeDataPoolContent::eUINT8:
            orc_Content.SetValueAU8Element(static_cast<uint8>(f64_IntegerValue), ou32_ArrayIndex);
            break;
         case C_OSCNodeDataPoolContent::eUINT16:
            orc_Content.SetValueAU16Element(static_cast<uint16>(f64_IntegerValue), ou32_ArrayIndex);
            break;
         case C_OSCNodeDataPoolContent::eUINT32:
            orc_Content.SetValueAU32Element(static_cast<uint32>(f64_IntegerValue), ou32_ArrayIndex);
            break;
         case C_OSCNodeDataPoolContent::eUINT64:
            orc_Content.SetValueAU64Element(static_cast<uint64>(f64_IntegerValue), ou32_ArrayIndex);
            break;
         case C_OSCNodeDataPoolContent::eSINT8:
            orc_Content.SetValueAS8Element(static_cast<sint8>(f64_IntegerValue), ou32_ArrayIndex);
            break;
         case C_OSCNodeDataPoolContent::eSINT16:
            orc_Content.SetValueAS16Element(static_cast<sint16>(f64_IntegerValue), ou32_ArrayIndex);
            break;
         case C_OSCNodeDataPoolContent::eSINT32:
            orc_Content.SetValueAS32Element(static_cast<sint32>(f64_IntegerValue), ou32_ArrayIndex);
            break;
         case C_OSCNodeDataPoolContent::eSINT64:
            orc_Content.SetValueAS64Element(static_cast<sint64>(f64_IntegerValue), ou32_ArrayIndex);
            break;
         case C_OSCNodeDataPoolContent::eFLOAT32:
            orc_Content.SetValueAF32Element(static_cast<float32>(of64_Value), ou32_ArrayIndex);
            break;
         case C_OSCNodeDataPoolContent::eFLOAT64:
            orc_Content.SetValueAF64Element(of64_Value, ou32_ArrayIndex);
            break;
         default:
            break;
         }
      }
      else
      {
         switch (orc_Content.GetType())
         {
         case C_OSCNodeDataPoolContent::eUINT8:
            orc_Content.SetValueU8(static_cast<uint8>(f64_IntegerValue));
            break;
         case C_OSCNodeDataPoolContent::eUINT16:
            orc_Content.SetValueU16(static_cast<uint16>(f64_IntegerValue));
            break;
         case C_OSCNodeDataPoolContent::eUINT32:
            orc_Content.SetValueU32(static_cast<uint32>(f64_IntegerValue));
            break;
         case C_OSCNodeDataPoolContent::eUINT64:
            orc_Content.SetValueU64(static_cast<uint64>(f64_IntegerValue));
            break;
         case C_OSCNodeDataPoolContent::eSINT8:
            orc_Content.SetValueS8(static_cast<sint8>(f64_IntegerValue));
            break;
         case C_OSCNodeDataPoolContent::eSINT16:
            orc_Content.SetValueS16(static_cast<sint16>(f64_IntegerValue));
            break;
         case C_OSCNodeDataPoolContent::eSINT32:
            orc_Content.SetValueS32(static_cast<sint32>(f64_IntegerValue));
            break;
         case C_OSCNodeDataPoolContent::eSINT64:
            orc_Content.SetValueS64(static_cast<sint64>(f64_IntegerValue));
            break;
         case C_OSCNodeDataPoolContent::eFLOAT32:
            orc_Content.SetValueF32(static_cast<float32>(of64_Value));
            break;
         case C_OSCNodeDataPoolContent::eFLOAT64:
            orc_Content.SetValueF64(of64_Value);
            break;
         default:
            break;
         }
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Sets a value in range of minimum and maximum value.

   If value is in range that means between min and max value, the caller can
   choose if value should be left as it is or should be set to zero.

   If value is out of range, the value is set to closer extremum (min or max value).

   Examples:
   * min: -1.0, max: 2.0, value: 1.0, eLeaveValue   ; after function call value is: 1.0    (was in range: eNoChange)
   * min: -1.0, max: 2.0, value: 1.0, eToZero       ; after function call value is: 0.0    (was in range: eZero)
   * min: -1.0, max: 1.0, value: 2.0, eLeaveValue   ; after function call value is: 1.0    (was out of range: eMax)
   * min: -1.0, max: 1.0, value: 2.0, eToZero       ; after function call value is: 1.0    (was out of range: eMax)
   * min: -1.0, max: 1.0, value: -2.0, eLeaveValue  ; after function call value is: -1.0   (was out of range: eMin)
   * min: -1.0, max: 1.0, value: -2.0, eToZero      ; after function call value is: -1.0   (was out of range: eMin)

   Assumptions:
   * the datatype (E_Type) of min, max and value is set and equal
   * max value is greater than min value

   Hints:
   * to avoid numerical problems the range check is done with a precision of 6 decimal points
   * function is not tested with precision issues for huge values like 2^51
   * Also works with arrays!

   \param[in]     orc_Min            minimum value
   \param[in]     orc_Max            maximum value
   \param[in,out] orc_Value          specific value which is should be set in range
   \param[out]    ore_ValueChangedTo flag for what was done with value:
                                     value was in range (eNoChange or eZero) respectively out of range (eMin or eMax
   \param[in]     oe_SetValue        flag for user choice if value should be left in range as it is or set to zero

   \return
   C_NO_ERR    expected standard case, value was set (or left) successfully
   C_CONFIG    min, max and value have not the same datatype
   C_RANGE     min and max values are interchanged (max < min)
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_OSCNodeDataPoolContentUtil::h_SetValueInMinMaxRange(const C_OSCNodeDataPoolContent & orc_Min,
                                                             const C_OSCNodeDataPoolContent & orc_Max,
                                                             C_OSCNodeDataPoolContent & orc_Value,
                                                             E_ValueChangedTo & ore_ValueChangedTo,
                                                             const E_SetValue oe_SetValue)
{
   sint32 s32_Return = C_NO_ERR;

   if (((orc_Min.GetArray() != orc_Max.GetArray()) || (orc_Min.GetArray() != orc_Value.GetArray())) ||
       ((orc_Min.GetArraySize() != orc_Max.GetArraySize()) || (orc_Min.GetArraySize() != orc_Value.GetArraySize())))
   {
      s32_Return = C_CONFIG;
   }
   else
   {
      for (uint32 u32_ItElement = 0; u32_ItElement < orc_Min.GetArraySize(); ++u32_ItElement)
      {
         float64 f64_Min;
         float64 f64_Max;
         float64 f64_Value;

         orc_Min.GetAnyValueAsFloat64(f64_Min, u32_ItElement);
         orc_Max.GetAnyValueAsFloat64(f64_Max, u32_ItElement);

         // precondition: check if min, max and value have the same types
         if ((orc_Min.GetType() != orc_Max.GetType()) || (orc_Min.GetType() != orc_Value.GetType()))
         {
            s32_Return = C_CONFIG;
         }

         // precondition: check if min and max value are interchanged
         if (s32_Return == C_NO_ERR)
         {
            if (f64_Max < f64_Min)
            {
               s32_Return = C_RANGE;
            }
         }

         // main part of function
         if (s32_Return == C_NO_ERR)
         {
            if (oe_SetValue == C_OSCNodeDataPoolContentUtil::eTO_ZERO)
            {
               C_OSCNodeDataPoolContentUtil::h_SetValueInContent(0.0, orc_Value, u32_ItElement);
            }
            orc_Value.GetAnyValueAsFloat64(f64_Value, u32_ItElement);

            // check if value is between min and max value
            // because of possible numeric problems (1.25987e-321) it is checked in an epsilon environment
            // precision is 6 decimal points (should be enough and is openSYDE default)

            if (((f64_Value < f64_Min) && (std::abs(f64_Value - f64_Min) >= mhf64_EPSILON)) ||
                ((f64_Value > f64_Max) && (std::abs(f64_Value - f64_Max) >= mhf64_EPSILON)))
            {
               // initial value not in min and max range, take closer min or max value
               if (std::abs(f64_Value - f64_Min) <= std::abs(f64_Value - f64_Max))
               {
                  ore_ValueChangedTo = C_OSCNodeDataPoolContentUtil::eMIN;
                  // set value, special handling for signed/unsigned 64-bit integer because of bit masks and accuracy
                  // problems
                  if ((orc_Value.GetType() != C_OSCNodeDataPoolContent::eSINT64) &&
                      (orc_Value.GetType() != C_OSCNodeDataPoolContent::eUINT64))
                  {
                     C_OSCNodeDataPoolContentUtil::h_SetValueInContent(f64_Min, orc_Value, u32_ItElement);
                  }
                  else if (orc_Value.GetType() == C_OSCNodeDataPoolContent::eSINT64)
                  {
                     if (orc_Min.GetArray() == true)
                     {
                        orc_Value.SetValueAS64Element(orc_Min.GetValueAS64Element(u32_ItElement), u32_ItElement);
                     }
                     else
                     {
                        orc_Value.SetValueS64(orc_Min.GetValueS64());
                     }
                  }
                  else
                  {
                     if (orc_Min.GetArray() == true)
                     {
                        orc_Value.SetValueAU64Element(orc_Min.GetValueAU64Element(u32_ItElement), u32_ItElement);
                     }
                     else
                     {
                        orc_Value.SetValueU64(orc_Min.GetValueU64());
                     }
                  }
               }
               else
               {
                  ore_ValueChangedTo = C_OSCNodeDataPoolContentUtil::eMAX;
                  // set value, special handling for signed/unsigned 64-bit integer because of bit masks and accuracy
                  // problems
                  if ((orc_Value.GetType() != C_OSCNodeDataPoolContent::eSINT64) &&
                      (orc_Value.GetType() != C_OSCNodeDataPoolContent::eUINT64))
                  {
                     C_OSCNodeDataPoolContentUtil::h_SetValueInContent(f64_Max, orc_Value, u32_ItElement);
                  }
                  else if (orc_Value.GetType() == C_OSCNodeDataPoolContent::eSINT64)
                  {
                     if (orc_Min.GetArray() == true)
                     {
                        orc_Value.SetValueAS64Element(orc_Max.GetValueAS64Element(u32_ItElement), u32_ItElement);
                     }
                     else
                     {
                        orc_Value.SetValueS64(orc_Max.GetValueS64());
                     }
                  }
                  else
                  {
                     if (orc_Min.GetArray() == true)
                     {
                        orc_Value.SetValueAU64Element(orc_Max.GetValueAU64Element(u32_ItElement), u32_ItElement);
                     }
                     else
                     {
                        orc_Value.SetValueU64(orc_Max.GetValueU64());
                     }
                  }
               }
            }
            else
            {
               // value in range, check what to do with it
               if (oe_SetValue == C_OSCNodeDataPoolContentUtil::eLEAVE_VALUE)
               {
                  // do nothing with value
                  ore_ValueChangedTo = C_OSCNodeDataPoolContentUtil::eNO_CHANGE;
               }
               else
               {
                  // set to zero
                  ore_ValueChangedTo = C_OSCNodeDataPoolContentUtil::eZERO;
               }
            }
         }
      }
   }

   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get data type size in byte

   \param[in] oe_Type Type

   \return
   Data type size in byte
*/
//----------------------------------------------------------------------------------------------------------------------
uint32 C_OSCNodeDataPoolContentUtil::h_GetDataTypeSizeInByte(const C_OSCNodeDataPoolContent::E_Type oe_Type)
{
   uint32 u32_Retval = 0UL;

   switch (oe_Type)
   {
   case C_OSCNodeDataPoolContent::eSINT8:
   case C_OSCNodeDataPoolContent::eUINT8:
      u32_Retval = 1UL;
      break;
   case C_OSCNodeDataPoolContent::eSINT16:
   case C_OSCNodeDataPoolContent::eUINT16:
      u32_Retval = 2UL;
      break;
   case C_OSCNodeDataPoolContent::eSINT32:
   case C_OSCNodeDataPoolContent::eUINT32:
   case C_OSCNodeDataPoolContent::eFLOAT32:
      u32_Retval = 4UL;
      break;
   case C_OSCNodeDataPoolContent::eSINT64:
   case C_OSCNodeDataPoolContent::eUINT64:
   case C_OSCNodeDataPoolContent::eFLOAT64:
      u32_Retval = 8UL;
      break;
   default:
      break;
   }
   return u32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Default constructor
*/
//----------------------------------------------------------------------------------------------------------------------
C_OSCNodeDataPoolContentUtil::C_OSCNodeDataPoolContentUtil(void)
{
}
