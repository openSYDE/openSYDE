//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Section header including a label and optional button for expand/collapse. (implementation)

   Section header including a label and optional button for expand/collapse.

   \copyright   Copyright 2018 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.hpp"

#include "C_CamOgeWiSectionHeader.hpp"
#include "ui_C_CamOgeWiSectionHeader.h"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
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
C_CamOgeWiSectionHeader::C_CamOgeWiSectionHeader(QWidget * const opc_Parent) :
   C_OgeWiOnlyBackground(opc_Parent),
   mpc_Ui(new Ui::C_CamOgeWiSectionHeader),
   me_ButtonType(eNOBUTTON)
{
   this->mpc_Ui->setupUi(this);

   // initialize background color
   this->SetBackgroundColor(5);

   // title label
   this->mpc_Ui->pc_Label->SetBackgroundColor(-1);
   this->mpc_Ui->pc_Label->SetForegroundColor(11);
   this->mpc_Ui->pc_Label->SetFontPixel(10, false, true);

   // expand collapse button
   this->mpc_Ui->pc_PushButton->setCheckable(true);

   connect(this->mpc_Ui->pc_PushButton, &C_OgePubSvgIconOnly::toggled,
           this, &C_CamOgeWiSectionHeader::m_OnExpandToggled);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default destructor
*/
//----------------------------------------------------------------------------------------------------------------------
C_CamOgeWiSectionHeader::~C_CamOgeWiSectionHeader()
{
   delete this->mpc_Ui;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set button type.

   \param[in]     ore_ButtonType    Up down: button states are ^ and v
                                    Left right: button states are < and >
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CamOgeWiSectionHeader::SetIconType(const C_CamOgeWiSectionHeader::E_ButtonType & ore_ButtonType)
{
   this->me_ButtonType = ore_ButtonType;

   // set right icons
   if (this->me_ButtonType == eLEFTRIGHT)
   {
      // expanded = checked = arrow right (arrow implies future state); collapsed = unchecked = arrow left
      this->mpc_Ui->pc_PushButton->SetSvg("://images/IconDoubleArrowLeft.svg", "",  "",
                                          "://images/IconDoubleArrowRight.svg");
      this->mpc_Ui->pc_PushButton->setVisible(true);
   }
   else if (this->me_ButtonType == eUPDOWN)
   {
      // expanded = checked = arrow down (arrow implies future state); collapsed = unchecked = arrow up
      this->mpc_Ui->pc_PushButton->SetSvg("://images/IconDoubleArrowUp.svg", "",  "",
                                          "://images/IconDoubleArrowDown.svg");
      this->mpc_Ui->pc_PushButton->setVisible(true);
   }
   else
   {
      this->mpc_Ui->pc_PushButton->setVisible(false);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set state of button.

   \param[in]     orq_IsOpen     true: section is open; false: section is closed
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CamOgeWiSectionHeader::SetOpen(const bool & orq_IsOpen) const
{
   this->mpc_Ui->pc_PushButton->setChecked(orq_IsOpen);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set title of section.

   \param[in]     orc_Title      Title
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CamOgeWiSectionHeader::SetTitle(const QString & orc_Title) const
{
   this->mpc_Ui->pc_Label->setText(orc_Title.toUpper());
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Slot for button toggle.
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CamOgeWiSectionHeader::m_OnExpandToggled(const bool oq_Expand)
{
   Q_EMIT (this->SigExpandSection(oq_Expand));

   if (this->me_ButtonType == E_ButtonType::eLEFTRIGHT)
   {
      this->mpc_Ui->pc_Label->setVisible(oq_Expand);
   }
}
