//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Utilitiy functions for C_OSCNodeDataPoolContent access (implementation)

   Utilitiy functions for C_OSCNodeDataPoolContent access

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.h"

#include <cmath>
#include <limits>
#include <QLocale>
#include "C_Uti.h"
#include "TGLUtils.h"
#include "stwtypes.h"
#include "stwerrors.h"
#include "C_OSCUtils.h"
#include "C_OSCXMLParser.h"
#include "C_OSCNodeDataPoolFiler.h"
#include "C_SdNdeDataPoolContentUtil.h"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw_tgl;
using namespace stw_types;
using namespace stw_errors;
using namespace stw_opensyde_core;
using namespace stw_opensyde_gui_logic;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */
const float64 C_SdNdeDataPoolContentUtil::mhf64_Epsilon = 1e-5; // to check for a 6 digit precision (openSYDE default)

/* -- Types --------------------------------------------------------------------------------------------------------- */

/* -- Global Variables ---------------------------------------------------------------------------------------------- */

/* -- Module Global Variables --------------------------------------------------------------------------------------- */

/* -- Module Global Function Prototypes ----------------------------------------------------------------------------- */

/* -- Implementation ------------------------------------------------------------------------------------------------ */

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default constructor
*/
//----------------------------------------------------------------------------------------------------------------------
C_SdNdeDataPoolContentUtil::C_SdNdeDataPoolContentUtil(void)
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Convert value to double

   \param[in]  orc_Value             Data element value
   \param[in]  orc_DataPoolElementId Data element ID
   \param[out] orf64_Output          Value as double

   \return
   C_NO_ERR Success
   C_RANGE  Content is a array
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_SdNdeDataPoolContentUtil::h_GetValueAsFloat64(const C_OSCNodeDataPoolContent & orc_Content,
                                                       float64 & orf64_Output)
{
   sint32 s32_Retval = C_NO_ERR;

   orf64_Output = 0.0;

   if (orc_Content.GetArray() == true)
   {
      //Not supported
      s32_Retval = C_RANGE;
   }
   else
   {
      switch (orc_Content.GetType())
      {
      case C_OSCNodeDataPoolContent::eUINT8:
         orf64_Output = static_cast<float64>(orc_Content.GetValueU8());
         break;
      case C_OSCNodeDataPoolContent::eUINT16:
         orf64_Output = static_cast<float64>(orc_Content.GetValueU16());
         break;
      case C_OSCNodeDataPoolContent::eUINT32:
         orf64_Output = static_cast<float64>(orc_Content.GetValueU32());
         break;
      case C_OSCNodeDataPoolContent::eUINT64:
         orf64_Output = static_cast<float64>(orc_Content.GetValueU64());
         break;
      case C_OSCNodeDataPoolContent::eSINT8:
         orf64_Output = static_cast<float64>(orc_Content.GetValueS8());
         break;
      case C_OSCNodeDataPoolContent::eSINT16:
         orf64_Output = static_cast<float64>(orc_Content.GetValueS16());
         break;
      case C_OSCNodeDataPoolContent::eSINT32:
         orf64_Output = static_cast<float64>(orc_Content.GetValueS32());
         break;
      case C_OSCNodeDataPoolContent::eSINT64:
         orf64_Output = static_cast<float64>(orc_Content.GetValueS64());
         break;
      case C_OSCNodeDataPoolContent::eFLOAT32:
         orf64_Output = static_cast<float64>(orc_Content.GetValueF32());
         break;
      case C_OSCNodeDataPoolContent::eFLOAT64:
         orf64_Output = orc_Content.GetValueF64();
         break;
      }
   }

   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Convert value to double

   The vector will be cleared first.

   If it is an array, the size of the vector is > 1
   If it is no array, the size of the vector is 1
   If an error ocured, the size of the vector is 0

   \param[in]  orc_Content             Data element value
   \param[in]  orc_Output              Vector with values
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDataPoolContentUtil::h_GetValuesAsFloat64(const C_OSCNodeDataPoolContent & orc_Content,
                                                      std::vector<float64> & orc_Output)
{
   float64 f64_Value = 0.0;

   orc_Output.clear();

   if (orc_Content.GetArray() == true)
   {
      for (uint32 u32_It = 0; u32_It < orc_Content.GetArraySize(); ++u32_It)
      {
         switch (orc_Content.GetType())
         {
         case C_OSCNodeDataPoolContent::eUINT8:
            f64_Value = static_cast<float64>(orc_Content.GetValueAU8Element(u32_It));
            break;
         case C_OSCNodeDataPoolContent::eUINT16:
            f64_Value = static_cast<float64>(orc_Content.GetValueAU16Element(u32_It));
            break;
         case C_OSCNodeDataPoolContent::eUINT32:
            f64_Value = static_cast<float64>(orc_Content.GetValueAU32Element(u32_It));
            break;
         case C_OSCNodeDataPoolContent::eUINT64:
            f64_Value = static_cast<float64>(orc_Content.GetValueAU64Element(u32_It));
            break;
         case C_OSCNodeDataPoolContent::eSINT8:
            f64_Value = static_cast<float64>(orc_Content.GetValueAS8Element(u32_It));
            break;
         case C_OSCNodeDataPoolContent::eSINT16:
            f64_Value = static_cast<float64>(orc_Content.GetValueAS16Element(u32_It));
            break;
         case C_OSCNodeDataPoolContent::eSINT32:
            f64_Value = static_cast<float64>(orc_Content.GetValueAS32Element(u32_It));
            break;
         case C_OSCNodeDataPoolContent::eSINT64:
            f64_Value = static_cast<float64>(orc_Content.GetValueAS64Element(u32_It));
            break;
         case C_OSCNodeDataPoolContent::eFLOAT32:
            f64_Value = static_cast<float64>(orc_Content.GetValueAF32Element(u32_It));
            break;
         case C_OSCNodeDataPoolContent::eFLOAT64:
            f64_Value = orc_Content.GetValueAF64Element(u32_It);
            break;
         }

         orc_Output.push_back(f64_Value);
      }
   }
   else
   {
      C_SdNdeDataPoolContentUtil::h_GetValueAsFloat64(orc_Content, f64_Value);
      orc_Output.push_back(f64_Value);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Convert value to uint64

   \param[in]  orc_Content  Data element value
   \param[in]  oru64_Output Output value
   \param[in]  ou32_Index   Optional fallback index if array
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDataPoolContentUtil::h_GetAnyValueAsUint64(const C_OSCNodeDataPoolContent & orc_Content,
                                                       uint64 & oru64_Output, const uint32 ou32_Index)
{
   oru64_Output = 0;

   if (orc_Content.GetArray() == true)
   {
      if (ou32_Index < orc_Content.GetArraySize())
      {
         switch (orc_Content.GetType())
         {
         case C_OSCNodeDataPoolContent::eUINT8:
            oru64_Output = static_cast<uint64>(orc_Content.GetValueAU8Element(ou32_Index));
            break;
         case C_OSCNodeDataPoolContent::eUINT16:
            oru64_Output = static_cast<uint64>(orc_Content.GetValueAU16Element(ou32_Index));
            break;
         case C_OSCNodeDataPoolContent::eUINT32:
            oru64_Output = static_cast<uint64>(orc_Content.GetValueAU32Element(ou32_Index));
            break;
         case C_OSCNodeDataPoolContent::eUINT64:
            oru64_Output = static_cast<uint64>(orc_Content.GetValueAU64Element(ou32_Index));
            break;
         case C_OSCNodeDataPoolContent::eSINT8:
            oru64_Output = static_cast<uint64>(static_cast<uint8>(orc_Content.GetValueAS8Element(ou32_Index)));
            break;
         case C_OSCNodeDataPoolContent::eSINT16:
            oru64_Output = static_cast<uint64>(static_cast<uint16>(orc_Content.GetValueAS16Element(ou32_Index)));
            break;
         case C_OSCNodeDataPoolContent::eSINT32:
            oru64_Output = static_cast<uint64>(static_cast<uint32>(orc_Content.GetValueAS32Element(ou32_Index)));
            break;
         case C_OSCNodeDataPoolContent::eSINT64:
            oru64_Output = static_cast<uint64>(orc_Content.GetValueAS64Element(ou32_Index));
            break;
         case C_OSCNodeDataPoolContent::eFLOAT32:
            oru64_Output = static_cast<uint64>(orc_Content.GetValueAF32Element(ou32_Index));
            break;
         case C_OSCNodeDataPoolContent::eFLOAT64:
            oru64_Output = static_cast<uint64>(orc_Content.GetValueAF64Element(ou32_Index));
            break;
         }
      }
   }
   else
   {
      switch (orc_Content.GetType())
      {
      case C_OSCNodeDataPoolContent::eUINT8:
         oru64_Output = static_cast<uint64>(orc_Content.GetValueU8());
         break;
      case C_OSCNodeDataPoolContent::eUINT16:
         oru64_Output = static_cast<uint64>(orc_Content.GetValueU16());
         break;
      case C_OSCNodeDataPoolContent::eUINT32:
         oru64_Output = static_cast<uint64>(orc_Content.GetValueU32());
         break;
      case C_OSCNodeDataPoolContent::eUINT64:
         oru64_Output = static_cast<uint64>(orc_Content.GetValueU64());
         break;
      case C_OSCNodeDataPoolContent::eSINT8:
         oru64_Output = static_cast<uint64>(static_cast<uint8>(orc_Content.GetValueS8()));
         break;
      case C_OSCNodeDataPoolContent::eSINT16:
         oru64_Output = static_cast<uint64>(static_cast<uint16>(orc_Content.GetValueS16()));
         break;
      case C_OSCNodeDataPoolContent::eSINT32:
         oru64_Output = static_cast<uint64>(static_cast<uint32>(orc_Content.GetValueS32()));
         break;
      case C_OSCNodeDataPoolContent::eSINT64:
         oru64_Output = static_cast<uint64>(orc_Content.GetValueS64());
         break;
      case C_OSCNodeDataPoolContent::eFLOAT32:
         oru64_Output = static_cast<uint64>(orc_Content.GetValueF32());
         break;
      case C_OSCNodeDataPoolContent::eFLOAT64:
         oru64_Output = static_cast<uint64>(orc_Content.GetValueF64());
         break;
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Convert value to sint64

   \param[in]  orc_Content  Data element value
   \param[out] ors64_Output Output value
   \param[in]  ou32_Index   Optional fallback index if array
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDataPoolContentUtil::h_GetAnyValueAsSint64(const C_OSCNodeDataPoolContent & orc_Content,
                                                       sint64 & ors64_Output, const uint32 ou32_Index)
{
   ors64_Output = 0;

   if (orc_Content.GetArray() == true)
   {
      if (ou32_Index < orc_Content.GetArraySize())
      {
         switch (orc_Content.GetType())
         {
         case C_OSCNodeDataPoolContent::eUINT8:
            ors64_Output = static_cast<sint64>(orc_Content.GetValueAU8Element(ou32_Index));
            break;
         case C_OSCNodeDataPoolContent::eUINT16:
            ors64_Output = static_cast<sint64>(orc_Content.GetValueAU16Element(ou32_Index));
            break;
         case C_OSCNodeDataPoolContent::eUINT32:
            ors64_Output = static_cast<sint64>(orc_Content.GetValueAU32Element(ou32_Index));
            break;
         case C_OSCNodeDataPoolContent::eUINT64:
            ors64_Output = static_cast<sint64>(orc_Content.GetValueAU64Element(ou32_Index));
            break;
         case C_OSCNodeDataPoolContent::eSINT8:
            ors64_Output = static_cast<sint64>(orc_Content.GetValueAS8Element(ou32_Index));
            break;
         case C_OSCNodeDataPoolContent::eSINT16:
            ors64_Output = static_cast<sint64>(orc_Content.GetValueAS16Element(ou32_Index));
            break;
         case C_OSCNodeDataPoolContent::eSINT32:
            ors64_Output = static_cast<sint64>(orc_Content.GetValueAS32Element(ou32_Index));
            break;
         case C_OSCNodeDataPoolContent::eSINT64:
            ors64_Output = static_cast<sint64>(orc_Content.GetValueAS64Element(ou32_Index));
            break;
         case C_OSCNodeDataPoolContent::eFLOAT32:
            ors64_Output = static_cast<sint64>(orc_Content.GetValueAF32Element(ou32_Index));
            break;
         case C_OSCNodeDataPoolContent::eFLOAT64:
            ors64_Output = static_cast<sint64>(orc_Content.GetValueAF64Element(ou32_Index));
            break;
         }
      }
   }
   else
   {
      switch (orc_Content.GetType())
      {
      case C_OSCNodeDataPoolContent::eUINT8:
         ors64_Output = static_cast<sint64>(orc_Content.GetValueU8());
         break;
      case C_OSCNodeDataPoolContent::eUINT16:
         ors64_Output = static_cast<sint64>(orc_Content.GetValueU16());
         break;
      case C_OSCNodeDataPoolContent::eUINT32:
         ors64_Output = static_cast<sint64>(orc_Content.GetValueU32());
         break;
      case C_OSCNodeDataPoolContent::eUINT64:
         ors64_Output = static_cast<sint64>(orc_Content.GetValueU64());
         break;
      case C_OSCNodeDataPoolContent::eSINT8:
         ors64_Output = static_cast<sint64>(orc_Content.GetValueS8());
         break;
      case C_OSCNodeDataPoolContent::eSINT16:
         ors64_Output = static_cast<sint64>(orc_Content.GetValueS16());
         break;
      case C_OSCNodeDataPoolContent::eSINT32:
         ors64_Output = static_cast<sint64>(orc_Content.GetValueS32());
         break;
      case C_OSCNodeDataPoolContent::eSINT64:
         ors64_Output = static_cast<sint64>(orc_Content.GetValueS64());
         break;
      case C_OSCNodeDataPoolContent::eFLOAT32:
         ors64_Output = static_cast<sint64>(orc_Content.GetValueF32());
         break;
      case C_OSCNodeDataPoolContent::eFLOAT64:
         ors64_Output = static_cast<sint64>(orc_Content.GetValueF64());
         break;
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Sets a float value in the content class

   The of64_Value value will be casted to the current type of the orc_Content class.
   If the orc_Content holds an array, the value will be set to the position ou32_ArrayIndex.

   Hint: for integer datatypes an offset for rounding is added, so that the decimal place is
         not lost by casting.

   \param[in]     of64_Value         Value to set and cast
   \param[in,out] orc_Content        Content class with configured type and array configuration
   \param[in]    ou32_ArrayIndex     Optional index for array position if content class has an array
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDataPoolContentUtil::h_SetValueInContent(const float64 of64_Value, C_OSCNodeDataPoolContent & orc_Content,
                                                     const uint32 ou32_ArrayIndex)
{
   bool q_ValueAssigned = false;
   //Use this instead of +0.5 because this also works with negative numbers
   const float64 f64_IntegerValue = std::floor((of64_Value * 10.0) + 0.5) / 10.0;

   //Handle maximum values for integer type
   switch (orc_Content.GetType())
   {
   case C_OSCNodeDataPoolContent::eUINT8:
      //lint -e{530,10,1015,1013}  c++11 feature
      if (of64_Value <= static_cast<float64>(std::numeric_limits<uint8>::lowest()))
      {
         if (orc_Content.GetArray() == true)
         {
            //lint -e{530,10,734,1015,1013,1960}  c++11 feature
            orc_Content.SetValueAU8Element(std::numeric_limits<uint8>::lowest(), ou32_ArrayIndex);
         }
         else
         {
            //lint -e{530,10,734,1015,1013,1960}  c++11 feature
            orc_Content.SetValueU8(std::numeric_limits<uint8>::lowest());
         }
         q_ValueAssigned = true;
      }
      if (f64_IntegerValue >= static_cast<float64>(std::numeric_limits<uint8>::max()))
      {
         if (orc_Content.GetArray() == true)
         {
            //lint -e{530,10,1015,1013}  c++11 feature
            orc_Content.SetValueAU8Element(std::numeric_limits<uint8>::max(), ou32_ArrayIndex);
         }
         else
         {
            //lint -e{530,10,1015,1013}  c++11 feature
            orc_Content.SetValueU8(std::numeric_limits<uint8>::max());
         }
         q_ValueAssigned = true;
      }
      break;
   case C_OSCNodeDataPoolContent::eUINT16:
      //lint -e{530,10,1015,1013}  c++11 feature
      if (of64_Value <= static_cast<float64>(std::numeric_limits<uint16>::lowest()))
      {
         if (orc_Content.GetArray() == true)
         {
            //lint -e{530,10,734,1015,1013,1960}  c++11 feature
            orc_Content.SetValueAU16Element(std::numeric_limits<uint16>::lowest(), ou32_ArrayIndex);
         }
         else
         {
            //lint -e{530,10,734,1015,1013,1960}  c++11 feature
            orc_Content.SetValueU16(std::numeric_limits<uint16>::lowest());
         }
         q_ValueAssigned = true;
      }
      if (f64_IntegerValue >= static_cast<float64>(std::numeric_limits<uint16>::max()))
      {
         if (orc_Content.GetArray() == true)
         {
            //lint -e{530,10,1015,1013}  c++11 feature
            orc_Content.SetValueAU16Element(std::numeric_limits<uint16>::max(), ou32_ArrayIndex);
         }
         else
         {
            //lint -e{530,10,1015,1013}  c++11 feature
            orc_Content.SetValueU16(std::numeric_limits<uint16>::max());
         }
         q_ValueAssigned = true;
      }
      break;
   case C_OSCNodeDataPoolContent::eUINT32:
      //lint -e{530,10,1015,1013}  c++11 feature
      if (of64_Value <= static_cast<float64>(std::numeric_limits<uint32>::lowest()))
      {
         if (orc_Content.GetArray() == true)
         {
            //lint -e{530,10,1015,1013}  c++11 feature
            orc_Content.SetValueAU32Element(std::numeric_limits<uint32>::lowest(), ou32_ArrayIndex);
         }
         else
         {
            //lint -e{530,10,1015,1013}  c++11 feature
            orc_Content.SetValueU32(std::numeric_limits<uint32>::lowest());
         }
         q_ValueAssigned = true;
      }
      if (f64_IntegerValue >= static_cast<float64>(std::numeric_limits<uint32>::max()))
      {
         if (orc_Content.GetArray() == true)
         {
            //lint -e{530,10,1015,1013}  c++11 feature
            orc_Content.SetValueAU32Element(std::numeric_limits<uint32>::max(), ou32_ArrayIndex);
         }
         else
         {
            //lint -e{530,10,1015,1013}  c++11 feature
            orc_Content.SetValueU32(std::numeric_limits<uint32>::max());
         }
         q_ValueAssigned = true;
      }
      break;
   case C_OSCNodeDataPoolContent::eUINT64:
      //lint -e{530,10,1015,1013}  c++11 feature
      if (of64_Value <= static_cast<float64>(std::numeric_limits<uint64>::lowest()))
      {
         if (orc_Content.GetArray() == true)
         {
            //lint -e{530,10,734,747,1015,1013,1960}  c++11 feature
            orc_Content.SetValueAU64Element(std::numeric_limits<uint64>::lowest(), ou32_ArrayIndex);
         }
         else
         {
            //lint -e{530,10,734,747,1015,1013,1960}  c++11 feature
            orc_Content.SetValueU64(std::numeric_limits<uint64>::lowest());
         }
         q_ValueAssigned = true;
      }
      if (f64_IntegerValue >= static_cast<float64>(std::numeric_limits<uint64>::max()))
      {
         if (orc_Content.GetArray() == true)
         {
            //lint -e{530,10,1015,1013}  c++11 feature
            orc_Content.SetValueAU64Element(std::numeric_limits<uint64>::max(), ou32_ArrayIndex);
         }
         else
         {
            //lint -e{530,10,1015,1013}  c++11 feature
            orc_Content.SetValueU64(std::numeric_limits<uint64>::max());
         }
         q_ValueAssigned = true;
      }
      break;
   case C_OSCNodeDataPoolContent::eSINT8:
      //lint -e{530,10,1015,1013}  c++11 feature
      if (of64_Value <= static_cast<float64>(std::numeric_limits<sint8>::lowest()))
      {
         if (orc_Content.GetArray() == true)
         {
            //lint -e{530,10,734,1015,1013,1960}  c++11 feature
            orc_Content.SetValueAS8Element(std::numeric_limits<sint8>::lowest(), ou32_ArrayIndex);
         }
         else
         {
            //lint -e{530,10,734,1015,1013,1960}  c++11 feature
            orc_Content.SetValueS8(std::numeric_limits<sint8>::lowest());
         }
         q_ValueAssigned = true;
      }
      if (f64_IntegerValue >= static_cast<float64>(std::numeric_limits<sint8>::max()))
      {
         if (orc_Content.GetArray() == true)
         {
            //lint -e{530,10,1015,1013}  c++11 feature
            orc_Content.SetValueAS8Element(std::numeric_limits<sint8>::max(), ou32_ArrayIndex);
         }
         else
         {
            //lint -e{530,10,1015,1013}  c++11 feature
            orc_Content.SetValueS8(std::numeric_limits<sint8>::max());
         }
         q_ValueAssigned = true;
      }
      break;
   case C_OSCNodeDataPoolContent::eSINT16:
      //lint -e{530,10,1015,1013}  c++11 feature
      if (of64_Value <= static_cast<float64>(std::numeric_limits<sint16>::lowest()))
      {
         if (orc_Content.GetArray() == true)
         {
            //lint -e{530,10,734,1015,1013,1960}  c++11 feature
            orc_Content.SetValueAS16Element(std::numeric_limits<sint16>::lowest(), ou32_ArrayIndex);
         }
         else
         {
            //lint -e{530,10,734,1015,1013,1960}  c++11 feature
            orc_Content.SetValueS16(std::numeric_limits<sint16>::lowest());
         }
         q_ValueAssigned = true;
      }
      if (f64_IntegerValue >= static_cast<float64>(std::numeric_limits<sint16>::max()))
      {
         if (orc_Content.GetArray() == true)
         {
            //lint -e{530,10,1015,1013}  c++11 feature
            orc_Content.SetValueAS16Element(std::numeric_limits<sint16>::max(), ou32_ArrayIndex);
         }
         else
         {
            //lint -e{530,10,1015,1013}  c++11 feature
            orc_Content.SetValueS16(std::numeric_limits<sint16>::max());
         }
         q_ValueAssigned = true;
      }
      break;
   case C_OSCNodeDataPoolContent::eSINT32:
      //lint -e{530,10,1015,1013}  c++11 feature
      if (of64_Value <= static_cast<float64>(std::numeric_limits<sint32>::lowest()))
      {
         if (orc_Content.GetArray() == true)
         {
            //lint -e{530,10,1015,1013}  c++11 feature
            orc_Content.SetValueAS32Element(std::numeric_limits<sint32>::lowest(), ou32_ArrayIndex);
         }
         else
         {
            //lint -e{530,10,1015,1013}  c++11 feature
            orc_Content.SetValueS32(std::numeric_limits<sint32>::lowest());
         }
         q_ValueAssigned = true;
      }
      if (f64_IntegerValue >= static_cast<float64>(std::numeric_limits<sint32>::max()))
      {
         if (orc_Content.GetArray() == true)
         {
            //lint -e{530,10,1015,1013}  c++11 feature
            orc_Content.SetValueAS32Element(std::numeric_limits<sint32>::max(), ou32_ArrayIndex);
         }
         else
         {
            //lint -e{530,10,1015,1013}  c++11 feature
            orc_Content.SetValueS32(std::numeric_limits<sint32>::max());
         }
         q_ValueAssigned = true;
      }
      break;
   case C_OSCNodeDataPoolContent::eSINT64:
      //lint -e{530,10,1015,1013}  c++11 feature
      if (of64_Value <= static_cast<float64>(std::numeric_limits<sint64>::lowest()))
      {
         if (orc_Content.GetArray() == true)
         {
            //lint -e{530,10,734,747,1015,1013,1960}  c++11 feature
            orc_Content.SetValueAS64Element(std::numeric_limits<sint64>::lowest(), ou32_ArrayIndex);
         }
         else
         {
            //lint -e{530,10,734,747,1015,1013,1960}  c++11 feature
            orc_Content.SetValueS64(std::numeric_limits<sint64>::lowest());
         }
         q_ValueAssigned = true;
      }
      if (f64_IntegerValue >= static_cast<float64>(std::numeric_limits<sint64>::max()))
      {
         if (orc_Content.GetArray() == true)
         {
            //lint -e{530,10,1015,1013}  c++11 feature
            orc_Content.SetValueAS64Element(std::numeric_limits<sint64>::max(), ou32_ArrayIndex);
         }
         else
         {
            //lint -e{530,10,1015,1013}  c++11 feature
            orc_Content.SetValueS64(std::numeric_limits<sint64>::max());
         }
         q_ValueAssigned = true;
      }
      break;
   default:
      //Not an integer type
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
         }
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Sets a sint64 value in the content class

   The os64_Value value will be casted to the current type of the orc_Content class.
   If the orc_Content holds an array, the value will be set to the position ou32_ArrayIndex.

   Hint: for integer datatypes an offset for rounding is added, so that the decimal place is
         not lost by casting.

   \param[in]     os64_Value         Value to set and cast
   \param[in,out] orc_Content        Content class with configured type and array configuration
   \param[in]     ou32_ArrayIndex    Optional index for array position if content class has an array
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDataPoolContentUtil::h_SetValueInContentS64(const sint64 os64_Value, C_OSCNodeDataPoolContent & orc_Content,
                                                        const uint32 ou32_ArrayIndex)
{
   bool q_ValueAssigned = false;

   //Handle maximum values for integer type
   switch (orc_Content.GetType())
   {
   case C_OSCNodeDataPoolContent::eUINT8:
      //lint -e{530,10,1015,1013}  c++11 feature
      if (os64_Value <= static_cast<sint64>(std::numeric_limits<uint8>::lowest()))
      {
         if (orc_Content.GetArray() == true)
         {
            //lint -e{530,10,734,1015,1013,1960}  c++11 feature
            orc_Content.SetValueAU8Element(std::numeric_limits<uint8>::lowest(), ou32_ArrayIndex);
         }
         else
         {
            //lint -e{530,10,734,1015,1013,1960}  c++11 feature
            orc_Content.SetValueU8(std::numeric_limits<uint8>::lowest());
         }
         q_ValueAssigned = true;
      }
      if (os64_Value >= static_cast<sint64>(std::numeric_limits<uint8>::max()))
      {
         if (orc_Content.GetArray() == true)
         {
            //lint -e{530,10,1015,1013}  c++11 feature
            orc_Content.SetValueAU8Element(std::numeric_limits<uint8>::max(), ou32_ArrayIndex);
         }
         else
         {
            //lint -e{530,10,1015,1013}  c++11 feature
            orc_Content.SetValueU8(std::numeric_limits<uint8>::max());
         }
         q_ValueAssigned = true;
      }
      break;
   case C_OSCNodeDataPoolContent::eUINT16:
      //lint -e{530,10,1015,1013}  c++11 feature
      if (os64_Value <= static_cast<sint64>(std::numeric_limits<uint16>::lowest()))
      {
         if (orc_Content.GetArray() == true)
         {
            //lint -e{530,10,734,1015,1013,1960}  c++11 feature
            orc_Content.SetValueAU16Element(std::numeric_limits<uint16>::lowest(), ou32_ArrayIndex);
         }
         else
         {
            //lint -e{530,10,734,1015,1013,1960}  c++11 feature
            orc_Content.SetValueU16(std::numeric_limits<uint16>::lowest());
         }
         q_ValueAssigned = true;
      }
      if (os64_Value >= static_cast<sint64>(std::numeric_limits<uint16>::max()))
      {
         if (orc_Content.GetArray() == true)
         {
            //lint -e{530,10,1015,1013}  c++11 feature
            orc_Content.SetValueAU16Element(std::numeric_limits<uint16>::max(), ou32_ArrayIndex);
         }
         else
         {
            //lint -e{530,10,1015,1013}  c++11 feature
            orc_Content.SetValueU16(std::numeric_limits<uint16>::max());
         }
         q_ValueAssigned = true;
      }
      break;
   case C_OSCNodeDataPoolContent::eUINT32:
      //lint -e{530,10,1015,1013}  c++11 feature
      if (os64_Value <= static_cast<sint64>(std::numeric_limits<uint32>::lowest()))
      {
         if (orc_Content.GetArray() == true)
         {
            //lint -e{530,10,1015,1013}  c++11 feature
            orc_Content.SetValueAU32Element(std::numeric_limits<uint32>::lowest(), ou32_ArrayIndex);
         }
         else
         {
            //lint -e{530,10,1015,1013}  c++11 feature
            orc_Content.SetValueU32(std::numeric_limits<uint32>::lowest());
         }
         q_ValueAssigned = true;
      }
      if (os64_Value >= static_cast<sint64>(std::numeric_limits<uint32>::max()))
      {
         if (orc_Content.GetArray() == true)
         {
            //lint -e{530,10,1015,1013}  c++11 feature
            orc_Content.SetValueAU32Element(std::numeric_limits<uint32>::max(), ou32_ArrayIndex);
         }
         else
         {
            //lint -e{530,10,1015,1013}  c++11 feature
            orc_Content.SetValueU32(std::numeric_limits<uint32>::max());
         }
         q_ValueAssigned = true;
      }
      break;
   case C_OSCNodeDataPoolContent::eUINT64:
      //lint -e{530,10,1015,1013}  c++11 feature
      if (os64_Value <= static_cast<sint64>(std::numeric_limits<uint64>::lowest()))
      {
         if (orc_Content.GetArray() == true)
         {
            //lint -e{530,10,734,747,1015,1013,1960}  c++11 feature
            orc_Content.SetValueAU64Element(std::numeric_limits<uint64>::lowest(), ou32_ArrayIndex);
         }
         else
         {
            //lint -e{530,10,734,747,1015,1013,1960}  c++11 feature
            orc_Content.SetValueU64(std::numeric_limits<uint64>::lowest());
         }
         q_ValueAssigned = true;
      }
      //Value cannot be over uint64 max
      break;
   case C_OSCNodeDataPoolContent::eSINT8:
      //lint -e{530,10,1015,1013}  c++11 feature
      if (os64_Value <= static_cast<sint64>(std::numeric_limits<sint8>::lowest()))
      {
         if (orc_Content.GetArray() == true)
         {
            //lint -e{530,10,734,1015,1013,1960}  c++11 feature
            orc_Content.SetValueAS8Element(std::numeric_limits<sint8>::lowest(), ou32_ArrayIndex);
         }
         else
         {
            //lint -e{530,10,734,1015,1013,1960}  c++11 feature
            orc_Content.SetValueS8(std::numeric_limits<sint8>::lowest());
         }
         q_ValueAssigned = true;
      }
      if (os64_Value >= static_cast<sint64>(std::numeric_limits<sint8>::max()))
      {
         if (orc_Content.GetArray() == true)
         {
            //lint -e{530,10,1015,1013}  c++11 feature
            orc_Content.SetValueAS8Element(std::numeric_limits<sint8>::max(), ou32_ArrayIndex);
         }
         else
         {
            //lint -e{530,10,1015,1013}  c++11 feature
            orc_Content.SetValueS8(std::numeric_limits<sint8>::max());
         }
         q_ValueAssigned = true;
      }
      break;
   case C_OSCNodeDataPoolContent::eSINT16:
      //lint -e{530,10,1015,1013}  c++11 feature
      if (os64_Value <= static_cast<sint64>(std::numeric_limits<sint16>::lowest()))
      {
         if (orc_Content.GetArray() == true)
         {
            //lint -e{530,10,734,1015,1013,1960}  c++11 feature
            orc_Content.SetValueAS16Element(std::numeric_limits<sint16>::lowest(), ou32_ArrayIndex);
         }
         else
         {
            //lint -e{530,10,734,1015,1013,1960}  c++11 feature
            orc_Content.SetValueS16(std::numeric_limits<sint16>::lowest());
         }
         q_ValueAssigned = true;
      }
      if (os64_Value >= static_cast<sint64>(std::numeric_limits<sint16>::max()))
      {
         if (orc_Content.GetArray() == true)
         {
            //lint -e{530,10,1015,1013}  c++11 feature
            orc_Content.SetValueAS16Element(std::numeric_limits<sint16>::max(), ou32_ArrayIndex);
         }
         else
         {
            //lint -e{530,10,1015,1013}  c++11 feature
            orc_Content.SetValueS16(std::numeric_limits<sint16>::max());
         }
         q_ValueAssigned = true;
      }
      break;
   case C_OSCNodeDataPoolContent::eSINT32:
      //lint -e{530,10,1015,1013}  c++11 feature
      if (os64_Value <= static_cast<sint64>(std::numeric_limits<sint32>::lowest()))
      {
         if (orc_Content.GetArray() == true)
         {
            //lint -e{530,10,1015,1013}  c++11 feature
            orc_Content.SetValueAS32Element(std::numeric_limits<sint32>::lowest(), ou32_ArrayIndex);
         }
         else
         {
            //lint -e{530,10,1015,1013}  c++11 feature
            orc_Content.SetValueS32(std::numeric_limits<sint32>::lowest());
         }
         q_ValueAssigned = true;
      }
      if (os64_Value >= static_cast<sint64>(std::numeric_limits<sint32>::max()))
      {
         if (orc_Content.GetArray() == true)
         {
            //lint -e{530,10,1015,1013}  c++11 feature
            orc_Content.SetValueAS32Element(std::numeric_limits<sint32>::max(), ou32_ArrayIndex);
         }
         else
         {
            //lint -e{530,10,1015,1013}  c++11 feature
            orc_Content.SetValueS32(std::numeric_limits<sint32>::max());
         }
         q_ValueAssigned = true;
      }
      break;
   case C_OSCNodeDataPoolContent::eSINT64:
      //Value cannot be out of range for sint64
      break;
   default:
      //Not an integer type
      break;
   }

   if (q_ValueAssigned == false)
   {
      if (orc_Content.GetArray() == true)
      {
         switch (orc_Content.GetType())
         {
         case C_OSCNodeDataPoolContent::eUINT8:
            orc_Content.SetValueAU8Element(static_cast<uint8>(os64_Value), ou32_ArrayIndex);
            break;
         case C_OSCNodeDataPoolContent::eUINT16:
            orc_Content.SetValueAU16Element(static_cast<uint16>(os64_Value), ou32_ArrayIndex);
            break;
         case C_OSCNodeDataPoolContent::eUINT32:
            orc_Content.SetValueAU32Element(static_cast<uint32>(os64_Value), ou32_ArrayIndex);
            break;
         case C_OSCNodeDataPoolContent::eUINT64:
            orc_Content.SetValueAU64Element(static_cast<uint64>(os64_Value), ou32_ArrayIndex);
            break;
         case C_OSCNodeDataPoolContent::eSINT8:
            orc_Content.SetValueAS8Element(static_cast<sint8>(os64_Value), ou32_ArrayIndex);
            break;
         case C_OSCNodeDataPoolContent::eSINT16:
            orc_Content.SetValueAS16Element(static_cast<sint16>(os64_Value), ou32_ArrayIndex);
            break;
         case C_OSCNodeDataPoolContent::eSINT32:
            orc_Content.SetValueAS32Element(static_cast<sint32>(os64_Value), ou32_ArrayIndex);
            break;
         case C_OSCNodeDataPoolContent::eSINT64:
            orc_Content.SetValueAS64Element(os64_Value, ou32_ArrayIndex);
            break;
         case C_OSCNodeDataPoolContent::eFLOAT32:
            orc_Content.SetValueAF32Element(static_cast<float32>(os64_Value), ou32_ArrayIndex);
            break;
         case C_OSCNodeDataPoolContent::eFLOAT64:
            orc_Content.SetValueAF64Element(static_cast<float64>(os64_Value), ou32_ArrayIndex);
            break;
         }
      }
      else
      {
         switch (orc_Content.GetType())
         {
         case C_OSCNodeDataPoolContent::eUINT8:
            orc_Content.SetValueU8(static_cast<uint8>(os64_Value));
            break;
         case C_OSCNodeDataPoolContent::eUINT16:
            orc_Content.SetValueU16(static_cast<uint16>(os64_Value));
            break;
         case C_OSCNodeDataPoolContent::eUINT32:
            orc_Content.SetValueU32(static_cast<uint32>(os64_Value));
            break;
         case C_OSCNodeDataPoolContent::eUINT64:
            orc_Content.SetValueU64(static_cast<uint64>(os64_Value));
            break;
         case C_OSCNodeDataPoolContent::eSINT8:
            orc_Content.SetValueS8(static_cast<sint8>(os64_Value));
            break;
         case C_OSCNodeDataPoolContent::eSINT16:
            orc_Content.SetValueS16(static_cast<sint16>(os64_Value));
            break;
         case C_OSCNodeDataPoolContent::eSINT32:
            orc_Content.SetValueS32(static_cast<sint32>(os64_Value));
            break;
         case C_OSCNodeDataPoolContent::eSINT64:
            orc_Content.SetValueS64(os64_Value);
            break;
         case C_OSCNodeDataPoolContent::eFLOAT32:
            orc_Content.SetValueF32(static_cast<float32>(os64_Value));
            break;
         case C_OSCNodeDataPoolContent::eFLOAT64:
            orc_Content.SetValueF64(static_cast<float64>(os64_Value));
            break;
         }
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Sets a scaled float value in the content class

   The of64_Value value will be casted to the current type of the orc_Content class.
   If the orc_Content holds an array, the value will be set to the position ou32_ArrayIndex.

   Hint: for integer datatypes an offset for rounding is added, so that the decimal place is
         not lost by casting.

   \param[in]     of64_Value         Value to set and cast
   \param[in,out] orc_Content        Content class with configured type and array configuration
   \param[in]    ou32_ArrayIndex     Optional index for array position if content class has an array
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDataPoolContentUtil::h_SetScaledValueInContent(const float64 of64_ScaledValue,
                                                           C_OSCNodeDataPoolContent & orc_Content,
                                                           const float64 of64_Factor, const float64 of64_Offset,
                                                           const uint32 ou32_ArrayIndex)
{
   // Unscale the value
   const float64 f64_Value = C_OSCUtils::h_GetValueUnscaled(of64_ScaledValue,
                                                            of64_Factor,
                                                            of64_Offset);

   // Update the value
   C_SdNdeDataPoolContentUtil::h_SetValueInContent(f64_Value, orc_Content, ou32_ArrayIndex);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get number of steps for the specified element ID

   \param[in]  orc_Min     Unscaled minimum value
   \param[in]  orc_Max     Unscaled maximum value
   \param[out] oru64_Steps Number of steps, if type supported (sint8, uint8, sint16, uint16, sint32,
                                                               uint32, sint64, uint64)

   \return
   C_NO_ERR Operation success
   C_RANGE  Operation failure: parameter invalid or type not supported
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_SdNdeDataPoolContentUtil::h_GetNumberOfAvailableSteps(const C_OSCNodeDataPoolContent & orc_Min,
                                                               const C_OSCNodeDataPoolContent & orc_Max,
                                                               uint64 & oru64_Steps, const uint32 ou32_Index)
{
   sint32 s32_Retval = C_NO_ERR;

   //Default
   oru64_Steps = 0;
   if ((orc_Min.GetType() == orc_Max.GetType()) && (orc_Min.GetArray() == orc_Max.GetArray()))
   {
      sint64 s64_Min;
      sint64 s64_Max;
      uint64 u64_Min;
      uint64 u64_Max;
      switch (orc_Min.GetType())
      {
      case C_OSCNodeDataPoolContent::eUINT8:
         if (orc_Min.GetArray() == false)
         {
            u64_Max = static_cast<uint64>(orc_Max.GetValueU8());
            u64_Min = static_cast<uint64>(orc_Min.GetValueU8());
         }
         else
         {
            u64_Max = static_cast<uint64>(orc_Max.GetValueAU8Element(ou32_Index));
            u64_Min = static_cast<uint64>(orc_Min.GetValueAU8Element(ou32_Index));
         }
         if (u64_Max >= u64_Min)
         {
            oru64_Steps = u64_Max - u64_Min;
         }
         else
         {
            s32_Retval = C_RANGE;
         }
         break;
      case C_OSCNodeDataPoolContent::eSINT8:
         if (orc_Min.GetArray() == false)
         {
            s64_Max = static_cast<sint64>(orc_Max.GetValueS8());
            s64_Min = static_cast<sint64>(orc_Min.GetValueS8());
         }
         else
         {
            s64_Max = static_cast<sint64>(orc_Max.GetValueAS8Element(ou32_Index));
            s64_Min = static_cast<sint64>(orc_Min.GetValueAS8Element(ou32_Index));
         }
         if (s64_Max >= s64_Min)
         {
            //Overflow may happen (for sint64) but cast will lead to correct result
            oru64_Steps = static_cast<uint64>(s64_Max - s64_Min);
         }
         else
         {
            s32_Retval = C_RANGE;
         }
         break;
      case C_OSCNodeDataPoolContent::eUINT16:
         if (orc_Min.GetArray() == false)
         {
            u64_Max = static_cast<uint64>(orc_Max.GetValueU16());
            u64_Min = static_cast<uint64>(orc_Min.GetValueU16());
         }
         else
         {
            u64_Max = static_cast<uint64>(orc_Max.GetValueAU16Element(ou32_Index));
            u64_Min = static_cast<uint64>(orc_Min.GetValueAU16Element(ou32_Index));
         }
         if (u64_Max >= u64_Min)
         {
            oru64_Steps = u64_Max - u64_Min;
         }
         else
         {
            s32_Retval = C_RANGE;
         }
         break;
      case C_OSCNodeDataPoolContent::eSINT16:
         if (orc_Min.GetArray() == false)
         {
            s64_Max = static_cast<sint64>(orc_Max.GetValueS16());
            s64_Min = static_cast<sint64>(orc_Min.GetValueS16());
         }
         else
         {
            s64_Max = static_cast<sint64>(orc_Max.GetValueAS16Element(ou32_Index));
            s64_Min = static_cast<sint64>(orc_Min.GetValueAS16Element(ou32_Index));
         }
         if (s64_Max >= s64_Min)
         {
            //Overflow may happen (for sint64) but cast will lead to correct result
            oru64_Steps = static_cast<uint64>(s64_Max - s64_Min);
         }
         else
         {
            s32_Retval = C_RANGE;
         }
         break;
      case C_OSCNodeDataPoolContent::eUINT32:
         if (orc_Min.GetArray() == false)
         {
            u64_Max = static_cast<uint64>(orc_Max.GetValueU32());
            u64_Min = static_cast<uint64>(orc_Min.GetValueU32());
         }
         else
         {
            u64_Max = static_cast<uint64>(orc_Max.GetValueAU32Element(ou32_Index));
            u64_Min = static_cast<uint64>(orc_Min.GetValueAU32Element(ou32_Index));
         }
         if (u64_Max >= u64_Min)
         {
            oru64_Steps = u64_Max - u64_Min;
         }
         else
         {
            s32_Retval = C_RANGE;
         }
         break;
      case C_OSCNodeDataPoolContent::eSINT32:
         if (orc_Min.GetArray() == false)
         {
            s64_Max = static_cast<sint64>(orc_Max.GetValueS32());
            s64_Min = static_cast<sint64>(orc_Min.GetValueS32());
         }
         else
         {
            s64_Max = static_cast<sint64>(orc_Max.GetValueAS32Element(ou32_Index));
            s64_Min = static_cast<sint64>(orc_Min.GetValueAS32Element(ou32_Index));
         }
         if (s64_Max >= s64_Min)
         {
            //Overflow may happen (for sint64) but cast will lead to correct result
            oru64_Steps = static_cast<uint64>(s64_Max - s64_Min);
         }
         else
         {
            s32_Retval = C_RANGE;
         }
         break;
      case C_OSCNodeDataPoolContent::eUINT64:
         if (orc_Min.GetArray() == false)
         {
            u64_Max = orc_Max.GetValueU64();
            u64_Min = orc_Min.GetValueU64();
         }
         else
         {
            u64_Max = orc_Max.GetValueAU64Element(ou32_Index);
            u64_Min = orc_Min.GetValueAU64Element(ou32_Index);
         }
         if (u64_Max >= u64_Min)
         {
            oru64_Steps = u64_Max - u64_Min;
         }
         else
         {
            s32_Retval = C_RANGE;
         }
         break;
      case C_OSCNodeDataPoolContent::eSINT64:
         if (orc_Min.GetArray() == false)
         {
            s64_Max = orc_Max.GetValueS64();
            s64_Min = orc_Min.GetValueS64();
         }
         else
         {
            s64_Max = orc_Max.GetValueAS64Element(ou32_Index);
            s64_Min = orc_Min.GetValueAS64Element(ou32_Index);
         }
         if (s64_Max >= s64_Min)
         {
            //Overflow may happen (for sint64) but cast will lead to correct result
            oru64_Steps = static_cast<uint64>(s64_Max - s64_Min);
         }
         else
         {
            s32_Retval = C_RANGE;
         }
         break;
      default:
         s32_Retval = C_RANGE;
         break;
      }
   }
   else
   {
      s32_Retval = C_RANGE;
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get minimal required type after scaling to store all values

   \param[in]  orc_Min     Unscaled minimum
   \param[in]  orc_Max     Unscaled maximum
   \param[in]  orc_Scaling Current scaling
   \param[out] orc_NewMin  New minimum value
   \param[out] orc_NewMax  New maximum value

   \return
   C_NO_ERR Operation success
   C_RANGE  Operation failure: parameter invalid
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_SdNdeDataPoolContentUtil::h_GetMinMaxAfterScaling(const C_OSCNodeDataPoolContent & orc_Min,
                                                           const C_OSCNodeDataPoolContent & orc_Max,
                                                           const float64 of64_Factor, const float64 of64_Offset,
                                                           C_OSCNodeDataPoolContent & orc_NewMin,
                                                           C_OSCNodeDataPoolContent & orc_NewMax,
                                                           const uint32 ou32_Index)
{
   sint32 s32_Retval = C_NO_ERR;

   if ((orc_Min.GetType() == orc_Max.GetType()) && (orc_Min.GetArray() == orc_Max.GetArray()))
   {
      uint64 u64_Steps;
      float64 f64_Min;
      float64 f64_Max;
      if (orc_Min.GetArray() == false)
      {
         //Scale min & max
         tgl_assert(C_SdNdeDataPoolContentUtil::h_GetValueAsFloat64(orc_Min, f64_Min) == C_NO_ERR);
         tgl_assert(C_SdNdeDataPoolContentUtil::h_GetValueAsFloat64(orc_Max, f64_Max) == C_NO_ERR);
         f64_Min = C_OSCUtils::h_GetValueScaled(f64_Min, of64_Factor, of64_Offset);
         f64_Max = C_OSCUtils::h_GetValueScaled(f64_Max, of64_Factor, of64_Offset);
         if (C_SdNdeDataPoolContentUtil::h_GetNumberOfAvailableSteps(orc_Min, orc_Max, u64_Steps, 0) == C_NO_ERR)
         {
            const C_OSCNodeDataPoolContent::E_Type e_Type = C_SdNdeDataPoolContentUtil::h_GetMinimumTypeForRange(
               f64_Min, f64_Max, u64_Steps);
            //Prepare output
            orc_NewMin.SetType(e_Type);
            orc_NewMax.SetType(e_Type);
            orc_NewMin.SetArray(false);
            orc_NewMax.SetArray(false);
            switch (e_Type)
            {
            case C_OSCNodeDataPoolContent::eUINT8:
               //Set output
               orc_NewMin.SetValueU8(static_cast<uint8>(f64_Min));
               orc_NewMax.SetValueU8(static_cast<uint8>(f64_Max));
               break;
            case C_OSCNodeDataPoolContent::eUINT16:
               //Set output
               orc_NewMin.SetValueU16(static_cast<uint16>(f64_Min));
               orc_NewMax.SetValueU16(static_cast<uint16>(f64_Max));
               break;
            case C_OSCNodeDataPoolContent::eUINT32:
               //Set output
               orc_NewMin.SetValueU32(static_cast<uint32>(f64_Min));
               orc_NewMax.SetValueU32(static_cast<uint32>(f64_Max));
               break;
            case C_OSCNodeDataPoolContent::eUINT64:
               //Set output
               orc_NewMin.SetValueU64(static_cast<uint64>(f64_Min));
               orc_NewMax.SetValueU64(static_cast<uint64>(f64_Max));
               break;
            case C_OSCNodeDataPoolContent::eSINT8:
               //Set output
               orc_NewMin.SetValueS8(static_cast<sint8>(f64_Min));
               orc_NewMax.SetValueS8(static_cast<sint8>(f64_Max));
               break;
            case C_OSCNodeDataPoolContent::eSINT16:
               //Set output
               orc_NewMin.SetValueS16(static_cast<sint16>(f64_Min));
               orc_NewMax.SetValueS16(static_cast<sint16>(f64_Max));
               break;
            case C_OSCNodeDataPoolContent::eSINT32:
               //Set output
               orc_NewMin.SetValueS32(static_cast<sint32>(f64_Min));
               orc_NewMax.SetValueS32(static_cast<sint32>(f64_Max));
               break;
            case C_OSCNodeDataPoolContent::eSINT64:
               //Set output
               orc_NewMin.SetValueS64(static_cast<sint64>(f64_Min));
               orc_NewMax.SetValueS64(static_cast<sint64>(f64_Max));
               break;
            case C_OSCNodeDataPoolContent::eFLOAT32:
               //Set output
               orc_NewMin.SetValueF32(static_cast<float32>(f64_Min));
               orc_NewMax.SetValueF32(static_cast<float32>(f64_Max));
               break;
            case C_OSCNodeDataPoolContent::eFLOAT64:
               //Set output
               orc_NewMin.SetValueF64(f64_Min);
               orc_NewMax.SetValueF64(f64_Max);
               break;
            }
         }
         else
         {
            //Prepare output
            orc_NewMin.SetType(C_OSCNodeDataPoolContent::eFLOAT64);
            orc_NewMin.SetArray(false);
            orc_NewMax.SetType(C_OSCNodeDataPoolContent::eFLOAT64);
            orc_NewMax.SetArray(false);
            //Set output
            orc_NewMin.SetValueF64(f64_Min);
            orc_NewMax.SetValueF64(f64_Max);
         }
      }
      else
      {
         if ((orc_Min.GetArray() == orc_Max.GetArray()) && (ou32_Index < orc_Min.GetArraySize()))
         {
            //Scale min & max
            orc_Min.GetAnyValueAsFloat64(f64_Min, ou32_Index);
            orc_Max.GetAnyValueAsFloat64(f64_Max, ou32_Index);
            f64_Min = C_OSCUtils::h_GetValueScaled(f64_Min, of64_Factor, of64_Offset);
            f64_Max = C_OSCUtils::h_GetValueScaled(f64_Max, of64_Factor, of64_Offset);
            if (C_SdNdeDataPoolContentUtil::h_GetNumberOfAvailableSteps(orc_Min, orc_Max, u64_Steps,
                                                                        ou32_Index) == C_NO_ERR)
            {
               const C_OSCNodeDataPoolContent::E_Type e_Type = C_SdNdeDataPoolContentUtil::h_GetMinimumTypeForRange(
                  f64_Min, f64_Max, u64_Steps);
               //Prepare output
               orc_NewMin.SetType(e_Type);
               orc_NewMax.SetType(e_Type);
               orc_NewMin.SetArray(false);
               orc_NewMax.SetArray(false);
               switch (e_Type)
               {
               case C_OSCNodeDataPoolContent::eUINT8:
                  //Set output
                  orc_NewMin.SetValueU8(static_cast<uint8>(f64_Min));
                  orc_NewMax.SetValueU8(static_cast<uint8>(f64_Max));
                  break;
               case C_OSCNodeDataPoolContent::eUINT16:
                  //Set output
                  orc_NewMin.SetValueU16(static_cast<uint16>(f64_Min));
                  orc_NewMax.SetValueU16(static_cast<uint16>(f64_Max));
                  break;
               case C_OSCNodeDataPoolContent::eUINT32:
                  //Set output
                  orc_NewMin.SetValueU32(static_cast<uint32>(f64_Min));
                  orc_NewMax.SetValueU32(static_cast<uint32>(f64_Max));
                  break;
               case C_OSCNodeDataPoolContent::eUINT64:
                  //Set output
                  orc_NewMin.SetValueU64(static_cast<uint64>(f64_Min));
                  orc_NewMax.SetValueU64(static_cast<uint64>(f64_Max));
                  break;
               case C_OSCNodeDataPoolContent::eSINT8:
                  //Set output
                  orc_NewMin.SetValueS8(static_cast<sint8>(f64_Min));
                  orc_NewMax.SetValueS8(static_cast<sint8>(f64_Max));
                  break;
               case C_OSCNodeDataPoolContent::eSINT16:
                  //Set output
                  orc_NewMin.SetValueS16(static_cast<sint16>(f64_Min));
                  orc_NewMax.SetValueS16(static_cast<sint16>(f64_Max));
                  break;
               case C_OSCNodeDataPoolContent::eSINT32:
                  //Set output
                  orc_NewMin.SetValueS32(static_cast<sint32>(f64_Min));
                  orc_NewMax.SetValueS32(static_cast<sint32>(f64_Max));
                  break;
               case C_OSCNodeDataPoolContent::eSINT64:
                  //Set output
                  orc_NewMin.SetValueS64(static_cast<sint64>(f64_Min));
                  orc_NewMax.SetValueS64(static_cast<sint64>(f64_Max));
                  break;
               case C_OSCNodeDataPoolContent::eFLOAT32:
                  //Set output
                  orc_NewMin.SetValueF32(static_cast<float32>(f64_Min));
                  orc_NewMax.SetValueF32(static_cast<float32>(f64_Max));
                  break;
               case C_OSCNodeDataPoolContent::eFLOAT64:
                  //Set output
                  orc_NewMin.SetValueF64(f64_Min);
                  orc_NewMax.SetValueF64(f64_Max);
                  break;
               }
            }
            else
            {
               //Prepare output
               orc_NewMin.SetType(C_OSCNodeDataPoolContent::eFLOAT64);
               orc_NewMin.SetArray(false);
               orc_NewMax.SetType(C_OSCNodeDataPoolContent::eFLOAT64);
               orc_NewMax.SetArray(false);
               //Set output
               orc_NewMin.SetValueF64(f64_Min);
               orc_NewMax.SetValueF64(f64_Max);
            }
         }
         else
         {
            s32_Retval = C_RANGE;
         }
      }
   }
   else
   {
      s32_Retval = C_RANGE;
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get minimal required type after scaling to store all values

   \param[in]  orc_Min     Unscaled minimum
   \param[in]  orc_Max     Unscaled maximum
   \param[in]  orc_Scaling Current scaling
   \param[out] orc_Types   Minimum types after scaling

   \return
   C_NO_ERR Operation success
   C_RANGE  Operation failure: parameter invalid
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_SdNdeDataPoolContentUtil::h_GetMinimalTypeAfterScaling(const C_OSCNodeDataPoolContent & orc_Min,
                                                                const C_OSCNodeDataPoolContent & orc_Max,
                                                                const float64 of64_Factor, const float64 of64_Offset,
                                                                std::vector<C_OSCNodeDataPoolContent::E_Type> & orc_Types)
{
   sint32 s32_Retval = C_NO_ERR;

   orc_Types.clear();
   if ((orc_Min.GetType() == orc_Max.GetType()) && (orc_Min.GetArray() == orc_Max.GetArray()))
   {
      uint64 u64_Steps;
      float64 f64_Min;
      float64 f64_Max;
      if (orc_Min.GetArray() == false)
      {
         //Scale min & max
         tgl_assert(C_SdNdeDataPoolContentUtil::h_GetValueAsFloat64(orc_Min, f64_Min) == C_NO_ERR);
         tgl_assert(C_SdNdeDataPoolContentUtil::h_GetValueAsFloat64(orc_Max, f64_Max) == C_NO_ERR);
         f64_Min = C_OSCUtils::h_GetValueScaled(f64_Min, of64_Factor, of64_Offset);
         f64_Max = C_OSCUtils::h_GetValueScaled(f64_Max, of64_Factor, of64_Offset);
         if (C_SdNdeDataPoolContentUtil::h_GetNumberOfAvailableSteps(orc_Min, orc_Max, u64_Steps, 0) == C_NO_ERR)
         {
            orc_Types.push_back(C_SdNdeDataPoolContentUtil::h_GetMinimumTypeForRange(f64_Min, f64_Max, u64_Steps));
         }
         else
         {
            orc_Types.push_back(C_OSCNodeDataPoolContent::eFLOAT64);
         }
      }
      else
      {
         std::vector<float64> c_Min;
         std::vector<float64> c_Max;
         //Scale min & max
         C_SdNdeDataPoolContentUtil::h_GetValuesAsFloat64(orc_Min, c_Min);
         C_SdNdeDataPoolContentUtil::h_GetValuesAsFloat64(orc_Max, c_Max);
         if (((c_Min.size() == c_Max.size()) && (c_Min.size() == orc_Min.GetArraySize())) &&
             (orc_Min.GetArraySize() == orc_Max.GetArraySize()))
         {
            for (uint32 u32_ItArray = 0; u32_ItArray < orc_Max.GetArraySize(); ++u32_ItArray)
            {
               f64_Min = C_OSCUtils::h_GetValueScaled(c_Min[u32_ItArray], of64_Factor, of64_Offset);
               f64_Max = C_OSCUtils::h_GetValueScaled(c_Max[u32_ItArray], of64_Factor, of64_Offset);
               if (C_SdNdeDataPoolContentUtil::h_GetNumberOfAvailableSteps(orc_Min, orc_Max, u64_Steps,
                                                                           u32_ItArray) == C_NO_ERR)
               {
                  orc_Types.push_back(C_SdNdeDataPoolContentUtil::h_GetMinimumTypeForRange(f64_Min, f64_Max,
                                                                                           u64_Steps));
               }
               else
               {
                  orc_Types.push_back(C_OSCNodeDataPoolContent::eFLOAT64);
               }
            }
         }
         else
         {
            s32_Retval = C_RANGE;
         }
      }
   }
   else
   {
      s32_Retval = C_RANGE;
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get minimal required type after scaling to store all values

   \param[in] of64_Min   Unscaled minimum
   \param[in] of64_Max   Unscaled maximum
   \param[in] ou64_Steps Current scaling

   \return
   C_NO_ERR Operation success
   C_RANGE  Operation failure: parameter invalid
*/
//----------------------------------------------------------------------------------------------------------------------
C_OSCNodeDataPoolContent::E_Type C_SdNdeDataPoolContentUtil::h_GetMinimumTypeForRange(const float64 of64_Min,
                                                                                      const float64 of64_Max,
                                                                                      const uint64 ou64_Steps)
{
   C_OSCNodeDataPoolContent::E_Type e_Retval = C_OSCNodeDataPoolContent::eFLOAT64;
   //There is no point for this function if there are no steps
   //If min is over the maximum it's just invalid so also no point
   //If either min or max is no integer then probably a double is required
   if (((of64_Min > of64_Max) ||
        (C_Uti::h_CheckFloatHasNoFractionPart(of64_Min) == false)) ||
       (C_Uti::h_CheckFloatHasNoFractionPart(of64_Max) == false))
   {
      e_Retval = C_OSCNodeDataPoolContent::eFLOAT64;
   }
   else
   {
      //Check if step width is integer
      if ((ou64_Steps > 0ULL) &&
          (C_Uti::h_CheckFloatHasNoFractionPart((of64_Max - of64_Min) / static_cast<float64>(ou64_Steps)) == false))
      {
         e_Retval = C_OSCNodeDataPoolContent::eFLOAT64;
      }
      else
      {
         if (of64_Min < 0.0)
         {
            //Signed integer
            //lint -e{530,10,1015,1013}  c++11 feature
            if (((of64_Min >= static_cast<float64>(std::numeric_limits<sint8>::lowest())) &&
                 (of64_Max <= static_cast<float64>(std::numeric_limits<sint8>::max()))) &&
                (ou64_Steps <= static_cast<uint64>(std::numeric_limits<uint8>::max())))
            {
               e_Retval = C_OSCNodeDataPoolContent::eSINT8;
            }
            //lint -e{530,10,1015,1013}  c++11 feature
            else if (((of64_Min >= static_cast<float64>(std::numeric_limits<sint16>::lowest())) &&
                      (of64_Max <= static_cast<float64>(std::numeric_limits<sint16>::max()))) &&
                     (ou64_Steps <= static_cast<uint64>(std::numeric_limits<uint16>::max())))
            {
               e_Retval = C_OSCNodeDataPoolContent::eSINT16;
            }
            //lint -e{530,10,1015,1013}  c++11 feature
            else if (((of64_Min >= static_cast<float64>(std::numeric_limits<sint32>::lowest())) &&
                      (of64_Max <= static_cast<float64>(std::numeric_limits<sint32>::max()))) &&
                     (ou64_Steps <= static_cast<uint64>(std::numeric_limits<uint32>::max())))
            {
               e_Retval = C_OSCNodeDataPoolContent::eSINT32;
            }
            //lint -e{530,10,1015,1013}  c++11 feature
            else if ((of64_Min >= static_cast<float64>(std::numeric_limits<sint64>::lowest())) &&
                     (of64_Max <= static_cast<float64>(std::numeric_limits<sint64>::max())))
            {
               e_Retval = C_OSCNodeDataPoolContent::eSINT64;
            }
            else
            {
               e_Retval = C_OSCNodeDataPoolContent::eFLOAT64;
            }
         }
         else
         {
            //Unsigned integer
            if (((of64_Max <= static_cast<float64>(std::numeric_limits<uint8>::max()))) &&
                (ou64_Steps <= static_cast<uint64>(std::numeric_limits<uint8>::max())))
            {
               e_Retval = C_OSCNodeDataPoolContent::eUINT8;
            }
            else if (((of64_Max <= static_cast<float64>(std::numeric_limits<uint16>::max()))) &&
                     (ou64_Steps <= static_cast<uint64>(std::numeric_limits<uint16>::max())))
            {
               e_Retval = C_OSCNodeDataPoolContent::eUINT16;
            }
            else if (((of64_Max <= static_cast<float64>(std::numeric_limits<uint32>::max()))) &&
                     (ou64_Steps <= static_cast<uint64>(std::numeric_limits<uint32>::max())))
            {
               e_Retval = C_OSCNodeDataPoolContent::eUINT32;
            }
            else if ((of64_Max <= static_cast<float64>(std::numeric_limits<uint64>::max())))
            {
               e_Retval = C_OSCNodeDataPoolContent::eUINT64;
            }
            else
            {
               e_Retval = C_OSCNodeDataPoolContent::eFLOAT64;
            }
         }
      }
   }
   return e_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set input content values to zero

   \param[in,out] orc_Content Content
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDataPoolContentUtil::h_ZeroContent(C_OSCNodeDataPoolContent & orc_Content)
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
         }
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Simple conversion from content to QVariant

   Simple means: array types not supported

   \param[in]  orc_Content C_OSCNodeDataPoolContent input
   \param[out] orc_Variant QVariant output

   \return
   C_NO_ERR Operation success
   C_RANGE  Operation failure: parameter invalid
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_SdNdeDataPoolContentUtil::h_SimpleConvertToVariant(const C_OSCNodeDataPoolContent & orc_Content,
                                                            QVariant & orc_Variant)
{
   sint32 s32_Retval = C_NO_ERR;

   if (orc_Content.GetArray() == false)
   {
      switch (orc_Content.GetType())
      {
      case C_OSCNodeDataPoolContent::eSINT8:
         orc_Variant = static_cast<sintn>(orc_Content.GetValueS8());
         break;
      case C_OSCNodeDataPoolContent::eSINT16:
         orc_Variant = orc_Content.GetValueS16();
         break;
      case C_OSCNodeDataPoolContent::eSINT32:
         orc_Variant = static_cast<sint64>(orc_Content.GetValueS32());
         break;
      case C_OSCNodeDataPoolContent::eSINT64:
         orc_Variant = orc_Content.GetValueS64();
         break;
      case C_OSCNodeDataPoolContent::eUINT8:
         orc_Variant = static_cast<sintn>(orc_Content.GetValueU8());
         break;
      case C_OSCNodeDataPoolContent::eUINT16:
         orc_Variant = orc_Content.GetValueU16();
         break;
      case C_OSCNodeDataPoolContent::eUINT32:
         orc_Variant = static_cast<uint64>(orc_Content.GetValueU32());
         break;
      case C_OSCNodeDataPoolContent::eUINT64:
         orc_Variant = orc_Content.GetValueU64();
         break;
      case C_OSCNodeDataPoolContent::eFLOAT32:
         orc_Variant = orc_Content.GetValueF32();
         break;
      case C_OSCNodeDataPoolContent::eFLOAT64:
         orc_Variant = orc_Content.GetValueF64();
         break;
      }
   }
   else
   {
      s32_Retval = C_RANGE;
   }

   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Simple conversion from QVariant to content

   Simple means: not all types supported

   \param[in]  orc_Variant QVariant input
   \param[out] orc_Content C_OSCNodeDataPoolContent output

   \return
   C_NO_ERR Operation success
   C_RANGE  Operation failure: parameter invalid
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_SdNdeDataPoolContentUtil::h_SimpleConvertFromVariant(const QVariant & orc_Variant,
                                                              C_OSCNodeDataPoolContent & orc_Content)
{
   bool q_Ok;
   sint32 s32_Retval = C_NO_ERR;

   switch (orc_Variant.type())
   {
   case QVariant::Int:
      orc_Content.SetType(C_OSCNodeDataPoolContent::eSINT32);
      orc_Content.SetArray(false);
      orc_Content.SetValueS32(orc_Variant.toInt(&q_Ok));
      tgl_assert(q_Ok == true);
      break;
   case QVariant::UInt:
      orc_Content.SetType(C_OSCNodeDataPoolContent::eUINT32);
      orc_Content.SetArray(false);
      orc_Content.SetValueU32(orc_Variant.toUInt(&q_Ok));
      tgl_assert(q_Ok == true);
      break;
   case QVariant::LongLong:
      orc_Content.SetType(C_OSCNodeDataPoolContent::eSINT64);
      orc_Content.SetArray(false);
      orc_Content.SetValueS64(orc_Variant.toLongLong(&q_Ok));
      tgl_assert(q_Ok == true);
      break;
   case QVariant::ULongLong:
      orc_Content.SetType(C_OSCNodeDataPoolContent::eUINT64);
      orc_Content.SetArray(false);
      orc_Content.SetValueU64(orc_Variant.toULongLong(&q_Ok));
      tgl_assert(q_Ok == true);
      break;
   case QVariant::Double:
      orc_Content.SetType(C_OSCNodeDataPoolContent::eFLOAT64);
      orc_Content.SetArray(false);
      orc_Content.SetValueF64(orc_Variant.toDouble(&q_Ok));
      tgl_assert(q_Ok == true);
      break;
   default:
      s32_Retval = C_RANGE;
      break;
   }

   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get all values as scaled strings

   \param[in]  orc_Content The source content
   \param[in]  of64_Factor Scaling factor
   \param[in]  of64_Offset Scaling offset
   \param[out] orc_Output  Scaled strings
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDataPoolContentUtil::h_GetValuesAsScaledString(const C_OSCNodeDataPoolContent & orc_Content,
                                                           const float64 of64_Factor, const float64 of64_Offset,
                                                           std::vector<QString> & orc_Output)
{
   if (orc_Content.GetArray() == false)
   {
      QString c_Tmp;
      C_SdNdeDataPoolContentUtil::h_GetValueAsScaledString(orc_Content, of64_Factor, of64_Offset, c_Tmp, 0);
      orc_Output.push_back(c_Tmp);
   }
   else
   {
      orc_Output.reserve(orc_Content.GetArraySize());
      for (uint32 u32_ItElement = 0; u32_ItElement < orc_Content.GetArraySize(); ++u32_ItElement)
      {
         QString c_Tmp;
         C_SdNdeDataPoolContentUtil::h_GetValueAsScaledString(orc_Content, of64_Factor, of64_Offset, c_Tmp,
                                                              u32_ItElement);
         orc_Output.push_back(c_Tmp);
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get all values as one scaled and combined string

   \param[in]  orc_Content The source content
   \param[in]  of64_Factor Scaling factor
   \param[in]  of64_Offset Scaling offset
   \param[out] orc_Output  Scaled and combined string
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDataPoolContentUtil::h_GetValuesAsScaledCombinedString(const C_OSCNodeDataPoolContent & orc_Content,
                                                                   const float64 of64_Factor, const float64 of64_Offset,
                                                                   QString & orc_Output)
{
   std::vector<QString> c_Output;
   C_SdNdeDataPoolContentUtil::h_GetValuesAsScaledString(orc_Content, of64_Factor, of64_Offset,
                                                         c_Output);
   orc_Output = C_SdNdeDataPoolContentUtil::h_CombineString(c_Output);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Combine value string

   \param[in] orc_Input Values

   \return
   Combined value string
*/
//----------------------------------------------------------------------------------------------------------------------
QString C_SdNdeDataPoolContentUtil::h_CombineString(const std::vector<QString> & orc_Input)
{
   QString c_Retval = "";

   for (uint32 u32_It = 0; u32_It < orc_Input.size(); ++u32_It)
   {
      c_Retval += orc_Input[u32_It];
      if ((u32_It + 1) < orc_Input.size())
      {
         c_Retval += ";";
      }
   }
   return c_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get all value as scaled string

   \param[in]  orc_Content The source content
   \param[in]  of64_Factor Scaling factor
   \param[in]  of64_Offset Scaling offset
   \param[out] orc_Output  Scaled string
   \param[in]  ou32_Index  Index to use in case of array
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDataPoolContentUtil::h_GetValueAsScaledString(const C_OSCNodeDataPoolContent & orc_Content,
                                                          const float64 of64_Factor, const float64 of64_Offset,
                                                          QString & orc_Output, const uint32 ou32_Index)
{
   std::string c_Str;

   orc_Content.GetValueAsScaledString(of64_Factor, of64_Offset, c_Str, ou32_Index);
   orc_Output = c_Str.c_str();
   //Replace separators
   orc_Output.replace(QLocale::c().decimalPoint(), QLocale::system().decimalPoint(), Qt::CaseInsensitive);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set content to maximum type value

   \param[in,out] orc_Content Content
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDataPoolContentUtil::h_InitMax(C_OSCNodeDataPoolContent & orc_Content)
{
   switch (orc_Content.GetType())
   {
   case C_OSCNodeDataPoolContent::eSINT8:
      if (orc_Content.GetArray() == false)
      {
         orc_Content.SetValueS8(std::numeric_limits<sint8>::max());
      }
      else
      {
         for (uint32 u32_It = 0; u32_It < orc_Content.GetArraySize(); ++u32_It)
         {
            orc_Content.SetValueAS8Element(std::numeric_limits<sint8>::max(), u32_It);
         }
      }
      break;
   case C_OSCNodeDataPoolContent::eSINT16:
      if (orc_Content.GetArray() == false)
      {
         orc_Content.SetValueS16(std::numeric_limits<sint16>::max());
      }
      else
      {
         for (uint32 u32_It = 0; u32_It < orc_Content.GetArraySize(); ++u32_It)
         {
            orc_Content.SetValueAS16Element(std::numeric_limits<sint16>::max(), u32_It);
         }
      }
      break;
   case C_OSCNodeDataPoolContent::eSINT32:
      if (orc_Content.GetArray() == false)
      {
         orc_Content.SetValueS32(std::numeric_limits<sint32>::max());
      }
      else
      {
         for (uint32 u32_It = 0; u32_It < orc_Content.GetArraySize(); ++u32_It)
         {
            orc_Content.SetValueAS32Element(std::numeric_limits<sint32>::max(), u32_It);
         }
      }
      break;
   case C_OSCNodeDataPoolContent::eSINT64:
      if (orc_Content.GetArray() == false)
      {
         orc_Content.SetValueS64(std::numeric_limits<sint64>::max());
      }
      else
      {
         for (uint32 u32_It = 0; u32_It < orc_Content.GetArraySize(); ++u32_It)
         {
            orc_Content.SetValueAS64Element(std::numeric_limits<sint64>::max(), u32_It);
         }
      }
      break;
   case C_OSCNodeDataPoolContent::eUINT8:
      if (orc_Content.GetArray() == false)
      {
         orc_Content.SetValueU8(std::numeric_limits<uint8>::max());
      }
      else
      {
         for (uint32 u32_It = 0; u32_It < orc_Content.GetArraySize(); ++u32_It)
         {
            orc_Content.SetValueAU8Element(std::numeric_limits<uint8>::max(), u32_It);
         }
      }
      break;
   case C_OSCNodeDataPoolContent::eUINT16:
      if (orc_Content.GetArray() == false)
      {
         orc_Content.SetValueU16(std::numeric_limits<uint16>::max());
      }
      else
      {
         for (uint32 u32_It = 0; u32_It < orc_Content.GetArraySize(); ++u32_It)
         {
            orc_Content.SetValueAU16Element(std::numeric_limits<uint16>::max(), u32_It);
         }
      }
      break;
   case C_OSCNodeDataPoolContent::eUINT32:
      if (orc_Content.GetArray() == false)
      {
         orc_Content.SetValueU32(std::numeric_limits<uint32>::max());
      }
      else
      {
         for (uint32 u32_It = 0; u32_It < orc_Content.GetArraySize(); ++u32_It)
         {
            orc_Content.SetValueAU32Element(std::numeric_limits<uint32>::max(), u32_It);
         }
      }
      break;
   case C_OSCNodeDataPoolContent::eUINT64:
      if (orc_Content.GetArray() == false)
      {
         orc_Content.SetValueU64(std::numeric_limits<uint64>::max());
      }
      else
      {
         for (uint32 u32_It = 0; u32_It < orc_Content.GetArraySize(); ++u32_It)
         {
            orc_Content.SetValueAU64Element(std::numeric_limits<uint64>::max(), u32_It);
         }
      }
      break;
   case C_OSCNodeDataPoolContent::eFLOAT32:
      if (orc_Content.GetArray() == false)
      {
         orc_Content.SetValueF32(std::numeric_limits<float32>::max());
      }
      else
      {
         for (uint32 u32_It = 0; u32_It < orc_Content.GetArraySize(); ++u32_It)
         {
            orc_Content.SetValueAF32Element(std::numeric_limits<float32>::max(), u32_It);
         }
      }
      break;
   case C_OSCNodeDataPoolContent::eFLOAT64:
      if (orc_Content.GetArray() == false)
      {
         orc_Content.SetValueF64(std::numeric_limits<float64>::max());
      }
      else
      {
         for (uint32 u32_It = 0; u32_It < orc_Content.GetArraySize(); ++u32_It)
         {
            orc_Content.SetValueAF64Element(std::numeric_limits<float64>::max(), u32_It);
         }
      }
      break;
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set content to minimum type value

   \param[in,out] orc_Content Content
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDataPoolContentUtil::h_InitMin(C_OSCNodeDataPoolContent & orc_Content)
{
   switch (orc_Content.GetType())
   {
   case C_OSCNodeDataPoolContent::eSINT8:
      if (orc_Content.GetArray() == false)
      {
         //lint -e{10,530,734,1015,1013,1960}  c++11 feature
         orc_Content.SetValueS8(std::numeric_limits<sint8>::lowest());
      }
      else
      {
         for (uint32 u32_It = 0; u32_It < orc_Content.GetArraySize(); ++u32_It)
         {
            //lint -e{10,530,734,1015,1013,1960}  c++11 feature
            orc_Content.SetValueAS8Element(std::numeric_limits<sint8>::lowest(), u32_It);
         }
      }
      break;
   case C_OSCNodeDataPoolContent::eSINT16:
      if (orc_Content.GetArray() == false)
      {
         //lint -e{10,530,734,1015,1013,1960}  c++11 feature
         orc_Content.SetValueS16(std::numeric_limits<sint16>::lowest());
      }
      else
      {
         for (uint32 u32_It = 0; u32_It < orc_Content.GetArraySize(); ++u32_It)
         {
            //lint -e{10,530,734,1015,1013,1960}  c++11 feature
            orc_Content.SetValueAS16Element(std::numeric_limits<sint16>::lowest(), u32_It);
         }
      }
      break;
   case C_OSCNodeDataPoolContent::eSINT32:
      if (orc_Content.GetArray() == false)
      {
         //lint -e{530,10,1015,1013}  c++11 feature
         orc_Content.SetValueS32(std::numeric_limits<sint32>::lowest());
      }
      else
      {
         for (uint32 u32_It = 0; u32_It < orc_Content.GetArraySize(); ++u32_It)
         {
            //lint -e{530,10,1015,1013}  c++11 feature
            orc_Content.SetValueAS32Element(std::numeric_limits<sint32>::lowest(), u32_It);
         }
      }
      break;
   case C_OSCNodeDataPoolContent::eSINT64:
      if (orc_Content.GetArray() == false)
      {
         //lint -e{10,530,747,1015,1013}  c++11 feature
         orc_Content.SetValueS64(std::numeric_limits<sint64>::lowest());
      }
      else
      {
         for (uint32 u32_It = 0; u32_It < orc_Content.GetArraySize(); ++u32_It)
         {
            //lint -e{10,530,747,1015,1013}  c++11 feature
            orc_Content.SetValueAS64Element(std::numeric_limits<sint64>::lowest(), u32_It);
         }
      }
      break;
   case C_OSCNodeDataPoolContent::eUINT8:
      if (orc_Content.GetArray() == false)
      {
         //lint -e{10,530,734,1015,1013,1960}  c++11 feature
         orc_Content.SetValueU8(std::numeric_limits<uint8>::lowest());
      }
      else
      {
         for (uint32 u32_It = 0; u32_It < orc_Content.GetArraySize(); ++u32_It)
         {
            //lint -e{10,530,734,1015,1013,1960}  c++11 feature
            orc_Content.SetValueAU8Element(std::numeric_limits<uint8>::lowest(), u32_It);
         }
      }
      break;
   case C_OSCNodeDataPoolContent::eUINT16:
      if (orc_Content.GetArray() == false)
      {
         //lint -e{10,530,734,1015,1013,1960}  c++11 feature
         orc_Content.SetValueU16(std::numeric_limits<uint16>::lowest());
      }
      else
      {
         for (uint32 u32_It = 0; u32_It < orc_Content.GetArraySize(); ++u32_It)
         {
            //lint -e{10,530,734,1015,1013,1960}  c++11 feature
            orc_Content.SetValueAU16Element(std::numeric_limits<uint16>::lowest(), u32_It);
         }
      }
      break;
   case C_OSCNodeDataPoolContent::eUINT32:
      if (orc_Content.GetArray() == false)
      {
         //lint -e{530,10,1015,1013}  c++11 feature
         orc_Content.SetValueU32(std::numeric_limits<uint32>::lowest());
      }
      else
      {
         for (uint32 u32_It = 0; u32_It < orc_Content.GetArraySize(); ++u32_It)
         {
            //lint -e{530,10,1015,1013}  c++11 feature
            orc_Content.SetValueAU32Element(std::numeric_limits<uint32>::lowest(), u32_It);
         }
      }
      break;
   case C_OSCNodeDataPoolContent::eUINT64:
      if (orc_Content.GetArray() == false)
      {
         //lint -e{10,530,747,1015,1013}  c++11 feature
         orc_Content.SetValueU64(std::numeric_limits<uint64>::lowest());
      }
      else
      {
         for (uint32 u32_It = 0; u32_It < orc_Content.GetArraySize(); ++u32_It)
         {
            //lint -e{10,530,747,1015,1013}  c++11 feature
            orc_Content.SetValueAU64Element(std::numeric_limits<uint64>::lowest(), u32_It);
         }
      }
      break;
   case C_OSCNodeDataPoolContent::eFLOAT32:
      if (orc_Content.GetArray() == false)
      {
         //lint -e{10,530,747,1015,1013,1960}  c++11 feature
         orc_Content.SetValueF32(std::numeric_limits<float32>::lowest());
      }
      else
      {
         for (uint32 u32_It = 0; u32_It < orc_Content.GetArraySize(); ++u32_It)
         {
            //lint -e{10,530,747,1015,1013,1960}  c++11 feature
            orc_Content.SetValueAF32Element(std::numeric_limits<float32>::lowest(), u32_It);
         }
      }
      break;
   case C_OSCNodeDataPoolContent::eFLOAT64:
      if (orc_Content.GetArray() == false)
      {
         //lint -e{10,530,747,1015,1013,1960}  c++11 feature
         orc_Content.SetValueF64(std::numeric_limits<float64>::lowest());
      }
      else
      {
         for (uint32 u32_It = 0; u32_It < orc_Content.GetArraySize(); ++u32_It)
         {
            //lint -e{10,530,747,1015,1013,1960}  c++11 feature
            orc_Content.SetValueAF64Element(std::numeric_limits<float64>::lowest(), u32_It);
         }
      }
      break;
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Sets a value in range of minimum and maximum value.

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

   \param[in]     orc_Min              minimum value
   \param[in]     orc_Max              maximum value
   \param[in,out] orc_Value            specific value which is should be set in range
   \param[out]    ore_ValueChangeTo    flag for what was done with value:
                                        value was in range (eNoChange or eZero) respectively out of range (eMin or eMax)
   \param[in]     oe_SetValue          flag for user choice if value should be left in range as it is or set to zero

   \return
   C_NO_ERR    expected standard case, value was set (or left) successfully
   C_CONFIG    min, max and value have not the same datatype
   C_RANGE     min and max values are interchanged (max < min)
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_SdNdeDataPoolContentUtil::h_SetValueInMinMaxRange(const C_OSCNodeDataPoolContent & orc_Min,
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
            if (oe_SetValue == C_SdNdeDataPoolContentUtil::eTO_ZERO)
            {
               C_SdNdeDataPoolContentUtil::h_SetValueInContent(0.0, orc_Value, u32_ItElement);
            }
            orc_Value.GetAnyValueAsFloat64(f64_Value, u32_ItElement);

            // check if value is between min and max value
            // because of possible numeric problems (1.25987e-321) it is checked in an epsilon environment
            // precision is 6 decimal points (should be enough and is openSYDE default)

            if (((f64_Value < f64_Min) && (std::abs(f64_Value - f64_Min) >= mhf64_Epsilon)) ||
                ((f64_Value > f64_Max) && (std::abs(f64_Value - f64_Max) >= mhf64_Epsilon)))
            {
               // initial value not in min and max range, take closer min or max value
               if (std::abs(f64_Value - f64_Min) <= std::abs(f64_Value - f64_Max))
               {
                  ore_ValueChangedTo = C_SdNdeDataPoolContentUtil::eMIN;
                  // set value, special handling for signed/unsigned 64-bit integer because of bit masks and accuracy
                  // problems
                  if ((orc_Value.GetType() != C_OSCNodeDataPoolContent::eSINT64) &&
                      (orc_Value.GetType() != C_OSCNodeDataPoolContent::eUINT64))
                  {
                     C_SdNdeDataPoolContentUtil::h_SetValueInContent(f64_Min, orc_Value, u32_ItElement);
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
                  ore_ValueChangedTo = C_SdNdeDataPoolContentUtil::eMAX;
                  // set value, special handling for signed/unsigned 64-bit integer because of bit masks and accuracy
                  // problems
                  if ((orc_Value.GetType() != C_OSCNodeDataPoolContent::eSINT64) &&
                      (orc_Value.GetType() != C_OSCNodeDataPoolContent::eUINT64))
                  {
                     C_SdNdeDataPoolContentUtil::h_SetValueInContent(f64_Max, orc_Value, u32_ItElement);
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
               if (oe_SetValue == C_SdNdeDataPoolContentUtil::eLEAVE_VALUE)
               {
                  // do nothing with value
                  ore_ValueChangedTo = C_SdNdeDataPoolContentUtil::eNO_CHANGE;
               }
               else
               {
                  // set to zero
                  ore_ValueChangedTo = C_SdNdeDataPoolContentUtil::eZERO;
               }
            }
         }
      }
   }

   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Convert node data pool content type to QVariant

   \param[in] orc_Input   Node data pool content type
   \param[in] oru32_Index Optional array index

   \return
   QVariant content
*/
//----------------------------------------------------------------------------------------------------------------------
QVariant C_SdNdeDataPoolContentUtil::h_ConvertContentToGeneric(const C_OSCNodeDataPoolContent & orc_Input,
                                                               const uint32 & oru32_Index)
{
   QVariant c_Retval;

   if (orc_Input.GetArray() == false)
   {
      switch (orc_Input.GetType())
      {
      case C_OSCNodeDataPoolContent::E_Type::eUINT8:
         c_Retval = static_cast<uint64>(orc_Input.GetValueU8());
         break;
      case C_OSCNodeDataPoolContent::E_Type::eUINT16:
         c_Retval = static_cast<uint64>(orc_Input.GetValueU16());
         break;
      case C_OSCNodeDataPoolContent::E_Type::eUINT32:
         c_Retval = static_cast<uint64>(orc_Input.GetValueU32());
         break;
      case C_OSCNodeDataPoolContent::E_Type::eUINT64:
         c_Retval = orc_Input.GetValueU64();
         break;
      case C_OSCNodeDataPoolContent::E_Type::eSINT8:
         c_Retval = static_cast<sint64>(orc_Input.GetValueS8());
         break;
      case C_OSCNodeDataPoolContent::E_Type::eSINT16:
         c_Retval = static_cast<sint64>(orc_Input.GetValueS16());
         break;
      case C_OSCNodeDataPoolContent::E_Type::eSINT32:
         c_Retval = static_cast<sint64>(orc_Input.GetValueS32());
         break;
      case C_OSCNodeDataPoolContent::E_Type::eSINT64:
         c_Retval = orc_Input.GetValueS64();
         break;
      case C_OSCNodeDataPoolContent::E_Type::eFLOAT32:
         c_Retval = static_cast<float64>(orc_Input.GetValueF32());
         break;
      case C_OSCNodeDataPoolContent::E_Type::eFLOAT64:
         c_Retval = orc_Input.GetValueF64();
         break;
      default:
         //Nothing to do
         break;
      }
   }
   else
   {
      switch (orc_Input.GetType())
      {
      case C_OSCNodeDataPoolContent::E_Type::eUINT8:
         c_Retval = static_cast<uint64>(orc_Input.GetValueAU8Element(oru32_Index));
         break;
      case C_OSCNodeDataPoolContent::E_Type::eUINT16:
         c_Retval = static_cast<uint64>(orc_Input.GetValueAU16Element(oru32_Index));
         break;
      case C_OSCNodeDataPoolContent::E_Type::eUINT32:
         c_Retval = static_cast<uint64>(orc_Input.GetValueAU32Element(oru32_Index));
         break;
      case C_OSCNodeDataPoolContent::E_Type::eUINT64:
         c_Retval = orc_Input.GetValueAU64Element(oru32_Index);
         break;
      case C_OSCNodeDataPoolContent::E_Type::eSINT8:
         c_Retval = static_cast<sint64>(orc_Input.GetValueAS8Element(oru32_Index));
         break;
      case C_OSCNodeDataPoolContent::E_Type::eSINT16:
         c_Retval = static_cast<sint64>(orc_Input.GetValueAS16Element(oru32_Index));
         break;
      case C_OSCNodeDataPoolContent::E_Type::eSINT32:
         c_Retval = static_cast<sint64>(orc_Input.GetValueAS32Element(oru32_Index));
         break;
      case C_OSCNodeDataPoolContent::E_Type::eSINT64:
         c_Retval = orc_Input.GetValueAS64Element(oru32_Index);
         break;
      case C_OSCNodeDataPoolContent::E_Type::eFLOAT32:
         c_Retval = static_cast<float64>(orc_Input.GetValueAF32Element(oru32_Index));
         break;
      case C_OSCNodeDataPoolContent::E_Type::eFLOAT64:
         c_Retval = orc_Input.GetValueAF64Element(oru32_Index);
         break;
      default:
         //Nothing to do
         break;
      }
   }
   return c_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Convert node data pool content type to QVariant using scaling

   \param[in] orc_Input   Node data pool content type
   \param[in] of64_Factor Factor
   \param[in] of64_Offset Offset
   \param[in] oru32_Index Optional array index

   \return
   QVariant content
*/
//----------------------------------------------------------------------------------------------------------------------
QVariant C_SdNdeDataPoolContentUtil::h_ConvertScaledContentToGeneric(const C_OSCNodeDataPoolContent & orc_Input,
                                                                     const float64 of64_Factor,
                                                                     const float64 of64_Offset,
                                                                     const uint32 & oru32_Index)
{
   QVariant c_Retval;

   if (C_OSCUtils::h_IsScalingActive(of64_Factor, of64_Offset) == false)
   {
      c_Retval = C_SdNdeDataPoolContentUtil::h_ConvertContentToGeneric(orc_Input, oru32_Index);
   }
   else
   {
      std::vector<float64> c_Values;
      C_SdNdeDataPoolContentUtil::h_GetValuesAsFloat64(orc_Input, c_Values);
      if (oru32_Index < c_Values.size())
      {
         const float64 f64_ScaledValue = C_OSCUtils::h_GetValueScaled(c_Values[oru32_Index], of64_Factor, of64_Offset);
         if (C_Uti::h_CheckFloatHasNoFractionPart(f64_ScaledValue) == false)
         {
            c_Retval = f64_ScaledValue;
         }
         else
         {
            //Int is possible but only if in range
            if (((f64_ScaledValue < 0.0) &&
                 (f64_ScaledValue >= static_cast<float64>(std::numeric_limits<sint64>::min()))) &&
                (f64_ScaledValue <= static_cast<float64>(std::numeric_limits<sint64>::max())))
            {
               c_Retval = static_cast<sint64>(f64_ScaledValue);
            }
            else if ((f64_ScaledValue >= static_cast<float64>(std::numeric_limits<uint64>::min())) &&
                     (f64_ScaledValue <= static_cast<float64>(std::numeric_limits<uint64>::max())))
            {
               c_Retval = static_cast<uint64>(f64_ScaledValue);
            }
            else
            {
               c_Retval = f64_ScaledValue;
            }
         }
      }
   }
   return c_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set node data pool content according to QVariant data

   \param[in]  orc_Value    Variant value
   \param[out] orc_Output   Node data pool content to set
   \param[in]  oru32_Index  Optional array index
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_SdNdeDataPoolContentUtil::h_SetDataVariableFromGeneric(const QVariant & orc_Value,
                                                                C_OSCNodeDataPoolContent & orc_Output,
                                                                const uint32 & oru32_Index)
{
   sint32 s32_Retval = C_NO_ERR;

   switch (orc_Value.type())
   {
   case QVariant::ULongLong:
      if (orc_Output.GetArray() == false)
      {
         switch (orc_Output.GetType())
         {
         case C_OSCNodeDataPoolContent::E_Type::eUINT8:
            orc_Output.SetValueU8(static_cast<uint8>(orc_Value.toULongLong()));
            break;
         case C_OSCNodeDataPoolContent::E_Type::eUINT16:
            orc_Output.SetValueU16(static_cast<uint16>(orc_Value.toULongLong()));
            break;
         case C_OSCNodeDataPoolContent::E_Type::eUINT32:
            orc_Output.SetValueU32(static_cast<uint32>(orc_Value.toULongLong()));
            break;
         case C_OSCNodeDataPoolContent::E_Type::eUINT64:
            orc_Output.SetValueU64(static_cast<uint64>(orc_Value.toULongLong()));
            break;
         case C_OSCNodeDataPoolContent::E_Type::eSINT8:
            orc_Output.SetValueS8(static_cast<sint8>(orc_Value.toULongLong()));
            break;
         case C_OSCNodeDataPoolContent::E_Type::eSINT16:
            orc_Output.SetValueS16(static_cast<sint16>(orc_Value.toULongLong()));
            break;
         case C_OSCNodeDataPoolContent::E_Type::eSINT32:
            orc_Output.SetValueS32(static_cast<sint32>(orc_Value.toULongLong()));
            break;
         case C_OSCNodeDataPoolContent::E_Type::eSINT64:
            orc_Output.SetValueS64(static_cast<sint64>(orc_Value.toULongLong()));
            break;
         case C_OSCNodeDataPoolContent::E_Type::eFLOAT32:
            orc_Output.SetValueF32(static_cast<float32>(orc_Value.toULongLong()));
            break;
         case C_OSCNodeDataPoolContent::E_Type::eFLOAT64:
            orc_Output.SetValueF64(static_cast<float64>(orc_Value.toULongLong()));
            break;
         default:
            //Nothing to do
            break;
         }
      }
      else
      {
         switch (orc_Output.GetType())
         {
         case C_OSCNodeDataPoolContent::E_Type::eUINT8:
            orc_Output.SetValueAU8Element(static_cast<uint8>(orc_Value.toULongLong()), oru32_Index);
            break;
         case C_OSCNodeDataPoolContent::E_Type::eUINT16:
            orc_Output.SetValueAU16Element(static_cast<uint16>(orc_Value.toULongLong()), oru32_Index);
            break;
         case C_OSCNodeDataPoolContent::E_Type::eUINT32:
            orc_Output.SetValueAU32Element(static_cast<uint32>(orc_Value.toULongLong()), oru32_Index);
            break;
         case C_OSCNodeDataPoolContent::E_Type::eUINT64:
            orc_Output.SetValueAU64Element(static_cast<uint64>(orc_Value.toULongLong()), oru32_Index);
            break;
         case C_OSCNodeDataPoolContent::E_Type::eSINT8:
            orc_Output.SetValueAS8Element(static_cast<sint8>(orc_Value.toULongLong()), oru32_Index);
            break;
         case C_OSCNodeDataPoolContent::E_Type::eSINT16:
            orc_Output.SetValueAS16Element(static_cast<sint16>(orc_Value.toULongLong()), oru32_Index);
            break;
         case C_OSCNodeDataPoolContent::E_Type::eSINT32:
            orc_Output.SetValueAS32Element(static_cast<sint32>(orc_Value.toULongLong()), oru32_Index);
            break;
         case C_OSCNodeDataPoolContent::E_Type::eSINT64:
            orc_Output.SetValueAS64Element(static_cast<sint64>(orc_Value.toULongLong()), oru32_Index);
            break;
         case C_OSCNodeDataPoolContent::E_Type::eFLOAT32:
            orc_Output.SetValueAF32Element(static_cast<float32>(orc_Value.toULongLong()), oru32_Index);
            break;
         case C_OSCNodeDataPoolContent::E_Type::eFLOAT64:
            orc_Output.SetValueAF64Element(static_cast<float64>(orc_Value.toULongLong()), oru32_Index);
            break;
         default:
            //Nothing to do
            break;
         }
      }
      break;
   case QVariant::LongLong:
      if (orc_Output.GetArray() == false)
      {
         switch (orc_Output.GetType())
         {
         case C_OSCNodeDataPoolContent::E_Type::eUINT8:
            orc_Output.SetValueU8(static_cast<uint8>(orc_Value.toLongLong()));
            break;
         case C_OSCNodeDataPoolContent::E_Type::eUINT16:
            orc_Output.SetValueU16(static_cast<uint16>(orc_Value.toLongLong()));
            break;
         case C_OSCNodeDataPoolContent::E_Type::eUINT32:
            orc_Output.SetValueU32(static_cast<uint32>(orc_Value.toLongLong()));
            break;
         case C_OSCNodeDataPoolContent::E_Type::eUINT64:
            orc_Output.SetValueU64(static_cast<uint64>(orc_Value.toLongLong()));
            break;
         case C_OSCNodeDataPoolContent::E_Type::eSINT8:
            orc_Output.SetValueS8(static_cast<sint8>(orc_Value.toLongLong()));
            break;
         case C_OSCNodeDataPoolContent::E_Type::eSINT16:
            orc_Output.SetValueS16(static_cast<sint16>(orc_Value.toLongLong()));
            break;
         case C_OSCNodeDataPoolContent::E_Type::eSINT32:
            orc_Output.SetValueS32(static_cast<sint32>(orc_Value.toLongLong()));
            break;
         case C_OSCNodeDataPoolContent::E_Type::eSINT64:
            orc_Output.SetValueS64(static_cast<sint64>(orc_Value.toLongLong()));
            break;
         case C_OSCNodeDataPoolContent::E_Type::eFLOAT32:
            orc_Output.SetValueF32(static_cast<float32>(orc_Value.toLongLong()));
            break;
         case C_OSCNodeDataPoolContent::E_Type::eFLOAT64:
            orc_Output.SetValueF64(static_cast<float64>(orc_Value.toLongLong()));
            break;
         default:
            //Nothing to do
            break;
         }
      }
      else
      {
         switch (orc_Output.GetType())
         {
         case C_OSCNodeDataPoolContent::E_Type::eUINT8:
            orc_Output.SetValueAU8Element(static_cast<uint8>(orc_Value.toLongLong()), oru32_Index);
            break;
         case C_OSCNodeDataPoolContent::E_Type::eUINT16:
            orc_Output.SetValueAU16Element(static_cast<uint16>(orc_Value.toLongLong()), oru32_Index);
            break;
         case C_OSCNodeDataPoolContent::E_Type::eUINT32:
            orc_Output.SetValueAU32Element(static_cast<uint32>(orc_Value.toLongLong()), oru32_Index);
            break;
         case C_OSCNodeDataPoolContent::E_Type::eUINT64:
            orc_Output.SetValueAU64Element(static_cast<uint64>(orc_Value.toLongLong()), oru32_Index);
            break;
         case C_OSCNodeDataPoolContent::E_Type::eSINT8:
            orc_Output.SetValueAS8Element(static_cast<sint8>(orc_Value.toLongLong()), oru32_Index);
            break;
         case C_OSCNodeDataPoolContent::E_Type::eSINT16:
            orc_Output.SetValueAS16Element(static_cast<sint16>(orc_Value.toLongLong()), oru32_Index);
            break;
         case C_OSCNodeDataPoolContent::E_Type::eSINT32:
            orc_Output.SetValueAS32Element(static_cast<sint32>(orc_Value.toLongLong()), oru32_Index);
            break;
         case C_OSCNodeDataPoolContent::E_Type::eSINT64:
            orc_Output.SetValueAS64Element(static_cast<sint64>(orc_Value.toLongLong()), oru32_Index);
            break;
         case C_OSCNodeDataPoolContent::E_Type::eFLOAT32:
            orc_Output.SetValueAF32Element(static_cast<float32>(orc_Value.toLongLong()), oru32_Index);
            break;
         case C_OSCNodeDataPoolContent::E_Type::eFLOAT64:
            orc_Output.SetValueAF64Element(static_cast<float64>(orc_Value.toLongLong()), oru32_Index);
            break;
         default:
            //Nothing to do
            break;
         }
      }
      break;
   case QVariant::UInt:
      if (orc_Output.GetArray() == false)
      {
         switch (orc_Output.GetType())
         {
         case C_OSCNodeDataPoolContent::E_Type::eUINT8:
            orc_Output.SetValueU8(static_cast<uint8>(orc_Value.toUInt()));
            break;
         case C_OSCNodeDataPoolContent::E_Type::eUINT16:
            orc_Output.SetValueU16(static_cast<uint16>(orc_Value.toUInt()));
            break;
         case C_OSCNodeDataPoolContent::E_Type::eUINT32:
            orc_Output.SetValueU32(static_cast<uint32>(orc_Value.toUInt()));
            break;
         case C_OSCNodeDataPoolContent::E_Type::eUINT64:
            orc_Output.SetValueU64(static_cast<uint64>(orc_Value.toUInt()));
            break;
         case C_OSCNodeDataPoolContent::E_Type::eSINT8:
            orc_Output.SetValueS8(static_cast<sint8>(orc_Value.toUInt()));
            break;
         case C_OSCNodeDataPoolContent::E_Type::eSINT16:
            orc_Output.SetValueS16(static_cast<sint16>(orc_Value.toUInt()));
            break;
         case C_OSCNodeDataPoolContent::E_Type::eSINT32:
            orc_Output.SetValueS32(static_cast<sint32>(orc_Value.toUInt()));
            break;
         case C_OSCNodeDataPoolContent::E_Type::eSINT64:
            orc_Output.SetValueS64(static_cast<sint64>(orc_Value.toUInt()));
            break;
         case C_OSCNodeDataPoolContent::E_Type::eFLOAT32:
            orc_Output.SetValueF32(static_cast<float32>(orc_Value.toUInt()));
            break;
         case C_OSCNodeDataPoolContent::E_Type::eFLOAT64:
            orc_Output.SetValueF64(static_cast<float64>(orc_Value.toUInt()));
            break;
         default:
            //Nothing to do
            break;
         }
      }
      else
      {
         switch (orc_Output.GetType())
         {
         case C_OSCNodeDataPoolContent::E_Type::eUINT8:
            orc_Output.SetValueAU8Element(static_cast<uint8>(orc_Value.toUInt()), oru32_Index);
            break;
         case C_OSCNodeDataPoolContent::E_Type::eUINT16:
            orc_Output.SetValueAU16Element(static_cast<uint16>(orc_Value.toUInt()), oru32_Index);
            break;
         case C_OSCNodeDataPoolContent::E_Type::eUINT32:
            orc_Output.SetValueAU32Element(static_cast<uint32>(orc_Value.toUInt()), oru32_Index);
            break;
         case C_OSCNodeDataPoolContent::E_Type::eUINT64:
            orc_Output.SetValueAU64Element(static_cast<uint64>(orc_Value.toUInt()), oru32_Index);
            break;
         case C_OSCNodeDataPoolContent::E_Type::eSINT8:
            orc_Output.SetValueAS8Element(static_cast<sint8>(orc_Value.toUInt()), oru32_Index);
            break;
         case C_OSCNodeDataPoolContent::E_Type::eSINT16:
            orc_Output.SetValueAS16Element(static_cast<sint16>(orc_Value.toUInt()), oru32_Index);
            break;
         case C_OSCNodeDataPoolContent::E_Type::eSINT32:
            orc_Output.SetValueAS32Element(static_cast<sint32>(orc_Value.toUInt()), oru32_Index);
            break;
         case C_OSCNodeDataPoolContent::E_Type::eSINT64:
            orc_Output.SetValueAS64Element(static_cast<sint64>(orc_Value.toUInt()), oru32_Index);
            break;
         case C_OSCNodeDataPoolContent::E_Type::eFLOAT32:
            orc_Output.SetValueAF32Element(static_cast<float32>(orc_Value.toUInt()), oru32_Index);
            break;
         case C_OSCNodeDataPoolContent::E_Type::eFLOAT64:
            orc_Output.SetValueAF64Element(static_cast<float64>(orc_Value.toUInt()), oru32_Index);
            break;
         default:
            //Nothing to do
            break;
         }
      }
      break;
   case QVariant::Int:
      if (orc_Output.GetArray() == false)
      {
         switch (orc_Output.GetType())
         {
         case C_OSCNodeDataPoolContent::E_Type::eUINT8:
            orc_Output.SetValueU8(static_cast<uint8>(orc_Value.toInt()));
            break;
         case C_OSCNodeDataPoolContent::E_Type::eUINT16:
            orc_Output.SetValueU16(static_cast<uint16>(orc_Value.toInt()));
            break;
         case C_OSCNodeDataPoolContent::E_Type::eUINT32:
            orc_Output.SetValueU32(static_cast<uint32>(orc_Value.toInt()));
            break;
         case C_OSCNodeDataPoolContent::E_Type::eUINT64:
            orc_Output.SetValueU64(static_cast<uint64>(static_cast<uint32>(orc_Value.toInt())));
            break;
         case C_OSCNodeDataPoolContent::E_Type::eSINT8:
            orc_Output.SetValueS8(static_cast<sint8>(orc_Value.toInt()));
            break;
         case C_OSCNodeDataPoolContent::E_Type::eSINT16:
            orc_Output.SetValueS16(static_cast<sint16>(orc_Value.toInt()));
            break;
         case C_OSCNodeDataPoolContent::E_Type::eSINT32:
            orc_Output.SetValueS32(static_cast<sint32>(orc_Value.toInt()));
            break;
         case C_OSCNodeDataPoolContent::E_Type::eSINT64:
            orc_Output.SetValueS64(static_cast<sint64>(orc_Value.toInt()));
            break;
         case C_OSCNodeDataPoolContent::E_Type::eFLOAT32:
            orc_Output.SetValueF32(static_cast<float32>(orc_Value.toInt()));
            break;
         case C_OSCNodeDataPoolContent::E_Type::eFLOAT64:
            orc_Output.SetValueF64(static_cast<float64>(orc_Value.toInt()));
            break;
         default:
            //Nothing to do
            break;
         }
      }
      else
      {
         switch (orc_Output.GetType())
         {
         case C_OSCNodeDataPoolContent::E_Type::eUINT8:
            orc_Output.SetValueAU8Element(static_cast<uint8>(orc_Value.toInt()), oru32_Index);
            break;
         case C_OSCNodeDataPoolContent::E_Type::eUINT16:
            orc_Output.SetValueAU16Element(static_cast<uint16>(orc_Value.toInt()), oru32_Index);
            break;
         case C_OSCNodeDataPoolContent::E_Type::eUINT32:
            orc_Output.SetValueAU32Element(static_cast<uint32>(orc_Value.toInt()), oru32_Index);
            break;
         case C_OSCNodeDataPoolContent::E_Type::eUINT64:
            orc_Output.SetValueAU64Element(static_cast<uint64>(static_cast<uint32>(orc_Value.toInt())), oru32_Index);
            break;
         case C_OSCNodeDataPoolContent::E_Type::eSINT8:
            orc_Output.SetValueAS8Element(static_cast<sint8>(orc_Value.toInt()), oru32_Index);
            break;
         case C_OSCNodeDataPoolContent::E_Type::eSINT16:
            orc_Output.SetValueAS16Element(static_cast<sint16>(orc_Value.toInt()), oru32_Index);
            break;
         case C_OSCNodeDataPoolContent::E_Type::eSINT32:
            orc_Output.SetValueAS32Element(static_cast<sint32>(orc_Value.toInt()), oru32_Index);
            break;
         case C_OSCNodeDataPoolContent::E_Type::eSINT64:
            orc_Output.SetValueAS64Element(static_cast<sint64>(orc_Value.toInt()), oru32_Index);
            break;
         case C_OSCNodeDataPoolContent::E_Type::eFLOAT32:
            orc_Output.SetValueAF32Element(static_cast<float32>(orc_Value.toInt()), oru32_Index);
            break;
         case C_OSCNodeDataPoolContent::E_Type::eFLOAT64:
            orc_Output.SetValueAF64Element(static_cast<float64>(orc_Value.toInt()), oru32_Index);
            break;
         default:
            //Nothing to do
            break;
         }
      }
      break;
   case QVariant::Double:
      if (orc_Output.GetArray() == false)
      {
         switch (orc_Output.GetType())
         {
         case C_OSCNodeDataPoolContent::E_Type::eUINT8:
            orc_Output.SetValueU8(static_cast<uint8>(orc_Value.toDouble()));
            break;
         case C_OSCNodeDataPoolContent::E_Type::eUINT16:
            orc_Output.SetValueU16(static_cast<uint16>(orc_Value.toDouble()));
            break;
         case C_OSCNodeDataPoolContent::E_Type::eUINT32:
            orc_Output.SetValueU32(static_cast<uint32>(orc_Value.toDouble()));
            break;
         case C_OSCNodeDataPoolContent::E_Type::eUINT64:
            orc_Output.SetValueU64(static_cast<uint64>(orc_Value.toDouble()));
            break;
         case C_OSCNodeDataPoolContent::E_Type::eSINT8:
            orc_Output.SetValueS8(static_cast<sint8>(orc_Value.toDouble()));
            break;
         case C_OSCNodeDataPoolContent::E_Type::eSINT16:
            orc_Output.SetValueS16(static_cast<sint16>(orc_Value.toDouble()));
            break;
         case C_OSCNodeDataPoolContent::E_Type::eSINT32:
            orc_Output.SetValueS32(static_cast<sint32>(orc_Value.toDouble()));
            break;
         case C_OSCNodeDataPoolContent::E_Type::eSINT64:
            orc_Output.SetValueS64(static_cast<sint64>(orc_Value.toDouble()));
            break;
         case C_OSCNodeDataPoolContent::E_Type::eFLOAT32:
            orc_Output.SetValueF32(static_cast<float32>(orc_Value.toDouble()));
            break;
         case C_OSCNodeDataPoolContent::E_Type::eFLOAT64:
            orc_Output.SetValueF64(static_cast<float64>(orc_Value.toDouble()));
            break;
         default:
            //Nothing to do
            break;
         }
      }
      else
      {
         switch (orc_Output.GetType())
         {
         case C_OSCNodeDataPoolContent::E_Type::eUINT8:
            orc_Output.SetValueAU8Element(static_cast<uint8>(orc_Value.toDouble()), oru32_Index);
            break;
         case C_OSCNodeDataPoolContent::E_Type::eUINT16:
            orc_Output.SetValueAU16Element(static_cast<uint16>(orc_Value.toDouble()), oru32_Index);
            break;
         case C_OSCNodeDataPoolContent::E_Type::eUINT32:
            orc_Output.SetValueAU32Element(static_cast<uint32>(orc_Value.toDouble()), oru32_Index);
            break;
         case C_OSCNodeDataPoolContent::E_Type::eUINT64:
            orc_Output.SetValueAU64Element(static_cast<uint64>(orc_Value.toDouble()), oru32_Index);
            break;
         case C_OSCNodeDataPoolContent::E_Type::eSINT8:
            orc_Output.SetValueAS8Element(static_cast<sint8>(orc_Value.toDouble()), oru32_Index);
            break;
         case C_OSCNodeDataPoolContent::E_Type::eSINT16:
            orc_Output.SetValueAS16Element(static_cast<sint16>(orc_Value.toDouble()), oru32_Index);
            break;
         case C_OSCNodeDataPoolContent::E_Type::eSINT32:
            orc_Output.SetValueAS32Element(static_cast<sint32>(orc_Value.toDouble()), oru32_Index);
            break;
         case C_OSCNodeDataPoolContent::E_Type::eSINT64:
            orc_Output.SetValueAS64Element(static_cast<sint64>(orc_Value.toDouble()), oru32_Index);
            break;
         case C_OSCNodeDataPoolContent::E_Type::eFLOAT32:
            orc_Output.SetValueAF32Element(static_cast<float32>(orc_Value.toDouble()), oru32_Index);
            break;
         case C_OSCNodeDataPoolContent::E_Type::eFLOAT64:
            orc_Output.SetValueAF64Element(static_cast<float64>(orc_Value.toDouble()), oru32_Index);
            break;
         default:
            //Nothing to do
            break;
         }
      }
      break;
   default:
      //Unknown conversion
      s32_Retval = C_RANGE;
      break;
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set node data pool content according to QVariant data using scaling

   \param[in]  orc_Value    Variant value
   \param[out] orc_Output   Node data pool content to set
   \param[in]  of64_Factor  Factor
   \param[in]  of64_Offset  Offset
   \param[in]  oru32_Index  Optional array index
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_SdNdeDataPoolContentUtil::h_SetDataVariableFromGenericWithScaling(const QVariant & orc_Value,
                                                                           C_OSCNodeDataPoolContent & orc_Output,
                                                                           const float64 of64_Factor,
                                                                           const float64 of64_Offset,
                                                                           const uint32 & oru32_Index)
{
   sint32 s32_Retval = C_NO_ERR;

   if (C_OSCUtils::h_IsScalingActive(of64_Factor, of64_Offset) == false)
   {
      C_SdNdeDataPoolContentUtil::h_SetDataVariableFromGeneric(orc_Value, orc_Output, oru32_Index);
   }
   else
   {
      float64 f64_TmpValue = 0.0;
      bool q_Success;

      switch (orc_Value.type())
      {
      case QVariant::Int:
         f64_TmpValue = static_cast<float64>(orc_Value.toInt(&q_Success));
         break;
      case QVariant::UInt:
         f64_TmpValue = static_cast<float64>(orc_Value.toUInt(&q_Success));
         break;
      case QVariant::LongLong:
         f64_TmpValue = static_cast<float64>(orc_Value.toLongLong(&q_Success));
         break;
      case QVariant::ULongLong:
         f64_TmpValue = static_cast<float64>(orc_Value.toULongLong(&q_Success));
         break;
      case QVariant::Double:
         f64_TmpValue = orc_Value.toDouble(&q_Success);
         break;
      default:
         q_Success = false;
         break;
      }
      if (q_Success == true)
      {
         C_SdNdeDataPoolContentUtil::h_SetScaledValueInContent(f64_TmpValue, orc_Output, of64_Factor, of64_Offset,
                                                               oru32_Index);
      }
      else
      {
         s32_Retval = C_RANGE;
      }
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Store current content as string

   Use case: store to string to later use the same string to restore the same content

   \param[in] orc_Source Content to store

   \return
   Current content encoded as string
*/
//----------------------------------------------------------------------------------------------------------------------
QString C_SdNdeDataPoolContentUtil::h_GetAllContentAsString(const C_OSCNodeDataPoolContent & orc_Input)
{
   stw_scl::C_SCLString c_Retval;
   C_OSCXMLParserString c_XML;
   c_XML.CreateAndSelectNodeChild("opensyde-content");
   C_OSCNodeDataPoolFiler::h_SaveDataPoolContentV1(orc_Input, c_XML);
   c_XML.SaveToString(c_Retval);
   return c_Retval.c_str();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Restore content from string

   Use case: store to string to later use the same string to restore the same content

   \param[in]  orc_Input  String to base restoration on
   \param[out] orc_Output Output of restoration

   \return
   C_NO_ERR   data read
   C_CONFIG   content of file is invalid or incomplete
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_SdNdeDataPoolContentUtil::h_SetAllContentFromString(const QString & orc_Input,
                                                             C_OSCNodeDataPoolContent & orc_Output)
{
   C_OSCXMLParserString c_XML;

   c_XML.LoadFromString(orc_Input.toStdString().c_str());
   c_XML.SelectRoot();
   return C_OSCNodeDataPoolFiler::h_LoadDataPoolContentV1(orc_Output, c_XML);
}
