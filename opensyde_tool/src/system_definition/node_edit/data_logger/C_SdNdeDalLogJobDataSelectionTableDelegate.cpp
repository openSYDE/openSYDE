//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Table delegate for data selection widget

   \copyright   Copyright 2024 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.hpp"

#include "stwtypes.hpp"
#include "stwerrors.hpp"

#include "C_SdNdeDalLogJobDataSelectionTableDelegate.hpp"
#include "C_SdNdeDalLogJobDataSelectionTableModel.hpp"
#include "C_OgeCbxMultiSelectTableHalc.hpp"
#include "C_OgeCbxTableHalc.hpp"
#include "C_OgeSpxTableHalc.hpp"
#include "C_OgeLeTable.hpp"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw::opensyde_gui;
using namespace stw::opensyde_gui_elements;
using namespace stw::opensyde_gui_logic;
using namespace stw::errors;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

/* -- Global Variables ---------------------------------------------------------------------------------------------- */

/* -- Module Global Variables --------------------------------------------------------------------------------------- */

/* -- Module Global Function Prototypes ----------------------------------------------------------------------------- */

/* -- Implementation ------------------------------------------------------------------------------------------------ */

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Default constructor
 *
 *  \param[in,out] opc_Parent Optional pointer to parent
*/
//----------------------------------------------------------------------------------------------------------------------
C_SdNdeDalLogJobDataSelectionTableDelegate::C_SdNdeDalLogJobDataSelectionTableDelegate(QObject * const opc_Parent) :
   QStyledItemDelegate(opc_Parent)
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
QWidget * C_SdNdeDalLogJobDataSelectionTableDelegate::createEditor(QWidget * const opc_Parent,
                                                                   const QStyleOptionViewItem & orc_Option,
                                                                   const QModelIndex & orc_Index) const
{
   QWidget * pc_Retval = NULL;
   C_OgeLeTable * pc_LineEdit = NULL;

   Q_UNUSED(orc_Option)

   if ((opc_Parent != NULL) && (orc_Index.isValid() == true))
   {
      // Widget created based on the currently selected column
      const C_SdNdeDalLogJobDataSelectionTableModel::E_Columns e_Col =
         C_SdNdeDalLogJobDataSelectionTableModel::h_ColumnToEnum(
            orc_Index.column());

      // Line edit for "Logging Name" column
      if (e_Col == C_SdNdeDalLogJobDataSelectionTableModel::eLOGGING_NAME)
      {
         pc_LineEdit = new C_OgeLeTable(opc_Parent);
         pc_LineEdit->setTextMargins(0, 1, 0, 2);
         pc_Retval = pc_LineEdit;
      }
   }

   return pc_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Overwritten set editor data event slot

   Here: Pass relevant data

   \param[in,out] opc_Editor Editor widget
   \param[in]     orc_Index  Correlating index
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDalLogJobDataSelectionTableDelegate::setEditorData(QWidget * const opc_Editor,
                                                               const QModelIndex & orc_Index) const
{
   if (((opc_Editor != NULL) && (orc_Index.isValid() == true)))
   {
      //  Widget created based on the currently selected column
      const C_SdNdeDalLogJobDataSelectionTableModel::E_Columns e_Col =
         C_SdNdeDalLogJobDataSelectionTableModel::h_ColumnToEnum(
            orc_Index.column());

      // Line edit for "Logging Name" column
      if (e_Col == C_SdNdeDalLogJobDataSelectionTableModel::eLOGGING_NAME)
      {
         QLineEdit * const pc_LineEdit = dynamic_cast<QLineEdit * const>(opc_Editor);
         if (pc_LineEdit != NULL)
         {
            pc_LineEdit->setText(orc_Index.data(static_cast<int32_t>(Qt::EditRole)).toString());
         }
      }
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
void C_SdNdeDalLogJobDataSelectionTableDelegate::setModelData(QWidget * const opc_Editor,
                                                              QAbstractItemModel * const opc_Model,
                                                              const QModelIndex & orc_Index) const
{
   if (((opc_Editor != NULL) && (opc_Model != NULL)) && (orc_Index.isValid() == true))
   {
      // Widget created based on the currently selected column
      const C_SdNdeDalLogJobDataSelectionTableModel::E_Columns e_Col =
         C_SdNdeDalLogJobDataSelectionTableModel::h_ColumnToEnum(
            orc_Index.column());

      // Line edit for "Logging Name" column
      if (e_Col == C_SdNdeDalLogJobDataSelectionTableModel::eLOGGING_NAME)
      {
         const QLineEdit * const pc_LineEdit = dynamic_cast<const QLineEdit * const>(opc_Editor);

         if (pc_LineEdit != NULL)
         {
            // if no name is entered e.g. user erases entire text, default text is "[Namespace]"
            QString c_Value = pc_LineEdit->text();
            if (c_Value.isEmpty() == true)
            {
               c_Value = C_GtGetText::h_GetText("[Namespace]");
            }
            opc_Model->setData(orc_Index, c_Value, static_cast<int32_t>(Qt::EditRole));
         }
      }
   }
}
