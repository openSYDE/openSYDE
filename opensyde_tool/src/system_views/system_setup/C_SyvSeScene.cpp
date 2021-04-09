//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Graphics scene for system view setup (implementation)

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.h"

#include <QApplication>
#include <QGraphicsItem>
#include <QDialog>

#include "gitypes.h"

#include "TGLUtils.h"
#include "C_SyvSeScene.h"
#include "C_GiSvNodeSyvSetup.h"
#include "C_SdNdeNodeEditWidget.h"
#include "C_SdBueBusEditWidget.h"
#include "C_SebUtil.h"
#include "C_PuiSdHandler.h"
#include "C_GiSvPc.h"
#include "C_GiSvPcBusConnector.h"
#include "C_PuiSvHandler.h"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw_tgl;
using namespace stw_types;
using namespace stw_opensyde_gui;
using namespace stw_opensyde_core;
using namespace stw_opensyde_gui_logic;
using namespace stw_opensyde_gui_elements;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

/* -- Global Variables ---------------------------------------------------------------------------------------------- */

/* -- Module Global Variables --------------------------------------------------------------------------------------- */

/* -- Module Global Function Prototypes ----------------------------------------------------------------------------- */

/* -- Implementation ------------------------------------------------------------------------------------------------ */

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default constructor

   \param[in]     ou32_ViewIndex View index
   \param[in,out] opc_Parent     Optional pointer to parent
*/
//----------------------------------------------------------------------------------------------------------------------
C_SyvSeScene::C_SyvSeScene(const uint32 ou32_ViewIndex, QObject * const opc_Parent) :
   C_SyvTopologyBaseScene(ou32_ViewIndex, opc_Parent),
   mq_EditMode(false),
   mpc_SelectedBusConnectorItem(NULL),
   mc_UndoManager(this)
{
   // Init base scene. Initializing all parts which can not be initialized in the base constructor
   this->m_InitSceneUndoManager();
   this->m_InitSceneContextMenuManager();

   // configure context menu
   connect(&this->mc_ContextMenuManager, &C_SyvSeContextMenuManager::SigShowCanConfiguration,
           this, &C_SyvSeScene::m_Edit);

   // selection changed
   connect(this, &C_SyvSeScene::selectionChanged, this, &C_SyvSeScene::m_SelectionChanged);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   default destructor

   Clean up.
*/
//----------------------------------------------------------------------------------------------------------------------
//lint -e{1540} never took ownership of selected bus connector
C_SyvSeScene::~C_SyvSeScene(void)
{
   this->PrepareToClose();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Function to prepare closing the scene
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvSeScene::PrepareToClose(void)
{
   //Clean up all created dialogs
   for (std::list<QDialog *>::iterator c_ItDialog = this->mc_Dialogs.begin(); c_ItDialog != this->mc_Dialogs.end();
        ++c_ItDialog)
   {
      (*c_ItDialog)->close();
      delete (*c_ItDialog);
   }
   this->mc_Dialogs.clear();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Sets the state of the edit mode

   \param[in]     oq_Active      Flag for setting the edit mode
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvSeScene::SetEditMode(const bool oq_Active)
{
   const QList<QGraphicsItem *> & rc_Items = this->items();

   QList<QGraphicsItem *>::const_iterator c_ItItem;

   this->mq_EditMode = oq_Active;

   // inform the items
   for (c_ItItem = rc_Items.begin(); c_ItItem != rc_Items.end(); ++c_ItItem)
   {
      C_GiSvNodeSyvSetup * const pc_Node = dynamic_cast<C_GiSvNodeSyvSetup *>(*c_ItItem);

      C_GiSvPc * const pc_Pc = dynamic_cast<C_GiSvPc *>(*c_ItItem);

      C_GiSvPcBusConnector * const pc_PcConnection = dynamic_cast<C_GiSvPcBusConnector *>(*c_ItItem);
      if (pc_Node != NULL)
      {
         pc_Node->SetEditMode(oq_Active);
      }
      if (pc_Pc != NULL)
      {
         pc_Pc->SetEditMode(oq_Active);
      }
      if (pc_PcConnection != NULL)
      {
         pc_PcConnection->SetEditMode(oq_Active);
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Save data
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvSeScene::Save(void) const
{
   const QList<QGraphicsItem *> & rc_Items = this->items();
   bool q_NoConnection = true;

   QList<QGraphicsItem *>::const_iterator c_ItItem;

   // inform the items
   for (c_ItItem = rc_Items.begin(); c_ItItem != rc_Items.end(); ++c_ItItem)
   {
      C_GiSvNodeSyvSetup * const pc_Node = dynamic_cast<C_GiSvNodeSyvSetup *>(*c_ItItem);

      C_GiSvPc * const pc_Pc = dynamic_cast<C_GiSvPc *>(*c_ItItem);

      C_GiSvPcBusConnector * const pc_PcConnection = dynamic_cast<C_GiSvPcBusConnector *>(*c_ItItem);
      if (pc_Node != NULL)
      {
         pc_Node->UpdateData();
      }
      if (pc_Pc != NULL)
      {
         pc_Pc->UpdateData();
      }
      if (pc_PcConnection != NULL)
      {
         pc_PcConnection->UpdateData();
         q_NoConnection = false;
      }
   }
   if (q_NoConnection == true)
   {
      C_PuiSvHandler::h_GetInstance()->SetViewPCConnected(this->mu32_ViewIndex, false, 0);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Reloads all view relevant data again
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvSeScene::ReloadViewData(void)
{
   bool q_ConnectionFound = false;

   const QList<QGraphicsItem *> & rc_Items = this->items();

   QList<QGraphicsItem *>::const_iterator c_ItItem;

   QApplication::setOverrideCursor(Qt::WaitCursor);

   for (c_ItItem = rc_Items.begin(); c_ItItem != rc_Items.end(); ++c_ItItem)
   {
      C_GiSvNodeSyvSetup * const pc_Node = dynamic_cast<C_GiSvNodeSyvSetup *>(*c_ItItem);

      C_GiSvPc * const pc_Pc = dynamic_cast<C_GiSvPc *>(*c_ItItem);

      C_GiSvPcBusConnector * const pc_PcBusConnection = dynamic_cast<C_GiSvPcBusConnector *>(*c_ItItem);

      if (pc_Node != NULL)
      {
         pc_Node->LoadData();
      }
      if (pc_Pc != NULL)
      {
         pc_Pc->LoadData();
      }
      if (pc_PcBusConnection != NULL)
      {
         const C_PuiSvData * const pc_View = C_PuiSvHandler::h_GetInstance()->GetView(this->mu32_ViewIndex);
         q_ConnectionFound = true;
         if (pc_View != NULL)
         {
            if (pc_View->GetPcData().GetConnected() == true)
            {
               C_GiLiBus * pc_FoundBus = NULL;
               //Last bus
               for (QList<QGraphicsItem *>::const_iterator c_ItItem2 = rc_Items.begin(); c_ItItem2 != rc_Items.end();
                    ++c_ItItem2)
               {
                  C_GiLiBus * const pc_PcBus = dynamic_cast<C_GiLiBus *>(*c_ItItem2);
                  if (pc_PcBus != NULL)
                  {
                     if ((pc_PcBus->GetIndex() >= 0) &&
                         (static_cast<uint32>(pc_PcBus->GetIndex()) == pc_View->GetPcData().GetBusIndex()))
                     {
                        pc_FoundBus = pc_PcBus;
                     }
                  }
               }
               //Reset bus (might be different)
               pc_PcBusConnection->RevertBus(pc_FoundBus, NULL,
                                             pc_View->GetPcData().GetConnectionData().c_UIInteractionPoints[0]);
               pc_PcBusConnection->SetPoints(pc_View->GetPcData().GetConnectionData().c_UIInteractionPoints);
            }
            else
            {
               pc_PcBusConnection->deleteLater();
            }
         }
      }
   }
   if (q_ConnectionFound == false)
   {
      const C_PuiSvData * const pc_View = C_PuiSvHandler::h_GetInstance()->GetView(this->mu32_ViewIndex);
      if (pc_View != NULL)
      {
         if (pc_View->GetPcData().GetConnected() == true)
         {
            C_GiLiBus * pc_FoundBus = NULL;
            C_GiSvPc * pc_FoundPc = NULL;
            //Last bus and pc
            for (QList<QGraphicsItem *>::const_iterator c_ItItem2 = rc_Items.begin(); c_ItItem2 != rc_Items.end();
                 ++c_ItItem2)
            {
               C_GiLiBus * const pc_PcBus = dynamic_cast<C_GiLiBus *>(*c_ItItem2);

               C_GiSvPc * const pc_Pc = dynamic_cast<C_GiSvPc *>(*c_ItItem2);
               if (pc_Pc != NULL)
               {
                  pc_FoundPc = pc_Pc;
               }
               if (pc_PcBus != NULL)
               {
                  if ((pc_PcBus->GetIndex() >= 0) &&
                      (static_cast<uint32>(pc_PcBus->GetIndex()) == pc_View->GetPcData().GetBusIndex()))
                  {
                     pc_FoundBus = pc_PcBus;
                  }
               }
            }
            if ((pc_FoundPc != NULL) && (pc_FoundBus != NULL))
            {
               //Create connection
               m_AddPcBusConnector(pc_FoundBus, pc_FoundPc, pc_View->GetPcData().GetConnectionData());
            }
         }
      }
   }
   //Reload state
   static_cast<void>(m_CheckBusState());

   Q_EMIT this->SigErrorCheck();

   QApplication::restoreOverrideCursor();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Clear undo stack
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvSeScene::ClearUndoStack(void)
{
   this->mc_UndoManager.clear();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Sets the view connected state of all nodes to the parameter oq_Connected

   \param[in]     oq_Connected     Flag for view connection of all nodes
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvSeScene::SetAllNodesConnected(const bool oq_Connected)
{
   const QList<QGraphicsItem *> & rc_Items = this->items();

   QList<QGraphicsItem *>::const_iterator c_ItItem;
   bool q_Changed = false;

   QApplication::setOverrideCursor(Qt::WaitCursor);

   for (c_ItItem = rc_Items.begin(); c_ItItem != rc_Items.end(); ++c_ItItem)
   {
      C_GiSvNodeSyvSetup * const pc_Node = dynamic_cast<C_GiSvNodeSyvSetup *>(*c_ItItem);

      if (pc_Node != NULL)
      {
         if (pc_Node->IsViewConnected() != oq_Connected)
         {
            pc_Node->SetViewConnected(oq_Connected);
            q_Changed = true;
         }
      }
   }

   if (q_Changed == true)
   {
      m_OnConnectionChanged();
   }

   QApplication::restoreOverrideCursor();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Checking if any item can be added to the scene

   \return
   true     Items can be added
   false    Items cannot be added
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_SyvSeScene::IsAnyItemAddable(void) const
{
   return false;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Checking if the graphics item is movable on the scene

   \param[in]  opc_Item    Graphics item to check

   \return
   true     Item is movable
   false    Item is not movable
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_SyvSeScene::IsItemMovable(const QGraphicsItem * const opc_Item) const
{
   bool q_Return = false;

   if (opc_Item->type() == msn_GRAPHICS_ITEM_PC)
   {
      q_Return = true;
   }

   return q_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Checking if the graphics item is selectable on the scene

   \param[in]  opc_Item    Graphics item to check

   \return
   true     Item is selectable
   false    Item is not selectable
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_SyvSeScene::IsItemSelectable(const QGraphicsItem * const opc_Item) const
{
   bool q_Return = false;

   if (opc_Item->type() == msn_GRAPHICS_ITEM_PC)
   {
      q_Return = true;
   }

   return q_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Allow interaction

   \return
   True  Drag move is available
   False Drag move is not available
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_SyvSeScene::IsSceneRubberBandAvailable() const
{
   return this->mq_EditMode;
}

//----------------------------------------------------------------------------------------------------------------------
C_SebBaseContextMenuManager * C_SyvSeScene::m_GetContextMenuManager(void)
{
   return &this->mc_ContextMenuManager;
}

//----------------------------------------------------------------------------------------------------------------------
C_SebUnoBaseManager * C_SyvSeScene::m_GetUndoManager(void)
{
   return &this->mc_UndoManager;
}

//----------------------------------------------------------------------------------------------------------------------
C_GiNode * C_SyvSeScene::m_CreateNode(const sint32 & ors32_Index, const uint64 & oru64_ID, const float64 & orf64_Width,
                                      const float64 & orf64_Height, QGraphicsItem * const opc_Parent)
{
   return new C_GiSvNodeSyvSetup(this->mu32_ViewIndex, ors32_Index, oru64_ID, orf64_Width, orf64_Height, opc_Parent);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Add new node to scene and connect signals

   \param[in,out] opc_NodeGraphicsItem Pointer to new node
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvSeScene::m_AddNodeToScene(C_GiNode * const opc_NodeGraphicsItem)
{
   const C_GiSvNodeSyvSetup * const pc_SystemNode =
      dynamic_cast<const C_GiSvNodeSyvSetup * const>(opc_NodeGraphicsItem);

   if (pc_SystemNode != NULL)
   {
      connect(pc_SystemNode, &C_GiSvNodeSyvSetup::SigViewConnectionChanged, this, &C_SyvSeScene::m_OnConnectionChanged);
   }

   C_SyvTopologyBaseScene::m_AddNodeToScene(opc_NodeGraphicsItem);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Add new PC bus connector item to scene and connect signals

   \param[in,out] opc_PcBusConnectorGraphicsItem  PC bus connector item
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvSeScene::m_AddPcBusConnectorToScene(C_GiSvPcBusConnector * const opc_PcBusConnectorGraphicsItem)
{
   connect(opc_PcBusConnectorGraphicsItem, &C_GiSvPcBusConnector::SigStartConnect, this,
           &C_SyvSeScene::m_OnPcBusConnectionStartConnect);
   connect(opc_PcBusConnectorGraphicsItem, &C_GiSvPcBusConnector::SigStopConnect, this,
           &C_SyvSeScene::m_OnPcBusConnectionStopConnect);
   C_SyvTopologyBaseScene::m_AddPcBusConnectorToScene(opc_PcBusConnectorGraphicsItem);
   //Allow edit
   if (this->mq_EditMode == true)
   {
      opc_PcBusConnectorGraphicsItem->SetEditMode(true);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Add new text element to scene and connect signals

   \param[in,out] opc_Item  Text element
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvSeScene::m_AddTextElementToScene(C_GiBiTextElement * const opc_Item)
{
   opc_Item->SetEditable(false);

   C_SebTopologyBaseScene::m_AddTextElementToScene(opc_Item);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Overwritten mouse double click event slot

   Here: handle item edit

   \param[in,out] opc_Event Event identification and information
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvSeScene::mouseDoubleClickEvent(QGraphicsSceneMouseEvent * const opc_Event)
{
   QGraphicsItem * const pc_Item = this->itemAt(opc_Event->scenePos(), QTransform());

   if ((pc_Item != NULL) &&
       (opc_Event->buttons().testFlag(Qt::LeftButton) == true))
   {
      QGraphicsItem * const pc_Parent = C_SebUtil::h_GetHighestParent(pc_Item);

      if (pc_Parent != NULL)
      {
         if (pc_Parent->type() == msn_GRAPHICS_ITEM_PC)
         {
            this->m_Edit(pc_Parent);
         }
         else
         {
            // different handling because of interaction elements in the setup nodes (checkbox)
            // the original event has to be handled first
            C_SyvTopologyBaseScene::mouseDoubleClickEvent(opc_Event);

            if (opc_Event->isAccepted() == false)
            {
               //SSI 16.08.18 disabled. Deprecated with new navi bar.
               //Will be removed completely with SC pages redesign in future
               //this->m_ShowInfo(pc_Parent);
            }
         }
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Handle item edit

   \param[in] opc_GraphicsItem Edit item
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvSeScene::m_Edit(QGraphicsItem * const opc_GraphicsItem) const
{
   if (opc_GraphicsItem != NULL)
   {
      C_GiSvPc * const pc_Pc = dynamic_cast<C_GiSvPc * const>(opc_GraphicsItem);

      if (pc_Pc != NULL)
      {
         pc_Pc->OpenDialog();
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Handle selection change
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvSeScene::m_SelectionChanged(void)
{
   //Reset selected bus connector
   if (this->mpc_SelectedBusConnectorItem != NULL)
   {
      this->mpc_SelectedBusConnectorItem->RestoreZOrder();
      this->mpc_SelectedBusConnectorItem = NULL;
   }
   if (this->mq_ProxyWidgetInteractionActive == false)
   {
      QList<QGraphicsItem *>::const_iterator c_ItItem;
      QList<QGraphicsItem *> c_SelectedItems = this->selectedItems();

      // triggered by signal selectionChanged
      if (c_SelectedItems.size() == 1)
      {
         C_GiBiRectBaseGroup * const pc_Item = dynamic_cast<C_GiBiRectBaseGroup *>(c_SelectedItems[0]);

         // check if the only one selected item is a resizable rectangle based item
         if (pc_Item != NULL)
         {
            C_GiSvPc * const pc_Pc = dynamic_cast<C_GiSvPc *>(pc_Item);
            if (pc_Pc != NULL)
            {
               // activate the resizing only if exactly one node is selected
               pc_Item->SetResizing(true);
            }
         }

         if (c_SelectedItems[0]->type() == msn_GRAPHICS_ITEM_PC_CONNECTION)
         {
            // bring the selected bus connector to the top

            this->mpc_SelectedBusConnectorItem = dynamic_cast<C_GiSvPcBusConnector *>(c_SelectedItems[0]);
            if (this->mpc_SelectedBusConnectorItem != NULL)
            {
               this->mpc_SelectedBusConnectorItem->SetZValueCustom(mf64_ZORDER_MAX);
               this->mpc_SelectedBusConnectorItem->SetResizing(true);
            }
         }
      }
      else
      {
         // deactivate resizing for all nodes
         for (c_ItItem = c_SelectedItems.begin(); c_ItItem != c_SelectedItems.end(); ++c_ItItem)
         {
            // check if the only one selected item is a resizable rectangle based item

            C_GiBiRectBaseGroup * const pc_Item = dynamic_cast<C_GiBiRectBaseGroup *>(*c_ItItem);
            if (pc_Item != NULL)
            {
               pc_Item->SetResizing(false);
            }
            else
            {
               C_GiLiLineGroup * const pc_LineItem = dynamic_cast<C_GiLiLineGroup *>(*c_ItItem);
               if (pc_LineItem != NULL)
               {
                  pc_LineItem->SetResizing(false);
               }
            }
         }
      }
   }
   else
   {
      this->clearSelection();
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Handle bus connection start connect event

   \param[in]     ore_ConnectState Connect state
   \param[in,out] opc_Item         Connected generic item
   \param[in]     opc_Type         Bus type
   \param[in,out] opc_Connector    Source
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvSeScene::m_OnPcBusConnectionStartConnect(const C_GiLiBusConnectorBase::E_ConnectState & ore_ConnectState,
                                                   const QGraphicsItem * const opc_Item,
                                                   const C_OSCSystemBus::E_Type * const opc_Type,
                                                   const C_GiLiBusConnectorBase * const opc_Connector) const
{
   Q_UNUSED(opc_Item)
   Q_UNUSED(opc_Type)
   Q_UNUSED(opc_Connector)
   if (this->mq_EditMode == true)
   {
      if (ore_ConnectState == C_GiLiBusConnectorBase::eTO_GENERIC)
      {
         const QList<QGraphicsItem *> & rc_Items = this->items();

         QList<QGraphicsItem *>::const_iterator c_ItItem;

         for (c_ItItem = rc_Items.begin(); c_ItItem != rc_Items.end(); ++c_ItItem)
         {
            C_GiBiCustomMouseItem * const pc_Item = dynamic_cast<C_GiBiCustomMouseItem *>(*c_ItItem);
            if (pc_Item != NULL)
            {
               if (dynamic_cast<C_GiSvPc *>(*c_ItItem) == NULL)
               {
                  //Is not pc
                  pc_Item->SetTemporaryCursor(Qt::ForbiddenCursor);
               }
               else
               {
                  pc_Item->SetTemporaryCursor(Qt::CrossCursor);
               }
            }
         }
      }
      else if (ore_ConnectState == C_GiLiBusConnectorBase::eTO_BUS)
      {
         const QList<QGraphicsItem *> & rc_Items = this->items();

         QList<QGraphicsItem *>::const_iterator c_ItItem;

         for (c_ItItem = rc_Items.begin(); c_ItItem != rc_Items.end(); ++c_ItItem)
         {
            C_GiBiCustomMouseItem * const pc_Item = dynamic_cast<C_GiBiCustomMouseItem *>(*c_ItItem);
            if (pc_Item != NULL)
            {
               C_GiLiBus * const pc_Bus = dynamic_cast<C_GiLiBus *>(*c_ItItem);
               //Is bus
               if (pc_Bus != NULL)
               {
                  const bool q_Disabled =
                     C_PuiSvHandler::h_GetInstance()->CheckBusDisabled(this->mu32_ViewIndex,
                                                                       static_cast<uint32>(pc_Bus->GetIndex()));
                  if (q_Disabled == false)
                  {
                     pc_Item->SetTemporaryCursor(Qt::CrossCursor);
                  }
                  else
                  {
                     pc_Item->SetTemporaryCursor(Qt::ForbiddenCursor);
                  }
               }
               else
               {
                  pc_Item->SetTemporaryCursor(Qt::ForbiddenCursor);
               }
            }
         }
      }
      else
      {
         //Not supported
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Handle bus connection stop connect event
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvSeScene::m_OnPcBusConnectionStopConnect(void) const
{
   //Update system view data
   this->Save();

   m_RestoreCursors();

   //Trigger error check!
   Q_EMIT (this->SigErrorCheck());
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Handle connection change
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvSeScene::m_OnConnectionChanged(void)
{
   QString c_Dummy;

   //Update system view data
   this->Save();

   static_cast<void>(m_HandlePcReconnectIfNecessary(c_Dummy));

   Q_EMIT (this->SigChanged());
   //Trigger error check!
   Q_EMIT (this->SigErrorCheck());
   //Trigger connection changed signal
   Q_EMIT (this->SigConnectionChange());
}
