//-----------------------------------------------------------------------------
/*!
   \internal
   \file
   \brief       Dialog for system node programming properties (implementation)

   Dialog for system node programming properties.

   \implementation
   project     openSYDE
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     29.03.2018  STW/M.Echtler
   \endimplementation
*/
//-----------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------- */
#include <limits>

#include "stwerrors.h"
#include "TGLUtils.h"
#include "C_GtGetText.h"
#include "C_PuiSdHandler.h"

#include "C_SdNdeProgrammingOptions.h"
#include "ui_C_SdNdeProgrammingOptions.h"

/* -- Used Namespaces ------------------------------------------------------ */
using namespace stw_tgl;
using namespace stw_types;
using namespace stw_errors;
using namespace stw_opensyde_gui;
using namespace stw_opensyde_core;
using namespace stw_opensyde_gui_logic;

/* -- Module Global Constants ---------------------------------------------- */

/* -- Types ---------------------------------------------------------------- */

/* -- Global Variables ----------------------------------------------------- */

/* -- Module Global Variables ---------------------------------------------- */

/* -- Module Global Function Prototypes ------------------------------------ */

/* -- Implementation ------------------------------------------------------- */

//-----------------------------------------------------------------------------
/*!
   \brief   Default constructor

   Set up GUI with all elements.

   \param[in,out] orc_Parent     Reference to parent
   \param[in]     ou32_NodeIndex Node index

   \created     29.03.2018  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
C_SdNdeProgrammingOptions::C_SdNdeProgrammingOptions(stw_opensyde_gui_elements::C_OgePopUpDialog & orc_Parent,
                                                     const uint32 ou32_NodeIndex) :
   QWidget(&orc_Parent),
   mpc_Ui(new Ui::C_SdNdeProgrammingOptions),
   mrc_ParentDialog(orc_Parent),
   mu32_NodeIndex(ou32_NodeIndex)
{
   mpc_Ui->setupUi(this);

   InitStaticNames();

   //Ranges
   this->mpc_Ui->pc_SpinBoxNBR->SetMinimumCustom(0);
   this->mpc_Ui->pc_SpinBoxNBR->SetMaximumCustom(static_cast<sintn>(std::numeric_limits<uint16>::max()));
   this->mpc_Ui->pc_SpinBoxNBT->SetMinimumCustom(0);
   this->mpc_Ui->pc_SpinBoxNBT->SetMaximumCustom(static_cast<sintn>(std::numeric_limits<uint16>::max()));
   this->mpc_Ui->pc_SpinBoxParallelTransmissions->SetMinimumCustom(0);
   this->mpc_Ui->pc_SpinBoxParallelTransmissions->SetMaximumCustom(255);

   //Load AFTER ranges are valid
   m_Load();

   // register the widget for showing
   this->mrc_ParentDialog.SetWidget(this);

   // set main title
   this->mrc_ParentDialog.SetTitle(C_GtGetText::h_GetText("Code Generation"));
   this->mrc_ParentDialog.SetSubTitle(C_GtGetText::h_GetText("Settings"));

   // connects
   connect(this->mpc_Ui->pc_BushButtonOk, &QPushButton::clicked, this, &C_SdNdeProgrammingOptions::m_OkClicked);
   connect(this->mpc_Ui->pc_BushButtonCancel, &QPushButton::clicked, this, &C_SdNdeProgrammingOptions::m_CancelClicked);
}

//-----------------------------------------------------------------------------
/*!
   \brief   Default destructor

   Clean up.

   \created     29.03.2018  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
C_SdNdeProgrammingOptions::~C_SdNdeProgrammingOptions(void)
{
   delete mpc_Ui;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Initialize all displayed static names

   \created     29.03.2018  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_SdNdeProgrammingOptions::InitStaticNames(void) const
{
   this->mpc_Ui->pc_BushButtonOk->setText(C_GtGetText::h_GetText("OK"));
   this->mpc_Ui->pc_BushButtonCancel->setText(C_GtGetText::h_GetText("Cancel"));

   this->mpc_Ui->pc_LabelDPDHeading->setText(C_GtGetText::h_GetText("Diagnostic Protocol Driver"));
   this->mpc_Ui->pc_LabelDPD->setText(C_GtGetText::h_GetText("Diagnostic protocol driver is implemented by"));
   this->mpc_Ui->pc_LabelNBT->setText(C_GtGetText::h_GetText("Number of buffered CAN TX messages"));
   this->mpc_Ui->pc_LabelNBR->setText(C_GtGetText::h_GetText("Number of buffered CAN RX routing messages"));
   this->mpc_Ui->pc_LabelMNO->setText(C_GtGetText::h_GetText(
                                         "Max number of cyclic/event driven transmissions in parallel"));

   this->mpc_Ui->pc_LabelCPD->setText(C_GtGetText::h_GetText("Process Data Exchange Stacks"));
   this->mpc_Ui->pc_LabelOL2->setText(C_GtGetText::h_GetText("OSI Layer 2 protocol stack is implemented by"));
   this->mpc_Ui->pc_LabelEEPD->setText(C_GtGetText::h_GetText("ECES protocol stack is implemented by"));
   this->mpc_Ui->pc_LabelEOPD->setText(C_GtGetText::h_GetText("ECOS protocol stack is implemented by"));

   //Tool tips
   this->mpc_Ui->pc_LabelDPDHeading->SetToolTipInformation(C_GtGetText::h_GetText("Diagnostic Protocol Driver"),
                                                           C_GtGetText::h_GetText(
                                                              "The diagnostic protocol driver provides access to Datapool content via the openSYDE communication protocol."));
   this->mpc_Ui->pc_LabelDPD->SetToolTipInformation(C_GtGetText::h_GetText(
                                                       "Diagnostic protocol driver is implemented by"),
                                                    C_GtGetText::h_GetText(
                                                       "To which application should the diagnostic protocol driver be added?"));
   this->mpc_Ui->pc_LabelNBT->SetToolTipInformation(C_GtGetText::h_GetText(
                                                       "Number of buffered CAN TX messages"),
                                                    C_GtGetText::h_GetText(
                                                       "Maximum number of CAN TX messages the server can buffer. (Used for all transferred CAN messages)"
                                                       "\nDefault value: 585"));
   this->mpc_Ui->pc_LabelNBR->SetToolTipInformation(C_GtGetText::h_GetText(
                                                       "Number of buffered CAN RX routing messages"),
                                                    C_GtGetText::h_GetText(
                                                       "Maximum number of CAN RX routing messages the server can buffer before the client has to wait for an acknowledge."
                                                       "\nDefault value: 585"));
   this->mpc_Ui->pc_LabelMNO->SetToolTipInformation(C_GtGetText::h_GetText(
                                                       "Max number of cyclic/event driven transmissions in parallel"),
                                                    C_GtGetText::h_GetText(
                                                       "This refers to the maximum number of parallel cyclic and event driven diagnostic transmissions you are allowed to configure for this server.\n"
                                                       "This restricts the number transmissions in \"System Commissioning - Dashboards\" screen (openSYDE PC client)."
                                                       "\nDefault value: 16"));

   this->mpc_Ui->pc_LabelCPD->SetToolTipInformation(C_GtGetText::h_GetText("Process Data Exchange Stacks"),
                                                    C_GtGetText::h_GetText(
                                                       "The process data exchange stacks provide drivers for sending and receiving CAN messages containing process data.\n"
                                                       "The contained data is read from resp. written to openSYDE Datapools.\n"
                                                       "Three different communication protocols are supported.\n"));
   this->mpc_Ui->pc_LabelOL2->SetToolTipInformation(C_GtGetText::h_GetText(
                                                       "OSI Layer 2 Protocol Stack is implemented by"),
                                                    C_GtGetText::h_GetText(
                                                       "Which application implements the OSI Layer 2 protocol stack?"));
   this->mpc_Ui->pc_LabelEEPD->SetToolTipInformation(C_GtGetText::h_GetText("ECES protocol stack is implemented by"),
                                                     C_GtGetText::h_GetText(
                                                        "Which application implements the ESX CAN efficient safety protocol stack?"));
   this->mpc_Ui->pc_LabelEOPD->SetToolTipInformation(C_GtGetText::h_GetText("ECOS protocol stack is implemented by"),
                                                     C_GtGetText::h_GetText(
                                                        "Which application implements the ESX CANopen safety protocol stack?"));
}

//-----------------------------------------------------------------------------
/*!
   \brief   Save to RAM data storage

   \created     29.03.2018  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_SdNdeProgrammingOptions::Save(void) const
{
   C_OSCNodeOpenSYDEServerSettings c_Settings;

   //DPD
   if (this->mpc_Ui->pc_ComboBoxDPD->currentIndex() >= 0)
   {
      const C_OSCNode * const pc_Node = C_PuiSdHandler::h_GetInstance()->GetOSCNodeConst(this->mu32_NodeIndex);
      if (pc_Node != NULL)
      {
         sint16 s16_ApplicationIndex = -1;
         sint16 s16_Counter = 0;
         //Map combo box index to data block index
         for (uint32 u32_ItDataBlock = 0; u32_ItDataBlock < pc_Node->c_Applications.size(); ++u32_ItDataBlock)
         {
            const C_OSCNodeApplication & rc_DataBlock = pc_Node->c_Applications[u32_ItDataBlock];
            if (rc_DataBlock.e_Type == C_OSCNodeApplication::ePROGRAMMABLE_APPLICATION)
            {
               if (static_cast<sint16>(this->mpc_Ui->pc_ComboBoxDPD->currentIndex()) == s16_Counter)
               {
                  s16_ApplicationIndex = static_cast<sint16>(u32_ItDataBlock);
               }
               //Important iterator step for counter
               ++s16_Counter;
            }
         }
         c_Settings.s16_DPDDataBlockIndex = s16_ApplicationIndex;
      }
   }
   else
   {
      //Invalid application
      c_Settings.s16_DPDDataBlockIndex = -1;
   }

   //Spin boxes
   c_Settings.u16_MaxMessageBufferTx = static_cast<uint16>(this->mpc_Ui->pc_SpinBoxNBT->value());
   c_Settings.u16_MaxRoutingMessageBufferRx = static_cast<uint16>(this->mpc_Ui->pc_SpinBoxNBR->value());
   c_Settings.u8_MaxParallelTransmissions = static_cast<uint8>(this->mpc_Ui->pc_SpinBoxParallelTransmissions->value());
   tgl_assert(C_PuiSdHandler::h_GetInstance()->SetNodeOpenSYDEServerSettings(this->mu32_NodeIndex,
                                                                             c_Settings) == C_NO_ERR);
}

//-----------------------------------------------------------------------------
/*!
   \brief   Overwritten key press event slot

   Here: Handle specific enter key cases

   \param[in,out] opc_KeyEvent Event identification and information

   \created     29.03.2018  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_SdNdeProgrammingOptions::keyPressEvent(QKeyEvent * const opc_KeyEvent)
{
   bool q_CallOrg = true;

   //Handle all enter key cases manually
   if ((opc_KeyEvent->key() == static_cast<sintn>(Qt::Key_Enter)) ||
       (opc_KeyEvent->key() == static_cast<sintn>(Qt::Key_Return)))
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

//-----------------------------------------------------------------------------
/*!
   \brief   Load dynamic content

   \created     29.03.2018  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_SdNdeProgrammingOptions::m_Load(void) const
{
   const C_OSCNode * const pc_Node = C_PuiSdHandler::h_GetInstance()->GetOSCNodeConst(this->mu32_NodeIndex);

   if (pc_Node != NULL)
   {
      QString c_Text;
      sint32 s32_Counter = 0;
      sint32 s32_DataBlockIndex = -1;
      const C_OSCNodeDataPool * const pc_OSCDataPoolL2 = pc_Node->GetComDataPoolConst(C_OSCCanProtocol::eLAYER2);
      const C_OSCNodeDataPool * const pc_OSCDataPoolEE = pc_Node->GetComDataPoolConst(C_OSCCanProtocol::eECES);
      const C_OSCNodeDataPool * const pc_OSCDataPoolEO =
         pc_Node->GetComDataPoolConst(C_OSCCanProtocol::eCAN_OPEN_SAFETY);
      //DPD
      //Add all programmable applications and map data block index to combo box index
      for (uint32 u32_ItDataBlock = 0; u32_ItDataBlock < pc_Node->c_Applications.size(); ++u32_ItDataBlock)
      {
         const C_OSCNodeApplication & rc_DataBlock = pc_Node->c_Applications[u32_ItDataBlock];
         if (rc_DataBlock.e_Type == C_OSCNodeApplication::ePROGRAMMABLE_APPLICATION)
         {
            this->mpc_Ui->pc_ComboBoxDPD->addItem(rc_DataBlock.c_Name.c_str());
            if ((pc_Node->c_Properties.c_OpenSYDEServerSettings.s16_DPDDataBlockIndex >= 0) &&
                (static_cast<uint32>(static_cast<sint32>(pc_Node->c_Properties.c_OpenSYDEServerSettings.
                                                         s16_DPDDataBlockIndex)) == u32_ItDataBlock))
            {
               s32_DataBlockIndex = s32_Counter;
            }
            else
            {
               //Use first valid index as default
               if ((pc_Node->c_Properties.c_OpenSYDEServerSettings.s16_DPDDataBlockIndex < 0) &&
                   (s32_DataBlockIndex == -1))
               {
                  s32_DataBlockIndex = s32_Counter;
               }
            }
            //Important iteration step
            ++s32_Counter;
         }
      }
      this->mpc_Ui->pc_ComboBoxDPD->setCurrentIndex(s32_DataBlockIndex);

      //Spin boxes
      this->mpc_Ui->pc_SpinBoxNBR->setValue(pc_Node->c_Properties.c_OpenSYDEServerSettings.u16_MaxRoutingMessageBufferRx);
      this->mpc_Ui->pc_SpinBoxNBT->setValue(
         pc_Node->c_Properties.c_OpenSYDEServerSettings.u16_MaxMessageBufferTx);
      this->mpc_Ui->pc_SpinBoxParallelTransmissions->setValue(
         pc_Node->c_Properties.c_OpenSYDEServerSettings.u8_MaxParallelTransmissions);

      //Com protocols
      if (pc_OSCDataPoolL2 != NULL)
      {
         if ((pc_OSCDataPoolL2->s32_RelatedDataBlockIndex >= 0) &&
             (static_cast<uint32>(pc_OSCDataPoolL2->s32_RelatedDataBlockIndex) < pc_Node->c_Applications.size()))
         {
            const C_OSCNodeApplication & rc_Application =
               pc_Node->c_Applications[static_cast<uint32>(pc_OSCDataPoolL2->s32_RelatedDataBlockIndex)];
            c_Text = rc_Application.c_Name.c_str();
         }
         else
         {
            c_Text = C_GtGetText::h_GetText("Application not assigned");
         }
      }
      else
      {
         c_Text = C_GtGetText::h_GetText("not used");
      }
      this->mpc_Ui->pc_LabelOL2Value->setText(c_Text);
      if (pc_OSCDataPoolEE != NULL)
      {
         if ((pc_OSCDataPoolEE->s32_RelatedDataBlockIndex >= 0) &&
             (static_cast<uint32>(pc_OSCDataPoolEE->s32_RelatedDataBlockIndex) < pc_Node->c_Applications.size()))
         {
            const C_OSCNodeApplication & rc_Application =
               pc_Node->c_Applications[static_cast<uint32>(pc_OSCDataPoolEE->s32_RelatedDataBlockIndex)];
            c_Text = rc_Application.c_Name.c_str();
         }
         else
         {
            c_Text = C_GtGetText::h_GetText("Application not assigned");
         }
      }
      else
      {
         c_Text = C_GtGetText::h_GetText("not used");
      }
      this->mpc_Ui->pc_LabelEEPDValue->setText(c_Text);
      if (pc_OSCDataPoolEO != NULL)
      {
         if ((pc_OSCDataPoolEO->s32_RelatedDataBlockIndex >= 0) &&
             (static_cast<uint32>(pc_OSCDataPoolEO->s32_RelatedDataBlockIndex) < pc_Node->c_Applications.size()))
         {
            const C_OSCNodeApplication & rc_Application =
               pc_Node->c_Applications[static_cast<uint32>(pc_OSCDataPoolEO->s32_RelatedDataBlockIndex)];
            c_Text = rc_Application.c_Name.c_str();
         }
         else
         {
            c_Text = C_GtGetText::h_GetText("Application not assigned");
         }
      }
      else
      {
         c_Text = C_GtGetText::h_GetText("not used");
      }
      this->mpc_Ui->pc_LabelEOPDValue->setText(c_Text);
   }
}

//-----------------------------------------------------------------------------
/*!
   \brief   Slot of Ok button click

   \created     29.03.2018  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_SdNdeProgrammingOptions::m_OkClicked(void)
{
   this->mrc_ParentDialog.accept();
}

//-----------------------------------------------------------------------------
/*!
   \brief   Slot of Ok button click

   \created     29.03.2018  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_SdNdeProgrammingOptions::m_CancelClicked(void)
{
   this->mrc_ParentDialog.reject();
}
