//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Data pool list element data change undo command (implementation)

   Data pool list element data change undo command

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.hpp"

#include <limits>
#include <QChar>
#include "stwtypes.hpp"
#include "stwerrors.hpp"
#include "C_SdNdeUnoLeDataPoolListElementDataChangeCommand.hpp"
#include "C_PuiSdHandler.hpp"
#include "TglUtils.hpp"
#include "C_SdNdeDpContentUtil.hpp"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw::tgl;
using namespace stw::errors;
using namespace stw::opensyde_gui_logic;
using namespace stw::opensyde_core;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

/* -- Global Variables ---------------------------------------------------------------------------------------------- */

/* -- Module Global Variables --------------------------------------------------------------------------------------- */

/* -- Module Global Function Prototypes ----------------------------------------------------------------------------- */

/* -- Implementation ------------------------------------------------------------------------------------------------ */

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default constructor

   \param[in]     oru32_NodeIndex                  Node index
   \param[in]     oru32_DataPoolIndex              Node data pool index
   \param[in]     oru32_DataPoolListIndex          Node data pool list index
   \param[in,out] opc_DataPoolListModelViewManager Data pool lists model view manager to get objects to perform actions on
   \param[in]     oru32_DataPoolListElementIndex   Node data pool list element index
   \param[in]     orc_NewData                      New data
   \param[in]     ore_DataChangeType               Data change type
   \param[in]     oru32_ArrayIndex                 Optional array index
   \param[in]     ors32_DataSetIndex               Optional data set index
   \param[in,out] opc_Parent                       Optional pointer to parent
*/
//----------------------------------------------------------------------------------------------------------------------
C_SdNdeUnoLeDataPoolListElementDataChangeCommand::C_SdNdeUnoLeDataPoolListElementDataChangeCommand(
   const uint32_t & oru32_NodeIndex, const uint32_t & oru32_DataPoolIndex, const uint32_t & oru32_DataPoolListIndex,
   C_SdNdeDpListModelViewManager * const opc_DataPoolListModelViewManager,
   const uint32_t & oru32_DataPoolListElementIndex, const QVariant & orc_NewData,
   const C_SdNdeDpUtil::E_ElementDataChangeType & ore_DataChangeType, const uint32_t & oru32_ArrayIndex,
   const int32_t & ors32_DataSetIndex, QUndoCommand * const opc_Parent) :
   C_SdNdeUnoLeDataPoolListElementBaseCommand(oru32_NodeIndex, oru32_DataPoolIndex, oru32_DataPoolListIndex,
                                              opc_DataPoolListModelViewManager,
                                              "Change List element data", opc_Parent),
   mc_PreviousData(),
   mc_NewData(orc_NewData),
   mu32_DataPoolListElementIndex(oru32_DataPoolListElementIndex),
   me_DataChangeType(ore_DataChangeType),
   mu32_ArrayIndex(oru32_ArrayIndex),
   ms32_DataSetIndex(ors32_DataSetIndex),
   mq_Initial(true)
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Redo
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeUnoLeDataPoolListElementDataChangeCommand::redo(void)
{
   m_Change(this->mc_PreviousData, this->mc_NewData);
   //Apply parent taks = auto min max after current change
   C_SdNdeUnoLeDataPoolListElementBaseCommand::redo();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Undo
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeUnoLeDataPoolListElementDataChangeCommand::undo(void)
{
   m_Change(this->mc_NewData, this->mc_PreviousData);
   //Apply parent taks = auto min max after current change
   C_SdNdeUnoLeDataPoolListElementBaseCommand::undo();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Change data values and store previous value

   \param[out] orc_PreviousData Previous data value storage
   \param[in]  orc_NewData      New data value assignment
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeUnoLeDataPoolListElementDataChangeCommand::m_Change(QVariant & orc_PreviousData,
                                                                const QVariant & orc_NewData)
{
   C_PuiSdNodeDataPoolListElement c_UiElement;
   C_OscNodeDataPoolListElement c_OscElement;

   if (C_PuiSdHandler::h_GetInstance()->GetDataPoolListElement(this->mu32_NodeIndex, this->mu32_DataPoolIndex,
                                                               this->mu32_DataPoolListIndex,
                                                               this->mu32_DataPoolListElementIndex, c_OscElement,
                                                               c_UiElement) == C_NO_ERR)
   {
      bool q_ApplyAutoMin = false;
      bool q_ApplyAutoMax = false;
      bool q_ApplyMinToDataSet = false;
      bool q_ApplyMaxToDataSet = false;
      bool q_AutoAdaptArraySize = false;
      //Copy previous value
      C_SdNdeDpUtil::h_ConvertToElementGeneric(c_OscElement, c_UiElement, this->me_DataChangeType,
                                               orc_PreviousData, this->mu32_ArrayIndex, this->ms32_DataSetIndex);
      switch (this->me_DataChangeType)
      {
      case C_SdNdeDpUtil::eELEMENT_NAME:
         c_OscElement.c_Name = orc_NewData.toString().toStdString().c_str();
         break;
      case C_SdNdeDpUtil::eELEMENT_COMMENT:
         c_OscElement.c_Comment = orc_NewData.toString().toStdString().c_str();
         break;
      case C_SdNdeDpUtil::eELEMENT_VALUE_TYPE:
         if (orc_NewData.toInt() == 10)
         {
            c_UiElement.q_InterpretAsString = true;
            c_OscElement.SetType(C_OscNodeDataPoolContent::E_Type::eSINT8);
            //Handle min max (always valid)
            if (this->mq_Initial == true)
            {
               q_ApplyAutoMin = true;
               q_ApplyAutoMax = true;
            }
            if (c_OscElement.GetArraySize() == 1)
            {
               q_AutoAdaptArraySize = true;
            }
         }
         else
         {
            if (this->mq_Initial == true)
            {
               const C_OscNodeDataPoolContent::E_Type e_NewType =
                  C_SdNdeDpUtil::h_ConvertContentTypeFromComboBox(static_cast<int8_t>(orc_NewData.toInt()));
               c_UiElement.q_InterpretAsString = false;
               if (c_UiElement.q_AutoMinMaxActive == false)
               {
                  //Manual handling necessary
                  if (c_OscElement.c_MinValue.CheckInsideRange(e_NewType) == false)
                  {
                     q_ApplyAutoMin = true;
                  }
                  if (c_OscElement.c_MaxValue.CheckInsideRange(e_NewType) == false)
                  {
                     q_ApplyAutoMax = true;
                  }
               }
               c_OscElement.SetType(e_NewType);
               //Recheck auto min max
               if (c_UiElement.q_AutoMinMaxActive == true)
               {
                  q_ApplyAutoMin = true;
                  q_ApplyAutoMax = true;
               }
            }
            else
            {
               const C_OscNodeDataPoolContent::E_Type e_NewType =
                  C_SdNdeDpUtil::h_ConvertContentTypeFromComboBox(static_cast<int8_t>(orc_NewData.toInt()));
               c_OscElement.SetType(e_NewType);
               c_UiElement.q_InterpretAsString = false;
            }
         }
         break;
      case C_SdNdeDpUtil::eELEMENT_ARRAY:
         if (orc_NewData.toInt() <= 1)
         {
            c_OscElement.SetArray(false);
         }
         else
         {
            const uint32_t u32_NewSize = static_cast<uint32_t>(orc_NewData.toInt());
            const C_OscNodeDataPoolContent c_MinVal = m_GetCurrentTypeMinGeneric();
            const C_OscNodeDataPoolContent c_MaxVal = m_GetCurrentTypeMaxGeneric();
            uint32_t u32_PrevSize;
            c_OscElement.SetArray(true);
            u32_PrevSize = c_OscElement.GetArraySize();
            c_OscElement.SetArraySize(u32_NewSize);
            //Auto min max
            for (uint32_t u32_It = u32_PrevSize; u32_It < u32_NewSize; ++u32_It)
            {
               m_CopySingleValueToArrayIndex(c_MinVal, u32_It, c_OscElement.c_MinValue);
               m_CopySingleValueToArrayIndex(c_MaxVal, u32_It, c_OscElement.c_MaxValue);
            }
         }
         break;
      case C_SdNdeDpUtil::eELEMENT_AUTO_MIN_MAX:
         c_UiElement.q_AutoMinMaxActive = orc_NewData.toBool();
         if (this->mq_Initial == true)
         {
            if (c_UiElement.q_AutoMinMaxActive == true)
            {
               q_ApplyAutoMin = true;
               q_ApplyAutoMax = true;
            }
         }
         break;
      case C_SdNdeDpUtil::eELEMENT_MIN:
         //Save scaled value
         orc_PreviousData = C_SdNdeDpContentUtil::h_ConvertScaledContentToGeneric(c_OscElement.c_MinValue,
                                                                                  c_OscElement.f64_Factor,
                                                                                  c_OscElement.f64_Offset,
                                                                                  this->mu32_ArrayIndex);
         //Update value
         C_SdNdeDpContentUtil::h_SetDataVariableFromGenericWithScaling(orc_NewData, c_OscElement.c_MinValue,
                                                                       c_OscElement.f64_Factor,
                                                                       c_OscElement.f64_Offset,
                                                                       this->mu32_ArrayIndex);
         q_ApplyMinToDataSet = true;
         break;
      case C_SdNdeDpUtil::eELEMENT_MAX:
         //Save scaled value
         orc_PreviousData = C_SdNdeDpContentUtil::h_ConvertScaledContentToGeneric(c_OscElement.c_MaxValue,
                                                                                  c_OscElement.f64_Factor,
                                                                                  c_OscElement.f64_Offset,
                                                                                  this->mu32_ArrayIndex);
         //Update value
         C_SdNdeDpContentUtil::h_SetDataVariableFromGenericWithScaling(orc_NewData, c_OscElement.c_MaxValue,
                                                                       c_OscElement.f64_Factor,
                                                                       c_OscElement.f64_Offset,
                                                                       this->mu32_ArrayIndex);
         q_ApplyMaxToDataSet = true;
         break;
      case C_SdNdeDpUtil::eELEMENT_FACTOR:
         c_OscElement.f64_Factor = orc_NewData.toDouble();
         break;
      case C_SdNdeDpUtil::eELEMENT_OFFSET:
         c_OscElement.f64_Offset = orc_NewData.toDouble();
         break;
      case C_SdNdeDpUtil::eELEMENT_UNIT:
         c_OscElement.c_Unit = orc_NewData.toString().toStdString().c_str();
         break;
      case C_SdNdeDpUtil::eELEMENT_DATA_SET:
         if ((this->ms32_DataSetIndex >= 0) &&
             (static_cast<uint32_t>(this->ms32_DataSetIndex) < c_OscElement.c_DataSetValues.size()))
         {
            if (c_UiElement.q_InterpretAsString == true)
            {
               C_SdNdeDpContentUtil::h_SetString(
                  orc_NewData.toString(), c_OscElement.c_DataSetValues[static_cast<uint32_t>(this->ms32_DataSetIndex)]);
            }
            else
            {
               const uint32_t u32_DataSetIndex = static_cast<uint32_t>(this->ms32_DataSetIndex);
               //Save scaled value
               orc_PreviousData =
                  C_SdNdeDpContentUtil::h_ConvertScaledContentToGeneric(c_OscElement.c_DataSetValues[
                                                                           u32_DataSetIndex],
                                                                        c_OscElement.f64_Factor,
                                                                        c_OscElement.f64_Offset,
                                                                        this->mu32_ArrayIndex);
               C_SdNdeDpContentUtil::h_SetDataVariableFromGenericWithScaling(orc_NewData,
                                                                             c_OscElement.c_DataSetValues[
                                                                                u32_DataSetIndex],
                                                                             c_OscElement.f64_Factor,
                                                                             c_OscElement.f64_Offset,
                                                                             this->mu32_ArrayIndex);
            }
         }
         break;
      case C_SdNdeDpUtil::eELEMENT_ACCESS:
         c_OscElement.e_Access =
            C_SdNdeDpUtil::h_ConvertElementAccesFromComboBox(static_cast<int8_t>(orc_NewData.toInt()));
         break;
      case C_SdNdeDpUtil::eELEMENT_EVENT_CALL:
         c_OscElement.q_DiagEventCall = orc_NewData.toBool();
         break;
      default:
         break;
      }
      C_PuiSdHandler::h_GetInstance()->SetDataPoolListElement(this->mu32_NodeIndex, this->mu32_DataPoolIndex,
                                                              this->mu32_DataPoolListIndex,
                                                              this->mu32_DataPoolListElementIndex,
                                                              c_OscElement,
                                                              c_UiElement);
      //Auto min max handling AFTER changes were applied (Min max should include the changes)
      if (this->mq_Initial == true)
      {
         if (q_ApplyAutoMin == true)
         {
            m_ApplyAutoMin();
         }
         if (q_ApplyAutoMax == true)
         {
            m_ApplyAutoMax();
         }
         if (q_AutoAdaptArraySize == true)
         {
            new C_SdNdeUnoLeDataPoolListElementDataChangeCommand(this->mu32_NodeIndex, this->mu32_DataPoolIndex,
                                                                 this->mu32_DataPoolListIndex,
                                                                 this->mpc_DataPoolListModelViewManager,
                                                                 this->mu32_DataPoolListElementIndex,
                                                                 static_cast<QVariant>(2),
                                                                 C_SdNdeDpUtil::eELEMENT_ARRAY,
                                                                 0, 0, this);
         }
         //Adapt data set values to min/max changes (only if necessary)
         if (q_ApplyMinToDataSet == true)
         {
            const QVariant c_NewData = C_SdNdeDpContentUtil::h_ConvertScaledContentToGeneric(
               c_OscElement.c_MinValue,
               c_OscElement.f64_Factor,
               c_OscElement.f64_Offset,
               this->mu32_ArrayIndex);
            for (uint32_t u32_ItDataSet = 0; u32_ItDataSet < c_OscElement.c_DataSetValues.size(); ++u32_ItDataSet)
            {
               if (C_SdNdeDpUtil::h_CompareSpecifiedItemSmaller(c_OscElement.c_DataSetValues[u32_ItDataSet],
                                                                c_OscElement.c_MinValue,
                                                                this->mu32_ArrayIndex) == true)
               {
                  new C_SdNdeUnoLeDataPoolListElementDataChangeCommand(this->mu32_NodeIndex, this->mu32_DataPoolIndex,
                                                                       this->mu32_DataPoolListIndex,
                                                                       this->mpc_DataPoolListModelViewManager,
                                                                       this->mu32_DataPoolListElementIndex, c_NewData,
                                                                       C_SdNdeDpUtil::eELEMENT_DATA_SET,
                                                                       this->mu32_ArrayIndex, u32_ItDataSet, this);
               }
            }
         }
         if (q_ApplyMaxToDataSet == true)
         {
            const QVariant c_NewData = C_SdNdeDpContentUtil::h_ConvertScaledContentToGeneric(
               c_OscElement.c_MaxValue,
               c_OscElement.f64_Factor,
               c_OscElement.f64_Offset,
               this->mu32_ArrayIndex);
            for (uint32_t u32_ItDataSet = 0; u32_ItDataSet < c_OscElement.c_DataSetValues.size(); ++u32_ItDataSet)
            {
               if (C_SdNdeDpUtil::h_CompareSpecifiedItemSmaller(c_OscElement.c_MaxValue,
                                                                c_OscElement.c_DataSetValues[u32_ItDataSet],
                                                                this->mu32_ArrayIndex) == true)
               {
                  new C_SdNdeUnoLeDataPoolListElementDataChangeCommand(this->mu32_NodeIndex, this->mu32_DataPoolIndex,
                                                                       this->mu32_DataPoolListIndex,
                                                                       this->mpc_DataPoolListModelViewManager,
                                                                       this->mu32_DataPoolListElementIndex, c_NewData,
                                                                       C_SdNdeDpUtil::eELEMENT_DATA_SET,
                                                                       this->mu32_ArrayIndex, u32_ItDataSet, this);
               }
            }
         }
      }
      this->mq_Initial = false;

      //Signal data change
      if (this->mpc_DataPoolListModelViewManager != NULL)
      {
         C_SdNdeDpListTableModel * const pc_Model = this->mpc_DataPoolListModelViewManager->GetElementModel(
            this->mu32_NodeIndex, this->mu32_DataPoolIndex, this->mu32_DataPoolListIndex);
         if (pc_Model != NULL)
         {
            pc_Model->HandleDataChange(this->mu32_DataPoolListElementIndex,
                                       this->me_DataChangeType, this->ms32_DataSetIndex);
            //Auto min & max will always affect min & max cell
            if (this->me_DataChangeType == C_SdNdeDpUtil::eELEMENT_AUTO_MIN_MAX)
            {
               pc_Model->HandleDataChange(this->mu32_DataPoolListElementIndex,
                                          C_SdNdeDpUtil::eELEMENT_MIN, this->ms32_DataSetIndex);
               pc_Model->HandleDataChange(this->mu32_DataPoolListElementIndex,
                                          C_SdNdeDpUtil::eELEMENT_MAX, this->ms32_DataSetIndex);
            }
            else if (this->me_DataChangeType == C_SdNdeDpUtil::eELEMENT_ARRAY)
            {
               const C_OscNodeDataPoolList * const pc_List = C_PuiSdHandler::h_GetInstance()->GetOscDataPoolList(
                  this->mu32_NodeIndex, this->mu32_DataPoolIndex,
                  this->mu32_DataPoolListIndex);
               //Array size change can affect min & max & data sets cells
               pc_Model->HandleDataChange(this->mu32_DataPoolListElementIndex,
                                          C_SdNdeDpUtil::eELEMENT_MIN, this->ms32_DataSetIndex);
               pc_Model->HandleDataChange(this->mu32_DataPoolListElementIndex,
                                          C_SdNdeDpUtil::eELEMENT_MAX, this->ms32_DataSetIndex);
               //All data sets
               if (pc_List != NULL)
               {
                  for (uint32_t u32_ItDataSet = 0; u32_ItDataSet < pc_List->c_DataSets.size(); ++u32_ItDataSet)
                  {
                     pc_Model->HandleDataChange(this->mu32_DataPoolListElementIndex,
                                                C_SdNdeDpUtil::eELEMENT_DATA_SET,
                                                static_cast<int32_t>(u32_ItDataSet));
                  }
               }
            }
            else if (this->me_DataChangeType == C_SdNdeDpUtil::eELEMENT_VALUE_TYPE)
            {
               const C_OscNodeDataPoolList * const pc_List = C_PuiSdHandler::h_GetInstance()->GetOscDataPoolList(
                  this->mu32_NodeIndex, this->mu32_DataPoolIndex,
                  this->mu32_DataPoolListIndex);
               //Array size change can affect auto & min & max & factor & offset & unit data sets cells
               pc_Model->HandleDataChange(this->mu32_DataPoolListElementIndex,
                                          C_SdNdeDpUtil::eELEMENT_AUTO_MIN_MAX, this->ms32_DataSetIndex);
               pc_Model->HandleDataChange(this->mu32_DataPoolListElementIndex,
                                          C_SdNdeDpUtil::eELEMENT_MIN, this->ms32_DataSetIndex);
               pc_Model->HandleDataChange(this->mu32_DataPoolListElementIndex,
                                          C_SdNdeDpUtil::eELEMENT_MAX, this->ms32_DataSetIndex);
               pc_Model->HandleDataChange(this->mu32_DataPoolListElementIndex,
                                          C_SdNdeDpUtil::eELEMENT_FACTOR, this->ms32_DataSetIndex);
               pc_Model->HandleDataChange(this->mu32_DataPoolListElementIndex,
                                          C_SdNdeDpUtil::eELEMENT_OFFSET, this->ms32_DataSetIndex);
               pc_Model->HandleDataChange(this->mu32_DataPoolListElementIndex,
                                          C_SdNdeDpUtil::eELEMENT_UNIT, this->ms32_DataSetIndex);
               //All data sets
               if (pc_List != NULL)
               {
                  for (uint32_t u32_ItDataSet = 0; u32_ItDataSet < pc_List->c_DataSets.size(); ++u32_ItDataSet)
                  {
                     pc_Model->HandleDataChange(this->mu32_DataPoolListElementIndex,
                                                C_SdNdeDpUtil::eELEMENT_DATA_SET,
                                                static_cast<int32_t>(u32_ItDataSet));
                  }
               }
            }
            else
            {
               //No special handling
            }
         }
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Apply auto min
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeUnoLeDataPoolListElementDataChangeCommand::m_ApplyAutoMin(void)
{
   const C_OscNodeDataPoolListElement * const pc_OscElement =
      C_PuiSdHandler::h_GetInstance()->GetOscDataPoolListElement(this->mu32_NodeIndex, this->mu32_DataPoolIndex,
                                                                 this->mu32_DataPoolListIndex,
                                                                 this->mu32_DataPoolListElementIndex);

   tgl_assert(pc_OscElement != NULL);
   if (pc_OscElement != NULL)
   {
      QVariant c_Data;
      const C_OscNodeDataPoolContent c_TemporaryElement = m_GetCurrentTypeMinGeneric();
      c_Data = C_SdNdeDpContentUtil::h_ConvertScaledContentToGeneric(c_TemporaryElement,
                                                                     pc_OscElement->f64_Factor,
                                                                     pc_OscElement->f64_Offset, 0);

      //Append to current command
      if (pc_OscElement->c_MinValue.GetArray() == false)
      {
         new C_SdNdeUnoLeDataPoolListElementDataChangeCommand(this->mu32_NodeIndex, this->mu32_DataPoolIndex,
                                                              this->mu32_DataPoolListIndex,
                                                              this->mpc_DataPoolListModelViewManager,
                                                              this->mu32_DataPoolListElementIndex, c_Data,
                                                              C_SdNdeDpUtil::eELEMENT_MIN,
                                                              0, 0, this);
      }
      else
      {
         for (uint32_t u32_ItArray = 0; u32_ItArray < pc_OscElement->c_MinValue.GetArraySize(); ++u32_ItArray)
         {
            new C_SdNdeUnoLeDataPoolListElementDataChangeCommand(this->mu32_NodeIndex, this->mu32_DataPoolIndex,
                                                                 this->mu32_DataPoolListIndex,
                                                                 this->mpc_DataPoolListModelViewManager,
                                                                 this->mu32_DataPoolListElementIndex, c_Data,
                                                                 C_SdNdeDpUtil::eELEMENT_MIN,
                                                                 u32_ItArray, 0, this);
         }
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Apply auto max
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeUnoLeDataPoolListElementDataChangeCommand::m_ApplyAutoMax(void)
{
   const C_OscNodeDataPoolListElement * const pc_OscElement =
      C_PuiSdHandler::h_GetInstance()->GetOscDataPoolListElement(this->mu32_NodeIndex, this->mu32_DataPoolIndex,
                                                                 this->mu32_DataPoolListIndex,
                                                                 this->mu32_DataPoolListElementIndex);

   tgl_assert(pc_OscElement != NULL);
   if (pc_OscElement != NULL)
   {
      QVariant c_Data;
      const C_OscNodeDataPoolContent c_TemporaryElement = m_GetCurrentTypeMaxGeneric();
      c_Data = C_SdNdeDpContentUtil::h_ConvertScaledContentToGeneric(c_TemporaryElement,
                                                                     pc_OscElement->f64_Factor,
                                                                     pc_OscElement->f64_Offset, 0);

      //Append to current command
      if (pc_OscElement->c_MaxValue.GetArray() == false)
      {
         new C_SdNdeUnoLeDataPoolListElementDataChangeCommand(this->mu32_NodeIndex, this->mu32_DataPoolIndex,
                                                              this->mu32_DataPoolListIndex,
                                                              this->mpc_DataPoolListModelViewManager,
                                                              this->mu32_DataPoolListElementIndex, c_Data,
                                                              C_SdNdeDpUtil::eELEMENT_MAX,
                                                              0, 0, this);
      }
      else
      {
         for (uint32_t u32_ItArray = 0; u32_ItArray < pc_OscElement->c_MaxValue.GetArraySize(); ++u32_ItArray)
         {
            new C_SdNdeUnoLeDataPoolListElementDataChangeCommand(this->mu32_NodeIndex, this->mu32_DataPoolIndex,
                                                                 this->mu32_DataPoolListIndex,
                                                                 this->mpc_DataPoolListModelViewManager,
                                                                 this->mu32_DataPoolListElementIndex, c_Data,
                                                                 C_SdNdeDpUtil::eELEMENT_MAX,
                                                                 u32_ItArray, 0, this);
         }
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get current minimum value in generic format

   \return
   Current minimum value in generic format
*/
//----------------------------------------------------------------------------------------------------------------------
C_OscNodeDataPoolContent C_SdNdeUnoLeDataPoolListElementDataChangeCommand::m_GetCurrentTypeMinGeneric(void) const
{
   C_OscNodeDataPoolContent c_Retval;
   const C_OscNodeDataPoolListElement * const pc_OscElement =
      C_PuiSdHandler::h_GetInstance()->GetOscDataPoolListElement(this->mu32_NodeIndex, this->mu32_DataPoolIndex,
                                                                 this->mu32_DataPoolListIndex,
                                                                 this->mu32_DataPoolListElementIndex);

   if (pc_OscElement != NULL)
   {
      c_Retval.SetType(pc_OscElement->GetType());
      c_Retval.SetArray(false);

      switch (c_Retval.GetType())
      {
      case C_OscNodeDataPoolContent::E_Type::eUINT8:
         c_Retval.SetValueU8(std::numeric_limits<uint8_t>::lowest());
         break;
      case C_OscNodeDataPoolContent::E_Type::eUINT16:
         c_Retval.SetValueU16(std::numeric_limits<uint16_t>::lowest());
         break;
      case C_OscNodeDataPoolContent::E_Type::eUINT32:
         c_Retval.SetValueU32(std::numeric_limits<uint32_t>::lowest());
         break;
      case C_OscNodeDataPoolContent::E_Type::eUINT64:
         c_Retval.SetValueU64(std::numeric_limits<uint64_t>::lowest());
         break;
      case C_OscNodeDataPoolContent::E_Type::eSINT8:
         c_Retval.SetValueS8(std::numeric_limits<int8_t>::lowest());
         break;
      case C_OscNodeDataPoolContent::E_Type::eSINT16:
         c_Retval.SetValueS16(std::numeric_limits<int16_t>::lowest());
         break;
      case C_OscNodeDataPoolContent::E_Type::eSINT32:
         c_Retval.SetValueS32(std::numeric_limits<int32_t>::lowest());
         break;
      case C_OscNodeDataPoolContent::E_Type::eSINT64:
         c_Retval.SetValueS64(std::numeric_limits<int64_t>::lowest());
         break;
      case C_OscNodeDataPoolContent::E_Type::eFLOAT32:
         c_Retval.SetValueF32(std::numeric_limits<float32_t>::lowest());
         break;
      case C_OscNodeDataPoolContent::E_Type::eFLOAT64:
         c_Retval.SetValueF64(std::numeric_limits<float64_t>::lowest());
         break;
      default:
         //Unknown min/max
         break;
      }
   }
   return c_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get current maximum value in generic format

   \return
   Current maximum value in generic format
*/
//----------------------------------------------------------------------------------------------------------------------
C_OscNodeDataPoolContent C_SdNdeUnoLeDataPoolListElementDataChangeCommand::m_GetCurrentTypeMaxGeneric(void) const
{
   C_OscNodeDataPoolContent c_Retval;
   const C_OscNodeDataPoolListElement * const pc_OscElement =
      C_PuiSdHandler::h_GetInstance()->GetOscDataPoolListElement(this->mu32_NodeIndex, this->mu32_DataPoolIndex,
                                                                 this->mu32_DataPoolListIndex,
                                                                 this->mu32_DataPoolListElementIndex);

   if (pc_OscElement != NULL)
   {
      c_Retval.SetType(pc_OscElement->GetType());
      c_Retval.SetArray(false);

      switch (c_Retval.GetType())
      {
      case C_OscNodeDataPoolContent::E_Type::eUINT8:
         c_Retval.SetValueU8(std::numeric_limits<uint8_t>::max());
         break;
      case C_OscNodeDataPoolContent::E_Type::eUINT16:
         c_Retval.SetValueU16(std::numeric_limits<uint16_t>::max());
         break;
      case C_OscNodeDataPoolContent::E_Type::eUINT32:
         c_Retval.SetValueU32(std::numeric_limits<uint32_t>::max());
         break;
      case C_OscNodeDataPoolContent::E_Type::eUINT64:
         c_Retval.SetValueU64(std::numeric_limits<uint64_t>::max());
         break;
      case C_OscNodeDataPoolContent::E_Type::eSINT8:
         c_Retval.SetValueS8(std::numeric_limits<int8_t>::max());
         break;
      case C_OscNodeDataPoolContent::E_Type::eSINT16:
         c_Retval.SetValueS16(std::numeric_limits<int16_t>::max());
         break;
      case C_OscNodeDataPoolContent::E_Type::eSINT32:
         c_Retval.SetValueS32(std::numeric_limits<int32_t>::max());
         break;
      case C_OscNodeDataPoolContent::E_Type::eSINT64:
         c_Retval.SetValueS64(std::numeric_limits<int64_t>::max());
         break;
      case C_OscNodeDataPoolContent::E_Type::eFLOAT32:
         c_Retval.SetValueF32(std::numeric_limits<float32_t>::max());
         break;
      case C_OscNodeDataPoolContent::E_Type::eFLOAT64:
         c_Retval.SetValueF64(std::numeric_limits<float64_t>::max());
         break;
      default:
         //Unknown min/max
         break;
      }
   }
   return c_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Copy one single value element to one array index value

   \param[in]  orc_Single       Single value source element
   \param[in]  oru32_ArrayIndex Target array index
   \param[out] orc_Output       Target element
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeUnoLeDataPoolListElementDataChangeCommand::m_CopySingleValueToArrayIndex(
   const C_OscNodeDataPoolContent & orc_Single, const uint32_t & oru32_ArrayIndex,
   C_OscNodeDataPoolContent & orc_Output) const
{
   if ((((orc_Single.GetArray() == false) && (orc_Output.GetArray() == true)) &&
        (orc_Single.GetType() == orc_Output.GetType())) && (oru32_ArrayIndex < orc_Output.GetArraySize()))
   {
      switch (orc_Single.GetType())
      {
      case C_OscNodeDataPoolContent::eUINT8:
         orc_Output.SetValueArrU8Element(orc_Single.GetValueU8(), oru32_ArrayIndex);
         break;
      case C_OscNodeDataPoolContent::eUINT16:
         orc_Output.SetValueArrU16Element(orc_Single.GetValueU16(), oru32_ArrayIndex);
         break;
      case C_OscNodeDataPoolContent::eUINT32:
         orc_Output.SetValueArrU32Element(orc_Single.GetValueU32(), oru32_ArrayIndex);
         break;
      case C_OscNodeDataPoolContent::eUINT64:
         orc_Output.SetValueArrU64Element(orc_Single.GetValueU64(), oru32_ArrayIndex);
         break;
      case C_OscNodeDataPoolContent::eSINT8:
         orc_Output.SetValueArrS8Element(orc_Single.GetValueS8(), oru32_ArrayIndex);
         break;
      case C_OscNodeDataPoolContent::eSINT16:
         orc_Output.SetValueArrS16Element(orc_Single.GetValueS16(), oru32_ArrayIndex);
         break;
      case C_OscNodeDataPoolContent::eSINT32:
         orc_Output.SetValueArrS32Element(orc_Single.GetValueS32(), oru32_ArrayIndex);
         break;
      case C_OscNodeDataPoolContent::eSINT64:
         orc_Output.SetValueArrS64Element(orc_Single.GetValueS64(), oru32_ArrayIndex);
         break;
      case C_OscNodeDataPoolContent::eFLOAT32:
         orc_Output.SetValueArrF32Element(orc_Single.GetValueF32(), oru32_ArrayIndex);
         break;
      case C_OscNodeDataPoolContent::eFLOAT64:
         orc_Output.SetValueArrF64Element(orc_Single.GetValueF64(), oru32_ArrayIndex);
         break;
      default:
         break;
      }
   }
}
