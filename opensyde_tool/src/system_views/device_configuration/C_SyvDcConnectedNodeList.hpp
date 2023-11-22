//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       List for all connected node entries (header)

   See cpp file for detailed description

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_SYVDCCONNECTEDNODELIST_HPP
#define C_SYVDCCONNECTEDNODELIST_HPP

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <vector>
#include <QListWidget>
#include "stwtypes.hpp"
#include "C_SyvDcSequences.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_gui
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_SyvDcConnectedNodeList :
   public QListWidget
{
   Q_OBJECT

public:
   C_SyvDcConnectedNodeList(QWidget * const opc_Parent = NULL);

   void SetData(const std::vector<stw::opensyde_gui_logic::C_SyvDcDeviceInformation> & orc_Infos);
   void EnableSerialNumber(const stw::opensyde_core::C_OscProtocolSerialNumber & orc_SerialNumber) const;
   void DisableSerialNumber(const stw::opensyde_core::C_OscProtocolSerialNumber & orc_SerialNumber) const;

   //The signals keyword is necessary for Qt signal slot functionality
   //lint -save -e1736

Q_SIGNALS:
   //lint -restore
   void SigStartDrag(const QString & orc_DeviceName, const bool oq_DeviceNameValid);
   void SigStopDrag(void);

protected:
   void startDrag(const Qt::DropActions oc_Actions) override;
   QMimeData * mimeData(const QList<QListWidgetItem *> oc_Items) const override;

private:
   bool mq_GridSizeSet;
   std::vector<stw::opensyde_gui_logic::C_SyvDcDeviceInformation> mc_Data;
   static const QString mhc_MIME_DATA;
   static const QString mhc_MIME_DATA_EXT_FORMAT;
   static const QString mhc_MIME_DATA_MANUFACTURER_FORMAT;
   static const QString mhc_MIME_DATA_DEVICE;
   static const QString mhc_MIME_DATA_DEVICE_VALID;
   static const QString mhc_MIME_DATA_SUB_NODE_IDS_TO_OLD_NODE_IDS;

   void m_Init(void);
   void m_AppendNode(const stw::opensyde_gui_logic::C_SyvDcDeviceInformation & orc_Info, const std::map<uint8_t,
                                                                                                        stw::opensyde_gui_logic::C_SyvDcDeviceOldComConfig> & orc_SubNodeIdsToOldNodeIds);

   void m_ScrollBarRangeChangedVer(const int32_t os32_Min, const int32_t os32_Max) const;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
