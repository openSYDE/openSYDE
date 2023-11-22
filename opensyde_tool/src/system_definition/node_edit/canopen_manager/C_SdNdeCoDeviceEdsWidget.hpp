//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Widget for handling the EDS file of a specific device on a specific interface of the CANopen Manager
   \copyright   Copyright 2022 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_SDNDECODEVICEEDSWIDGET_HPP
#define C_SDNDECODEVICEEDSWIDGET_HPP

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <QWidget>
#include <QFileInfo>

#include "stwtypes.hpp"
#include "C_OscCanInterfaceId.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace Ui
{
class C_SdNdeCoDeviceEdsWidget;
}

namespace stw
{
namespace opensyde_gui
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_SdNdeCoDeviceEdsWidget :
   public QWidget
{
   Q_OBJECT

public:
   explicit C_SdNdeCoDeviceEdsWidget(QWidget * const opc_Parent = NULL);
   ~C_SdNdeCoDeviceEdsWidget(void) override;

   void InitStaticNames(void) const;
   void SetNodeIndexAndInterfaceId(const uint32_t ou32_ManagerNodeIndex, const uint8_t ou8_ManagerInterfaceId,
                                   const stw::opensyde_core::C_OscCanInterfaceId & orc_DeviceNodeId);

private:
   Ui::C_SdNdeCoDeviceEdsWidget * mpc_Ui;
   uint32_t mu32_ManagerNodeIndex;
   uint8_t mu8_ManagerInterfaceId;
   stw::opensyde_core::C_OscCanInterfaceId mc_DeviceInterfaceId;

   //Avoid call
   C_SdNdeCoDeviceEdsWidget(const C_SdNdeCoDeviceEdsWidget &);
   C_SdNdeCoDeviceEdsWidget & operator =(const C_SdNdeCoDeviceEdsWidget &) &;

   void m_OnUpdateClicked(void);
   void m_LoadPicture(const QFileInfo oc_FileInfo);
   void m_SetDetails(void);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
