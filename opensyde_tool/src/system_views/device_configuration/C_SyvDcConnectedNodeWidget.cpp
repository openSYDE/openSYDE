//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Widget for an connected node entry (implementation)

   Widget for an connected node entry

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "C_OgeWiUtil.h"
#include "C_GtGetText.h"
#include "C_SyvDcConnectedNodeWidget.h"
#include "ui_C_SyvDcConnectedNodeWidget.h"

#include "C_Uti.h"
#include "C_OSCUtils.h"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw_types;
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

   \param[in,out]    opc_Item    Optional pointer to widget item
   \param[in]        orc_Info    Device information
   \param[in,out]    opc_Parent  Optional pointer to parent
*/
//----------------------------------------------------------------------------------------------------------------------
C_SyvDcConnectedNodeWidget::C_SyvDcConnectedNodeWidget(QListWidgetItem * const opc_Item,
                                                       const C_SyvDcDeviceInformation & orc_Info,
                                                       QWidget * const opc_Parent) :
   QWidget(opc_Parent),
   mpc_Ui(new Ui::C_SyvDcConnectedNodeWidget),
   mpc_ListWidgetItem(opc_Item),
   mc_Info(orc_Info)
{
   const QPixmap c_Device = QPixmap("://images/system_views/DeviceSmall.svg").scaled(QSize(16, 16),
                                                                                     Qt::KeepAspectRatio,
                                                                                     Qt::SmoothTransformation);

   mpc_Ui->setupUi(this);

   m_Init();

   //Clear debug text
   this->mpc_Ui->pc_LabelIcon->setText("");
   this->mpc_Ui->pc_LabelIcon->setPixmap(c_Device);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default destructor

   Clean up.
*/
//----------------------------------------------------------------------------------------------------------------------
C_SyvDcConnectedNodeWidget::~C_SyvDcConnectedNodeWidget(void)
{
   delete mpc_Ui;
   //lint -e{1740} Never took ownership of mpc_ListWidgetItem
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get pure serial number string

   \return
   Pure serial number string
*/
//----------------------------------------------------------------------------------------------------------------------
QString C_SyvDcConnectedNodeWidget::GetSerialNumberString(void) const
{
   return QString(C_OSCUtils::h_SerialNumberToString(&this->mc_Info.au8_SerialNumber[0]).c_str());
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get device name

   \return
   Current device name
*/
//----------------------------------------------------------------------------------------------------------------------
QString C_SyvDcConnectedNodeWidget::GetDeviceName(void) const
{
   QString c_Retval = "";

   if (this->mc_Info.q_DeviceNameValid == true)
   {
      c_Retval = this->mc_Info.c_DeviceName.c_str();
   }
   return c_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Returns the valid state of the device name

   \return
   true     Device name is valid
   false    Device name is not valid
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_SyvDcConnectedNodeWidget::GetDeviceNameValid(void) const
{
   return this->mc_Info.q_DeviceNameValid;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Compare if widget matches to serial number

   \param[in] orc_SerialNumber Serial number

   \return
   True  Match
   False No match
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_SyvDcConnectedNodeWidget::CompareSerialNumber(const QString & orc_SerialNumber) const
{
   return (orc_SerialNumber.compare(this->GetSerialNumberString()) == 0);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Overwritten paint event slot

   Here: draw background
   (Not automatically drawn in any QWidget derivative)

   \param[in,out] opc_Event Event identification and information
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDcConnectedNodeWidget::paintEvent(QPaintEvent * const opc_Event)
{
   C_OgeWiUtil::h_DrawBackground(this);

   QWidget::paintEvent(opc_Event);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Init from data
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDcConnectedNodeWidget::m_Init(void)
{
   QString c_Name;
   QString c_Id;
   QString c_Device;
   const sintn sn_Height = 72;

   if (this->mc_Info.q_SerialNumberValid == true)
   {
      c_Name = QString(C_OSCUtils::h_SerialNumberToString(&this->mc_Info.au8_SerialNumber[0]).c_str());
   }
   else
   {
      c_Name = C_GtGetText::h_GetText("Unknown");
   }
   this->mpc_Ui->pc_LabelName->setText(QString("SN.: %1").arg(c_Name));
   if (this->mc_Info.q_NodeIdValid == true)
   {
      c_Id = QString::number(this->mc_Info.u8_NodeId);
   }
   else
   {
      c_Id = C_GtGetText::h_GetText("Unknown");
   }

   if (this->mc_Info.q_IpAddressValid == true)
   {
      // Show IP address if valid. If the IP address is valid, a Ethernet bus is used
      c_Id += " / IP: " + C_Uti::h_IpAddressToString(this->mc_Info.au8_IpAddress);
   }

   this->mpc_Ui->pc_LabelNodeId->setText(QString("Node-ID: %1").arg(c_Id));

   if (this->mc_Info.q_DeviceNameValid == true)
   {
      c_Device = this->mc_Info.c_DeviceName.c_str();
   }
   else
   {
      c_Device = C_GtGetText::h_GetText("Unknown");
   }
   this->mpc_Ui->pc_LabelDeviceType->setText(QString("Type: %1").arg(c_Device));
   //Resize
   this->resize(this->width(), sn_Height);
   this->setMinimumHeight(sn_Height);
   this->setMaximumHeight(sn_Height);
   if (this->mpc_ListWidgetItem != NULL)
   {
      this->mpc_ListWidgetItem->setSizeHint(QSize(this->mpc_ListWidgetItem->sizeHint().width(), sn_Height));
   }
   else
   {
      //Adapted for missing borders
      this->setMinimumHeight(sn_Height);
      this->setMaximumHeight(sn_Height);
   }
}
