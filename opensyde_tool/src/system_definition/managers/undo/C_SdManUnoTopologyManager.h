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
#include "stwtypes.h"
#include "C_PuiSdDataElement.h"
#include "C_SdManUnoTopologyAddCommand.h"
#include "C_SdTopologyDataSnapshot.h"
#include "C_GiLiBusConnector.h"
#include "C_SebUnoTopBaseManager.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */

namespace stw_opensyde_gui_logic
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_SdManUnoTopologyManager :
   public C_SebUnoTopBaseManager
{
   Q_OBJECT

public:
   C_SdManUnoTopologyManager(QGraphicsScene * const opc_Scene = NULL, QObject * const opc_Parent = NULL);
   virtual ~C_SdManUnoTopologyManager(void);

   virtual void AdaptZOrder(const QList<QGraphicsItem *> & orc_SelectedItems, const QList<QGraphicsItem *> & orc_Items,
                            const bool oq_BringToFront) override;
   virtual void DoDelete(const QList<QGraphicsItem *> & orc_Items) override;
   void DoAddGeneric(const C_SdManUnoTopologyAddCommand::E_ElementType & ore_Type,
                     const stw_types::uint64 & oru64_UniqueID, const QPointF & orc_NewPos,
                     const QString & orc_AdditionalInformation = "");
   void DoAddBus(const C_SdManUnoTopologyAddCommand::E_ElementType & ore_Type,
                 const stw_types::uint64 & oru64_UniqueIDBus, const stw_types::uint64 & oru64_UniqueIDBusName,
                 const QPointF & orc_NewPos);
   void DoAddBusConnector(const stw_types::uint64 & oru64_UniqueID, const QPointF & orc_NewPos,
                          const QGraphicsItem * const opc_Node, const QGraphicsItem * const opc_Bus,
                          const stw_types::uint8 & oru8_InterfaceNumber,
                          const std::vector<stw_types::uint8> & orc_NodeIds);
   void DoAddNodeToNodeConnectionAndCreateNewBus(const std::vector<stw_types::uint64> & orc_FourUniqueIds,
                                                 const stw_opensyde_core::C_OSCSystemBus::E_Type & ore_BusType,
                                                 const QString & orc_BusName, const QPointF & orc_BusPosition,
                                                 const stw_types::uint64 & oru64_Node1UniqueID,
                                                 const stw_types::uint64 & oru64_Node2UniqueID,
                                                 const stw_types::uint8 & oru8_Node1InterfaceNumber,
                                                 const stw_types::uint8 & oru8_Node2InterfaceNumber,
                                                 const std::vector<stw_types::uint8> & orc_Node1NodeIDs,
                                                 const std::vector<stw_types::uint8> & orc_Node2NodeIDs);
   void DoAddNodeToNodeConnectionUsingExistingBus(const std::vector<stw_types::uint64> & orc_TwoUniqueIds,
                                                  const stw_types::uint64 & oru64_BusUniqueID,
                                                  const QPointF & orc_Node1Position,
                                                  const stw_types::uint64 & oru64_Node1UniqueID,
                                                  const stw_types::uint8 & oru8_Node1InterfaceNumber,
                                                  const std::vector<stw_types::uint8> & orc_Node1IDs);
   void DoAddSnapshot(const std::vector<stw_types::uint64> & oru64_UniqueIDs,
                      const C_SdTopologyDataSnapshot & orc_Snapshot, const QPointF & orc_NewPos,
                      const stw_types::float64 of64_HighestUsedZValue);
   void DoReconnectNode(const stw_opensyde_gui::C_GiLiBusConnector * const opc_BusConnector,
                        const stw_opensyde_gui::C_GiNode * const opc_StartingNode,
                        const stw_opensyde_gui::C_GiNode * const opc_LastNode, const QPointF & orc_ConnectionPos,
                        const stw_types::sint32 & ors32_Interface, const std::vector<stw_types::uint8> & orc_NodeIds);
   void DoReconnectBus(const stw_opensyde_gui::C_GiLiBusConnector * const opc_BusConnector,
                       const stw_opensyde_gui::C_GiLiBus * const opc_StartingBus,
                       const stw_opensyde_gui::C_GiLiBus * const opc_LastBus, const QPointF & orc_ConnectionPos,
                       const stw_types::sint32 & ors32_Interface, const std::vector<stw_types::uint8> & orc_NodeIds);
   void DoChangeInterface(const stw_opensyde_gui::C_GiLiBusConnector * const opc_BusConnector,
                          const stw_types::uint8 & oru8_PreviousInterfaceNumber,
                          const stw_types::uint8 & oru8_NewInterfaceNumber,
                          const std::vector<stw_types::uint8> & orc_PreviousNodeIds,
                          const std::vector<stw_types::uint8> & orc_NewNodeIds);

protected:
   virtual C_SebUnoSetupStyleCommand * m_GetNewStyleCommand(const std::vector<stw_types::uint64> & orc_Items,
                                                            const bool oq_DarkMode) override;

private:
   void m_MergeWithPrev(QUndoCommand * const opc_Command);
   void m_OnErrorChange(void);
   void m_DoPushAndSignalError(QUndoCommand * const opc_UndoCommand);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif
