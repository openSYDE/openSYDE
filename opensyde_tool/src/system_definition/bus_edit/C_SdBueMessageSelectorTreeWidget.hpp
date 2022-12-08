//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Tree widget for bus messages (header)

   See cpp file for detailed description

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_SDBUEMESSAGESELECTORTREEWIDGET_H
#define C_SDBUEMESSAGESELECTORTREEWIDGET_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <QItemSelection>

#include "stwtypes.hpp"
#include "C_SdBueUnoManager.hpp"
#include "C_OgeTreeWidgetToolTipBase.hpp"
#include "C_PuiSdNodeCanMessageSyncManager.hpp"
#include "C_OscCanMessageIdentificationIndices.hpp"
#include "C_SdBusMessageSelectorTreeWidgetItem.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_gui
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_SdBueMessageSelectorTreeWidget :
   public stw::opensyde_gui_elements::C_OgeTreeWidgetToolTipBase
{
   Q_OBJECT

public:
   explicit C_SdBueMessageSelectorTreeWidget(QWidget * const opc_Parent = NULL);
   virtual ~C_SdBueMessageSelectorTreeWidget(void);

   QSize sizeHint(void) const override;

   void SetNodeId(const uint32_t ou32_NodeIndex, const uint32_t ou32_InterfaceIndex,
                  const std::vector<uint32_t> & orc_DatapoolIndexes);
   void SetBusId(const uint32_t ou32_BusIndex);
   void SetProtocolType(const stw::opensyde_core::C_OscCanProtocol::E_Type & ore_Value);
   void DeselectAllItems(void);
   void SetUndoManager(stw::opensyde_gui_logic::C_SdBueUnoManager * const opc_Value);
   void SetMessageSyncManager(stw::opensyde_gui_logic::C_PuiSdNodeCanMessageSyncManager * const opc_Value);
   void InitFromData(void);
   void Add(void);
   void AddMessage(void);
   void AddSignal(void);
   void AddSignalWithStartBit(const stw::opensyde_core::C_OscCanMessageIdentificationIndices & orc_MessageId,
                              const uint16_t ou16_StartBit, const bool oq_MultiplexedSignal,
                              const uint16_t ou16_MultiplexValue);
   void Delete(void);
   void DeleteSignal(const stw::opensyde_core::C_OscCanMessageIdentificationIndices & orc_MessageId,
                     const uint32_t ou32_SignalIndex);
   void Copy(void);
   void CopySignal(const stw::opensyde_core::C_OscCanMessageIdentificationIndices & orc_MessageId,
                   const uint32_t ou32_SignalIndex) const;
   void Cut(void);
   void CutSignal(const stw::opensyde_core::C_OscCanMessageIdentificationIndices & orc_MessageId,
                  const uint32_t ou32_SignalIndex);
   void Paste(void);
   void PasteSignal(const stw::opensyde_core::C_OscCanMessageIdentificationIndices & orc_MessageId,
                    const uint16_t ou16_StartBit);
   void ExpandAll(void);
   void CollapseAll(void);
   void EditName(void);
   void InternalAddMessage(const stw::opensyde_core::C_OscCanMessageIdentificationIndices & orc_MessageId);
   void InternalAddMessageCommit(void);
   uint32_t InternalDeleteMessage(const stw::opensyde_core::C_OscCanMessageIdentificationIndices & orc_MessageId);
   void InternalDeleteMessageCommit(const uint32_t ou32_LastInternalMessageIndex);
   void InternalAddSignal(const stw::opensyde_core::C_OscCanMessageIdentificationIndices & orc_MessageId,
                          const uint32_t & oru32_SignalIndex);
   void InternalDeleteSignal(const stw::opensyde_core::C_OscCanMessageIdentificationIndices & orc_MessageId,
                             const uint32_t & oru32_SignalIndex);
   void OnMessageIdChange(void);
   void OnMessageNameChange(void);
   void OnSignalNameChange(const stw::opensyde_core::C_OscCanMessageIdentificationIndices & orc_MessageId);
   void OnSignalPositionChange(const stw::opensyde_core::C_OscCanMessageIdentificationIndices & orc_MessageId);
   void OnNodeDisconnected(const uint32_t ou32_NodeIndex, const uint32_t ou32_InterfaceIndex);
   void RecheckErrorGlobal(const bool & orq_HandleSelection = true);
   void RecheckError(const stw::opensyde_core::C_OscCanMessageIdentificationIndices & orc_MessageId,
                     const bool & orq_AllowMessageIdUpdate = true);
   bool CheckIfAnyNodeConnected(void) const;
   void SelectMessage(const stw::opensyde_core::C_OscCanMessageIdentificationIndices & orc_MessageId,
                      const bool oq_BlockSignal = true);
   void SelectMessages(const std::vector<stw::opensyde_core::C_OscCanMessageIdentificationIndices> & orc_MessageIds);
   void SelectSignal(const stw::opensyde_core::C_OscCanMessageIdentificationIndices & orc_MessageId,
                     const uint32_t & oru32_SignalIndex, const bool oq_BlockSignal = true);
   int32_t GetLevelOfPos(const QPoint & orc_Pos) const;
   bool IsSelectedMessageContentReadOnly(void) const;

   //The signals keyword is necessary for Qt signal slot functionality
   //lint -save -e1736
Q_SIGNALS:
   //lint -restore
   void SigSelectionChanged(void);
   void SigMessageSelected(const stw::opensyde_core::C_OscCanMessageIdentificationIndices & orc_MessageId);
   void SigSignalSelected(const stw::opensyde_core::C_OscCanMessageIdentificationIndices & orc_MessageId,
                          const uint32_t & oru32_SignalIndex);
   void SigSignalCountOfMessageChanged(const stw::opensyde_core::C_OscCanMessageIdentificationIndices & orc_MessageId);
   void SigMessageCountChanged(void);
   void SigSelectName(void);
   void SigRefreshSelection(void);
   void SigZeroMessages(void);
   void SigErrorChanged(void);
   void SigReload(void);

protected:
   void paintEvent(QPaintEvent * const opc_Event) override;
   void dropEvent(QDropEvent * const opc_Event) override;
   void startDrag(const Qt::DropActions oc_SupportedActions) override;
   QMimeData * mimeData(const QList<QTreeWidgetItem *> oc_Items) const override;
   QStringList mimeTypes(void) const override;

   void m_LastMinuteToolTipUpdate(void) override;

private:
   //Avoid call
   C_SdBueMessageSelectorTreeWidget(const C_SdBueMessageSelectorTreeWidget &);
   C_SdBueMessageSelectorTreeWidget & operator =(const C_SdBueMessageSelectorTreeWidget &) &;

   void m_ReloadTree(const bool & orq_HandleSelection = true);
   void m_AddSignal(const uint32_t ou32_MessageIndex);
   void m_AddCoSignal(const stw::opensyde_core::C_OscCanMessageIdentificationIndices & orc_MessageId,
                      const uint32_t ou32_SignalIndex, const uint16_t ou16_StartBit);
   void m_AutoAdaptCoDlc(const stw::opensyde_core::C_OscCanMessageIdentificationIndices & orc_MessageId);
   void m_InsertMessage(const uint32_t & oru32_MessageIdIndex);
   void m_InsertSignal(QTreeWidgetItem * const opc_MessageItem, const uint32_t ou32_SignalIndex,
                       const QString & orc_SignalName) const;

   void m_SelectionChanged(const QItemSelection & orc_Selected, const QItemSelection & orc_Deselected);
   void m_CoMessageCheckedStateChanged(C_SdBusMessageSelectorTreeWidgetItem * const opc_Item);
   void m_DeselectChildren(const QTreeWidgetItem * const opc_Parent) const;
   void m_TreeSizeChanged(void);
   void m_ScrollBarRangeChanged(const int32_t os32_Min, const int32_t os32_Max) const;
   int32_t m_GetFirstConnectedNodeAndInterface(uint32_t & oru32_NodeIndex, uint32_t & oru32_InterfaceIndex,
                                               uint32_t & oru32_DatapoolIndex);
   int32_t m_MapMessageIdToInternalMessageIndex(
      const stw::opensyde_core::C_OscCanMessageIdentificationIndices & orc_MessageId,
      uint32_t & oru32_InternalMessageIndex) const;
   int32_t m_GetMessageIdForAdd(stw::opensyde_core::C_OscCanMessageIdentificationIndices & orc_MessageId);
   void m_UpdateUniqueMessageIds(void);
   void m_UpdateUniqueMessageIdsSignals(const uint32_t & oru32_InternalMessageIndex);
   void m_CoLoadEdsRestricitions(void);
   void m_DisconnectSelection(void);
   void m_ReconnectSelection(void);
   void m_SendMessageSelectionUpdate(const stw::opensyde_core::C_OscCanMessageIdentificationIndices & orc_MessageId);
   void m_SendSignalSelectionUpdate(const stw::opensyde_core::C_OscCanMessageIdentificationIndices & orc_MessageId,
                                    const uint32_t & oru32_SignalIndex);
   void m_SaveSelection(void);
   void m_RestoreSelection(const bool oq_AlsoSetCurrent, const bool oq_SendSignalForLastSelected);
   void m_SaveExpand(void);
   void m_RestoreExpand(void);
   int32_t m_MapSignalInternalIndexToDataIndex(const uint32_t & oru32_InternalMessageIndex,
                                               const uint32_t & oru32_InternalSignalIndex,
                                               uint32_t & oru32_SignalDataIndex) const;
   int32_t m_MapSignalDataIndexToInternalIndex(const uint32_t & oru32_InternalMessageIndex,
                                               const uint32_t & oru32_DataSignalIndex,
                                               uint32_t & oru32_SignalInternalIndex) const;
   static uint16_t mh_GetStartBit(const stw::opensyde_core::C_OscCanMessageIdentificationIndices & orc_MessageId);
   static uint16_t mh_GetStartBit(const stw::opensyde_core::C_OscCanMessage * const opc_Message,
                                  const uint16_t ou16_SignalSize,
                                  const stw::opensyde_core::C_OscCanSignal::E_ByteOrderType oe_Type);
   bool m_GetHighestSelected(QModelIndex & orc_Index) const;

   stw::opensyde_gui_logic::C_SdBueUnoManager * mpc_UndoManager;
   stw::opensyde_gui_logic::C_PuiSdNodeCanMessageSyncManager * mpc_MessageSyncManager;
   std::vector<stw::opensyde_core::C_OscCanMessageIdentificationIndices> mc_UniqueMessageIds;
   std::vector<uint8_t> mc_CoUniqueMessagesPdoMappingRo;
   std::vector<std::vector<uint32_t> > mc_UniqueMessageIdsSignalsOrder;
   std::vector<stw::opensyde_core::C_OscCanMessageIdentificationIndices> mc_SelectedMessageIds;
   std::vector<stw::opensyde_core::C_OscCanMessageIdentificationIndices> mc_ExpandedMessageIds;
   std::vector<std::vector<uint32_t> > mc_SelectedSignals;
   bool mq_StopSigSelectionChanged;
   bool mq_ModeSingleNode;
   uint32_t mu32_NodeIndex;                  // Used by node mode
   uint32_t mu32_InterfaceIndex;             // Used by node mode
   std::vector<uint32_t> mc_DatapoolIndexes; // Used by node mode
   uint32_t mu32_BusIndex;                   // Used by bus mode
   stw::opensyde_core::C_OscCanProtocol::E_Type me_ProtocolType;
   bool mq_NoSelectionUpdate;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
