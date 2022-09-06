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

#include "stwtypes.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace Ui
{
class C_SdNdeCoManagerIntfWidget;
}

namespace stw_opensyde_gui
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_SdNdeCoManagerIntfWidget :
   public QWidget
{
   Q_OBJECT

public:
   explicit C_SdNdeCoManagerIntfWidget(QWidget * const opc_Parent = NULL);
   ~C_SdNdeCoManagerIntfWidget(void);

   void InitStaticNames(void) const;
   void SetNodeIndexAndInterfaceId(const stw_types::uint32 ou32_NodeIndex, const stw_types::uint8 ou8_InterfaceId);
   void SaveToData(void) const;
   void Refresh();

   //The signals keyword is necessary for Qt signal slot functionality
   //lint -save -e1736

Q_SIGNALS:
   //lint -restore
   void SigErrorChange(void) const;

private:
   Ui::C_SdNdeCoManagerIntfWidget * mpc_Ui;
   stw_types::uint32 mu32_NodeIndex;
   stw_types::uint8 mu8_InterfaceId;

   //Avoid call
   C_SdNdeCoManagerIntfWidget(const C_SdNdeCoManagerIntfWidget &);
   C_SdNdeCoManagerIntfWidget & operator =(const C_SdNdeCoManagerIntfWidget &);

   void m_OnStartDevicesChanged(void) const;
   void m_OnHeartbeatEnableChanged(void) const;
   void m_HandleHeartbeatEnableState(void) const;
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
} //end of namespace

#endif
