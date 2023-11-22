//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       A concrete node selector widget for C_SdBueNodeSelectorCheckBoxListWidget (header)

   See cpp file for detailed description

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_SDBUENODESELECTORCHECKBOXITEMWIDGET_HPP
#define C_SDBUENODESELECTORCHECKBOXITEMWIDGET_HPP

/* -- Includes ------------------------------------------------------------------------------------------------------ */

#include <QIcon>

#include "C_OgeWiWithToolTip.hpp"
#include "C_OgeChxDefaultSmall.hpp"
#include "C_OscCanProtocol.hpp"

#include "stwtypes.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */

namespace Ui
{
class C_SdBueNodeSelectorCheckBoxItemWidget;
}

namespace stw
{
namespace opensyde_gui
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_SdBueNodeSelectorCheckBoxItemWidget :
   public stw::opensyde_gui_elements::C_OgeWiWithToolTip
{
   Q_OBJECT

public:
   explicit C_SdBueNodeSelectorCheckBoxItemWidget(const uint32_t ou32_NodeIndex, const uint32_t ou32_InterfaceIndex,
                                                  const QString & orc_Name, QWidget * const opc_Parent = NULL);
   ~C_SdBueNodeSelectorCheckBoxItemWidget() override;

   void GetIndexes(uint32_t & oru32_Index, uint32_t & oru32_SubIndex) const;
   void SetChecked(const bool oq_Checked);
   bool IsChecked(void) const;
   void SetProtocol(const stw::opensyde_core::C_OscCanProtocol::E_Type oe_Protocol);
   void SetNodeAsManager(const bool oq_Manager);
   void SetNodeAsDevice(const bool oq_Device, const stw::opensyde_core::C_OscCanInterfaceId * const opc_DeviceId,
                        const uint32_t ou32_ManagerNodeIndex = 0U, const uint32_t ou32_ManagerIntfIndex = 0U);
   void UpdateToolTip(void);

   //The signals keyword is necessary for Qt signal slot functionality
   //lint -save -e1736

Q_SIGNALS:
   //lint -restore
   void SigNodeToggled(const uint32_t ou32_NodeIndex, const uint32_t ou32_InterfaceIndex, const bool oq_Checked);
   void SigSwitchToCoManager(const uint32_t ou32_NodeIndex, const QString & orc_NodeName,
                             const uint8_t ou8_InterfaceNumber) const;

private:
   //Avoid call
   C_SdBueNodeSelectorCheckBoxItemWidget(const C_SdBueNodeSelectorCheckBoxItemWidget &);
   C_SdBueNodeSelectorCheckBoxItemWidget & operator =(const C_SdBueNodeSelectorCheckBoxItemWidget &) &;

   void m_NodeToggled(const uint32_t ou32_NodeIndex, const uint32_t ou32_InterfaceIndex, const bool oq_Checked);
   void m_AdaptIcon(const bool oq_Checked);
   void m_OnLinkSwitchToManager(const QString & orc_Link) const;

   Ui::C_SdBueNodeSelectorCheckBoxItemWidget * mpc_Ui;

   stw::opensyde_gui_elements::C_OgeChxDefaultSmall * mpc_CheckBox;

   QIcon mc_IconActive;
   QIcon mc_IconInactive;
   stw::opensyde_core::C_OscCanProtocol::E_Type me_ProtocolType;

   // CANopen specific
   bool mq_IsManager;
   bool mq_IsDevice;
   // Only relevant if mq_IsDevice == true
   stw::opensyde_core::C_OscCanInterfaceId mc_DeviceId;
   uint32_t mu32_ManagerNodeIndex;
   uint32_t mu32_ManagerIntfIndex;
};
}
}
#endif
