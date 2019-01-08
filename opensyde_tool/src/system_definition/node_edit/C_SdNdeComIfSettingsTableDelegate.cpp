//-----------------------------------------------------------------------------
/*!
   \internal
   \file
   \brief       COM IF settings table delegate (implementation)

   COM IF settings table delegate

   \implementation
   project     openSYDE
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     20.02.2017  STW/S.Singer
   \endimplementation
*/
//-----------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------- */
#include "precomp_headers.h"

#include <QPainter>

#include "constants.h"
#include "C_SdNdeComIfSettingsTableDelegate.h"
#include "C_OgeChxTristate.h"
#include "C_OSCDeviceDefinition.h"
#include "C_OSCNode.h"
#include "C_PuiSdHandler.h"
#include "C_OgeSpxTableComIf.h"
#include "C_OgeWiUtil.h"
#include "C_SdUtil.h"

/* -- Used Namespaces ------------------------------------------------------ */
using namespace stw_opensyde_gui_logic;
using namespace stw_opensyde_gui_elements;
using namespace stw_opensyde_gui;
using namespace stw_opensyde_core;
using namespace stw_types;

/* -- Module Global Constants ---------------------------------------------- */

/* -- Types ---------------------------------------------------------------- */

/* -- Global Variables ----------------------------------------------------- */

/* -- Module Global Variables ---------------------------------------------- */

/* -- Module Global Function Prototypes ------------------------------------ */

/* -- Implementation ------------------------------------------------------- */

//-----------------------------------------------------------------------------
/*!
   \brief   Default constructor

   Set up GUI with all elements.

   \param[in,out] opc_Parent Optional pointer to parent

   \created     08.02.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
C_SdNdeComIfSettingsTableDelegate::C_SdNdeComIfSettingsTableDelegate(QObject * const opc_Parent,
                                                                     const stw_types::uint32 ou32_NodeIndex) :
   QStyledItemDelegate(opc_Parent)
{
   this->mu32_NodeIndex = ou32_NodeIndex;

   connect(this, &C_SdNdeComIfSettingsTableDelegate::SigEdit, this, &C_SdNdeComIfSettingsTableDelegate::m_StoreEdit);
}

//-----------------------------------------------------------------------------
/*!
   \brief   Overwritten create editor event slot

   Here: Create appropiate editor widget

   \param[in,out] opc_Parent Parent widget
   \param[in]     orc_Option Style options
   \param[in]     orc_Index  Correlating index

   \return
   Editor widget

   \created     20.02.2017  STW/S.Singer
*/
//-----------------------------------------------------------------------------
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
         //lint -e{64,826,918,929,1025,1703}  Qt interface
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

//-----------------------------------------------------------------------------
/*!
   \brief   Overwritten set editor data event slot

   Here: Pass relevant data

   \param[in,out] opc_Editor Editor widget
   \param[in]     orc_Index  Correlating index

   \created     20.02.2017  STW/S.Singer
*/
//-----------------------------------------------------------------------------
void C_SdNdeComIfSettingsTableDelegate::setEditorData(QWidget * const opc_Editor, const QModelIndex & orc_Index) const
{
   if ((opc_Editor != NULL) && (orc_Index.isValid() == true))
   {
      //lint -e{929}  false positive in PC-Lint: allowed by MISRA 5-2-2
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

//-----------------------------------------------------------------------------
/*!
   \brief   Overwritten set model data event slot

   Here: Pass relevant data

   \param[in,out] opc_Editor Editor widget
   \param[in,out] opc_Model  Model object
   \param[in]     orc_Index  Correlating index

   \created     20.02.2017  STW/S.Singer
*/
//-----------------------------------------------------------------------------
void C_SdNdeComIfSettingsTableDelegate::setModelData(QWidget * const opc_Editor, QAbstractItemModel * const opc_Model,
                                                     const QModelIndex & orc_Index) const
{
   if (((opc_Editor != NULL) && (opc_Model != NULL)) && (orc_Index.isValid() == true))
   {
      //lint -e{929}  false positive in PC-Lint: allowed by MISRA 5-2-2
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

//-----------------------------------------------------------------------------
/*!
   \brief   Check new node id value

   \param[in] orsn_Value New node id

   \created     08.06.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_SdNdeComIfSettingsTableDelegate::m_CheckValue(const sintn & orsn_Value) const
{
   //lint -e{929}  false positive in PC-Lint: allowed by MISRA 5-2-2
   C_OgeSpxToolTipBase * const pc_Widget = dynamic_cast<C_OgeSpxToolTipBase * const>(this->sender());

   if (pc_Widget != NULL)
   {
      const bool q_IdIsValid =
         C_PuiSdHandler::h_GetInstance()->GetOSCSystemDefinitionConst().CheckInterfaceIsAvailable(this->mu32_NodeIndex,
                                                                                                  this->mc_Edit.row(),
                                                                                                  static_cast<uint8>(
                                                                                                     orsn_Value));
      //Deactivate error tool tip on interaction item
      //if (this->mc_Edit.row() >= 0)
      //{
      //   const C_OSCNode * const pc_Node = C_PuiSdHandler::h_GetInstance()->GetOSCNodeConst(this->mu32_NodeIndex);
      //   if (pc_Node != NULL)
      //   {
      //      const uint32 u32_Interface = static_cast<uint32>(this->mc_Edit.row());
      //      if (u32_Interface < pc_Node->c_Properties.c_ComInterfaces.size())
      //      {
      //         const C_OSCNodeComInterfaceSettings & rc_Interface =
      //            pc_Node->c_Properties.c_ComInterfaces[u32_Interface];
      //         if (rc_Interface.q_IsBusConnected == true)
      //         {
      //            const C_OSCSystemBus * const pc_Bus = C_PuiSdHandler::h_GetInstance()->GetOSCBus(
      //               rc_Interface.u32_BusIndex);
      //            if (pc_Bus != NULL)
      //            {
      //               const std::vector<uint32> c_UsedIds = C_SdUtil::h_GetUsedNodeIdsForBusUniqueAndSortedAscending(
      //                  rc_Interface.u32_BusIndex, this->mu32_NodeIndex, u32_Interface);
      //               const QString c_Content = C_SdUtil::h_InitUsedIdsString(c_UsedIds,
      //                                                                       pc_Bus->c_Name.c_str(),
      //                                                                       C_GtGetText::h_GetText("bus"));
      //               pc_Widget->SetToolTipAdditionalInfo(c_Content, !q_IdIsValid);
      //            }
      //         }
      //      }
      //   }
      //}

      C_OgeWiUtil::h_ApplyStylesheetProperty(pc_Widget, "Valid", q_IdIsValid);
   }
}

//-----------------------------------------------------------------------------
/*!
   \brief   Store currently edited index

   \param[in] orc_Index Currently edited index

   \created     08.06.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_SdNdeComIfSettingsTableDelegate::m_StoreEdit(const QModelIndex & orc_Index)
{
   this->mc_Edit = orc_Index;
}
