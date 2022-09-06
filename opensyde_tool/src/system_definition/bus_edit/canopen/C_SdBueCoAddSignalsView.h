//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       View part for add signals tree
   \copyright   Copyright 2022 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_SDBUECOADDSIGNALSVIEW_H
#define C_SDBUECOADDSIGNALSVIEW_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "C_OgeTreeViewToolTipBase.h"
#include "C_SdBueCoAddSignalsModel.h"
#include "C_TblTreMultiColumnLeafSortFilter.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw_opensyde_gui
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_SdBueCoAddSignalsView :
   public stw_opensyde_gui_elements::C_OgeTreeViewToolTipBase
{
   Q_OBJECT

public:
   C_SdBueCoAddSignalsView(QWidget * const opc_Parent = NULL);
   void SetIndex(const stw_opensyde_core::C_OSCCanMessageIdentificationIndices & orc_MessageId);
   void PrepareCleanUp(void);
   std::vector<stw_opensyde_core::C_OSCCanOpenManagerMappableSignal> GetSelectedSignals(void) const;
   bool IsEmpty(void) const;
   void Search(const QString & orc_Text);

   //The signals keyword is necessary for Qt signal slot functionality
   //lint -save -e1736

Q_SIGNALS:
   //lint -restore
   void SigAccept(void);
   void SigSelectionChanged(const stw_types::sintn osn_Count);

protected:
   virtual void mouseDoubleClickEvent(QMouseEvent * const opc_Event) override;
   virtual void selectionChanged(const QItemSelection & orc_Selected, const QItemSelection & orc_Deselected) override;

private:
   stw_opensyde_gui_logic::C_SdBueCoAddSignalsModel mc_Model;
   stw_opensyde_gui_logic::C_TblTreMultiColumnLeafSortFilter mc_SortModel;

   void m_InitColumns(void);
   void m_ShowHideVerticalScrollBar(const stw_types::sintn osn_Min, const stw_types::sintn osn_Max) const;
   void m_ShowHideHorizontalScrollBar(const stw_types::sintn osn_Min, const stw_types::sintn osn_Max) const;
   QModelIndexList m_MapModelIndices(const QModelIndexList & orc_SortModelIndices) const;

   static stw_types::uint32 mh_CountUnique(const QModelIndexList & orc_Indices);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif
