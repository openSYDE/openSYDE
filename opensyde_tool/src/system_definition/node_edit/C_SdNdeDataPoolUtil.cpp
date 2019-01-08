//-----------------------------------------------------------------------------
/*!
   \internal
   \file
   \brief       Node data pool util class (implementation)

   Node data pool util class

   \implementation
   project     openSYDE
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     27.02.2017  STW/M.Echtler
   \endimplementation
*/
//-----------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------- */
#include "precomp_headers.h"

#include <cmath>
#include <limits>
#include <QApplication>
#include "stwerrors.h"
#include "C_OSCUtils.h"
#include "C_SdNdeDataPoolUtil.h"
#include "C_SdUtil.h"
#include "C_Uti.h"
#include "constants.h"
#include "C_PuiSdHandler.h"
#include "C_SdNdeDataPoolListHeaderWidget.h"
#include "C_OgeWiTableSpinBoxGroup.h"
#include "C_OgeWiParamSpinBoxGroup.h"
#include "C_OgeLeTable.h"
#include "C_OgeLeParam.h"
#include "C_GtGetText.h"
#include "C_SdNdeDataPoolContentUtil.h"

/* -- Used Namespaces ------------------------------------------------------ */
using namespace stw_types;
using namespace stw_errors;
using namespace stw_opensyde_gui_logic;
using namespace stw_opensyde_gui;
using namespace stw_opensyde_gui_elements;
using namespace stw_opensyde_core;

/* -- Module Global Constants ---------------------------------------------- */

/* -- Types ---------------------------------------------------------------- */

/* -- Global Variables ----------------------------------------------------- */

/* -- Module Global Variables ---------------------------------------------- */

/* -- Module Global Function Prototypes ------------------------------------ */

/* -- Implementation ------------------------------------------------------- */

//-----------------------------------------------------------------------------
/*!
   \brief   Check if indices connected

   \param[in] orc_Items Indices to connect

   \return
   true  Indices connected
   false Indices not connected

   \created     27.02.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
bool C_SdNdeDataPoolUtil::h_CheckConnected(const std::vector<uint32> & orc_Items)
{
   bool q_Retval = true;

   //Convert & sort
   const std::vector<uint32> c_Items = C_Uti::h_UniquifyAndSortAscending(orc_Items);

   //Check connected
   if (c_Items.size() > 0)
   {
      const uint32 u32_Start = c_Items[0];

      for (uint32 u32_It = 1; u32_It < c_Items.size(); ++u32_It)
      {
         if (c_Items[u32_It] != (u32_Start + u32_It))
         {
            q_Retval = false;
            break;
         }
      }
   }
   return q_Retval;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Convert item selection to std vector

   \param[in] orc_Items Item selection
   \param[in] orq_Rows  Optional indicator if selection should be on rows or columns

   \return
   Std vector of selected items

   \created     27.02.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
std::vector<uint32> C_SdNdeDataPoolUtil::h_ConvertVector(const QModelIndexList & orc_Items, const bool & orq_Rows)
{
   std::vector<uint32> c_Retval;
   c_Retval.reserve(orc_Items.size());
   for (uint32 u32_ItInput = 0; u32_ItInput < static_cast<uint32>(orc_Items.size()); ++u32_ItInput)
   {
      if (orq_Rows == true)
      {
         c_Retval.push_back(orc_Items.at(u32_ItInput).row());
      }
      else
      {
         c_Retval.push_back(orc_Items.at(u32_ItInput).column());
      }
   }
   return c_Retval;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Convert element type to generic

   \param[in]  orc_OSCElement     OSC Element
   \param[in]  orc_UIElement      UI Element
   \param[in]  ore_Type           Type to convert
   \param[out] orc_Generic        Generic output
   \param[in]  oru32_Index        Optional array index
   \param[in]  ors32_DataSetIndex Optional data set index

   \created     10.03.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_SdNdeDataPoolUtil::h_ConvertToGeneric(const stw_opensyde_core::C_OSCNodeDataPoolListElement & orc_OSCElement,
                                             const C_PuiSdNodeDataPoolListElement & orc_UIElement,
                                             const C_SdNdeDataPoolUtil::E_ElementDataChangeType & ore_Type,
                                             QVariant & orc_Generic, const uint32 & oru32_Index,
                                             const sint32 & ors32_DataSetIndex)
{
   switch (ore_Type)
   {
   case eELEMENT_NAME:
      orc_Generic = orc_OSCElement.c_Name.c_str();
      break;
   case eELEMENT_COMMENT:
      orc_Generic = orc_OSCElement.c_Comment.c_str();
      break;
   case eELEMENT_VALUE_TYPE:
      if (orc_UIElement.q_InterpretAsString == true)
      {
         orc_Generic = 10;
      }
      else
      {
         orc_Generic = h_ConvertContentTypeToComboBox(orc_OSCElement.c_Value.GetType());
      }
      break;
   case eELEMENT_ARRAY:
      if (orc_OSCElement.GetArray() == false)
      {
         orc_Generic = 1;
      }
      else
      {
         orc_Generic = static_cast<stw_types::sintn>(orc_OSCElement.GetArraySize());
      }
      break;
   case eELEMENT_AUTO_MIN_MAX:
      orc_Generic = orc_UIElement.q_AutoMinMaxActive;
      break;
   case eELEMENT_MIN:
      orc_Generic = h_DataVariable(orc_OSCElement.c_MinValue, oru32_Index);
      break;
   case eELEMENT_MAX:
      orc_Generic = h_DataVariable(orc_OSCElement.c_MaxValue, oru32_Index);
      break;
   case eELEMENT_FACTOR:
      orc_Generic = orc_OSCElement.f64_Factor;
      break;
   case eELEMENT_OFFSET:
      orc_Generic = orc_OSCElement.f64_Offset;
      break;
   case eELEMENT_UNIT:
      orc_Generic = orc_OSCElement.c_Unit.c_str();
      break;
   case eELEMENT_DATA_SET:
      if ((ors32_DataSetIndex >= 0) &&
          (static_cast<uint32>(ors32_DataSetIndex) < orc_OSCElement.c_DataSetValues.size()))
      {
         if (orc_UIElement.q_InterpretAsString == true)
         {
            orc_Generic =
               C_SdNdeDataPoolUtil::h_ConvertToString(
                  orc_OSCElement.c_DataSetValues[static_cast<uint32>(ors32_DataSetIndex)]);
         }
         else
         {
            orc_Generic = h_DataVariable(orc_OSCElement.c_DataSetValues[static_cast<uint32>(ors32_DataSetIndex)],
                                         oru32_Index);
         }
      }
      break;
   case eELEMENT_ACCESS:
      orc_Generic = h_ConvertElementAccessToComboBox(orc_OSCElement.e_Access);
      break;
   case eELEMENT_EVENT_CALL:
      orc_Generic = orc_OSCElement.q_DiagEventCall;
      break;
   default:
      break;
   }
}

//-----------------------------------------------------------------------------
/*!
   \brief   Convert node data pool content type to QVariant

   \param[in] orc_Input   Node data pool content type
   \param[in] oru32_Index Optional array index

   \return
   QVariant content

   \created     15.02.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
QVariant C_SdNdeDataPoolUtil::h_DataVariable(const C_OSCNodeDataPoolContent & orc_Input, const uint32 & oru32_Index)
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

//-----------------------------------------------------------------------------
/*!
   \brief   Convert node data pool content type to QVariant using scaling

   \param[in] orc_Input   Node data pool content type
   \param[in] of64_Factor Factor
   \param[in] of64_Offset Offset
   \param[in] oru32_Index Optional array index

   \return
   QVariant content

   \created     13.11.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
QVariant C_SdNdeDataPoolUtil::h_ScaledDataVariable(const C_OSCNodeDataPoolContent & orc_Input,
                                                   const float64 of64_Factor, const float64 of64_Offset,
                                                   const uint32 & oru32_Index)
{
   QVariant c_Retval;

   if (C_OSCUtils::h_IsScalingActive(of64_Factor, of64_Offset) == false)
   {
      c_Retval = C_SdNdeDataPoolUtil::h_DataVariable(orc_Input, oru32_Index);
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

//-----------------------------------------------------------------------------
/*!
   \brief   Set node data pool content according to QVariant data

   \param[in]  orc_Value    Variant value
   \param[out] orc_Output   Node data pool content to set
   \param[in]  oru32_Index  Optional array index

   \created     15.02.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
sint32 C_SdNdeDataPoolUtil::h_SetDataVariable(const QVariant & orc_Value, C_OSCNodeDataPoolContent & orc_Output,
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

//-----------------------------------------------------------------------------
/*!
   \brief   Set node data pool content according to QVariant data using scaling

   \param[in]  orc_Value    Variant value
   \param[out] orc_Output   Node data pool content to set
   \param[in]  of64_Factor  Factor
   \param[in]  of64_Offset  Offset
   \param[in]  oru32_Index  Optional array index

   \created     13.11.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
sint32 C_SdNdeDataPoolUtil::h_SetUnscaledDataVariable(const QVariant & orc_Value, C_OSCNodeDataPoolContent & orc_Output,
                                                      const float64 of64_Factor, const float64 of64_Offset,
                                                      const uint32 & oru32_Index)
{
   sint32 s32_Retval = C_NO_ERR;

   if (C_OSCUtils::h_IsScalingActive(of64_Factor, of64_Offset) == false)
   {
      C_SdNdeDataPoolUtil::h_SetDataVariable(orc_Value, orc_Output, oru32_Index);
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

//-----------------------------------------------------------------------------
/*!
   \brief   Convert node data pool content type to combo box index

   \param[in] ore_Type Node data pool content type

   \return
   Combo box index

   \created     15.02.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
sint8 C_SdNdeDataPoolUtil::h_ConvertContentTypeToComboBox(const C_OSCNodeDataPoolContent::E_Type & ore_Type)
{
   sint8 s8_Retval;

   switch (ore_Type)
   {
   case C_OSCNodeDataPoolContent::E_Type::eUINT8:
      s8_Retval = 0;
      break;
   case C_OSCNodeDataPoolContent::E_Type::eUINT16:
      s8_Retval = 2;
      break;
   case C_OSCNodeDataPoolContent::E_Type::eUINT32:
      s8_Retval = 4;
      break;
   case C_OSCNodeDataPoolContent::E_Type::eUINT64:
      s8_Retval = 6;
      break;
   case C_OSCNodeDataPoolContent::E_Type::eSINT8:
      s8_Retval = 1;
      break;
   case C_OSCNodeDataPoolContent::E_Type::eSINT16:
      s8_Retval = 3;
      break;
   case C_OSCNodeDataPoolContent::E_Type::eSINT32:
      s8_Retval = 5;
      break;
   case C_OSCNodeDataPoolContent::E_Type::eSINT64:
      s8_Retval = 7;
      break;
   case C_OSCNodeDataPoolContent::E_Type::eFLOAT32:
      s8_Retval = 8;
      break;
   case C_OSCNodeDataPoolContent::E_Type::eFLOAT64:
      s8_Retval = 9;
      break;
   default:
      s8_Retval = -1;
      break;
   }

   return s8_Retval;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Convert combo box index to node data pool content type

   \param[in] ors8_Type Combo box index

   \return
   Node data pool content type

   \created     15.02.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
C_OSCNodeDataPoolContent::E_Type C_SdNdeDataPoolUtil::h_ConvertContentTypeFromComboBox(const sint8 & ors8_Type)
{
   C_OSCNodeDataPoolContent::E_Type e_Retval;
   switch (ors8_Type)
   {
   case 0:
      e_Retval = C_OSCNodeDataPoolContent::E_Type::eUINT8;
      break;
   case 2:
      e_Retval = C_OSCNodeDataPoolContent::E_Type::eUINT16;
      break;
   case 4:
      e_Retval = C_OSCNodeDataPoolContent::E_Type::eUINT32;
      break;
   case 6:
      e_Retval = C_OSCNodeDataPoolContent::E_Type::eUINT64;
      break;
   case 1:
      e_Retval = C_OSCNodeDataPoolContent::E_Type::eSINT8;
      break;
   case 3:
      e_Retval = C_OSCNodeDataPoolContent::E_Type::eSINT16;
      break;
   case 5:
      e_Retval = C_OSCNodeDataPoolContent::E_Type::eSINT32;
      break;
   case 7:
      e_Retval = C_OSCNodeDataPoolContent::E_Type::eSINT64;
      break;
   case 8:
      e_Retval = C_OSCNodeDataPoolContent::E_Type::eFLOAT32;
      break;
   case 9:
      e_Retval = C_OSCNodeDataPoolContent::E_Type::eFLOAT64;
      break;
   default:
      e_Retval = C_OSCNodeDataPoolContent::E_Type::eSINT8;
      break;
   }
   return e_Retval;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Convert node data pool list element access type to combo box index

   \param[in] ore_Type Node data pool list element access type

   \return
   Combo box index

   \created     17.02.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
sint8 C_SdNdeDataPoolUtil::h_ConvertElementAccessToComboBox(const C_OSCNodeDataPoolListElement::E_Access & ore_Type)
{
   sint8 s8_Retval;

   switch (ore_Type)
   {
   case C_OSCNodeDataPoolListElement::E_Access::eACCESS_RW:
      s8_Retval = 0;
      break;
   case C_OSCNodeDataPoolListElement::E_Access::eACCESS_RO:
      s8_Retval = 1;
      break;
   default:
      s8_Retval = -1;
      break;
   }

   return s8_Retval;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Convert combo box index to node data pool list element access type

   \param[in] ors8_Type Combo box index

   \return
   Node data pool list element access type

   \created     17.02.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
C_OSCNodeDataPoolListElement::E_Access C_SdNdeDataPoolUtil::h_ConvertElementAccesFromComboBox(const sint8 & ors8_Type)
{
   C_OSCNodeDataPoolListElement::E_Access e_Retval;
   switch (ors8_Type)
   {
   case 0:
      e_Retval = C_OSCNodeDataPoolListElement::E_Access::eACCESS_RW;
      break;
   case 1:
      e_Retval = C_OSCNodeDataPoolListElement::E_Access::eACCESS_RO;
      break;
   default:
      e_Retval = C_OSCNodeDataPoolListElement::E_Access::eACCESS_RO;
      break;
   }
   return e_Retval;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Convert node data pool content type to string

   \param[in] ore_Type Node data pool content type

   \return
   String representing the current type

   \created     21.02.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
QString C_SdNdeDataPoolUtil::h_ConvertContentTypeToString(const C_OSCNodeDataPoolContent::E_Type & ore_Type)
{
   QString c_Retval;

   switch (ore_Type)
   {
   case C_OSCNodeDataPoolContent::E_Type::eUINT8:
      c_Retval = "uint8";
      break;
   case C_OSCNodeDataPoolContent::E_Type::eUINT16:
      c_Retval = "uint16";
      break;
   case C_OSCNodeDataPoolContent::E_Type::eUINT32:
      c_Retval = "uint32";
      break;
   case C_OSCNodeDataPoolContent::E_Type::eUINT64:
      c_Retval = "uint64";
      break;
   case C_OSCNodeDataPoolContent::E_Type::eSINT8:
      c_Retval = "sint8";
      break;
   case C_OSCNodeDataPoolContent::E_Type::eSINT16:
      c_Retval = "sint16";
      break;
   case C_OSCNodeDataPoolContent::E_Type::eSINT32:
      c_Retval = "sint32";
      break;
   case C_OSCNodeDataPoolContent::E_Type::eSINT64:
      c_Retval = "sint64";
      break;
   case C_OSCNodeDataPoolContent::E_Type::eFLOAT32:
      c_Retval = "float32";
      break;
   case C_OSCNodeDataPoolContent::E_Type::eFLOAT64:
      c_Retval = "float64";
      break;
   default:
      c_Retval = "";
      break;
   }

   return c_Retval;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Convert node data pool list element access type to string

   \param[in] ore_Type Node data pool list element access type

   \return
   String representing the current type

   \created     21.02.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
QString C_SdNdeDataPoolUtil::h_ConvertElementAccessToString(const C_OSCNodeDataPoolListElement::E_Access & ore_Type)
{
   QString c_Retval;

   switch (ore_Type)
   {
   case C_OSCNodeDataPoolListElement::E_Access::eACCESS_RO:
      c_Retval = C_GtGetText::h_GetText("RO");
      break;
   case C_OSCNodeDataPoolListElement::E_Access::eACCESS_RW:
      c_Retval = C_GtGetText::h_GetText("RW");
      break;
   default:
      c_Retval = "";
      break;
   }

   return c_Retval;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Draw list element to painter

   \param[in,out] opc_Painter         Painter
   \param[in]     orc_Rectangle       Rectangle
   \param[in]     oru32_NodeIndex     Node index
   \param[in]     oru32_DataPoolIndex Node data pool index
   \param[in]     oru32_ListIndex     Node data pool list index

   \created     14.03.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_SdNdeDataPoolUtil::h_DrawList(QPainter * const opc_Painter, const QRect & orc_Rectangle,
                                     const uint32 & oru32_NodeIndex, const uint32 & oru32_DataPoolIndex,
                                     const uint32 & oru32_ListIndex)
{
   if (opc_Painter != NULL)
   {
      //Element valid
      if (C_PuiSdHandler::h_GetInstance()->GetOSCDataPoolList(oru32_NodeIndex, oru32_DataPoolIndex,
                                                              oru32_ListIndex) != NULL)
      {
         // creating local QWidget (that's why i think it should be fasted, cause we
         // don't touch the heap and don't deal with a QWidget except painting)
         C_SdNdeDataPoolListHeaderWidget c_ItemWidget(NULL, NULL, NULL, NULL, oru32_NodeIndex, oru32_DataPoolIndex,
                                                      oru32_ListIndex);

         // geometry
         c_ItemWidget.setGeometry(orc_Rectangle);

         // rendering of QWidget itself
         // store the configuration of the original painter before translation. The change of the coordinates
         // is only necessary here.
         opc_Painter->save();
         opc_Painter->translate(orc_Rectangle.topLeft());

         // rendering of QWidget itself
         c_ItemWidget.render(opc_Painter,
                             QPoint(0, 0),
                             QRegion(0, 0, orc_Rectangle.width(), orc_Rectangle.height()),
                             QWidget::RenderFlag::DrawChildren);

         // restore the previous coordinates
         opc_Painter->restore();
      }
   }
}

//-----------------------------------------------------------------------------
/*!
   \brief   Create widget to use for editing this value

   \param[in,out] opc_Parent           Parent widget
   \param[in]     orc_Index            Correlating index
   \param[in]     orc_Min              Minimum value(s)
   \param[in]     orc_Max              Maximum value(s)
   \param[in]     of64_Factor          Scaling factor
   \param[in]     of64_Offset          Scaling offset
   \param[in]     oru32_ArrayIndex     Array index to access single element
   \param[in]     oq_UseParamVariant   Optional indicator if param widget variant should be returned
                                        instead of default table one

   \return
   Editor widget

   \created     15.02.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
QWidget * C_SdNdeDataPoolUtil::h_CreateGenericEditor(QWidget * const opc_Parent, const QModelIndex & orc_Index,
                                                     const C_OSCNodeDataPoolContent & orc_Min,
                                                     const C_OSCNodeDataPoolContent & orc_Max,
                                                     const float64 of64_Factor, const float64 of64_Offset,
                                                     const uint32 & oru32_ArrayIndex, const bool oq_UseParamVariant)
{
   QWidget * pc_Retval = NULL;

   if (((orc_Index.isValid() == true) && (orc_Min.GetType() == orc_Max.GetType())) &&
       (orc_Min.GetArray() == orc_Max.GetArray()))
   {
      const QVariant c_Data = orc_Index.data(static_cast<sintn>(Qt::EditRole));
      if (c_Data.type() == QVariant::Type::String)
      {
         QLineEdit * pc_LineEdit;
         if (oq_UseParamVariant == false)
         {
            pc_LineEdit = new C_OgeLeTable(opc_Parent);
         }
         else
         {
            pc_LineEdit = new C_OgeLeParam(opc_Parent);
         }
         //Restrict count
         pc_LineEdit->setMaxLength(static_cast<sintn>(orc_Min.GetArraySize()) - 1);
         pc_Retval = pc_LineEdit;
      }
      else if (c_Data.type() == QVariant::Point)
      {
         //No edit widget allowed
      }
      else
      {
         C_OSCNodeDataPoolContent c_Min;
         C_OSCNodeDataPoolContent c_Max;
         if (C_SdNdeDataPoolContentUtil::h_GetMinMaxAfterScaling(orc_Min, orc_Max, of64_Factor, of64_Offset, c_Min,
                                                                 c_Max, oru32_ArrayIndex) == C_NO_ERR)
         {
            if (c_Min.GetType() == c_Max.GetType())
            {
               C_OgeWiSpinBoxGroup * pc_SpinBoxGroup;
               if (oq_UseParamVariant == false)
               {
                  pc_SpinBoxGroup = new C_OgeWiTableSpinBoxGroup(opc_Parent);
               }
               else
               {
                  pc_SpinBoxGroup = new C_OgeWiParamSpinBoxGroup(opc_Parent);
               }
               pc_SpinBoxGroup->Init(orc_Min, orc_Max, of64_Factor, of64_Offset, oru32_ArrayIndex);
               pc_Retval = pc_SpinBoxGroup;
            }
         }
      }
   }
   return pc_Retval;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Set value to widget used for editing this value

   \param[in,out] opc_Editor Editor widget
   \param[in]     orc_Index  Correlating index

   \created     15.02.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_SdNdeDataPoolUtil::h_SetGenericEditorDataVariable(QWidget * const opc_Editor, const QModelIndex & orc_Index)
{
   //lint -e{929}  false positive in PC-Lint: allowed by MISRA 5-2-2
   C_OgeWiSpinBoxGroup * const pc_SpinBox = dynamic_cast<C_OgeWiSpinBoxGroup * const>(opc_Editor);
   //lint -e{929}  false positive in PC-Lint: allowed by MISRA 5-2-2
   QLineEdit * const pc_LineEdit = dynamic_cast<QLineEdit * const>(opc_Editor);

   if (pc_SpinBox != NULL)
   {
      pc_SpinBox->SetValue(orc_Index.data(static_cast<sintn>(Qt::EditRole)));
      pc_SpinBox->SelectAll();
   }
   if (pc_LineEdit != NULL)
   {
      pc_LineEdit->setText(orc_Index.data(static_cast<sintn>(Qt::EditRole)).toString());
   }
}

//-----------------------------------------------------------------------------
/*!
   \brief   Set model value from widget used to edit this value

   \param[in,out] opc_Editor Editor widget
   \param[in,out] opc_Model  Model object
   \param[in]     orc_Index  Correlating index

   \created     15.02.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_SdNdeDataPoolUtil::h_SetModelGenericDataVariable(QWidget * const opc_Editor,
                                                        QAbstractItemModel * const opc_Model,
                                                        const QModelIndex & orc_Index)
{
   //lint -e{929}  false positive in PC-Lint: allowed by MISRA 5-2-2
   C_OgeWiSpinBoxGroup * const pc_SpinBox = dynamic_cast<C_OgeWiSpinBoxGroup * const>(opc_Editor);
   //lint -e{929}  false positive in PC-Lint: allowed by MISRA 5-2-2
   QLineEdit * const pc_LineEdit = dynamic_cast<QLineEdit * const>(opc_Editor);

   if (pc_SpinBox != NULL)
   {
      pc_SpinBox->InterpretText();
      opc_Model->setData(orc_Index, pc_SpinBox->GetValue());
   }
   if (pc_LineEdit != NULL)
   {
      opc_Model->setData(orc_Index, pc_LineEdit->text());
   }
}

//-----------------------------------------------------------------------------
/*!
   \brief   Convert generic data type to string

   \param[in] orc_Data Generic data type

   \return
   Generic data type string content

   \created     21.03.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
QString C_SdNdeDataPoolUtil::h_ConvertToString(const C_OSCNodeDataPoolContent & orc_Data)
{
   QString c_Retval = "";

   //Each element is an character
   if (orc_Data.GetType() == C_OSCNodeDataPoolContent::eSINT8)
   {
      if (orc_Data.GetArray() == true)
      {
         for (uint32 u32_ItContent = 0; u32_ItContent < orc_Data.GetArraySize(); ++u32_ItContent)
         {
            const charn cn_Char = static_cast<charn>(orc_Data.GetValueAS8Element(u32_ItContent));
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
         c_Retval = QString(static_cast<charn>(orc_Data.GetValueS8()));
      }
   }
   return c_Retval;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Handle QString to OSCContent conversion

   \param[in]     orc_Content QString input
   \param[in,out] orc_Output  OSCContent output

   \created     21.03.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_SdNdeDataPoolUtil::h_SetString(const QString & orc_String, C_OSCNodeDataPoolContent & orc_Output)
{
   if (orc_String.size() <= static_cast<sint32>(orc_Output.GetArraySize()))
   {
      if (orc_Output.GetArraySize() == 1)
      {
         if (orc_String.size() == 1)
         {
            orc_Output.SetValueS8(static_cast<sint8>(orc_String.at(0).toLatin1()));
         }
      }
      else
      {
         for (uint32 u32_ItOutput = 0; u32_ItOutput < orc_Output.GetArraySize(); ++u32_ItOutput)
         {
            //As long as there is a string content use that
            if (static_cast<sintn>(u32_ItOutput) < orc_String.length())
            {
               orc_Output.SetValueAS8Element(static_cast<sint8>(orc_String.at(static_cast<sintn>(u32_ItOutput)).
                                                                toLatin1()), u32_ItOutput);
            }
            else
            {
               //If the string has ended fill up all remaining values with defined content
               orc_Output.SetValueAS8Element(static_cast<sint8>('\0'), u32_ItOutput);
            }
         }
      }
   }
}

//-----------------------------------------------------------------------------
/*!
   \brief   Convert sint8 array to string

   \param[in] orc_Data     sint8 array

   \return
   String content

   \created     21.03.2017  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
QString C_SdNdeDataPoolUtil::h_ConvertToString(const std::vector<sint8> & orc_Data)
{
   QString c_Retval = "";

   //Each element is an character
   for (uint32 u32_ItContent = 0; u32_ItContent < orc_Data.size(); ++u32_ItContent)
   {
      const charn cn_Char = static_cast<charn>(orc_Data[u32_ItContent]);
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

//-----------------------------------------------------------------------------
/*!
   \brief   Convert node data pool content type to simplified name

   \param[in] ore_Type Node data pool content type

   \return
   Simplified name

   \created     04.05.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
QString C_SdNdeDataPoolUtil::h_ConvertTypeToNameSimplified(const C_OSCNodeDataPoolContent::E_Type & ore_Type)
{
   QString c_Retval;

   switch (ore_Type)
   {
   case C_OSCNodeDataPoolContent::eUINT8:
   case C_OSCNodeDataPoolContent::eUINT16:
   case C_OSCNodeDataPoolContent::eUINT32:
   case C_OSCNodeDataPoolContent::eUINT64:
      c_Retval = C_GtGetText::h_GetText("unsigned");
      break;
   case C_OSCNodeDataPoolContent::eSINT8:
   case C_OSCNodeDataPoolContent::eSINT16:
   case C_OSCNodeDataPoolContent::eSINT32:
   case C_OSCNodeDataPoolContent::eSINT64:
      c_Retval = C_GtGetText::h_GetText("signed");
      break;
   case C_OSCNodeDataPoolContent::eFLOAT32:
      c_Retval = C_GtGetText::h_GetText("IEEE float (32)");
      break;
   case C_OSCNodeDataPoolContent::eFLOAT64:
      c_Retval = C_GtGetText::h_GetText("IEEE float (64)");
      break;
   default:
      c_Retval = "";
      break;
   }
   return c_Retval;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Paint table boolean item

   \param[in,out] opc_Painter                   Painter
   \param[in]     orc_Option                    Painter option
   \param[in]     orc_Index                     Current index (for value)
   \param[in]     orc_CheckMark                 Check mark pixmap (performance reasons)
   \param[in]     orq_IsEditing                 Optional flag if boolean in edit mode

   \created     05.05.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_SdNdeDataPoolUtil::h_DrawTableBoolean(QPainter * const opc_Painter, const QStyleOptionViewItem & orc_Option,
                                             const QModelIndex & orc_Index, const QPixmap & orc_CheckMark,
                                             const bool & orq_IsEditing)
{
   //Adjust rectangle because there is an border in edit mode (top & bottom)
   const sintn sn_TopCenterX = orc_Option.rect.topLeft().x() + (orc_Option.rect.width() / 2);
   const QRect c_AdaptedRect =
      orc_Option.rect.adjusted((orc_Option.rect.width() / 2) - 20, 1, -(orc_Option.rect.width() / 2) + 20, -1);
   QPoint c_TopLeft = QPoint(sn_TopCenterX - 20, orc_Option.rect.topLeft().y());
   QPoint c_TopRight = QPoint(sn_TopCenterX + 20, orc_Option.rect.topLeft().y());
   QPoint c_BottomLeft = QPoint(sn_TopCenterX - 20, orc_Option.rect.bottomLeft().y());
   QPoint c_BottomRight = QPoint(sn_TopCenterX + 20, orc_Option.rect.bottomLeft().y());

   if (orc_Option.state.testFlag(QStyle::State_MouseOver) == true)
   {
      c_TopLeft.setY(c_TopLeft.y() + 1);
      c_TopRight.setY(c_TopRight.y() + 1);
      c_BottomLeft.setY(c_BottomLeft.y() - 1);
      c_BottomRight.setY(c_BottomRight.y() - 1);
   }

   //Paint checkbox (If not edit mode)
   if (orq_IsEditing == false)
   {
      if (orc_Index.data(static_cast<sintn>(Qt::EditRole)).toBool() == true)
      {
         const QPixmap c_ScaledCheckBoxPixmap = orc_CheckMark.scaled(
            c_AdaptedRect.size(), Qt::KeepAspectRatio, Qt::SmoothTransformation);

         //Checked symbol
         opc_Painter->drawPixmap(sn_TopCenterX - (c_ScaledCheckBoxPixmap.width() / 2),
                                 c_AdaptedRect.topLeft().y(),
                                 c_ScaledCheckBoxPixmap.width(),
                                 c_ScaledCheckBoxPixmap.height(), c_ScaledCheckBoxPixmap);
      }
   }
   //White lines
   opc_Painter->save();
   opc_Painter->setPen(mc_STYLE_GUIDE_COLOR_0);
   opc_Painter->drawLine(c_BottomLeft, c_TopLeft);
   opc_Painter->drawLine(c_BottomRight, c_TopRight);
   opc_Painter->restore();
}

//-----------------------------------------------------------------------------
/*!
   \brief   Paint table item background

   \param[in,out] opc_Painter                   Painter
   \param[in]     orc_Option                    Option
   \param[in]     orq_UseSpecialBackgroundColor Optional flag to indicate if the field uses a special background color

   \created     05.05.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_SdNdeDataPoolUtil::h_DrawTableBackground(QPainter * const opc_Painter, const QStyleOptionViewItem & orc_Option,
                                                const bool & orq_UseSpecialBackgroundColor)
{
   if (orq_UseSpecialBackgroundColor == true)
   {
      C_SdNdeDataPoolUtil::h_DrawTableBackgroundGeneric(opc_Painter, orc_Option, mc_STYLE_GUIDE_COLOR_11,
                                                        mc_STYLE_GUIDE_COLOR_0, mc_STYLE_GUIDE_COLOR_10);
   }
   else
   {
      C_SdNdeDataPoolUtil::h_DrawTableBackgroundGeneric(opc_Painter, orc_Option, mc_STYLE_GUIDE_COLOR_11,
                                                        mc_STYLE_GUIDE_COLOR_12, mc_STYLE_GUIDE_COLOR_10);
   }
}

//-----------------------------------------------------------------------------
/*!
   \brief   Paint table item background

   \param[in,out] opc_Painter   Painter
   \param[in]     orc_Option    Option
   \param[in]     orc_Default   Default color
   \param[in]     orc_Alternate Alternating color
   \param[in]     orc_Selected  Selection color

   \created     30.08.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_SdNdeDataPoolUtil::h_DrawTableBackgroundGeneric(QPainter * const opc_Painter,
                                                       const QStyleOptionViewItem & orc_Option,
                                                       const QColor & orc_Default, const QColor & orc_Alternate,
                                                       const QColor & orc_Selected)
{
   opc_Painter->save();
   //Draw background (Keep alternating & selected in mind)
   //Done manually because parent paint always draws text "true" or "false" because of model index content
   opc_Painter->setPen(Qt::NoPen);
   if (orc_Option.state.testFlag(QStyle::State_Selected) == true)
   {
      opc_Painter->setBrush(orc_Selected);
   }
   else
   {
      if (orc_Option.features.testFlag(QStyleOptionViewItem::ViewItemFeature::Alternate) == true)
      {
         opc_Painter->setBrush(orc_Alternate);
      }
      else
      {
         opc_Painter->setBrush(orc_Default);
      }
   }
   opc_Painter->drawRect(orc_Option.rect);
   opc_Painter->restore();
}

//-----------------------------------------------------------------------------
/*!
   \brief   Draw selected table cell

   \param[in,out] opc_Painter    Active painter
   \param[in]     orc_Option     Active option(s)
   \param[in]     orc_Index      Active index
   \param[in]     opc_TableModel Active table model

   \created     07.06.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_SdNdeDataPoolUtil::h_DrawTableSelected(QPainter * const opc_Painter, const QStyleOptionViewItem & orc_Option,
                                              const QModelIndex & orc_Index,
                                              const QAbstractTableModel * const opc_TableModel)
{
   if (opc_Painter != NULL)
   {
      //Draw background
      C_SdNdeDataPoolUtil::h_DrawTableBackground(opc_Painter, orc_Option, false);
      if (opc_TableModel != NULL)
      {
         QTextOption c_Option;
         //Somehow these values seem to fit
         QRect c_Target = orc_Option.rect.adjusted(4, 1, -4, -1);
         const QFont c_Font =
            qvariant_cast<QFont>(opc_TableModel->data(orc_Index, static_cast<sintn>(Qt::FontRole)));
         QColor c_Color =
            qvariant_cast<QColor>(opc_TableModel->data(orc_Index, static_cast<sintn>(Qt::ForegroundRole)));
         c_Option.setAlignment(
            Qt::Alignment(opc_TableModel->data(orc_Index, static_cast<sintn>(Qt::TextAlignmentRole)).toInt()));
         QString c_Text = opc_TableModel->data(orc_Index, static_cast<sintn>(Qt::DisplayRole)).toString();
         const QFontMetrics c_FontMetrics(c_Font);
         //Configure text length
         c_Text = C_Uti::h_AdaptStringToSize(c_Text, c_FontMetrics,
                                             static_cast<float64>(c_Target.width()));
         //Special color handling
         if (c_Color == mc_STYLE_GUIDE_COLOR_10)
         {
            c_Color = mc_STYLE_GUIDE_COLOR_11;
         }
         //Style label
         //Draw foreground
         opc_Painter->save();
         opc_Painter->setPen(QPen(c_Color));
         opc_Painter->setFont(c_Font);
         opc_Painter->drawText(c_Target, c_Text, c_Option);
         opc_Painter->restore();
      }
   }
}

//-----------------------------------------------------------------------------
/*!
   \brief   Utility function to compare one item of two content types

   \param[in] orc_Content1 Content 1
   \param[in] orc_Content2 Content 2
   \param[in] oru32_Index  Specific index

   \return
   True  Content1 smaller than Content2
   False Content1 greater or equal to Content2

   \created     04.05.2018  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
bool C_SdNdeDataPoolUtil::h_CompareSpecifiedItemSmaller(const C_OSCNodeDataPoolContent & orc_Content1,
                                                        const C_OSCNodeDataPoolContent & orc_Content2,
                                                        const uint32 & oru32_Index)
{
   bool q_Retval;
   float64 f64_Value1;
   float64 f64_Value2;

   orc_Content1.GetAnyValueAsFloat64(f64_Value1, oru32_Index);
   orc_Content2.GetAnyValueAsFloat64(f64_Value2, oru32_Index);
   if (f64_Value1 < f64_Value2)
   {
      q_Retval = true;
   }
   else
   {
      q_Retval = false;
   }
   return q_Retval;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Central function to calculate table widget height

   \param[in] ou32_NodeIndex     Node index
   \param[in] ou32_DataPoolIndex Datapool index
   \param[in] ou32_ListIndex     List index

   \return
   Table widget height

   \created     06.11.2018  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
sint32 C_SdNdeDataPoolUtil::h_GetTableSize(const uint32 ou32_NodeIndex, const uint32 ou32_DataPoolIndex,
                                           const uint32 ou32_ListIndex)
{
   sint32 s32_Retval;
   const C_OSCNodeDataPool * const pc_Datapool = C_PuiSdHandler::h_GetInstance()->GetOSCDataPool(ou32_NodeIndex,
                                                                                                 ou32_DataPoolIndex);
   const C_OSCNodeDataPoolList * const pc_List = C_PuiSdHandler::h_GetInstance()->GetOSCDataPoolList(ou32_NodeIndex,
                                                                                                     ou32_DataPoolIndex,
                                                                                                     ou32_ListIndex);

   if ((pc_List != NULL) && (pc_Datapool != NULL))
   {
      //Configure
      const sintn sn_ConstOffset = 73;
      const sintn sn_ConstChecksumOffset = 49;
      const sintn sn_ItemOffset = 40;
      const uint32 u32_VisibleItemCount = 7;
      const uint32 u32_ItemCount = pc_List->c_Elements.size();
      sintn sn_Height = sn_ConstOffset;

      //Adapt dynamic elements
      if (pc_Datapool->e_Type == C_OSCNodeDataPool::eNVM)
      {
         sn_Height += sn_ConstChecksumOffset;
      }
      if ((u32_ItemCount <= u32_VisibleItemCount) ||
          (static_cast<uint32>(pc_Datapool->c_Lists.size() - 1) == ou32_ListIndex))
      {
         sn_Height += sn_ItemOffset * static_cast<sintn>(u32_ItemCount);
      }
      else
      {
         sn_Height += sn_ItemOffset * static_cast<sintn>(u32_VisibleItemCount);
      }
      s32_Retval = sn_Height;
   }
   else
   {
      //Fallback, should not happen
      s32_Retval = 66;
   }
   return s32_Retval;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Default constructor

   \created     27.02.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
C_SdNdeDataPoolUtil::C_SdNdeDataPoolUtil(void)
{
}

//-----------------------------------------------------------------------------
/*!
   \brief   Apply style

   \param[in,out] opc_Widget Widget
   \param[in]     orc_Font   Font (Only supports pixel size!)
   \param[in]     orc_Color  Foreground color

   \created     06.06.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_SdNdeDataPoolUtil::mh_ApplyStyleSheet(QWidget * const opc_Widget, const QFont & orc_Font,
                                             const QColor & orc_Color)
{
   if (opc_Widget != NULL)
   {
      const QString c_ColorPart = QString("color:rgba(%1,%2,%3,%4);").arg(orc_Color.red()).arg(orc_Color.green()).arg(
         orc_Color.blue()).arg(orc_Color.alpha());
      QString c_Font = "";
      //Font
      c_Font += QString("font-family:\"%1\";").arg(orc_Font.family());
      c_Font += QString("font-size: %1px;").arg(orc_Font.pixelSize());
      if (orc_Font.weight() == static_cast<sintn>(QFont::Bold))
      {
         c_Font += "font-weight:bold;";
      }
      else
      {
         c_Font += "font-weight:normal;";
      }
      if (orc_Font.underline() == true)
      {
         c_Font += "text-decoration: underline;";
      }

      //Apply
      opc_Widget->setStyleSheet(QString("QLabel{%1%2}").arg(c_Font, c_ColorPart));
   }
}
