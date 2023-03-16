//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Widget to display one data pool list content (header)

   See cpp file for detailed description

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_SDNDEDPLISTTABLEWIDGET_H
#define C_SDNDEDPLISTTABLEWIDGET_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */

#include <QWidget>
#include <QTreeWidgetItem>
#include <QUndoStack>
#include "stwtypes.hpp"
#include "C_PuiSdNodeDataPoolList.hpp"
#include "C_OscNodeDataPoolList.hpp"
#include "C_SdNdeUnoDataPoolManager.hpp"
#include "C_SdNdeDpListModelViewManager.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */

namespace Ui
{
class C_SdNdeDpListTableWidget;
}

namespace stw
{
namespace opensyde_gui
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */
class C_SdNdeDpListTableWidget :
   public QWidget
{
   Q_OBJECT

public:
   explicit C_SdNdeDpListTableWidget(QWidget * const opc_Parent = NULL, QTreeWidget * const opc_TreeWidget = NULL, stw::opensyde_gui_logic::C_SdNdeUnoDataPoolManager * const opc_UndoStack =
                                        NULL,
                                     const bool oq_PopUp = true);
   ~C_SdNdeDpListTableWidget(void) override;
   void InitStaticNames(void) const;
   void SetTreeWidgetAndUndoManager(QTreeWidget * const opc_TreeWidget,
                                    stw::opensyde_gui_logic::C_SdNdeUnoDataPoolManager * const opc_UndoStack);
   void SetList(const uint32_t ou32_NodeIndex, const uint32_t ou32_DataPoolIndex, const uint32_t ou32_ListIndex);
   void ClearSelection(void) const;
   void Copy(void) const;
   void Cut(void) const;
   void Paste(void) const;
   void Delete(void) const;
   void InsertAction(void) const;
   void Insert(const bool & orq_SetFocus = true) const;
   void DoMoveUp(void) const;
   void DoMoveDown(void) const;
   void Edit(void) const;
   void SetModelViewManager(stw::opensyde_gui_logic::C_SdNdeDpListModelViewManager * const opc_Value) const;
   void SelectDataElement(const int32_t os32_DataElementIndex) const;
   bool IsSelected(void) const;
   void Reset(void);
   void SetSelectedVariableNames(const std::vector<QString> & orc_VariableNames) const;
   std::vector<QString> GetSelectedVariableNames(void) const;

   //The signals keyword is necessary for Qt signal slot functionality
   //lint -save -e1736

Q_SIGNALS:
   //lint -restore
   void SigErrorChangePossible(void);
   void SigSizeChangePossible(void);
   void SigSelectionChanged(const uint32_t & oru32_ListIndex, const uint32_t & oru32_Count);
   void SigButtonChange(const bool & orq_AddActive, const bool & orq_CutActive, const bool & orq_CopyActive,
                        const bool & orq_PasteActive, const bool & orq_DeleteActive, const bool & orq_MoveDownActive,
                        const bool & orq_MoveUpActive);

private:
   Ui::C_SdNdeDpListTableWidget * mpc_Ui;
   QTreeWidget * mpc_TreeWidget;
   stw::opensyde_gui_logic::C_SdNdeUnoDataPoolManager * mpc_UndoStack;
   uint32_t mu32_NodeIndex;
   uint32_t mu32_DataPoolIndex;
   uint32_t mu32_ListIndex;
   bool mq_PopUp;

   void m_OnErrorChangePossible(void);
   void m_OnSizeChangePossible(void);
   void m_OnSelectionChanged(const uint32_t & oru32_Count);
   void m_OnButtonChange(const bool & orq_AddActive, const bool & orq_CutActive, const bool & orq_CopyActive,
                         const bool & orq_PasteActive, const bool & orq_DeleteActive, const bool & orq_MoveDownActive,
                         const bool & orq_MoveUpActive);
   void m_OnCrcOptionChange(const bool & orq_Value);
   void m_InitCrcOption(void);
   void m_SetCrcVisibility(const bool & orq_Visible) const;
   void m_AdjustToItems(const bool & orq_Initial = false);
   //Avoid call
   C_SdNdeDpListTableWidget(const C_SdNdeDpListTableWidget &);
   C_SdNdeDpListTableWidget & operator =(const C_SdNdeDpListTableWidget &) &;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
