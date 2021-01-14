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

#include "stwtypes.h"
#include "C_SdBueUnoManager.h"
#include "C_OgeTreeWidgetToolTipBase.h"
#include "C_PuiSdNodeCanMessageSyncManager.h"
#include "C_OSCCanMessageIdentificationIndices.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw_opensyde_gui
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_SdBueMessageSelectorTreeWidget :
   public stw_opensyde_gui_elements::C_OgeTreeWidgetToolTipBase
{
   Q_OBJECT

public:
   explicit C_SdBueMessageSelectorTreeWidget(QWidget * const opc_Parent = NULL);
   virtual ~C_SdBueMessageSelectorTreeWidget(void);

   virtual QSize sizeHint(void) const override;

   void SetNodeId(const stw_types::uint32 ou32_NodeIndex, const stw_types::uint32 ou32_InterfaceIndex,
                  const std::vector<stw_types::uint32> & orc_DatapoolIndexes);
   void SetBusId(const stw_types::uint32 ou32_BusIndex);
   void SetProtocolType(const stw_opensyde_core::C_OSCCanProtocol::E_Type & ore_Value);
   void DeselectAllItems(void);
   void SetUndoManager(stw_opensyde_gui_logic::C_SdBueUnoManager * const opc_Value);
   void SetMessageSyncManager(stw_opensyde_gui_logic::C_PuiSdNodeCanMessageSyncManager * const opc_Value);
   void InitFromData(void);
   void Add(void);
   void AddMessage(void);
   void AddSignal(void);
   void AddSignalWithStartBit(const stw_opensyde_core::C_OSCCanMessageIdentificationIndices & orc_MessageId,
                              const stw_types::uint16 ou16_StartBit, const bool oq_MultiplexedSignal,
                              const stw_types::uint16 ou16_MultiplexValue);
   void Delete(void);
   void DeleteSignal(const stw_opensyde_core::C_OSCCanMessageIdentificationIndices & orc_MessageId,
                     const stw_types::uint32 ou32_SignalIndex);
   void Copy(void);
   void CopySignal(const stw_opensyde_core::C_OSCCanMessageIdentificationIndices & orc_MessageId,
                   const stw_types::uint32 ou32_SignalIndex) const;
   void Cut(void);
   void CutSignal(const stw_opensyde_core::C_OSCCanMessageIdentificationIndices & orc_MessageId,
                  const stw_types::uint32 ou32_SignalIndex);
   void Paste(void);
   void PasteSignal(const stw_opensyde_core::C_OSCCanMessageIdentificationIndices & orc_MessageId,
                    const stw_types::uint16 ou16_StartBit);
   void ExpandAll(void);
   void CollapseAll(void);
   void EditName(void);
   void InternalAddMessage(const stw_opensyde_core::C_OSCCanMessageIdentificationIndices & orc_MessageId);
   void InternalAddMessageCommit(void);
   stw_types::uint32 InternalDeleteMessage(
      const stw_opensyde_core::C_OSCCanMessageIdentificationIndices & orc_MessageId);
   void InternalDeleteMessageCommit(const stw_types::uint32 ou32_LastInternalMessageIndex);
   void InternalAddSignal(const stw_opensyde_core::C_OSCCanMessageIdentificationIndices & orc_MessageId,
                          const stw_types::uint32 & oru32_SignalIndex);
   void InternalDeleteSignal(const stw_opensyde_core::C_OSCCanMessageIdentificationIndices & orc_MessageId,
                             const stw_types::uint32 & oru32_SignalIndex);
   void OnMessageIdChange(void);
   void OnMessageNameChange(void);
   void OnSignalNameChange(const stw_opensyde_core::C_OSCCanMessageIdentificationIndices & orc_MessageId);
   void OnNodeDisconnected(const stw_types::uint32 ou32_NodeIndex, const stw_types::uint32 ou32_InterfaceIndex);
   void RecheckErrorGlobal(const bool & orq_HandleSelection = true);
   void RecheckError(const stw_opensyde_core::C_OSCCanMessageIdentificationIndices & orc_MessageId,
                     const bool & orq_AllowMessageIdUpdate = true);
   bool CheckIfAnyNodeConnected(void) const;
   void SelectMessage(const stw_opensyde_core::C_OSCCanMessageIdentificationIndices & orc_MessageId,
                      const bool oq_BlockSignal = true);
   void SelectMessages(const std::vector<stw_opensyde_core::C_OSCCanMessageIdentificationIndices> & orc_MessageIds);
   void SelectSignal(const stw_opensyde_core::C_OSCCanMessageIdentificationIndices & orc_MessageId,
                     const stw_types::uint32 & oru32_SignalIndex, const bool oq_BlockSignal = true);
   stw_types::sint32 GetLevelOfPos(const QPoint & orc_Pos) const;

   //The signals keyword is necessary for Qt signal slot functionality
   //lint -save -e1736
Q_SIGNALS:
   //lint -restore
   void SigSelectionChanged(void);
   void SigMessageSelected(const stw_opensyde_core::C_OSCCanMessageIdentificationIndices & orc_MessageId);
   void SigSignalSelected(const stw_opensyde_core::C_OSCCanMessageIdentificationIndices & orc_MessageId,
                          const stw_types::uint32 & oru32_SignalIndex);
   void SigSignalCountOfMessageChanged(const stw_opensyde_core::C_OSCCanMessageIdentificationIndices & orc_MessageId);
   void SigMessageCountChanged(void);
   void SigSelectName(void);
   void SigZeroMessages(void);
   void SigErrorChanged(void);
   void SigReload(void);

protected:
   virtual void paintEvent(QPaintEvent * const opc_Event) override;
   virtual void dropEvent(QDropEvent * const opc_Event) override;
   virtual void startDrag(const Qt::DropActions oc_SupportedActions) override;
   virtual QMimeData * mimeData(const QList<QTreeWidgetItem *> oc_Items) const override;
   virtual QStringList mimeTypes(void) const override;

   virtual void m_LastMinuteToolTipUpdate(void) override;

private:
   //Avoid call
   C_SdBueMessageSelectorTreeWidget(const C_SdBueMessageSelectorTreeWidget &);
   C_SdBueMessageSelectorTreeWidget & operator =(const C_SdBueMessageSelectorTreeWidget &);

   void m_ReloadTree(const bool & orq_HandleSelection = true);
   void m_InsertMessage(const stw_types::uint32 & oru32_MessageIdIndex);
   void m_InsertSignal(QTreeWidgetItem * const opc_MessageItem, const stw_types::uint32 ou32_SignalIndex,
                       const QString & orc_SignalName) const;

   void m_SelectionChanged(const QItemSelection & orc_Selected, const QItemSelection & orc_Deselected);
   void m_DeselectChildren(const QTreeWidgetItem * const opc_Parent) const;
   void m_TreeSizeChanged(void);
   void m_ScrollBarRangeChanged(const stw_types::sintn osn_Min, const stw_types::sintn osn_Max) const;
   stw_types::sint32 m_GetFirstConnectedNodeAndInterface(stw_types::uint32 & oru32_NodeIndex,
                                                         stw_types::uint32 & oru32_InterfaceIndex,
                                                         stw_types::uint32 & oru32_DatapoolIndex);
   stw_types::sint32 m_MapMessageIdToInternalMessageIndex(
      const stw_opensyde_core::C_OSCCanMessageIdentificationIndices & orc_MessageId,
      stw_types::uint32 & oru32_InternalMessageIndex) const;
   stw_types::sint32 m_GetMessageIdForAdd(stw_opensyde_core::C_OSCCanMessageIdentificationIndices & orc_MessageId);
   void m_UpdateUniqueMessageIds(void);
   void m_UpdateUniqueMessageIdsSignals(const stw_types::uint32 & oru32_InternalMessageIndex);
   void m_DisconnectSelection(void);
   void m_ReconnectSelection(void);
   void m_SendMessageSelectionUpdate(const stw_opensyde_core::C_OSCCanMessageIdentificationIndices & orc_MessageId);
   void m_SendSignalSelectionUpdate(const stw_opensyde_core::C_OSCCanMessageIdentificationIndices & orc_MessageId,
                                    const stw_types::uint32 & oru32_SignalIndex);
   void m_SaveSelection(void);
   void m_RestoreSelection(const bool oq_AlsoSetCurrent, const bool oq_SendSignalForLastSelected);
   void m_SaveExpand(void);
   void m_RestoreExpand(void);
   stw_types::sint32 m_MapSignalInternalIndexToDataIndex(const stw_types::uint32 & oru32_InternalMessageIndex,
                                                         const stw_types::uint32 & oru32_InternalSignalIndex,
                                                         stw_types::uint32 & oru32_SignalDataIndex) const;
   stw_types::sint32 m_MapSignalDataIndexToInternalIndex(const stw_types::uint32 & oru32_InternalMessageIndex,
                                                         const stw_types::uint32 & oru32_DataSignalIndex,
                                                         stw_types::uint32 & oru32_SignalInternalIndex) const;
   static stw_types::uint16 mh_GetStartBit(
      const stw_opensyde_core::C_OSCCanMessageIdentificationIndices & orc_MessageId);
   static stw_types::uint16 mh_GetStartBit(const stw_opensyde_core::C_OSCCanMessage * const opc_Message,
                                           const stw_types::uint16 ou16_SignalSize,
                                           const stw_opensyde_core::C_OSCCanSignal::E_ByteOrderType oe_Type);
   bool m_GetHighestSelected(QModelIndex & orc_Index) const;

   stw_opensyde_gui_logic::C_SdBueUnoManager * mpc_UndoManager;
   stw_opensyde_gui_logic::C_PuiSdNodeCanMessageSyncManager * mpc_MessageSyncManager;
   std::vector<stw_opensyde_core::C_OSCCanMessageIdentificationIndices> mc_UniqueMessageIds;
   std::vector<std::vector<stw_types::uint32> > mc_UniqueMessageIdsSignalsOrder;
   std::vector<stw_opensyde_core::C_OSCCanMessageIdentificationIndices> mc_SelectedMessageIds;
   std::vector<stw_opensyde_core::C_OSCCanMessageIdentificationIndices> mc_ExpandedMessageIds;
   std::vector<std::vector<stw_types::uint32> > mc_SelectedSignals;
   bool mq_StopSigSelectionChanged;
   bool mq_ModeSingleNode;
   stw_types::uint32 mu32_NodeIndex;                  // Used by node mode
   stw_types::uint32 mu32_InterfaceIndex;             // Used by node mode
   std::vector<stw_types::uint32> mc_DatapoolIndexes; // Used by node mode
   stw_types::uint32 mu32_BusIndex;                   // Used by bus mode
   stw_opensyde_core::C_OSCCanProtocol::E_Type me_ProtocolType;
   bool mq_NoSelectionUpdate;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif
