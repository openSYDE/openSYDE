//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Generic part of add variables button

   Generic part of add variables button

   \copyright   Copyright 2023 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.hpp"

#include "stwtypes.hpp"
#include "C_GtGetText.hpp"
#include "C_CamOgePubPathVariablesBase.hpp"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
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

   \param[in,out] opc_Parent Optional pointer to parent
*/
//----------------------------------------------------------------------------------------------------------------------
C_CamOgePubPathVariablesBase::C_CamOgePubPathVariablesBase(QWidget * const opc_Parent) :
   C_OgePubToolTipBase(opc_Parent),
   mpc_Menu(new C_OgeMuSections(opc_Parent))
{
   QIcon c_Icon = QIcon("://images/IconPathVariables.svg").pixmap(16, 16);

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
C_CamOgePubPathVariablesBase::~C_CamOgePubPathVariablesBase()
{
   delete this->mpc_Menu;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Add entry

   \param[in]  orc_MenuText      Menu text
   \param[in]  orc_LineEditText  Line edit text
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CamOgePubPathVariablesBase::m_AddEntry(const QString & orc_MenuText, const QString & orc_LineEditText)
{
   const QAction * const pc_Action = mpc_Menu->addAction(orc_MenuText,
                                                         this, &C_CamOgePubPathVariablesBase::m_ActionTriggered);

   mc_Actions.insert(pc_Action, orc_LineEditText);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Add heading

   \param[in]  orc_Text    Text
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CamOgePubPathVariablesBase::m_AddHeading(const QString & orc_Text)
{
   mpc_Menu->AddCustomSection(orc_Text);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Set menu min width

   \param[in]  os32_Width   Width
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CamOgePubPathVariablesBase::m_SetMenuMinWidth(const int32_t os32_Width)
{
   mpc_Menu->setMinimumWidth(os32_Width);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Action triggered
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CamOgePubPathVariablesBase::m_ActionTriggered()
{
   Q_EMIT (this->SigVariableSelected(this->mc_Actions[dynamic_cast<const QAction * const>(this->sender())]));
}
