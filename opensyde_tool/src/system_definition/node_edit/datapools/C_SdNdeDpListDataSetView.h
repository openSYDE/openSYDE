//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Data pool list data set table view (header)

   See cpp file for detailed description

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_SDNDEDPLISTDATASETVIEW_H
#define C_SDNDEDPLISTDATASETVIEW_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */

#include <QUndoStack>
#include <QLabel>
#include "stwtypes.h"
#include "C_TblViewScroll.h"
#include "C_PuiSdNodeDataPoolList.h"
#include "C_OSCNodeDataPoolList.h"
#include "C_SdNdeUnoDasDataPoolListManager.h"
#include "C_SdNdeDpListDataSetDelegate.h"
#include "C_SdNdeDpUtil.h"
#include "C_SdNdeDpListModelViewManager.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw_opensyde_gui
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_SdNdeDpListDataSetView :
   public C_TblViewScroll
{
   Q_OBJECT

public:
   C_SdNdeDpListDataSetView(QWidget * const opc_Parent = NULL);
   ~C_SdNdeDpListDataSetView(void);
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
                     const stw_opensyde_gui_logic::C_SdNdeDpUtil::E_DataSetDataChangeType & ore_DataChangeType);
   void SetModelViewManager(stw_opensyde_gui_logic::C_SdNdeDpListModelViewManager * const opc_Value);
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
   virtual void dropEvent(QDropEvent * const opc_Event) override;
   virtual void mouseMoveEvent(QMouseEvent * const opc_Event) override;
   virtual void leaveEvent(QEvent * const opc_Event) override;
   virtual void selectionChanged(const QItemSelection & orc_Selected, const QItemSelection & orc_Deselected) override;
   virtual void startDrag(const Qt::DropActions oc_SupportedActions) override;

private:
   stw_types::uint32 mu32_NodeIndex;
   stw_types::uint32 mu32_DataPoolIndex;
   stw_types::uint32 mu32_ListIndex;
   stw_opensyde_gui_logic::C_SdNdeUnoDasDataPoolListManager mc_UndoManager;
   stw_opensyde_gui_logic::C_SdNdeDpListDataSetDelegate mc_Delegate;
   stw_opensyde_gui_logic::C_SdNdeDpListModelViewManager * mpc_ModelViewManager;
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
   void m_CommentConfirmed(void);
   //Avoid call
   C_SdNdeDpListDataSetView(const C_SdNdeDpListDataSetView &);
   C_SdNdeDpListDataSetView & operator =(const C_SdNdeDpListDataSetView &);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif
