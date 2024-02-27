//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Widget for an existing node entry (header)

   See cpp file for detailed description

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_SYVDCEXISTINGNODEWIDGET_HPP
#define C_SYVDCEXISTINGNODEWIDGET_HPP

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <QWidget>
#include <QListWidgetItem>
#include "stwtypes.hpp"
#include "C_SyvDcSequences.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace Ui
{
class C_SyvDcExistingNodeWidget;
}

namespace stw
{
namespace opensyde_gui
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_SyvDcExistingNodeWidget :
   public QWidget
{
   Q_OBJECT

public:
   explicit C_SyvDcExistingNodeWidget(QWidget * const opc_Parent = NULL);
   ~C_SyvDcExistingNodeWidget(void) override;

   int32_t SetIndex(const uint32_t ou32_ViewIndex, const uint32_t ou32_NodeIndex, const bool oq_PartOfSquad,
                    QListWidgetItem * const opc_Item, const bool oq_ShowAssignment);
   bool CompareIndex(const uint32_t ou32_NodeIndex) const;
   void ConnectSerialNumber(const stw::opensyde_core::C_OscProtocolSerialNumber & orc_SerialNumber,
                            const std::map<uint8_t,
                                           stw::opensyde_core::C_OscDcDeviceOldComConfig> & orc_SubNodeIdsToOldNodeIds)
   const;
   void DisconnectSerialNumber(const stw::opensyde_core::C_OscProtocolSerialNumber & orc_SerialNumber) const;
   bool IsAssigned(void) const;
   void AppendDeviceConfig(std::vector<stw::opensyde_gui_logic::C_SyvDcDeviceConfiguation> & orc_Configs) const;

   void StartDrag(const QString & orc_DeviceName, const bool oq_DeviceNameValid) const;
   void StopDrag(void) const;

   //The signals keyword is necessary for Qt signal slot functionality
   //lint -save -e1736

Q_SIGNALS:
   //lint -restore
   void SigConnect(const uint32_t ou32_NodeIndex,
                   const stw::opensyde_core::C_OscProtocolSerialNumber & orc_SerialNumber, const std::map<uint8_t,
                                                                                                          stw::opensyde_core::C_OscDcDeviceOldComConfig> & orc_SubNodeIdsToOldNodeIds);
   void SigDisconnect(const uint32_t ou32_NodeIndex, const stw::opensyde_core::C_OscProtocolSerialNumber);

protected:
   void paintEvent(QPaintEvent * const opc_Event) override;
   void dragEnterEvent(QDragEnterEvent * const opc_Event) override;
   void dragMoveEvent(QDragMoveEvent * const opc_Event) override;
   void dragLeaveEvent(QDragLeaveEvent * const opc_Event) override;
   void dropEvent(QDropEvent * const opc_Event) override;

private:
   Ui::C_SyvDcExistingNodeWidget * mpc_Ui;
   QListWidgetItem * mpc_ListWidgetItem;
   uint32_t mu32_ViewIndex;
   uint32_t mu32_NodeIndex;
   bool mq_PartOfSquad;
   QString mc_DeviceName;
   static const QString mhc_MIME_DATA;
   static const QString mhc_MIME_DATA_EXT_FORMAT;
   static const QString mhc_MIME_DATA_MANUFACTURER_FORMAT;
   static const QString mhc_MIME_DATA_DEVICE;
   static const QString mhc_MIME_DATA_DEVICE_VALID;
   static const QString mhc_MIME_DATA_SUB_NODE_IDS_TO_OLD_NODE_IDS;

   int32_t m_Init(void);
   void m_OnDisconnectRequest(const stw::opensyde_core::C_OscProtocolSerialNumber & orc_SerialNumber);
   static void mh_AppendDeviceConfigForNode(const uint32_t ou32_NodeIndex, const uint32_t ou32_ConnectedBusIndex,
                                            stw::opensyde_gui_logic::C_SyvDcDeviceConfiguation oc_NodeConfig,
                                            const std::map<uint8_t,
                                                           stw::opensyde_core::C_OscDcDeviceOldComConfig> & orc_SubNodeIdsToOldNodeIds, std::vector<stw::opensyde_gui_logic::C_SyvDcDeviceConfiguation> & orc_Configs);

   //Avoid call
   C_SyvDcExistingNodeWidget(const C_SyvDcExistingNodeWidget &);
   C_SyvDcExistingNodeWidget & operator =(const C_SyvDcExistingNodeWidget &) &;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
