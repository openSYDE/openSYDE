//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Connection of node to bus (header)

   \copyright   Copyright 2016 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_GILIBUSCONNECTOR_H
#define C_GILIBUSCONNECTOR_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <QObject>
#include <QTimer>

#include "stwtypes.hpp"

#include "C_GiLiBusConnectorBase.hpp"
#include "C_GiNode.hpp"
#include "C_GiPort.hpp"
#include "C_GiLiBus.hpp"
#include "C_GiLiLine.hpp"
#include "C_GiLiLineConnection.hpp"
#include "C_OscNode.hpp"
#include "C_PuiSdNode.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_gui
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_GiLiBusConnector :
   public C_GiLiBusConnectorBase
{
   Q_OBJECT

public:
   C_GiLiBusConnector(const uint64_t & oru64_Id, const QPointF & orc_TriggerPos, C_GiNode * const opc_NodeItem,
                      const C_GiLiBus * const opc_BusItem, const bool oq_MiddleLine = false,
                      QGraphicsItem * const opc_Parent = NULL);
   C_GiLiBusConnector(const uint64_t & oru64_Id, const std::vector<QPointF> & orc_InteractionPoints,
                      C_GiNode * const opc_NodeItem, const C_GiLiBus * const opc_BusItem,
                      const bool oq_MiddleLine = false, QGraphicsItem * const opc_Parent = NULL);
   ~C_GiLiBusConnector(void) override;
   int32_t type(void) const override;
   void DeleteConnection(void);
   void SetPoints(const std::vector<QPointF> & orc_ScenePos) override;

   const C_GiNode * GetNodeItem(void) const;
   C_GiNode * GetNodeItem(void);
   void UpdateData(stw::opensyde_gui_logic::C_PuiSdNodeConnection * const opc_UiConnection) const;
   void GenerateHint(void) override;
   const stw::opensyde_gui_logic::C_PuiSdNodeConnectionId * GetConnectionData(void) const;
   void ChangeInterface(const uint8_t & oru8_NewInterface,
                        const std::vector<stw::opensyde_gui_logic::C_PuiSdNodeInterfaceAutomaticProperties> & orc_Properties);
   void Revert(C_GiNode * const opc_StartingNode, const C_GiNode * const opc_LastNode, const QPointF & orc_ScenePos);
   void Reconnect(C_GiNode * const opc_StartingNode, stw::opensyde_gui::C_GiNode * const opc_LastNode,
                  const QPointF & orc_ConnectionPos, const int32_t & ors32_Interface,
                  const std::vector<stw::opensyde_gui_logic::C_PuiSdNodeInterfaceAutomaticProperties> & orc_Properties);
   void Reconnect(const stw::opensyde_gui::C_GiLiBus * const opc_StartingBus,
                  const stw::opensyde_gui::C_GiLiBus * const opc_LastBus, const QPointF & orc_ConnectionPos,
                  const int32_t & ors32_Interface,
                  const std::vector<stw::opensyde_gui_logic::C_PuiSdNodeInterfaceAutomaticProperties> & orc_Properties);
   const C_GiPort * GetPortItem(void) const;

   //The signals keyword is necessary for Qt signal slot functionality
   //lint -save -e1736

Q_SIGNALS:
   //lint -restore
   void SigShowReconnectContextMenu(const QPointF & orc_ScenePos, C_GiNode * const opc_Node, const C_GiLiBus * opc_Bus,
                                    const int32_t & ors32_FreeInterface, C_GiLiBusConnector * const opc_BusConnector);
   void SigHintUpdate(void);

protected:
   void m_OnInteractionPointMove(void) override;
   void m_OnIterationGenericInteractionPointMove(QGraphicsItem * const opc_HighestParentItem,
                                                 const QPointF & orc_CurPos, bool & orq_RestoreMouseCursor) override;
   bool m_OnGenericInteractionPointMouseRelease(const QPointF & orc_ScenePos) override;
   void m_OnBusChange(const QPointF & orc_ScenePos) override;

private:
   bool mq_OnInteractionPointMoveFoundNode;

   //Avoid call
   C_GiLiBusConnector(const C_GiLiBusConnector &);
   C_GiLiBusConnector & operator =(const C_GiLiBusConnector &) &; //lint !e1511 //we want to hide the base func.

   void m_InitConnector(C_GiNode * const opc_NodeItem, const QPointF & orc_Pos);
   void m_UpdatePort(const QPointF & orc_Pos);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
