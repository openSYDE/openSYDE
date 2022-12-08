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
#include "stwtypes.hpp"
#include "C_TblViewScroll.hpp"
#include "C_PuiSdNodeDataPoolList.hpp"
#include "C_OscNodeDataPoolList.hpp"
#include "C_SdNdeUnoDasDataPoolListManager.hpp"
#include "C_SdNdeDpListDataSetDelegate.hpp"
#include "C_SdNdeDpUtil.hpp"
#include "C_SdNdeDpListModelViewManager.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_gui
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_SdNdeDpListDataSetView :
   public C_TblViewScroll
{
   Q_OBJECT

public:
   C_SdNdeDpListDataSetView(QWidget * const opc_Parent = NULL);
   virtual ~C_SdNdeDpListDataSetView(void);
   void SetList(const uint32_t & oru32_NodeIndex, const uint32_t & oru32_DataPoolIndex,
                const uint32_t & oru32_ListIndex);
   bool Equals(const uint32_t & oru32_NodeIndex, const uint32_t & oru32_DataPoolIndex,
               const uint32_t & oru32_ListIndex) const;
   void Copy(void) const;
   void Cut(void);
   void Paste(void);
   void Delete(void);
   void Insert(const bool & orq_SetFocus);
   void DoMoveLeft(void);
   void DoMoveRight(void);
   QUndoCommand * TakeUndoCommand(void);
   void OnDataChange(const uint32_t & oru32_DataPoolListDataSetIndex, const QVariant & orc_NewData,
                     const stw::opensyde_gui_logic::C_SdNdeDpUtil::E_DataSetDataChangeType & ore_DataChangeType);
   void SetModelViewManager(stw::opensyde_gui_logic::C_SdNdeDpListModelViewManager * const opc_Value);
   void OnColumnCountChange(const int32_t & ors32_NewColumnCount);

   //The signals keyword is necessary for Qt signal slot functionality
   //lint -save -e1736

Q_SIGNALS:
   //lint -restore
   void SigSelectionChanged(const uint32_t & oru32_Count);
   void SigButtonChange(const bool & orq_AddActive, const bool & orq_CutActive, const bool & orq_CopyActive,
                        const bool & orq_PasteActive, const bool & orq_DeleteActive, const bool & orq_MoveDownActive,
                        const bool & orq_MoveUpActive);
   void SigColumnCountChange(const int32_t & ors32_NewColumnCount);

protected:
   void dropEvent(QDropEvent * const opc_Event) override;
   void mouseMoveEvent(QMouseEvent * const opc_Event) override;
   void leaveEvent(QEvent * const opc_Event) override;
   void selectionChanged(const QItemSelection & orc_Selected, const QItemSelection & orc_Deselected) override;
   void startDrag(const Qt::DropActions oc_SupportedActions) override;

private:
   uint32_t mu32_NodeIndex;
   uint32_t mu32_DataPoolIndex;
   uint32_t mu32_ListIndex;
   stw::opensyde_gui_logic::C_SdNdeUnoDasDataPoolListManager mc_UndoManager;
   stw::opensyde_gui_logic::C_SdNdeDpListDataSetDelegate mc_Delegate;
   stw::opensyde_gui_logic::C_SdNdeDpListModelViewManager * mpc_ModelViewManager;
   QLabel * mpc_LabelCorner;
   bool mq_AllowMoveLeft;
   bool mq_AllowMoveRight;

   void m_DeleteIndices(const std::vector<uint32_t> & orc_Indices);
   void m_Move(const std::vector<uint32_t> & orc_SourceIndices, const std::vector<uint32_t> & orc_TargetIndices);
   std::vector<uint32_t> m_GetSelectedIndices(void) const;
   void m_CheckActions(const std::vector<uint32_t> & orc_SelectedIndices);
   void m_UpdateModelView(void);
   void m_UpdateCornerButton(void);
   uint32_t m_GetOneAfterHighestSelected(void);
   void m_CommentConfirmed(void);
   //Avoid call
   C_SdNdeDpListDataSetView(const C_SdNdeDpListDataSetView &);
   C_SdNdeDpListDataSetView & operator =(const C_SdNdeDpListDataSetView &) &;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
