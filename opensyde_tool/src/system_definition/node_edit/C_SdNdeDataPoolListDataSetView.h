//-----------------------------------------------------------------------------
/*!
   \file
   \brief       Data pool list data set table view (header)

   See cpp file for detailed description

   \implementation
   project     openSYDE
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     03.02.2017  STW/M.Echtler
   \endimplementation
*/
//-----------------------------------------------------------------------------
#ifndef C_SDNDEDATAPOOLLISTDATASETVIEW_H
#define C_SDNDEDATAPOOLLISTDATASETVIEW_H

/* -- Includes ------------------------------------------------------------- */

#include <QUndoStack>
#include <QLabel>
#include "stwtypes.h"
#include "C_TblViewScroll.h"
#include "C_PuiSdNodeDataPoolList.h"
#include "C_OSCNodeDataPoolList.h"
#include "C_SdNdeUnoDasDataPoolListManager.h"
#include "C_SdNdeDataPoolListDataSetDelegate.h"
#include "C_SdNdeDataPoolUtil.h"
#include "C_SdNdeDataPoolListModelViewManager.h"

/* -- Namespace ------------------------------------------------------------ */
namespace stw_opensyde_gui
{
/* -- Global Constants ----------------------------------------------------- */

/* -- Types ---------------------------------------------------------------- */

//lint -e{1960} Forward declarations necessary for automatic registration
class C_SdNdeDataPoolListDataSetView :
   public C_TblViewScroll
{
   Q_OBJECT

public:
   C_SdNdeDataPoolListDataSetView(QWidget * const opc_Parent = NULL);
   ~C_SdNdeDataPoolListDataSetView(void);
   void SetList(const stw_types::uint32 & oru32_NodeIndex, const stw_types::uint32 & oru32_DataPoolIndex,
                const stw_types::uint32 & oru32_ListIndex);
   bool Equals(const stw_types::uint32 & oru32_NodeIndex, const stw_types::uint32 & oru32_DataPoolIndex,
               const stw_types::uint32 & oru32_ListIndex) const;
   void Copy(void) const;
   void Cut(void);
   void Paste(void);
   void Delete(void);
   void Insert(const bool & orq_SetFocus);
   void DoMoveLeft(void);
   void DoMoveRight(void);
   QUndoCommand * TakeUndoCommand(void);
   void OnDataChange(const stw_types::uint32 & oru32_DataPoolListDataSetIndex, const QVariant & orc_NewData,
                     const stw_opensyde_gui_logic::C_SdNdeDataPoolUtil::E_DataSetDataChangeType & ore_DataChangeType);
   void SetModelViewManager(stw_opensyde_gui_logic::C_SdNdeDataPoolListModelViewManager * const opc_Value);
   void OnColumnCountChange(const stw_types::sint32 & ors32_NewColumnCount);

   //The signals keyword is necessary for Qt signal slot functionality
   //lint -save -e1736

Q_SIGNALS:
   //lint -restore
   void SigSelectionChanged(const stw_types::uint32 & oru32_Count);
   void SigButtonChange(const bool & orq_AddActive, const bool & orq_CutActive, const bool & orq_CopyActive,
                        const bool & orq_PasteActive, const bool & orq_DeleteActive, const bool & orq_MoveDownActive,
                        const bool & orq_MoveUpActive);
   void SigColumnCountChange(const stw_types::sint32 & ors32_NewColumnCount);

protected:
   // The naming of the Qt parameters can't be changed and are not compliant with the naming conventions
   //lint -save -e1960
   virtual void dropEvent(QDropEvent * const opc_Event) override;
   virtual void mouseMoveEvent(QMouseEvent * const opc_Event) override;
   virtual void leaveEvent(QEvent * const opc_Event) override;
   virtual void selectionChanged(const QItemSelection & orc_Selected, const QItemSelection & orc_Deselected) override;
   virtual void startDrag(const Qt::DropActions oc_SupportedActions) override;
   //lint -restore

private:
   stw_types::uint32 mu32_NodeIndex;
   stw_types::uint32 mu32_DataPoolIndex;
   stw_types::uint32 mu32_ListIndex;
   stw_opensyde_gui_logic::C_SdNdeUnoDasDataPoolListManager mc_UndoManager;
   stw_opensyde_gui_logic::C_SdNdeDataPoolListDataSetDelegate mc_Delegate;
   stw_opensyde_gui_logic::C_SdNdeDataPoolListModelViewManager * mpc_ModelViewManager;
   QLabel * mpc_LabelCorner;
   bool mq_AllowMoveLeft;
   bool mq_AllowMoveRight;

   void m_DeleteIndices(const std::vector<stw_types::uint32> & orc_Indices);
   void m_Move(const std::vector<stw_types::uint32> & oru32_SourceIndices,
               const std::vector<stw_types::uint32> & oru32_TargetIndices);
   std::vector<stw_types::uint32> m_GetSelectedIndices(void) const;
   void m_CheckActions(const std::vector<stw_types::uint32> & orc_SelectedIndices);
   void m_UpdateModelView(void);
   void m_UpdateCornerButton(void);
   stw_types::uint32 m_GetOneAfterHighestSelected(void);
   //Avoid call
   C_SdNdeDataPoolListDataSetView(const C_SdNdeDataPoolListDataSetView &);
   C_SdNdeDataPoolListDataSetView & operator =(const C_SdNdeDataPoolListDataSetView &);
};

/* -- Extern Global Variables ---------------------------------------------- */
} //end of namespace

#endif
