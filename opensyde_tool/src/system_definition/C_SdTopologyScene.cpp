//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Main graphics scene for system topology (implementation)

   It handles the graphics item functionality.

   \copyright   Copyright 2016 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.h"

#include <climits>
#include <QGraphicsScene>
#include <QClipboard>
#include <QToolTip>
#include <QApplication>
#include <QGraphicsView>
#include <QElapsedTimer>

#include "constants.h"
#include "gitypes.h"

#include "C_OSCUtils.h"
#include "C_SdTopologyScene.h"
#include "C_GiLiCANBus.h"
#include "C_GiLiEthernetBus.h"
#include "C_GiBiCustomMouseItem.h"
#include "C_GiBiCustomToolTip.h"
#include "C_GiLiTemporaryLine.h"
#include "C_GiCustomFunctions.h"
#include "C_PuiSdNode.h"
#include "C_PuiSdBus.h"
#include "C_GtGetText.h"
#include "C_PuiProject.h"
#include "C_PuiSdHandler.h"
#include "C_SebUtil.h"
#include "C_SdUtil.h"
#include "TGLUtils.h"
#include "C_OgePopUpDialog.h"
#include "C_SdNodeComIfSetupWidget.h"
#include "C_SdNodeToNodeConnectionSetupWidget.h"
#include "C_SdManTopologyBusConnectorReconnectManager.h"
#include "C_OgeWiCustomMessage.h"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw_opensyde_gui;
using namespace stw_opensyde_gui_logic;
using namespace stw_opensyde_gui_elements;
using namespace stw_opensyde_core;
using namespace stw_types;
using namespace stw_tgl;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */
const QString mc_CAN_BUS = "CAN Bus";
const QString mc_ETHERNET_BUS = "Ethernet Bus";
const bool C_SdTopologyScene::mhq_NewConnectState = false;

/* -- Types --------------------------------------------------------------------------------------------------------- */

/* -- Global Variables ---------------------------------------------------------------------------------------------- */

/* -- Module Global Variables --------------------------------------------------------------------------------------- */

/* -- Module Global Function Prototypes ----------------------------------------------------------------------------- */

/* -- Implementation ------------------------------------------------------------------------------------------------ */

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default constructor

   \param[in]      orq_LoadSystemDefintion   Optional indicator if system definition should be loaded
   \param[in,out]  opc_Parent                Optional pointer to parent
*/
//----------------------------------------------------------------------------------------------------------------------
C_SdTopologyScene::C_SdTopologyScene(const bool & orq_LoadSystemDefintion, QObject * const opc_Parent) :
   C_SebTopologyBaseScene(opc_Parent),
   mpc_EmptyConnectItem(NULL),
   mpc_NodeConnectItem(NULL),
   mpc_BusConnectItem(NULL),
   mpc_ArrowCursorButton(new C_GiArrowCursorButton),
   mpc_ConnectorLine(NULL),
   mpc_BusNameLineBus(NULL),
   mpc_BusNameLine(NULL),
   mc_UndoManager(this),
   mq_ConnectState(false),
   mpc_SelectedBusConnectorItem(NULL),
   mq_RestoreMouseCursorWhenPossible(false),
   mu64_MouseOverrideCounter(0)
{
   // Init base scene. Initializing all parts which can not be initilized in the base constructor
   this->m_InitSceneUndoManager();
   this->m_InitSceneContextMenuManager();

   if (orq_LoadSystemDefintion == true)
   {
      //Load
      C_SdTopologyScene::Load();
   }

   //Arrow
   mpc_ArrowCursorButton->setVisible(false);
   this->addItem(mpc_ArrowCursorButton);
   connect(mpc_ArrowCursorButton, &C_GiArrowCursorButton::StartConnector, this, &C_SdTopologyScene::m_StartConnector);

   // configure context menu
   connect(&this->mc_ContextMenuManager, &C_SdManTopologyContextMenuManager::SigEdit,
           this, &C_SdTopologyScene::m_EditClicked);
   connect(&mc_ContextMenuManager, &C_SdManTopologyContextMenuManager::SigInterfaceAssignment,
           this, &C_SdTopologyScene::m_ShowInterfaceChangePopUp);
   //Reconnection manager
   connect(&(this->mc_BusConnectorReconnectManager), &C_SdManTopologyBusConnectorReconnectManager::SigCleanUpPorts,
           this, &C_SdTopologyScene::m_CleanUpPorts);
   connect(&(this->mc_BusConnectorReconnectManager), &C_SdManTopologyBusConnectorReconnectManager::SigReconnectNode,
           this, &C_SdTopologyScene::m_ReconnectBusConnectorNode);
   connect(&(this->mc_BusConnectorReconnectManager), &C_SdManTopologyBusConnectorReconnectManager::SigReconnectBus,
           this, &C_SdTopologyScene::m_ReconnectBusConnectorBus);
   connect(&(this->mc_BusConnectorReconnectManager), &C_SdManTopologyBusConnectorReconnectManager::SigRevertNode,
           this, &C_SdTopologyScene::m_RevertBusConnectorNode);
   connect(&(this->mc_BusConnectorReconnectManager), &C_SdManTopologyBusConnectorReconnectManager::SigRevertBus,
           this, &C_SdTopologyScene::m_RevertBusConnectorBus);
   connect(&(this->mc_BusConnectorReconnectManager),
           &C_SdManTopologyBusConnectorReconnectManager::SigCleanUpTemporaryLine,
           this, &C_SdTopologyScene::m_RemoveConnectorAndLeaveConnectState);

   //error handling
   connect(&this->mc_UndoManager, &C_SdManUnoTopologyManager::SigErrorChanged, this,
           &C_SdTopologyScene::SigErrorChange);

   // selection changed
   connect(this, &C_SdTopologyScene::selectionChanged, this, &C_SdTopologyScene::m_SelectionChanged);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   default destructor

   Clean up.
*/
//----------------------------------------------------------------------------------------------------------------------
//lint -e{1579}  no memory leak because of the parents of the items and the Qt memory management
C_SdTopologyScene::~C_SdTopologyScene()
{
   UpdateSystemDefinition();
   //lint -e{1540}  no memory leak because of the parents of the items and the Qt memory management
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Adds a new node

   \param[in]  orc_NodeType      Node type
   \param[in]  orc_Pos           Position to place item at
   \param[in]  opu64_UniqueID    Optional pointer to unique ID to use for new item
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdTopologyScene::AddNode(const QString & orc_NodeType, const QPointF & orc_Pos,
                                const uint64 * const opu64_UniqueID)
{
   C_GiNode * pc_Item;

   stw_opensyde_core::C_OSCNode c_OSCNode;
   stw_opensyde_gui_logic::C_PuiSdNode c_UINode;
   sint32 s32_Index;
   uint64 u64_UniqueID;

   //Check if a specific unique ID should be used
   if (opu64_UniqueID != NULL)
   {
      u64_UniqueID = *opu64_UniqueID;
   }
   else
   {
      u64_UniqueID = m_GetNewUniqueID();
   }

   //Selection
   this->clearSelection();

   //Object
   this->m_InitNodeData(c_OSCNode, orc_NodeType);
   //UI
   c_UINode.f64_ZOrder = this->GetHighestUsedZValueList(this->items()) + 1.0;
   s32_Index = C_PuiSdHandler::h_GetInstance()->AddNodeAndSort(c_OSCNode, c_UINode);
   this->m_SyncIndex(C_GiNode::eNODE, s32_Index, C_GiNode::eADD);

   //Graphics
   pc_Item = new C_GiNode(s32_Index, u64_UniqueID);
   pc_Item->setPos(orc_Pos);
   pc_Item->setZValue(c_UINode.f64_ZOrder);

   m_AddNodeToScene(pc_Item);

   //Selection
   m_UpdateSelection(pc_Item);

   Q_EMIT this->SigNodeChanged(s32_Index);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Adds a new CAN bus

   \param[in]  orc_Pos              Position to place item at
   \param[in]  opu64_UniqueID       Optional pointer to unique ID to use for new item
   \param[in]  of64_ZValue          Z value to use
   \param[in]  opc_TextElementBus   Pointer to bus text element
   \param[in]  opc_NameProposal     Name proposal
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdTopologyScene::AddCanBus(const QPointF & orc_Pos, const stw_types::uint64 * const opu64_UniqueID,
                                  const float64 of64_ZValue, C_GiTextElementBus * const opc_TextElementBus,
                                  const QString * const opc_NameProposal)
{
   std::vector<QPointF> c_Points;
   C_GiLiCANBus * pc_CANBus;
   C_OSCSystemBus c_OSCBus;
   C_PuiSdBus c_UIBus;
   sint32 s32_Index;
   uint64 u64_UniqueID;

   //Check if a specific unique ID should be used
   if (opu64_UniqueID != NULL)
   {
      u64_UniqueID = *opu64_UniqueID;
   }
   else
   {
      u64_UniqueID = m_GetNewUniqueID();
   }

   //Selection
   this->clearSelection();

   //Initial points
   c_Points.push_back(QPointF(std::max(0., orc_Pos.x() - 150.), orc_Pos.y()));
   c_Points.push_back(QPointF(orc_Pos.x() + 150., orc_Pos.y()));

   //Object
   c_OSCBus.e_Type = C_OSCSystemBus::E_Type::eCAN;
   c_OSCBus.c_Name = "NewCANBus";
   //UI
   c_UIBus.f64_ZOrder = of64_ZValue;
   s32_Index = C_PuiSdHandler::h_GetInstance()->AddBusAndSort(c_OSCBus, c_UIBus, opc_NameProposal);
   this->m_SyncIndex(C_GiLiCANBus::eBUS, s32_Index, C_GiLiCANBus::eADD);

   // Create specific text element for showing bus name

   //Configure bus
   pc_CANBus = new C_GiLiCANBus(s32_Index, u64_UniqueID, opc_TextElementBus, true, &c_Points);
   pc_CANBus->SetWidth(6);
   pc_CANBus->SetColor(mc_STYLE_GUIDE_COLOR_10);
   pc_CANBus->setZValue(c_UIBus.f64_ZOrder);

   //Add to scene
   this->m_AddBusToScene(pc_CANBus);

   Q_EMIT this->SigBusChanged(s32_Index);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Adds a new Ethernet bus

   \param[in]  orc_Pos              Position to place item at
   \param[in]  opu64_UniqueID       Optional pointer to unique ID to use for new item
   \param[in]  of64_ZValue          Z value to use
   \param[in]  opc_TextElementBus   Pointer to bus text element
   \param[in]  opc_NameProposal     Name proposal
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdTopologyScene::AddEthernetBus(const QPointF & orc_Pos, const stw_types::uint64 * const opu64_UniqueID,
                                       const float64 of64_ZValue, C_GiTextElementBus * const opc_TextElementBus,
                                       const QString * const opc_NameProposal)
{
   std::vector<QPointF> c_Points;
   C_GiLiEthernetBus * pc_EthernetBus;
   C_PuiSdBus c_UIBus;
   C_OSCSystemBus c_OSCBus;
   sint32 s32_Index;
   uint64 u64_UniqueID;

   //Check if a specific unique ID should be used
   if (opu64_UniqueID != NULL)
   {
      u64_UniqueID = *opu64_UniqueID;
   }
   else
   {
      u64_UniqueID = m_GetNewUniqueID();
   }

   //Selection
   this->clearSelection();

   //Initial points
   c_Points.push_back(QPointF(std::max(0., orc_Pos.x() - 150.), orc_Pos.y()));
   c_Points.push_back(QPointF(orc_Pos.x() + 150., orc_Pos.y()));

   //Object
   c_OSCBus.e_Type = C_OSCSystemBus::E_Type::eETHERNET;
   c_OSCBus.c_Name = "NewEthernetBus";
   //UI
   c_UIBus.f64_ZOrder = of64_ZValue;
   s32_Index = C_PuiSdHandler::h_GetInstance()->AddBusAndSort(c_OSCBus, c_UIBus, opc_NameProposal);
   this->m_SyncIndex(C_GiLiEthernetBus::eBUS, s32_Index, C_GiLiEthernetBus::eADD);
   //Configure bus
   pc_EthernetBus = new C_GiLiEthernetBus(s32_Index, u64_UniqueID, opc_TextElementBus, true, &c_Points);
   pc_EthernetBus->SetWidth(6);
   pc_EthernetBus->SetColor(mc_STYLE_GUIDE_COLOR_5413);
   pc_EthernetBus->setZValue(c_UIBus.f64_ZOrder);

   //Add to scene
   this->m_AddBusToScene(pc_EthernetBus);

   Q_EMIT this->SigBusChanged(s32_Index);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Adds a new boundary

   \param[in]  orc_Pos           Position to place item at
   \param[in]  opu64_UniqueID    Optional pointer to unique ID to use for new item
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdTopologyScene::AddBoundary(const QPointF & orc_Pos, const stw_types::uint64 * const opu64_UniqueID)
{
   C_PuiBsBoundary c_BoundaryData;
   C_GiSdBoundary * pc_Item;
   sint32 s32_Index;
   uint64 u64_UniqueID;

   //Check if a specific unique ID should be used
   if (opu64_UniqueID != NULL)
   {
      u64_UniqueID = *opu64_UniqueID;
   }
   else
   {
      u64_UniqueID = m_GetNewUniqueID();
   }

   s32_Index = static_cast<sint32>(C_PuiSdHandler::h_GetInstance()->c_Elements.c_Boundaries.size());

   //Default
   c_BoundaryData.c_UIPosition = orc_Pos;
   c_BoundaryData.f64_Height = 100.0;
   c_BoundaryData.f64_Width = 100.0;
   c_BoundaryData.f64_ZOrder = this->GetHighestUsedZValueList(this->items()) + 1.0;

   C_PuiSdHandler::h_GetInstance()->c_Elements.c_Boundaries.push_back(c_BoundaryData);

   this->m_SyncIndex(C_PuiSdDataElement::eBOUNDARY, s32_Index, C_PuiSdDataElement::eADD);
   pc_Item = new C_GiSdBoundary(s32_Index, u64_UniqueID, c_BoundaryData.f64_Width, c_BoundaryData.f64_Height);

   pc_Item->LoadData();

   m_AddBoundaryToScene(pc_Item);

   //Selection
   m_UpdateSelection(pc_Item);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Adds a new text element

   \param[in]  orc_Text          Initial text
   \param[in]  orc_Pos           Position to place item at
   \param[in]  opu64_UniqueID    Optional pointer to unique ID to use for new item
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdTopologyScene::AddTextElement(const QString & orc_Text, const QPointF & orc_Pos,
                                       const stw_types::uint64 * const opu64_UniqueID)
{
   C_PuiBsTextElement c_TextElementData;
   C_GiSdTextElement * pc_Item;
   sint32 s32_Index;
   uint64 u64_UniqueID;

   //Check if a specific unique ID should be used
   if (opu64_UniqueID != NULL)
   {
      u64_UniqueID = *opu64_UniqueID;
   }
   else
   {
      u64_UniqueID = m_GetNewUniqueID();
   }

   s32_Index = static_cast<sint32>(C_PuiSdHandler::h_GetInstance()->c_Elements.c_TextElements.size());

   //Default
   c_TextElementData.c_UIText = orc_Text;
   c_TextElementData.c_UIPosition = orc_Pos;
   c_TextElementData.f64_ZOrder = this->GetHighestUsedZValueList(this->items()) + 1.0;

   C_PuiSdHandler::h_GetInstance()->c_Elements.c_TextElements.push_back(c_TextElementData);

   //Selection
   this->clearSelection();

   this->m_SyncIndex(C_PuiSdDataElement::eTEXT_ELEMENT, s32_Index, C_PuiSdDataElement::eADD);
   pc_Item = new C_GiSdTextElement(s32_Index, u64_UniqueID);

   pc_Item->LoadData();

   m_AddTextElementToScene(pc_Item);

   //Selection
   m_UpdateSelection(pc_Item);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Adds a new text element for a concrete bus

   \param[in]   opu64_UniqueID   Optional pointer to unique ID to use for new item
   \param[out]  orf64_BusZValue  Z value proposal for bus

   \return
   Pointer to bus text element
*/
//----------------------------------------------------------------------------------------------------------------------
C_GiTextElementBus * C_SdTopologyScene::AddTextElementBus(const uint64 * const opu64_UniqueID,
                                                          float64 & orf64_BusZValue)
{
   C_GiTextElementBus * pc_Item;
   sint32 s32_Index;
   uint64 u64_UniqueID;
   C_PuiSdTextElementBus c_Data;

   //Check if a specific unique ID should be used
   if (opu64_UniqueID != NULL)
   {
      u64_UniqueID = *opu64_UniqueID;
   }
   else
   {
      u64_UniqueID = m_GetNewUniqueID();
   }

   s32_Index = static_cast<sint32>(C_PuiSdHandler::h_GetInstance()->c_BusTextElements.size());
   //Get next free z value
   orf64_BusZValue = this->GetHighestUsedZValueList(this->items()) + 1.0;
   //Place text element over the bus
   c_Data.f64_ZOrder = orf64_BusZValue + 1.0;
   C_PuiSdHandler::h_GetInstance()->c_BusTextElements.push_back(c_Data);

   this->m_SyncIndex(C_PuiSdDataElement::eTEXT_ELEMENT_BUS, s32_Index, C_PuiSdDataElement::eADD);
   pc_Item = this->m_CreateBusTextElement(s32_Index, u64_UniqueID, NULL);
   pc_Item->setZValue(c_Data.f64_ZOrder);

   m_AddBusTextElementToScene(pc_Item);

   return pc_Item;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Adds a new line

   \param[in]  orc_Pos           Position to place item at
   \param[in]  opu64_UniqueID    Optional pointer to unique ID to use for new item
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdTopologyScene::AddLine(const QPointF & orc_Pos, const stw_types::uint64 * const opu64_UniqueID)
{
   C_PuiBsLineArrow c_LineArrowData;
   C_GiSdArrow * pc_Item;

   sint32 s32_Index;
   uint64 u64_UniqueID;

   //Check if a specific unique ID should be used
   if (opu64_UniqueID != NULL)
   {
      u64_UniqueID = *opu64_UniqueID;
   }
   else
   {
      u64_UniqueID = m_GetNewUniqueID();
   }

   s32_Index = static_cast<sint32>(C_PuiSdHandler::h_GetInstance()->c_Elements.c_LineArrows.size());

   //Default c_LineArrowData
   c_LineArrowData.f64_ZOrder = this->GetHighestUsedZValueList(this->items()) + 1.0;
   c_LineArrowData.c_UIInteractionPoints.push_back(orc_Pos);
   c_LineArrowData.c_UIInteractionPoints.push_back(orc_Pos + QPointF(200.0, 0.0));

   C_PuiSdHandler::h_GetInstance()->c_Elements.c_LineArrows.push_back(c_LineArrowData);

   //Selection
   this->clearSelection();

   //Configure line arrow
   this->m_SyncIndex(C_PuiSdDataElement::eLINE_ARROW, s32_Index, C_PuiSdDataElement::eADD);
   pc_Item = new C_GiSdArrow(s32_Index, u64_UniqueID, &c_LineArrowData.c_UIInteractionPoints);

   pc_Item->LoadData();

   //Add to scene
   this->m_AddLineArrowToScene(pc_Item);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Adds a new bus connector

   \param[in,out]  opc_Node                  Node to connect to
   \param[in]      opc_Bus                   Bus to connect to
   \param[in]      oru8_InterfaceNumber      Number of interface to use
   \param[in]      oru8_NodeId               New node id
   \param[in]      oq_ActivateDatapoolL2     Activate datapool L2
   \param[in]      oq_ActivateDatapoolECeS   Activate datapool ECeS
   \param[in]      oq_ActivateDatapoolECoS   Activate datapool ECoS
   \param[in]      orc_Pos                   Position to place item at
   \param[in]      opu64_UniqueID            Optional pointer to unique ID to use for new item
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdTopologyScene::AddBusConnector(C_GiNode * const opc_Node, const C_GiLiBus * const opc_Bus,
                                        const stw_types::uint8 & oru8_InterfaceNumber,
                                        const stw_types::uint8 & oru8_NodeId, const bool oq_ActivateDatapoolL2,
                                        const bool oq_ActivateDatapoolECeS, const bool oq_ActivateDatapoolECoS,
                                        const QPointF & orc_Pos, const stw_types::uint64 * const opu64_UniqueID)
{
   if ((opc_Node != NULL) &&
       (opc_Bus != NULL))
   {
      std::vector<QPointF> c_Points;
      uint64 u64_UniqueID;

      //Check if a specific unique ID should be used
      if (opu64_UniqueID != NULL)
      {
         u64_UniqueID = *opu64_UniqueID;
      }
      else
      {
         u64_UniqueID = m_GetNewUniqueID();
      }

      //Set up connection
      C_PuiSdHandler::h_GetInstance()->AddConnection(
         static_cast<uint32>(opc_Node->GetIndex()), oru8_InterfaceNumber, oru8_NodeId,
         static_cast<uint32>(opc_Bus->GetIndex()));

      //Configure DP
      if (opc_Bus->GetType() == C_OSCSystemBus::eCAN)
      {
         C_SdUtil::h_ConfigureComDatapools(
            opc_Node->GetIndex(), oru8_InterfaceNumber, oq_ActivateDatapoolL2, oq_ActivateDatapoolECeS,
            oq_ActivateDatapoolECoS);
      }

      {
         //Start
         c_Points.push_back(orc_Pos);
         //End
         c_Points.push_back(orc_Pos);
      }
      // Connect the node to the bus
      C_GiLiBusConnector * const pc_Connector = new C_GiLiBusConnector(u64_UniqueID, c_Points,
                                                                       opc_Node,
                                                                       opc_Bus);
      //Initial z order
      pc_Connector->RestoreZOrder();
      opc_Node->AddConnection(pc_Connector);
      m_AddBusConnectorToScene(pc_Connector);

      m_UpdateHints();
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Signal for update of current scaling

   \param[in]  orc_Transform  Current scaling
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdTopologyScene::UpdateTransform(const QTransform & orc_Transform)
{
   //Not necessary with new connect icon
   //QList<QGraphicsItem *> c_Items = this->items();
   //for (QList<QGraphicsItem *>::iterator c_ItItem = c_Items.begin(); c_ItItem != c_Items.end(); ++c_ItItem)
   //{
   //   QGraphicsItem * const pc_Parent = C_SebUtil::h_GetHighestParent(*c_ItItem);
   //   C_GiArrowCursorButton * const pc_ArrowCursorButton = dynamic_cast<C_GiArrowCursorButton * const>(pc_Parent);
   //   if (pc_ArrowCursorButton != NULL)
   //   {
   //      pc_ArrowCursorButton->UpdateTransform(orc_Transform);
   //   }
   //}
   C_SebTopologyBaseScene::UpdateTransform(orc_Transform);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Adds a new image

   \param[in]  orc_FilePath      Image file path
   \param[in]  orc_Pos           Position to place item at
   \param[in]  opu64_UniqueID    Optional pointer to unique ID to use for new item
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdTopologyScene::AddImage(const QString & orc_FilePath, const QPointF & orc_Pos,
                                 const stw_types::uint64 * const opu64_UniqueID)
{
   C_GiSdImageGroup * pc_Item;
   sint32 s32_Index;
   uint64 u64_UniqueID;
   C_PuiBsImage c_ImageData;

   //Check if a specific unique ID should be used
   if (opu64_UniqueID != NULL)
   {
      u64_UniqueID = *opu64_UniqueID;
   }
   else
   {
      u64_UniqueID = m_GetNewUniqueID();
   }

   //Default
   c_ImageData.f64_ZOrder = this->GetHighestUsedZValueList(this->items()) + 1.0;

   s32_Index = static_cast<sint32>(C_PuiSdHandler::h_GetInstance()->c_Elements.c_Images.size());
   C_PuiSdHandler::h_GetInstance()->c_Elements.c_Images.push_back(c_ImageData);

   this->m_SyncIndex(C_PuiSdDataElement::eIMAGE, s32_Index, C_PuiSdDataElement::eADD);
   pc_Item = new C_GiSdImageGroup(s32_Index, u64_UniqueID, orc_FilePath);

   pc_Item->setPos(orc_Pos);
   pc_Item->setZValue(c_ImageData.f64_ZOrder);

   m_AddImageGroupToScene(pc_Item);

   //Selection
   m_UpdateSelection(pc_Item);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Copy items to scene from copy paste manager

   \param[in]  opc_Pos  Optional position offset
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdTopologyScene::CopyFromManagerToScene(const QPointF * const opc_Pos)
{
   QGraphicsView * const pc_View = this->views().at(0);
   const C_PuiBsElements * const pc_Data = this->mc_CopyPasteManager.GetSnapshot(pc_View);

   const C_SdTopologyDataSnapshot * const pc_SnapShot =
      dynamic_cast<const C_SdTopologyDataSnapshot * const>(pc_Data);

   if (pc_SnapShot != NULL)
   {
      const QPointF c_Offset = QPointF(10.0, 10.0);
      const uint32 u32_ItemCount = pc_SnapShot->Count();

      std::vector<uint64> c_UniqueIDs;
      QPointF c_TotalOffset;
      if (opc_Pos == NULL)
      {
         //Add point offset
         this->mc_CopyPasteManager.IncrementPasteCounter();
         c_TotalOffset = static_cast<float64>(this->mc_CopyPasteManager.GetPasteCounter()) * c_Offset;
      }
      else
      {
         //Set absolute position
         c_TotalOffset = this->mc_CopyPasteManager.GetDiff(*opc_Pos);
      }
      c_UniqueIDs.resize(u32_ItemCount);
      for (uint32 u32_ItItem = 0; u32_ItItem < u32_ItemCount; ++u32_ItItem)
      {
         c_UniqueIDs[u32_ItItem] = m_GetNewUniqueID();
      }
      this->mc_UndoManager.DoAddSnapshot(c_UniqueIDs, *pc_SnapShot, c_TotalOffset,
                                         this->GetHighestUsedZValueList(this->items()));
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Copy snapshot to scene

   \param[in]  orc_Snapshot   Object snapshot
   \param[in]  opc_IDMap      Optional map for IDs to use
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdTopologyScene::CopyFromSnapshotToScene(const stw_opensyde_gui_logic::C_SdTopologyDataSnapshot & orc_Snapshot,
                                                const QMap<C_PuiBsTemporaryDataID,
                                                           uint64> * const opc_IDMap)
{
   uint32 u32_ItElem;

   QMap<uint32, uint32> c_MapOldBusIndexToNewBusIndex;
   QVector<uint32> c_NodeIndices;
   QVector<uint32> c_BusIndices;
   /*
   0: Boundary
   1: Text element
   2: Line arrow
   3: Image
   4: Bus text element
   */
   QVector<uint32> c_OtherIndices;
   //Copy buses
   for (u32_ItElem = 0; u32_ItElem < orc_Snapshot.c_OSCBuses.size(); ++u32_ItElem)
   {
      C_OSCSystemBus c_OSCBus = orc_Snapshot.c_OSCBuses[u32_ItElem];
      const C_PuiSdBus & rc_UIBus = orc_Snapshot.c_UIBuses[u32_ItElem];
      const uint32 u32_NewBusIndex = C_PuiSdHandler::h_GetInstance()->AddBusAndSort(c_OSCBus, rc_UIBus, NULL, false);
      //Remember new index
      mh_AddAndUpdateHigher(c_BusIndices, u32_NewBusIndex);
      this->m_SyncIndex(C_PuiSdDataElement::eBUS,
                        c_BusIndices[c_BusIndices.size() - 1], C_PuiSdDataElement::eADD);
      //Map old to new index
      c_MapOldBusIndexToNewBusIndex.insert(u32_ItElem, u32_NewBusIndex);
   }
   //Copy nodes
   for (u32_ItElem = 0; u32_ItElem < orc_Snapshot.c_OSCNodes.size(); ++u32_ItElem)
   {
      C_OSCNode c_OSCInitialNode = orc_Snapshot.c_OSCNodes[u32_ItElem];
      const uint32 u32_NewNodeIndex = C_PuiSdHandler::h_GetInstance()->AddNodeAndSort(c_OSCInitialNode,
                                                                                      orc_Snapshot.c_UINodes[u32_ItElem]);
      const C_OSCNode * const pc_OSCNode = C_PuiSdHandler::h_GetInstance()->GetOSCNodeConst(u32_NewNodeIndex);
      const C_PuiSdNode * const pc_UINode = C_PuiSdHandler::h_GetInstance()->GetUINode(u32_NewNodeIndex);
      //Remember new index
      mh_AddAndUpdateHigher(c_NodeIndices, u32_NewNodeIndex);
      this->m_SyncIndex(C_PuiSdDataElement::eNODE, u32_NewNodeIndex, C_PuiSdDataElement::eADD);
      //Replace old connection names to new ones and remove connection if bus was not copied
      if ((pc_OSCNode != NULL) && (pc_UINode != NULL))
      {
         C_OSCNodeProperties c_OSCAdaptedNodeproperties = pc_OSCNode->c_Properties;
         std::vector<C_PuiSdNodeConnection> c_UIAdaptedNodeConnections = pc_UINode->c_UIBusConnections;
         for (std::vector<C_PuiSdNodeConnection>::iterator c_ItConn = c_UIAdaptedNodeConnections.begin();
              c_ItConn != c_UIAdaptedNodeConnections.end();)
         {
            uint32 u32_BusIndex = std::numeric_limits<uint32>::max();
            C_PuiSdNodeConnection & rc_CurConn = *c_ItConn;

            //Get according bus index
            for (uint32 u32_ItComInterface = 0;
                 u32_ItComInterface < c_OSCAdaptedNodeproperties.c_ComInterfaces.size();
                 ++u32_ItComInterface)
            {
               C_OSCNodeComInterfaceSettings & rc_ComInt =
                  c_OSCAdaptedNodeproperties.c_ComInterfaces[u32_ItComInterface];
               if (rc_ComInt.q_IsBusConnected == true)
               {
                  if ((rc_CurConn.c_ConnectionID.e_InterfaceType == rc_ComInt.e_InterfaceType) &&
                      (rc_CurConn.c_ConnectionID.u8_InterfaceNumber == rc_ComInt.u8_InterfaceNumber))
                  {
                     //Replace bus index
                     u32_BusIndex = rc_ComInt.u32_BusIndex;
                     u32_BusIndex =
                        c_MapOldBusIndexToNewBusIndex.value(u32_BusIndex, std::numeric_limits<uint32>::max());
                     tgl_assert(u32_BusIndex != std::numeric_limits<uint32>::max());
                     rc_ComInt.u32_BusIndex = u32_BusIndex;
                  }
               }
            }
            if (u32_BusIndex == std::numeric_limits<uint32>::max())
            {
               //Sync core and UI data
               c_OSCAdaptedNodeproperties.DisconnectComInterface(rc_CurConn.c_ConnectionID.e_InterfaceType,
                                                                 rc_CurConn.c_ConnectionID.u8_InterfaceNumber);
               c_ItConn = c_UIAdaptedNodeConnections.erase(c_ItConn);
            }
            else
            {
               ++c_ItConn;
            }
         }
         C_PuiSdHandler::h_GetInstance()->SetOSCNodeProperties(u32_NewNodeIndex, c_OSCAdaptedNodeproperties);
         C_PuiSdHandler::h_GetInstance()->SetUINodeConnections(u32_NewNodeIndex, c_UIAdaptedNodeConnections);
      }
   }
   //Copy other elements
   c_OtherIndices.push_back(C_PuiSdHandler::h_GetInstance()->c_Elements.c_Boundaries.size());
   c_OtherIndices.push_back(C_PuiSdHandler::h_GetInstance()->c_Elements.c_TextElements.size());
   c_OtherIndices.push_back(C_PuiSdHandler::h_GetInstance()->c_Elements.c_LineArrows.size());
   c_OtherIndices.push_back(C_PuiSdHandler::h_GetInstance()->c_Elements.c_Images.size());
   c_OtherIndices.push_back(C_PuiSdHandler::h_GetInstance()->c_BusTextElements.size());
   for (u32_ItElem = 0; u32_ItElem < orc_Snapshot.c_Boundaries.size(); ++u32_ItElem)
   {
      C_PuiSdHandler::h_GetInstance()->c_Elements.c_Boundaries.push_back(orc_Snapshot.c_Boundaries[u32_ItElem]);
   }
   for (u32_ItElem = 0; u32_ItElem < orc_Snapshot.c_TextElements.size(); ++u32_ItElem)
   {
      C_PuiSdHandler::h_GetInstance()->c_Elements.c_TextElements.push_back(orc_Snapshot.c_TextElements[u32_ItElem]);
   }
   for (u32_ItElem = 0; u32_ItElem < orc_Snapshot.c_BusTextElements.size(); ++u32_ItElem)
   {
      C_PuiSdTextElementBus c_BusElement = orc_Snapshot.c_BusTextElements[u32_ItElem];
      c_BusElement.u32_BusIndex =
         c_MapOldBusIndexToNewBusIndex.value(c_BusElement.u32_BusIndex, std::numeric_limits<uint32>::max());
      tgl_assert(c_BusElement.u32_BusIndex != std::numeric_limits<uint32>::max());
      C_PuiSdHandler::h_GetInstance()->c_BusTextElements.push_back(c_BusElement);
   }
   for (u32_ItElem = 0; u32_ItElem < orc_Snapshot.c_LineArrows.size(); ++u32_ItElem)
   {
      C_PuiSdHandler::h_GetInstance()->c_Elements.c_LineArrows.push_back(orc_Snapshot.c_LineArrows[u32_ItElem]);
   }
   for (u32_ItElem = 0; u32_ItElem < orc_Snapshot.c_Images.size(); ++u32_ItElem)
   {
      C_PuiSdHandler::h_GetInstance()->c_Elements.c_Images.push_back(orc_Snapshot.c_Images[u32_ItElem]);
   }
   m_LoadSnapshot(c_NodeIndices, c_BusIndices, c_OtherIndices, true, &(orc_Snapshot.c_BusConnections), opc_IDMap);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Save system definition
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdTopologyScene::Save(void) const
{
   UpdateSystemDefinition();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Clear item

   Hint: leaves all items which are not included in the system definition

   \param[in,out]  opc_Item   Pointer to item which may be deleted
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdTopologyScene::DeleteItem(QGraphicsItem * const opc_Item)
{
   // if the selected item is a bus, the bus class itself is the parent
   QGraphicsItem * const pc_Item = C_SebUtil::h_GetHighestParent(opc_Item);

   if (pc_Item != NULL)
   {
      if (pc_Item->type() != msn_GRAPHICS_ITEM_ARROWCURSORBTN)
      {
         if (pc_Item == this->mpc_SelectedBusConnectorItem)
         {
            // single selected item will be deleted
            this->mpc_SelectedBusConnectorItem = NULL;
         }
         C_PuiSdDataElement * const pc_DataElement = dynamic_cast<C_PuiSdDataElement *>(pc_Item);
         QObject * const pc_Object = dynamic_cast<QObject *>(pc_Item);
         C_GiLiBusConnector * const pc_Connector = dynamic_cast<C_GiLiBusConnector *>(pc_Item);

         // clean up specific things
         if (pc_Connector != NULL)
         {
            m_RemoveBusConnectorOfScene(pc_Connector);
         }
         else
         {
            C_GiLiBus * const pc_Bus = dynamic_cast<C_GiLiBus *>(pc_Item);
            if (pc_Bus != NULL)
            {
               Q_EMIT this->SigBusDeleted(static_cast<uint32>(pc_Bus->GetIndex()));
               m_RemoveBusOfScene(pc_Bus);
            }
            else
            {
               C_GiNode * const pc_Node = dynamic_cast<C_GiNode *>(pc_Item);
               if (pc_Node != NULL)
               {
                  Q_EMIT this->SigNodeDeleted(static_cast<uint32>(pc_Node->GetIndex()));
                  m_RemoveNodeOfScene(pc_Node);
               }
               else
               {
                  C_GiSdBoundary * const pc_Boundary = dynamic_cast<C_GiSdBoundary *>(pc_Item);
                  if (pc_Boundary != NULL)
                  {
                     m_RemoveBoundaryOfScene(pc_Boundary);
                  }
                  else
                  {
                     C_GiTextElementBus * const pc_BusTextElement = dynamic_cast<C_GiTextElementBus *>(pc_Item);
                     if (pc_BusTextElement != NULL)
                     {
                        m_RemoveTextElementBusOfScene(pc_BusTextElement);
                     }
                     else
                     {
                        C_GiSdTextElement * const pc_TextElement = dynamic_cast<C_GiSdTextElement *>(pc_Item);
                        if (pc_TextElement != NULL)
                        {
                           m_RemoveTextElementOfScene(pc_TextElement);
                        }
                        else
                        {
                           C_GiSdImageGroup * const pc_ImageGroup = dynamic_cast<C_GiSdImageGroup *>(pc_Item);
                           if (pc_ImageGroup != NULL)
                           {
                              m_RemoveImageGroupOfScene(pc_ImageGroup);
                           }
                           else
                           {
                              C_GiSdArrow * const pc_Arrow = dynamic_cast<C_GiSdArrow *>(pc_Item);
                              if (pc_Arrow != NULL)
                              {
                                 m_RemoveLineArrowOfScene(pc_Arrow);
                              }
                           }
                        }
                     }
                  }
               }
            }
         }

         // common data delete function
         if (pc_DataElement != NULL)
         {
            pc_DataElement->DeleteData();
         }

         // remove from scene TODO: investigate delete bug
         //this->removeItem(pc_Item);

         // common object clean up
         if (pc_Object != NULL)
         {
            pc_Object->deleteLater();
         }
         else
         {
            //Delete
            delete (pc_Item);
         }

         //Clean up tool tip if any existing
         this->m_HandleHideToolTip();
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Checking if the graphics item is movable on the scene

   \param[in]  opc_Item    Item to check

   \return
   true     Item is movable
   false    Item is not movable
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_SdTopologyScene::IsItemMovable(const QGraphicsItem * const opc_Item) const
{
   bool q_Return = false;
   const sintn sn_Type = opc_Item->type();

   if ((sn_Type == msn_GRAPHICS_ITEM_NODE) ||
       (sn_Type == msn_GRAPHICS_ITEM_BUS) ||
       (sn_Type == msn_GRAPHICS_ITEM_CANBUS) ||
       (sn_Type == msn_GRAPHICS_ITEM_ETHERNETBUS) ||
       (sn_Type == msn_GRAPHICS_ITEM_TEXTELEMENT) ||
       (sn_Type == msn_GRAPHICS_ITEM_TEXTELEMENT_BUS) ||
       (sn_Type == msn_GRAPHICS_ITEM_LINE_ARROW) ||
       (sn_Type == msn_GRAPHICS_ITEM_IMAGE) ||
       (sn_Type == msn_GRAPHICS_ITEM_BOUNDARY) ||
       (sn_Type == msn_GRAPHICS_ITEM_BUS_CONNECT))
   {
      q_Return = true;
   }

   return q_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Checking if the graphics item is selectable on the scene

   \param[in]  opc_Item    Item to check

   \return
   true     Item is selectable
   false    Item is not selectable
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_SdTopologyScene::IsItemSelectable(const QGraphicsItem * const opc_Item) const
{
   bool q_Return = false;
   const sintn sn_Type = opc_Item->type();

   if ((sn_Type == msn_GRAPHICS_ITEM_NODE) ||
       (sn_Type == msn_GRAPHICS_ITEM_BUS) ||
       (sn_Type == msn_GRAPHICS_ITEM_CANBUS) ||
       (sn_Type == msn_GRAPHICS_ITEM_ETHERNETBUS) ||
       (sn_Type == msn_GRAPHICS_ITEM_TEXTELEMENT) ||
       (sn_Type == msn_GRAPHICS_ITEM_TEXTELEMENT_BUS) ||
       (sn_Type == msn_GRAPHICS_ITEM_LINE_ARROW) ||
       (sn_Type == msn_GRAPHICS_ITEM_IMAGE) ||
       (sn_Type == msn_GRAPHICS_ITEM_BOUNDARY) ||
       (sn_Type == msn_GRAPHICS_ITEM_ARROWCURSORBTN) ||
       (sn_Type == msn_GRAPHICS_ITEM_BUS_CONNECT))
   {
      q_Return = true;
   }

   return q_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Checking if the graphics item is deletable on the scene

   \param[in]  opc_Item    Item to check

   \return
   true     Item is deletable
   false    Item is not deletable
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_SdTopologyScene::IsItemDeletable(const QGraphicsItem * const opc_Item) const
{
   bool q_Return = true;

   if (opc_Item->type() == msn_GRAPHICS_ITEM_TEXTELEMENT_BUS)
   {
      q_Return = false;
   }

   return q_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Checking if the graphics item is changeable in the zorder

   \param[in]  opc_Item    Item to check

   \return
   true     Z order is changeable
   false    Z order is not changeable
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_SdTopologyScene::IsZOrderChangeable(const QGraphicsItem * const opc_Item) const
{
   bool q_Return = false;

   // check if the graphics items z order is changeable
   const sintn sn_Type = opc_Item->type();

   if ((sn_Type == msn_GRAPHICS_ITEM_NODE) ||
       (sn_Type == msn_GRAPHICS_ITEM_BUS) ||
       (sn_Type == msn_GRAPHICS_ITEM_CANBUS) ||
       (sn_Type == msn_GRAPHICS_ITEM_ETHERNETBUS) ||
       (sn_Type == msn_GRAPHICS_ITEM_TEXTELEMENT) ||
       (sn_Type == msn_GRAPHICS_ITEM_TEXTELEMENT_BUS) ||
       (sn_Type == msn_GRAPHICS_ITEM_LINE_ARROW) ||
       (sn_Type == msn_GRAPHICS_ITEM_IMAGE) ||
       (sn_Type == msn_GRAPHICS_ITEM_BOUNDARY) ||
       (sn_Type == msn_GRAPHICS_ITEM_BUS_CONNECT))
   {
      q_Return = true;
   }

   return q_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Checking if the graphics item can be aligned

   \param[in]  opc_Item    Item to check

   \return
   true     Item can be aligned
   false    Item can not be alined
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_SdTopologyScene::IsAlignmentUsable(const QGraphicsItem * const opc_Item) const
{
   bool q_Return = true;

   // check if the graphics items can be aligned
   if (opc_Item->type() == msn_GRAPHICS_ITEM_BUS_CONNECT)
   {
      // item is not allowed to be aligned
      q_Return = false;
   }

   return q_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Checking if the drag move over the scene can be used

   \return
   true     Drag move is available
   false    Drag move is available
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_SdTopologyScene::IsSceneRubberBandAvailable(void) const
{
   // no restrictions here
   return true;
}

//----------------------------------------------------------------------------------------------------------------------
C_SebBaseContextMenuManager * C_SdTopologyScene::m_GetContextMenuManager(void)
{
   return &this->mc_ContextMenuManager;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get current copy paste manager

   \return
   NULL No copy paste manager
   Else Valid copy paste manager
*/
//----------------------------------------------------------------------------------------------------------------------
C_SebBaseCopyPasteManager * C_SdTopologyScene::m_GetCopyPasteManager(void)
{
   return &this->mc_CopyPasteManager;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get current copy paste manager

   \return
   NULL No copy paste manager
   Else Valid copy paste manager
*/
//----------------------------------------------------------------------------------------------------------------------
const C_SebBaseCopyPasteManager * C_SdTopologyScene::m_GetCopyPasteManagerConst(void) const
{
   return &this->mc_CopyPasteManager;
}

//----------------------------------------------------------------------------------------------------------------------
C_SebUnoBaseManager * C_SdTopologyScene::m_GetUndoManager(void)
{
   return &this->mc_UndoManager;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Overwritten mouse move event slot

   Here: Update temporary line

   \param[in,out]  opc_Event  Event identification and information
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdTopologyScene::mouseMoveEvent(QGraphicsSceneMouseEvent * const opc_Event)
{
   if (mq_ProxyWidgetInteractionActive == false)
   {
      if (this->mpc_ConnectorLine != NULL)
      {
         this->mpc_ConnectorLine->UpdateP2(opc_Event->scenePos());
         //Temporary hide
         this->mpc_ConnectorLine->hide();
      }

      if (mq_ConnectState)
      {
         bool q_Found = false;
         QGraphicsItem * const pc_Item = this->itemAt(opc_Event->scenePos(), QTransform());

         m_DisableEverythingForConnectState(this->mpc_NodeConnectItem);
         if ((pc_Item != NULL) && (pc_Item != this->mpc_EmptyConnectItem))
         {
            //Delete temporary item
            //lint -e{845,944,948,774,506} Variable necessary to deactivate feature until finished
            if ((C_SdTopologyScene::mhq_NewConnectState == true) && (this->mpc_EmptyConnectItem != NULL))
            {
               delete (this->mpc_EmptyConnectItem);
               this->mpc_EmptyConnectItem = NULL;
            }
            mc_ToolTipPos = opc_Event->scenePos();
            QGraphicsItem * const pc_ItemParent = C_SebUtil::h_GetHighestParent(pc_Item);
            C_GiLiBus * const pc_Bus = dynamic_cast<C_GiLiBus * const>(pc_ItemParent);
            C_GiNode * const pc_Node = dynamic_cast<C_GiNode * const>(pc_ItemParent);
            if (pc_Bus != NULL)
            {
               mpc_CurrentHoverItem = pc_Bus;
               this->m_StartTimerForToolTipDirect();
               q_Found = true;
               //lint -e{948,774,506} Variable necessary to deactivate feature until finished
               if (C_SdTopologyScene::mhq_NewConnectState == true)
               {
                  pc_Bus->SetDisabledLook(false);
               }
            }
            if (pc_Node != NULL)
            {
               mpc_CurrentHoverItem = pc_Node;
               this->m_StartTimerForToolTipDirect();
               q_Found = true;
               //lint -e{948,774,506} Variable necessary to deactivate feature until finished
               if (C_SdTopologyScene::mhq_NewConnectState == true)
               {
                  pc_Node->SetDrawWhiteFilter(false);
               }
            }
            if ((q_Found == true) && (pc_ItemParent != NULL))
            {
               //Handle mouse cursor manually while connect state
               //Done manually due to bug related to "setAcceptHoverEvents" function call
               //   which seems to deactivate the qt mouse handling while click active
               //TODO custom cursor
               QApplication::setOverrideCursor(pc_ItemParent->cursor());
               ++mu64_MouseOverrideCounter;
            }
         }
         if (q_Found == false)
         {
            this->m_HandleHideToolTip();
            //Handle mouse cursor manually while connect state
            //TODO custom cursor
            m_RevertOverrideCursor();
            //Add temporary item?
            //lint -e{948,774,506} Variable necessary to deactivate feature until finished
            if (C_SdTopologyScene::mhq_NewConnectState == true)
            {
               if (this->mpc_EmptyConnectItem == NULL)
               {
                  mpc_EmptyConnectItem = new C_GiSvgGraphicsItem("://images/IconAddEnabled.svg", 100.0, 100.0);
                  this->addItem(mpc_EmptyConnectItem);
               }
               this->mpc_EmptyConnectItem->setPos(opc_Event->scenePos());
            }
         }
         if (this->mpc_ConnectorLine != NULL)
         {
            //Temporary hide undo
            this->mpc_ConnectorLine->setVisible(true);
         }
      }
      else
      {
         const QList<QGraphicsItem *> & rc_SelectedItems = this->selectedItems();

         // special case: moving a bus text element
         if ((rc_SelectedItems.size() == 1) &&
             (opc_Event->buttons().testFlag(Qt::LeftButton) == true) &&
             (this->m_IsRubberBandActive() == false))
         {
            const C_GiTextElementBus * const pc_BusName = dynamic_cast<const C_GiTextElementBus * const>(rc_SelectedItems[0]);

            if (pc_BusName != NULL)
            {
               if (this->mpc_BusNameLine == NULL)
               {
                  const QList<QGraphicsItem *> & rc_Items = this->items();
                  QList<QGraphicsItem *>::const_iterator c_ItItem;

                  // search the associated bus
                  this->mpc_BusNameLineBus = NULL;

                  for (c_ItItem = rc_Items.begin(); c_ItItem != rc_Items.end(); ++c_ItItem)
                  {
                     const C_GiLiBus * const pc_Bus = dynamic_cast<const C_GiLiBus * const>(*c_ItItem);
                     if (pc_Bus != NULL)
                     {
                        if (static_cast<sint32>(pc_BusName->GetBusIndex()) == pc_Bus->GetIndex())
                        {
                           this->mpc_BusNameLineBus = pc_Bus;
                           break;
                        }
                     }
                  }

                  // create the line if the correct bus was found
                  if (this->mpc_BusNameLineBus != NULL)
                  {
                     this->mpc_BusNameLine = new C_GiLiTemporaryLine();
                     this->addItem(this->mpc_BusNameLine);
                  }
               }

               if ((this->mpc_BusNameLineBus != NULL) &&
                   (this->mpc_BusNameLine != NULL))
               {
                  // update the position of the line
                  QPointF c_ClosestPointOnBus;
                  const QPointF c_PointOnTextElement = pc_BusName->mapToScene(pc_BusName->boundingRect().center());
                  // get the position on the bus
                  this->mpc_BusNameLineBus->FindClosestPoint(c_PointOnTextElement, c_ClosestPointOnBus);
                  this->mpc_BusNameLine->UpdateP2(c_ClosestPointOnBus);

                  // set the position on the bus text element
                  this->mpc_BusNameLine->setPos(c_PointOnTextElement);
               }
            }
         }

         if (mq_RestoreMouseCursorWhenPossible == true)
         {
            //Revert mouse cursor manually after connect state
            //Might cause performance issues
            QGraphicsItem * const pc_Item = this->itemAt(opc_Event->scenePos(), QTransform());
            if (pc_Item == NULL)
            {
               //TODO custom cursor
               m_RevertOverrideCursor();
               mq_RestoreMouseCursorWhenPossible = false;
            }
         }
      }
   }

   C_SebTopologyBaseScene::mouseMoveEvent(opc_Event);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Overwritten mouse release event slot

   Here: Handle connect

   \param[in,out]  opc_Event  Event identification and information
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdTopologyScene::mouseReleaseEvent(QGraphicsSceneMouseEvent * const opc_Event)
{
   C_SebScene::mouseReleaseEvent(opc_Event);
   if (mq_ProxyWidgetInteractionActive == false)
   {
      if (this->mpc_ConnectorLine != NULL)
      {
         //Hide temporary line
         this->mpc_ConnectorLine->hide();
         if ((C_OSCUtils::h_IsFloat64NearlyEqual(opc_Event->scenePos().x() - this->mc_ConnectStartPoint.x(),
                                                 0.0) == true) ||
             (C_OSCUtils::h_IsFloat64NearlyEqual(opc_Event->scenePos().y() - this->mc_ConnectStartPoint.y(),
                                                 0.0) == true))
         {
            QGraphicsView * const pc_View = this->views().at(0);
            C_OgeWiCustomMessage c_MessageBox(pc_View, C_OgeWiCustomMessage::E_Type::eINFORMATION);
            c_MessageBox.SetHeading(C_GtGetText::h_GetText("Link Use"));
            c_MessageBox.SetDescription(C_GtGetText::h_GetText(
                                           "Connect existing elements: Click on the link icon and drag and drop it "
                                           "either to an existing node or to a bus element."));
            c_MessageBox.SetOKButtonText(C_GtGetText::h_GetText("OK"));
            c_MessageBox.SetCustomMinHeight(180, 180);
            c_MessageBox.Execute();
         }
         else
         {
            //Create connector if selection valid
            const QPointF c_Pos = opc_Event->scenePos();
            const QTransform c_Transform;
            QGraphicsItem * pc_BusChild;

            //Now get most top item
            pc_BusChild = this->itemAt(c_Pos, c_Transform);

            // is the item at the cursor position a QGraphicsItem
            if (pc_BusChild != NULL)
            {
               // the parent of the C_GiLiLineConnection is the bus item
               pc_BusChild = C_SebUtil::h_GetHighestParent(pc_BusChild);
               this->mpc_BusConnectItem = dynamic_cast<C_GiLiBus *>(pc_BusChild);

               if (opc_Event->button() == Qt::LeftButton)
               {
                  if (this->mpc_BusConnectItem != NULL)
                  {
                     this->mc_ConnectEndPoint = opc_Event->scenePos();
                     this->m_ShowNewConnectionPopUp(this->mpc_NodeConnectItem, this->mpc_BusConnectItem, false, false,
                                                    -1,
                                                    NULL);
                  }
                  else
                  {
                     C_GiNode * const pc_Node = dynamic_cast<C_GiNode * const>(pc_BusChild);
                     //Check node to node
                     this->m_ShowNewNodeToNodeConnectionPopUp(this->mpc_NodeConnectItem, pc_Node);
                  }
               }
            }
         }
         m_RemoveConnectorAndLeaveConnectState();
      }
   }

   // clean up if necessary
   this->m_RemoveBusNameLine();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Overwritten mouse double click event slot

   \param[in,out]  opc_Event  Event identification and information
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdTopologyScene::mouseDoubleClickEvent(QGraphicsSceneMouseEvent * const opc_Event)
{
   const QList<QGraphicsItem *> & rc_SelectedItems = this->selectedItems();

   if (rc_SelectedItems.count() == 1)
   {
      const sintn sn_Type = rc_SelectedItems[0]->type();

      if (sn_Type != msn_GRAPHICS_ITEM_TEXTELEMENT)
      {
         if ((sn_Type == msn_GRAPHICS_ITEM_NODE) ||
             (sn_Type == msn_GRAPHICS_ITEM_TEXTELEMENT_BUS) ||
             (sn_Type == msn_GRAPHICS_ITEM_BUS) ||
             (sn_Type == msn_GRAPHICS_ITEM_CANBUS) ||
             (sn_Type == msn_GRAPHICS_ITEM_ETHERNETBUS))
         {
            this->m_Edit(rc_SelectedItems[0], true);
         }
         else
         {
            this->m_Edit(rc_SelectedItems[0]);
         }
      }
      else
      {
         C_SebScene::mouseDoubleClickEvent(opc_Event);
      }
   }
   else
   {
      C_SebScene::mouseDoubleClickEvent(opc_Event);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Overwritten key press release event slot

   \param[in,out]  opc_KeyEvent  Key event identification and information
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdTopologyScene::keyPressEvent(QKeyEvent * const opc_KeyEvent)
{
   bool q_CallBase = true;

   if (this->mq_ProxyWidgetInteractionActive == false)
   {
      switch (opc_KeyEvent->key())
      {
      case Qt::Key_Escape:
         //Trigger abort of reconnection
         this->mc_BusConnectorReconnectManager.ContextMenuAboutToClose();
         this->m_LeaveConnectState();
         this->clearSelection();
         //Don't call base and accept to properly signal a handled escape
         q_CallBase = false;
         opc_KeyEvent->accept();
         break;
      case Qt::Key_F2:
         this->m_EditPressed();
         break;
      default:
         break;
      }
   }
   if (q_CallBase == true)
   {
      C_SebScene::keyPressEvent(opc_KeyEvent);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Overwritten context menu event

   \param[in,out]  opc_Event  Event identification and information
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdTopologyScene::contextMenuEvent(QGraphicsSceneContextMenuEvent * const opc_Event)
{
   if (this->mq_BlockContextMenu == false)
   {
      m_StopToolTip();
   }

   C_SebScene::contextMenuEvent(opc_Event);
}

//----------------------------------------------------------------------------------------------------------------------
void C_SdTopologyScene::m_OpenContextMenu(const QPointF & orc_Pos)
{
   if (this->mq_BlockContextMenu == false)
   {
      QGraphicsSceneContextMenuEvent c_Event;
      QPoint c_ScreenPos;

      m_StopToolTip();

      m_MapToGlobal(orc_Pos, c_ScreenPos);

      c_Event.setScreenPos(c_ScreenPos);

      C_SebScene::contextMenuEvent(&c_Event);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Add image to scene

   \param[in]  orc_Path       Image path
   \param[in]  orc_Position   Image scene position
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdTopologyScene::m_AddImage(const QString & orc_Path, const QPointF & orc_Position)
{
   this->mc_UndoManager.DoAddGeneric(C_SdManUnoTopologyAddCommand::eIMAGE,
                                     m_GetNewUniqueID(), orc_Position, orc_Path);
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
bool C_SdTopologyScene::m_AddOfMime(const QMimeData * const opc_MimeData, const QPointF & orc_Position)
{
   bool q_Retval = true;

   if (opc_MimeData != NULL)
   {
      C_SdManUnoTopologyAddCommand::E_ElementType e_Type = C_SdManUnoTopologyAddCommand::E_ElementType::eUNKNOWN;
      QString c_Text;
      if (opc_MimeData->hasFormat("text/plain") == true)
      {
         const C_OSCDeviceDefinition * pc_Node;
         c_Text = opc_MimeData->text();

         // check if it is a node
         pc_Node = C_OSCSystemDefinition::hc_Devices.LookForDevice(c_Text.toStdString().c_str());
         if (pc_Node != NULL)
         {
            e_Type = C_SdManUnoTopologyAddCommand::E_ElementType::eNODE;
         }
         else if (c_Text == mc_CAN_BUS)
         {
            e_Type = C_SdManUnoTopologyAddCommand::E_ElementType::eCAN_BUS;
         }
         else if (c_Text == mc_ETHERNET_BUS)
         {
            e_Type = C_SdManUnoTopologyAddCommand::E_ElementType::eETHERNET_BUS;
         }
         else if (c_Text == C_SebScene::mhc_BOUNDARY)
         {
            e_Type = C_SdManUnoTopologyAddCommand::E_ElementType::eBOUNDARY;
         }
         else if (c_Text == C_SebScene::mhc_LINE)
         {
            e_Type = C_SdManUnoTopologyAddCommand::E_ElementType::eLINE_ARROW;
         }
         else
         {
            // check if it is an image file
            const bool q_ImageFileExist = C_SebUtil::h_CheckFilePathForImage(c_Text);

            if (q_ImageFileExist == true)
            {
               e_Type = C_SdManUnoTopologyAddCommand::E_ElementType::eIMAGE;
            }
            else if (c_Text == C_SebScene::mhc_IMAGE)
            {
               this->m_AddImageWithFileDialog(orc_Position);
            }
            else if (c_Text != "")
            {
               // no known element, add the text as text element
               e_Type = C_SdManUnoTopologyAddCommand::E_ElementType::eTEXT_ELEMENT;
            }
            else
            {
               // nothing to do
            }
         }
      }
      else if (opc_MimeData->hasUrls() == true)
      {
         c_Text = opc_MimeData->text();

         // check if it is an image file
         const bool q_ImageFileExist = C_SebUtil::h_CheckFilePathForImage(c_Text);

         if (q_ImageFileExist == true)
         {
            e_Type = C_SdManUnoTopologyAddCommand::E_ElementType::eIMAGE;
         }
         else
         {
            q_Retval = false;
         }
      }
      else
      {
         q_Retval = false;
      }
      if ((e_Type == C_SdManUnoTopologyAddCommand::E_ElementType::eCAN_BUS) ||
          (e_Type == C_SdManUnoTopologyAddCommand::E_ElementType::eETHERNET_BUS))
      {
         this->mc_UndoManager.DoAddBus(e_Type, m_GetNewUniqueID(), m_GetNewUniqueID(), orc_Position);
      }
      else if (e_Type != C_SdManUnoTopologyAddCommand::E_ElementType::eUNKNOWN)
      {
         this->mc_UndoManager.DoAddGeneric(e_Type, m_GetNewUniqueID(),
                                           orc_Position, c_Text);
      }
      else
      {
         // nothing to do
      }
   }
   else
   {
      q_Retval = false;
   }
   return q_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Copy items
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdTopologyScene::m_Copy(void)
{
   QList<QGraphicsItem *> c_SelectedItems;
   QList<QGraphicsItem *> c_ToAdd;
   QList<QGraphicsItem *> c_ToRemove;
   this->UpdateSystemDefinition();
   c_SelectedItems = this->selectedItems();
   //Patch in all text elements (Doubles should be no problem)
   for (QList<QGraphicsItem *>::iterator c_ItItem = c_SelectedItems.begin();
        c_ItItem != c_SelectedItems.end();
        ++c_ItItem)
   {
      //Bus
      C_GiLiBus * const pc_Bus = dynamic_cast<C_GiLiBus *>(C_SebUtil::h_GetHighestParent(*c_ItItem));
      C_GiTextElementBus * const pc_TextBus = dynamic_cast<C_GiTextElementBus *>(C_SebUtil::h_GetHighestParent(*c_ItItem));
      if (pc_Bus != NULL)
      {
         c_ToAdd.push_back(pc_Bus->GetTextElementBus());
      }
      if (pc_TextBus != NULL)
      {
         c_ToRemove.push_back(pc_TextBus);
      }
   }
   //Remove all text elements (First)
   for (QList<QGraphicsItem *>::iterator c_ItItem = c_ToRemove.begin(); c_ItItem != c_ToRemove.end(); ++c_ItItem)
   {
      c_SelectedItems.removeAll(C_SebUtil::h_GetHighestParent(*c_ItItem));
   }
   //Add all necessary text elements (Second)
   for (QList<QGraphicsItem *>::iterator c_ItItem = c_ToAdd.begin(); c_ItItem != c_ToAdd.end(); ++c_ItItem)
   {
      c_SelectedItems.push_back(*c_ItItem);
   }
   this->m_CopyItemsToCopyPasteManager(c_SelectedItems);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Cut items
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdTopologyScene::m_Cut(void)
{
   this->UpdateSystemDefinition();
   this->m_CopyItemsToCopyPasteManager(this->selectedItems());

   m_Delete(true);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Optional check for user confirmation on delete action

   \param[in]  orc_SelectedItems    Selected items

   \return
   true  Continue
   false Abort
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_SdTopologyScene::m_HandleDeleteUserConfirmation(const QList<QGraphicsItem *> & orc_SelectedItems) const
{
   bool q_Retval = true;

   Q_UNUSED(orc_SelectedItems)

   if (this->views().size() > 0)
   {
      QGraphicsView * const pc_View = this->views().at(0);
      if (pc_View != NULL)
      {
         C_OgeWiCustomMessage::E_Outputs e_ReturnMessageBox;
         C_OgeWiCustomMessage c_MessageBox(pc_View, C_OgeWiCustomMessage::E_Type::eQUESTION);
         c_MessageBox.SetDescription(C_GtGetText::h_GetText("Do you really want to delete the selected item(s)?"));
         c_MessageBox.SetHeading(C_GtGetText::h_GetText("Items delete"));
         c_MessageBox.SetOKButtonText(C_GtGetText::h_GetText("Delete"));
         c_MessageBox.SetNOButtonText(C_GtGetText::h_GetText("Keep"));
         c_MessageBox.SetCustomMinHeight(180, 180);
         e_ReturnMessageBox = c_MessageBox.Execute();

         if (e_ReturnMessageBox == C_OgeWiCustomMessage::eOK)
         {
            q_Retval = true;
         }
         else
         {
            q_Retval = false;
         }
      }
   }
   return q_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Paste objects of clipboard

   \param[in]  opc_Pos  Optional position to paste at (Otherwise current mouse cursor position is chosen)
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdTopologyScene::m_PasteOfClipBoard(const QPointF * const opc_Pos)
{
   QClipboard * const pc_Clipboard = QApplication::clipboard();

   if (pc_Clipboard != NULL)
   {
      if (opc_Pos != NULL)
      {
         (void) m_AddOfMime(pc_Clipboard->mimeData(), *opc_Pos);
      }
      else
      {
         QPointF c_Pos;

         //Mouse scene position
         m_MapFromGlobal(QCursor::pos(), c_Pos);

         C_GiCustomFunctions::h_AdaptMouseRangePos(c_Pos);
         (void) m_AddOfMime(pc_Clipboard->mimeData(), c_Pos);
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
C_GiNode * C_SdTopologyScene::m_CreateNode(const sint32 & ors32_Index, const uint64 & oru64_ID,
                                           const float64 & orf64_Width, const float64 & orf64_Height,
                                           QGraphicsItem * const opc_Parent)
{
   return new C_GiNode(ors32_Index, oru64_ID, orf64_Width, orf64_Height, opc_Parent);
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
C_GiLiCANBus * C_SdTopologyScene::m_CreateCANBus(const sint32 & ors32_Index, const uint64 & oru64_ID,
                                                 C_GiTextElementBus * const opc_TextElementName,
                                                 const std::vector<QPointF> * const opc_Points,
                                                 QGraphicsItem * const opc_Parent)
{
   return new C_GiLiCANBus(ors32_Index, oru64_ID, opc_TextElementName, true, opc_Points, opc_Parent);
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
C_GiLiEthernetBus * C_SdTopologyScene::m_CreateEthernetBus(const sint32 & ors32_Index, const uint64 & oru64_ID,
                                                           C_GiTextElementBus * const opc_TextElementName,
                                                           const std::vector<QPointF> * const opc_Points,
                                                           QGraphicsItem * const opc_Parent)
{
   return new C_GiLiEthernetBus(ors32_Index, oru64_ID, opc_TextElementName, true, opc_Points, opc_Parent);
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
C_GiTextElementBus * C_SdTopologyScene::m_CreateBusTextElement(const sint32 & ors32_Index, const uint64 & oru64_ID,
                                                               QGraphicsItem * const opc_Parent)
{
   return new C_GiTextElementBus(ors32_Index, oru64_ID, opc_Parent);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Add new node to scene and connect signals

   \param[in,out]  opc_NodeGraphicsItem   Pointer to new node
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdTopologyScene::m_AddNodeToScene(C_GiNode * const opc_NodeGraphicsItem)
{
   // Context menu button of node
   connect(opc_NodeGraphicsItem, &C_GiNode::SigOpenContextMenu, this,
           &C_SdTopologyScene::m_OpenContextMenu);

   // Context menu button of node
   C_SebTopologyBaseScene::m_AddNodeToScene(opc_NodeGraphicsItem);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Add new bus connector to scene and connect signals

   \param[in,out]  opc_BusConnectorGraphicsItem    Connector item
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdTopologyScene::m_AddBusConnectorToScene(C_GiLiBusConnector * const opc_BusConnectorGraphicsItem)
{
   if (opc_BusConnectorGraphicsItem != NULL)
   {
      //Connections
      connect(opc_BusConnectorGraphicsItem, &C_GiLiBusConnector::SigStartConnect, this,
              &C_SdTopologyScene::m_BusConnectorReconnectionStart);
      connect(opc_BusConnectorGraphicsItem, &C_GiLiBusConnector::SigStopConnect, this,
              &C_SdTopologyScene::m_LeaveConnectState);
      connect(opc_BusConnectorGraphicsItem, &C_GiLiBusConnector::ShowReconnectContextMenu, this,
              &C_SdTopologyScene::m_ShowBusConnectorReconnectionContextMenu);

      C_SebTopologyBaseScene::m_AddBusConnectorToScene(opc_BusConnectorGraphicsItem);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Add new text element to scene and connect signals

   \param[in,out]  opc_Item   Text element
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdTopologyScene::m_AddTextElementToScene(C_GiBiTextElement * const opc_Item)
{
   connect(opc_Item, &C_GiBiTextElement::SigTextInteractionModeStateChanged, this,
           &C_SdTopologyScene::m_HandleProxyWidgetInteractionChange);

   C_SebTopologyBaseScene::m_AddTextElementToScene(opc_Item);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Handle after resize action

   \param[in]  ors32_InteractionPointIndex   Interaction point index
   \param[in]  orc_PositionDifference        Position difference of resize
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdTopologyScene::m_HandleRevertableResizeLine(const sint32 & ors32_InteractionPointIndex,
                                                     const QPointF & orc_PositionDifference)
{
   if (this->mc_BusConnectorReconnectManager.Active() == false)
   {
      C_SebScene::m_HandleRevertableResizeLine(ors32_InteractionPointIndex, orc_PositionDifference);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Call setup style dialog

   \param[in,out]  opc_Item   Item to change style for

   \return
   False Failure or abort by user
   True  Success and accept
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_SdTopologyScene::m_CallSetupStyle(QGraphicsItem * const opc_Item) const
{
   bool q_Retval = false;

   C_GiLiBus * const pc_Bus = dynamic_cast<C_GiLiBus *>(opc_Item);

   if (pc_Bus != NULL)
   {
      q_Retval = pc_Bus->OpenStyleDialog();
   }
   else
   {
      q_Retval = C_SebTopologyBaseScene::m_CallSetupStyle(opc_Item);
   }
   return q_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
void C_SdTopologyScene::m_SelectionChanged(void)
{
   if (this->mq_ProxyWidgetInteractionActive == false)
   {
      QList<QGraphicsItem *>::const_iterator c_ItItem;
      QList<QGraphicsItem *> c_SelectedItems = this->selectedItems();

      if (this->mpc_SelectedBusConnectorItem != NULL)
      {
         // bring the last single selected item back
         this->mpc_SelectedBusConnectorItem->RestoreZOrder();
         this->mpc_SelectedBusConnectorItem = NULL;
      }

      //Prepare selection
      //deactivate arrow cursor button selection
      this->mpc_ArrowCursorButton->setSelected(false);
      this->mpc_ArrowCursorButton->setVisible(false);
      if (c_SelectedItems.size() > 1)
      {
         for (c_ItItem = c_SelectedItems.begin(); c_ItItem != c_SelectedItems.end(); ++c_ItItem)
         {

            C_GiLiBusConnector * const pc_BusConnector = dynamic_cast<C_GiLiBusConnector *>(*c_ItItem);
            if (pc_BusConnector != NULL)
            {
               pc_BusConnector->setSelected(false);
            }
         }
      }
      //Update valid selected items
      c_SelectedItems = this->selectedItems();

      // triggered by signal selectionChanged
      if (c_SelectedItems.size() == 1)
      {

         C_GiBiRectBaseGroup * const pc_Item = dynamic_cast<C_GiBiRectBaseGroup *>(c_SelectedItems[0]);

         // check if the only one selected item is a resizable rectangle based item
         if (pc_Item != NULL)
         {

            C_GiNode * const pc_Node = dynamic_cast<C_GiNode *>(pc_Item);
            if (pc_Node != NULL)
            {
               this->mpc_ArrowCursorButton->DetachNode();
               this->mpc_ArrowCursorButton->AttachToNode(pc_Node);
               this->mpc_NodeConnectItem = pc_Node;
               this->mpc_ArrowCursorButton->setVisible(true);
            }
            // activate the resizing only if exactly one node is selected
            pc_Item->SetResizing(true);
         }
         else
         {

            C_GiLiLineGroup * const pc_LineItem = dynamic_cast<C_GiLiLineGroup *>(c_SelectedItems[0]);
            if (pc_LineItem != NULL)
            {
               //Custom rubberband flag
               if (this->m_IsRubberBandActive() == true)
               {

                  C_GiLiBusConnector * const pc_BusConnector = dynamic_cast<C_GiLiBusConnector *>(pc_LineItem);
                  if (pc_BusConnector == NULL)
                  {
                     pc_LineItem->SetResizing(false);
                  }
               }
               else
               {
                  pc_LineItem->SetResizing(true);
               }
            }
         }

         if (c_SelectedItems[0]->type() == msn_GRAPHICS_ITEM_BUS_CONNECT)
         {
            // bring the selected bus connector to the top

            this->mpc_SelectedBusConnectorItem = dynamic_cast<C_GiLiBusConnector *>(c_SelectedItems[0]);
            if (this->mpc_SelectedBusConnectorItem != NULL)
            {
               this->mpc_SelectedBusConnectorItem->setZValue(this->GetHighestUsedZValueList(this->items()) + 1.0);
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
/*! \brief   Handle start connection

   \param[in]  orc_LineStart        Start of connector position
   \param[in]  orc_SceneTriggerPos  Scene trigger position of event
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdTopologyScene::m_StartConnector(const QPointF & orc_LineStart, const QPointF & orc_SceneTriggerPos)
{
   m_RemoveConnectorLine();
   this->mc_ConnectStartPoint = orc_SceneTriggerPos;
   this->mpc_ConnectorLine = new C_GiLiTemporaryLine();

   this->mpc_ConnectorLine->setPos(orc_LineStart);
   //Always on top
   this->mpc_ConnectorLine->setZValue(mf64_ZORDER_MAX);
   this->addItem(mpc_ConnectorLine);

   this->m_EnterConnectState(C_GiLiBusConnector::E_ConnectState::eTO_GENERIC_AND_BUS, this->mpc_NodeConnectItem);
}

//----------------------------------------------------------------------------------------------------------------------
void C_SdTopologyScene::m_EditPressed(void)
{
   const QList<QGraphicsItem *> & rc_SelectedItems = this->selectedItems();

   if (rc_SelectedItems.count() == 1)
   {
      const sintn sn_Type = rc_SelectedItems[0]->type();

      if ((sn_Type == msn_GRAPHICS_ITEM_NODE) ||
          (sn_Type == msn_GRAPHICS_ITEM_TEXTELEMENT_BUS) ||
          (sn_Type == msn_GRAPHICS_ITEM_BUS) ||
          (sn_Type == msn_GRAPHICS_ITEM_CANBUS) ||
          (sn_Type == msn_GRAPHICS_ITEM_ETHERNETBUS))
      {
         this->m_Edit(rc_SelectedItems[0], true);
      }
      else
      {
         this->m_Edit(rc_SelectedItems[0]);
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
void C_SdTopologyScene::m_EditClicked(const QGraphicsItem * const opc_Item)
{
   this->m_Edit(opc_Item, false);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Edit actual item

   \param[in]  opc_Item       Item to edit
   \param[in]  oq_FocusName   Flag if name edit or properties edit is requested
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdTopologyScene::m_Edit(const QGraphicsItem * const opc_Item, const bool oq_FocusName)
{
   uint32 u32_Flag;

   if (oq_FocusName == true)
   {
      u32_Flag = mu32_FLAG_EDIT_NAME;
   }
   else
   {
      u32_Flag = mu32_FLAG_OPEN_PROPERTIES;
   }

   if (opc_Item->type() == msn_GRAPHICS_ITEM_NODE)
   {

      const C_GiNode * const pc_Node = dynamic_cast<const C_GiNode *>(opc_Item);

      if (pc_Node != NULL)
      {
         if (pc_Node->GetIndex() >= 0)
         {
            Q_EMIT this->SigChangeMode(ms32_MODE_SYSDEF, ms32_SUBMODE_SYSDEF_NODEEDIT,
                                       static_cast<uint32>(pc_Node->GetIndex()), pc_Node->GetText(), "", u32_Flag);
         }
      }
   }
   else if ((opc_Item->type() == msn_GRAPHICS_ITEM_BUS) ||
            (opc_Item->type() == msn_GRAPHICS_ITEM_CANBUS) ||
            (opc_Item->type() == msn_GRAPHICS_ITEM_ETHERNETBUS))
   {

      const C_GiLiBus * const pc_Bus = dynamic_cast<const C_GiLiBus *>(opc_Item);

      if (pc_Bus != NULL)
      {
         if (pc_Bus->GetIndex() >= 0)
         {
            Q_EMIT this->SigChangeMode(ms32_MODE_SYSDEF, ms32_SUBMODE_SYSDEF_BUSEDIT,
                                       static_cast<uint32>(pc_Bus->GetIndex()), pc_Bus->GetName(), "", u32_Flag);
         }
      }
   }
   else if (opc_Item->type() == msn_GRAPHICS_ITEM_TEXTELEMENT_BUS)
   {

      const C_GiTextElementBus * const pc_Text = dynamic_cast<const C_GiTextElementBus *>(opc_Item);

      if (pc_Text != NULL)
      {
         Q_EMIT this->SigChangeMode(ms32_MODE_SYSDEF, ms32_SUBMODE_SYSDEF_BUSEDIT,
                                    static_cast<uint32>(pc_Text->GetBusIndex()), pc_Text->GetText(), "", u32_Flag);
      }
   }
   else
   {
      // nothing to do
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Synchronize system definition and scene
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdTopologyScene::UpdateSystemDefinition(void) const
{
   QList<QGraphicsItem *> c_Items = this->items();
   //Fill
   for (QList<QGraphicsItem *>::iterator c_ItItem = c_Items.begin(); c_ItItem != c_Items.end();
        ++c_ItItem)
   {
      //Node
      C_PuiSdDataElement * const pc_DataElement = dynamic_cast<C_PuiSdDataElement *>(*c_ItItem);
      if (pc_DataElement != NULL)
      {
         pc_DataElement->UpdateData();
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Checking if any item can be added to the scene

   \return
   true     Items can be added
   false    Items cannot be added
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_SdTopologyScene::IsAnyItemAddable(void) const
{
   return true;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Adapt index if item in array was deleted

   \param[in]  ore_Type       Type of vector which changed
   \param[in]  ors32_Index    Index of vector which changed
   \param[in]  ore_Action     Type of action
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdTopologyScene::m_SyncIndex(const stw_opensyde_gui_logic::C_PuiSdDataElement::E_Type & ore_Type,
                                    const stw_types::sint32 & ors32_Index,
                                    const stw_opensyde_gui_logic::C_PuiSdDataElement::E_Action & ore_Action) const
{
   C_PuiSdDataElement * pc_DataElement;

   QList<QGraphicsItem *> c_Items = this->items();

   //Fill
   for (QList<QGraphicsItem *>::iterator c_ItItem = c_Items.begin(); c_ItItem != c_Items.end();
        ++c_ItItem)
   {
      //Node
      try
      {
         pc_DataElement = dynamic_cast<C_PuiSdDataElement *>(*c_ItItem);
         if (pc_DataElement != NULL)
         {
            pc_DataElement->SyncIndex(ore_Type, ors32_Index, ore_Action);
         }
      }
      catch (...)
      {
         //Cast failed
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Add new connection

   \param[in]  oru8_InterfaceNumber       Number of interface on bus
   \param[in]  oru8_NodeId                Node id
   \param[in]  oq_ActivateDatapoolL2      Activate datapool L2
   \param[in]  oq_ActivateDatapoolECeS    Activate datapool ECeS
   \param[in]  oq_ActivateDatapoolECoS    Activate datapool ECoS
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdTopologyScene::m_ConnectNodeToBus(const stw_types::uint8 & oru8_InterfaceNumber,
                                           const stw_types::uint8 & oru8_NodeId, const bool oq_ActivateDatapoolL2,
                                           const bool oq_ActivateDatapoolECeS, const bool oq_ActivateDatapoolECoS)
{
   if ((this->mpc_NodeConnectItem != NULL) &&
       (this->mpc_BusConnectItem != NULL))
   {
      this->mc_UndoManager.DoAddBusConnector(
         m_GetNewUniqueID(),  this->mc_ConnectEndPoint, this->mpc_NodeConnectItem,
         this->mpc_BusConnectItem, oru8_InterfaceNumber, oru8_NodeId, oq_ActivateDatapoolL2, oq_ActivateDatapoolECeS,
         oq_ActivateDatapoolECoS);

      //Reset pointers
      this->mpc_NodeConnectItem = NULL;
      this->mpc_BusConnectItem = NULL;
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Change existing connection

   \param[in]  oru8_InterfaceNumber       Number of interface on bus
   \param[in]  oru8_NodeId                New node id
   \param[in]  oq_ActivateDatapoolL2      Activate datapool L2
   \param[in]  oq_ActivateDatapoolECeS    Activate datapool ECeS
   \param[in]  oq_ActivateDatapoolECoS    Activate datapool ECoS
   \param[in]  opc_Connector              Current bus connector for change of interface
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdTopologyScene::m_ChangeInterface(const uint8 & oru8_InterfaceNumber, const uint8 & oru8_NodeId,
                                          const bool oq_ActivateDatapoolL2, const bool oq_ActivateDatapoolECeS,
                                          const bool oq_ActivateDatapoolECoS, C_GiLiBusConnector * const opc_Connector)
{
   if (opc_Connector != NULL)
   {
      //Update interface
      const C_PuiSdNodeConnectionId * const pc_Conn = opc_Connector->GetConnectionData();
      if (pc_Conn != NULL)
      {
         const C_GiNode * const pc_Node = opc_Connector->GetNodeItem();
         if (pc_Node != NULL)
         {
            const C_OSCNode * const pc_NodeData = C_PuiSdHandler::h_GetInstance()->GetOSCNodeConst(pc_Node->GetIndex());
            if (pc_NodeData != NULL)
            {
               for (uint32 u32_ItComInterface =
                       0; u32_ItComInterface < pc_NodeData->c_Properties.c_ComInterfaces.size();
                    ++u32_ItComInterface)
               {
                  const C_OSCNodeComInterfaceSettings & rc_ComInterface =
                     pc_NodeData->c_Properties.c_ComInterfaces[u32_ItComInterface];
                  if ((rc_ComInterface.u8_InterfaceNumber == pc_Conn->u8_InterfaceNumber) &&
                      (pc_Conn->e_InterfaceType == rc_ComInterface.e_InterfaceType))
                  {
                     this->mc_UndoManager.DoChangeInterface(opc_Connector, pc_Conn->u8_InterfaceNumber,
                                                            oru8_InterfaceNumber, rc_ComInterface.u8_NodeID,
                                                            oru8_NodeId, oq_ActivateDatapoolL2, oq_ActivateDatapoolECeS,
                                                            oq_ActivateDatapoolECoS);
                  }
               }
            }
         }
      }
      m_UpdateHints();
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Restore default ToolTips for all items
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdTopologyScene::m_RestoreToolTips(void) const
{
   C_GiBiCustomToolTip * pc_CustomToolTip;

   QList<QGraphicsItem *> c_Items = this->items();

   //Fill
   for (QList<QGraphicsItem *>::iterator c_ItItem = c_Items.begin(); c_ItItem != c_Items.end();
        ++c_ItItem)
   {
      //Node
      try
      {
         pc_CustomToolTip = dynamic_cast<C_GiBiCustomToolTip *>(C_SebUtil::h_GetHighestParent(*c_ItItem));
         if (pc_CustomToolTip != NULL)
         {
            pc_CustomToolTip->RestoreDefaultToolTip();
         }
      }
      catch (...)
      {
         //Cast failed
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Delete node of scene and disconnect signals

   \param[in,out]  opc_NodeGraphicsItem   Pointer to existing node
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdTopologyScene::m_RemoveNodeOfScene(C_GiNode * const opc_NodeGraphicsItem)
{
   if (opc_NodeGraphicsItem != NULL)
   {
      // Context menu button of node
      disconnect(opc_NodeGraphicsItem, &C_GiNode::SigOpenContextMenu, this,
                 &C_SdTopologyScene::m_OpenContextMenu);
      disconnect(opc_NodeGraphicsItem, &C_GiNode::SigHideToolTip, this,
                 &C_SdTopologyScene::m_HandleHideToolTip);
      m_RemoveRectBaseGroupOfScene(opc_NodeGraphicsItem);
      //unregister arrow
      this->mpc_ArrowCursorButton->DetachNode();
      //Update indices
      this->m_SyncIndex(C_PuiSdDataElement::eNODE, opc_NodeGraphicsItem->GetIndex(),
                        C_PuiSdDataElement::eDELETE);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Delete bus of scene and disconnect signals

   \param[in,out]  opc_BusGraphicsItem    Pointer to existing bus
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdTopologyScene::m_RemoveBusOfScene(C_GiLiBus * const opc_BusGraphicsItem)
{
   QList<QGraphicsItem *> c_Items = this->items();
   disconnect(opc_BusGraphicsItem, &C_GiLiBus::SigHideToolTip, this,
              &C_SdTopologyScene::m_HandleHideToolTip);
   m_RemoveLineGroupOfScene(opc_BusGraphicsItem);

   this->m_SyncIndex(C_PuiSdDataElement::eBUS, opc_BusGraphicsItem->GetIndex(), C_PuiSdDataElement::eDELETE);
   //Fill
   for (QList<QGraphicsItem *>::iterator c_ItItem = c_Items.begin(); c_ItItem != c_Items.end(); ++c_ItItem)
   {
      // update all other busses and its bus text elements because of the possible new bus index

      C_GiLiBus * const pc_Bus = dynamic_cast<C_GiLiBus *>(*c_ItItem);
      if (pc_Bus != NULL)
      {
         pc_Bus->UpdateData();
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Delete bus connector of scene and disconnect signals

   \param[in,out]  opc_BusConnectorGraphicsItem    Connector item
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdTopologyScene::m_RemoveBusConnectorOfScene(C_GiLiBusConnector * const opc_BusConnectorGraphicsItem)
{
   if (opc_BusConnectorGraphicsItem != NULL)
   {
      C_GiNode * const pc_Node = opc_BusConnectorGraphicsItem->GetNodeItem();
      if (pc_Node != NULL)
      {
         pc_Node->RemoveConnector(opc_BusConnectorGraphicsItem);
      }
      //Connections
      disconnect(opc_BusConnectorGraphicsItem, &C_GiLiBusConnector::SigStopConnect, this,
                 &C_SdTopologyScene::m_LeaveConnectState);
      disconnect(opc_BusConnectorGraphicsItem, &C_GiLiBusConnector::ShowReconnectContextMenu, this,
                 &C_SdTopologyScene::m_ShowBusConnectorReconnectionContextMenu);
      disconnect(opc_BusConnectorGraphicsItem, &C_GiLiBusConnector::SigHideToolTip, this,
                 &C_SdTopologyScene::m_HandleHideToolTip);
      disconnect(opc_BusConnectorGraphicsItem, &C_GiLiBusConnector::SigShowToolTip, this,
                 &C_SdTopologyScene::m_StartTimerForToolTip);
      disconnect(opc_BusConnectorGraphicsItem, &C_GiLiBusConnector::SigHintUpdate, this,
                 &C_SdTopologyScene::m_UpdateHints);
      //Undo
      disconnect(opc_BusConnectorGraphicsItem, &C_GiLiBusConnector::SigPrepareMove, &(this->mc_UndoManager),
                 &C_SdManUnoTopologyManager::SaveBusConnectorInitialPoints);
      m_RemoveLineGroupOfScene(opc_BusConnectorGraphicsItem);
      //Item
      opc_BusConnectorGraphicsItem->DeleteConnection();
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Delete line arrow of scene and disconnect signals

   \param[in,out]  opc_Item   Line arrow item
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdTopologyScene::m_RemoveLineArrowOfScene(C_GiSdArrow * const opc_Item)
{
   if (opc_Item != NULL)
   {
      m_RemoveLineGroupOfScene(opc_Item);
      //Update indices
      this->m_SyncIndex(C_PuiSdDataElement::eLINE_ARROW, opc_Item->GetIndex(),
                        C_PuiSdDataElement::eDELETE);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Delete boundary of scene and disconnect signals

   \param[in,out]  opc_Item   Boundary item
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdTopologyScene::m_RemoveBoundaryOfScene(C_GiSdBoundary * const opc_Item)
{
   if (opc_Item != NULL)
   {
      m_RemoveRectBaseGroupOfScene(opc_Item);
      //Update indices
      this->m_SyncIndex(C_PuiSdDataElement::eBOUNDARY, opc_Item->GetIndex(),
                        C_PuiSdDataElement::eDELETE);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Delete image of scene and disconnect signals

   \param[in,out]  opc_Item   Image group item
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdTopologyScene::m_RemoveImageGroupOfScene(C_GiSdImageGroup * const opc_Item)
{
   if (opc_Item != NULL)
   {
      m_RemoveRectBaseGroupOfScene(opc_Item);
      //Update indices
      this->m_SyncIndex(C_PuiSdDataElement::eIMAGE, opc_Item->GetIndex(),
                        C_PuiSdDataElement::eDELETE);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Delete text element of scene and disconnect signals

   \param[in,out]  opc_Item   Text element
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdTopologyScene::m_RemoveTextElementOfScene(C_GiSdTextElement * const opc_Item)
{
   if (opc_Item != NULL)
   {
      disconnect(opc_Item, &C_GiBiTextElement::SigTextInteractionModeStateChanged, this,
                 &C_SdTopologyScene::m_HandleProxyWidgetInteractionChange);
      m_RemoveRectBaseGroupOfScene(opc_Item);
      //Update indices
      this->m_SyncIndex(C_PuiSdDataElement::eTEXT_ELEMENT, opc_Item->GetIndex(),
                        C_PuiSdDataElement::eDELETE);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Delete text element of scene and disconnect signals

   \param[in,out]  opc_Item   Text element
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdTopologyScene::m_RemoveTextElementBusOfScene(C_GiTextElementBus * const opc_Item)
{
   if (opc_Item != NULL)
   {
      disconnect(opc_Item, &C_GiBiTextElement::SigTextInteractionModeStateChanged, this,
                 &C_SdTopologyScene::m_HandleProxyWidgetInteractionChange);
      m_RemoveRectBaseGroupOfScene(opc_Item);
      //Update indices
      this->m_SyncIndex(C_PuiSdDataElement::eTEXT_ELEMENT_BUS, opc_Item->GetIndex(),
                        C_PuiSdDataElement::eDELETE);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Enter connect state

   if new connection is created or an existing connection is changed
   the scene switches to a custom state
   with custom mouse cursors
   and custom tooltips which have to be displayed manually

   \param[in]  ore_ConnectState  Connection state
   \param[in]  opc_Node          Relevant node (optional)
   \param[in]  ope_Type          Last used bus type (optional)
                                 NULL: No special handling for any bus type
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdTopologyScene::m_EnterConnectState(const C_GiLiBusConnector::E_ConnectState & ore_ConnectState,
                                            const C_GiNode * const opc_Node,
                                            const C_OSCSystemBus::E_Type * const ope_Type)
{
   QGraphicsItem * pc_Parent;
   C_GiNode * pc_Node;
   C_GiLiBus * pc_Bus;
   C_GiBiCustomMouseItem * pc_Other;
   const QList<QGraphicsItem *> c_Items = this->items();

   mq_ConnectState = true;

   m_DisableEverythingForConnectState(opc_Node);

   //Change temporary cursor
   switch (ore_ConnectState)
   {
   case C_GiLiBusConnector::eTO_GENERIC_AND_BUS:
   case C_GiLiBusConnector::eTO_BUS:
      if (opc_Node != NULL)
      {
         bool q_NodeToNodeAllowEthernet = false;
         bool q_NodeToNodeAllowCAN = false;
         if (ore_ConnectState == C_GiLiBusConnector::eTO_GENERIC_AND_BUS)
         {
            q_NodeToNodeAllowEthernet = opc_Node->CheckConnectionAvailable(C_OSCSystemBus::eETHERNET);
            q_NodeToNodeAllowCAN = opc_Node->CheckConnectionAvailable(C_OSCSystemBus::eCAN);
         }

         //Fill
         for (QList<QGraphicsItem *>::const_iterator c_ItItem = c_Items.begin(); c_ItItem != c_Items.end();
              ++c_ItItem)
         {
            //Node
            try
            {
               pc_Parent = C_SebUtil::h_GetHighestParent(*c_ItItem);

               pc_Bus = dynamic_cast<C_GiLiBus *>(pc_Parent);

               pc_Node = dynamic_cast<C_GiNode *>(pc_Parent);
               if (pc_Bus != NULL)
               {
                  //Indicator for special handling (applied if connector is reconnected to equal bus type,
                  // should be allowed in any case)
                  bool q_SpecialBusTypeHandling = false;
                  //Check if bus type is known and needs to be checked
                  if (ope_Type != NULL)
                  {
                     if (pc_Bus->GetType() == *ope_Type)
                     {
                        pc_Bus->SetTemporaryCursor(Qt::CrossCursor);
                        pc_Bus->DeactivateToolTipTemporarily();
                        q_SpecialBusTypeHandling = true;
                     }
                  }
                  //If bustype does not match or no special handling is requested, use default handling
                  if (q_SpecialBusTypeHandling == false)
                  {
                     if (opc_Node->CheckConnectionAvailable(pc_Bus->GetType()))
                     {
                        pc_Bus->SetTemporaryCursor(Qt::CrossCursor);
                        pc_Bus->DeactivateToolTipTemporarily();
                     }
                     else
                     {
                        pc_Bus->SetTemporaryCursor(Qt::ForbiddenCursor);
                        {
                           QString c_BusType;
                           if (pc_Bus->GetType() == C_OSCSystemBus::E_Type::eCAN)
                           {
                              c_BusType = C_GtGetText::h_GetText("CAN");
                           }
                           else
                           {
                              c_BusType = C_GtGetText::h_GetText("Ethernet");
                           }
                           //Translation 1 = Bus name
                           pc_Bus->SetTemporaryToolTipHeading(static_cast<QString>(C_GtGetText::h_GetText("%1:")
                                                                                   ).arg(pc_Bus->GetName()));
                           //Translation 1 = Bus type string 2 = Node name
                           pc_Bus->SetTemporaryToolTipContent(static_cast<QString>(
                                                                 C_GtGetText::h_GetText(
                                                                    "No free %1 Interfaces of \"%2\" available")).arg(
                                                                 c_BusType, opc_Node->GetText()));
                        }
                     }
                  }
               }
               if (pc_Node != NULL)
               {
                  bool q_Allow = false;
                  if ((ore_ConnectState == C_GiLiBusConnector::eTO_GENERIC_AND_BUS) && (opc_Node != pc_Node))
                  {
                     //Check for node to node
                     if (q_NodeToNodeAllowEthernet == true)
                     {
                        q_Allow = pc_Node->HasConnectionType(C_OSCSystemBus::eETHERNET);
                     }
                     if ((q_NodeToNodeAllowCAN == true) && (q_Allow == false))
                     {
                        q_Allow = pc_Node->HasConnectionType(C_OSCSystemBus::eCAN);
                     }
                  }
                  if (q_Allow == false)
                  {
                     pc_Node->SetTemporaryCursor(Qt::ForbiddenCursor);
                     pc_Node->DeactivateToolTipTemporarily();
                  }
                  else
                  {
                     pc_Node->SetTemporaryCursor(Qt::CrossCursor);
                  }
               }
               else
               {
                  pc_Other = dynamic_cast<C_GiBiCustomMouseItem *>(pc_Parent);
                  if ((pc_Other != NULL) && (pc_Bus == NULL))
                  {
                     pc_Other->SetTemporaryCursor(Qt::ForbiddenCursor);
                  }
               }
            }
            catch (...)
            {
               //Cast failed
            }
         }
      }
      break;
   case C_GiLiBusConnector::E_ConnectState::eTO_GENERIC:
      if (ope_Type != NULL)
      {
         for (QList<QGraphicsItem *>::const_iterator c_ItItem = c_Items.begin(); c_ItItem != c_Items.end();
              ++c_ItItem)
         {
            //Node
            try
            {
               pc_Parent = C_SebUtil::h_GetHighestParent(*c_ItItem);

               pc_Node = dynamic_cast<C_GiNode *>(pc_Parent);

               pc_Bus = dynamic_cast<C_GiLiBus *>(pc_Parent);
               if (pc_Node != NULL)
               {
                  //Ignore same node
                  if (opc_Node != pc_Node)
                  {
                     if (pc_Node->CheckConnectionAvailable(*ope_Type) == false)
                     {
                        QString c_BusType;
                        if (*ope_Type == C_OSCSystemBus::E_Type::eCAN)
                        {
                           c_BusType = C_GtGetText::h_GetText("CAN");
                        }
                        else
                        {
                           c_BusType = C_GtGetText::h_GetText("Ethernet");
                        }
                        //Adapt
                        //Translation 1 = Node name
                        pc_Node->SetTemporaryToolTipHeading(
                           static_cast<QString>(C_GtGetText::h_GetText("%1:")
                                                ).arg(pc_Node->GetText()));
                        //Translation 1 = Bus type string 2 = Node name
                        pc_Node->SetTemporaryToolTipContent(
                           static_cast<QString>(C_GtGetText::h_GetText(
                                                   "No free %1 Interfaces of \"%2\" available")).arg(
                              c_BusType, opc_Node->GetText()));
                        pc_Node->SetTemporaryCursor(Qt::ForbiddenCursor);
                     }
                     else
                     {
                        pc_Node->SetTemporaryCursor(Qt::CrossCursor);
                        pc_Node->DeactivateToolTipTemporarily();
                     }
                  }
                  else
                  {
                     pc_Node->SetTemporaryCursor(Qt::CrossCursor);
                     pc_Node->DeactivateToolTipTemporarily();
                  }
               }
               if (pc_Bus != NULL)
               {
                  pc_Bus->SetTemporaryCursor(Qt::ForbiddenCursor);
                  pc_Bus->DeactivateToolTipTemporarily();
               }
               else
               {

                  pc_Other = dynamic_cast<C_GiBiCustomMouseItem *>(pc_Parent);
                  if ((pc_Other != NULL) && (pc_Node == NULL))
                  {
                     pc_Other->SetTemporaryCursor(Qt::ForbiddenCursor);
                  }
               }
            }
            catch (...)
            {
               //Cast failed
            }
         }
      }
      break;
   default:
      break;
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Leave connect state

   Restore scene default state
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdTopologyScene::m_LeaveConnectState(void)
{
   m_StopToolTip();
   m_RestoreCursors();
   m_RestoreToolTips();
   this->mq_ConnectState = false;
   this->mq_RestoreMouseCursorWhenPossible = true;
   this->mc_BusConnectorReconnectManager.DeactivateReconnection();
   //lint -e{948,774,506} Variable necessary to deactivate feature until finished
   if (C_SdTopologyScene::mhq_NewConnectState == true)
   {
      const QList<QGraphicsItem *> c_Items = this->items();
      //Reenable all nodes and busses
      for (QList<QGraphicsItem *>::const_iterator c_It = c_Items.begin(); c_It != c_Items.end(); ++c_It)
      {

         C_GiNode * const pc_Node = dynamic_cast<C_GiNode * const>(*c_It);

         C_GiLiBus * const pc_Bus = dynamic_cast<C_GiLiBus * const>(*c_It);

         C_GiLiBusConnector * const pc_BusConnector =
            dynamic_cast<C_GiLiBusConnector * const>(C_SebUtil::h_GetHighestParent(*c_It));
         if (pc_Node != NULL)
         {
            pc_Node->SetDrawWhiteFilter(false);
         }
         if (pc_Bus != NULL)
         {
            pc_Bus->SetDisabledLook(false);
         }
         if (pc_BusConnector != NULL)
         {
            pc_BusConnector->SetDisabledLook(false);
         }
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Delete temporary connector and leave connect state

   Restore scene default state
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdTopologyScene::m_RemoveConnectorAndLeaveConnectState()
{
   this->m_RemoveConnectorLine();
   this->m_LeaveConnectState();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set disabled mode for all interactable items in connect state

   \param[in]  opc_Node    Node to skip
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdTopologyScene::m_DisableEverythingForConnectState(const C_GiNode * const opc_Node) const
{
   //lint -e{948,774,506} Variable necessary to deactivate feature until finished
   if (C_SdTopologyScene::mhq_NewConnectState == true)
   {
      const QList<QGraphicsItem *> c_Items = this->items();

      //Disable all nodes and busses
      for (QList<QGraphicsItem *>::const_iterator c_It = c_Items.begin(); c_It != c_Items.end(); ++c_It)
      {

         C_GiNode * const pc_Node = dynamic_cast<C_GiNode * const>(*c_It);

         C_GiLiBus * const pc_Bus = dynamic_cast<C_GiLiBus * const>(*c_It);

         C_GiLiBusConnector * const pc_BusConnector =
            dynamic_cast<C_GiLiBusConnector * const>(C_SebUtil::h_GetHighestParent(*c_It));
         if ((pc_Node != NULL) && (pc_Node != opc_Node))
         {
            pc_Node->SetDrawWhiteFilter(true);
         }
         if (pc_Bus != NULL)
         {
            pc_Bus->SetDisabledLook(true);
         }
         if (pc_BusConnector != NULL)
         {
            pc_BusConnector->SetDisabledLook(true);
         }
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Display custom context menu for connection

   \param[in]  opc_Node                Node (part of connection)
   \param[in]  opc_Bus                 Bus (part of connection)
   \param[in]  orq_ChangeInterface     Flag if in change interface state
   \param[in]  orq_Reconnect           Flag if in reconnect state
   \param[in]  ors32_SpecialInterface  Interface number to ignore
   \param[in]  opc_Connector           Current bus connector for change of interface (and reconnect bus)
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdTopologyScene::m_ShowNewConnectionPopUp(const C_GiNode * const opc_Node, const C_GiLiBus * const opc_Bus,
                                                 const bool & orq_ChangeInterface, const bool & orq_Reconnect,
                                                 const sint32 & ors32_SpecialInterface,
                                                 C_GiLiBusConnector * const opc_Connector)
{
   if ((opc_Node != NULL) && (opc_Bus != NULL))
   {
      sint32 s32_SpecialInterface = -1;
      QGraphicsView * const pc_View = this->views().at(0);
      QPointer<C_OgePopUpDialog> const c_Dialog = new C_OgePopUpDialog(pc_View, pc_View);

      //Clean up state
      this->m_HandleHideToolTip();
      m_RevertOverrideCursor();
      if (opc_Connector != NULL)
      {
         const C_PuiSdNodeConnectionId * const pc_Data = opc_Connector->GetConnectionData();
         if (pc_Data != NULL)
         {
            if (pc_Data->e_InterfaceType == opc_Bus->GetType())
            {
               s32_SpecialInterface = ors32_SpecialInterface;
            }
         }
      }

      C_SdNodeComIfSetupWidget * const pc_ComIfWidget = new C_SdNodeComIfSetupWidget(*c_Dialog,
                                                                                     opc_Node->GetIndex(),
                                                                                     opc_Bus->GetIndex(),
                                                                                     s32_SpecialInterface);
      //Resize
      c_Dialog->SetSize(QSize(800, 450));
      if (pc_ComIfWidget->GetInteractionPossible() == true)
      {
         if (c_Dialog->exec() == static_cast<sintn>(QDialog::Accepted))
         {
            bool q_DatapoolL2;
            bool q_DatapoolECeS;
            bool q_DatapoolECoS;
            const uint32 u32_SelectedInterface = pc_ComIfWidget->GetSelectedInterface();

            pc_ComIfWidget->GetComDataPoolConfiguration(q_DatapoolL2, q_DatapoolECeS, q_DatapoolECoS);

            if (orq_ChangeInterface == true)
            {
               m_ChangeInterface(static_cast<uint8>(u32_SelectedInterface),
                                 pc_ComIfWidget->GetNodeId(), q_DatapoolL2, q_DatapoolECeS, q_DatapoolECoS,
                                 opc_Connector);
            }
            else
            {
               if (orq_Reconnect == true)
               {
                  this->mc_BusConnectorReconnectManager.ContextMenuAccepted(
                     u32_SelectedInterface, pc_ComIfWidget->GetNodeId(), q_DatapoolL2, q_DatapoolECeS, q_DatapoolECoS);
               }
               else
               {
                  m_ConnectNodeToBus(
                     static_cast<uint8>(u32_SelectedInterface),
                     pc_ComIfWidget->GetNodeId(), q_DatapoolL2, q_DatapoolECeS, q_DatapoolECoS);
               }
            }
         }
      }
      this->mc_BusConnectorReconnectManager.ContextMenuAboutToClose();

      if (c_Dialog != NULL)
      {
         c_Dialog->HideOverlay();
      }
   } //lint !e429  //no memory leak because of the parent of pc_Dialog and the Qt memory management
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Display custom context menu for connection

   \param[in]  opc_Node1   Node 1 (part of connection)
   \param[in]  opc_Node2   Node 2 (part of connection)
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdTopologyScene::m_ShowNewNodeToNodeConnectionPopUp(const C_GiNode * const opc_Node1,
                                                           const C_GiNode * const opc_Node2)
{
   if (((opc_Node1 != NULL) && (opc_Node2 != NULL)) && (opc_Node1 != opc_Node2))
   {
      QGraphicsView * const pc_View = this->views().at(0);
      QPointer<C_OgePopUpDialog> const c_Dialog = new C_OgePopUpDialog(pc_View, pc_View);

      //Clean up state
      this->m_HandleHideToolTip();
      m_RevertOverrideCursor();

      C_SdNodeToNodeConnectionSetupWidget * const pc_ComIfWidget = new C_SdNodeToNodeConnectionSetupWidget(*c_Dialog,
                                                                                                           opc_Node1->GetIndex(),
                                                                                                           opc_Node2->GetIndex());
      //Resize
      c_Dialog->SetSize(QSize(800, 450));
      if (pc_ComIfWidget->GetInteractionPossible() == true)
      {
         if (c_Dialog->exec() == static_cast<sintn>(QDialog::Accepted))
         {
            std::vector<uint64> c_Ids;
            bool q_Node1DatapoolL2;
            bool q_Node1DatapoolECeS;
            bool q_Node1DatapoolECoS;
            bool q_Node2DatapoolL2;
            bool q_Node2DatapoolECeS;
            bool q_Node2DatapoolECoS;

            // Adapt the COM datapool configuration for the first node
            pc_ComIfWidget->GetComDataPoolConfigurationNode1(q_Node1DatapoolL2, q_Node1DatapoolECeS,
                                                             q_Node1DatapoolECoS);

            // Adapt the COM datapool configuration for the second node
            pc_ComIfWidget->GetComDataPoolConfigurationNode2(q_Node2DatapoolL2, q_Node2DatapoolECeS,
                                                             q_Node2DatapoolECoS);

            if (pc_ComIfWidget->CheckIfCreateNew() == true)
            {
               const QPointF c_Node1Bottom(opc_Node1->sceneBoundingRect().center().x(),
                                           opc_Node1->sceneBoundingRect().bottom());
               const QPointF c_BusPosition = QPointF(c_Node1Bottom.x(), c_Node1Bottom.y() + 100.0);

               //4 unique ids
               c_Ids.push_back(this->m_GetNewUniqueID());
               c_Ids.push_back(this->m_GetNewUniqueID());
               c_Ids.push_back(this->m_GetNewUniqueID());
               c_Ids.push_back(this->m_GetNewUniqueID());

               this->mc_UndoManager.DoAddNodeToNodeConnectionAndCreateNewBus(c_Ids,
                                                                             pc_ComIfWidget->GetBusType(),
                                                                             pc_ComIfWidget->GetBusName(), c_BusPosition,
                                                                             opc_Node1->GetID(), opc_Node2->GetID(),
                                                                             static_cast<uint8>(
                                                                                pc_ComIfWidget->GetSelectedInterface1()),
                                                                             static_cast<uint8>(
                                                                                pc_ComIfWidget->GetSelectedInterface2()),
                                                                             pc_ComIfWidget->GetNodeId1(),
                                                                             pc_ComIfWidget->GetNodeId2(),
                                                                             q_Node1DatapoolL2, q_Node1DatapoolECeS, q_Node1DatapoolECoS,
                                                                             q_Node2DatapoolL2, q_Node2DatapoolECeS,
                                                                             q_Node2DatapoolECoS);
            }
            else
            {
               const uint32 u32_BusDataIndex = pc_ComIfWidget->GetBusIndex();
               uint64 u64_UniqueBusId = 0;
               const QPointF c_Node1Center = opc_Node1->sceneBoundingRect().center();
               const QPointF c_Node2Center = opc_Node2->sceneBoundingRect().center();
               QList<QGraphicsItem *>::const_iterator c_ItItem;
               const QList<QGraphicsItem *> & rc_Items = this->items();

               //2 unique ids
               c_Ids.push_back(this->m_GetNewUniqueID());
               c_Ids.push_back(this->m_GetNewUniqueID());

               //Map data index to unique index
               for (c_ItItem = rc_Items.begin(); c_ItItem != rc_Items.end(); ++c_ItItem)
               {

                  C_GiLiBus * const pc_Item = dynamic_cast<C_GiLiBus * const>(C_SebUtil::h_GetHighestParent(*c_ItItem));

                  if (pc_Item != NULL)
                  {
                     if (pc_Item->GetIndex() >= 0)
                     {
                        if (static_cast<uint32>(pc_Item->GetIndex()) == u32_BusDataIndex)
                        {
                           u64_UniqueBusId = pc_Item->GetID();
                        }
                     }
                  }
               }

               this->mc_UndoManager.DoAddNodeToNodeConnectionUsingExistingBus(c_Ids, u64_UniqueBusId, c_Node1Center,
                                                                              c_Node2Center,
                                                                              opc_Node1->GetID(), opc_Node2->GetID(),
                                                                              static_cast<uint8>(
                                                                                 pc_ComIfWidget->GetSelectedInterface1()),
                                                                              static_cast<uint8>(
                                                                                 pc_ComIfWidget->GetSelectedInterface2()),
                                                                              pc_ComIfWidget->GetNodeId1(),
                                                                              pc_ComIfWidget->GetNodeId2(),
                                                                              q_Node1DatapoolL2, q_Node1DatapoolECeS, q_Node1DatapoolECoS,
                                                                              q_Node2DatapoolL2, q_Node2DatapoolECeS,
                                                                              q_Node2DatapoolECoS);
            }
         }
      }

      if (c_Dialog != NULL)
      {
         c_Dialog->HideOverlay();
      }
   } //lint !e429  //no memory leak because of the parent of pc_Dialog and the Qt memory management
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Trigger change interface dialog

   \param[in,out]  opc_Item   Bus connection item
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdTopologyScene::m_ShowInterfaceChangePopUp(QGraphicsItem * const opc_Item)
{

   C_GiLiBusConnector * const pc_BusConn = dynamic_cast<C_GiLiBusConnector * const>(opc_Item);

   if (pc_BusConn != NULL)
   {
      const C_PuiSdNodeConnectionId * const pc_ConnectionData = pc_BusConn->GetConnectionData();
      //Get current interface
      if (pc_ConnectionData != NULL)
      {
         m_ShowNewConnectionPopUp(pc_BusConn->GetNodeItem(),
                                  pc_BusConn->GetBusItem(), true, false, pc_ConnectionData->u8_InterfaceNumber,
                                  pc_BusConn);
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Load subset of system definition entries

   \param[in]  orc_NodeIndices               Node indices to add
   \param[in]  orc_BusIndices                Bus indices to add
   \param[in]  orc_OtherStartIndices         Start indices
                                             0: Boundary
                                             1: Text element
                                             2: Line arrow
                                             3: Image
                                             4: Bus text element
   \param[in]  orq_Selection                 False: Ignore selection
   \param[in]  opc_AdditionalConnectionData  Additional data for bus connections
   \param[in]  opc_IDMap                     Optional map for IDs to use
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdTopologyScene::m_LoadSnapshot(const QVector<uint32> & orc_NodeIndices, const QVector<uint32> & orc_BusIndices,
                                       const QVector<uint32> & orc_OtherStartIndices, const bool & orq_Selection,
                                       const std::vector<C_PuiSdCompleteBusConnectionData> * const opc_AdditionalConnectionData,
                                       const QMap<C_PuiBsTemporaryDataID, uint64> * const opc_IDMap)
{
   QVector<uint32> c_SaveNodeIndices;
   QVector<uint32> c_SaveBusIndices;
   QVector<uint32> c_SaveIndices;

   //Fix index if necessary

   //Copy
   for (sint32 s32_It = 0; s32_It < orc_NodeIndices.size(); ++s32_It)
   {
      c_SaveNodeIndices.append(orc_NodeIndices[s32_It]);
   }

   //Copy
   for (sint32 s32_It = 0; s32_It < orc_BusIndices.size(); ++s32_It)
   {
      c_SaveBusIndices.append(orc_BusIndices[s32_It]);
   }

   //Copy
   for (sint32 s32_It = 0; s32_It < orc_OtherStartIndices.size(); ++s32_It)
   {
      c_SaveIndices.append(orc_OtherStartIndices[s32_It]);
   }

   //Fill up with zeros
   c_SaveIndices.append(0);
   c_SaveIndices.append(0);
   c_SaveIndices.append(0);
   c_SaveIndices.append(0);
   c_SaveIndices.append(0);

   this->m_LoadSubset(orc_NodeIndices, orc_BusIndices, orc_OtherStartIndices, orq_Selection,
                      opc_AdditionalConnectionData, opc_IDMap);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Add value to vector and update all values which are higher or equal

   \param[in,out]  orc_Vec    Vector
   \param[in]      oru32_New  New value
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdTopologyScene::mh_AddAndUpdateHigher(QVector<uint32> & orc_Vec, const uint32 & oru32_New)
{
   for (sint32 s32_ItVec = 0; s32_ItVec < orc_Vec.size(); ++s32_ItVec)
   {
      if (orc_Vec[s32_ItVec] >= oru32_New)
      {
         uint32 & ru32_CurVal = orc_Vec[s32_ItVec];
         ++ru32_CurVal;
      }
   }
   orc_Vec.push_back(oru32_New);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Bus connector is starting reconnect state

   \param[in]      ore_ConnectState    Connection state
   \param[in]      opc_Item            Bus connection item
   \param[in]      ope_Type            Last used bus type (optional)
                                       NULL: No special handling for any bus type
   \param[in,out]  opc_BusConnector    Bus connector
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdTopologyScene::m_BusConnectorReconnectionStart(const C_GiLiBusConnector::E_ConnectState & ore_ConnectState,
                                                        const QGraphicsItem * const opc_Item,
                                                        const C_OSCSystemBus::E_Type * const ope_Type,
                                                        C_GiLiBusConnectorBase * const opc_BusConnector)
{

   m_EnterConnectState(ore_ConnectState, dynamic_cast<const C_GiNode * const>(opc_Item), ope_Type);

   this->mc_BusConnectorReconnectManager.StartReconnectMode(dynamic_cast<C_GiLiBusConnector * const>(opc_BusConnector),
                                                            ore_ConnectState);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Show context menu and signal reconnect manager

   \param[in]      orc_ScenePos              Event scene position
   \param[in,out]  opc_Node                  Current node
   \param[in]      opc_Bus                   Current bus
   \param[in]      ors32_SpecialInterface    Last used interface
   \param[in,out]  opc_BusConnector          Bus connector
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdTopologyScene::m_ShowBusConnectorReconnectionContextMenu(const QPointF & orc_ScenePos,
                                                                  C_GiNode * const opc_Node,
                                                                  const C_GiLiBus * const opc_Bus,
                                                                  const sint32 & ors32_SpecialInterface,
                                                                  C_GiLiBusConnector * const opc_BusConnector)
{
   this->mc_BusConnectorReconnectManager.EnterWaitForContextMenuState(orc_ScenePos, opc_Node, opc_Bus);
   m_ShowNewConnectionPopUp(opc_Node, opc_Bus, false, true, ors32_SpecialInterface, opc_BusConnector);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Helper to reinitialize port display behaviour
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdTopologyScene::m_CleanUpPorts(void) const
{
   C_GiNode * pc_Node;

   QList<QGraphicsItem *> c_Items = this->items();

   //Fill
   for (QList<QGraphicsItem *>::iterator c_ItItem = c_Items.begin(); c_ItItem != c_Items.end();
        ++c_ItItem)
   {
      //Node
      try
      {

         pc_Node = dynamic_cast<C_GiNode *>(C_SebUtil::h_GetHighestParent(*c_ItItem));
         if (pc_Node != NULL)
         {
            pc_Node->ReevaluatePortState();
         }
      }
      catch (...)
      {
         //Cast failed
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Revert bus connector to previous node

   \param[in,out]  opc_BusConnector    Current bus connector
   \param[in,out]  opc_StartingNode    Previously connected node
   \param[in,out]  opc_LastNode        New / current node
   \param[in]      orc_ScenePos        Last known scene position of interaction point
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdTopologyScene::m_RevertBusConnectorNode(stw_opensyde_gui::C_GiLiBusConnector * const opc_BusConnector,
                                                 stw_opensyde_gui::C_GiNode * const opc_StartingNode,
                                                 stw_opensyde_gui::C_GiNode * const opc_LastNode,
                                                 const QPointF & orc_ScenePos) const
{
   if (opc_BusConnector != NULL)
   {
      opc_BusConnector->Revert(opc_StartingNode, opc_LastNode, orc_ScenePos);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Connect bus connector to new node

   \param[in,out]  opc_BusConnector          Current bus connector
   \param[in,out]  opc_StartingNode          Previously connected node
   \param[in,out]  opc_LastNode              New node
   \param[in]      orc_ConnectionPos         Position of connection event
   \param[in]      ors32_Interface           Newly used interface
   \param[in]      oru8_NodeId               New node id
   \param[in]      oq_ActivateDatapoolL2     Activate datapool L2
   \param[in]      oq_ActivateDatapoolECeS   Activate datapool ECeS
   \param[in]      oq_ActivateDatapoolECoS   Activate datapool ECoS
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdTopologyScene::m_ReconnectBusConnectorNode(const stw_opensyde_gui::C_GiLiBusConnector * const opc_BusConnector,
                                                    const C_GiNode * const opc_StartingNode,
                                                    const C_GiNode * const opc_LastNode,
                                                    const QPointF & orc_ConnectionPos, const sint32 & ors32_Interface,
                                                    const uint8 & oru8_NodeId, const bool oq_ActivateDatapoolL2,
                                                    const bool oq_ActivateDatapoolECeS,
                                                    const bool oq_ActivateDatapoolECoS)
{
   this->mc_UndoManager.DoReconnectNode(opc_BusConnector, opc_StartingNode, opc_LastNode,
                                        orc_ConnectionPos,
                                        ors32_Interface, oru8_NodeId, oq_ActivateDatapoolL2, oq_ActivateDatapoolECeS,
                                        oq_ActivateDatapoolECoS);
   m_UpdateHints();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Revert bus connector to previous bus

   \param[in,out]  opc_BusConnector    Current bus connector
   \param[in,out]  opc_StartingBus     Previously connected bus
   \param[in,out]  opc_LastBus         New / current bus
   \param[in]      orc_ScenePos        Last known scene position of interaction point
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdTopologyScene::m_RevertBusConnectorBus(stw_opensyde_gui::C_GiLiBusConnector * const opc_BusConnector,
                                                const stw_opensyde_gui::C_GiLiBus * const opc_StartingBus,
                                                const stw_opensyde_gui::C_GiLiBus * const opc_LastBus,
                                                const QPointF & orc_ScenePos) const
{
   if (opc_BusConnector != NULL)
   {
      opc_BusConnector->RevertBus(opc_StartingBus, opc_LastBus, orc_ScenePos);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Connect bus connector to new bus

   \param[in,out]  opc_BusConnector          Current bus connector
   \param[in,out]  opc_StartingBus           Previously connected bus
   \param[in,out]  opc_LastBus               New bus
   \param[in]      orc_ConnectionPos         Position of connection event
   \param[in]      ors32_Interface           Newly used interface
   \param[in]      oru8_NodeId               New node id
   \param[in]      oq_ActivateDatapoolL2     Activate datapool L2
   \param[in]      oq_ActivateDatapoolECeS   Activate datapool ECeS
   \param[in]      oq_ActivateDatapoolECoS   Activate datapool ECoS
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdTopologyScene::m_ReconnectBusConnectorBus(const stw_opensyde_gui::C_GiLiBusConnector * const opc_BusConnector,
                                                   const stw_opensyde_gui::C_GiLiBus * const opc_StartingBus,
                                                   const stw_opensyde_gui::C_GiLiBus * const opc_LastBus,
                                                   const QPointF & orc_ConnectionPos, const sint32 & ors32_Interface,
                                                   const uint8 & oru8_NodeId, const bool oq_ActivateDatapoolL2,
                                                   const bool oq_ActivateDatapoolECeS,
                                                   const bool oq_ActivateDatapoolECoS)
{
   this->mc_UndoManager.DoReconnectBus(opc_BusConnector, opc_StartingBus, opc_LastBus,
                                       orc_ConnectionPos,
                                       ors32_Interface, oru8_NodeId, oq_ActivateDatapoolL2, oq_ActivateDatapoolECeS,
                                       oq_ActivateDatapoolECoS);
   m_UpdateHints();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Clean up temporary line item
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdTopologyScene::m_RemoveConnectorLine(void)
{
   this->m_RemoveTemporaryLine(&this->mpc_ConnectorLine);
   //lint -e{845,944,948,774,506} Variable necessary to deactivate feature until finished
   if ((C_SdTopologyScene::mhq_NewConnectState == true) && (this->mpc_EmptyConnectItem != NULL))
   {
      delete (this->mpc_EmptyConnectItem);
      this->mpc_EmptyConnectItem = NULL;
   }
}

//----------------------------------------------------------------------------------------------------------------------
void C_SdTopologyScene::m_RemoveBusNameLine(void)
{
   this->m_RemoveTemporaryLine(&this->mpc_BusNameLine);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Clean up temporary line item

   \param[in]  opc_TemporaryLine    Pointer to pointer of Temporary line
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdTopologyScene::m_RemoveTemporaryLine(C_GiLiTemporaryLine ** const opc_TemporaryLine)
{
   if (*opc_TemporaryLine != NULL)
   {
      //Remove temporary line
      (*opc_TemporaryLine)->setVisible(false);
      this->removeItem(*opc_TemporaryLine);
      (*opc_TemporaryLine)->deleteLater();
      (*opc_TemporaryLine) = NULL;
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Restore all local override cursor

   TODO: evaluate if counter grows too big -> override cursor stack may cause performance problems
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdTopologyScene::m_RevertOverrideCursor(void)
{
   for (uint64 u64_It = 0; u64_It < this->mu64_MouseOverrideCounter; ++u64_It)
   {
      QApplication::restoreOverrideCursor();
   }
   this->mu64_MouseOverrideCounter = 0;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Init node data

   \param[in,out]  orc_OSCNode   newly created node
   \param[in]      orc_NodeType  Node type
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdTopologyScene::m_InitNodeData(C_OSCNode & orc_OSCNode, const QString & orc_NodeType) const
{
   orc_OSCNode.pc_DeviceDefinition =
      C_OSCSystemDefinition::hc_Devices.LookForDevice(orc_NodeType.toStdString().c_str());
   tgl_assert(orc_OSCNode.pc_DeviceDefinition != NULL);
   //default name: same as device type
   orc_OSCNode.c_Properties.c_Name = C_PuiSdHandler::h_AutomaticCStringAdaptation(
      orc_OSCNode.pc_DeviceDefinition->GetDisplayName().c_str()).toStdString().c_str();
   orc_OSCNode.c_DeviceType = orc_NodeType.toStdString().c_str();
   //---Init COM IF Settings (BEFORE initial datablock)
   this->m_InitNodeComIfSettings(orc_OSCNode);
   //Handle initial datablock (if necessary)
   if ((orc_OSCNode.pc_DeviceDefinition->q_ProgrammingSupport == false) &&
       (orc_OSCNode.c_Properties.e_FlashLoader != C_OSCNodeProperties::eFL_NONE))
   {
      C_OSCNodeApplication c_Appl;
      c_Appl.c_Name = C_GtGetText::h_GetText("Firmware");
      c_Appl.e_Type = C_OSCNodeApplication::eBINARY;
      c_Appl.c_Comment = C_GtGetText::h_GetText("Automatically generated Data Block.");
      orc_OSCNode.c_Applications.push_back(c_Appl);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Init Node Com interface setting

   This function is called after node create.
   Default concept: Activate all services if they are supported by the device type

   \param[in,out]  orc_OSCNode   newly created node
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdTopologyScene::m_InitNodeComIfSettings(C_OSCNode & orc_OSCNode) const
{
   //get available services of nodes device type

   //the node has not been added to the system definition yet, so the "pc_DeviceDefinition" pointer
   // was not set yet: search list of deviced
   const C_OSCDeviceDefinition * const pc_DeviceDefinition =
      C_OSCSystemDefinition::hc_Devices.LookForDevice(orc_OSCNode.c_DeviceType);

   tgl_assert(pc_DeviceDefinition != NULL);

   //create interfaces
   if (pc_DeviceDefinition != NULL)
   {
      orc_OSCNode.c_Properties.CreateComInterfaces(*(pc_DeviceDefinition));

      // In case of both protocols are supported, openSYDE is default
      //server and flashloader: Device Type settings = node settings.
      if (pc_DeviceDefinition->q_DiagnosticProtocolOpenSydeCan == true)
      {
         //openSYDE
         orc_OSCNode.c_Properties.e_DiagnosticServer = C_OSCNodeProperties::eDS_OPEN_SYDE;
      }
      else if (pc_DeviceDefinition->q_DiagnosticProtocolKefex == true)
      {
         //KEFEX
         orc_OSCNode.c_Properties.e_DiagnosticServer = C_OSCNodeProperties::eDS_KEFEX;
      }
      else
      {
         //not supported
         orc_OSCNode.c_Properties.e_DiagnosticServer = C_OSCNodeProperties::eDS_NONE;
      }

      //flashloader
      if (pc_DeviceDefinition->q_FlashloaderOpenSydeCan == true)
      {
         //open SYDE
         orc_OSCNode.c_Properties.e_FlashLoader = C_OSCNodeProperties::eFL_OPEN_SYDE;
      }
      else if (pc_DeviceDefinition->q_FlashloaderStwCan == true)
      {
         //STW
         orc_OSCNode.c_Properties.e_FlashLoader = C_OSCNodeProperties::eFL_STW;
      }
      else
      {
         //not supported
         orc_OSCNode.c_Properties.e_FlashLoader = C_OSCNodeProperties::eFL_NONE;
      }

      //init interfaces
      for (uint32 u32_Index = 0U; u32_Index < orc_OSCNode.c_Properties.c_ComInterfaces.size(); ++u32_Index)
      {
         C_OSCNodeComInterfaceSettings & rc_CurInterface = orc_OSCNode.c_Properties.c_ComInterfaces[u32_Index];
         const bool q_IsUpdateAvailable    = pc_DeviceDefinition->IsUpdateAvailable(rc_CurInterface.e_InterfaceType);
         const bool q_IsRoutingAvailable   = orc_OSCNode.IsRoutingAvailable(rc_CurInterface.e_InterfaceType);
         const bool q_IsDiagnosisAvailable = pc_DeviceDefinition->IsDiagnosisAvailable(rc_CurInterface.e_InterfaceType);
         rc_CurInterface.q_IsUpdateEnabled = q_IsUpdateAvailable;
         rc_CurInterface.q_IsRoutingEnabled = q_IsRoutingAvailable;
         rc_CurInterface.q_IsDiagnosisEnabled = q_IsDiagnosisAvailable;
      }
   }
}
