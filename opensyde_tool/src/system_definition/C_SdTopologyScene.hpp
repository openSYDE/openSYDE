//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Main graphics scene for system topology (header)

   \copyright   Copyright 2016 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_SDTOPOLOGYSCENE_HPP
#define C_SDTOPOLOGYSCENE_HPP

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <QMimeData>
#include <QMap>
#include <QTransform>

#include "stwtypes.hpp"

#include "C_SebTopologyBaseScene.hpp"
#include "C_GiNode.hpp"
#include "C_GiLiCanBus.hpp"
#include "C_GiLiEthernetBus.hpp"
#include "C_GiLiBusConnector.hpp"
#include "C_GiArrowCursorButton.hpp"
#include "C_GiLiTemporaryLine.hpp"
#include "C_SdManTopologyContextMenuManager.hpp"
#include "C_SdManTopologyCopyPasteManager.hpp"
#include "C_SdManUnoTopologyManager.hpp"
#include "C_PuiSdDataElement.hpp"
#include "C_GiSdArrow.hpp"
#include "C_GiSdBoundary.hpp"
#include "C_GiSdImageGroup.hpp"
#include "C_GiSdTextElement.hpp"
#include "C_GiTextElementBus.hpp"
#include "C_SdTopologyDataSnapshot.hpp"
#include "C_PuiBsTemporaryDataId.hpp"
#include "C_OgeWiCustomMessage.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_gui
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_SdTopologyScene :
   public C_SebTopologyBaseScene
{
   Q_OBJECT

public:
   explicit C_SdTopologyScene(const bool & orq_LoadSystemDefintion, QObject * const opc_Parent = NULL);
   ~C_SdTopologyScene() override;

   void AddNode(const QString & orc_NodeType, const QPointF & orc_Pos, const uint64_t * const opu64_UniqueId);
   void AddCanBus(const QPointF & orc_Pos, const uint64_t * const opu64_UniqueId, const float64_t of64_ZetValue,
                  C_GiTextElementBus * const opc_TextElementBus, const QString * const opc_NameProposal = NULL);
   void AddEthernetBus(const QPointF & orc_Pos, const uint64_t * const opu64_UniqueId, const float64_t of64_ZetValue,
                       C_GiTextElementBus * const opc_TextElementBus, const QString * const opc_NameProposal = NULL);
   void AddBoundary(const QPointF & orc_Pos, const uint64_t * const opu64_UniqueId);
   void AddTextElement(const QString & orc_Text, const QPointF & orc_Pos, const uint64_t * const opu64_UniqueId);
   C_GiTextElementBus * AddTextElementBus(const uint64_t * const opu64_UniqueId, float64_t & orf64_BusZetValue);
   void AddLine(const QPointF & orc_Pos, const uint64_t * const opu64_UniqueId);
   void AddImage(const QString & orc_FilePath, const QPointF & orc_Pos, const uint64_t * const opu64_UniqueId);
   void AddBusConnector(C_GiNode * const opc_Node, const C_GiLiBus * const opc_Bus,
                        const uint8_t & oru8_InterfaceNumber,
                        const std::vector<stw::opensyde_gui_logic::C_PuiSdNodeInterfaceAutomaticProperties> & orc_Properties, const QPointF & orc_Pos, const uint64_t * const opu64_UniqueId = NULL);

   void UpdateTransform(const QTransform & orc_Transform) override;
   //lint -e{1735} Suppression, because default parameters are identical
   void CopyFromManagerToScene(const QPointF * const opc_Pos = NULL) override;
   void CopyFromSnapshotToScene(const stw::opensyde_gui_logic::C_SdTopologyDataSnapshot & orc_Snapshot,
                                const QMap<stw::opensyde_gui_logic::C_PuiBsTemporaryDataId,
                                           uint64_t> * const opc_IdMap =
                                   NULL);
   void Save(void) const;
   void DeleteItem(QGraphicsItem * const opc_Item);

   void UpdateSystemDefinition(void) const;

   bool IsAnyItemAddable(void) const override;
   bool IsItemMovable(const QGraphicsItem * const opc_Item) const override;
   bool IsItemSelectable(const QGraphicsItem * const opc_Item) const override;
   bool IsItemDeletable(const QGraphicsItem * const opc_Item) const override;
   bool IsZetOrderChangeable(const QGraphicsItem * const opc_Item) const override;
   bool IsAlignmentUsable(const QGraphicsItem * const opc_Item) const override;
   bool IsSceneRubberBandAvailable(void) const override;

   //The signals keyword is necessary for Qt signal slot functionality
   //lint -save -e1736

Q_SIGNALS:
   //lint -restore
   void SigChangeMode(const int32_t os32_Mode, const int32_t os32_SubMode, const uint32_t ou32_Index,
                      const QString & orc_Name, const QString & orc_SubSubItemName, const uint32_t ou32_Flag,
                      const bool oq_ChangeUseCase = false);
   void SigNodeDeleted(const uint32_t ou32_Index);
   void SigBusDeleted(const uint32_t ou32_Index);
   bool SigOpenTsp(void);
   void SigErrorChange(void);

protected:
   C_SebBaseContextMenuManager * m_GetContextMenuManager(void) override;
   stw::opensyde_gui_logic::C_SebBaseCopyPasteManager * m_GetCopyPasteManager(void) override;
   const stw::opensyde_gui_logic::C_SebBaseCopyPasteManager * m_GetCopyPasteManagerConst(void) const override;
   stw::opensyde_gui_logic::C_SebUnoBaseManager * m_GetUndoManager(void) override;

   void mouseMoveEvent(QGraphicsSceneMouseEvent * const opc_Event) override;
   void mouseReleaseEvent(QGraphicsSceneMouseEvent * const opc_Event) override;
   void mouseDoubleClickEvent(QGraphicsSceneMouseEvent * const opc_Event) override;
   void keyPressEvent(QKeyEvent * const opc_KeyEvent) override;

   void contextMenuEvent(QGraphicsSceneContextMenuEvent * const opc_Event) override;

   void m_OpenContextMenu(const QPointF & orc_Pos);

   // drag and drop function
   void m_AddImage(const QString & orc_Path, const QPointF & orc_Position) override;
   bool m_AddOfMime(const QMimeData * const opc_MimeData, const QPointF & orc_Position) override;

   // copy past functions
   void m_Copy(void) override;
   void m_Cut(void) override;
   bool m_HandleDeleteUserConfirmation(const QList<QGraphicsItem *> & orc_SelectedItems) const override;
   //lint -e{1735} Suppression, because default parameters are identical
   void m_PasteOfClipBoard(const QPointF * const opc_Pos = NULL) override;

   C_GiNode * m_CreateNode(const int32_t & ors32_Index, const uint64_t & oru64_Id, const float64_t & orf64_Width,
                           const float64_t & orf64_Height, QGraphicsItem * const opc_Parent) override;
   C_GiLiCanBus * m_CreateCanBus(const int32_t & ors32_Index, const uint64_t & oru64_Id,
                                 C_GiTextElementBus * const opc_TextElementName,
                                 const std::vector<QPointF> * const opc_Points,
                                 QGraphicsItem * const opc_Parent) override;
   C_GiLiEthernetBus * m_CreateEthernetBus(const int32_t & ors32_Index, const uint64_t & oru64_Id,
                                           C_GiTextElementBus * const opc_TextElementName,
                                           const std::vector<QPointF> * const opc_Points,
                                           QGraphicsItem * const opc_Parent) override;
   C_GiTextElementBus * m_CreateBusTextElement(const int32_t & ors32_Index, const uint64_t & oru64_Id,
                                               QGraphicsItem * const opc_Parent) override;

   void m_AddNodeToScene(C_GiNode * const opc_NodeGraphicsItem) override;
   void m_AddBusConnectorToScene(C_GiLiBusConnector * const opc_BusConnectorGraphicsItem) override;
   void m_AddTextElementToScene(C_GiBiTextElement * const opc_Item) override;
   //Move
   void m_HandleRevertableResizeLine(const int32_t & ors32_InteractionPointIndex,
                                     const QPointF & orc_PositionDifference) override;
   //Setup style
   bool m_CallSetupStyle(QGraphicsItem * const opc_Item) const override;

private:
   //Avoid call
   C_SdTopologyScene(const C_SdTopologyScene &);
   C_SdTopologyScene & operator =(const C_SdTopologyScene &) &; //lint !e1511 //we want to hide the base func.

   QVector<uint32_t> m_CopyFromSnapshotToSceneHandleNodes(
      const stw::opensyde_gui_logic::C_SdTopologyDataSnapshot & orc_Snapshot, const QMap<uint32_t,
                                                                                         uint32_t> & orc_MapOldBusIndexToNewBusIndex)
   const;
   static uint32_t mh_CopyFromSnapshotToSceneHandleNodesAddNewNodes(
      const stw::opensyde_gui_logic::C_SdTopologyDataSnapshot & orc_Snapshot,
      const uint32_t ou32_SnapshotNodeIndex);
   static void mh_CopyFromSnapshotToSceneHandleNodesAdaptConnections(const uint32_t ou32_DataNodeIndex,
                                                                     const QMap<uint32_t,
                                                                                uint32_t> & orc_MapOldBusIndexToNewBusIndex);
   void m_SelectionChanged(void);
   void m_HandleArrowButton(const bool & orq_Selection);
   void m_StartConnector(const QPointF & orc_LineStart, const QPointF & orc_SceneTriggerPos);

   void m_EditPressed(void);
   void m_EditClicked(const QGraphicsItem * const opc_Item);
   void m_Edit(const QGraphicsItem * const opc_Item, const bool oq_FocusName = false);

   void m_SyncIndex(const stw::opensyde_gui_logic::C_PuiSdDataElement::E_Type & ore_Type, const int32_t & ors32_Index,
                    const stw::opensyde_gui_logic::C_PuiSdDataElement::E_Action & ore_Action) const;
   void m_ConnectNodeToBus(const uint8_t &  oru8_InterfaceNumber,
                           const std::vector<stw::opensyde_gui_logic::C_PuiSdNodeInterfaceAutomaticProperties> & orc_Properties);
   void m_ChangeInterface(const uint8_t & oru8_InterfaceNumber,
                          const std::vector<stw::opensyde_gui_logic::C_PuiSdNodeInterfaceAutomaticProperties> & orc_Properties, C_GiLiBusConnector * const opc_Connector);
   void m_RestoreToolTips(void) const;
   void m_RemoveNodeOfScene(const C_GiNode * const opc_NodeGraphicsItem);
   void m_RemoveBusOfScene(const C_GiLiBus * const opc_BusGraphicsItem);
   void m_RemoveBusConnectorOfScene(C_GiLiBusConnector * const opc_BusConnectorGraphicsItem);
   void m_RemoveLineArrowOfScene(const C_GiSdArrow * const opc_Item);
   void m_RemoveBoundaryOfScene(const C_GiSdBoundary * const opc_Item);
   void m_RemoveImageGroupOfScene(const C_GiSdImageGroup * const opc_Item);
   void m_RemoveTextElementOfScene(const C_GiSdTextElement * const opc_Item);
   void m_RemoveTextElementBusOfScene(const C_GiTextElementBus * const opc_Item);
   void m_EnterConnectState(const C_GiLiBusConnector::E_ConnectState & ore_ConnectState,
                            const C_GiNode * const opc_Node = NULL,
                            const stw::opensyde_core::C_OscSystemBus::E_Type * const ope_Type = NULL);
   void m_LeaveConnectState(void);
   void m_RemoveConnectorAndLeaveConnectState(void);
   void m_DisableEverythingForConnectState(const C_GiNode * const opc_Node) const;
   void m_ShowNewConnectionPopUp(const C_GiNode * const opc_Node, const C_GiLiBus * const opc_Bus,
                                 const bool & orq_ChangeInterface, const bool & orq_Reconnect,
                                 const int32_t & ors32_SpecialInterface, C_GiLiBusConnector * const opc_Connector);
   void m_ShowNewNodeToNodeConnectionPopUp(const C_GiNode * const opc_Node1, const C_GiNode * const opc_Node2);
   std::vector<std::vector<uint8_t> > m_AssignNodeProperty(const std::vector<uint32_t> & orc_NodeIndices, const stw::opensyde_core::C_OscSystemBus::E_Type
                                                           & ore_BusType, const uint32_t & oru32_BusIndex,
                                                           const std::vector<uint8_t> & orc_InterfaceIndices,
                                                           const bool oq_BusExists,
                                                           const bool oq_GenerateId);
   uint32_t m_GeneratePropertyUsingExisting(const stw::opensyde_core::C_OscNode & orc_Node,
                                            const uint32_t & oru32_BusIndex, const std::vector<
                                               uint8_t> & orc_ExistingNode1Properties,
                                            const std::vector<uint8_t> & orc_ExistingNode2Properties,
                                            const bool oq_BusExists, const bool oq_GenerateId,
                                            const uint8_t ou8_CurrentProperty);
   std::vector<uint8_t> m_BuildCompleteIpAddress(const uint8_t & oru8_IpLastByte);

   void m_ShowInterfaceChangePopUp(QGraphicsItem * const opc_Item);
   void m_LoadSnapshot(const QVector<uint32_t> & orc_NodeIndices, const QVector<uint32_t> & orc_BusIndices,
                       const QVector<uint32_t> & orc_OtherStartIndices, const bool & orq_Selection, const std::vector<
                          stw::opensyde_gui_logic::C_PuiSdCompleteBusConnectionData
                          > * const opc_AdditionalConnectionData,
                       const QMap<stw::opensyde_gui_logic::C_PuiBsTemporaryDataId,
                                  uint64_t> * const opc_IdMap);
   static void mh_AddAndUpdateHigher(QVector<uint32_t> & orc_Vec, const uint32_t & oru32_New);
   void m_BusConnectorReconnectionStart(const C_GiLiBusConnector::E_ConnectState & ore_ConnectState,
                                        const QGraphicsItem * const opc_Item = NULL,
                                        const stw::opensyde_core::C_OscSystemBus::E_Type * const ope_Type = NULL,
                                        C_GiLiBusConnectorBase * const opc_BusConnector = NULL);
   void m_ShowBusConnectorReconnectionContextMenu(const QPointF & orc_ScenePos, C_GiNode * const opc_Node,
                                                  const C_GiLiBus * const opc_Bus,
                                                  const int32_t & ors32_SpecialInterface,
                                                  C_GiLiBusConnector * const opc_BusConnector);
   void m_CleanUpPorts(void) const;
   void m_RevertBusConnectorNode(stw::opensyde_gui::C_GiLiBusConnector * const opc_BusConnector,
                                 C_GiNode * const opc_StartingNode, const C_GiNode * const opc_LastNode,
                                 const QPointF & orc_ScenePos) const;
   void m_ReconnectBusConnectorNode(const C_GiLiBusConnector * const opc_BusConnector,
                                    const C_GiNode * const opc_StartingNode, const C_GiNode * const opc_LastNode,
                                    const QPointF & orc_ConnectionPos, const int32_t & ors32_Interface,
                                    const std::vector<stw::opensyde_gui_logic::C_PuiSdNodeInterfaceAutomaticProperties> & orc_Properties);
   void m_RevertBusConnectorBus(C_GiLiBusConnector * const opc_BusConnector,
                                const stw::opensyde_gui::C_GiLiBus * const opc_StartingBus,
                                const stw::opensyde_gui::C_GiLiBus * const opc_LastBus,
                                const QPointF & orc_ScenePos) const;
   void m_ReconnectBusConnectorBus(const stw::opensyde_gui::C_GiLiBusConnector * const opc_BusConnector,
                                   const stw::opensyde_gui::C_GiLiBus * const opc_StartingBus,
                                   const stw::opensyde_gui::C_GiLiBus * const opc_LastBus,
                                   const QPointF & orc_ConnectionPos, const int32_t & ors32_Interface,
                                   const std::vector<stw::opensyde_gui_logic::C_PuiSdNodeInterfaceAutomaticProperties> & orc_Properties);
   void m_RemoveConnectorLine(void);
   void m_RemoveBusNameLine(void);
   void m_RemoveTemporaryLine(C_GiLiTemporaryLine ** const opc_TemporaryLine);
   void m_RevertOverrideCursor(void);
   void m_InitNodeData(stw::opensyde_core::C_OscNode & orc_OscNode, const QString & orc_NodeType,
                       const QString & orc_MainDevice) const;
   void m_InitNodeComIfSettings(stw::opensyde_core::C_OscNode & orc_OscNode, const QString & orc_NodeType,
                                const QString & orc_MainDevice) const;

   bool m_ActivateTspShortcut (const scl::C_SclString & orc_NodeName, const uint32_t & oru32_SubNodeIndex);
   void m_AddTspForAllSubNodes(const uint32_t & oru32_SubDevicesSize, const uint32_t & oru32_OriginalOscNodeSize,
                               const scl::C_SclString & orc_NodeName);
   bool m_ShowShortcutTspOption(const QString & orc_NodeName, const uint32_t & oru32_OriginalOscNodeSize,
                                const uint32_t & oru32_SubDevicesSize);

   C_GiSvgGraphicsItem * mpc_EmptyConnectItem;
   C_GiNode * mpc_NodeConnectItem;
   const C_GiLiBus * mpc_BusConnectItem;
   QPointF mc_ConnectStartPoint;
   QPointF mc_ConnectEndPoint;
   C_GiArrowCursorButton * mpc_ArrowCursorButton;
   C_GiLiTemporaryLine * mpc_ConnectorLine;
   const C_GiLiBus * mpc_BusNameLineBus;
   C_GiLiTemporaryLine * mpc_BusNameLine;
   C_SdManTopologyContextMenuManager mc_ContextMenuManager;
   stw::opensyde_gui_logic::C_SdManTopologyCopyPasteManager mc_CopyPasteManager;
   stw::opensyde_gui_logic::C_SdManUnoTopologyManager mc_UndoManager;
   bool mq_ConnectState;
   C_GiLiBusConnector * mpc_SelectedBusConnectorItem;
   bool mq_RestoreMouseCursorWhenPossible;
   uint64_t mu64_MouseOverrideCounter;
   static const bool mhq_NEW_CONNECT_STATE;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
