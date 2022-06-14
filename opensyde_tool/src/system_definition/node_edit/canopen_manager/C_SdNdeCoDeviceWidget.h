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

   void SetDeviceUseCase(const stw_types::uint32 ou32_NodeIndex, const stw_types::uint8 ou8_InterfaceId,
                         const stw_opensyde_core::C_OSCCanInterfaceId & orc_NodeId,
                         const stw_types::uint32 ou32_UseCaseIndex);

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
