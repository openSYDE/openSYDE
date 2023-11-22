//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       View part for add signals tree
   \copyright   Copyright 2022 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_SDBUECOADDSIGNALSVIEW_HPP
#define C_SDBUECOADDSIGNALSVIEW_HPP

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "C_OgeTreeViewToolTipBase.hpp"
#include "C_SdBueCoAddSignalsModel.hpp"
#include "C_TblTreMultiColumnLeafSortFilter.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_gui
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_SdBueCoAddSignalsView :
   public stw::opensyde_gui_elements::C_OgeTreeViewToolTipBase
{
   Q_OBJECT

public:
   C_SdBueCoAddSignalsView(QWidget * const opc_Parent = NULL);
   void SetIndex(const stw::opensyde_core::C_OscCanMessageIdentificationIndices & orc_MessageId);
   void PrepareCleanUp(void);
   std::vector<stw::opensyde_core::C_OscCanOpenManagerMappableSignal> GetSelectedSignals(void) const;
   bool IsEmpty(void) const;
   void Search(const QString & orc_Text);

   //The signals keyword is necessary for Qt signal slot functionality
   //lint -save -e1736

Q_SIGNALS:
   //lint -restore
   void SigAccept(void);
   void SigSelectionChanged(const int32_t os32_Count);

protected:
   void mouseDoubleClickEvent(QMouseEvent * const opc_Event) override;
   void selectionChanged(const QItemSelection & orc_Selected, const QItemSelection & orc_Deselected) override;

private:
   stw::opensyde_gui_logic::C_SdBueCoAddSignalsModel mc_Model;
   stw::opensyde_gui_logic::C_TblTreMultiColumnLeafSortFilter mc_SortModel;

   void m_InitColumns(void);
   void m_ShowHideVerticalScrollBar(const int32_t os32_Min, const int32_t os32_Max) const;
   void m_ShowHideHorizontalScrollBar(const int32_t os32_Min, const int32_t os32_Max) const;
   QModelIndexList m_MapModelIndices(const QModelIndexList & orc_SortModelIndices) const;

   static uint32_t mh_CountUnique(const QModelIndexList & orc_Indices);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
