//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Widget for showing a dashboard with a graphics view and scene with different dashboard items
   \copyright   Copyright 2020 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_SYVDADASHBOARDSCENEWIDGET_H
#define C_SYVDADASHBOARDSCENEWIDGET_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <QTimer>

#include "stwtypes.h"

#include "C_SyvDaDashboardContentBaseWidget.h"
#include "C_SyvDaDashboardScene.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace Ui
{
class C_SyvDaDashboardSceneWidget;
}

namespace stw_opensyde_gui
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_SyvDaDashboardSceneWidget :
   public C_SyvDaDashboardContentBaseWidget
{
   Q_OBJECT

public:
   explicit C_SyvDaDashboardSceneWidget(const stw_types::uint32 ou32_ViewIndex,
                                        const stw_types::uint32 ou32_DashboardIndex, QWidget * const opc_Parent = NULL);
   ~C_SyvDaDashboardSceneWidget(void);

   virtual void SetDashboardIndex(const stw_types::uint32 ou32_Value) override;

   virtual void SetEditMode(const bool oq_Active) override;
   virtual void SetDarkMode(const bool oq_Active) override;
   virtual void SetDrawingActive(const bool oq_Active) override;
   virtual void Save(void) override;

   virtual void RegisterWidgets(stw_opensyde_gui_logic::C_SyvComDriverDiag & orc_ComDriver) override;
   virtual void ConnectionActiveChanged(const bool oq_Active) override;
   virtual void UpdateShowValues(void) override;
   virtual void UpdateTransmissionConfiguration(void) override;
   virtual void HandleManualOperationFinished(const stw_types::sint32 os32_Result,
                                              const stw_types::uint8 ou8_NRC) override;

   virtual void SetFocus(void) const override;

protected:
   virtual void showEvent(QShowEvent * const opc_Event) override;

private:
   //Avoid call
   C_SyvDaDashboardSceneWidget(const C_SyvDaDashboardSceneWidget &);
   C_SyvDaDashboardSceneWidget & operator =(const C_SyvDaDashboardSceneWidget &); //lint !e1511 //we want to hide base

   Ui::C_SyvDaDashboardSceneWidget * mpc_Ui;
   const stw_types::uint32 mu32_ViewIndex;
   stw_types::uint32 mu32_DashboardIndex;
   QTimer mc_UpdateTimer;
   C_SyvDaDashboardScene * mpc_Scene;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif
