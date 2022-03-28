//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Widget for a drop area (implementation)

   Widget for a drop area

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "C_GtGetText.h"
#include "C_OSCUtils.h"
#include "C_OgeWiUtil.h"
#include "C_SyvDcExistingNodeDropAreaWidget.h"
#include "ui_C_SyvDcExistingNodeDropAreaWidget.h"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw_types;
using namespace stw_opensyde_gui;
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
C_SyvDcExistingNodeDropAreaWidget::C_SyvDcExistingNodeDropAreaWidget(QWidget * const opc_Parent) :
   QWidget(opc_Parent),
   mpc_Ui(new Ui::C_SyvDcExistingNodeDropAreaWidget),
   mq_Assigned(false)
{
   const stw_opensyde_core::C_OSCProtocolSerialNumber c_EmptySerialNumber;
   const std::map<uint8, C_SyvDcDeviceOldComConfig> c_EmptySubNodeIdsToOldNodeIds;
   const QPixmap c_Device =
      static_cast<QPixmap>("://images/system_views/DeviceSmall.svg").scaled(QSize(16, 16), Qt::KeepAspectRatio,
                                                                            Qt::SmoothTransformation);

   mpc_Ui->setupUi(this);

   this->InitStaticNames();

   //Deactivate debug string
   this->mpc_Ui->pc_GroupBoxEmpty->setTitle("");
   this->mpc_Ui->pc_GroupBoxValid->setTitle("");
   this->mpc_Ui->pc_LabelIcon->setText("");
   this->mpc_Ui->pc_PushButtonClose->setText("");
   this->mpc_Ui->pc_LabelIcon->setPixmap(c_Device);
   this->mpc_Ui->pc_PushButtonClose->setIcon(static_cast<QPixmap>("://images/system_views/IconTabClose.svg"));

   //Default
   this->SetContent(false, c_EmptySerialNumber, c_EmptySubNodeIdsToOldNodeIds);

   //Connects
   connect(this->mpc_Ui->pc_PushButtonClose, &stw_opensyde_gui_elements::C_OgePubTabClose::clicked, this,
           &C_SyvDcExistingNodeDropAreaWidget::m_OnDisconnectRequest);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default destructor

   Clean up.
*/
//----------------------------------------------------------------------------------------------------------------------
C_SyvDcExistingNodeDropAreaWidget::~C_SyvDcExistingNodeDropAreaWidget(void)
{
   delete mpc_Ui;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Initialize all displayed static names
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDcExistingNodeDropAreaWidget::InitStaticNames(void) const
{
   this->mpc_Ui->pc_LabelContent->setText(C_GtGetText::h_GetText("Drop device here"));
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set content and assignment state

   \param[in] oq_ValidSerialNumber        Flag if serial number valid
   \param[in] orc_PureSerialNumber        Serial number (only used if valid)
   \param[in] orc_SubNodeIdsToOldNodeIds  Detected sub node ids and the associated used node ids
                                          with same serial number
                                          - In case of a normal node, exact one sub node id which should be 0
                                          - In case of a multiple CPU, at least two sub node ids
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDcExistingNodeDropAreaWidget::SetContent(const bool oq_ValidSerialNumber,
                                                   const stw_opensyde_core::C_OSCProtocolSerialNumber & orc_PureSerialNumber, const std::map<uint8,
                                                                                                                                             C_SyvDcDeviceOldComConfig> & orc_SubNodeIdsToOldNodeIds)
{
   const QString c_ShowedSerialNumber = orc_PureSerialNumber.GetSerialNumberAsFormattedString().c_str();

   this->mc_PureSerialNumber = orc_PureSerialNumber;
   this->mpc_Ui->pc_LabelSerialNumber->setText(
      static_cast<QString>(C_GtGetText::h_GetText("SN.: %1")).arg(c_ShowedSerialNumber));
   this->mc_SubNodeIdsToOldNodeIds = orc_SubNodeIdsToOldNodeIds;

   if (oq_ValidSerialNumber == true)
   {
      this->mpc_Ui->pc_GroupBoxEmpty->setVisible(false);
      this->mpc_Ui->pc_GroupBoxValid->setVisible(true);
   }
   else
   {
      this->mpc_Ui->pc_GroupBoxEmpty->setVisible(true);
      this->mpc_Ui->pc_GroupBoxValid->setVisible(false);
   }

   this->mq_Assigned = oq_ValidSerialNumber;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Check if node has serial number assigned

   \return
   True  Assigned
   False Not assigned
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_SyvDcExistingNodeDropAreaWidget::IsAssigned(void) const
{
   return this->mq_Assigned;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Returns content

   \param[out] orc_PureSerialNumber        Serial number
   \param[out] opc_SubNodeIdsToOldNodeIds  Detected sub node ids and the associated used node ids
                                           with same serial number (optional)
                                           - In case of a normal node, exact one sub node id which should be 0
                                           - In case of a multiple CPU, at least two sub node ids
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDcExistingNodeDropAreaWidget::GetContent(stw_opensyde_core::C_OSCProtocolSerialNumber & orc_PureSerialNumber,
                                                   std::map<uint8,
                                                            C_SyvDcDeviceOldComConfig> * const opc_SubNodeIdsToOldNodeIds)
const
{
   orc_PureSerialNumber = this->mc_PureSerialNumber;
   if (opc_SubNodeIdsToOldNodeIds != NULL)
   {
      *opc_SubNodeIdsToOldNodeIds = this->mc_SubNodeIdsToOldNodeIds;
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Overwritten paint event slot

   Here: draw background
   (Not automatically drawn in any QWidget derivative)

   \param[in,out] opc_Event Event identification and information
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDcExistingNodeDropAreaWidget::paintEvent(QPaintEvent * const opc_Event)
{
   stw_opensyde_gui_logic::C_OgeWiUtil::h_DrawBackground(this);

   QWidget::paintEvent(opc_Event);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Forward request disconnect serial number
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDcExistingNodeDropAreaWidget::m_OnDisconnectRequest(void)
{
   Q_EMIT (this->SigDisconnect(this->mc_PureSerialNumber));
}
