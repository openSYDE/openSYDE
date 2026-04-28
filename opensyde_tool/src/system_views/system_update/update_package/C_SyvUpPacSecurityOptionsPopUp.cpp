//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Popup dialog for security options

   Let the user select to activate/deactivate/do not change security options like authentication
   debugger and traffic encryption.

   \copyright   Copyright 2021 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.hpp"

#include "stwtypes.hpp"
#include "TglUtils.hpp"
#include "C_GtGetText.hpp"
#include "C_SyvUpPacSecurityOptionsPopUp.hpp"
#include "ui_C_SyvUpPacSecurityOptionsPopUp.h"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw::tgl;
using namespace stw::opensyde_core;
using namespace stw::opensyde_gui;
using namespace stw::opensyde_gui_logic;
using namespace stw::opensyde_gui_elements;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */
const int32_t C_SyvUpPacSecurityOptionsPopUp::mhs32_SEC_INDEX_NO_CHANGE = 0;
const int32_t C_SyvUpPacSecurityOptionsPopUp::mhs32_SEC_INDEX_ACTIVATE = 1;
const int32_t C_SyvUpPacSecurityOptionsPopUp::mhs32_SEC_INDEX_DEACTIVATE = 2;
const int32_t C_SyvUpPacSecurityOptionsPopUp::mhs32_DEB_INDEX_NO_CHANGE = 0;
const int32_t C_SyvUpPacSecurityOptionsPopUp::mhs32_DEB_INDEX_ACTIVATE = 1;
const int32_t C_SyvUpPacSecurityOptionsPopUp::mhs32_DEB_INDEX_DEACTIVATE = 2;
const int32_t C_SyvUpPacSecurityOptionsPopUp::mhs32_TEN_INDEX_NO_CHANGE = 0;
const int32_t C_SyvUpPacSecurityOptionsPopUp::mhs32_TEN_INDEX_ACTIVATE = 1;
const int32_t C_SyvUpPacSecurityOptionsPopUp::mhs32_TEN_INDEX_DEACTIVATE = 2;

/* -- Types --------------------------------------------------------------------------------------------------------- */

/* -- Global Variables ---------------------------------------------------------------------------------------------- */

/* -- Module Global Variables --------------------------------------------------------------------------------------- */

/* -- Module Global Function Prototypes ----------------------------------------------------------------------------- */

/* -- Implementation ------------------------------------------------------------------------------------------------ */

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default constructor

   Set up GUI with all elements.

   \param[in,out]  orc_Parent                      Reference to parent
   \param[in]      oe_StateSecureAuthentication    State authentication
   \param[in]      oe_StateDebugger                State debugger
   \param[in]      oe_StateTrafficEncryption       State encryption
*/
//----------------------------------------------------------------------------------------------------------------------
C_SyvUpPacSecurityOptionsPopUp::C_SyvUpPacSecurityOptionsPopUp(
   stw::opensyde_gui_elements::C_OgePopUpDialog & orc_Parent,
   const C_OscViewNodeUpdate::E_StateSecureAuthentication oe_StateSecureAuthentication,
   const C_OscViewNodeUpdate::E_StateDebugger oe_StateDebugger,
   const C_OscViewNodeUpdate::E_StateTrafficEncryption oe_StateTrafficEncryption, const QString & orc_NodeName) :
   QWidget(&orc_Parent),
   mpc_Ui(new Ui::C_SyvUpPacSecurityOptionsPopUp),
   mrc_ParentDialog(orc_Parent),
   mc_NodeName(orc_NodeName)
{
   this->mpc_Ui->setupUi(this);

   InitStaticNames();

   this->m_InitComboBoxSec(oe_StateSecureAuthentication);
   this->m_InitComboBoxDeb(oe_StateDebugger);
   this->m_InitComboBoxTen(oe_StateTrafficEncryption);

   // register the widget for showing
   this->mrc_ParentDialog.SetWidget(this);

   connect(this->mpc_Ui->pc_PushButtonOk, &QPushButton::clicked, this, &C_SyvUpPacSecurityOptionsPopUp::m_OkClicked);
   connect(this->mpc_Ui->pc_PushButtonCancel, &QPushButton::clicked, this,
           &C_SyvUpPacSecurityOptionsPopUp::m_CancelClicked);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default destructor
*/
//----------------------------------------------------------------------------------------------------------------------
C_SyvUpPacSecurityOptionsPopUp::~C_SyvUpPacSecurityOptionsPopUp(void)
{
   delete this->mpc_Ui;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Initialize all displayed static names
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvUpPacSecurityOptionsPopUp::InitStaticNames(void) const
{
   const QString c_ToolTipContent = C_GtGetText::h_GetText("No Change: Do nothing\n"
                                                           "Activate: Enable %1 on next system update\n"
                                                           "Deactivate: Disable %1 on next system update\n");

   this->mrc_ParentDialog.SetTitle(mc_NodeName);
   this->mrc_ParentDialog.SetSubTitle(C_GtGetText::h_GetText("Security Settings"));
   this->mpc_Ui->pc_LabelHeading->setText(C_GtGetText::h_GetText("Services"));
   this->mpc_Ui->pc_LabelAuth->setText(C_GtGetText::h_GetText("Activate Authentication"));
   this->mpc_Ui->pc_LabelDebugger->setText(C_GtGetText::h_GetText("Activate Debugger"));
   this->mpc_Ui->pc_LabelTrafficEncryption->setText(C_GtGetText::h_GetText("Activate Traffic Encryption"));

   this->mpc_Ui->pc_PushButtonCancel->setText(C_GtGetText::h_GetText("Cancel"));
   this->mpc_Ui->pc_PushButtonOk->setText(C_GtGetText::h_GetText("OK"));

   //Combo Box
   this->mpc_Ui->pc_ComboBoxAuth->addItem(C_GtGetText::h_GetText("No Change"));
   this->mpc_Ui->pc_ComboBoxAuth->addItem(C_GtGetText::h_GetText("Activate"));
   this->mpc_Ui->pc_ComboBoxAuth->addItem(C_GtGetText::h_GetText("Deactivate"));
   this->mpc_Ui->pc_ComboBoxDebugger->addItem(C_GtGetText::h_GetText("No Change"));
   this->mpc_Ui->pc_ComboBoxDebugger->addItem(C_GtGetText::h_GetText("Activate"));
   this->mpc_Ui->pc_ComboBoxDebugger->addItem(C_GtGetText::h_GetText("Deactivate"));
   this->mpc_Ui->pc_ComboBoxTrafficEncryption->addItem(C_GtGetText::h_GetText("No Change"));
   this->mpc_Ui->pc_ComboBoxTrafficEncryption->addItem(C_GtGetText::h_GetText("Activate"));
   this->mpc_Ui->pc_ComboBoxTrafficEncryption->addItem(C_GtGetText::h_GetText("Deactivate"));

   //ToolTips
   this->mpc_Ui->pc_LabelAuth->SetToolTipInformation(
      C_GtGetText::h_GetText("Activate Authentication"),
      c_ToolTipContent.arg(C_GtGetText::h_GetText("authentication")));
   this->mpc_Ui->pc_LabelDebugger->SetToolTipInformation(
      C_GtGetText::h_GetText("Activate Debugger"),
      c_ToolTipContent.arg(C_GtGetText::h_GetText("debugger")));
   this->mpc_Ui->pc_LabelTrafficEncryption->SetToolTipInformation(
      C_GtGetText::h_GetText("Activate Traffic Encryption"),
      c_ToolTipContent.arg(C_GtGetText::h_GetText("traffic encryption")));
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get combo box sec state

   \return
   Combo box sec state
*/
//----------------------------------------------------------------------------------------------------------------------
C_OscViewNodeUpdate::E_StateSecureAuthentication C_SyvUpPacSecurityOptionsPopUp::GetComboBoxSecState() const
{
   C_OscViewNodeUpdate::E_StateSecureAuthentication e_Retval = C_OscViewNodeUpdate::eST_SEC_NO_CHANGE;
   switch (this->mpc_Ui->pc_ComboBoxAuth->currentIndex())
   {
   case C_SyvUpPacSecurityOptionsPopUp::mhs32_SEC_INDEX_NO_CHANGE:
      e_Retval = C_OscViewNodeUpdate::eST_SEC_NO_CHANGE;
      break;
   case C_SyvUpPacSecurityOptionsPopUp::mhs32_SEC_INDEX_ACTIVATE:
      e_Retval = C_OscViewNodeUpdate::eST_SEC_ACTIVATE;
      break;
   case C_SyvUpPacSecurityOptionsPopUp::mhs32_SEC_INDEX_DEACTIVATE:
      e_Retval = C_OscViewNodeUpdate::eST_SEC_DEACTIVATE;
      break;
   default:
      tgl_assert(false);
      break;
   }
   return e_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get combo box deb state

   \return
   Combo box deb state
*/
//----------------------------------------------------------------------------------------------------------------------
C_OscViewNodeUpdate::E_StateDebugger C_SyvUpPacSecurityOptionsPopUp::GetComboBoxDebState() const
{
   C_OscViewNodeUpdate::E_StateDebugger e_Retval = C_OscViewNodeUpdate::eST_DEB_NO_CHANGE;
   switch (this->mpc_Ui->pc_ComboBoxDebugger->currentIndex())
   {
   case C_SyvUpPacSecurityOptionsPopUp::mhs32_DEB_INDEX_NO_CHANGE:
      e_Retval = C_OscViewNodeUpdate::eST_DEB_NO_CHANGE;
      break;
   case C_SyvUpPacSecurityOptionsPopUp::mhs32_DEB_INDEX_ACTIVATE:
      e_Retval = C_OscViewNodeUpdate::eST_DEB_ACTIVATE;
      break;
   case C_SyvUpPacSecurityOptionsPopUp::mhs32_DEB_INDEX_DEACTIVATE:
      e_Retval = C_OscViewNodeUpdate::eST_DEB_DEACTIVATE;
      break;
   default:
      tgl_assert(false);
      break;
   }
   return e_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get combo box ten state

   \return
   Combo box ten state
*/
//----------------------------------------------------------------------------------------------------------------------
C_OscViewNodeUpdate::E_StateTrafficEncryption C_SyvUpPacSecurityOptionsPopUp::GetComboBoxTenState() const
{
   C_OscViewNodeUpdate::E_StateTrafficEncryption e_Retval = C_OscViewNodeUpdate::eST_TEN_NO_CHANGE;
   switch (this->mpc_Ui->pc_ComboBoxTrafficEncryption->currentIndex())
   {
   case C_SyvUpPacSecurityOptionsPopUp::mhs32_TEN_INDEX_NO_CHANGE:
      e_Retval = C_OscViewNodeUpdate::eST_TEN_NO_CHANGE;
      break;
   case C_SyvUpPacSecurityOptionsPopUp::mhs32_TEN_INDEX_ACTIVATE:
      e_Retval = C_OscViewNodeUpdate::eST_TEN_ACTIVATE;
      break;
   case C_SyvUpPacSecurityOptionsPopUp::mhs32_TEN_INDEX_DEACTIVATE:
      e_Retval = C_OscViewNodeUpdate::eST_TEN_DEACTIVATE;
      break;
   default:
      tgl_assert(false);
      break;
   }
   return e_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Overwritten key press event slot

   Here: Handle specific enter key cases

   \param[in,out]  opc_KeyEvent  Event identification and information
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvUpPacSecurityOptionsPopUp::keyPressEvent(QKeyEvent * const opc_KeyEvent)
{
   bool q_CallOrg = true;

   //Handle all enter key cases manually
   if ((opc_KeyEvent->key() == static_cast<int32_t>(Qt::Key_Enter)) ||
       (opc_KeyEvent->key() == static_cast<int32_t>(Qt::Key_Return)))
   {
      if (((opc_KeyEvent->modifiers().testFlag(Qt::ControlModifier) == true) &&
           (opc_KeyEvent->modifiers().testFlag(Qt::AltModifier) == false)) &&
          (opc_KeyEvent->modifiers().testFlag(Qt::ShiftModifier) == false))
      {
         this->mrc_ParentDialog.accept();
      }
      else
      {
         q_CallOrg = false;
      }
   }
   if (q_CallOrg == true)
   {
      QWidget::keyPressEvent(opc_KeyEvent);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Slot of Ok button click
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvUpPacSecurityOptionsPopUp::m_OkClicked(void)
{
   this->mrc_ParentDialog.accept();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Slot of Cancel button click
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvUpPacSecurityOptionsPopUp::m_CancelClicked()
{
   this->mrc_ParentDialog.reject();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Init combo box sec

   \param[in]  oe_StateSecureAuthentication  State security
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvUpPacSecurityOptionsPopUp::m_InitComboBoxSec(
   const C_OscViewNodeUpdate::E_StateSecureAuthentication oe_StateSecureAuthentication)
{
   //lint -e{9042} Warning wanted if new cases are added
   switch (oe_StateSecureAuthentication)
   {
   case C_OscViewNodeUpdate::eST_SEC_NO_CHANGE:
      this->mpc_Ui->pc_ComboBoxAuth->setCurrentIndex(C_SyvUpPacSecurityOptionsPopUp::mhs32_SEC_INDEX_NO_CHANGE);
      break;
   case C_OscViewNodeUpdate::eST_SEC_ACTIVATE:
      this->mpc_Ui->pc_ComboBoxAuth->setCurrentIndex(C_SyvUpPacSecurityOptionsPopUp::mhs32_SEC_INDEX_ACTIVATE);
      break;
   case C_OscViewNodeUpdate::eST_SEC_DEACTIVATE:
      this->mpc_Ui->pc_ComboBoxAuth->setCurrentIndex(C_SyvUpPacSecurityOptionsPopUp::mhs32_SEC_INDEX_DEACTIVATE);
      break;
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Init combo box deb

   \param[in]  oe_StateDebugger  State debugger
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvUpPacSecurityOptionsPopUp::m_InitComboBoxDeb(const C_OscViewNodeUpdate::E_StateDebugger oe_StateDebugger)
{
   //lint -e{9042} Warning wanted if new cases are added
   switch (oe_StateDebugger)
   {
   case C_OscViewNodeUpdate::eST_DEB_NO_CHANGE:
      this->mpc_Ui->pc_ComboBoxDebugger->setCurrentIndex(C_SyvUpPacSecurityOptionsPopUp::mhs32_DEB_INDEX_NO_CHANGE);
      break;
   case C_OscViewNodeUpdate::eST_DEB_ACTIVATE:
      this->mpc_Ui->pc_ComboBoxDebugger->setCurrentIndex(C_SyvUpPacSecurityOptionsPopUp::mhs32_DEB_INDEX_ACTIVATE);
      break;
   case C_OscViewNodeUpdate::eST_DEB_DEACTIVATE:
      this->mpc_Ui->pc_ComboBoxDebugger->setCurrentIndex(C_SyvUpPacSecurityOptionsPopUp::mhs32_DEB_INDEX_DEACTIVATE);
      break;
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Init combo box ten

   \param[in]  oe_StateTrafficEncryption  State Traffic Encryption
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvUpPacSecurityOptionsPopUp::m_InitComboBoxTen(
   const C_OscViewNodeUpdate::E_StateTrafficEncryption oe_StateTrafficEncryption)
{
   //lint -e{9042} Warning wanted if new cases are added
   switch (oe_StateTrafficEncryption)
   {
   case C_OscViewNodeUpdate::eST_TEN_NO_CHANGE:
      this->mpc_Ui->pc_ComboBoxTrafficEncryption->setCurrentIndex(
         C_SyvUpPacSecurityOptionsPopUp::mhs32_TEN_INDEX_NO_CHANGE);
      break;
   case C_OscViewNodeUpdate::eST_TEN_ACTIVATE:
      this->mpc_Ui->pc_ComboBoxTrafficEncryption->setCurrentIndex(
         C_SyvUpPacSecurityOptionsPopUp::mhs32_TEN_INDEX_ACTIVATE);
      break;
   case C_OscViewNodeUpdate::eST_TEN_DEACTIVATE:
      this->mpc_Ui->pc_ComboBoxTrafficEncryption->setCurrentIndex(
         C_SyvUpPacSecurityOptionsPopUp::mhs32_TEN_INDEX_DEACTIVATE);
      break;
   }
}
