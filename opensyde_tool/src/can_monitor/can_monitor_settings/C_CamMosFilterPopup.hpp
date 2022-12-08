//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Widget for create/edit filter dialog. (header)

   See cpp file for detailed description

   \copyright   Copyright 2018 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_CAMMOSFILTERPOPUP_H
#define C_CAMMOSFILTERPOPUP_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <QWidget>

#include "C_OgePopUpDialog.hpp"
#include "C_CamProFilterData.hpp"
#include "C_CamMosFilterTableModel.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace Ui
{
class C_CamMosFilterPopup;
}

namespace stw
{
namespace opensyde_gui
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_CamMosFilterPopup :
   public QWidget
{
   Q_OBJECT

public:
   explicit C_CamMosFilterPopup(const stw::opensyde_gui_logic::C_CamProFilterData & orc_FilterData,
                                stw::opensyde_gui_elements::C_OgePopUpDialog & orc_Parent);
   ~C_CamMosFilterPopup(void) override;

   stw::opensyde_gui_logic::C_CamProFilterData GetFilterData(void) const;

protected:
   void keyPressEvent(QKeyEvent * const opc_KeyEvent) override;

private:
   Ui::C_CamMosFilterPopup * mpc_Ui;
   //lint -e{1725} Only problematic if copy or assignment is allowed
   stw::opensyde_gui_elements::C_OgePopUpDialog & mrc_ParentDialog;
   // usually the table model is a member of the view, but because most access is from this popup class we put it here
   stw::opensyde_gui_logic::C_CamMosFilterTableModel * mpc_TableModel;
   const QString mc_UneditedName; // initial name needed on accept for not triggering duplicate check

   void m_InitStaticNames(void) const;
   void m_InitStaticGuiElements(void) const;
   void m_InitFromData(const stw::opensyde_gui_logic::C_CamProFilterData & orc_FilterData) const;
   void m_UpdateSettingsSection(const stw::opensyde_gui_logic::C_CamProFilterItemData & orc_FilterItemData) const;
   void m_ShowNoFilter(const bool oq_NoFilter) const;
   void m_ShowTypeSpecificWidgets(const int32_t os32_NewType) const;
   void m_UpdateLineEdits(const uint32_t ou32_RowIndex) const;
   void m_OnCancel(void) const;
   void m_OnOk(void);
   void m_OnAddFilterItem(void) const;
   void m_OnIndexClicked(const QModelIndex & orc_NewIndex) const;
   void m_OnDeleteKeyPressed(void) const;
   void m_OnRowChanged(const QModelIndex & orc_NewIndex, const QModelIndex & orc_PrevIndex) const;
   void m_OnTypeChanged(const int32_t os32_NewType) const;
   void m_OnRadioButtonToggle(const bool oq_Checked) const;
   void m_OnStartIdEdited(void);
   void m_OnEndIdEdited(void);
   void m_OnExtendedToggled(const bool oq_Checked);
   void m_OnAddFromDatabase(void);
   int32_t m_GetCurrentSelectedRowIndex(void) const;
   void m_UpdateTitleFilterItemCount(void) const;
   void m_SetMessageDataFromDatabase(const uint32_t ou32_CanId, const bool oq_IsExtended);

   //Avoid call
   C_CamMosFilterPopup(const C_CamMosFilterPopup &);
   C_CamMosFilterPopup & operator =(const C_CamMosFilterPopup &) &;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
