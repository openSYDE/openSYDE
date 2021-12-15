//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Widget for an existing node entry (header)

   See cpp file for detailed description

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_SYVDCEXISTINGNODEWIDGET_H
#define C_SYVDCEXISTINGNODEWIDGET_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <QWidget>
#include <QListWidgetItem>
#include "stwtypes.h"
#include "C_SyvDcSequences.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace Ui
{
class C_SyvDcExistingNodeWidget;
}

namespace stw_opensyde_gui
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_SyvDcExistingNodeWidget :
   public QWidget
{
   Q_OBJECT

public:
   explicit C_SyvDcExistingNodeWidget(QWidget * const opc_Parent = NULL);
   ~C_SyvDcExistingNodeWidget(void);

   stw_types::sint32 SetIndex(const stw_types::uint32 ou32_ViewIndex, const stw_types::uint32 ou32_NodeIndex,
                              const bool oq_PartOfSquad, QListWidgetItem * const opc_Item,
                              const bool oq_ShowAssignment);
   bool CompareIndex(const stw_types::uint32 ou32_NodeIndex) const;
   void ConnectSerialNumber(const stw_opensyde_core::C_OSCProtocolSerialNumber & orc_SerialNumber,
                            const std::map<stw_types::uint8,
                                           stw_opensyde_gui_logic::C_SyvDcDeviceOldComConfig> & orc_SubNodeIdsToOldNodeIds)
   const;
   void DisconnectSerialNumber(const stw_opensyde_core::C_OSCProtocolSerialNumber & orc_SerialNumber) const;
   bool IsAssigned(void) const;
   void AppendDeviceConfig(std::vector<stw_opensyde_gui_logic::C_SyvDcDeviceConfiguation> & orc_Configs) const;

   void StartDrag(const QString & orc_DeviceName, const bool oq_DeviceNameValid) const;
   void StopDrag(void) const;

   //The signals keyword is necessary for Qt signal slot functionality
   //lint -save -e1736

Q_SIGNALS:
   //lint -restore
   void SigConnect(const stw_types::uint32 ou32_NodeIndex,
                   const stw_opensyde_core::C_OSCProtocolSerialNumber & orc_SerialNumber,
                   const std::map<stw_types::uint8,
                                  stw_opensyde_gui_logic::C_SyvDcDeviceOldComConfig> & orc_SubNodeIdsToOldNodeIds);
   void SigDisconnect(const stw_types::uint32 ou32_NodeIndex, const stw_opensyde_core::C_OSCProtocolSerialNumber);

protected:
   virtual void paintEvent(QPaintEvent * const opc_Event) override;
   virtual void dragEnterEvent(QDragEnterEvent * const opc_Event) override;
   virtual void dragMoveEvent(QDragMoveEvent * const opc_Event) override;
   virtual void dragLeaveEvent(QDragLeaveEvent * const opc_Event) override;
   virtual void dropEvent(QDropEvent * const opc_Event) override;

private:
   Ui::C_SyvDcExistingNodeWidget * mpc_Ui;
   QListWidgetItem * mpc_ListWidgetItem;
   stw_types::uint32 mu32_ViewIndex;
   stw_types::uint32 mu32_NodeIndex;
   bool mq_PartOfSquad;
   QString mc_DeviceName;
   static const QString mhc_MimeData;
   static const QString mhc_MimeDataExtFormat;
   static const QString mhc_MimeDataManufacturerFormat;
   static const QString mhc_MimeDataDevice;
   static const QString mhc_MimeDataDeviceValid;
   static const QString mhc_MimeDataSubNodeIdsToOldNodeIds;

   stw_types::sint32 m_Init(void);
   void m_OnDisconnectRequest(const stw_opensyde_core::C_OSCProtocolSerialNumber & orc_SerialNumber);
   static void mh_AppendDeviceConfigForNode(const stw_types::uint32 ou32_NodeIndex,
                                            const stw_types::uint32 ou32_ConnectedBusIndex,
                                            stw_opensyde_gui_logic::C_SyvDcDeviceConfiguation oc_NodeConfig,
                                            const std::map<stw_types::uint8,
                                                           stw_opensyde_gui_logic::C_SyvDcDeviceOldComConfig> & orc_SubNodeIdsToOldNodeIds, std::vector<stw_opensyde_gui_logic::C_SyvDcDeviceConfiguation> & orc_Configs);

   //Avoid call
   C_SyvDcExistingNodeWidget(const C_SyvDcExistingNodeWidget &);
   C_SyvDcExistingNodeWidget & operator =(const C_SyvDcExistingNodeWidget &);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif
