//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Widget for an existing node entry (implementation)

   Widget for an existing node entry

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <QMimeData>
#include <QDragMoveEvent>
#include <QDragEnterEvent>
#include <QDragLeaveEvent>

#include "stwerrors.h"
#include "C_OgeWiUtil.h"
#include "C_GtGetText.h"
#include "C_PuiSdHandler.h"
#include "C_PuiSvHandler.h"
#include "C_OgeWiCustomMessage.h"
#include "C_SyvDcExistingNodeWidget.h"
#include "ui_C_SyvDcExistingNodeWidget.h"
#include "C_Uti.h"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw_types;
using namespace stw_errors;
using namespace stw_opensyde_gui;
using namespace stw_opensyde_core;
using namespace stw_opensyde_gui_logic;
using namespace stw_opensyde_gui_elements;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */
const QString C_SyvDcExistingNodeWidget::mhc_MimeData = "stw_opensyde_connected_node";
const QString C_SyvDcExistingNodeWidget::mhc_MimeDataDevice = "stw_opensyde_connected_node_device";
const QString C_SyvDcExistingNodeWidget::mhc_MimeDataDeviceValid = "stw_opensyde_connected_node_device_valid";

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
C_SyvDcExistingNodeWidget::C_SyvDcExistingNodeWidget(QWidget * const opc_Parent) :
   QWidget(opc_Parent),
   mpc_Ui(new Ui::C_SyvDcExistingNodeWidget),
   mpc_ListWidgetItem(NULL),
   mu32_ViewIndex(0),
   mu32_NodeIndex(0),
   ms32_ItemIndex(-1)
{
   mpc_Ui->setupUi(this);

   this->setAcceptDrops(true);

   //Clear debug text
   this->mpc_Ui->pc_LabelIcon->setText("");
   this->mpc_Ui->pc_LabelIcon->setPixmap(QPixmap("://images/system_definition/IconNode.svg"));

   //Deactivate debug string
   this->mpc_Ui->pc_GroupBoxSerialNumber->setTitle("");

   C_OgeWiUtil::h_ApplyStylesheetProperty(this->mpc_Ui->pc_WidgetSerialNumber, "Hovered", false);
   C_OgeWiUtil::h_ApplyStylesheetProperty(this->mpc_Ui->pc_WidgetSerialNumber, "Assigned", false);

   //Connects
   connect(this->mpc_Ui->pc_WidgetSerialNumber, &C_SyvDcExistingNodeDropAreaWidget::SigDisconnect, this,
           &C_SyvDcExistingNodeWidget::m_OnDisconnectRequest);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default destructor

   Clean up.
*/
//----------------------------------------------------------------------------------------------------------------------
C_SyvDcExistingNodeWidget::~C_SyvDcExistingNodeWidget(void)
{
   delete mpc_Ui;
   //lint -e{1740} Never took ownership of mpc_ListWidgetItem
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set indices

   \param[in] ou32_ViewIndex     View index
   \param[in] ou32_NodeIndex     Node index
   \param[in] os32_ItemIndex     Item index
   \param[in] opc_Item           List item
   \param[in] oq_ShowAssignment  Show assignment flag

   \return
   C_NO_ERR    Initialization successful
   C_CONFIG    Node configuration invalid (too many connections to a bus)
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_SyvDcExistingNodeWidget::SetIndex(const stw_types::uint32 ou32_ViewIndex,
                                           const stw_types::uint32 ou32_NodeIndex, const sint32 os32_ItemIndex,
                                           QListWidgetItem * const opc_Item, const bool oq_ShowAssignment)
{
   const C_OSCNode * const pc_Node = C_PuiSdHandler::h_GetInstance()->GetOSCNodeConst(ou32_NodeIndex);

   if (pc_Node != NULL)
   {
      this->mc_DeviceName = pc_Node->c_DeviceType.c_str();
   }
   else
   {
      this->mc_DeviceName = "";
   }

   this->mu32_ViewIndex = ou32_ViewIndex;
   this->mu32_NodeIndex = ou32_NodeIndex;
   this->ms32_ItemIndex = os32_ItemIndex;
   this->mpc_ListWidgetItem = opc_Item;
   this->mpc_Ui->pc_GroupBoxSerialNumber->setVisible(oq_ShowAssignment);

   return m_Init();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Compare if widget matches to index

   \param[in] ou32_NodeIndex Node index

   \return
   True  Match
   False No match
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_SyvDcExistingNodeWidget::CompareIndex(const uint32 ou32_NodeIndex) const
{
   return (this->mu32_NodeIndex == ou32_NodeIndex);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Do assiment for specified serial number

   \param[in] orc_SerialNumber Serial number
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDcExistingNodeWidget::ConnectSerialNumber(const QString & orc_SerialNumber) const
{
   this->mpc_Ui->pc_WidgetSerialNumber->SetContent(true, orc_SerialNumber);
   C_OgeWiUtil::h_ApplyStylesheetProperty(this->mpc_Ui->pc_WidgetSerialNumber, "Assigned", true);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Disconnect assiment for specified  serial number

   \param[in] orc_SerialNumber Serial number
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDcExistingNodeWidget::DisconnectSerialNumber(const QString & orc_SerialNumber) const
{
   this->mpc_Ui->pc_WidgetSerialNumber->SetContent(false, orc_SerialNumber);
   C_OgeWiUtil::h_ApplyStylesheetProperty(this->mpc_Ui->pc_WidgetSerialNumber, "Assigned", false);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Check if node has serial number assigned

   \return
   True  Assigned
   False Not assigned
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_SyvDcExistingNodeWidget::IsAssigned(void) const
{
   return this->mpc_Ui->pc_WidgetSerialNumber->IsAssigned();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Append all configs for this node

   \param[in,out] orc_Configs All configs
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDcExistingNodeWidget::AppendDeviceConfig(std::vector<C_SyvDcDeviceConfiguation> & orc_Configs) const
{
   C_SyvDcDeviceConfiguation c_Config;
   const stw_scl::C_SCLString c_SerialNumber =
      this->mpc_Ui->pc_WidgetSerialNumber->GetPureSerialNumber().toStdString().c_str();
   const C_PuiSvData * const pc_View = C_PuiSvHandler::h_GetInstance()->GetView(this->mu32_ViewIndex);
   const C_OSCNode * const pc_Node = C_PuiSdHandler::h_GetInstance()->GetOSCNodeConst(this->mu32_NodeIndex);

   C_SyvDcDeviceInformation::h_SerialNumberFromStringToArray(c_SerialNumber, &c_Config.au8_SerialNumber[0]);
   if (((pc_Node != NULL) && (pc_View != NULL)) && (pc_View->GetPcData().GetConnected() == true))
   {
      for (uint32 u32_ItInterface = 0; u32_ItInterface < pc_Node->c_Properties.c_ComInterfaces.size();
           ++u32_ItInterface)
      {
         const C_OSCNodeComInterfaceSettings & rc_CurInterface = pc_Node->c_Properties.c_ComInterfaces[u32_ItInterface];
         if ((rc_CurInterface.q_IsBusConnected == true) &&
             (rc_CurInterface.u32_BusIndex == pc_View->GetPcData().GetBusIndex()))
         {
            //Append current ID
            c_Config.c_NodeIds.push_back(rc_CurInterface.u8_NodeID);
            //Append to overall vector
            orc_Configs.push_back(c_Config);
            //Stop on first find
            break;
         }
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Prepares the widget for starting drag and drop of connected nodes

   \param[in]     orc_DeviceName         Device name (device type)
   \param[in]     oq_DeviceNameValid     Flag if device name is valid
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDcExistingNodeWidget::StartDrag(const QString & orc_DeviceName, const bool oq_DeviceNameValid) const
{
   // Allow only matching devices or devices with unknown device names because of same node ids
   const bool q_Visible = ((this->mc_DeviceName.compare(orc_DeviceName) == 0) ||
                           (oq_DeviceNameValid == false));

   this->mpc_Ui->pc_GroupBoxSerialNumber->setVisible(q_Visible);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Handles the stop of drag and drop of connected nodes
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDcExistingNodeWidget::StopDrag(void) const
{
   this->mpc_Ui->pc_GroupBoxSerialNumber->setVisible(true);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Overwritten paint event slot

   Here: draw background
   (Not automatically drawn in any QWidget derivative)

   \param[in,out] opc_Event Event identification and information
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDcExistingNodeWidget::paintEvent(QPaintEvent * const opc_Event)
{
   C_OgeWiUtil::h_DrawBackground(this);

   QWidget::paintEvent(opc_Event);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Overwritten drag enter event slot

   Here: Accept drag

   \param[in,out] opc_Event Event identification and information
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDcExistingNodeWidget::dragEnterEvent(QDragEnterEvent * const opc_Event)
{
   const QMimeData * const pc_Mime = opc_Event->mimeData();

   if ((pc_Mime != NULL) && (pc_Mime->hasFormat(C_SyvDcExistingNodeWidget::mhc_MimeDataDevice) == true))
   {
      const QString c_DroppedDevice = pc_Mime->data(C_SyvDcExistingNodeWidget::mhc_MimeDataDevice);
      const QString c_DroppedDeviceValid = pc_Mime->data(C_SyvDcExistingNodeWidget::mhc_MimeDataDeviceValid);

      // Allow only matching devices or devices with unknown device names because of same node ids
      if ((this->mc_DeviceName.compare(c_DroppedDevice) == 0) ||
          (c_DroppedDeviceValid == "0"))
      {
         opc_Event->acceptProposedAction();
         C_OgeWiUtil::h_ApplyStylesheetProperty(this->mpc_Ui->pc_WidgetSerialNumber, "Hovered", true);
      }
   }
   QWidget::dragEnterEvent(opc_Event);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Overwritten drag move event slot

   Here: Accept drag

   \param[in,out] opc_Event Event identification and information
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDcExistingNodeWidget::dragMoveEvent(QDragMoveEvent * const opc_Event)
{
   const QMimeData * const pc_Mime = opc_Event->mimeData();

   if ((pc_Mime != NULL) && (pc_Mime->hasFormat(C_SyvDcExistingNodeWidget::mhc_MimeDataDevice) == true))
   {
      opc_Event->acceptProposedAction();
   }
   QWidget::dragMoveEvent(opc_Event);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Overwritten drag leave event slot

   Here: Accept drag

   \param[in,out] opc_Event Event identification and information
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDcExistingNodeWidget::dragLeaveEvent(QDragLeaveEvent * const opc_Event)
{
   C_OgeWiUtil::h_ApplyStylesheetProperty(this->mpc_Ui->pc_WidgetSerialNumber, "Hovered", false);
   QWidget::dragLeaveEvent(opc_Event);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Overwritten drop event slot

   Here: Accept drop

   \param[in,out] opc_Event Event identification and information
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDcExistingNodeWidget::dropEvent(QDropEvent * const opc_Event)
{
   const QMimeData * const pc_Mime = opc_Event->mimeData();

   if ((pc_Mime != NULL) && (pc_Mime->hasFormat(C_SyvDcExistingNodeWidget::mhc_MimeDataDevice) == true))
   {
      const QString c_DroppedDevice = pc_Mime->data(C_SyvDcExistingNodeWidget::mhc_MimeDataDevice);
      const QString c_DroppedDeviceValid = pc_Mime->data(C_SyvDcExistingNodeWidget::mhc_MimeDataDeviceValid);

      //Disconnect previous one
      if (this->mpc_Ui->pc_WidgetSerialNumber->IsAssigned() == true)
      {
         Q_EMIT this->SigDisconnect(this->mu32_NodeIndex,
                                    this->mpc_Ui->pc_WidgetSerialNumber->GetPureSerialNumber());
      }
      if ((this->mc_DeviceName.compare(c_DroppedDevice) == 0) ||
          (c_DroppedDeviceValid == "0"))
      {
         // Device type matches or the found device type is invalid
         // The device type can be invalid if at least two nodes are found with the same node it
         // In this case, it is not possible to read the device name

         QString c_SerialNumber;
         //Connect new one
         if (pc_Mime->hasFormat(C_SyvDcExistingNodeWidget::mhc_MimeData) == true)
         {
            c_SerialNumber = pc_Mime->data(C_SyvDcExistingNodeWidget::mhc_MimeData);
         }
         Q_EMIT this->SigConnect(this->mu32_NodeIndex, c_SerialNumber);
      }
      else
      {
         C_OgeWiCustomMessage c_Box(this, C_OgeWiCustomMessage::E_Type::eERROR);
         c_Box.SetHeading(C_GtGetText::h_GetText("Device Assignment"));
         c_Box.SetDescription(QString(C_GtGetText::h_GetText(
                                         "Device type does not match. Expected \"%1\", dropped: \"%2\"")).arg(this->
                                                                                                              mc_DeviceName).arg(
                                 c_DroppedDevice));
         c_Box.SetCustomMinHeight(180, 180);
         c_Box.Execute();
      }
      //Always disable the hover state after drop
      C_OgeWiUtil::h_ApplyStylesheetProperty(this->mpc_Ui->pc_WidgetSerialNumber, "Hovered", false);
   }
   QWidget::dropEvent(opc_Event);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Init from data

   \return
   C_NO_ERR    Initialization successful
   C_CONFIG    Node configuration invalid (too many connections to a bus)
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_SyvDcExistingNodeWidget::m_Init(void)
{
   const sintn sn_Height = 72;
   const C_PuiSvData * const pc_View = C_PuiSvHandler::h_GetInstance()->GetView(this->mu32_ViewIndex);
   const C_OSCNode * const pc_Node = C_PuiSdHandler::h_GetInstance()->GetOSCNodeConst(this->mu32_NodeIndex);
   sint32 s32_Return = C_NO_ERR;

   if (((pc_Node != NULL) && (pc_View != NULL)) && (pc_View->GetPcData().GetConnected() == true))
   {
      QString c_Ids;
      //Translation: 1: Node name
      this->mpc_Ui->pc_LabelName->setText(QString(C_GtGetText::h_GetText("%1")).arg(
                                             pc_Node->c_Properties.c_Name.c_str()));

      for (uint32 u32_ItInt = 0; u32_ItInt < pc_Node->c_Properties.c_ComInterfaces.size(); ++u32_ItInt)
      {
         const C_OSCNodeComInterfaceSettings & rc_Interface = pc_Node->c_Properties.c_ComInterfaces[u32_ItInt];
         if ((rc_Interface.q_IsBusConnected == true) &&
             (rc_Interface.u32_BusIndex == pc_View->GetPcData().GetBusIndex()))
         {
            if (c_Ids.compare("") == 0)
            {
               c_Ids += QString::number(rc_Interface.u8_NodeID);
               if (rc_Interface.e_InterfaceType == C_OSCSystemBus::eETHERNET)
               {
                  c_Ids += " / IP: ";
                  c_Ids += C_Uti::h_IpAddressToString(rc_Interface.c_Ip.au8_IpAddress);
               }
            }
            else
            {
               if (rc_Interface.e_InterfaceType == C_OSCSystemBus::eETHERNET)
               {
                  c_Ids += QString(", Node-ID: %1").arg(rc_Interface.u8_NodeID);
                  c_Ids += " / IP: ";
                  c_Ids += C_Uti::h_IpAddressToString(rc_Interface.c_Ip.au8_IpAddress);
               }
               else
               {
                  c_Ids += QString(", %1").arg(rc_Interface.u8_NodeID);
               }
               s32_Return = C_CONFIG;
            }
         }
      }
      //Translation: 1: Node ID
      this->mpc_Ui->pc_LabelNodeId->setText(QString(C_GtGetText::h_GetText("Node-ID: %1")).arg(c_Ids));
      //Translation: 1: Node type
      this->mpc_Ui->pc_LabelDeviceType->setText(QString(C_GtGetText::h_GetText("Type: %1")).arg(pc_Node->c_DeviceType.
                                                                                                c_str()));
   }
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

   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Forward request disconnect serial number
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDcExistingNodeWidget::m_OnDisconnectRequest(const QString & orc_SerialNumber)
{
   Q_EMIT this->SigDisconnect(this->mu32_NodeIndex, orc_SerialNumber);
}
