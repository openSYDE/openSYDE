//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Base functions and data storing for datapool elements and its content (implementation)

   detailed description

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.h"

#include "stwerrors.h"
#include "constants.h"
#include "C_OSCLoggingHandler.h"
#include "C_GtGetText.h"
#include "C_PuiSvDbDataElementHandler.h"
#include "C_PuiSvHandler.h"
#include "C_PuiSvData.h"
#include "C_OSCUtils.h"
#include "TGLUtils.h"
#include "TGLTime.h"
#include "C_PuiSdUtil.h"
#include "C_PuiSdHandler.h"
#include "C_SdNdeDpContentUtil.h"
#include "C_OSCNodeDataPoolContentUtil.h"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw_tgl;
using namespace stw_types;
using namespace stw_errors;
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

   long description of function within several lines

   \param[in]  oru32_ViewIndex            Index of system view
   \param[in]  oru32_DashboardIndex       Index of dashboard in system view
   \param[in]  ors32_DataIndex            Index of connected data item
   \param[in]  ore_Type                   Type of data
   \param[in]  ou32_MaximumDataElements   Maximum number of shown data elements of the widget
   \param[in]  oq_ReadItem                Flag if item is a read only item and has an rail assignment
*/
//----------------------------------------------------------------------------------------------------------------------
C_PuiSvDbDataElementHandler::C_PuiSvDbDataElementHandler(const uint32 & oru32_ViewIndex,
                                                         const uint32 & oru32_DashboardIndex,
                                                         const sint32 & ors32_DataIndex, const E_Type & ore_Type,
                                                         const uint32 ou32_MaximumDataElements,
                                                         const bool oq_ReadItem) :
   C_PuiSvDbDataElement(oru32_ViewIndex, oru32_DashboardIndex, ors32_DataIndex, ore_Type),
   mq_ReadItem(oq_ReadItem),
   mu32_WidgetDataPoolElementCountMaximum(ou32_MaximumDataElements),
   mu32_WidgetDataPoolElementCount(0U)
{
   this->mc_VecDataValues.resize(this->mu32_WidgetDataPoolElementCount);
   this->mc_VecNvmValue.resize(this->mu32_WidgetDataPoolElementCount);
   this->mc_VecNvmValueReceived.resize(this->mu32_WidgetDataPoolElementCount);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Adds the datapool element to the shown elements of the widget

   This function is thread safe.

   \param[in]  orc_WidgetDataPoolElementId   Datapool element identificator
   \param[in]  orc_DataPoolElementScaling    Datapool element scaling configuration

   \return
   C_NO_ERR    Datapool element added
   C_NOACT     Datapool element was added already. Nothing done.
   C_RANGE     No further datapool elements can be added. Nothing done.
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_PuiSvDbDataElementHandler::RegisterDataPoolElement(
   const C_PuiSvDbNodeDataPoolListElementId & orc_WidgetDataPoolElementId,
   const C_PuiSvDbDataElementScaling & orc_DataPoolElementScaling)
{
   sint32 s32_Return = C_RANGE;

   if (this->mc_MappingDpElementToDataSerie.size() < static_cast<sintn>(this->mu32_WidgetDataPoolElementCountMaximum))
   {
      QMap<stw_opensyde_gui_logic::C_PuiSvDbNodeDataPoolListElementId, stw_types::uint32>::iterator c_ItItem;

      c_ItItem = this->mc_MappingDpElementToDataSerie.find(orc_WidgetDataPoolElementId);

      if (c_ItItem == this->mc_MappingDpElementToDataSerie.end())
      {
         this->mc_CriticalSection.Acquire();
         // Use the current count of maps as index. It will be used as index for the vector mc_VecDataValues
         this->mc_MappingDpElementToDataSerie.insert(
            orc_WidgetDataPoolElementId,
            static_cast<uint32>(this->mc_MappingDpElementToDataSerie.size()));

         this->mc_MappingDpElementToScaling.insert(orc_WidgetDataPoolElementId, orc_DataPoolElementScaling);

         // Update the counter
         this->m_SetWidgetDataPoolElementCount(this->mc_MappingDpElementToDataSerie.size());
         this->m_UpdateDataPoolElementTimeoutAndValidFlag(orc_WidgetDataPoolElementId);
         this->m_DataPoolElementsChanged();
         this->mc_CriticalSection.Release();

         s32_Return = C_NO_ERR;
      }
      else
      {
         // Datapool element was added already
         s32_Return = C_NOACT;
      }
   }

   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Register cyclic transmission error for specific data element

   Info: no default handling

   \param[in]  orc_WidgetDataPoolElementId   Affected data element
   \param[in]  ou8_ErrorCode                 Registered error code
*/
//----------------------------------------------------------------------------------------------------------------------
//lint -e{9175}  //intentionally no functionality in default implementation
void C_PuiSvDbDataElementHandler::RegisterDataPoolElementCyclicError(
   const C_PuiSvDbNodeDataPoolListElementId & orc_WidgetDataPoolElementId, const uint8 ou8_ErrorCode)
{
   Q_UNUSED(orc_WidgetDataPoolElementId)
   Q_UNUSED(ou8_ErrorCode)
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Removes the datapool element from the shown elements of the widget

   This function is thread safe.

   \param[in]  orc_WidgetDataPoolElementId   Datapool element identificator
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PuiSvDbDataElementHandler::RemoveDataPoolElement(
   const C_PuiSvDbNodeDataPoolListElementId & orc_WidgetDataPoolElementId)
{
   QMap<C_PuiSvDbNodeDataPoolListElementId, uint32>::iterator c_ItItemDataSerie;
   QMap<C_PuiSvDbNodeDataPoolListElementId, C_PuiSvDbDataElementScaling>::iterator c_ItItemScaling;

   this->mc_CriticalSection.Acquire();

   c_ItItemDataSerie = this->mc_MappingDpElementToDataSerie.find(orc_WidgetDataPoolElementId);

   if (c_ItItemDataSerie != this->mc_MappingDpElementToDataSerie.end())
   {
      const uint32 u32_ValueOfDeletedItem = c_ItItemDataSerie.value();

      this->mc_MappingDpElementToDataSerie.erase(c_ItItemDataSerie);

      // Update the counter
      this->m_SetWidgetDataPoolElementCount(this->mc_MappingDpElementToDataSerie.size());

      // Update the indexes of the other registered elements
      if (this->mc_MappingDpElementToDataSerie.size() > 0)
      {
         QMap<C_PuiSvDbNodeDataPoolListElementId, uint32>::iterator c_ItItem;
         for (c_ItItem = this->mc_MappingDpElementToDataSerie.begin();
              c_ItItem != this->mc_MappingDpElementToDataSerie.end();
              ++c_ItItem)
         {
            if (c_ItItem.value() > u32_ValueOfDeletedItem)
            {
               // Replace the entry with the new index
               c_ItItem.value() -= 1U;
            }
         }
      }

      // Update all relevant items
      this->m_UpdateDataPoolElementTimeoutAndValidFlag();
      this->m_DataPoolElementsChanged();
   }

   // Remove scaling
   c_ItItemScaling = this->mc_MappingDpElementToScaling.find(orc_WidgetDataPoolElementId);

   if (c_ItItemScaling != this->mc_MappingDpElementToScaling.end())
   {
      this->mc_MappingDpElementToScaling.erase(c_ItItemScaling);
   }

   this->mc_CriticalSection.Release();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Clear the datapool elements from the shown elements of the widget

   This function is thread safe.
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PuiSvDbDataElementHandler::ClearDataPoolElements(void)
{
   this->mc_CriticalSection.Acquire();
   this->mc_MappingDpElementToDataSerie.clear();
   this->mc_MappingDpElementToScaling.clear();
   this->m_SetWidgetDataPoolElementCount(this->mc_MappingDpElementToDataSerie.size());
   this->mc_CriticalSection.Release();

   this->m_DataPoolElementsChanged();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Handle changes of transmission mode for any data element
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PuiSvDbDataElementHandler::UpdateElementTransmissionConfiguration(void)
{
   const uint32 u32_Count = this->GetWidgetDataPoolElementCount();

   std::vector<C_PuiSvDbNodeDataPoolListElementId> c_Ids;
   std::vector<C_PuiSvDbDataElementScaling> c_Scalings;

   //Reserve
   c_Ids.resize(u32_Count);
   c_Scalings.resize(u32_Count);
   //Reregister all data elements
   for (uint32 u32_It = 0; u32_It < u32_Count; ++u32_It)
   {
      tgl_assert(this->GetDataPoolElementIndex(u32_It, c_Ids[u32_It]) == C_NO_ERR);
      tgl_assert(this->GetDataPoolElementScaling(u32_It, c_Scalings[u32_It]) == C_NO_ERR);
   }
   this->ClearDataPoolElements();
   for (uint32 u32_It = 0; u32_It < u32_Count; ++u32_It)
   {
      this->RegisterDataPoolElement(c_Ids[u32_It], c_Scalings[u32_It]);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Returns the count of shown datapool elements

   \return
   Count of shown datapool elements
*/
//----------------------------------------------------------------------------------------------------------------------
uint32 C_PuiSvDbDataElementHandler::GetWidgetDataPoolElementCount(void) const
{
   return this->mu32_WidgetDataPoolElementCount;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Gets all identification informations about the registered datapool element

   \param[in]   ou32_WidgetDataPoolElementIndex    Index of shown datapool element in widget
   \param[out]  orc_DataPoolId                     Datapool element identificator

   \return
   C_NO_ERR    Datapool element index valid and index id was filled
   C_RANGE     Datapool element index not valid. Id was not filled.
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_PuiSvDbDataElementHandler::GetDataPoolElementIndex(const uint32 ou32_WidgetDataPoolElementIndex,
                                                            C_PuiSvDbNodeDataPoolListElementId & orc_DataPoolId) const
{
   sint32 s32_Return = C_RANGE;

   QMap<stw_opensyde_gui_logic::C_PuiSvDbNodeDataPoolListElementId, stw_types::uint32>::const_iterator c_ItItem;

   // Searching reverse in the map, we search with the value.
   for (c_ItItem = this->mc_MappingDpElementToDataSerie.begin();
        c_ItItem != this->mc_MappingDpElementToDataSerie.end();
        ++c_ItItem)
   {
      if (c_ItItem.value() == ou32_WidgetDataPoolElementIndex)
      {
         // Return the key as result
         orc_DataPoolId = c_ItItem.key();
         s32_Return = C_NO_ERR;
         break;
      }
   }

   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Gets index of datapool element in widget with identification informations

   \param[in]   orc_DataPoolId                     Datapool element identificator
   \param[out]  oru32_WidgetDataPoolElementIndex   Index of shown datapool element in widget

   \return
   C_NO_ERR    Datapool element index valid and index id was filled
   C_RANGE     Datapool element index not valid. Id was not filled.
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_PuiSvDbDataElementHandler::GetWidgetDataPoolElementIndex(
   const C_PuiSvDbNodeDataPoolListElementId & orc_DataPoolId, uint32 & oru32_WidgetDataPoolElementIndex) const
{
   sint32 s32_Return = C_RANGE;

   QMap<stw_opensyde_gui_logic::C_PuiSvDbNodeDataPoolListElementId, stw_types::uint32>::const_iterator c_ItItem;

   c_ItItem = this->mc_MappingDpElementToDataSerie.find(orc_DataPoolId);

   if (c_ItItem != this->mc_MappingDpElementToDataSerie.end())
   {
      oru32_WidgetDataPoolElementIndex = c_ItItem.value();
      s32_Return = C_NO_ERR;
   }

   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Gets all scaling informations about the registered datapool element

   \param[in]   ou32_WidgetDataPoolElementIndex    Index of shown datapool element in widget
   \param[out]  orc_DataPoolScaling                Datapool element scaling configuration

   \return
   C_NO_ERR    Datapool element index valid and index id was filled
   C_RANGE     Datapool element index not valid. Id was not filled.
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_PuiSvDbDataElementHandler::GetDataPoolElementScaling(const uint32 ou32_WidgetDataPoolElementIndex,
                                                              C_PuiSvDbDataElementScaling & orc_DataPoolScaling) const
{
   C_PuiSvDbNodeDataPoolListElementId c_ElementId;
   sint32 s32_Return = this->GetDataPoolElementIndex(ou32_WidgetDataPoolElementIndex, c_ElementId);

   if (s32_Return == C_NO_ERR)
   {
      QMap<C_PuiSvDbNodeDataPoolListElementId, C_PuiSvDbDataElementScaling>::const_iterator c_ItItem;

      c_ItItem = this->mc_MappingDpElementToScaling.find(c_ElementId);

      if (c_ItItem != this->mc_MappingDpElementToScaling.end())
      {
         orc_DataPoolScaling = c_ItItem.value();
      }
      else
      {
         s32_Return = C_RANGE;
      }
   }

   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   short description of function

   \param[in]  orc_WidgetDataPoolElementId   Datapool element identificator

   \return
   true     Data element is already registered
   false    Data element is not registered
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_PuiSvDbDataElementHandler::IsDataElementRegistered(
   const C_PuiSvDbNodeDataPoolListElementId & orc_WidgetDataPoolElementId) const
{
   bool q_Return = false;

   QMap<C_PuiSvDbNodeDataPoolListElementId, stw_types::uint32>::const_iterator c_ItItem;

   // Check the data series
   c_ItItem = this->mc_MappingDpElementToDataSerie.find(orc_WidgetDataPoolElementId);

   if (c_ItItem != this->mc_MappingDpElementToDataSerie.end())
   {
      q_Return = true;
   }

   return q_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Inserts a new received value to the widget queue

   This function is thread safe.

   \param[in]  orc_WidgetDataPoolElementId   Datapool element identificator
   \param[in]  orc_NewValue                  New received value
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PuiSvDbDataElementHandler::InsertNewValueIntoQueue(
   const C_PuiSvDbNodeDataPoolListElementId & orc_WidgetDataPoolElementId,
   const C_PuiSvDbDataElementContent & orc_NewValue)
{
   QMap<stw_opensyde_gui_logic::C_PuiSvDbNodeDataPoolListElementId, stw_types::uint32>::iterator c_ItItem;

   //manual find because match is unreliable
   for (c_ItItem = this->mc_MappingDpElementToDataSerie.begin(); c_ItItem != this->mc_MappingDpElementToDataSerie.end();
        ++c_ItItem)
   {
      // Get the correct data series
      if (c_ItItem.key().CheckSameDataElement(orc_WidgetDataPoolElementId))
      {
         // Add the new value to the correct data series
         const uint32 u32_DataSerieIndex = c_ItItem.value();
         if (static_cast<sintn>(u32_DataSerieIndex) < this->mc_VecDataValues.size())
         {
            this->mc_CriticalSection.Acquire();
            this->mc_VecDataValues[u32_DataSerieIndex].push_back(orc_NewValue);
            this->mc_CriticalSection.Release();
         }
         //Don't stop as there might be multiple matches
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Adds a new received NVM value to the widget queue

   This function is thread safe.

   \param[in]  orc_WidgetDataPoolElementId   Datapool element identificator
   \param[in]  orc_NewValue                  New received value
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PuiSvDbDataElementHandler::AddNewNvmValueIntoQueue(
   const C_PuiSvDbNodeDataPoolListElementId & orc_WidgetDataPoolElementId,
   const C_PuiSvDbDataElementContent & orc_NewValue)
{
   QMap<stw_opensyde_gui_logic::C_PuiSvDbNodeDataPoolListElementId, stw_types::uint32>::iterator c_ItItem;

   // Get the correct data series
   c_ItItem = this->mc_MappingDpElementToDataSerie.find(orc_WidgetDataPoolElementId);

   if (c_ItItem != this->mc_MappingDpElementToDataSerie.end())
   {
      // Add the new value to the correct data series
      const uint32 u32_DataSerieIndex = c_ItItem.value();
      if ((static_cast<sintn>(u32_DataSerieIndex) < this->mc_VecNvmValue.size()) &&
          (static_cast<sintn>(u32_DataSerieIndex) < this->mc_VecNvmValueReceived.size()))
      {
         this->mc_CriticalSection.Acquire();
         this->mc_VecNvmValue[u32_DataSerieIndex] = orc_NewValue;
         this->mc_VecNvmValueReceived[u32_DataSerieIndex] = true;
         this->mc_CriticalSection.Release();
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get unscaled value as scaled string

   Warning: expected to be in range of min & max

   \param[in]  of64_Value           Unscaled value
   \param[in]  ou32_IndexElement    Optional index of element (if more than one)

   \return
   Value as scaled string
*/
//----------------------------------------------------------------------------------------------------------------------
QString C_PuiSvDbDataElementHandler::GetUnscaledValueAsScaledString(const float64 of64_Value,
                                                                    const uint32 ou32_IndexElement) const
{
   QString c_Retval;
   C_PuiSvDbNodeDataPoolListElementId c_ID;
   C_PuiSvDbDataElementScaling c_Scaling;

   if ((this->GetDataPoolElementIndex(ou32_IndexElement, c_ID) == C_NO_ERR) &&
       (this->GetDataPoolElementScaling(ou32_IndexElement, c_Scaling) == C_NO_ERR))
   {
      const C_OSCNodeDataPoolListElement * const pc_Element =
         C_PuiSdHandler::h_GetInstance()->GetOSCDataPoolListElement(c_ID.u32_NodeIndex, c_ID.u32_DataPoolIndex,
                                                                    c_ID.u32_ListIndex, c_ID.u32_ElementIndex);
      if (pc_Element != NULL)
      {
         C_OSCNodeDataPoolContent c_Tmp = pc_Element->c_MinValue;
         C_OSCNodeDataPoolContentUtil::h_SetValueInContent(of64_Value, c_Tmp, 0UL);
         C_SdNdeDpContentUtil::h_GetValueAsScaledString(c_Tmp, c_Scaling.f64_Factor, c_Scaling.f64_Offset,
                                                        c_Retval, 0UL);
      }
   }
   if (c_Retval.compare("") == 0)
   {
      c_Retval = QString::number(of64_Value);
   }
   return c_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get unscaled value (does not need to be in range of min & max) as scaled string

   \param[in]      of64_Value          Unscaled value (does not need to be in range of min & max)
   \param[in]      ou32_IndexElement   Optional index of element (if more than one)
   \param[in,out]  opf64_Progress      Optional parameter to store the progress of the new applied value in range of min and max [0,1]

   \return
   Value as scaled string
*/
//----------------------------------------------------------------------------------------------------------------------
QString C_PuiSvDbDataElementHandler::GetUnscaledValueInRangeAsScaledString(const float64 of64_Value,
                                                                           const uint32 ou32_IndexElement,
                                                                           float64 * const opf64_Progress)
const
{
   QString c_Retval;
   C_PuiSvDbNodeDataPoolListElementId c_ID;
   C_PuiSvDbDataElementScaling c_Scaling;

   if ((this->GetDataPoolElementIndex(ou32_IndexElement, c_ID) == C_NO_ERR) &&
       (this->GetDataPoolElementScaling(ou32_IndexElement, c_Scaling) == C_NO_ERR))
   {
      const C_OSCNodeDataPoolListElement * const pc_Element =
         C_PuiSdHandler::h_GetInstance()->GetOSCDataPoolListElement(c_ID.u32_NodeIndex, c_ID.u32_DataPoolIndex,
                                                                    c_ID.u32_ListIndex, c_ID.u32_ElementIndex);
      if (pc_Element != NULL)
      {
         //Restrict value to min & max
         C_OSCNodeDataPoolContentUtil::E_ValueChangedTo e_Change;
         C_OSCNodeDataPoolContent c_Tmp = pc_Element->c_MinValue;
         C_OSCNodeDataPoolContentUtil::h_SetValueInContent(of64_Value, c_Tmp, 0UL);
         C_OSCNodeDataPoolContentUtil::h_SetValueInMinMaxRange(pc_Element->c_MinValue, pc_Element->c_MaxValue, c_Tmp,
                                                               e_Change);
         C_SdNdeDpContentUtil::h_GetValueAsScaledString(c_Tmp, c_Scaling.f64_Factor, c_Scaling.f64_Offset,
                                                        c_Retval, 0UL);
         if (opf64_Progress != NULL)
         {
            float64 f64_Min;
            float64 f64_Max;
            float64 f64_Value;
            const uint32 u32_Index = c_ID.GetArrayElementIndexOrZero();
            //Get restricted value "progress"
            C_SdNdeDpContentUtil::h_GetValueAsFloat64(pc_Element->c_MinValue, f64_Min, u32_Index);
            C_SdNdeDpContentUtil::h_GetValueAsFloat64(pc_Element->c_MaxValue, f64_Max, u32_Index);
            C_SdNdeDpContentUtil::h_GetValueAsFloat64(c_Tmp, f64_Value, u32_Index);
            *opf64_Progress = (f64_Value - f64_Min) / (f64_Max - f64_Min);
         }
      }
   }
   if (c_Retval.compare("") == 0)
   {
      c_Retval = QString::number(of64_Value);
   }
   return c_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Returns the newest registered value of a specific datapool element. It can be an array too.

   The vector will be cleared first.

   If it is an array, the size of the vector is > 1
   If it is no array, the size of the vector is 1
   If an error occurred, the size of the vector is 0

   This function is thread safe.

   \param[in]   ou32_WidgetDataPoolElementIndex    Index of shown datapool element in widget
   \param[out]  orc_Values                         Vector with result value(s)
   \param[out]  opc_StringifiedValues              Optional vector with result strings
   \param[in]   oq_UseScaling                      Optional flag if return value should include internally stored scaling

   \return
   C_NO_ERR    Value read
   C_RANGE     Index invalid
   C_NOACT     No value received
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_PuiSvDbDataElementHandler::m_GetLastValue(const uint32 ou32_WidgetDataPoolElementIndex,
                                                   std::vector<float64> & orc_Values,
                                                   std::vector<QString> * const opc_StringifiedValues,
                                                   const bool oq_UseScaling)
{
   sint32 s32_Return = C_RANGE;

   orc_Values.clear();

   if ((static_cast<sintn>(ou32_WidgetDataPoolElementIndex) < this->mc_VecDataValues.size()) &&
       ((ou32_WidgetDataPoolElementIndex < this->mc_UsedScaling.size()) || (oq_UseScaling == false)))
   {
      this->mc_CriticalSection.Acquire();
      if (this->mc_VecDataValues[ou32_WidgetDataPoolElementIndex].size() > 0)
      {
         const sintn sn_LastIndex = this->mc_VecDataValues[ou32_WidgetDataPoolElementIndex].size() - 1;
         uint32 u32_Counter;
         float64 f64_Temp;

         // Get the newest value
         C_SdNdeDpContentUtil::h_GetValuesAsFloat64(
            this->mc_VecDataValues[ou32_WidgetDataPoolElementIndex][sn_LastIndex],
            orc_Values);

         // Scale all values
         if (oq_UseScaling == true)
         {
            for (u32_Counter = 0U; u32_Counter < orc_Values.size(); ++u32_Counter)
            {
               f64_Temp = orc_Values[u32_Counter];
               orc_Values[u32_Counter] = C_OSCUtils::h_GetValueScaled(f64_Temp,
                                                                      this->mc_UsedScaling[
                                                                         ou32_WidgetDataPoolElementIndex].f64_Factor,
                                                                      this->mc_UsedScaling[
                                                                         ou32_WidgetDataPoolElementIndex].f64_Offset);
            }
         }

         //Save last timestamp
         if (ou32_WidgetDataPoolElementIndex < this->mc_LastDataPoolElementTimeStampsMs.size())
         {
            this->mc_LastDataPoolElementTimeStampsMs[ou32_WidgetDataPoolElementIndex] =
               this->mc_VecDataValues[ou32_WidgetDataPoolElementIndex][sn_LastIndex].GetTimeStamp();
         }
         //Set timestamp valid
         if (ou32_WidgetDataPoolElementIndex < this->mc_LastDataPoolElementTimeStampsValid.size())
         {
            auto && rc_Value = this->mc_LastDataPoolElementTimeStampsValid[ou32_WidgetDataPoolElementIndex];
            rc_Value = true;
         }

         // Remove all values for the next call
         this->mc_VecDataValues[ou32_WidgetDataPoolElementIndex].clear();
         s32_Return = C_NO_ERR;
      }
      else
      {
         s32_Return = C_NOACT;
      }
      this->mc_CriticalSection.Release();
   }
   if ((opc_StringifiedValues != NULL) && (ou32_WidgetDataPoolElementIndex < this->mc_MinimumType.size()))
   {
      const std::vector<stw_opensyde_core::C_OSCNodeDataPoolContent::E_Type> & rc_Types = this->mc_MinimumType[
         ou32_WidgetDataPoolElementIndex];
      opc_StringifiedValues->reserve(orc_Values.size());
      if (orc_Values.size() == rc_Types.size())
      {
         for (uint32 u32_ItValue = 0; u32_ItValue < orc_Values.size(); ++u32_ItValue)
         {
            opc_StringifiedValues->push_back(mh_GetStringForScaledValue(orc_Values[u32_ItValue],
                                                                        rc_Types[u32_ItValue]));
         }
      }
   }

   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Returns the newest registered value of a specific datapool element as float

   This function is thread safe.

   \param[in]   ou32_WidgetDataPoolElementIndex    Index of shown datapool element in widget
   \param[out]  orf64_Value                        Result value
   \param[in]   oq_UseScaling                      Optional flag if return value should include internally stored scaling

   \return
   C_NO_ERR    Value read
   C_RANGE     Index invalid
   C_NOACT     No value received
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_PuiSvDbDataElementHandler::m_GetLastValue(const uint32 ou32_WidgetDataPoolElementIndex, float64 & orf64_Value,
                                                   const bool oq_UseScaling)
{
   sint32 s32_Return = C_RANGE;
   C_PuiSvDbNodeDataPoolListElementId c_Id;

   if (((static_cast<sintn>(ou32_WidgetDataPoolElementIndex) < this->mc_VecDataValues.size()) &&
        ((ou32_WidgetDataPoolElementIndex < this->mc_UsedScaling.size()) || (oq_UseScaling == false))) && (
          this->GetDataPoolElementIndex(ou32_WidgetDataPoolElementIndex, c_Id) == C_NO_ERR))
   {
      this->mc_CriticalSection.Acquire();
      if (this->mc_VecDataValues[ou32_WidgetDataPoolElementIndex].size() > 0)
      {
         const sintn sn_LastIndex = this->mc_VecDataValues[ou32_WidgetDataPoolElementIndex].size() - 1;
         float64 f64_Temp;
         const uint32 u32_Index = c_Id.GetArrayElementIndexOrZero();
         // Get the newest value as float
         C_SdNdeDpContentUtil::h_GetValueAsFloat64(this->mc_VecDataValues[ou32_WidgetDataPoolElementIndex][
                                                      sn_LastIndex], f64_Temp, u32_Index);
         // And calculate the scaling on the value
         if (oq_UseScaling == true)
         {
            orf64_Value = C_OSCUtils::h_GetValueScaled(
               f64_Temp,
               this->mc_UsedScaling[ou32_WidgetDataPoolElementIndex].f64_Factor,
               this->mc_UsedScaling[ou32_WidgetDataPoolElementIndex].f64_Offset);
         }
         else
         {
            orf64_Value = f64_Temp;
         }

         //Save last timestamp
         if (ou32_WidgetDataPoolElementIndex < this->mc_LastDataPoolElementTimeStampsMs.size())
         {
            this->mc_LastDataPoolElementTimeStampsMs[ou32_WidgetDataPoolElementIndex] =
               this->mc_VecDataValues[ou32_WidgetDataPoolElementIndex][sn_LastIndex].GetTimeStamp();
         }
         //Set timestamp valid
         if (ou32_WidgetDataPoolElementIndex < this->mc_LastDataPoolElementTimeStampsValid.size())
         {
            auto && rc_Value = this->mc_LastDataPoolElementTimeStampsValid[ou32_WidgetDataPoolElementIndex];
            rc_Value = true;
         }

         // Remove all values for the next call
         this->mc_VecDataValues[ou32_WidgetDataPoolElementIndex].clear();
         s32_Return = C_NO_ERR;
      }
      else
      {
         s32_Return = C_NOACT;
      }
      this->mc_CriticalSection.Release();
   }

   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Returns all registered values of a specific datapool element

   This function is thread safe.

   \param[in]   ou32_WidgetDataPoolElementIndex    Index of shown datapool element in widget
   \param[out]  orc_Values                         List with all values
   \param[out]  orc_Timestamps                     List with all timestamps
   \param[in]   oq_UseScaling                      Use scaling

   \return
   C_NO_ERR    Value read
   C_RANGE     Index invalid
   C_NOACT     No value received
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_PuiSvDbDataElementHandler::m_GetAllValues(const uint32 ou32_WidgetDataPoolElementIndex,
                                                   QVector<float64> & orc_Values, QVector<uint32> & orc_Timestamps,
                                                   const bool oq_UseScaling)
{
   sint32 s32_Return = C_RANGE;
   C_PuiSvDbNodeDataPoolListElementId c_Id;

   if (((static_cast<sintn>(ou32_WidgetDataPoolElementIndex) < this->mc_VecDataValues.size()) &&
        ((ou32_WidgetDataPoolElementIndex < this->mc_UsedScaling.size()) || (oq_UseScaling == false))) && (
          this->GetDataPoolElementIndex(ou32_WidgetDataPoolElementIndex, c_Id) == C_NO_ERR))
   {
      this->mc_CriticalSection.Acquire();
      if (this->mc_VecDataValues[ou32_WidgetDataPoolElementIndex].size() > 0)
      {
         uint32 u32_Counter = 0U;
         QList<C_PuiSvDbDataElementContent>::const_iterator c_ItItem;

         // Copy all values and scale the values if necessary
         orc_Values.resize(this->mc_VecDataValues[ou32_WidgetDataPoolElementIndex].size());
         orc_Timestamps.resize(this->mc_VecDataValues[ou32_WidgetDataPoolElementIndex].size());
         for (c_ItItem = this->mc_VecDataValues[ou32_WidgetDataPoolElementIndex].begin();
              c_ItItem != this->mc_VecDataValues[ou32_WidgetDataPoolElementIndex].end();
              ++c_ItItem)
         {
            float64 f64_Value;
            const uint32 u32_Index = c_Id.GetArrayElementIndexOrZero();

            // Convert the value to a float64
            C_SdNdeDpContentUtil::h_GetValueAsFloat64(*c_ItItem, f64_Value, u32_Index);
            // And calculate the scaling on the value
            if (oq_UseScaling == true)
            {
               orc_Values[u32_Counter] = C_OSCUtils::h_GetValueScaled(
                  f64_Value,
                  this->mc_UsedScaling[ou32_WidgetDataPoolElementIndex].f64_Factor,
                  this->mc_UsedScaling[ou32_WidgetDataPoolElementIndex].f64_Offset);
            }
            else
            {
               orc_Values[u32_Counter] = f64_Value;
            }
            // Copy the timestamp
            orc_Timestamps[u32_Counter] = (*c_ItItem).GetTimeStamp();
            ++u32_Counter;
         }

         // Remove all values for the next call
         this->mc_VecDataValues[ou32_WidgetDataPoolElementIndex].clear();
         s32_Return = C_NO_ERR;

         //Save last timestamp
         if (ou32_WidgetDataPoolElementIndex < this->mc_LastDataPoolElementTimeStampsMs.size())
         {
            this->mc_LastDataPoolElementTimeStampsMs[ou32_WidgetDataPoolElementIndex] =
               orc_Timestamps[orc_Timestamps.size() - 1];
         }
         //Set timestamp valid
         if (ou32_WidgetDataPoolElementIndex < this->mc_LastDataPoolElementTimeStampsValid.size())
         {
            auto && rc_Value = this->mc_LastDataPoolElementTimeStampsValid[ou32_WidgetDataPoolElementIndex];
            rc_Value = true;
         }
      }
      else
      {
         s32_Return = C_NOACT;
      }
      this->mc_CriticalSection.Release();
   }
   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Returns the newest registered NVM value of a specific NVM datapool element. It can be an array too.

   The vector will be cleared first.

   If it is an array, the size of the vector is > 1
   If it is no array, the size of the vector is 1
   If an error ocured, the size of the vector is 0

   This function is thread safe.

   \param[in]   ou32_WidgetDataPoolElementIndex    Index of shown datapool element in widget
   \param[out]  orc_Value                          Raw result value

   \return
   C_NO_ERR    Value read
   C_RANGE     Index invalid
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_PuiSvDbDataElementHandler::m_GetLastNvmValue(const uint32 ou32_WidgetDataPoolElementIndex,
                                                      C_OSCNodeDataPoolContent & orc_Value)
{
   sint32 s32_Return = C_RANGE;

   if ((static_cast<sintn>(ou32_WidgetDataPoolElementIndex) < this->mc_VecNvmValue.size()) &&
       (static_cast<sintn>(ou32_WidgetDataPoolElementIndex) < this->mc_VecNvmValueReceived.size()) &&
       (ou32_WidgetDataPoolElementIndex < this->mc_UsedScaling.size()))
   {
      this->mc_CriticalSection.Acquire();

      if (this->mc_VecNvmValueReceived[ou32_WidgetDataPoolElementIndex] == true)
      {
         // Get the newest value
         orc_Value = this->mc_VecNvmValue[ou32_WidgetDataPoolElementIndex];

         // Remove all values for the next call
         this->mc_VecNvmValueReceived[ou32_WidgetDataPoolElementIndex] = false;
         s32_Return = C_NO_ERR;
      }
      else
      {
         s32_Return = C_NOACT;
      }

      this->mc_CriticalSection.Release();
   }

   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get percentage of timeout progress for specified element

   \param[in]   ou32_DataElementIndex     Index of time stamp of last received data package
   \param[out]  oru8_TimoutPercentage100  0-100 Progression of timeout
                                          -> 0: Current value has been received right now
                                          -> 100: Last known value has reached the exact specified timeout
                                          Else  Should not happen

   \return
   C_NO_ERR Valid value
   C_RANGE  Index out of range
   C_NOACT Timeout invalid
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_PuiSvDbDataElementHandler::m_GetTimoutPercentage100(const uint32 ou32_DataElementIndex,
                                                             uint8 & oru8_TimoutPercentage100) const
{
   sint32 s32_Retval;

   if ((ou32_DataElementIndex < this->mc_DataPoolElementValid.size()) &&
       (ou32_DataElementIndex < this->mc_LastDataPoolElementTimeStampsMs.size()) &&
       (ou32_DataElementIndex < this->mc_DataPoolElementTimeoutsMs.size()) &&
       (ou32_DataElementIndex < this->mc_LastDataPoolElementTimeStampsValid.size()))
   {
      if ((this->mc_LastDataPoolElementTimeStampsValid[ou32_DataElementIndex] == true) &&
          (this->mc_DataPoolElementValid[ou32_DataElementIndex] == true))
      {
         const uint32 u32_TimeStampMs = this->mc_LastDataPoolElementTimeStampsMs[ou32_DataElementIndex];
         const uint32 u32_TimeoutMs = this->mc_DataPoolElementTimeoutsMs[ou32_DataElementIndex];
         const uint32 u32_CurrentTimeMs = TGL_GetTickCount();
         const uint32 u32_Percentage =
            (std::min((u32_CurrentTimeMs - u32_TimeStampMs), u32_TimeoutMs) * 100U) / u32_TimeoutMs;
         //Attempt to reduce the jumpy nature of gray out engine -> only use multiples of 10
         oru8_TimoutPercentage100 = static_cast<uint8>(((u32_Percentage + 5UL) / 10UL) * 10UL);
         oru8_TimoutPercentage100 = static_cast<uint8>(std::min(static_cast<sintn>(oru8_TimoutPercentage100), 100));

         if (oru8_TimoutPercentage100 <= stw_opensyde_gui::mu32_DASHBOARD_TIMEOUT_TOLERANCE)
         {
            // Tolerated percentages
            oru8_TimoutPercentage100 = 0U;
         }
         s32_Retval = C_NO_ERR;
      }
      else
      {
         oru8_TimoutPercentage100 = 100;
         s32_Retval = C_NOACT;
      }
   }
   else
   {
      oru8_TimoutPercentage100 = 100;
      s32_Retval = C_RANGE;
   }

   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Scales a minimum and maximum value with the active scaling configuration

   \param[in]      ou32_WidgetDataPoolElementIndex    Index of shown datapool element in widget
   \param[in,out]  orf64_Min                          Minimum value
   \param[in,out]  orf64_Max                          Maximum value

   \return
   C_NO_ERR Valid value
   C_RANGE  Index out of range
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_PuiSvDbDataElementHandler::m_ScaleMinMax(const uint32 ou32_WidgetDataPoolElementIndex, float64 & orf64_Min,
                                                  float64 & orf64_Max) const
{
   sint32 s32_Return = C_RANGE;

   if (ou32_WidgetDataPoolElementIndex < this->mc_UsedScaling.size())
   {
      orf64_Min = C_OSCUtils::h_GetValueScaled(
         orf64_Min,
         this->mc_UsedScaling[ou32_WidgetDataPoolElementIndex].f64_Factor,
         this->mc_UsedScaling[ou32_WidgetDataPoolElementIndex].f64_Offset);

      orf64_Max = C_OSCUtils::h_GetValueScaled(
         orf64_Max,
         this->mc_UsedScaling[ou32_WidgetDataPoolElementIndex].f64_Factor,
         this->mc_UsedScaling[ou32_WidgetDataPoolElementIndex].f64_Offset);

      s32_Return = C_NO_ERR;
   }

   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Updates the timeout values and valid flags for all registered datapool elements
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PuiSvDbDataElementHandler::m_UpdateDataPoolElementTimeoutAndValidFlag(void)
{
   QMap<C_PuiSvDbNodeDataPoolListElementId, stw_types::uint32>::const_iterator c_ItItem;

   for (c_ItItem = this->mc_MappingDpElementToDataSerie.cbegin();
        c_ItItem != this->mc_MappingDpElementToDataSerie.cend();
        ++c_ItItem)
   {
      m_UpdateDataPoolElementTimeoutAndValidFlag(c_ItItem.key());
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Updates the timeout values and valid flags for a specific registered datapool element

   \param[in]  orc_Id   Which datapool element to update
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PuiSvDbDataElementHandler::m_UpdateDataPoolElementTimeoutAndValidFlag(
   const C_PuiSvDbNodeDataPoolListElementId & orc_Id)
{
   const C_PuiSvData * const pc_View = C_PuiSvHandler::h_GetInstance()->GetView(this->mu32_ViewIndex);

   tgl_assert(pc_View != NULL);
   if (pc_View != NULL)
   {
      const QMap<C_PuiSvDbNodeDataPoolListElementId,
                 stw_types::uint32>::const_iterator c_ItItem = this->mc_MappingDpElementToDataSerie.constFind(orc_Id);

      tgl_assert(c_ItItem != this->mc_MappingDpElementToDataSerie.cend());
      if (c_ItItem != this->mc_MappingDpElementToDataSerie.cend())
      {
         const C_PuiSvDbNodeDataPoolListElementId & rc_ElementId = c_ItItem.key();
         auto && rc_Value = this->mc_DataPoolElementValid[c_ItItem.value()];
         uint16 u16_UpdateRate = 0U;
         // Update the valid flag
         rc_Value = rc_ElementId.GetIsValid();

         if (rc_ElementId.GetIsValid() == true)
         {
            sint32 s32_Return;

            if (this->mq_ReadItem == true)
            {
               if (rc_ElementId.GetType() == C_PuiSvDbNodeDataPoolListElementId::eDATAPOOL_ELEMENT)
               {
                  // Update the timeout configuration
                  C_PuiSvReadDataConfiguration c_ReadConfig;
                  s32_Return = pc_View->GetReadRailAssignment(rc_ElementId, c_ReadConfig);

                  if (s32_Return == C_NO_ERR)
                  {
                     s32_Return = pc_View->GetUpdateRate(c_ReadConfig.u8_RailIndex, u16_UpdateRate);
                  }
               }
               else
               {
                  // TODO: Get timeout time from CAN message config
                  s32_Return = C_NO_ERR;
               }

               if (s32_Return != C_NO_ERR)
               {
                  const QString c_ElementDetail = static_cast<QString>(
                     "Problem with Element %1. Indexes: Node %2; Datapool %3; List %4; Element %5")
                                                  .arg(C_PuiSdUtil::h_GetNamespace(rc_ElementId).toStdString().c_str())
                                                  .arg(rc_ElementId.u32_NodeIndex)
                                                  .arg(rc_ElementId.u32_DataPoolIndex)
                                                  .arg(rc_ElementId.u32_ListIndex)
                                                  .arg(rc_ElementId.u32_ElementIndex);
                  tgl_assertdetail(s32_Return == C_NO_ERR, c_ElementDetail.toStdString().c_str());
               }

               if ((s32_Return == C_NO_ERR) &&
                   (c_ItItem.value() < this->mc_DataPoolElementTimeoutsMs.size()))
               {
                  //Ten steps with 0.5 seconds each
                  const uint32 u32_TIMEOUT_MS = 5000U;
                  this->mc_DataPoolElementTimeoutsMs[c_ItItem.value()] = u32_TIMEOUT_MS;
               }
            }
            else
            {
               s32_Return = C_NO_ERR;
            }

            // Update the scaling configuration
            if ((s32_Return == C_NO_ERR) &&
                (c_ItItem.value() < this->mc_UsedScaling.size()))
            {
               QMap<C_PuiSvDbNodeDataPoolListElementId, C_PuiSvDbDataElementScaling>::const_iterator c_ItItemScaling;
               c_ItItemScaling = this->mc_MappingDpElementToScaling.find(c_ItItem.key());

               if (c_ItItemScaling != this->mc_MappingDpElementToScaling.end())
               {
                  C_PuiSvDbDataElementScaling c_Scaling = c_ItItemScaling.value();
                  if (c_Scaling.q_UseDefault == true)
                  {
                     // Use the default scaling configuration of the datapool element in the system definition
                     const C_OSCNodeDataPoolListElement * pc_OSCContent;

                     pc_OSCContent = C_PuiSdHandler::h_GetInstance()->GetOSCDataPoolListElement(
                        rc_ElementId.u32_NodeIndex,
                        rc_ElementId.u32_DataPoolIndex,
                        rc_ElementId.u32_ListIndex,
                        rc_ElementId.u32_ElementIndex);

                     tgl_assert(pc_OSCContent != NULL);
                     if (pc_OSCContent != NULL)
                     {
                        // Set the configured elements of the system definition
                        c_Scaling.f64_Factor = pc_OSCContent->f64_Factor;
                        c_Scaling.f64_Offset = pc_OSCContent->f64_Offset;
                     }
                  }

                  this->mc_UsedScaling[c_ItItem.value()] = c_Scaling;
               }
               else
               {
                  //Should not happen
                  tgl_assert(false);
               }
            }

            // Update the minimum type configuration
            if (((s32_Return == C_NO_ERR) && (c_ItItem.value() < this->mc_MinimumType.size())) &&
                (c_ItItem.value() < this->mc_UsedScaling.size()))
            {
               const C_OSCNodeDataPoolListElement * const pc_OSCContent =
                  C_PuiSdHandler::h_GetInstance()->GetOSCDataPoolListElement(rc_ElementId.u32_NodeIndex,
                                                                             rc_ElementId.u32_DataPoolIndex,
                                                                             rc_ElementId.u32_ListIndex,
                                                                             rc_ElementId.u32_ElementIndex);

               tgl_assert(pc_OSCContent != NULL);
               if (pc_OSCContent != NULL)
               {
                  std::vector<stw_opensyde_core::C_OSCNodeDataPoolContent::E_Type> c_Types;
                  const C_PuiSvDbDataElementScaling & rc_Scaling = this->mc_UsedScaling[c_ItItem.value()];
                  if (C_SdNdeDpContentUtil::h_GetMinimalTypeAfterScaling(pc_OSCContent->c_MinValue,
                                                                         pc_OSCContent->c_MaxValue,
                                                                         rc_Scaling.f64_Factor,
                                                                         rc_Scaling.f64_Offset,
                                                                         c_Types) == C_NO_ERR)
                  {
                     this->mc_MinimumType[c_ItItem.value()] = c_Types;
                  }
               }
            }
         }
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Check for any invalid elements

   \param[out]  orc_FirstInvalidElementName  Name of first invalid element

   \return
   true  Found
   false Not found
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_PuiSvDbDataElementHandler::m_CheckHasValidElements(QString & orc_FirstInvalidElementName) const
{
   bool q_ValidElement = false;
   const QMap<C_PuiSvDbNodeDataPoolListElementId, uint32> & rc_Elements = this->m_GetMappingDpElementToDataSerie();

   for (QMap<C_PuiSvDbNodeDataPoolListElementId, uint32>::const_iterator c_ItElement = rc_Elements.begin();
        c_ItElement != rc_Elements.end(); ++c_ItElement)
   {
      const C_PuiSvDbNodeDataPoolListElementId c_ElementId = c_ItElement.key();

      // Is the data element valid?
      if (c_ElementId.GetIsValid())
      {
         q_ValidElement = true;
      }
      else
      {
         orc_FirstInvalidElementName = c_ElementId.GetInvalidNamePlaceholder();
      }
   }
   return q_ValidElement;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Check for any manual read items

   \return
   true  Found
   false Not found
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_PuiSvDbDataElementHandler::m_CheckManualReadRequired(void) const
{
   bool q_ManualReadElement = false;

   if (this->mq_ReadItem == true)
   {
      // Check for invalid elements
      const QMap<C_PuiSvDbNodeDataPoolListElementId, uint32> & rc_Elements = this->m_GetMappingDpElementToDataSerie();

      for (QMap<C_PuiSvDbNodeDataPoolListElementId, uint32>::const_iterator c_ItElement = rc_Elements.begin();
           c_ItElement != rc_Elements.end(); ++c_ItElement)
      {
         const C_PuiSvDbNodeDataPoolListElementId c_ElementId = c_ItElement.key();

         // Is the data element valid and a datapool element
         if ((c_ElementId.GetIsValid() == true) &&
             (c_ElementId.GetType() == C_PuiSvDbNodeDataPoolListElementId::eDATAPOOL_ELEMENT))
         {
            const C_PuiSvData * const pc_View = C_PuiSvHandler::h_GetInstance()->GetView(this->mu32_ViewIndex);

            C_PuiSvReadDataConfiguration c_ReadConfig;
            if (pc_View != NULL)
            {
               pc_View->GetReadRailAssignment(c_ElementId, c_ReadConfig);
               if (c_ReadConfig.e_TransmissionMode == C_PuiSvReadDataConfiguration::eTM_ON_TRIGGER)
               {
                  q_ManualReadElement = true;
                  break;
               }
            }
         }
      }
   }
   return q_ManualReadElement;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Check for any required nodes inactive

   \return
   true  Found
   false Not found
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_PuiSvDbDataElementHandler::m_CheckHasAnyRequiredNodesActive(void) const
{
   bool q_AtLeastOneValidElement = false;

   const QMap<C_PuiSvDbNodeDataPoolListElementId, uint32> & rc_Elements = this->m_GetMappingDpElementToDataSerie();

   for (QMap<C_PuiSvDbNodeDataPoolListElementId, uint32>::const_iterator c_ItElement = rc_Elements.begin();
        c_ItElement != rc_Elements.end(); ++c_ItElement)
   {
      const C_PuiSvDbNodeDataPoolListElementId c_ElementId = c_ItElement.key();
      if (c_ElementId.GetIsValid() == true)
      {
         const C_PuiSvData * const pc_View = C_PuiSvHandler::h_GetInstance()->GetView(this->mu32_ViewIndex);

         const std::vector<uint8> & rc_ActiveNodes = pc_View->GetNodeActiveFlags();
         //Is corresponding view active
         if ((c_ElementId.u32_NodeIndex < rc_ActiveNodes.size()) &&
             (rc_ActiveNodes[c_ElementId.u32_NodeIndex] == 1U))
         {
            q_AtLeastOneValidElement = true;
            break;
         }
      }
   }

   return q_AtLeastOneValidElement;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Check for any required nodes with valid dashboard routing

   \return
   true  Found
   false Not found
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_PuiSvDbDataElementHandler::m_CheckHasAnyRequiredNodesValidDashboardRouting(void) const
{
   bool q_AtLeastOneValidElement = false;

   const QMap<C_PuiSvDbNodeDataPoolListElementId, uint32> & rc_Elements = this->m_GetMappingDpElementToDataSerie();

   for (QMap<C_PuiSvDbNodeDataPoolListElementId, uint32>::const_iterator c_ItElement = rc_Elements.begin();
        c_ItElement != rc_Elements.end(); ++c_ItElement)
   {
      const C_PuiSvDbNodeDataPoolListElementId c_ElementId = c_ItElement.key();
      if (c_ElementId.GetIsValid() == true)
      {
         bool q_Error = false;
         tgl_assert(C_PuiSvHandler::h_GetInstance()->CheckViewNodeDashboardRoutingError(this->mu32_ViewIndex,
                                                                                        c_ElementId.u32_NodeIndex,
                                                                                        q_Error) ==
                    C_NO_ERR);

         if (q_Error == false)
         {
            q_AtLeastOneValidElement = true;
            break;
         }
      }
   }

   return q_AtLeastOneValidElement;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Check for any required buses not connected in view

   \return
   true  Found
   false Not found
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_PuiSvDbDataElementHandler::m_CheckHasAnyRequiredBusesConnected(void) const
{
   bool q_AtLeastOneValidElement = false;
   const C_PuiSvData * const pc_View = C_PuiSvHandler::h_GetInstance()->GetView(this->mu32_ViewIndex);

   if (pc_View != NULL)
   {
      const QMap<C_PuiSvDbNodeDataPoolListElementId, uint32> & rc_Elements = this->m_GetMappingDpElementToDataSerie();

      for (QMap<C_PuiSvDbNodeDataPoolListElementId, uint32>::const_iterator c_ItElement = rc_Elements.begin();
           c_ItElement != rc_Elements.end(); ++c_ItElement)
      {
         const C_PuiSvDbNodeDataPoolListElementId c_ElementId = c_ItElement.key();
         if ((c_ElementId.GetIsValid() == true) &&
             (c_ElementId.GetType() == C_PuiSvDbNodeDataPoolListElementId::eBUS_SIGNAL))
         {
            C_OSCCanMessageIdentificationIndices c_MessageID;
            uint32 u32_SignalIndex;
            if ((pc_View->GetPcData().GetConnected() == true) &&
                (C_PuiSdUtil::h_ConvertIndex(c_ElementId, c_MessageID, u32_SignalIndex) == C_NO_ERR))
            {
               const C_OSCNode * const pc_Node =
                  C_PuiSdHandler::h_GetInstance()->GetOSCNodeConst(c_MessageID.u32_NodeIndex);
               if ((pc_Node != NULL) && (c_MessageID.u32_InterfaceIndex < pc_Node->c_Properties.c_ComInterfaces.size()))
               {
                  const C_OSCNodeComInterfaceSettings & rc_Interface =
                     pc_Node->c_Properties.c_ComInterfaces[c_MessageID.u32_InterfaceIndex];
                  if (rc_Interface.GetBusConnected() == true)
                  {
                     if (rc_Interface.u32_BusIndex == pc_View->GetPcData().GetBusIndex())
                     {
                        //Active
                        q_AtLeastOneValidElement = true;
                     }
                     else
                     {
                        //Inactive
                        q_AtLeastOneValidElement = false;
                     }
                  }
                  else
                  {
                     //Failure
                     q_AtLeastOneValidElement = false;
                  }
               }
               else
               {
                  //Failure
                  q_AtLeastOneValidElement = false;
               }
            }
            else
            {
               //Failure
               q_AtLeastOneValidElement = false;
            }
         }
         else
         {
            //Allow continue as this means at least one element is not a bus element
            //so this check can at least clarify that one element is valid
            q_AtLeastOneValidElement = true;
         }
         if (q_AtLeastOneValidElement == true)
         {
            break;
         }
      }
   }
   else
   {
      //Failure
      q_AtLeastOneValidElement = false;
   }

   return q_AtLeastOneValidElement;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Check if requested node is active

   \param[in]  ou32_NodeIndex    Node index to check

   \return
   True  Definitely active
   False Either not active or something else is wrong
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_PuiSvDbDataElementHandler::m_CheckNodeActive(const uint32 ou32_NodeIndex) const
{
   bool q_Retval = false;
   const C_PuiSvData * const pc_View = C_PuiSvHandler::h_GetInstance()->GetView(this->mu32_ViewIndex);

   if (pc_View != NULL)
   {
      const std::vector<uint8> & rc_ActiveNodes = pc_View->GetNodeActiveFlags();
      if (ou32_NodeIndex < rc_ActiveNodes.size())
      {
         if (rc_ActiveNodes[ou32_NodeIndex] == true)
         {
            q_Retval = true;
         }
      }
   }

   return q_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Check for node valid dashboard routing

   \param[in]  ou32_NodeIndex    Node index to check

   \return
   true  Dashboard routing valid
   false Dashboard routing not valid
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_PuiSvDbDataElementHandler::m_CheckNodeHasAnyRequiredValidDashboardRouting(const uint32 ou32_NodeIndex) const
{
   bool q_Valid;
   bool q_Error = false;

   tgl_assert(C_PuiSvHandler::h_GetInstance()->CheckViewNodeDashboardRoutingError(this->mu32_ViewIndex,
                                                                                  ou32_NodeIndex,
                                                                                  q_Error) ==
              C_NO_ERR);

   q_Valid  = !q_Error;

   return q_Valid;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Check if the specified ID is configured as on trigger

   \param[in]  orc_Id   ID to check

   \retval   true   Error occurred or on trigger configured
   \retval   false  Definitely not on trigger configured
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_PuiSvDbDataElementHandler::m_CheckIsOnTrigger(const C_PuiSvDbNodeDataPoolListElementId & orc_Id) const
{
   bool q_Retval = true;

   if (orc_Id.GetType() == C_PuiSvDbNodeDataPoolListElementId::eDATAPOOL_ELEMENT)
   {
      const C_PuiSvData * const pc_View = C_PuiSvHandler::h_GetInstance()->GetView(this->mu32_ViewIndex);

      if (pc_View != NULL)
      {
         const QMap<C_OSCNodeDataPoolListElementId,
                    C_PuiSvReadDataConfiguration> & rc_RailAssignments = pc_View->GetReadRailAssignments();
         const QMap<C_OSCNodeDataPoolListElementId,
                    C_PuiSvReadDataConfiguration>::const_iterator c_ItRailAssignment = rc_RailAssignments.find(
            orc_Id);
         if (c_ItRailAssignment != rc_RailAssignments.end())
         {
            //Only allow manual transmission for "on trigger" elements
            if (c_ItRailAssignment.value().e_TransmissionMode != C_PuiSvReadDataConfiguration::eTM_ON_TRIGGER)
            {
               q_Retval = false;
            }
         }
      }
   }
   else
   {
      //Never trigger manual for bus element
      q_Retval = false;
   }

   return q_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Check if the element was already read, assuming the order was in the internally stored data element order

   \param[in]  ou32_ItemIndex    Internally stored data element order index
   \param[in]  orc_Id            ID of the current index

   \retval   true    Already read
   \retval   false   New read required
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_PuiSvDbDataElementHandler::m_CheckElementAlreadyRead(const uint32 ou32_ItemIndex,
                                                            const C_PuiSvDbNodeDataPoolListElementId & orc_Id) const
{
   bool q_AlreadyRead = false;

   for (uint32 u32_ItItem = 0UL; (u32_ItItem < this->GetWidgetDataPoolElementCount()) && (u32_ItItem < ou32_ItemIndex);
        ++u32_ItItem)
   {
      C_PuiSvDbNodeDataPoolListElementId c_CurId;
      if (this->GetDataPoolElementIndex(u32_ItItem, c_CurId) == C_NO_ERR)
      {
         if (c_CurId.CheckSameDataElement(orc_Id))
         {
            q_AlreadyRead = true;
            break;
         }
      }
   }
   return q_AlreadyRead;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Event function if the datapool element configuration was changed

   Base class implementation does nothing. If there are configurable properties for the dashboard element
   the derived class must reimplement this function.
*/
//----------------------------------------------------------------------------------------------------------------------
//lint -e{9175}  //intentionally no functionality in default implementation
void C_PuiSvDbDataElementHandler::m_DataPoolElementsChanged(void)
{
   // Nothing to do in the base class implementation
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get error description for manual operation

   \param[in]   os32_Result         Operation result
   \param[in]   ou8_NRC             Negative response code, if any
   \param[out]  orc_Description     Error description
   \param[out]  orc_Details         Details for error
   \param[out]  ors32_TextHeight    Necessary height of text
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PuiSvDbDataElementHandler::m_GetErrorDescriptionForManualOperation(const sint32 os32_Result, const uint8 ou8_NRC,
                                                                          QString & orc_Description,
                                                                          QString & orc_Details,
                                                                          sint32 & ors32_TextHeight) const
{
   switch (os32_Result)
   {
   case C_CONFIG:
   case C_RANGE:
   case C_RD_WR:
   case C_OVERFLOW:
      orc_Description = C_GtGetText::h_GetText("Operation failed with an internal error.");
      //Update log file
      C_OSCLoggingHandler::h_Flush();
      orc_Details = static_cast<QString>("%1<a href=\"file:%2\"><span style=\"color: %3;\">%4</span></a>.").
                    arg(C_GtGetText::h_GetText("For details see ")).
                    arg(C_OSCLoggingHandler::h_GetCompleteLogFileLocation().c_str()).
                    arg(stw_opensyde_gui::mc_STYLESHEET_GUIDE_COLOR_LINK).
                    arg(C_GtGetText::h_GetText("log file"));
      break;
   case C_TIMEOUT:
      orc_Description = C_GtGetText::h_GetText("Operation did not get a response within timeout interval.");
      break;
   case C_NOACT:
      orc_Description = C_GtGetText::h_GetText("Operation could not send request (e.g. Tx buffer full).");
      break;
   case C_WARN:
      if (this->mq_ReadItem == true)
      {
         switch (ou8_NRC)
         {
         case 0x13:
            orc_Details = C_GtGetText::h_GetText("Incorrect length of request.");
            ors32_TextHeight = 250;
            break;
         case 0x31:
            orc_Details = C_GtGetText::h_GetText("Datapool element specified by data identifier is not available.");
            ors32_TextHeight = 250;
            break;
         case 0x22:
            orc_Details = C_GtGetText::h_GetText("Access to Datapool element blocked by application.");
            ors32_TextHeight = 250;
            break;
         case 0x33:
            orc_Details = C_GtGetText::h_GetText("Required security level was not unlocked.");
            ors32_TextHeight = 250;
            break;
         case 0x14:
            orc_Details = C_GtGetText::h_GetText(
               "The total length of the response message exceeds the available buffer size.");
            ors32_TextHeight = 250;
            break;
         case 0x7F:
            orc_Details = C_GtGetText::h_GetText(
               "The requested service is not available in the currently active session.");
            ors32_TextHeight = 250;
            break;
         default:
            orc_Details =
               static_cast<QString>(C_GtGetText::h_GetText("Unknown NRC: 0x%1")).arg(QString::number(ou8_NRC,
                                                                                                     16));
            ors32_TextHeight = 250;
            break;
         }
      }
      else
      {
         switch (ou8_NRC)
         {
         case 0x13:
            orc_Details = C_GtGetText::h_GetText("Incorrect length of request.\n"
                                                 "Specifically for this service: Size of payload data does not "
                                                 "match size of Datapool element.");
            ors32_TextHeight = 300;
            break;
         case 0x31:
            orc_Details = C_GtGetText::h_GetText("Datapool element specified by data identifier is not available.");
            ors32_TextHeight = 250;
            break;
         case 0x22:
            orc_Details = C_GtGetText::h_GetText("Access to Datapool element blocked by application.");
            ors32_TextHeight = 250;
            break;
         case 0x33:
            orc_Details = C_GtGetText::h_GetText("Required security level was not unlocked.");
            ors32_TextHeight = 250;
            break;
         case 0x14:
            orc_Details = C_GtGetText::h_GetText(
               "The total length of the response message exceeds the available buffer size.");
            ors32_TextHeight = 250;
            break;
         case 0x7F:
            orc_Details = C_GtGetText::h_GetText(
               "The requested service is not available in the currently active session.");
            ors32_TextHeight = 250;
            break;
         default:
            orc_Details =
               static_cast<QString>(C_GtGetText::h_GetText("Unknown NRC: 0x%1")).arg(QString::number(ou8_NRC,
                                                                                                     16));
            ors32_TextHeight = 250;
            break;
         }
      }
      orc_Description =
         static_cast<QString>(C_GtGetText::h_GetText("Operation failed with error response (%1).")).arg(orc_Details);
      orc_Details = "";
      ors32_TextHeight = 230;
      break;
   default:
      orc_Description = C_GtGetText::h_GetText("Operation failure, cause unknown.");
      ors32_TextHeight = 180;
      break;
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get configured scaling configuration for each data element

   \return
   Current configured scaling configuration for each data element
*/
//----------------------------------------------------------------------------------------------------------------------
const QMap<C_PuiSvDbNodeDataPoolListElementId, uint32> & C_PuiSvDbDataElementHandler::m_GetMappingDpElementToDataSerie(
   void) const
{
   return this->mc_MappingDpElementToDataSerie;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Returns all registered elements

   \param[out]  orc_RegisteredElements    Vector with all elements
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PuiSvDbDataElementHandler::m_GetAllRegisteredElements(
   std::vector<C_PuiSvDbNodeDataPoolListElementId> & orc_RegisteredElements) const
{
   QMap<C_PuiSvDbNodeDataPoolListElementId, C_PuiSvDbDataElementScaling>::const_iterator c_ItItem;

   orc_RegisteredElements.clear();
   orc_RegisteredElements.reserve(this->mc_MappingDpElementToScaling.size());
   for (c_ItItem = this->mc_MappingDpElementToScaling.begin(); c_ItItem != this->mc_MappingDpElementToScaling.end();
        ++c_ItItem)
   {
      orc_RegisteredElements.push_back(c_ItItem.key());
   }
}

//----------------------------------------------------------------------------------------------------------------------
void C_PuiSvDbDataElementHandler::m_SetWidgetDataPoolElementCount(const uint32 ou32_Count)
{
   this->mu32_WidgetDataPoolElementCount = ou32_Count;
   this->mc_VecDataValues.resize(this->mu32_WidgetDataPoolElementCount);
   this->mc_VecNvmValue.resize(this->mu32_WidgetDataPoolElementCount);
   this->mc_VecNvmValueReceived.resize(this->mu32_WidgetDataPoolElementCount);
   this->mc_LastDataPoolElementTimeStampsMs.resize(this->mu32_WidgetDataPoolElementCount);
   this->mc_LastDataPoolElementTimeStampsValid.resize(this->mu32_WidgetDataPoolElementCount);
   this->mc_DataPoolElementTimeoutsMs.resize(this->mu32_WidgetDataPoolElementCount);
   this->mc_DataPoolElementValid.resize(this->mu32_WidgetDataPoolElementCount);
   this->mc_UsedScaling.resize(this->mu32_WidgetDataPoolElementCount);
   this->mc_MinimumType.resize(this->mu32_WidgetDataPoolElementCount);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get string for scaled value

   \param[in]  of64_Value  Scaled value
   \param[in]  oe_Type     Target type after scaling

   \return
   String for value
*/
//----------------------------------------------------------------------------------------------------------------------
QString C_PuiSvDbDataElementHandler::mh_GetStringForScaledValue(const float64 of64_Value,
                                                                const C_OSCNodeDataPoolContent::E_Type oe_Type)
{
   QString c_Retval;

   switch (oe_Type)
   {
   case C_OSCNodeDataPoolContent::eUINT8:
      c_Retval = QString::number(static_cast<uint8>(of64_Value));
      break;
   case C_OSCNodeDataPoolContent::eUINT16:
      c_Retval = QString::number(static_cast<uint16>(of64_Value));
      break;
   case C_OSCNodeDataPoolContent::eUINT32:
      c_Retval = QString::number(static_cast<uint32>(of64_Value));
      break;
   case C_OSCNodeDataPoolContent::eUINT64:
      c_Retval = QString::number(static_cast<uint64>(of64_Value));
      break;
   case C_OSCNodeDataPoolContent::eSINT8:
      c_Retval = QString::number(static_cast<sint8>(of64_Value));
      break;
   case C_OSCNodeDataPoolContent::eSINT16:
      c_Retval = QString::number(static_cast<sint16>(of64_Value));
      break;
   case C_OSCNodeDataPoolContent::eSINT32:
      c_Retval = QString::number(static_cast<sint32>(of64_Value));
      break;
   case C_OSCNodeDataPoolContent::eSINT64:
      c_Retval = QString::number(static_cast<sint64>(of64_Value));
      break;
   case C_OSCNodeDataPoolContent::eFLOAT32:
   case C_OSCNodeDataPoolContent::eFLOAT64:
      c_Retval = QString::number(of64_Value);
      break;
   default:
      tgl_assert(false);
      break;
   }
   return c_Retval;
}
