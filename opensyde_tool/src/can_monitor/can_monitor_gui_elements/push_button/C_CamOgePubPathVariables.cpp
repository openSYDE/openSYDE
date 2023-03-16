//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Button with menu for selecting path variables

   Button with menu for selecting path variables. Styled to fit seamless between
   path line edit and browse button.

   \copyright   Copyright 2019 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.hpp"

#include "constants.hpp"
#include "cam_constants.hpp"
#include "C_GtGetText.hpp"
#include "C_CamOgePubPathVariables.hpp"
#include "C_OscUtils.hpp"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw::opensyde_gui_elements;
using namespace stw::opensyde_gui_logic;
using namespace stw::opensyde_gui;
using namespace stw::opensyde_core;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

/* -- Global Variables ---------------------------------------------------------------------------------------------- */

/* -- Module Global Variables --------------------------------------------------------------------------------------- */

/* -- Module Global Function Prototypes ----------------------------------------------------------------------------- */

/* -- Implementation ------------------------------------------------------------------------------------------------ */

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default constructor

   Set up GUI with all elements.

   \param[in,out] opc_Parent Optional pointer to parent
*/
//----------------------------------------------------------------------------------------------------------------------
C_CamOgePubPathVariables::C_CamOgePubPathVariables(QWidget * const opc_Parent) :
   C_OgePubToolTipBase(opc_Parent),
   mpc_Menu(new C_OgeMuSections(opc_Parent))
{
   QIcon c_Icon = QIcon("://images/IconPathVariables.svg").pixmap(16, 16);

   // menu
   mpc_Menu->AddCustomSection(C_GtGetText::h_GetText("CAN Monitor"));
   mpc_Menu->addAction(C_GtGetText::h_GetText("CAN Monitor Binary"),
                       this, &C_CamOgePubPathVariables::m_CanMonitorExeTriggered);
   mpc_Menu->addAction(C_GtGetText::h_GetText("CAN Monitor Project"),
                       this, &C_CamOgePubPathVariables::m_CanMonitorProjTriggered);
   mpc_Menu->AddCustomSection(C_GtGetText::h_GetText("System"));
   mpc_Menu->addAction(C_GtGetText::h_GetText("User Name"), this, &C_CamOgePubPathVariables::m_UserNameTriggered);
   mpc_Menu->addAction(C_GtGetText::h_GetText("Computer Name"), this,
                       &C_CamOgePubPathVariables::m_ComputerNameTriggered);
   mpc_Menu->setMinimumWidth(140);

   this->setMenu(mpc_Menu);

   // icon
   c_Icon.addPixmap(QIcon("://images/IconPathVariablesDisabled.svg").pixmap(16, 16), QIcon::Disabled);
   this->setIcon(c_Icon);

   // tool tip
   this->SetToolTipInformation(C_GtGetText::h_GetText("Insert Variable"),
                               C_GtGetText::h_GetText("Use common locations like your project path as variables."));
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default destructor
*/
//----------------------------------------------------------------------------------------------------------------------
C_CamOgePubPathVariables::~C_CamOgePubPathVariables()
{
   delete this->mpc_Menu;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Slot of openSYDE binary menu action.

   Emit signal with variable as string.
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CamOgePubPathVariables::m_CanMonitorExeTriggered(void)
{
   Q_EMIT (this->SigVariableSelected(mc_PATH_VARIABLE_CAN_MONITOR_BIN));
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Slot of openSYDE project menu action.

   Emit signal with variable as string.
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CamOgePubPathVariables::m_CanMonitorProjTriggered(void)
{
   Q_EMIT (this->SigVariableSelected(mc_PATH_VARIABLE_CAN_MONITOR_PROJ));
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Slot of user name menu action.

   Emit signal with variable as string.
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CamOgePubPathVariables::m_UserNameTriggered(void)
{
   Q_EMIT (this->SigVariableSelected(C_OscUtils::hc_PATH_VARIABLE_USER_NAME.c_str()));
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Slot of computer name menu action.

   Emit signal with variable as string.
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CamOgePubPathVariables::m_ComputerNameTriggered(void)
{
   Q_EMIT (this->SigVariableSelected(C_OscUtils::hc_PATH_VARIABLE_COMPUTER_NAME.c_str()));
}
