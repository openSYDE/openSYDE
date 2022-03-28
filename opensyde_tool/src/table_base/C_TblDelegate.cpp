//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Delegate (interaction) component base for any table (implementation)

   Delegate (interaction) component base for any table

   \copyright   Copyright 2018 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.h"

#include <QBitArray>
#include <QSvgRenderer>

#include "TGLUtils.h"
#include "stwerrors.h"
#include "C_GtGetText.h"
#include "C_TblDelegate.h"
#include "C_OgeWiCustomMessage.h"
#include "C_SdNdeDpContentUtil.h"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw_tgl;
using namespace stw_types;
using namespace stw_errors;
using namespace stw_opensyde_gui;
using namespace stw_opensyde_core;
using namespace stw_opensyde_gui_logic;
using namespace stw_opensyde_gui_elements;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

/* -- Global Variables ---------------------------------------------------------------------------------------------- */

/* -- Module Global Variables --------------------------------------------------------------------------------------- */

/* -- Module Global Function Prototypes ----------------------------------------------------------------------------- */

/* -- Implementation ------------------------------------------------------------------------------------------------ */

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Default constructor

   \param[in,out]  opc_Parent    Optional pointer to parent
*/
//----------------------------------------------------------------------------------------------------------------------
C_TblDelegate::C_TblDelegate(QObject * const opc_Parent) :
   QStyledItemDelegate(opc_Parent),
   mq_InitialSelection(true)
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Overwritten create editor event slot

   Here: Create appropriate editor widget

   \param[in,out]  opc_Parent    Parent widget
   \param[in]      orc_Option    Style options
   \param[in]      orc_Index     Correlating index

   \return
   Editor widget
*/
//----------------------------------------------------------------------------------------------------------------------
QWidget * C_TblDelegate::createEditor(QWidget * const opc_Parent, const QStyleOptionViewItem & orc_Option,
                                      const QModelIndex & orc_Index) const
{
   QWidget * pc_Retval = NULL;

   if (orc_Index.data(msn_USER_ROLE_INTERACTION_IS_LINK).toBool() == false)
   {
      C_OgeCbxTableBase * pc_ComboBox;
      C_OgeCbxMultiSelect * pc_MultiSelectComboBox;
      QStringList c_StringList;
      QStringList c_StringListValues;
      QStringList c_StringListStrings;
      switch (C_TblDelegate::mh_GetInteractionElementValue(orc_Index))
      {
      case eURIEL_LINE_EDIT:
         pc_Retval = m_CreateLineEdit(opc_Parent);
         // extra connection with model via commitData not necessary here
         break;
      case eURIEL_COMBO_BOX:
         pc_ComboBox = m_CreateComboBox(opc_Parent);
         c_StringListValues = orc_Index.data(msn_USER_ROLE_INTERACTION_COMBO_BOX_VALUES_LIST).toStringList();
         c_StringListStrings = orc_Index.data(msn_USER_ROLE_INTERACTION_COMBO_BOX_STRINGS_LIST).toStringList();
         pc_ComboBox->InitFromStringList(c_StringListStrings, c_StringListValues);
         if (orc_Index.data(msn_USER_ROLE_INTERACTION_COMBO_BOX_EDITABLE).toBool() == true)
         {
            // make editable
            pc_ComboBox->setEditable(true);
            pc_ComboBox->setInsertPolicy(QComboBox::NoInsert);
            // minimum and maximum
            c_StringList = orc_Index.data(msn_USER_ROLE_INTERACTION_GENERIC_SPIN_BOX_PARAMETERS_LIST).toStringList();
            if (c_StringList.size() == 4)
            {
               C_OSCNodeDataPoolContent c_Min;
               C_OSCNodeDataPoolContent c_Max;
               float64 f64_Factor;
               float64 f64_Offset;
               bool q_Ok;
               const QString & rc_Factor = c_StringList.at(2);
               const QString & rc_Offset = c_StringList.at(3);
               tgl_assert(C_SdNdeDpContentUtil::h_SetAllContentFromString(c_StringList.at(0), c_Min) == C_NO_ERR);
               tgl_assert(C_SdNdeDpContentUtil::h_SetAllContentFromString(c_StringList.at(1), c_Max) == C_NO_ERR);
               f64_Factor = rc_Factor.toDouble(&q_Ok);
               tgl_assert(q_Ok == true);
               f64_Offset = rc_Offset.toDouble(&q_Ok);
               tgl_assert(q_Ok == true);
               pc_ComboBox->InitMinMaxAndScaling(c_Min, c_Max, f64_Factor, f64_Offset);
            }
         }
         // connect combo box with model (commit on index change; includes commit on text change)
         connect(pc_ComboBox, static_cast<void (QComboBox::*)(sintn)>(&QComboBox::currentIndexChanged),
                 this, &C_TblDelegate::m_CommitData);
         pc_Retval = pc_ComboBox;
         break;
      case eURIEL_MULTI_SELECT_COMBO_BOX:
         pc_MultiSelectComboBox = m_CreateMultiSelectComboBox(opc_Parent);
         // connect combo box with model (commit on text change)
         connect(pc_MultiSelectComboBox, &C_OgeCbxMultiSelect::SigValueChanged, this, &C_TblDelegate::m_CommitData);
         pc_Retval = pc_MultiSelectComboBox;
         break;
      case eURIEL_GENERIC_SPIN_BOX:
         c_StringList = orc_Index.data(msn_USER_ROLE_INTERACTION_GENERIC_SPIN_BOX_PARAMETERS_LIST).toStringList();
         if (c_StringList.size() == 4)
         {
            C_OSCNodeDataPoolContent c_Min;
            C_OSCNodeDataPoolContent c_Max;
            float64 f64_Factor;
            float64 f64_Offset;
            bool q_Ok;
            const QString & rc_Factor = c_StringList.at(2);
            const QString & rc_Offset = c_StringList.at(3);
            tgl_assert(C_SdNdeDpContentUtil::h_SetAllContentFromString(c_StringList.at(0), c_Min) == C_NO_ERR);
            tgl_assert(C_SdNdeDpContentUtil::h_SetAllContentFromString(c_StringList.at(1), c_Max) == C_NO_ERR);
            f64_Factor = rc_Factor.toDouble(&q_Ok);
            tgl_assert(q_Ok == true);
            f64_Offset = rc_Offset.toDouble(&q_Ok);
            tgl_assert(q_Ok == true);
            pc_Retval = m_CreateGenericEditor(opc_Parent, orc_Index, c_Min, c_Max, f64_Factor, f64_Offset, 0UL);
         }
         break;
      case eURIEL_NONE:
         pc_Retval = QStyledItemDelegate::createEditor(opc_Parent, orc_Option, orc_Index);
         break;
      default:
         tgl_assert(false);
         break;
      }
   }
   return pc_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Overwritten set editor data event slot

   \param[in,out]  opc_Editor    Editor widget
   \param[in]      orc_Index     Correlating index
*/
//----------------------------------------------------------------------------------------------------------------------
void C_TblDelegate::setEditorData(QWidget * const opc_Editor, const QModelIndex & orc_Index) const
{
   C_TblEditLineEditBase * pc_LineEdit;
   C_OgeCbxTableBase * pc_ComboBox;
   C_OgeCbxMultiSelect * pc_MultiSelectComboBox;

   switch (C_TblDelegate::mh_GetInteractionElementValue(orc_Index))
   {
   case eURIEL_LINE_EDIT:

      pc_LineEdit = dynamic_cast<C_TblEditLineEditBase *>(opc_Editor);
      if (pc_LineEdit != NULL)
      {
         pc_LineEdit->SetFromVariant(orc_Index.data(static_cast<sintn>(Qt::DisplayRole)),
                                     orc_Index.data(static_cast<sintn>(Qt::EditRole)));
         if (orc_Index.data(msn_USER_ROLE_INTERACTION_USE_MIN_VALUE).toBool())
         {
            pc_LineEdit->SetMinFromVariant(orc_Index.data(msn_USER_ROLE_INTERACTION_MINIMUM_VALUE));
         }
         if (orc_Index.data(msn_USER_ROLE_INTERACTION_USE_MAX_VALUE).toBool())
         {
            pc_LineEdit->SetMaxFromVariant(orc_Index.data(msn_USER_ROLE_INTERACTION_MAXIMUM_VALUE));
         }
      }
      break;
   case eURIEL_COMBO_BOX:

      pc_ComboBox = dynamic_cast<C_OgeCbxTableBase *>(opc_Editor);
      if (pc_ComboBox != NULL)
      {
         bool q_Ok;
         const sint64 s64_Val = orc_Index.data(static_cast<sintn>(Qt::EditRole)).toLongLong(&q_Ok);
         if (q_Ok)
         {
            pc_ComboBox->SetValue(s64_Val);
         }
      }
      break;
   case eURIEL_MULTI_SELECT_COMBO_BOX:

      pc_MultiSelectComboBox = dynamic_cast<C_OgeCbxMultiSelect *>(opc_Editor);
      if (pc_MultiSelectComboBox != NULL)
      {
         const QBitArray c_BitArrayValues = orc_Index.data(static_cast<sintn>(Qt::EditRole)).toBitArray();
         const QStringList c_StringListStrings =
            orc_Index.data(msn_USER_ROLE_INTERACTION_MULTI_SELECT_COMBO_BOX_STRINGS_LIST).toStringList();
         pc_MultiSelectComboBox->Init(c_StringListStrings, c_BitArrayValues);
      }
      break;
   case eURIEL_GENERIC_SPIN_BOX:
      m_SetGenericEditorDataVariable(opc_Editor, orc_Index);
      break;
   case eURIEL_NONE:
      QStyledItemDelegate::setEditorData(opc_Editor, orc_Index);
      break;
   default:
      tgl_assert(false);
      break;
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Overwritten set model data event slot

   Here: Pass relevant data

   \param[in,out]  opc_Editor    Editor widget
   \param[in,out]  opc_Model     Model object
   \param[in]      orc_Index     Correlating index
*/
//----------------------------------------------------------------------------------------------------------------------
void C_TblDelegate::setModelData(QWidget * const opc_Editor, QAbstractItemModel * const opc_Model,
                                 const QModelIndex & orc_Index) const
{
   C_TblEditLineEditBase * pc_LineEdit;
   C_OgeCbxTableBase * pc_ComboBox;
   C_OgeCbxMultiSelect * pc_MultiSelectComboBox;

   switch (C_TblDelegate::mh_GetInteractionElementValue(orc_Index))
   {
   case eURIEL_LINE_EDIT:

      pc_LineEdit = dynamic_cast<C_TblEditLineEditBase *>(opc_Editor);
      if (pc_LineEdit != NULL)
      {
         QString c_ErrorDescription;
         QVariant c_Value;
         if (pc_LineEdit->GetValueAsVariant(c_Value, c_ErrorDescription) == C_NO_ERR)
         {
            opc_Model->setData(orc_Index, c_Value, static_cast<sintn>(Qt::EditRole));
         }
         else
         {
            C_OgeWiCustomMessage c_Message(opc_Editor, C_OgeWiCustomMessage::eWARNING);
            c_Message.SetHeading(C_GtGetText::h_GetText("Invalid input"));
            c_Message.SetDescription(c_ErrorDescription);
            c_Message.SetCustomMinHeight(230, 270);
            c_Message.Execute();
            //Reset data to last known value
            this->setEditorData(pc_LineEdit, orc_Index);
         }
      }
      break;
   case eURIEL_COMBO_BOX:

      pc_ComboBox = dynamic_cast<C_OgeCbxTableBase *>(opc_Editor);
      if (pc_ComboBox != NULL)
      {
         QString c_ErrorDescription;
         sint64 s64_Value;
         if (pc_ComboBox->GetValue(s64_Value, c_ErrorDescription) == C_NO_ERR)
         {
            opc_Model->setData(orc_Index, s64_Value, static_cast<sintn>(Qt::EditRole));
         }
         else
         {
            C_OgeWiCustomMessage c_Message(opc_Editor, C_OgeWiCustomMessage::eWARNING);
            c_Message.SetHeading(C_GtGetText::h_GetText("Invalid input"));
            c_Message.SetDescription(c_ErrorDescription);
            c_Message.Execute();
            //Reset data to last known value
            this->setEditorData(pc_ComboBox, orc_Index);
         }
      }
      break;
   case eURIEL_MULTI_SELECT_COMBO_BOX:

      pc_MultiSelectComboBox = dynamic_cast<C_OgeCbxMultiSelect *>(opc_Editor);
      if (pc_MultiSelectComboBox != NULL)
      {
         const QBitArray c_BitArrayValues = pc_MultiSelectComboBox->GetValuesAsBitArray();
         opc_Model->setData(orc_Index, c_BitArrayValues, static_cast<sintn>(Qt::EditRole));
      }
      break;
   case eURIEL_GENERIC_SPIN_BOX:
      mh_SetModelGenericDataVariable(opc_Editor, opc_Model, orc_Index);
      break;
   case eURIEL_NONE:
      QStyledItemDelegate::setModelData(opc_Editor, opc_Model, orc_Index);
      break;
   default:
      tgl_assert(false);
      break;
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Overridden paint method.

   Here: Center icon and deactivate icon highlighting.

   \param[in]  opc_Painter    pointer to painter
   \param[in]  orc_Option     style option
   \param[in]  orc_Index      model index
*/
//----------------------------------------------------------------------------------------------------------------------
void C_TblDelegate::paint(QPainter * const opc_Painter, const QStyleOptionViewItem & orc_Option,
                          const QModelIndex & orc_Index) const
{
   const QStringList c_IconPaths = orc_Index.data(msn_USER_ROLE_ICON).toStringList();

   QStyledItemDelegate::paint(opc_Painter, orc_Option, orc_Index);
   if (c_IconPaths.count() == 2)
   {
      const QSize c_ICON_SIZE(16, 16);
      const QRect c_Rect(orc_Option.rect.topLeft() + QPoint((orc_Option.rect.width() - c_ICON_SIZE.width()) / 2,
                                                            (orc_Option.rect.height() - c_ICON_SIZE.height()) / 2),
                         c_ICON_SIZE);
      if (orc_Option.state.testFlag(QStyle::State_Selected) == true)
      {
         QSvgRenderer c_Renderer(c_IconPaths.at(1));
         c_Renderer.render(opc_Painter, c_Rect);
      }
      else
      {
         QSvgRenderer c_Renderer(c_IconPaths.at(0));
         c_Renderer.render(opc_Painter, c_Rect);
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get interaction element value as enum from index

   \param[in]  orc_Index   Index

   \return
   Interaction element value as enum from index
*/
//----------------------------------------------------------------------------------------------------------------------
E_UserRoleInteractionElementValue C_TblDelegate::mh_GetInteractionElementValue(const QModelIndex & orc_Index)
{
   E_UserRoleInteractionElementValue e_Retval;

   switch (orc_Index.data(msn_USER_ROLE_INTERACTION_ELEMENT_TYPE).toInt())
   {
   case 1:
      e_Retval = eURIEL_LINE_EDIT;
      break;
   case 2:
      e_Retval = eURIEL_COMBO_BOX;
      break;
   case 3:
      e_Retval = eURIEL_GENERIC_SPIN_BOX;
      break;
   case 4:
      e_Retval = eURIEL_MULTI_SELECT_COMBO_BOX;
      break;
   default:
      e_Retval = eURIEL_NONE;
      break;
   }
   return e_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Create widget to use for editing this value

   \param[in,out]  opc_Parent          Parent widget
   \param[in]      orc_Index           Correlating index
   \param[in]      orc_Min             Minimum value(s)
   \param[in]      orc_Max             Maximum value(s)
   \param[in]      of64_Factor         Scaling factor
   \param[in]      of64_Offset         Scaling offset
   \param[in]      oru32_ArrayIndex    Array index to access single element

   \return
   Editor widget
*/
//----------------------------------------------------------------------------------------------------------------------
QWidget * C_TblDelegate::m_CreateGenericEditor(QWidget * const opc_Parent, const QModelIndex & orc_Index,
                                               const C_OSCNodeDataPoolContent & orc_Min,
                                               const C_OSCNodeDataPoolContent & orc_Max, const float64 of64_Factor,
                                               const float64 of64_Offset, const uint32 & oru32_ArrayIndex) const
{
   QWidget * pc_Retval = NULL;

   //Check if the input is consistent
   if (((orc_Index.isValid() == true) && (orc_Min.GetType() == orc_Max.GetType())) &&
       (orc_Min.GetArray() == orc_Max.GetArray()))
   {
      //Create
      C_OgeWiSpinBoxGroup * const pc_SpinBoxGroup = m_CreateSpinBox(opc_Parent);
      //Init type
      pc_SpinBoxGroup->Init(orc_Min, orc_Max, of64_Factor, of64_Offset, oru32_ArrayIndex);
      pc_Retval = pc_SpinBoxGroup;
      // connect spin box with model (commit on value change)
      connect(pc_SpinBoxGroup, &C_OgeWiSpinBoxGroup::SigValueChanged, this, &C_TblDelegate::m_CommitData);
   }
   return pc_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Set value to widget used for editing this value

   \param[in,out]  opc_Editor    Editor widget
   \param[in]      orc_Index     Correlating index
*/
//----------------------------------------------------------------------------------------------------------------------
void C_TblDelegate::m_SetGenericEditorDataVariable(QWidget * const opc_Editor, const QModelIndex & orc_Index) const
{
   C_OgeWiSpinBoxGroup * const pc_SpinBox = dynamic_cast<C_OgeWiSpinBoxGroup * const>(opc_Editor);

   if (pc_SpinBox != NULL)
   {
      //Special setter
      pc_SpinBox->SetValue(orc_Index.data(static_cast<sintn>(Qt::EditRole)));
      //Initial selection
      if (mq_InitialSelection)
      {
         pc_SpinBox->SelectAll();
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Slot to emit commitData signal from sender widget.

   The signal commitData must be emitted when the editor widget has completed editing the data,
   and wants to write it back into the model.
   Adopted from Star Delegate Example of Qt Documentation.
*/
//----------------------------------------------------------------------------------------------------------------------
void C_TblDelegate::m_CommitData(void)
{
   QWidget * const pc_Editor = qobject_cast<QWidget * const>(this->sender());

   Q_EMIT (this->commitData(pc_Editor));
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Set model value from widget used to edit this value

   \param[in,out]  opc_Editor    Editor widget
   \param[in,out]  opc_Model     Model object
   \param[in]      orc_Index     Correlating index
*/
//----------------------------------------------------------------------------------------------------------------------
void C_TblDelegate::mh_SetModelGenericDataVariable(QWidget * const opc_Editor, QAbstractItemModel * const opc_Model,
                                                   const QModelIndex & orc_Index)
{
   C_OgeWiSpinBoxGroup * const pc_SpinBox = dynamic_cast<C_OgeWiSpinBoxGroup * const>(opc_Editor);

   if (pc_SpinBox != NULL)
   {
      //First read in the newest value and store as internal value
      pc_SpinBox->InterpretText();
      //Get internal value
      opc_Model->setData(orc_Index, pc_SpinBox->GetValue());
   }
}
