//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Widget to display one data pool list entry (header)

   See cpp file for detailed description

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_SDNDEDPLISTHEADERWIDGET_H
#define C_SDNDEDPLISTHEADERWIDGET_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */

#include <QIcon>
#include <QTimer>
#include <QWidget>
#include <QTreeWidgetItem>
#include "stwtypes.hpp"
#include "C_SdNdeUnoDataPoolManager.hpp"
#include "C_SdNdeDpListModelViewManager.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */

namespace Ui
{
class C_SdNdeDpListHeaderWidget;
}

namespace stw
{
namespace opensyde_gui
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */
class C_SdNdeDpListHeaderWidget :
   public QWidget
{
   Q_OBJECT

public:
   explicit C_SdNdeDpListHeaderWidget(QWidget * const opc_Parent = NULL, QTreeWidget * const opc_ListWidget = NULL,
                                      stw::opensyde_gui_logic::C_SdNdeUnoDataPoolManager * const opc_UndoManager = NULL,
                                      stw::opensyde_gui_logic::C_SdNdeDpListModelViewManager * const opc_ModelViewManager = NULL, const uint32_t ou32_NodeIndex = 0, const uint32_t ou32_DataPoolIndex = 0,
                                      const uint32_t ou32_ListIndex = 0);
   ~C_SdNdeDpListHeaderWidget(void) override;

   void SetInitParameters(QTreeWidget * const opc_TreeWidget,
                          stw::opensyde_gui_logic::C_SdNdeUnoDataPoolManager * const opc_UndoManager,
                          stw::opensyde_gui_logic::C_SdNdeDpListModelViewManager * const opc_ModelViewManager,
                          const uint32_t ou32_NodeIndex, const uint32_t ou32_DataPoolIndex,
                          const uint32_t ou32_ListIndex);
   void SetIndex(const uint32_t & oru32_Value);
   void InitStaticNames(void) const;
   void RegisterExpandOrCollapse(const bool & orq_Expanded) const;
   void CheckError(void);
   void UpdateUi(void);
   void UpdateDataSetCount(void);
   void NotifySelection(const bool & orq_Selected);
   void SetEditFocus(void);
   void PrepareExpandedMode(void) const;
   void PopUp(void);

   //The signals keyword is necessary for Qt signal slot functionality
   //lint -save -e1736

Q_SIGNALS:
   //lint -restore
   void SigExpand(const C_SdNdeDpListHeaderWidget * const opc_Item, const bool oq_Expand);
   void SigErrorChange(void);
   void SigClose(void);
   void SigSave(void);
   void SigSaveAs(void);
   void SigUpdated(void);
   void SigUpdateTable(void);
   void SigExclusiveSelection(const uint32_t & oru32_ListIndex);

private:
   Ui::C_SdNdeDpListHeaderWidget * const mpc_Ui;
   QTreeWidget * mpc_TreeWidget;
   stw::opensyde_gui_logic::C_SdNdeUnoDataPoolManager * mpc_UndoManager;
   stw::opensyde_gui_logic::C_SdNdeDpListModelViewManager * mpc_ModelViewManager;
   uint32_t mu32_NodeIndex;
   uint32_t mu32_DataPoolIndex;
   uint32_t mu32_ListIndex;
   bool mq_InitActive;
   bool mq_DataSetError;
   QTimer mc_DoubleClickTimer;

   void m_OnPushButtonExpandClicked(const bool oq_Checked);
   void m_UpdateListNamePrefix(void) const;
   void m_HandleFocus(void);
   void m_UpdateUi(void);
   void m_EditNameFinished(void);
   void m_OnEditCommentClicked(void);
   void m_EditSizeFinished(void);
   void m_OpenDataSetEdit(void);
   void m_CheckName(void) const;
   void m_OnDoubleClickTimeout(void) const;
   void m_OnOk(void);
   void m_UpdateErrorToolTip(void) const;
   //Avoid call
   C_SdNdeDpListHeaderWidget(const C_SdNdeDpListHeaderWidget &);
   C_SdNdeDpListHeaderWidget & operator =(const C_SdNdeDpListHeaderWidget &) &;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
