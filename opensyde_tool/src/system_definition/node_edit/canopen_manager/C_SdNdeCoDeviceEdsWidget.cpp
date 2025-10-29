//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Widget for handling the EDS file of a specific device on a specific interface of the CANopen Manager

   Widget for handling the EDS file of a specific device on a specific interface of the CANopen Manager

   \copyright   Copyright 2022 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.hpp"

#include <QImageReader>
#include <QDir>

#include "C_SdNdeCoDeviceEdsWidget.hpp"
#include "ui_C_SdNdeCoDeviceEdsWidget.h"

#include "stwerrors.hpp"
#include "C_CieUtil.hpp"
#include "C_PuiUtil.hpp"
#include "C_GtGetText.hpp"
#include "C_PuiProject.hpp"
#include "C_OgeWiUtil.hpp"
#include "C_UsHandler.hpp"
#include "C_OgePopUpDialog.hpp"
#include "C_SdNdeCoConfigTreeView.hpp"
#include "C_SdNdeCoAddDeviceDialog.hpp"
#include "C_OscCanOpenObjectDictionary.hpp"
#include "C_OscCanOpenManagerDeviceInfo.hpp"
#include "C_SdNdeCoDeviceUpdateEdsDialog.hpp"
#include "C_PuiSdHandler.hpp"
#include "C_SdUtil.hpp"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw::scl;
using namespace stw::errors;
using namespace stw::opensyde_gui;
using namespace stw::opensyde_core;
using namespace stw::opensyde_gui_logic;
using namespace stw::opensyde_gui_elements;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

/* -- Global Variables ---------------------------------------------------------------------------------------------- */

/* -- Module Global Variables --------------------------------------------------------------------------------------- */

/* -- Module Global Function Prototypes ----------------------------------------------------------------------------- */

/* -- Implementation ------------------------------------------------------------------------------------------------ */

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default constructor

   Set up GUI with all elements.

   \param[in,out]  opc_Parent    Optional pointer to parent
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
void C_SdNdeCoDeviceEdsWidget::m_OnUpdateClicked(void)
{
   const QString c_FilePath = C_SdNdeCoAddDeviceDialog::h_AskForCanOpenPath(this);

   if (c_FilePath.isEmpty() == false)
   {
      uint32_t u32_DeviceInterfaceIndex;
      if (C_PuiSdHandler::h_GetInstance()->TranslateCanInterfaceNumberToIndex(this->mc_DeviceInterfaceId.u32_NodeIndex,
                                                                              this->mc_DeviceInterfaceId.
                                                                              u8_InterfaceNumber,
                                                                              u32_DeviceInterfaceIndex) == C_NO_ERR)
      {
         const C_OscNode * const pc_Node = C_PuiSdHandler::h_GetInstance()->GetOscNodeConst(
            this->mc_DeviceInterfaceId.u32_NodeIndex);

         tgl_assert(pc_Node != NULL);
         if (pc_Node != NULL)
         {
            tgl_assert(u32_DeviceInterfaceIndex < pc_Node->c_Properties.c_ComInterfaces.size());
            if (u32_DeviceInterfaceIndex < pc_Node->c_Properties.c_ComInterfaces.size())
            {
               QString c_ParsingError;
               C_CieImportDataAssignment c_NodeAssignment;
               C_CieImportDataAssignment c_InvalidNodeAssignment;
               const C_OscNodeComInterfaceSettings & rc_CurInterface =
                  pc_Node->c_Properties.c_ComInterfaces[u32_DeviceInterfaceIndex];
               const int32_t s32_ImportResult = C_CieUtil::h_GetDeviceInfo(this->mc_DeviceInterfaceId.u32_NodeIndex,
                                                                           u32_DeviceInterfaceIndex,
                                                                           this->mu32_ManagerNodeIndex,
                                                                           this->mu8_ManagerInterfaceId, c_FilePath,
                                                                           c_NodeAssignment, c_InvalidNodeAssignment,
                                                                           c_ParsingError);
               if (s32_ImportResult == C_NO_ERR)
               {
                  std::vector<C_CieImportDataAssignment> c_NodeAssignmenVector = {c_NodeAssignment};
                  std::vector<C_CieImportDataAssignment> c_InvalidNodeAssignmentVector = {c_InvalidNodeAssignment};
                  //Prepare data
                  C_CieUtil::h_AdaptImportMessages(c_NodeAssignmenVector, C_OscCanProtocol::eCAN_OPEN, true);
                  C_CieUtil::h_AdaptImportMessages(c_InvalidNodeAssignmentVector, C_OscCanProtocol::eCAN_OPEN, true);
                  {
                     const C_OscCanOpenManagerDeviceInfo c_NewConfig = C_SdNdeCoConfigTreeView::h_CreateNewDevice(
                        c_FilePath);
                     const QPointer<C_OgePopUpDialog> c_PopUp = new C_OgePopUpDialog(this, this);
                     const C_SdNdeCoDeviceUpdateEdsDialog * const pc_AddDialog = new C_SdNdeCoDeviceUpdateEdsDialog(
                        *c_PopUp,
                        this->mu32_ManagerNodeIndex, this->mu8_ManagerInterfaceId, this->mc_DeviceInterfaceId,
                        c_FilePath, c_NewConfig,
                        c_NodeAssignmenVector[0],
                        c_InvalidNodeAssignmentVector[0]);

                     //Resize
                     c_PopUp->SetSize(QSize(1200, 745));

                     if (c_PopUp->exec() == static_cast<int32_t>(QDialog::Accepted))
                     {
                        // update a EDS file
                        pc_AddDialog->DoApplyData();
                        //Reset screen
                        this->m_SetDetails();
                     }

                     if (c_PopUp != NULL)
                     {
                        c_PopUp->HideOverlay();
                        c_PopUp->deleteLater();
                     }
                  } //lint !e429  //no memory leak because of the parent of pc_Dialog and the Qt memory management
               }
               else
               {
                  C_CieUtil::h_ReportEdsImportError(this, s32_ImportResult, c_ParsingError, rc_CurInterface.u8_NodeId);
               }
            }
         }
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Node Index and Inteface ID setter

   Sets the private node index and interface id

   \param[in]  ou32_ManagerNodeIndex   new node index
   \param[in]  ou8_ManagerInterfaceId  new interface index
   \param[in]  orc_DeviceNodeId        new Node ID
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeCoDeviceEdsWidget::SetNodeIndexAndInterfaceId(const uint32_t ou32_ManagerNodeIndex,
                                                          const uint8_t ou8_ManagerInterfaceId,
                                                          const stw::opensyde_core::C_OscCanInterfaceId & orc_DeviceNodeId)
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
   const C_OscCanOpenManagerDeviceInfo * const pc_CanOpenDeviceInfo =
      C_PuiSdHandler::h_GetInstance()->GetCanOpenManagerDevice(this->mu32_ManagerNodeIndex,
                                                               this->mu8_ManagerInterfaceId,
                                                               this->mc_DeviceInterfaceId);

   if (pc_CanOpenDeviceInfo != NULL)
   {
      const C_OscCanOpenObjectDictionary c_CanOpenObjDictionary = pc_CanOpenDeviceInfo->GetEdsFileContent();

      this->mpc_Ui->pc_LabFileCurrent->setText(
         C_GtGetText::h_GetText(pc_CanOpenDeviceInfo->c_OriginalEdsFileName.AsStdString()->c_str()));

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
