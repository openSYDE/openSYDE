//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Widget for displaying and editing CANopen Manager
   \copyright   Copyright 2022 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_SDNDECOWIDGET_H
#define C_SDNDECOWIDGET_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <QWidget>

#include "stwtypes.h"
#include "C_OSCCanInterfaceId.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace Ui
{
class C_SdNdeCoWidget;
}

namespace stw_opensyde_gui
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_SdNdeCoWidget :
   public QWidget
{
   Q_OBJECT

public:
   explicit C_SdNdeCoWidget(QWidget * const opc_Parent = NULL);
   ~C_SdNdeCoWidget(void);

   void LoadUserSettings(void) const;
   void SaveUserSettings(void) const;
   void InitStaticNames(void) const;
   void SetNode(const stw_types::uint32 ou32_NodeIndex);

   //The signals keyword is necessary for Qt signal slot functionality
   //lint -save -e1736

Q_SIGNALS:
   //lint -restore
   void SigErrorChange(void);
   void SigCommDatapoolsChanged(void);

protected:
   virtual void showEvent(QShowEvent * const opc_Event) override;

private:
   Ui::C_SdNdeCoWidget * mpc_Ui;
   stw_types::uint32 mu32_NodeIndex;

   //Avoid call
   C_SdNdeCoWidget(const C_SdNdeCoWidget &);
   C_SdNdeCoWidget & operator =(const C_SdNdeCoWidget &);

   void m_OnOverviewClicked(void) const;
   void m_OnInterfaceSelected(const stw_types::uint8 ou8_InterfaceNumber);
   void m_OnDeviceSelected(const stw_types::uint8 ou8_InterfaceNumber,
                           const stw_opensyde_core::C_OSCCanInterfaceId & orc_NodeId,
                           const stw_types::uint32 ou32_UseCaseIndex);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif
