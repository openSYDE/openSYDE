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

#include "stwtypes.hpp"
#include "C_OscCanInterfaceId.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace Ui
{
class C_SdNdeCoDeviceConfigWidget;
}

namespace stw
{
namespace opensyde_gui
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_SdNdeCoDeviceConfigWidget :
   public QWidget
{
   Q_OBJECT

public:
   explicit C_SdNdeCoDeviceConfigWidget(QWidget * const opc_Parent = NULL);
   ~C_SdNdeCoDeviceConfigWidget(void) override;

   void InitStaticNames(void) const;
   void SetNodeIndexAndInterfaceId(const uint32_t ou32_ManagerNodeIndex, const uint8_t ou8_ManagerInterfaceId,
                                   const stw::opensyde_core::C_OscCanInterfaceId & orc_DeviceNodeId);
   void SaveToData(void);
   void Refresh(void);

   //The signals keyword is necessary for Qt signal slot functionality
   //lint -save -e1736

Q_SIGNALS:
   //lint -restore
   void SigErrorChange(void) const;

private:
   Ui::C_SdNdeCoDeviceConfigWidget * mpc_Ui;
   uint32_t mu32_ManagerNodeIndex;
   uint8_t mu8_ManagerInterfaceId;
   stw::opensyde_core::C_OscCanInterfaceId mc_DeviceInterfaceId;
   std::map<int32_t, uint8_t> mc_ComboboxIndexToFactorySettingsSub;

   //Avoid call
   C_SdNdeCoDeviceConfigWidget(const C_SdNdeCoDeviceConfigWidget &);
   C_SdNdeCoDeviceConfigWidget & operator =(const C_SdNdeCoDeviceConfigWidget &) &;

   void m_OnHeartbeatProducingEnableChanged(void) const;
   void m_OnHeartbeatConsumingEnableChanged(void);
   void m_HandleHeartbeatConsumingEnableState(void) const;
   void m_OnFactorySetingsChanged(void) const;
   void m_OnSameAsOpensydeNodeIdChanged(void);
   void m_HandleSameAsOpensydeNodeIdState(void) const;
   void m_OnCoNodeIdChanged(void);
   void m_CheckCoNodeId(void) const;
   void m_OnConsumerTimeAutoChanged(void);
   void m_HandleConsumerTimeAutoState(void);
   void m_OnHeartbeatConsumerTimeChanged(void);
   void m_CheckHeartbeatConsumerTime(void) const;

   void m_LoadFromData(void);
   void m_SaveChanges(void);

   bool m_GetIndexOfSub(const uint8_t ou8_Sub, int32_t & ors32_FoundIndex);
   bool m_GetSubOfIndex(const int32_t os32_Index, uint8_t & oru8_FoundSub);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
