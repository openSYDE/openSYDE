//-----------------------------------------------------------------------------
/*!
   \file
   \brief       Main graphics scene for system topology (header)

   \implementation
   project     openSYDE
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     10.08.2016  STW/B.Bayer
   \endimplementation
*/
//-----------------------------------------------------------------------------
#ifndef C_SDTOPOLOGYSCENE_H
#define C_SDTOPOLOGYSCENE_H

/* -- Includes ------------------------------------------------------------- */
#include <QMimeData>
#include <QMap>
#include <QTransform>

#include "stwtypes.h"

#include "C_SebTopologyBaseScene.h"
#include "C_GiNode.h"
#include "C_GiLiCANBus.h"
#include "C_GiLiEthernetBus.h"
#include "C_GiLiBusConnector.h"
#include "C_GiArrowCursorButton.h"
#include "C_GiLiTemporaryLine.h"
#include "C_SdManTopologyContextMenuManager.h"
#include "C_SdManTopologyCopyPasteManager.h"
#include "C_SdManUnoTopologyManager.h"
#include "C_PuiSdDataElement.h"
#include "C_GiSdArrow.h"
#include "C_GiSdBoundary.h"
#include "C_GiSdImageGroup.h"
#include "C_GiSdTextElement.h"
#include "C_GiTextElementBus.h"
#include "C_SdTopologyDataSnapshot.h"
#include "C_PuiBsTemporaryDataID.h"

/* -- Namespace ------------------------------------------------------------ */
namespace stw_opensyde_gui
{
/* -- Global Constants ----------------------------------------------------- */

/* -- Types ---------------------------------------------------------------- */

class C_SdTopologyScene :
   public C_SebTopologyBaseScene
{
   Q_OBJECT

public:
   explicit C_SdTopologyScene(const bool & orq_LoadSystemDefintion, QObject * const opc_Parent = NULL);
   virtual ~C_SdTopologyScene();

   void AddNode(const QString & orc_NodeType, const QPointF & orc_Pos, const stw_types::uint64 * const opu64_UniqueID);
   void AddCanBus(const QPointF & orc_Pos, const stw_types::uint64 * const opu64_UniqueID,
                  C_GiTextElementBus * const opc_TextElementBus, const QString * const opc_NameProposal = NULL);
   void AddEthernetBus(const QPointF & orc_Pos, const stw_types::uint64 * const opu64_UniqueID,
                       C_GiTextElementBus * const opc_TextElementBus, const QString * const opc_NameProposal = NULL);
   void AddBoundary(const QPointF & orc_Pos, const stw_types::uint64 * const opu64_UniqueID);
   void AddTextElement(const QString & orc_Text, const QPointF & orc_Pos,
                       const stw_types::uint64 * const opu64_UniqueID);
   C_GiTextElementBus * AddTextElementBus(const stw_types::uint64 * const opu64_UniqueID);
   void AddLine(const QPointF & orc_Pos, const stw_types::uint64 * const opu64_UniqueID);
   void AddImage(const QString & orc_FilePath, const QPointF & orc_Pos, const stw_types::uint64 * const opu64_UniqueID);
   void AddBusConnector(C_GiNode * const opc_Node, const C_GiLiBus * const opc_Bus,
                        const stw_types::uint8 & oru8_InterfaceNumber, const stw_types::uint8 & oru8_NodeId,
                        const QPointF & orc_Pos, const stw_types::uint64 * const opu64_UniqueID = NULL);

   virtual void UpdateTransform(const QTransform & orc_Transform) override;
   //lint -e{1735} Suppression, because default parameters are identical
   virtual void CopyFromManagerToScene(const QPointF * const opc_Pos = NULL) override;
   void CopyFromSnapshotToScene(const stw_opensyde_gui_logic::C_SdTopologyDataSnapshot & orc_Snapshot,
                                const QPointF * const opc_Pos = NULL,
                                const QMap<stw_opensyde_gui_logic::C_PuiBsTemporaryDataID,
                                           stw_types::uint64> * const opc_IDMap =
                                   NULL);
   void Save(void) const;
   void DeleteItem(QGraphicsItem * const opc_Item);

   void UpdateSystemDefinition(void) const;

   virtual bool IsAnyItemAddable(void) const override;
   virtual bool IsItemMovable(const QGraphicsItem * const opc_Item) const override;
   virtual bool IsItemSelectable(const QGraphicsItem * const opc_Item) const override;
   virtual bool IsItemDeletable(const QGraphicsItem * const opc_Item) const override;
   virtual bool IsZOrderChangeable(const QGraphicsItem * const opc_Item) const override;
   virtual bool IsAlignmentUsable(const QGraphicsItem * const opc_Item) const override;
   virtual bool IsSceneRubberBandAvailable(void) const override;

   //The signals keyword is necessary for Qt signal slot functionality
   //lint -save -e1736

Q_SIGNALS:
   //lint -restore
   void SigChangeMode(const stw_types::sint32 os32_Mode, const stw_types::sint32 os32_SubMode,
                      const stw_types::uint32 ou32_Index, const QString & orc_Name, const QString & orc_SubSubItemName,
                      const stw_types::uint32 ou32_Flag);
   void SigNodeDeleted(const stw_types::uint32 ou32_Index);
   void SigBusDeleted(const stw_types::uint32 ou32_Index);
   void SigErrorChange(void);

protected:
   virtual C_SebBaseContextMenuManager * m_GetContextMenuManager(void) override;
   virtual stw_opensyde_gui_logic::C_SebBaseCopyPasteManager * m_GetCopyPasteManager(void) override;
   virtual const stw_opensyde_gui_logic::C_SebBaseCopyPasteManager * m_GetCopyPasteManagerConst(void) const override;
   virtual stw_opensyde_gui_logic::C_SebUnoBaseManager * m_GetUndoManager(void) override;

   // The naming of the Qt parameters can't be changed and are not compliant with the naming conventions
   //lint -save -e1960
   virtual void mouseMoveEvent(QGraphicsSceneMouseEvent * const opc_Event) override;
   virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent * const opc_Event) override;
   virtual void mouseDoubleClickEvent(QGraphicsSceneMouseEvent * const opc_Event) override;
   virtual void keyPressEvent(QKeyEvent * const opc_KeyEvent) override;

   virtual void contextMenuEvent(QGraphicsSceneContextMenuEvent * const opc_Event) override;
   //lint -restore

   void m_OpenContextMenu(const QPointF & orc_Pos);

   // drag and drop function
   virtual void m_AddImage(const QString & orc_Path, const QPointF & orc_Position) override;
   virtual bool m_AddOfMime(const QMimeData * const opc_MimeData, const QPointF & orc_Position) override;

   // copy past functions
   virtual void m_Copy(void);
   virtual void m_Cut(void);
   virtual bool m_HandleDeleteUserConfirmation(const QList<QGraphicsItem *> & orc_SelectedItems) const override;
   //lint -e{1735} Suppression, because default parameters are identical
   virtual void m_PasteOfClipBoard(const QPointF * const opc_Pos = NULL);

   virtual C_GiNode * m_CreateNode(const stw_types::sint32 & ors32_Index, const stw_types::uint64 & oru64_ID,
                                   const stw_types::float64 & orf64_Width, const stw_types::float64 & orf64_Height,
                                   QGraphicsItem * const opc_Parent) override;
   virtual C_GiLiCANBus * m_CreateCANBus(const stw_types::sint32 & ors32_Index, const stw_types::uint64 & oru64_ID,
                                         C_GiTextElementBus * const opc_TextElementName,
                                         const std::vector<QPointF> * const opc_Points,
                                         QGraphicsItem * const opc_Parent) override;
   virtual C_GiLiEthernetBus * m_CreateEthernetBus(const stw_types::sint32 & ors32_Index,
                                                   const stw_types::uint64 & oru64_ID,
                                                   C_GiTextElementBus * const opc_TextElementName,
                                                   const std::vector<QPointF> * const opc_Points,
                                                   QGraphicsItem * const opc_Parent) override;
   virtual C_GiTextElementBus * m_CreateBusTextElement(const stw_types::sint32 & ors32_Index,
                                                       const stw_types::uint64 & oru64_ID,
                                                       QGraphicsItem * const opc_Parent) override;

   virtual void m_AddNodeToScene(C_GiNode * const opc_NodeGraphicsItem) override;
   virtual void m_AddBusConnectorToScene(C_GiLiBusConnector * const opc_BusConnectorGraphicsItem) override;
   virtual void m_AddTextElementToScene(C_GiBiTextElement * const opc_Item) override;
   //Move
   virtual void m_HandleRevertableResizeLine(const stw_types::sint32 & ors32_InteractionPointIndex,
                                             const QPointF & orc_PositionDifference);
   //Setup style
   virtual bool m_CallSetupStyle(QGraphicsItem * const opc_Item) const;

private:
   //Avoid call
   C_SdTopologyScene(const C_SdTopologyScene &);
   C_SdTopologyScene & operator =(const C_SdTopologyScene &);

   void m_SelectionChanged(void);
   void m_HandleArrowButton(const bool & orq_Selection);
   void m_StartConnector(const QPointF & orc_LineStart, const QPointF & orc_SceneTriggerPos);

   void m_EditPressed(void);
   void m_EditClicked(const QGraphicsItem * const opc_Item);
   void m_Edit(const QGraphicsItem * const opc_Item, const bool oq_FocusName = false);

   void m_SyncIndex(const stw_opensyde_gui_logic::C_PuiSdDataElement::E_Type & ore_Type,
                    const stw_types::sint32 & ors32_Index,
                    const stw_opensyde_gui_logic::C_PuiSdDataElement::E_Action & ore_Action) const;
   void m_ConnectNodeToBus(const stw_types::uint8 &  oru8_InterfaceNumber, const stw_types::uint8 & oru8_NodeId);
   void m_ChangeInterface(const stw_types::uint8 & oru8_InterfaceNumber, const stw_types::uint8 & oru8_NodeId,
                          C_GiLiBusConnector * const opc_Connector);
   void m_RestoreToolTips(void) const;
   void m_RemoveNodeOfScene(C_GiNode * const opc_NodeGraphicsItem);
   void m_RemoveBusOfScene(C_GiLiBus * const opc_BusGraphicsItem);
   void m_RemoveBusConnectorOfScene(C_GiLiBusConnector * const opc_BusConnectorGraphicsItem);
   void m_RemoveLineArrowOfScene(stw_opensyde_gui::C_GiSdArrow * const opc_Item);
   void m_RemoveBoundaryOfScene(stw_opensyde_gui::C_GiSdBoundary * const opc_Item);
   void m_RemoveImageGroupOfScene(stw_opensyde_gui::C_GiSdImageGroup * const opc_Item);
   void m_RemoveTextElementOfScene(C_GiSdTextElement * const opc_Item);
   void m_RemoveTextElementBusOfScene(C_GiTextElementBus * const opc_Item);
   void m_EnterConnectState(const C_GiLiBusConnector::E_ConnectState & ore_ConnectState,
                            const C_GiNode * const opc_Node = NULL,
                            const stw_opensyde_core::C_OSCSystemBus::E_Type * const ope_Type = NULL);
   void m_LeaveConnectState(void);
   void m_RemoveConnectorAndLeaveConnectState(void);
   void m_DisableEverythingForConnectState(const C_GiNode * const opc_Node) const;
   void m_ShowNewConnectionPopUp(const C_GiNode * const opc_Node, const C_GiLiBus * const opc_Bus,
                                 const bool & orq_ChangeInterface, const bool & orq_Reconnect,
                                 const stw_types::sint32 & ors32_SpecialInterface,
                                 C_GiLiBusConnector * const opc_Connector);
   void m_ShowNewNodeToNodeConnectionPopUp(const C_GiNode * const opc_Node1, const C_GiNode * const opc_Node2);
   void m_ConfiugreComDatapools(const C_GiNode * const opc_Node, const stw_types::uint32 ou32_InterfaceIndex,
                                const bool oq_ComProtocolL2, const bool oq_ComProtocolECeS,
                                const bool oq_ComProtocolECoS) const;
   void m_ConfiugreComDatapool(const C_GiNode * const opc_Node, const stw_types::uint32 ou32_InterfaceIndex,
                               const stw_opensyde_core::C_OSCCanProtocol::E_Type oe_ProtocolType,
                               const bool oq_Active) const;
   void m_ShowInterfaceChangePopUp(QGraphicsItem * const opc_Item);
   void m_LoadSnapshot(const QVector<stw_types::uint32> & orc_NodeIndices,
                       const QVector<stw_types::uint32> & orc_BusIndices,
                       const QVector<stw_types::uint32> & orc_OtherStartIndices, const QPointF * const opc_Offset,
                       const bool & orq_Selection, const std::vector<
                          stw_opensyde_gui_logic::C_PuiSdCompleteBusConnectionData
                          > * const opc_AdditionalConnectionData,
                       const QMap<stw_opensyde_gui_logic::C_PuiBsTemporaryDataID, stw_types::uint64> * const opc_IDMap);
   static void mh_AddAndUpdateHigher(QVector<stw_types::uint32> & orc_Vec, const stw_types::uint32 & oru32_New);
   void m_BusConnectorReconnectionStart(const C_GiLiBusConnector::E_ConnectState & ore_ConnectState,
                                        const QGraphicsItem * const opc_Item = NULL,
                                        const stw_opensyde_core::C_OSCSystemBus::E_Type * const ope_Type = NULL,
                                        C_GiLiBusConnectorBase * const opc_BusConnector = NULL);
   void m_ShowBusConnectorReconnectionContextMenu(const QPointF & orc_ScenePos, C_GiNode * const opc_Node,
                                                  const C_GiLiBus * const opc_Bus,
                                                  const stw_types::sint32 & ors32_SpecialInterface,
                                                  C_GiLiBusConnector * const opc_BusConnector);
   void m_CleanUpPorts(void) const;
   void m_RevertBusConnectorNode(stw_opensyde_gui::C_GiLiBusConnector * const opc_BusConnector,
                                 C_GiNode * const opc_StartingNode, C_GiNode * const opc_LastNode,
                                 const QPointF & orc_ScenePos) const;
   void m_ReconnectBusConnectorNode(const C_GiLiBusConnector * const opc_BusConnector,
                                    const C_GiNode * const opc_StartingNode, const C_GiNode * const opc_LastNode,
                                    const QPointF & orc_ConnectionPos, const stw_types::sint32 & ors32_Interface,
                                    const stw_types::uint8 & oru8_NodeId);
   void m_RevertBusConnectorBus(C_GiLiBusConnector * const opc_BusConnector,
                                const stw_opensyde_gui::C_GiLiBus * const opc_StartingBus,
                                const stw_opensyde_gui::C_GiLiBus * const opc_LastBus,
                                const QPointF & orc_ScenePos) const;
   void m_ReconnectBusConnectorBus(const stw_opensyde_gui::C_GiLiBusConnector * const opc_BusConnector,
                                   const stw_opensyde_gui::C_GiLiBus * const opc_StartingBus,
                                   const stw_opensyde_gui::C_GiLiBus * const opc_LastBus,
                                   const QPointF & orc_ConnectionPos, const stw_types::sint32 & ors32_Interface,
                                   const stw_types::uint8 & oru8_NodeId);
   void m_RemoveConnectorLine(void);
   void m_RemoveBusNameLine(void);
   void m_RemoveTemporaryLine(C_GiLiTemporaryLine ** const opc_TemporaryLine);
   void m_RevertOverrideCursor(void);
   void m_InitNodeComIfSettings(stw_opensyde_core::C_OSCNode & orc_OSCNode) const;

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
   stw_opensyde_gui_logic::C_SdManTopologyCopyPasteManager mc_CopyPasteManager;
   stw_opensyde_gui_logic::C_SdManUnoTopologyManager mc_UndoManager;
   bool mq_ConnectState;
   C_GiLiBusConnector * mpc_SelectedBusConnectorItem;
   bool mq_RestoreMouseCursorWhenPossible;
   stw_types::uint64 mu64_MouseOverrideCounter;
   static const bool mhq_NewConnectState;
};

/* -- Extern Global Variables ---------------------------------------------- */
} //end of namespace

#endif
