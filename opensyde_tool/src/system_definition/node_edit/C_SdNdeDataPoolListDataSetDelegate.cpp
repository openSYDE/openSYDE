//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Node data pool list data set drawing delegate (implementation)

   Node data pool list data set drawing delegate

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.h"

#include <limits>
#include <QPainter>
#include "stwtypes.h"
#include "constants.h"
#include "C_SdNdeDataPoolListDataSetDelegate.h"
#include "C_OgeTedTable.h"
#include "C_OgeLeTable.h"
#include "C_OgeWiUtil.h"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw_types;
using namespace stw_opensyde_gui_logic;
using namespace stw_opensyde_gui_elements;
using namespace stw_opensyde_gui;
using namespace stw_opensyde_core;

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
C_SdNdeDataPoolListDataSetDelegate::C_SdNdeDataPoolListDataSetDelegate(QObject * const opc_Parent) :
   QStyledItemDelegate(opc_Parent),
   mpc_Model(NULL),
   ms32_HoveredCol(-1)
{
   connect(this, &C_SdNdeDataPoolListDataSetDelegate::SigStore, this, &C_SdNdeDataPoolListDataSetDelegate::m_Store);
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
QWidget * C_SdNdeDataPoolListDataSetDelegate::createEditor(QWidget * const opc_Parent,
                                                           const QStyleOptionViewItem & orc_Option,
                                                           const QModelIndex & orc_Index) const
{
   QWidget * pc_Retval = NULL;

   Q_UNUSED(orc_Option)
   if (orc_Index.isValid() == true)
   {
      C_OgeLeTable * pc_LineEdit;
      const C_SdNdeDataPoolListDataSetModel::E_Rows e_Row = C_SdNdeDataPoolListDataSetModel::h_RowToEnum(
         orc_Index.row());
      switch (e_Row)
      {
      case C_SdNdeDataPoolListDataSetModel::E_Rows::eNAME:
         pc_LineEdit = new C_OgeLeTable(opc_Parent);
         //Ui restriction
         pc_LineEdit->setMaxLength(msn_C_ITEM_MAX_CHAR_COUNT);
         pc_Retval = pc_LineEdit;
         //lint -e{64,918,1025,1703} Qt interface
         connect(pc_LineEdit, &C_OgeLeTable::textChanged, this, &C_SdNdeDataPoolListDataSetDelegate::m_OnNameChange);
         break;
      case C_SdNdeDataPoolListDataSetModel::E_Rows::eCOMMENT:
         pc_Retval = new C_OgeTedTable(opc_Parent);
         break;
      default:
         break;
      }
   }
   Q_EMIT this->SigStore(orc_Index);
   return pc_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Overwritten set editor data event slot

   Here: Pass relevant data

   \param[in,out] opc_Editor Editor widget
   \param[in]     orc_Index  Correlating index
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDataPoolListDataSetDelegate::setEditorData(QWidget * const opc_Editor, const QModelIndex & orc_Index) const
{
   if ((opc_Editor != NULL) && (orc_Index.isValid() == true))
   {
      //lint -e{929}  false positive in PC-Lint: allowed by MISRA 5-2-2
      QLineEdit * const pc_LineEdit = dynamic_cast<QLineEdit * const>(opc_Editor);
      //lint -e{929}  false positive in PC-Lint: allowed by MISRA 5-2-2
      QTextEdit * const pc_TextEdit = dynamic_cast<QTextEdit * const>(opc_Editor);
      const C_SdNdeDataPoolListDataSetModel::E_Rows e_Row =
         C_SdNdeDataPoolListDataSetModel::h_RowToEnum(orc_Index.row());
      switch (e_Row)
      {
      case C_SdNdeDataPoolListDataSetModel::E_Rows::eNAME:
         if (pc_LineEdit != NULL)
         {
            pc_LineEdit->setText(orc_Index.data().toString());
         }
         break;
      case C_SdNdeDataPoolListDataSetModel::E_Rows::eCOMMENT:
         if (pc_TextEdit != NULL)
         {
            pc_TextEdit->setText(orc_Index.data().toString());
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
void C_SdNdeDataPoolListDataSetDelegate::setModelData(QWidget * const opc_Editor, QAbstractItemModel * const opc_Model,
                                                      const QModelIndex & orc_Index) const
{
   if (((opc_Editor != NULL) && (opc_Model != NULL)) && (orc_Index.isValid() == true))
   {
      //lint -e{929}  false positive in PC-Lint: allowed by MISRA 5-2-2
      const QLineEdit * const pc_LineEdit = dynamic_cast<const QLineEdit * const>(opc_Editor);
      //lint -e{929}  false positive in PC-Lint: allowed by MISRA 5-2-2
      const QTextEdit * const pc_TextEdit = dynamic_cast<const QTextEdit * const>(opc_Editor);
      const C_SdNdeDataPoolListDataSetModel::E_Rows e_Row =
         C_SdNdeDataPoolListDataSetModel::h_RowToEnum(orc_Index.row());
      switch (e_Row)
      {
      case C_SdNdeDataPoolListDataSetModel::E_Rows::eNAME:
         if (pc_LineEdit != NULL)
         {
            opc_Model->setData(orc_Index, pc_LineEdit->text());
         }
         break;
      case C_SdNdeDataPoolListDataSetModel::E_Rows::eCOMMENT:
         if (pc_TextEdit != NULL)
         {
            opc_Model->setData(orc_Index, pc_TextEdit->toPlainText());
         }
         break;
      default:
         break;
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Paint item

   Here: special handling for boolean & deactivated cells

   \param[in,out] opc_Painter Painter
   \param[in]     orc_Option  Option
   \param[in]     orc_Index   Index
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDataPoolListDataSetDelegate::paint(QPainter * const opc_Painter, const QStyleOptionViewItem & orc_Option,
                                               const QModelIndex & orc_Index) const
{
   QStyledItemDelegate::paint(opc_Painter, orc_Option, orc_Index);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set hovered row index

   \param[in] orc_Value New hovered row index

   \return
   true  Change
   false No change
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_SdNdeDataPoolListDataSetDelegate::SetHoveredCol(const stw_types::sint32 & ors32_Value)
{
   bool q_Retval;

   if (this->ms32_HoveredCol == ors32_Value)
   {
      q_Retval = false;
   }
   else
   {
      q_Retval = true;
   }
   this->ms32_HoveredCol = ors32_Value;
   return q_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set model for column look up

   \param[in] opc_Model  Model for column look up
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDataPoolListDataSetDelegate::SetModel(const C_SdNdeDataPoolListDataSetModel * const opc_Value)
{
   this->mpc_Model = opc_Value;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Store current index

   \param[in] orc_Index Index
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDataPoolListDataSetDelegate::m_Store(const QModelIndex & orc_Index)
{
   this->mc_Edit = orc_Index;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Register name change

   \param[in] orc_Text New text
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDataPoolListDataSetDelegate::m_OnNameChange(const QString & orc_Text) const
{
   //lint -e{929}  false positive in PC-Lint: allowed by MISRA 5-2-2
   QWidget * const pc_Widget = dynamic_cast<QWidget * const>(this->sender());

   if ((pc_Widget != NULL) && (this->mpc_Model != NULL))
   {
      //Check name
      const bool q_Valid = this->mpc_Model->CheckName(this->mc_Edit.column(), orc_Text);
      C_OgeWiUtil::h_ApplyStylesheetProperty(pc_Widget, "Valid", q_Valid);
   }
}
