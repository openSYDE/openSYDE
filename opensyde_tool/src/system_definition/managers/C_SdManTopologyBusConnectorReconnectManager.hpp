//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Bus connector reconnect action manager of system definition toplogy (header)

   See cpp file for detailed description

   \copyright   Copyright 2016 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_SDMANTOPOLOGYBUSCONNECTORRECONNECTMANAGER_HPP
#define C_SDMANTOPOLOGYBUSCONNECTORRECONNECTMANAGER_HPP

/* -- Includes ------------------------------------------------------------------------------------------------------ */

#include <QObject>
#include "C_GiLiBusConnector.hpp"
#include "C_GiNode.hpp"
#include "C_GiLiBus.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_gui_logic
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_SdManTopologyBusConnectorReconnectManager :
   public QObject
{
   Q_OBJECT

public:
   C_SdManTopologyBusConnectorReconnectManager();

   void StartReconnectMode(stw::opensyde_gui::C_GiLiBusConnector * const opc_BusConnector,
                           const stw::opensyde_gui::C_GiLiBusConnector::E_ConnectState & ore_ConnectState);
   void EnterWaitForContextMenuState(const QPointF & orc_ScenePos, stw::opensyde_gui::C_GiNode * const opc_Node,
                                     const stw::opensyde_gui::C_GiLiBus * const opc_Bus);
   void ContextMenuAboutToClose(void);
   void ContextMenuAccepted(const int32_t & ors32_Interface,
                            const std::vector<C_PuiSdNodeInterfaceAutomaticProperties> & orc_Properties);
   void DeactivateReconnection(void);
   bool Active(void) const;

   //The signals keyword is necessary for Qt signal slot functionality
   //lint -save -e1736

Q_SIGNALS:
   //lint -restore
   void SigCleanUpPorts(void);
   void SigRevertNode(stw::opensyde_gui::C_GiLiBusConnector * const opc_BusConnector,
                      stw::opensyde_gui::C_GiNode * const opc_StartingNode,
                      stw::opensyde_gui::C_GiNode * const opc_LastNode, const QPointF & orc_ScenePos);
   void SigReconnectNode(stw::opensyde_gui::C_GiLiBusConnector * const opc_BusConnector,
                         stw::opensyde_gui::C_GiNode * const opc_StartingNode,
                         stw::opensyde_gui::C_GiNode * const opc_LastNode, const QPointF & orc_ConnectionPos,
                         const int32_t & ors32_Interface,
                         const std::vector<C_PuiSdNodeInterfaceAutomaticProperties> & orc_Properties);
   void SigRevertBus(stw::opensyde_gui::C_GiLiBusConnector * const opc_BusConnector,
                     const stw::opensyde_gui::C_GiLiBus * const opc_StartingBus,
                     const stw::opensyde_gui::C_GiLiBus * const opc_LastBus, const QPointF & orc_ScenePos);
   void SigReconnectBus(stw::opensyde_gui::C_GiLiBusConnector * const opc_BusConnector,
                        const stw::opensyde_gui::C_GiLiBus * const opc_StartingBus,
                        const stw::opensyde_gui::C_GiLiBus * const opc_LastBus, const QPointF & orc_ConnectionPos,
                        const int32_t & ors32_Interface,
                        const std::vector<C_PuiSdNodeInterfaceAutomaticProperties> & orc_Properties);
   void SigCleanUpTemporaryLine(void);

private:
   stw::opensyde_gui::C_GiLiBusConnector * mpc_BusConnector;
   stw::opensyde_gui::C_GiNode * mpc_StartingNode;
   const stw::opensyde_gui::C_GiLiBus * mpc_StartingBus;
   stw::opensyde_gui::C_GiNode * mpc_LastNode;
   const stw::opensyde_gui::C_GiLiBus * mpc_LastBus;
   stw::opensyde_gui::C_GiLiBusConnector::E_ConnectState me_ConnectState;
   int32_t ms32_NewInterface;
   std::vector<C_PuiSdNodeInterfaceAutomaticProperties> mc_Properties;
   QPointF mc_ConnectionPos;
   QPointF mc_LastKnownInteractionPointPosition;
   bool mq_ContextMenuActive;

   void m_FinishReconnect(void);
   void m_InitLastKnownInteractionPointPosition(void);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
