//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       List with all datapools of a specific type of one node(implementation)

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.h"

#include <QPainter>
#include <QApplication>

#include "stwtypes.h"
#include "stwerrors.h"
#include "constants.h"

#include "C_SdNdeDpSelectorListWidget.h"
#include "C_GtGetText.h"
#include "C_OgePopUpDialog.h"
#include "C_SdNdeDpProperties.h"
#include "C_SdNdeDpSelectorAddWidget.h"
#include "C_PuiSdHandler.h"
#include "C_SdClipBoardHelper.h"
#include "C_Uti.h"
#include "C_PuiSdUtil.h"
#include "C_OgeWiCustomMessage.h"
#include "C_SdUtil.h"
#include "TGLUtils.h"
#include "C_PuiSdSharedDatapools.h"
#include "C_OSCLoggingHandler.h"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw_types;
using namespace stw_errors;
using namespace stw_opensyde_gui;
using namespace stw_opensyde_gui_logic;
using namespace stw_opensyde_gui_elements;
using namespace stw_opensyde_core;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

/* -- Global Variables ---------------------------------------------------------------------------------------------- */

/* -- Module Global Variables --------------------------------------------------------------------------------------- */

/* -- Module Global Function Prototypes ----------------------------------------------------------------------------- */

/* -- Implementation ------------------------------------------------------------------------------------------------ */

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default constructor

   \param[in,out] opc_Parent Optional pointer to parent
*/
//----------------------------------------------------------------------------------------------------------------------
C_SdNdeDpSelectorListWidget::C_SdNdeDpSelectorListWidget(QWidget * const opc_Parent) :
   QWidget(opc_Parent),
   mpc_ContextMenu(NULL),
   mpc_AddAction(NULL),
   mpc_EditAction(NULL),
   mpc_EditContentAction(NULL),
   mpc_EditActionSeparator(NULL),
   mpc_DeleteAction(NULL),
   mpc_DeleteActionSeparator(NULL),
   mpc_CopyAction(NULL),
   mpc_CutAction(NULL),
   mpc_PasteAction(NULL),
   mpc_MoveActionSeparator(NULL),
   mpc_MoveLeftAction(NULL),
   mpc_MoveRightAction(NULL),
   mpc_MoveUpAction(NULL),
   mpc_MoveDownAction(NULL),
   mq_Selected(false),
   msn_ItemsPerRow(3),
   msn_ItemsPerViewPage(-1),
   msn_RowsPerViewPage(-1),
   msn_CountViewPages(1),
   msn_CurrentViewPage(0),
   msn_SelectedItemIndex(-1),
   mu32_NodeIndex(0U),
   me_DataPoolType(stw_opensyde_core::C_OSCNodeDataPool::eDIAG),
   me_ProtocolType(stw_opensyde_core::C_OSCCanProtocol::eLAYER2),
   mq_UsageViewActive(false),
   msn_ItemWidth(146),
   msn_ItemHeight(141),
   msn_HeightHint(141)
{
   // TODO BAY:
   //this->setItemDelegate(&this->mc_Delegate);

   //Custom context menu
   m_SetupContextMenu();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   default destructor

   Clean up.
*/
//----------------------------------------------------------------------------------------------------------------------
//lint -e{1540}  no memory leak because of the parents of mpc_ContextMenu and the Qt memory management
C_SdNdeDpSelectorListWidget::~C_SdNdeDpSelectorListWidget()
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Specify associated node

   \param[in] oe_Type              Datapool type
   \param[in] ou32_NodeIndex       Node index
   \param[in] oq_UsageViewActive   Flag for showing usage bars for datapool

   \return
   true   Datapool added
   false  no Datapool added
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_SdNdeDpSelectorListWidget::SetTypeAndNode(const stw_opensyde_core::C_OSCNodeDataPool::E_Type oe_Type,
                                                 const uint32 ou32_NodeIndex, const bool oq_UsageViewActive)
{
   bool q_Return = false;

   this->mq_UsageViewActive = oq_UsageViewActive;
   this->me_DataPoolType = oe_Type;
   this->mu32_NodeIndex = ou32_NodeIndex;

   // Reset the selected index to avoid problems when the selection will be restored with the same index
   this->msn_SelectedItemIndex = -1;

   this->m_InitFromData();

   if (this->mc_DpItems.size() > 0)
   {
      q_Return = true;
   }

   return q_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Sets the widget selected and adapts the sub widgets when necessary

   Adapts the stylesheet of the items

   \param[in]  oq_Selected   Flag if widget is selected or not
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDpSelectorListWidget::SetSelected(const bool oq_Selected)
{
   C_SdNdeDpSelectorItemWidget * pc_WidgetItem;
   sintn sn_Counter;

   if (oq_Selected == false)
   {
      // deactivate all items
      for (sn_Counter = 0; sn_Counter < this->mc_DpItems.size(); ++sn_Counter)
      {
         pc_WidgetItem = this->mc_DpItems.at(sn_Counter);
         if (pc_WidgetItem != NULL)
         {
            // ask before to improve performance
            if (pc_WidgetItem->GetSelected() == true)
            {
               pc_WidgetItem->SetSelected(false);
            }
         }
      }
      this->mq_Selected = false;
   }
   else if (this->mq_Selected == false) // only if no item is already active
   {
      sintn sn_CurrentView = this->GetCurrentItemIndex();
      if (sn_CurrentView < 0)
      {
         sn_CurrentView = 0;
      }

      for (sn_Counter = 0; sn_Counter < this->mc_DpItems.size(); ++sn_Counter)
      {
         // activate the first item and deactivate all other items
         pc_WidgetItem = this->mc_DpItems.at(sn_Counter);
         if (pc_WidgetItem != NULL)
         {
            if (sn_Counter == sn_CurrentView)
            {
               pc_WidgetItem->SetSelected(true);
               this->mq_Selected = true;
            }
            else
            {
               // ask before to improve performance
               if (pc_WidgetItem->GetSelected() == true)
               {
                  pc_WidgetItem->SetSelected(false);
               }
            }
         }
      }
   }
   else
   {
      // nothing to do
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Sets the conflict state of the active datapool

   \param[in] osn_DataPoolWidgetIndex  Datapool type index of Datapool which is affected
   \param[in] oq_Active     Flag if conflict is active or not

   \return
   true     minimum one conflict
   false    no conflicts in all Datapools
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_SdNdeDpSelectorListWidget::SetActualDataPoolConflict(const stw_types::sintn osn_DataPoolWidgetIndex,
                                                            const bool oq_Active) const
{
   bool q_Return = false;
   sintn sn_Counter;
   C_SdNdeDpSelectorItemWidget * pc_WidgetItem;
   const C_PuiSdSharedDatapools & rc_SharedDatapools = C_PuiSdHandler::h_GetInstance()->GetSharedDatapoolsConst();

   C_OSCNodeDataPoolId c_ActiveDatapool;

   std::vector<C_OSCNodeDataPoolId> c_ChangedDatapools;

   // Search the active Widget
   if (osn_DataPoolWidgetIndex < this->mc_DpItems.size())
   {
      pc_WidgetItem = this->mc_DpItems.at(osn_DataPoolWidgetIndex);
      if (pc_WidgetItem != NULL)
      {
         //Avoid current row because on error change this widget does not necessarily have the focus
         c_ActiveDatapool = pc_WidgetItem->GetDatapoolId();
      }
   }

   rc_SharedDatapools.IsSharedAndGetDatapoolGroup(c_ActiveDatapool, c_ChangedDatapools);
   // Add the active Datapool to the result. It will not be added by IsSharedAndGetDatapoolGroup
   c_ChangedDatapools.push_back(c_ActiveDatapool);

   // Check all Datapools for a conflict and deactivate the conflict state of the actual datapool
   for (sn_Counter = 0; sn_Counter < this->mc_DpItems.size(); ++sn_Counter)
   {
      pc_WidgetItem = this->mc_DpItems.at(sn_Counter);
      if (pc_WidgetItem != NULL)
      {
         uint32 u32_ChangedDpCounter;

         for (u32_ChangedDpCounter = 0U; u32_ChangedDpCounter < c_ChangedDatapools.size(); ++u32_ChangedDpCounter)
         {
            // Update all relevant Datapools
            if (pc_WidgetItem->GetDatapoolId() == c_ChangedDatapools[u32_ChangedDpCounter])
            {
               // Adapt the conflict of this Datapool
               pc_WidgetItem->SetStateConflict(oq_Active);

               break;
            }
         }

         // ask before to improve performance
         if (pc_WidgetItem->GetStateConflict() == true)
         {
            // Has any Datapool a conflict?
            q_Return = true;
         }
      }
   }

   return q_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Updates the widget for the all Datapools
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDpSelectorListWidget::UpdateDataPools(void)
{
   sintn sn_ItDp;

   for (sn_ItDp = 0; sn_ItDp < this->mc_DpItems.size(); ++sn_ItDp)
   {
      C_SdNdeDpSelectorItemWidget * const pc_WidgetItem = this->mc_DpItems.at(sn_ItDp);

      // Update the widget
      pc_WidgetItem->UpdateData();
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Updates the widget for the actual Datapool
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDpSelectorListWidget::UpdateActualDataPool(void)
{
   C_SdNdeDpSelectorItemWidget * pc_WidgetItem = this->mc_DpItems.at(this->GetCurrentItemIndex());

   if (pc_WidgetItem != NULL)
   {
      const C_PuiSdSharedDatapools & rc_SharedDatapools = C_PuiSdHandler::h_GetInstance()->GetSharedDatapoolsConst();

      C_OSCNodeDataPoolId c_ActiveDatapool;
      std::vector<C_OSCNodeDataPoolId> c_SharedDatapoolGroup;

      // Get the selected Datapool
      c_ActiveDatapool = pc_WidgetItem->GetDatapoolId();

      // Update the selected widget
      pc_WidgetItem->UpdateData();

      // Get the shared Datapools
      if (rc_SharedDatapools.IsSharedAndGetDatapoolGroup(c_ActiveDatapool, c_SharedDatapoolGroup) == true)
      {
         uint32 u32_DpCounter;

         for (u32_DpCounter = 0U; u32_DpCounter < c_SharedDatapoolGroup.size(); ++u32_DpCounter)
         {
            // Only shared Datapools of same node relevant
            if (c_SharedDatapoolGroup[u32_DpCounter].u32_NodeIndex == this->mu32_NodeIndex)
            {
               sintn sn_Counter;

               // Update all shared Datapools too
               for (sn_Counter = 0; sn_Counter < this->mc_DpItems.size(); ++sn_Counter)
               {
                  pc_WidgetItem = this->mc_DpItems.at(sn_Counter);
                  if (pc_WidgetItem != NULL)
                  {
                     // Update all relevant Datapools
                     if (pc_WidgetItem->GetDatapoolId() == c_SharedDatapoolGroup[u32_DpCounter])
                     {
                        // Adapt the conflict of this Datapool
                        pc_WidgetItem->UpdateData();

                        break;
                     }
                  }
               }
            }
         }
      }

      Q_EMIT (this->SigDataPoolChanged());
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Adds a new datapool widget to the list

   \param[in] oru32_DataPoolIndex   Data pool index

   \return
   C_NO_ERR OK
   C_RANGE  Something out of range
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDpSelectorListWidget::m_AddDataPoolWidget(const uint32 ou32_DataPoolIndex)
{
   //lint -e{429}  no memory leak because of the parent of pc_ItemWidget and the Qt memory management
   C_SdNdeDpSelectorItemWidget * const pc_ItemWidget = new C_SdNdeDpSelectorItemWidget(
      this->mq_UsageViewActive, this);
   const sintn sn_Index = this->mc_DpItems.size();
   const sintn sn_Number = sn_Index + 1;

   pc_ItemWidget->SetNumber(sn_Number);
   pc_ItemWidget->SetData(C_OSCNodeDataPoolId(this->mu32_NodeIndex, ou32_DataPoolIndex));
   connect(pc_ItemWidget, &C_SdNdeDpSelectorItemWidget::SigUpdateErrorToolTip, this,
           &C_SdNdeDpSelectorListWidget::m_UpdateItemErrorToolTip);
   connect(pc_ItemWidget, &C_SdNdeDpSelectorItemWidget::SigHideOtherToolTips, this,
           &C_SdNdeDpSelectorListWidget::SigHideOtherToolTips);

   connect(pc_ItemWidget, &C_SdNdeDpSelectorItemWidget::SigClicked,
           this, &C_SdNdeDpSelectorListWidget::m_ItemClicked);
   connect(pc_ItemWidget, &C_SdNdeDpSelectorItemWidget::SigDoubleClicked,
           this, &C_SdNdeDpSelectorListWidget::m_ItemDoubleClicked);
   connect(pc_ItemWidget, &C_SdNdeDpSelectorItemWidget::SigContextMenuRequested,
           this, &C_SdNdeDpSelectorListWidget::m_OnItemCustomContextMenuRequested);
   connect(pc_ItemWidget, &C_SdNdeDpSelectorItemWidget::SigHoverStateChanged,
           this, &C_SdNdeDpSelectorListWidget::SigDataPoolHoverStateChanged);

   this->m_AdaptSize(pc_ItemWidget->size());
   pc_ItemWidget->setVisible(false);

   this->mc_DpItems.push_back(pc_ItemWidget);
} //lint !e429  //no memory leak because of the parent of pc_Item and the Qt memory management

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Updates a datapool widget in the list

   \param[in] oru32_DataPoolIndex        Data pool index
   \param[in] ou32_DataPoolWidgetIndex   Data pool widget index

   \return
   C_NO_ERR OK
   C_RANGE  Something out of range
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDpSelectorListWidget::m_UpdateDataPoolWidget(const uint32 ou32_DataPoolIndex,
                                                         const sintn osn_DataPoolWidgetIndex) const
{
   if (osn_DataPoolWidgetIndex < this->mc_DpItems.size())
   {
      C_SdNdeDpSelectorItemWidget * pc_WidgetItem;

      // update the widget
      pc_WidgetItem = this->mc_DpItems.at(osn_DataPoolWidgetIndex);
      if (pc_WidgetItem != NULL)
      {
         pc_WidgetItem->SetData(C_OSCNodeDataPoolId(this->mu32_NodeIndex, ou32_DataPoolIndex));
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Synchronize the property changes of the Datapool with its shared Datapools

   \param[in]     orc_AdaptedDatapool   Datapool Id of adapted Datapool
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDpSelectorListWidget::m_SynchronizeDatapoolProperties(const C_OSCNodeDataPoolId & orc_AdaptedDatapool)
const
{
   const C_PuiSdSharedDatapools & rc_SharedDatapools = C_PuiSdHandler::h_GetInstance()->GetSharedDatapoolsConst();
   uint32 u32_SharedGroup;
   // Check for shared state again. The relation could be broken
   const bool q_IsShared = rc_SharedDatapools.IsSharedDatapool(orc_AdaptedDatapool, &u32_SharedGroup);

   if (q_IsShared == true)
   {
      std::vector<C_OSCNodeDataPoolId> c_Group;
      const C_OSCNodeDataPool * const pc_AdaptedDatapool = C_PuiSdHandler::h_GetInstance()->GetOSCDataPool(
         orc_AdaptedDatapool.u32_NodeIndex,
         orc_AdaptedDatapool.u32_DataPoolIndex);

      tgl_assert(pc_AdaptedDatapool != NULL);
      if (pc_AdaptedDatapool != NULL)
      {
         uint32 u32_DpIdCounter;
         tgl_assert(rc_SharedDatapools.GetSharedDatapoolGroup(u32_SharedGroup, c_Group) == C_NO_ERR);

         // Synchronize with all registered shared Datapools in the same group
         for (u32_DpIdCounter = 0U; u32_DpIdCounter < c_Group.size(); ++u32_DpIdCounter)
         {
            if (c_Group[u32_DpIdCounter] != orc_AdaptedDatapool)
            {
               C_OSCNode * const pc_SyncNode = C_PuiSdHandler::h_GetInstance()->GetOSCNode(
                  c_Group[u32_DpIdCounter].u32_NodeIndex);

               tgl_assert(pc_SyncNode != NULL);
               if ((pc_SyncNode != NULL) &&
                   (c_Group[u32_DpIdCounter].u32_DataPoolIndex < pc_SyncNode->c_DataPools.size()))
               {
                  C_OSCNodeDataPool & rc_SyncDatapool =
                     pc_SyncNode->c_DataPools[c_Group[u32_DpIdCounter].u32_DataPoolIndex];

                  memcpy(rc_SyncDatapool.au8_Version, pc_AdaptedDatapool->au8_Version, 3);
                  rc_SyncDatapool.u32_NvMSize = pc_AdaptedDatapool->u32_NvMSize;
               }
            }
         }
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Adds a new datapool by opening the datapool properties datapool
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDpSelectorListWidget::AddNewDatapool(void)
{
   const C_OSCNode * const pc_Node = C_PuiSdHandler::h_GetInstance()->GetOSCNodeConst(this->mu32_NodeIndex);

   if ((pc_Node != NULL) && (pc_Node->pc_DeviceDefinition != NULL) &&
       (pc_Node->u32_SubDeviceIndex < pc_Node->pc_DeviceDefinition->c_SubDevices.size()))
   {
      // is enough space available
      if (C_SdUtil::h_CheckDatapoolNumber(this->mu32_NodeIndex, false, this) == true)
      {
         C_OSCNodeDataPool c_NewDatapool;
         C_PuiSdNodeDataPool c_UIDataPool;
         bool q_SharedDatapoolSelected = false;
         C_OSCNodeDataPoolId c_SharedDatapoolId;
         bool q_ShareNewDatapool = false;

         // the type must be initialized
         c_NewDatapool.c_Name = C_PuiSdHandler::h_GetInstance()->GetUniqueDataPoolName(this->mu32_NodeIndex,
                                                                                       c_NewDatapool.c_Name);
         c_NewDatapool.e_Type = this->me_DataPoolType;

         // ask for sharing if possible
         // (only if the Datapool is DIAG or NVM Datapool - no sharing between COMM and HAL Datapools)
         if ((this->me_DataPoolType == C_OSCNodeDataPool::eDIAG) ||
             (this->me_DataPoolType == C_OSCNodeDataPool::eNVM))
         {
            q_ShareNewDatapool =
               this->m_OpenShareDataPoolDialog(c_NewDatapool, q_SharedDatapoolSelected, c_SharedDatapoolId);
         }

         if ((this->me_DataPoolType == C_OSCNodeDataPool::eCOM) ||
             (this->me_DataPoolType == C_OSCNodeDataPool::eHALC) ||
             (this->me_DataPoolType == C_OSCNodeDataPool::eHALC_NVM) ||
             (q_ShareNewDatapool == true))
         {
            const C_OSCNodeDataPoolId * opc_SharedDatapoolId = NULL;

            if (q_SharedDatapoolSelected == true)
            {
               // Datapool should be shared. Synchronize the Datapool before opening the properties dialog
               C_OSCNodeDataPool c_SharedDatapool;
               C_PuiSdNodeDataPool c_SharedUIDataPool;

               const sint32 s32_SharedDpReturn =
                  C_PuiSdHandler::h_GetInstance()->GetDataPool(c_SharedDatapoolId.u32_NodeIndex,
                                                               c_SharedDatapoolId.u32_DataPoolIndex,
                                                               c_SharedDatapool,
                                                               c_SharedUIDataPool);

               tgl_assert(s32_SharedDpReturn == C_NO_ERR);
               if (s32_SharedDpReturn == C_NO_ERR)
               {
                  // Copy all shared items
                  memcpy(c_NewDatapool.au8_Version, c_SharedDatapool.au8_Version, 3);
                  c_NewDatapool.u32_NvMSize = c_SharedDatapool.u32_NvMSize;

                  c_NewDatapool.c_Lists = c_SharedDatapool.c_Lists;
                  c_UIDataPool.c_DataPoolLists = c_SharedUIDataPool.c_DataPoolLists;
               }

               // The dialog needs the DatapoolId of the shared Datapool
               opc_SharedDatapoolId = &c_SharedDatapoolId;
            }

            if ((this->me_DataPoolType == C_OSCNodeDataPool::eNVM) ||
                (this->me_DataPoolType == C_OSCNodeDataPool::eHALC_NVM))
            {
               // Get a potential start address
               uint32 u32_AreaCounter;
               std::vector<C_PuiSdHandler::C_PuiSdHandlerNodeLogicNvmArea> c_NvmAreas;
               tgl_assert(C_PuiSdHandler::h_GetInstance()->GetNodeNvmDataPoolAreas(this->mu32_NodeIndex,
                                                                                   c_NvmAreas) == C_NO_ERR);

               // Search the first free gap
               for (u32_AreaCounter = 0; u32_AreaCounter < c_NvmAreas.size(); ++u32_AreaCounter)
               {
                  const C_PuiSdHandlerNodeLogic::C_PuiSdHandlerNodeLogicNvmArea & rc_Area = c_NvmAreas[u32_AreaCounter];
                  if (rc_Area.c_DataPoolIndexes.size() == 0)
                  {
                     // Gap found
                     c_NewDatapool.u32_NvMStartAddress = rc_Area.u32_StartAddress;
                     break;
                  }
               }
            }

            // Open the Datapool properties dialog for the new Datapool
            if (this->m_OpenDataPoolDialog(c_NewDatapool, c_UIDataPool, opc_SharedDatapoolId,
                                           pc_Node->pc_DeviceDefinition->c_SubDevices[pc_Node->u32_SubDeviceIndex].
                                           q_ProgrammingSupport,
                                           -1) == true)
            {
               // Initialize the Datapool only when not shared
               if (q_SharedDatapoolSelected == false)
               {
                  //Initialize (after type is certain)
                  for (uint32 u32_ItList = 0; u32_ItList < c_NewDatapool.c_Lists.size(); ++u32_ItList)
                  {
                     C_OSCNodeDataPoolList & rc_List = c_NewDatapool.c_Lists[u32_ItList];
                     for (uint32 u32_ItElement = 0; u32_ItElement < rc_List.c_Elements.size(); ++u32_ItElement)
                     {
                        C_PuiSdHandler::h_InitDataElement(c_NewDatapool.e_Type, c_NewDatapool.q_IsSafety,
                                                          rc_List.c_Elements[u32_ItElement]);
                     }

                     // Safety property of eHALC_NVM is not changeable, no handling necessary
                     if ((c_NewDatapool.e_Type == C_OSCNodeDataPool::eNVM) && (c_NewDatapool.q_IsSafety == true))
                     {
                        rc_List.q_NvMCRCActive = true;
                     }
                  }
               }

               //Add step
               this->m_AddNewDataPool(c_NewDatapool, c_UIDataPool, -1, false, !q_SharedDatapoolSelected,
                                      q_SharedDatapoolSelected, &c_SharedDatapoolId);
               Q_EMIT (this->SigDataPoolChanged());
               //Check
               Q_EMIT (this->SigErrorCheck());
            }
         }
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Paste list item
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDpSelectorListWidget::Paste(void)
{
   const uint16 u16_TimerId = osc_write_log_performance_start();
   C_OSCNodeDataPool c_OSCContent;
   C_PuiSdNodeDataPool c_UIContent;
   sint32 s32_Return;

   if (C_SdUtil::h_CheckDatapoolNumber(this->mu32_NodeIndex, false, this) == true)
   {
      QApplication::setOverrideCursor(Qt::WaitCursor);

      // load from "clipboard"
      s32_Return = C_SdClipBoardHelper::h_LoadToDataPool(c_OSCContent, c_UIContent);

      //Check and replace application if necessary
      if (c_OSCContent.s32_RelatedDataBlockIndex >= 0)
      {
         const C_OSCNode * const pc_Node = C_PuiSdHandler::h_GetInstance()->GetOSCNodeConst(this->mu32_NodeIndex);
         if (pc_Node != NULL)
         {
            bool q_ReplaceNecessary = false;
            //Check if current application index is valid
            if (static_cast<uint32>(c_OSCContent.s32_RelatedDataBlockIndex) < pc_Node->c_Applications.size())
            {
               const C_OSCNodeApplication & rc_Application =
                  pc_Node->c_Applications[static_cast<uint32>(c_OSCContent.s32_RelatedDataBlockIndex)];
               if (rc_Application.e_Type != C_OSCNodeApplication::ePROGRAMMABLE_APPLICATION)
               {
                  q_ReplaceNecessary = true;
               }
            }
            else
            {
               q_ReplaceNecessary = true;
            }
            //Replace
            if (q_ReplaceNecessary == true)
            {
               bool q_Found = false;
               for (uint32 u32_ItApp = 0; u32_ItApp < pc_Node->c_Applications.size(); ++u32_ItApp)
               {
                  const C_OSCNodeApplication & rc_Application = pc_Node->c_Applications[u32_ItApp];
                  if (rc_Application.e_Type != C_OSCNodeApplication::ePROGRAMMABLE_APPLICATION)
                  {
                     c_OSCContent.s32_RelatedDataBlockIndex = static_cast<sint32>(u32_ItApp);
                     q_Found = true;
                     break;
                  }
               }
               if (q_Found == false)
               {
                  //Should not really happen
                  c_OSCContent.s32_RelatedDataBlockIndex = -1;
               }
            }
         }
      }

      if (s32_Return == C_NO_ERR)
      {
         if (c_OSCContent.e_Type == this->me_DataPoolType)
         {
            sint32 s32_LastIndex = 0;
            if (this->GetCurrentItemIndex() >= 0)
            {
               //Add after current index
               s32_LastIndex = static_cast<sint32>(this->GetCurrentItemIndex()) + 1;
            }
            else
            {
               //Add at end
               s32_LastIndex = this->mc_DpItems.size();
            }

            this->m_AddNewDataPool(c_OSCContent, c_UIContent, s32_LastIndex, true, false);
            Q_EMIT (this->SigDataPoolChanged());
            //Check error
            Q_EMIT (this->SigErrorCheck());
            QApplication::restoreOverrideCursor();
         }
         else
         {
            C_OgeWiCustomMessage c_MessageBox(this);
            QApplication::restoreOverrideCursor();
            c_MessageBox.SetDescription(C_GtGetText::h_GetText(
                                           "Cannot paste Datapool. Datapool type is not compatible."));
            c_MessageBox.Execute();
         }
      }
      else
      {
         QApplication::restoreOverrideCursor();
      }
   }
   osc_write_log_performance_stop(u16_TimerId, "Paste datapool");
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Checks all datapools for conflicts and sets the icon

   \param[out]  opc_InvalidDatapoolIndices    Invalid Datapool indices (optional)

   \return
   true   Conflict found
   false  No conflict found
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_SdNdeDpSelectorListWidget::CheckDataPoolsForConflict(std::vector<uint32> * const opc_InvalidDatapoolIndices)
const
{
   const C_OSCNode * const pc_Node = C_PuiSdHandler::h_GetInstance()->GetOSCNodeConst(this->mu32_NodeIndex);
   bool q_Return = false;

   if (pc_Node != NULL)
   {
      uint32 u32_DpCounter;
      sintn sn_TypeCounter = 0;

      // check all Datapools of the lists
      for (u32_DpCounter = 0; u32_DpCounter < pc_Node->c_DataPools.size(); ++u32_DpCounter)
      {
         if (pc_Node->c_DataPools[u32_DpCounter].e_Type == this->me_DataPoolType)
         {
            if (sn_TypeCounter <= this->mc_DpItems.size())
            {
               // conflict detected, update the widget
               C_SdNdeDpSelectorItemWidget * const pc_WidgetItem  = this->mc_DpItems.at(sn_TypeCounter);

               // update the widget
               if (pc_WidgetItem != NULL)
               {
                  bool q_NameConflict;
                  bool q_NameInvalid;
                  bool q_IsErrorInListOrMessage;
                  bool q_NumberOfListsOrElementsInvalid;
                  //Check error for one datapool
                  pc_Node->CheckErrorDataPool(u32_DpCounter, &q_NameConflict, &q_NameInvalid, &q_IsErrorInListOrMessage,
                                              &q_NumberOfListsOrElementsInvalid, NULL);

                  if (((q_NameConflict == false) && (q_NameInvalid == false)) && (q_IsErrorInListOrMessage == false) &&
                      (q_NumberOfListsOrElementsInvalid == false))
                  {
                     //No error
                     pc_WidgetItem->SetStateConflict(false);
                  }
                  else
                  {
                     //Error
                     pc_WidgetItem->SetStateConflict(true);
                     //Mark at least one error
                     q_Return = true;
                     //Add to invalid datapools list
                     if (opc_InvalidDatapoolIndices != NULL)
                     {
                        opc_InvalidDatapoolIndices->push_back(u32_DpCounter);
                     }
                  }
               }
            }

            ++sn_TypeCounter;
         }
      }
   }

   return q_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Sets a new height for the size hint

   \param[in]       osn_MaxHeight     Possible maximum height
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDpSelectorListWidget::UpdateSizeHint(const sintn osn_MaxHeight)
{
   const sintn sn_CountRowPerView = osn_MaxHeight / this->msn_ItemHeight;

   this->msn_HeightHint = sn_CountRowPerView * this->msn_ItemHeight;

   this->m_UpdateCounters();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get size hint (handled expanded or not expanded size)

   \return
   Size hint (handled expanded or not expanded size)
*/
//----------------------------------------------------------------------------------------------------------------------
QSize C_SdNdeDpSelectorListWidget::sizeHint(void) const
{
   QSize c_Size = QWidget::sizeHint();

   c_Size.setHeight(this->msn_HeightHint);
   return c_Size;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Returns the number of Datapool items

   \return
   Number of Datapool items
*/
//----------------------------------------------------------------------------------------------------------------------
sintn C_SdNdeDpSelectorListWidget::GetItemCount(void) const
{
   return this->mc_DpItems.size();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Returns the index of the current item

   \return
   Index of current index
*/
//----------------------------------------------------------------------------------------------------------------------
sintn C_SdNdeDpSelectorListWidget::GetCurrentItemIndex(void) const
{
   return this->msn_SelectedItemIndex;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Returns the number of view pages

   \return
   Number of view pages
*/
//----------------------------------------------------------------------------------------------------------------------
sintn C_SdNdeDpSelectorListWidget::GetViewPageCount(void) const
{
   return this->msn_CountViewPages;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Returns the number of the current view page

   \return
   Number of current view page
*/
//----------------------------------------------------------------------------------------------------------------------
sintn C_SdNdeDpSelectorListWidget::GetCurrentViewPage(void) const
{
   return this->msn_CurrentViewPage;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Sets the selected Datapool

   \param[in]       ou32_Index     Index of Datapool item
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDpSelectorListWidget::SetSelectedItem(const uint32 ou32_Index)
{
   if ((static_cast<sintn>(ou32_Index) < this->mc_DpItems.size()) &&
       (static_cast<sintn>(ou32_Index) != this->msn_SelectedItemIndex))
   {
      this->msn_SelectedItemIndex = static_cast<sintn>(ou32_Index);
      this->m_SelectionChanged();
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Sets the the new row number

   The native row of the list is each item.

   \param[in]  osn_ViewIndex       New view index
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDpSelectorListWidget::SetCurrentViewPage(const sintn osn_ViewIndex)
{
   if ((osn_ViewIndex < this->msn_CountViewPages) &&
       (osn_ViewIndex != this->msn_CountViewPages))
   {
      sintn sn_ItDp;
      const sintn sn_FirstVisibleRow = osn_ViewIndex * this->msn_RowsPerViewPage;
      const sintn sn_LastVisibleRow = ((osn_ViewIndex + 1) * this->msn_RowsPerViewPage) - 1;

      for (sn_ItDp = 0; sn_ItDp < this->mc_DpItems.size(); ++sn_ItDp)
      {
         const sintn sn_Row = sn_ItDp / this->msn_ItemsPerRow;
         const bool q_RowVisible = ((sn_Row >= sn_FirstVisibleRow) && (sn_Row <= sn_LastVisibleRow)) ? true : false;

         this->mc_DpItems.at(sn_ItDp)->setVisible(q_RowVisible);
      }

      this->msn_CurrentViewPage = osn_ViewIndex;

      Q_EMIT (this->SigListChanged());
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Sets the line to the position of the item

   \param[in]  ou32_Index   Item index
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDpSelectorListWidget::ScrollToItem(const uint32 ou32_Index)
{
   sintn sn_ViewIndex;

   if (this->msn_ItemsPerViewPage > 0)
   {
      sn_ViewIndex = static_cast<sintn>(ou32_Index) / this->msn_ItemsPerViewPage;
   }
   else
   {
      sn_ViewIndex = 0;
   }

   this->SetCurrentViewPage(sn_ViewIndex);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Overridden paint event

   Draws the background element

   \param[in,out] opc_Event  Pointer to paint event
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDpSelectorListWidget::paintEvent(QPaintEvent * const opc_Event)
{
   QWidget::paintEvent(opc_Event);

   if (this->mc_DpItems.size() == 0)
   {
      QPainter c_Painter(this);
      // show a tip for adding a datapool
      QPen c_Pen = c_Painter.pen();
      QFont c_Font;
      QString c_Text;

      if ((this->me_DataPoolType != C_OSCNodeDataPool::eHALC) &&
          (this->me_DataPoolType != C_OSCNodeDataPool::eHALC_NVM))
      {
         c_Text = static_cast<QString>(C_GtGetText::h_GetText("No")) + static_cast<QString>(" ") +
                  C_PuiSdUtil::h_ConvertDataPoolTypeToString(this->me_DataPoolType) +
                  static_cast<QString>(C_GtGetText::h_GetText(
                                          " Datapool is declared.\nAdd any via the '+' button."));
      }
      else
      {
         c_Text = static_cast<QString>(C_GtGetText::h_GetText("No")) + static_cast<QString>(" ") +
                  C_PuiSdUtil::h_ConvertDataPoolTypeToString(this->me_DataPoolType) +
                  static_cast<QString>(C_GtGetText::h_GetText(
                                          " Datapool is declared. \nHardware configuration is handled in tab 'Hardware Configurator'."));
      }

      // configure color
      c_Pen.setColor(mc_STYLE_GUIDE_COLOR_8);
      c_Painter.setPen(c_Pen);

      // configure font
      c_Font = mc_STYLE_GUIDE_FONT_REGULAR_13;
      c_Font.setPixelSize(c_Font.pointSize());
      c_Painter.setFont(c_Font);

      c_Painter.drawText(this->rect(), static_cast<sintn>(Qt::AlignCenter), c_Text);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Overwritten resize event

   Updates the information about the count of columns and items per row

   \param[in,out] opc_Event Event identification and information
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDpSelectorListWidget::resizeEvent(QResizeEvent * const opc_Event)
{
   QWidget::resizeEvent(opc_Event);

   this->m_UpdateCounters();

   if (this->GetCurrentItemIndex() >= 0)
   {
      this->ScrollToItem(static_cast<uint32>(this->GetCurrentItemIndex()));
   }

   Q_EMIT (this->SigListChanged());
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Overwritten key press event slot

   Here: handle list actions

   \param[in,out] opc_Event Event identification and information
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDpSelectorListWidget::keyPressEvent(QKeyEvent * const opc_Event)
{
   bool q_CallOrig = true;

   if ((this->me_DataPoolType != stw_opensyde_core::C_OSCNodeDataPool::eHALC) &&
       (this->me_DataPoolType != stw_opensyde_core::C_OSCNodeDataPool::eHALC_NVM))
   {
      const sintn sn_CurrentItemIndex = this->GetCurrentItemIndex();
      if (C_Uti::h_CheckKeyModifier(opc_Event->modifiers(), Qt::ControlModifier) == true)
      {
         switch (opc_Event->key())
         {
         case Qt::Key_C:
            m_Copy();
            break;
         case Qt::Key_X:
            m_Cut();
            break;
         case Qt::Key_Left:
            q_CallOrig = false;
            m_MoveLeft();
            break;
         case Qt::Key_Right:
            q_CallOrig = false;
            m_MoveRight();
            break;
         case Qt::Key_Up:
            q_CallOrig = false;
            m_MoveUp();
            break;
         case Qt::Key_Down:
            q_CallOrig = false;
            m_MoveDown();
            break;
         default:
            //Nothing to do
            break;
         }
      }
      else
      {
         switch (opc_Event->key())
         {
         case Qt::Key_Delete:
            m_Delete(true);
            break;
         case Qt::Key_Left:
            if (sn_CurrentItemIndex > 0)
            {
               const sintn sn_NewIndex = sn_CurrentItemIndex - 1;
               this->ScrollToItem(sn_NewIndex);
               this->SetSelectedItem(sn_NewIndex);
               q_CallOrig = false;
            }
            break;
         case Qt::Key_Right:
            if (sn_CurrentItemIndex < (this->mc_DpItems.size() - 1))
            {
               const sintn sn_NewIndex = sn_CurrentItemIndex + 1;
               this->ScrollToItem(sn_NewIndex);
               this->SetSelectedItem(sn_NewIndex);
               q_CallOrig = false;
            }
            break;
         case Qt::Key_Up:
            if (sn_CurrentItemIndex >= this->msn_ItemsPerRow)
            {
               const sintn sn_NewIndex = sn_CurrentItemIndex - this->msn_ItemsPerRow;
               this->ScrollToItem(sn_NewIndex);
               this->SetSelectedItem(sn_NewIndex);
               q_CallOrig = false;
            }
            break;
         case Qt::Key_Down:
            if (sn_CurrentItemIndex < (this->mc_DpItems.size() - this->msn_ItemsPerRow))
            {
               const sintn sn_NewIndex = sn_CurrentItemIndex + this->msn_ItemsPerRow;
               this->ScrollToItem(sn_NewIndex);
               this->SetSelectedItem(sn_NewIndex);
               q_CallOrig = false;
            }
            break;
         case Qt::Key_F2:
            this->m_Edit(true);
            q_CallOrig = false;
            break;
         default:
            //Nothing to do
            break;
         }
      }
   }

   if (q_CallOrig == true)
   {
      QWidget::keyPressEvent(opc_Event);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Overridden focus in event

   \param[in,out] opc_Event  Pointer to paint event
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDpSelectorListWidget::focusInEvent(QFocusEvent * const opc_Event)
{
   QWidget::focusInEvent(opc_Event);

   this->SetSelected(true);

   if ((this->GetCurrentItemIndex() < 0) &&
       (this->mc_DpItems.size() > 0))
   {
      this->SetSelectedItem(0);
   }

   Q_EMIT (this->SigWidgetFocused());
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Overwritten resize event

   Updates the information about the count of columns and items per row

   \param[in,out]  opc_Event  Event identification and information
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDpSelectorListWidget::wheelEvent(QWheelEvent * const opc_Event)
{
   QWidget::wheelEvent(opc_Event);

   if ((opc_Event->angleDelta().y() > 0) &&
       (this->GetCurrentViewPage() > 0))
   {
      // wheel up
      this->SetCurrentViewPage(this->GetCurrentViewPage() - 1);
   }
   else if ((opc_Event->angleDelta().y() < 0) &&
            (this->GetCurrentViewPage() < (this->GetViewPageCount() - 1)))
   {
      // wheel down
      this->SetCurrentViewPage(this->GetCurrentViewPage() + 1);
   }
   else
   {
      // nothing todo
   }
}

//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDpSelectorListWidget::m_MoveItem(const sintn osn_SourceIndex, const sintn osn_TargetIndex)
{
   this->m_MoveDatapool(osn_SourceIndex, osn_TargetIndex);
}

//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDpSelectorListWidget::m_UpdateNumbers(void) const
{
   sintn sn_Counter;

   // Update all visible numbers of the concrete widgets
   for (sn_Counter = 0; sn_Counter < this->mc_DpItems.size(); ++sn_Counter)
   {
      C_SdNdeDpSelectorItemWidget * const pc_WidgetItem = this->mc_DpItems.at(sn_Counter);
      if (pc_WidgetItem != NULL)
      {
         pc_WidgetItem->SetNumber(static_cast<uint32>(sn_Counter + 1));
      }
   }
}

// TODO BAY:
////----------------------------------------------------------------------------------------------------------------------
//void C_SdNdeDpSelectorListWidget::m_DelegateStartPaint(void)
//{
//
//   C_SdNdeDpSelectorItemWidget * const pc_ItemWidget =
//      dynamic_cast<C_SdNdeDpSelectorItemWidget *>(this->itemWidget(this->item(this->currentIndex().row())));
//
//   this->mc_Delegate.StartPaint(this->currentIndex().row(), pc_ItemWidget);
//}
//
////----------------------------------------------------------------------------------------------------------------------
//void C_SdNdeDpSelectorListWidget::m_DelegateStopPaint(void)
//{
//   this->mc_Delegate.StopPaint();
//}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Opens the add datapool dialog for configuring the shared datapool configuration

   Checking if it is necessary to open the dialog first. If no other Datapools exist, sharing is not possible

   \param[in,out] orc_OSCDataPool             Reference to the actual core datapool object
   \param[out]    orq_SharedDatapoolSelected  Flag if datapool was selected. Only if true oru32_SharedNodeIndex and
                                              oru32_SharedDatapoolIndex are valid
   \param[out]    orc_SharedDatapoolId        Datapool ID of selected shared datapool partner to the new datapool

   \return
   User accepted dialog or dialog was not necessary
   User aborted dialog
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_SdNdeDpSelectorListWidget::m_OpenShareDataPoolDialog(C_OSCNodeDataPool & orc_OSCDataPool,
                                                            bool & orq_SharedDatapoolSelected,
                                                            C_OSCNodeDataPoolId & orc_SharedDatapoolId)
{
   bool q_Return = false;
   bool q_DatapoolFound = false;
   uint32 u32_NodeIndex;

   // Search for Datapools of the same type
   for (u32_NodeIndex = 0U; u32_NodeIndex < C_PuiSdHandler::h_GetInstance()->GetOSCNodesSize(); ++u32_NodeIndex)
   {
      const C_OSCNode * const pc_Node = C_PuiSdHandler::h_GetInstance()->GetOSCNodeConst(u32_NodeIndex);

      if (pc_Node != NULL)
      {
         uint32 u32_DpCounter;

         for (u32_DpCounter = 0U; u32_DpCounter < pc_Node->c_DataPools.size(); ++u32_DpCounter)
         {
            if (pc_Node->c_DataPools[u32_DpCounter].e_Type == this->me_DataPoolType)
            {
               // At least one Datapool of the same type exists
               q_DatapoolFound = true;
               break;
            }
         }
      }

      if (q_DatapoolFound == true)
      {
         break;
      }
   }

   if (q_DatapoolFound == true)
   {
      // At least one Datapool of the same type exists, show the dialog
      const QPointer<C_OgePopUpDialog> c_New = new C_OgePopUpDialog(this, this);
      C_SdNdeDpSelectorAddWidget * const pc_Dialog = new C_SdNdeDpSelectorAddWidget(*c_New,
                                                                                    this->mu32_NodeIndex,
                                                                                    orc_OSCDataPool);

      Q_UNUSED(pc_Dialog)

      if (c_New->exec() == static_cast<sintn>(QDialog::Accepted))
      {
         q_Return = true;
         orq_SharedDatapoolSelected =
            pc_Dialog->GetSelectedSharedDatapool(orc_SharedDatapoolId);
      }
      else
      {
         orq_SharedDatapoolSelected = false;
      }

      c_New->HideOverlay();
      c_New->deleteLater();
   } //lint !e429  //no memory leak because of the parent of pc_Dialog and the Qt memory management
   else
   {
      q_Return = true;
      orq_SharedDatapoolSelected = false;
   }

   return q_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Opens the Datapool properties dialog

   \param[in,out] orc_OSCDataPool             Reference to the actual core Datapool object
   \param[in,out] orc_UiDataPool              Reference to the actual ui datapool object
   \param[in]     opc_SharedDatapoolId        In case of a new shared Datapool, the Id is the shared Datapool of the new
                                              Datapool. In case of an edited or stand alone Datapool the pointer is NULL
   \param[in]     oq_NodeProgrammingSupport   Flag if node has programming support
   \param[in]     os32_DataPoolIndex          Flag for new Datapool (-1 is new Datapool, >= 0 is existing Datapool)
   \param[in]     oq_SelectName               Selects the Datapool name for instant editing

   \return
   User accepted dialog
   User aborted dialog
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_SdNdeDpSelectorListWidget::m_OpenDataPoolDialog(C_OSCNodeDataPool & orc_OSCDataPool,
                                                       C_PuiSdNodeDataPool & orc_UiDataPool,
                                                       const C_OSCNodeDataPoolId * const opc_SharedDatapoolId,
                                                       const bool oq_NodeProgrammingSupport,
                                                       const sint32 os32_DataPoolIndex, const bool oq_SelectName)
{
   bool q_Return = false;

   const QPointer<C_OgePopUpDialog> c_New = new C_OgePopUpDialog(this, this);
   C_SdNdeDpProperties * const pc_Dialog =
      new C_SdNdeDpProperties(*c_New, &orc_OSCDataPool, &orc_UiDataPool, &this->me_ProtocolType,
                              os32_DataPoolIndex, this->mu32_NodeIndex, oq_SelectName, oq_NodeProgrammingSupport,
                              opc_SharedDatapoolId);

   Q_UNUSED(pc_Dialog)

   if (c_New->exec() == static_cast<sintn>(QDialog::Accepted))
   {
      q_Return = true;
   }

   if (c_New != NULL)
   {
      c_New->HideOverlay();
      c_New->deleteLater();
   }

   return q_Return; //lint !e429  //no memory leak because of the parent of pc_Dialog and the Qt memory management
}

//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDpSelectorListWidget::m_AddNewDataPool(const C_OSCNodeDataPool & orc_OSCDataPool,
                                                   const C_PuiSdNodeDataPool & orc_UIDataPool,
                                                   const sint32 os32_DataPoolIndex, const bool oq_AllowNameAdaptation,
                                                   const bool oq_AllowDataAdaptation,
                                                   const bool oq_SharedDatapoolSelected,
                                                   const C_OSCNodeDataPoolId * const opc_SharedDatapoolId)
{
   sint32 s32_Return;
   sintn sn_Row = os32_DataPoolIndex;
   sintn sn_RealIndex;

   QApplication::setOverrideCursor(Qt::WaitCursor);

   if (os32_DataPoolIndex == -1)
   {
      s32_Return = C_PuiSdHandler::h_GetInstance()->AddDataPool(this->mu32_NodeIndex, orc_OSCDataPool, orc_UIDataPool,
                                                                this->me_ProtocolType, oq_AllowNameAdaptation,
                                                                oq_AllowDataAdaptation);
      // the new datapool will be at the end
      sn_Row = this->mc_DpItems.size();

      if ((s32_Return == C_NO_ERR) &&
          (oq_SharedDatapoolSelected == true) &&
          (opc_SharedDatapoolId != NULL))
      {
         // New datapool is shared. Register it
         C_PuiSdSharedDatapools & rc_SharedDatapools = C_PuiSdHandler::h_GetInstance()->GetSharedDatapools();
         C_OSCNodeDataPoolId c_NewDatapoolId;

         sn_RealIndex = C_PuiSdHandler::h_GetInstance()->GetDataPoolIndex(this->mu32_NodeIndex,
                                                                          this->me_DataPoolType,
                                                                          sn_Row);
         tgl_assert(sn_RealIndex >= 0);

         c_NewDatapoolId.u32_NodeIndex = this->mu32_NodeIndex;
         // Getting the real index of the new datapool
         c_NewDatapoolId.u32_DataPoolIndex = static_cast<uint32>(sn_RealIndex);

         rc_SharedDatapools.AddSharedDatapool(c_NewDatapoolId, *opc_SharedDatapoolId);

         // Version and Datapool size in case of a NVM Datapool can be changed in the properties dialog
         this->m_SynchronizeDatapoolProperties(c_NewDatapoolId);
      }
   }
   else if (os32_DataPoolIndex >= 0)
   {
      sn_RealIndex = C_PuiSdHandler::h_GetInstance()->GetDataPoolIndex(this->mu32_NodeIndex, this->me_DataPoolType,
                                                                       os32_DataPoolIndex);
      if (sn_RealIndex >= 0)
      {
         s32_Return = C_PuiSdHandler::h_GetInstance()->InsertDataPool(this->mu32_NodeIndex,
                                                                      static_cast<uint32>(sn_RealIndex),
                                                                      orc_OSCDataPool, orc_UIDataPool,
                                                                      this->me_ProtocolType, oq_AllowNameAdaptation,
                                                                      oq_AllowDataAdaptation);
      }
      else
      {
         s32_Return = C_PuiSdHandler::h_GetInstance()->AddDataPool(this->mu32_NodeIndex, orc_OSCDataPool,
                                                                   orc_UIDataPool,
                                                                   this->me_ProtocolType, oq_AllowNameAdaptation,
                                                                   oq_AllowDataAdaptation);
         // the new datapool will be at the end
         sn_Row = this->mc_DpItems.size();
      }
   }
   else
   {
      s32_Return = C_RANGE;
   }

   // update the UI
   if (s32_Return == C_NO_ERR)
   {
      this->m_InitFromData();
      // scroll to the position and set the item active
      this->SetSelectedItem(sn_Row);
      this->ScrollToItem(sn_Row);

      Q_EMIT (this->SigListChanged());
   }
   QApplication::restoreOverrideCursor();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Show custom context menu

   \param[in] orc_Pos Local context menu position
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDpSelectorListWidget::m_OnCustomContextMenuRequested(const QPoint & orc_Pos)
{
   this->m_OpenCustomContextMenu(orc_Pos, NULL);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Show custom context menu

   \param[in] orc_Pos Local context menu position
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDpSelectorListWidget::m_OnItemCustomContextMenuRequested(const QPoint & orc_Pos,
                                                                     const stw_types::uint32 ou32_Index)
{
   if (static_cast<sintn>(ou32_Index) < this->mc_DpItems.size())
   {
      this->SetSelectedItem(ou32_Index);
      this->m_OpenCustomContextMenu(orc_Pos, this->mc_DpItems.at(ou32_Index));
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Show custom context menu

   \param[in] orc_Pos   Local context menu position
   \param[in] opc_Item  Pointer to clicked Datapool item. NULL if no specific item was clicked
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDpSelectorListWidget::m_OpenCustomContextMenu(const QPoint & orc_Pos,
                                                          const C_SdNdeDpSelectorItemWidget * const opc_Item)
{
   bool q_ShowContextMenu = true;

   // add action shall be shown only if no item concrete was clicked
   if (opc_Item != NULL)
   {
      this->mpc_AddAction->setVisible(false);

      this->mpc_EditAction->setVisible(true);
      this->mpc_EditContentAction->setVisible(true);
      this->mpc_EditActionSeparator->setVisible(true);
      this->mpc_DeleteAction->setVisible(true);
      this->mpc_DeleteActionSeparator->setVisible(true);
      this->mpc_CopyAction->setVisible(true);
      this->mpc_CutAction->setVisible(true);
      this->mpc_PasteAction->setVisible(true);
      this->mpc_MoveLeftAction->setVisible(true);
      this->mpc_MoveRightAction->setVisible(true);
      this->mpc_MoveUpAction->setVisible(true);
      this->mpc_MoveDownAction->setVisible(true);
      this->mpc_MoveActionSeparator->setVisible(true);

      // actions depends on count of Datapools
      if ((this->mc_DpItems.size() > 1) &&
          (this->me_DataPoolType != C_OSCNodeDataPool::eHALC) &&
          (this->me_DataPoolType != C_OSCNodeDataPool::eHALC_NVM))
      {
         this->mpc_MoveLeftAction->setEnabled(this->GetCurrentItemIndex() > 0);
         this->mpc_MoveRightAction->setEnabled(this->GetCurrentItemIndex() < (this->mc_DpItems.size() - 1));
         this->mpc_MoveUpAction->setEnabled(this->GetCurrentItemIndex() >= this->msn_ItemsPerRow);
         this->mpc_MoveDownAction->setEnabled(this->GetCurrentItemIndex() <
                                              (this->mc_DpItems.size() - this->msn_ItemsPerRow));
         this->mpc_MoveActionSeparator->setEnabled(true);
      }
      else
      {
         this->mpc_MoveLeftAction->setEnabled(false);
         this->mpc_MoveRightAction->setEnabled(false);
         this->mpc_MoveUpAction->setEnabled(false);
         this->mpc_MoveDownAction->setEnabled(false);
         this->mpc_MoveActionSeparator->setEnabled(false);
      }

      // actions depend on Datapool type
      if ((this->me_DataPoolType == C_OSCNodeDataPool::eCOM) ||
          (this->me_DataPoolType == C_OSCNodeDataPool::eHALC) ||
          (this->me_DataPoolType == C_OSCNodeDataPool::eHALC_NVM))
      {
         this->mpc_CopyAction->setEnabled(false);
         this->mpc_CutAction->setEnabled(false);
         this->mpc_PasteAction->setEnabled(false);
      }
      else
      {
         this->mpc_CopyAction->setEnabled(true);
         this->mpc_CutAction->setEnabled(true);
         this->mpc_PasteAction->setEnabled(true);
      }

      if ((this->me_DataPoolType == C_OSCNodeDataPool::eHALC) ||
          (this->me_DataPoolType == C_OSCNodeDataPool::eHALC_NVM))
      {
         this->mpc_DeleteAction->setEnabled(false);
      }
      else
      {
         this->mpc_DeleteAction->setEnabled(true);
      }
   }
   else
   {
      if ((this->me_DataPoolType == C_OSCNodeDataPool::eHALC) ||
          (this->me_DataPoolType == C_OSCNodeDataPool::eHALC_NVM))
      {
         // For HAL Datapools no context menu available when no concrete Datapool was selected
         q_ShowContextMenu = false;
      }
      else
      {
         this->mpc_AddAction->setVisible(true);
         this->mpc_EditActionSeparator->setVisible(true);
         this->mpc_PasteAction->setVisible(true);

         this->mpc_EditAction->setVisible(false);
         this->mpc_EditContentAction->setVisible(false);
         this->mpc_DeleteAction->setVisible(false);
         this->mpc_DeleteActionSeparator->setVisible(false);
         this->mpc_CopyAction->setVisible(false);
         this->mpc_CutAction->setVisible(false);
         this->mpc_MoveLeftAction->setVisible(false);
         this->mpc_MoveRightAction->setVisible(false);
         this->mpc_MoveUpAction->setVisible(false);
         this->mpc_MoveDownAction->setVisible(false);
         this->mpc_MoveActionSeparator->setVisible(false);
      }
   }

   if (q_ShowContextMenu == true)
   {
      this->mpc_ContextMenu->popup(this->mapToGlobal(orc_Pos));
   }
}

//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDpSelectorListWidget::m_SetupContextMenu(void)
{
   this->mpc_ContextMenu = new stw_opensyde_gui_elements::C_OgeContextMenu(this);

   this->mpc_AddAction = this->mpc_ContextMenu->addAction(C_GtGetText::h_GetText(
                                                             "Add new Datapool"), this,
                                                          &C_SdNdeDpSelectorListWidget::AddNewDatapool,
                                                          static_cast<sintn>(Qt::CTRL) +
                                                          static_cast<sintn>(Qt::Key_Plus));

   this->mpc_EditContentAction = this->mpc_ContextMenu->addAction(C_GtGetText::h_GetText(
                                                                     "Edit Content"), this,
                                                                  &C_SdNdeDpSelectorListWidget::m_EditContent);
   this->mpc_EditAction = this->mpc_ContextMenu->addAction(C_GtGetText::h_GetText(
                                                              "Edit Properties"), this,
                                                           &C_SdNdeDpSelectorListWidget::m_Edit);

   this->mpc_EditActionSeparator = this->mpc_ContextMenu->addSeparator();

   this->mpc_CutAction = this->mpc_ContextMenu->addAction(C_GtGetText::h_GetText(
                                                             "Cut"), this, &C_SdNdeDpSelectorListWidget::m_Cut,
                                                          static_cast<sintn>(Qt::CTRL) + static_cast<sintn>(Qt::Key_X));
   this->mpc_CopyAction = this->mpc_ContextMenu->addAction(C_GtGetText::h_GetText(
                                                              "Copy"), this, &C_SdNdeDpSelectorListWidget::m_Copy,
                                                           static_cast<sintn>(Qt::CTRL) +
                                                           static_cast<sintn>(Qt::Key_C));
   this->mpc_PasteAction = this->mpc_ContextMenu->addAction(C_GtGetText::h_GetText(
                                                               "Paste"), this, &C_SdNdeDpSelectorListWidget::Paste,
                                                            static_cast<sintn>(Qt::CTRL) +
                                                            static_cast<sintn>(Qt::Key_V));

   this->mpc_MoveActionSeparator = this->mpc_ContextMenu->addSeparator();

   this->mpc_MoveLeftAction = this->mpc_ContextMenu->addAction(C_GtGetText::h_GetText(
                                                                  "Move Left"), this,
                                                               &C_SdNdeDpSelectorListWidget::m_MoveLeft,
                                                               static_cast<sintn>(Qt::CTRL) +
                                                               static_cast<sintn>(Qt::Key_Left));
   this->mpc_MoveRightAction = this->mpc_ContextMenu->addAction(C_GtGetText::h_GetText(
                                                                   "Move Right"), this,
                                                                &C_SdNdeDpSelectorListWidget::m_MoveRight,
                                                                static_cast<sintn>(Qt::CTRL) +
                                                                static_cast<sintn>(Qt::Key_Right));
   this->mpc_MoveUpAction = this->mpc_ContextMenu->addAction(C_GtGetText::h_GetText(
                                                                "Move Up"), this,
                                                             &C_SdNdeDpSelectorListWidget::m_MoveUp,
                                                             static_cast<sintn>(Qt::CTRL) +
                                                             static_cast<sintn>(Qt::Key_Up));
   this->mpc_MoveDownAction = this->mpc_ContextMenu->addAction(C_GtGetText::h_GetText(
                                                                  "Move Down"), this,
                                                               &C_SdNdeDpSelectorListWidget::m_MoveDown,
                                                               static_cast<sintn>(Qt::CTRL) +
                                                               static_cast<sintn>(Qt::Key_Down));

   this->mpc_DeleteActionSeparator = this->mpc_ContextMenu->addSeparator();

   this->mpc_DeleteAction = this->mpc_ContextMenu->addAction(C_GtGetText::h_GetText(
                                                                "Delete"), this, &C_SdNdeDpSelectorListWidget::m_DeleteSlot,
                                                             static_cast<sintn>(Qt::Key_Delete));

   this->setContextMenuPolicy(Qt::CustomContextMenu);

   connect(this, &C_SdNdeDpSelectorListWidget::customContextMenuRequested, this,
           &C_SdNdeDpSelectorListWidget::m_OnCustomContextMenuRequested);
}

//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDpSelectorListWidget::m_Edit(const bool oq_SelectName)
{
   const C_OSCNode * const pc_Node = C_PuiSdHandler::h_GetInstance()->GetOSCNodeConst(this->mu32_NodeIndex);

   if ((pc_Node != NULL) && (pc_Node->pc_DeviceDefinition != NULL))
   {
      C_OSCNodeDataPool c_OSCDatapool;
      C_PuiSdNodeDataPool c_UIDataPool;

      // get the real datapool index
      const sint32 s32_DpIndex = C_PuiSdHandler::h_GetInstance()->GetDataPoolIndex(this->mu32_NodeIndex,
                                                                                   this->me_DataPoolType,
                                                                                   this->GetCurrentItemIndex());

      if (s32_DpIndex >= 0)
      {
         // fill the structure
         if (C_PuiSdHandler::h_GetInstance()->GetDataPool(this->mu32_NodeIndex, s32_DpIndex,
                                                          c_OSCDatapool, c_UIDataPool) == C_NO_ERR)
         {
            uint32 u32_SharedDatapoolGroup;
            const C_OSCNodeDataPoolId c_EditDatapoolId(this->mu32_NodeIndex,
                                                       static_cast<uint32>(s32_DpIndex));
            const C_PuiSdSharedDatapools & rc_SharedDatapools =
               C_PuiSdHandler::h_GetInstance()->GetSharedDatapoolsConst();
            // Check the share state before opening the property dialog. In case of a change of shared values
            // and in case of a break of the relation, the other Datapools must be refreshed in the list.
            const bool q_IsDatapoolShared =
               rc_SharedDatapools.IsSharedDatapool(c_EditDatapoolId, &u32_SharedDatapoolGroup);

            // open the dialog
            if (this->m_OpenDataPoolDialog(c_OSCDatapool, c_UIDataPool, NULL,
                                           pc_Node->pc_DeviceDefinition->c_SubDevices[pc_Node->u32_SubDeviceIndex].
                                           q_ProgrammingSupport, s32_DpIndex,
                                           oq_SelectName) == true)
            {
               // Update crc active if safety
               // Safety property of eHALC_NVM is not changeable, no handling necessary
               if ((c_OSCDatapool.e_Type == C_OSCNodeDataPool::eNVM) && (c_OSCDatapool.q_IsSafety == true))
               {
                  for (uint32 u32_ItList = 0; u32_ItList < c_OSCDatapool.c_Lists.size(); ++u32_ItList)
                  {
                     C_OSCNodeDataPoolList & rc_List = c_OSCDatapool.c_Lists[u32_ItList];
                     rc_List.q_NvMCRCActive = true;
                  }
               }
               // save the changes
               if (C_PuiSdHandler::h_GetInstance()->SetDataPool(this->mu32_NodeIndex, s32_DpIndex,
                                                                c_OSCDatapool, c_UIDataPool, true,
                                                                this->me_ProtocolType) == C_NO_ERR)
               {
                  this->m_UpdateDataPoolWidget(s32_DpIndex, this->GetCurrentItemIndex());
                  Q_EMIT (this->SigDataPoolChanged());
                  //Update may be necessary for CRC option
                  // eHALC_NVM has no editable lists
                  if (c_OSCDatapool.e_Type == C_OSCNodeDataPool::eNVM)
                  {
                     Q_EMIT (this->SigUpdateLists(this->mu32_NodeIndex, s32_DpIndex));
                  }

                  //Check
                  Q_EMIT (this->SigErrorCheck());
               }

               if (q_IsDatapoolShared == true)
               {
                  this->m_SynchronizeDatapoolProperties(c_EditDatapoolId);
               }
            }

            // Update the other Datapool items of this node which are or were shared Datapools.
            // When the relation was braked, the former shared Datapools must be refreshed.
            // Update even the dialog was canceled. The user could have braked the relation and canceled the dialog.
            if (q_IsDatapoolShared == true)
            {
               sintn sn_WidgetCounter;

               for (sn_WidgetCounter = 0U; sn_WidgetCounter < this->mc_DpItems.size(); ++sn_WidgetCounter)
               {
                  // update all widgets in this case
                  C_SdNdeDpSelectorItemWidget * const pc_WidgetItem = this->mc_DpItems.at(sn_WidgetCounter);

                  if (pc_WidgetItem != NULL)
                  {
                     pc_WidgetItem->UpdateData();
                  }
               }
            }
         }
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Edit content slot for context menu
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDpSelectorListWidget::m_EditContent(void)
{
   Q_EMIT (this->SigOpenDataPoolContent(this->GetCurrentItemIndex()));
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Copy list item to clipboard
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDpSelectorListWidget::m_Copy(void) const
{
   const uint16 u16_TimerId = osc_write_log_performance_start();

   if ((this->me_DataPoolType == C_OSCNodeDataPool::eDIAG) || (this->me_DataPoolType == C_OSCNodeDataPool::eNVM))
   {
      const sint32 s32_DpIndex = C_PuiSdHandler::h_GetInstance()->GetDataPoolIndex(this->mu32_NodeIndex,
                                                                                   this->me_DataPoolType,
                                                                                   this->GetCurrentItemIndex());

      QApplication::setOverrideCursor(Qt::WaitCursor);

      if (s32_DpIndex >= 0)
      {
         const C_OSCNodeDataPool * const pc_OSCDataPool = C_PuiSdHandler::h_GetInstance()->GetOSCDataPool(
            this->mu32_NodeIndex, s32_DpIndex);
         const C_PuiSdNodeDataPool * const pc_UIDataPool = C_PuiSdHandler::h_GetInstance()->GetUIDataPool(
            this->mu32_NodeIndex, s32_DpIndex);
         if ((pc_OSCDataPool != NULL) && (pc_UIDataPool != NULL))
         {
            C_SdClipBoardHelper::h_StoreDataPool(*pc_OSCDataPool, *pc_UIDataPool);
         }
      }

      QApplication::restoreOverrideCursor();
   }

   osc_write_log_performance_stop(u16_TimerId, "Copy datapool");
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Copy tree item to clipboard and delete it afterwards
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDpSelectorListWidget::m_Cut(void)
{
   m_Copy();
   m_Delete(false);
}

//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDpSelectorListWidget::m_DeleteSlot(void)
{
   this->m_Delete(true);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Delete datapool list item

   \param   oq_AskUser     Flag for asking user for continuing the deletion
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDpSelectorListWidget::m_Delete(const bool oq_AskUser)
{
   bool q_Continue = true;

   if (oq_AskUser == true)
   {
      C_OgeWiCustomMessage::E_Outputs e_ReturnMessageBox;
      C_OgeWiCustomMessage c_MessageBox(this, C_OgeWiCustomMessage::E_Type::eQUESTION);
      c_MessageBox.SetHeading(C_GtGetText::h_GetText("Datapool delete"));
      c_MessageBox.SetDescription(C_GtGetText::h_GetText("Do you really want to delete the selected Datapool(s)?"));
      c_MessageBox.SetOKButtonText(C_GtGetText::h_GetText("Delete"));
      c_MessageBox.SetNOButtonText(C_GtGetText::h_GetText("Keep"));
      c_MessageBox.SetCustomMinHeight(180, 180);
      e_ReturnMessageBox = c_MessageBox.Execute();

      if (e_ReturnMessageBox != C_OgeWiCustomMessage::eYES)
      {
         q_Continue = false;
      }
   }

   if (q_Continue == true)
   {
      //starting timer for performance measurement here, so the delay for waiting for user to confirm does not count
      const uint16 u16_TimerId = osc_write_log_performance_start();
      sint32 s32_Return;
      const sint32 s32_DpIndex = C_PuiSdHandler::h_GetInstance()->GetDataPoolIndex(this->mu32_NodeIndex,
                                                                                   this->me_DataPoolType,
                                                                                   this->GetCurrentItemIndex());

      QApplication::setOverrideCursor(Qt::WaitCursor);

      if (s32_DpIndex >= 0)
      {
         s32_Return = C_PuiSdHandler::h_GetInstance()->RemoveDataPool(this->mu32_NodeIndex, s32_DpIndex);
         if (s32_Return == C_NO_ERR)
         {
            this->m_InitFromData();

            // Reset the index to avoid problems if the previous index had 0
            this->msn_SelectedItemIndex = -1;

            if (this->mc_DpItems.size() > 0)
            {
               this->SetSelectedItem(0U);
               this->ScrollToItem(0U);
            }
            else
            {
               this->SetCurrentViewPage(0);
               Q_EMIT (this->SigNoDataPoolSelected());
            }
            Q_EMIT (this->SigListChanged());
            Q_EMIT (this->SigDataPoolChanged());
            //Check
            Q_EMIT (this->SigErrorCheck());
         }
      }

      QApplication::restoreOverrideCursor();
      osc_write_log_performance_stop(u16_TimerId, "Delete datapool");
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Move selected item left by one slot
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDpSelectorListWidget::m_MoveLeft(void)
{
   const sintn sn_Index = this->GetCurrentItemIndex();

   if (sn_Index > 0)
   {
      this->m_MoveDatapool(sn_Index, (sn_Index - 1));
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Move selected item right by one slot
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDpSelectorListWidget::m_MoveRight(void)
{
   const sintn sn_Index = this->GetCurrentItemIndex();

   if (sn_Index < (this->mc_DpItems.size() - 1))
   {
      this->m_MoveDatapool(sn_Index, (sn_Index + 1));
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Move selected item up by one row
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDpSelectorListWidget::m_MoveUp(void)
{
   const sintn sn_Index = this->GetCurrentItemIndex();

   if (sn_Index >= this->msn_ItemsPerRow)
   {
      this->m_MoveDatapool(sn_Index, (sn_Index - this->msn_ItemsPerRow));
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Move selected item down by one row
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDpSelectorListWidget::m_MoveDown(void)
{
   const sintn sn_Index = this->GetCurrentItemIndex();

   if (sn_Index < (this->mc_DpItems.size() - this->msn_ItemsPerRow))
   {
      this->m_MoveDatapool(sn_Index, (sn_Index + this->msn_ItemsPerRow));
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Slot for click

   Select the Datapool

   \param[in]  ou32_Index    Index of Datapool item which was clicked
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDpSelectorListWidget::m_ItemClicked(const uint32 ou32_Index)
{
   if (static_cast<sintn>(ou32_Index) < this->mc_DpItems.size())
   {
      // Item was clicked once
      QApplication::setOverrideCursor(Qt::WaitCursor);

      this->SetSelectedItem(ou32_Index);
      this->ScrollToItem(ou32_Index);

      QApplication::restoreOverrideCursor();
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Slot for double click

   Open the Datapool content

   \param[in]  ou32_Index    Index of Datapool item which was double clicked
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDpSelectorListWidget::m_ItemDoubleClicked(const uint32 ou32_Index)
{
   if (static_cast<sintn>(ou32_Index) < this->mc_DpItems.size())
   {
      Q_EMIT (this->SigOpenDataPoolContent(ou32_Index));
   }
}

//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDpSelectorListWidget::m_MoveDatapool(const sintn osn_SourceIndex, const sintn osn_TargetIndex)
{
   const uint16 u16_TimerId = osc_write_log_performance_start();
   sintn osn_TargetIndexAdapted = osn_TargetIndex;
   sint32 s32_SourceDpIndex;
   sint32 s32_TargetDpIndex;

   if (osn_TargetIndexAdapted >= this->mc_DpItems.size())
   {
      // Move 'below' the end cause an invalid index. It equals the number of elements
      --osn_TargetIndexAdapted;
   }

   s32_SourceDpIndex = C_PuiSdHandler::h_GetInstance()->GetDataPoolIndex(this->mu32_NodeIndex,
                                                                         this->me_DataPoolType,
                                                                         osn_SourceIndex);
   s32_TargetDpIndex = C_PuiSdHandler::h_GetInstance()->GetDataPoolIndex(this->mu32_NodeIndex,
                                                                         this->me_DataPoolType,
                                                                         osn_TargetIndexAdapted);

   QApplication::setOverrideCursor(Qt::WaitCursor);

   // check for index errors
   if ((s32_SourceDpIndex >= 0) &&
       (s32_TargetDpIndex >= 0))
   {
      C_PuiSdHandler::h_GetInstance()->MoveDataPool(this->mu32_NodeIndex, s32_SourceDpIndex, s32_TargetDpIndex);

      // reload data. swapping in the list is not possible without loosing the widget
      this->m_InitFromData(true);

      this->SetSelectedItem(osn_TargetIndexAdapted);
      this->ScrollToItem(osn_TargetIndexAdapted); // Sends signal SigListChanged too

      Q_EMIT (this->SigDataPoolChanged());
   }

   QApplication::restoreOverrideCursor();

   osc_write_log_performance_stop(u16_TimerId, "Move datapool");
}

//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDpSelectorListWidget::m_InitFromData(const bool oq_Update)
{
   const C_OSCNode * const pc_Node = C_PuiSdHandler::h_GetInstance()->GetOSCNodeConst(this->mu32_NodeIndex);

   QApplication::setOverrideCursor(Qt::WaitCursor);

   if (pc_Node != NULL)
   {
      uint32 u32_DpCounter;
      sintn sn_TypeCounter = 0;

      if (oq_Update == false)
      {
         this->m_Clear();
      }

      // add all datapools to the lists
      for (u32_DpCounter = 0; u32_DpCounter < pc_Node->c_DataPools.size(); ++u32_DpCounter)
      {
         if (pc_Node->c_DataPools[u32_DpCounter].e_Type == this->me_DataPoolType)
         {
            if (oq_Update == false)
            {
               // add new
               this->m_AddDataPoolWidget(u32_DpCounter);
            }
            else
            {
               // update all widgets
               this->m_UpdateDataPoolWidget(u32_DpCounter, sn_TypeCounter);
            }
            ++sn_TypeCounter;
         }
      }

      this->m_UpdateCounters(true);

      Q_EMIT (this->SigListChanged());
   }

   QApplication::restoreOverrideCursor();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Clears all Datapool items
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDpSelectorListWidget::m_Clear(void)
{
   sintn sn_ItDp;

   for (sn_ItDp = 0; sn_ItDp < this->mc_DpItems.size(); ++sn_ItDp)
   {
      C_SdNdeDpSelectorItemWidget * const pc_ItemWidget = this->mc_DpItems.at(sn_ItDp);

      disconnect(pc_ItemWidget, &C_SdNdeDpSelectorItemWidget::SigUpdateErrorToolTip, this,
                 &C_SdNdeDpSelectorListWidget::m_UpdateItemErrorToolTip);
      disconnect(pc_ItemWidget, &C_SdNdeDpSelectorItemWidget::SigHideOtherToolTips, this,
                 &C_SdNdeDpSelectorListWidget::SigHideOtherToolTips);

      disconnect(pc_ItemWidget, &C_SdNdeDpSelectorItemWidget::SigClicked,
                 this, &C_SdNdeDpSelectorListWidget::m_ItemClicked);
      disconnect(pc_ItemWidget, &C_SdNdeDpSelectorItemWidget::SigDoubleClicked,
                 this, &C_SdNdeDpSelectorListWidget::m_ItemDoubleClicked);
      disconnect(pc_ItemWidget, &C_SdNdeDpSelectorItemWidget::SigContextMenuRequested,
                 this, &C_SdNdeDpSelectorListWidget::m_OnItemCustomContextMenuRequested);
      disconnect(pc_ItemWidget, &C_SdNdeDpSelectorItemWidget::SigHoverStateChanged,
                 this, &C_SdNdeDpSelectorListWidget::SigDataPoolHoverStateChanged);

      pc_ItemWidget->setParent(NULL);

      delete pc_ItemWidget;
   }

   this->mc_DpItems.clear();
}

//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDpSelectorListWidget::m_SelectionChanged(void)
{
   if ((this->msn_SelectedItemIndex >= 0) &&
       (this->msn_SelectedItemIndex < this->mc_DpItems.size()))
   {
      C_SdNdeDpSelectorItemWidget * const pc_SelectedWidgetItem = this->mc_DpItems.at(this->msn_SelectedItemIndex);
      sintn sn_Counter;
      QApplication::setOverrideCursor(Qt::WaitCursor);

      Q_EMIT (this->SigWidgetFocused());

      for (sn_Counter = 0; sn_Counter < this->mc_DpItems.size(); ++sn_Counter)
      {
         // search the item which was clicked and adapt the stylesheet
         C_SdNdeDpSelectorItemWidget * const pc_WidgetItem = this->mc_DpItems.at(sn_Counter);
         if (pc_WidgetItem != NULL)
         {
            if (pc_WidgetItem == pc_SelectedWidgetItem)
            {
               pc_WidgetItem->SetSelected(true);
            }
            else
            {
               // deactivate all other items if they are active
               // ask before to improve performance
               if (pc_WidgetItem->GetSelected() == true)
               {
                  pc_WidgetItem->SetSelected(false);
               }
            }
         }
      }

      // update the visible "line"
      this->SetCurrentViewPage(this->GetCurrentItemIndex() / this->msn_ItemsPerViewPage);

      QApplication::restoreOverrideCursor();
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Sets the size of the item widgets

   \param[in]       orc_WidgetSize     Size of an item widget
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDpSelectorListWidget::m_AdaptSize(const QSize & orc_WidgetSize)
{
   this->msn_ItemWidth = orc_WidgetSize.width();
   this->msn_ItemHeight = orc_WidgetSize.height();
   this->setMinimumHeight(this->msn_ItemHeight);
   this->setMinimumWidth(this->msn_ItemWidth + 10);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Updates all counters, limits and layout variables

   \param[in]       oq_ForceRearrageItems     Flag if arranging of the widgets is always necessary
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDpSelectorListWidget::m_UpdateCounters(const bool oq_ForceRearrageItems)
{
   bool q_ChangedLayout = false;

   // The minus one results from a unknown error in the switch between the number of displayed Datapools
   // The switch between three and two Datapools happens at the width 435 to 436.
   // Mathematically it should happen at 434 to 435
   sintn sn_RowsPerViewPage = this->msn_HeightHint / this->msn_ItemHeight;
   sintn sn_ItemsPerRow = (this->width() - 1) / this->msn_ItemWidth;
   sintn sn_CountViewPages;

   if (sn_ItemsPerRow < 1)
   {
      sn_ItemsPerRow = 1;
   }
   if (sn_RowsPerViewPage < 1)
   {
      sn_RowsPerViewPage = 1;
   }

   // Check for relevant check
   if ((sn_RowsPerViewPage != this->msn_RowsPerViewPage) ||
       (sn_ItemsPerRow != this->msn_ItemsPerRow))
   {
      q_ChangedLayout = true;
   }
   this->msn_RowsPerViewPage = sn_RowsPerViewPage;
   this->msn_ItemsPerRow = sn_ItemsPerRow;
   this->msn_ItemsPerViewPage = sn_RowsPerViewPage * sn_ItemsPerRow;

   // Calculate page count
   sn_CountViewPages = this->mc_DpItems.size() / this->msn_ItemsPerViewPage;

   // correct rounding error
   if ((this->mc_DpItems.size() % this->msn_ItemsPerViewPage) > 0)
   {
      ++sn_CountViewPages;
   }
   if (sn_CountViewPages < 1)
   {
      sn_CountViewPages = 1;
   }
   this->msn_CountViewPages = sn_CountViewPages;
   if (sn_CountViewPages != this->msn_CountViewPages)
   {
      q_ChangedLayout = true;
   }

   // Check for change and adapt only in this case the widget arrangement
   if ((q_ChangedLayout == true) ||
       (oq_ForceRearrageItems == true))
   {
      this->m_RearrangeItems();
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Rearranging the items in the layout
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDpSelectorListWidget::m_RearrangeItems(void)
{
   sintn sn_ItDp;
   const sintn sn_FirstVisibleRow = this->msn_CurrentViewPage * this->msn_RowsPerViewPage;
   const sintn sn_LastVisibleRow = ((this->msn_CurrentViewPage + 1) * this->msn_RowsPerViewPage) - 1;

   // Update positions and visibility of all widgets
   for (sn_ItDp = 0; sn_ItDp < this->mc_DpItems.size(); ++sn_ItDp)
   {
      const sintn sn_Row = sn_ItDp / this->msn_ItemsPerRow;
      const sintn sn_Column = sn_ItDp % this->msn_ItemsPerRow;
      const sintn sn_PosX = sn_Column * msn_ItemWidth;
      const sintn sn_PosY = (sn_Row % this->msn_RowsPerViewPage) * msn_ItemHeight;
      const bool q_RowVisible = ((sn_Row >= sn_FirstVisibleRow) && (sn_Row <= sn_LastVisibleRow)) ? true : false;

      this->mc_DpItems.at(sn_ItDp)->setGeometry(QRect(QPoint(sn_PosX, sn_PosY),
                                                      this->mc_DpItems.at(sn_ItDp)->size()));
      this->mc_DpItems.at(sn_ItDp)->setVisible(q_RowVisible);
   }

   this->updateGeometry();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Trigger live update of error tooltip
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDpSelectorListWidget::m_UpdateItemErrorToolTip(const uint32 ou32_Index) const
{
   if (static_cast<sintn>(ou32_Index) < this->mc_DpItems.size())
   {
      C_SdNdeDpSelectorItemWidget * const pc_Sender = this->mc_DpItems.at(ou32_Index);
      if (pc_Sender != NULL)
      {
         const C_OSCNode * const pc_Node = C_PuiSdHandler::h_GetInstance()->GetOSCNodeConst(this->mu32_NodeIndex);
         const sint32 s32_DpIndex =
            C_PuiSdHandler::h_GetInstance()->GetDataPoolIndex(this->mu32_NodeIndex, this->me_DataPoolType, ou32_Index);
         if ((s32_DpIndex >= 0) && (pc_Node != NULL))
         {
            const uint32 u32_DataPoolIndex = static_cast<uint32>(s32_DpIndex);
            bool q_NameConflict;
            bool q_NameInvalid;
            bool q_IsErrorInListOrMessage;
            bool q_NumberOfListsOrElementsInvalid;
            std::vector<uint32> c_InvalidListIndices;
            pc_Node->CheckErrorDataPool(u32_DataPoolIndex, &q_NameConflict, &q_NameInvalid,
                                        &q_IsErrorInListOrMessage, &q_NumberOfListsOrElementsInvalid,
                                        &c_InvalidListIndices);
            if (((q_NameConflict == false) && (q_NameInvalid == false)) && (q_IsErrorInListOrMessage == false) &&
                (q_NumberOfListsOrElementsInvalid == false))
            {
               pc_Sender->SetErrorToolTip(C_GtGetText::h_GetText("No error found!"), "");
            }
            else
            {
               const QString c_Heading = C_GtGetText::h_GetText("Datapool has invalid content");
               QString c_Content;
               if ((q_NameConflict == true) || (q_NameInvalid == true))
               {
                  c_Content += C_GtGetText::h_GetText("Invalid properties:\n");
                  if (q_NameConflict == true)
                  {
                     c_Content += C_GtGetText::h_GetText("Duplicate datapool name detected.\n");
                  }
                  if (q_NameInvalid == true)
                  {
                     c_Content += C_GtGetText::h_GetText("Datapool name is empty or contains invalid characters.\n");
                  }
                  c_Content += "\n";
               }
               if (q_IsErrorInListOrMessage == true)
               {
                  if (c_InvalidListIndices.size() > 0)
                  {
                     if (this->me_DataPoolType != C_OSCNodeDataPool::eCOM)
                     {
                        c_Content += C_GtGetText::h_GetText("Lists:\n");
                     }
                     else
                     {
                        c_Content += C_GtGetText::h_GetText("Interfaces:\n");
                     }

                     for (uint32 u32_ItList = 0;
                          (u32_ItList < c_InvalidListIndices.size()) &&
                          (u32_ItList < mu32_TOOL_TIP_MAXIMUM_ITEMS);
                          ++u32_ItList)
                     {
                        if (this->me_DataPoolType != C_OSCNodeDataPool::eCOM)
                        {
                           const C_OSCNodeDataPoolList * const pc_List =
                              C_PuiSdHandler::h_GetInstance()->GetOSCDataPoolList(
                                 this->mu32_NodeIndex, u32_DataPoolIndex, c_InvalidListIndices[u32_ItList]);
                           if (pc_List != NULL)
                           {
                              c_Content += static_cast<QString>("%1\n").arg(pc_List->c_Name.c_str());
                           }
                        }
                        else
                        {
                           // In case of a COMM Datapool the values in c_InvalidListIndices equals not the list index,
                           // the values equals the interface index
                           const QString c_InterfaceName =
                              C_PuiSdUtil::h_GetInterfaceName(C_OSCSystemBus::eCAN,
                                                              static_cast<uint8>(c_InvalidListIndices[u32_ItList]));
                           c_Content += c_InterfaceName + "\n";
                        }
                     }
                     if (mu32_TOOL_TIP_MAXIMUM_ITEMS < c_InvalidListIndices.size())
                     {
                        c_Content +=
                           static_cast<QString>("+%1\n").arg(static_cast<uint32>(c_InvalidListIndices.size()) -
                                                             mu32_TOOL_TIP_MAXIMUM_ITEMS);
                     }
                     c_Content += "\n";
                  }
               }
               if (q_NumberOfListsOrElementsInvalid == true)
               {
                  c_Content += C_GtGetText::h_GetText("The number of lists or elements in the data pool is invalid.\n");
               }

               pc_Sender->SetErrorToolTip(c_Heading, c_Content);
            }
         }
      }
   }
}
