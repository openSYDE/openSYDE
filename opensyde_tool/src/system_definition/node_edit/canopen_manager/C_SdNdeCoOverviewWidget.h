//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Overview widget with current CANopen configuration of Manager and devices on the used interfaces
   \copyright   Copyright 2022 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_SDNDECOOVERVIEWWIDGET_H
#define C_SDNDECOOVERVIEWWIDGET_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <QWidget>
#include "stwtypes.h"
#include "C_OSCCanInterfaceId.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace Ui
{
class C_SdNdeCoOverviewWidget;
}

namespace stw_opensyde_gui
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_SdNdeCoOverviewWidget :
   public QWidget
{
   Q_OBJECT

public:
   explicit C_SdNdeCoOverviewWidget(QWidget * const opc_Parent = NULL);
   ~C_SdNdeCoOverviewWidget(void);

   void LoadUserSettings(void) const;
   void SaveUserSettings(void) const;
   void InitStaticNames(void) const;
   void UpdateData(void);
   void SetNodeIndex(const stw_types::uint32 ou32_NodeIndex);

   //The signals keyword is necessary for Qt signal slot functionality
   //lint -save -e1736

Q_SIGNALS:
   //lint -restore
   void SigInterfaceSelected(const stw_types::uint8 ou8_InterfaceNumber);
   void SigDeviceSelected(const stw_types::uint8 ou8_InterfaceNumber,
                          const stw_opensyde_core::C_OSCCanInterfaceId & orc_CanInterfaceId,
                          const stw_types::uint32 ou32_UseCaseIndex);

private:
   Ui::C_SdNdeCoOverviewWidget * mpc_Ui;

   //Avoid call
   C_SdNdeCoOverviewWidget(const C_SdNdeCoOverviewWidget &);
   C_SdNdeCoOverviewWidget & operator =(const C_SdNdeCoOverviewWidget &);

   void m_UpdateUi(void);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif
