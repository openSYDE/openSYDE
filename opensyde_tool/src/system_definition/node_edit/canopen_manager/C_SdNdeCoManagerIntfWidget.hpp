//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Widget with current CANopen settings of Manager for a specific interface
   \copyright   Copyright 2022 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_SDNDECOMANAGERINTFWIDGET_H
#define C_SDNDECOMANAGERINTFWIDGET_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <QWidget>

#include "stwtypes.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace Ui
{
class C_SdNdeCoManagerIntfWidget;
}

namespace stw
{
namespace opensyde_gui
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_SdNdeCoManagerIntfWidget :
   public QWidget
{
   Q_OBJECT

public:
   explicit C_SdNdeCoManagerIntfWidget(QWidget * const opc_Parent = NULL);
   ~C_SdNdeCoManagerIntfWidget(void) override;

   void InitStaticNames(void) const;
   void SetNodeIndexAndInterfaceId(const uint32_t ou32_NodeIndex, const uint8_t ou8_InterfaceId);
   void SaveToData(void) const;
   void Refresh();

   //The signals keyword is necessary for Qt signal slot functionality
   //lint -save -e1736

Q_SIGNALS:
   //lint -restore
   void SigErrorChange(void) const;
   void SigCommDatapoolsChanged(void) const;

private:
   Ui::C_SdNdeCoManagerIntfWidget * mpc_Ui;
   uint32_t mu32_NodeIndex;
   uint8_t mu8_InterfaceId;

   //Avoid call
   C_SdNdeCoManagerIntfWidget(const C_SdNdeCoManagerIntfWidget &);
   C_SdNdeCoManagerIntfWidget & operator =(const C_SdNdeCoManagerIntfWidget &) &;

   void m_OnStartDevicesChanged(void) const;
   void m_OnHeartbeatEnableChanged(void) const;
   void m_HandleHeartbeatEnableState(void) const;
   void m_OnPdoSyncEnableChanged(void) const;
   void m_HandlePdoSyncEnableState(void) const;
   void m_OnPdoSyncTimeValueChanged(void) const;
   void m_OnSameAsOpensydeNodeIdChanged(void) const;
   void m_HandleSameAsOpensydeNodeIdState(void) const;
   void m_OnCoNodeIdChanged(void) const;
   void m_CheckCoNodeId(void) const;
   void m_OnHeartbeatProducerTimeChanged(void) const;
   void m_CheckHeartbeatProducerTime(void) const;

   void m_LoadFromData(void);
   void m_SaveChanges(void) const;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
