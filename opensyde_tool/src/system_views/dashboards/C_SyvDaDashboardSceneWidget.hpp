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

#include "stwtypes.hpp"

#include "C_SyvDaDashboardContentBaseWidget.hpp"
#include "C_SyvDaDashboardScene.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace Ui
{
class C_SyvDaDashboardSceneWidget;
}

namespace stw
{
namespace opensyde_gui
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_SyvDaDashboardSceneWidget :
   public C_SyvDaDashboardContentBaseWidget
{
   Q_OBJECT

public:
   explicit C_SyvDaDashboardSceneWidget(const uint32_t ou32_ViewIndex, const uint32_t ou32_DashboardIndex,
                                        QWidget * const opc_Parent = NULL);
   ~C_SyvDaDashboardSceneWidget(void) override;

   void SetDashboardIndex(const uint32_t ou32_Value) override;

   void SetEditMode(const bool oq_Active) override;
   void SetDarkMode(const bool oq_Active) override;
   void SetDrawingActive(const bool oq_Active) override;
   void Save(void) override;

   void RegisterWidgets(stw::opensyde_gui_logic::C_SyvComDriverDiag & orc_ComDriver) override;
   void ConnectionActiveChanged(const bool oq_Active) override;
   void UpdateShowValues(void) override;
   void UpdateTransmissionConfiguration(void) override;
   void HandleManualOperationFinished(const int32_t os32_Result, const uint8_t ou8_Nrc) override;

   void SetFocus(void) const override;

protected:
   void showEvent(QShowEvent * const opc_Event) override;

private:
   //Avoid call
   C_SyvDaDashboardSceneWidget(const C_SyvDaDashboardSceneWidget &);
   C_SyvDaDashboardSceneWidget & operator =(const C_SyvDaDashboardSceneWidget &) &; //lint !e1511 //we want to hide base

   Ui::C_SyvDaDashboardSceneWidget * mpc_Ui;
   const uint32_t mu32_ViewIndex;
   uint32_t mu32_DashboardIndex;
   QTimer mc_UpdateTimer;
   C_SyvDaDashboardScene * mpc_Scene;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
