//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       COM IF settings table delegate (implementation)

   COM IF settings table delegate

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.h"

#include <QPainter>

#include "constants.h"
#include "C_SdNdeComIfSettingsTableDelegate.h"
#include "C_OSCDeviceDefinition.h"
#include "C_OSCNode.h"
#include "C_PuiSdHandler.h"
#include "C_OgeSpxTableComIf.h"
#include "C_OgeWiUtil.h"
#include "C_SdUtil.h"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw_opensyde_gui_logic;
using namespace stw_opensyde_gui_elements;
using namespace stw_opensyde_gui;
using namespace stw_opensyde_core;
using namespace stw_types;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

/* -- Global Variables ---------------------------------------------------------------------------------------------- */

/* -- Module Global Variables --------------------------------------------------------------------------------------- */

/* -- Module Global Function Prototypes ----------------------------------------------------------------------------- */

/* -- Implementation ------------------------------------------------------------------------------------------------ */

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default constructor

   Set up GUI with all elements.

   \param[in,out] opc_Parent        Optional pointer to parent
   \param[in]     ou32_NodeIndex    Node index
*/
//----------------------------------------------------------------------------------------------------------------------
C_SdNdeComIfSettingsTableDelegate::C_SdNdeComIfSettingsTableDelegate(QObject * const opc_Parent,
                                                                     const stw_types::uint32 ou32_NodeIndex) :
   QStyledItemDelegate(opc_Parent)
{
   this->mu32_NodeIndex = ou32_NodeIndex;

   connect(this, &C_SdNdeComIfSettingsTableDelegate::SigEdit, this, &C_SdNdeComIfSettingsTableDelegate::m_StoreEdit);
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
QWidget * C_SdNdeComIfSettingsTableDelegate::createEditor(QWidget * const opc_Parent,
                                                          const QStyleOptionViewItem & orc_Option,
                                                          const QModelIndex & orc_Index) const
{
   Q_UNUSED(orc_Option)
   QWidget * pc_Retval = NULL;
   C_OgeSpxTableComIf * pc_SpinBox = NULL;
   const uint8 u8_Maximum = C_SdUtil::h_GetNodeIdMaximum(this->mu32_NodeIndex);

   if (orc_Index.isValid() == true)
   {
      switch (orc_Index.column())
      {
      case C_SdNdeComIfSettingsTableDelegate::E_Columns::eNODEID:
         pc_SpinBox = new C_OgeSpxTableComIf(opc_Parent);
         pc_SpinBox->SetMaximumCustom(u8_Maximum);
         connect(pc_SpinBox, static_cast<void (QSpinBox::*)(sintn)>(&QSpinBox::valueChanged), this,
                 &C_SdNdeComIfSettingsTableDelegate::m_CheckValue);
         pc_Retval = pc_SpinBox;
         Q_EMIT this->SigEdit(orc_Index);
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

   Here: Pass relevant data

   \param[in,out] opc_Editor Editor widget
   \param[in]     orc_Index  Correlating index
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeComIfSettingsTableDelegate::setEditorData(QWidget * const opc_Editor, const QModelIndex & orc_Index) const
{
   if ((opc_Editor != NULL) && (orc_Index.isValid() == true))
   {
      QSpinBox * const pc_SpinBoxInt = dynamic_cast<QSpinBox * const>(opc_Editor);

      switch (orc_Index.column())
      {
      case C_SdNdeComIfSettingsTableDelegate::E_Columns::eNODEID:
         if (pc_SpinBoxInt != NULL)
         {
            //pc_SpinBoxInt->setAlignment(Qt::AlignCenter);
            pc_SpinBoxInt->setValue(orc_Index.data().toInt());
         }
         break;
      default:
         QStyledItemDelegate::setEditorData(opc_Editor, orc_Index);
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
void C_SdNdeComIfSettingsTableDelegate::setModelData(QWidget * const opc_Editor, QAbstractItemModel * const opc_Model,
                                                     const QModelIndex & orc_Index) const
{
   if (((opc_Editor != NULL) && (opc_Model != NULL)) && (orc_Index.isValid() == true))
   {
      QSpinBox * const pc_SpinBoxInt = dynamic_cast<QSpinBox * const>(opc_Editor);

      switch (orc_Index.column())
      {
      case C_SdNdeComIfSettingsTableDelegate::E_Columns::eNODEID:
         if (pc_SpinBoxInt != NULL)
         {
            opc_Model->setData(orc_Index, pc_SpinBoxInt->value());
         }
         break;
      default:
         QStyledItemDelegate::setModelData(opc_Editor, opc_Model, orc_Index);
         break;
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Check new node id value

   \param[in] orsn_Value New node id
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeComIfSettingsTableDelegate::m_CheckValue(const sintn & orsn_Value) const
{
   C_OgeSpxToolTipBase * const pc_Widget = dynamic_cast<C_OgeSpxToolTipBase * const>(this->sender());

   if (pc_Widget != NULL)
   {
      const bool q_IdIsValid =
         C_PuiSdHandler::h_GetInstance()->GetOSCSystemDefinitionConst().CheckInterfaceIsAvailable(this->mu32_NodeIndex,
                                                                                                  this->mc_Edit.row(),
                                                                                                  static_cast<uint8>(
                                                                                                     orsn_Value));
      C_OgeWiUtil::h_ApplyStylesheetProperty(pc_Widget, "Valid", q_IdIsValid);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Store currently edited index

   \param[in] orc_Index Currently edited index
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeComIfSettingsTableDelegate::m_StoreEdit(const QModelIndex & orc_Index)
{
   this->mc_Edit = orc_Index;
}
