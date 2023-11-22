//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Base functions and data storing for datapool elements and its content (implementation)

   detailed description

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.hpp"

#include "stwerrors.hpp"
#include "constants.hpp"
#include "C_OscLoggingHandler.hpp"
#include "C_GtGetText.hpp"
#include "C_PuiSvDbDataElementHandler.hpp"
#include "C_PuiSvHandler.hpp"
#include "C_PuiSvData.hpp"
#include "C_OscUtils.hpp"
#include "TglUtils.hpp"
#include "TglTime.hpp"
#include "C_PuiSdUtil.hpp"
#include "C_PuiSdHandler.hpp"
#include "C_SdNdeDpContentUtil.hpp"
#include "C_OscNodeDataPoolContentUtil.hpp"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw::tgl;
using namespace stw::errors;
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

   long description of function within several lines

   \param[in]  oru32_ViewIndex            Index of system view
   \param[in]  oru32_DashboardIndex       Index of dashboard in system view
   \param[in]  ors32_DataIndex            Index of connected data item
   \param[in]  ore_Type                   Type of data
   \param[in]  ou32_MaximumDataElements   Maximum number of shown data elements of the widget
   \param[in]  oq_ReadItem                Flag if item is a read only item and has an rail assignment
*/
//----------------------------------------------------------------------------------------------------------------------
C_PuiSvDbDataElementHandler::C_PuiSvDbDataElementHandler(const uint32_t & oru32_ViewIndex,
                                                         const uint32_t & oru32_DashboardIndex,
                                                         const int32_t & ors32_DataIndex, const E_Type & ore_Type,
                                                         const uint32_t ou32_MaximumDataElements,
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
   \param[in]  orc_DataPoolElementFormatter  Datapool element formatter configuration

   \return
   C_NO_ERR    Datapool element added
   C_NOACT     Datapool element was added already. Nothing done.
   C_RANGE     No further datapool elements can be added. Nothing done.
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_PuiSvDbDataElementHandler::RegisterDataPoolElement(
   const C_PuiSvDbNodeDataPoolListElementId & orc_WidgetDataPoolElementId,
   const C_PuiSvDbDataElementScaling & orc_DataPoolElementScaling,
   const C_PuiSvDbDataElementDisplayFormatter & orc_DataPoolElementFormatter)
{
   int32_t s32_Return = C_RANGE;

   if (this->mc_MappingDpElementToDataSerie.size() < static_cast<int32_t>(this->mu32_WidgetDataPoolElementCountMaximum))
   {
      QMap<stw::opensyde_gui_logic::C_PuiSvDbNodeDataPoolListElementId, uint32_t>::iterator c_ItItem;

      c_ItItem = this->mc_MappingDpElementToDataSerie.find(orc_WidgetDataPoolElementId);

      this->mc_CriticalSection.Acquire();

      if (c_ItItem == this->mc_MappingDpElementToDataSerie.end())
      {
         C_DpElementConfig c_ElementConfig;

         const uint32_t u32_WidgetDataPoolElementIndex =
            static_cast<uint32_t>(this->mc_MappingDpElementToDataSerie.size());

         // Use the current count of maps as index. It will be used as index for the vector mc_VecDataValues
         this->mc_MappingDpElementToDataSerie.insert(
            orc_WidgetDataPoolElementId,
            u32_WidgetDataPoolElementIndex);

         c_ElementConfig.c_Scaling = orc_DataPoolElementScaling;
         c_ElementConfig.c_FormatterConfig.SetDisplayFormatter(orc_DataPoolElementFormatter);

         this->mc_MappingDpElementToConfig.insert(orc_WidgetDataPoolElementId, c_ElementConfig);

         // Update the counter
         this->m_SetWidgetDataPoolElementCount(this->mc_MappingDpElementToDataSerie.size());
         this->m_UpdateDataPoolElementTimeoutAndValidFlag(orc_WidgetDataPoolElementId);
         this->m_DataPoolElementsChanged();

         // The Datapool element config was changed eventually without adding as new element. Inform in both cases
         this->m_OnDataElementRegistered(u32_WidgetDataPoolElementIndex);

         s32_Return = C_NO_ERR;
      }
      else
      {
         // Datapool element was added already
         s32_Return = C_NOACT;
      }

      this->mc_CriticalSection.Release();
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
   const C_PuiSvDbNodeDataPoolListElementId & orc_WidgetDataPoolElementId, const uint8_t ou8_ErrorCode)
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
   QMap<C_PuiSvDbNodeDataPoolListElementId, uint32_t>::iterator c_ItItemDataSerie;
   QMap<C_PuiSvDbNodeDataPoolListElementId, C_DpElementConfig>::iterator c_ItItemScaling;

   this->mc_CriticalSection.Acquire();

   c_ItItemDataSerie = this->mc_MappingDpElementToDataSerie.find(orc_WidgetDataPoolElementId);

   if (c_ItItemDataSerie != this->mc_MappingDpElementToDataSerie.end())
   {
      const uint32_t u32_ValueOfDeletedItem = c_ItItemDataSerie.value();

      this->mc_MappingDpElementToDataSerie.erase(c_ItItemDataSerie);

      // Update the counter
      this->m_SetWidgetDataPoolElementCount(this->mc_MappingDpElementToDataSerie.size());

      // Update the indexes of the other registered elements
      if (this->mc_MappingDpElementToDataSerie.size() > 0)
      {
         QMap<C_PuiSvDbNodeDataPoolListElementId, uint32_t>::iterator c_ItItem;
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
   c_ItItemScaling = this->mc_MappingDpElementToConfig.find(orc_WidgetDataPoolElementId);

   if (c_ItItemScaling != this->mc_MappingDpElementToConfig.end())
   {
      this->mc_MappingDpElementToConfig.erase(c_ItItemScaling);
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
   this->mc_MappingDpElementToConfig.clear();
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
   const uint32_t u32_Count = this->GetWidgetDataPoolElementCount();

   std::vector<C_PuiSvDbNodeDataPoolListElementId> c_Ids;
   std::vector<C_PuiSvDbDataElementScaling> c_Scalings;
   std::vector<C_PuiSvDbDataElementDisplayFormatter> c_FormatterConfis;

   //Reserve
   c_Ids.resize(u32_Count);
   c_Scalings.resize(u32_Count);
   c_FormatterConfis.resize(u32_Count);
   //Reregister all data elements
   for (uint32_t u32_It = 0; u32_It < u32_Count; ++u32_It)
   {
      tgl_assert(this->GetDataPoolElementIndex(u32_It, c_Ids[u32_It]) == C_NO_ERR);
      tgl_assert(this->GetDataPoolElementScaling(u32_It, c_Scalings[u32_It]) == C_NO_ERR);
      tgl_assert(this->GetDataPoolElementFormatter(u32_It, c_FormatterConfis[u32_It]) == C_NO_ERR);
   }
   this->ClearDataPoolElements();
   for (uint32_t u32_It = 0; u32_It < u32_Count; ++u32_It)
   {
      this->RegisterDataPoolElement(c_Ids[u32_It], c_Scalings[u32_It], c_FormatterConfis[u32_It]);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Returns the count of shown datapool elements

   \return
   Count of shown datapool elements
*/
//----------------------------------------------------------------------------------------------------------------------
uint32_t C_PuiSvDbDataElementHandler::GetWidgetDataPoolElementCount(void) const
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
int32_t C_PuiSvDbDataElementHandler::GetDataPoolElementIndex(const uint32_t ou32_WidgetDataPoolElementIndex,
                                                             C_PuiSvDbNodeDataPoolListElementId & orc_DataPoolId) const
{
   int32_t s32_Return = C_RANGE;

   QMap<stw::opensyde_gui_logic::C_PuiSvDbNodeDataPoolListElementId, uint32_t>::const_iterator c_ItItem;

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
int32_t C_PuiSvDbDataElementHandler::GetWidgetDataPoolElementIndex(
   const C_PuiSvDbNodeDataPoolListElementId & orc_DataPoolId, uint32_t & oru32_WidgetDataPoolElementIndex) const
{
   int32_t s32_Return = C_RANGE;

   QMap<stw::opensyde_gui_logic::C_PuiSvDbNodeDataPoolListElementId, uint32_t>::const_iterator c_ItItem;

   c_ItItem = this->mc_MappingDpElementToDataSerie.find(orc_DataPoolId);

   if (c_ItItem != this->mc_MappingDpElementToDataSerie.end())
   {
      oru32_WidgetDataPoolElementIndex = c_ItItem.value();
      s32_Return = C_NO_ERR;
   }

   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Gets all scaling information about the registered datapool element

   \param[in]   ou32_WidgetDataPoolElementIndex    Index of shown datapool element in widget
   \param[out]  orc_DataPoolScaling                Datapool element scaling configuration

   \return
   C_NO_ERR    Datapool element index valid and index id was filled
   C_RANGE     Datapool element index not valid. Id was not filled.
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_PuiSvDbDataElementHandler::GetDataPoolElementScaling(const uint32_t ou32_WidgetDataPoolElementIndex,
                                                               C_PuiSvDbDataElementScaling & orc_DataPoolScaling) const
{
   C_PuiSvDbNodeDataPoolListElementId c_ElementId;
   int32_t s32_Return = this->GetDataPoolElementIndex(ou32_WidgetDataPoolElementIndex, c_ElementId);

   if (s32_Return == C_NO_ERR)
   {
      QMap<C_PuiSvDbNodeDataPoolListElementId, C_DpElementConfig>::const_iterator c_ItItem;

      c_ItItem = this->mc_MappingDpElementToConfig.find(c_ElementId);

      if (c_ItItem != this->mc_MappingDpElementToConfig.end())
      {
         orc_DataPoolScaling = c_ItItem.value().c_Scaling;
      }
      else
      {
         s32_Return = C_RANGE;
      }
   }

   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Gets all display formatter information about the registered datapool element

   \param[in]   ou32_WidgetDataPoolElementIndex    Index of shown datapool element in widget
   \param[out]  orc_FormatterConfig                Datapool element formatter configuration

   \return
   C_NO_ERR    Datapool element index valid and index id was filled
   C_RANGE     Datapool element index not valid. Id was not filled.
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_PuiSvDbDataElementHandler::GetDataPoolElementFormatter(const uint32_t ou32_WidgetDataPoolElementIndex,
                                                                 C_PuiSvDbDataElementDisplayFormatter & orc_FormatterConfig)
const
{
   C_PuiSvDbNodeDataPoolListElementId c_ElementId;
   int32_t s32_Return = this->GetDataPoolElementIndex(ou32_WidgetDataPoolElementIndex, c_ElementId);

   if (s32_Return == C_NO_ERR)
   {
      QMap<C_PuiSvDbNodeDataPoolListElementId, C_DpElementConfig>::const_iterator c_ItItem;

      c_ItItem = this->mc_MappingDpElementToConfig.find(c_ElementId);

      if (c_ItItem != this->mc_MappingDpElementToConfig.end())
      {
         orc_FormatterConfig = c_ItItem.value().c_FormatterConfig;
      }
      else
      {
         s32_Return = C_RANGE;
      }
   }

   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Gets all configurred display formatter information about the registered datapool element

   \param[in]   ou32_WidgetDataPoolElementIndex    Index of shown datapool element in widget
   \param[out]  orc_FormatterConfig                Configurred datapool element formatter configuration

   \return
   C_NO_ERR    Datapool element index valid and index id was filled
   C_RANGE     Datapool element index not valid. Id was not filled.
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_PuiSvDbDataElementHandler::GetDataPoolElementFormatterConfig(const uint32_t ou32_WidgetDataPoolElementIndex,
                                                                       C_PuiSvDbDataElementDisplayFormatterConfig & orc_FormatterConfig)
const
{
   int32_t s32_Return = C_NO_ERR;

   if (ou32_WidgetDataPoolElementIndex < this->mc_UsedConfig.size())
   {
      orc_FormatterConfig = this->mc_UsedConfig[ou32_WidgetDataPoolElementIndex].c_FormatterConfig;
   }
   else
   {
      s32_Return = C_RANGE;
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

   QMap<C_PuiSvDbNodeDataPoolListElementId, uint32_t>::const_iterator c_ItItem;

   // Check the data series
   c_ItItem = this->mc_MappingDpElementToDataSerie.find(orc_WidgetDataPoolElementId);

   if (c_ItItem != this->mc_MappingDpElementToDataSerie.end())
   {
      q_Return = true;
   }

   return q_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Returns the flag if element is a read or write item

   \retval   true    Read item
   \retval   false   Write item
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_PuiSvDbDataElementHandler::IsReadItem() const
{
   return this->mq_ReadItem;
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
   QMap<stw::opensyde_gui_logic::C_PuiSvDbNodeDataPoolListElementId, uint32_t>::iterator c_ItItem;

   //manual find because match is unreliable
   for (c_ItItem = this->mc_MappingDpElementToDataSerie.begin(); c_ItItem != this->mc_MappingDpElementToDataSerie.end();
        ++c_ItItem)
   {
      // Get the correct data series
      if (c_ItItem.key().CheckSameDataElement(orc_WidgetDataPoolElementId))
      {
         // Add the new value to the correct data series
         const uint32_t u32_DataSerieIndex = c_ItItem.value();
         if (static_cast<int32_t>(u32_DataSerieIndex) < this->mc_VecDataValues.size())
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
   QMap<stw::opensyde_gui_logic::C_PuiSvDbNodeDataPoolListElementId, uint32_t>::iterator c_ItItem;

   // Get the correct data series
   c_ItItem = this->mc_MappingDpElementToDataSerie.find(orc_WidgetDataPoolElementId);

   if (c_ItItem != this->mc_MappingDpElementToDataSerie.end())
   {
      // Add the new value to the correct data series
      const uint32_t u32_DataSerieIndex = c_ItItem.value();
      if ((static_cast<int32_t>(u32_DataSerieIndex) < this->mc_VecNvmValue.size()) &&
          (static_cast<int32_t>(u32_DataSerieIndex) < this->mc_VecNvmValueReceived.size()))
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
QString C_PuiSvDbDataElementHandler::GetUnscaledValueAsScaledString(const float64_t of64_Value,
                                                                    const uint32_t ou32_IndexElement) const
{
   QString c_Retval;
   C_PuiSvDbDataElementScaling c_Scaling;
   C_PuiSvDbNodeDataPoolListElementId c_Id;
   C_PuiSvDbDataElementDisplayFormatterConfig c_FormatterConfig;

   if ((this->GetDataPoolElementIndex(ou32_IndexElement, c_Id) == C_NO_ERR) &&
       (this->GetDataPoolElementScaling(ou32_IndexElement, c_Scaling) == C_NO_ERR) &&
       (this->GetDataPoolElementFormatterConfig(ou32_IndexElement, c_FormatterConfig) == C_NO_ERR))
   {
      const C_OscNodeDataPoolListElement * const pc_Element =
         C_PuiSdHandler::h_GetInstance()->GetOscDataPoolListElement(c_Id.u32_NodeIndex, c_Id.u32_DataPoolIndex,
                                                                    c_Id.u32_ListIndex, c_Id.u32_ElementIndex);
      if (pc_Element != NULL)
      {
         C_OscNodeDataPoolContent c_Tmp = pc_Element->c_MinValue;
         C_OscNodeDataPoolContentUtil::h_SetValueInContent(of64_Value, c_Tmp, 0UL);
         c_Retval = c_FormatterConfig.GetSingleValueContentFormatted(c_Tmp, 0UL, c_Scaling, NULL);
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
QString C_PuiSvDbDataElementHandler::GetUnscaledValueInRangeAsScaledString(const float64_t of64_Value,
                                                                           const uint32_t ou32_IndexElement,
                                                                           float64_t * const opf64_Progress)
const
{
   QString c_Retval;
   C_PuiSvDbDataElementScaling c_Scaling;
   C_PuiSvDbNodeDataPoolListElementId c_Id;
   C_PuiSvDbDataElementDisplayFormatterConfig c_FormatterConfig;

   if ((this->GetDataPoolElementIndex(ou32_IndexElement, c_Id) == C_NO_ERR) &&
       (this->GetDataPoolElementScaling(ou32_IndexElement, c_Scaling) == C_NO_ERR) &&
       (this->GetDataPoolElementFormatterConfig(ou32_IndexElement, c_FormatterConfig) == C_NO_ERR))
   {
      const C_OscNodeDataPoolListElement * const pc_Element =
         C_PuiSdHandler::h_GetInstance()->GetOscDataPoolListElement(c_Id.u32_NodeIndex, c_Id.u32_DataPoolIndex,
                                                                    c_Id.u32_ListIndex, c_Id.u32_ElementIndex);
      if (pc_Element != NULL)
      {
         //Restrict value to min & max
         C_OscNodeDataPoolContentUtil::E_ValueChangedTo e_Change;
         C_OscNodeDataPoolContent c_Tmp = pc_Element->c_MinValue;
         C_OscNodeDataPoolContentUtil::h_SetValueInContent(of64_Value, c_Tmp, 0UL);
         C_OscNodeDataPoolContentUtil::h_SetValueInMinMaxRange(pc_Element->c_MinValue, pc_Element->c_MaxValue, c_Tmp,
                                                               e_Change);
         c_Retval = c_FormatterConfig.GetSingleValueContentFormatted(c_Tmp, 0UL, c_Scaling, NULL);
         if (opf64_Progress != NULL)
         {
            float64_t f64_Min;
            float64_t f64_Max;
            float64_t f64_Value;
            const uint32_t u32_Index = c_Id.GetArrayElementIndexOrZero();
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
/*! \brief   Function for handling a new registered data element

   Called at the end of RegisterDataPoolElement after registration of a specific Datapool element

   Here: Empty implementation. Derived classes must implement specific handling if necessary

   \param[in]       ou32_WidgetDataPoolElementIndex     Index of new registered element
*/
//----------------------------------------------------------------------------------------------------------------------
//lint -e{9175}  //intentionally no functionality in default implementation
void C_PuiSvDbDataElementHandler::m_OnDataElementRegistered(const uint32_t ou32_WidgetDataPoolElementIndex)
{
   Q_UNUSED(ou32_WidgetDataPoolElementIndex)
   // Nothing to do here
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Returns the newest registered value of a specific datapool element as formatted and scaled string

   This function is thread safe.

   \param[in]     ou32_WidgetDataPoolElementIndex Index of shown datapool element in widget
   \param[out]    orc_ScaledValue                 String with result string
   \param[out]    opf64_UnscaledValueAsFloat      Optional float with unscaled value
   \param[out]    opf64_ScaledValueAsFloat        Optional float with scaled value

   \return
   C_NO_ERR    Value read
   C_RANGE     Index invalid
   C_NOACT     No value received
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_PuiSvDbDataElementHandler::m_GetLastValue(const uint32_t ou32_WidgetDataPoolElementIndex,
                                                    QString & orc_ScaledValue,
                                                    float64_t * const opf64_UnscaledValueAsFloat,
                                                    float64_t * const opf64_ScaledValueAsFloat)
{
   int32_t s32_Return = C_RANGE;
   C_PuiSvDbNodeDataPoolListElementId c_Id;

   if (((static_cast<int32_t>(ou32_WidgetDataPoolElementIndex) < this->mc_VecDataValues.size()) &&
        (ou32_WidgetDataPoolElementIndex < this->mc_UsedConfig.size())) && (
          this->GetDataPoolElementIndex(ou32_WidgetDataPoolElementIndex, c_Id) == C_NO_ERR))
   {
      this->mc_CriticalSection.Acquire();
      if (this->mc_VecDataValues[ou32_WidgetDataPoolElementIndex].size() > 0)
      {
         const int32_t s32_LastIndex = this->mc_VecDataValues[ou32_WidgetDataPoolElementIndex].size() - 1;
         const uint32_t u32_ArrayIndex = c_Id.GetArrayElementIndexOrZero();

         orc_ScaledValue = this->mc_UsedConfig[ou32_WidgetDataPoolElementIndex].GetSingleValueContentFormatted(
            this->mc_VecDataValues[ou32_WidgetDataPoolElementIndex][s32_LastIndex],
            u32_ArrayIndex, opf64_UnscaledValueAsFloat, opf64_ScaledValueAsFloat);

         this->m_SaveTimeStamp(ou32_WidgetDataPoolElementIndex, s32_LastIndex);

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
/*! \brief   Returns the newest registered value of a specific datapool element. It can be an array too.

   The vector will be cleared first.

   If it is an array, the size of the vector is > 1
   If it is no array, the size of the vector is 1
   If an error occurred, the size of the vector is 0

   Special case: If the element is configured as string (sint8 array): The return value has one string and the
   orc_UnscaledValueAsFloat array has one value with 0.0

   This function is thread safe.

   \param[in]   ou32_WidgetDataPoolElementIndex    Index of shown datapool element in widget
   \param[out]  orc_ScaledValues                   Vector with result scaled and formatted value(s)
   \param[out]  orc_UnscaledValues                 Vector with unscaled float values

   \return
   C_NO_ERR    Value read
   C_RANGE     Index invalid
   C_NOACT     No value received
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_PuiSvDbDataElementHandler::m_GetLastValue(const uint32_t ou32_WidgetDataPoolElementIndex,
                                                    std::vector<QString> & orc_ScaledValues,
                                                    std::vector<float64_t> & orc_UnscaledValues)
{
   int32_t s32_Return = C_RANGE;

   orc_ScaledValues.clear();
   orc_UnscaledValues.clear();

   if ((static_cast<int32_t>(ou32_WidgetDataPoolElementIndex) < this->mc_VecDataValues.size()) &&
       (ou32_WidgetDataPoolElementIndex < this->mc_UsedConfig.size()))
   {
      this->mc_CriticalSection.Acquire();
      if (this->mc_VecDataValues[ou32_WidgetDataPoolElementIndex].size() > 0)
      {
         const int32_t s32_LastIndex = this->mc_VecDataValues[ou32_WidgetDataPoolElementIndex].size() - 1;

         // Get the newest value(s)
         orc_ScaledValues = this->mc_UsedConfig[ou32_WidgetDataPoolElementIndex].GetValuesContentFormatted(
            this->mc_VecDataValues[ou32_WidgetDataPoolElementIndex][s32_LastIndex],
            orc_UnscaledValues);

         this->m_SaveTimeStamp(ou32_WidgetDataPoolElementIndex, s32_LastIndex);

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
/*! \brief   Returns the newest registered value of a specific datapool element. It can be an array too.

   The vector will be cleared first.

   If it is an array, the size of the vector is > 1
   If it is no array, the size of the vector is 1
   If an error occurred, the size of the vector is 0

   This function is thread safe.

   \param[in]   ou32_WidgetDataPoolElementIndex    Index of shown datapool element in widget
   \param[out]  orc_Values                         Vector with result value(s)
   \param[in]   oq_UseScaling                      Optional flag if return value should include internally stored scaling

   \return
   C_NO_ERR    Value read
   C_RANGE     Index invalid
   C_NOACT     No value received
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_PuiSvDbDataElementHandler::m_GetLastValue(const uint32_t ou32_WidgetDataPoolElementIndex,
                                                    std::vector<float64_t> & orc_Values, const bool oq_UseScaling)
{
   int32_t s32_Return = C_RANGE;

   orc_Values.clear();

   if ((static_cast<int32_t>(ou32_WidgetDataPoolElementIndex) < this->mc_VecDataValues.size()) &&
       ((ou32_WidgetDataPoolElementIndex < this->mc_UsedConfig.size()) || (oq_UseScaling == false)))
   {
      this->mc_CriticalSection.Acquire();
      if (this->mc_VecDataValues[ou32_WidgetDataPoolElementIndex].size() > 0)
      {
         const int32_t s32_LastIndex = this->mc_VecDataValues[ou32_WidgetDataPoolElementIndex].size() - 1;
         float64_t f64_Temp;

         // Get the newest value
         C_SdNdeDpContentUtil::h_GetValuesAsFloat64(
            this->mc_VecDataValues[ou32_WidgetDataPoolElementIndex][s32_LastIndex],
            orc_Values);

         // Scale all values
         if (oq_UseScaling == true)
         {
            for (uint32_t u32_Counter = 0U; u32_Counter < orc_Values.size(); ++u32_Counter)
            {
               f64_Temp = orc_Values[u32_Counter];
               orc_Values[u32_Counter] = C_OscUtils::h_GetValueScaled(
                  f64_Temp,
                  this->mc_UsedConfig[ou32_WidgetDataPoolElementIndex].c_Scaling.f64_Factor,
                  this->mc_UsedConfig[ou32_WidgetDataPoolElementIndex].c_Scaling.f64_Offset);
            }
         }

         this->m_SaveTimeStamp(ou32_WidgetDataPoolElementIndex, s32_LastIndex);

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
   \param[out]  orc_ScaledFormattedLastValue       Last element as scaled and formatted value
   \param[out]  orc_ScaledValues                   List with all scaled float values
   \param[out]  orc_Timestamps                     List with all timestamps

   \return
   C_NO_ERR    Value read
   C_RANGE     Index invalid
   C_NOACT     No value received
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_PuiSvDbDataElementHandler::m_GetAllValues(const uint32_t ou32_WidgetDataPoolElementIndex,
                                                    QString & orc_ScaledFormattedLastValue,
                                                    QVector<float64_t> & orc_ScaledValues,
                                                    QVector<uint32_t> & orc_Timestamps)
{
   int32_t s32_Return = C_RANGE;
   C_PuiSvDbNodeDataPoolListElementId c_Id;

   if ((static_cast<int32_t>(ou32_WidgetDataPoolElementIndex) < this->mc_VecDataValues.size()) &&
       (ou32_WidgetDataPoolElementIndex < this->mc_UsedConfig.size()) &&
       (this->GetDataPoolElementIndex(ou32_WidgetDataPoolElementIndex, c_Id) == C_NO_ERR))
   {
      this->mc_CriticalSection.Acquire();
      if (this->mc_VecDataValues[ou32_WidgetDataPoolElementIndex].size() > 0)
      {
         uint32_t u32_Counter = 0U;
         QList<C_PuiSvDbDataElementContent>::const_iterator c_ItItem;
         const uint32_t u32_ArrayIndex = c_Id.GetArrayElementIndexOrZero();
         const int32_t s32_LastIndex = this->mc_VecDataValues[ou32_WidgetDataPoolElementIndex].size() - 1;

         // Copy all values and scale the values
         orc_ScaledValues.resize(this->mc_VecDataValues[ou32_WidgetDataPoolElementIndex].size());
         orc_Timestamps.resize(this->mc_VecDataValues[ou32_WidgetDataPoolElementIndex].size());
         for (c_ItItem = this->mc_VecDataValues[ou32_WidgetDataPoolElementIndex].begin();
              c_ItItem != this->mc_VecDataValues[ou32_WidgetDataPoolElementIndex].end();
              ++c_ItItem)
         {
            float64_t f64_Temp;
            C_SdNdeDpContentUtil::h_GetValueAsFloat64(*c_ItItem, f64_Temp, u32_ArrayIndex);
            orc_ScaledValues[u32_Counter] = C_OscUtils::h_GetValueScaled(
               f64_Temp,
               this->mc_UsedConfig[ou32_WidgetDataPoolElementIndex].c_Scaling.f64_Factor,
               this->mc_UsedConfig[ou32_WidgetDataPoolElementIndex].c_Scaling.f64_Offset);

            // Copy the timestamp
            orc_Timestamps[u32_Counter] = (*c_ItItem).GetTimeStamp();
            ++u32_Counter;
         }

         // The last value will be returned formatted too
         orc_ScaledFormattedLastValue =
            this->mc_UsedConfig[ou32_WidgetDataPoolElementIndex].GetSingleValueContentFormatted(
               this->mc_VecDataValues[ou32_WidgetDataPoolElementIndex][s32_LastIndex], u32_ArrayIndex, NULL);

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
int32_t C_PuiSvDbDataElementHandler::m_GetLastNvmValue(const uint32_t ou32_WidgetDataPoolElementIndex,
                                                       C_OscNodeDataPoolContent & orc_Value)
{
   int32_t s32_Return = C_RANGE;

   if ((static_cast<int32_t>(ou32_WidgetDataPoolElementIndex) < this->mc_VecNvmValue.size()) &&
       (static_cast<int32_t>(ou32_WidgetDataPoolElementIndex) < this->mc_VecNvmValueReceived.size()) &&
       (ou32_WidgetDataPoolElementIndex < this->mc_UsedConfig.size()))
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
int32_t C_PuiSvDbDataElementHandler::m_GetTimoutPercentage100(const uint32_t ou32_DataElementIndex,
                                                              uint8_t & oru8_TimoutPercentage100) const
{
   int32_t s32_Retval;

   if ((ou32_DataElementIndex < this->mc_DataPoolElementValid.size()) &&
       (ou32_DataElementIndex < this->mc_LastDataPoolElementTimeStampsMs.size()) &&
       (ou32_DataElementIndex < this->mc_DataPoolElementTimeoutsMs.size()) &&
       (ou32_DataElementIndex < this->mc_LastDataPoolElementTimeStampsValid.size()))
   {
      if ((this->mc_LastDataPoolElementTimeStampsValid[ou32_DataElementIndex] == true) &&
          (this->mc_DataPoolElementValid[ou32_DataElementIndex] == true))
      {
         const uint32_t u32_TimeStampMs = this->mc_LastDataPoolElementTimeStampsMs[ou32_DataElementIndex];
         const uint32_t u32_TimeoutMs = this->mc_DataPoolElementTimeoutsMs[ou32_DataElementIndex];
         const uint32_t u32_CurrentTimeMs = TglGetTickCount();
         const uint32_t u32_Percentage =
            (std::min((u32_CurrentTimeMs - u32_TimeStampMs), u32_TimeoutMs) * 100U) / u32_TimeoutMs;
         //Attempt to reduce the jumpy nature of gray out engine -> only use multiples of 10
         oru8_TimoutPercentage100 = static_cast<uint8_t>(((u32_Percentage + 5UL) / 10UL) * 10UL);
         oru8_TimoutPercentage100 = static_cast<uint8_t>(std::min(static_cast<int32_t>(oru8_TimoutPercentage100), 100));

         if (oru8_TimoutPercentage100 <= stw::opensyde_gui::mu32_DASHBOARD_TIMEOUT_TOLERANCE)
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
int32_t C_PuiSvDbDataElementHandler::m_ScaleMinMax(const uint32_t ou32_WidgetDataPoolElementIndex,
                                                   float64_t & orf64_Min, float64_t & orf64_Max) const
{
   int32_t s32_Return = C_RANGE;

   if (ou32_WidgetDataPoolElementIndex < this->mc_UsedConfig.size())
   {
      orf64_Min = C_OscUtils::h_GetValueScaled(
         orf64_Min,
         this->mc_UsedConfig[ou32_WidgetDataPoolElementIndex].c_Scaling.f64_Factor,
         this->mc_UsedConfig[ou32_WidgetDataPoolElementIndex].c_Scaling.f64_Offset);

      orf64_Max = C_OscUtils::h_GetValueScaled(
         orf64_Max,
         this->mc_UsedConfig[ou32_WidgetDataPoolElementIndex].c_Scaling.f64_Factor,
         this->mc_UsedConfig[ou32_WidgetDataPoolElementIndex].c_Scaling.f64_Offset);

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
   QMap<C_PuiSvDbNodeDataPoolListElementId, uint32_t>::const_iterator c_ItItem;

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
                 uint32_t>::const_iterator c_ItItem = this->mc_MappingDpElementToDataSerie.constFind(orc_Id);

      tgl_assert(c_ItItem != this->mc_MappingDpElementToDataSerie.cend());
      if (c_ItItem != this->mc_MappingDpElementToDataSerie.cend())
      {
         const C_PuiSvDbNodeDataPoolListElementId & rc_ElementId = c_ItItem.key();
         auto && rc_Value = this->mc_DataPoolElementValid[c_ItItem.value()];
         // Update the valid flag
         rc_Value = rc_ElementId.GetIsValid();

         if (rc_ElementId.GetIsValid() == true)
         {
            if (this->mq_ReadItem == true)
            {
               if (c_ItItem.value() < this->mc_DataPoolElementTimeoutsMs.size())
               {
                  //Ten steps with 0.5 seconds each
                  const uint32_t u32_TIMEOUT_MS = 5000U;
                  this->mc_DataPoolElementTimeoutsMs[c_ItItem.value()] = u32_TIMEOUT_MS;
               }
            }

            // Update the scaling configuration and initialize and check the formatter configuration
            if (c_ItItem.value() < this->mc_UsedConfig.size())
            {
               QMap<C_PuiSvDbNodeDataPoolListElementId, C_DpElementConfig>::const_iterator c_ItItemConfig;
               c_ItItemConfig = this->mc_MappingDpElementToConfig.find(c_ItItem.key());

               if (c_ItItemConfig != this->mc_MappingDpElementToConfig.end())
               {
                  C_PuiSvDbDataElementScaling c_Scaling = c_ItItemConfig.value().c_Scaling;
                  C_PuiSvDbDataElementDisplayFormatterConfig c_Formatter = c_ItItemConfig.value().c_FormatterConfig;

                  const C_OscNodeDataPoolListElement * pc_OscContent;

                  pc_OscContent = C_PuiSdHandler::h_GetInstance()->GetOscDataPoolListElement(
                     rc_ElementId.u32_NodeIndex,
                     rc_ElementId.u32_DataPoolIndex,
                     rc_ElementId.u32_ListIndex,
                     rc_ElementId.u32_ElementIndex);

                  tgl_assert(pc_OscContent != NULL);
                  if (pc_OscContent != NULL)
                  {
                     const C_PuiSdNodeDataPoolListElement * const pc_UiElement =
                        C_PuiSdHandler::h_GetInstance()->GetUiDataPoolListElement(rc_ElementId.u32_NodeIndex,
                                                                                  rc_ElementId.u32_DataPoolIndex,
                                                                                  rc_ElementId.u32_ListIndex,
                                                                                  rc_ElementId.u32_ElementIndex);

                     // Scaling
                     if (c_Scaling.q_UseDefault == true)
                     {
                        // Use the default scaling configuration of the datapool element in the system definition

                        // Set the configured elements of the system definition
                        c_Scaling.f64_Factor = pc_OscContent->f64_Factor;
                        c_Scaling.f64_Offset = pc_OscContent->f64_Offset;
                     }

                     this->mc_UsedConfig[c_ItItem.value()].c_Scaling = c_Scaling;

                     // Formatter
                     tgl_assert(pc_UiElement != NULL);
                     if (pc_UiElement != NULL)
                     {
                        // Set type and check the formatter if active
                        c_Formatter.SetType(
                           C_PuiSvDbDataElementDisplayFormatter::h_GetTypeCategory(
                              pc_OscContent->c_MinValue,
                              c_Scaling,
                              pc_UiElement->q_InterpretAsString));
                     }

                     this->mc_UsedConfig[c_ItItem.value()].c_FormatterConfig = c_Formatter;
                  }
               }
               else
               {
                  //Should not happen
                  tgl_assert(false);
               }
            }

            // Update the minimum type configuration
            if ((c_ItItem.value() < this->mc_MinimumType.size()) &&
                (c_ItItem.value() < this->mc_UsedConfig.size()))
            {
               const C_OscNodeDataPoolListElement * const pc_OscContent =
                  C_PuiSdHandler::h_GetInstance()->GetOscDataPoolListElement(rc_ElementId.u32_NodeIndex,
                                                                             rc_ElementId.u32_DataPoolIndex,
                                                                             rc_ElementId.u32_ListIndex,
                                                                             rc_ElementId.u32_ElementIndex);

               tgl_assert(pc_OscContent != NULL);
               if (pc_OscContent != NULL)
               {
                  std::vector<stw::opensyde_core::C_OscNodeDataPoolContent::E_Type> c_Types;
                  const C_PuiSvDbDataElementScaling & rc_Scaling = this->mc_UsedConfig[c_ItItem.value()].c_Scaling;
                  if (C_SdNdeDpContentUtil::h_GetMinimalTypeAfterScaling(pc_OscContent->c_MinValue,
                                                                         pc_OscContent->c_MaxValue,
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
   const QMap<C_PuiSvDbNodeDataPoolListElementId, uint32_t> & rc_Elements = this->m_GetMappingDpElementToDataSerie();

   for (QMap<C_PuiSvDbNodeDataPoolListElementId, uint32_t>::const_iterator c_ItElement = rc_Elements.begin();
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
      const QMap<C_PuiSvDbNodeDataPoolListElementId, uint32_t> & rc_Elements = this->m_GetMappingDpElementToDataSerie();

      for (QMap<C_PuiSvDbNodeDataPoolListElementId, uint32_t>::const_iterator c_ItElement = rc_Elements.begin();
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

   const QMap<C_PuiSvDbNodeDataPoolListElementId, uint32_t> & rc_Elements = this->m_GetMappingDpElementToDataSerie();

   for (QMap<C_PuiSvDbNodeDataPoolListElementId, uint32_t>::const_iterator c_ItElement = rc_Elements.begin();
        c_ItElement != rc_Elements.end(); ++c_ItElement)
   {
      const C_PuiSvDbNodeDataPoolListElementId c_ElementId = c_ItElement.key();
      if (c_ElementId.GetIsValid() == true)
      {
         std::vector<uint8_t> c_ActiveNodes;
         const int32_t s32_Retval =
            C_PuiSvHandler::h_GetInstance()->GetNodeActiveFlagsWithSquadAdaptions(this->mu32_ViewIndex,
                                                                                  c_ActiveNodes);

         //Is corresponding view active
         if ((s32_Retval == C_NO_ERR) &&
             (c_ElementId.u32_NodeIndex < c_ActiveNodes.size()) &&
             (c_ActiveNodes[c_ElementId.u32_NodeIndex] == 1U))
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

   const QMap<C_PuiSvDbNodeDataPoolListElementId, uint32_t> & rc_Elements = this->m_GetMappingDpElementToDataSerie();

   for (QMap<C_PuiSvDbNodeDataPoolListElementId, uint32_t>::const_iterator c_ItElement = rc_Elements.begin();
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
      const QMap<C_PuiSvDbNodeDataPoolListElementId, uint32_t> & rc_Elements = this->m_GetMappingDpElementToDataSerie();

      for (QMap<C_PuiSvDbNodeDataPoolListElementId, uint32_t>::const_iterator c_ItElement = rc_Elements.begin();
           c_ItElement != rc_Elements.end(); ++c_ItElement)
      {
         const C_PuiSvDbNodeDataPoolListElementId c_ElementId = c_ItElement.key();
         if ((c_ElementId.GetIsValid() == true) &&
             (c_ElementId.GetType() == C_PuiSvDbNodeDataPoolListElementId::eBUS_SIGNAL))
         {
            C_OscCanMessageIdentificationIndices c_MessageId;
            uint32_t u32_SignalIndex;
            if ((pc_View->GetOscPcData().GetConnected() == true) &&
                (C_PuiSdUtil::h_ConvertIndex(c_ElementId, c_MessageId, u32_SignalIndex) == C_NO_ERR))
            {
               const C_OscNode * const pc_Node =
                  C_PuiSdHandler::h_GetInstance()->GetOscNodeConst(c_MessageId.u32_NodeIndex);
               if ((pc_Node != NULL) && (c_MessageId.u32_InterfaceIndex < pc_Node->c_Properties.c_ComInterfaces.size()))
               {
                  const C_OscNodeComInterfaceSettings & rc_Interface =
                     pc_Node->c_Properties.c_ComInterfaces[c_MessageId.u32_InterfaceIndex];
                  if (rc_Interface.GetBusConnected() == true)
                  {
                     if (rc_Interface.u32_BusIndex == pc_View->GetOscPcData().GetBusIndex())
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
bool C_PuiSvDbDataElementHandler::m_CheckNodeActive(const uint32_t ou32_NodeIndex) const
{
   bool q_Retval = false;

   std::vector<uint8_t> c_ActiveNodes;
   const int32_t s32_Retval = C_PuiSvHandler::h_GetInstance()->GetNodeActiveFlagsWithSquadAdaptions(
      this->mu32_ViewIndex,
      c_ActiveNodes);

   if (s32_Retval == C_NO_ERR)
   {
      if (ou32_NodeIndex < c_ActiveNodes.size())
      {
         if (static_cast<bool>(c_ActiveNodes[ou32_NodeIndex]) == true)
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
bool C_PuiSvDbDataElementHandler::m_CheckNodeHasAnyRequiredValidDashboardRouting(const uint32_t ou32_NodeIndex) const
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
         const QMap<C_OscNodeDataPoolListElementId,
                    C_PuiSvReadDataConfiguration> & rc_RailAssignments = pc_View->GetReadRailAssignments();
         const QMap<C_OscNodeDataPoolListElementId,
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
bool C_PuiSvDbDataElementHandler::m_CheckElementAlreadyRead(const uint32_t ou32_ItemIndex,
                                                            const C_PuiSvDbNodeDataPoolListElementId & orc_Id) const
{
   bool q_AlreadyRead = false;

   for (uint32_t u32_ItItem =
           0UL; (u32_ItItem < this->GetWidgetDataPoolElementCount()) && (u32_ItItem < ou32_ItemIndex);
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
   \param[in]   ou8_Nrc             Negative response code, if any
   \param[out]  orc_Description     Error description
   \param[out]  orc_Details         Details for error
   \param[out]  ors32_TextHeight    Necessary height of text
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PuiSvDbDataElementHandler::m_GetErrorDescriptionForManualOperation(const int32_t os32_Result,
                                                                          const uint8_t ou8_Nrc,
                                                                          QString & orc_Description,
                                                                          QString & orc_Details,
                                                                          int32_t & ors32_TextHeight) const
{
   switch (os32_Result)
   {
   case C_CONFIG:
   case C_RANGE:
   case C_RD_WR:
   case C_OVERFLOW:
      orc_Description = C_GtGetText::h_GetText("Operation failed with an internal error.");
      //Update log file
      C_OscLoggingHandler::h_Flush();
      orc_Details = static_cast<QString>("%1<a href=\"file:%2\"><span style=\"color: %3;\">%4</span></a>.").
                    arg(C_GtGetText::h_GetText("For details see ")).
                    arg(C_OscLoggingHandler::h_GetCompleteLogFileLocation().c_str()).
                    arg(stw::opensyde_gui::mc_STYLESHEET_GUIDE_COLOR_LINK).
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
         ors32_TextHeight = 250;

         switch (ou8_Nrc)
         {
         case 0x13:
            orc_Details = C_GtGetText::h_GetText("Incorrect length of request.");

            break;
         case 0x31:
            orc_Details = C_GtGetText::h_GetText("Datapool element specified by data identifier is not available.");
            break;
         case 0x22:
            orc_Details = C_GtGetText::h_GetText("Access to Datapool element blocked by application.");
            break;
         case 0x33:
            orc_Details = C_GtGetText::h_GetText("Required security level was not unlocked.");
            break;
         case 0x14:
            orc_Details = C_GtGetText::h_GetText(
               "The total length of the response message exceeds the available buffer size.");
            break;
         case 0x7F:
            orc_Details = C_GtGetText::h_GetText(
               "The requested service is not available in the currently active session.");
            break;
         default:
            orc_Details =
               static_cast<QString>(C_GtGetText::h_GetText("Unknown NRC: 0x%1")).arg(QString::number(ou8_Nrc,
                                                                                                     16));
            break;
         }
      }
      else
      {
         switch (ou8_Nrc)
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
               static_cast<QString>(C_GtGetText::h_GetText("Unknown NRC: 0x%1")).arg(QString::number(ou8_Nrc,
                                                                                                     16));
            ors32_TextHeight = 250;
            break;
         }
      }
      orc_Description =
         static_cast<QString>(C_GtGetText::h_GetText("Operation failed with error response (%1).")).arg(orc_Details);
      orc_Details = "";
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
const QMap<C_PuiSvDbNodeDataPoolListElementId,
           uint32_t> & C_PuiSvDbDataElementHandler::m_GetMappingDpElementToDataSerie(void) const
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
   QMap<C_PuiSvDbNodeDataPoolListElementId, C_DpElementConfig>::const_iterator c_ItItem;

   orc_RegisteredElements.clear();
   orc_RegisteredElements.reserve(this->mc_MappingDpElementToConfig.size());
   for (c_ItItem = this->mc_MappingDpElementToConfig.begin(); c_ItItem != this->mc_MappingDpElementToConfig.end();
        ++c_ItItem)
   {
      orc_RegisteredElements.push_back(c_ItItem.key());
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get the value of Dp content as formatted and scaled string

   \param[in]     orc_Value                       Dp content to convert, format and scale
   \param[in]     ou32_Index                      Array index in content (0 in case of no array)
   \param[out]    opf64_UnscaledValueAsFloat      Optional float with unscaled value
   \param[out]    opf64_ScaledValueAsFloat        Optional float with scaled value

   \return
   Formatted and scaled string
*/
//----------------------------------------------------------------------------------------------------------------------
QString C_PuiSvDbDataElementHandler::C_DpElementConfig::GetSingleValueContentFormatted(
   const C_PuiSvDbDataElementContent & orc_Value, const uint32_t ou32_Index,
   float64_t * const opf64_UnscaledValueAsFloat, float64_t * const opf64_ScaledValueAsFloat) const
{
   return this->c_FormatterConfig.GetSingleValueContentFormatted(orc_Value, ou32_Index, this->c_Scaling,
                                                                 opf64_UnscaledValueAsFloat,
                                                                 opf64_ScaledValueAsFloat);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get the values of Dp content as formatted and scaled strings

   If it is an array, the size of the vector is > 1
   If it is no array, the size of the vector is 1
   If an error occurred, the size of the vector is 0

   Special case: If the element is configured as string (sint8 array): The return value has one string and the
   orc_UnscaledValueAsFloat array has one value with 0.0

   \param[in]     orc_Value                       Dp content to convert, format and scale
   \param[out]    orc_UnscaledValueAsFloat        Optional floats with unscaled values

   \return
   Formatted and scaled strings
*/
//----------------------------------------------------------------------------------------------------------------------
std::vector<QString> C_PuiSvDbDataElementHandler::C_DpElementConfig::GetValuesContentFormatted(
   const C_PuiSvDbDataElementContent & orc_Value, std::vector<float64_t> & orc_UnscaledValueAsFloat)
const
{
   return this->c_FormatterConfig.GetValuesContentFormatted(orc_Value, this->c_Scaling, orc_UnscaledValueAsFloat);
}

//----------------------------------------------------------------------------------------------------------------------
void C_PuiSvDbDataElementHandler::m_SetWidgetDataPoolElementCount(const uint32_t ou32_Count)
{
   this->mu32_WidgetDataPoolElementCount = ou32_Count;
   this->mc_VecDataValues.resize(this->mu32_WidgetDataPoolElementCount);
   this->mc_VecNvmValue.resize(this->mu32_WidgetDataPoolElementCount);
   this->mc_VecNvmValueReceived.resize(this->mu32_WidgetDataPoolElementCount);
   this->mc_LastDataPoolElementTimeStampsMs.resize(this->mu32_WidgetDataPoolElementCount);
   this->mc_LastDataPoolElementTimeStampsValid.resize(this->mu32_WidgetDataPoolElementCount);
   this->mc_DataPoolElementTimeoutsMs.resize(this->mu32_WidgetDataPoolElementCount);
   this->mc_DataPoolElementValid.resize(this->mu32_WidgetDataPoolElementCount);
   this->mc_UsedConfig.resize(this->mu32_WidgetDataPoolElementCount);
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
QString C_PuiSvDbDataElementHandler::mh_GetStringForScaledValue(const float64_t of64_Value,
                                                                const C_OscNodeDataPoolContent::E_Type oe_Type)
{
   QString c_Retval;

   switch (oe_Type)
   {
   case C_OscNodeDataPoolContent::eUINT8:
      c_Retval = QString::number(static_cast<uint8_t>(of64_Value));
      break;
   case C_OscNodeDataPoolContent::eUINT16:
      c_Retval = QString::number(static_cast<uint16_t>(of64_Value));
      break;
   case C_OscNodeDataPoolContent::eUINT32:
      c_Retval = QString::number(static_cast<uint32_t>(of64_Value));
      break;
   case C_OscNodeDataPoolContent::eUINT64:
      c_Retval = QString::number(static_cast<uint64_t>(of64_Value));
      break;
   case C_OscNodeDataPoolContent::eSINT8:
      c_Retval = QString::number(static_cast<int8_t>(of64_Value));
      break;
   case C_OscNodeDataPoolContent::eSINT16:
      c_Retval = QString::number(static_cast<int16_t>(of64_Value));
      break;
   case C_OscNodeDataPoolContent::eSINT32:
      c_Retval = QString::number(static_cast<int32_t>(of64_Value));
      break;
   case C_OscNodeDataPoolContent::eSINT64:
      c_Retval = QString::number(static_cast<int64_t>(of64_Value));
      break;
   case C_OscNodeDataPoolContent::eFLOAT32:
   case C_OscNodeDataPoolContent::eFLOAT64:
      c_Retval = QString::number(of64_Value);
      break;
   default:
      tgl_assert(false);
      break;
   }
   return c_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Saves the last timestamp for specific element and sets it as valid

   \param[in]   ou32_WidgetDataPoolElementIndex    Index of shown datapool element in widget
   \param[in]   os32_ValueIndex                     Index of value
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PuiSvDbDataElementHandler::m_SaveTimeStamp(const uint32_t ou32_WidgetDataPoolElementIndex,
                                                  const int32_t os32_ValueIndex)
{
   //Save last timestamp
   if (ou32_WidgetDataPoolElementIndex < this->mc_LastDataPoolElementTimeStampsMs.size())
   {
      this->mc_LastDataPoolElementTimeStampsMs[ou32_WidgetDataPoolElementIndex] =
         this->mc_VecDataValues[ou32_WidgetDataPoolElementIndex][os32_ValueIndex].GetTimeStamp();
   }
   //Set timestamp valid
   if (ou32_WidgetDataPoolElementIndex < this->mc_LastDataPoolElementTimeStampsValid.size())
   {
      auto && rc_Value = this->mc_LastDataPoolElementTimeStampsValid[ou32_WidgetDataPoolElementIndex];
      rc_Value = true;
   }
}
