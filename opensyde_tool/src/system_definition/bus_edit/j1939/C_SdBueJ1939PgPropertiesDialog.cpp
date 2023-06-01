//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Dialog for showing and editing of all J1939 specific PG properties

   Dialog for showing and editing of all J1939 specific PG properties

   \copyright   Copyright 2022 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.hpp"

#include "stwtypes.hpp"
#include "C_GtGetText.hpp"
#include "C_SdBueJ1939PgPropertiesDialog.hpp"
#include "ui_C_SdBueJ1939PgPropertiesDialog.h"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw::opensyde_gui;
using namespace stw::opensyde_gui_logic;
using namespace stw::opensyde_gui_elements;
using namespace stw::opensyde_core;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

/* -- Global Variables ---------------------------------------------------------------------------------------------- */

/* -- Module Global Variables --------------------------------------------------------------------------------------- */

/* -- Module Global Function Prototypes ----------------------------------------------------------------------------- */

/* -- Implementation ------------------------------------------------------------------------------------------------ */

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default constructor

   Set up GUI with all elements.

   \param[in]     ou32_CanId        Current CAN ID
   \param[in,out] orc_Parent        Reference to parent
   \param[in]     orc_MessageName   Message name
*/
//----------------------------------------------------------------------------------------------------------------------
C_SdBueJ1939PgPropertiesDialog::C_SdBueJ1939PgPropertiesDialog(const uint32_t ou32_CanId, C_OgePopUpDialog & orc_Parent,
                                                               const QString & orc_MessageName) :
   QWidget(&orc_Parent),
   mpc_Ui(new Ui::C_SdBueJ1939PgPropertiesDialog),
   mrc_ParentDialog(orc_Parent),
   mc_MessageName(orc_MessageName)
{
   this->mpc_Ui->setupUi(this);

   InitStaticNames();

   // Get the PG information from the CAN ID
   C_OscCanUtil::h_GetJ1939PgInfoFromCanId(ou32_CanId, this->mc_PgInfo);

   // Initialize UI elements
   this->mpc_Ui->pc_SpinBoxPgn->SetMinimumCustom(0);
   this->mpc_Ui->pc_SpinBoxPgn->SetMaximumCustom(262143);
   this->mpc_Ui->pc_SpinBoxPgn->setKeyboardTracking(false);

   this->mpc_Ui->pc_SpinBoxSourceAddress->SetMinimumCustom(0);
   this->mpc_Ui->pc_SpinBoxSourceAddress->SetMaximumCustom(255);

   this->mpc_Ui->pc_SpinBoxDestinationAddress->SetMinimumCustom(0);
   this->mpc_Ui->pc_SpinBoxDestinationAddress->SetMaximumCustom(255);
   this->mpc_Ui->pc_SpinBoxDestinationAddress->setKeyboardTracking(false);

   this->m_HandleDestinationSectionVisibility(this->mc_PgInfo.q_HasDestinationAddress);

   const QStringList c_PriorityList = {"0", "1", "2", "3", "4", "5", "6", "7"};
   this->mpc_Ui->pc_CbxPriority->addItems(c_PriorityList);

   // Populate UI elements with Pg Info values
   m_InitPgInfo();

   // register the widget for showing
   this->mrc_ParentDialog.SetWidget(this);

   //lint -e{929} Cast required to avoid ambiguous signal of qt interface
   connect(this->mpc_Ui->pc_SpinBoxPgn, static_cast<void (QSpinBox::*)(int32_t)>(&QSpinBox::valueChanged), this,
           &C_SdBueJ1939PgPropertiesDialog::m_PgnValueChanged);
   connect(this->mpc_Ui->pc_PushButtonOk, &QPushButton::clicked, this, &C_SdBueJ1939PgPropertiesDialog::m_OkClicked);
   connect(this->mpc_Ui->pc_PushButtonCancel, &QPushButton::clicked, this,
           &C_SdBueJ1939PgPropertiesDialog::m_CancelClicked);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default destructor
*/
//----------------------------------------------------------------------------------------------------------------------
C_SdBueJ1939PgPropertiesDialog::~C_SdBueJ1939PgPropertiesDialog(void)
{
   delete this->mpc_Ui;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Initialize all displayed static names
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueJ1939PgPropertiesDialog::InitStaticNames(void) const
{
   this->mrc_ParentDialog.SetTitle(C_GtGetText::h_GetText(mc_MessageName.toStdString().c_str()));
   this->mrc_ParentDialog.SetSubTitle(C_GtGetText::h_GetText("J1939 CAN Settings"));
   this->mpc_Ui->pc_LabelHeadingPreview->setText(C_GtGetText::h_GetText("Settings"));
   this->mpc_Ui->pc_LabelPgn->setText(C_GtGetText::h_GetText("PGN"));
   this->mpc_Ui->pc_LabelPriority->setText(C_GtGetText::h_GetText("Priority"));
   this->mpc_Ui->pc_LabelSourceAddress->setText(C_GtGetText::h_GetText("Source Address"));
   this->mpc_Ui->pc_LabelDestinationAddress->setText(C_GtGetText::h_GetText("Destination Address"));
   this->mpc_Ui->pc_PushButtonOk->setText(C_GtGetText::h_GetText("OK"));
   this->mpc_Ui->pc_PushButtonCancel->setText(C_GtGetText::h_GetText("Cancel"));
   this->mpc_Ui->pc_LabelDestinationAddressBroadcast->setText(C_GtGetText::h_GetText("All"));

   // Set tool tip info
   this->mpc_Ui->pc_LabelPgn->SetToolTipInformation(C_GtGetText::h_GetText("PGN"),
                                                    C_GtGetText::h_GetText(
                                                       "J1939 Parameter Group Number"));
   this->mpc_Ui->pc_LabelPriority->SetToolTipInformation(C_GtGetText::h_GetText("Priority"),
                                                         C_GtGetText::h_GetText(
                                                            "The priority for this message"));
   this->mpc_Ui->pc_LabelSourceAddress->SetToolTipInformation(C_GtGetText::h_GetText(
                                                                 "Source Address"),
                                                              C_GtGetText::h_GetText(
                                                                 "Address of source node"));
   this->mpc_Ui->pc_LabelDestinationAddress->SetToolTipInformation(C_GtGetText::h_GetText(
                                                                      "Destination Address"),
                                                                   C_GtGetText::h_GetText(
                                                                      "Address of target node"));
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Returns the CAN ID filled with the changed PG information

   \return
   New CAN ID
*/
//----------------------------------------------------------------------------------------------------------------------
uint32_t C_SdBueJ1939PgPropertiesDialog::GetNewCanId(void) const
{
   uint32_t u32_CanId;

   stw::opensyde_core::C_OscCanUtilJ1939PgInfo c_PgInfo = this->mc_PgInfo;

   //Fill the mc_PgInfo with the modified information
   m_SetPgInfo(c_PgInfo);

   C_OscCanUtil::h_GetCanIdFromJ1939PgInfo(c_PgInfo, u32_CanId);

   return u32_CanId;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Overwritten key press event slot

   Here: Handle specific enter key cases

   \param[in,out] opc_KeyEvent Event identification and information
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueJ1939PgPropertiesDialog::keyPressEvent(QKeyEvent * const opc_KeyEvent)
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
void C_SdBueJ1939PgPropertiesDialog::m_OkClicked(void)
{
   this->mrc_ParentDialog.accept();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Slot of Cancel button click
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueJ1939PgPropertiesDialog::m_CancelClicked(void)
{
   this->mrc_ParentDialog.reject();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Calculate existence of destination address on the basis of current PGN

   \retval
   true     PGN contains destination address (Point-to-Point)
   false    PGN does not contain destination address (Broadcast)
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_SdBueJ1939PgPropertiesDialog::m_HasDestinationAddress(void) const
{
   const uint32_t u32_Pgn = static_cast<uint32_t>(this->mpc_Ui->pc_SpinBoxPgn->value());
   const uint8_t u8_PduFormat = static_cast<uint8_t>((u32_Pgn & 0x0000FF00U) >> 8U);
   const bool q_HasDestinationAddress = (u8_PduFormat <= 0xEFU);

   return q_HasDestinationAddress;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Pgn value changed
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueJ1939PgPropertiesDialog::m_PgnValueChanged()
{
   this->m_HandleDestinationSectionVisibility(this->m_HasDestinationAddress());
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Handle destination section visibility

   \param[in]  oq_HasDestinationAddress   Has destination address
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueJ1939PgPropertiesDialog::m_HandleDestinationSectionVisibility(const bool oq_HasDestinationAddress)
{
   // if Destination Address exists, display Spinbox (Point-to-Point), else Label for Broadcast
   if (oq_HasDestinationAddress)
   {
      this->mpc_Ui->pc_SpinBoxDestinationAddress->setVisible(true);
      this->mpc_Ui->pc_LabelDestinationAddressBroadcast->setVisible(false);
   }
   else
   {
      this->mpc_Ui->pc_SpinBoxDestinationAddress->setVisible(false);
      this->mpc_Ui->pc_LabelDestinationAddressBroadcast->setVisible(true);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Initialize UI elements with PG Info
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueJ1939PgPropertiesDialog::m_InitPgInfo(void) const
{
   const uint32_t u32_VisiblePgn = C_OscCanUtil::h_GetVisiblePgn(this->mc_PgInfo.u32_Pgn);

   this->mpc_Ui->pc_CbxPriority->setCurrentText(QString::number(this->mc_PgInfo.u8_Priority));
   this->mpc_Ui->pc_SpinBoxSourceAddress->setValue(this->mc_PgInfo.u8_SourceAddress);
   this->mpc_Ui->pc_SpinBoxPgn->setValue(u32_VisiblePgn);

   // Initialize destination address for PDU 1 (Point-to-Point)
   if (this->mc_PgInfo.q_HasDestinationAddress)
   {
      this->mpc_Ui->pc_SpinBoxDestinationAddress->setValue(this->mc_PgInfo.u8_PduSpecific);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Fetch PG Info from UI elements
 *
 *   \param[in,out]     orc_PgInfo   Reference to Pg Information
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueJ1939PgPropertiesDialog::m_SetPgInfo(stw::opensyde_core::C_OscCanUtilJ1939PgInfo & orc_PgInfo) const
{
   uint32_t u32_VisiblePgn = this->mpc_Ui->pc_SpinBoxPgn->value();

   if (m_HasDestinationAddress() == true)
   {
      const uint8_t u8_DestinationAddress =
         (static_cast<uint8_t>(this->mpc_Ui->pc_SpinBoxDestinationAddress->value()));

      u32_VisiblePgn = (u32_VisiblePgn) | (static_cast<uint32_t>(u8_DestinationAddress));
   }

   orc_PgInfo.SetPgn(u32_VisiblePgn);
   orc_PgInfo.u8_Priority = static_cast<uint8_t>(this->mpc_Ui->pc_CbxPriority->currentText().toUInt());
   orc_PgInfo.u8_SourceAddress = static_cast<uint8_t>(this->mpc_Ui->pc_SpinBoxSourceAddress->value());
}
