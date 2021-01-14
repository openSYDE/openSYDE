//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       short description (implementation)

   detailed description

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.h"

#include "constants.h"
#include "C_GtGetText.h"
#include "C_OgeCbxTable.h"
#include "C_OgeSpxTable.h"
#include "C_SdNdeSfoResetMessageTableModel.h"
#include "C_SdNdeSfoResetMessageTableDelegate.h"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw_types;
using namespace stw_opensyde_gui;
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

   Set up GUI with all elements.

   \param[in,out] opc_Parent Optional pointer to parent
*/
//----------------------------------------------------------------------------------------------------------------------
C_SdNdeSfoResetMessageTableDelegate::C_SdNdeSfoResetMessageTableDelegate(QObject * const opc_Parent) :
   QStyledItemDelegate(opc_Parent)
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Overwritten create editor event slot

   Here: Create appropiate editor widget

   \param[in,out] opc_Parent Parent widget
   \param[in]     orc_Option Style options
   \param[in]     orc_Index  Correlating index

   \return
   Editor widget
*/
//----------------------------------------------------------------------------------------------------------------------
QWidget * C_SdNdeSfoResetMessageTableDelegate::createEditor(QWidget * const opc_Parent,
                                                            const QStyleOptionViewItem & orc_Option,
                                                            const QModelIndex & orc_Index) const
{
   QWidget * pc_Retval = NULL;

   Q_UNUSED(orc_Option)
   if (orc_Index.isValid() == true)
   {
      C_OgeSpxTable * pc_SpinBox;
      C_OgeCbxTable * pc_ComboBox;
      const C_SdNdeSfoResetMessageTableModel::E_Columns e_Col = C_SdNdeSfoResetMessageTableModel::h_ColumnToEnum(
         orc_Index.column());
      switch (e_Col)
      {
      case C_SdNdeSfoResetMessageTableModel::eID:
         pc_SpinBox = new C_OgeSpxTable(opc_Parent);
         pc_SpinBox->setPrefix(C_GtGetText::h_GetText("0x"));
         pc_SpinBox->setDisplayIntegerBase(16);
         pc_SpinBox->SetMinimumCustom(0);
         if (orc_Index.data(msn_USER_ROLE_CONDITIONAL_VALUE) == 0)
         {
            pc_SpinBox->SetMaximumCustom(0x7FF);
         }
         else
         {
            pc_SpinBox->SetMaximumCustom(0x1FFFFFFF);
         }
         pc_Retval = pc_SpinBox;
         break;
      case C_SdNdeSfoResetMessageTableModel::eDLC:
         pc_SpinBox = new C_OgeSpxTable(opc_Parent);
         pc_SpinBox->SetMinimumCustom(0);
         pc_SpinBox->SetMaximumCustom(8);
         pc_Retval = pc_SpinBox;
         break;
      case C_SdNdeSfoResetMessageTableModel::eBYTE1:
      case C_SdNdeSfoResetMessageTableModel::eBYTE2:
      case C_SdNdeSfoResetMessageTableModel::eBYTE3:
      case C_SdNdeSfoResetMessageTableModel::eBYTE4:
      case C_SdNdeSfoResetMessageTableModel::eBYTE5:
      case C_SdNdeSfoResetMessageTableModel::eBYTE6:
      case C_SdNdeSfoResetMessageTableModel::eBYTE7:
      case C_SdNdeSfoResetMessageTableModel::eBYTE8:
         pc_SpinBox = new C_OgeSpxTable(opc_Parent);
         pc_SpinBox->setPrefix(C_GtGetText::h_GetText("0x"));
         pc_SpinBox->setDisplayIntegerBase(16);
         pc_SpinBox->SetMinimumCustom(0);
         pc_SpinBox->SetMaximumCustom(255);
         pc_Retval = pc_SpinBox;
         break;
      case C_SdNdeSfoResetMessageTableModel::eEXTENDED_FORMAT:
         pc_ComboBox = new C_OgeCbxTable(opc_Parent);
         pc_ComboBox->addItem(C_GtGetText::h_GetText("11 Bit"));
         pc_ComboBox->addItem(C_GtGetText::h_GetText("29 Bit"));
         pc_Retval = pc_ComboBox;
         break;
      default:
         break;
      }
   }
   return pc_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Overwritten set editor data event slot

   Here: Pass relevant data

   \param[in,out] opc_Editor Editor widget
   \param[in]     orc_Index  Correlating index
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeSfoResetMessageTableDelegate::setEditorData(QWidget * const opc_Editor, const QModelIndex & orc_Index) const
{
   if ((opc_Editor != NULL) && (orc_Index.isValid() == true))
   {
      QSpinBox * pc_SpinBox;
      QComboBox * pc_ComboBox;
      const C_SdNdeSfoResetMessageTableModel::E_Columns e_Col = C_SdNdeSfoResetMessageTableModel::h_ColumnToEnum(
         orc_Index.column());
      switch (e_Col)
      {
      case C_SdNdeSfoResetMessageTableModel::eID:
      case C_SdNdeSfoResetMessageTableModel::eDLC:
      case C_SdNdeSfoResetMessageTableModel::eBYTE1:
      case C_SdNdeSfoResetMessageTableModel::eBYTE2:
      case C_SdNdeSfoResetMessageTableModel::eBYTE3:
      case C_SdNdeSfoResetMessageTableModel::eBYTE4:
      case C_SdNdeSfoResetMessageTableModel::eBYTE5:
      case C_SdNdeSfoResetMessageTableModel::eBYTE6:
      case C_SdNdeSfoResetMessageTableModel::eBYTE7:
      case C_SdNdeSfoResetMessageTableModel::eBYTE8:
         
         pc_SpinBox = dynamic_cast<QSpinBox *>(opc_Editor);
         if (pc_SpinBox != NULL)
         {
            pc_SpinBox->setValue(orc_Index.data(static_cast<sintn>(Qt::EditRole)).toInt());
         }
         break;
      case C_SdNdeSfoResetMessageTableModel::eEXTENDED_FORMAT:
         
         pc_ComboBox = dynamic_cast<QComboBox *>(opc_Editor);
         if (pc_ComboBox != NULL)
         {
            pc_ComboBox->setCurrentIndex(orc_Index.data(static_cast<sintn>(Qt::EditRole)).toInt());
         }
         break;
      default:
         break;
      }
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
void C_SdNdeSfoResetMessageTableDelegate::setModelData(QWidget * const opc_Editor, QAbstractItemModel * const opc_Model,
                                                       const QModelIndex & orc_Index) const
{
   if (((opc_Editor != NULL) && (opc_Model != NULL)) && (orc_Index.isValid() == true))
   {
      QSpinBox * pc_SpinBox;
      QComboBox * pc_ComboBox;
      const C_SdNdeSfoResetMessageTableModel::E_Columns e_Col = C_SdNdeSfoResetMessageTableModel::h_ColumnToEnum(
         orc_Index.column());
      switch (e_Col)
      {
      case C_SdNdeSfoResetMessageTableModel::eID:
      case C_SdNdeSfoResetMessageTableModel::eDLC:
      case C_SdNdeSfoResetMessageTableModel::eBYTE1:
      case C_SdNdeSfoResetMessageTableModel::eBYTE2:
      case C_SdNdeSfoResetMessageTableModel::eBYTE3:
      case C_SdNdeSfoResetMessageTableModel::eBYTE4:
      case C_SdNdeSfoResetMessageTableModel::eBYTE5:
      case C_SdNdeSfoResetMessageTableModel::eBYTE6:
      case C_SdNdeSfoResetMessageTableModel::eBYTE7:
      case C_SdNdeSfoResetMessageTableModel::eBYTE8:
         
         pc_SpinBox = dynamic_cast<QSpinBox *>(opc_Editor);
         if (pc_SpinBox != NULL)
         {
            opc_Model->setData(orc_Index, pc_SpinBox->value());
         }
         break;
      case C_SdNdeSfoResetMessageTableModel::eEXTENDED_FORMAT:
         
         pc_ComboBox = dynamic_cast<QComboBox *>(opc_Editor);
         if (pc_ComboBox != NULL)
         {
            opc_Model->setData(orc_Index, pc_ComboBox->currentIndex());
         }
         break;
      }
   }
}
