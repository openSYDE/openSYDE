//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Popup dialog for security options
   \copyright   Copyright 2021 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_SYVUPPACSECURITYOPTIONSPOPUP_HPP
#define C_SYVUPPACSECURITYOPTIONSPOPUP_HPP

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <QWidget>
#include "C_OgePopUpDialog.hpp"
#include "C_OscViewNodeUpdate.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace Ui
{
class C_SyvUpPacSecurityOptionsPopUp;
}

namespace stw
{
namespace opensyde_gui
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_SyvUpPacSecurityOptionsPopUp :
   public QWidget
{
   Q_OBJECT

public:
   explicit C_SyvUpPacSecurityOptionsPopUp(stw::opensyde_gui_elements::C_OgePopUpDialog & orc_Parent,
                                           const stw::opensyde_core::C_OscViewNodeUpdate::E_StateSecureAuthentication oe_StateSecureAuthentication, const stw::opensyde_core::C_OscViewNodeUpdate::E_StateDebugger oe_StateDebugger, const stw::opensyde_core::C_OscViewNodeUpdate::E_StateTrafficEncryption oe_StateTrafficEncryption,
                                           const QString & orc_NodeName);
   ~C_SyvUpPacSecurityOptionsPopUp(void) override;

   void InitStaticNames(void) const;
   stw::opensyde_core::C_OscViewNodeUpdate::E_StateSecureAuthentication GetComboBoxSecState(void) const;
   stw::opensyde_core::C_OscViewNodeUpdate::E_StateDebugger GetComboBoxDebState(void) const;
   stw::opensyde_core::C_OscViewNodeUpdate::E_StateTrafficEncryption GetComboBoxTenState(void) const;

protected:
   void keyPressEvent(QKeyEvent * const opc_KeyEvent) override;

private:
   Ui::C_SyvUpPacSecurityOptionsPopUp * mpc_Ui;
   //lint -e{1725} Only problematic if copy or assignment is allowed
   stw::opensyde_gui_elements::C_OgePopUpDialog & mrc_ParentDialog;
   const QString mc_NodeName;

   static const int32_t mhs32_SEC_INDEX_NO_CHANGE;
   static const int32_t mhs32_SEC_INDEX_ACTIVATE;
   static const int32_t mhs32_SEC_INDEX_DEACTIVATE;
   static const int32_t mhs32_DEB_INDEX_NO_CHANGE;
   static const int32_t mhs32_DEB_INDEX_ACTIVATE;
   static const int32_t mhs32_DEB_INDEX_DEACTIVATE;
   static const int32_t mhs32_TEN_INDEX_NO_CHANGE;
   static const int32_t mhs32_TEN_INDEX_ACTIVATE;
   static const int32_t mhs32_TEN_INDEX_DEACTIVATE;

   void m_OkClicked(void);
   void m_CancelClicked(void);

   void m_InitComboBoxSec(
      const stw::opensyde_core::C_OscViewNodeUpdate::E_StateSecureAuthentication oe_StateSecureAuthentication);
   void m_InitComboBoxDeb(const stw::opensyde_core::C_OscViewNodeUpdate::E_StateDebugger oe_StateDebugger);
   void m_InitComboBoxTen(
      const stw::opensyde_core::C_OscViewNodeUpdate::E_StateTrafficEncryption oe_StateTrafficEncryption);

   //Avoid call
   C_SyvUpPacSecurityOptionsPopUp(const C_SyvUpPacSecurityOptionsPopUp &);
   C_SyvUpPacSecurityOptionsPopUp & operator =(const C_SyvUpPacSecurityOptionsPopUp &) &;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
