//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Widget for handling the EDS file of a specific device on a specific interface of the CANopen Manager
   \copyright   Copyright 2022 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_SDNDECODEVICEEDSWIDGET_H
#define C_SDNDECODEVICEEDSWIDGET_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <QWidget>
#include <QFileInfo>

#include "stwtypes.h"
#include "C_OSCCanInterfaceId.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace Ui
{
class C_SdNdeCoDeviceEdsWidget;
}

namespace stw_opensyde_gui
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_SdNdeCoDeviceEdsWidget :
   public QWidget
{
   Q_OBJECT

public:
   explicit C_SdNdeCoDeviceEdsWidget(QWidget * const opc_Parent = NULL);
   ~C_SdNdeCoDeviceEdsWidget(void);

   void InitStaticNames(void) const;
   void SetNodeIndexAndInterfaceId(const stw_types::uint32 ou32_ManagerNodeIndex,
                                   const stw_types::uint8 ou8_ManagerInterfaceId,
                                   const stw_opensyde_core::C_OSCCanInterfaceId & orc_DeviceNodeId);

private:
   Ui::C_SdNdeCoDeviceEdsWidget * mpc_Ui;
   stw_types::uint32 mu32_ManagerNodeIndex;
   stw_types::uint8 mu8_ManagerInterfaceId;
   stw_opensyde_core::C_OSCCanInterfaceId mc_DeviceInterfaceId;

   //Avoid call
   C_SdNdeCoDeviceEdsWidget(const C_SdNdeCoDeviceEdsWidget &);
   C_SdNdeCoDeviceEdsWidget & operator =(const C_SdNdeCoDeviceEdsWidget &);

   void m_OnUpdateClicked(void) const;
   void m_LoadPicture(const QFileInfo oc_FileInfo);
   void m_SetDetails(void);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif
