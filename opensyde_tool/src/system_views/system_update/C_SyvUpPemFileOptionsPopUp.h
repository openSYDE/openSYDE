//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Screen for pem file options
   \copyright   Copyright 2021 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_SYVUPPEMFILEOPTIONSPOPUP_H
#define C_SYVUPPEMFILEOPTIONSPOPUP_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <QWidget>
#include "C_OgePopUpDialog.h"
#include "C_PuiSvNodeUpdate.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace Ui
{
class C_SyvUpPemFileOptionsPopUp;
}

namespace stw_opensyde_gui
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_SyvUpPemFileOptionsPopUp :
   public QWidget
{
   Q_OBJECT

public:
   explicit C_SyvUpPemFileOptionsPopUp(stw_opensyde_gui_elements::C_OgePopUpDialog & orc_Parent,
                                       const stw_opensyde_gui_logic::C_PuiSvNodeUpdate::E_StateSecurity oe_StateSecurity,
                                       const stw_opensyde_gui_logic::C_PuiSvNodeUpdate::E_StateDebugger oe_StateDebugger);
   ~C_SyvUpPemFileOptionsPopUp(void);

   void InitStaticNames(void) const;
   stw_opensyde_gui_logic::C_PuiSvNodeUpdate::E_StateSecurity GetComboBoxSecState(void) const;
   stw_opensyde_gui_logic::C_PuiSvNodeUpdate::E_StateDebugger GetComboBoxDebState(void) const;

protected:
   virtual void keyPressEvent(QKeyEvent * const opc_KeyEvent) override;

private:
   Ui::C_SyvUpPemFileOptionsPopUp * mpc_Ui;
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
   C_SyvUpPemFileOptionsPopUp(const C_SyvUpPemFileOptionsPopUp &);
   C_SyvUpPemFileOptionsPopUp & operator =(const C_SyvUpPemFileOptionsPopUp &);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif
