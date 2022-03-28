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
#include "C_OgePopUpDialog.h"
#include "C_PuiSvNodeUpdate.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace Ui
{
class C_SyvUpPacPemFileOptionsPopUp;
}

namespace stw_opensyde_gui
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_SyvUpPacPemFileOptionsPopUp :
   public QWidget
{
   Q_OBJECT

public:
   explicit C_SyvUpPacPemFileOptionsPopUp(stw_opensyde_gui_elements::C_OgePopUpDialog & orc_Parent,
                                          const stw_opensyde_gui_logic::C_PuiSvNodeUpdate::E_StateSecurity oe_StateSecurity,
                                          const stw_opensyde_gui_logic::C_PuiSvNodeUpdate::E_StateDebugger oe_StateDebugger);
   ~C_SyvUpPacPemFileOptionsPopUp(void);

   void InitStaticNames(void) const;
   stw_opensyde_gui_logic::C_PuiSvNodeUpdate::E_StateSecurity GetComboBoxSecState(void) const;
   stw_opensyde_gui_logic::C_PuiSvNodeUpdate::E_StateDebugger GetComboBoxDebState(void) const;

protected:
   virtual void keyPressEvent(QKeyEvent * const opc_KeyEvent) override;

private:
   Ui::C_SyvUpPacPemFileOptionsPopUp * mpc_Ui;
   //lint -e{1725} Only problematic if copy or assignment is allowed
   stw_opensyde_gui_elements::C_OgePopUpDialog & mrc_ParentDialog;

   static const stw_types::sintn mhsn_SEC_INDEX_NO_CHANGE;
   static const stw_types::sintn mhsn_SEC_INDEX_ACTIVATE;
   static const stw_types::sintn mhsn_SEC_INDEX_DEACTIVATE;
   static const stw_types::sintn mhsn_DEB_INDEX_NO_CHANGE;
   static const stw_types::sintn mhsn_DEB_INDEX_ACTIVATE;
   static const stw_types::sintn mhsn_DEB_INDEX_DEACTIVATE;

   void m_OkClicked(void);
   void m_CancelClicked(void);

   void m_InitComboBoxSec(const stw_opensyde_gui_logic::C_PuiSvNodeUpdate::E_StateSecurity oe_StateSecurity);
   void m_InitComboBoxDeb(const stw_opensyde_gui_logic::C_PuiSvNodeUpdate::E_StateDebugger oe_StateDebugger);

   //Avoid call
   C_SyvUpPacPemFileOptionsPopUp(const C_SyvUpPacPemFileOptionsPopUp &);
   C_SyvUpPacPemFileOptionsPopUp & operator =(const C_SyvUpPacPemFileOptionsPopUp &);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif
