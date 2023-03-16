//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Screen for pem file options
   \copyright   Copyright 2021 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_SYVUPPACPEMFILEOPTIONSPOPUP_H
#define C_SYVUPPACPEMFILEOPTIONSPOPUP_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <QWidget>
#include "C_OgePopUpDialog.hpp"
#include "C_OscViewNodeUpdate.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace Ui
{
class C_SyvUpPacPemFileOptionsPopUp;
}

namespace stw
{
namespace opensyde_gui
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_SyvUpPacPemFileOptionsPopUp :
   public QWidget
{
   Q_OBJECT

public:
   explicit C_SyvUpPacPemFileOptionsPopUp(stw::opensyde_gui_elements::C_OgePopUpDialog & orc_Parent,
                                          const stw::opensyde_core::C_OscViewNodeUpdate::E_StateSecurity oe_StateSecurity,
                                          const stw::opensyde_core::C_OscViewNodeUpdate::E_StateDebugger oe_StateDebugger);
   ~C_SyvUpPacPemFileOptionsPopUp(void) override;

   void InitStaticNames(void) const;
   stw::opensyde_core::C_OscViewNodeUpdate::E_StateSecurity GetComboBoxSecState(void) const;
   stw::opensyde_core::C_OscViewNodeUpdate::E_StateDebugger GetComboBoxDebState(void) const;

protected:
   void keyPressEvent(QKeyEvent * const opc_KeyEvent) override;

private:
   Ui::C_SyvUpPacPemFileOptionsPopUp * mpc_Ui;
   //lint -e{1725} Only problematic if copy or assignment is allowed
   stw::opensyde_gui_elements::C_OgePopUpDialog & mrc_ParentDialog;

   static const int32_t mhs32_SEC_INDEX_NO_CHANGE;
   static const int32_t mhs32_SEC_INDEX_ACTIVATE;
   static const int32_t mhs32_SEC_INDEX_DEACTIVATE;
   static const int32_t mhs32_DEB_INDEX_NO_CHANGE;
   static const int32_t mhs32_DEB_INDEX_ACTIVATE;
   static const int32_t mhs32_DEB_INDEX_DEACTIVATE;

   void m_OkClicked(void);
   void m_CancelClicked(void);

   void m_InitComboBoxSec(const stw::opensyde_core::C_OscViewNodeUpdate::E_StateSecurity oe_StateSecurity);
   void m_InitComboBoxDeb(const stw::opensyde_core::C_OscViewNodeUpdate::E_StateDebugger oe_StateDebugger);

   //Avoid call
   C_SyvUpPacPemFileOptionsPopUp(const C_SyvUpPacPemFileOptionsPopUp &);
   C_SyvUpPacPemFileOptionsPopUp & operator =(const C_SyvUpPacPemFileOptionsPopUp &) &;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
