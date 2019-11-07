//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Generic heading widget for subsections of settings (implementation)

   Including icon, label and optional toggle.

   \copyright   Copyright 2018 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.h"

#include "C_CamOgeWiSettingSubSection.h"
#include "ui_C_CamOgeWiSettingSubSection.h"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw_opensyde_gui_elements;

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
C_CamOgeWiSettingSubSection::C_CamOgeWiSettingSubSection(QWidget * const opc_Parent) :
   C_OgeWiOnlyBackground(opc_Parent),
   mpc_Ui(new Ui::C_CamOgeWiSettingSubSection)
{
   this->mpc_Ui->setupUi(this);

   // initialize styles
   this->mpc_Ui->pc_LabTitle->SetBackgroundColor(-1);
   this->mpc_Ui->pc_LabTitle->SetFontPixel(13, true);
   this->mpc_Ui->pc_LabTitle->SetForegroundColor(0);

   this->SetBackgroundColor(100);

   // expanded = checked = arrow down (arrow implies actual state); collapsed = unchecked = arrow right
   this->mpc_Ui->pc_PbExpColl->setCheckable(true);
   this->mpc_Ui->pc_PbExpColl->SetSvg("://images/IconArrowRight.svg", "", "", "://images/IconArrowDown.svg");

   connect(this->mpc_Ui->pc_PbExpColl, &C_OgePubSvgIconOnly::toggled, this,
           &C_CamOgeWiSettingSubSection::SigExpandSection);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default destructor
*/
//----------------------------------------------------------------------------------------------------------------------
C_CamOgeWiSettingSubSection::~C_CamOgeWiSettingSubSection()
{
   delete this->mpc_Ui;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set title of subsection.

   \param[in]     orc_Title      title of subsection
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CamOgeWiSettingSubSection::SetTitle(const QString & orc_Title) const
{
   this->mpc_Ui->pc_LabTitle->setText(orc_Title);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set icon of subsection

   \param[in]     orc_Icon       path to icon resource svg
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CamOgeWiSettingSubSection::SetIcon(const QString & orc_Icon) const
{
   this->mpc_Ui->pc_LabIcon->SetSvg(orc_Icon);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set if toggle button is shown.

   \param[in]     oq_HasToggle   true: show toggle, false: hide toggle
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CamOgeWiSettingSubSection::SetToggle(const bool oq_HasToggle)
{
   this->mpc_Ui->pc_PbToggle->setVisible(oq_HasToggle);
   this->mpc_Ui->pc_PbToggle->setCheckable(true);
   this->mpc_Ui->pc_PbToggle->SetSvg("://images/ToggleOff.svg", "://images/ToggleOffDisabled.svg", "",
                                     "://images/ToggleOn.svg", "://images/ToggleOnDisabled.svg");

   connect(this->mpc_Ui->pc_PbToggle, &C_OgePubSvgIconOnly::toggled, this, &C_CamOgeWiSettingSubSection::SigToggled);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set tool tip information of toggle button.

   \param[in]     orc_Heading         tool tip title
   \param[in]     orc_Content         tool tip content
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CamOgeWiSettingSubSection::SetToggleToolTip(const QString & orc_Heading, const QString & orc_Content) const
{
   this->mpc_Ui->pc_PbToggle->SetToolTipInformation(orc_Heading, orc_Content);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set toggle button state (enabled/disabled)

   \param[in] oq_Enabled      state of button
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CamOgeWiSettingSubSection::SetToggleState(const bool oq_Enabled) const
{
   this->mpc_Ui->pc_PbToggle->setChecked(oq_Enabled);
   // checked = enabled = on, unchecked = disabled = off
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set button expand collapse state.

   \param[in]  oq_Expanded   expand collapse state
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CamOgeWiSettingSubSection::SetExpanded(const bool oq_Expanded) const
{
   this->mpc_Ui->pc_PbExpColl->setChecked(oq_Expanded);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Hide or show expand button.

   \param[in]     oq_Visible        true: show button; false: hide button
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CamOgeWiSettingSubSection::ShowExpandButton(const bool oq_Visible) const
{
   this->mpc_Ui->pc_PbExpColl->setVisible(oq_Visible);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Overwritten mouse press event slot

   Here: expand/collapse on  mouse press

   \param[in,out] opc_Event Event identification and information
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CamOgeWiSettingSubSection::mousePressEvent(QMouseEvent * const opc_Event)
{
   C_OgeWiOnlyBackground::mousePressEvent(opc_Event);

   if (this->mpc_Ui->pc_PbExpColl->isVisible() == true)
   {
      this->mpc_Ui->pc_PbExpColl->toggle();
   }
   else
   {
      // Popup mode. Hide the widget.
      Q_EMIT(this->SigHide());
   }
}
