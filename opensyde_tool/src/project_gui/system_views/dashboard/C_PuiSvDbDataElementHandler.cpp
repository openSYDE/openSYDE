//-----------------------------------------------------------------------------
/*!
   \internal
   \file
   \brief       Base functions and data storing for datapool elements and its content (implementation)

   detailed description

   \implementation
   project     openSYDE
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     24.08.2017  STW/B.Bayer
   \endimplementation
*/
//-----------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------- */
#include "precomp_headers.h"

#include "stwerrors.h"
#include "constants.h"
#include "C_PuiSvDbDataElementHandler.h"
#include "C_PuiSvHandler.h"
#include "C_PuiSvData.h"
#include "C_OSCUtils.h"
#include "TGLUtils.h"
#include "TGLTime.h"
#include "C_PuiSdHandler.h"
#include "C_SdNdeDataPoolContentUtil.h"

/* -- Used Namespaces ------------------------------------------------------ */
using namespace stw_tgl;
using namespace stw_types;
using namespace stw_errors;
using namespace stw_opensyde_core;
using namespace stw_opensyde_gui_logic;

/* -- Module Global Constants ---------------------------------------------- */

/* -- Types ---------------------------------------------------------------- */

/* -- Global Variables ----------------------------------------------------- */

/* -- Module Global Variables ---------------------------------------------- */

/* -- Module Global Function Prototypes ------------------------------------ */

/* -- Implementation ------------------------------------------------------- */

//-----------------------------------------------------------------------------
/*!
   \brief   Default constructor

   long description of function within several lines

   \param[in] oru32_ViewIndex            Index of system view
   \param[in] oru32_DashboardIndex       Index of dashboard in system view
   \param[in] ors32_DataIndex            Index of connected data item
   \param[in] ore_Type                   Type of data
   \param[in] ou32_MaximumDataElements   Maximum number of shown data elements of the widget
   \param[in] oq_ReadItem                Flag if item is a read only item and has an rail assignment

   \created     24.08.2017  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
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

//-----------------------------------------------------------------------------
/*!
   \brief   Adds the datapool element to the shown elements of the widget

   This function is thread safe.

   \param[in]     orc_WidgetDataPoolElementId         Datapool element identificator
   \param[in]     orc_DataPoolElementScaling          Datapool element scaling configuration

   \return
   C_NO_ERR    Datapool element added
   C_NOACT     Datapool element was added already. Nothing done.
   C_RANGE     No further datapool elements can be added. Nothing done.

   \created     24.08.2017  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
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
         this->m_UpdateDataPoolElementTimeoutAndValidFlag();
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

//-----------------------------------------------------------------------------
/*!
   \brief   Register cyclic transmission error for specific data element

   Info: no default handling

   \param[in] orc_WidgetDataPoolElementId Affected data element
   \param[in] ou8_ErrorCode               Registered error code

   \created     17.07.2018  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_PuiSvDbDataElementHandler::RegisterDataPoolElementCyclicError(
   const C_PuiSvDbNodeDataPoolListElementId & orc_WidgetDataPoolElementId, const uint8 ou8_ErrorCode)
{
   Q_UNUSED(orc_WidgetDataPoolElementId)
   Q_UNUSED(ou8_ErrorCode)
}

//-----------------------------------------------------------------------------
/*!
   \brief   Removes the datapool element from the shown elements of the widget

   This function is thread safe.

   \param[in]     orc_WidgetDataPoolElementId         Datapool element identificator

   \created     24.08.2017  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
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

//-----------------------------------------------------------------------------
/*!
   \brief   Clear the datapool elements from the shown elements of the widget

   This function is thread safe.

   \created     24.08.2017  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
void C_PuiSvDbDataElementHandler::ClearDataPoolElements(void)
{
   this->mc_CriticalSection.Acquire();
   this->mc_MappingDpElementToDataSerie.clear();
   this->m_SetWidgetDataPoolElementCount(this->mc_MappingDpElementToDataSerie.size());
   this->mc_CriticalSection.Release();

   this->m_DataPoolElementsChanged();
}

//-----------------------------------------------------------------------------
/*!
   \brief   Handle changes of transmission mode for any data element

   \created     11.10.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_PuiSvDbDataElementHandler::UpdateTransmissionConfiguration(void)
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

//-----------------------------------------------------------------------------
/*!
   \brief   Returns the count of shown datapool elements

   \return
   Count of shown datapool elements

   \created     23.08.2017  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
uint32 C_PuiSvDbDataElementHandler::GetWidgetDataPoolElementCount(void) const
{
   return this->mu32_WidgetDataPoolElementCount;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Gets all identification informations about the registered datapool element

   \param[in]     ou32_WidgetDataPoolElementIndex       Index of shown datapool element in widget
   \param[out]    orc_DataPoolId                        Datapool element identificator

   \return
   C_NO_ERR    Datapool element index valid and index id was filled
   C_RANGE     Datapool element index not valid. Id was not filled.

   \created     23.08.2017  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
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

//-----------------------------------------------------------------------------
/*!
   \brief   Gets index of datapool element in widget with identification informations

   \param[in]      orc_DataPoolId                        Datapool element identificator
   \param[out]     oru32_WidgetDataPoolElementIndex      Index of shown datapool element in widget

   \return
   C_NO_ERR    Datapool element index valid and index id was filled
   C_RANGE     Datapool element index not valid. Id was not filled.

   \created     10.10.2017  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
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

//-----------------------------------------------------------------------------
/*!
   \brief   Gets all scaling informations about the registered datapool element

   \param[in]     ou32_WidgetDataPoolElementIndex       Index of shown datapool element in widget
   \param[out]    orc_DataPoolScaling                   Datapool element scaling configuration

   \return
   C_NO_ERR    Datapool element index valid and index id was filled
   C_RANGE     Datapool element index not valid. Id was not filled.

   \created     08.09.2017  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
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

//-----------------------------------------------------------------------------
/*!
   \brief   short description of function

   \param[in]     orc_WidgetDataPoolElementId         Datapool element identificator

   \return
   true     Data element is already registered
   false    Data element is not registered

   \created     08.09.2017  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
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

//-----------------------------------------------------------------------------
/*!
   \brief   Inserts a new received value to the widget queue

   This function is thread safe.

   \param[in]     orc_WidgetDataPoolElementId         Datapool element identificator
   \param[in]     orc_NewValue                        New received value

   \created     23.08.2017  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
void C_PuiSvDbDataElementHandler::InsertNewValueIntoQueue(
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
      if (static_cast<sintn>(u32_DataSerieIndex) < this->mc_VecDataValues.size())
      {
         this->mc_CriticalSection.Acquire();
         this->mc_VecDataValues[u32_DataSerieIndex].push_back(orc_NewValue);
         this->mc_CriticalSection.Release();
      }
   }
}

//-----------------------------------------------------------------------------
/*!
   \brief   Adds a new received NVM value to the widget queue

   This function is thread safe.

   \param[in]     orc_WidgetDataPoolElementId         Datapool element identificator
   \param[in]     orc_NewValue                        New received value

   \created     23.08.2017  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
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

//-----------------------------------------------------------------------------
/*!
   \brief   Get unscaled value as scaled string

   Warning: expected to be in range of min & max

   \param[in] f64_Value         Unscaled value
   \param[in] ou32_IndexElement Optional index of element (if more than one)

   \return
   Value as scaled string

   \created     25.05.2018  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
QString C_PuiSvDbDataElementHandler::GetUnscaledValueAsScaledString(const float64 f64_Value,
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
         C_SdNdeDataPoolContentUtil::h_SetValueInContent(f64_Value, c_Tmp, 0UL);
         C_SdNdeDataPoolContentUtil::h_GetValueAsScaledString(c_Tmp, c_Scaling.f64_Factor, c_Scaling.f64_Offset,
                                                              c_Retval, 0UL);
      }
   }
   if (c_Retval.compare("") == 0)
   {
      c_Retval = QString::number(f64_Value);
   }
   return c_Retval;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Get unscaled value (does not need to be in range of min & max) as scaled string

   \param[in]     of64_Value        Unscaled value (does not need to be in range of min & max)
   \param[in]     ou32_IndexElement Optional index of element (if more than one)
   \param[in,out] opf64_Progress    Optional parameter to store the progress of the new applied value in range of min and max [0,1]

   \return
   Value as scaled string

   \created     07.11.2018  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
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
         C_SdNdeDataPoolContentUtil::E_ValueChangedTo e_Change;
         C_OSCNodeDataPoolContent c_Tmp = pc_Element->c_MinValue;
         C_SdNdeDataPoolContentUtil::h_SetValueInContent(of64_Value, c_Tmp, 0UL);
         C_SdNdeDataPoolContentUtil::h_SetValueInMinMaxRange(pc_Element->c_MinValue, pc_Element->c_MaxValue, c_Tmp,
                                                             e_Change);
         C_SdNdeDataPoolContentUtil::h_GetValueAsScaledString(c_Tmp, c_Scaling.f64_Factor, c_Scaling.f64_Offset,
                                                              c_Retval, 0UL);
         if (opf64_Progress != NULL)
         {
            float64 f64_Min;
            float64 f64_Max;
            float64 f64_Value;
            //Get restricted value "progress"
            C_SdNdeDataPoolContentUtil::h_GetValueAsFloat64(pc_Element->c_MinValue, f64_Min);
            C_SdNdeDataPoolContentUtil::h_GetValueAsFloat64(pc_Element->c_MaxValue, f64_Max);
            C_SdNdeDataPoolContentUtil::h_GetValueAsFloat64(c_Tmp, f64_Value);
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

//-----------------------------------------------------------------------------
/*!
   \brief   Returns the newest registered value of a specific datapool element. It can be an array too.

   The vector will be cleared first.

   If it is an array, the size of the vector is > 1
   If it is no array, the size of the vector is 1
   If an error occurred, the size of the vector is 0

   This function is thread safe.

   \param[in]     ou32_WidgetDataPoolElementIndex       Index of shown datapool element in widget
   \param[out]    orc_Values                            Vector with result value(s)
   \param[out]    opc_StringifiedValues                 Optional vector with result strings

   \return
   C_NO_ERR    Value read
   C_RANGE     Index invalid
   C_NOACT     No value received

   \created     24.08.2017  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
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
         C_SdNdeDataPoolContentUtil::h_GetValuesAsFloat64(
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
            std::vector<bool>::reference c_Value =
               this->mc_LastDataPoolElementTimeStampsValid[ou32_WidgetDataPoolElementIndex];
            c_Value = true;
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

//-----------------------------------------------------------------------------
/*!
   \brief   Returns the newest registered value of a specific datapool element as float

   This function is thread safe.

   \param[in]     ou32_WidgetDataPoolElementIndex       Index of shown datapool element in widget
   \param[out]    of64_Value                            Result value

   \return
   C_NO_ERR    Value read
   C_RANGE     Index invalid
   C_NOACT     No value received

   \created     28.09.2017  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
sint32 C_PuiSvDbDataElementHandler::m_GetLastValue(const uint32 ou32_WidgetDataPoolElementIndex, float64 & orf64_Value,
                                                   const bool oq_UseScaling)
{
   sint32 s32_Return = C_RANGE;

   if ((static_cast<sintn>(ou32_WidgetDataPoolElementIndex) < this->mc_VecDataValues.size()) &&
       ((ou32_WidgetDataPoolElementIndex < this->mc_UsedScaling.size()) || (oq_UseScaling == false)))
   {
      this->mc_CriticalSection.Acquire();
      if (this->mc_VecDataValues[ou32_WidgetDataPoolElementIndex].size() > 0)
      {
         const sintn sn_LastIndex = this->mc_VecDataValues[ou32_WidgetDataPoolElementIndex].size() - 1;
         float64 f64_Temp;

         // Get the newest value as float
         C_SdNdeDataPoolContentUtil::h_GetValueAsFloat64(this->mc_VecDataValues[ou32_WidgetDataPoolElementIndex][
                                                            sn_LastIndex], f64_Temp);
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
            std::vector<bool>::reference c_Value =
               this->mc_LastDataPoolElementTimeStampsValid[ou32_WidgetDataPoolElementIndex];
            c_Value = true;
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

//-----------------------------------------------------------------------------
/*!
   \brief   Returns all registered values of a specific datapool element

   This function is thread safe.

   \param[in]     ou32_WidgetDataPoolElementIndex       Index of shown datapool element in widget
   \param[out]    orc_Value                             List with all values

   \return
   C_NO_ERR    Value read
   C_RANGE     Index invalid
   C_NOACT     No value received

   \created     24.08.2017  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
sint32 C_PuiSvDbDataElementHandler::m_GetAllValues(const uint32 ou32_WidgetDataPoolElementIndex,
                                                   QVector<float64> & orc_Values, QVector<uint32> & orc_Timestamps,
                                                   const bool oq_UseScaling)
{
   sint32 s32_Return = C_RANGE;

   if ((static_cast<sintn>(ou32_WidgetDataPoolElementIndex) < this->mc_VecDataValues.size()) &&
       ((ou32_WidgetDataPoolElementIndex < this->mc_UsedScaling.size()) || (oq_UseScaling == false)))
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
            // Convert the value to a float64
            C_SdNdeDataPoolContentUtil::h_GetValueAsFloat64(*c_ItItem, f64_Value);
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
            std::vector<bool>::reference c_Value =
               this->mc_LastDataPoolElementTimeStampsValid[ou32_WidgetDataPoolElementIndex];
            c_Value = true;
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

//-----------------------------------------------------------------------------
/*!
   \brief   Returns the newest registered NVM value of a specific NVM datapool element. It can be an array too.

   The vector will be cleared first.

   If it is an array, the size of the vector is > 1
   If it is no array, the size of the vector is 1
   If an error ocured, the size of the vector is 0

   This function is thread safe.

   \param[in]     ou32_WidgetDataPoolElementIndex       Index of shown datapool element in widget
   \param[out]    orc_Values                            Vector with result value(s)
   \param[out]    opc_StringifiedValues                 Optional vector with result strings

   \return
   C_NO_ERR    Value read
   C_RANGE     Index invalid

   \created     25.10.2017  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
sint32 C_PuiSvDbDataElementHandler::m_GetLastNvmValue(const uint32 ou32_WidgetDataPoolElementIndex,
                                                      std::vector<float64> & orc_Values,
                                                      std::vector<QString> * const opc_StringifiedValues)
{
   sint32 s32_Return = C_RANGE;

   orc_Values.clear();

   if ((static_cast<sintn>(ou32_WidgetDataPoolElementIndex) < this->mc_VecNvmValue.size()) &&
       (static_cast<sintn>(ou32_WidgetDataPoolElementIndex) < this->mc_VecNvmValueReceived.size()) &&
       (ou32_WidgetDataPoolElementIndex < this->mc_UsedScaling.size()))
   {
      this->mc_CriticalSection.Acquire();

      if (this->mc_VecNvmValueReceived[ou32_WidgetDataPoolElementIndex] == true)
      {
         uint32 u32_Counter;
         float64 f64_Temp;

         // Get the newest value
         C_SdNdeDataPoolContentUtil::h_GetValuesAsFloat64(
            this->mc_VecNvmValue[ou32_WidgetDataPoolElementIndex],
            orc_Values);

         // Scale all values
         for (u32_Counter = 0U; u32_Counter < orc_Values.size(); ++u32_Counter)
         {
            f64_Temp = orc_Values[u32_Counter];
            orc_Values[u32_Counter] = C_OSCUtils::h_GetValueScaled(
               f64_Temp,
               this->mc_UsedScaling[ou32_WidgetDataPoolElementIndex].f64_Factor,
               this->mc_UsedScaling[ou32_WidgetDataPoolElementIndex].f64_Offset);
         }

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

//-----------------------------------------------------------------------------
/*!
   \brief   Get percentage of timeout progress for specified element

   \param[in]  ou32_DataElementIndex              Index of time stamp of last received data package
   \param[out] oru8_TimoutPercentage100           0-100 Progression of timeout
                                                      -> 0: Current value has been received right now
                                                      -> 100: Last known value has reached the exact specified timeout
   Else  Should not happen

   \return
   C_NO_ERR Valid value
   C_RANGE  Index out of range
   C_NOACT Timeout invalid

   \created     31.08.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
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

//-----------------------------------------------------------------------------
/*!
   \brief   Scales a minimum and maximum value with the active scaling configuration

   \param[in]     ou32_WidgetDataPoolElementIndex       Index of shown datapool element in widget
   \param[in,out] orf64_Min                             Minimum value
   \param[in,out] orf64_Max                             Maximum value

   \return
   C_NO_ERR Valid value
   C_RANGE  Index out of range

   \created     28.09.2017  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
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

//-----------------------------------------------------------------------------
/*!
   \brief   Updates the timeout values and valid flags for all registered datapool elements

   \created     20.09.2017  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
void C_PuiSvDbDataElementHandler::m_UpdateDataPoolElementTimeoutAndValidFlag(void)
{
   const C_PuiSvData * const pc_View = C_PuiSvHandler::h_GetInstance()->GetView(this->mu32_ViewIndex);

   tgl_assert(pc_View != NULL);
   if (pc_View != NULL)
   {
      QMap<C_PuiSvDbNodeDataPoolListElementId, stw_types::uint32>::const_iterator c_ItItem;

      for (c_ItItem = this->mc_MappingDpElementToDataSerie.begin();
           c_ItItem != this->mc_MappingDpElementToDataSerie.end();
           ++c_ItItem)
      {
         const C_PuiSvDbNodeDataPoolListElementId & rc_ElementId = c_ItItem.key();
         std::vector<bool>::reference c_Value = this->mc_DataPoolElementValid[c_ItItem.value()];
         uint16 u16_UpdateRate = 0U;
         // Update the valid flag
         c_Value = rc_ElementId.GetIsValid();

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

                  tgl_assert(s32_Return == C_NO_ERR);
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

               tgl_assert(s32_Return == C_NO_ERR);
               if ((s32_Return == C_NO_ERR) &&
                   (c_ItItem.value() < this->mc_DataPoolElementTimeoutsMs.size()))
               {
                  //Ten steps with 0.5 seconds each
                  const uint32 u32_TimeoutMs = 5000;
                  this->mc_DataPoolElementTimeoutsMs[c_ItItem.value()] = u32_TimeoutMs;
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
                  if (C_SdNdeDataPoolContentUtil::h_GetMinimalTypeAfterScaling(pc_OSCContent->c_MinValue,
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

//-----------------------------------------------------------------------------
/*!
   \brief   Event function if the datapool element configuration was changed

   Base class implementation does nothing. If there are configurable properties for the dashboard element
   the derived class must reimplement this function.

   \created     19.09.2017  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
void C_PuiSvDbDataElementHandler::m_DataPoolElementsChanged(void)
{
   // Nothing to do in the base class implementation
}

//-----------------------------------------------------------------------------
/*!
   \brief   Get configured scaling configuration for each data element

   \return
   Current configured scaling configuration for each data element

   \created     08.02.2018  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
const QMap<C_PuiSvDbNodeDataPoolListElementId, uint32> & C_PuiSvDbDataElementHandler::GetMappingDpElementToDataSerie(
   void) const
{
   return this->mc_MappingDpElementToDataSerie;
}

//-----------------------------------------------------------------------------
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

//-----------------------------------------------------------------------------
const C_PuiSvDbWidgetBase * C_PuiSvDbDataElementHandler::m_GetPuiSvBase(void) const
{
   const C_PuiSvDbWidgetBase * pc_Base = NULL;
   const C_PuiSvData * const pc_View = C_PuiSvHandler::h_GetInstance()->GetView(this->mu32_ViewIndex);

   if (pc_View != NULL)
   {
      const C_PuiSvDashboard * const pc_DashBoard = pc_View->GetDashboard(this->mu32_DashboardIndex);

      if (pc_DashBoard != NULL)
      {
         switch (this->me_Type)
         {
         case eCHART:
            //lint -e{929}  false positive in PC-Lint: allowed by MISRA 5-2-2
            pc_Base = pc_DashBoard->GetChart(static_cast<uint32>(this->ms32_Index));
            break;
         case eLABEL:
            //lint -e{929}  false positive in PC-Lint: allowed by MISRA 5-2-2
            pc_Base = pc_DashBoard->GetLabel(static_cast<uint32>(this->ms32_Index));
            break;
         case ePARAM:
            //lint -e{929}  false positive in PC-Lint: allowed by MISRA 5-2-2
            pc_Base = pc_DashBoard->GetParam(static_cast<uint32>(this->ms32_Index));
            break;
         case ePIE_CHART:
            //lint -e{929}  false positive in PC-Lint: allowed by MISRA 5-2-2
            pc_Base = pc_DashBoard->GetPieChart(static_cast<uint32>(this->ms32_Index));
            break;
         case eSPIN_BOX:
            //lint -e{929}  false positive in PC-Lint: allowed by MISRA 5-2-2
            pc_Base = pc_DashBoard->GetSpinBox(static_cast<uint32>(this->ms32_Index));
            break;
         case eSLIDER:
            //lint -e{929}  false positive in PC-Lint: allowed by MISRA 5-2-2
            pc_Base = pc_DashBoard->GetSlider(static_cast<uint32>(this->ms32_Index));
            break;
         case eTABLE:
            //lint -e{929}  false positive in PC-Lint: allowed by MISRA 5-2-2
            pc_Base = pc_DashBoard->GetTable(static_cast<uint32>(this->ms32_Index));
            break;
         case ePROGRESS_BAR:
            //lint -e{929}  false positive in PC-Lint: allowed by MISRA 5-2-2
            pc_Base = pc_DashBoard->GetProgressBar(static_cast<uint32>(this->ms32_Index));
            break;
         case eTOGGLE:
            //lint -e{929}  false positive in PC-Lint: allowed by MISRA 5-2-2
            pc_Base = pc_DashBoard->GetToggle(static_cast<uint32>(this->ms32_Index));
            break;
         case eLINE_ARROW:   // Is no C_GiSvDaRectBaseGroup
         case eTEXT_ELEMENT: // Is no C_GiSvDaRectBaseGroup
         case eIMAGE:        // Is no C_GiSvDaRectBaseGroup
         case eUNKNOWN:      // Is no C_GiSvDaRectBaseGroup
         default:
            // Shall not happen
            tgl_assert(false);
            break;
         }
      }
   }

   return pc_Base;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Get string for scaled value

   \param[in] of64_Value Scaled value
   \param[in] oe_Type    Target type after scaling

   \return
   String for value

   \created     09.11.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
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
   }
   return c_Retval;
}
