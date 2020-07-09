//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Dialog for system node programming properties (implementation)

   Dialog for system node programming properties.

   \copyright   Copyright 2018 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <limits>

#include "stwerrors.h"
#include "TGLUtils.h"
#include "C_GtGetText.h"
#include "C_PuiSdHandler.h"

#include "C_SdNdeProgrammingOptions.h"
#include "ui_C_SdNdeProgrammingOptions.h"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw_tgl;
using namespace stw_types;
using namespace stw_errors;
using namespace stw_opensyde_gui;
using namespace stw_opensyde_core;
using namespace stw_opensyde_gui_logic;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

/* -- Global Variables ---------------------------------------------------------------------------------------------- */

/* -- Module Global Variables --------------------------------------------------------------------------------------- */

/* -- Module Global Function Prototypes ----------------------------------------------------------------------------- */

/* -- Implementation ------------------------------------------------------------------------------------------------ */

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default constructor

   Set up GUI with all elements.

   \param[in,out] orc_Parent     Reference to parent
   \param[in]     ou32_NodeIndex Node index
*/
//----------------------------------------------------------------------------------------------------------------------
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

   //Scaling combobox
   this->mpc_Ui->pc_ComboBoxScaling->insertItem(0, "Float32",
                                                static_cast<sintn>(C_OSCNodeCodeExportSettings::eFLOAT32));
   this->mpc_Ui->pc_ComboBoxScaling->insertItem(1, "Float64",
                                                static_cast<sintn>(C_OSCNodeCodeExportSettings::eFLOAT64));
   this->mpc_Ui->pc_ComboBoxScaling->insertItem(2, C_GtGetText::h_GetText("Disabled"),
                                                static_cast<sintn>(C_OSCNodeCodeExportSettings::eNONE));

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

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default destructor

   Clean up.
*/
//----------------------------------------------------------------------------------------------------------------------
C_SdNdeProgrammingOptions::~C_SdNdeProgrammingOptions(void)
{
   delete mpc_Ui;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Initialize all displayed static names
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeProgrammingOptions::InitStaticNames(void) const
{
   this->mpc_Ui->pc_BushButtonOk->setText(C_GtGetText::h_GetText("OK"));
   this->mpc_Ui->pc_BushButtonCancel->setText(C_GtGetText::h_GetText("Cancel"));

   this->mpc_Ui->pc_LabelGeneral->setText(C_GtGetText::h_GetText("General"));
   this->mpc_Ui->pc_LabelScaling->setText(C_GtGetText::h_GetText("Scaling values data type"));

   this->mpc_Ui->pc_LabelDPDHeading->setText(C_GtGetText::h_GetText("Diagnostic Protocol Driver"));
   this->mpc_Ui->pc_LabelDPD->setText(C_GtGetText::h_GetText("Diagnostic protocol driver is implemented by"));
   this->mpc_Ui->pc_LabelNBT->setText(C_GtGetText::h_GetText("Number of buffered CAN Tx messages"));
   this->mpc_Ui->pc_LabelNBR->setText(C_GtGetText::h_GetText("Number of buffered CAN Rx routing messages"));
   this->mpc_Ui->pc_LabelMNO->setText(C_GtGetText::h_GetText("Maximum number of cyclic/event driven transmissions "
                                                             "in parallel"));

   //Tool tips
   this->mpc_Ui->pc_LabelScaling->SetToolTipInformation(
      C_GtGetText::h_GetText("Scaling Values"),
      C_GtGetText::h_GetText("Choose data type for offset and factor constants or disable scaling support.\n"
                             "Options:\n"
                             " - Float32: Offset and factor constants are generated as single-precision float, e.g. 1.23F\n"
                             " - Float64: Offset and factor constants are generated as double-precision float, e.g. 1.23\n"
                             " - Disabled: Offset, factor and scaling macros are not generated at all"));
   this->mpc_Ui->pc_LabelDPDHeading->SetToolTipInformation(
      C_GtGetText::h_GetText("Diagnostic Protocol Driver"),
      C_GtGetText::h_GetText("The diagnostic protocol driver provides access to Datapool content via the openSYDE "
                             "communication protocol."));
   this->mpc_Ui->pc_LabelDPD->SetToolTipInformation(
      C_GtGetText::h_GetText("Diagnostic protocol driver is implemented by"),
      C_GtGetText::h_GetText("To which application should the diagnostic protocol driver be added?"));
   this->mpc_Ui->pc_LabelNBT->SetToolTipInformation(
      C_GtGetText::h_GetText("Number of buffered CAN Tx messages"),
      C_GtGetText::h_GetText("Maximum number of CAN Tx messages the server can buffer "
                             "(used for all transferred CAN messages).\n"
                             "\nDefault value: 585"));
   this->mpc_Ui->pc_LabelNBR->SetToolTipInformation(
      C_GtGetText::h_GetText("Number of buffered CAN Rx routing messages"),
      C_GtGetText::h_GetText("Maximum number of CAN Rx routing messages the server can buffer before the client has "
                             "to wait for an acknowledge.\n"
                             "\nDefault value: 585"));
   this->mpc_Ui->pc_LabelMNO->SetToolTipInformation(
      C_GtGetText::h_GetText("Max number of cyclic/event driven transmissions in parallel"),
      C_GtGetText::h_GetText("This refers to the maximum number of parallel cyclic and event driven diagnostic "
                             "transmissions you are allowed to configure for this server.\n"
                             "\nDefault value: 64"));
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Save to RAM data storage
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeProgrammingOptions::Save(void) const
{
   C_OSCNodeOpenSYDEServerSettings c_DPDSettings;
   C_OSCNodeCodeExportSettings c_GeneralSettings;

   // Scaling
   c_GeneralSettings.e_ScalingSupport =
      static_cast<C_OSCNodeCodeExportSettings::E_Scaling>(this->mpc_Ui->pc_ComboBoxScaling->currentData().toInt());
   tgl_assert(C_PuiSdHandler::h_GetInstance()->SetNodeCodeExportSettings(this->mu32_NodeIndex,
                                                                         c_GeneralSettings) == C_NO_ERR);

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
         c_DPDSettings.s16_DPDDataBlockIndex = s16_ApplicationIndex;
      }
   }
   else
   {
      //Invalid application
      c_DPDSettings.s16_DPDDataBlockIndex = -1;
   }

   //Spin boxes
   c_DPDSettings.u16_MaxMessageBufferTx = static_cast<uint16>(this->mpc_Ui->pc_SpinBoxNBT->value());
   c_DPDSettings.u16_MaxRoutingMessageBufferRx = static_cast<uint16>(this->mpc_Ui->pc_SpinBoxNBR->value());
   c_DPDSettings.u8_MaxParallelTransmissions =
      static_cast<uint8>(this->mpc_Ui->pc_SpinBoxParallelTransmissions->value());
   tgl_assert(C_PuiSdHandler::h_GetInstance()->SetNodeOpenSYDEServerSettings(this->mu32_NodeIndex,
                                                                             c_DPDSettings) == C_NO_ERR);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Overwritten key press event slot

   Here: Handle specific enter key cases

   \param[in,out] opc_KeyEvent Event identification and information
*/
//----------------------------------------------------------------------------------------------------------------------
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
         this->m_OkClicked();
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
/*! \brief   Load dynamic content
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeProgrammingOptions::m_Load(void) const
{
   const C_OSCNode * const pc_Node = C_PuiSdHandler::h_GetInstance()->GetOSCNodeConst(this->mu32_NodeIndex);

   if (pc_Node != NULL)
   {
      sint32 s32_Counter = 0;
      sint32 s32_DataBlockIndex = -1;

      //Scaling
      this->mpc_Ui->pc_ComboBoxScaling->setCurrentIndex(
         this->mpc_Ui->pc_ComboBoxScaling->findData(
            static_cast<sintn>(pc_Node->c_Properties.c_CodeExportSettings.e_ScalingSupport)));

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
      this->mpc_Ui->pc_SpinBoxNBR->setValue(
         pc_Node->c_Properties.c_OpenSYDEServerSettings.u16_MaxRoutingMessageBufferRx);
      this->mpc_Ui->pc_SpinBoxNBT->setValue(pc_Node->c_Properties.c_OpenSYDEServerSettings.u16_MaxMessageBufferTx);
      this->mpc_Ui->pc_SpinBoxParallelTransmissions->setValue(
         pc_Node->c_Properties.c_OpenSYDEServerSettings.u8_MaxParallelTransmissions);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Slot of Ok button click
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeProgrammingOptions::m_OkClicked(void)
{
   this->mrc_ParentDialog.accept();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Slot of Ok button click
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeProgrammingOptions::m_CancelClicked(void)
{
   this->mrc_ParentDialog.reject();
}
