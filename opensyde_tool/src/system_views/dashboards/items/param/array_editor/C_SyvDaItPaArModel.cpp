//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Node data pool list array edit model (implementation)

   Node data pool list array edit model

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.h"

#include "C_Uti.h"
#include "stwtypes.h"
#include "TGLUtils.h"
#include "stwerrors.h"
#include "constants.h"
#include "C_GtGetText.h"
#include "C_GiSvDaParam.h"
#include "C_PuiSdHandler.h"
#include "C_PuiSvHandler.h"
#include "C_SyvDaItPaArModel.h"
#include "C_SdNdeDpUtil.h"
#include "C_SdNdeDpContentUtil.h"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw_tgl;
using namespace stw_types;
using namespace stw_errors;
using namespace stw_opensyde_gui;
using namespace stw_opensyde_core;
using namespace stw_opensyde_gui_logic;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

/* -- Global Variables ---------------------------------------------------------------------------------------------- */

/* -- Module Global Variables --------------------------------------------------------------------------------------- */

/* -- Module Global Function Prototypes ----------------------------------------------------------------------------- */

/* -- Implementation ------------------------------------------------------------------------------------------------ */

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default constructor

   Set up GUI with all elements.

   \param[in,out]  opc_Parent    Optional pointer to parent
*/
//----------------------------------------------------------------------------------------------------------------------
C_SyvDaItPaArModel::C_SyvDaItPaArModel(QObject * const opc_Parent) :
   QAbstractTableModel(opc_Parent),
   mq_ECUValues(false),
   mu32_ElementIndex(0),
   mpc_DataWidget(NULL)
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Specify associated list

   \param[in]  ou32_ElementIndex    Element index
   \param[in]  opc_DataWidget       Data widget
   \param[in]  oq_ECUValues         Optional flag if the shown values are ECU values
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaItPaArModel::SetElement(const uint32 ou32_ElementIndex, C_PuiSvDbDataElementHandler * const opc_DataWidget,
                                    const bool oq_ECUValues)
{
   beginResetModel();
   this->mq_ECUValues = oq_ECUValues;
   this->mu32_ElementIndex = ou32_ElementIndex;
   this->mpc_DataWidget = opc_DataWidget;
   endResetModel();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get header data

   \param[in]  osn_Section       Section
   \param[in]  oe_Orientation    Orientation
   \param[in]  osn_Role          Role

   \return
   Header string
*/
//----------------------------------------------------------------------------------------------------------------------
QVariant C_SyvDaItPaArModel::headerData(const sintn osn_Section, const Qt::Orientation oe_Orientation,
                                        const sintn osn_Role) const
{
   QVariant c_Retval = QAbstractTableModel::headerData(osn_Section, oe_Orientation, osn_Role);

   if (oe_Orientation == Qt::Orientation::Vertical)
   {
      if (osn_Role == static_cast<sintn>(Qt::DisplayRole))
      {
         c_Retval = C_GtGetText::h_GetText("Value");
      }
      else if (osn_Role == static_cast<sintn>(Qt::TextAlignmentRole))
      {
         c_Retval = QVariant(Qt::AlignLeft | Qt::AlignVCenter);
      }
      else if (osn_Role == static_cast<sintn>(Qt::FontRole))
      {
         c_Retval = C_Uti::h_GetFontPixel(mc_STYLE_GUIDE_FONT_REGULAR_13);
      }
      else
      {
         //No special handling
      }
   }
   else
   {
      if (osn_Role == static_cast<sintn>(Qt::DisplayRole))
      {
         c_Retval = QString::number(osn_Section);
      }
      else if (osn_Role == static_cast<sintn>(Qt::FontRole))
      {
         c_Retval = C_Uti::h_GetFontPixel(mc_STYLE_GUIDE_FONT_REGULAR_13);
      }
      else if (osn_Role == static_cast<sintn>(Qt::TextAlignmentRole))
      {
         c_Retval = QVariant(Qt::AlignHCenter | Qt::AlignVCenter);
      }
      else
      {
         //No special handling
      }
   }
   return c_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get table row count

   \param[in]  orc_Parent  Parent

   \return
   Row count
*/
//----------------------------------------------------------------------------------------------------------------------
sintn C_SyvDaItPaArModel::rowCount(const QModelIndex & orc_Parent) const
{
   sintn sn_Retval = 0;

   if (!orc_Parent.isValid())
   {
      //For table parent should always be invalid
      sn_Retval = 1;
   }
   return sn_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get table column count

   \param[in]  orc_Parent  Parent

   \return
   Column count
*/
//----------------------------------------------------------------------------------------------------------------------
sintn C_SyvDaItPaArModel::columnCount(const QModelIndex & orc_Parent) const
{
   stw_types::sintn sn_Retval = 0;
   if (!orc_Parent.isValid())
   {
      const C_OSCNodeDataPoolContent * const pc_Element = this->GetElementData();
      if (pc_Element != NULL)
      {
         //For table parent should always be invalid
         sn_Retval = pc_Element->GetArraySize();
      }
   }
   return sn_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get data at index

   \param[in]  orc_Index   Index
   \param[in]  osn_Role    Data role

   \return
   Data
*/
//----------------------------------------------------------------------------------------------------------------------
QVariant C_SyvDaItPaArModel::data(const QModelIndex & orc_Index, const sintn osn_Role) const
{
   QVariant c_Retval;

   if (orc_Index.isValid() == true)
   {
      if ((osn_Role == static_cast<sintn>(Qt::DisplayRole)) || (osn_Role == static_cast<sintn>(Qt::EditRole)))
      {
         if (orc_Index.column() >= 0)
         {
            const C_OSCNodeDataPoolListElement * const pc_Element = this->GetOSCElement();
            if (pc_Element != NULL)
            {
               if (this->mq_ECUValues == true)
               {
                  c_Retval = C_SdNdeDpContentUtil::h_ConvertScaledContentToGeneric(pc_Element->c_NvmValue,
                                                                                   pc_Element->f64_Factor,
                                                                                   pc_Element->f64_Offset,
                                                                                   static_cast<uint32>(orc_Index.
                                                                                                       column()));
               }
               else
               {
                  const C_OSCNodeDataPoolContent * const pc_Data = this->GetElementData();
                  if (pc_Data != NULL)
                  {
                     c_Retval = C_SdNdeDpContentUtil::h_ConvertScaledContentToGeneric(*pc_Data,
                                                                                      pc_Element->f64_Factor,
                                                                                      pc_Element->f64_Offset,
                                                                                      static_cast<uint32>(
                                                                                         orc_Index.
                                                                                         column()));
                  }
               }
            }
         }
      }
      else if (osn_Role == static_cast<sintn>(Qt::TextAlignmentRole))
      {
         c_Retval = QVariant(Qt::AlignHCenter | Qt::AlignVCenter);
      }
      else if (osn_Role == static_cast<sintn>(Qt::FontRole))
      {
         QFont c_Font = C_Uti::h_GetFontPixel(mc_STYLE_GUIDE_FONT_REGULAR_13);
         if (this->m_CheckError(orc_Index))
         {
            c_Font.setBold(true);
         }
         c_Retval = c_Font;
      }
      else if (osn_Role == static_cast<sintn>(Qt::ForegroundRole))
      {
         //Default
         c_Retval = mc_STYLE_GUIDE_COLOR_6;
         if (this->m_CheckError(orc_Index))
         {
            c_Retval = mc_STYLE_GUIDE_COLOR_24;
         }
      }
   }
   return c_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set data at index

   \param[in]  orc_Index   Index
   \param[in]  orc_Value   New data
   \param[in]  osn_Role    Data role

   \return
   true  success
   false failure
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_SyvDaItPaArModel::setData(const QModelIndex & orc_Index, const QVariant & orc_Value, const sintn osn_Role)
{
   bool q_Retval = false;

   if (data(orc_Index, osn_Role) != orc_Value)
   {
      if (osn_Role == static_cast<sintn>(Qt::EditRole))
      {
         if (orc_Index.column() >= 0)
         {
            //lint -e{929}  false positive in PC-Lint: allowed by MISRA 5-2-2
            const C_GiSvDaParam * const pc_ParamWidget =
               dynamic_cast<const C_GiSvDaParam * const>(this->mpc_DataWidget);

            if (pc_ParamWidget != NULL)
            {
               const C_PuiSvDbParam * const pc_Param = pc_ParamWidget->GetParamItem();
               if (pc_Param != NULL)
               {
                  const C_OSCNodeDataPoolListElement * const pc_OSCElement = this->GetOSCElement();
                  C_PuiSvDbParam c_Copy = *pc_Param;
                  if ((this->mu32_ElementIndex < c_Copy.c_ListValues.size()) && (pc_OSCElement != NULL))
                  {
                     const uint32 u32_Index = static_cast<uint32>(orc_Index.column());
                     C_OSCNodeDataPoolContent & rc_Content = c_Copy.c_ListValues[this->mu32_ElementIndex];
                     C_SdNdeDpContentUtil::h_SetDataVariableFromGenericWithScaling(orc_Value, rc_Content,
                                                                                   pc_OSCElement->f64_Factor,
                                                                                   pc_OSCElement->f64_Offset,
                                                                                   u32_Index);
                     tgl_assert(pc_ParamWidget->SetParamItem(c_Copy) == C_NO_ERR);
                  }
               }
            }

            //lint -e{1793} Qt example
            Q_EMIT this->dataChanged(orc_Index, orc_Index, QVector<stw_types::sintn>() << osn_Role);
         }
      }
   }

   return q_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get flags for item

   \param[in]  orc_Index   Item

   \return
   Flags for item
*/
//----------------------------------------------------------------------------------------------------------------------
Qt::ItemFlags C_SyvDaItPaArModel::flags(const QModelIndex & orc_Index) const
{
   Qt::ItemFlags c_Retval;
   if (orc_Index.isValid() == true)
   {
      //Each item
      c_Retval = QAbstractTableModel::flags(orc_Index) | Qt::ItemIsEnabled | Qt::ItemIsSelectable;
      if ((this->mq_ECUValues == false) && (this->m_GetDataSetIndex() < 0))
      {
         c_Retval = c_Retval | Qt::ItemIsEditable;
      }
   }
   else
   {
      c_Retval = Qt::NoItemFlags;
   }
   return c_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Signal model update

   \param[in]  oru32_Column   Changed column
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaItPaArModel::HandleDataChange(const uint32 & oru32_Column)
{
   Q_EMIT this->dataChanged(this->index(0, oru32_Column), this->index(0, oru32_Column));
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get type of current data

   \return
   Type of current data
*/
//----------------------------------------------------------------------------------------------------------------------
C_OSCNodeDataPoolContent::E_Type C_SyvDaItPaArModel::GetType(void) const
{
   C_OSCNodeDataPoolContent::E_Type e_Retval = C_OSCNodeDataPoolContent::eUINT8;
   const C_PuiSvDbNodeDataPoolListElementId * const pc_Id = this->m_GetElementId();
   if (pc_Id != NULL)
   {
      const C_OSCNodeDataPoolListElement * const pc_OSCElement =
         C_PuiSdHandler::h_GetInstance()->GetOSCDataPoolListElement(pc_Id->u32_NodeIndex,
                                                                    pc_Id->u32_DataPoolIndex,
                                                                    pc_Id->u32_ListIndex,
                                                                    pc_Id->u32_ElementIndex);
      if (pc_OSCElement != NULL)
      {
         e_Retval = pc_OSCElement->GetType();
      }
   }
   return e_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get osc node data pool list element

   \return
   OSC node data pool list element
   Else NULL
*/
//----------------------------------------------------------------------------------------------------------------------
const C_OSCNodeDataPoolListElement * C_SyvDaItPaArModel::GetOSCElement(void) const
{
   const C_OSCNodeDataPoolListElement * pc_Retval = NULL;
   const C_PuiSvDbNodeDataPoolListElementId * const pc_Id = this->m_GetElementId();

   if (pc_Id != NULL)
   {
      pc_Retval = C_PuiSdHandler::h_GetInstance()->GetOSCDataPoolListElement(pc_Id->u32_NodeIndex,
                                                                             pc_Id->u32_DataPoolIndex,
                                                                             pc_Id->u32_ListIndex,
                                                                             pc_Id->u32_ElementIndex);
   }
   return pc_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get data element

   \return
   Data element
   Else NULL
*/
//----------------------------------------------------------------------------------------------------------------------
const C_OSCNodeDataPoolContent * C_SyvDaItPaArModel::GetElementData(void) const
{
   const C_OSCNodeDataPoolContent * pc_Retval = NULL;
   //lint -e{929}  false positive in PC-Lint: allowed by MISRA 5-2-2
   const C_GiSvDaParam * const pc_ParamWidget =
      dynamic_cast<const C_GiSvDaParam * const>(this->mpc_DataWidget);

   if (pc_ParamWidget != NULL)
   {
      const C_PuiSvDbParam * const pc_Param = pc_ParamWidget->GetParamItem();
      if (pc_Param != NULL)
      {
         if (this->mu32_ElementIndex < pc_Param->c_ListValues.size())
         {
            pc_Retval = &pc_Param->c_ListValues[this->mu32_ElementIndex];
         }
      }
   }
   return pc_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Register model data reset
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaItPaArModel::Reset(void)
{
   this->beginResetModel();
   this->endResetModel();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get current generic element ID

   \return
   NULL Generic element ID not found
   Else Valid generic element ID
*/
//----------------------------------------------------------------------------------------------------------------------
const C_PuiSvDbNodeDataPoolListElementId * C_SyvDaItPaArModel::m_GetElementId(void) const
{
   const C_PuiSvDbNodeDataPoolListElementId * pc_Retval = NULL;
   //lint -e{929}  false positive in PC-Lint: allowed by MISRA 5-2-2
   const C_GiSvDaParam * const pc_ParamWidget =
      dynamic_cast<const C_GiSvDaParam * const>(this->mpc_DataWidget);

   if (pc_ParamWidget != NULL)
   {
      const C_PuiSvDbParam * const pc_Param = pc_ParamWidget->GetParamItem();
      if (pc_Param != NULL)
      {
         if (this->mu32_ElementIndex < pc_Param->c_DataPoolElementsConfig.size())
         {
            const C_PuiSvDbNodeDataElementConfig & rc_Config =
               pc_Param->c_DataPoolElementsConfig[this->mu32_ElementIndex];
            pc_Retval = &rc_Config.c_ElementId;
         }
      }
   }
   return pc_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Check if table editable

   \return
   True  Editable
   False Not editable
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_SyvDaItPaArModel::m_GetDataSetIndex(void) const
{
   sint32 s32_Retval = -1;
   //lint -e{929}  false positive in PC-Lint: allowed by MISRA 5-2-2
   const C_GiSvDaParam * const pc_ParamWidget =
      dynamic_cast<const C_GiSvDaParam * const>(this->mpc_DataWidget);

   if (pc_ParamWidget != NULL)
   {
      const C_PuiSvDbParam * const pc_Param = pc_ParamWidget->GetParamItem();
      if ((pc_Param != NULL) && (this->mu32_ElementIndex < pc_Param->c_DataSetSelectionIndices.size()))
      {
         s32_Retval = pc_Param->c_DataSetSelectionIndices[this->mu32_ElementIndex];
      }
   }

   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Check error for current cell

   \param[in]  orc_Index   Index to check the error for

   \return
   True  Error detected
   False No error detected
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_SyvDaItPaArModel::m_CheckError(const QModelIndex & orc_Index) const
{
   bool q_Error;
   const C_OSCNodeDataPoolListElement * const pc_Element = this->GetOSCElement();
   const C_OSCNodeDataPoolContent * const pc_Data = this->GetElementData();

   if (this->mq_ECUValues == false)
   {
      if ((pc_Element != NULL) && (pc_Data != NULL))
      {
         const uint32 u32_Index = static_cast<uint32>(orc_Index.column());
         //Check error min
         q_Error = pc_Element->c_MinValue.CompareArrayGreater(*pc_Data, u32_Index);
         //Check error max (don't disable min error)
         q_Error = q_Error || (pc_Element->c_MaxValue.CompareArrayGreaterOrEqual(*pc_Data, u32_Index) == false);
      }
      else
      {
         q_Error = true;
      }
   }
   else
   {
      q_Error = false;
   }
   return q_Error;
}
