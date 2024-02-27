//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       SydeFlash variant of DLL widget
   \copyright   Copyright 2024 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_FLADLLWIDGET_HPP
#define C_FLADLLWIDGET_HPP

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "C_CamMosDllWidget.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_gui
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_FlaDllWidget :
   public C_CamMosDllWidget
{
public:
   C_FlaDllWidget(QWidget * const opc_Parent = NULL);
   void LoadUserSettings(void) const override;

protected:
   void m_LoadConfig(void) const override;
   QString m_GetCurrentDllPath(void) const override;
   void m_SaveDllPath(const QString & orc_DllPath) const override;
   void m_SaveDllType(const E_CanDllType oe_DllType) const override;
   QString m_ResolvePlaceholderVariables(const QString & orc_Path) const override;
   stw::opensyde_gui_elements::C_OgeLeFilePathBase * m_GetPlaceholderLineEdit(void) override;
   stw::opensyde_gui_elements::C_CamOgePubPathVariablesBase * m_GetPathVariablesButton(void) override;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
