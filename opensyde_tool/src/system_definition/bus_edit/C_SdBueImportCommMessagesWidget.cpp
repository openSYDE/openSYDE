//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Popup Widget to import the COMM messages

   \copyright   Copyright 2024 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.hpp"

#include "C_SdBueImportCommMessagesWidget.hpp"
#include "C_OscCanProtocol.hpp"
#include "C_PuiSdUtil.hpp"
#include "ui_C_SdBueImportCommMessagesWidget.h"

#include "C_SdBueComIfDescriptionWidget.hpp"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw::opensyde_core;
using namespace stw::opensyde_gui;
using namespace stw::opensyde_gui_elements;
using namespace stw::scl;
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

   \param[in]    orc_Parent    Defines C_OgePopUpDialog
   \param[in]    orc_Name      Title name
*/
//----------------------------------------------------------------------------------------------------------------------
C_SdBueImportCommMessagesWidget::C_SdBueImportCommMessagesWidget(C_OgePopUpDialog & orc_Parent,
                                                                 const C_SclString & orc_Name) :
   QWidget(&orc_Parent),
   mpc_Ui(new Ui::C_SdImportCommMessagesWidget),
   mrc_ParentDialog(orc_Parent),
   me_SelectedCommProtocol(C_OscCanProtocol::E_Type::eLAYER2)
{
   const uint8_t u8_DEFAULT_COMBOBOX_ENTRY = 0;

   this->mpc_Ui->setupUi(this);

   // register the widget for showing
   this->mrc_ParentDialog.SetWidget(this);

   // set model for view (important!)
   this->m_InitStaticNames(orc_Name);
   this->m_AddPrototcolsToCombobox();

   // connects
   connect(this->mpc_Ui->pc_PushButtonOk, &QPushButton::clicked, this,
           &C_SdBueImportCommMessagesWidget::m_OkClicked);
   connect(this->mpc_Ui->pc_PushButtonCancel, &QPushButton::clicked, this,
           &C_SdBueImportCommMessagesWidget::m_CancelClicked);

   //Set default index of the combobox
   this->mpc_Ui->comboBox->setCurrentIndex(u8_DEFAULT_COMBOBOX_ENTRY);

   this->mpc_Ui->pc_PushButtonOk->setFocus();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default destructor
*/
//----------------------------------------------------------------------------------------------------------------------
C_SdBueImportCommMessagesWidget::~C_SdBueImportCommMessagesWidget()
{
   delete this->mpc_Ui;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Initialize all displayed static names

    \param[in]  orc_Name    Defines title of the popup window

*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueImportCommMessagesWidget::m_InitStaticNames(const C_SclString & orc_Name) const
{
   const QString c_Title(orc_Name.c_str());

   this->mrc_ParentDialog.SetTitle(c_Title);
   this->mrc_ParentDialog.SetSubTitle(C_GtGetText::h_GetText("Import Messages"));

   // labels and buttons

   this->mpc_Ui->pc_LabelHeadingReport->setText(C_GtGetText::h_GetText(
                                                   "Select Protocol"));
   this->mpc_Ui->pc_LabelCommMessageProtocol->setText(C_GtGetText::h_GetText(
                                                         "COMM Messages Protocol"));
   this->mpc_Ui->pc_PushButtonOk->setText(C_GtGetText::h_GetText("Continue"));
   this->mpc_Ui->pc_PushButtonCancel->setText(C_GtGetText::h_GetText("Cancel"));

   // Tool tips
   this->mpc_Ui->pc_LabelCommMessageProtocol->SetToolTipInformation(
      C_GtGetText::h_GetText("COMM Messages Protocol"),
      C_GtGetText::h_GetText("Select the COMM protocol where to import the messages."));
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Add possible options for combobox selection
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueImportCommMessagesWidget::m_AddPrototcolsToCombobox()
{
   const std::vector<C_OscCanProtocol::E_Type> & rc_AvailableCanProtocols = C_OscCanProtocol::hc_ALL_PROTOCOLS;

   for (uint32_t u32_ProtocolIndex = 0; u32_ProtocolIndex < rc_AvailableCanProtocols.size(); ++u32_ProtocolIndex)
   {
      const C_OscCanProtocol::E_Type e_AvailableCanProtocol = rc_AvailableCanProtocols[u32_ProtocolIndex];
      const int32_t s32_Index = C_SdBueComIfDescriptionWidget::h_GetIndexOfProtocol(e_AvailableCanProtocol);

      const QString c_ProtocolName = C_PuiSdUtil::h_ConvertProtocolTypeToString(e_AvailableCanProtocol);
      this->mpc_Ui->comboBox->insertItem(s32_Index, c_ProtocolName);
   }
}
//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Set default index text of the combobox.

   \param[in]       ore_ProtocolType     Communication protocol type
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueImportCommMessagesWidget::SetDefaultCommunicationProtocol(const C_OscCanProtocol::E_Type & ore_ProtocolType)
{
   const QString c_CurrentProtocolViewName = C_PuiSdUtil::h_ConvertProtocolTypeToString(ore_ProtocolType);

   this->mpc_Ui->comboBox->setCurrentText(c_CurrentProtocolViewName);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get selected communication protocol type

   \return
   Selected communication protocol
*/
//----------------------------------------------------------------------------------------------------------------------
C_OscCanProtocol::E_Type C_SdBueImportCommMessagesWidget::GetSelectedCommunicationProtocol(void) const
{
   return me_SelectedCommProtocol;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Overwritten key press event slot

   Here: Handle specific enter key cases

   \param[in] opc_KeyEvent Event identification and information
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueImportCommMessagesWidget::keyPressEvent(QKeyEvent * const opc_KeyEvent)
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
/*! \brief  Green/Ok/Continue button has been clicked

   Get current combobox selection, close the dialog, and emit the selected protocol
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueImportCommMessagesWidget::m_OkClicked(void)
{
   const uint32_t u32_SelectedCommProtocolIndex = this->mpc_Ui->comboBox->currentIndex();

   me_SelectedCommProtocol = C_SdBueComIfDescriptionWidget::h_GetProtocolOfIndex(u32_SelectedCommProtocolIndex);
   mrc_ParentDialog.accept();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Close button has been clicked
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueImportCommMessagesWidget::m_CancelClicked(void)
{
   mrc_ParentDialog.reject();
}
