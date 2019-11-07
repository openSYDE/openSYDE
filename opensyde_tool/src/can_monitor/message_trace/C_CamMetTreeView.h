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

#include "C_CamMetTreeModel.h"
#include "C_CamMetTreeDelegate.h"
#include "C_CamMetTreeGUIBuffer.h"
#include "C_OgePubIconOnly.h"
#include "C_SyvComMessageMonitor.h"
#include "C_CamMetTreeLoggerData.h"
#include "C_OgeTreeViewToolTipBase.h"
#include "C_OgeContextMenu.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw_opensyde_gui
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_CamMetTreeView :
   public stw_opensyde_gui_elements::C_OgeTreeViewToolTipBase,
   public stw_opensyde_gui_logic::C_SyvComMessageMonitor
{
   Q_OBJECT

public:
   C_CamMetTreeView(QWidget * const opc_Parent = NULL);
   ~C_CamMetTreeView(void);

   virtual void Continue(void) override;
   virtual void Pause(void) override;
   virtual void Stop(void) override;
   virtual void Start(void) override;

   void ActionClearData(void);
   virtual void SetProtocol(const stw_cmon_protocol::e_CMONL7Protocols oe_Protocol) override;
   void SetDisplayTree(const bool oq_Value);
   void SetDisplayUniqueMessages(const bool oq_Value);
   void SetDisplayAsHex(const bool oq_Value);
   void SetDisplayTimestampRelative(const bool oq_Value);
   bool GetDisplayAsHex(void);
   bool GetDisplayTimestampRelative(void);

   void SaveUserSettings(void) const;
   void SetCurrentColumnWidths(const std::vector<stw_types::sint32> & orc_ColumnWidths);
   std::vector<stw_types::sint32> GetCurrentColumnWidths(void) const;

protected:
   // The naming of the Qt parameters can't be changed and are not compliant with the naming conventions
   //lint -save -e1960
   virtual void showEvent(QShowEvent * const opc_Event) override;
   virtual void resizeEvent(QResizeEvent * const opc_Event) override;
   virtual void selectionChanged(const QItemSelection & orc_Selected, const QItemSelection & orc_Deselected);
   virtual void keyPressEvent(QKeyEvent * const opc_Event) override;
   virtual void drawBranches(QPainter * const opc_Painter, const QRect & orc_Rect,
                             const QModelIndex & orc_Index) const override;
   virtual stw_types::sintn sizeHintForColumn(stw_types::sintn osn_Column) const override;
   //lint -restore

private:
   static const QString mhc_Stylesheet;
   std::mutex mc_MutexUpdate;
   QThread mc_ThreadGUIBuffer;
   QTimer mc_TimerHandleMessages;
   QSortFilterProxyModel mc_SortProxyModel;
   stw_opensyde_gui_logic::C_CamMetTreeModel mc_Model;
   stw_opensyde_gui_logic::C_CamMetTreeDelegate mc_Delegate;
   stw_opensyde_gui_logic::C_CamMetTreeGUIBuffer mc_GUIBuffer;
   stw_opensyde_gui_elements::C_OgePubIconOnly * mpc_PushButtonScrollTop;
   stw_opensyde_gui_elements::C_OgePubIconOnly * mpc_PushButtonScrollBottom;
   stw_opensyde_gui_elements::C_OgeContextMenu * mpc_ContextMenu;
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
   void m_UpdateUi(const std::list<stw_opensyde_gui_logic::C_CamMetTreeLoggerData> & orc_Data);
   void m_SetChildColumns(const std::vector<stw_types::sint32> & orc_Indices);
   void m_SetChildColumns(const QModelIndex & orc_ModelIndex);
   void m_SetAllChildren(void);
   void m_RepositionButtons(void);
   void m_DoScrollTop(void);
   void m_DoScrollBottom(void);
   void m_RestoreUserSettings(void);
   void m_ExpandAll(void);
   void m_CollapseAll(void);
   void m_OnCollapse(const QModelIndex & orc_Index);
   std::vector<stw_types::sint32> m_GetCurrentColumnPositionIndices(void) const;
   void m_SetColumnPositionIndices(const std::vector<stw_types::sint32> & orc_NewColPositionIndices);
   bool m_ColumnsSortedAsExpected(const std::vector<stw_types::sint32> & orc_NewColPositionIndices) const;
   void m_HandleSorting(void);
   static void mh_DrawL(QPainter * const opc_Painter, const QPoint & orc_TopLeft, const stw_types::sint32 os32_Offset);
   static void mh_DrawPlus(QPainter * const opc_Painter, const QPoint & orc_TopLeft,
                           const stw_types::sint32 os32_Offset);
   static void mh_DrawVLine(QPainter * const opc_Painter, const QPoint & orc_TopLeft,
                            const stw_types::sint32 os32_Offset);

   const static stw_types::sintn mhsn_COL_WIDTH_TIME_STAMP;
   const static stw_types::sintn mhsn_COL_WIDTH_CAN_ID;
   const static stw_types::sintn mhsn_COL_WIDTH_CAN_NAME;
   const static stw_types::sintn mhsn_COL_WIDTH_CAN_DIR;
   const static stw_types::sintn mhsn_COL_WIDTH_CAN_DLC;
   const static stw_types::sintn mhsn_COL_WIDTH_CAN_DATA;
   const static stw_types::sintn mhsn_COL_WIDTH_CAN_COUNTER;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif
