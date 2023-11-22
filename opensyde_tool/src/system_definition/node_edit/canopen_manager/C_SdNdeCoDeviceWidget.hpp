//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Widget with current CANopen settings of Manager for a specific device on one interface
   \copyright   Copyright 2022 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_SDNDECODEVICEWIDGET_HPP
#define C_SDNDECODEVICEWIDGET_HPP

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <QWidget>

#include "stwtypes.hpp"
#include "C_OscCanProtocol.hpp"
#include "C_OscCanMessageIdentificationIndices.hpp"
#include "C_OscCanInterfaceId.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace Ui
{
class C_SdNdeCoDeviceWidget;
}

namespace stw
{
namespace opensyde_gui
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_SdNdeCoDeviceWidget :
   public QWidget
{
   Q_OBJECT

public:
   explicit C_SdNdeCoDeviceWidget(QWidget * const opc_Parent = NULL);
   ~C_SdNdeCoDeviceWidget(void) override;

   void LoadUserSettings() const;
   void SaveUserSettings(void) const;
   void SetDeviceUseCase(const uint32_t ou32_ManagerNodeIndex, const uint8_t ou8_ManagerInterfaceId,
                         const stw::opensyde_core::C_OscCanInterfaceId & orc_DeviceNodeId,
                         const uint32_t ou32_UseCaseIndex);
   void Refresh();

   //The signals keyword is necessary for Qt signal slot functionality
   //lint -save -e1736
Q_SIGNALS:
   //lint -restore
   void SigErrorChange(void) const;
   void SigSwitchToBusProtocol(const uint32_t ou32_Index, const QString & orc_BusName,
                               const stw::opensyde_core::C_OscCanProtocol::E_Type oe_ProtocolType) const;
   void SigSwitchToBusProtocolMessage(const uint32_t ou32_Index, const QString & orc_BusName,
                                      const stw::opensyde_core::C_OscCanMessageIdentificationIndices & orc_MessageId)
   const;

private:
   Ui::C_SdNdeCoDeviceWidget * mpc_Ui;
   static const uint32_t mhu32_INDEX_DEVICE_USE_CASE_EDS_FILE;
   static const uint32_t mhu32_INDEX_DEVICE_USE_CASE_CONFIGURATION;
   static const uint32_t mhu32_INDEX_DEVICE_USE_CASE_PDOS;

   //Avoid call
   C_SdNdeCoDeviceWidget(const C_SdNdeCoDeviceWidget &);
   C_SdNdeCoDeviceWidget & operator =(const C_SdNdeCoDeviceWidget &) &;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
