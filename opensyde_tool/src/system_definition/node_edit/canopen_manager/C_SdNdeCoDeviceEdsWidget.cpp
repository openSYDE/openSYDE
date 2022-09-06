//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Widget for handling the EDS file of a specific device on a specific interface of the CANopen Manager

   Widget for handling the EDS file of a specific device on a specific interface of the CANopen Manager

   \copyright   Copyright 2022 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.h"

#include <QImageReader>
#include <QDir>

#include "C_SdNdeCoDeviceEdsWidget.h"
#include "ui_C_SdNdeCoDeviceEdsWidget.h"

#include "stwerrors.h"
#include "C_GtGetText.h"
#include "C_PuiUtil.h"
#include "C_PuiProject.h"
#include "C_OgeWiUtil.h"
#include "C_UsHandler.h"
#include "C_OSCCanOpenObjectDictionary.h"
#include "C_OSCCanOpenManagerDeviceInfo.h"
#include "C_PuiSdHandler.h"
#include "C_SdUtil.h"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw_scl;
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

   \param[in,out] opc_Parent Optional pointer to parent
*/
//----------------------------------------------------------------------------------------------------------------------
C_SdNdeCoDeviceEdsWidget::C_SdNdeCoDeviceEdsWidget(QWidget * const opc_Parent) :
   QWidget(opc_Parent),
   mpc_Ui(new Ui::C_SdNdeCoDeviceEdsWidget),
   mu32_ManagerNodeIndex(0),
   mu8_ManagerInterfaceId(0)
{
   this->mpc_Ui->setupUi(this);

   this->InitStaticNames();

   // panel colors
   this->mpc_Ui->pc_WiEDSFilePanel->SetBackgroundColor(11);
   this->mpc_Ui->pc_LabFileTitle->SetBackgroundColor(0);
   this->mpc_Ui->pc_LabFileTitle->SetForegroundColor(3);
   this->mpc_Ui->pc_LabFileTitle->SetFontPixel(16, true);
   this->mpc_Ui->pc_LabFileCurrent->SetBackgroundColor(0);
   this->mpc_Ui->pc_LabFileCurrent->SetForegroundColor(3);
   this->mpc_Ui->pc_LabFileCurrent->SetFontPixel(16);

   this->mpc_Ui->pc_TedHtmlReport->setReadOnly(true);

   //not supported yet
   this->mpc_Ui->pc_PubUpdate->setVisible(false);

   // connects
   connect(this->mpc_Ui->pc_PubUpdate, &QPushButton::clicked, this, &C_SdNdeCoDeviceEdsWidget::m_OnUpdateClicked);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default destructor
*/
//----------------------------------------------------------------------------------------------------------------------
C_SdNdeCoDeviceEdsWidget::~C_SdNdeCoDeviceEdsWidget()
{
   delete this->mpc_Ui;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Initialize all displayed static names
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeCoDeviceEdsWidget::InitStaticNames(void) const
{
   this->mpc_Ui->pc_PubUpdate->setText(C_GtGetText::h_GetText("Update"));
   this->mpc_Ui->pc_PubUpdate->SetToolTipInformation(C_GtGetText::h_GetText("Update EDS File"),
                                                     C_GtGetText::h_GetText(
                                                        "Update the current EDS file to a new version by "
                                                        "selecting a new EDS file *.eds."));
   this->mpc_Ui->pc_LabFileTitle->setText(C_GtGetText::h_GetText("EDS File:"));
   this->mpc_Ui->pc_LabFileTitle->SetToolTipInformation(C_GtGetText::h_GetText("EDS File"),
                                                        C_GtGetText::h_GetText("The EDS file *.eds includes the "
                                                                               "node's messages and signals."));
   this->mpc_Ui->pc_LabelDetails->setText(C_GtGetText::h_GetText("Details"));
   this->mpc_Ui->pc_TedHtmlReport->setPlainText(C_GtGetText::h_GetText("<EDS file description>"));
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Slot of update button

   Load a EDS file update.
*/
//----------------------------------------------------------------------------------------------------------------------
//lint -e{9175}  //implementation of function is TODO
void C_SdNdeCoDeviceEdsWidget::m_OnUpdateClicked(void) const
{
   // TODO
   // update a EDS file
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Node Index and Inteface ID setter

   Sets the private node index and interface id

   \param[in]  ou32_ManagerNodeIndex     new node index
   \param[in]  ou8_ManagerInterfaceId    new interface index
   \param[in]  orc_DeviceNodeId          new Node ID
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeCoDeviceEdsWidget::SetNodeIndexAndInterfaceId(const uint32 ou32_ManagerNodeIndex,
                                                          const uint8 ou8_ManagerInterfaceId,
                                                          const stw_opensyde_core::C_OSCCanInterfaceId & orc_DeviceNodeId)
{
   this->mu32_ManagerNodeIndex = ou32_ManagerNodeIndex;
   this->mu8_ManagerInterfaceId = ou8_ManagerInterfaceId;
   this->mc_DeviceInterfaceId = orc_DeviceNodeId;

   this->m_SetDetails();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Handle details
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeCoDeviceEdsWidget::m_SetDetails(void)
{
   const C_OSCCanOpenManagerDeviceInfo * const pc_CanOpenDeviceInfo =
      C_PuiSdHandler::h_GetInstance()->GetCanOpenManagerDevice(this->mu32_ManagerNodeIndex,
                                                               this->mu8_ManagerInterfaceId,
                                                               this->mc_DeviceInterfaceId);

   if (pc_CanOpenDeviceInfo != NULL)
   {
      C_OSCCanOpenObjectDictionary const c_CanOpenObjDictionary = pc_CanOpenDeviceInfo->c_EDSFileContent;

      this->mpc_Ui->pc_LabFileCurrent->setText(C_GtGetText::h_GetText(pc_CanOpenDeviceInfo->c_EDSFileName.AsStdString()
                                                                      ->
                                                                      c_str()));

      if (C_SdUtil::h_GetEdsFileDetails(c_CanOpenObjDictionary) != "")
      {
         this->mpc_Ui->pc_TedHtmlReport->setPlainText(C_SdUtil::h_GetEdsFileDetails(c_CanOpenObjDictionary));
      }
      else
      {
         this->mpc_Ui->pc_TedHtmlReport->setPlainText(C_GtGetText::h_GetText("<No data>"));
      }
   }
   else
   {
      this->mpc_Ui->pc_TedHtmlReport->setPlainText(C_GtGetText::h_GetText("<EDS file description>"));
   }
}
