//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Creating Security Certificate Package
   \copyright   Copyright 2025 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_SYVUPPACSECURITYCERTIFICATEPACKAGEDIALOG_H
#define C_SYVUPPACSECURITYCERTIFICATEPACKAGEDIALOG_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <QWidget>
#include <QVBoxLayout>
#include "C_OgePopUpDialog.hpp"
#include "C_SyvUpPacPemFilesListWidget.hpp"
#include "C_SclString.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace Ui
{
class C_SyvUpPacSecurityCertificatePackageDialog;
}

namespace stw
{
namespace opensyde_gui
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_SyvUpPacSecurityCertificatePackageDialog :
   public QWidget
{
   Q_OBJECT

public:
   explicit C_SyvUpPacSecurityCertificatePackageDialog(stw::opensyde_gui_elements::C_OgePopUpDialog & orc_Parent);
   ~C_SyvUpPacSecurityCertificatePackageDialog(void) override;

   QString GetPublicKeyPath(void) const;
   QString GetPassword(void) const;
   std::vector<stw::scl::C_SclString> GetPemFiles(void);
   bool GetOptionAddPemFiles(void) const;
   bool GetOptionAddSecureAuthentification(void) const;

protected:
   void keyPressEvent(QKeyEvent * const opc_KeyEvent) override;

private:
   //Avoid call
   C_SyvUpPacSecurityCertificatePackageDialog(const C_SyvUpPacSecurityCertificatePackageDialog &);
   C_SyvUpPacSecurityCertificatePackageDialog & operator =(const C_SyvUpPacSecurityCertificatePackageDialog &) &;

   Ui::C_SyvUpPacSecurityCertificatePackageDialog * mpc_Ui;
   //lint -e{1725} Only problematic if copy or assignment is allowed
   stw::opensyde_gui_elements::C_OgePopUpDialog & mrc_ParentDialog;

   void m_InitStaticNames(void) const;
   void m_OkClicked(void);
   void m_CancelClicked(void);
   void m_ReloadLastSavedData(void);
   void m_UpdatePemFileWidget(void);
   void m_HandleAddPemFile(void);
   void m_HandleClearAllPemFiles(void);
   void m_UpdateLabelTitleWithFileCounter(void);
   void m_ReloadLastSavedPassword(void);
   void m_PublicKeyPathClicked(void);
   void m_ShowPassword(void);
   void m_HidePassword(void);
   int32_t m_OnValidateSecureUpdateConfig(void);
   int32_t m_OnValidateSecureAuthenticationConfig(void);
   int32_t m_CheckUpdatePath(void);
   int32_t m_CheckAuthPemFiles(QString & orc_ErrorPath) const;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
