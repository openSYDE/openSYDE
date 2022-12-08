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
#include "stwtypes.hpp"
#include "C_OscCanInterfaceId.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace Ui
{
class C_SdNdeCoOverviewWidget;
}

namespace stw
{
namespace opensyde_gui
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_SdNdeCoOverviewWidget :
   public QWidget
{
   Q_OBJECT

public:
   explicit C_SdNdeCoOverviewWidget(QWidget * const opc_Parent = NULL);
   ~C_SdNdeCoOverviewWidget(void) override;

   void LoadUserSettings(void) const;
   void SaveUserSettings(void) const;
   void InitStaticNames(void) const;
   void UpdateData(void);
   void SetNodeIndex(const uint32_t ou32_NodeIndex);

   //The signals keyword is necessary for Qt signal slot functionality
   //lint -save -e1736

Q_SIGNALS:
   //lint -restore
   void SigInterfaceSelected(const uint8_t ou8_InterfaceNumber);
   void SigDeviceSelected(const uint8_t ou8_InterfaceNumber,
                          const stw::opensyde_core::C_OscCanInterfaceId & orc_CanInterfaceId,
                          const uint32_t ou32_UseCaseIndex);

private:
   Ui::C_SdNdeCoOverviewWidget * mpc_Ui;

   //Avoid call
   C_SdNdeCoOverviewWidget(const C_SdNdeCoOverviewWidget &);
   C_SdNdeCoOverviewWidget & operator =(const C_SdNdeCoOverviewWidget &) &;

   void m_UpdateUi(void);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
