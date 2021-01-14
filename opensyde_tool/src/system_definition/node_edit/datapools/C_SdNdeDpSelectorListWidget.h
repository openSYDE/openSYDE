//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       List with all datapools of a specific type of one node (header)

   See cpp file for detailed description

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_SDNDEDPSELECTORLISTWIDGET_H
#define C_SDNDEDPSELECTORLISTWIDGET_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */

#include <QWidget>
#include <QAction>
#include <QList>

#include "stwtypes.h"

//#include "C_SdNdeDpSelectorListDelegate.h"

#include "C_SdNdeDpSelectorItemWidget.h"
#include "C_OgeContextMenu.h"
#include "C_OSCNodeDataPool.h"
#include "C_OSCNodeDataPoolId.h"
#include "C_OSCCanProtocol.h"
#include "C_PuiSdNodeDataPool.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw_opensyde_gui
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_SdNdeDpSelectorListWidget :
   public QWidget
{
   Q_OBJECT

public:
   explicit C_SdNdeDpSelectorListWidget(QWidget * const opc_Parent = NULL);
   virtual ~C_SdNdeDpSelectorListWidget();

   bool SetTypeAndNode(const stw_opensyde_core::C_OSCNodeDataPool::E_Type oe_Type,
                       const stw_types::uint32 ou32_NodeIndex, const bool oq_UsageViewActive);
   void SetSelected(const bool oq_Selected);
   bool SetActualDataPoolConflict(const stw_types::sintn osn_DataPoolWidgetIndex, const bool oq_Active) const;
   void UpdateActualDataPool(void);
   void AddNewDatapool(void);
   void Paste(void);
   bool CheckDataPoolsForConflict(std::vector<stw_types::uint32> * const opc_InvalidDatapoolIndices = NULL) const;

   void UpdateSizeHint(const stw_types::sintn osn_MaxHeight);
   virtual QSize sizeHint(void) const override;

   stw_types::sintn GetItemCount(void) const;
   stw_types::sintn GetCurrentItemIndex(void) const;
   stw_types::sintn GetViewPageCount(void) const;
   stw_types::sintn GetCurrentViewPage(void) const;

   void SetSelectedItem(const stw_types::uint32 ou32_Index);
   void SetCurrentViewPage(const stw_types::sintn osn_ViewIndex);
   void ScrollToItem(const stw_types::uint32 ou32_Index);

   //The signals keyword is necessary for Qt signal slot functionality
   //lint -save -e1736
Q_SIGNALS:
   //lint -restore
   void SigListChanged(void);
   void SigWidgetFocused(void);
   void SigOpenDataPoolContent(const stw_types::sintn osn_DataPoolWidgetIndex);
   void SigDataPoolChanged(void);
   void SigErrorCheck(void);
   void SigUpdateLists(const stw_types::uint32 ou32_NodeIndex, const stw_types::uint32 ou32_DataPoolIndex);
   void SigNoDataPoolSelected(void);
   void SigHideOtherToolTips(void);

protected:
   virtual void paintEvent(QPaintEvent * const opc_Event) override;
   virtual void resizeEvent(QResizeEvent * const opc_Event) override;
   virtual void keyPressEvent(QKeyEvent * const opc_Event) override;
   virtual void focusInEvent(QFocusEvent * const opc_Event) override;
   virtual void wheelEvent(QWheelEvent * const opc_Event) override;

   void m_MoveItem(const stw_types::sintn osn_SourceIndex, const stw_types::sintn osn_TargetIndex);
   void m_UpdateNumbers(void) const;
   //void m_DelegateStartPaint(void);
   //void m_DelegateStopPaint(void);

private:
   //Avoid call
   C_SdNdeDpSelectorListWidget(const C_SdNdeDpSelectorListWidget &);
   C_SdNdeDpSelectorListWidget & operator =(const C_SdNdeDpSelectorListWidget &);

   bool m_OpenShareDataPoolDialog(stw_opensyde_core::C_OSCNodeDataPool & orc_OSCDataPool,
                                  bool & orq_SharedDatapoolSelected,
                                  stw_opensyde_core::C_OSCNodeDataPoolId & orc_SharedDatapoolId);
   bool m_OpenDataPoolDialog(stw_opensyde_core::C_OSCNodeDataPool & orc_OSCDataPool,
                             stw_opensyde_gui_logic::C_PuiSdNodeDataPool & orc_UiDataPool,
                             const stw_opensyde_core::C_OSCNodeDataPoolId * const opc_SharedDatapoolId,
                             const bool oq_ShowApplicationSection, const stw_types::sint32 os32_DataPoolIndex = -1,
                             const bool oq_SelectName = false);
   void m_AddNewDataPool(const stw_opensyde_core::C_OSCNodeDataPool & orc_OSCDataPool,
                         const stw_opensyde_gui_logic::C_PuiSdNodeDataPool & orc_UIDataPool,
                         const stw_types::sint32 os32_DataPoolIndex, const bool oq_AllowNameAdaptation,
                         const bool oq_AllowDataAdaptation, const bool oq_SharedDatapoolSelected = false,
                         const stw_opensyde_core::C_OSCNodeDataPoolId * const opc_SharedDatapoolId = NULL);
   void m_AddDataPoolWidget(const stw_types::uint32 ou32_DataPoolIndex);
   void m_UpdateDataPoolWidget(const stw_types::uint32 ou32_DataPoolIndex,
                               const stw_types::sintn osn_DataPoolWidgetIndex) const;
   void m_SynchronizeDatapoolProperties(const stw_opensyde_core::C_OSCNodeDataPoolId & orc_AdaptedDatapool) const;

   void m_OnCustomContextMenuRequested(const QPoint & orc_Pos);
   void m_OnItemCustomContextMenuRequested(const QPoint & orc_Pos, const stw_types::uint32 ou32_Index);
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
   void m_ItemClicked(const stw_types::uint32 ou32_Index);
   void m_ItemDoubleClicked(const stw_types::uint32 ou32_Index);

   void m_MoveDatapool(const stw_types::sintn osn_SourceIndex, const stw_types::sintn osn_TargetIndex);
   void m_InitFromData(const bool oq_Update = false);
   void m_Clear(void);
   void m_SelectionChanged(void);
   void m_AdaptSize(const QSize & orc_WidgetSize);
   void m_UpdateCounters(const bool oq_ForceRearrageItems = false);
   void m_RearrangeItems(void);
   void m_UpdateItemErrorToolTip(const stw_types::uint32 ou32_Index) const;

   // TODO BAY:
   //stw_opensyde_gui_logic::C_SdNdeDpSelectorListDelegate mc_Delegate;
   stw_opensyde_gui_elements::C_OgeContextMenu * mpc_ContextMenu;
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
   stw_types::sintn msn_ItemsPerRow;
   stw_types::sintn msn_ItemsPerViewPage;

   stw_types::sintn msn_RowsPerViewPage;

   stw_types::sintn msn_CountViewPages;
   stw_types::sintn msn_CurrentViewPage;

   stw_types::sintn msn_SelectedItemIndex;
   QList<C_SdNdeDpSelectorItemWidget *> mc_DpItems;

   stw_types::uint32 mu32_NodeIndex;
   stw_opensyde_core::C_OSCNodeDataPool::E_Type me_DataPoolType;
   stw_opensyde_core::C_OSCCanProtocol::E_Type me_ProtocolType;
   bool mq_UsageViewActive;
   stw_types::sintn msn_ItemWidth;
   stw_types::sintn msn_ItemHeight;
   stw_types::sintn msn_HeightHint;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif
