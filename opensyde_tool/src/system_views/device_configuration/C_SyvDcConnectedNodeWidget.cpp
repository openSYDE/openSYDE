//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Widget for an connected node entry (implementation)

   Widget for an connected node entry

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "C_OgeWiUtil.hpp"
#include "C_GtGetText.hpp"
#include "C_SyvDcConnectedNodeWidget.hpp"
#include "ui_C_SyvDcConnectedNodeWidget.h"

#include "C_Uti.hpp"
#include "C_OscUtils.hpp"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw::opensyde_gui;
using namespace stw::opensyde_core;
using namespace stw::opensyde_gui_logic;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */
const int32_t C_SyvDcConnectedNodeWidget::mhs32_HEIGHT = 72;
const int32_t C_SyvDcConnectedNodeWidget::mhs32_WIDTH = 294;

// Refer ScrollBar.qss
const int32_t C_SyvDcConnectedNodeWidget::mhs32_SCROLLBAR_WIDTH = 10;

/* -- Types --------------------------------------------------------------------------------------------------------- */

/* -- Global Variables ---------------------------------------------------------------------------------------------- */

/* -- Module Global Variables --------------------------------------------------------------------------------------- */

/* -- Module Global Function Prototypes ----------------------------------------------------------------------------- */

/* -- Implementation ------------------------------------------------------------------------------------------------ */

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default constructor

   Set up GUI with all elements.

   \param[in,out]    opc_Item                    Optional pointer to widget item
   \param[in]        orc_Info                    Device information
   \param[in]        orc_SubNodeIdsToOldNodeIds  Detected sub node ids and the associated used node ids
                                                 with same serial number
                                                 - In case of a normal node, exact one sub node id which should be 0
                                                 - In case of a multiple CPU, at least two sub node ids
   \param[in,out]    opc_Parent                  Optional pointer to parent
*/
//----------------------------------------------------------------------------------------------------------------------
C_SyvDcConnectedNodeWidget::C_SyvDcConnectedNodeWidget(QListWidgetItem * const opc_Item,
                                                       const C_OscDcDeviceInformation & orc_Info,
                                                       const std::map<uint8_t,
                                                                      C_OscDcDeviceOldComConfig> & orc_SubNodeIdsToOldNodeIds,
                                                       QWidget * const opc_Parent) :
   QWidget(opc_Parent),
   mpc_Ui(new Ui::C_SyvDcConnectedNodeWidget),
   mpc_ListWidgetItem(opc_Item),
   mc_Info(orc_Info),
   mc_SubNodeIdsToOldNodeIds(orc_SubNodeIdsToOldNodeIds)
{
   const QPixmap c_Device =
      static_cast<QPixmap>("://images/system_views/DeviceSmall.svg").scaled(QSize(16, 16), Qt::KeepAspectRatio,
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
//lint -e{1540} Never took ownership of mpc_ListWidgetItem
C_SyvDcConnectedNodeWidget::~C_SyvDcConnectedNodeWidget(void) noexcept
{
   delete mpc_Ui;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get pure serial number string

   \return
   Pure serial number string
*/
//----------------------------------------------------------------------------------------------------------------------
QString C_SyvDcConnectedNodeWidget::GetPlainSerialNumberString(void) const
{
   return this->mc_Info.c_SerialNumber.GetSerialNumberAsPlainString().c_str();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get flag for ext format of serial number

   \return
   Flag for ext format of serial number
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_SyvDcConnectedNodeWidget::GetExtFormat(void) const
{
   return this->mc_Info.c_SerialNumber.q_ExtFormatUsed;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get manufacturer format for ext format of serial number

   In case of standard serial number the value is 0

   \return
   Manufacturer format for ext format of serial number
*/
//----------------------------------------------------------------------------------------------------------------------
uint8_t C_SyvDcConnectedNodeWidget::GetManufacturerFormat(void) const
{
   return this->mc_Info.c_SerialNumber.u8_SerialNumberManufacturerFormat;
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
/*! \brief   Get sub node ids to node ids mapping of node

   \return
   Sub node ids mapping to node ids
*/
//----------------------------------------------------------------------------------------------------------------------
std::map<uint8_t, C_OscDcDeviceOldComConfig> C_SyvDcConnectedNodeWidget::GetSubNodeIdsToOldNodeIds(void) const
{
   return this->mc_SubNodeIdsToOldNodeIds;
}
//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Compare if widget matches to serial number

   \param[in] orc_SerialNumber       Serial number

   \return
   True  Match
   False No match
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_SyvDcConnectedNodeWidget::CompareSerialNumber(
   const stw::opensyde_core::C_OscProtocolSerialNumber & orc_SerialNumber) const
{
   bool q_Return = false;

   if (orc_SerialNumber == this->mc_Info.c_SerialNumber)
   {
      q_Return = true;
   }

   return q_Return;
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

   if (this->mc_Info.c_SerialNumber.q_IsValid == true)
   {
      c_Name = this->mc_Info.c_SerialNumber.GetSerialNumberAsFormattedString().c_str();
   }
   else
   {
      c_Name = C_GtGetText::h_GetText("Unknown");
   }

   this->mpc_Ui->pc_LabelName->setText(static_cast<QString>("SN.: %1").arg(c_Name));

   if (this->mc_Info.q_NodeIdValid == true)
   {
      if (this->mc_SubNodeIdsToOldNodeIds.size() > 1)
      {
         c_Id = C_GtGetText::h_GetText("<multiple>");
      }
      else
      {
         c_Id = QString::number(this->mc_Info.u8_NodeId);
      }
   }
   else
   {
      c_Id = C_GtGetText::h_GetText("Unknown");
   }

   if ((this->mc_Info.q_IpAddressValid == true) && (this->mc_SubNodeIdsToOldNodeIds.size() == 1))
   {
      // Show IP address if valid. If the IP address is valid, a Ethernet bus is used
      c_Id += " / IP: " + C_Uti::h_IpAddressToString(this->mc_Info.au8_IpAddress);
   }

   this->mpc_Ui->pc_LabelNodeId->setText(static_cast<QString>("Node ID: %1").arg(c_Id));

   if (this->mc_Info.q_DeviceNameValid == true)
   {
      c_Device = this->mc_Info.c_DeviceName.c_str();
   }
   else
   {
      c_Device = C_GtGetText::h_GetText("Unknown");
   }
   this->mpc_Ui->pc_LabelDeviceType->setText(static_cast<QString>("Type: %1").arg(c_Device));
   //Resize
   this->resize(mhs32_WIDTH, mhs32_HEIGHT);
   this->setMinimumHeight(mhs32_HEIGHT);
   this->setMaximumHeight(mhs32_HEIGHT);
   this->setMinimumWidth(mhs32_WIDTH);
   this->setMaximumWidth(mhs32_WIDTH);
   if (this->mpc_ListWidgetItem != NULL)
   {
      this->mpc_ListWidgetItem->setSizeHint(QSize(mhs32_WIDTH, mhs32_HEIGHT));
   }
   else
   {
      //Adapted for missing borders
      this->setMinimumWidth(mhs32_WIDTH);
      this->setMaximumWidth(mhs32_WIDTH);
      this->setMinimumHeight(mhs32_HEIGHT);
      this->setMaximumHeight(mhs32_HEIGHT);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Sets width of the list item

   \param[in]       os32_WidgetWidth     Width value to be set
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDcConnectedNodeWidget::SetWidth(const int32_t os32_WidgetWidth)
{
   if (this->mpc_ListWidgetItem != NULL)
   {
      this->setMaximumWidth(os32_WidgetWidth - mhs32_SCROLLBAR_WIDTH);
      this->mpc_ListWidgetItem->setSizeHint(QSize(os32_WidgetWidth - mhs32_SCROLLBAR_WIDTH, mhs32_HEIGHT));

      // this applies the actual change in widget width (checked with .width() method)
      this->resize(os32_WidgetWidth - mhs32_SCROLLBAR_WIDTH, mhs32_HEIGHT);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get node ID

   \retval   Node ID
*/
//----------------------------------------------------------------------------------------------------------------------
uint8_t C_SyvDcConnectedNodeWidget::GetNodeId(void) const
{
   const uint8_t u8_Retval = this->mc_Info.u8_NodeId;

   return u8_Retval;
}
