//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Offers visualization and functionality of a node (implementation)

   detailed description

   \copyright   Copyright 2016 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.h"

#include <limits>
#include <QGraphicsSceneMouseEvent>

#include "stwerrors.h"
#include "constants.h"
#include "C_GiLiBusConnector.h"
#include "C_GiNodeBoundary.h"
#include "C_GiNode.h"
#include "C_GtGetText.h"
#include "C_SdTopologyScene.h"
#include "C_PuiSdDataElement.h"
#include "C_PuiSdHandler.h"
#include "gitypes.h"
#include "C_SdUtil.h"
#include "C_PuiSdUtil.h"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */

using namespace stw_types;
using namespace stw_errors;
using namespace stw_opensyde_gui;
using namespace stw_opensyde_core;
using namespace stw_opensyde_gui_logic;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */
const sint32 ms32_INDEX_ABOVE = 0;
const sint32 ms32_INDEX_RIGHT = 1;
const sint32 ms32_INDEX_LEFT = 2;
const sint32 ms32_INDEX_BOTTOM = 3;

const float64 mf64_ACTION_POINT_OFFSET_NODE = 15.0;

const float64 C_GiNode::mhf64_MIN_WIDTH_NODE = 107.0;
const float64 C_GiNode::mhf64_MIN_HEIGHT_NODE = 71.0;
const float64 C_GiNode::mhf64_INITIAL_WIDTH_NODE = 165.0;
const float64 C_GiNode::mhf64_INITIAL_HEIGHT_NODE = 110.0;

const uint32 C_GiNode::mhu32_SCALE_CATEGORY_0 = 7U;
const uint32 C_GiNode::mhu32_SCALE_CATEGORY_1 = 0U; // no scaling -> default
const uint32 C_GiNode::mhu32_SCALE_CATEGORY_2 = 1U;
const uint32 C_GiNode::mhu32_SCALE_CATEGORY_3 = 2U;
const uint32 C_GiNode::mhu32_SCALE_CATEGORY_4 = 3U;
const uint32 C_GiNode::mhu32_SCALE_CATEGORY_5 = 4U;
const uint32 C_GiNode::mhu32_SCALE_CATEGORY_6 = 5U;
const uint32 C_GiNode::mhu32_SCALE_CATEGORY_7 = 6U;

const float64 C_GiNode::mhaf64_SCALE_MIN_WIDTH_NODE[7] =
{
   150.0, 200.0, 250.0, 300.0, 350.0, 400.0, 450.0
};
const float64 C_GiNode::mhaf64_SCALE_MIN_HEIGHT_NODE[7] =
{
   100.0, 130.0, 165.0, 200.0, 230.0, 265.0, 300.0
};

/* -- Types --------------------------------------------------------------------------------------------------------- */

/* -- Global Variables ---------------------------------------------------------------------------------------------- */

/* -- Module Global Variables --------------------------------------------------------------------------------------- */

/* -- Module Global Function Prototypes ----------------------------------------------------------------------------- */

/* -- Implementation ------------------------------------------------------------------------------------------------ */

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default constructor

   Set up GUI with all elements.

   \param[in]      ors32_Index   Index of data element in system definition
   \param[in]      oru64_ID      Unique ID
   \param[in]      orf64_Width   Width of node
   \param[in]      orf64_Height  Height of node
   \param[in,out]  opc_Parent    Optional pointer to parent
*/
//----------------------------------------------------------------------------------------------------------------------
C_GiNode::C_GiNode(const sint32 & ors32_Index, const uint64 & oru64_ID, const float64 & orf64_Width,
                   const float64 & orf64_Height, QGraphicsItem * const opc_Parent) :
   //lint -e{1938}  static const is guaranteed preinitialized before main
   C_GiRectBaseGroup(ors32_Index, C_PuiSdDataElement::eNODE, oru64_ID, mhf64_MIN_WIDTH_NODE,
                     mhf64_MIN_HEIGHT_NODE, mf64_ACTION_POINT_OFFSET_NODE, false, opc_Parent),
   mq_ErrorIconHovered(false),
   mq_Valid(true),
   mq_DrawWhiteFilter(false),
   ms32_IconSize(24)
{
   QString c_Name = "Node";
   uint32 u32_SubNodesCount = 0;
   uint32 u32_NodeSquadIndex;
   const C_OSCNode * const pc_Node = C_PuiSdHandler::h_GetInstance()->GetOSCNodeConst(this->ms32_Index);

   if (pc_Node != NULL)
   {
      c_Name = C_PuiSdUtil::h_GetNodeBaseNameOrName(this->ms32_Index);

      //is part of squad?
      if (C_PuiSdHandler::h_GetInstance()->GetNodeSquadIndexWithNodeIndex(this->ms32_Index,
                                                                          u32_NodeSquadIndex) == C_NO_ERR)
      {
         //squad node
         const stw_opensyde_core::C_OSCNodeSquad * const pc_NodeSquad =
            C_PuiSdHandler::h_GetInstance()->GetOSCNodeSquadConst(u32_NodeSquadIndex);
         if (pc_NodeSquad != NULL)
         {
            u32_SubNodesCount = static_cast<uint32>(pc_NodeSquad->c_SubNodeIndexes.size());
         }
      }
      else
      {
         //no squad node
         u32_SubNodesCount = 0;
      }
   }

   //create boundary

   //lint -e{1938}  static const is guaranteed preinitialized before main
   this->mpc_Boundary = new C_GiNodeBoundary(c_Name, std::max(mhf64_MIN_WIDTH_NODE, orf64_Width),
                                             std::max(mhf64_MIN_HEIGHT_NODE, orf64_Height), u32_SubNodesCount);
   this->m_DetectIconSize();

   // Notify the base class about the boundary as biggest item as orientation. Very important!
   this->m_SetBiggestItem(*this->mpc_Boundary);

   this->m_InitPorts();
   this->m_InitConflictIcon();

   //add parent (with all children to group)
   this->addToGroup(this->mpc_Boundary);

   //lint -e{1938}  static const is guaranteed preinitialized before main
   this->m_UpdateItems((std::max(mhf64_MIN_WIDTH_NODE, orf64_Width) - mhf64_MIN_WIDTH_NODE),
                       (std::max(mhf64_MIN_HEIGHT_NODE, orf64_Height) - mhf64_MIN_HEIGHT_NODE),
                       true);

   // Init z order
   this->setZValue(mf64_ZORDER_INIT_NODE);

   //Allow hover events for tool tip hide
   this->setAcceptHoverEvents(true);

   // check node for errors
   this->CheckNodeForChanges();
}

//----------------------------------------------------------------------------------------------------------------------
void C_GiNode::m_InitPorts()
{
   //Create ports
   this->mc_Ports.append(new C_GiPort());
   this->mc_Ports.append(new C_GiPort());
   this->mc_Ports.append(new C_GiPort());
   this->mc_Ports.append(new C_GiPort());

   {
      //configure port above
      this->mc_Ports[ms32_INDEX_ABOVE]->setParentItem(this->mpc_Boundary);
      this->mc_Ports[ms32_INDEX_ABOVE]->setX(
         this->mc_Ports[ms32_INDEX_ABOVE]->x() +
         ((this->mpc_Boundary->boundingRect().width() / 2.0) -
          (this->mc_Ports[ms32_INDEX_ABOVE]->boundingRect().width() / 2.0)));
      this->mc_Ports[ms32_INDEX_ABOVE]->setY((
                                                this->mc_Ports[ms32_INDEX_ABOVE]->y() -
                                                this->mc_Ports[ms32_INDEX_ABOVE]->boundingRect().height()) + 2.0);
   }

   {
      //configure port right
      this->mc_Ports[ms32_INDEX_RIGHT]->setParentItem(this->mpc_Boundary);
      this->mc_Ports[ms32_INDEX_RIGHT]->setX((
                                                this->mc_Ports[ms32_INDEX_RIGHT]->x() +
                                                (this->mpc_Boundary->boundingRect().width()) +
                                                this->mc_Ports[ms32_INDEX_RIGHT]->boundingRect().height()) - 2.0);
      this->mc_Ports[ms32_INDEX_RIGHT]->setY(
         this->mc_Ports[ms32_INDEX_RIGHT]->y() -
         ((-1.0 * (this->mc_Ports[ms32_INDEX_RIGHT]->boundingRect().height() / 2.0)) - 8.0));
      this->mc_Ports[ms32_INDEX_RIGHT]->setRotation(90.0);
   }

   {
      //configure port left
      this->mc_Ports[ms32_INDEX_LEFT]->setParentItem(this->mpc_Boundary);
      this->mc_Ports[ms32_INDEX_LEFT]->setX((
                                               this->mc_Ports[ms32_INDEX_LEFT]->x() -
                                               this->mc_Ports[ms32_INDEX_LEFT]->boundingRect().height()) + 2.0);
      this->mc_Ports[ms32_INDEX_LEFT]->setY(
         this->mc_Ports[ms32_INDEX_LEFT]->y() +
         (((this->mpc_Boundary->boundingRect().height()) -
           (this->mc_Ports[ms32_INDEX_LEFT]->boundingRect().height() / 2.0)) - 7.0));
      this->mc_Ports[ms32_INDEX_LEFT]->setRotation(-90.0);
   }

   {
      //configure port bottom
      this->mc_Ports[ms32_INDEX_BOTTOM]->setParentItem(this->mpc_Boundary);
      this->mc_Ports[ms32_INDEX_BOTTOM]->setX(
         this->mc_Ports[ms32_INDEX_BOTTOM]->x() +
         ((this->mpc_Boundary->boundingRect().width() / 2.0) +
          (this->mc_Ports[ms32_INDEX_BOTTOM]->boundingRect().width() / 2.0)));
      this->mc_Ports[ms32_INDEX_BOTTOM]->setY((
                                                 this->mc_Ports[ms32_INDEX_BOTTOM]->y() +
                                                 this->mc_Ports[ms32_INDEX_BOTTOM]->boundingRect().height() +
                                                 this->mpc_Boundary->boundingRect().height()) - 2.01);
      this->mc_Ports[ms32_INDEX_BOTTOM]->setRotation(180.0);
   }
}

//----------------------------------------------------------------------------------------------------------------------
void C_GiNode::m_InitConflictIcon(void)
{
   const float64 f64_PosX = (this->mpc_Boundary->boundingRect().width() - 33.0) - // static offset for correct position
                            (static_cast<float64>(this->ms32_IconSize) - 24.0);   // offset of scaled icon

   // create the conflict icon
   this->mpc_ConflictIcon = new C_GiRectPixmap(QRectF(f64_PosX, 9.0,
                                                      static_cast<float64>(this->ms32_IconSize),
                                                      static_cast<float64>(this->ms32_IconSize)));

   this->mpc_ConflictIcon->SetSvg("://images/Error_iconV2.svg");

   // set the position of the icon
   this->mpc_ConflictIcon->setParentItem(this->mpc_Boundary);

   // the icon will be shown if a conflict is detected
   this->mpc_ConflictIcon->setVisible(false);
}

//----------------------------------------------------------------------------------------------------------------------
void C_GiNode::m_DetectIconSize(void)
{
   const uint32 u32_ScaleCategory = this->m_GetScaleCategory();

   switch (u32_ScaleCategory)
   {
   case mhu32_SCALE_CATEGORY_0:
      this->ms32_IconSize = 16;
      break;
   case mhu32_SCALE_CATEGORY_1:
      this->ms32_IconSize = 24;
      break;
   case mhu32_SCALE_CATEGORY_2:
      this->ms32_IconSize = 32;
      break;
   case mhu32_SCALE_CATEGORY_3:
      this->ms32_IconSize = 40;
      break;
   case mhu32_SCALE_CATEGORY_4:
      this->ms32_IconSize = 48;
      break;
   case mhu32_SCALE_CATEGORY_5:
      this->ms32_IconSize = 56;
      break;
   case mhu32_SCALE_CATEGORY_6:
      this->ms32_IconSize = 64;
      break;
   case mhu32_SCALE_CATEGORY_7:
      this->ms32_IconSize = 72;
      break;
   default:
      // fallback
      this->ms32_IconSize = 16;
      break;
   }
}

//----------------------------------------------------------------------------------------------------------------------
void C_GiNode::m_UpdateItems(const float64 of64_DiffWidth, const float64 of64_DiffHeight, const bool oq_Initial)
{
   const uint32 u32_ScaleCategory = this->m_GetScaleCategory();
   QFont c_BoundaryFont;
   const sint32 s32_OldIconSize = this->ms32_IconSize;

   // update the scale category
   this->m_DetectIconSize();

   // adapt conflict icon
   // and adapt ports
   // move the ports to the center of the respective side
   if (oq_Initial == false)
   {
      const sint32 s32_IconSizeDiff = this->ms32_IconSize - s32_OldIconSize;
      this->mpc_ConflictIcon->moveBy(of64_DiffWidth - static_cast<float64>(s32_IconSizeDiff), 0.0);
      this->mpc_ConflictIcon->SetNewSize(QSizeF(static_cast<float64>(this->ms32_IconSize),
                                                static_cast<float64>(this->ms32_IconSize)));
      this->mpc_ConflictIcon->update();

      // move only by mouse movements
      this->mc_Ports[ms32_INDEX_ABOVE]->moveBy((of64_DiffWidth / 2.0), 0.0);
      this->mc_Ports[ms32_INDEX_BOTTOM]->moveBy((of64_DiffWidth / 2.0), of64_DiffHeight);
      this->mc_Ports[ms32_INDEX_LEFT]->moveBy(0.0, (of64_DiffHeight / 2.0));
      this->mc_Ports[ms32_INDEX_RIGHT]->moveBy(of64_DiffWidth, 0.0);
   }
   else
   {
      // move only by constructor
      this->mc_Ports[ms32_INDEX_LEFT]->moveBy(0.0, (of64_DiffHeight / 2.0) * -1.0);
   }

   // move on both situations
   this->mc_Ports[ms32_INDEX_RIGHT]->moveBy(0.0, (of64_DiffHeight / 2.0));

   // adapting the size of the ports
   this->mc_Ports[ms32_INDEX_ABOVE]->StretchPort((of64_DiffWidth / 2.0));
   this->mc_Ports[ms32_INDEX_BOTTOM]->StretchPort((of64_DiffWidth / 2.0));
   if (this->mpc_Boundary->f64_Height >= mhf64_MIN_WIDTH_NODE)
   {
      // Resize the two ports only if the height is minimum long as the width
      // The aim is to get the ports left and right to the same proportions like the port above and bottom
      this->mc_Ports[ms32_INDEX_LEFT]->StretchPort((of64_DiffHeight / 2.0));
      this->mc_Ports[ms32_INDEX_RIGHT]->StretchPort((of64_DiffHeight / 2.0));
   }
   else
   {
      // below the minimum resizing size, reset to default
      this->mc_Ports[ms32_INDEX_LEFT]->ResizePortToDefault();
      this->mc_Ports[ms32_INDEX_RIGHT]->ResizePortToDefault();
   }

   this->mc_Ports[ms32_INDEX_ABOVE]->update();
   this->mc_Ports[ms32_INDEX_BOTTOM]->update();
   this->mc_Ports[ms32_INDEX_LEFT]->update();
   this->mc_Ports[ms32_INDEX_RIGHT]->update();

   switch (u32_ScaleCategory)
   {
   case mhu32_SCALE_CATEGORY_0:
      c_BoundaryFont = mc_STYLE_GUIDE_FONT_REGULAR_13;
      break;
   case mhu32_SCALE_CATEGORY_1:
      c_BoundaryFont = mc_STYLE_GUIDE_FONT_REGULAR_18;
      break;
   case mhu32_SCALE_CATEGORY_2:
      c_BoundaryFont = mc_STYLE_GUIDE_FONT_REGULAR_23;
      break;
   case mhu32_SCALE_CATEGORY_3:
      c_BoundaryFont = mc_STYLE_GUIDE_FONT_REGULAR_28;
      break;
   case mhu32_SCALE_CATEGORY_4:
      c_BoundaryFont = mc_STYLE_GUIDE_FONT_REGULAR_33;
      break;
   case mhu32_SCALE_CATEGORY_5:
      c_BoundaryFont = mc_STYLE_GUIDE_FONT_REGULAR_38;
      break;
   case mhu32_SCALE_CATEGORY_6:
      c_BoundaryFont = mc_STYLE_GUIDE_FONT_REGULAR_43;
      break;
   case mhu32_SCALE_CATEGORY_7:
      c_BoundaryFont = mc_STYLE_GUIDE_FONT_REGULAR_48;
      break;
   default:
      // fallback
      c_BoundaryFont = mc_STYLE_GUIDE_FONT_REGULAR_13;
      break;
   }

   this->mpc_Boundary->SetFont(c_BoundaryFont);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   default destructor

   Clean up.
*/
//----------------------------------------------------------------------------------------------------------------------
C_GiNode::~C_GiNode()
{
   for (sint32 s32_ItConn = 0; s32_ItConn < this->mc_Connections.size(); ++s32_ItConn)
   {
      this->mc_Connections[s32_ItConn]->DeleteConnection();
      if (this->scene() != NULL)
      {
         this->scene()->removeItem(this->mc_Connections[s32_ItConn]);
         this->scene()->removeEventFilter(this->mc_Connections[s32_ItConn]);
      }
      this->mc_Connections[s32_ItConn]->deleteLater();
      this->mc_Connections[s32_ItConn] = NULL;
   }
   //Deleted via Qt parent mechanism
   this->mpc_Boundary = NULL;
   this->mpc_ConflictIcon = NULL;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Find closest point in shape to scene position

   \param[in]   orc_ScenePoint   Scene position
   \param[out]  orc_Closest      Closest point in shape
*/
//----------------------------------------------------------------------------------------------------------------------
void C_GiNode::FindClosestPoint(const QPointF & orc_ScenePoint, QPointF & orc_Closest) const
{
   //
   QPointF c_CurClosest;
   float64 f64_CurDist;
   float64 f64_Best = std::numeric_limits<float64>::max();

   for (sint32 s32_ItPort = 0; s32_ItPort < this->mc_Ports.size(); ++s32_ItPort)
   {
      this->mc_Ports[s32_ItPort]->FindClosestPoint(orc_ScenePoint, c_CurClosest);
      C_GiBiConnectableItem::h_DistToPoint(orc_ScenePoint, c_CurClosest, f64_CurDist);
      if (f64_Best > f64_CurDist)
      {
         f64_Best = f64_CurDist;
         orc_Closest = c_CurClosest;
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Find closest port in shape to scene position

   \param[in]   orc_ScenePoint   Scene position
   \param[out]  orpc_Closest     Closest port in shape
*/
//----------------------------------------------------------------------------------------------------------------------
void C_GiNode::FindClosestPort(const QPointF & orc_ScenePoint, C_GiPort * (&orpc_Closest)) const
{
   QPointF c_CurClosest;
   float64 f64_CurDist;
   float64 f64_Best = std::numeric_limits<float64>::max();

   for (sint32 s32_ItPort = 0; s32_ItPort < this->mc_Ports.size(); ++s32_ItPort)
   {
      this->mc_Ports[s32_ItPort]->FindClosestPoint(orc_ScenePoint, c_CurClosest);
      C_GiBiConnectableItem::h_DistToPoint(orc_ScenePoint, c_CurClosest, f64_CurDist);
      if (f64_Best > f64_CurDist)
      {
         f64_Best = f64_CurDist;
         orpc_Closest = this->mc_Ports[s32_ItPort];
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Returns the type of this itme

   \return  ID
*/
//----------------------------------------------------------------------------------------------------------------------
sintn C_GiNode::type() const
{
   return msn_GRAPHICS_ITEM_NODE;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Check if node has at least one connection of this bus type

   \param[in]  oe_Type  Bus type

   \return
   true     Node has connection of this type
   false    Node has not a connection of this type
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_GiNode::HasConnectionType(const C_OSCSystemBus::E_Type oe_Type) const
{
   bool q_Return = false;

   const C_OSCNode * const pc_Node = C_PuiSdHandler::h_GetInstance()->GetOSCNodeConst(this->ms32_Index);

   if (pc_Node != NULL)
   {
      q_Return = C_SdUtil::h_HasConnectionType(*pc_Node, oe_Type);
   }

   return q_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Check if there are any connections available for this bus type

   \param[in]  ore_Type    Bus type

   \retval  true   connection available
   \retval  false  connection not available
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_GiNode::CheckConnectionAvailable(const C_OSCSystemBus::E_Type & ore_Type) const
{
   bool q_Retval = false;

   const C_OSCNode * const pc_Node = C_PuiSdHandler::h_GetInstance()->GetOSCNodeConst(this->ms32_Index);

   if (pc_Node != NULL)
   {
      q_Retval = C_SdUtil::h_CheckNodeInterfaceAvailable(pc_Node->c_Properties.c_ComInterfaces, ore_Type);
   }

   return q_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Check if the specified interface is available

   \param[in]  ore_Type          Bus type
   \param[in]  oru8_Interface    Interface number

   \retval  true   interface available
   \retval  false  interface not available
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_GiNode::CheckInterfaceAvailable(const C_OSCSystemBus::E_Type & ore_Type, const uint8 & oru8_Interface)
{
   bool q_Retval = CheckConnectionAvailable(ore_Type);

   if (q_Retval == true)
   {
      const C_OSCNode * const pc_Node = C_PuiSdHandler::h_GetInstance()->GetOSCNodeConst(this->ms32_Index);
      this->UpdateData();
      if (pc_Node != NULL)
      {
         const C_OSCNodeComInterfaceSettings * const pc_ComInterface = pc_Node->c_Properties.GetComInterface(ore_Type,
                                                                                                             oru8_Interface);
         if (pc_ComInterface != NULL)
         {
            q_Retval = !pc_ComInterface->GetBusConnectedRawValue();
         }
      }
   }
   return q_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get index of specified connection

   \param[in]  opc_Connection    Connection

   \return
   index of specified connection
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_GiNode::GetIndexOfConnector(const C_GiLiBusConnector * const opc_Connection) const
{
   sint32 s32_Retval = -1;

   if (opc_Connection != NULL)
   {
      for (sint32 s32_ItConn = 0; s32_ItConn < this->mc_Connections.size(); ++s32_ItConn)
      {
         if (opc_Connection == this->mc_Connections[s32_ItConn])
         {
            s32_Retval = s32_ItConn;
         }
      }
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get data node

   \param[out]  orpc_Node  Node
*/
//----------------------------------------------------------------------------------------------------------------------
void C_GiNode::GetOSCNodeConst(const C_OSCNode * (&orpc_Node)) const
{
   orpc_Node = C_PuiSdHandler::h_GetInstance()->GetOSCNodeConst(static_cast<uint32>(this->ms32_Index));
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Restore default mouse cursor
*/
//----------------------------------------------------------------------------------------------------------------------
void C_GiNode::RestoreDefaultCursor(void)
{
   C_GiRectBaseGroup::RestoreDefaultCursor();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Change mouse cursor temporarily

   \param[in]  orc_TemporaryCursor  New mouse cursor
*/
//----------------------------------------------------------------------------------------------------------------------
void C_GiNode::SetTemporaryCursor(const QCursor & orc_TemporaryCursor)
{
   C_GiRectBaseGroup::SetTemporaryCursor(orc_TemporaryCursor);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   generate custom tool tip
*/
//----------------------------------------------------------------------------------------------------------------------
void C_GiNode::GenerateHint(void)
{
   if (this->mq_ErrorIconHovered == true)
   {
      this->SetDefaultToolTipHeading(C_GtGetText::h_GetText("Node has invalid content"));
      this->SetDefaultToolTipContent(this->mc_ErrorText);
      this->SetDefaultToolTipType(C_NagToolTip::eERROR);
   }
   else
   {
      const C_OSCNode * const pc_Node = C_PuiSdHandler::h_GetInstance()->GetOSCNodeConst(this->ms32_Index);

      this->SetDefaultToolTipType(C_NagToolTip::eDEFAULT);

      if (pc_Node != NULL)
      {
         const C_OSCDeviceDefinition * const pc_Device = pc_Node->pc_DeviceDefinition;
         if (pc_Device != NULL)
         {
            bool q_Found;
            QString c_ToolTip;
            QString c_Entry;
            QString c_BusName;
            QString c_Title;
            //Translation: 1 = bus name
            //tooltip title
            c_Title = this->GetText();
            c_Title.append(static_cast<QString>(" (Type: %1)").arg(pc_Node->pc_DeviceDefinition->c_DeviceName.c_str()));
            this->SetDefaultToolTipHeading(c_Title);

            //comment
            if (pc_Node->c_Properties.c_Comment.IsEmpty() == false)
            {
               c_ToolTip = pc_Node->c_Properties.c_Comment.c_str();
               c_ToolTip.append("\n\n");
            }

            c_ToolTip.append(C_GtGetText::h_GetText("Communication Interface Information:\n"));

            //CAN
            if (pc_Device->u8_NumCanBusses > 0)
            {
               for (uint8 u8_ItInterface = 0; u8_ItInterface < pc_Device->u8_NumCanBusses; ++u8_ItInterface)
               {
                  q_Found = false;
                  for (uint32 u32_ItBusConnection = 0;
                       (u32_ItBusConnection < pc_Node->c_Properties.c_ComInterfaces.size()) && (q_Found == false);
                       ++u32_ItBusConnection)
                  {
                     const C_OSCNodeComInterfaceSettings & rc_Conn =
                        pc_Node->c_Properties.c_ComInterfaces[u32_ItBusConnection];
                     if (rc_Conn.e_InterfaceType == C_OSCSystemBus::E_Type::eCAN)
                     {
                        if (rc_Conn.u8_InterfaceNumber == u8_ItInterface)
                        {
                           if (rc_Conn.GetBusConnectedRawValue() == true)
                           {
                              const C_OSCSystemBus * const pc_Bus = C_PuiSdHandler::h_GetInstance()->GetOSCBus(
                                 rc_Conn.u32_BusIndex);
                              if (pc_Bus != NULL)
                              {
                                 c_BusName = C_GtGetText::h_GetText("Linked to ");
                                 c_BusName.append(pc_Bus->c_Name.c_str());
                                 //Bus + Node Id Info
                                 if (pc_Bus->e_Type == C_OSCSystemBus::eCAN)
                                 {
                                    c_BusName.append(static_cast<QString>(" (Bitrate: %1 kbit/s)").
                                                     arg(QString::number(pc_Bus->u64_BitRate / 1000ULL)));
                                 }
                                 q_Found = true;
                              }
                           }
                        }
                     }
                  }
                  if (q_Found == false)
                  {
                     c_BusName = C_GtGetText::h_GetText("-");
                  }
                  //Translation: 1 = Interface number, 2 = Bus name

                  if (c_Entry.isEmpty() == true)
                  {
                     c_Entry = "";
                  }
                  else
                  {
                     c_Entry = "\n";
                  }
                  c_ToolTip.append(c_Entry);
                  c_Entry =  static_cast<QString>("%1: %2").arg(C_PuiSdUtil::h_GetInterfaceName(C_OSCSystemBus::eCAN,
                                                                                                u8_ItInterface),
                                                                c_BusName);
                  c_ToolTip.append(c_Entry);
               }
            }
            //Ethernet
            if (pc_Device->u8_NumEthernetBusses > 0)
            {
               for (uint8 u8_ItInterface = 0; u8_ItInterface < pc_Device->u8_NumEthernetBusses; ++u8_ItInterface)
               {
                  q_Found = false;
                  for (uint32 u32_ItBusConnection = 0;
                       (u32_ItBusConnection < pc_Node->c_Properties.c_ComInterfaces.size()) && (q_Found == false);
                       ++u32_ItBusConnection)
                  {
                     const C_OSCNodeComInterfaceSettings & rc_Conn =
                        pc_Node->c_Properties.c_ComInterfaces[u32_ItBusConnection];
                     if (rc_Conn.e_InterfaceType == C_OSCSystemBus::E_Type::eETHERNET)
                     {
                        if (rc_Conn.u8_InterfaceNumber == u8_ItInterface)
                        {
                           if (rc_Conn.GetBusConnectedRawValue() == true)
                           {
                              const C_OSCSystemBus * const pc_Bus = C_PuiSdHandler::h_GetInstance()->GetOSCBus(
                                 rc_Conn.u32_BusIndex);
                              if (pc_Bus != NULL)
                              {
                                 c_BusName = C_GtGetText::h_GetText("Linked to ");
                                 c_BusName.append(pc_Bus->c_Name.c_str());

                                 q_Found = true;
                              }
                           }
                        }
                     }
                  }
                  if (q_Found == false)
                  {
                     c_BusName = C_GtGetText::h_GetText("-");
                  }
                  //Translation: 1 = Interface number, 2 = Bus name
                  if (c_Entry.isEmpty() == true)
                  {
                     c_Entry = "";
                  }
                  else
                  {
                     c_Entry = "\n";
                  }
                  c_ToolTip.append(c_Entry);
                  c_Entry =
                     static_cast<QString>("%1: %2").arg(C_PuiSdUtil::h_GetInterfaceName(C_OSCSystemBus::eETHERNET,
                                                                                        u8_ItInterface),
                                                        c_BusName);
                  c_ToolTip.append(c_Entry);
               }
            }

            this->SetDefaultToolTipContent(c_ToolTip);
         }
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Reset port looks

   Recount port connections and display ports accordingly
*/
//----------------------------------------------------------------------------------------------------------------------
void C_GiNode::ReevaluatePortState(void)
{
   C_GiPort * pc_CurPort;
   const C_GiPort * pc_CurRefPort;
   C_GiLiBusConnector * pc_CurConn;

   for (QVector<C_GiPort *>::iterator pc_ItPort = mc_Ports.begin(); pc_ItPort != mc_Ports.end(); ++pc_ItPort)
   {
      pc_CurPort = *pc_ItPort;
      pc_CurPort->AbortTemporaryUnregister();
      pc_CurPort->ResetConnectorCount();
   }
   for (QVector<C_GiLiBusConnector *>::iterator pc_ItConn =
           mc_Connections.begin(); pc_ItConn != mc_Connections.end();
        ++pc_ItConn)
   {
      pc_CurConn = *pc_ItConn;
      if (pc_CurConn != NULL)
      {
         pc_CurRefPort = pc_CurConn->GetPortItem();
         if (pc_CurRefPort != NULL)
         {
            for (QVector<C_GiPort *>::iterator pc_ItPort = mc_Ports.begin(); pc_ItPort != mc_Ports.end();
                 ++pc_ItPort)
            {
               pc_CurPort = *pc_ItPort;
               if (pc_CurRefPort == pc_CurPort)
               {
                  pc_CurPort->AddConnectorToRegistry();
               }
            }
         }
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Checking the node data for errors and updates the conflict icon
*/
//----------------------------------------------------------------------------------------------------------------------
void C_GiNode::CheckNodeForChanges(void)
{
   //check node (Datapools)
   if (this->mq_DrawWhiteFilter == false)
   {
      this->mc_ErrorText = "";

      this->mpc_ConflictIcon->setVisible(this->m_UpdateError());
   }
   else
   {
      // error icon irrelevant when not connected to view
      this->mpc_ConflictIcon->setVisible(false);
   }

   //check for new name
   this->mpc_Boundary->SetText(this->GetText());
   this->mpc_Boundary->update();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Overwritten mouse press event slot

   \param[in,out]  opc_Event  Event identification and information
*/
//----------------------------------------------------------------------------------------------------------------------
void C_GiNode::mousePressEvent(QGraphicsSceneMouseEvent * const opc_Event)
{
   C_GiRectBaseGroup::mousePressEvent(opc_Event);

   //Hide tool tip
   Q_EMIT this->SigHideToolTip();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Overwritten mouse move event slot

   \param[in,out]  opc_Event  Event identification and information
*/
//----------------------------------------------------------------------------------------------------------------------
void C_GiNode::hoverMoveEvent(QGraphicsSceneHoverEvent * const opc_Event)
{
   const bool q_PreviousState = this->mq_ErrorIconHovered;

   this->mq_ErrorIconHovered = false;

   if (this->mpc_ConflictIcon != NULL)
   {
      if (this->mpc_ConflictIcon->isVisible() == true)
      {
         if (this->mpc_ConflictIcon->sceneBoundingRect().contains(opc_Event->scenePos()) == true)
         {
            this->mq_ErrorIconHovered = true;
         }
      }
   }
   if (q_PreviousState != this->mq_ErrorIconHovered)
   {
      Q_EMIT this->SigHideToolTip();
   }

   C_GiRectBaseGroup::hoverMoveEvent(opc_Event);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Overwritten hover leave event slot

   Here: Hide tool tip

   \param[in,out]  opc_Event  Event identification and information
*/
//----------------------------------------------------------------------------------------------------------------------
void C_GiNode::hoverLeaveEvent(QGraphicsSceneHoverEvent * const opc_Event)
{
   Q_EMIT this->SigHideToolTip();

   C_GiRectBaseGroup::hoverLeaveEvent(opc_Event);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get current error state and update error tooltip accordingly

   \return
   True  Error detected
   False No error detected
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_GiNode::m_UpdateError(void)
{
   bool q_ErrorDetected;

   C_SdUtil::h_GetErrorToolTipNode(static_cast<uint32>(this->ms32_Index), this->mc_ErrorText, q_ErrorDetected, true);
   return q_ErrorDetected;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get current error status

   \retval   true    Error active
   \retval   false   Error not yet detected
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_GiNode::m_GetErrorStatus(void) const
{
   bool q_Retval = true;

   if (this->mpc_ConflictIcon != NULL)
   {
      q_Retval = this->mpc_ConflictIcon->isVisible();
   }
   return q_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
void C_GiNode::m_ResizeUpdateItems(const float64 of64_DiffWidth, const float64 of64_DiffHeight)
{
   this->m_UpdateItems(of64_DiffWidth, of64_DiffHeight, false);
}

//----------------------------------------------------------------------------------------------------------------------
void C_GiNode::m_SetDrawBorder(const bool oq_Active)
{
   sintn sn_Counter;

   for (sn_Counter = 0U; sn_Counter < this->mc_Ports.size(); ++sn_Counter)
   {
      this->mc_Ports[sn_Counter]->SetDrawBorder(oq_Active);
   }

   this->mpc_Boundary->SetDrawBorder(oq_Active);
}

//----------------------------------------------------------------------------------------------------------------------
const C_GiNodeBoundary * C_GiNode::m_GetBoundary(void) const
{
   return this->mpc_Boundary;
}

//----------------------------------------------------------------------------------------------------------------------
sint32 C_GiNode::m_GetIconSize(void) const
{
   return this->ms32_IconSize;
}

//----------------------------------------------------------------------------------------------------------------------
uint32 C_GiNode::m_GetScaleCategory(void) const
{
   const QSizeF c_ActSize = this->mpc_Boundary->boundingRect().size();
   uint32 u32_ScaleCategory = mhu32_SCALE_CATEGORY_0;

   // check first scaling
   if ((c_ActSize.width() >= mhaf64_SCALE_MIN_WIDTH_NODE[mhu32_SCALE_CATEGORY_1]) &&
       (c_ActSize.height() >= mhaf64_SCALE_MIN_HEIGHT_NODE[mhu32_SCALE_CATEGORY_1]))
   {
      // check second scaling
      if ((c_ActSize.width() >= mhaf64_SCALE_MIN_WIDTH_NODE[mhu32_SCALE_CATEGORY_2]) &&
          (c_ActSize.height() >= mhaf64_SCALE_MIN_HEIGHT_NODE[mhu32_SCALE_CATEGORY_2]))
      {
         // check third scaling
         if ((c_ActSize.width() >= mhaf64_SCALE_MIN_WIDTH_NODE[mhu32_SCALE_CATEGORY_3]) &&
             (c_ActSize.height() >= mhaf64_SCALE_MIN_HEIGHT_NODE[mhu32_SCALE_CATEGORY_3]))
         {
            // check fourth scaling
            if ((c_ActSize.width() >= mhaf64_SCALE_MIN_WIDTH_NODE[mhu32_SCALE_CATEGORY_4]) &&
                (c_ActSize.height() >= mhaf64_SCALE_MIN_HEIGHT_NODE[mhu32_SCALE_CATEGORY_4]))
            {
               // check fifth scaling
               if ((c_ActSize.width() >= mhaf64_SCALE_MIN_WIDTH_NODE[mhu32_SCALE_CATEGORY_5]) &&
                   (c_ActSize.height() >= mhaf64_SCALE_MIN_HEIGHT_NODE[mhu32_SCALE_CATEGORY_5]))
               {
                  // check sixth scaling
                  if ((c_ActSize.width() >= mhaf64_SCALE_MIN_WIDTH_NODE[mhu32_SCALE_CATEGORY_6]) &&
                      (c_ActSize.height() >= mhaf64_SCALE_MIN_HEIGHT_NODE[mhu32_SCALE_CATEGORY_6]))
                  {
                     // check seventh scaling
                     if ((c_ActSize.width() >= mhaf64_SCALE_MIN_WIDTH_NODE[mhu32_SCALE_CATEGORY_7]) &&
                         (c_ActSize.height() >= mhaf64_SCALE_MIN_HEIGHT_NODE[mhu32_SCALE_CATEGORY_7]))
                     {
                        u32_ScaleCategory = mhu32_SCALE_CATEGORY_7;
                     }
                     else
                     {
                        u32_ScaleCategory = mhu32_SCALE_CATEGORY_6;
                     }
                  }
                  else
                  {
                     u32_ScaleCategory = mhu32_SCALE_CATEGORY_5;
                  }
               }
               else
               {
                  u32_ScaleCategory = mhu32_SCALE_CATEGORY_4;
               }
            }
            else
            {
               u32_ScaleCategory = mhu32_SCALE_CATEGORY_3;
            }
         }
         else
         {
            u32_ScaleCategory = mhu32_SCALE_CATEGORY_2;
         }
      }
      else
      {
         u32_ScaleCategory = mhu32_SCALE_CATEGORY_1;
      }
   }

   return u32_ScaleCategory;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get flag for node data in valid state

   \return
   true  Node valid
   false Node invalid (Probably about to be deleted)
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_GiNode::GetValid(void) const
{
   return mq_Valid;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Sets the white filter for the node drawing

   \param[in]  oq_Active   Flag if filter is active or not
*/
//----------------------------------------------------------------------------------------------------------------------
void C_GiNode::SetDrawWhiteFilter(const bool oq_Active)
{
   sintn sn_Counter;

   this->mq_DrawWhiteFilter = oq_Active;

   for (sn_Counter = 0U; sn_Counter < this->mc_Ports.size(); ++sn_Counter)
   {
      this->mc_Ports[sn_Counter]->SetDrawWhiteFilter(oq_Active);
   }

   this->mpc_Boundary->SetDrawWhiteFilter(oq_Active);

   this->CheckNodeForChanges();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Apply new Z value

   \param[in]  of64_ZValue    New Z value
*/
//----------------------------------------------------------------------------------------------------------------------
void C_GiNode::SetZValueCustom(const float64 of64_ZValue)
{
   C_GiRectBaseGroup::SetZValueCustom(of64_ZValue);
   Q_EMIT this->SigChangedZOrder();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Returns the text

   \return     Text
*/
//----------------------------------------------------------------------------------------------------------------------
QString C_GiNode::GetText(void) const
{
   QString c_Name = "Node";

   const C_OSCNode * const pc_Node = C_PuiSdHandler::h_GetInstance()->GetOSCNodeConst(ms32_Index);

   if (pc_Node != NULL)
   {
      c_Name = C_PuiSdUtil::h_GetNodeBaseNameOrName(this->ms32_Index);
   }

   return c_Name;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Function for initially loading internal data
*/
//----------------------------------------------------------------------------------------------------------------------
void C_GiNode::LoadData(void)
{
   const C_PuiSdNode * const pc_Node = C_PuiSdHandler::h_GetInstance()->GetUINode(ms32_Index);

   if (pc_Node != NULL)
   {
      this->LoadBasicData(*pc_Node);
   }
   //Object name for test
   this->setObjectName(static_cast<QString>("Node: %1").arg(this->GetText()));
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Slot function for updating internal data
*/
//----------------------------------------------------------------------------------------------------------------------
void C_GiNode::UpdateData(void)
{
   C_PuiBsBox c_BoxData;

   stw_opensyde_gui_logic::C_PuiSdNodeConnection c_Conn;
   std::vector<C_PuiSdNodeConnection> c_BusConnections;

   this->UpdateBasicData(c_BoxData);

   //Connections
   for (sint32 s32_ItConn = 0; s32_ItConn < this->mc_Connections.size(); ++s32_ItConn)
   {
      this->mc_Connections[s32_ItConn]->UpdateData(&c_Conn);
      c_BusConnections.push_back(c_Conn);
   }
   C_PuiSdHandler::h_GetInstance()->SetUINodeBox(ms32_Index, c_BoxData);
   C_PuiSdHandler::h_GetInstance()->SetUINodeConnections(ms32_Index, c_BusConnections);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Remove pointer to connection and all dependencies

   \param[in]  opc_BusConnectorGraphicsItem  Referenced connection
*/
//----------------------------------------------------------------------------------------------------------------------
void C_GiNode::RemoveConnector(const C_GiLiBusConnector * const opc_BusConnectorGraphicsItem)
{
   sint32 s32_ItConn;

   const C_PuiSdNodeConnectionId * const pc_ConnId = this->GetNodeConnectionId(opc_BusConnectorGraphicsItem);

   if (pc_ConnId != NULL)
   {
      C_PuiSdHandler::h_GetInstance()->RemoveConnection(static_cast<uint32>(this->ms32_Index), *pc_ConnId);
   }

   for (s32_ItConn = 0; s32_ItConn < this->mc_Connections.size(); ++s32_ItConn)
   {
      if (opc_BusConnectorGraphicsItem == this->mc_Connections[s32_ItConn])
      {
         this->mc_Connections.remove(s32_ItConn);
         break;
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Check index relevant for this node

   \param[in]  ou32_NodeIndex    Node index

   \return
   Is index relevant for this node
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_GiNode::CheckIndexRelevantForThisNode(const uint32 ou32_NodeIndex) const
{
   bool q_Retval = false;

   if (this->ms32_Index >= 0)
   {
      q_Retval =
         C_PuiSdHandler::h_GetInstance()->CheckNodeIndexAssociatedWithAnotherNodeIndex(
            static_cast<uint32>(this->ms32_Index), ou32_NodeIndex);
   }
   return q_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Delete data in system definition
*/
//----------------------------------------------------------------------------------------------------------------------
void C_GiNode::DeleteData(void)
{
   C_PuiSdHandler::h_GetInstance()->RemoveNode(ms32_Index);
   this->mq_Valid = false;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Remember associated connection

   \param[in,out]  opc_Connection   New connection
*/
//----------------------------------------------------------------------------------------------------------------------
void C_GiNode::AddConnection(C_GiLiBusConnector * const opc_Connection)
{
   this->mc_Connections.push_back(opc_Connection);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Add new connection with data

   \param[in,out]  opc_Connection      New connection
   \param[in,out]  orc_NodeConnection  New data
   \param[in]      orc_Properties      Properties
   \param[in,out]  oru32_BusIndex      New bus index to connect to
*/
//----------------------------------------------------------------------------------------------------------------------
void C_GiNode::AddConnectionAndData(C_GiLiBusConnector * const opc_Connection,
                                    const C_PuiSdNodeConnectionId & orc_NodeConnection,
                                    const std::vector<C_PuiSdNodeInterfaceAutomaticProperties> & orc_Properties,
                                    const uint32 & oru32_BusIndex)
{
   if (opc_Connection != NULL)
   {
      this->AddConnection(opc_Connection);
      C_PuiSdHandler::h_GetInstance()->AddConnection(
         static_cast<uint32>(this->GetIndex()), orc_NodeConnection.u8_InterfaceNumber, orc_Properties,
         oru32_BusIndex);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Update internal connection data identified by specified C_GiLiBusConnector instance

   \param[in]  opc_Connection       Old connection for reference only
   \param[in]  orc_NodeConnection   New connection data
   \param[in]  orc_Properties       Properties
   \param[in]  oru32_BusIndex       Bus index to use instead of last used one
*/
//----------------------------------------------------------------------------------------------------------------------
void C_GiNode::UpdateConnection(const C_GiLiBusConnector * const opc_Connection,
                                const C_PuiSdNodeConnectionId & orc_NodeConnection,
                                const std::vector<C_PuiSdNodeInterfaceAutomaticProperties> & orc_Properties,
                                const uint32 & oru32_BusIndex) const
{
   const C_PuiSdNodeConnectionId * const pc_PrevConn = this->GetNodeConnectionId(opc_Connection);

   if (pc_PrevConn != NULL)
   {
      C_PuiSdHandler::h_GetInstance()->ChangeCompleteConnection(static_cast<uint32>(this->ms32_Index), *pc_PrevConn,
                                                                orc_NodeConnection, orc_Properties,
                                                                oru32_BusIndex);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Reconnect bus connection to new interface

   \param[in]  ou8_Interface     New interface number
   \param[in]  opc_Connection    Associated connection
   \param[in]  orc_Properties    Properties
*/
//----------------------------------------------------------------------------------------------------------------------
void C_GiNode::ChangeInterface(const uint8 ou8_Interface, const C_GiLiBusConnector * const opc_Connection,
                               const std::vector<C_PuiSdNodeInterfaceAutomaticProperties> & orc_Properties) const
{
   if (opc_Connection != NULL)
   {
      const C_GiLiBus * const pc_Bus = opc_Connection->GetBusItem();
      if (pc_Bus != NULL)
      {
         const C_PuiSdNodeConnectionId * const pc_ConnectionId = this->GetNodeConnectionId(opc_Connection);
         if (pc_ConnectionId != NULL)
         {
            C_PuiSdHandler::h_GetInstance()->ChangeConnection(static_cast<uint32>(this->ms32_Index), *pc_ConnectionId,
                                                              ou8_Interface, orc_Properties);
         }
         pc_Bus->GetType();
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Search for id of connection

   \param[in,out]  opc_Connection   Connection to search for

   \return
   NULL Error
   else Pointer to associated node connection id
*/
//----------------------------------------------------------------------------------------------------------------------
const C_PuiSdNodeConnectionId * C_GiNode::GetNodeConnectionId(const C_GiLiBusConnector * const opc_Connection) const
{
   const C_PuiSdNodeConnectionId * pc_Retval = NULL;

   if (opc_Connection != NULL)
   {
      const C_PuiSdNode * const pc_Node =
         C_PuiSdHandler::h_GetInstance()->GetUINode(static_cast<uint32>(this->ms32_Index));
      if (pc_Node != NULL)
      {
         const sint32 s32_Index = GetIndexOfConnector(opc_Connection);
         if (s32_Index >= 0)
         {
            const uint32 u32_Index = static_cast<uint32>(s32_Index);
            if (u32_Index < pc_Node->c_UIBusConnections.size())
            {
               pc_Retval = &(pc_Node->c_UIBusConnections[u32_Index].c_ConnectionID);
            }
         }
      }
   }
   return pc_Retval;
}
