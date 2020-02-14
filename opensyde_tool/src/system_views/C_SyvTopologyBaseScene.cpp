//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Common graphics scene with system view functionality (implementation)

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.h"

#include <QGraphicsView>

#include "C_SyvTopologyBaseScene.h"
#include "C_GiSvPc.h"
#include "C_GiSvCANBus.h"
#include "C_GiSvEthernetBus.h"
#include "C_GiSvTextElementBus.h"
#include "C_GiSvPcBusConnector.h"
#include "gitypes.h"
#include "TGLUtils.h"
#include "C_PuiSvHandler.h"
#include "C_GtGetText.h"
#include "C_OgeWiCustomMessage.h"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw_tgl;
using namespace stw_types;
using namespace stw_opensyde_gui;
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

   \param[in]      ou32_ViewIndex   View index
   \param[in,out]  opc_Parent       Optional pointer to parent
*/
//----------------------------------------------------------------------------------------------------------------------
C_SyvTopologyBaseScene::C_SyvTopologyBaseScene(const stw_types::uint32 ou32_ViewIndex, QObject * const opc_Parent) :
   C_SebTopologyBaseScene(opc_Parent),
   mu32_ViewIndex(ou32_ViewIndex)
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   default destructor

   Clean up.
*/
//----------------------------------------------------------------------------------------------------------------------
C_SyvTopologyBaseScene::~C_SyvTopologyBaseScene()
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Load system definition
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvTopologyBaseScene::Load(void)
{
   bool q_Change = false;
   C_GiSvPc * const pc_PC = new C_GiSvPc(this->m_GetNewUniqueID(), this->mu32_ViewIndex);
   const C_PuiSvData * const pc_View = C_PuiSvHandler::h_GetInstance()->GetView(this->mu32_ViewIndex);
   QString c_BusName = "";

   C_SebTopologyBaseScene::Load();

   pc_PC->LoadData();
   m_AddRectBaseGroupToScene(pc_PC);

   //Tool tip connect (no disconnect necessary because PC cannot be deleted)
   connect(pc_PC, &C_GiSvPc::SigHideToolTip, this, &C_SyvTopologyBaseScene::m_HandleHideToolTip);

   m_AddAnyItemToScene(pc_PC);
   if (pc_View != NULL)
   {
      if (pc_View->GetPcData().GetConnected() == true)
      {
         const uint32 u32_BusIndex = pc_View->GetPcData().GetBusIndex();
         QList<QGraphicsItem *> c_Items = this->items();
         C_GiLiBus * pc_Bus = NULL;

         //Search for matching bus
         for (QList<QGraphicsItem *>::iterator c_ItItem = c_Items.begin(); c_ItItem != c_Items.end(); ++c_ItItem)
         {
            QGraphicsItem * const pc_CurItemParent = C_SebUtil::h_GetHighestParent(*c_ItItem);
            if (pc_CurItemParent != NULL)
            {
               //lint -e{929}  false positive in PC-Lint: allowed by MISRA 5-2-2
               C_GiLiBus * const pc_CurBus = dynamic_cast<C_GiLiBus *>(pc_CurItemParent);
               if (pc_CurBus != NULL)
               {
                  if (static_cast<uint32>(pc_CurBus->GetIndex()) == u32_BusIndex)
                  {
                     pc_Bus = pc_CurBus;
                     break;
                  }
               }
            }
         }
         if (pc_Bus != NULL)
         {
            m_AddPcBusConnector(pc_Bus, pc_PC, pc_View->GetPcData().GetConnectionData());
         }
         else
         {
            q_Change = true;
         }
      }
      else
      {
         q_Change = pc_View->GetPcData().CheckIndirectDisconnection();
      }
   }
   //Set bus state
   q_Change = m_HandlePcReconnectIfNecessary(c_BusName) || q_Change;
   if (q_Change == true)
   {
      QGraphicsView * const pc_GraphicsView = this->views().at(0);
      C_OgeWiCustomMessage c_MessageBox(pc_GraphicsView, C_OgeWiCustomMessage::E_Type::eQUESTION);
      c_MessageBox.SetType(C_OgeWiCustomMessage::E_Type::eWARNING);
      c_MessageBox.SetHeading(C_GtGetText::h_GetText("Connection updated!"));
      Q_EMIT this->SigChanged();
      if (c_BusName != "")
      {
         c_MessageBox.SetDescription(C_GtGetText::h_GetText(
                                        "Due to changes in SYSTEM DEFINITION the PC - connected bus has been changed.\n"
                                        "PC is connected to ") + c_BusName + C_GtGetText::h_GetText(" now."));
         c_MessageBox.SetCustomMinHeight(180, 180);
         c_MessageBox.Execute();
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Copy items to scene from copy paste manager

   \param[in]  opc_Pos  Optional position offset
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvTopologyBaseScene::CopyFromManagerToScene(const QPointF * const opc_Pos)
{
   // Nothing to do yet
   Q_UNUSED(opc_Pos)
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Checking if the graphics item is deletable on the scene

   \param[in]  opc_Item    Item to check

   \return
   true     Item is deletable
   false    Item is not deletable
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_SyvTopologyBaseScene::IsItemDeletable(const QGraphicsItem * const opc_Item) const
{
   Q_UNUSED(opc_Item)

   // no deleting here
   return false;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Checking if the graphics item is changeable in the zorder

   \param[in]  opc_Item    Item to check

   \return
   true     Z order is changeable
   false    Z order is not changeable
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_SyvTopologyBaseScene::IsZOrderChangeable(const QGraphicsItem * const opc_Item) const
{
   Q_UNUSED(opc_Item)

   // no change of zorder allowed here
   return false;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Checking if the graphics item can be aligned

   \param[in]  opc_Item    Item to check

   \return
   true     Item can be aligned
   false    Item can not be alined
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_SyvTopologyBaseScene::IsAlignmentUsable(const QGraphicsItem * const opc_Item) const
{
   Q_UNUSED(opc_Item)

   // no algining here
   return false;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Checking if the drag move over the scene can be used

   \return
   true     Drag move is available
   false    Drag move is available
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_SyvTopologyBaseScene::IsSceneRubberBandAvailable(void) const
{
   // no dragging here
   return false;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get current copy paste manager

   \return
   NULL No copy paste manager
   Else Valid copy paste manager
*/
//----------------------------------------------------------------------------------------------------------------------
stw_opensyde_gui_logic::C_SebBaseCopyPasteManager * C_SyvTopologyBaseScene::m_GetCopyPasteManager(void)
{
   return NULL;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get current copy paste manager

   \return
   NULL No copy paste manager
   Else Valid copy paste manager
*/
//----------------------------------------------------------------------------------------------------------------------
const stw_opensyde_gui_logic::C_SebBaseCopyPasteManager * C_SyvTopologyBaseScene::m_GetCopyPasteManagerConst(void) const
{
   return NULL;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get specific CAN bus

   \param[in]      ors32_Index            Index of data element in system definition
   \param[in]      oru64_ID               Unique ID
   \param[in]      opc_TextElementName    Pointer to text element for showing bus name
   \param[in]      opc_Points             Points for line
   \param[in,out]  opc_Parent             Optional pointer to parent

   \return
   Specific CAN bus
*/
//----------------------------------------------------------------------------------------------------------------------
C_GiLiCANBus * C_SyvTopologyBaseScene::m_CreateCANBus(const sint32 & ors32_Index, const uint64 & oru64_ID,
                                                      C_GiTextElementBus * const opc_TextElementName,
                                                      const std::vector<QPointF> * const opc_Points,
                                                      QGraphicsItem * const opc_Parent)
{
   return new C_GiSvCANBus(this->mu32_ViewIndex, ors32_Index, oru64_ID, opc_TextElementName, opc_Points, opc_Parent);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get specific ethernet bus

   \param[in]      ors32_Index            Index of data element in system definition
   \param[in]      oru64_ID               Unique ID
   \param[in]      opc_TextElementName    Pointer to text element for showing bus name
   \param[in]      opc_Points             Points for line
   \param[in,out]  opc_Parent             Optional pointer to parent

   \return
   Specific ethernet bus
*/
//----------------------------------------------------------------------------------------------------------------------
C_GiLiEthernetBus * C_SyvTopologyBaseScene::m_CreateEthernetBus(const sint32 & ors32_Index, const uint64 & oru64_ID,
                                                                C_GiTextElementBus * const opc_TextElementName,
                                                                const std::vector<QPointF> * const opc_Points,
                                                                QGraphicsItem * const opc_Parent)
{
   return new C_GiSvEthernetBus(this->mu32_ViewIndex, ors32_Index, oru64_ID, opc_TextElementName, opc_Points,
                                opc_Parent);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get specific bus text element

   \param[in]      ors32_Index   Index of data element in system definition
   \param[in]      oru64_ID      Unique ID
   \param[in,out]  opc_Parent    Optional pointer to parent

   \return
   Specific bus text element
*/
//----------------------------------------------------------------------------------------------------------------------
C_GiTextElementBus * C_SyvTopologyBaseScene::m_CreateBusTextElement(const sint32 & ors32_Index, const uint64 & oru64_ID,
                                                                    QGraphicsItem * const opc_Parent)
{
   return new C_GiSvTextElementBus(ors32_Index, oru64_ID, opc_Parent);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Add new bus to scene and connect signals

   \param[in,out]  opc_BusGraphicsItem    Pointer to new bus
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvTopologyBaseScene::m_AddBusToScene(C_GiLiBus * const opc_BusGraphicsItem)
{
   C_SebTopologyBaseScene::m_AddBusToScene(opc_BusGraphicsItem);

   opc_BusGraphicsItem->setFlag(QGraphicsItem::ItemIsMovable, false);
   opc_BusGraphicsItem->SetDefaultCursor(Qt::ArrowCursor);
   opc_BusGraphicsItem->setFlag(QGraphicsItem::ItemIsSelectable, false);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Add new bus connector to scene and connect signals

   \param[in,out]  opc_BusConnectorGraphicsItem    Connector item
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvTopologyBaseScene::m_AddBusConnectorToScene(C_GiLiBusConnector * const opc_BusConnectorGraphicsItem)
{
   if (opc_BusConnectorGraphicsItem != NULL)
   {
      C_SebTopologyBaseScene::m_AddBusConnectorToScene(opc_BusConnectorGraphicsItem);

      opc_BusConnectorGraphicsItem->setFlag(QGraphicsItem::ItemIsMovable, false);
      opc_BusConnectorGraphicsItem->SetDefaultCursor(Qt::ArrowCursor);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Add new text element to scene and connect signals

   \param[in,out]  opc_Item   Text element
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvTopologyBaseScene::m_AddTextElementToScene(C_GiBiTextElement * const opc_Item)
{
   C_SebTopologyBaseScene::m_AddTextElementToScene(opc_Item);

   opc_Item->setFlag(QGraphicsItem::ItemIsMovable, false);
   opc_Item->SetDefaultCursor(Qt::ArrowCursor);
   opc_Item->setFlag(QGraphicsItem::ItemIsSelectable, false);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Add new line arrow to scene and connect signals

   \param[in,out]  opc_Item   Line arrow item
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvTopologyBaseScene::m_AddLineArrowToScene(C_GiBiArrow * const opc_Item)
{
   C_SebTopologyBaseScene::m_AddLineArrowToScene(opc_Item);

   opc_Item->setFlag(QGraphicsItem::ItemIsMovable, false);
   opc_Item->SetDefaultCursor(Qt::ArrowCursor);
   opc_Item->setFlag(QGraphicsItem::ItemIsSelectable, false);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Add new boundary to scene and connect signals

   \param[in,out]  opc_Item   Boundary item
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvTopologyBaseScene::m_AddBoundaryToScene(C_GiBiBoundary * const opc_Item)
{
   C_SebTopologyBaseScene::m_AddBoundaryToScene(opc_Item);

   opc_Item->setFlag(QGraphicsItem::ItemIsMovable, false);
   opc_Item->SetDefaultCursor(Qt::ArrowCursor);
   opc_Item->setFlag(QGraphicsItem::ItemIsSelectable, false);
}
//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Add new image to scene and connect signals

   \param[in,out]  opc_Item   Image group item
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvTopologyBaseScene::m_AddImageGroupToScene(C_GiBiImageGroup * const opc_Item)
{
   C_SebTopologyBaseScene::m_AddImageGroupToScene(opc_Item);

   opc_Item->setFlag(QGraphicsItem::ItemIsMovable, false);
   opc_Item->SetDefaultCursor(Qt::ArrowCursor);
   opc_Item->setFlag(QGraphicsItem::ItemIsSelectable, false);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Add new rectangle based item to scene and connect signals

   \param[in,out]  opc_Item   Rectangle based item
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvTopologyBaseScene::m_AddRectBaseGroupToScene(C_GiBiRectBaseGroup * const opc_Item)
{
   C_SebTopologyBaseScene::m_AddRectBaseGroupToScene(opc_Item);

   opc_Item->setFlag(QGraphicsItem::ItemIsMovable, false);
   opc_Item->SetDefaultCursor(Qt::ArrowCursor);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Add new line based item to scene and connect signals

   \param[in,out]  opc_Item   Line based item
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvTopologyBaseScene::m_AddLineGroupToScene(C_GiLiLineGroup * const opc_Item)
{
   C_SebTopologyBaseScene::m_AddLineGroupToScene(opc_Item);

   opc_Item->setFlag(QGraphicsItem::ItemIsMovable, false);
   opc_Item->SetDefaultCursor(Qt::ArrowCursor);
   opc_Item->setFlag(QGraphicsItem::ItemIsSelectable, false);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Add new PC bus connector item to scene and connect signals

   \param[in,out]  opc_PcBusConnectorGraphicsItem  PC bus connector item
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvTopologyBaseScene::m_AddPcBusConnectorToScene(C_GiSvPcBusConnector * const opc_PcBusConnectorGraphicsItem)
{
   m_AddBusConnectorBaseToScene(opc_PcBusConnectorGraphicsItem);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Add data from mime data

   \param[in]  opc_MimeData   Mime data to add to scene
   \param[in]  orc_Position   Position to add data at

   \return
   true: Item was added
   false: No item was added
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_SyvTopologyBaseScene::m_AddOfMime(const QMimeData * const opc_MimeData, const QPointF & orc_Position)
{
   // Nothing to do
   Q_UNUSED(opc_MimeData)
   Q_UNUSED(orc_Position)

   return false;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Copy items
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvTopologyBaseScene::m_Copy(void)
{
   // Nothing to do
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Cut items
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvTopologyBaseScene::m_Cut(void)
{
   // Nothing to do
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Check active busses

   \return
   NULL No active bus
   Else Active bus
*/
//----------------------------------------------------------------------------------------------------------------------
C_GiLiBus * C_SyvTopologyBaseScene::m_CheckBusState(void) const
{
   C_GiLiBus * pc_Retval = NULL;
   const QList<QGraphicsItem *> & rc_Items = this->items();

   QList<QGraphicsItem *>::const_iterator c_ItItem;
   for (c_ItItem = rc_Items.begin(); c_ItItem != rc_Items.end(); ++c_ItItem)
   {
      //lint -e{929}  false positive in PC-Lint: allowed by MISRA 5-2-2
      C_GiLiBus * const pc_Item = dynamic_cast<C_GiLiBus *>(*c_ItItem);
      if (pc_Item != NULL)
      {
         const bool q_Disabled = C_PuiSvHandler::h_GetInstance()->CheckBusDisabled(this->mu32_ViewIndex,
                                                                                   pc_Item->GetIndex());
         //Change look
         pc_Item->SetDisabledLook(q_Disabled);
         //Update return value
         if (q_Disabled == false)
         {
            pc_Retval = pc_Item;
         }
         //Update error
         pc_Item->CheckBusForChanges();
      }
      else
      {
         //lint -e{929}  false positive in PC-Lint: allowed by MISRA 5-2-2
         C_GiLiBusConnector * const pc_BusConnector = dynamic_cast<C_GiLiBusConnector *>(*c_ItItem);
         if (pc_BusConnector != NULL)
         {
            const C_GiNode * const pc_Node = pc_BusConnector->GetNodeItem();
            const C_PuiSvData * const pc_View = C_PuiSvHandler::h_GetInstance()->GetView(this->mu32_ViewIndex);
            tgl_assert((pc_Node != NULL) && (pc_View != NULL));
            if ((pc_Node != NULL) && (pc_View != NULL))
            {
               const std::vector<uint8> & rc_NodeActiveFlages = pc_View->GetNodeActiveFlags();
               const sint32 s32_NodeIndex = pc_Node->GetIndex();
               tgl_assert((s32_NodeIndex >= 0) && (static_cast<uint32>(s32_NodeIndex) < rc_NodeActiveFlages.size()));
               if ((s32_NodeIndex >= 0) && (static_cast<uint32>(s32_NodeIndex) < rc_NodeActiveFlages.size()))
               {
                  pc_BusConnector->SetDisabledLook(rc_NodeActiveFlages[static_cast<uint32>(s32_NodeIndex)] == false);
               }
            }
         }
      }
   }
   return pc_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Add PC - bus connection item

   \param[in]      opc_Bus       Bus item
   \param[in,out]  opc_Pc        PC item
   \param[in]      orc_LineData  Line data
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvTopologyBaseScene::m_AddPcBusConnector(const C_GiLiBus * const opc_Bus, C_GiBiConnectableItem * const opc_Pc,
                                                 const C_PuiBsLineBase & orc_LineData)
{
   std::vector<QPointF> c_UIInteractionPoints;
   //Initial points
   if (orc_LineData.c_UIInteractionPoints.size() == 0)
   {
      //lint -e{929}  false positive in PC-Lint: allowed by MISRA 5-2-2
      //lint -e{740}  no problem because of common base class
      const QGraphicsItem * const pc_GI = dynamic_cast<const QGraphicsItem * const>(opc_Pc);
      const QPointF c_BottomCenter = mh_GetInitialConnectionPoint(pc_GI);
      c_UIInteractionPoints.push_back(c_BottomCenter);
      c_UIInteractionPoints.push_back(c_BottomCenter);
   }
   else
   {
      c_UIInteractionPoints = orc_LineData.c_UIInteractionPoints;
   }
   if ((opc_Bus != NULL) && (opc_Pc != NULL))
   {
      //Create and add bus connection
      C_GiSvPcBusConnector * const pc_PcBusConnection = new C_GiSvPcBusConnector(
         this->m_GetNewUniqueID(), this->mu32_ViewIndex, c_UIInteractionPoints, opc_Bus, opc_Pc);
      //Update Z value
      pc_PcBusConnection->RestoreZOrder();
      //Save/Update
      pc_PcBusConnection->UpdateData();
      m_AddPcBusConnectorToScene(pc_PcBusConnection);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get initial connection point for any graphics item

   \param[in]  opc_Item    Any graphics item

   \return
   (0.0,0.0) Default
   Else      Bottom center of graphics item
*/
//----------------------------------------------------------------------------------------------------------------------
QPointF C_SyvTopologyBaseScene::mh_GetInitialConnectionPoint(const QGraphicsItem * const opc_Item)
{
   QPointF c_Retval(0.0, 0.0);

   if (opc_Item != NULL)
   {
      c_Retval = QPointF(opc_Item->sceneBoundingRect().center().x(), opc_Item->sceneBoundingRect().bottom());
   }
   return c_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Check if PC bus reconnection necessary

   \param[out]  orc_BusName   - Name of the new connected bus
                              - Empty string if no new bus found

   \return
   True  PC bus connection reconnected (Emit change necessary)
   False No change
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_SyvTopologyBaseScene::m_HandlePcReconnectIfNecessary(QString & orc_BusName)
{
   bool q_Retval = false;
   C_GiLiBus * pc_ValidBus;

   //Recheck busses if active
   pc_ValidBus = m_CheckBusState();
   orc_BusName = "";

   if (pc_ValidBus == NULL)
   {
      const QList<QGraphicsItem *> & rc_Items = this->items();
      std::vector<QGraphicsItem *> c_ItemsToBeDeleted;

      QList<QGraphicsItem *>::const_iterator c_ItItem;
      for (c_ItItem = rc_Items.begin(); c_ItItem != rc_Items.end(); ++c_ItItem)
      {
         //lint -e{929}  false positive in PC-Lint: allowed by MISRA 5-2-2
         C_GiSvPcBusConnector * const pc_Item = dynamic_cast<C_GiSvPcBusConnector *>(*c_ItItem);
         if (pc_Item != NULL)
         {
            c_ItemsToBeDeleted.push_back(pc_Item);
         }
      }
      //Delete connection if existing
      tgl_assert(c_ItemsToBeDeleted.size() <= 1);
      for (uint32 u32_ItItemToBeDeleted = 0; u32_ItItemToBeDeleted < c_ItemsToBeDeleted.size(); ++u32_ItItemToBeDeleted)
      {
         QGraphicsItem * const pc_ItemToBeDeleted = c_ItemsToBeDeleted[u32_ItItemToBeDeleted];
         delete (pc_ItemToBeDeleted);
         q_Retval = true;
      }
      //Save/Update
      C_PuiSvHandler::h_GetInstance()->SetViewPCConnected(this->mu32_ViewIndex, false, 0);
   }
   else
   {
      const C_PuiSvData * const pc_PcData = C_PuiSvHandler::h_GetInstance()->GetView(this->mu32_ViewIndex);
      bool q_Found = false;
      C_GiLiBus * pc_LastBus = NULL;
      const QList<QGraphicsItem *> & rc_Items = this->items();

      QList<QGraphicsItem *>::const_iterator c_ItItem;
      //Search for LAST connected bus index
      if (pc_PcData != NULL)
      {
         for (c_ItItem = rc_Items.begin(); c_ItItem != rc_Items.end(); ++c_ItItem)
         {
            //lint -e{929}  false positive in PC-Lint: allowed by MISRA 5-2-2
            C_GiLiBus * const pc_Item = dynamic_cast<C_GiLiBus *>(*c_ItItem);
            if (pc_Item != NULL)
            {
               if (static_cast<uint32>(pc_Item->GetIndex()) == pc_PcData->GetPcData().GetBusIndex())
               {
                  //Check if valid
                  const bool q_Disabled = C_PuiSvHandler::h_GetInstance()->CheckBusDisabled(this->mu32_ViewIndex,
                                                                                            pc_Item->GetIndex());
                  if (q_Disabled == false)
                  {
                     pc_LastBus = pc_Item;
                  }
                  //It's unlikely there is another bus with the same index
                  break;
               }
            }
         }
      }
      //Search for existing bus connection
      for (c_ItItem = rc_Items.begin(); c_ItItem != rc_Items.end(); ++c_ItItem)
      {
         //lint -e{929}  false positive in PC-Lint: allowed by MISRA 5-2-2
         C_GiSvPcBusConnector * const pc_PcConnector = dynamic_cast<C_GiSvPcBusConnector *>(*c_ItItem);
         if (pc_PcConnector != NULL)
         {
            const C_GiLiBus * const pc_CurBus = pc_PcConnector->GetBusItem();
            tgl_assert(pc_CurBus != NULL);
            if (pc_CurBus != NULL)
            {
               const bool q_Disabled = C_PuiSvHandler::h_GetInstance()->CheckBusDisabled(this->mu32_ViewIndex,
                                                                                         pc_CurBus->GetIndex());
               //Reconnect if necessary (== current bus invalid)
               if (q_Disabled == true)
               {
                  const C_GiBiConnectableItem * const pc_PC = pc_PcConnector->GetGenericPositionItem();
                  //lint -e{929}  false positive in PC-Lint: allowed by MISRA 5-2-2
                  //lint -e{740}  no problem because of common base class
                  const QGraphicsItem * const pc_GraphicsItem = dynamic_cast<const QGraphicsItem * const>(pc_PC);
                  if (pc_LastBus != NULL)
                  {
                     pc_PcConnector->RevertBus(pc_LastBus, NULL,
                                               C_SyvTopologyBaseScene::mh_GetInitialConnectionPoint(pc_GraphicsItem));
                     orc_BusName = pc_LastBus->GetName();
                  }
                  else
                  {
                     pc_PcConnector->RevertBus(pc_ValidBus, NULL,
                                               C_SyvTopologyBaseScene::mh_GetInitialConnectionPoint(pc_GraphicsItem));
                     orc_BusName = pc_ValidBus->GetName();
                  }
                  q_Retval = true;
                  //Update Z value
                  // (misuse of interface does no update of z value as the class thinks it has already the right one)
                  pc_PcConnector->RestoreZOrder();
                  //Save/Update
                  pc_PcConnector->UpdateData();
               }
            }
            //Signal no creation necessary
            q_Found = true;
         }
      }
      //Bus connector creation necessary
      if (q_Found == false)
      {
         C_GiSvPc * pc_PCItem = NULL;

         //Search pc item
         for (c_ItItem = rc_Items.begin(); c_ItItem != rc_Items.end(); ++c_ItItem)
         {
            //lint -e{929}  false positive in PC-Lint: allowed by MISRA 5-2-2
            C_GiSvPc * const pc_Item = dynamic_cast<C_GiSvPc *>(*c_ItItem);
            if (pc_Item != NULL)
            {
               pc_PCItem = pc_Item;
            }
         }
         if (pc_LastBus != NULL)
         {
            m_AddPcBusConnector(pc_LastBus, pc_PCItem);
            orc_BusName = pc_LastBus->GetName();
         }
         else
         {
            m_AddPcBusConnector(pc_ValidBus, pc_PCItem);
            orc_BusName = pc_ValidBus->GetName();
         }
         q_Retval = true;
      }
   }
   return q_Retval;
}
