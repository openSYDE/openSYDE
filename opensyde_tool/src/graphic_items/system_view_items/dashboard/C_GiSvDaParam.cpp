//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Class for system view dashboard parameterization table item (implementation)

   Class for system view dashboard parameterization table item

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.hpp"

#include <limits>
#include <QMouseEvent>
#include <QGraphicsView>
#include "gitypes.hpp"
#include "stwtypes.hpp"
#include "TglUtils.hpp"
#include "stwerrors.hpp"
#include "C_GtGetText.hpp"
#include "C_OgeWiUtil.hpp"
#include "C_GiSvDaParam.hpp"
#include "C_PuiSvHandler.hpp"

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

   \param[in]      oru32_ViewIndex        Index of system view
   \param[in]      oru32_DashboardIndex   Index of dashboard in system view
   \param[in]      ors32_DataIndex        Index of data element in dashboard in system view
   \param[in]      oru64_Id               Unique ID
   \param[in,out]  opc_Parent             Optional pointer to parent
*/
//----------------------------------------------------------------------------------------------------------------------
C_GiSvDaParam::C_GiSvDaParam(const uint32_t & oru32_ViewIndex, const uint32_t & oru32_DashboardIndex,
                             const int32_t & ors32_DataIndex, const uint64_t & oru64_Id,
                             QGraphicsItem * const opc_Parent) :
   C_GiSvDaRectBaseGroup(oru32_ViewIndex, oru32_DashboardIndex, ors32_DataIndex, C_PuiSvDbDataElement::ePARAM,
                         static_cast<uint32_t>(std::numeric_limits<int32_t>::max()),
                         oru64_Id, 90.0, 50.0, 180.0, 100.0, false, true, opc_Parent),
   mq_Connected(false),
   mpc_AddDataElement(NULL)
{
   this->mpc_ParamWidget = new C_SyvDaItPaWidgetNew(oru32_ViewIndex, this);
   this->mpc_Widget->SetWidget(this->mpc_ParamWidget);
   //Activate child handles its own events for combo box pop up
   // No edit and no edit content mode: The child must handle its own events
   this->setHandlesChildEvents(false);
   //Handle initial edit mode
   this->C_GiSvDaParam::EditModeActiveChanged(true);

   //Connections
   connect(this->mpc_ParamWidget, &C_SyvDaItPaWidgetNew::SigNvmReadList, this, &C_GiSvDaParam::SigNvmReadList);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default destructor

   Clean up.
*/
//----------------------------------------------------------------------------------------------------------------------
//lint -e{1540} Either handled by Qt parent handling or not owned by this class in the first place
C_GiSvDaParam::~C_GiSvDaParam(void)
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Returns the type of this item

   \return  ID
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_GiSvDaParam::type(void) const
{
   return ms32_GRAPHICS_ITEM_DB_PARAM;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Apply style

   \param[in]  oe_Style       New style type
   \param[in]  oq_DarkMode    Flag if dark mode is active
*/
//----------------------------------------------------------------------------------------------------------------------
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

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Adjust font to current size
*/
//----------------------------------------------------------------------------------------------------------------------
//lint -e{9175} intentionally no functionality in this implementation
void C_GiSvDaParam::ReInitializeSize(void)
{
   //Nothing to do, yet...
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Load data from system view dashboard
*/
//----------------------------------------------------------------------------------------------------------------------
void C_GiSvDaParam::LoadData(void)
{
   const C_PuiSvDashboard * const pc_Dashboard = this->m_GetSvDashboard();

   if (pc_Dashboard != NULL)
   {
      const C_PuiSvDbParam * const pc_Box = pc_Dashboard->GetParam(static_cast<uint32_t>(this->ms32_Index));
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
               const std::vector<int32_t> c_Empty;
               this->mpc_ParamWidget->SetColumnWidth(c_Empty);
            }
            this->mpc_ParamWidget->SetAllExpandedTreeItems(pc_Box->c_ExpandedItems);
            this->mpc_ParamWidget->SetColumnPositionIndices(pc_Box->c_ColPosIndices);
         }
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Update data in system view dashboard
*/
//----------------------------------------------------------------------------------------------------------------------
void C_GiSvDaParam::UpdateData(void)
{
   const C_PuiSvDashboard * const pc_Dashboard = this->m_GetSvDashboard();

   if (pc_Dashboard != NULL)
   {
      const C_PuiSvDbParam * const pc_Box = pc_Dashboard->GetParam(static_cast<uint32_t>(this->ms32_Index));
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
                                                                        static_cast<uint32_t>(this->ms32_Index),
                                                                        &c_Box, this->me_Type) == C_NO_ERR);
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Delete data in system view dashboard
*/
//----------------------------------------------------------------------------------------------------------------------
void C_GiSvDaParam::DeleteData(void)
{
   if (this->ms32_Index >= 0)
   {
      tgl_assert(C_PuiSvHandler::h_GetInstance()->DeleteDashboardWidget(this->mu32_ViewIndex, this->mu32_DashboardIndex,
                                                                        static_cast<uint32_t>(this->ms32_Index),
                                                                        this->me_Type) ==
                 C_NO_ERR);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Updates the shown value of the element
*/
//----------------------------------------------------------------------------------------------------------------------
void C_GiSvDaParam::UpdateShowValue(void)
{
   C_GiSvDaRectBaseGroup::UpdateShowValue();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Information about the start or stop of a connection

   \param[in]  oq_Active   Flag if connection is active or not active now
*/
//----------------------------------------------------------------------------------------------------------------------
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

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Information about the start or stop of edit mode

   \param[in]  oq_Active   Flag if edit mode is active or not active now
*/
//----------------------------------------------------------------------------------------------------------------------
void C_GiSvDaParam::EditModeActiveChanged(const bool oq_Active)
{
   C_GiSvDaRectBaseGroup::EditModeActiveChanged(oq_Active);

   //Adapt child handles its own events for combo box pop up depending of the edit mode
   // No edit and no edit content mode: The child must handle its own events
   // Edit mode and no edit content mode: The child shall not handle its own events due to moving functionality
   // Edit mode and edit content mode: The child must handle its own events
   this->setHandlesChildEvents(oq_Active);

   tgl_assert(this->mpc_ParamWidget != NULL);
   if (this->mpc_ParamWidget != NULL)
   {
      this->mpc_ParamWidget->SetEditMode(this->mq_EditModeActive, this->mq_EditContentModeEnabled);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Enable edit content mode for widgets

   \retval false  nothing done
   \retval true   edit content mode enabled
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_GiSvDaParam::EnableEditContent(void)
{
   const bool q_Return = C_GiSvDaRectBaseGroup::EnableEditContent();

   //Adapt child handles its own events for combo box pop up depending of the edit mode
   // Edit mode and edit content mode: The child must handle its own events
   this->setHandlesChildEvents(false);

   if (q_Return == true)
   {
      tgl_assert(this->mpc_ParamWidget != NULL);
      if (this->mpc_ParamWidget != NULL)
      {
         this->mpc_ParamWidget->SetEditMode(this->mq_EditModeActive, true);
      }
   }
   return q_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Disable edit content mode
*/
//----------------------------------------------------------------------------------------------------------------------
void C_GiSvDaParam::DisableEditContent(void)
{
   C_GiSvDaRectBaseGroup::DisableEditContent();

   //Adapt child handles its own events for combo box pop up depending of the edit mode
   // Edit mode and no edit content mode: The child shall not handle its own events due to moving functionality
   this->setHandlesChildEvents(true);

   tgl_assert(this->mpc_ParamWidget != NULL);
   if (this->mpc_ParamWidget != NULL)
   {
      this->mpc_ParamWidget->SetEditMode(this->mq_EditModeActive, false);
      // In case of a still visible tool tip, it will no disappear due to the not forwarded events and an own scene
      // independent handling, it must be hided manually
      this->mpc_ParamWidget->HideToolTip();
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Activates or deactivates all relevant context menu entries for this item

   Context menu functions:
   - Add

   \param[in]  opc_ContextMenuManager  Pointer to context menu manager for registration of actions
   \param[in]  oq_Active               Flag if context menu entries have to be shown or not
*/
//----------------------------------------------------------------------------------------------------------------------
void C_GiSvDaParam::ConfigureContextMenu(C_SyvDaContextMenuManager * const opc_ContextMenuManager, const bool oq_Active)
{
   if (this->mq_EditContentModeEnabled == false)
   {
      // Normal edit mode, use default dashboard context menu
      C_GiSvDaRectBaseGroup::ConfigureContextMenu(opc_ContextMenuManager, oq_Active);
   }
   else
   {
      // Deactivate the default dashboard scene actions.
      // In content edit mode only widget specific functions are relevant
      opc_ContextMenuManager->SetSpecificActionsAvailable(false, true);

      tgl_assert(this->mpc_ParamWidget != NULL);
      if ((oq_Active == true) && (this->mpc_ParamWidget != NULL))
      {
         // Initial registration of the context menu
         if (mpc_AddDataElement == NULL)
         {
            mpc_AddDataElement =
               opc_ContextMenuManager->RegisterActionWithKeyboardShortcut(C_GtGetText::h_GetText(
                                                                             "Add list(s)"),
                                                                          static_cast<int32_t>(Qt::CTRL) +
                                                                          static_cast<int32_t>(Qt::Key_Plus));
            // The action has to be set invisible initial. Only with that the function SetVisibleWithAutoHide can work.
            this->mpc_AddDataElement->setVisible(false);
            connect(mpc_AddDataElement, &QAction::triggered, this->mpc_ParamWidget,
                    &C_SyvDaItPaWidgetNew::ButtonAddClicked);
         }
         if (mpc_AddDataElement != NULL)
         {
            opc_ContextMenuManager->SetVisibleWithAutoHide(this->mpc_AddDataElement);
         }
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Handle manual user operation finished event

   \param[in]  os32_Result    Operation result
   \param[in]  ou8_Nrc        Negative response code, if any
*/
//----------------------------------------------------------------------------------------------------------------------
void C_GiSvDaParam::HandleManualOperationFinished(const int32_t os32_Result, const uint8_t ou8_Nrc)
{
   bool q_WidgetRelevant = false;

   if (this->mpc_ParamWidget != NULL)
   {
      q_WidgetRelevant = this->mpc_ParamWidget->HandleManualOperationFinished(os32_Result, ou8_Nrc);
   }

   if (q_WidgetRelevant == false)
   {
      C_GiSvDaRectBaseGroup::HandleManualOperationFinished(os32_Result, ou8_Nrc);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Sets the com driver for parametrization functions

   \param[in]  orc_ComDriver  Reference to the com driver
*/
//----------------------------------------------------------------------------------------------------------------------
void C_GiSvDaParam::SetSyvComDriver(C_SyvComDriverDiag & orc_ComDriver)
{
   tgl_assert(this->mpc_ParamWidget != NULL);
   if (this->mpc_ParamWidget != NULL)
   {
      this->mpc_ParamWidget->SetSyvComDriver(orc_ComDriver);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Returns the newest registered value of a specific datapool element

   This function is thread safe.

   \param[in]   ou32_WidgetDataPoolElementIndex    Index of shown datapool element in widget
   \param[out]  orc_Value                          Raw result value

   \return
   C_NO_ERR    Value read
   C_RANGE     Index invalid
   C_NOACT     No value received
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_GiSvDaParam::GetLastValue(const uint32_t ou32_WidgetDataPoolElementIndex,
                                    C_OscNodeDataPoolContent & orc_Value)
{
   uint32_t u32_InternalIndex;
   int32_t s32_Retval = this->m_MapDataElementIndexToInternalElementIndex(ou32_WidgetDataPoolElementIndex,
                                                                          u32_InternalIndex);

   if (s32_Retval == C_NO_ERR)
   {
      s32_Retval = this->m_GetLastNvmValue(ou32_WidgetDataPoolElementIndex, orc_Value);
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get view index

   \return
   View index
*/
//----------------------------------------------------------------------------------------------------------------------
uint32_t C_GiSvDaParam::GetViewIndex(void) const
{
   return this->mu32_ViewIndex;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get table data item

   \return
   NULL Error
   Else Pointer to table data
*/
//----------------------------------------------------------------------------------------------------------------------
const C_PuiSvDbParam * C_GiSvDaParam::GetParamItem(void) const
{
   const C_PuiSvDbParam * pc_Retval = NULL;
   const C_PuiSvData * const pc_View = C_PuiSvHandler::h_GetInstance()->GetView(this->mu32_ViewIndex);

   if (pc_View != NULL)
   {
      const C_PuiSvDashboard * const pc_Dashboard = pc_View->GetDashboard(this->mu32_DashboardIndex);
      if ((pc_Dashboard != NULL) && (this->GetIndex() >= 0))
      {
         pc_Retval = pc_Dashboard->GetParam(static_cast<uint32_t>(this->GetIndex()));
      }
   }
   return pc_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Apply new param content

   WARNING: Data element changes have to trigger HandleNewDataElement

   \param[in]  orc_Content    New content

   \return
   C_NO_ERR Operation success
   C_RANGE  Operation failure: parameter invalid
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_GiSvDaParam::SetParamItem(const C_PuiSvDbParam & orc_Content) const
{
   int32_t s32_Retval = C_NO_ERR;

   if (this->ms32_Index >= 0)
   {
      s32_Retval = C_PuiSvHandler::h_GetInstance()->SetDashboardWidget(this->mu32_ViewIndex, this->mu32_DashboardIndex,
                                                                       static_cast<uint32_t>(this->ms32_Index),
                                                                       &orc_Content, C_PuiSvDbDataElement::ePARAM);
   }
   else
   {
      s32_Retval = C_RANGE;
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   View dashboard param widget add new data pool element

   \param[in]  orc_Id         New ID
   \param[in]  opc_Content    Optional init value

   \return
   C_NO_ERR Operation success
   C_RANGE  Operation failure: parameter invalid
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_GiSvDaParam::AddParamItemDataElement(const C_OscNodeDataPoolListElementId & orc_Id,
                                               const C_OscNodeDataPoolContent * const opc_Content)
{
   int32_t s32_Retval = C_NO_ERR;

   if (this->ms32_Index >= 0)
   {
      const C_PuiSvDbParam * pc_ParamItem;
      s32_Retval = C_PuiSvHandler::h_GetInstance()->AddViewDashboardParamNewDataPoolElement(this->mu32_ViewIndex,
                                                                                            this->mu32_DashboardIndex,
                                                                                            static_cast<uint32_t>(this->
                                                                                                                  ms32_Index),
                                                                                            orc_Id, opc_Content);
      //Trigger data element reload
      pc_ParamItem = this->GetParamItem();
      if (pc_ParamItem != NULL)
      {
         for (uint32_t u32_ItConfig = 0; u32_ItConfig < pc_ParamItem->c_DataPoolElementsConfig.size(); ++u32_ItConfig)
         {
            const C_PuiSvDbNodeDataElementConfig & rc_Config = pc_ParamItem->c_DataPoolElementsConfig[u32_ItConfig];
            if ((rc_Config.c_ElementId.GetIsValid() == true) && (rc_Config.c_ElementId == orc_Id))
            {
               this->RegisterDataPoolElement(rc_Config.c_ElementId, rc_Config.c_ElementScaling,
                                             rc_Config.c_DisplayFormatter);
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

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Delete lists

   \param[in]  orc_ListIds    Lists to delete

   \return
   C_NO_ERR Operation success
   C_CONFIG Operation failure: configuration invalid
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_GiSvDaParam::DeleteLists(const std::vector<stw::opensyde_core::C_OscNodeDataPoolListId> & orc_ListIds)
{
   int32_t s32_Retval = C_NO_ERR;

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
               //Check each element (back to front!)
               for (uint32_t u32_ItElement = pc_Param->c_DataPoolElementsConfig.size(); u32_ItElement > 0UL;
                    --u32_ItElement)
               {
                  const C_PuiSvDbNodeDataElementConfig & rc_Config =
                     pc_Param->c_DataPoolElementsConfig[static_cast<uint32_t>(u32_ItElement) - 1U];
                  //Each found list
                  for (uint32_t u32_ItListId = 0; u32_ItListId < orc_ListIds.size(); ++u32_ItListId)
                  {
                     const C_OscNodeDataPoolListId & rc_ListId = orc_ListIds[u32_ItListId];
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
                           c_Copy.RemoveElement(u32_ItElement - 1UL);
                           //No iteration because new element at current index
                        }
                     }
                  }
               }
               //Each found list
               for (uint32_t u32_ItListId = 0; u32_ItListId < orc_ListIds.size(); ++u32_ItListId)
               {
                  const C_OscNodeDataPoolListId & rc_ListId = orc_ListIds[u32_ItListId];
                  //Handle expansion (back to front!)
                  for (uint32_t u32_ItExpansion = c_Copy.c_ExpandedItems.size(); u32_ItExpansion > 0UL;
                       --u32_ItExpansion)
                  {
                     const C_PuiSvDbExpandedTreeIndex & rc_Expanded =
                        c_Copy.c_ExpandedItems[static_cast<uint32_t>(u32_ItExpansion) - 1U];
                     if (((rc_Expanded.c_ExpandedId.u32_NodeIndex == rc_ListId.u32_NodeIndex) &&
                          (rc_Expanded.c_ExpandedId.u32_DataPoolIndex == rc_ListId.u32_DataPoolIndex)) &&
                         (rc_Expanded.c_ExpandedId.u32_ListIndex == rc_ListId.u32_ListIndex))
                     {
                        //New item at current position
                        c_Copy.c_ExpandedItems.erase(c_Copy.c_ExpandedItems.begin() +
                                                     (static_cast<int32_t>(u32_ItExpansion) - 1));
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
            for (uint32_t u32_ItDeletedItem = 0; (u32_ItDeletedItem < c_DeletedIds.size()) && (s32_Retval == C_NO_ERR);
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

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get improved parent for all pop ups

   \return
   Improved parent for all pop ups
*/
//----------------------------------------------------------------------------------------------------------------------
QWidget * C_GiSvDaParam::GetPopUpParent(void) const
{
   QWidget * const pc_Retval = this->scene()->views().at(0);

   return pc_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Check if warning icon is allowed

   \return
   True  Warning icon is allowed
   False Warning icon is not allowed
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_GiSvDaParam::m_AllowWarningIcon(void) const
{
   return false;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Check if element supports the edit content mode

   \return
   True  Element does have and support a widget specific edit content mode
   False Element does not have and support a widget specific edit content mode
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_GiSvDaParam::m_HasEditContentMode(void) const
{
   return true;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Check if the refresh button is available

   \return
   True  Refresh icon will be displayed if necessary
   False Refresh icon will never be displayed
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_GiSvDaParam::m_AllowRefreshButton(void) const
{
   return false;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get common tool tip content if no other item takes precedence over the tool tip

   \return
   Common tool tip content if no other item takes precedence over the tool tip
*/
//----------------------------------------------------------------------------------------------------------------------
QString C_GiSvDaParam::m_GetCommonToolTipContent(void) const
{
   //No common tool tip!
   return "";
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Map widget data element index to data element handler index

   \param[in]   ou32_DataElementIndex        Widget data element index
   \param[out]  oru32_InternalElementIndex   Data element handler index

   \return
   C_NO_ERR Operation success
   C_RANGE  Operation failure: parameter invalid
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_GiSvDaParam::m_MapDataElementIndexToInternalElementIndex(const uint32_t ou32_DataElementIndex,
                                                                   uint32_t & oru32_InternalElementIndex) const
{
   int32_t s32_Retval = C_RANGE;
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
