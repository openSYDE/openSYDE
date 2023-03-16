//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Widget for bus selection for database type openSYDE system definition. (header)

   See cpp file for detailed description

   \copyright   Copyright 2018 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_CAMMOSDATABASEBUSSELECTIONPOPUP_H
#define C_CAMMOSDATABASEBUSSELECTIONPOPUP_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <QWidget>
#include "C_OgePopUpDialog.hpp"
#include "C_OscSystemBus.hpp"
#include "C_CamProDatabaseData.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace Ui
{
class C_CamMosDatabaseBusSelectionPopup;
}

namespace stw
{
namespace opensyde_gui
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_CamMosDatabaseBusSelectionPopup :
   public QWidget
{
   Q_OBJECT

public:
   explicit C_CamMosDatabaseBusSelectionPopup(const std::vector<stw::opensyde_core::C_OscSystemBus> & orc_Busses,
                                              const stw::opensyde_gui_logic::C_CamProDatabaseData & orc_Database,
                                              stw::opensyde_gui_elements::C_OgePopUpDialog & orc_Parent);
   ~C_CamMosDatabaseBusSelectionPopup(void) override;

   uint32_t GetSelectedBus(void) const;

protected:
   void keyPressEvent(QKeyEvent * const opc_KeyEvent) override;

private:
   Ui::C_CamMosDatabaseBusSelectionPopup * mpc_Ui;
   //lint -e{1725} Only problematic if copy or assignment is allowed
   stw::opensyde_gui_elements::C_OgePopUpDialog & mrc_ParentDialog;
   //Don't use reference as this somehow can be deleted in the background
   const stw::opensyde_gui_logic::C_CamProDatabaseData mc_Database;
   std::vector<stw::opensyde_core::C_OscSystemBus> mc_Busses;

   void m_InitStaticNames(void) const;
   void m_InitStaticGuiElements(void) const;
   void m_InitComboBox(void) const;

   void m_OnComboBoxChanged(const int32_t os32_Index) const;

   void m_CancelClicked(void);
   void m_OkClicked(void);

   int32_t m_ComboboxIndexToBusIndex(const int32_t os32_ComboboxIndex) const;
   int32_t m_BusIndexToComboboxIndex(const int32_t os32_BusIndex) const;

   //Avoid call
   C_CamMosDatabaseBusSelectionPopup(const C_CamMosDatabaseBusSelectionPopup &);
   C_CamMosDatabaseBusSelectionPopup & operator =(const C_CamMosDatabaseBusSelectionPopup &) &;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
