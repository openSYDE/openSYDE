//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Widget for handling the configuration of a specific device on a specific interface of the CANopen Manager
   \copyright   Copyright 2022 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_SDNDECODEVICECONFIGWIDGET_H
#define C_SDNDECODEVICECONFIGWIDGET_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <QWidget>

#include "stwtypes.h"
#include "C_OSCCanInterfaceId.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace Ui
{
class C_SdNdeCoDeviceConfigWidget;
}

namespace stw_opensyde_gui
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_SdNdeCoDeviceConfigWidget :
   public QWidget
{
   Q_OBJECT

public:
   explicit C_SdNdeCoDeviceConfigWidget(QWidget * const opc_Parent = NULL);
   ~C_SdNdeCoDeviceConfigWidget(void);

   void InitStaticNames(void) const;
   void SetNodeIndexAndInterfaceId(const stw_types::uint32 ou32_NodeIndex, const stw_types::uint8 ou8_InterfaceId,
                                   const stw_opensyde_core::C_OSCCanInterfaceId & orc_NodeId);
   void SaveToData(void);

private:
   Ui::C_SdNdeCoDeviceConfigWidget * mpc_Ui;
   stw_types::uint32 mu32_NodeIndex;
   stw_types::uint8 mu8_InterfaceId;
   stw_opensyde_core::C_OSCCanInterfaceId mc_CanInterfaceId;
   std::map<stw_types::sintn, stw_types::uint8> mc_ComboboxIndexToFactorySettingsSub;

   //Avoid call
   C_SdNdeCoDeviceConfigWidget(const C_SdNdeCoDeviceConfigWidget &);
   C_SdNdeCoDeviceConfigWidget & operator =(const C_SdNdeCoDeviceConfigWidget &);

   void m_OnHeartbeatProducingEnableChanged(void) const;
   void m_OnHeartbeatConsumingEnableChanged(void) const;
   void m_OnFactorySetingsChanged(void) const;
   void m_OnSameAsOpensydeNodeIdChanged(void) const;
   void m_OnConsumerTimeAutoChanged(void) const;
   void m_LoadFromData(void);
   void m_SaveChanges(void);

   bool m_GetIndexOfSub(const stw_types::uint8 ou8_Sub, stw_types::sintn & orsn_FoundIndex);
   bool m_GetSubOfIndex(const stw_types::sintn osn_Index, stw_types::uint8 & oru8_FoundSub);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif
