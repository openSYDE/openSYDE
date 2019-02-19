//-----------------------------------------------------------------------------
/*!
   \file
   \brief       Widget to display one data pool list entry (header)

   See cpp file for detailed description

   \implementation
   project     openSYDE
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     17.01.2017  STW/M.Echtler
   \endimplementation
*/
//-----------------------------------------------------------------------------
#ifndef C_SDNDEDATAPOOLLISTHEADERWIDGET_H
#define C_SDNDEDATAPOOLLISTHEADERWIDGET_H

/* -- Includes ------------------------------------------------------------- */

#include <QIcon>
#include <QTimer>
#include <QWidget>
#include <QTreeWidgetItem>
#include "stwtypes.h"
#include "C_SdNdeUnoDataPoolManager.h"
#include "C_SdNdeDataPoolListModelViewManager.h"

/* -- Namespace ------------------------------------------------------------ */

namespace Ui
{
class C_SdNdeDataPoolListHeaderWidget;
}

namespace stw_opensyde_gui
{
/* -- Global Constants ----------------------------------------------------- */

/* -- Types ---------------------------------------------------------------- */
class C_SdNdeDataPoolListHeaderWidget :
   public QWidget
{
   Q_OBJECT

public:
   explicit C_SdNdeDataPoolListHeaderWidget(QWidget * const opc_Parent = NULL,
                                            QTreeWidget * const opc_ListWidget = NULL,
                                            stw_opensyde_gui_logic::C_SdNdeUnoDataPoolManager * const opc_UndoManager = NULL, stw_opensyde_gui_logic::C_SdNdeDataPoolListModelViewManager * const opc_ModelViewManager = NULL, const stw_types::uint32 ou32_NodeIndex = 0, const stw_types::uint32 ou32_DataPoolIndex = 0,
                                            const stw_types::uint32 ou32_ListIndex = 0);
   ~C_SdNdeDataPoolListHeaderWidget(void);

   void SetInitParameters(QTreeWidget * const opc_TreeWidget,
                          stw_opensyde_gui_logic::C_SdNdeUnoDataPoolManager * const opc_UndoManager,
                          stw_opensyde_gui_logic::C_SdNdeDataPoolListModelViewManager * const opc_ModelViewManager,
                          const stw_types::uint32 ou32_NodeIndex, const stw_types::uint32 ou32_DataPoolIndex,
                          const stw_types::uint32 ou32_ListIndex);
   void SetIndex(const stw_types::uint32 & oru32_Value);
   void InitStaticNames(void) const;
   void RegisterExpandOrCollapse(const bool & orq_Expanded) const;
   void CheckError(void);
   void VariablesSizeChange(void);
   void UpdateDataSetCount(void);
   void NotifySelection(const bool & orq_Selected);
   void SetEditFocus(void);
   void PrepareExpandedMode(void) const;
   void PopUp(void);

   //The signals keyword is necessary for Qt signal slot functionality
   //lint -save -e1736

Q_SIGNALS:
   //lint -restore
   void SigNewHeight(const C_SdNdeDataPoolListHeaderWidget * const opc_Item);
   void SigExpand(const C_SdNdeDataPoolListHeaderWidget * const opc_Item, const bool oq_Expand);
   void SigUpdateAddress(void);
   void SigErrorChange(void);
   void SigClose(void);
   void SigSave(void);
   void SigSaveAs(void);
   void SigUpdated(void);
   void SigUpdateTable(void);
   void SigExclusiveSelection(const stw_types::uint32 & oru32_ListIndex);

private:
   Ui::C_SdNdeDataPoolListHeaderWidget * const mpc_Ui;
   QTreeWidget * mpc_TreeWidget;
   stw_opensyde_gui_logic::C_SdNdeUnoDataPoolManager * mpc_UndoManager;
   stw_opensyde_gui_logic::C_SdNdeDataPoolListModelViewManager * mpc_ModelViewManager;
   stw_types::uint32 mu32_NodeIndex;
   stw_types::uint32 mu32_DataPoolIndex;
   stw_types::uint32 mu32_ListIndex;
   static const stw_types::uint32 mhu32_HeaderHeight;
   static const stw_types::uint32 mhu32_HeaderExpandedHeight;
   static const stw_types::sintn mhsn_GroupSize;
   stw_types::uint32 mu32_ExpandedHeight;
   bool mq_InitActive;
   bool mq_DataSetError;
   QTimer mc_DoubleClickTimer;

   void m_OnPushButtonExpandClicked(const bool oq_Checked);
   void m_HandleWidgetResize(const stw_types::uint32 ou32_NewHeight);
   void m_UpdateListNamePrefix(void) const;
   void m_HandleFocus(void);
   void m_HandleTextEditFocus(const bool oq_Active);
   void m_UpdateUi(void);
   void m_EditNameFinished(void);
   void m_EditCommentFinished(void);
   void m_EditSizeFinished(void);
   void m_OpenDataSetEdit(void);
   void m_ChangeName(void) const;
   void m_OnDoubleClickTimeout(void) const;
   void m_OnOK(void);
   void m_UpdateErrorToolTip(void) const;
   //Avoid call
   C_SdNdeDataPoolListHeaderWidget(const C_SdNdeDataPoolListHeaderWidget &);
   C_SdNdeDataPoolListHeaderWidget & operator =(const C_SdNdeDataPoolListHeaderWidget &);
};

/* -- Extern Global Variables ---------------------------------------------- */
} //end of namespace

#endif
