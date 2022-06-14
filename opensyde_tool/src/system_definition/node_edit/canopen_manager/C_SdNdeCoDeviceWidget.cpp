//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Widget with current CANopen settings of Manager for a specific device on one interface

   Widget with current CANopen settings of Manager for a specific device on one interface

   \copyright   Copyright 2022 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.h"

#include "TGLUtils.h"
#include "C_SdNdeCoDeviceWidget.h"
#include "ui_C_SdNdeCoDeviceWidget.h"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw_tgl;
using namespace stw_types;
using namespace stw_opensyde_gui;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */
const uint32 C_SdNdeCoDeviceWidget::mhu32_INDEX_DEVICE_USE_CASE_EDS_FILE = 0UL;
const uint32 C_SdNdeCoDeviceWidget::mhu32_INDEX_DEVICE_USE_CASE_CONFIGURATION = 1UL;
const uint32 C_SdNdeCoDeviceWidget::mhu32_INDEX_DEVICE_USE_CASE_PDOS = 2UL;

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
C_SdNdeCoDeviceWidget::C_SdNdeCoDeviceWidget(QWidget * const opc_Parent) :
   QWidget(opc_Parent),
   mpc_Ui(new Ui::C_SdNdeCoDeviceWidget)
{
   this->mpc_Ui->setupUi(this);

   this->mpc_Ui->pc_DevConfigWidget->setVisible(false);
   this->mpc_Ui->pc_DevPdoWidget->setVisible(false);
   this->mpc_Ui->pc_DevEdsWidget->setVisible(false);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default destructor
*/
//----------------------------------------------------------------------------------------------------------------------
C_SdNdeCoDeviceWidget::~C_SdNdeCoDeviceWidget()
{
   delete this->mpc_Ui;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Set device use case

   \param[in]  ou32_NodeIndex       Node index
   \param[in]  ou8_InterfaceId      Interface ID
   \param[in]  orc_NodeId           Node ID
   \param[in]  ou32_UseCaseIndex    Use case index
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeCoDeviceWidget::SetDeviceUseCase(const uint32 ou32_NodeIndex, const uint8 ou8_InterfaceId,
                                             const stw_opensyde_core::C_OSCCanInterfaceId & orc_NodeId,
                                             const uint32 ou32_UseCaseIndex)
{
   bool q_EdsWidgetVisibility = false;
   bool q_PdoWidgetVisibility = false;
   bool q_ConfigWidgetVisibility = false;

   switch (ou32_UseCaseIndex)
   {
   case C_SdNdeCoDeviceWidget::mhu32_INDEX_DEVICE_USE_CASE_EDS_FILE:
      q_EdsWidgetVisibility = true;
      break;
   case C_SdNdeCoDeviceWidget::mhu32_INDEX_DEVICE_USE_CASE_CONFIGURATION:
      q_ConfigWidgetVisibility = true;
      this->mpc_Ui->pc_DevConfigWidget->SetNodeIndexAndInterfaceId(ou32_NodeIndex, ou8_InterfaceId, orc_NodeId);
      break;
   case C_SdNdeCoDeviceWidget::mhu32_INDEX_DEVICE_USE_CASE_PDOS:
      q_PdoWidgetVisibility = true;
      break;
   default:
      //Should not happen
      tgl_assert(false);
      break;
   }
   this->mpc_Ui->pc_DevConfigWidget->setVisible(q_ConfigWidgetVisibility);
   this->mpc_Ui->pc_DevPdoWidget->setVisible(q_PdoWidgetVisibility);
   this->mpc_Ui->pc_DevEdsWidget->setVisible(q_EdsWidgetVisibility);
}
