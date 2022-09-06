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
#include "C_UsNodeDatapool.h"
#include "C_OSCCanProtocol.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw_opensyde_gui_logic
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
   stw_opensyde_core::C_OSCCanProtocol::E_Type GetSelectedProtocol(void) const;
   stw_types::uint32 GetSelectedInterface(void) const;
   const std::vector<stw_types::sint32> & GetCANopenOverviewColumnWidth(void) const;
   const std::vector<stw_types::sint32> & GetCANopenPdoOverviewColumnWidth(void) const;
   stw_types::uint8 GetSelectedCANopenManager(void) const;
   stw_types::uint8 GetSelectedCANopenDeviceInterfaceNumber(void) const;
   QString GetSelectedCANopenDeviceNodeName(void) const;
   stw_types::uint32 GetSelectedCANopenDeviceUseCaseIndex(void) const;
   std::map<stw_types::uint8, bool> GetExpandedCANopenManager(void) const;
   std::map<stw_types::uint8, bool> GetExpandedCANopenDevices(void) const;
   std::map<std::pair<stw_types::uint8, std::pair<stw_types::uint8, stw_scl::C_SCLString> >,
            bool> GetExpandedCANopenDevice(void) const;
   bool GetCANopenSelectedUseCaseOrInterface(void) const;
   const std::vector<stw_types::sint32> & GetHalcOverviewColumnWidth(void) const;
   const std::vector<stw_types::sint32> & GetHalcConfigColumnWidth(void) const;
   QString GetSelectedHalcDomainName(void) const;
   QString GetSelectedHalcChannel(void) const;

   void SetSelectedDatapoolName(const QString & orc_DatapoolName);
   void SetSelectedProtocol(const stw_opensyde_core::C_OSCCanProtocol::E_Type oe_Protocol);
   void SetSelectedInterface(const stw_types::uint32 ou32_InterfaceIndex);
   void SetDatapoolOpenListNames(const QString & orc_DatapoolName, const std::vector<QString> & orc_New);
   void SetDatapoolSelectedListNames(const QString & orc_DatapoolName, const std::vector<QString> & orc_New);
   void SetDatapoolSelectedVariableNames(const QString & orc_DatapoolName, const std::vector<QString> & orc_New);
   void SetDatapoolCommMessageOverviewColumnWidth(const QString & orc_DatapoolName, const QString & orc_ListName,
                                                  const std::vector<stw_types::sint32> & orc_Value);
   void SetDatapoolCommSignalOverviewColumnWidth(const QString & orc_DatapoolName, const QString & orc_ListName,
                                                 const std::vector<stw_types::sint32> & orc_Value);
   void SetDatapoolListSelectedMessage(const QString & orc_DatapoolName, const QString & orc_ListName,
                                       const stw_opensyde_core::C_OSCCanProtocol::E_Type oe_SelectedProtocol,
                                       const bool oq_MessageSelected, const QString & orc_SelectedMessageName,
                                       const bool oq_SignalSelected, const QString & orc_SelectedSignalName);
   void SetDatapoolListColumnSizes(const QString & orc_DatapoolName, const QString & orc_ListName,
                                   const std::vector<stw_types::sint32> & orc_ColumnWidths);
   void SetCANopenOverviewColumnWidth(const std::vector<stw_types::sint32> & orc_Value);
   void SetCANopenPdoOverviewColumnWidth(const std::vector<stw_types::sint32> & orc_Value);
   void SetSelectedCANopenManager(const stw_types::uint8 & oru8_Value);
   void SetSelectedCANopenDevice(const stw_types::uint8 & oru8_Number, const QString & orc_Value);
   void SetSelectedCANopenDeviceUseCaseIndex(const stw_types::uint32 & oru32_Value);
   void SetExpandedCANopenManager(const std::map<stw_types::uint8, bool> & orc_Interfaces);
   void SetExpandedCANopenDevices(const std::map<stw_types::uint8, bool> & orc_Devices);
   void SetExpandedCANopenDevice(const std::map<std::pair<stw_types::uint8, std::pair<stw_types::uint8,
                                                                                      stw_scl::C_SCLString> >,
                                                bool> & orc_Device);
   void SetCANopenSelectedUseCaseOrInterface(const bool orq_IsUseCaseSelected);
   void SetHalcOverviewColumnWidth(const std::vector<stw_types::sint32> & orc_Value);
   void SetHalcConfigColumnWidth(const std::vector<stw_types::sint32> & orc_Value);
   void SetSelectedHalcDomain(const QString & orc_Value);
   void SetSelectedHalcChannel(const QString & orc_Value);

private:
   std::vector<stw_types::sint32> mc_CANopenOverviewColumnWidth;
   std::vector<stw_types::sint32> mc_CANopenPdoOverviewColumnWidth;
   stw_types::uint8 mu8_CANopenManager;
   stw_types::uint8 mu8_DeviceInterfaceNumber;
   QString mc_CANopenNodeName;
   stw_types::uint32 mu32_CANopenDeviceUseCaseIndex;
   std::map<stw_types::uint8, bool> mc_CANopenExpandedManager;
   std::map<stw_types::uint8, bool> mc_CANopenExpandedDevices;
   std::map<std::pair<stw_types::uint8, std::pair<stw_types::uint8, stw_scl::C_SCLString> >,
            bool> mc_CANopenExpandedDevice;
   bool mq_IsUseCaseIndexSelected;
   std::vector<stw_types::sint32> mc_HalcOverviewColumnWidth;
   std::vector<stw_types::sint32> mc_HalcConfigColumnWidth;
   QString mc_SelectedDataPoolName;
   QString mc_SelectedHalcDomainName;
   QString mc_SelectedHalcChannelId;
   stw_opensyde_core::C_OSCCanProtocol::E_Type me_SelectedProtocol;
   stw_types::uint32 mu32_SelectedInterface;
   QMap<QString, C_UsNodeDatapool> mc_Datapools; ///< History of last known node datapool user settings
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif
