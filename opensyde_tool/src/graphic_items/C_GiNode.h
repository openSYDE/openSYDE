//-----------------------------------------------------------------------------
/*!
   \file
   \brief       Offers visualization and functionality of a node (header)

   \implementation
   project     openSYDE
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     04.08.2016  STW/S.Singer
   \endimplementation
*/
//-----------------------------------------------------------------------------
#ifndef C_GILINODE_H
#define C_GILINODE_H

/* -- Includes ------------------------------------------------------------- */
#include <QObject>
#include <QPainter>
#include <QGraphicsItem>
#include <QGraphicsDropShadowEffect>

#include "stwtypes.h"
#include "C_GiRectBaseGroup.h"
#include "C_GiPort.h"
#include "C_OSCNode.h"
#include "C_GiNodeBoundary.h"
#include "C_GiBiCustomToolTip.h"
#include "C_PuiSdNodeConnectionId.h"
#include "C_GiRectPixmap.h"

/* -- Namespace ------------------------------------------------------------ */
namespace stw_opensyde_gui
{
/* -- Global Constants ----------------------------------------------------- */

/* -- Types ---------------------------------------------------------------- */
class C_GiLiBusConnector;

class C_GiNode :
   public C_GiRectBaseGroup,
   public C_GiBiCustomToolTip
{
   Q_OBJECT

public:
   //lint -e{1938}  static const is guaranteed preinitialized before main
   C_GiNode(const stw_types::sint32 & ors32_Index, const stw_types::uint64 & oru64_ID,
            const stw_types::float64 & orf64_Width = mhf64_MinWidthNode,
            const stw_types::float64 & orf64_Height = mhf64_MinHeightNode, QGraphicsItem * const opc_Parent = NULL);
   virtual ~C_GiNode();
   virtual void FindClosestPoint(const QPointF & orc_ScenePoint, QPointF & orc_Closest) const;
   virtual void FindClosestPort(const QPointF &orc_ScenePoint, C_GiPort * (&orpc_Closest)) const;
   virtual stw_types::sintn type() const override;

   QString GetText(void) const;

   void AddConnection(C_GiLiBusConnector * const opc_Connection);
   void AddConnectionAndData(C_GiLiBusConnector * const opc_Connection,
                             const stw_opensyde_gui_logic::C_PuiSdNodeConnectionId & orc_NodeConnection,
                             const stw_types::uint8 & oru8_NodeId, const stw_types::uint32 & oru32_BusIndex);
   void UpdateConnection(const C_GiLiBusConnector * const opc_Connection,
                         const stw_opensyde_gui_logic::C_PuiSdNodeConnectionId & orc_NodeConnection,
                         const stw_types::uint8 & oru8_NodeId, const stw_types::uint32 & oru32_BusIndex) const;
   void ChangeInterface(const stw_types::uint8 ou8_Interface, const C_GiLiBusConnector * const opc_Connection,
                        const stw_types::uint8 & oru8_NodeId) const;
   const stw_opensyde_gui_logic::C_PuiSdNodeConnectionId * GetNodeConnectionId(
      const C_GiLiBusConnector * const opc_Connection) const;
   void SetNodeConnectionId(const C_GiLiBusConnector * const opc_Connection,
                            const stw_opensyde_gui_logic::C_PuiSdNodeConnectionId & orc_ConnId) const;
   virtual void LoadData(void) override;
   virtual void UpdateData(void) override;
   void RemoveConnector(const C_GiLiBusConnector * const opc_BusConnectorGraphicsItem);
   virtual void DeleteData(void);
   void DisconnectBus(const QString & orc_BusName);
   bool CheckConnectionAvailable(const stw_opensyde_core::C_OSCSystemBus::E_Type & ore_Type) const;
   bool CheckInterfaceAvailable(const stw_opensyde_core::C_OSCSystemBus::E_Type & ore_Type,
                                const stw_types::uint8 & oru8_Interface);
   stw_types::sint32 GetIndexOfConnector(const C_GiLiBusConnector * const opc_Connection) const;
   void GetOSCNodeConst(const stw_opensyde_core::C_OSCNode * (&orpc_Node)) const;
   void SetOSCNode(const stw_opensyde_core::C_OSCNode & orc_Node) const;
   virtual void RestoreDefaultCursor(void);
   virtual void SetTemporaryCursor(const QCursor & orc_TemporaryCursor);
   virtual void GenerateHint(void) override;
   void ReevaluatePortState(void);
   void CheckNodeForChanges(void);

   bool GetValid(void) const;

   void SetDrawWhiteFilter(const bool oq_Active);

   //The signals keyword is necessary for Qt signal slot functionality
   //lint -save -e1736

Q_SIGNALS:
   //lint -restore
   void SigOpenContextMenu(const QPointF & orc_Pos);
   void SigHideToolTip(void);

protected:
   // The naming of the Qt parameters can't be changed and are not compliant with the naming conventions
   //lint -save -e1960
   virtual void mousePressEvent(QGraphicsSceneMouseEvent * const opc_Event) override;
   virtual void hoverMoveEvent(QGraphicsSceneHoverEvent * const opc_Event) override;
   virtual void hoverLeaveEvent(QGraphicsSceneHoverEvent * const opc_Event) override;
   //lint -restore

   virtual void m_ResizeUpdateItems(const stw_types::float64 of64_DiffWidth,
                                    const stw_types::float64 of64_DiffHeight) override;

   void m_SetDrawBorder(const bool oq_Active);
   const C_GiNodeBoundary * m_GetBoundary(void) const;
   stw_types::sint32 m_GetIconSize(void) const;
   stw_types::uint32 m_GetScaleCategory(void) const;

   static const stw_types::float64 mhf64_MinWidthNode;
   static const stw_types::float64 mhf64_MinHeightNode;
   static const stw_types::uint32 mhu32_ScaleCategory0;
   static const stw_types::uint32 mhu32_ScaleCategory1;
   static const stw_types::uint32 mhu32_ScaleCategory2;
   static const stw_types::uint32 mhu32_ScaleCategory3;
   static const stw_types::uint32 mhu32_ScaleCategory4;
   static const stw_types::uint32 mhu32_ScaleCategory5;
   static const stw_types::uint32 mhu32_ScaleCategory6;

   bool mq_ErrorIconHovered;

private:
   //Avoid call
   C_GiNode(const C_GiNode &);
   C_GiNode & operator =(const C_GiNode &);

   C_GiNodeBoundary * mpc_Boundary;
   QVector<C_GiPort *> mc_Ports;

   QVector<C_GiLiBusConnector *> mc_Connections;
   C_GiRectPixmap * mpc_ConflictIcon;
   bool mq_Valid;
   bool mq_DrawWhiteFilter;
   QString mc_ErrorText;
   stw_types::sint32 ms32_IconSize;

   void m_InitPorts(void);
   void m_InitConflictIcon(void);
   void m_DetectIconSize(void);

   void m_UpdateItems(const stw_types::float64 of64_DiffWidth, const stw_types::float64 of64_DiffHeight,
                      const bool oq_Initial);

   static const stw_types::float64 mahf64_ScaleMinWidthNode[6];
   static const stw_types::float64 mahf64_ScaleMinHeightNode[6];
};

/* -- Extern Global Variables ---------------------------------------------- */
} //end of namespace

#endif
