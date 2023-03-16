//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Button with menu for selecting path variables

   Button with menu for selecting path variables.Styled to fit seamless between
   path line edit and browse button.

   \copyright   Copyright 2019 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.hpp"

#include "constants.hpp"
#include "C_OscUtils.hpp"
#include "C_GtGetText.hpp"
#include "C_OgePubPathVariables.hpp"
#include "C_OgeLabGenericNoPaddingNoMargins.hpp"

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
C_OgePubPathVariables::C_OgePubPathVariables(QWidget * const opc_Parent) :
   C_OgePubToolTipBase(opc_Parent),
   mpc_Menu(new C_OgeMuSections(opc_Parent))
{
   // first section: openSYDE actions
   this->mpc_Menu->AddCustomSection(C_GtGetText::h_GetText("openSYDE"));
   this->mpc_Menu->addAction(C_GtGetText::h_GetText("openSYDE Binary"),
                             this, &C_OgePubPathVariables::m_OpenSydeExeTriggered);
   this->mpc_Menu->addAction(C_GtGetText::h_GetText("openSYDE Project"),
                             this, &C_OgePubPathVariables::m_OpenSydeProjTriggered);

   // second section: system actions
   this->mpc_Menu->AddCustomSection(C_GtGetText::h_GetText("System"));
   this->mpc_Menu->addAction(C_GtGetText::h_GetText("User Name"), this, &C_OgePubPathVariables::m_UserNameTriggered);
   this->mpc_Menu->addAction(C_GtGetText::h_GetText("Computer Name"),
                             this, &C_OgePubPathVariables::m_ComputerNameTriggered);
   this->mpc_Menu->setMinimumWidth(140);

   this->setMenu(this->mpc_Menu);

   this->setIcon(QIcon("://images/IconPathVariables.svg"));
   this->SetToolTipInformation(C_GtGetText::h_GetText("Insert Variable"),
                               C_GtGetText::h_GetText("Use common locations like your project path as variables."));
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default destructor
*/
//----------------------------------------------------------------------------------------------------------------------
C_OgePubPathVariables::~C_OgePubPathVariables()
{
   delete this->mpc_Menu;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Add Data Block section to menu
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OgePubPathVariables::AddDatablockSection(void)
{
   this->mpc_Menu->AddCustomSection(C_GtGetText::h_GetText("Data Block"));
   this->mpc_Menu->addAction(C_GtGetText::h_GetText("Project Path"),
                             this, &C_OgePubPathVariables::m_DataBlockProjTriggered);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Slot of openSYDE binary menu action.

   Emit signal with variable as string.
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OgePubPathVariables::m_OpenSydeExeTriggered(void)
{
   Q_EMIT (this->SigVariableSelected(C_OscUtils::hc_PATH_VARIABLE_OPENSYDE_BIN.c_str()));
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Slot of openSYDE project menu action.

   Emit signal with variable as string.
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OgePubPathVariables::m_OpenSydeProjTriggered(void)
{
   Q_EMIT (this->SigVariableSelected(C_OscUtils::hc_PATH_VARIABLE_OPENSYDE_PROJ.c_str()));
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Slot of data block project menu action.

   Emit signal with variable as string.
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OgePubPathVariables::m_DataBlockProjTriggered(void)
{
   Q_EMIT (this->SigVariableSelected(C_OscUtils::hc_PATH_VARIABLE_DATABLOCK_PROJ.c_str()));
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Slot of user name menu action.

   Emit signal with variable as string.
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OgePubPathVariables::m_UserNameTriggered(void)
{
   Q_EMIT (this->SigVariableSelected(C_OscUtils::hc_PATH_VARIABLE_USER_NAME.c_str()));
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Slot of computer name menu action.

   Emit signal with variable as string.
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OgePubPathVariables::m_ComputerNameTriggered(void)
{
   Q_EMIT (this->SigVariableSelected(C_OscUtils::hc_PATH_VARIABLE_COMPUTER_NAME.c_str()));
}
