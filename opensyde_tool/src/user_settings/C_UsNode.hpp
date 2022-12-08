//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Project node user settings (header)

   See cpp file for detailed description

   \copyright   Copyright 2018 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_USNODE_H
#define C_USNODE_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <QString>
#include <QMap>
#include "C_UsNodeDatapool.hpp"
#include "C_OscCanProtocol.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_gui_logic
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_UsNode
{
public:
   C_UsNode(void);

   C_UsNodeDatapool GetDatapool(const QString & orc_DatapoolName) const;
   const QList<QString> GetDatapoolKeysInternal(void) const;
   QString GetSelectedDatapoolName(void) const;
   stw::opensyde_core::C_OscCanProtocol::E_Type GetSelectedProtocol(void) const;
   uint32_t GetSelectedInterface(void) const;
   const std::vector<int32_t> & GetCanOpenOverviewColumnWidth(void) const;
   const std::vector<int32_t> & GetCanOpenPdoOverviewColumnWidth(void) const;
   uint8_t GetSelectedCanOpenManager(void) const;
   uint8_t GetSelectedCanOpenDeviceInterfaceNumber(void) const;
   QString GetSelectedCanOpenDeviceNodeName(void) const;
   uint32_t GetSelectedCanOpenDeviceUseCaseIndex(void) const;
   std::map<uint8_t, bool> GetExpandedCanOpenManager(void) const;
   std::map<uint8_t, bool> GetExpandedCanOpenDevices(void) const;
   std::map<std::pair<uint8_t, std::pair<uint8_t, stw::scl::C_SclString> >,
            bool> GetExpandedCanOpenDevice(void) const;
   bool GetCanOpenSelectedUseCaseOrInterface(void) const;
   const std::vector<int32_t> & GetHalcOverviewColumnWidth(void) const;
   const std::vector<int32_t> & GetHalcConfigColumnWidth(void) const;
   QString GetSelectedHalcDomainName(void) const;
   QString GetSelectedHalcChannel(void) const;

   void SetSelectedDatapoolName(const QString & orc_DatapoolName);
   void SetSelectedProtocol(const stw::opensyde_core::C_OscCanProtocol::E_Type oe_Protocol);
   void SetSelectedInterface(const uint32_t ou32_InterfaceIndex);
   void SetDatapoolOpenListNames(const QString & orc_DatapoolName, const std::vector<QString> & orc_New);
   void SetDatapoolSelectedListNames(const QString & orc_DatapoolName, const std::vector<QString> & orc_New);
   void SetDatapoolSelectedVariableNames(const QString & orc_DatapoolName, const std::vector<QString> & orc_New);
   void SetDatapoolCommMessageOverviewColumnWidth(const QString & orc_DatapoolName, const QString & orc_ListName,
                                                  const std::vector<int32_t> & orc_Value);
   void SetDatapoolCommSignalOverviewColumnWidth(const QString & orc_DatapoolName, const QString & orc_ListName,
                                                 const std::vector<int32_t> & orc_Value);
   void SetDatapoolListSelectedMessage(const QString & orc_DatapoolName, const QString & orc_ListName,
                                       const stw::opensyde_core::C_OscCanProtocol::E_Type oe_SelectedProtocol,
                                       const bool oq_MessageSelected, const QString & orc_SelectedMessageName,
                                       const bool oq_SignalSelected, const QString & orc_SelectedSignalName);
   void SetDatapoolListColumnSizes(const QString & orc_DatapoolName, const QString & orc_ListName,
                                   const std::vector<int32_t> & orc_ColumnWidths);
   void SetCanOpenOverviewColumnWidth(const std::vector<int32_t> & orc_Value);
   void SetCanOpenPdoOverviewColumnWidth(const std::vector<int32_t> & orc_Value);
   void SetSelectedCanOpenManager(const uint8_t & oru8_Value);
   void SetSelectedCanOpenDevice(const uint8_t & oru8_Number, const QString & orc_Value);
   void SetSelectedCanOpenDeviceUseCaseIndex(const uint32_t & oru32_Value);
   void SetExpandedCanOpenManager(const std::map<uint8_t, bool> & orc_Interfaces);
   void SetExpandedCanOpenDevices(const std::map<uint8_t, bool> & orc_Devices);
   void SetExpandedCanOpenDevice(const std::map<std::pair<uint8_t, std::pair<uint8_t,
                                                                             stw::scl::C_SclString> >,
                                                bool> & orc_Device);
   void SetCanOpenSelectedUseCaseOrInterface(const bool orq_IsUseCaseSelected);
   void SetHalcOverviewColumnWidth(const std::vector<int32_t> & orc_Value);
   void SetHalcConfigColumnWidth(const std::vector<int32_t> & orc_Value);
   void SetSelectedHalcDomain(const QString & orc_Value);
   void SetSelectedHalcChannel(const QString & orc_Value);

private:
   std::vector<int32_t> mc_CanOpenOverviewColumnWidth;
   std::vector<int32_t> mc_CanOpenPdoOverviewColumnWidth;
   uint8_t mu8_CanOpenManager;
   uint8_t mu8_DeviceInterfaceNumber;
   QString mc_CanOpenNodeName;
   uint32_t mu32_CanOpenDeviceUseCaseIndex;
   std::map<uint8_t, bool> mc_CanOpenExpandedManager;
   std::map<uint8_t, bool> mc_CanOpenExpandedDevices;
   std::map<std::pair<uint8_t, std::pair<uint8_t, stw::scl::C_SclString> >,
            bool> mc_CanOpenExpandedDevice;
   bool mq_IsUseCaseIndexSelected;
   std::vector<int32_t> mc_HalcOverviewColumnWidth;
   std::vector<int32_t> mc_HalcConfigColumnWidth;
   QString mc_SelectedDataPoolName;
   QString mc_SelectedHalcDomainName;
   QString mc_SelectedHalcChannelId;
   stw::opensyde_core::C_OscCanProtocol::E_Type me_SelectedProtocol;
   uint32_t mu32_SelectedInterface;
   QMap<QString, C_UsNodeDatapool> mc_Datapools; ///< History of last known node datapool user settings
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
