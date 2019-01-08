//-----------------------------------------------------------------------------
/*!
   \internal
   \file
   \brief       Class for system view dashboard parameterization table item (implementation)

   Class for system view dashboard parameterization table item

   \implementation
   project     openSYDE
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     25.10.2017  STW/M.Echtler
   \endimplementation
*/
//-----------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------- */
#include "precomp_headers.h"

#include <limits>
#include <QMouseEvent>
#include <QGraphicsView>
#include "gitypes.h"
#include "stwtypes.h"
#include "TGLUtils.h"
#include "stwerrors.h"
#include "C_OgeWiUtil.h"
#include "C_GiSvDaParam.h"
#include "C_PuiSvHandler.h"

/* -- Used Namespaces ------------------------------------------------------ */
using namespace stw_tgl;
using namespace stw_types;
using namespace stw_errors;
using namespace stw_opensyde_gui;
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

   Set up GUI with all elements.

   \param[in]     oru32_ViewIndex      Index of system view
   \param[in]     oru32_DashboardIndex Index of dashboard in system view
   \param[in]     ors32_DataIndex      Index of data element in dashboard in system view
   \param[in]     oru64_ID             Unique ID
   \param[in,out] opc_Parent           Optional pointer to parent

   \created     25.10.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
C_GiSvDaParam::C_GiSvDaParam(const uint32 & oru32_ViewIndex, const uint32 & oru32_DashboardIndex,
                             const sint32 & ors32_DataIndex, const uint64 & oru64_ID,
                             QGraphicsItem * const opc_Parent) :
   C_GiSvDaRectBaseGroup(oru32_ViewIndex, oru32_DashboardIndex, ors32_DataIndex, C_PuiSvDbDataElement::ePARAM,
                         static_cast<uint32>(std::numeric_limits<sintn>::max()),
                         oru64_ID, 90.0, 50.0, 180.0, 100.0, false, true, opc_Parent),
   mq_Connected(false),
   mq_EditActive(false)
{
   this->mpc_ParamWidget = new C_SyvDaItPaWidgetNew(oru32_ViewIndex, this);
   this->mpc_Widget->SetWidget(this->mpc_ParamWidget);
   //Activate child handles its own events for combo box pop up
   this->setHandlesChildEvents(false);
   //Handle initial edit mode
   this->EditModeActiveChanged(true);
}

//-----------------------------------------------------------------------------
/*!
   \brief   Default destructor

   Clean up.

   \created     25.10.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
C_GiSvDaParam::~C_GiSvDaParam(void)
{
   //lint -e{1540} Either handled by Qt parent handling or not owned by this class in the first place
}

//-----------------------------------------------------------------------------
/*!
   \brief   Returns the type of this item

   \return  ID

   \created     25.10.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
sintn C_GiSvDaParam::type(void) const
{
   return msn_GRAPHICS_ITEM_DB_PARAM;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Apply style

   \param[in] oe_Style    New style type
   \param[in] oq_DarkMode Flag if dark mode is active

   \created     25.10.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_GiSvDaParam::SetDisplayStyle(const C_PuiSvDbWidgetBase::E_Style oe_Style, const bool oq_DarkMode)
{
   C_GiSvDaRectBaseGroup::SetDisplayStyle(oe_Style, oq_DarkMode);
   if (this->mpc_ParamWidget != NULL)
   {
      this->mpc_ParamWidget->SetDark(oq_DarkMode);
   }
   //Reapply dark mode
   C_OgeWiUtil::h_ApplyStylesheetPropertyToItselfAndAllChildren(this->mpc_ParamWidget, "DarkMode", oq_DarkMode);
}

//-----------------------------------------------------------------------------
/*!
   \brief   Adjust font to current size

   \created     25.10.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_GiSvDaParam::ReInitializeSize(void)
{
   //Nothing to do, yet...
}

//-----------------------------------------------------------------------------
/*!
   \brief   Load data from system view dashboard

   \created     25.10.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_GiSvDaParam::LoadData(void)
{
   const C_PuiSvDashboard * const pc_Dashboard = this->GetSvDashboard();

   if (pc_Dashboard != NULL)
   {
      const C_PuiSvDbParam * const pc_Box = pc_Dashboard->GetParam(static_cast<uint32>(this->ms32_Index));
      tgl_assert(pc_Box != NULL);
      if (pc_Box != NULL)
      {
         this->LoadSvBasicData(*pc_Box);
         if (this->mpc_ParamWidget != NULL)
         {
            if (pc_Box->c_ColWidth.size() > 0UL)
            {
               this->mpc_ParamWidget->SetColumnWidth(pc_Box->c_ColWidth[0]);
            }
            else
            {
               //Initialize with default values
               std::vector<stw_types::sint32> c_Empty;
               this->mpc_ParamWidget->SetColumnWidth(c_Empty);
            }
            this->mpc_ParamWidget->SetAllExpandedTreeItems(pc_Box->c_ExpandedItems);
            this->mpc_ParamWidget->SetColumnPositionIndices(pc_Box->c_ColPosIndices);
         }
      }
   }
}

//-----------------------------------------------------------------------------
/*!
   \brief   Update data in system view dashboard

   \created     25.10.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_GiSvDaParam::UpdateData(void)
{
   const C_PuiSvDashboard * const pc_Dashboard = this->GetSvDashboard();

   if (pc_Dashboard != NULL)
   {
      const C_PuiSvDbParam * const pc_Box = pc_Dashboard->GetParam(static_cast<uint32>(this->ms32_Index));
      tgl_assert(pc_Box != NULL);
      if (pc_Box != NULL)
      {
         C_PuiSvDbParam c_Box = *pc_Box;
         this->UpdateSvBasicData(c_Box);
         c_Box.c_ColWidth.clear();
         if (this->mpc_ParamWidget != NULL)
         {
            c_Box.c_ColWidth.push_back(this->mpc_ParamWidget->GetCurrentColumnWidths());
            c_Box.c_ExpandedItems = this->mpc_ParamWidget->GetAllExpandedTreeItems();
            c_Box.c_ColPosIndices = this->mpc_ParamWidget->GetCurrentColumnPositionIndices();
         }
         tgl_assert(C_PuiSvHandler::h_GetInstance()->SetDashboardWidget(this->mu32_ViewIndex,
                                                                        this->mu32_DashboardIndex,
                                                                        static_cast<uint32>(this->ms32_Index),
                                                                        &c_Box, this->me_Type) == C_NO_ERR);
      }
   }
}

//-----------------------------------------------------------------------------
/*!
   \brief   Delete data in system view dashboard

   \created     25.10.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_GiSvDaParam::DeleteData(void)
{
   if (this->ms32_Index >= 0)
   {
      tgl_assert(C_PuiSvHandler::h_GetInstance()->DeleteDashboardWidget(this->mu32_ViewIndex, this->mu32_DashboardIndex,
                                                                        static_cast<uint32>(this->ms32_Index),
                                                                        this->me_Type) ==
                 C_NO_ERR);
   }
}

//-----------------------------------------------------------------------------
/*!
   \brief   Handle double click

   \created     23.11.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
bool C_GiSvDaParam::CallProperties(void)
{
   return false;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Updates the shown value of the element

   \created     01.09.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_GiSvDaParam::UpdateShowValue(void)
{
   C_GiSvDaRectBaseGroup::UpdateShowValue();
}

//-----------------------------------------------------------------------------
/*!
   \brief   Information about the start or stop of a connection

   \param[in]  oq_Active      Flag if connection is active or not active now

   \created     06.11.2017  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
void C_GiSvDaParam::ConnectionActiveChanged(const bool oq_Active)
{
   this->mq_Connected = oq_Active;
   if (this->mpc_ParamWidget != NULL)
   {
      this->mpc_ParamWidget->ConnectionActiveChanged(oq_Active);
   }

   //Avoid base implementation because of manual read button
   if (oq_Active == true)
   {
      // The configuration of the rail updaterates may have changed. Update it.
      this->m_UpdateDataPoolElementTimeoutAndValidFlag();
   }
}

//-----------------------------------------------------------------------------
/*!
   \brief   Information about the start or stop of edit mode

   \param[in]  oq_Active  Flag if edit mode is active or not active now

   \created     05.12.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_GiSvDaParam::EditModeActiveChanged(const bool oq_Active)
{
   this->mq_EditActive = oq_Active;
   tgl_assert(this->mpc_ParamWidget != NULL);
   if (this->mpc_ParamWidget != NULL)
   {
      this->mpc_ParamWidget->SetEditModeActive(oq_Active);
   }
}

//-----------------------------------------------------------------------------
/*!
   \brief   Handle manual user operation finished event

   \param[in] os32_Result Operation result
   \param[in] ou8_NRC     Negative response code, if any

   \created     15.11.2017  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
void C_GiSvDaParam::HandleManualOperationFinished(const sint32 os32_Result, const uint8 ou8_NRC)
{
   bool q_WidgetRelevant = false;

   if (this->mpc_ParamWidget != NULL)
   {
      q_WidgetRelevant = this->mpc_ParamWidget->HandleManualOperationFinished(os32_Result, ou8_NRC);
   }

   if (q_WidgetRelevant == false)
   {
      C_GiSvDaRectBaseGroup::HandleManualOperationFinished(os32_Result, ou8_NRC);
   }
}

//-----------------------------------------------------------------------------
/*!
   \brief   Signal all widgets which read rail element ID registrations failed

   \param[in]     orc_FailedIdRegisters    Failed IDs
   \param[in,out] orc_FailedIdErrorDetails Error details for element IDs which failed registration (if any)

   \created     16.07.2018  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_GiSvDaParam::SetErrorForFailedCyclicElementIdRegistrations(
   const std::vector<C_OSCNodeDataPoolListElementId> & orc_FailedIdRegisters,
   const std::vector<QString> & orc_FailedIdErrorDetails)
{
   //Explicitly do not call base implementation to disable this error for param widgets
   Q_UNUSED(orc_FailedIdRegisters)
   Q_UNUSED(orc_FailedIdErrorDetails)
}

//-----------------------------------------------------------------------------
/*!
   \brief   Sets the com driver for parametrization functions

   \param[in]  opc_ComDriver     Pointer to the com driver

   \created     27.10.2017  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
void C_GiSvDaParam::SetSyvComDriver(C_SyvComDriverDiag & orc_ComDriver)
{
   tgl_assert(this->mpc_ParamWidget != NULL);
   if (this->mpc_ParamWidget != NULL)
   {
      this->mpc_ParamWidget->SetSyvComDriver(orc_ComDriver);
   }
}

//-----------------------------------------------------------------------------
/*!
   \brief   Returns the newest registered value of a specific datapool element

   This function is thread safe.

   \param[in]     ou32_WidgetDataPoolElementIndex       Index of shown datapool element in widget
   \param[out]    orc_Value                             Result value

   \return
   C_NO_ERR    Value read
   C_RANGE     Index invalid
   C_NOACT     No value received

   \created     01.09.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
sint32 C_GiSvDaParam::GetLastValue(const uint32 ou32_WidgetDataPoolElementIndex, std::vector<QString> & orc_Values)
{
   std::vector<float64> c_Values;
   uint32 u32_InternalIndex;
   sint32 s32_Retval = this->m_MapDataElementIndexToInternalElementIndex(ou32_WidgetDataPoolElementIndex,
                                                                         u32_InternalIndex);

   if (s32_Retval == C_NO_ERR)
   {
      s32_Retval = this->m_GetLastNvmValue(ou32_WidgetDataPoolElementIndex, c_Values, &orc_Values);
   }
   return s32_Retval;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Returns the newest registered value of a specific datapool element

   This function is thread safe.

   \param[in]     ou32_WidgetDataPoolElementIndex       Index of shown datapool element in widget
   \param[out]    orc_Value                             Result value

   \return
   C_NO_ERR    Value read
   C_RANGE     Index invalid
   C_NOACT     No value received

   \created     27.06.2018  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
sint32 C_GiSvDaParam::GetLastValue2(const uint32 ou32_WidgetDataPoolElementIndex, std::vector<float64> & orc_Values)
{
   uint32 u32_InternalIndex;
   sint32 s32_Retval = this->m_MapDataElementIndexToInternalElementIndex(ou32_WidgetDataPoolElementIndex,
                                                                         u32_InternalIndex);

   if (s32_Retval == C_NO_ERR)
   {
      s32_Retval = this->m_GetLastNvmValue(ou32_WidgetDataPoolElementIndex, orc_Values, NULL);
   }
   return s32_Retval;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Get view index

   \return
   View index

   \created     25.06.2018  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
uint32 C_GiSvDaParam::GetViewIndex(void) const
{
   return this->mu32_ViewIndex;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Get table data item

   \return
   NULL Error
   Else Pointer to table data

   \created     11.10.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
const C_PuiSvDbParam * C_GiSvDaParam::GetParamItem(void) const
{
   const C_PuiSvDbParam * pc_Retval = NULL;
   const C_PuiSvData * const pc_View = C_PuiSvHandler::h_GetInstance()->GetView(this->mu32_ViewIndex);

   if (pc_View != NULL)
   {
      const C_PuiSvDashboard * const pc_Dashboard = pc_View->GetDashboard(this->mu32_DashboardIndex);
      if ((pc_Dashboard != NULL) && (this->GetIndex() >= 0))
      {
         pc_Retval = pc_Dashboard->GetParam(static_cast<uint32>(this->GetIndex()));
      }
   }
   return pc_Retval;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Apply new param content

   WARNING: Data element changes have to trigger HandleNewDataElement

   \param[in] orc_Content New content

   \return
   C_NO_ERR Operation success
   C_RANGE  Operation failure: parameter invalid

   \created     10.11.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
sint32 C_GiSvDaParam::SetParamItem(const C_PuiSvDbParam & orc_Content) const
{
   sint32 s32_Retval = C_NO_ERR;

   if (this->ms32_Index >= 0)
   {
      s32_Retval = C_PuiSvHandler::h_GetInstance()->SetDashboardWidget(this->mu32_ViewIndex, this->mu32_DashboardIndex,
                                                                       static_cast<uint32>(this->ms32_Index),
                                                                       &orc_Content, C_PuiSvDbDataElement::ePARAM);
   }
   else
   {
      s32_Retval = C_RANGE;
   }
   return s32_Retval;
}

//-----------------------------------------------------------------------------
/*!
   \brief   View dashboard param widget clear all data pool elements

   \return
   C_NO_ERR Operation success
   C_RANGE  Operation failure: parameter invalid

   \created     05.12.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
sint32 C_GiSvDaParam::ClearParamItemDataElement(void)
{
   sint32 s32_Retval = C_NO_ERR;

   if (this->ms32_Index >= 0)
   {
      this->ClearDataPoolElements();
      s32_Retval = C_PuiSvHandler::h_GetInstance()->ClearViewDashboardParamDataPoolElements(this->mu32_ViewIndex,
                                                                                            this->mu32_DashboardIndex,
                                                                                            static_cast<uint32>(this->
                                                                                                                ms32_Index));
   }
   else
   {
      s32_Retval = C_RANGE;
   }
   return s32_Retval;
}

//-----------------------------------------------------------------------------
/*!
   \brief   View dashboard param widget add new data pool element

   \param[in] orc_NewId   New ID
   \param[in] opc_Content Optional init value

   \return
   C_NO_ERR Operation success
   C_RANGE  Operation failure: parameter invalid

   \created     21.11.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
sint32 C_GiSvDaParam::AddParamItemDataElement(const C_OSCNodeDataPoolListElementId & orc_Id,
                                              const C_OSCNodeDataPoolContent * const opc_Content)
{
   sint32 s32_Retval = C_NO_ERR;

   if (this->ms32_Index >= 0)
   {
      const C_PuiSvDbParam * pc_ParamItem;
      s32_Retval = C_PuiSvHandler::h_GetInstance()->AddViewDashboardParamNewDataPoolElement(this->mu32_ViewIndex,
                                                                                            this->mu32_DashboardIndex,
                                                                                            static_cast<uint32>(this->
                                                                                                                ms32_Index),
                                                                                            orc_Id, opc_Content);
      //Trigger data element reload
      pc_ParamItem = this->GetParamItem();
      if (pc_ParamItem != NULL)
      {
         for (uint32 u32_ItConfig = 0; u32_ItConfig < pc_ParamItem->c_DataPoolElementsConfig.size(); ++u32_ItConfig)
         {
            const C_PuiSvDbNodeDataElementConfig & rc_Config = pc_ParamItem->c_DataPoolElementsConfig[u32_ItConfig];
            if ((rc_Config.c_ElementId.GetIsValid() == true) && (rc_Config.c_ElementId == orc_Id))
            {
               this->RegisterDataPoolElement(rc_Config.c_ElementId, rc_Config.c_ElementScaling);
            }
         }
      }
   }
   else
   {
      s32_Retval = C_RANGE;
   }
   return s32_Retval;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Accept external data element changes

   \created     08.11.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_GiSvDaParam::HandleNewDataElement(void)
{
   const C_PuiSvDashboard * const pc_Dashboard = this->GetSvDashboard();

   if (pc_Dashboard != NULL)
   {
      const C_PuiSvDbParam * const pc_Box = pc_Dashboard->GetParam(static_cast<uint32>(this->ms32_Index));
      tgl_assert(pc_Box != NULL);
      if (pc_Box != NULL)
      {
         this->ClearDataPoolElements();
         for (uint32 u32_It = 0; u32_It < pc_Box->c_DataPoolElementsConfig.size(); ++u32_It)
         {
            this->RegisterDataPoolElement(pc_Box->c_DataPoolElementsConfig[u32_It].c_ElementId,
                                          pc_Box->c_DataPoolElementsConfig[u32_It].c_ElementScaling);
         }
      }
   }
}

//-----------------------------------------------------------------------------
/*!
   \brief   Delete lists

   \param[in] orc_ListIds Lists to delete

   \return
   C_NO_ERR Operation success
   C_CONFIG Operation failure: configuration invalid

   \created     16.11.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
sint32 C_GiSvDaParam::DeleteLists(const std::vector<stw_opensyde_core::C_OSCNodeDataPoolListId> & orc_ListIds)
{
   sint32 s32_Retval = C_NO_ERR;

   //Implicitly update the data this one is important for the following reload step!
   this->UpdateData();

   if (orc_ListIds.size() > 0)
   {
      std::vector<C_PuiSvDbNodeDataPoolListElementId> c_DeletedIds;
      const C_PuiSvDbParam * const pc_Param = this->GetParamItem();
      tgl_assert(pc_Param != NULL);
      if (pc_Param != NULL)
      {
         tgl_assert(pc_Param->c_DataPoolElementsConfig.size() == pc_Param->c_DataSetSelectionIndices.size());
         if (pc_Param->c_DataPoolElementsConfig.size() == pc_Param->c_DataSetSelectionIndices.size())
         {
            tgl_assert(pc_Param->c_DataPoolElementsConfig.size() == pc_Param->c_ListValues.size());
            if (pc_Param->c_DataPoolElementsConfig.size() == pc_Param->c_ListValues.size())
            {
               C_PuiSvDbParam c_Copy = *pc_Param;
               //Each found list
               for (uint32 u32_ItListId = 0; u32_ItListId < orc_ListIds.size(); ++u32_ItListId)
               {
                  const C_OSCNodeDataPoolListId & rc_ListId = orc_ListIds[u32_ItListId];
                  //Check each element
                  for (uint32 u32_ItElement = 0; u32_ItElement < c_Copy.c_DataPoolElementsConfig.size();)
                  {
                     const C_PuiSvDbNodeDataElementConfig & rc_Config =
                        c_Copy.c_DataPoolElementsConfig[u32_ItElement];
                     tgl_assert(rc_Config.c_ElementId.GetIsValid() == true);
                     if (rc_Config.c_ElementId.GetIsValid() == true)
                     {
                        if (((rc_Config.c_ElementId.u32_NodeIndex == rc_ListId.u32_NodeIndex) &&
                             (rc_Config.c_ElementId.u32_DataPoolIndex == rc_ListId.u32_DataPoolIndex)) &&
                            (rc_Config.c_ElementId.u32_ListIndex == rc_ListId.u32_ListIndex))
                        {
                           //Save for read rail verification (BEFORE deleting)
                           c_DeletedIds.push_back(rc_Config.c_ElementId);
                           //Update internal data (BEFORE deleting)
                           this->RemoveDataPoolElement(rc_Config.c_ElementId);
                           //Match -> delete all relevant items
                           c_Copy.c_DataPoolElementsConfig.erase(
                              c_Copy.c_DataPoolElementsConfig.begin() + u32_ItElement);
                           c_Copy.c_DataSetSelectionIndices.erase(
                              c_Copy.c_DataSetSelectionIndices.begin() + u32_ItElement);
                           c_Copy.c_ListValues.erase(c_Copy.c_ListValues.begin() + u32_ItElement);
                           //No iteration because new element at current index
                        }
                        else
                        {
                           //Important iterator step
                           ++u32_ItElement;
                        }
                     }
                     else
                     {
                        //Important iterator step
                        ++u32_ItElement;
                     }
                  }
                  for (uint32 u32_ItExpansion = 0UL; u32_ItExpansion < c_Copy.c_ExpandedItems.size();)
                  {
                     const C_PuiSvDbExpandedTreeIndex & rc_Expanded = c_Copy.c_ExpandedItems[u32_ItExpansion];
                     if (((rc_Expanded.c_ExpandedId.u32_NodeIndex == rc_ListId.u32_NodeIndex) &&
                          (rc_Expanded.c_ExpandedId.u32_DataPoolIndex == rc_ListId.u32_DataPoolIndex)) &&
                         (rc_Expanded.c_ExpandedId.u32_ListIndex == rc_ListId.u32_ListIndex))
                     {
                        //New item at current position
                        c_Copy.c_ExpandedItems.erase(c_Copy.c_ExpandedItems.begin() + u32_ItExpansion);
                     }
                     else
                     {
                        //Important iterator step
                        ++u32_ItExpansion;
                     }
                  }
               }
               tgl_assert(this->SetParamItem(c_Copy) == C_NO_ERR);
            }
            else
            {
               s32_Retval = C_CONFIG;
            }
         }
         else
         {
            s32_Retval = C_CONFIG;
         }
      }
      else
      {
         s32_Retval = C_CONFIG;
      }
      if (s32_Retval == C_NO_ERR)
      {
         //Check read rails
         const C_PuiSvData * const pc_View = C_PuiSvHandler::h_GetInstance()->GetView(this->mu32_ViewIndex);
         if (pc_View != NULL)
         {
            for (uint32 u32_ItDeletedItem = 0; (u32_ItDeletedItem < c_DeletedIds.size()) && (s32_Retval == C_NO_ERR);
                 ++u32_ItDeletedItem)
            {
               if (pc_View->CheckReadUsage(c_DeletedIds[u32_ItDeletedItem]) == false)
               {
                  s32_Retval = C_PuiSvHandler::h_GetInstance()->RemoveViewReadRailItem(this->mu32_ViewIndex,
                                                                                       c_DeletedIds[u32_ItDeletedItem]);
               }
            }
         }
      }
   }
   return s32_Retval;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Get improved parent for all pop ups

   \return
   Improved parent for all pop ups

   \created     18.07.2018  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
QWidget * C_GiSvDaParam::GetPopUpParent(void) const
{
   QWidget * const pc_Retval = this->scene()->views().at(0);

   return pc_Retval;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Check if warning icon is allowed

   \return
   True  Warning icon is allowed
   False Warning icon is not allowed

   \created     20.08.2018  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
bool C_GiSvDaParam::m_AllowWarningIcon(void) const
{
   return false;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Check if the refresh button is available

   \return
   True  Refresh icon will be displayed if necessary
   False Refresh icon will never be displayed

   \created     14.05.2018  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
bool C_GiSvDaParam::m_AllowRefreshButton(void) const
{
   return false;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Get common tool tip content if no other item takes precedence over the tool tip

   \return
   Common tool tip content if no other item takes precedence over the tool tip

   \created     20.08.2018  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
QString C_GiSvDaParam::m_GetCommonToolTipContent(void) const
{
   //No common tool tip!
   return "";
}

//-----------------------------------------------------------------------------
/*!
   \brief   Map widget data element index to data element handler index

   \param[in]  ou32_DataElementIndex      Widget data element index
   \param[out] oru32_InternalElementIndex Data element handler index

   \return
   C_NO_ERR Operation success
   C_RANGE  Operation failure: parameter invalid

   \created     09.11.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
sint32 C_GiSvDaParam::m_MapDataElementIndexToInternalElementIndex(const uint32 ou32_DataElementIndex,
                                                                  uint32 & oru32_InternalElementIndex) const
{
   sint32 s32_Retval = C_RANGE;
   const C_PuiSvDbParam * const pc_Param = this->GetParamItem();

   if (pc_Param != NULL)
   {
      if (ou32_DataElementIndex < pc_Param->c_DataPoolElementsConfig.size())
      {
         const C_PuiSvDbNodeDataElementConfig & rc_Config = pc_Param->c_DataPoolElementsConfig[ou32_DataElementIndex];
         s32_Retval = this->GetWidgetDataPoolElementIndex(rc_Config.c_ElementId, oru32_InternalElementIndex);
      }
   }
   return s32_Retval;
}
