//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Widget for showing the PDO configuration of a specific device on a specific interface of the CANopen Manager

   Widget for showing the PDO configuration of a specific device on a specific interface of the CANopen Manager

   \copyright   Copyright 2022 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_SDNDECOPDOWIDGET_HPP
#define C_SDNDECOPDOWIDGET_HPP

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <QWidget>
#include "stwtypes.hpp"
#include "C_OscCanProtocol.hpp"
#include "C_OscCanInterfaceId.hpp"
#include "C_OscCanMessageIdentificationIndices.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace Ui
{
class C_SdNdeCoPdoWidget;
}

namespace stw
{
namespace opensyde_gui
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_SdNdeCoPdoWidget :
   public QWidget
{
   Q_OBJECT

public:
   explicit C_SdNdeCoPdoWidget(QWidget * const opc_Parent = NULL);
   ~C_SdNdeCoPdoWidget(void) override;

   void LoadUserSettings(void) const;
   void SaveUserSettings(void) const;
   void InitStaticNames(void) const;
   void UpdateData(void);
   void SetNodeIndexAndInterfaceId(const uint32_t ou32_ManagerNodeIndex, const uint8_t ou8_ManagerInterfaceId,
                                   const stw::opensyde_core::C_OscCanInterfaceId & orc_DeviceNodeId);

   //The signals keyword is necessary for Qt signal slot functionality
   //lint -save -e1736
Q_SIGNALS:
   //lint -restore
   void SigSwitchToBusProtocol(const uint32_t ou32_Index, const QString & orc_BusName,
                               const stw::opensyde_core::C_OscCanProtocol::E_Type oe_ProtocolType) const;
   void SigSwitchToBusProtocolMessage(const uint32_t ou32_Index, const QString & orc_BusName,
                                      const stw::opensyde_core::C_OscCanMessageIdentificationIndices & orc_MessageId)
   const;

private:
   Ui::C_SdNdeCoPdoWidget * mpc_Ui;
   uint32_t mu32_ManagerNodeIndex;
   uint8_t mu8_ManagerInterfaceId;
   stw::opensyde_core::C_OscCanInterfaceId mc_DeviceInterfaceId;

   //Avoid call
   C_SdNdeCoPdoWidget(const C_SdNdeCoPdoWidget &);
   C_SdNdeCoPdoWidget & operator =(const C_SdNdeCoPdoWidget &) &;

   void m_UpdateUi(void);
   void m_OnLinkSwitchToBus(const QString & orc_Link) const;
   void m_OnLinkSwitchToBusProtocolMessage(
      const stw::opensyde_core::C_OscCanMessageIdentificationIndices & orc_MessageId) const;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
