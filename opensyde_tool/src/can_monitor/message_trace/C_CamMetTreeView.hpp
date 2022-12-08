//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Max performance view (header)

   See cpp file for detailed description

   \copyright   Copyright 2018 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_CAMMETTREEVIEW_H
#define C_CAMMETTREEVIEW_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <mutex>
#include <QTimer>
#include <QThread>
#include <QTreeView>
#include <QAction>
#include <QSortFilterProxyModel>

#include "C_CamMetTreeModel.hpp"
#include "C_CamMetTreeDelegate.hpp"
#include "C_CamMetTreeGuiBuffer.hpp"
#include "C_OgePubIconOnly.hpp"
#include "C_SyvComMessageMonitor.hpp"
#include "C_CamMetTreeLoggerData.hpp"
#include "C_OgeTreeViewToolTipBase.hpp"
#include "C_OgeContextMenu.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_gui
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_CamMetTreeView :
   public stw::opensyde_gui_elements::C_OgeTreeViewToolTipBase,
   public stw::opensyde_gui_logic::C_SyvComMessageMonitor
{
   Q_OBJECT

public:
   C_CamMetTreeView(QWidget * const opc_Parent = NULL);
   ~C_CamMetTreeView(void) noexcept override;

   void Continue(void) override;
   void Pause(void) override;
   void Stop(void) override;
   void Start(void) override;

   void ActionClearData(void);
   void SetProtocol(const stw::cmon_protocol::e_CanMonL7Protocols oe_Protocol) override;
   void SetDisplayTree(const bool oq_Value);
   void SetDisplayUniqueMessages(const bool oq_Value);
   void SetDisplayAsHex(const bool oq_Value);
   void SetDisplayTimestampRelative(const bool oq_Value);
   void SetDisplayTimestampAbsoluteTimeOfDay(const bool oq_Value);
   void SetTraceBufferSize(const uint32_t ou32_Value);
   bool GetDisplayAsHex(void) const;
   bool GetDisplayTimestampRelative(void) const;

   void SearchTrace(const QString & orc_SearchString, const bool oq_Next);

   void SaveUserSettings(void) const;
   void SetCurrentColumnWidths(const std::vector<int32_t> & orc_ColumnWidths);
   std::vector<int32_t> GetCurrentColumnWidths(void) const;

protected:
   void showEvent(QShowEvent * const opc_Event) override;
   void resizeEvent(QResizeEvent * const opc_Event) override;
   void selectionChanged(const QItemSelection & orc_Selected, const QItemSelection & orc_Deselected) override;
   void keyPressEvent(QKeyEvent * const opc_Event) override;
   void drawBranches(QPainter * const opc_Painter, const QRect & orc_Rect,
                     const QModelIndex & orc_Index) const override;
   int32_t sizeHintForColumn(const int32_t os32_Column) const override;

private:
   static const QString mhc_STYLESHEET;
   std::mutex mc_MutexUpdate;
   QThread mc_ThreadGuiBuffer;
   QTimer mc_TimerHandleMessages;
   QSortFilterProxyModel mc_SortProxyModel;
   stw::opensyde_gui_logic::C_CamMetTreeModel mc_Model;
   stw::opensyde_gui_logic::C_CamMetTreeDelegate mc_Delegate;
   stw::opensyde_gui_logic::C_CamMetTreeGuiBuffer mc_GuiBuffer;
   stw::opensyde_gui_elements::C_OgePubIconOnly * mpc_PushButtonScrollTop;
   stw::opensyde_gui_elements::C_OgePubIconOnly * mpc_PushButtonScrollBottom;
   stw::opensyde_gui_elements::C_OgeContextMenu * mpc_ContextMenu;
   QAction * mpc_ActionCopy;
   QAction * mpc_ActionExpandAll;
   QAction * mpc_ActionCollapseAll;
   bool mq_UniqueMessageMode;
   bool mq_IsRunning;
   const bool mq_AllowSorting;

   void m_SetupContextMenu(void);
   void m_OnCustomContextMenuRequested(const QPoint & orc_Pos);
   void m_CopySelection(void);
   void m_HandleMessages(void);
   void m_UpdateUi(const std::list<stw::opensyde_gui_logic::C_CamMetTreeLoggerData> & orc_Data);
   void m_SetChildColumns(const std::vector<int32_t> & orc_Indices);
   void m_SetChildColumns(const QModelIndex & orc_ModelIndex);
   void m_SetAllChildren(void);
   void m_RepositionButtons(void);
   void m_DoScrollTop(void);
   void m_DoScrollBottom(void);
   void m_RestoreUserSettings(void);
   void m_ExpandAll(void);
   void m_CollapseAll(void);
   void m_OnCollapse(const QModelIndex & orc_Index);
   std::vector<int32_t> m_GetCurrentColumnPositionIndices(void) const;
   void m_SetColumnPositionIndices(const std::vector<int32_t> & orc_NewColPositionIndices);
   bool m_ColumnsSortedAsExpected(const std::vector<int32_t> & orc_NewColPositionIndices) const;
   void m_HandleSorting(void);
   static void mh_DrawEl(QPainter * const opc_Painter, const QPoint & orc_TopLeft, const int32_t os32_Offset);
   static void mh_DrawPlus(QPainter * const opc_Painter, const QPoint & orc_TopLeft, const int32_t os32_Offset);
   static void mh_DrawVerticalLine(QPainter * const opc_Painter, const QPoint & orc_TopLeft, const int32_t os32_Offset);

   const static int32_t mhs32_COL_WIDTH_TIME_STAMP;
   const static int32_t mhs32_COL_WIDTH_CAN_ID;
   const static int32_t mhs32_COL_WIDTH_CAN_NAME;
   const static int32_t mhs32_COL_WIDTH_CAN_DIR;
   const static int32_t mhs32_COL_WIDTH_CAN_DLC;
   const static int32_t mhs32_COL_WIDTH_CAN_DATA;
   const static int32_t mhs32_COL_WIDTH_CAN_COUNTER;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
