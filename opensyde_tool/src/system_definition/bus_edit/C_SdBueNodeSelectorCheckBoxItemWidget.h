//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       A concrete node selector widget for C_SdBueNodeSelectorCheckBoxListWidget (header)

   See cpp file for detailed description

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_SDBUENODESELECTORCHECKBOXITEMWIDGET_H
#define C_SDBUENODESELECTORCHECKBOXITEMWIDGET_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */

#include <QIcon>

#include "C_OgeWiWithToolTip.h"
#include "C_OgeChxDefaultSmall.h"
#include "C_OSCCanProtocol.h"

#include "stwtypes.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */

namespace Ui
{
class C_SdBueNodeSelectorCheckBoxItemWidget;
}

namespace stw_opensyde_gui
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_SdBueNodeSelectorCheckBoxItemWidget :
   public stw_opensyde_gui_elements::C_OgeWiWithToolTip
{
   Q_OBJECT

public:
   explicit C_SdBueNodeSelectorCheckBoxItemWidget(const stw_types::uint32 ou32_NodeIndex,
                                                  const stw_types::uint32 ou32_InterfaceIndex, const QString & orc_Name,
                                                  QWidget * const opc_Parent = NULL);
   ~C_SdBueNodeSelectorCheckBoxItemWidget();

   void GetIndexes(stw_types::uint32 & oru32_Index, stw_types::uint32 & oru32_SubIndex) const;
   void SetChecked(const bool oq_Checked);
   bool IsChecked(void) const;
   void SetProtocol(const stw_opensyde_core::C_OSCCanProtocol::E_Type oe_Protocol);
   void SetNodeAsManager(const bool oq_Manager);
   void SetNodeAsDevice(const bool oq_Device, const stw_opensyde_core::C_OSCCanInterfaceId * const opc_DeviceId,
                        const stw_types::uint32 ou32_ManagerNodeIndex = 0U,
                        const stw_types::uint32 ou32_ManagerIntfIndex = 0U);
   void UpdateToolTip(void);

   //The signals keyword is necessary for Qt signal slot functionality
   //lint -save -e1736

Q_SIGNALS:
   //lint -restore
   void SigNodeToggled(const stw_types::uint32 ou32_NodeIndex, const stw_types::uint32 ou32_InterfaceIndex,
                       const bool oq_Checked);
   void SigSwitchToCoManager(const stw_types::uint32 ou32_NodeIndex, const QString & orc_NodeName,
                             const stw_types::uint8 ou8_InterfaceNumber) const;

private:
   //Avoid call
   C_SdBueNodeSelectorCheckBoxItemWidget(const C_SdBueNodeSelectorCheckBoxItemWidget &);
   C_SdBueNodeSelectorCheckBoxItemWidget & operator =(const C_SdBueNodeSelectorCheckBoxItemWidget &);

   void m_NodeToggled(const stw_types::uint32 ou32_NodeIndex, const stw_types::uint32 ou32_InterfaceIndex,
                      const bool oq_Checked);
   void m_AdaptIcon(const bool oq_Checked);
   void m_OnLinkSwitchToManager(const QString & orc_Link) const;

   Ui::C_SdBueNodeSelectorCheckBoxItemWidget * mpc_Ui;

   stw_opensyde_gui_elements::C_OgeChxDefaultSmall * mpc_CheckBox;

   QIcon mc_IconActive;
   QIcon mc_IconInactive;
   stw_opensyde_core::C_OSCCanProtocol::E_Type me_ProtocolType;

   // CANopen specific
   bool mq_IsManager;
   bool mq_IsDevice;
   // Only relevant if mq_IsDevice == true
   stw_opensyde_core::C_OSCCanInterfaceId mc_DeviceId;
   stw_types::uint32 mu32_ManagerNodeIndex;
   stw_types::uint32 mu32_ManagerIntfIndex;
};
}
#endif
