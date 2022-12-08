//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Node data pool list array edit model (implementation)

   Node data pool list array edit model

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.hpp"

#include "C_Uti.hpp"
#include "stwtypes.hpp"
#include "TglUtils.hpp"
#include "stwerrors.hpp"
#include "constants.hpp"
#include "C_GtGetText.hpp"
#include "C_GiSvDaParam.hpp"
#include "C_PuiSdHandler.hpp"
#include "C_PuiSvHandler.hpp"
#include "C_SyvDaItPaArModel.hpp"
#include "C_SdNdeDpUtil.hpp"
#include "C_SdNdeDpContentUtil.hpp"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw::tgl;
using namespace stw::errors;
using namespace stw::opensyde_gui;
using namespace stw::opensyde_core;
using namespace stw::opensyde_gui_logic;

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
   \param[in]  oq_EcuValues         Optional flag if the shown values are ECU values
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaItPaArModel::SetElement(const uint32_t ou32_ElementIndex,
                                    C_PuiSvDbDataElementHandler * const opc_DataWidget, const bool oq_EcuValues)
{
   beginResetModel();
   this->mq_ECUValues = oq_EcuValues;
   this->mu32_ElementIndex = ou32_ElementIndex;
   this->mpc_DataWidget = opc_DataWidget;
   endResetModel();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get header data

   \param[in]  os32_Section       Section
   \param[in]  oe_Orientation    Orientation
   \param[in]  os32_Role          Role

   \return
   Header string
*/
//----------------------------------------------------------------------------------------------------------------------
QVariant C_SyvDaItPaArModel::headerData(const int32_t os32_Section, const Qt::Orientation oe_Orientation,
                                        const int32_t os32_Role) const
{
   QVariant c_Retval = QAbstractTableModel::headerData(os32_Section, oe_Orientation, os32_Role);

   if (oe_Orientation == Qt::Orientation::Vertical)
   {
      if (os32_Role == static_cast<int32_t>(Qt::DisplayRole))
      {
         c_Retval = C_GtGetText::h_GetText("Value");
      }
      else if (os32_Role == static_cast<int32_t>(Qt::TextAlignmentRole))
      {
         c_Retval = static_cast<QVariant>(Qt::AlignLeft | Qt::AlignVCenter);
      }
      else
      {
         //No special handling
      }
   }
   else
   {
      if (os32_Role == static_cast<int32_t>(Qt::DisplayRole))
      {
         c_Retval = QString::number(os32_Section);
      }
      else if (os32_Role == static_cast<int32_t>(Qt::TextAlignmentRole))
      {
         c_Retval = static_cast<QVariant>(Qt::AlignHCenter | Qt::AlignVCenter);
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
int32_t C_SyvDaItPaArModel::rowCount(const QModelIndex & orc_Parent) const
{
   int32_t s32_Retval = 0;

   if (!orc_Parent.isValid())
   {
      //For table parent should always be invalid
      s32_Retval = 1;
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get table column count

   \param[in]  orc_Parent  Parent

   \return
   Column count
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_SyvDaItPaArModel::columnCount(const QModelIndex & orc_Parent) const
{
   int32_t s32_Retval = 0;

   if (!orc_Parent.isValid())
   {
      const C_OscNodeDataPoolContent * const pc_Element = this->GetElementData();
      if (pc_Element != NULL)
      {
         //For table parent should always be invalid
         s32_Retval = pc_Element->GetArraySize();
      }
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get data at index

   \param[in]  orc_Index   Index
   \param[in]  os32_Role    Data role

   \return
   Data
*/
//----------------------------------------------------------------------------------------------------------------------
QVariant C_SyvDaItPaArModel::data(const QModelIndex & orc_Index, const int32_t os32_Role) const
{
   QVariant c_Retval;

   if (orc_Index.isValid() == true)
   {
      if ((os32_Role == static_cast<int32_t>(Qt::DisplayRole)) || (os32_Role == static_cast<int32_t>(Qt::EditRole)))
      {
         if (orc_Index.column() >= 0)
         {
            const C_OscNodeDataPoolListElement * const pc_Element = this->GetOscElement();
            if (pc_Element != NULL)
            {
               if (this->mq_ECUValues == true)
               {
                  c_Retval = C_SdNdeDpContentUtil::h_ConvertScaledContentToGeneric(pc_Element->c_NvmValue,
                                                                                   pc_Element->f64_Factor,
                                                                                   pc_Element->f64_Offset,
                                                                                   static_cast<uint32_t>(orc_Index.
                                                                                                         column()),
                                                                                   false);
               }
               else
               {
                  const C_OscNodeDataPoolContent * const pc_Data = this->GetElementData();
                  if (pc_Data != NULL)
                  {
                     c_Retval = C_SdNdeDpContentUtil::h_ConvertScaledContentToGeneric(*pc_Data,
                                                                                      pc_Element->f64_Factor,
                                                                                      pc_Element->f64_Offset,
                                                                                      static_cast<uint32_t>(
                                                                                         orc_Index.
                                                                                         column()), false);
                  }
               }
            }
         }
      }
      else if (os32_Role == static_cast<int32_t>(Qt::TextAlignmentRole))
      {
         c_Retval = static_cast<QVariant>(Qt::AlignHCenter | Qt::AlignVCenter);
      }
      else if (os32_Role == static_cast<int32_t>(Qt::FontRole))
      {
         QFont c_Font = C_Uti::h_GetFontPixel(mc_STYLE_GUIDE_FONT_REGULAR_12);
         if (this->m_CheckError(orc_Index))
         {
            c_Font.setBold(true);
         }
         c_Retval = c_Font;
      }
      else if (os32_Role == static_cast<int32_t>(Qt::ForegroundRole))
      {
         //Default
         c_Retval = mc_STYLE_GUIDE_COLOR_6;
         if (this->m_CheckError(orc_Index))
         {
            c_Retval = mc_STYLE_GUIDE_COLOR_18;
         }
      }
      else
      {
         // no special handling here
      }
   }
   return c_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set data at index

   \param[in]  orc_Index   Index
   \param[in]  orc_Value   New data
   \param[in]  os32_Role    Data role

   \return
   true  success
   false failure
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_SyvDaItPaArModel::setData(const QModelIndex & orc_Index, const QVariant & orc_Value, const int32_t os32_Role)
{
   bool q_Retval = false;

   if (data(orc_Index, os32_Role) != orc_Value)
   {
      if (os32_Role == static_cast<int32_t>(Qt::EditRole))
      {
         if (orc_Index.column() >= 0)
         {
            const C_GiSvDaParam * const pc_ParamWidget =
               dynamic_cast<const C_GiSvDaParam * const>(this->mpc_DataWidget);

            if (pc_ParamWidget != NULL)
            {
               const C_PuiSvDbParam * const pc_Param = pc_ParamWidget->GetParamItem();
               if (pc_Param != NULL)
               {
                  const C_OscNodeDataPoolListElement * const pc_OscElement = this->GetOscElement();
                  C_PuiSvDbParam c_Copy = *pc_Param;
                  if ((this->mu32_ElementIndex < c_Copy.c_ListValues.size()) && (pc_OscElement != NULL))
                  {
                     const uint32_t u32_Index = static_cast<uint32_t>(orc_Index.column());
                     C_OscNodeDataPoolContent & rc_Content = c_Copy.c_ListValues[this->mu32_ElementIndex];
                     C_SdNdeDpContentUtil::h_SetDataVariableFromGenericWithScaling(orc_Value, rc_Content,
                                                                                   pc_OscElement->f64_Factor,
                                                                                   pc_OscElement->f64_Offset,
                                                                                   u32_Index);
                     tgl_assert(pc_ParamWidget->SetParamItem(c_Copy) == C_NO_ERR);
                     q_Retval = true;
                  }
               }
            }

            Q_EMIT this->dataChanged(orc_Index, orc_Index, QVector<int32_t>() << os32_Role);
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
void C_SyvDaItPaArModel::HandleDataChange(const uint32_t & oru32_Column)
{
   Q_EMIT this->dataChanged(this->index(0, oru32_Column), this->index(0, oru32_Column));
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get type of current data

   \return
   Type of current data
*/
//----------------------------------------------------------------------------------------------------------------------
C_OscNodeDataPoolContent::E_Type C_SyvDaItPaArModel::GetType(void) const
{
   C_OscNodeDataPoolContent::E_Type e_Retval = C_OscNodeDataPoolContent::eUINT8;
   const C_PuiSvDbNodeDataPoolListElementId * const pc_Id = this->m_GetElementId();
   if (pc_Id != NULL)
   {
      const C_OscNodeDataPoolListElement * const pc_OscElement =
         C_PuiSdHandler::h_GetInstance()->GetOscDataPoolListElement(pc_Id->u32_NodeIndex,
                                                                    pc_Id->u32_DataPoolIndex,
                                                                    pc_Id->u32_ListIndex,
                                                                    pc_Id->u32_ElementIndex);
      if (pc_OscElement != NULL)
      {
         e_Retval = pc_OscElement->GetType();
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
const C_OscNodeDataPoolListElement * C_SyvDaItPaArModel::GetOscElement(void) const
{
   const C_OscNodeDataPoolListElement * pc_Retval = NULL;
   const C_PuiSvDbNodeDataPoolListElementId * const pc_Id = this->m_GetElementId();

   if (pc_Id != NULL)
   {
      pc_Retval = C_PuiSdHandler::h_GetInstance()->GetOscDataPoolListElement(pc_Id->u32_NodeIndex,
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
const C_OscNodeDataPoolContent * C_SyvDaItPaArModel::GetElementData(void) const
{
   const C_OscNodeDataPoolContent * pc_Retval = NULL;

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
int32_t C_SyvDaItPaArModel::m_GetDataSetIndex(void) const
{
   int32_t s32_Retval = -1;

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
   const C_OscNodeDataPoolListElement * const pc_Element = this->GetOscElement();
   const C_OscNodeDataPoolContent * const pc_Data = this->GetElementData();

   if (this->mq_ECUValues == false)
   {
      if ((pc_Element != NULL) && (pc_Data != NULL))
      {
         const uint32_t u32_Index = static_cast<uint32_t>(orc_Index.column());
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
