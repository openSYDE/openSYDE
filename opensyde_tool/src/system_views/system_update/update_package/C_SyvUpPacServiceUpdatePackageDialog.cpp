//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Dialog for creating service update package

   Creates service update package

   \copyright   Copyright 2024 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.hpp"

#include "C_SyvUpPacServiceUpdatePackageDialog.hpp"
#include "ui_C_SyvUpPacServiceUpdatePackageDialog.h"

#include "C_GtGetText.hpp"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw::opensyde_gui;
using namespace stw::opensyde_gui_elements;
using namespace stw::opensyde_gui_logic;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

/* -- Global Variables ---------------------------------------------------------------------------------------------- */

/* -- Module Global Variables --------------------------------------------------------------------------------------- */

/* -- Module Global Function Prototypes ----------------------------------------------------------------------------- */

/* -- Implementation ------------------------------------------------------------------------------------------------ */

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default constructor

   Set up GUI with all elements.

   \param[in,out] orc_Parent Optional pointer to parent
*/
//----------------------------------------------------------------------------------------------------------------------
C_SyvUpPacServiceUpdatePackageDialog::C_SyvUpPacServiceUpdatePackageDialog(
   stw::opensyde_gui_elements::C_OgePopUpDialog & orc_Parent) :
   QWidget(&orc_Parent),
   mpc_Ui(new Ui::C_SyvUpPacServiceUpdatePackageDialog),
   mrc_ParentDialog(orc_Parent)
{
   this->mpc_Ui->setupUi(this);
   this->InitStaticNames();

   // register the widget for display
   this->mrc_ParentDialog.SetWidget(this);

   // set default option
   this->mpc_Ui->pc_RbArchiveFile->setChecked(true);

   this->mpc_Ui->pc_CbxFileVersion->addItem("Version 1");
   this->mpc_Ui->pc_CbxFileVersion->addItem("Version 2");

   this->mpc_Ui->pc_CbxFileVersion->setCurrentIndex(1);

   // connects
   connect(this->mpc_Ui->pc_PushButtonOk, &QPushButton::clicked,
           this, &C_SyvUpPacServiceUpdatePackageDialog::m_OkClicked);
   connect(this->mpc_Ui->pc_PushButtonCancel, &QPushButton::clicked,
           this, &C_SyvUpPacServiceUpdatePackageDialog::m_CancelClicked);

   connect(this->mpc_Ui->pc_RbArchiveFile, &QRadioButton::toggled, this,
           &C_SyvUpPacServiceUpdatePackageDialog::m_OnRadioButtonToggled);
   connect(this->mpc_Ui->pc_RbUnzip, &QRadioButton::toggled, this,
           &C_SyvUpPacServiceUpdatePackageDialog::m_OnRadioButtonToggled);
   connect(this->mpc_Ui->pc_RbSecureArchive, &QRadioButton::toggled, this,
           &C_SyvUpPacServiceUpdatePackageDialog::m_OnRadioButtonToggled);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default destructor
*/

//----------------------------------------------------------------------------------------------------------------------
C_SyvUpPacServiceUpdatePackageDialog::~C_SyvUpPacServiceUpdatePackageDialog()
{
   delete this->mpc_Ui;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Initializes all visible strings on the widget
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvUpPacServiceUpdatePackageDialog::InitStaticNames() const
{
   this->mrc_ParentDialog.SetTitle(C_GtGetText::h_GetText("Update Package "));
   this->mrc_ParentDialog.SetSubTitle(C_GtGetText::h_GetText("Create Service Update Package"));
   this->mpc_Ui->pc_PushButtonOk->setText(C_GtGetText::h_GetText("Continue"));
   this->mpc_Ui->pc_PushButtonCancel->setText(C_GtGetText::h_GetText("Cancel"));
   this->mpc_Ui->pc_RbArchiveFile->setText((C_GtGetText::h_GetText("Archive File (*.syde_sup)")));
   this->mpc_Ui->pc_RbSecureArchive->setText((C_GtGetText::h_GetText("Secure Archive File (*.syde_sup)")));
   this->mpc_Ui->pc_RbUnzip->setText((C_GtGetText::h_GetText("Un-zipped to a Directory")));
   this->mpc_Ui->pc_LabelType->setText((C_GtGetText::h_GetText("Select Type")));
   this->mpc_Ui->pc_LabFileVersion->setText((C_GtGetText::h_GetText("File Version")));

   // Tool tips
   this->mpc_Ui->pc_RbArchiveFile->SetToolTipInformation(C_GtGetText::h_GetText("Archive File"),
                                                         C_GtGetText::h_GetText(
                                                            "Saves the Service Update Package as archive, here the files are saved as a zipped archive with the file extension .syde_sup."));
   this->mpc_Ui->pc_LabFileVersion->SetToolTipInformation(C_GtGetText::h_GetText("*.syde_sup File Version"),
                                                          C_GtGetText::h_GetText(
                                                             "Defines the file version of the .syde_sup package. \n"
                                                             "Version 1: use this version for compatibility with older SYDEsup tools, which do not support Version 2.\n"
                                                             "Version 2: Default version to use. Supports the complete feature set."));
   this->mpc_Ui->pc_RbUnzip->SetToolTipInformation(C_GtGetText::h_GetText("Directory"),
                                                   C_GtGetText::h_GetText(
                                                      "Saves the Service Update Package as a directory, in which all files are present individually. \n"
                                                      "Here, individual files such as HEX-files can be easily exchanged. Individual files can still be edited directly, "
                                                      "e.g. changing node IDs, without having to generate a new Service Update Package via the openSYDE PC Tool."));
   this->mpc_Ui->pc_RbSecureArchive->SetToolTipInformation(C_GtGetText::h_GetText(
                                                              "Secure Archive File"),
                                                           C_GtGetText::h_GetText(
                                                              "Saves the Service Update Package as a signed archive .syde_sup. "
                                                              "\nThis option allows the user to sign the archive with a private signature key and also encrypt the archive content with a password."));
}
//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get the selected sydesup version

   \return
   Version 1 ..... returns when version 1 is selected
   Version 2 ..... returns when version 2 is selected
*/
//----------------------------------------------------------------------------------------------------------------------
QString C_SyvUpPacServiceUpdatePackageDialog::GetSelectedVersion() const
{
   return this->mpc_Ui->pc_CbxFileVersion->currentText();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get the selected save package type
 *
 *  \return
 *  The type of service package to be created
*/
//----------------------------------------------------------------------------------------------------------------------
C_SyvUpPacServiceUpdatePackageDialog::E_PackageType C_SyvUpPacServiceUpdatePackageDialog::GetSelectedOption()
const
{
   E_PackageType e_Type;

   if (this->mpc_Ui->pc_RbArchiveFile->isChecked())
   {
      e_Type = E_PackageType::eARCHIVE;
   }
   else if (this->mpc_Ui->pc_RbUnzip->isChecked())
   {
      e_Type =  E_PackageType::eDIRECTORY;
   }
   else if (this->mpc_Ui->pc_RbSecureArchive->isChecked())
   {
      e_Type =  E_PackageType::eSECURE_ARCHIVE;
   }
   else
   {
      e_Type =  E_PackageType::eARCHIVE;
   }
   return e_Type;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Slot of OK button click
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvUpPacServiceUpdatePackageDialog::m_OkClicked(void) const
{
   this->mrc_ParentDialog.accept();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Slot of Cancel button
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvUpPacServiceUpdatePackageDialog::m_CancelClicked(void) const
{
   this->mrc_ParentDialog.reject();
}
//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Slot for Radio button's toggle
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvUpPacServiceUpdatePackageDialog::m_OnRadioButtonToggled()
{
   const C_SyvUpPacServiceUpdatePackageDialog::E_PackageType e_Type = this->GetSelectedOption();

   switch (e_Type)
   {
   case E_PackageType::eARCHIVE:
      this->mpc_Ui->pc_CbxFileVersion->setEnabled(true);
      break;
   case E_PackageType::eDIRECTORY:
      this->mpc_Ui->pc_CbxFileVersion->setEnabled(false);
      break;
   case E_PackageType::eSECURE_ARCHIVE:
      this->mpc_Ui->pc_CbxFileVersion->setEnabled(false);
      break;
   default:
      this->mpc_Ui->pc_CbxFileVersion->setEnabled(true);
      break;
   }
}
