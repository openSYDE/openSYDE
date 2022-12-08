//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Project node user settings (implementation)

   Project node user settings

   \copyright   Copyright 2018 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.hpp"

#include "C_UsNode.hpp"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw::opensyde_gui_logic;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

/* -- Global Variables ---------------------------------------------------------------------------------------------- */

/* -- Module Global Variables --------------------------------------------------------------------------------------- */

/* -- Module Global Function Prototypes ----------------------------------------------------------------------------- */

/* -- Implementation ------------------------------------------------------------------------------------------------ */

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default constructor
*/
//----------------------------------------------------------------------------------------------------------------------
C_UsNode::C_UsNode(void) :
   mu8_CanOpenManager(0),
   mu8_DeviceInterfaceNumber(0),
   mu32_CanOpenDeviceUseCaseIndex(0),
   mq_IsUseCaseIndexSelected(false),
   me_SelectedProtocol(stw::opensyde_core::C_OscCanProtocol::eLAYER2),
   mu32_SelectedInterface(0)
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get project system definition node datapool user settings

   \param[in]  orc_DatapoolName  Project system definition node datapool name (identifier)

   \return
   Project system definition node datapool user settings
*/
//----------------------------------------------------------------------------------------------------------------------
C_UsNodeDatapool C_UsNode::GetDatapool(const QString & orc_DatapoolName) const
{
   return this->mc_Datapools.value(orc_DatapoolName, C_UsNodeDatapool());
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get datapool keys internal structure

   \return
   Datapool keys internal structure
*/
//----------------------------------------------------------------------------------------------------------------------
const QList<QString> C_UsNode::GetDatapoolKeysInternal(void) const
{
   return this->mc_Datapools.keys();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get selected data pool name

   \return
   Selected data pool name
*/
//----------------------------------------------------------------------------------------------------------------------
QString C_UsNode::GetSelectedDatapoolName(void) const
{
   return this->mc_SelectedDataPoolName;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get selected protocol type

   \return
   Selected protocol type
*/
//----------------------------------------------------------------------------------------------------------------------
stw::opensyde_core::C_OscCanProtocol::E_Type C_UsNode::GetSelectedProtocol(void) const
{
   return this->me_SelectedProtocol;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get selected CAN interface index

   \return
   Selected CAN interface index
*/
//----------------------------------------------------------------------------------------------------------------------
uint32_t C_UsNode::GetSelectedInterface(void) const
{
   return this->mu32_SelectedInterface;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get CANopen overview column width

   \return
   CANopen overview column width
*/
//----------------------------------------------------------------------------------------------------------------------
const std::vector<int32_t> & C_UsNode::GetCanOpenOverviewColumnWidth(void) const
{
   return this->mc_CanOpenOverviewColumnWidth;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get CANopen PDO overview column width

   \return
   CANopen PDO overview column width
*/
//----------------------------------------------------------------------------------------------------------------------
const std::vector<int32_t> & C_UsNode::GetCanOpenPdoOverviewColumnWidth(void) const
{
   return this->mc_CanOpenPdoOverviewColumnWidth;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get CANopen manager

   \return
   CANopen manager interface number
*/
//----------------------------------------------------------------------------------------------------------------------
uint8_t C_UsNode::GetSelectedCanOpenManager(void) const
{
   return this->mu8_CanOpenManager;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get CANopen device interface number

   \return
   CANopen device interface number
*/
//----------------------------------------------------------------------------------------------------------------------
uint8_t C_UsNode::GetSelectedCanOpenDeviceInterfaceNumber(void) const
{
   return this->mu8_DeviceInterfaceNumber;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get CANopen device node name

   \return
   CANopen device node name
*/
//----------------------------------------------------------------------------------------------------------------------
QString C_UsNode::GetSelectedCanOpenDeviceNodeName(void) const
{
   return this->mc_CanOpenNodeName;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get CANopen device use case

   \return
   CANopen device use case index
*/
//----------------------------------------------------------------------------------------------------------------------
uint32_t C_UsNode::GetSelectedCanOpenDeviceUseCaseIndex(void) const
{
   return this->mu32_CanOpenDeviceUseCaseIndex;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get expanded CANopen manager

   \return
   CANopen expandend manager
*/
//----------------------------------------------------------------------------------------------------------------------
std::map<uint8_t, bool> C_UsNode::GetExpandedCanOpenManager(void) const
{
   return this->mc_CanOpenExpandedManager;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get expanded CANopen devices

   \return
   CANopen expanded devices
*/
//----------------------------------------------------------------------------------------------------------------------
std::map<uint8_t, bool> C_UsNode::GetExpandedCanOpenDevices(void) const
{
   return this->mc_CanOpenExpandedDevices;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get expanded CANopen device

   \return
   CANopen expanded device
*/
//----------------------------------------------------------------------------------------------------------------------
std::map<std::pair<uint8_t, std::pair<uint8_t, stw::scl::C_SclString> >,
         bool> C_UsNode::GetExpandedCanOpenDevice(void) const
{
   return this->mc_CanOpenExpandedDevice;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get selected item

   \return
   true:    Use case is selected
   false:   Interface is selected
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_UsNode::GetCanOpenSelectedUseCaseOrInterface(void) const
{
   return this->mq_IsUseCaseIndexSelected;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get halc overview column width

   \return
   Halc overview column width
*/
//----------------------------------------------------------------------------------------------------------------------
const std::vector<int32_t> & C_UsNode::GetHalcOverviewColumnWidth(void) const
{
   return this->mc_HalcOverviewColumnWidth;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get HALC parameter configuration table column widths

   \return
   widths of parameter configuration table columns
*/
//----------------------------------------------------------------------------------------------------------------------
const std::vector<int32_t> & C_UsNode::GetHalcConfigColumnWidth() const
{
   return this->mc_HalcConfigColumnWidth;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get selected HALC domain name resp. domain name of selected HALC channel

   \return
   selected HALC domain name
*/
//----------------------------------------------------------------------------------------------------------------------
QString C_UsNode::GetSelectedHalcDomainName(void) const
{
   return this->mc_SelectedHalcDomainName;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get selected HALC channel ID

   \return
   Selected channel ID
*/
//----------------------------------------------------------------------------------------------------------------------
QString C_UsNode::GetSelectedHalcChannel(void) const
{
   return this->mc_SelectedHalcChannelId;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set selected data pool name

   \param[in]  orc_DatapoolName  Selected data pool name
*/
//----------------------------------------------------------------------------------------------------------------------
void C_UsNode::SetSelectedDatapoolName(const QString & orc_DatapoolName)
{
   this->mc_SelectedDataPoolName = orc_DatapoolName;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set selected protocol

   \param[in]  oe_Protocol    Selected protocol
*/
//----------------------------------------------------------------------------------------------------------------------
void C_UsNode::SetSelectedProtocol(const stw::opensyde_core::C_OscCanProtocol::E_Type oe_Protocol)
{
   this->me_SelectedProtocol = oe_Protocol;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get selected CAN interface index

   \param[in]  ou32_InterfaceIndex    Selected CAN interface index
*/
//----------------------------------------------------------------------------------------------------------------------
void C_UsNode::SetSelectedInterface(const uint32_t ou32_InterfaceIndex)
{
   this->mu32_SelectedInterface = ou32_InterfaceIndex;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set project system definition node datapool expanded list names

   \param[in]  orc_DatapoolName  Project system definition node datapool name (identifier)
   \param[in]  orc_New           Expanded list names
*/
//----------------------------------------------------------------------------------------------------------------------
void C_UsNode::SetDatapoolOpenListNames(const QString & orc_DatapoolName, const std::vector<QString> & orc_New)
{
   if (this->mc_Datapools.contains(orc_DatapoolName) == true)
   {
      //Do not insert as this will replace all currently known user settings for this item
      this->mc_Datapools.operator [](orc_DatapoolName).SetExpandedListNames(orc_New);
   }
   else
   {
      C_UsNodeDatapool c_Datapool;
      c_Datapool.SetExpandedListNames(orc_New);
      this->mc_Datapools.insert(orc_DatapoolName, c_Datapool);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set project system definition node datapool selected list names

   \param[in]  orc_DatapoolName  Project system definition node datapool name (identifier)
   \param[in]  orc_New           Selected list names
*/
//----------------------------------------------------------------------------------------------------------------------
void C_UsNode::SetDatapoolSelectedListNames(const QString & orc_DatapoolName, const std::vector<QString> & orc_New)
{
   if (this->mc_Datapools.contains(orc_DatapoolName) == true)
   {
      //Do not insert as this will replace all currently known user settings for this item
      this->mc_Datapools.operator [](orc_DatapoolName).SetSelectedListNames(orc_New);
   }
   else
   {
      C_UsNodeDatapool c_Datapool;
      c_Datapool.SetSelectedListNames(orc_New);
      this->mc_Datapools.insert(orc_DatapoolName, c_Datapool);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set project system definition node datapool selected variable names

   \param[in]  orc_DatapoolName  Project system definition node datapool name (identifier)
   \param[in]  orc_New           Selected variable names
*/
//----------------------------------------------------------------------------------------------------------------------
void C_UsNode::SetDatapoolSelectedVariableNames(const QString & orc_DatapoolName, const std::vector<QString> & orc_New)
{
   if (this->mc_Datapools.contains(orc_DatapoolName) == true)
   {
      //Do not insert as this will replace all currently known user settings for this item
      this->mc_Datapools.operator [](orc_DatapoolName).SetSelectedVariableNames(orc_New);
   }
   else
   {
      C_UsNodeDatapool c_Datapool;
      c_Datapool.SetSelectedVariableNames(orc_New);
      this->mc_Datapools.insert(orc_DatapoolName, c_Datapool);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Set datapool comm overview column width

   \param[in]  orc_DatapoolName  Datapool name
   \param[in]  orc_ListName      List name
   \param[in]  orc_Value         Value
*/
//----------------------------------------------------------------------------------------------------------------------
void C_UsNode::SetDatapoolCommMessageOverviewColumnWidth(const QString & orc_DatapoolName, const QString & orc_ListName,
                                                         const std::vector<int32_t> & orc_Value)
{
   if (this->mc_Datapools.contains(orc_DatapoolName) == true)
   {
      //Do not insert as this will replace all currently known user settings for this item
      this->mc_Datapools.operator [](orc_DatapoolName).SetCommMessageOverviewColumnWidth(orc_ListName, orc_Value);
   }
   else
   {
      C_UsNodeDatapool c_Datapool;
      c_Datapool.SetCommMessageOverviewColumnWidth(orc_ListName, orc_Value);
      this->mc_Datapools.insert(orc_DatapoolName, c_Datapool);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Set datapool comm signal overview column width

   \param[in]  orc_DatapoolName  Datapool name
   \param[in]  orc_ListName      List name
   \param[in]  orc_Value         Value
*/
//----------------------------------------------------------------------------------------------------------------------
void C_UsNode::SetDatapoolCommSignalOverviewColumnWidth(const QString & orc_DatapoolName, const QString & orc_ListName,
                                                        const std::vector<int32_t> & orc_Value)
{
   if (this->mc_Datapools.contains(orc_DatapoolName) == true)
   {
      //Do not insert as this will replace all currently known user settings for this item
      this->mc_Datapools.operator [](orc_DatapoolName).SetCommSignalOverviewColumnWidth(orc_ListName, orc_Value);
   }
   else
   {
      C_UsNodeDatapool c_Datapool;
      c_Datapool.SetCommSignalOverviewColumnWidth(orc_ListName, orc_Value);
      this->mc_Datapools.insert(orc_DatapoolName, c_Datapool);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set datapool list selected message

   \param[in]  orc_DatapoolName           Datapool name (identifier)
   \param[in]  orc_ListName               Datapool list name (identifier)
   \param[in]  oe_SelectedProtocol        Protocol type
   \param[in]  oq_MessageSelected         Set flag if there is a selected message
   \param[in]  orc_SelectedMessageName    Selected message name if any
   \param[in]  oq_SignalSelected          Flag if signal selected
   \param[in]  orc_SelectedSignalName     Selected signal name if any
*/
//----------------------------------------------------------------------------------------------------------------------
void C_UsNode::SetDatapoolListSelectedMessage(const QString & orc_DatapoolName, const QString & orc_ListName,
                                              const stw::opensyde_core::C_OscCanProtocol::E_Type oe_SelectedProtocol,
                                              const bool oq_MessageSelected, const QString & orc_SelectedMessageName,
                                              const bool oq_SignalSelected, const QString & orc_SelectedSignalName)
{
   if (this->mc_Datapools.contains(orc_DatapoolName) == true)
   {
      //Do not insert as this will replace all currently known user settings for this item
      this->mc_Datapools.operator [](orc_DatapoolName).SetListSelectedMessage(orc_ListName, oe_SelectedProtocol,
                                                                              oq_MessageSelected,
                                                                              orc_SelectedMessageName,
                                                                              oq_SignalSelected,
                                                                              orc_SelectedSignalName);
   }
   else
   {
      C_UsNodeDatapool c_Datapool;
      c_Datapool.SetListSelectedMessage(orc_ListName, oe_SelectedProtocol, oq_MessageSelected, orc_SelectedMessageName,
                                        oq_SignalSelected, orc_SelectedSignalName);
      this->mc_Datapools.insert(orc_DatapoolName, c_Datapool);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set datapool list column widths

   \param[in]  orc_DatapoolName  Project system definition node datapool name (identifier)
   \param[in]  orc_ListName      Project system definition node datapool list name (identifier)
   \param[in]  orc_ColumnWidths  Last known column widths
*/
//----------------------------------------------------------------------------------------------------------------------
void C_UsNode::SetDatapoolListColumnSizes(const QString & orc_DatapoolName, const QString & orc_ListName,
                                          const std::vector<int32_t> & orc_ColumnWidths)
{
   if (this->mc_Datapools.contains(orc_DatapoolName) == true)
   {
      //Do not insert as this will replace all currently known user settings for this item
      this->mc_Datapools.operator [](orc_DatapoolName).SetListColumnSizes(orc_ListName, orc_ColumnWidths);
   }
   else
   {
      C_UsNodeDatapool c_Datapool;
      c_Datapool.SetListColumnSizes(orc_ListName, orc_ColumnWidths);
      this->mc_Datapools.insert(orc_DatapoolName, c_Datapool);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Set CANopen overview column width

   \param[in]  orc_Value   Value
*/
//----------------------------------------------------------------------------------------------------------------------
void C_UsNode::SetCanOpenOverviewColumnWidth(const std::vector<int32_t> & orc_Value)
{
   this->mc_CanOpenOverviewColumnWidth = orc_Value;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Set CANopen PDO overview column width

   \param[in]  orc_Value   Value
*/
//----------------------------------------------------------------------------------------------------------------------
void C_UsNode::SetCanOpenPdoOverviewColumnWidth(const std::vector<int32_t> & orc_Value)
{
   this->mc_CanOpenPdoOverviewColumnWidth = orc_Value;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Set selected CANopen manager

   \param[in]  oru8_Value   Value
*/
//----------------------------------------------------------------------------------------------------------------------
void C_UsNode::SetSelectedCanOpenManager(const uint8_t & oru8_Value)
{
   this->mu8_CanOpenManager = oru8_Value;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Set selected CANopen device

   \param[in]  oru8_Number     Number
   \param[in]  orc_Value      Value
*/
//----------------------------------------------------------------------------------------------------------------------
void C_UsNode::SetSelectedCanOpenDevice(const uint8_t & oru8_Number, const QString & orc_Value)
{
   this->mu8_DeviceInterfaceNumber = oru8_Number;
   this->mc_CanOpenNodeName = orc_Value;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Set selected CANopen device use case

   \param[in]  oru32_Value   Value
*/
//----------------------------------------------------------------------------------------------------------------------
void C_UsNode::SetSelectedCanOpenDeviceUseCaseIndex(const uint32_t & oru32_Value)
{
   this->mu32_CanOpenDeviceUseCaseIndex = oru32_Value;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Set expanded CANopen manager

   \param[in]  orc_Interfaces   Interfaces
*/
//----------------------------------------------------------------------------------------------------------------------
void C_UsNode::SetExpandedCanOpenManager(const std::map<uint8_t, bool> & orc_Interfaces)
{
   this->mc_CanOpenExpandedManager = orc_Interfaces;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Set expanded CANopen devices

   \param[in]  orc_Devices   Devices
*/
//----------------------------------------------------------------------------------------------------------------------
void C_UsNode::SetExpandedCanOpenDevices(const std::map<uint8_t, bool> & orc_Devices)
{
   this->mc_CanOpenExpandedDevices = orc_Devices;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Set expanded CANopen device

   \param[in]  orc_Device   Device
*/
//----------------------------------------------------------------------------------------------------------------------
void C_UsNode::SetExpandedCanOpenDevice(const std::map<std::pair<uint8_t, std::pair<uint8_t,
                                                                                    stw::scl::C_SclString> >,
                                                       bool> & orc_Device)
{
   this->mc_CanOpenExpandedDevice = orc_Device;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Set selected item

   \param[in]  orq_IsUseCaseSelected   true or false
*/
//----------------------------------------------------------------------------------------------------------------------
void C_UsNode::SetCanOpenSelectedUseCaseOrInterface(const bool orq_IsUseCaseSelected)
{
   this->mq_IsUseCaseIndexSelected = orq_IsUseCaseSelected;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Set halc overview column width

   \param[in]  orc_Value   Value
*/
//----------------------------------------------------------------------------------------------------------------------
void C_UsNode::SetHalcOverviewColumnWidth(const std::vector<int32_t> & orc_Value)
{
   this->mc_HalcOverviewColumnWidth = orc_Value;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Set HALC parameter configuration table column widths

   \param[in]  orc_Value   Column widths
*/
//----------------------------------------------------------------------------------------------------------------------
void C_UsNode::SetHalcConfigColumnWidth(const std::vector<int32_t> & orc_Value)
{
   this->mc_HalcConfigColumnWidth = orc_Value;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Set selected HALC domain name

   \param[in]  orc_Value   Domain name
*/
//----------------------------------------------------------------------------------------------------------------------
void C_UsNode::SetSelectedHalcDomain(const QString & orc_Value)
{
   this->mc_SelectedHalcDomainName = orc_Value;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Set selected HALC channel ID

   \param[in]  orc_Value   Channel ID
*/
//----------------------------------------------------------------------------------------------------------------------
void C_UsNode::SetSelectedHalcChannel(const QString & orc_Value)
{
   this->mc_SelectedHalcChannelId = orc_Value;
}
