//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Handler class for topology undo command stack (header)

   See cpp file for detailed description

   \copyright   Copyright 2016 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_SDMANUNOTOPOLOGYMANAGER_H
#define C_SDMANUNOTOPOLOGYMANAGER_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */

#include <QList>
#include <QGraphicsItem>
#include <QGraphicsScene>
#include <QPointF>
#include <QSizeF>
#include "stwtypes.hpp"
#include "C_PuiSdDataElement.hpp"
#include "C_SdManUnoTopologyAddCommand.hpp"
#include "C_SdTopologyDataSnapshot.hpp"
#include "C_GiLiBusConnector.hpp"
#include "C_SebUnoTopBaseManager.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */

namespace stw
{
namespace opensyde_gui_logic
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_SdManUnoTopologyManager :
   public C_SebUnoTopBaseManager
{
   Q_OBJECT

public:
   C_SdManUnoTopologyManager(QGraphicsScene * const opc_Scene = NULL, QObject * const opc_Parent = NULL);
   ~C_SdManUnoTopologyManager(void) override;

   void AdaptZetOrder(const QList<QGraphicsItem *> & orc_SelectedItems, const QList<QGraphicsItem *> & orc_Items,
                      const bool oq_BringToFront) override;
   void DoDelete(const QList<QGraphicsItem *> & orc_Items) override;
   void DoAddGeneric(const C_SdManUnoTopologyAddCommand::E_ElementType & ore_Type, const uint64_t & oru64_UniqueId,
                     const QPointF & orc_NewPos, const QString & orc_AdditionalInformation = "");
   void DoAddBus(const C_SdManUnoTopologyAddCommand::E_ElementType & ore_Type, const uint64_t & oru64_UniqueIdBus,
                 const uint64_t & oru64_UniqueIdBusName, const QPointF & orc_NewPos);
   void DoAddBusConnector(const uint64_t & oru64_UniqueId, const QPointF & orc_NewPos,
                          const QGraphicsItem * const opc_Node, const QGraphicsItem * const opc_Bus,
                          const uint8_t & oru8_InterfaceNumber,
                          const std::vector<C_PuiSdNodeInterfaceAutomaticProperties> & orc_Properties);
   void DoAddNodeToNodeConnectionAndCreateNewBus(const std::vector<uint64_t> & orc_FourUniqueIds,
                                                 const stw::opensyde_core::C_OscSystemBus::E_Type & ore_BusType,
                                                 const QString & orc_BusName, const QPointF & orc_BusPosition,
                                                 const uint64_t & oru64_Node1UniqueId,
                                                 const uint64_t & oru64_Node2UniqueId,
                                                 const uint8_t & oru8_Node1InterfaceNumber,
                                                 const uint8_t & oru8_Node2InterfaceNumber,
                                                 const std::vector<C_PuiSdNodeInterfaceAutomaticProperties> & orc_Node1Properties, const std::vector<C_PuiSdNodeInterfaceAutomaticProperties> & orc_Node2Properties);
   void DoAddNodeToNodeConnectionUsingExistingBus(const std::vector<uint64_t> & orc_TwoUniqueIds,
                                                  const uint64_t & oru64_BusUniqueId, const QPointF & orc_Node1Position,
                                                  const uint64_t & oru64_Node1UniqueId,
                                                  const uint8_t & oru8_Node1InterfaceNumber,
                                                  const std::vector<C_PuiSdNodeInterfaceAutomaticProperties> & orc_Node1Properties);
   void DoAddSnapshot(const std::vector<uint64_t> & orc_UniqueIds, const C_SdTopologyDataSnapshot & orc_Snapshot,
                      const QPointF & orc_NewPos, const float64_t of64_HighestUsedZetValue);
   void DoReconnectNode(const stw::opensyde_gui::C_GiLiBusConnector * const opc_BusConnector,
                        const stw::opensyde_gui::C_GiNode * const opc_StartingNode,
                        const stw::opensyde_gui::C_GiNode * const opc_LastNode, const QPointF & orc_ConnectionPos,
                        const int32_t & ors32_Interface,
                        const std::vector<C_PuiSdNodeInterfaceAutomaticProperties> & orc_Properties);
   void DoReconnectBus(const stw::opensyde_gui::C_GiLiBusConnector * const opc_BusConnector,
                       const stw::opensyde_gui::C_GiLiBus * const opc_StartingBus,
                       const stw::opensyde_gui::C_GiLiBus * const opc_LastBus, const QPointF & orc_ConnectionPos,
                       const int32_t & ors32_Interface,
                       const std::vector<C_PuiSdNodeInterfaceAutomaticProperties> & orc_Properties);
   void DoChangeInterface(const stw::opensyde_gui::C_GiLiBusConnector * const opc_BusConnector,
                          const uint8_t & oru8_PreviousInterfaceNumber, const uint8_t & oru8_NewInterfaceNumber,
                          const std::vector<C_PuiSdNodeInterfaceAutomaticProperties> & orc_PreviousProperties,
                          const std::vector<C_PuiSdNodeInterfaceAutomaticProperties> & orc_NewProperties);

protected:
   C_SebUnoSetupStyleCommand * m_GetNewStyleCommand(const std::vector<uint64_t> & orc_Items,
                                                    const bool oq_DarkMode) override;

private:
   void m_MergeWithPrev(QUndoCommand * const opc_Command);
   void m_OnErrorChange(void);
   void m_DoPushAndSignalError(QUndoCommand * const opc_UndoCommand);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
