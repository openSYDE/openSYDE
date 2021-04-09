//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Connection of any item to bus (implementation)

   Connection of any item to bus

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.h"

#include <QGraphicsScene>
#include "gitypes.h"
#include "C_SebUtil.h"
#include "C_OSCUtils.h"
#include "C_GiLiBusConnectorBase.h"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw_types;
using namespace stw_opensyde_gui;
using namespace stw_opensyde_core;
using namespace stw_opensyde_gui_logic;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

/* -- Global Variables ---------------------------------------------------------------------------------------------- */

/* -- Module Global Variables --------------------------------------------------------------------------------------- */

/* -- Module Global Function Prototypes ----------------------------------------------------------------------------- */

/* -- Implementation ------------------------------------------------------------------------------------------------ */

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Default constructor

   Set up GUI with all elements.

   \param[in]     oru64_ID       Unique ID
   \param[in]     orc_TriggerPos Ideal position of connector
   \param[in]     opc_BusItem    End of connection at bus
   \param[in]     oq_MiddleLine  Indicator if middle line is required
   \param[in,out] opc_Parent     Optional pointer to parent
*/
//----------------------------------------------------------------------------------------------------------------------
C_GiLiBusConnectorBase::C_GiLiBusConnectorBase(const uint64 & oru64_ID, const QPointF & orc_TriggerPos,
                                               const C_GiLiBus * const opc_BusItem, const bool oq_MiddleLine,
                                               QGraphicsItem * const opc_Parent) :
   C_GiLiLineGroup(NULL, oq_MiddleLine, opc_Parent),
   C_GiUnique(oru64_ID),
   ms32_ConnIndex(-1),
   ms32_KnownLineCount(-1),
   mf64_ConnProgress(0.0),
   mpc_LastKnownBusItem(NULL),
   mpc_GenericSignalItem(NULL),
   mpc_LastKnownGenericSignalItem(NULL),
   mpc_GenericPositionItem(NULL),
   mpc_LastKnownGenericPositionItem(NULL),
   mc_KnownLocalPosGeneric(),
   mq_InAir(false),
   mpc_BusItem(NULL)
{
   std::vector<QPointF> c_Points;
   c_Points.push_back(orc_TriggerPos);
   c_Points.push_back(orc_TriggerPos);
   this->m_Init(c_Points);

   //Bus
   m_UpdateBus(orc_TriggerPos, opc_BusItem);

   m_CommonInit();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Default constructor

   Set up GUI with all elements.

   \param[in]     oru64_ID              Unique ID
   \param[in]     orc_InteractionPoints Ideal interaction point positions
   \param[in]     opc_BusItem           End of connection at bus
   \param[in]     oq_MiddleLine         Indicator if middle line is required
   \param[in,out] opc_Parent            Optional pointer to parent
*/
//----------------------------------------------------------------------------------------------------------------------
C_GiLiBusConnectorBase::C_GiLiBusConnectorBase(const uint64 & oru64_ID,
                                               const std::vector<QPointF> & orc_InteractionPoints,
                                               const C_GiLiBus * const opc_BusItem, const bool oq_MiddleLine,
                                               QGraphicsItem * const opc_Parent) :
   C_GiLiLineGroup(NULL, oq_MiddleLine, opc_Parent),
   C_GiUnique(oru64_ID),
   ms32_ConnIndex(-1),
   ms32_KnownLineCount(-1),
   mf64_ConnProgress(0.0),
   mpc_LastKnownBusItem(NULL),
   mpc_GenericSignalItem(NULL),
   mpc_LastKnownGenericSignalItem(NULL),
   mpc_GenericPositionItem(NULL),
   mpc_LastKnownGenericPositionItem(NULL),
   mc_KnownLocalPosGeneric(),
   mq_InAir(false),
   mpc_BusItem(NULL)
{
   std::vector<QPointF> c_SaveVec = orc_InteractionPoints;
   // Minimum two points are necessary. If vector has not enough points, add two points.
   if (c_SaveVec.size() < 2)
   {
      c_SaveVec.push_back(QPointF(0.0, 0.0));
      c_SaveVec.push_back(QPointF(0.0, 0.0));
   }
   this->m_Init(c_SaveVec);

   //Bus
   m_UpdateBus(c_SaveVec[c_SaveVec.size() - 1], opc_BusItem);

   m_CommonInit();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Default destructor

   Clean up.
*/
//----------------------------------------------------------------------------------------------------------------------
C_GiLiBusConnectorBase::~C_GiLiBusConnectorBase(void)
{
   this->mpc_LastKnownBusItem = NULL;
   this->mpc_LastKnownGenericPositionItem = NULL;
   this->mpc_LastKnownGenericSignalItem = NULL;
   this->mpc_GenericSignalItem = NULL;
   this->mpc_GenericPositionItem = NULL;
   this->mpc_BusItem = NULL;
   //lint -e{1740}  no memory leak because the ownership of these objects was never transfered to this class
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Returns the position of the point which is connected to the bus

   \return  Position of item
*/
//----------------------------------------------------------------------------------------------------------------------
QPointF C_GiLiBusConnectorBase::GetPos(void) const
{
   QPointF c_Pos;
   const QVector<C_GiLiLineConnection *> & rc_Lines = this->mpc_LinePath->GetLines();

   if (rc_Lines.size() > 0)
   {
      c_Pos = rc_Lines[0]->GetP2();
   }
   else
   {
      c_Pos.setX(0.0);
      c_Pos.setY(0.0);
   }

   return c_Pos;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get all points

   \return
   All points
*/
//----------------------------------------------------------------------------------------------------------------------
std::vector<QPointF> C_GiLiBusConnectorBase::GetPointsScenePos(void) const
{
   std::vector<QPointF> c_Retval;
   const QVector<C_GiLiLineConnection *> c_Lines = this->GetLines();
   const C_GiLiLineConnection * pc_CurConn = NULL;
   for (QVector<C_GiLiLineConnection *>::const_iterator pc_ItLine = c_Lines.begin(); pc_ItLine != c_Lines.end();
        ++pc_ItLine)
   {
      pc_CurConn = *pc_ItLine;
      c_Retval.push_back(pc_CurConn->mapToScene(pc_CurConn->line().p1()));
   }
   if (pc_CurConn != NULL)
   {
      c_Retval.push_back(pc_CurConn->mapToScene(pc_CurConn->line().p2()));
   }
   return c_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Set new point positions

   \param[in] orc_ScenePos New point position set
*/
//----------------------------------------------------------------------------------------------------------------------
void C_GiLiBusConnectorBase::SetPoints(const std::vector<QPointF> & orc_ScenePos)
{
   sint32 s32_Index = 0;

   this->mpc_LastKnownGenericPositionItem = NULL;
   for (std::vector<QPointF>::const_iterator c_ItScenePos = orc_ScenePos.begin(); c_ItScenePos != orc_ScenePos.end();
        ++c_ItScenePos)
   {
      UpdatePoint(s32_Index, *c_ItScenePos);
      if (s32_Index == this->m_GetBusInteractionIndex())
      {
         this->m_UpdateConnection(*c_ItScenePos);
      }
      ++s32_Index;
   }
   this->m_UpdateExternal();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Returns the bus item

   \return  Bus item
*/
//----------------------------------------------------------------------------------------------------------------------
const C_GiLiBus * C_GiLiBusConnectorBase::GetBusItem(void) const
{
   return this->mpc_BusItem;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Returns the generic position item

   \return  Generic position item
*/
//----------------------------------------------------------------------------------------------------------------------
const C_GiBiConnectableItem * C_GiLiBusConnectorBase::GetGenericPositionItem(void) const
{
   return this->mpc_GenericPositionItem;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Change z order for bus connector
*/
//----------------------------------------------------------------------------------------------------------------------
void C_GiLiBusConnectorBase::RestoreZOrder(void)
{
   const C_GiLiBus * const pc_Bus = this->GetBusItem();
   const QGraphicsItem * const pc_Item = dynamic_cast<const QGraphicsItem *>(this->mpc_GenericSignalItem);

   if ((pc_Bus != NULL) && (pc_Item != NULL))
   {
      this->SetZValueCustom(std::min(pc_Bus->zValue(), pc_Item->zValue()) - 1.0);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get scene position of specific interaction point

   \param[out] orc_ScenePos Scene position of specific interaction point
*/
//----------------------------------------------------------------------------------------------------------------------
void C_GiLiBusConnectorBase::GetNodeInteractionScenePos(QPointF & orc_ScenePos) const
{
   const sint32 s32_Index = mh_GetGenericInteractionIndex();

   orc_ScenePos = this->GetPointScenePos(s32_Index);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get scene position of specific interaction point

   \param[out] orc_ScenePos Scene position of specific interaction point
*/
//----------------------------------------------------------------------------------------------------------------------
void C_GiLiBusConnectorBase::GetBusInteractionScenePos(QPointF & orc_ScenePos) const
{
   const sint32 s32_Index = m_GetBusInteractionIndex();

   orc_ScenePos = this->GetPointScenePos(s32_Index);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Revert bus connection to last known bus item

   \param[in,out] opc_StartingBus Last known bus this bus connection was connected to
   \param[in,out] opc_LastBus     New bus this bus connection is currently connected to
   \param[in]     orc_ScenePos    Last known scene position of interaction point
*/
//----------------------------------------------------------------------------------------------------------------------
void C_GiLiBusConnectorBase::RevertBus(const stw_opensyde_gui::C_GiLiBus * const opc_StartingBus,
                                       const stw_opensyde_gui::C_GiLiBus * const opc_LastBus,
                                       const QPointF & orc_ScenePos)
{
   Q_UNUSED(opc_LastBus)
   this->m_SetBus(opc_StartingBus);
   m_UpdateConnection(orc_ScenePos);
   m_Reconnect();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Apply new Z value

   \param[in] of64_ZValue New Z value
*/
//----------------------------------------------------------------------------------------------------------------------
void C_GiLiBusConnectorBase::SetZValueCustom(const float64 of64_ZValue)
{
   C_GiLiLineGroup::SetZValueCustom(of64_ZValue);
   //For bus connectors no data update should happen for the z value
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Overwritten mouse press event slot

   \param[in,out] opc_Event Event identification and information
*/
//----------------------------------------------------------------------------------------------------------------------
void C_GiLiBusConnectorBase::mousePressEvent(QGraphicsSceneMouseEvent * const opc_Event)
{
   C_GiLiLineGroup::mousePressEvent(opc_Event);
   if (this->me_ActiveResizeMode == C_GiLiLineGroup::ePOINT)
   {
      if (this->GetBusItem() != NULL)
      {
         const C_OSCSystemBus::E_Type e_Type = this->GetBusItem()->GetType();
         if (this->msn_ActiveItemIndex == mh_GetGenericInteractionIndex())
         {
            Q_EMIT this->SigStartConnect(E_ConnectState::eTO_GENERIC,
                                         dynamic_cast<QGraphicsItem *>(this->mpc_GenericSignalItem),
                                         &e_Type, this);
            this->mq_InAir = true;
         }
         else if (this->msn_ActiveItemIndex == m_GetBusInteractionIndex())
         {
            Q_EMIT this->SigStartConnect(E_ConnectState::eTO_BUS,
                                         dynamic_cast<QGraphicsItem *>(this->mpc_GenericSignalItem),
                                         &e_Type, this);
            this->mq_InAir = true;
         }
         else
         {
            //Nothing to do
         }
         if (this->mq_InAir == true)
         {
            this->mpc_LastKnownBusItem = this->GetBusItem();
            this->mpc_LastKnownGenericSignalItem = this->mpc_GenericSignalItem;
            this->mpc_LastKnownGenericPositionItem = this->mpc_GenericPositionItem;
         }
      }
   }
   //Notify undo redo of last valid state
   Q_EMIT this->SigPrepareMove(this);

   //Hide tool tip
   Q_EMIT this->SigHideToolTip();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Overwritten hover leave event slot

   Here: Hide tool tip

   \param[in,out] opc_Event Event identification and information
*/
//----------------------------------------------------------------------------------------------------------------------
void C_GiLiBusConnectorBase::hoverLeaveEvent(QGraphicsSceneHoverEvent * const opc_Event)
{
   Q_EMIT this->SigHideToolTip();

   C_GiLiLineGroup::hoverLeaveEvent(opc_Event);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Overwritten mouse release event slot

   \param[in,out] opc_Event Event identification and information
*/
//----------------------------------------------------------------------------------------------------------------------
void C_GiLiBusConnectorBase::mouseReleaseEvent(QGraphicsSceneMouseEvent * const opc_Event)
{
   if (this->me_ActiveResizeMode == C_GiLiBusConnectorBase::ePOINT)
   {
      if ((this->msn_ActiveItemIndex == mh_GetGenericInteractionIndex()) ||
          (this->msn_ActiveItemIndex == m_GetBusInteractionIndex()))
      {
         const QPointF c_ScenePos = opc_Event->scenePos();
         bool q_Revert = true;
         if (this->msn_ActiveItemIndex == mh_GetGenericInteractionIndex())
         {
            q_Revert = m_OnGenericInteractionPointMouseRelease(c_ScenePos);
         }
         if (this->msn_ActiveItemIndex == m_GetBusInteractionIndex())
         {
            if (this->GetBusItem() != NULL)
            {
               if (this->GetBusItem() != this->mpc_LastKnownBusItem)
               {
                  if (this->GetBusItem()->contains(this->GetBusItem()->mapFromScene(c_ScenePos)) == true)
                  {
                     m_OnBusChange(c_ScenePos);
                     q_Revert = false;
                  }
               }
            }
         }
         if (q_Revert == true)
         {
            this->m_Reconnect();
         }
         Q_EMIT this->SigStopConnect();
      }
   }
   C_GiLiLineGroup::mouseReleaseEvent(opc_Event);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Set new bus

   \param[in] opc_Bus New bus
*/
//----------------------------------------------------------------------------------------------------------------------
void C_GiLiBusConnectorBase::m_SetBus(const C_GiLiBus * const opc_Bus)
{
   this->mpc_BusItem = opc_Bus;
   this->m_UpdateLastKnownBusPoints();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Handle new bus

   Save new bus and update connection at position.
   Dismiss temporary bus.

   \param[in] orc_Pos     Position to evaluate for connection
   \param[in] opc_BusItem New bus
*/
//----------------------------------------------------------------------------------------------------------------------
void C_GiLiBusConnectorBase::m_UpdateBus(const QPointF & orc_Pos, const C_GiLiBus * const opc_BusItem)
{
   if (this->GetBusItem() != NULL)
   {
      disconnect(this->GetBusItem(), &C_GiLiBus::SigChangedGraphic,
                 this, &C_GiLiBusConnectorBase::m_UpdateExternal);
      disconnect(this->GetBusItem(), &C_GiLiBus::SigChangedZOrder,
                 this, &C_GiLiBusConnectorBase::RestoreZOrder);
   }
   this->m_SetBus(opc_BusItem);
   if (this->GetBusItem() != NULL)
   {
      QVector<C_GiLiLineConnection *> c_Lines;
      m_AdaptStyleToBus(this->GetBusItem());

      c_Lines = this->GetBusItem()->GetLines();
      ms32_KnownLineCount = c_Lines.size();
      connect(this->GetBusItem(), &C_GiLiBus::SigChangedGraphic,
              this, &C_GiLiBusConnectorBase::m_UpdateExternal);
      connect(this->GetBusItem(), &C_GiLiBus::SigChangedZOrder,
              this, &C_GiLiBusConnectorBase::RestoreZOrder);
      m_UpdateConnection(orc_Pos);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Update connection

   Find and save new connection at position.

   \param[in] orc_ScenePos Scene position
*/
//----------------------------------------------------------------------------------------------------------------------
void C_GiLiBusConnectorBase::m_UpdateConnection(const QPointF & orc_ScenePos)
{
   if (this->GetBusItem() != NULL)
   {
      C_GiLiLineConnection * pc_Conn;
      this->GetBusItem()->FindClosestConnection(orc_ScenePos, this->ms32_ConnIndex);
      pc_Conn = m_GetCurrentConn();
      if (pc_Conn != NULL)
      {
         m_CalcConnProgress(pc_Conn, orc_ScenePos);
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get current connection

   \return
   NULL: Invalid
   else: Current connection
*/
//----------------------------------------------------------------------------------------------------------------------
C_GiLiLineConnection * C_GiLiBusConnectorBase::m_GetCurrentConn(void) const
{
   C_GiLiLineConnection * pc_Retval = NULL;

   if (this->GetBusItem() != NULL)
   {
      QVector<C_GiLiLineConnection *> c_Lines = this->GetBusItem()->GetLines();
      if ((this->ms32_ConnIndex >= 0) && (this->ms32_ConnIndex < c_Lines.size()))
      {
         pc_Retval = c_Lines[this->ms32_ConnIndex];
      }
   }
   return pc_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Calculate the relative position for the connection

   \param[in] opc_Conn     Connection to calculate the relative position for
   \param[in] orc_ScenePos Position to use for relative calculation
*/
//----------------------------------------------------------------------------------------------------------------------
void C_GiLiBusConnectorBase::m_CalcConnProgress(const C_GiLiLineConnection * const opc_Conn,
                                                const QPointF & orc_ScenePos)
{
   opc_Conn->FindClosestPointRelative(orc_ScenePos, this->mf64_ConnProgress);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Handle update of connected item
*/
//----------------------------------------------------------------------------------------------------------------------
void C_GiLiBusConnectorBase::m_UpdateExternal(void)
{
   if (this->GetBusItem() != NULL)
   {
      const QGraphicsItem * const pc_Item = dynamic_cast<QGraphicsItem *>(this->mpc_GenericSignalItem);
      //Bus

      // update the style
      m_AdaptStyleToBus(this->GetBusItem());

      {
         //Replace line count
         const QVector<C_GiLiLineConnection *> c_Lines = this->GetBusItem()->GetLines();
         const sintn sn_LineCount = c_Lines.size();
         if (sn_LineCount < ms32_KnownLineCount)
         {
            //Deleted line
            m_UpdateConnection(this->mc_Points[m_GetBusInteractionIndex()]->scenePos());
         }
         else if (sn_LineCount > ms32_KnownLineCount)
         {
            //New line
            m_UpdateConnection(this->mc_Points[m_GetBusInteractionIndex()]->scenePos());
         }
         else
         {
            //Normal
         }
         ms32_KnownLineCount = sn_LineCount;
      }

      if (pc_Item != NULL)
      {
         if (this->mc_Points.size() >= 2)
         {
            //Check if both connected items are selected
            if ((pc_Item->isSelected() == true) && (this->GetBusItem()->isSelected() == true))
            {
               //Move all
               const QPointF c_Old = this->mc_Points[m_GetBusInteractionIndex()]->scenePos();
               QPointF c_New;
               QPointF c_Diff;

               m_UpdatePointBus();
               c_New = this->mc_Points[m_GetBusInteractionIndex()]->scenePos();
               c_Diff = c_New - c_Old;
               for (sint32 s32_ItPoint = 0; s32_ItPoint < this->mc_Points.size(); ++s32_ItPoint)
               {
                  if (s32_ItPoint != m_GetBusInteractionIndex())
                  {
                     UpdatePoint(s32_ItPoint, this->mc_Points[s32_ItPoint]->scenePos() + c_Diff);
                  }
               }
            }
            else
            {
               if (m_CheckAnyRelevantBusPointChanged())
               {
                  m_UpdatePointBus();
               }
            }
            this->m_UpdateLastKnownBusPoints();
            //Generic point
            m_UpdateGenericPoint();
         }
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Update of line and its position
*/
//----------------------------------------------------------------------------------------------------------------------
void C_GiLiBusConnectorBase::m_UpdateInternal(void)
{
   if ((this->me_ActiveResizeMode == C_GiLiBusConnectorBase::eNO_ELEMENT) ||
       (this->me_ActiveResizeMode == C_GiLiBusConnectorBase::eALL))
   {
      const QGraphicsItem * const pc_Item = dynamic_cast<const QGraphicsItem *>(this->mpc_GenericSignalItem);
      disconnect(this, &C_GiLiBusConnectorBase::SigChangedGraphic, this, &C_GiLiBusConnectorBase::m_UpdateInternal);
      //Moved by other element
      //Check connections

      if (pc_Item != NULL)
      {
         if (pc_Item->isSelected() == false)
         {
            //Port
            this->m_CalcInitialLocalPos(this->mc_Points[mh_GetGenericInteractionIndex()]->scenePos());
            m_UpdateGenericPoint();
         }
      }

      if (this->GetBusItem() != NULL)
      {
         if (this->GetBusItem()->isSelected() == false)
         {
            //Connection
            m_UpdateConnection(this->mc_Points[m_GetBusInteractionIndex()]->scenePos());
            m_UpdatePointBus();
         }
      }
      connect(this, &C_GiLiBusConnectorBase::SigChangedGraphic, this, &C_GiLiBusConnectorBase::m_UpdateInternal);
   }
   else if (this->me_ActiveResizeMode == C_GiLiBusConnectorBase::eLINE)
   {
      //Check if active line is at border
      if ((this->msn_ActiveItemIndex == mh_GetGenericInteractionIndex()) ||
          (this->msn_ActiveItemIndex == static_cast<sintn>(m_GetBusInteractionIndex() - 1)))
      {
         if (((this->mpc_GenericPositionItem != NULL) && (this->GetBusItem() != NULL)) &&
             (this->mpc_GenericSignalItem != NULL))
         {
            disconnect(this, &C_GiLiBusConnectorBase::SigChangedGraphic,
                       this, &C_GiLiBusConnectorBase::m_UpdateInternal);
            if (this->mc_Points.size() >= 2)
            {
               QPointF c_PNew;
               QPointF c_POld;
               const QGraphicsItem * const pc_Item = dynamic_cast<const QGraphicsItem *>(this->mpc_GenericSignalItem);
               //find closest point in shape and set pLast to closest point in shape
               if (this->GetBusItem()->isSelected() == false)
               {
                  c_POld = this->mc_Points[m_GetBusInteractionIndex()]->scenePos();
                  this->GetBusItem()->FindClosestPoint(c_POld, c_PNew);
                  this->UpdatePoint(m_GetBusInteractionIndex(), c_PNew);
                  m_UpdateConnection(c_PNew);
               }

               //find closest point in shape and set pFirst to closest point in shape
               if (pc_Item != NULL)
               {
                  if (pc_Item->isSelected() == false)
                  {
                     c_POld = this->mc_Points[mh_GetGenericInteractionIndex()]->scenePos();
                     this->mpc_GenericPositionItem->FindClosestPoint(c_POld, c_PNew);
                     this->UpdatePoint(mh_GetGenericInteractionIndex(), c_PNew);
                     this->m_CalcInitialLocalPos(this->mc_Points[mh_GetGenericInteractionIndex()]->scenePos());
                  }
               }
            }
            connect(this, &C_GiLiBusConnectorBase::SigChangedGraphic,
                    this, &C_GiLiBusConnectorBase::m_UpdateInternal);
         }
      }
   }
   else if (this->me_ActiveResizeMode == C_GiLiBusConnectorBase::ePOINT)
   {
      m_OnInteractionPointMove();
   }
   else
   {
      // Undefined resize mode. Nothing to do.
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get bus interaction index

   \return
   Bus interaction index
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_GiLiBusConnectorBase::m_GetBusInteractionIndex(void) const
{
   return (static_cast<sint32>(this->mc_Points.size()) - 1L);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Update bus point position
*/
//----------------------------------------------------------------------------------------------------------------------
void C_GiLiBusConnectorBase::m_UpdatePointBus(void)
{
   const C_GiLiLineConnection * const pc_Conn = m_GetCurrentConn();

   if (pc_Conn != NULL)
   {
      QPointF c_PNew;
      if (m_CheckOnlyOneBusPointMoved())
      {
         C_GiLiInteractionPoint * const pc_Point = this->mc_Points[this->m_GetBusInteractionIndex()];
         //Allow point to move along bus
         this->m_CalcConnProgress(pc_Conn, pc_Point->scenePos());
      }
      //find closest point in shape and set pLast to closest point in shape
      this->m_CalcConnPos(pc_Conn, c_PNew);
      this->UpdatePoint(m_GetBusInteractionIndex(), c_PNew);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get the position for the selected connection

   \param[in]  opc_Conn   Connection to calculate the position for
   \param[out] orc_OutPos Calculated scene position on connection
*/
//----------------------------------------------------------------------------------------------------------------------
void C_GiLiBusConnectorBase::m_CalcConnPos(const C_GiLiLineConnection * const opc_Conn, QPointF & orc_OutPos) const
{
   const QPointF c_P1 = opc_Conn->mapToScene(opc_Conn->line().p1());
   const QPointF c_P2 = opc_Conn->mapToScene(opc_Conn->line().p2());

   orc_OutPos = c_P1 + (this->mf64_ConnProgress * (c_P2 - c_P1));
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Update generic point position
*/
//----------------------------------------------------------------------------------------------------------------------
void C_GiLiBusConnectorBase::m_UpdateGenericPoint(void)
{
   QGraphicsItem * const pc_Item = dynamic_cast<QGraphicsItem *>(this->mpc_GenericPositionItem);

   if ((pc_Item != NULL) && (this->mpc_GenericPositionItem != NULL))
   {
      QPointF c_Pos;

      //Reconnect
      this->mpc_GenericPositionItem->FindClosestPoint(pc_Item->mapToScene(this->mc_KnownLocalPosGeneric), c_Pos);
      this->UpdatePoint(mh_GetGenericInteractionIndex(), c_Pos);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get generic interaction index

   \return
   Generic interaction index
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_GiLiBusConnectorBase::mh_GetGenericInteractionIndex(void)
{
   return 0;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Save local position of orc_InPos mapped to current port

   \param[in] orc_InPos Position to map
*/
//----------------------------------------------------------------------------------------------------------------------
void C_GiLiBusConnectorBase::m_CalcInitialLocalPos(const QPointF & orc_InPos)
{
   QGraphicsItem * const pc_Item = dynamic_cast<QGraphicsItem *>(this->mpc_GenericPositionItem);

   if ((pc_Item != NULL) && (this->mpc_GenericPositionItem != NULL))
   {
      this->mpc_GenericPositionItem->FindClosestPoint(orc_InPos, this->mc_KnownLocalPosGeneric);
      this->mc_KnownLocalPosGeneric = pc_Item->mapFromScene(this->mc_KnownLocalPosGeneric);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Handle generic item

   Save generic item.
   Dismiss temporary generic item.

   \param[in] opc_NewItem New generic item
*/
//----------------------------------------------------------------------------------------------------------------------
void C_GiLiBusConnectorBase::m_UpdateGenericItem(C_GiBiConnectableItem * const opc_NewItem)
{
   if (this->mpc_GenericSignalItem != NULL)
   {
      disconnect(this->mpc_GenericSignalItem, &C_GiBiConnectableItem::SigChangedGraphic,
                 this, &C_GiLiBusConnectorBase::m_UpdateExternal);
      disconnect(this->mpc_GenericSignalItem, &C_GiBiConnectableItem::SigChangedZOrder,
                 this, &C_GiLiBusConnectorBase::RestoreZOrder);
   }
   this->mpc_GenericSignalItem = opc_NewItem;
   if (this->mpc_GenericSignalItem != NULL)
   {
      connect(this->mpc_GenericSignalItem, &C_GiBiConnectableItem::SigChangedGraphic,
              this, &C_GiLiBusConnectorBase::m_UpdateExternal);
      connect(this->mpc_GenericSignalItem, &C_GiBiConnectableItem::SigChangedZOrder,
              this, &C_GiLiBusConnectorBase::RestoreZOrder);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Restore bus connection state to normal and reconnect all in air interaction points
*/
//----------------------------------------------------------------------------------------------------------------------
void C_GiLiBusConnectorBase::m_Reconnect(void)
{
   this->mq_InAir = false;
   this->me_ActiveResizeMode = C_GiLiLineGroup::eNO_ELEMENT;
   m_UpdatePointBus();
   m_UpdateGenericPoint();
   this->m_UpdateInternal();
   this->m_UpdateExternal();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Handle interaction point move
*/
//----------------------------------------------------------------------------------------------------------------------
void C_GiLiBusConnectorBase::m_OnInteractionPointMove(void)
{
   if (mq_InAir == true)
   {
      //Check if active line is at border
      if ((this->msn_ActiveItemIndex == mh_GetGenericInteractionIndex()) ||
          (this->msn_ActiveItemIndex == m_GetBusInteractionIndex()))
      {
         if (this->scene() != NULL)
         {
            bool q_FoundBus = false;
            bool q_RestoreMouseCursor = true;
            const QPointF c_CurPos = this->mc_Points[this->msn_ActiveItemIndex]->scenePos();
            const QList<QGraphicsItem *> c_All = this->scene()->items();
            for (QList<QGraphicsItem *>::const_iterator c_ItItem = c_All.begin(); c_ItItem != c_All.end(); ++c_ItItem)
            {
               QGraphicsItem * const pc_Parent = C_SebUtil::h_GetHighestParent(c_ItItem.operator *());
               C_GiLiBus * const pc_Bus = dynamic_cast<C_GiLiBus *>(pc_Parent);
               //try node
               if (this->msn_ActiveItemIndex == mh_GetGenericInteractionIndex())
               {
                  m_OnIterationGenericInteractionPointMove(pc_Parent, c_CurPos, q_RestoreMouseCursor);
                  if (pc_Bus != NULL)
                  {
                     if (pc_Bus->isUnderMouse() == true)
                     {
                        this->mc_Points[this->msn_ActiveItemIndex]->setCursor(pc_Bus->cursor());
                        q_RestoreMouseCursor = false;
                        //Stop after connectable item found under cursor
                        break;
                     }
                  }
               }
               //try bus
               if (this->msn_ActiveItemIndex == m_GetBusInteractionIndex())
               {
                  if (pc_Bus != NULL)
                  {
                     if (pc_Bus->isUnderMouse() == true)
                     {
                        this->mc_Points[this->msn_ActiveItemIndex]->setCursor(pc_Bus->cursor());
                        q_RestoreMouseCursor = false;
                        if (pc_Bus->cursor().shape() == Qt::CrossCursor)
                        {
                           m_UpdateBus(c_CurPos, pc_Bus);
                           q_FoundBus = true;
                        }
                        else
                        {
                           //Change cursor
                           Q_EMIT this->SigShowToolTip(c_CurPos, pc_Bus);
                        }
                     }
                  }
                  m_OnIterationBusInteractionPointMoveCleanUp(pc_Parent, q_RestoreMouseCursor);
                  //Stop after connectable item found under cursor
                  if (q_RestoreMouseCursor == false)
                  {
                     //lint -e{9011} Loop has to stop here
                     break;
                  }
               }
            }
            if (this->msn_ActiveItemIndex == m_GetBusInteractionIndex())
            {
               if (q_FoundBus == false)
               {
                  m_UpdateBus(this->mc_Points[m_GetBusInteractionIndex()]->scenePos(), this->mpc_LastKnownBusItem);
               }
            }
            if (q_RestoreMouseCursor == true)
            {
               //Restore default cursor (Not done at start as this will trigger the cursor to start flickering)
               this->mc_Points[this->msn_ActiveItemIndex]->RestoreDefaultCursor();
            }
         }
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Handle bus item interaction point move item iteration

   Active:
   * Iteration over current scene items
   * Bus interaction point moving
   * After any other handling is done

   \param[in,out] opc_HighestParentItem  Highest parent of current item
   \param[in,out] orq_RestoreMouseCursor Restore mouse cursor flag (Only write false allowed if necessary)
*/
//----------------------------------------------------------------------------------------------------------------------
void C_GiLiBusConnectorBase::m_OnIterationBusInteractionPointMoveCleanUp(QGraphicsItem * const opc_HighestParentItem,
                                                                         bool & orq_RestoreMouseCursor)
{
   C_GiBiCustomMouseItem * const pc_Mouse = dynamic_cast<C_GiBiCustomMouseItem *>(opc_HighestParentItem);

   if ((pc_Mouse != NULL) && (pc_Mouse != this))
   {
      if (opc_HighestParentItem->isUnderMouse() == true)
      {
         this->mc_Points[this->msn_ActiveItemIndex]->setCursor(opc_HighestParentItem->cursor());
         orq_RestoreMouseCursor = false;
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Adapt own style to bus

   \param[in] opc_Bus Connected bus item
*/
//----------------------------------------------------------------------------------------------------------------------
void C_GiLiBusConnectorBase::m_AdaptStyleToBus(const C_GiLiBus * const opc_Bus)
{
   this->SetWidth(opc_Bus->GetWidth() / 2L);
   this->SetColor(opc_Bus->GetColor());
   this->SetMiddleLineColor(opc_Bus->GetMiddleLineColor());
   this->SetMiddleLine(opc_Bus->GetMiddleLine());
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Check if last known bus points and current bus points differ at any relevant point

   based on current known connection

   \retval true  The statement is true
   \retval false The statement is false
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_GiLiBusConnectorBase::m_CheckAnyRelevantBusPointChanged(void) const
{
   bool q_Retval;

   if ((this->GetBusItem() != NULL) &&
       ((this->ms32_ConnIndex >= 0) &&
        (this->ms32_ConnIndex < (static_cast<sint32>(this->mc_LastKnownBusScenePoints.size()) - 1))))
   {
      if (this->GetBusItem()->GetNumberPoints() != static_cast<sintn>(this->mc_LastKnownBusScenePoints.size()))
      {
         //Bend or unbend occured
         q_Retval = true;
      }
      else
      {
         //Move or move point occured
         q_Retval = false;

         for (sint32 s32_ItP = this->ms32_ConnIndex; s32_ItP <= (this->ms32_ConnIndex + 1); ++s32_ItP)
         {
            const QPointF c_NewBusPoint = this->GetBusItem()->GetPointScenePos(s32_ItP);
            const QPointF c_PrevBusPoint = this->mc_LastKnownBusScenePoints[s32_ItP];
            //Check if point moved
            if ((C_OSCUtils::h_IsFloat64NearlyEqual(c_PrevBusPoint.x(), c_NewBusPoint.x()) == false) ||
                (C_OSCUtils::h_IsFloat64NearlyEqual(c_PrevBusPoint.y(), c_NewBusPoint.y()) == false))
            {
               q_Retval = true;
               break;
            }
         }
      }
   }
   else
   {
      //Unknown bus or connection
      q_Retval = true;
   }
   return q_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Check if last known bus points and current bus points differ by exactly one moved point

   \retval true  The statement is true
   \retval false The statement is false
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_GiLiBusConnectorBase::m_CheckOnlyOneBusPointMoved(void) const
{
   bool q_Retval;

   if ((this->GetBusItem() != NULL) &&
       ((this->ms32_ConnIndex >= 0) &&
        (this->ms32_ConnIndex < (static_cast<sint32>(this->mc_LastKnownBusScenePoints.size()) - 1))))
   {
      if (this->GetBusItem()->GetNumberPoints() != static_cast<sintn>(this->mc_LastKnownBusScenePoints.size()))
      {
         //Bend or unbend occured
         q_Retval = false;
      }
      else
      {
         //Move or move point occured
         bool q_FoundOneMovedPoint = false;

         for (sint32 s32_ItP = this->ms32_ConnIndex; s32_ItP <= (this->ms32_ConnIndex + 1); ++s32_ItP)
         {
            const QPointF c_NewBusPoint = this->GetBusItem()->GetPointScenePos(s32_ItP);
            const QPointF c_PrevBusPoint = this->mc_LastKnownBusScenePoints[s32_ItP];
            //Check if point moved
            if ((C_OSCUtils::h_IsFloat64NearlyEqual(c_PrevBusPoint.x(), c_NewBusPoint.x()) == false) ||
                (C_OSCUtils::h_IsFloat64NearlyEqual(c_PrevBusPoint.y(), c_NewBusPoint.y()) == false))
            {
               //Check if first moved point
               if (q_FoundOneMovedPoint)
               {
                  //Second move: abort search with result: false
                  q_FoundOneMovedPoint = false;
                  break;
               }
               else
               {
                  //For now exactly one point moved
                  q_FoundOneMovedPoint = true;
               }
            }
         }
         if (q_FoundOneMovedPoint)
         {
            //Found exactly one moved point
            q_Retval = true;
         }
         else
         {
            //Found more or found none
            q_Retval = false;
         }
      }
   }
   else
   {
      //Unknown bus
      q_Retval = false;
   }
   return q_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Update last known bus interaction point positions
*/
//----------------------------------------------------------------------------------------------------------------------
void C_GiLiBusConnectorBase::m_UpdateLastKnownBusPoints(void)
{
   //Always clear first (otherwise appending doesn't really lead to the expected result)
   this->mc_LastKnownBusScenePoints.clear();
   if (this->GetBusItem() != NULL)
   {
      //Get points, basically
      this->mc_LastKnownBusScenePoints.reserve(this->GetBusItem()->GetNumberPoints());
      for (sint32 s32_ItP = 0UL; s32_ItP < this->GetBusItem()->GetNumberPoints(); ++s32_ItP)
      {
         const QPointF c_NewBusPoint = this->GetBusItem()->GetPointScenePos(s32_ItP);
         this->mc_LastKnownBusScenePoints.push_back(c_NewBusPoint);
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Initialize common settings
*/
//----------------------------------------------------------------------------------------------------------------------
void C_GiLiBusConnectorBase::m_CommonInit(void)
{
   // connect
   connect(this, &C_GiLiBusConnectorBase::SigChangedGraphic, this, &C_GiLiBusConnectorBase::m_UpdateInternal);

   //Allow hover events for tool tip hide
   this->setAcceptHoverEvents(true);

   // Init z order
   this->setZValue(mf64_ZORDER_INIT_BUS_CONNECT);
}
