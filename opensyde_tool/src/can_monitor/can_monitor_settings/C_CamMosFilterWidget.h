//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Widget for handling CAN Message filter configuration
 (header)

   See cpp file for detailed description

   \copyright   Copyright 2018 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_CAMMOSFILTERWIDGET_H
#define C_CAMMOSFILTERWIDGET_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "C_OgeWiOnlyBackground.h"
#include "C_CamMosFilterItemWidget.h"
#include "C_CamProFilterData.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace Ui
{
class C_CamMosFilterWidget;
}

namespace stw_opensyde_gui
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_CamMosFilterWidget :
   public stw_opensyde_gui_elements::C_OgeWiOnlyBackground
{
   Q_OBJECT

public:
   explicit C_CamMosFilterWidget(QWidget * const opc_Parent = NULL);
   ~C_CamMosFilterWidget(void);

   void LoadUserSettings(void) const;
   void PrepareForExpanded(const bool oq_Expand) const;

   void Clear(void);

   //The signals keyword is necessary for Qt signal slot functionality
   //lint -save -e1736
Q_SIGNALS:
   //lint -restore
   void SigFilterNumberChanged(const stw_types::uint32 ou32_FilterNumber);
   void SigAddFilterItems(const QList<stw_opensyde_gui_logic::C_CamProFilterItemData> & orc_FilterItems);
   void SigRemoveFilterItems(const QList<stw_opensyde_gui_logic::C_CamProFilterItemData> & orc_FilterItems);
   void SigRemoveAllFilters(void);

private:
   Ui::C_CamMosFilterWidget * mpc_Ui;
   std::vector<C_CamMosFilterItemWidget *> mc_Entries;

   //Avoid call
   C_CamMosFilterWidget(const C_CamMosFilterWidget &);
   C_CamMosFilterWidget & operator =(const C_CamMosFilterWidget &);

   void m_InitUi(void);
   void m_LoadConfig(void);
   void m_RemoveFilterWidget(C_CamMosFilterItemWidget * const opc_ItemWidget) const;
   void m_EnableFilters(const bool & orq_Enabled);
   void m_OnAddClicked(void);
   void m_AddFilterWidget(const stw_opensyde_gui_logic::C_CamProFilterData & orc_FilterData);
   void m_RemoveFilter(C_CamMosFilterItemWidget * const opc_ItemWidget);
   void m_ActivateFilter(const C_CamMosFilterItemWidget * const opc_ItemWidget, const bool & orq_Enable);
   void m_UpdateFilterConfiguration(const C_CamMosFilterItemWidget * const opc_ItemWidget,
                                    const stw_opensyde_gui_logic::C_CamProFilterData & orc_FilterOld,
                                    const stw_opensyde_gui_logic::C_CamProFilterData & orc_FilterNew);
   stw_types::sint32 m_CountActiveFilterPackages(void) const;
   stw_types::sint32 m_GetIndexFromWidget(const C_CamMosFilterItemWidget * const opc_ItemWidget);
   void m_GetActiveFilterItems(QList<stw_opensyde_gui_logic::C_CamProFilterItemData> & orc_ActiveFilterItems) const;
   void m_OnExpand(const bool oq_Expand) const;
   void m_UpdateTitleFilterCount(void) const;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif
