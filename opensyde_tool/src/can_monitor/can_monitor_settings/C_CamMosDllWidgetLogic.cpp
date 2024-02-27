//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Instance specific part for handling CAN DLL configuration

   Instance specific part for handling CAN DLL configuration

   \copyright   Copyright 2023 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.hpp"

#include "stwtypes.hpp"
#include "C_CamUti.hpp"
#include "cam_constants.hpp"
#include "C_CamProHandler.hpp"
#include "C_CamOgeLeFilePath.hpp"
#include "C_CamMosDllWidgetLogic.hpp"
#include "C_CamOgePubPathVariables.hpp"

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
C_CamMosDllWidgetLogic::C_CamMosDllWidgetLogic(QWidget * const opc_Parent) :
   C_CamMosDllWidget(opc_Parent)
{
   m_InitUi();
   // configuration load
   connect(C_CamProHandler::h_GetInstance(), &C_CamProHandler::SigNewConfiguration,
           this, &C_CamMosDllWidgetLogic::m_LoadConfig);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Load config
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CamMosDllWidgetLogic::m_LoadConfig() const
{
   C_CamMosDllWidget::E_CanDllType e_TargetDllType;
   const C_CamProHandler::E_CanDllType e_CanDllType = C_CamProHandler::h_GetInstance()->GetCanDllType();
   // check radio button depending on type
   switch (e_CanDllType)
   {
   case C_CamProHandler::ePEAK:
      e_TargetDllType = C_CamMosDllWidget::ePEAK;
      break;
   case C_CamProHandler::eVECTOR:
      e_TargetDllType = C_CamMosDllWidget::eVECTOR;
      break;
   case C_CamProHandler::eOTHER:
      e_TargetDllType = C_CamMosDllWidget::eOTHER;
      break;
   default:
      e_TargetDllType = C_CamMosDllWidget::ePEAK;
      break;
   }

   // set line edit text to last known custom path
   const QString c_Path = C_CamProHandler::h_GetInstance()->GetCustomCanDllPath();
   this->m_LoadSpecifiedConfig(e_TargetDllType, c_Path);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get current dll path

   \return
   Current dll path
*/
//----------------------------------------------------------------------------------------------------------------------
QString C_CamMosDllWidgetLogic::m_GetCurrentDllPath() const
{
   QString c_Path;

   // Get absolute DLL path (resolve variables and make absolute if it is relative ant not empty)
   c_Path = C_CamProHandler::h_GetInstance()->GetCanDllPath();
   if (c_Path.isEmpty() == false)
   {
      c_Path = C_CamUti::h_GetResolvedAbsolutePathFromExe(c_Path);
   }
   return c_Path;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Save dll path

   \param[in]  orc_DllPath    Dll path
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CamMosDllWidgetLogic::m_SaveDllPath(const QString & orc_DllPath) const
{
   C_CamProHandler::h_GetInstance()->SetCustomCanDllPath(orc_DllPath);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Save dll path

   \param[in]  oe_DllType  Dll type
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CamMosDllWidgetLogic::m_SaveDllType(const C_CamMosDllWidget::E_CanDllType oe_DllType) const
{
   C_CamProHandler::E_CanDllType e_TargetDllType;
   // check radio button depending on type
   switch (oe_DllType)
   {
   case C_CamMosDllWidget::ePEAK:
      e_TargetDllType = C_CamProHandler::ePEAK;
      break;
   case C_CamMosDllWidget::eVECTOR:
      e_TargetDllType = C_CamProHandler::eVECTOR;
      break;
   case C_CamMosDllWidget::eOTHER:
      e_TargetDllType = C_CamProHandler::eOTHER;
      break;
   default:
      e_TargetDllType = C_CamProHandler::ePEAK;
      break;
   }
   C_CamProHandler::h_GetInstance()->SetCanDllType(e_TargetDllType);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Resolve placeholder variables

   \param[in]  orc_Path    Path

   \return
   Path
*/
//----------------------------------------------------------------------------------------------------------------------
QString C_CamMosDllWidgetLogic::m_ResolvePlaceholderVariables(const QString & orc_Path) const
{
   return C_CamUti::h_ResolvePlaceholderVariables(orc_Path);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get placeholder line edit

   \return
   Placeholder line edit
*/
//----------------------------------------------------------------------------------------------------------------------
C_OgeLeFilePathBase * C_CamMosDllWidgetLogic::m_GetPlaceholderLineEdit()
{
   return new C_CamOgeLeFilePath(this);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get path variables button

   \return
   Path variables button
*/
//----------------------------------------------------------------------------------------------------------------------
C_CamOgePubPathVariablesBase * C_CamMosDllWidgetLogic::m_GetPathVariablesButton()
{
   return new C_CamOgePubPathVariables(this);
}
