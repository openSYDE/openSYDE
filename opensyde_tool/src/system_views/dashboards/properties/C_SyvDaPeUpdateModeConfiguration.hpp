//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Widget for system view dashboard data element update mode (header)

   See cpp file for detailed description

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_SYVDAPEUPDATEMODECONFIGURATION_HPP
#define C_SYVDAPEUPDATEMODECONFIGURATION_HPP

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <QWidget>
#include "stwtypes.hpp"
#include "C_OgePopUpDialog.hpp"
#include "C_PuiSvDbNodeDataPoolListElementId.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace Ui
{
class C_SyvDaPeUpdateModeConfiguration;
}

namespace stw
{
namespace opensyde_gui
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */
class C_SyvDaPeUpdateModeConfiguration :
   public QWidget
{
   Q_OBJECT

public:
   explicit C_SyvDaPeUpdateModeConfiguration(stw::opensyde_gui_elements::C_OgePopUpDialog & orc_Parent,
                                             const uint32_t ou32_ViewIndex);
   ~C_SyvDaPeUpdateModeConfiguration(void) override;

   void InitStaticNames(void) const;
   void Select(const stw::opensyde_gui_logic::C_PuiSvDbNodeDataPoolListElementId & orc_Id) const;

protected:
   void keyPressEvent(QKeyEvent * const opc_KeyEvent) override;

private:
   //Avoid call
   C_SyvDaPeUpdateModeConfiguration(const C_SyvDaPeUpdateModeConfiguration &);
   C_SyvDaPeUpdateModeConfiguration & operator =(const C_SyvDaPeUpdateModeConfiguration &) &;

   Ui::C_SyvDaPeUpdateModeConfiguration * mpc_Ui;
   const uint32_t mu32_ViewIndex;
   //lint -e{1725} Only problematic if copy or assignment is allowed
   stw::opensyde_gui_elements::C_OgePopUpDialog & mrc_ParentDialog;

   void m_OkClicked(void);
   void m_CancelClicked(void);
   void m_ConfigureClicked(void) const;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
