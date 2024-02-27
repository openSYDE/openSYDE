//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       SydeFlash variant of DLL widget

   SydeFlash variant of DLL widget

   \copyright   Copyright 2024 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.hpp"

#include "stwtypes.hpp"
#include "C_FlaUti.hpp"
#include "C_UsHandler.hpp"
#include "C_FlaDllWidget.hpp"
#include "C_FlaOgeLeFilePath.hpp"
#include "C_FlaOgePubPathVariables.hpp"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw::opensyde_gui;
using namespace stw::opensyde_gui_logic;
using namespace stw::opensyde_gui_elements;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

/* -- Global Variables ---------------------------------------------------------------------------------------------- */

/* -- Module Global Variables --------------------------------------------------------------------------------------- */

/* -- Module Global Function Prototypes ----------------------------------------------------------------------------- */

/* -- Implementation ------------------------------------------------------------------------------------------------ */

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default constructor

   Set up GUI with all elements.

   \param[in,out]  opc_Parent    Optional pointer to parent
*/
//----------------------------------------------------------------------------------------------------------------------
C_FlaDllWidget::C_FlaDllWidget(QWidget * const opc_Parent) :
   C_CamMosDllWidget(opc_Parent)
{
   m_InitUi();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Load user settings
*/
//----------------------------------------------------------------------------------------------------------------------
void C_FlaDllWidget::LoadUserSettings() const
{
   this->m_LoadConfig();
   C_CamMosDllWidget::LoadUserSettings();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Load config
*/
//----------------------------------------------------------------------------------------------------------------------
void C_FlaDllWidget::m_LoadConfig() const
{
   C_CamMosDllWidget::E_CanDllType e_TargetDllType;
   const C_UsHandler::E_CanDllType e_CanDllType = C_UsHandler::h_GetInstance()->GetCanDllType();
   const QString c_Path = C_UsHandler::h_GetInstance()->GetCustomCanDllPath();
   // check radio button depending on type
   switch (e_CanDllType)
   {
   case C_UsHandler::ePEAK:
      e_TargetDllType = C_CamMosDllWidget::ePEAK;
      break;
   case C_UsHandler::eVECTOR:
      e_TargetDllType = C_CamMosDllWidget::eVECTOR;
      break;
   case C_UsHandler::eOTHER:
      e_TargetDllType = C_CamMosDllWidget::eOTHER;
      break;
   default:
      e_TargetDllType = C_CamMosDllWidget::ePEAK;
      break;
   }

   // set line edit text to last known custom path
   this->m_LoadSpecifiedConfig(e_TargetDllType, c_Path);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get current dll path

   \return
   Current dll path
*/
//----------------------------------------------------------------------------------------------------------------------
QString C_FlaDllWidget::m_GetCurrentDllPath() const
{
   QString c_Path;

   // Get absolute DLL path (resolve variables and make absolute if it is relative ant not empty)
   c_Path = C_UsHandler::h_GetInstance()->GetCanDllPath();
   if (c_Path.isEmpty() == false)
   {
      c_Path = C_FlaUti::h_GetResolvedAbsolutePathFromExe(c_Path);
   }
   return c_Path;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Save dll path

   \param[in]  orc_DllPath    Dll path
*/
//----------------------------------------------------------------------------------------------------------------------
void C_FlaDllWidget::m_SaveDllPath(const QString & orc_DllPath) const
{
   C_UsHandler::h_GetInstance()->SetCustomCanDllPath(orc_DllPath);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Save dll path

   \param[in]  oe_DllType  Dll type
*/
//----------------------------------------------------------------------------------------------------------------------
void C_FlaDllWidget::m_SaveDllType(const C_CamMosDllWidget::E_CanDllType oe_DllType) const
{
   C_UsHandler::E_CanDllType e_TargetDllType;
   // check radio button depending on type
   switch (oe_DllType)
   {
   case C_CamMosDllWidget::ePEAK:
      e_TargetDllType = C_UsHandler::ePEAK;
      break;
   case C_CamMosDllWidget::eVECTOR:
      e_TargetDllType = C_UsHandler::eVECTOR;
      break;
   case C_CamMosDllWidget::eOTHER:
      e_TargetDllType = C_UsHandler::eOTHER;
      break;
   default:
      e_TargetDllType = C_UsHandler::ePEAK;
      break;
   }
   C_UsHandler::h_GetInstance()->SetCanDllType(e_TargetDllType);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Resolve placeholder variables

   \param[in]  orc_Path    Path

   \return
   Path
*/
//----------------------------------------------------------------------------------------------------------------------
//lint -e{715} false positive: orc_Path is referenced in a call, but somehow PC Lint does not get this
QString C_FlaDllWidget::m_ResolvePlaceholderVariables(const QString & orc_Path) const
{
   return C_FlaUti::h_ResolvePlaceholderVariables(orc_Path);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get placeholder line edit

   \return
   Placeholder line edit
*/
//----------------------------------------------------------------------------------------------------------------------
C_OgeLeFilePathBase * C_FlaDllWidget::m_GetPlaceholderLineEdit()
{
   return new C_FlaOgeLeFilePath(this);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get path variables button

   \return
   Path variables button
*/
//----------------------------------------------------------------------------------------------------------------------
C_CamOgePubPathVariablesBase * C_FlaDllWidget::m_GetPathVariablesButton()
{
   return new C_FlaOgePubPathVariables(this);
}
