//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Utilitiy functions for C_OscNodeDataPoolContent access (implementation)

   Utility functions for C_OscNodeDataPoolContent access

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.hpp"

#include <cmath>
#include <limits>
#include <QLocale>
#include "C_Uti.hpp"
#include "TglUtils.hpp"
#include "stwtypes.hpp"
#include "stwerrors.hpp"
#include "C_OscUtils.hpp"
#include "C_OscXmlParser.hpp"
#include "C_OscNodeDataPoolFiler.hpp"
#include "C_SdNdeDpContentUtil.hpp"
#include "C_OscNodeDataPoolContentUtil.hpp"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw::tgl;
using namespace stw::errors;
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
C_SdNdeDpContentUtil::C_SdNdeDpContentUtil(void)
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Convert value to double

   \param[in]   orc_Content   Data element value
   \param[out]  orf64_Output  Value as double
   \param[in]   ou32_Index    Index

   \return
   C_NO_ERR Success
   C_RANGE  Content is a array
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_SdNdeDpContentUtil::h_GetValueAsFloat64(const C_OscNodeDataPoolContent & orc_Content,
                                                  float64_t & orf64_Output, const uint32_t ou32_Index)
{
   int32_t s32_Retval = C_NO_ERR;

   orf64_Output = 0.0;

   if (orc_Content.GetArray())
   {
      if (ou32_Index < orc_Content.GetArraySize())
      {
         switch (orc_Content.GetType())
         {
         case C_OscNodeDataPoolContent::eUINT8:
            orf64_Output = static_cast<float64_t>(orc_Content.GetValueArrU8Element(ou32_Index));
            break;
         case C_OscNodeDataPoolContent::eUINT16:
            orf64_Output = static_cast<float64_t>(orc_Content.GetValueArrU16Element(ou32_Index));
            break;
         case C_OscNodeDataPoolContent::eUINT32:
            orf64_Output = static_cast<float64_t>(orc_Content.GetValueArrU32Element(ou32_Index));
            break;
         case C_OscNodeDataPoolContent::eUINT64:
            orf64_Output = static_cast<float64_t>(orc_Content.GetValueArrU64Element(ou32_Index));
            break;
         case C_OscNodeDataPoolContent::eSINT8:
            orf64_Output = static_cast<float64_t>(orc_Content.GetValueArrS8Element(ou32_Index));
            break;
         case C_OscNodeDataPoolContent::eSINT16:
            orf64_Output = static_cast<float64_t>(orc_Content.GetValueArrS16Element(ou32_Index));
            break;
         case C_OscNodeDataPoolContent::eSINT32:
            orf64_Output = static_cast<float64_t>(orc_Content.GetValueArrS32Element(ou32_Index));
            break;
         case C_OscNodeDataPoolContent::eSINT64:
            orf64_Output = static_cast<float64_t>(orc_Content.GetValueArrS64Element(ou32_Index));
            break;
         case C_OscNodeDataPoolContent::eFLOAT32:
            orf64_Output = static_cast<float64_t>(orc_Content.GetValueArrF32Element(ou32_Index));
            break;
         case C_OscNodeDataPoolContent::eFLOAT64:
            orf64_Output = orc_Content.GetValueArrF64Element(ou32_Index);
            break;
         default:
            break;
         }
      }
      else
      {
         //Not supported
         s32_Retval = C_RANGE;
      }
   }
   else
   {
      switch (orc_Content.GetType())
      {
      case C_OscNodeDataPoolContent::eUINT8:
         orf64_Output = static_cast<float64_t>(orc_Content.GetValueU8());
         break;
      case C_OscNodeDataPoolContent::eUINT16:
         orf64_Output = static_cast<float64_t>(orc_Content.GetValueU16());
         break;
      case C_OscNodeDataPoolContent::eUINT32:
         orf64_Output = static_cast<float64_t>(orc_Content.GetValueU32());
         break;
      case C_OscNodeDataPoolContent::eUINT64:
         orf64_Output = static_cast<float64_t>(orc_Content.GetValueU64());
         break;
      case C_OscNodeDataPoolContent::eSINT8:
         orf64_Output = static_cast<float64_t>(orc_Content.GetValueS8());
         break;
      case C_OscNodeDataPoolContent::eSINT16:
         orf64_Output = static_cast<float64_t>(orc_Content.GetValueS16());
         break;
      case C_OscNodeDataPoolContent::eSINT32:
         orf64_Output = static_cast<float64_t>(orc_Content.GetValueS32());
         break;
      case C_OscNodeDataPoolContent::eSINT64:
         orf64_Output = static_cast<float64_t>(orc_Content.GetValueS64());
         break;
      case C_OscNodeDataPoolContent::eFLOAT32:
         orf64_Output = static_cast<float64_t>(orc_Content.GetValueF32());
         break;
      case C_OscNodeDataPoolContent::eFLOAT64:
         orf64_Output = orc_Content.GetValueF64();
         break;
      default:
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

   \param[in]  orc_Content    Data element value
   \param[in]  orc_Output     Vector with values
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDpContentUtil::h_GetValuesAsFloat64(const C_OscNodeDataPoolContent & orc_Content,
                                                std::vector<float64_t> & orc_Output)
{
   float64_t f64_Value = 0.0;

   orc_Output.clear();

   if (orc_Content.GetArray() == true)
   {
      for (uint32_t u32_It = 0; u32_It < orc_Content.GetArraySize(); ++u32_It)
      {
         switch (orc_Content.GetType())
         {
         case C_OscNodeDataPoolContent::eUINT8:
            f64_Value = static_cast<float64_t>(orc_Content.GetValueArrU8Element(u32_It));
            break;
         case C_OscNodeDataPoolContent::eUINT16:
            f64_Value = static_cast<float64_t>(orc_Content.GetValueArrU16Element(u32_It));
            break;
         case C_OscNodeDataPoolContent::eUINT32:
            f64_Value = static_cast<float64_t>(orc_Content.GetValueArrU32Element(u32_It));
            break;
         case C_OscNodeDataPoolContent::eUINT64:
            f64_Value = static_cast<float64_t>(orc_Content.GetValueArrU64Element(u32_It));
            break;
         case C_OscNodeDataPoolContent::eSINT8:
            f64_Value = static_cast<float64_t>(orc_Content.GetValueArrS8Element(u32_It));
            break;
         case C_OscNodeDataPoolContent::eSINT16:
            f64_Value = static_cast<float64_t>(orc_Content.GetValueArrS16Element(u32_It));
            break;
         case C_OscNodeDataPoolContent::eSINT32:
            f64_Value = static_cast<float64_t>(orc_Content.GetValueArrS32Element(u32_It));
            break;
         case C_OscNodeDataPoolContent::eSINT64:
            f64_Value = static_cast<float64_t>(orc_Content.GetValueArrS64Element(u32_It));
            break;
         case C_OscNodeDataPoolContent::eFLOAT32:
            f64_Value = static_cast<float64_t>(orc_Content.GetValueArrF32Element(u32_It));
            break;
         case C_OscNodeDataPoolContent::eFLOAT64:
            f64_Value = orc_Content.GetValueArrF64Element(u32_It);
            break;
         default:
            break;
         }

         orc_Output.push_back(f64_Value);
      }
   }
   else
   {
      C_SdNdeDpContentUtil::h_GetValueAsFloat64(orc_Content, f64_Value, 0UL);
      orc_Output.push_back(f64_Value);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Convert value to uint64

   \param[in]  orc_Content    Data element value
   \param[in]  oru64_Output   Output value
   \param[in]  ou32_Index     Optional fallback index if array
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDpContentUtil::h_GetAnyValueAsUint64(const C_OscNodeDataPoolContent & orc_Content, uint64_t & oru64_Output,
                                                 const uint32_t ou32_Index)
{
   oru64_Output = 0;

   if (orc_Content.GetArray() == true)
   {
      if (ou32_Index < orc_Content.GetArraySize())
      {
         switch (orc_Content.GetType())
         {
         case C_OscNodeDataPoolContent::eUINT8:
            oru64_Output = static_cast<uint64_t>(orc_Content.GetValueArrU8Element(ou32_Index));
            break;
         case C_OscNodeDataPoolContent::eUINT16:
            oru64_Output = static_cast<uint64_t>(orc_Content.GetValueArrU16Element(ou32_Index));
            break;
         case C_OscNodeDataPoolContent::eUINT32:
            oru64_Output = static_cast<uint64_t>(orc_Content.GetValueArrU32Element(ou32_Index));
            break;
         case C_OscNodeDataPoolContent::eUINT64:
            oru64_Output = static_cast<uint64_t>(orc_Content.GetValueArrU64Element(ou32_Index));
            break;
         case C_OscNodeDataPoolContent::eSINT8:
            oru64_Output = static_cast<uint64_t>(static_cast<uint8_t>(orc_Content.GetValueArrS8Element(ou32_Index)));
            break;
         case C_OscNodeDataPoolContent::eSINT16:
            oru64_Output = static_cast<uint64_t>(static_cast<uint16_t>(orc_Content.GetValueArrS16Element(ou32_Index)));
            break;
         case C_OscNodeDataPoolContent::eSINT32:
            oru64_Output = static_cast<uint64_t>(static_cast<uint32_t>(orc_Content.GetValueArrS32Element(ou32_Index)));
            break;
         case C_OscNodeDataPoolContent::eSINT64:
            oru64_Output = static_cast<uint64_t>(orc_Content.GetValueArrS64Element(ou32_Index));
            break;
         case C_OscNodeDataPoolContent::eFLOAT32:
            oru64_Output = static_cast<uint64_t>(orc_Content.GetValueArrF32Element(ou32_Index));
            break;
         case C_OscNodeDataPoolContent::eFLOAT64:
            oru64_Output = static_cast<uint64_t>(orc_Content.GetValueArrF64Element(ou32_Index));
            break;
         default:
            break;
         }
      }
   }
   else
   {
      switch (orc_Content.GetType())
      {
      case C_OscNodeDataPoolContent::eUINT8:
         oru64_Output = static_cast<uint64_t>(orc_Content.GetValueU8());
         break;
      case C_OscNodeDataPoolContent::eUINT16:
         oru64_Output = static_cast<uint64_t>(orc_Content.GetValueU16());
         break;
      case C_OscNodeDataPoolContent::eUINT32:
         oru64_Output = static_cast<uint64_t>(orc_Content.GetValueU32());
         break;
      case C_OscNodeDataPoolContent::eUINT64:
         oru64_Output = static_cast<uint64_t>(orc_Content.GetValueU64());
         break;
      case C_OscNodeDataPoolContent::eSINT8:
         oru64_Output = static_cast<uint64_t>(static_cast<uint8_t>(orc_Content.GetValueS8()));
         break;
      case C_OscNodeDataPoolContent::eSINT16:
         oru64_Output = static_cast<uint64_t>(static_cast<uint16_t>(orc_Content.GetValueS16()));
         break;
      case C_OscNodeDataPoolContent::eSINT32:
         oru64_Output = static_cast<uint64_t>(static_cast<uint32_t>(orc_Content.GetValueS32()));
         break;
      case C_OscNodeDataPoolContent::eSINT64:
         oru64_Output = static_cast<uint64_t>(orc_Content.GetValueS64());
         break;
      case C_OscNodeDataPoolContent::eFLOAT32:
         oru64_Output = static_cast<uint64_t>(orc_Content.GetValueF32());
         break;
      case C_OscNodeDataPoolContent::eFLOAT64:
         oru64_Output = static_cast<uint64_t>(orc_Content.GetValueF64());
         break;
      default:
         break;
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Convert value to sint64

   \param[in]   orc_Content   Data element value
   \param[out]  ors64_Output  Output value
   \param[in]   ou32_Index    Optional fallback index if array
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDpContentUtil::h_GetAnyValueAsSint64(const C_OscNodeDataPoolContent & orc_Content, int64_t & ors64_Output,
                                                 const uint32_t ou32_Index)
{
   ors64_Output = 0;

   if (orc_Content.GetArray() == true)
   {
      if (ou32_Index < orc_Content.GetArraySize())
      {
         switch (orc_Content.GetType())
         {
         case C_OscNodeDataPoolContent::eUINT8:
            ors64_Output = static_cast<int64_t>(orc_Content.GetValueArrU8Element(ou32_Index));
            break;
         case C_OscNodeDataPoolContent::eUINT16:
            ors64_Output = static_cast<int64_t>(orc_Content.GetValueArrU16Element(ou32_Index));
            break;
         case C_OscNodeDataPoolContent::eUINT32:
            ors64_Output = static_cast<int64_t>(orc_Content.GetValueArrU32Element(ou32_Index));
            break;
         case C_OscNodeDataPoolContent::eUINT64:
            ors64_Output = static_cast<int64_t>(orc_Content.GetValueArrU64Element(ou32_Index));
            break;
         case C_OscNodeDataPoolContent::eSINT8:
            ors64_Output = static_cast<int64_t>(orc_Content.GetValueArrS8Element(ou32_Index));
            break;
         case C_OscNodeDataPoolContent::eSINT16:
            ors64_Output = static_cast<int64_t>(orc_Content.GetValueArrS16Element(ou32_Index));
            break;
         case C_OscNodeDataPoolContent::eSINT32:
            ors64_Output = static_cast<int64_t>(orc_Content.GetValueArrS32Element(ou32_Index));
            break;
         case C_OscNodeDataPoolContent::eSINT64:
            ors64_Output = static_cast<int64_t>(orc_Content.GetValueArrS64Element(ou32_Index));
            break;
         case C_OscNodeDataPoolContent::eFLOAT32:
            ors64_Output = static_cast<int64_t>(orc_Content.GetValueArrF32Element(ou32_Index));
            break;
         case C_OscNodeDataPoolContent::eFLOAT64:
            ors64_Output = static_cast<int64_t>(orc_Content.GetValueArrF64Element(ou32_Index));
            break;
         default:
            break;
         }
      }
   }
   else
   {
      switch (orc_Content.GetType())
      {
      case C_OscNodeDataPoolContent::eUINT8:
         ors64_Output = static_cast<int64_t>(orc_Content.GetValueU8());
         break;
      case C_OscNodeDataPoolContent::eUINT16:
         ors64_Output = static_cast<int64_t>(orc_Content.GetValueU16());
         break;
      case C_OscNodeDataPoolContent::eUINT32:
         ors64_Output = static_cast<int64_t>(orc_Content.GetValueU32());
         break;
      case C_OscNodeDataPoolContent::eUINT64:
         ors64_Output = static_cast<int64_t>(orc_Content.GetValueU64());
         break;
      case C_OscNodeDataPoolContent::eSINT8:
         ors64_Output = static_cast<int64_t>(orc_Content.GetValueS8());
         break;
      case C_OscNodeDataPoolContent::eSINT16:
         ors64_Output = static_cast<int64_t>(orc_Content.GetValueS16());
         break;
      case C_OscNodeDataPoolContent::eSINT32:
         ors64_Output = static_cast<int64_t>(orc_Content.GetValueS32());
         break;
      case C_OscNodeDataPoolContent::eSINT64:
         ors64_Output = static_cast<int64_t>(orc_Content.GetValueS64());
         break;
      case C_OscNodeDataPoolContent::eFLOAT32:
         ors64_Output = static_cast<int64_t>(orc_Content.GetValueF32());
         break;
      case C_OscNodeDataPoolContent::eFLOAT64:
         ors64_Output = static_cast<int64_t>(orc_Content.GetValueF64());
         break;
      default:
         break;
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Sets a sint64 value in the content class

   The os64_Value value will be casted to the current type of the orc_Content class.
   If the orc_Content holds an array, the value will be set to the position ou32_ArrayIndex.

   Hint: for integer datatypes an offset for rounding is added, so that the decimal place is
         not lost by casting.

   \param[in]      os64_Value       Value to set and cast
   \param[in,out]  orc_Content      Content class with configured type and array configuration
   \param[in]      ou32_ArrayIndex  Optional index for array position if content class has an array
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDpContentUtil::h_SetValueInContentS64(const int64_t os64_Value, C_OscNodeDataPoolContent & orc_Content,
                                                  const uint32_t ou32_ArrayIndex)
{
   bool q_ValueAssigned = false;

   //Handle maximum values for integer type
   switch (orc_Content.GetType())
   {
   case C_OscNodeDataPoolContent::eUINT8:
      if (os64_Value <= static_cast<int64_t>(std::numeric_limits<uint8_t>::lowest()))
      {
         if (orc_Content.GetArray() == true)
         {
            orc_Content.SetValueArrU8Element(std::numeric_limits<uint8_t>::lowest(), ou32_ArrayIndex);
         }
         else
         {
            orc_Content.SetValueU8(std::numeric_limits<uint8_t>::lowest());
         }
         q_ValueAssigned = true;
      }
      if (os64_Value >= static_cast<int64_t>(std::numeric_limits<uint8_t>::max()))
      {
         if (orc_Content.GetArray() == true)
         {
            orc_Content.SetValueArrU8Element(std::numeric_limits<uint8_t>::max(), ou32_ArrayIndex);
         }
         else
         {
            orc_Content.SetValueU8(std::numeric_limits<uint8_t>::max());
         }
         q_ValueAssigned = true;
      }
      break;
   case C_OscNodeDataPoolContent::eUINT16:
      if (os64_Value <= static_cast<int64_t>(std::numeric_limits<uint16_t>::lowest()))
      {
         if (orc_Content.GetArray() == true)
         {
            orc_Content.SetValueArrU16Element(std::numeric_limits<uint16_t>::lowest(), ou32_ArrayIndex);
         }
         else
         {
            orc_Content.SetValueU16(std::numeric_limits<uint16_t>::lowest());
         }
         q_ValueAssigned = true;
      }
      if (os64_Value >= static_cast<int64_t>(std::numeric_limits<uint16_t>::max()))
      {
         if (orc_Content.GetArray() == true)
         {
            orc_Content.SetValueArrU16Element(std::numeric_limits<uint16_t>::max(), ou32_ArrayIndex);
         }
         else
         {
            orc_Content.SetValueU16(std::numeric_limits<uint16_t>::max());
         }
         q_ValueAssigned = true;
      }
      break;
   case C_OscNodeDataPoolContent::eUINT32:
      if (os64_Value <= static_cast<int64_t>(std::numeric_limits<uint32_t>::lowest()))
      {
         if (orc_Content.GetArray() == true)
         {
            orc_Content.SetValueArrU32Element(std::numeric_limits<uint32_t>::lowest(), ou32_ArrayIndex);
         }
         else
         {
            orc_Content.SetValueU32(std::numeric_limits<uint32_t>::lowest());
         }
         q_ValueAssigned = true;
      }
      if (os64_Value >= static_cast<int64_t>(std::numeric_limits<uint32_t>::max()))
      {
         if (orc_Content.GetArray() == true)
         {
            orc_Content.SetValueArrU32Element(std::numeric_limits<uint32_t>::max(), ou32_ArrayIndex);
         }
         else
         {
            orc_Content.SetValueU32(std::numeric_limits<uint32_t>::max());
         }
         q_ValueAssigned = true;
      }
      break;
   case C_OscNodeDataPoolContent::eUINT64:
      if (os64_Value <= static_cast<int64_t>(std::numeric_limits<uint64_t>::lowest()))
      {
         if (orc_Content.GetArray() == true)
         {
            orc_Content.SetValueArrU64Element(std::numeric_limits<uint64_t>::lowest(), ou32_ArrayIndex);
         }
         else
         {
            orc_Content.SetValueU64(std::numeric_limits<uint64_t>::lowest());
         }
         q_ValueAssigned = true;
      }
      //Value cannot be over uint64 max
      break;
   case C_OscNodeDataPoolContent::eSINT8:
      if (os64_Value <= static_cast<int64_t>(std::numeric_limits<int8_t>::lowest()))
      {
         if (orc_Content.GetArray() == true)
         {
            orc_Content.SetValueArrS8Element(std::numeric_limits<int8_t>::lowest(), ou32_ArrayIndex);
         }
         else
         {
            orc_Content.SetValueS8(std::numeric_limits<int8_t>::lowest());
         }
         q_ValueAssigned = true;
      }
      if (os64_Value >= static_cast<int64_t>(std::numeric_limits<int8_t>::max()))
      {
         if (orc_Content.GetArray() == true)
         {
            orc_Content.SetValueArrS8Element(std::numeric_limits<int8_t>::max(), ou32_ArrayIndex);
         }
         else
         {
            orc_Content.SetValueS8(std::numeric_limits<int8_t>::max());
         }
         q_ValueAssigned = true;
      }
      break;
   case C_OscNodeDataPoolContent::eSINT16:
      if (os64_Value <= static_cast<int64_t>(std::numeric_limits<int16_t>::lowest()))
      {
         if (orc_Content.GetArray() == true)
         {
            orc_Content.SetValueArrS16Element(std::numeric_limits<int16_t>::lowest(), ou32_ArrayIndex);
         }
         else
         {
            orc_Content.SetValueS16(std::numeric_limits<int16_t>::lowest());
         }
         q_ValueAssigned = true;
      }
      if (os64_Value >= static_cast<int64_t>(std::numeric_limits<int16_t>::max()))
      {
         if (orc_Content.GetArray() == true)
         {
            orc_Content.SetValueArrS16Element(std::numeric_limits<int16_t>::max(), ou32_ArrayIndex);
         }
         else
         {
            orc_Content.SetValueS16(std::numeric_limits<int16_t>::max());
         }
         q_ValueAssigned = true;
      }
      break;
   case C_OscNodeDataPoolContent::eSINT32:
      if (os64_Value <= static_cast<int64_t>(std::numeric_limits<int32_t>::lowest()))
      {
         if (orc_Content.GetArray() == true)
         {
            orc_Content.SetValueArrS32Element(std::numeric_limits<int32_t>::lowest(), ou32_ArrayIndex);
         }
         else
         {
            orc_Content.SetValueS32(std::numeric_limits<int32_t>::lowest());
         }
         q_ValueAssigned = true;
      }
      if (os64_Value >= static_cast<int64_t>(std::numeric_limits<int32_t>::max()))
      {
         if (orc_Content.GetArray() == true)
         {
            orc_Content.SetValueArrS32Element(std::numeric_limits<int32_t>::max(), ou32_ArrayIndex);
         }
         else
         {
            orc_Content.SetValueS32(std::numeric_limits<int32_t>::max());
         }
         q_ValueAssigned = true;
      }
      break;
   case C_OscNodeDataPoolContent::eSINT64:
      //Value cannot be out of range for sint64
      break;
   case C_OscNodeDataPoolContent::eFLOAT32:
   case C_OscNodeDataPoolContent::eFLOAT64:
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
         case C_OscNodeDataPoolContent::eUINT8:
            orc_Content.SetValueArrU8Element(static_cast<uint8_t>(os64_Value), ou32_ArrayIndex);
            break;
         case C_OscNodeDataPoolContent::eUINT16:
            orc_Content.SetValueArrU16Element(static_cast<uint16_t>(os64_Value), ou32_ArrayIndex);
            break;
         case C_OscNodeDataPoolContent::eUINT32:
            orc_Content.SetValueArrU32Element(static_cast<uint32_t>(os64_Value), ou32_ArrayIndex);
            break;
         case C_OscNodeDataPoolContent::eUINT64:
            orc_Content.SetValueArrU64Element(static_cast<uint64_t>(os64_Value), ou32_ArrayIndex);
            break;
         case C_OscNodeDataPoolContent::eSINT8:
            orc_Content.SetValueArrS8Element(static_cast<int8_t>(os64_Value), ou32_ArrayIndex);
            break;
         case C_OscNodeDataPoolContent::eSINT16:
            orc_Content.SetValueArrS16Element(static_cast<int16_t>(os64_Value), ou32_ArrayIndex);
            break;
         case C_OscNodeDataPoolContent::eSINT32:
            orc_Content.SetValueArrS32Element(static_cast<int32_t>(os64_Value), ou32_ArrayIndex);
            break;
         case C_OscNodeDataPoolContent::eSINT64:
            orc_Content.SetValueArrS64Element(os64_Value, ou32_ArrayIndex);
            break;
         case C_OscNodeDataPoolContent::eFLOAT32:
            orc_Content.SetValueArrF32Element(static_cast<float32_t>(os64_Value), ou32_ArrayIndex);
            break;
         case C_OscNodeDataPoolContent::eFLOAT64:
            orc_Content.SetValueArrF64Element(static_cast<float64_t>(os64_Value), ou32_ArrayIndex);
            break;
         default:
            break;
         }
      }
      else
      {
         switch (orc_Content.GetType())
         {
         case C_OscNodeDataPoolContent::eUINT8:
            orc_Content.SetValueU8(static_cast<uint8_t>(os64_Value));
            break;
         case C_OscNodeDataPoolContent::eUINT16:
            orc_Content.SetValueU16(static_cast<uint16_t>(os64_Value));
            break;
         case C_OscNodeDataPoolContent::eUINT32:
            orc_Content.SetValueU32(static_cast<uint32_t>(os64_Value));
            break;
         case C_OscNodeDataPoolContent::eUINT64:
            orc_Content.SetValueU64(static_cast<uint64_t>(os64_Value));
            break;
         case C_OscNodeDataPoolContent::eSINT8:
            orc_Content.SetValueS8(static_cast<int8_t>(os64_Value));
            break;
         case C_OscNodeDataPoolContent::eSINT16:
            orc_Content.SetValueS16(static_cast<int16_t>(os64_Value));
            break;
         case C_OscNodeDataPoolContent::eSINT32:
            orc_Content.SetValueS32(static_cast<int32_t>(os64_Value));
            break;
         case C_OscNodeDataPoolContent::eSINT64:
            orc_Content.SetValueS64(os64_Value);
            break;
         case C_OscNodeDataPoolContent::eFLOAT32:
            orc_Content.SetValueF32(static_cast<float32_t>(os64_Value));
            break;
         case C_OscNodeDataPoolContent::eFLOAT64:
            orc_Content.SetValueF64(static_cast<float64_t>(os64_Value));
            break;
         default:
            break;
         }
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Sets a scaled float value in the content class

   The value will be casted to the current type of the orc_Content class.
   If the orc_Content holds an array, the value will be set to the position ou32_ArrayIndex.

   Hint: for integer datatypes an offset for rounding is added, so that the decimal place is
         not lost by casting.

   \param[in]      of64_ScaledValue    Scaled value to set and cast
   \param[in,out]  orc_Content         Content class with configured type and array configuration
   \param[in]      of64_Factor         Factor
   \param[in]      of64_Offset         Offset
   \param[in]      ou32_ArrayIndex     Optional index for array position if content class has an array
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDpContentUtil::h_SetScaledValueInContent(const float64_t of64_ScaledValue,
                                                     C_OscNodeDataPoolContent & orc_Content,
                                                     const float64_t of64_Factor, const float64_t of64_Offset,
                                                     const uint32_t ou32_ArrayIndex)
{
   // Unscale the value
   const float64_t f64_Value = C_OscUtils::h_GetValueUnscaled(of64_ScaledValue,
                                                              of64_Factor,
                                                              of64_Offset);

   // Update the value
   C_OscNodeDataPoolContentUtil::h_SetValueInContent(f64_Value, orc_Content, ou32_ArrayIndex);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get number of steps for the specified element ID

   \param[in]   orc_Min       Unscaled minimum value
   \param[in]   orc_Max       Unscaled maximum value
   \param[out]  oru64_Steps   Number of steps, if type supported (sint8, uint8, sint16, uint16, sint32,
                              uint32, sint64, uint64)
   \param[in]   ou32_Index    Index

   \return
   C_NO_ERR Operation success
   C_RANGE  Operation failure: parameter invalid or type not supported
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_SdNdeDpContentUtil::h_GetNumberOfAvailableSteps(const C_OscNodeDataPoolContent & orc_Min,
                                                          const C_OscNodeDataPoolContent & orc_Max,
                                                          uint64_t & oru64_Steps, const uint32_t ou32_Index)
{
   int32_t s32_Retval = C_NO_ERR;

   //Default
   oru64_Steps = 0;
   if ((orc_Min.GetType() == orc_Max.GetType()) && (orc_Min.GetArray() == orc_Max.GetArray()))
   {
      int64_t s64_Min;
      int64_t s64_Max;
      uint64_t u64_Min;
      uint64_t u64_Max;
      switch (orc_Min.GetType())
      {
      case C_OscNodeDataPoolContent::eUINT8:
         if (orc_Min.GetArray() == false)
         {
            u64_Max = static_cast<uint64_t>(orc_Max.GetValueU8());
            u64_Min = static_cast<uint64_t>(orc_Min.GetValueU8());
         }
         else
         {
            u64_Max = static_cast<uint64_t>(orc_Max.GetValueArrU8Element(ou32_Index));
            u64_Min = static_cast<uint64_t>(orc_Min.GetValueArrU8Element(ou32_Index));
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
      case C_OscNodeDataPoolContent::eSINT8:
         if (orc_Min.GetArray() == false)
         {
            s64_Max = static_cast<int64_t>(orc_Max.GetValueS8());
            s64_Min = static_cast<int64_t>(orc_Min.GetValueS8());
         }
         else
         {
            s64_Max = static_cast<int64_t>(orc_Max.GetValueArrS8Element(ou32_Index));
            s64_Min = static_cast<int64_t>(orc_Min.GetValueArrS8Element(ou32_Index));
         }
         if (s64_Max >= s64_Min)
         {
            //Overflow may happen (for sint64) but cast will lead to correct result
            oru64_Steps = static_cast<uint64_t>(s64_Max - s64_Min);
         }
         else
         {
            s32_Retval = C_RANGE;
         }
         break;
      case C_OscNodeDataPoolContent::eUINT16:
         if (orc_Min.GetArray() == false)
         {
            u64_Max = static_cast<uint64_t>(orc_Max.GetValueU16());
            u64_Min = static_cast<uint64_t>(orc_Min.GetValueU16());
         }
         else
         {
            u64_Max = static_cast<uint64_t>(orc_Max.GetValueArrU16Element(ou32_Index));
            u64_Min = static_cast<uint64_t>(orc_Min.GetValueArrU16Element(ou32_Index));
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
      case C_OscNodeDataPoolContent::eSINT16:
         if (orc_Min.GetArray() == false)
         {
            s64_Max = static_cast<int64_t>(orc_Max.GetValueS16());
            s64_Min = static_cast<int64_t>(orc_Min.GetValueS16());
         }
         else
         {
            s64_Max = static_cast<int64_t>(orc_Max.GetValueArrS16Element(ou32_Index));
            s64_Min = static_cast<int64_t>(orc_Min.GetValueArrS16Element(ou32_Index));
         }
         if (s64_Max >= s64_Min)
         {
            //Overflow may happen (for sint64) but cast will lead to correct result
            oru64_Steps = static_cast<uint64_t>(s64_Max - s64_Min);
         }
         else
         {
            s32_Retval = C_RANGE;
         }
         break;
      case C_OscNodeDataPoolContent::eUINT32:
         if (orc_Min.GetArray() == false)
         {
            u64_Max = static_cast<uint64_t>(orc_Max.GetValueU32());
            u64_Min = static_cast<uint64_t>(orc_Min.GetValueU32());
         }
         else
         {
            u64_Max = static_cast<uint64_t>(orc_Max.GetValueArrU32Element(ou32_Index));
            u64_Min = static_cast<uint64_t>(orc_Min.GetValueArrU32Element(ou32_Index));
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
      case C_OscNodeDataPoolContent::eSINT32:
         if (orc_Min.GetArray() == false)
         {
            s64_Max = static_cast<int64_t>(orc_Max.GetValueS32());
            s64_Min = static_cast<int64_t>(orc_Min.GetValueS32());
         }
         else
         {
            s64_Max = static_cast<int64_t>(orc_Max.GetValueArrS32Element(ou32_Index));
            s64_Min = static_cast<int64_t>(orc_Min.GetValueArrS32Element(ou32_Index));
         }
         if (s64_Max >= s64_Min)
         {
            //Overflow may happen (for sint64) but cast will lead to correct result
            oru64_Steps = static_cast<uint64_t>(s64_Max - s64_Min);
         }
         else
         {
            s32_Retval = C_RANGE;
         }
         break;
      case C_OscNodeDataPoolContent::eUINT64:
         if (orc_Min.GetArray() == false)
         {
            u64_Max = orc_Max.GetValueU64();
            u64_Min = orc_Min.GetValueU64();
         }
         else
         {
            u64_Max = orc_Max.GetValueArrU64Element(ou32_Index);
            u64_Min = orc_Min.GetValueArrU64Element(ou32_Index);
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
      case C_OscNodeDataPoolContent::eSINT64:
         if (orc_Min.GetArray() == false)
         {
            s64_Max = orc_Max.GetValueS64();
            s64_Min = orc_Min.GetValueS64();
         }
         else
         {
            s64_Max = orc_Max.GetValueArrS64Element(ou32_Index);
            s64_Min = orc_Min.GetValueArrS64Element(ou32_Index);
         }
         if (s64_Max >= s64_Min)
         {
            //Overflow may happen (for sint64) but cast will lead to correct result
            oru64_Steps = static_cast<uint64_t>(s64_Max - s64_Min);
         }
         else
         {
            s32_Retval = C_RANGE;
         }
         break;
      case C_OscNodeDataPoolContent::eFLOAT32:
      case C_OscNodeDataPoolContent::eFLOAT64:
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

   \param[in]   orc_Min       Unscaled minimum
   \param[in]   orc_Max       Unscaled maximum
   \param[in]   of64_Factor   Factor
   \param[in]   of64_Offset   Offset
   \param[out]  orc_NewMin    New minimum value
   \param[out]  orc_NewMax    New maximum value
   \param[in]   ou32_Index    Index

   \return
   C_NO_ERR Operation success
   C_RANGE  Operation failure: parameter invalid
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_SdNdeDpContentUtil::h_GetMinMaxAfterScaling(const C_OscNodeDataPoolContent & orc_Min,
                                                      const C_OscNodeDataPoolContent & orc_Max,
                                                      const float64_t of64_Factor, const float64_t of64_Offset,
                                                      C_OscNodeDataPoolContent & orc_NewMin,
                                                      C_OscNodeDataPoolContent & orc_NewMax, const uint32_t ou32_Index)
{
   int32_t s32_Retval = C_NO_ERR;

   if ((orc_Min.GetType() == orc_Max.GetType()) && (orc_Min.GetArray() == orc_Max.GetArray()))
   {
      uint64_t u64_Steps;
      float64_t f64_Min;
      float64_t f64_Max;
      if (orc_Min.GetArray() == false)
      {
         //Scale min & max
         tgl_assert(C_SdNdeDpContentUtil::h_GetValueAsFloat64(orc_Min, f64_Min, ou32_Index) == C_NO_ERR);
         tgl_assert(C_SdNdeDpContentUtil::h_GetValueAsFloat64(orc_Max, f64_Max, ou32_Index) == C_NO_ERR);
         f64_Min = C_OscUtils::h_GetValueScaled(f64_Min, of64_Factor, of64_Offset);
         f64_Max = C_OscUtils::h_GetValueScaled(f64_Max, of64_Factor, of64_Offset);
         if (C_SdNdeDpContentUtil::h_GetNumberOfAvailableSteps(orc_Min, orc_Max, u64_Steps, 0) == C_NO_ERR)
         {
            const C_OscNodeDataPoolContent::E_Type e_Type = C_SdNdeDpContentUtil::h_GetMinimumTypeForRange(
               f64_Min, f64_Max, u64_Steps);
            //Prepare output
            orc_NewMin.SetType(e_Type);
            orc_NewMax.SetType(e_Type);
            orc_NewMin.SetArray(false);
            orc_NewMax.SetArray(false);
            switch (e_Type)
            {
            case C_OscNodeDataPoolContent::eUINT8:
               //Set output
               orc_NewMin.SetValueU8(static_cast<uint8_t>(f64_Min));
               orc_NewMax.SetValueU8(static_cast<uint8_t>(f64_Max));
               break;
            case C_OscNodeDataPoolContent::eUINT16:
               //Set output
               orc_NewMin.SetValueU16(static_cast<uint16_t>(f64_Min));
               orc_NewMax.SetValueU16(static_cast<uint16_t>(f64_Max));
               break;
            case C_OscNodeDataPoolContent::eUINT32:
               //Set output
               orc_NewMin.SetValueU32(static_cast<uint32_t>(f64_Min));
               orc_NewMax.SetValueU32(static_cast<uint32_t>(f64_Max));
               break;
            case C_OscNodeDataPoolContent::eUINT64:
               //Set output
               orc_NewMin.SetValueU64(static_cast<uint64_t>(f64_Min));
               orc_NewMax.SetValueU64(static_cast<uint64_t>(f64_Max));
               break;
            case C_OscNodeDataPoolContent::eSINT8:
               //Set output
               orc_NewMin.SetValueS8(static_cast<int8_t>(f64_Min));
               orc_NewMax.SetValueS8(static_cast<int8_t>(f64_Max));
               break;
            case C_OscNodeDataPoolContent::eSINT16:
               //Set output
               orc_NewMin.SetValueS16(static_cast<int16_t>(f64_Min));
               orc_NewMax.SetValueS16(static_cast<int16_t>(f64_Max));
               break;
            case C_OscNodeDataPoolContent::eSINT32:
               //Set output
               orc_NewMin.SetValueS32(static_cast<int32_t>(f64_Min));
               orc_NewMax.SetValueS32(static_cast<int32_t>(f64_Max));
               break;
            case C_OscNodeDataPoolContent::eSINT64:
               //Set output
               orc_NewMin.SetValueS64(static_cast<int64_t>(f64_Min));
               orc_NewMax.SetValueS64(static_cast<int64_t>(f64_Max));
               break;
            case C_OscNodeDataPoolContent::eFLOAT32:
               //Set output
               orc_NewMin.SetValueF32(static_cast<float32_t>(f64_Min));
               orc_NewMax.SetValueF32(static_cast<float32_t>(f64_Max));
               break;
            case C_OscNodeDataPoolContent::eFLOAT64:
               //Set output
               orc_NewMin.SetValueF64(f64_Min);
               orc_NewMax.SetValueF64(f64_Max);
               break;
            default:
               break;
            }
         }
         else
         {
            //Prepare output
            orc_NewMin.SetType(C_OscNodeDataPoolContent::eFLOAT64);
            orc_NewMin.SetArray(false);
            orc_NewMax.SetType(C_OscNodeDataPoolContent::eFLOAT64);
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
            f64_Min = C_OscUtils::h_GetValueScaled(f64_Min, of64_Factor, of64_Offset);
            f64_Max = C_OscUtils::h_GetValueScaled(f64_Max, of64_Factor, of64_Offset);
            if (C_SdNdeDpContentUtil::h_GetNumberOfAvailableSteps(orc_Min, orc_Max, u64_Steps,
                                                                  ou32_Index) == C_NO_ERR)
            {
               const C_OscNodeDataPoolContent::E_Type e_Type = C_SdNdeDpContentUtil::h_GetMinimumTypeForRange(
                  f64_Min, f64_Max, u64_Steps);
               //Prepare output
               orc_NewMin.SetType(e_Type);
               orc_NewMax.SetType(e_Type);
               orc_NewMin.SetArray(false);
               orc_NewMax.SetArray(false);
               switch (e_Type)
               {
               case C_OscNodeDataPoolContent::eUINT8:
                  //Set output
                  orc_NewMin.SetValueU8(static_cast<uint8_t>(f64_Min));
                  orc_NewMax.SetValueU8(static_cast<uint8_t>(f64_Max));
                  break;
               case C_OscNodeDataPoolContent::eUINT16:
                  //Set output
                  orc_NewMin.SetValueU16(static_cast<uint16_t>(f64_Min));
                  orc_NewMax.SetValueU16(static_cast<uint16_t>(f64_Max));
                  break;
               case C_OscNodeDataPoolContent::eUINT32:
                  //Set output
                  orc_NewMin.SetValueU32(static_cast<uint32_t>(f64_Min));
                  orc_NewMax.SetValueU32(static_cast<uint32_t>(f64_Max));
                  break;
               case C_OscNodeDataPoolContent::eUINT64:
                  //Set output
                  orc_NewMin.SetValueU64(static_cast<uint64_t>(f64_Min));
                  orc_NewMax.SetValueU64(static_cast<uint64_t>(f64_Max));
                  break;
               case C_OscNodeDataPoolContent::eSINT8:
                  //Set output
                  orc_NewMin.SetValueS8(static_cast<int8_t>(f64_Min));
                  orc_NewMax.SetValueS8(static_cast<int8_t>(f64_Max));
                  break;
               case C_OscNodeDataPoolContent::eSINT16:
                  //Set output
                  orc_NewMin.SetValueS16(static_cast<int16_t>(f64_Min));
                  orc_NewMax.SetValueS16(static_cast<int16_t>(f64_Max));
                  break;
               case C_OscNodeDataPoolContent::eSINT32:
                  //Set output
                  orc_NewMin.SetValueS32(static_cast<int32_t>(f64_Min));
                  orc_NewMax.SetValueS32(static_cast<int32_t>(f64_Max));
                  break;
               case C_OscNodeDataPoolContent::eSINT64:
                  //Set output
                  orc_NewMin.SetValueS64(static_cast<int64_t>(f64_Min));
                  orc_NewMax.SetValueS64(static_cast<int64_t>(f64_Max));
                  break;
               case C_OscNodeDataPoolContent::eFLOAT32:
                  //Set output
                  orc_NewMin.SetValueF32(static_cast<float32_t>(f64_Min));
                  orc_NewMax.SetValueF32(static_cast<float32_t>(f64_Max));
                  break;
               case C_OscNodeDataPoolContent::eFLOAT64:
                  //Set output
                  orc_NewMin.SetValueF64(f64_Min);
                  orc_NewMax.SetValueF64(f64_Max);
                  break;
               default:
                  break;
               }
            }
            else
            {
               //Prepare output
               orc_NewMin.SetType(C_OscNodeDataPoolContent::eFLOAT64);
               orc_NewMin.SetArray(false);
               orc_NewMax.SetType(C_OscNodeDataPoolContent::eFLOAT64);
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

   \param[in]   orc_Min       Unscaled minimum
   \param[in]   orc_Max       Unscaled maximum
   \param[in]   of64_Factor   Factor
   \param[in]   of64_Offset   Offset
   \param[out]  orc_Types     Minimum types after scaling

   \return
   C_NO_ERR Operation success
   C_RANGE  Operation failure: parameter invalid
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_SdNdeDpContentUtil::h_GetMinimalTypeAfterScaling(const C_OscNodeDataPoolContent & orc_Min,
                                                           const C_OscNodeDataPoolContent & orc_Max,
                                                           const float64_t of64_Factor, const float64_t of64_Offset,
                                                           std::vector<C_OscNodeDataPoolContent::E_Type> & orc_Types)
{
   int32_t s32_Retval = C_NO_ERR;

   orc_Types.clear();
   if ((orc_Min.GetType() == orc_Max.GetType()) && (orc_Min.GetArray() == orc_Max.GetArray()))
   {
      uint64_t u64_Steps;
      float64_t f64_Min;
      float64_t f64_Max;
      if (orc_Min.GetArray() == false)
      {
         //Scale min & max
         tgl_assert(C_SdNdeDpContentUtil::h_GetValueAsFloat64(orc_Min, f64_Min, 0UL) == C_NO_ERR);
         tgl_assert(C_SdNdeDpContentUtil::h_GetValueAsFloat64(orc_Max, f64_Max, 0UL) == C_NO_ERR);
         f64_Min = C_OscUtils::h_GetValueScaled(f64_Min, of64_Factor, of64_Offset);
         f64_Max = C_OscUtils::h_GetValueScaled(f64_Max, of64_Factor, of64_Offset);
         if (C_SdNdeDpContentUtil::h_GetNumberOfAvailableSteps(orc_Min, orc_Max, u64_Steps, 0) == C_NO_ERR)
         {
            orc_Types.push_back(C_SdNdeDpContentUtil::h_GetMinimumTypeForRange(f64_Min, f64_Max, u64_Steps));
         }
         else
         {
            orc_Types.push_back(C_OscNodeDataPoolContent::eFLOAT64);
         }
      }
      else
      {
         std::vector<float64_t> c_Min;
         std::vector<float64_t> c_Max;
         //Scale min & max
         C_SdNdeDpContentUtil::h_GetValuesAsFloat64(orc_Min, c_Min);
         C_SdNdeDpContentUtil::h_GetValuesAsFloat64(orc_Max, c_Max);
         if (((c_Min.size() == c_Max.size()) && (c_Min.size() == orc_Min.GetArraySize())) &&
             (orc_Min.GetArraySize() == orc_Max.GetArraySize()))
         {
            for (uint32_t u32_ItArray = 0; u32_ItArray < orc_Max.GetArraySize(); ++u32_ItArray)
            {
               f64_Min = C_OscUtils::h_GetValueScaled(c_Min[u32_ItArray], of64_Factor, of64_Offset);
               f64_Max = C_OscUtils::h_GetValueScaled(c_Max[u32_ItArray], of64_Factor, of64_Offset);
               if (C_SdNdeDpContentUtil::h_GetNumberOfAvailableSteps(orc_Min, orc_Max, u64_Steps,
                                                                     u32_ItArray) == C_NO_ERR)
               {
                  orc_Types.push_back(C_SdNdeDpContentUtil::h_GetMinimumTypeForRange(f64_Min, f64_Max,
                                                                                     u64_Steps));
               }
               else
               {
                  orc_Types.push_back(C_OscNodeDataPoolContent::eFLOAT64);
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

   \param[in]  of64_Min    Unscaled minimum
   \param[in]  of64_Max    Unscaled maximum
   \param[in]  ou64_Steps  Current scaling

   \return
   C_NO_ERR Operation success
   C_RANGE  Operation failure: parameter invalid
*/
//----------------------------------------------------------------------------------------------------------------------
C_OscNodeDataPoolContent::E_Type C_SdNdeDpContentUtil::h_GetMinimumTypeForRange(const float64_t of64_Min,
                                                                                const float64_t of64_Max,
                                                                                const uint64_t ou64_Steps)
{
   C_OscNodeDataPoolContent::E_Type e_Retval;
   //There is no point for this function if there are no steps
   //If min is over the maximum it's just invalid so also no point
   //If either min or max is no integer then probably a double is required
   if (((of64_Min > of64_Max) ||
        (C_Uti::h_CheckFloatHasNoFractionPart(of64_Min) == false)) ||
       (C_Uti::h_CheckFloatHasNoFractionPart(of64_Max) == false))
   {
      e_Retval = C_OscNodeDataPoolContent::eFLOAT64;
   }
   else
   {
      //Check if step width is integer
      if ((ou64_Steps > 0ULL) &&
          (C_Uti::h_CheckFloatHasNoFractionPart((of64_Max - of64_Min) / static_cast<float64_t>(ou64_Steps)) == false))
      {
         e_Retval = C_OscNodeDataPoolContent::eFLOAT64;
      }
      else
      {
         if (of64_Min < 0.0)
         {
            //Signed integer
            if (((of64_Min >= static_cast<float64_t>(std::numeric_limits<int8_t>::lowest())) &&
                 (of64_Max <= static_cast<float64_t>(std::numeric_limits<int8_t>::max()))) &&
                (ou64_Steps <= static_cast<uint64_t>(std::numeric_limits<uint8_t>::max())))
            {
               e_Retval = C_OscNodeDataPoolContent::eSINT8;
            }
            else if (((of64_Min >= static_cast<float64_t>(std::numeric_limits<int16_t>::lowest())) &&
                      (of64_Max <= static_cast<float64_t>(std::numeric_limits<int16_t>::max()))) &&
                     (ou64_Steps <= static_cast<uint64_t>(std::numeric_limits<uint16_t>::max())))
            {
               e_Retval = C_OscNodeDataPoolContent::eSINT16;
            }
            else if (((of64_Min >= static_cast<float64_t>(std::numeric_limits<int32_t>::lowest())) &&
                      (of64_Max <= static_cast<float64_t>(std::numeric_limits<int32_t>::max()))) &&
                     (ou64_Steps <= static_cast<uint64_t>(std::numeric_limits<uint32_t>::max())))
            {
               e_Retval = C_OscNodeDataPoolContent::eSINT32;
            }
            else if ((of64_Min >= static_cast<float64_t>(std::numeric_limits<int64_t>::lowest())) &&
                     (of64_Max <= static_cast<float64_t>(std::numeric_limits<int64_t>::max())))
            {
               e_Retval = C_OscNodeDataPoolContent::eSINT64;
            }
            else
            {
               e_Retval = C_OscNodeDataPoolContent::eFLOAT64;
            }
         }
         else
         {
            //Unsigned integer
            if (((of64_Max <= static_cast<float64_t>(std::numeric_limits<uint8_t>::max()))) &&
                (ou64_Steps <= static_cast<uint64_t>(std::numeric_limits<uint8_t>::max())))
            {
               e_Retval = C_OscNodeDataPoolContent::eUINT8;
            }
            else if (((of64_Max <= static_cast<float64_t>(std::numeric_limits<uint16_t>::max()))) &&
                     (ou64_Steps <= static_cast<uint64_t>(std::numeric_limits<uint16_t>::max())))
            {
               e_Retval = C_OscNodeDataPoolContent::eUINT16;
            }
            else if (((of64_Max <= static_cast<float64_t>(std::numeric_limits<uint32_t>::max()))) &&
                     (ou64_Steps <= static_cast<uint64_t>(std::numeric_limits<uint32_t>::max())))
            {
               e_Retval = C_OscNodeDataPoolContent::eUINT32;
            }
            else if ((of64_Max <= static_cast<float64_t>(std::numeric_limits<uint64_t>::max())))
            {
               e_Retval = C_OscNodeDataPoolContent::eUINT64;
            }
            else
            {
               e_Retval = C_OscNodeDataPoolContent::eFLOAT64;
            }
         }
      }
   }
   return e_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Simple conversion from content to QVariant

   Simple means: array types not supported

   \param[in]   orc_Content   C_OscNodeDataPoolContent input
   \param[out]  orc_Variant   QVariant output

   \return
   C_NO_ERR Operation success
   C_RANGE  Operation failure: parameter invalid
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_SdNdeDpContentUtil::h_SimpleConvertToVariant(const C_OscNodeDataPoolContent & orc_Content,
                                                       QVariant & orc_Variant)
{
   int32_t s32_Retval = C_NO_ERR;

   if (orc_Content.GetArray() == false)
   {
      switch (orc_Content.GetType())
      {
      case C_OscNodeDataPoolContent::eSINT8:
         orc_Variant = static_cast<int32_t>(orc_Content.GetValueS8());
         break;
      case C_OscNodeDataPoolContent::eSINT16:
         orc_Variant = orc_Content.GetValueS16();
         break;
      case C_OscNodeDataPoolContent::eSINT32:
         orc_Variant = static_cast<int64_t>(orc_Content.GetValueS32());
         break;
      case C_OscNodeDataPoolContent::eSINT64:
         orc_Variant = orc_Content.GetValueS64();
         break;
      case C_OscNodeDataPoolContent::eUINT8:
         orc_Variant = static_cast<int32_t>(orc_Content.GetValueU8());
         break;
      case C_OscNodeDataPoolContent::eUINT16:
         orc_Variant = orc_Content.GetValueU16();
         break;
      case C_OscNodeDataPoolContent::eUINT32:
         orc_Variant = static_cast<uint64_t>(orc_Content.GetValueU32());
         break;
      case C_OscNodeDataPoolContent::eUINT64:
         orc_Variant = orc_Content.GetValueU64();
         break;
      case C_OscNodeDataPoolContent::eFLOAT32:
         orc_Variant = orc_Content.GetValueF32();
         break;
      case C_OscNodeDataPoolContent::eFLOAT64:
         orc_Variant = orc_Content.GetValueF64();
         break;
      default:
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

   \param[in]   orc_Variant   QVariant input
   \param[out]  orc_Content   C_OscNodeDataPoolContent output

   \return
   C_NO_ERR Operation success
   C_RANGE  Operation failure: parameter invalid
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_SdNdeDpContentUtil::h_SimpleConvertFromVariant(const QVariant & orc_Variant,
                                                         C_OscNodeDataPoolContent & orc_Content)
{
   bool q_Ok;
   int32_t s32_Retval = C_NO_ERR;

   switch (orc_Variant.type()) //lint !e788 not all types handled here
   {
   case QVariant::Int:
      orc_Content.SetType(C_OscNodeDataPoolContent::eSINT32);
      orc_Content.SetArray(false);
      orc_Content.SetValueS32(orc_Variant.toInt(&q_Ok));
      tgl_assert(q_Ok == true);
      break;
   case QVariant::UInt:
      orc_Content.SetType(C_OscNodeDataPoolContent::eUINT32);
      orc_Content.SetArray(false);
      orc_Content.SetValueU32(orc_Variant.toUInt(&q_Ok));
      tgl_assert(q_Ok == true);
      break;
   case QVariant::LongLong:
      orc_Content.SetType(C_OscNodeDataPoolContent::eSINT64);
      orc_Content.SetArray(false);
      orc_Content.SetValueS64(orc_Variant.toLongLong(&q_Ok));
      tgl_assert(q_Ok == true);
      break;
   case QVariant::ULongLong:
      orc_Content.SetType(C_OscNodeDataPoolContent::eUINT64);
      orc_Content.SetArray(false);
      orc_Content.SetValueU64(orc_Variant.toULongLong(&q_Ok));
      tgl_assert(q_Ok == true);
      break;
   case QVariant::Double:
      orc_Content.SetType(C_OscNodeDataPoolContent::eFLOAT64);
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

   \param[in]   orc_Content               The source content
   \param[in]   of64_Factor               Scaling factor
   \param[in]   of64_Offset               Scaling offset
   \param[out]  orc_Output                Scaled strings
   \param[in]   oq_AllowRangeAdaptation   Allow range adaptation
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDpContentUtil::h_GetValuesAsScaledString(const C_OscNodeDataPoolContent & orc_Content,
                                                     const float64_t of64_Factor, const float64_t of64_Offset,
                                                     std::vector<QString> & orc_Output,
                                                     const bool oq_AllowRangeAdaptation)
{
   if (orc_Content.GetArray() == false)
   {
      QString c_Tmp;
      C_SdNdeDpContentUtil::h_GetValueAsScaledString(orc_Content, of64_Factor, of64_Offset, c_Tmp, 0,
                                                     oq_AllowRangeAdaptation);
      orc_Output.push_back(c_Tmp);
   }
   else
   {
      orc_Output.reserve(orc_Content.GetArraySize());
      for (uint32_t u32_ItElement = 0; u32_ItElement < orc_Content.GetArraySize(); ++u32_ItElement)
      {
         QString c_Tmp;
         C_SdNdeDpContentUtil::h_GetValueAsScaledString(orc_Content, of64_Factor, of64_Offset, c_Tmp,
                                                        u32_ItElement, oq_AllowRangeAdaptation);
         orc_Output.push_back(c_Tmp);
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get all values as one scaled and combined string

   \param[in]   orc_Content               The source content
   \param[in]   of64_Factor               Scaling factor
   \param[in]   of64_Offset               Scaling offset
   \param[out]  orc_Output                Scaled and combined string
   \param[in]   oq_AllowRangeAdaptation   Allow range adaptation
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDpContentUtil::h_GetValuesAsScaledCombinedString(const C_OscNodeDataPoolContent & orc_Content,
                                                             const float64_t of64_Factor, const float64_t of64_Offset,
                                                             QString & orc_Output, const bool oq_AllowRangeAdaptation)
{
   std::vector<QString> c_Output;
   C_SdNdeDpContentUtil::h_GetValuesAsScaledString(orc_Content, of64_Factor, of64_Offset,
                                                   c_Output, oq_AllowRangeAdaptation);
   orc_Output = C_SdNdeDpContentUtil::h_CombineString(c_Output);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Combine value string

   \param[in]  orc_Input   Values

   \return
   Combined value string
*/
//----------------------------------------------------------------------------------------------------------------------
QString C_SdNdeDpContentUtil::h_CombineString(const std::vector<QString> & orc_Input)
{
   QString c_Retval = "";

   for (uint32_t u32_It = 0; u32_It < orc_Input.size(); ++u32_It)
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

   \param[in]   orc_Content               The source content
   \param[in]   of64_Factor               Scaling factor
   \param[in]   of64_Offset               Scaling offset
   \param[out]  orc_Output                Scaled string
   \param[in]   ou32_Index                Index to use in case of array
   \param[in]   oq_AllowRangeAdaptation   Allow range adaptation
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDpContentUtil::h_GetValueAsScaledString(const C_OscNodeDataPoolContent & orc_Content,
                                                    const float64_t of64_Factor, const float64_t of64_Offset,
                                                    QString & orc_Output, const uint32_t ou32_Index,
                                                    const bool oq_AllowRangeAdaptation)
{
   std::string c_Str;

   orc_Content.GetValueAsScaledString(of64_Factor, of64_Offset, c_Str, ou32_Index, oq_AllowRangeAdaptation);
   orc_Output = c_Str.c_str();
   //Replace separators
   orc_Output.replace(QLocale::c().decimalPoint(), QLocale::system().decimalPoint(), Qt::CaseInsensitive);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set content to maximum type value

   \param[in,out]  orc_Content   Content
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDpContentUtil::h_InitMax(C_OscNodeDataPoolContent & orc_Content)
{
   switch (orc_Content.GetType())
   {
   case C_OscNodeDataPoolContent::eSINT8:
      if (orc_Content.GetArray() == false)
      {
         orc_Content.SetValueS8(std::numeric_limits<int8_t>::max());
      }
      else
      {
         for (uint32_t u32_It = 0; u32_It < orc_Content.GetArraySize(); ++u32_It)
         {
            orc_Content.SetValueArrS8Element(std::numeric_limits<int8_t>::max(), u32_It);
         }
      }
      break;
   case C_OscNodeDataPoolContent::eSINT16:
      if (orc_Content.GetArray() == false)
      {
         orc_Content.SetValueS16(std::numeric_limits<int16_t>::max());
      }
      else
      {
         for (uint32_t u32_It = 0; u32_It < orc_Content.GetArraySize(); ++u32_It)
         {
            orc_Content.SetValueArrS16Element(std::numeric_limits<int16_t>::max(), u32_It);
         }
      }
      break;
   case C_OscNodeDataPoolContent::eSINT32:
      if (orc_Content.GetArray() == false)
      {
         orc_Content.SetValueS32(std::numeric_limits<int32_t>::max());
      }
      else
      {
         for (uint32_t u32_It = 0; u32_It < orc_Content.GetArraySize(); ++u32_It)
         {
            orc_Content.SetValueArrS32Element(std::numeric_limits<int32_t>::max(), u32_It);
         }
      }
      break;
   case C_OscNodeDataPoolContent::eSINT64:
      if (orc_Content.GetArray() == false)
      {
         orc_Content.SetValueS64(std::numeric_limits<int64_t>::max());
      }
      else
      {
         for (uint32_t u32_It = 0; u32_It < orc_Content.GetArraySize(); ++u32_It)
         {
            orc_Content.SetValueArrS64Element(std::numeric_limits<int64_t>::max(), u32_It);
         }
      }
      break;
   case C_OscNodeDataPoolContent::eUINT8:
      if (orc_Content.GetArray() == false)
      {
         orc_Content.SetValueU8(std::numeric_limits<uint8_t>::max());
      }
      else
      {
         for (uint32_t u32_It = 0; u32_It < orc_Content.GetArraySize(); ++u32_It)
         {
            orc_Content.SetValueArrU8Element(std::numeric_limits<uint8_t>::max(), u32_It);
         }
      }
      break;
   case C_OscNodeDataPoolContent::eUINT16:
      if (orc_Content.GetArray() == false)
      {
         orc_Content.SetValueU16(std::numeric_limits<uint16_t>::max());
      }
      else
      {
         for (uint32_t u32_It = 0; u32_It < orc_Content.GetArraySize(); ++u32_It)
         {
            orc_Content.SetValueArrU16Element(std::numeric_limits<uint16_t>::max(), u32_It);
         }
      }
      break;
   case C_OscNodeDataPoolContent::eUINT32:
      if (orc_Content.GetArray() == false)
      {
         orc_Content.SetValueU32(std::numeric_limits<uint32_t>::max());
      }
      else
      {
         for (uint32_t u32_It = 0; u32_It < orc_Content.GetArraySize(); ++u32_It)
         {
            orc_Content.SetValueArrU32Element(std::numeric_limits<uint32_t>::max(), u32_It);
         }
      }
      break;
   case C_OscNodeDataPoolContent::eUINT64:
      if (orc_Content.GetArray() == false)
      {
         orc_Content.SetValueU64(std::numeric_limits<uint64_t>::max());
      }
      else
      {
         for (uint32_t u32_It = 0; u32_It < orc_Content.GetArraySize(); ++u32_It)
         {
            orc_Content.SetValueArrU64Element(std::numeric_limits<uint64_t>::max(), u32_It);
         }
      }
      break;
   case C_OscNodeDataPoolContent::eFLOAT32:
      if (orc_Content.GetArray() == false)
      {
         orc_Content.SetValueF32(std::numeric_limits<float32_t>::max());
      }
      else
      {
         for (uint32_t u32_It = 0; u32_It < orc_Content.GetArraySize(); ++u32_It)
         {
            orc_Content.SetValueArrF32Element(std::numeric_limits<float32_t>::max(), u32_It);
         }
      }
      break;
   case C_OscNodeDataPoolContent::eFLOAT64:
      if (orc_Content.GetArray() == false)
      {
         orc_Content.SetValueF64(std::numeric_limits<float64_t>::max());
      }
      else
      {
         for (uint32_t u32_It = 0; u32_It < orc_Content.GetArraySize(); ++u32_It)
         {
            orc_Content.SetValueArrF64Element(std::numeric_limits<float64_t>::max(), u32_It);
         }
      }
      break;
   default:
      break;
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set content to minimum type value

   \param[in,out]  orc_Content   Content
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDpContentUtil::h_InitMin(C_OscNodeDataPoolContent & orc_Content)
{
   switch (orc_Content.GetType())
   {
   case C_OscNodeDataPoolContent::eSINT8:
      if (orc_Content.GetArray() == false)
      {
         orc_Content.SetValueS8(std::numeric_limits<int8_t>::lowest());
      }
      else
      {
         for (uint32_t u32_It = 0; u32_It < orc_Content.GetArraySize(); ++u32_It)
         {
            orc_Content.SetValueArrS8Element(std::numeric_limits<int8_t>::lowest(), u32_It);
         }
      }
      break;
   case C_OscNodeDataPoolContent::eSINT16:
      if (orc_Content.GetArray() == false)
      {
         orc_Content.SetValueS16(std::numeric_limits<int16_t>::lowest());
      }
      else
      {
         for (uint32_t u32_It = 0; u32_It < orc_Content.GetArraySize(); ++u32_It)
         {
            orc_Content.SetValueArrS16Element(std::numeric_limits<int16_t>::lowest(), u32_It);
         }
      }
      break;
   case C_OscNodeDataPoolContent::eSINT32:
      if (orc_Content.GetArray() == false)
      {
         orc_Content.SetValueS32(std::numeric_limits<int32_t>::lowest());
      }
      else
      {
         for (uint32_t u32_It = 0; u32_It < orc_Content.GetArraySize(); ++u32_It)
         {
            orc_Content.SetValueArrS32Element(std::numeric_limits<int32_t>::lowest(), u32_It);
         }
      }
      break;
   case C_OscNodeDataPoolContent::eSINT64:
      if (orc_Content.GetArray() == false)
      {
         orc_Content.SetValueS64(std::numeric_limits<int64_t>::lowest());
      }
      else
      {
         for (uint32_t u32_It = 0; u32_It < orc_Content.GetArraySize(); ++u32_It)
         {
            orc_Content.SetValueArrS64Element(std::numeric_limits<int64_t>::lowest(), u32_It);
         }
      }
      break;
   case C_OscNodeDataPoolContent::eUINT8:
      if (orc_Content.GetArray() == false)
      {
         orc_Content.SetValueU8(std::numeric_limits<uint8_t>::lowest());
      }
      else
      {
         for (uint32_t u32_It = 0; u32_It < orc_Content.GetArraySize(); ++u32_It)
         {
            orc_Content.SetValueArrU8Element(std::numeric_limits<uint8_t>::lowest(), u32_It);
         }
      }
      break;
   case C_OscNodeDataPoolContent::eUINT16:
      if (orc_Content.GetArray() == false)
      {
         orc_Content.SetValueU16(std::numeric_limits<uint16_t>::lowest());
      }
      else
      {
         for (uint32_t u32_It = 0; u32_It < orc_Content.GetArraySize(); ++u32_It)
         {
            orc_Content.SetValueArrU16Element(std::numeric_limits<uint16_t>::lowest(), u32_It);
         }
      }
      break;
   case C_OscNodeDataPoolContent::eUINT32:
      if (orc_Content.GetArray() == false)
      {
         orc_Content.SetValueU32(std::numeric_limits<uint32_t>::lowest());
      }
      else
      {
         for (uint32_t u32_It = 0; u32_It < orc_Content.GetArraySize(); ++u32_It)
         {
            orc_Content.SetValueArrU32Element(std::numeric_limits<uint32_t>::lowest(), u32_It);
         }
      }
      break;
   case C_OscNodeDataPoolContent::eUINT64:
      if (orc_Content.GetArray() == false)
      {
         orc_Content.SetValueU64(std::numeric_limits<uint64_t>::lowest());
      }
      else
      {
         for (uint32_t u32_It = 0; u32_It < orc_Content.GetArraySize(); ++u32_It)
         {
            orc_Content.SetValueArrU64Element(std::numeric_limits<uint64_t>::lowest(), u32_It);
         }
      }
      break;
   case C_OscNodeDataPoolContent::eFLOAT32:
      if (orc_Content.GetArray() == false)
      {
         orc_Content.SetValueF32(std::numeric_limits<float32_t>::lowest());
      }
      else
      {
         for (uint32_t u32_It = 0; u32_It < orc_Content.GetArraySize(); ++u32_It)
         {
            orc_Content.SetValueArrF32Element(std::numeric_limits<float32_t>::lowest(), u32_It);
         }
      }
      break;
   case C_OscNodeDataPoolContent::eFLOAT64:
      if (orc_Content.GetArray() == false)
      {
         orc_Content.SetValueF64(std::numeric_limits<float64_t>::lowest());
      }
      else
      {
         for (uint32_t u32_It = 0; u32_It < orc_Content.GetArraySize(); ++u32_It)
         {
            orc_Content.SetValueArrF64Element(std::numeric_limits<float64_t>::lowest(), u32_It);
         }
      }
      break;
   default:
      break;
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Initialize maximum depending on bit length (utility for CAN signals)

   \param[in,out]  orc_Content      Content
   \param[in]      ou16_BitLength   Signal bit length
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDpContentUtil::h_InitMaxForSignal(C_OscNodeDataPoolContent & orc_Content, const uint16_t ou16_BitLength)
{
   bool q_IsUnsigned = false;
   bool q_IsSigned = false;
   bool q_IsFloat = false;
   bool q_BitLengthInvalid = false;

   //Check if type is realistic for number of bits
   switch (orc_Content.GetType())
   {
   case C_OscNodeDataPoolContent::eUINT8:
      if (ou16_BitLength > 8)
      {
         q_BitLengthInvalid = true;
      }
      q_IsUnsigned = true;
      break;
   case C_OscNodeDataPoolContent::eUINT16:
      if (ou16_BitLength > 16)
      {
         q_BitLengthInvalid = true;
      }
      q_IsUnsigned = true;
      break;
   case C_OscNodeDataPoolContent::eUINT32:
      if (ou16_BitLength > 32)
      {
         q_BitLengthInvalid = true;
      }
      q_IsUnsigned = true;
      break;
   case C_OscNodeDataPoolContent::eUINT64:
      if (ou16_BitLength > 64)
      {
         q_BitLengthInvalid = true;
      }
      q_IsUnsigned = true;
      break;
   case C_OscNodeDataPoolContent::eSINT8:
      if (ou16_BitLength > 8)
      {
         q_BitLengthInvalid = true;
      }
      q_IsSigned = true;
      break;
   case C_OscNodeDataPoolContent::eSINT16:
      if (ou16_BitLength > 16)
      {
         q_BitLengthInvalid = true;
      }
      q_IsSigned = true;
      break;
   case C_OscNodeDataPoolContent::eSINT32:
      if (ou16_BitLength > 32)
      {
         q_BitLengthInvalid = true;
      }
      q_IsSigned = true;
      break;
   case C_OscNodeDataPoolContent::eSINT64:
      if (ou16_BitLength > 64)
      {
         q_BitLengthInvalid = true;
      }
      q_IsSigned = true;
      break;
   case C_OscNodeDataPoolContent::eFLOAT32:
      if (ou16_BitLength > 32)
      {
         q_BitLengthInvalid = true;
      }
      q_IsFloat = true;
      break;
   case C_OscNodeDataPoolContent::eFLOAT64:
      if (ou16_BitLength > 64)
      {
         q_BitLengthInvalid = true;
      }
      q_IsFloat = true;
      break;
   default:
      q_BitLengthInvalid = true;
      break;
   }
   if (q_BitLengthInvalid == false)
   {
      //Unsigned
      if (q_IsUnsigned == true)
      {
         uint64_t u64_Max = 0;

         for (uint16_t u16_ItBit = 0; u16_ItBit < ou16_BitLength; ++u16_ItBit)
         {
            u64_Max += static_cast<uint64_t>(1ULL) << static_cast<uint64_t>(u16_ItBit);
         }

         switch (orc_Content.GetType()) //lint !e788 //only unsigned types handled explicitly
         {
         case C_OscNodeDataPoolContent::eUINT8:
            orc_Content.SetValueU8(static_cast<uint8_t>(u64_Max));
            break;
         case C_OscNodeDataPoolContent::eUINT16:
            orc_Content.SetValueU16(static_cast<uint16_t>(u64_Max));
            break;
         case C_OscNodeDataPoolContent::eUINT32:
            orc_Content.SetValueU32(static_cast<uint32_t>(u64_Max));
            break;
         case C_OscNodeDataPoolContent::eUINT64:
            orc_Content.SetValueU64(u64_Max);
            break;
         default:
            tgl_assert(false);
            break;
         }
      }
      //Signed
      if (q_IsSigned == true)
      {
         uint64_t u64_Max = 0;

         //We need exactly one more than half of the unsigned maximum
         if (ou16_BitLength > 0U)
         {
            u64_Max += static_cast<uint64_t>(1ULL) << (static_cast<uint64_t>(ou16_BitLength) - 1ULL);
         }

         switch (orc_Content.GetType()) //lint !e788 //only signed types handled explicitly
         {
         case C_OscNodeDataPoolContent::eSINT8:
            orc_Content.SetValueS8(static_cast<int8_t>(u64_Max - 1U));
            break;
         case C_OscNodeDataPoolContent::eSINT16:
            orc_Content.SetValueS16(static_cast<int16_t>(u64_Max - 1U));
            break;
         case C_OscNodeDataPoolContent::eSINT32:
            orc_Content.SetValueS32(static_cast<int32_t>(u64_Max - 1UL));
            break;
         case C_OscNodeDataPoolContent::eSINT64:
            orc_Content.SetValueS64(static_cast<int64_t>(u64_Max - 1ULL));
            break;
         default:
            tgl_assert(false);
            break;
         }
      }
      //Float
      if (q_IsFloat == true)
      {
         switch (orc_Content.GetType()) //lint !e788 //only float types handled explicitly
         {
         case C_OscNodeDataPoolContent::eFLOAT32:
            orc_Content.SetValueF32(std::numeric_limits<float32_t>::max());
            break;
         case C_OscNodeDataPoolContent::eFLOAT64:
            orc_Content.SetValueF64(std::numeric_limits<float64_t>::max());
            break;
         default:
            tgl_assert(false);
            break;
         }
      }
   }
   else
   {
      h_InitMax(orc_Content);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Initialize minimum depending on bit length (utility for CAN signals)

   \param[in,out]  orc_Content      Content
   \param[in]      ou16_BitLength   Signal bit length
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDpContentUtil::h_InitMinForSignal(C_OscNodeDataPoolContent & orc_Content, const uint16_t ou16_BitLength)
{
   bool q_IsUnsigned = false;
   bool q_IsSigned = false;
   bool q_IsFloat = false;
   bool q_BitLengthInvalid = false;

   //Check if type is realistic for number of bits
   switch (orc_Content.GetType())
   {
   case C_OscNodeDataPoolContent::eUINT8:
      if (ou16_BitLength > 8)
      {
         q_BitLengthInvalid = true;
      }
      q_IsUnsigned = true;
      break;
   case C_OscNodeDataPoolContent::eUINT16:
      if (ou16_BitLength > 16)
      {
         q_BitLengthInvalid = true;
      }
      q_IsUnsigned = true;
      break;
   case C_OscNodeDataPoolContent::eUINT32:
      if (ou16_BitLength > 32)
      {
         q_BitLengthInvalid = true;
      }
      q_IsUnsigned = true;
      break;
   case C_OscNodeDataPoolContent::eUINT64:
      if (ou16_BitLength > 64)
      {
         q_BitLengthInvalid = true;
      }
      q_IsUnsigned = true;
      break;
   case C_OscNodeDataPoolContent::eSINT8:
      if (ou16_BitLength > 8)
      {
         q_BitLengthInvalid = true;
      }
      q_IsSigned = true;
      break;
   case C_OscNodeDataPoolContent::eSINT16:
      if (ou16_BitLength > 16)
      {
         q_BitLengthInvalid = true;
      }
      q_IsSigned = true;
      break;
   case C_OscNodeDataPoolContent::eSINT32:
      if (ou16_BitLength > 32)
      {
         q_BitLengthInvalid = true;
      }
      q_IsSigned = true;
      break;
   case C_OscNodeDataPoolContent::eSINT64:
      if (ou16_BitLength > 64)
      {
         q_BitLengthInvalid = true;
      }
      q_IsSigned = true;
      break;
   case C_OscNodeDataPoolContent::eFLOAT32:
      if (ou16_BitLength > 32)
      {
         q_BitLengthInvalid = true;
      }
      q_IsFloat = true;
      break;
   case C_OscNodeDataPoolContent::eFLOAT64:
      if (ou16_BitLength > 64)
      {
         q_BitLengthInvalid = true;
      }
      q_IsFloat = true;
      break;
   default:
      q_BitLengthInvalid = true;
      break;
   }
   if (q_BitLengthInvalid == false)
   {
      //Unsigned
      if (q_IsUnsigned == true)
      {
         switch (orc_Content.GetType()) //lint !e788 //only unsigned types handled explicitly
         {
         case C_OscNodeDataPoolContent::eUINT8:
            orc_Content.SetValueU8(0);
            break;
         case C_OscNodeDataPoolContent::eUINT16:
            orc_Content.SetValueU16(0);
            break;
         case C_OscNodeDataPoolContent::eUINT32:
            orc_Content.SetValueU32(0);
            break;
         case C_OscNodeDataPoolContent::eUINT64:
            orc_Content.SetValueU64(0ULL);
            break;
         default:
            tgl_assert(false);
            break;
         }
      }
      //Signed
      if (q_IsSigned == true)
      {
         uint64_t u64_Max = 0;

         //We need exactly one more than half of the unsigned maximum
         if (ou16_BitLength > 0U)
         {
            u64_Max += static_cast<uint64_t>(1ULL) << (static_cast<uint64_t>(ou16_BitLength) - 1ULL);
         }

         switch (orc_Content.GetType()) //lint !e788 //only signed types handled explicitly
         {
         case C_OscNodeDataPoolContent::eSINT8:
            orc_Content.SetValueS8(-static_cast<int8_t>(u64_Max));
            break;
         case C_OscNodeDataPoolContent::eSINT16:
            orc_Content.SetValueS16(-static_cast<int16_t>(u64_Max));
            break;
         case C_OscNodeDataPoolContent::eSINT32:
            orc_Content.SetValueS32(-static_cast<int32_t>(u64_Max));
            break;
         case C_OscNodeDataPoolContent::eSINT64:
            orc_Content.SetValueS64(-static_cast<int64_t>(u64_Max));
            break;
         default:
            tgl_assert(false);
            break;
         }
      }
      //Float
      if (q_IsFloat == true)
      {
         switch (orc_Content.GetType()) //lint !e788 //only float types handled explicitly
         {
         case C_OscNodeDataPoolContent::eFLOAT32:
            orc_Content.SetValueF32(-std::numeric_limits<float32_t>::max());
            break;
         case C_OscNodeDataPoolContent::eFLOAT64:
            orc_Content.SetValueF64(-std::numeric_limits<float64_t>::max());
            break;
         default:
            tgl_assert(false);
            break;
         }
      }
   }
   else
   {
      h_InitMin(orc_Content);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Convert node data pool content type to QVariant

   \param[in]  orc_Input      Node data pool content type
   \param[in]  oru32_Index    Optional array index
   \param[in]  os32_Role      Data role

   \return
   QVariant content
*/
//----------------------------------------------------------------------------------------------------------------------
QVariant C_SdNdeDpContentUtil::h_ConvertContentToGeneric(const C_OscNodeDataPoolContent & orc_Input,
                                                         const uint32_t & oru32_Index, const int32_t os32_Role)
{
   QVariant c_Retval;

   if (orc_Input.GetArray() == false)
   {
      switch (orc_Input.GetType())
      {
      case C_OscNodeDataPoolContent::E_Type::eUINT8:
         c_Retval = static_cast<uint64_t>(orc_Input.GetValueU8());
         break;
      case C_OscNodeDataPoolContent::E_Type::eUINT16:
         c_Retval = static_cast<uint64_t>(orc_Input.GetValueU16());
         break;
      case C_OscNodeDataPoolContent::E_Type::eUINT32:
         c_Retval = static_cast<uint64_t>(orc_Input.GetValueU32());
         break;
      case C_OscNodeDataPoolContent::E_Type::eUINT64:
         c_Retval = orc_Input.GetValueU64();
         break;
      case C_OscNodeDataPoolContent::E_Type::eSINT8:
         c_Retval = static_cast<int64_t>(orc_Input.GetValueS8());
         break;
      case C_OscNodeDataPoolContent::E_Type::eSINT16:
         c_Retval = static_cast<int64_t>(orc_Input.GetValueS16());
         break;
      case C_OscNodeDataPoolContent::E_Type::eSINT32:
         c_Retval = static_cast<int64_t>(orc_Input.GetValueS32());
         break;
      case C_OscNodeDataPoolContent::E_Type::eSINT64:
         c_Retval = orc_Input.GetValueS64();
         break;
      case C_OscNodeDataPoolContent::E_Type::eFLOAT32:
         if (os32_Role == static_cast<int32_t>(Qt::ItemDataRole::DisplayRole))
         {
            QString c_Precison = QString::number(orc_Input.GetValueF32(), 'g', 9);
            c_Precison.replace(QLocale::c().decimalPoint(), QLocale::system().decimalPoint(), Qt::CaseInsensitive);
            c_Retval = c_Precison;
         }
         else
         {
            c_Retval = static_cast<float64_t>(orc_Input.GetValueF32());
         }
         break;
      case C_OscNodeDataPoolContent::E_Type::eFLOAT64:
         if (os32_Role == static_cast<int32_t>(Qt::ItemDataRole::DisplayRole))
         {
            QString c_Precison = QString::number(orc_Input.GetValueF64(), 'g', 17);
            c_Precison.replace(QLocale::c().decimalPoint(), QLocale::system().decimalPoint(),
                               Qt::CaseInsensitive);
            c_Retval = c_Precison;
         }
         else
         {
            c_Retval = orc_Input.GetValueF64();
         }
         break;
      default:
         break;
      }
   }
   else
   {
      switch (orc_Input.GetType())
      {
      case C_OscNodeDataPoolContent::E_Type::eUINT8:
         c_Retval = static_cast<uint64_t>(orc_Input.GetValueArrU8Element(oru32_Index));
         break;
      case C_OscNodeDataPoolContent::E_Type::eUINT16:
         c_Retval = static_cast<uint64_t>(orc_Input.GetValueArrU16Element(oru32_Index));
         break;
      case C_OscNodeDataPoolContent::E_Type::eUINT32:
         c_Retval = static_cast<uint64_t>(orc_Input.GetValueArrU32Element(oru32_Index));
         break;
      case C_OscNodeDataPoolContent::E_Type::eUINT64:
         c_Retval = orc_Input.GetValueArrU64Element(oru32_Index);
         break;
      case C_OscNodeDataPoolContent::E_Type::eSINT8:
         c_Retval = static_cast<int64_t>(orc_Input.GetValueArrS8Element(oru32_Index));
         break;
      case C_OscNodeDataPoolContent::E_Type::eSINT16:
         c_Retval = static_cast<int64_t>(orc_Input.GetValueArrS16Element(oru32_Index));
         break;
      case C_OscNodeDataPoolContent::E_Type::eSINT32:
         c_Retval = static_cast<int64_t>(orc_Input.GetValueArrS32Element(oru32_Index));
         break;
      case C_OscNodeDataPoolContent::E_Type::eSINT64:
         c_Retval = orc_Input.GetValueArrS64Element(oru32_Index);
         break;
      case C_OscNodeDataPoolContent::E_Type::eFLOAT32:
         c_Retval = static_cast<float64_t>(orc_Input.GetValueArrF32Element(oru32_Index));
         break;
      case C_OscNodeDataPoolContent::E_Type::eFLOAT64:
         c_Retval = orc_Input.GetValueArrF64Element(oru32_Index);
         break;
      default:
         break;
      }
   }

   return c_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Convert node data pool content type to QVariant using scaling

   \param[in]  orc_Input                  Node data pool content type
   \param[in]  of64_Factor                Factor
   \param[in]  of64_Offset                Offset
   \param[in]  oru32_Index                Optional array index
   \param[in]  oq_AllowRangeAdaptation    Allow range adaptation
   \param[in]  os32_Role                   Data role

   \return
   QVariant content
*/
//----------------------------------------------------------------------------------------------------------------------
QVariant C_SdNdeDpContentUtil::h_ConvertScaledContentToGeneric(const C_OscNodeDataPoolContent & orc_Input,
                                                               const float64_t of64_Factor, const float64_t of64_Offset,
                                                               const uint32_t & oru32_Index,
                                                               const bool oq_AllowRangeAdaptation,
                                                               const int32_t os32_Role)
{
   QVariant c_Retval;

   if (C_OscUtils::h_IsScalingActive(of64_Factor, of64_Offset) == false)
   {
      c_Retval = C_SdNdeDpContentUtil::h_ConvertContentToGeneric(orc_Input, oru32_Index, os32_Role);
   }
   else
   {
      QString c_Precison;
      std::vector<float64_t> c_Values;
      C_SdNdeDpContentUtil::h_GetValuesAsFloat64(orc_Input, c_Values);
      if (oru32_Index < c_Values.size())
      {
         const float64_t f64_ScaledValue = C_OscUtils::h_GetValueScaled(c_Values[oru32_Index], of64_Factor, of64_Offset,
                                                                        oq_AllowRangeAdaptation);
         if (C_Uti::h_CheckFloatHasNoFractionPart(f64_ScaledValue) == false)
         {
            if (os32_Role == static_cast<int32_t>(Qt::ItemDataRole::DisplayRole))
            {
               c_Precison = QString::number(f64_ScaledValue, 'g', 17);
               c_Precison.replace(QLocale::c().decimalPoint(), QLocale::system().decimalPoint(),
                                  Qt::CaseInsensitive);
               c_Retval = c_Precison;
            }
            else
            {
               c_Retval = f64_ScaledValue;
            }
         }
         else
         {
            //Int is possible but only if in range
            if (((f64_ScaledValue < 0.0) &&
                 (f64_ScaledValue >= static_cast<float64_t>(std::numeric_limits<int64_t>::min()))) &&
                (f64_ScaledValue <= static_cast<float64_t>(std::numeric_limits<int64_t>::max())))
            {
               c_Retval = static_cast<int64_t>(f64_ScaledValue);
            }
            else if ((f64_ScaledValue >= static_cast<float64_t>(std::numeric_limits<uint64_t>::min())) &&
                     (f64_ScaledValue <= static_cast<float64_t>(std::numeric_limits<uint64_t>::max())))
            {
               c_Retval = static_cast<uint64_t>(f64_ScaledValue);
            }
            else
            {
               if (os32_Role == static_cast<int32_t>(Qt::ItemDataRole::DisplayRole))
               {
                  c_Precison = QString::number(f64_ScaledValue, 'g', 17);
                  c_Precison.replace(QLocale::c().decimalPoint(), QLocale::system().decimalPoint(),
                                     Qt::CaseInsensitive);
                  c_Retval = c_Precison;
               }
               else
               {
                  c_Retval = f64_ScaledValue;
               }
            }
         }
      }
   }
   return c_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set node data pool content according to QVariant data

   \param[in]   orc_Value     Variant value
   \param[out]  orc_Output    Node data pool content to set
   \param[in]   oru32_Index   Optional array index

   \retval   C_NO_ERR     Successful conversion
   \retval   C_RANGE      Unknown conversion
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_SdNdeDpContentUtil::h_SetDataVariableFromGeneric(const QVariant & orc_Value,
                                                           C_OscNodeDataPoolContent & orc_Output,
                                                           const uint32_t & oru32_Index)
{
   int32_t s32_Retval = C_NO_ERR;

   switch (orc_Value.type()) //lint !e788 not all types handled here
   {
   case QVariant::ULongLong:
      if (orc_Output.GetArray() == false)
      {
         switch (orc_Output.GetType())
         {
         case C_OscNodeDataPoolContent::E_Type::eUINT8:
            orc_Output.SetValueU8(static_cast<uint8_t>(orc_Value.toULongLong()));
            break;
         case C_OscNodeDataPoolContent::E_Type::eUINT16:
            orc_Output.SetValueU16(static_cast<uint16_t>(orc_Value.toULongLong()));
            break;
         case C_OscNodeDataPoolContent::E_Type::eUINT32:
            orc_Output.SetValueU32(static_cast<uint32_t>(orc_Value.toULongLong()));
            break;
         case C_OscNodeDataPoolContent::E_Type::eUINT64:
            orc_Output.SetValueU64(static_cast<uint64_t>(orc_Value.toULongLong()));
            break;
         case C_OscNodeDataPoolContent::E_Type::eSINT8:
            orc_Output.SetValueS8(static_cast<int8_t>(orc_Value.toULongLong()));
            break;
         case C_OscNodeDataPoolContent::E_Type::eSINT16:
            orc_Output.SetValueS16(static_cast<int16_t>(orc_Value.toULongLong()));
            break;
         case C_OscNodeDataPoolContent::E_Type::eSINT32:
            orc_Output.SetValueS32(static_cast<int32_t>(orc_Value.toULongLong()));
            break;
         case C_OscNodeDataPoolContent::E_Type::eSINT64:
            orc_Output.SetValueS64(static_cast<int64_t>(orc_Value.toULongLong()));
            break;
         case C_OscNodeDataPoolContent::E_Type::eFLOAT32:
            orc_Output.SetValueF32(static_cast<float32_t>(orc_Value.toULongLong()));
            break;
         case C_OscNodeDataPoolContent::E_Type::eFLOAT64:
            orc_Output.SetValueF64(static_cast<float64_t>(orc_Value.toULongLong()));
            break;
         default:
            break;
         }
      }
      else
      {
         switch (orc_Output.GetType())
         {
         case C_OscNodeDataPoolContent::E_Type::eUINT8:
            orc_Output.SetValueArrU8Element(static_cast<uint8_t>(orc_Value.toULongLong()), oru32_Index);
            break;
         case C_OscNodeDataPoolContent::E_Type::eUINT16:
            orc_Output.SetValueArrU16Element(static_cast<uint16_t>(orc_Value.toULongLong()), oru32_Index);
            break;
         case C_OscNodeDataPoolContent::E_Type::eUINT32:
            orc_Output.SetValueArrU32Element(static_cast<uint32_t>(orc_Value.toULongLong()), oru32_Index);
            break;
         case C_OscNodeDataPoolContent::E_Type::eUINT64:
            orc_Output.SetValueArrU64Element(static_cast<uint64_t>(orc_Value.toULongLong()), oru32_Index);
            break;
         case C_OscNodeDataPoolContent::E_Type::eSINT8:
            orc_Output.SetValueArrS8Element(static_cast<int8_t>(orc_Value.toULongLong()), oru32_Index);
            break;
         case C_OscNodeDataPoolContent::E_Type::eSINT16:
            orc_Output.SetValueArrS16Element(static_cast<int16_t>(orc_Value.toULongLong()), oru32_Index);
            break;
         case C_OscNodeDataPoolContent::E_Type::eSINT32:
            orc_Output.SetValueArrS32Element(static_cast<int32_t>(orc_Value.toULongLong()), oru32_Index);
            break;
         case C_OscNodeDataPoolContent::E_Type::eSINT64:
            orc_Output.SetValueArrS64Element(static_cast<int64_t>(orc_Value.toULongLong()), oru32_Index);
            break;
         case C_OscNodeDataPoolContent::E_Type::eFLOAT32:
            orc_Output.SetValueArrF32Element(static_cast<float32_t>(orc_Value.toULongLong()), oru32_Index);
            break;
         case C_OscNodeDataPoolContent::E_Type::eFLOAT64:
            orc_Output.SetValueArrF64Element(static_cast<float64_t>(orc_Value.toULongLong()), oru32_Index);
            break;
         default:
            break;
         }
      }
      break;
   case QVariant::LongLong:
      if (orc_Output.GetArray() == false)
      {
         switch (orc_Output.GetType())
         {
         case C_OscNodeDataPoolContent::E_Type::eUINT8:
            orc_Output.SetValueU8(static_cast<uint8_t>(orc_Value.toLongLong()));
            break;
         case C_OscNodeDataPoolContent::E_Type::eUINT16:
            orc_Output.SetValueU16(static_cast<uint16_t>(orc_Value.toLongLong()));
            break;
         case C_OscNodeDataPoolContent::E_Type::eUINT32:
            orc_Output.SetValueU32(static_cast<uint32_t>(orc_Value.toLongLong()));
            break;
         case C_OscNodeDataPoolContent::E_Type::eUINT64:
            orc_Output.SetValueU64(static_cast<uint64_t>(orc_Value.toLongLong()));
            break;
         case C_OscNodeDataPoolContent::E_Type::eSINT8:
            orc_Output.SetValueS8(static_cast<int8_t>(orc_Value.toLongLong()));
            break;
         case C_OscNodeDataPoolContent::E_Type::eSINT16:
            orc_Output.SetValueS16(static_cast<int16_t>(orc_Value.toLongLong()));
            break;
         case C_OscNodeDataPoolContent::E_Type::eSINT32:
            orc_Output.SetValueS32(static_cast<int32_t>(orc_Value.toLongLong()));
            break;
         case C_OscNodeDataPoolContent::E_Type::eSINT64:
            orc_Output.SetValueS64(static_cast<int64_t>(orc_Value.toLongLong()));
            break;
         case C_OscNodeDataPoolContent::E_Type::eFLOAT32:
            orc_Output.SetValueF32(static_cast<float32_t>(orc_Value.toLongLong()));
            break;
         case C_OscNodeDataPoolContent::E_Type::eFLOAT64:
            orc_Output.SetValueF64(static_cast<float64_t>(orc_Value.toLongLong()));
            break;
         default:
            break;
         }
      }
      else
      {
         switch (orc_Output.GetType())
         {
         case C_OscNodeDataPoolContent::E_Type::eUINT8:
            orc_Output.SetValueArrU8Element(static_cast<uint8_t>(orc_Value.toLongLong()), oru32_Index);
            break;
         case C_OscNodeDataPoolContent::E_Type::eUINT16:
            orc_Output.SetValueArrU16Element(static_cast<uint16_t>(orc_Value.toLongLong()), oru32_Index);
            break;
         case C_OscNodeDataPoolContent::E_Type::eUINT32:
            orc_Output.SetValueArrU32Element(static_cast<uint32_t>(orc_Value.toLongLong()), oru32_Index);
            break;
         case C_OscNodeDataPoolContent::E_Type::eUINT64:
            orc_Output.SetValueArrU64Element(static_cast<uint64_t>(orc_Value.toLongLong()), oru32_Index);
            break;
         case C_OscNodeDataPoolContent::E_Type::eSINT8:
            orc_Output.SetValueArrS8Element(static_cast<int8_t>(orc_Value.toLongLong()), oru32_Index);
            break;
         case C_OscNodeDataPoolContent::E_Type::eSINT16:
            orc_Output.SetValueArrS16Element(static_cast<int16_t>(orc_Value.toLongLong()), oru32_Index);
            break;
         case C_OscNodeDataPoolContent::E_Type::eSINT32:
            orc_Output.SetValueArrS32Element(static_cast<int32_t>(orc_Value.toLongLong()), oru32_Index);
            break;
         case C_OscNodeDataPoolContent::E_Type::eSINT64:
            orc_Output.SetValueArrS64Element(static_cast<int64_t>(orc_Value.toLongLong()), oru32_Index);
            break;
         case C_OscNodeDataPoolContent::E_Type::eFLOAT32:
            orc_Output.SetValueArrF32Element(static_cast<float32_t>(orc_Value.toLongLong()), oru32_Index);
            break;
         case C_OscNodeDataPoolContent::E_Type::eFLOAT64:
            orc_Output.SetValueArrF64Element(static_cast<float64_t>(orc_Value.toLongLong()), oru32_Index);
            break;
         default:
            break;
         }
      }
      break;
   case QVariant::UInt:
      if (orc_Output.GetArray() == false)
      {
         switch (orc_Output.GetType())
         {
         case C_OscNodeDataPoolContent::E_Type::eUINT8:
            orc_Output.SetValueU8(static_cast<uint8_t>(orc_Value.toUInt()));
            break;
         case C_OscNodeDataPoolContent::E_Type::eUINT16:
            orc_Output.SetValueU16(static_cast<uint16_t>(orc_Value.toUInt()));
            break;
         case C_OscNodeDataPoolContent::E_Type::eUINT32:
            orc_Output.SetValueU32(static_cast<uint32_t>(orc_Value.toUInt()));
            break;
         case C_OscNodeDataPoolContent::E_Type::eUINT64:
            orc_Output.SetValueU64(static_cast<uint64_t>(orc_Value.toUInt()));
            break;
         case C_OscNodeDataPoolContent::E_Type::eSINT8:
            orc_Output.SetValueS8(static_cast<int8_t>(orc_Value.toUInt()));
            break;
         case C_OscNodeDataPoolContent::E_Type::eSINT16:
            orc_Output.SetValueS16(static_cast<int16_t>(orc_Value.toUInt()));
            break;
         case C_OscNodeDataPoolContent::E_Type::eSINT32:
            orc_Output.SetValueS32(static_cast<int32_t>(orc_Value.toUInt()));
            break;
         case C_OscNodeDataPoolContent::E_Type::eSINT64:
            orc_Output.SetValueS64(static_cast<int64_t>(orc_Value.toUInt()));
            break;
         case C_OscNodeDataPoolContent::E_Type::eFLOAT32:
            orc_Output.SetValueF32(static_cast<float32_t>(orc_Value.toUInt()));
            break;
         case C_OscNodeDataPoolContent::E_Type::eFLOAT64:
            orc_Output.SetValueF64(static_cast<float64_t>(orc_Value.toUInt()));
            break;
         default:
            break;
         }
      }
      else
      {
         switch (orc_Output.GetType())
         {
         case C_OscNodeDataPoolContent::E_Type::eUINT8:
            orc_Output.SetValueArrU8Element(static_cast<uint8_t>(orc_Value.toUInt()), oru32_Index);
            break;
         case C_OscNodeDataPoolContent::E_Type::eUINT16:
            orc_Output.SetValueArrU16Element(static_cast<uint16_t>(orc_Value.toUInt()), oru32_Index);
            break;
         case C_OscNodeDataPoolContent::E_Type::eUINT32:
            orc_Output.SetValueArrU32Element(static_cast<uint32_t>(orc_Value.toUInt()), oru32_Index);
            break;
         case C_OscNodeDataPoolContent::E_Type::eUINT64:
            orc_Output.SetValueArrU64Element(static_cast<uint64_t>(orc_Value.toUInt()), oru32_Index);
            break;
         case C_OscNodeDataPoolContent::E_Type::eSINT8:
            orc_Output.SetValueArrS8Element(static_cast<int8_t>(orc_Value.toUInt()), oru32_Index);
            break;
         case C_OscNodeDataPoolContent::E_Type::eSINT16:
            orc_Output.SetValueArrS16Element(static_cast<int16_t>(orc_Value.toUInt()), oru32_Index);
            break;
         case C_OscNodeDataPoolContent::E_Type::eSINT32:
            orc_Output.SetValueArrS32Element(static_cast<int32_t>(orc_Value.toUInt()), oru32_Index);
            break;
         case C_OscNodeDataPoolContent::E_Type::eSINT64:
            orc_Output.SetValueArrS64Element(static_cast<int64_t>(orc_Value.toUInt()), oru32_Index);
            break;
         case C_OscNodeDataPoolContent::E_Type::eFLOAT32:
            orc_Output.SetValueArrF32Element(static_cast<float32_t>(orc_Value.toUInt()), oru32_Index);
            break;
         case C_OscNodeDataPoolContent::E_Type::eFLOAT64:
            orc_Output.SetValueArrF64Element(static_cast<float64_t>(orc_Value.toUInt()), oru32_Index);
            break;
         default:
            break;
         }
      }
      break;
   case QVariant::Int:
      if (orc_Output.GetArray() == false)
      {
         switch (orc_Output.GetType())
         {
         case C_OscNodeDataPoolContent::E_Type::eUINT8:
            orc_Output.SetValueU8(static_cast<uint8_t>(orc_Value.toInt()));
            break;
         case C_OscNodeDataPoolContent::E_Type::eUINT16:
            orc_Output.SetValueU16(static_cast<uint16_t>(orc_Value.toInt()));
            break;
         case C_OscNodeDataPoolContent::E_Type::eUINT32:
            orc_Output.SetValueU32(static_cast<uint32_t>(orc_Value.toInt()));
            break;
         case C_OscNodeDataPoolContent::E_Type::eUINT64:
            orc_Output.SetValueU64(static_cast<uint64_t>(static_cast<uint32_t>(orc_Value.toInt())));
            break;
         case C_OscNodeDataPoolContent::E_Type::eSINT8:
            orc_Output.SetValueS8(static_cast<int8_t>(orc_Value.toInt()));
            break;
         case C_OscNodeDataPoolContent::E_Type::eSINT16:
            orc_Output.SetValueS16(static_cast<int16_t>(orc_Value.toInt()));
            break;
         case C_OscNodeDataPoolContent::E_Type::eSINT32:
            orc_Output.SetValueS32(static_cast<int32_t>(orc_Value.toInt()));
            break;
         case C_OscNodeDataPoolContent::E_Type::eSINT64:
            orc_Output.SetValueS64(static_cast<int64_t>(orc_Value.toInt()));
            break;
         case C_OscNodeDataPoolContent::E_Type::eFLOAT32:
            orc_Output.SetValueF32(static_cast<float32_t>(orc_Value.toInt()));
            break;
         case C_OscNodeDataPoolContent::E_Type::eFLOAT64:
            orc_Output.SetValueF64(static_cast<float64_t>(orc_Value.toInt()));
            break;
         default:
            break;
         }
      }
      else
      {
         switch (orc_Output.GetType())
         {
         case C_OscNodeDataPoolContent::E_Type::eUINT8:
            orc_Output.SetValueArrU8Element(static_cast<uint8_t>(orc_Value.toInt()), oru32_Index);
            break;
         case C_OscNodeDataPoolContent::E_Type::eUINT16:
            orc_Output.SetValueArrU16Element(static_cast<uint16_t>(orc_Value.toInt()), oru32_Index);
            break;
         case C_OscNodeDataPoolContent::E_Type::eUINT32:
            orc_Output.SetValueArrU32Element(static_cast<uint32_t>(orc_Value.toInt()), oru32_Index);
            break;
         case C_OscNodeDataPoolContent::E_Type::eUINT64:
            orc_Output.SetValueArrU64Element(static_cast<uint64_t>(static_cast<uint32_t>(orc_Value.toInt())),
                                             oru32_Index);
            break;
         case C_OscNodeDataPoolContent::E_Type::eSINT8:
            orc_Output.SetValueArrS8Element(static_cast<int8_t>(orc_Value.toInt()), oru32_Index);
            break;
         case C_OscNodeDataPoolContent::E_Type::eSINT16:
            orc_Output.SetValueArrS16Element(static_cast<int16_t>(orc_Value.toInt()), oru32_Index);
            break;
         case C_OscNodeDataPoolContent::E_Type::eSINT32:
            orc_Output.SetValueArrS32Element(static_cast<int32_t>(orc_Value.toInt()), oru32_Index);
            break;
         case C_OscNodeDataPoolContent::E_Type::eSINT64:
            orc_Output.SetValueArrS64Element(static_cast<int64_t>(orc_Value.toInt()), oru32_Index);
            break;
         case C_OscNodeDataPoolContent::E_Type::eFLOAT32:
            orc_Output.SetValueArrF32Element(static_cast<float32_t>(orc_Value.toInt()), oru32_Index);
            break;
         case C_OscNodeDataPoolContent::E_Type::eFLOAT64:
            orc_Output.SetValueArrF64Element(static_cast<float64_t>(orc_Value.toInt()), oru32_Index);
            break;
         default:
            break;
         }
      }
      break;
   case QVariant::Double:
      if (orc_Output.GetArray() == false)
      {
         switch (orc_Output.GetType())
         {
         case C_OscNodeDataPoolContent::E_Type::eUINT8:
            orc_Output.SetValueU8(static_cast<uint8_t>(orc_Value.toDouble()));
            break;
         case C_OscNodeDataPoolContent::E_Type::eUINT16:
            orc_Output.SetValueU16(static_cast<uint16_t>(orc_Value.toDouble()));
            break;
         case C_OscNodeDataPoolContent::E_Type::eUINT32:
            orc_Output.SetValueU32(static_cast<uint32_t>(orc_Value.toDouble()));
            break;
         case C_OscNodeDataPoolContent::E_Type::eUINT64:
            orc_Output.SetValueU64(static_cast<uint64_t>(orc_Value.toDouble()));
            break;
         case C_OscNodeDataPoolContent::E_Type::eSINT8:
            orc_Output.SetValueS8(static_cast<int8_t>(orc_Value.toDouble()));
            break;
         case C_OscNodeDataPoolContent::E_Type::eSINT16:
            orc_Output.SetValueS16(static_cast<int16_t>(orc_Value.toDouble()));
            break;
         case C_OscNodeDataPoolContent::E_Type::eSINT32:
            orc_Output.SetValueS32(static_cast<int32_t>(orc_Value.toDouble()));
            break;
         case C_OscNodeDataPoolContent::E_Type::eSINT64:
            orc_Output.SetValueS64(static_cast<int64_t>(orc_Value.toDouble()));
            break;
         case C_OscNodeDataPoolContent::E_Type::eFLOAT32:
            orc_Output.SetValueF32(static_cast<float32_t>(orc_Value.toDouble()));
            break;
         case C_OscNodeDataPoolContent::E_Type::eFLOAT64:
            orc_Output.SetValueF64(static_cast<float64_t>(orc_Value.toDouble()));
            break;
         default:
            break;
         }
      }
      else
      {
         switch (orc_Output.GetType())
         {
         case C_OscNodeDataPoolContent::E_Type::eUINT8:
            orc_Output.SetValueArrU8Element(static_cast<uint8_t>(orc_Value.toDouble()), oru32_Index);
            break;
         case C_OscNodeDataPoolContent::E_Type::eUINT16:
            orc_Output.SetValueArrU16Element(static_cast<uint16_t>(orc_Value.toDouble()), oru32_Index);
            break;
         case C_OscNodeDataPoolContent::E_Type::eUINT32:
            orc_Output.SetValueArrU32Element(static_cast<uint32_t>(orc_Value.toDouble()), oru32_Index);
            break;
         case C_OscNodeDataPoolContent::E_Type::eUINT64:
            orc_Output.SetValueArrU64Element(static_cast<uint64_t>(orc_Value.toDouble()), oru32_Index);
            break;
         case C_OscNodeDataPoolContent::E_Type::eSINT8:
            orc_Output.SetValueArrS8Element(static_cast<int8_t>(orc_Value.toDouble()), oru32_Index);
            break;
         case C_OscNodeDataPoolContent::E_Type::eSINT16:
            orc_Output.SetValueArrS16Element(static_cast<int16_t>(orc_Value.toDouble()), oru32_Index);
            break;
         case C_OscNodeDataPoolContent::E_Type::eSINT32:
            orc_Output.SetValueArrS32Element(static_cast<int32_t>(orc_Value.toDouble()), oru32_Index);
            break;
         case C_OscNodeDataPoolContent::E_Type::eSINT64:
            orc_Output.SetValueArrS64Element(static_cast<int64_t>(orc_Value.toDouble()), oru32_Index);
            break;
         case C_OscNodeDataPoolContent::E_Type::eFLOAT32:
            orc_Output.SetValueArrF32Element(static_cast<float32_t>(orc_Value.toDouble()), oru32_Index);
            break;
         case C_OscNodeDataPoolContent::E_Type::eFLOAT64:
            orc_Output.SetValueArrF64Element(static_cast<float64_t>(orc_Value.toDouble()), oru32_Index);
            break;
         default:
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

   \param[in]   orc_Value     Variant value
   \param[out]  orc_Output    Node data pool content to set
   \param[in]   of64_Factor   Factor
   \param[in]   of64_Offset   Offset
   \param[in]   oru32_Index   Optional array index

   \retval   C_NO_ERR     Success
   \retval   C_RANGE      Unknown conversion
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_SdNdeDpContentUtil::h_SetDataVariableFromGenericWithScaling(const QVariant & orc_Value,
                                                                      C_OscNodeDataPoolContent & orc_Output,
                                                                      const float64_t of64_Factor,
                                                                      const float64_t of64_Offset,
                                                                      const uint32_t & oru32_Index)
{
   int32_t s32_Retval = C_NO_ERR;

   if (C_OscUtils::h_IsScalingActive(of64_Factor, of64_Offset) == false)
   {
      C_SdNdeDpContentUtil::h_SetDataVariableFromGeneric(orc_Value, orc_Output, oru32_Index);
   }
   else
   {
      float64_t f64_TmpValue = 0.0;
      bool q_Success;

      switch (orc_Value.type()) //lint !e788 not all types handled here
      {
      case QVariant::Int:
         f64_TmpValue = static_cast<float64_t>(orc_Value.toInt(&q_Success));
         break;
      case QVariant::UInt:
         f64_TmpValue = static_cast<float64_t>(orc_Value.toUInt(&q_Success));
         break;
      case QVariant::LongLong:
         f64_TmpValue = static_cast<float64_t>(orc_Value.toLongLong(&q_Success));
         break;
      case QVariant::ULongLong:
         f64_TmpValue = static_cast<float64_t>(orc_Value.toULongLong(&q_Success));
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
         C_SdNdeDpContentUtil::h_SetScaledValueInContent(f64_TmpValue, orc_Output, of64_Factor, of64_Offset,
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

   \param[in]  orc_Input   Content to store

   \return
   Current content encoded as string
*/
//----------------------------------------------------------------------------------------------------------------------
QString C_SdNdeDpContentUtil::h_GetAllContentAsString(const C_OscNodeDataPoolContent & orc_Input)
{
   stw::scl::C_SclString c_Retval;
   C_OscXmlParser c_Xml;
   c_Xml.CreateAndSelectNodeChild("opensyde-content");
   C_OscNodeDataPoolFiler::h_SaveDataPoolContentV1(orc_Input, c_Xml);
   c_Xml.SaveToString(c_Retval);
   return c_Retval.c_str();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Restore content from string

   Use case: store to string to later use the same string to restore the same content

   \param[in]   orc_Input     String to base restoration on
   \param[out]  orc_Output    Output of restoration

   \return
   C_NO_ERR   data read
   C_CONFIG   content of file is invalid or incomplete
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_SdNdeDpContentUtil::h_SetAllContentFromString(const QString & orc_Input,
                                                        C_OscNodeDataPoolContent & orc_Output)
{
   C_OscXmlParser c_Xml;

   c_Xml.LoadFromString(orc_Input.toStdString().c_str());
   c_Xml.SelectRoot();
   return C_OscNodeDataPoolFiler::h_LoadDataPoolContentV1(orc_Output, c_Xml);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Convert generic data type to string

   \param[in]  orc_Data    Generic data type

   \return
   Generic data type string content
*/
//----------------------------------------------------------------------------------------------------------------------
QString C_SdNdeDpContentUtil::h_ConvertToString(const C_OscNodeDataPoolContent & orc_Data)
{
   QString c_Retval = "";

   //Each element is an character
   if (orc_Data.GetType() == C_OscNodeDataPoolContent::eSINT8)
   {
      if (orc_Data.GetArray() == true)
      {
         for (uint32_t u32_ItContent = 0; u32_ItContent < orc_Data.GetArraySize(); ++u32_ItContent)
         {
            const char_t cn_Char = static_cast<char_t>(orc_Data.GetValueArrS8Element(u32_ItContent));
            if (cn_Char == '\0')
            {
               break;
            }
            else
            {
               c_Retval += cn_Char;
            }
         }
      }
      else
      {
         c_Retval = static_cast<QString>(static_cast<char_t>(orc_Data.GetValueS8()));
      }
   }
   return c_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Convert sint8 array to string

   \param[in]  orc_Data    sint8 array

   \return
   String content
*/
//----------------------------------------------------------------------------------------------------------------------
QString C_SdNdeDpContentUtil::h_ConvertToString(const std::vector<int8_t> & orc_Data)
{
   QString c_Retval = "";

   //Each element is an character
   for (uint32_t u32_ItContent = 0; u32_ItContent < orc_Data.size(); ++u32_ItContent)
   {
      const char_t cn_Char = static_cast<char_t>(orc_Data[u32_ItContent]);
      if (cn_Char == '\0')
      {
         break;
      }
      else
      {
         c_Retval += cn_Char;
      }
   }

   return c_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Handle QString to OSCContent conversion

   \param[in]      orc_String    QString input
   \param[in,out]  orc_Output    OSCContent output
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDpContentUtil::h_SetString(const QString & orc_String, C_OscNodeDataPoolContent & orc_Output)
{
   if (orc_String.size() <= static_cast<int32_t>(orc_Output.GetArraySize()))
   {
      if (orc_Output.GetArraySize() == 1)
      {
         if (orc_String.size() == 1)
         {
            orc_Output.SetValueS8(static_cast<int8_t>(orc_String.at(0).toLatin1()));
         }
      }
      else
      {
         for (uint32_t u32_ItOutput = 0; u32_ItOutput < orc_Output.GetArraySize(); ++u32_ItOutput)
         {
            //As long as there is a string content use that
            if (static_cast<int32_t>(u32_ItOutput) < orc_String.length())
            {
               orc_Output.SetValueArrS8Element(static_cast<int8_t>(orc_String.at(static_cast<int32_t>(u32_ItOutput)).
                                                                   toLatin1()), u32_ItOutput);
            }
            else
            {
               //If the string has ended fill up all remaining values with defined content
               orc_Output.SetValueArrS8Element(static_cast<int8_t>('\0'), u32_ItOutput);
            }
         }
      }
   }
}
