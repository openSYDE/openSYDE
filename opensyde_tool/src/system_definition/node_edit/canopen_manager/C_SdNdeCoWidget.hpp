//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Widget for displaying and editing CANopen Manager
   \copyright   Copyright 2022 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_SDNDECOWIDGET_HPP
#define C_SDNDECOWIDGET_HPP

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <QWidget>

#include "stwtypes.hpp"
#include "C_OscCanProtocol.hpp"
#include "C_OscCanMessageIdentificationIndices.hpp"
#include "C_OscCanInterfaceId.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace Ui
{
class C_SdNdeCoWidget;
}

namespace stw
{
namespace opensyde_gui
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_SdNdeCoWidget :
   public QWidget
{
   Q_OBJECT

public:
   explicit C_SdNdeCoWidget(QWidget * const opc_Parent = NULL);
   ~C_SdNdeCoWidget(void) override;

   void LoadUserSettings(void) const;
   void SaveUserSettings(void) const;
   void InitStaticNames(void) const;
   void SetNode(const uint32_t ou32_NodeIndex);
   void Refresh();
   void OpenManagerConfiguration(const uint8_t ou8_InterfaceNumber);
   void OpenDeviceConfiguration(const uint32_t ou32_DeviceNodeIndex);

   //The signals keyword is necessary for Qt signal slot functionality
   //lint -save -e1736

Q_SIGNALS:
   //lint -restore
   void SigErrorChange(void);
   void SigCommDatapoolsChanged(void);
   void SigSwitchToDeviceNodeInCoManager(const uint32_t ou32_ManagerNodeIndex, const QString & orc_ManagerNodeName,
                                         const uint32_t ou32_DeviceNodeIndex) const;
   void SigSwitchToBusProtocol(const uint32_t ou32_Index, const QString & orc_BusName,
                               const stw::opensyde_core::C_OscCanProtocol::E_Type oe_ProtocolType) const;
   void SigSwitchToBusProtocolMessage(const uint32_t ou32_Index, const QString & orc_BusName,
                                      const stw::opensyde_core::C_OscCanMessageIdentificationIndices & orc_MessageId)
   const;

protected:
   void showEvent(QShowEvent * const opc_Event) override;

private:
   Ui::C_SdNdeCoWidget * mpc_Ui;
   uint32_t mu32_NodeIndex;

   //Avoid call
   C_SdNdeCoWidget(const C_SdNdeCoWidget &);
   C_SdNdeCoWidget & operator =(const C_SdNdeCoWidget &) &;

   void m_OnOverviewClicked(void) const;
   void m_OnInterfaceSelected(const uint8_t ou8_InterfaceNumber);
   void m_OnDeviceSelected(const uint8_t ou8_ManagerInterfaceNumber,
                           const stw::opensyde_core::C_OscCanInterfaceId & orc_DeviceNodeId,
                           const uint32_t ou32_UseCaseIndex);
   void m_OnLinkSwitchToManager(const QString & orc_Link) const;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
