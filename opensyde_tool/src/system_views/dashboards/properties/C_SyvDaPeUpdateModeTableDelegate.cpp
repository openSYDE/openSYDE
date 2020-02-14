//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       System view dashboard data element update mode table delegate (implementation)

   System view dashboard data element update mode table delegate

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.h"

#include "constants.h"
#include "C_GtGetText.h"
#include "C_OgeCbxTable.h"
#include "C_PuiSvHandler.h"
#include "C_SdNdeDpUtil.h"
#include "C_OgeSpxFactorTable.h"
#include "C_TblTreDelegateUtil.h"
#include "C_OgeSpxInt64FactorTable.h"
#include "C_SyvDaPeUpdateModeTableModel.h"
#include "C_SyvDaPeUpdateModeTableDelegate.h"

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
/*! \brief   Default destructor

   \param[in]  ou32_ViewIndex   View index
*/
//----------------------------------------------------------------------------------------------------------------------
C_SyvDaPeUpdateModeTableDelegate::C_SyvDaPeUpdateModeTableDelegate(const uint32 ou32_ViewIndex) :
   mu32_ViewIndex(ou32_ViewIndex)
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
QWidget * C_SyvDaPeUpdateModeTableDelegate::createEditor(QWidget * const opc_Parent,
                                                         const QStyleOptionViewItem & orc_Option,
                                                         const QModelIndex & orc_Index) const
{
   QWidget * pc_Retval = NULL;

   Q_UNUSED(orc_Option)
   if (orc_Index.isValid() == true)
   {
      C_OgeCbxTable * pc_ComboBox;
      const C_PuiSvData * pc_View;
      const C_SyvDaPeUpdateModeTableModel::E_Columns e_Col = C_SyvDaPeUpdateModeTableModel::h_ColumnToEnum(
         orc_Index.column());
      switch (e_Col)
      {
      case C_SyvDaPeUpdateModeTableModel::eTRANSMISSION_MODE:
         pc_ComboBox = new C_OgeCbxTable(opc_Parent);
         //Init
         pc_ComboBox->addItem(C_GtGetText::h_GetText("Cyclic"));
         pc_ComboBox->addItem(C_GtGetText::h_GetText("On Change"));
         pc_ComboBox->addItem(C_GtGetText::h_GetText("On Trigger"));
         pc_ComboBox->ResizeViewToContents();
         pc_Retval = pc_ComboBox;
         break;
      case C_SyvDaPeUpdateModeTableModel::eCYCLIC_INTERVAL:
         pc_View = C_PuiSvHandler::h_GetInstance()->GetView(this->mu32_ViewIndex);
         if (pc_View != NULL)
         {
            pc_ComboBox = new C_OgeCbxTable(opc_Parent);
            //Init
            pc_ComboBox->addItem(QString(C_GtGetText::h_GetText("Fast (%1 ms)")).arg(pc_View->GetUpdateRateFast()));
            pc_ComboBox->addItem(QString(C_GtGetText::h_GetText("Medium (%1 ms)")).arg(pc_View->GetUpdateRateMedium()));
            pc_ComboBox->addItem(QString(C_GtGetText::h_GetText("Slow (%1 ms)")).arg(pc_View->GetUpdateRateSlow()));
            pc_ComboBox->ResizeViewToContents();
            pc_Retval = pc_ComboBox;
         }
         break;
      case C_SyvDaPeUpdateModeTableModel::eTHRESHOLD:
         {
            const QVariant c_Data = orc_Index.data(static_cast<sintn>(Qt::EditRole));
            const QVariant c_Max = orc_Index.data(msn_USER_ROLE_INTERACTION_MAXIMUM_VALUE);
            if (c_Data.type() == QVariant::Type::Double)
            {
               C_OgeSpxFactorTable * const pc_SpinBox = new C_OgeSpxFactorTable(opc_Parent);
               if (c_Max.type() == QVariant::Type::Double)
               {
                  //Factor needs to be above 0
                  pc_SpinBox->SetMinimumCustom(C_OgeSpxFactor::mhf64_FACTOR_MIN);
                  pc_SpinBox->SetMaximumCustom(c_Max.toDouble());
               }
               pc_Retval = pc_SpinBox;
            }
            else if ((c_Data.type() == QVariant::Type::Int) || (c_Data.type() == QVariant::Type::LongLong))
            {
               C_OgeSpxInt64FactorTable * const pc_SpinBox = new C_OgeSpxInt64FactorTable(opc_Parent, false);
               if (c_Max.type() == QVariant::Type::LongLong)
               {
                  const QVariant c_Min(1LL);
                  pc_SpinBox->SetMinimum(c_Min);
                  pc_SpinBox->SetMaximum(c_Max);
               }
               pc_Retval = pc_SpinBox;
            }
            else if ((c_Data.type() == QVariant::Type::UInt) || (c_Data.type() == QVariant::Type::ULongLong))
            {
               C_OgeSpxInt64FactorTable * const pc_SpinBox = new C_OgeSpxInt64FactorTable(opc_Parent, true);
               if (c_Max.type() == QVariant::Type::ULongLong)
               {
                  const QVariant c_Min(1ULL);
                  pc_SpinBox->SetMinimum(c_Min);
                  pc_SpinBox->SetMaximum(c_Max);
               }
               pc_Retval = pc_SpinBox;
            }
            else
            {
               //Unknown
            }
            break;
         }
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
void C_SyvDaPeUpdateModeTableDelegate::setEditorData(QWidget * const opc_Editor, const QModelIndex & orc_Index) const
{
   if ((opc_Editor != NULL) && (orc_Index.isValid() == true))
   {
      //lint -e{929}  false positive in PC-Lint: allowed by MISRA 5-2-2
      QDoubleSpinBox * const pc_DoubleSpinBox = dynamic_cast<QDoubleSpinBox * const>(opc_Editor);
      //lint -e{929}  false positive in PC-Lint: allowed by MISRA 5-2-2
      C_OgeSpxInt64 * const pc_Int64SpinBox = dynamic_cast<C_OgeSpxInt64 * const>(opc_Editor);
      //lint -e{929}  false positive in PC-Lint: allowed by MISRA 5-2-2
      QComboBox * const pc_ComboBox = dynamic_cast<QComboBox * const>(opc_Editor);
      const C_SyvDaPeUpdateModeTableModel::E_Columns e_Col = C_SyvDaPeUpdateModeTableModel::h_ColumnToEnum(
         orc_Index.column());
      switch (e_Col)
      {
      case C_SyvDaPeUpdateModeTableModel::eTRANSMISSION_MODE:
      case C_SyvDaPeUpdateModeTableModel::eCYCLIC_INTERVAL:
         if (pc_ComboBox != NULL)
         {
            pc_ComboBox->setCurrentIndex(orc_Index.data(static_cast<sintn>(Qt::EditRole)).toInt());
         }
         break;
      case C_SyvDaPeUpdateModeTableModel::eTHRESHOLD:
         if (pc_DoubleSpinBox != NULL)
         {
            const QVariant c_Data = orc_Index.data(static_cast<sintn>(Qt::EditRole));
            switch (c_Data.type())
            {
            case QVariant::Double:
               pc_DoubleSpinBox->setValue(c_Data.toDouble());
               break;
            case QVariant::Int:
               pc_DoubleSpinBox->setValue(static_cast<float64>(c_Data.toInt()));
               break;
            case QVariant::UInt:
               pc_DoubleSpinBox->setValue(static_cast<float64>(c_Data.toUInt()));
               break;
            case QVariant::LongLong:
               pc_DoubleSpinBox->setValue(static_cast<float64>(c_Data.toLongLong()));
               break;
            case QVariant::ULongLong:
               pc_DoubleSpinBox->setValue(static_cast<float64>(c_Data.toULongLong()));
               break;
            default:
               //No handling possible
               break;
            }
         }
         if (pc_Int64SpinBox != NULL)
         {
            pc_Int64SpinBox->SetValue(orc_Index.data(static_cast<sintn>(Qt::EditRole)), false);
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
void C_SyvDaPeUpdateModeTableDelegate::setModelData(QWidget * const opc_Editor, QAbstractItemModel * const opc_Model,
                                                    const QModelIndex & orc_Index) const
{
   if (((opc_Editor != NULL) && (opc_Model != NULL)) && (orc_Index.isValid() == true))
   {
      //lint -e{929}  false positive in PC-Lint: allowed by MISRA 5-2-2
      QDoubleSpinBox * const pc_DoubleSpinBox = dynamic_cast<QDoubleSpinBox * const>(opc_Editor);
      //lint -e{929}  false positive in PC-Lint: allowed by MISRA 5-2-2
      C_OgeSpxInt64Factor * const pc_Int64SpinBox = dynamic_cast<C_OgeSpxInt64Factor * const>(opc_Editor);
      //lint -e{929}  false positive in PC-Lint: allowed by MISRA 5-2-2
      QComboBox * const pc_ComboBox = dynamic_cast<QComboBox * const>(opc_Editor);
      const C_SyvDaPeUpdateModeTableModel::E_Columns e_Col = C_SyvDaPeUpdateModeTableModel::h_ColumnToEnum(
         orc_Index.column());
      switch (e_Col)
      {
      case C_SyvDaPeUpdateModeTableModel::eTRANSMISSION_MODE:
      case C_SyvDaPeUpdateModeTableModel::eCYCLIC_INTERVAL:
         if (pc_ComboBox != NULL)
         {
            const sintn sn_NewValue = pc_ComboBox->currentIndex();
            opc_Model->setData(orc_Index, sn_NewValue);
         }
         break;
      case C_SyvDaPeUpdateModeTableModel::eTHRESHOLD:
         if (pc_DoubleSpinBox != NULL)
         {
            opc_Model->setData(orc_Index, pc_DoubleSpinBox->value());
         }
         if (pc_Int64SpinBox != NULL)
         {
            //Trigger manual interpretation to avoid accepting intermediate values
            pc_Int64SpinBox->InterpretValue();
            opc_Model->setData(orc_Index, pc_Int64SpinBox->GetValue());
         }
         break;
      default:
         break;
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Paint item

   Here: handle icons

   \param[in,out] opc_Painter Painter
   \param[in]     orc_Option  Option
   \param[in]     orc_Index   Index
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaPeUpdateModeTableDelegate::paint(QPainter * const opc_Painter, const QStyleOptionViewItem & orc_Option,
                                             const QModelIndex & orc_Index) const
{
   QStyledItemDelegate::paint(opc_Painter, orc_Option, orc_Index);
   C_TblTreDelegateUtil::h_PaintIcon(opc_Painter, orc_Option, orc_Index);
}
