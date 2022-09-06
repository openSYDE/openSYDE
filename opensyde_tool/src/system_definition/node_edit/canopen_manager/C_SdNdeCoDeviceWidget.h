//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Widget with current CANopen settings of Manager for a specific device on one interface
   \copyright   Copyright 2022 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_SDNDECODEVICEWIDGET_H
#define C_SDNDECODEVICEWIDGET_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <QWidget>

#include "stwtypes.h"
#include "C_OSCCanProtocol.h"
#include "C_OSCCanMessageIdentificationIndices.h"
#include "C_OSCCanInterfaceId.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace Ui
{
class C_SdNdeCoDeviceWidget;
}

namespace stw_opensyde_gui
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_SdNdeCoDeviceWidget :
   public QWidget
{
   Q_OBJECT

public:
   explicit C_SdNdeCoDeviceWidget(QWidget * const opc_Parent = NULL);
   ~C_SdNdeCoDeviceWidget(void);

   void LoadUserSettings() const;
   void SaveUserSettings(void) const;
   void SetDeviceUseCase(const stw_types::uint32 ou32_ManagerNodeIndex, const stw_types::uint8 ou8_ManagerInterfaceId,
                         const stw_opensyde_core::C_OSCCanInterfaceId & orc_DeviceNodeId,
                         const stw_types::uint32 ou32_UseCaseIndex);
   void Refresh();

   //The signals keyword is necessary for Qt signal slot functionality
   //lint -save -e1736
Q_SIGNALS:
   //lint -restore
   void SigErrorChange(void) const;
   void SigSwitchToBusProtocol(const stw_types::uint32 ou32_Index, const QString & orc_BusName,
                               const stw_opensyde_core::C_OSCCanProtocol::E_Type oe_ProtocolType) const;
   void SigSwitchToBusProtocolMessage(const stw_types::uint32 ou32_Index, const QString & orc_BusName,
                                      const stw_opensyde_core::C_OSCCanMessageIdentificationIndices & orc_MessageId)
   const;

private:
   Ui::C_SdNdeCoDeviceWidget * mpc_Ui;
   static const stw_types::uint32 mhu32_INDEX_DEVICE_USE_CASE_EDS_FILE;
   static const stw_types::uint32 mhu32_INDEX_DEVICE_USE_CASE_CONFIGURATION;
   static const stw_types::uint32 mhu32_INDEX_DEVICE_USE_CASE_PDOS;

   //Avoid call
   C_SdNdeCoDeviceWidget(const C_SdNdeCoDeviceWidget &);
   C_SdNdeCoDeviceWidget & operator =(const C_SdNdeCoDeviceWidget &);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif
