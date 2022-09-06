//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Widget for showing the PDO configuration of a specific device on a specific interface of the CANopen Manager

   Widget for showing the PDO configuration of a specific device on a specific interface of the CANopen Manager

   \copyright   Copyright 2022 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_SDNDECOPDOWIDGET_H
#define C_SDNDECOPDOWIDGET_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <QWidget>
#include "stwtypes.h"
#include "C_OSCCanProtocol.h"
#include "C_OSCCanInterfaceId.h"
#include "C_OSCCanMessageIdentificationIndices.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace Ui
{
class C_SdNdeCoPdoWidget;
}

namespace stw_opensyde_gui
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_SdNdeCoPdoWidget :
   public QWidget
{
   Q_OBJECT

public:
   explicit C_SdNdeCoPdoWidget(QWidget * const opc_Parent = NULL);
   ~C_SdNdeCoPdoWidget(void);

   void LoadUserSettings(void) const;
   void SaveUserSettings(void) const;
   void InitStaticNames(void) const;
   void UpdateData(void);
   void SetNodeIndexAndInterfaceId(const stw_types::uint32 ou32_ManagerNodeIndex,
                                   const stw_types::uint8 ou8_ManagerInterfaceId,
                                   const stw_opensyde_core::C_OSCCanInterfaceId & orc_DeviceNodeId);

   //The signals keyword is necessary for Qt signal slot functionality
   //lint -save -e1736
Q_SIGNALS:
   //lint -restore
   void SigSwitchToBusProtocol(const stw_types::uint32 ou32_Index, const QString & orc_BusName,
                               const stw_opensyde_core::C_OSCCanProtocol::E_Type oe_ProtocolType) const;
   void SigSwitchToBusProtocolMessage(const stw_types::uint32 ou32_Index, const QString & orc_BusName,
                                      const stw_opensyde_core::C_OSCCanMessageIdentificationIndices & orc_MessageId)
   const;

private:
   Ui::C_SdNdeCoPdoWidget * mpc_Ui;
   stw_types::uint32 mu32_ManagerNodeIndex;
   stw_types::uint8 mu8_ManagerInterfaceId;
   stw_opensyde_core::C_OSCCanInterfaceId mc_DeviceInterfaceId;

   //Avoid call
   C_SdNdeCoPdoWidget(const C_SdNdeCoPdoWidget &);
   C_SdNdeCoPdoWidget & operator =(const C_SdNdeCoPdoWidget &);

   void m_UpdateUi(void);
   void m_OnLinkSwitchToBus(const QString & orc_Link) const;
   void m_OnLinkSwitchToBusProtocolMessage(
      const stw_opensyde_core::C_OSCCanMessageIdentificationIndices & orc_MessageId) const;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif
