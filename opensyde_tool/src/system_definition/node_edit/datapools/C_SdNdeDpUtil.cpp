//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Node data pool util class (implementation)

   Node data pool util class

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.hpp"

#include <cmath>
#include <limits>
#include <QApplication>
#include "stwerrors.hpp"
#include "C_OscUtils.hpp"
#include "C_SdNdeDpUtil.hpp"
#include "C_SdUtil.hpp"
#include "C_Uti.hpp"
#include "constants.hpp"
#include "C_PuiSdHandler.hpp"
#include "C_OgeWiTableSpinBoxGroup.hpp"
#include "C_OgeWiParamSpinBoxGroup.hpp"
#include "C_OgeLeTable.hpp"
#include "C_OgeLeParam.hpp"
#include "C_GtGetText.hpp"
#include "C_SdNdeDpContentUtil.hpp"
#include "C_PuiSdSharedDatapools.hpp"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw::errors;
using namespace stw::opensyde_gui_logic;
using namespace stw::opensyde_gui;
using namespace stw::opensyde_gui_elements;
using namespace stw::opensyde_core;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

/* -- Global Variables ---------------------------------------------------------------------------------------------- */

/* -- Module Global Variables --------------------------------------------------------------------------------------- */

/* -- Module Global Function Prototypes ----------------------------------------------------------------------------- */

/* -- Implementation ------------------------------------------------------------------------------------------------ */

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Convert item selection to std vector

   \param[in]  orc_Items   Item selection
   \param[in]  orq_Rows    Optional indicator if selection should be on rows or columns

   \return
   Std vector of selected items
*/
//----------------------------------------------------------------------------------------------------------------------
std::vector<uint32_t> C_SdNdeDpUtil::h_ConvertVector(const QModelIndexList & orc_Items, const bool & orq_Rows)
{
   std::vector<uint32_t> c_Retval;
   c_Retval.reserve(orc_Items.size());
   for (uint32_t u32_ItInput = 0; u32_ItInput < static_cast<uint32_t>(orc_Items.size()); ++u32_ItInput)
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

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Convert element type to generic

   \param[in]   orc_OscElement      OSC Element
   \param[in]   orc_UiElement       UI Element
   \param[in]   ore_Type            Type to convert
   \param[out]  orc_Generic         Generic output
   \param[in]   oru32_Index         Optional array index
   \param[in]   ors32_DataSetIndex  Optional data set index
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDpUtil::h_ConvertToElementGeneric(const stw::opensyde_core::C_OscNodeDataPoolListElement & orc_OscElement,
                                              const C_PuiSdNodeDataPoolListElement & orc_UiElement,
                                              const C_SdNdeDpUtil::E_ElementDataChangeType & ore_Type,
                                              QVariant & orc_Generic, const uint32_t & oru32_Index,
                                              const int32_t & ors32_DataSetIndex)
{
   QString c_Precison;

   switch (ore_Type)
   {
   case eELEMENT_NAME:
      orc_Generic = orc_OscElement.c_Name.c_str();
      break;
   case eELEMENT_COMMENT:
      orc_Generic = orc_OscElement.c_Comment.c_str();
      break;
   case eELEMENT_VALUE_TYPE:
      if (orc_UiElement.q_InterpretAsString == true)
      {
         orc_Generic = 10;
      }
      else
      {
         orc_Generic = h_ConvertContentTypeToComboBox(orc_OscElement.c_Value.GetType());
      }
      break;
   case eELEMENT_ARRAY:
      if (orc_OscElement.GetArray() == false)
      {
         orc_Generic = 1;
      }
      else
      {
         orc_Generic = static_cast<int32_t>(orc_OscElement.GetArraySize());
      }
      break;
   case eELEMENT_AUTO_MIN_MAX:
      orc_Generic = orc_UiElement.q_AutoMinMaxActive;
      break;
   case eELEMENT_MIN:
      orc_Generic = C_SdNdeDpContentUtil::h_ConvertContentToGeneric(orc_OscElement.c_MinValue, oru32_Index);
      break;
   case eELEMENT_MAX:
      orc_Generic = C_SdNdeDpContentUtil::h_ConvertContentToGeneric(orc_OscElement.c_MaxValue, oru32_Index);
      break;
   case eELEMENT_FACTOR:
      c_Precison = QString::number(orc_OscElement.f64_Factor, 'g', 17);
      c_Precison.replace(QLocale::c().decimalPoint(), QLocale::system().decimalPoint(),
                         Qt::CaseInsensitive);
      orc_Generic = c_Precison;
      break;
   case eELEMENT_OFFSET:
      c_Precison = QString::number(orc_OscElement.f64_Offset, 'g', 17);
      c_Precison.replace(QLocale::c().decimalPoint(), QLocale::system().decimalPoint(),
                         Qt::CaseInsensitive);
      orc_Generic = c_Precison;
      break;
   case eELEMENT_UNIT:
      orc_Generic = orc_OscElement.c_Unit.c_str();
      break;
   case eELEMENT_DATA_SET:
      if ((ors32_DataSetIndex >= 0) &&
          (static_cast<uint32_t>(ors32_DataSetIndex) < orc_OscElement.c_DataSetValues.size()))
      {
         if (orc_UiElement.q_InterpretAsString == true)
         {
            orc_Generic =
               C_SdNdeDpContentUtil::h_ConvertToString(
                  orc_OscElement.c_DataSetValues[static_cast<uint32_t>(ors32_DataSetIndex)]);
         }
         else
         {
            orc_Generic =
               C_SdNdeDpContentUtil::h_ConvertContentToGeneric(orc_OscElement.c_DataSetValues[static_cast<uint32_t>(
                                                                                                 ors32_DataSetIndex)],
                                                               oru32_Index);
         }
      }
      break;
   case eELEMENT_ACCESS:
      orc_Generic = h_ConvertElementAccessToComboBox(orc_OscElement.e_Access);
      break;
   case eELEMENT_EVENT_CALL:
      orc_Generic = orc_OscElement.q_DiagEventCall;
      break;
   default:
      break;
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Convert node data pool content type to combo box index

   \param[in]  ore_Type    Node data pool content type

   \return
   Combo box index
*/
//----------------------------------------------------------------------------------------------------------------------
int8_t C_SdNdeDpUtil::h_ConvertContentTypeToComboBox(const C_OscNodeDataPoolContent::E_Type & ore_Type)
{
   int8_t s8_Retval;

   switch (ore_Type)
   {
   case C_OscNodeDataPoolContent::E_Type::eUINT8:
      s8_Retval = 0;
      break;
   case C_OscNodeDataPoolContent::E_Type::eUINT16:
      s8_Retval = 2;
      break;
   case C_OscNodeDataPoolContent::E_Type::eUINT32:
      s8_Retval = 4;
      break;
   case C_OscNodeDataPoolContent::E_Type::eUINT64:
      s8_Retval = 6;
      break;
   case C_OscNodeDataPoolContent::E_Type::eSINT8:
      s8_Retval = 1;
      break;
   case C_OscNodeDataPoolContent::E_Type::eSINT16:
      s8_Retval = 3;
      break;
   case C_OscNodeDataPoolContent::E_Type::eSINT32:
      s8_Retval = 5;
      break;
   case C_OscNodeDataPoolContent::E_Type::eSINT64:
      s8_Retval = 7;
      break;
   case C_OscNodeDataPoolContent::E_Type::eFLOAT32:
      s8_Retval = 8;
      break;
   case C_OscNodeDataPoolContent::E_Type::eFLOAT64:
      s8_Retval = 9;
      break;
   default:
      s8_Retval = -1;
      break;
   }

   return s8_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Convert combo box index to node data pool content type

   \param[in]  ors8_Type   Combo box index

   \return
   Node data pool content type
*/
//----------------------------------------------------------------------------------------------------------------------
C_OscNodeDataPoolContent::E_Type C_SdNdeDpUtil::h_ConvertContentTypeFromComboBox(const int8_t & ors8_Type)
{
   C_OscNodeDataPoolContent::E_Type e_Retval;
   switch (ors8_Type)
   {
   case 0:
      e_Retval = C_OscNodeDataPoolContent::E_Type::eUINT8;
      break;
   case 2:
      e_Retval = C_OscNodeDataPoolContent::E_Type::eUINT16;
      break;
   case 4:
      e_Retval = C_OscNodeDataPoolContent::E_Type::eUINT32;
      break;
   case 6:
      e_Retval = C_OscNodeDataPoolContent::E_Type::eUINT64;
      break;
   case 1:
      e_Retval = C_OscNodeDataPoolContent::E_Type::eSINT8;
      break;
   case 3:
      e_Retval = C_OscNodeDataPoolContent::E_Type::eSINT16;
      break;
   case 5:
      e_Retval = C_OscNodeDataPoolContent::E_Type::eSINT32;
      break;
   case 7:
      e_Retval = C_OscNodeDataPoolContent::E_Type::eSINT64;
      break;
   case 8:
      e_Retval = C_OscNodeDataPoolContent::E_Type::eFLOAT32;
      break;
   case 9:
      e_Retval = C_OscNodeDataPoolContent::E_Type::eFLOAT64;
      break;
   default:
      e_Retval = C_OscNodeDataPoolContent::E_Type::eSINT8;
      break;
   }
   return e_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Convert node data pool list element access type to combo box index

   \param[in]  ore_Type    Node data pool list element access type

   \return
   Combo box index
*/
//----------------------------------------------------------------------------------------------------------------------
int8_t C_SdNdeDpUtil::h_ConvertElementAccessToComboBox(const C_OscNodeDataPoolListElement::E_Access & ore_Type)
{
   int8_t s8_Retval;

   switch (ore_Type)
   {
   case C_OscNodeDataPoolListElement::E_Access::eACCESS_RW:
      s8_Retval = 0;
      break;
   case C_OscNodeDataPoolListElement::E_Access::eACCESS_RO:
      s8_Retval = 1;
      break;
   default:
      s8_Retval = -1;
      break;
   }

   return s8_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Convert combo box index to node data pool list element access type

   \param[in]  ors8_Type   Combo box index

   \return
   Node data pool list element access type
*/
//----------------------------------------------------------------------------------------------------------------------
C_OscNodeDataPoolListElement::E_Access C_SdNdeDpUtil::h_ConvertElementAccesFromComboBox(const int8_t & ors8_Type)
{
   C_OscNodeDataPoolListElement::E_Access e_Retval;
   switch (ors8_Type)
   {
   case 0:
      e_Retval = C_OscNodeDataPoolListElement::E_Access::eACCESS_RW;
      break;
   case 1:
      e_Retval = C_OscNodeDataPoolListElement::E_Access::eACCESS_RO;
      break;
   default:
      e_Retval = C_OscNodeDataPoolListElement::E_Access::eACCESS_RO;
      break;
   }
   return e_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Convert node data pool content type to string

   \param[in]  ore_Type    Node data pool content type

   \return
   String representing the current type
*/
//----------------------------------------------------------------------------------------------------------------------
QString C_SdNdeDpUtil::h_ConvertContentTypeToString(const C_OscNodeDataPoolContent::E_Type & ore_Type)
{
   QString c_Retval;

   switch (ore_Type)
   {
   case C_OscNodeDataPoolContent::E_Type::eUINT8:
      c_Retval = "uint8";
      break;
   case C_OscNodeDataPoolContent::E_Type::eUINT16:
      c_Retval = "uint16";
      break;
   case C_OscNodeDataPoolContent::E_Type::eUINT32:
      c_Retval = "uint32";
      break;
   case C_OscNodeDataPoolContent::E_Type::eUINT64:
      c_Retval = "uint64";
      break;
   case C_OscNodeDataPoolContent::E_Type::eSINT8:
      c_Retval = "sint8";
      break;
   case C_OscNodeDataPoolContent::E_Type::eSINT16:
      c_Retval = "sint16";
      break;
   case C_OscNodeDataPoolContent::E_Type::eSINT32:
      c_Retval = "sint32";
      break;
   case C_OscNodeDataPoolContent::E_Type::eSINT64:
      c_Retval = "sint64";
      break;
   case C_OscNodeDataPoolContent::E_Type::eFLOAT32:
      c_Retval = "float32";
      break;
   case C_OscNodeDataPoolContent::E_Type::eFLOAT64:
      c_Retval = "float64";
      break;
   default:
      c_Retval = "";
      break;
   }

   return c_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Convert node data pool list element access type to string

   \param[in]  ore_Type    Node data pool list element access type

   \return
   String representing the current type
*/
//----------------------------------------------------------------------------------------------------------------------
QString C_SdNdeDpUtil::h_ConvertElementAccessToString(const C_OscNodeDataPoolListElement::E_Access & ore_Type)
{
   QString c_Retval;

   switch (ore_Type)
   {
   case C_OscNodeDataPoolListElement::E_Access::eACCESS_RO:
      c_Retval = C_GtGetText::h_GetText("RO");
      break;
   case C_OscNodeDataPoolListElement::E_Access::eACCESS_RW:
      c_Retval = C_GtGetText::h_GetText("RW");
      break;
   default:
      c_Retval = "";
      break;
   }

   return c_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Create widget to use for editing this value

   \param[in,out]  opc_Parent          Parent widget
   \param[in]      orc_Index           Correlating index
   \param[in]      orc_Min             Minimum value(s)
   \param[in]      orc_Max             Maximum value(s)
   \param[in]      of64_Factor         Scaling factor
   \param[in]      of64_Offset         Scaling offset
   \param[in]      oru32_ArrayIndex    Array index to access single element
   \param[in]      oq_UseParamVariant  Optional indicator if param widget variant should be returned
                                       instead of default table one

   \return
   Editor widget
*/
//----------------------------------------------------------------------------------------------------------------------
QWidget * C_SdNdeDpUtil::h_CreateGenericEditor(QWidget * const opc_Parent, const QModelIndex & orc_Index,
                                               const C_OscNodeDataPoolContent & orc_Min,
                                               const C_OscNodeDataPoolContent & orc_Max, const float64_t of64_Factor,
                                               const float64_t of64_Offset, const uint32_t & oru32_ArrayIndex,
                                               const bool oq_UseParamVariant)
{
   QWidget * pc_Retval = NULL;

   if (((orc_Index.isValid() == true) && (orc_Min.GetType() == orc_Max.GetType())) &&
       (orc_Min.GetArray() == orc_Max.GetArray()))
   {
      const QVariant c_Data = orc_Index.data(static_cast<int32_t>(Qt::EditRole));
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
         pc_LineEdit->setMaxLength(static_cast<int32_t>(orc_Min.GetArraySize()) - 1);
         pc_Retval = pc_LineEdit;
      }
      else if (c_Data.type() == QVariant::Point)
      {
         //No edit widget allowed
      }
      else
      {
         C_OscNodeDataPoolContent c_Min;
         C_OscNodeDataPoolContent c_Max;
         if (C_SdNdeDpContentUtil::h_GetMinMaxAfterScaling(orc_Min, orc_Max, of64_Factor, of64_Offset, c_Min,
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

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set value to widget used for editing this value

   \param[in,out]  opc_Editor    Editor widget
   \param[in]      orc_Index     Correlating index
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDpUtil::h_SetGenericEditorDataVariable(QWidget * const opc_Editor, const QModelIndex & orc_Index)
{
   C_OgeWiSpinBoxGroup * const pc_SpinBox = dynamic_cast<C_OgeWiSpinBoxGroup * const>(opc_Editor);

   QLineEdit * const pc_LineEdit = dynamic_cast<QLineEdit * const>(opc_Editor);

   if (pc_SpinBox != NULL)
   {
      pc_SpinBox->SetValue(orc_Index.data(static_cast<int32_t>(Qt::EditRole)));
      pc_SpinBox->SelectAll();
   }
   if (pc_LineEdit != NULL)
   {
      pc_LineEdit->setText(orc_Index.data(static_cast<int32_t>(Qt::EditRole)).toString());
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set model value from widget used to edit this value

   \param[in,out]  opc_Editor    Editor widget
   \param[in,out]  opc_Model     Model object
   \param[in]      orc_Index     Correlating index
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDpUtil::h_SetModelGenericDataVariable(QWidget * const opc_Editor, QAbstractItemModel * const opc_Model,
                                                  const QModelIndex & orc_Index)
{
   C_OgeWiSpinBoxGroup * const pc_SpinBox = dynamic_cast<C_OgeWiSpinBoxGroup * const>(opc_Editor);

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

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Paint table boolean item

   \param[in,out]  opc_Painter      Painter
   \param[in]      orc_Option       Painter option
   \param[in]      orc_Index        Current index (for value)
   \param[in]      orc_CheckMark    Check mark pixmap (performance reasons)
   \param[in]      orq_IsEditing    Optional flag if boolean in edit mode
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDpUtil::h_DrawTableBoolean(QPainter * const opc_Painter, const QStyleOptionViewItem & orc_Option,
                                       const QModelIndex & orc_Index, const QPixmap & orc_CheckMark,
                                       const bool & orq_IsEditing)
{
   //Adjust rectangle because there is an border in edit mode (top & bottom)
   const int32_t s32_TOP_CENTER_X = orc_Option.rect.topLeft().x() + (orc_Option.rect.width() / 2);
   const QRect c_AdaptedRect =
      orc_Option.rect.adjusted((orc_Option.rect.width() / 2) - 20, 1, -(orc_Option.rect.width() / 2) + 20, -1);
   QPoint c_TopLeft = QPoint(s32_TOP_CENTER_X - 20, orc_Option.rect.topLeft().y());
   QPoint c_TopRight = QPoint(s32_TOP_CENTER_X + 20, orc_Option.rect.topLeft().y());
   QPoint c_BottomLeft = QPoint(s32_TOP_CENTER_X - 20, orc_Option.rect.bottomLeft().y());
   QPoint c_BottomRight = QPoint(s32_TOP_CENTER_X + 20, orc_Option.rect.bottomLeft().y());

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
      if (orc_Index.data(static_cast<int32_t>(Qt::EditRole)).toBool() == true)
      {
         const QPixmap c_ScaledCheckBoxPixmap = orc_CheckMark.scaled(
            c_AdaptedRect.size(), Qt::KeepAspectRatio, Qt::SmoothTransformation);

         //Checked symbol
         opc_Painter->drawPixmap(s32_TOP_CENTER_X - (c_ScaledCheckBoxPixmap.width() / 2),
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

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Paint table item background

   \param[in,out]  opc_Painter                     Painter
   \param[in]      orc_Option                      Option
   \param[in]      orq_UseSpecialBackgroundColor   Optional flag to indicate if the field uses a special background color
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDpUtil::h_DrawTableBackground(QPainter * const opc_Painter, const QStyleOptionViewItem & orc_Option,
                                          const bool & orq_UseSpecialBackgroundColor)
{
   if (orq_UseSpecialBackgroundColor == true)
   {
      C_SdNdeDpUtil::h_DrawTableBackgroundGeneric(opc_Painter, orc_Option, mc_STYLE_GUIDE_COLOR_11,
                                                  mc_STYLE_GUIDE_COLOR_0, mc_STYLE_GUIDE_COLOR_10);
   }
   else
   {
      C_SdNdeDpUtil::h_DrawTableBackgroundGeneric(opc_Painter, orc_Option, mc_STYLE_GUIDE_COLOR_11,
                                                  mc_STYLE_GUIDE_COLOR_12, mc_STYLE_GUIDE_COLOR_10);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Paint table item background

   \param[in,out]  opc_Painter      Painter
   \param[in]      orc_Option       Option
   \param[in]      orc_Default      Default color
   \param[in]      orc_Alternate    Alternating color
   \param[in]      orc_Selected     Selection color
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDpUtil::h_DrawTableBackgroundGeneric(QPainter * const opc_Painter, const QStyleOptionViewItem & orc_Option,
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

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Utility function to compare one item of two content types

   \param[in]  orc_Content1   Content 1
   \param[in]  orc_Content2   Content 2
   \param[in]  oru32_Index    Specific index

   \return
   True  Content1 smaller than Content2
   False Content1 greater or equal to Content2
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_SdNdeDpUtil::h_CompareSpecifiedItemSmaller(const C_OscNodeDataPoolContent & orc_Content1,
                                                  const C_OscNodeDataPoolContent & orc_Content2,
                                                  const uint32_t & oru32_Index)
{
   bool q_Retval;
   float64_t f64_Value1;
   float64_t f64_Value2;

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

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Central function to calculate table widget height

   \param[in]  ou32_NodeIndex       Node index
   \param[in]  ou32_DataPoolIndex   Datapool index
   \param[in]  ou32_ListIndex       List index
   \param[in]  os32_MaximumHeight    Check for maximum height in case of the last table/list
                                    Default parameter is 0, in this case this check will not be run

   \return
   Table widget height
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_SdNdeDpUtil::h_GetTableSize(const uint32_t ou32_NodeIndex, const uint32_t ou32_DataPoolIndex,
                                      const uint32_t ou32_ListIndex, const int32_t os32_MaximumHeight)
{
   int32_t s32_Retval;
   const C_OscNodeDataPool * const pc_Datapool = C_PuiSdHandler::h_GetInstance()->GetOscDataPool(ou32_NodeIndex,
                                                                                                 ou32_DataPoolIndex);
   const C_OscNodeDataPoolList * const pc_List = C_PuiSdHandler::h_GetInstance()->GetOscDataPoolList(ou32_NodeIndex,
                                                                                                     ou32_DataPoolIndex,
                                                                                                     ou32_ListIndex);
   const int32_t s32_HEADER_OFFSET = 40;

   if ((pc_List != NULL) && (pc_Datapool != NULL))
   {
      const int32_t s32_CONST_OFFSET = 65;
      const int32_t s32_CONST_CHECKSUM_OFFSET = 33;
      const int32_t s32_ITEM_OFFSET = 30;
      const uint32_t u32_VISIBLE_ITEM_COUNT = 7U;
      const uint32_t u32_ITEM_COUNT = pc_List->c_Elements.size();
      int32_t s32_Height = s32_CONST_OFFSET;

      //Add checksum checkbox offset for NVM Datapool lists
      if (pc_Datapool->e_Type == C_OscNodeDataPool::eNVM)
      {
         s32_Height += s32_CONST_CHECKSUM_OFFSET;
      }

      if (u32_ITEM_COUNT <= u32_VISIBLE_ITEM_COUNT)
      {
         // case: number of rows smaller than limit -> show all rows
         s32_Height += s32_ITEM_OFFSET * static_cast<int32_t>(u32_ITEM_COUNT);
      }
      else
      {
         // case: more rows than limit -> show limited number of rows
         s32_Height += s32_ITEM_OFFSET * static_cast<int32_t>(u32_VISIBLE_ITEM_COUNT);
      }

      // special case: last list -> if enough space exists, show as much rows as possible
      if (static_cast<uint32_t>(pc_Datapool->c_Lists.size() - 1) == ou32_ListIndex)
      {
         uint32_t u32_ListCounter;
         int32_t s32_HeightOtherLists = 0;
         int32_t s32_RemainingSpace;

         // sum up sizes of lists above (in their expanded state)
         for (u32_ListCounter = 0U; u32_ListCounter < (static_cast<uint32_t>(pc_Datapool->c_Lists.size()) - 1UL);
              ++u32_ListCounter)
         {
            s32_HeightOtherLists +=
               static_cast<int32_t>(C_SdNdeDpUtil::h_GetTableSize(ou32_NodeIndex, ou32_DataPoolIndex,
                                                                  u32_ListCounter)) +
               s32_HEADER_OFFSET; // no infinite recursion because of list index check above
         }

         // calculate remaining space for last table
         s32_RemainingSpace = os32_MaximumHeight - s32_HeightOtherLists;

         // check if more remaining space than the current proposed height
         if ((s32_Height + s32_HEADER_OFFSET) < s32_RemainingSpace)
         {
            // list can use the remaining space
            s32_Height = s32_RemainingSpace - s32_HEADER_OFFSET;
         }
      }

      s32_Retval = s32_Height;
   }
   else
   {
      //Fallback, should not happen
      s32_Retval = s32_HEADER_OFFSET;
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Returns all connected shared datapools of one group

   \param[in]   ou32_SharedDatapoolGroup     Index of shared datapool group
   \param[in]   orc_BaseDatapoolId           Datapool Id of datapool which shall not be added to result
   \param[in]   ou32_NodeIndex               Node index which node name shall not be added as
                                             namespace to the datapool names
   \param[out]  orc_SharedDatapoolNameGroup  Result vector with all connected shared datapool names of one group
                                             Will be cleared in any case

   \return
   C_NO_ERR    Shared datapool group found and copied
   C_RANGE     ou32_SharedDatapoolGroup is invalid
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_SdNdeDpUtil::h_GetSharedDatapoolGroup(const uint32_t ou32_SharedDatapoolGroup,
                                                const C_OscNodeDataPoolId & orc_BaseDatapoolId,
                                                const uint32_t ou32_NodeIndex,
                                                std::vector<QString> & orc_SharedDatapoolNameGroup)
{
   int32_t s32_Return = C_RANGE;
   const C_PuiSdSharedDatapools & rc_SharedDatapools = C_PuiSdHandler::h_GetInstance()->GetSharedDatapoolsConst();

   std::vector<C_OscNodeDataPoolId> c_SharedDatapoolGroup;

   orc_SharedDatapoolNameGroup.clear();

   if (rc_SharedDatapools.GetSharedDatapoolGroup(ou32_SharedDatapoolGroup, c_SharedDatapoolGroup) == C_NO_ERR)
   {
      uint32_t u32_DatapoolIdCounter;

      s32_Return = C_NO_ERR;

      for (u32_DatapoolIdCounter = 0U; u32_DatapoolIdCounter < c_SharedDatapoolGroup.size();
           ++u32_DatapoolIdCounter)
      {
         const C_OscNodeDataPoolId & rc_DpId = c_SharedDatapoolGroup[u32_DatapoolIdCounter];

         // Do not show the selected datapool
         if (rc_DpId != orc_BaseDatapoolId)
         {
            const C_OscNode * const pc_Node = C_PuiSdHandler::h_GetInstance()->GetOscNodeConst(rc_DpId.u32_NodeIndex);
            if ((pc_Node != NULL) &&
                (rc_DpId.u32_DataPoolIndex < pc_Node->c_DataPools.size()))
            {
               QString c_Text = "";
               if (rc_DpId.u32_NodeIndex != ou32_NodeIndex)
               {
                  // Add the node name if the datapool is of an other node
                  c_Text = static_cast<QString>(pc_Node->c_Properties.c_Name.c_str()) + "::";
               }

               // Get the datapool name
               c_Text += pc_Node->c_DataPools[rc_DpId.u32_DataPoolIndex].c_Name.c_str();

               orc_SharedDatapoolNameGroup.push_back(c_Text);
            }
         }
      }
   }

   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get next DIAG or NVM Datapool index.

   Wraps around:
      Forwards from last NVM Datapool index is the first DIAG Datapool index, i.e. 0.
      Backwards from first DIAG Datapool index is last NVM Datapool index.

   \param[in]  ou32_NodeIndex       Node index
   \param[in]  ou32_DatapoolIndex   Datapool index
   \param[in]  oq_Forwards          true: select next Datapool
                                    false: select previous Datapool

   \return
   Index of next DIAG or NVM Datapool (-1 if parameters are invalid)
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_SdNdeDpUtil::h_GetNextDiagOrNvmDpIndex(const uint32_t ou32_NodeIndex, const uint32_t ou32_DatapoolIndex,
                                                 const bool oq_Forwards)
{
   int32_t s32_Return = -1;
   const int32_t s32_NumberNvm = C_PuiSdHandler::h_GetInstance()->GetDataPoolCount(ou32_NodeIndex,
                                                                                   C_OscNodeDataPool::eNVM);
   const int32_t s32_NumberDiag = C_PuiSdHandler::h_GetInstance()->GetDataPoolCount(ou32_NodeIndex,
                                                                                    C_OscNodeDataPool::eDIAG);

   if ((s32_NumberNvm >= 0) && (s32_NumberDiag >= 0))
   {
      const int32_t s32_Max = s32_NumberNvm + s32_NumberDiag;

      if (static_cast<int32_t>(ou32_DatapoolIndex) < s32_Max) // indirectly checks if NVM/DIAG Datapool index was passed
      {
         int32_t s32_NextDpIndex = ou32_DatapoolIndex;

         // count one index forwards or backwards
         if (oq_Forwards == true)
         {
            s32_NextDpIndex += 1;
         }
         else
         {
            s32_NextDpIndex -= 1;
         }

         // forward wrap: select first Datapool
         if (s32_NextDpIndex >= s32_Max)
         {
            s32_NextDpIndex = 0;
         }

         // backward wrap: select last DIAG Datapool
         if (s32_NextDpIndex < 0)
         {
            s32_NextDpIndex = s32_Max - 1; // -1 if no NVM and no DIAG Datapool exist (which is invalid)
         }

         s32_Return = s32_NextDpIndex;
      }
   }

   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default constructor
*/
//----------------------------------------------------------------------------------------------------------------------
C_SdNdeDpUtil::C_SdNdeDpUtil(void)
{
}
