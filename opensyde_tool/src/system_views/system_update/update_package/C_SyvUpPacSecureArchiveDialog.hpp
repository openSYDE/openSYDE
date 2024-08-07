//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Dialog for creating secure archive file (.syde_sup)
   \copyright   Copyright 2024 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_SYVUPPACSECUREARCHIVEDIALOG_H
#define C_SYVUPPACSECUREARCHIVEDIALOG_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <QWidget>

#include "C_OgePopUpDialog.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace Ui
{
class C_SyvUpPacSecureArchiveDialog;
}

namespace stw
{
namespace opensyde_gui
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_SyvUpPacSecureArchiveDialog :
   public QWidget
{
   Q_OBJECT

public:
   explicit C_SyvUpPacSecureArchiveDialog(stw::opensyde_gui_elements::C_OgePopUpDialog & orc_Parent);
   ~C_SyvUpPacSecureArchiveDialog(void) override;

   void InitStaticNames(void) const;
   QString GetPassword(void) const;
   QString GetPrivateKeyPath(void) const;

private:
   //Avoid call
   C_SyvUpPacSecureArchiveDialog(const C_SyvUpPacSecureArchiveDialog &);
   C_SyvUpPacSecureArchiveDialog & operator =(const C_SyvUpPacSecureArchiveDialog &) &;

   void m_OkClicked(void);
   void m_CancelClicked(void) const;
   void m_InitPwdLineEdit(void);
   void m_ShowPassword(void);
   void m_HidePassword(void);
   void m_SetPrivateKeyPath(const QString & orc_Path) const;
   void m_PrivateKeyPathClicked(void);
   int32_t m_CheckPath(void);

   Ui::C_SyvUpPacSecureArchiveDialog * mpc_Ui;
   //lint -e{1725} Only problematic if copy or assignment is allowed
   stw::opensyde_gui_elements::C_OgePopUpDialog & mrc_ParentDialog;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
