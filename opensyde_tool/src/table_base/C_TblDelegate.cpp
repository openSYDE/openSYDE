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

#include <QSvgRenderer>

#include "TGLUtils.h"
#include "stwerrors.h"
#include "C_GtGetText.h"
#include "C_TblDelegate.h"
#include "C_CamOgeLeTableEdit.h"
#include "C_OgeWiCustomMessage.h"
#include "C_CamOgeCbxTableSmall.h"
#include "C_SdNdeDataPoolContentUtil.h"
#include "C_CamOgeWiSpinBoxGroupTable.h"

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
/*! \brief   Default constructor

   \param[in,out] opc_Parent Optional pointer to parent
*/
//----------------------------------------------------------------------------------------------------------------------
C_TblDelegate::C_TblDelegate(QObject * const opc_Parent) :
   QStyledItemDelegate(opc_Parent),
   mq_InitialSelection(true)
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Overwritten create editor event slot

   Here: Create appropriate editor widget

   \param[in,out] opc_Parent Parent widget
   \param[in]     orc_Option Style options
   \param[in]     orc_Index  Correlating index

   \return
   Editor widget
*/
//----------------------------------------------------------------------------------------------------------------------
QWidget * C_TblDelegate::createEditor(QWidget * const opc_Parent, const QStyleOptionViewItem & orc_Option,
                                      const QModelIndex & orc_Index) const
{
   QWidget * pc_Retval;

   if (orc_Index.data(msn_USER_ROLE_INTERACTION_IS_LINK) == true)
   {
      pc_Retval = NULL;
   }
   else
   {
      QComboBox * pc_ComboBox;
      QStringList c_StringList;
      switch (C_TblDelegate::mh_GetInteractionElementValue(orc_Index))
      {
      case eURIEL_LINE_EDIT:
         pc_Retval = m_CreateLineEdit(opc_Parent);
         break;
      case eURIEL_COMBO_BOX:
         pc_ComboBox = m_CreateComboBox(opc_Parent);
         c_StringList = orc_Index.data(msn_USER_ROLE_INTERACTION_COMBO_BOX_VALUES_LIST).toStringList();
         for (QStringList::const_iterator c_ItComboBoxValue = c_StringList.begin();
              c_ItComboBoxValue != c_StringList.end(); ++c_ItComboBoxValue)
         {
            pc_ComboBox->addItem(*c_ItComboBoxValue);
         }
         pc_Retval = pc_ComboBox;
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
            tgl_assert(C_SdNdeDataPoolContentUtil::h_SetAllContentFromString(c_StringList.at(0), c_Min) == C_NO_ERR);
            tgl_assert(C_SdNdeDataPoolContentUtil::h_SetAllContentFromString(c_StringList.at(1), c_Max) == C_NO_ERR);
            f64_Factor = rc_Factor.toDouble(&q_Ok);
            tgl_assert(q_Ok == true);
            f64_Offset = rc_Offset.toDouble(&q_Ok);
            tgl_assert(q_Ok == true);
            pc_Retval = m_CreateGenericEditor(opc_Parent, orc_Index, c_Min, c_Max, f64_Factor, f64_Offset, 0UL);
         }
         else
         {
            pc_Retval = NULL;
         }
         break;
      default:
         pc_Retval = QStyledItemDelegate::createEditor(opc_Parent, orc_Option, orc_Index);
         break;
      }
   }
   return pc_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Overwritten set editor data event slot

   \param[in,out] opc_Editor Editor widget
   \param[in]     orc_Index  Correlating index
*/
//----------------------------------------------------------------------------------------------------------------------
void C_TblDelegate::setEditorData(QWidget * const opc_Editor, const QModelIndex & orc_Index) const
{
   C_TblEditLineEditBase * pc_LineEdit;
   QComboBox * pc_ComboBox;

   switch (C_TblDelegate::mh_GetInteractionElementValue(orc_Index))
   {
   case eURIEL_LINE_EDIT:
      //lint -e{929}  false positive in PC-Lint: allowed by MISRA 5-2-2
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
      //lint -e{929}  false positive in PC-Lint: allowed by MISRA 5-2-2
      pc_ComboBox = dynamic_cast<QComboBox *>(opc_Editor);
      if (pc_ComboBox != NULL)
      {
         pc_ComboBox->setCurrentIndex(orc_Index.data(static_cast<sintn>(Qt::EditRole)).toInt());
      }
      break;
   case eURIEL_GENERIC_SPIN_BOX:
      m_SetGenericEditorDataVariable(opc_Editor, orc_Index);
      break;
   default:
      QStyledItemDelegate::setEditorData(opc_Editor, orc_Index);
      break;
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Overwritten set model data event slot

   Here: Pass relevant data

   \param[in,out] opc_Editor Editor widget
   \param[in,out] opc_Model  Model object
   \param[in]     orc_Index  Correlating index
*/
//----------------------------------------------------------------------------------------------------------------------
void C_TblDelegate::setModelData(QWidget * const opc_Editor, QAbstractItemModel * const opc_Model,
                                 const QModelIndex & orc_Index) const
{
   C_TblEditLineEditBase * pc_LineEdit;
   QComboBox * pc_ComboBox;

   switch (C_TblDelegate::mh_GetInteractionElementValue(orc_Index))
   {
   case eURIEL_LINE_EDIT:
      //lint -e{929}  false positive in PC-Lint: allowed by MISRA 5-2-2
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
            c_Message.Execute();
         }
      }
      break;
   case eURIEL_COMBO_BOX:
      //lint -e{929}  false positive in PC-Lint: allowed by MISRA 5-2-2
      pc_ComboBox = dynamic_cast<QComboBox *>(opc_Editor);
      if (pc_ComboBox != NULL)
      {
         opc_Model->setData(orc_Index, pc_ComboBox->currentIndex(), static_cast<sintn>(Qt::EditRole));
      }
      break;
   case eURIEL_GENERIC_SPIN_BOX:
      mh_SetModelGenericDataVariable(opc_Editor, opc_Model, orc_Index);
      break;
   default:
      QStyledItemDelegate::setModelData(opc_Editor, opc_Model, orc_Index);
      break;
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Overridden paint method.

   Here: Center icon and deactivate icon highlighting.

   \param[in]     opc_Painter       pointer to painter
   \param[in]     orc_Option        style option
   \param[in]     orc_Index         model index
*/
//----------------------------------------------------------------------------------------------------------------------
void C_TblDelegate::paint(QPainter * const opc_Painter, const QStyleOptionViewItem & orc_Option,
                          const QModelIndex & orc_Index) const
{
   const QStringList c_IconPaths = orc_Index.data(msn_USER_ROLE_ICON).toStringList();

   QStyledItemDelegate::paint(opc_Painter, orc_Option, orc_Index);
   if (c_IconPaths.count() == 2)
   {
      const QSize c_IconSize(16, 16);
      const QRect c_Rect(orc_Option.rect.topLeft() + QPoint((orc_Option.rect.width() - c_IconSize.width()) / 2,
                                                            (orc_Option.rect.height() - c_IconSize.height()) / 2),
                         c_IconSize);
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
/*! \brief   Create generic combo box interaction element

   \param[in,out] opc_Parent Optional pointer to parent

   \return
   Generic combo box interaction element
*/
//----------------------------------------------------------------------------------------------------------------------
QComboBox * C_TblDelegate::m_CreateComboBox(QWidget * const opc_Parent) const
{
   return new C_CamOgeCbxTableSmall(opc_Parent);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Create generic line edit interaction element

   \param[in,out] opc_Parent Optional pointer to parent

   \return
   Generic line edit interaction element
*/
//----------------------------------------------------------------------------------------------------------------------
C_TblEditLineEditBase * C_TblDelegate::m_CreateLineEdit(QWidget * const opc_Parent) const
{
   return new C_CamOgeLeTableEdit(opc_Parent);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Create generic spin box interaction element

   \param[in,out] opc_Parent Optional pointer to parent

   \return
   Generic spin box interaction element
*/
//----------------------------------------------------------------------------------------------------------------------
C_OgeWiSpinBoxGroup * C_TblDelegate::m_CreateSpinBox(QWidget * const opc_Parent) const
{
   return new C_CamOgeWiSpinBoxGroupTable(opc_Parent);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get interaction element value as enum from index

   \param[in] orc_Index Index

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
   default:
      e_Retval = eURIEL_NONE;
   }
   return e_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Create widget to use for editing this value

   \param[in,out] opc_Parent           Parent widget
   \param[in]     orc_Index            Correlating index
   \param[in]     orc_Min              Minimum value(s)
   \param[in]     orc_Max              Maximum value(s)
   \param[in]     of64_Factor          Scaling factor
   \param[in]     of64_Offset          Scaling offset
   \param[in]     oru32_ArrayIndex     Array index to access single element

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
   }
   return pc_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set value to widget used for editing this value

   \param[in,out] opc_Editor Editor widget
   \param[in]     orc_Index  Correlating index
*/
//----------------------------------------------------------------------------------------------------------------------
void C_TblDelegate::m_SetGenericEditorDataVariable(QWidget * const opc_Editor, const QModelIndex & orc_Index) const
{
   //lint -e{929}  false positive in PC-Lint: allowed by MISRA 5-2-2
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
/*! \brief   Set model value from widget used to edit this value

   \param[in,out] opc_Editor Editor widget
   \param[in,out] opc_Model  Model object
   \param[in]     orc_Index  Correlating index
*/
//----------------------------------------------------------------------------------------------------------------------
void C_TblDelegate::mh_SetModelGenericDataVariable(QWidget * const opc_Editor, QAbstractItemModel * const opc_Model,
                                                   const QModelIndex & orc_Index)
{
   //lint -e{929}  false positive in PC-Lint: allowed by MISRA 5-2-2
   C_OgeWiSpinBoxGroup * const pc_SpinBox = dynamic_cast<C_OgeWiSpinBoxGroup * const>(opc_Editor);

   if (pc_SpinBox != NULL)
   {
      //First read in the newest value and store as internal value
      pc_SpinBox->InterpretText();
      //Get internal value
      opc_Model->setData(orc_Index, pc_SpinBox->GetValue());
   }
}
