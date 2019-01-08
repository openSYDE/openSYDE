//-----------------------------------------------------------------------------
/*!
   \file
   \brief       Connection of node to bus (header)

   \implementation
   project     openSYDE
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     18.08.2016  STW/B.Bayer
   \endimplementation
*/
//-----------------------------------------------------------------------------
#ifndef C_GILIBUSCONNECTOR_H
#define C_GILIBUSCONNECTOR_H

/* -- Includes ------------------------------------------------------------- */
#include <QObject>
#include <QTimer>

#include "stwtypes.h"

#include "C_GiLiBusConnectorBase.h"
#include "C_GiNode.h"
#include "C_GiPort.h"
#include "C_GiLiBus.h"
#include "C_GiLiLine.h"
#include "C_GiLiLineConnection.h"
#include "C_OSCNode.h"
#include "C_PuiSdNode.h"

/* -- Namespace ------------------------------------------------------------ */
namespace stw_opensyde_gui
{
/* -- Global Constants ----------------------------------------------------- */

/* -- Types ---------------------------------------------------------------- */

//lint -e{1960} Forward-declaration was necessary
class C_GiLiBusConnector :
   public C_GiLiBusConnectorBase
{
   Q_OBJECT

public:
   C_GiLiBusConnector(const stw_types::uint64 & oru64_ID, const QPointF & orc_TriggerPos, C_GiNode * const opc_NodeItem,
                      const C_GiLiBus * const opc_BusItem, const bool oq_MiddleLine = false,
                      QGraphicsItem * const opc_Parent = NULL);
   C_GiLiBusConnector(const stw_types::uint64 & oru64_ID, const std::vector<QPointF> & orc_InteractionPoints,
                      C_GiNode * const opc_NodeItem, const C_GiLiBus * const opc_BusItem,
                      const bool oq_MiddleLine = false, QGraphicsItem * const opc_Parent = NULL);
   virtual ~C_GiLiBusConnector(void);
   virtual stw_types::sintn type(void) const override;
   void DeleteConnection(void);
   virtual void SetPoints(const std::vector<QPointF> & orc_ScenePos);

   const C_GiNode * GetNodeItem(void) const;
   C_GiNode * GetNodeItem(void);
   void UpdateData(stw_opensyde_gui_logic::C_PuiSdNodeConnection * const opc_UIConnection) const;
   virtual void GenerateHint(void);
   const stw_opensyde_gui_logic::C_PuiSdNodeConnectionId * GetConnectionData(void) const;
   void ChangeInterface(const stw_types::uint8 & oru8_NewInterface, const stw_types::uint8 & oru8_NodeId);
   void Revert(C_GiNode * const opc_StartingNode, C_GiNode * const opc_LastNode, const QPointF & orc_ScenePos);
   void Reconnect(C_GiNode * const opc_StartingNode, stw_opensyde_gui::C_GiNode * const opc_LastNode,
                  const QPointF & orc_ConnectionPos, const stw_types::sint32 & ors32_Interface,
                  const stw_types::uint8 & oru8_NodeId);
   void Reconnect(const stw_opensyde_gui::C_GiLiBus * const opc_StartingBus,
                  const stw_opensyde_gui::C_GiLiBus * const opc_LastBus, const QPointF & orc_ConnectionPos,
                  const stw_types::sint32 & ors32_Interface, const stw_types::uint8 & oru8_NodeId);
   const C_GiPort * GetPortItem(void) const;

   //The signals keyword is necessary for Qt signal slot functionality
   //lint -save -e1736

Q_SIGNALS:
   //lint -restore
   void ShowReconnectContextMenu(const QPointF & orc_ScenePos, C_GiNode * const opc_Node, const C_GiLiBus * opc_Bus,
                                 const stw_types::sint32 & ors32_FreeInterface,
                                 C_GiLiBusConnector * const opc_BusConnector);
   void SigHintUpdate(void);

protected:
   virtual void m_OnInteractionPointMove(void);
   virtual void m_OnIterationGenericInteractionPointMove(QGraphicsItem * const opc_HighestParentItem,
                                                         const QPointF & orc_CurPos, bool & orq_RestoreMouseCursor);
   virtual bool m_OnGenericInteractionPointMouseRelease(const QPointF & orc_ScenePos);
   virtual void m_OnBusChange(const QPointF & orc_ScenePos);

private:
   bool mq_OnInteractionPointMoveFoundNode;

   //Avoid call
   C_GiLiBusConnector(const C_GiLiBusConnector &);
   C_GiLiBusConnector & operator =(const C_GiLiBusConnector &);

   void m_InitConnector(C_GiNode * const opc_NodeItem, const QPointF & orc_Pos);
   void m_UpdatePort(const QPointF & orc_Pos);
   void m_RestoreLastValidNode(void);
   void m_AcceptNewPort(void);
};

/* -- Extern Global Variables ---------------------------------------------- */
} //end of namespace

#endif
