//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       List with all datapools of a specific type of one node (header)

   See cpp file for detailed description

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_SDNDEDPSELECTORLISTWIDGET_HPP
#define C_SDNDEDPSELECTORLISTWIDGET_HPP

/* -- Includes ------------------------------------------------------------------------------------------------------ */

#include <QWidget>
#include <QAction>
#include <QList>

#include "stwtypes.hpp"

#include "C_SdNdeDpSelectorItemWidget.hpp"
#include "C_OgeContextMenu.hpp"
#include "C_OscNodeDataPool.hpp"
#include "C_OscNodeDataPoolId.hpp"
#include "C_OscCanProtocol.hpp"
#include "C_PuiSdNodeDataPool.hpp"
#include "C_SdNdeDpSelectorAddWidget.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_gui
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_SdNdeDpSelectorListWidget :
   public QWidget
{
   Q_OBJECT

public:
   explicit C_SdNdeDpSelectorListWidget(QWidget * const opc_Parent = NULL);
   ~C_SdNdeDpSelectorListWidget() override;

   bool SetTypeAndNode(const stw::opensyde_core::C_OscNodeDataPool::E_Type oe_Type, const uint32_t ou32_NodeIndex,
                       const bool oq_UsageViewActive);
   void SetSelected(const bool oq_Selected);
   bool SetActualDataPoolConflict(const int32_t os32_DataPoolWidgetIndex, const bool oq_Active) const;
   void ReloadDataPools(void);
   void UpdateDataPools(void);
   void UpdateActualDataPool(void);
   void AddNewDatapool(void);
   void Paste(void);
   bool CheckDataPoolsForConflict(std::vector<uint32_t> * const opc_InvalidDatapoolIndices = NULL) const;

   void UpdateSizeHint(const int32_t os32_MaxHeight);
   QSize sizeHint(void) const override;

   int32_t GetItemCount(void) const;
   int32_t GetCurrentItemIndex(void) const;
   int32_t GetViewPageCount(void) const;
   int32_t GetCurrentViewPage(void) const;

   void SetSelectedItem(const uint32_t ou32_Index);
   void SetCurrentViewPage(const int32_t os32_ViewIndex);
   void ScrollToItem(const uint32_t ou32_Index);

   //The signals keyword is necessary for Qt signal slot functionality
   //lint -save -e1736
Q_SIGNALS:
   //lint -restore
   void SigListChanged(void);
   void SigUpdateFollowingLists(stw::opensyde_core::C_OscNodeDataPool::E_Type oe_DataPoolType);
   void SigWidgetFocused(void);
   void SigOpenDataPoolContent(const int32_t os32_DataPoolWidgetIndex);
   void SigDataPoolChanged(void);
   void SigErrorCheck(void);
   void SigUpdateLists(const uint32_t ou32_NodeIndex, const uint32_t ou32_DataPoolIndex);
   void SigNoDataPoolSelected(void);
   void SigHideOtherToolTips(void);
   void SigDataPoolHoverStateChanged(const uint32_t ou32_DataPoolIndex, const bool oq_Hovered);

protected:
   void paintEvent(QPaintEvent * const opc_Event) override;
   void resizeEvent(QResizeEvent * const opc_Event) override;
   void keyPressEvent(QKeyEvent * const opc_Event) override;
   void focusInEvent(QFocusEvent * const opc_Event) override;
   void wheelEvent(QWheelEvent * const opc_Event) override;

   void m_MoveItem(const int32_t os32_SourceIndex, const int32_t os32_TargetIndex);
   void m_UpdateNumbers(void) const;
   //void m_DelegateStartPaint(void);
   //void m_DelegateStopPaint(void);

private:
   //Avoid call
   C_SdNdeDpSelectorListWidget(const C_SdNdeDpSelectorListWidget &);
   C_SdNdeDpSelectorListWidget & operator =(const C_SdNdeDpSelectorListWidget &) &;

   bool m_OpenDataPoolSelectDialog(stw::opensyde_core::C_OscNodeDataPool & orc_OscDataPool,
                                   C_SdNdeDpSelectorAddWidget::E_SelectionResult & ore_DialogResult,
                                   stw::opensyde_core::C_OscNodeDataPoolId & orc_SharedDatapoolId,
                                   QString & orc_RamViewFilePath);
   bool m_OpenDataPoolDialog(stw::opensyde_core::C_OscNodeDataPool & orc_OscDataPool,
                             stw::opensyde_gui_logic::C_PuiSdNodeDataPool & orc_UiDataPool,
                             const stw::opensyde_core::C_OscNodeDataPoolId * const opc_SharedDatapoolId,
                             const bool oq_NodeProgrammingSupport, const int32_t os32_DataPoolIndex = -1,
                             const bool oq_SelectName = false);
   void m_AddNewDataPool(const stw::opensyde_core::C_OscNodeDataPool & orc_OscDataPool,
                         const stw::opensyde_gui_logic::C_PuiSdNodeDataPool & orc_UiDataPool,
                         const int32_t os32_DataPoolIndex, const bool oq_AllowNameAdaptation,
                         const bool oq_AllowDataAdaptation, const bool oq_SharedDatapoolSelected = false,
                         const stw::opensyde_core::C_OscNodeDataPoolId * const opc_SharedDatapoolId = NULL);
   void m_AddDataPoolWidget(const uint32_t ou32_DataPoolIndex);
   void m_UpdateDataPoolWidget(const uint32_t ou32_DataPoolIndex, const int32_t os32_DataPoolWidgetIndex) const;
   void m_SynchronizeDatapoolProperties(const stw::opensyde_core::C_OscNodeDataPoolId & orc_AdaptedDatapool) const;

   void m_OnCustomContextMenuRequested(const QPoint & orc_Pos);
   void m_OnItemCustomContextMenuRequested(const QPoint & orc_Pos, const uint32_t ou32_Index);
   void m_OpenCustomContextMenu(const QPoint & orc_Pos, const C_SdNdeDpSelectorItemWidget * const opc_Item);
   void m_SetupContextMenu(void);
   void m_Edit(const bool oq_SelectName = false);
   void m_EditContent(void);
   void m_Copy(void) const;
   void m_Cut(void);
   void m_DeleteSlot(void);
   void m_Delete(const bool oq_AskUser);
   void m_MoveLeft(void);
   void m_MoveRight(void);
   void m_MoveUp(void);
   void m_MoveDown(void);
   void m_ItemClicked(const uint32_t ou32_Index);
   void m_ItemDoubleClicked(const uint32_t ou32_Index);

   void m_MoveDatapool(const int32_t os32_SourceIndex, const int32_t os32_TargetIndex);
   void m_InitFromData(const bool oq_Update = false);
   void m_Clear(void);
   void m_SelectionChanged(void);
   void m_AdaptSize(const QSize & orc_WidgetSize);
   void m_UpdateCounters(const bool oq_ForceRearrageItems = false);
   void m_RearrangeItems(void);
   static void mh_HandleDefaultNvmSize(stw::opensyde_core::C_OscNodeDataPool & orc_DataPool);
   void m_UpdateItemErrorToolTip(const uint32_t ou32_Index) const;
   bool m_IsItemComCanOpenDatapool(const C_SdNdeDpSelectorItemWidget * const opc_Item) const;

   stw::opensyde_gui_elements::C_OgeContextMenu * mpc_ContextMenu;
   QAction * mpc_AddAction;
   QAction * mpc_EditAction;
   QAction * mpc_EditContentAction;
   QAction * mpc_EditActionSeparator;
   QAction * mpc_DeleteAction;
   QAction * mpc_DeleteActionSeparator;
   QAction * mpc_CopyAction;
   QAction * mpc_CutAction;
   QAction * mpc_PasteAction;
   QAction * mpc_MoveActionSeparator;
   QAction * mpc_MoveLeftAction;
   QAction * mpc_MoveRightAction;
   QAction * mpc_MoveUpAction;
   QAction * mpc_MoveDownAction;
   bool mq_Selected;
   int32_t ms32_ItemsPerRow;
   int32_t ms32_ItemsPerViewPage;

   int32_t ms32_RowsPerViewPage;

   int32_t ms32_CountViewPages;
   int32_t ms32_CurrentViewPage;

   int32_t ms32_SelectedItemIndex;
   QList<C_SdNdeDpSelectorItemWidget *> mc_DpItems;

   uint32_t mu32_NodeIndex;
   stw::opensyde_core::C_OscNodeDataPool::E_Type me_DataPoolType;
   stw::opensyde_core::C_OscCanProtocol::E_Type me_ProtocolType;
   bool mq_UsageViewActive;
   int32_t ms32_ItemWidth;
   int32_t ms32_ItemHeight;
   int32_t ms32_HeightHint;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
