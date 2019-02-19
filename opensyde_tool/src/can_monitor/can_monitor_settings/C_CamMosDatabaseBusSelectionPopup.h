//-----------------------------------------------------------------------------
/*!
   \file
   \brief       Widget for bus selection in for database type openSYDE system definition. (header)

   See cpp file for detailed description

   \implementation
   project     openSYDE
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     19.12.2018  STW/G.Landsgesell
   \endimplementation
*/
//-----------------------------------------------------------------------------
#ifndef C_CAMMOSDATABASEBUSSELECTIONPOPUP_H
#define C_CAMMOSDATABASEBUSSELECTIONPOPUP_H

/* -- Includes ------------------------------------------------------------- */
#include <QWidget>
#include "C_OgePopUpDialog.h"
#include "C_OSCSystemBus.h"
#include "C_CamProDatabaseData.h"

/* -- Namespace ------------------------------------------------------------ */
namespace Ui
{
class C_CamMosDatabaseBusSelectionPopup;
}

namespace stw_opensyde_gui
{
/* -- Global Constants ----------------------------------------------------- */

/* -- Types ---------------------------------------------------------------- */

class C_CamMosDatabaseBusSelectionPopup :
   public QWidget
{
   Q_OBJECT

public:
   explicit C_CamMosDatabaseBusSelectionPopup(const std::vector<stw_opensyde_core::C_OSCSystemBus> & orc_Busses,
                                              const stw_opensyde_gui_logic::C_CamProDatabaseData & orc_Database,
                                              stw_opensyde_gui_elements::C_OgePopUpDialog & orc_Parent);
   ~C_CamMosDatabaseBusSelectionPopup(void);

   stw_types::uint32 GetSelectedBus(void) const;

protected:
   // The naming of the Qt parameters can't be changed and are not compliant with the naming conventions
   //lint -save -e1960
   virtual void keyPressEvent(QKeyEvent * const opc_KeyEvent) override;
   //lint -restore

private:
   Ui::C_CamMosDatabaseBusSelectionPopup * mpc_Ui;
   //lint -e{1725} Only problematic if copy or assignment is allowed
   stw_opensyde_gui_elements::C_OgePopUpDialog & mrc_ParentDialog;
   //Don't use reference as this somehow can be deleted in the background
   const stw_opensyde_gui_logic::C_CamProDatabaseData mc_Database;
   std::vector<stw_opensyde_core::C_OSCSystemBus> mc_Busses;

   void m_InitStaticNames(void) const;
   void m_InitStaticGUIElements(void) const;
   void m_InitComboBox(void) const;

   void m_OnComboBoxChanged(const stw_types::sint32 os32_Index) const;

   void m_CancelClicked(void);
   void m_OkClicked(void);

   stw_types::sint32 m_ComboboxIndexToBusIndex(const stw_types::sint32 os32_ComboboxIndex) const;
   stw_types::sint32 m_BusIndexToComboboxIndex(const stw_types::sint32 os32_BusIndex) const;

   //Avoid call
   C_CamMosDatabaseBusSelectionPopup(const C_CamMosDatabaseBusSelectionPopup &);
   C_CamMosDatabaseBusSelectionPopup & operator =(const C_CamMosDatabaseBusSelectionPopup &);
};

/* -- Extern Global Variables ---------------------------------------------- */
} //end of namespace

#endif
