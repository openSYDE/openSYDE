//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Graphics scene for the message layout viewer (header)

   See cpp file for detailed description

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_SDBUEMLVGRAPHICSSCENE_HPP
#define C_SDBUEMLVGRAPHICSSCENE_HPP

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <QGraphicsScene>
#include <QVector>
#include <set>
#include <vector>
#include <array>
#include <QCursor>
#include <QObject>
#include <QAction>

#include "C_SdBueMlvBorderItem.hpp"
#include "C_SdBueMlvEmptyItem.hpp"
#include "C_SdBueMlvSignalManager.hpp"
#include "C_OscCanMessageIdentificationIndices.hpp"
#include "C_OscCanProtocol.hpp"
#include "C_PuiSdNodeCanMessageSyncManager.hpp"
#include "C_OgeContextMenu.hpp"
#include "C_GiBiCustomToolTip.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_gui
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_SdBueMlvGraphicsScene :
   public QGraphicsScene
{
   Q_OBJECT

public:
   C_SdBueMlvGraphicsScene(QObject * const opc_Parent = NULL);
   ~C_SdBueMlvGraphicsScene(void) override;

   void SetMessageSyncManager(stw::opensyde_gui_logic::C_PuiSdNodeCanMessageSyncManager * const opc_Value);
   void SetComProtocol(const stw::opensyde_core::C_OscCanProtocol::E_Type & ore_Value);
   void SetMessage(const stw::opensyde_core::C_OscCanMessageIdentificationIndices & orc_MessageId,
                   const bool oq_MultiplexedMessage, const uint16_t ou16_MultiplexValue);
   void SetMultiplexValue(const uint16_t ou16_MultiplexValue);
   void SetSignal(const uint32_t ou32_SignalIndex);
   void Clear(void);
   void DisplayToolTip(const QPointF & orc_ScenePos);
   void RefreshColors(void);

   //The signals keyword is necessary for Qt signal slot functionality
   //lint -save -e1736

Q_SIGNALS:
   //lint -restore
   void SigSignalActivated(const uint32_t ou32_SignalIndex);
   void SigMessageUpdated(void);
   void SigChangeCursor(const Qt::CursorShape oe_Cursor);
   void SigShowToolTip(const QPointF & orc_ScenePos, const QString & orc_Heading, const QString & orc_Content,
                       const bool oq_Error);
   void SigHideToolTip(void);

   void SigAddSignal(const stw::opensyde_core::C_OscCanMessageIdentificationIndices & orc_MessageId,
                     const uint16_t ou16_StartBit);
   void SigAddSignalMultiplexed(const stw::opensyde_core::C_OscCanMessageIdentificationIndices & orc_MessageId,
                                const uint16_t ou16_StartBit, const uint16_t ou16_MultiplexValue);
   void SigCopySignal(const stw::opensyde_core::C_OscCanMessageIdentificationIndices & orc_MessageId,
                      const uint32_t ou32_SignalIndex);
   void SigCutSignal(const stw::opensyde_core::C_OscCanMessageIdentificationIndices & orc_MessageId,
                     const uint32_t ou32_SignalIndex);
   void SigPasteSignal(const stw::opensyde_core::C_OscCanMessageIdentificationIndices & orc_MessageId,
                       const uint16_t ou16_StartBit);
   void SigDeleteSignal(const stw::opensyde_core::C_OscCanMessageIdentificationIndices & orc_MessageId,
                        const uint32_t ou32_SignalIndex);

protected:
   void mousePressEvent(QGraphicsSceneMouseEvent * const opc_Event) override;
   void mouseMoveEvent(QGraphicsSceneMouseEvent * const opc_Event) override;
   void mouseReleaseEvent(QGraphicsSceneMouseEvent * const opc_Event) override;
   void contextMenuEvent(QGraphicsSceneContextMenuEvent * const opc_Event) override;

private:
   //Avoid call
   C_SdBueMlvGraphicsScene(const C_SdBueMlvGraphicsScene &);
   C_SdBueMlvGraphicsScene & operator =(const C_SdBueMlvGraphicsScene &) &;

   void m_InitBorderItems(void);
   void m_InitEmptyItems(void);
   void m_InitProtocolItems(void);
   void m_CoLoadEdsRestricitions(void);
   void m_AddSignal(const uint32_t ou32_SignalIndex);
   void m_RemoveSignal(const uint32_t ou32_SignalIndex);

   void m_UpdateAll(void);
   void m_UpdateBorderItems(void);
   void m_UpdateEmptyItems(void);
   void m_UpdateSignalManager(void);
   void m_UpdateConcreteSignalManager(C_SdBueMlvSignalManager * const opc_Item);
   void m_UpdateProtocolItems(void);

   void m_AddSignalToGridMapping(C_SdBueMlvSignalManager * const opc_Item);
   void m_UpdateSignalInGridMapping(C_SdBueMlvSignalManager * const opc_Item);
   void m_RemoveSignalFromGridMapping(C_SdBueMlvSignalManager * const opc_Item);
   void m_RemoveSignalFromGridMappingPosition(C_SdBueMlvSignalManager * const opc_Item, const uint16_t ou16_Pos);
   void m_CheckGridMappingPositionForError(const uint16_t ou16_Pos);

   void m_AddItemSlot(C_SdBueMlvSignalItem * const opc_Item);
   void m_RemoveItemSlot(C_SdBueMlvSignalItem * const opc_Item);

   C_SdBueMlvSignalManager::C_SignalItemColors m_GetNextNotUsedColors(void);
   C_SdBueMlvSignalManager::C_SignalItemColors m_GetConcreteColors(const uint8_t ou8_Index);
   void m_SetColorsUnused(const C_SdBueMlvSignalManager::C_SignalItemColors & orc_Colors);
   void m_PrepareNextColorSection(void);

   void m_SceneRectChanged(void);
   void m_SearchClickedItem(const QPointF & orc_Pos);
   void m_BringActualSignalToTop(void);
   void m_ChangeCursor(const Qt::CursorShape oe_Cursor);
   int32_t m_GetGridIndex(const QPointF & orc_Pos) const;

   void m_HandleHideToolTip(void);

   // Context menu
   void m_SetupContextMenu(void);
   void m_OnCustomContextMenuRequested(const QGraphicsSceneContextMenuEvent * const opc_Event);

   void m_ActionAdd(void);
   void m_ActionAddMultiplexed(void);
   void m_ActionCopy(void);
   void m_ActionPaste(void);
   void m_ActionCut(void);
   void m_ActionDelete(void);

   stw::opensyde_gui_elements::C_OgeContextMenu * mpc_ContextMenu;
   QAction * mpc_Add;
   QAction * mpc_AddMultiplexed;
   QAction * mpc_Copy;
   QAction * mpc_Cut;
   QAction * mpc_Paste;
   QAction * mpc_Delete;

   stw::opensyde_gui_logic::C_PuiSdNodeCanMessageSyncManager * mpc_MessageSyncManager;
   stw::opensyde_core::C_OscCanProtocol::E_Type me_Protocol;

   // identification information about the message
   stw::opensyde_core::C_OscCanMessageIdentificationIndices mc_MessageId;
   bool mq_MultiplexedMessage;
   uint16_t mu16_MultiplexerValue;
   uint16_t mu16_MaximumCountBits;

   float64_t mf64_SingleItemWidth;
   float64_t mf64_SingleItemHeight;

   // Border items
   C_SdBueMlvBorderItem * mpc_BorderItemUpperLeft;
   // all vertical items without item in the upper left corner
   QVector<C_SdBueMlvBorderItem *> mc_VecBorderItemsVertical;
   // all horizontal items without item in the upper left corner
   QVector<C_SdBueMlvBorderItem *> mc_VecBorderItemsHorizontal;

   // All empty items
   QVector<C_SdBueMlvEmptyItem *> mc_VecEmptyItems;

   // All signals
   QVector<C_SdBueMlvSignalManager *> mc_VecSignals;

   // Tooltip information
   C_SdBueMlvSignalManager * mpc_HoveredSignal;
   // Signal interaction information
   C_SdBueMlvSignalManager * mpc_ActualSignal;
   C_SdBueMlvSignalManager::E_InterActionMode me_InteractionMode;
   QPointF mc_LastMousePos;
   int32_t ms32_LastGridIndex;
   bool mq_SignalChanged;

   static const float64_t mhf64_SPACE;
   static const float64_t mhf64_Z_ORDER_BELOW_ALL_ITEMS;
   static const float64_t mhf64_Z_ORDER_DEFAULT;
   static const float64_t mhf64_Z_ORDER_INTERACTION;
   static const float64_t mhf64_Z_ORDER_EMPTY_ITEM;
   static const float64_t mhf64_Z_ORDER_HINT_ITEM;
   static const uint8_t mhu8_MAX_NUM_BITS;

   // Array for grid states. It saves the pointer to the signals on each bit
   std::set<C_SdBueMlvSignalManager *> mac_SetGridState[64];
   uint16_t mu16_LastGridPosFilled;

   // Color configuration for the signals
   static const C_SdBueMlvSignalManager::C_SignalItemColors mhac_SIGNALS_COLORS[64];
   std::vector<std::array<bool, 64> > mc_SignalsColorsUsed;

   // ECeS hint
   C_SdBueMlvBaseItem * mapc_EcesHints[2];

   // CANopen specific
   bool mq_CoFixedMapping;

   bool mq_ResizingEnabled;
   bool mq_ByteAligned;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
