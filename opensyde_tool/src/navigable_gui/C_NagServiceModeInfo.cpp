//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Information widget for service mode active

   Information widget for service mode active

   \copyright   Copyright 2021 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.hpp"

#include <QPainter>

#include "C_GtGetText.hpp"
#include "C_OgeWiUtil.hpp"
#include "C_NagServiceModeInfo.hpp"
#include "ui_C_NagServiceModeInfo.h"

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
C_NagServiceModeInfo::C_NagServiceModeInfo(QWidget * const opc_Parent) :
   C_OgeWiWithToolTip(opc_Parent),
   mpc_Ui(new Ui::C_NagServiceModeInfo)
{
   const QString c_IconPath = "://images/main_page_and_navi_bar/Icon_service_mode_exclamation_16x16.svg";

   this->mpc_Ui->setupUi(this);

   this->mpc_Ui->pc_InfoLabel->SetFontPixel(16, false, false);
   this->mpc_Ui->pc_InfoLabel->SetForegroundColor(4);

   this->InitStaticNames();

   this->mpc_Ui->pc_LabelIcon->SetSvg(c_IconPath);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default destructor
*/
//----------------------------------------------------------------------------------------------------------------------
C_NagServiceModeInfo::~C_NagServiceModeInfo()
{
   delete this->mpc_Ui;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Initialize all displayed static names
*/
//----------------------------------------------------------------------------------------------------------------------
void C_NagServiceModeInfo::InitStaticNames()
{
   this->mpc_Ui->pc_InfoLabel->setText(C_GtGetText::h_GetText("Service Project - Limited Access"));
   this->SetToolTipInformation(C_GtGetText::h_GetText("Service Project"),
                               C_GtGetText::h_GetText("Project for service mode use case.\n"
                                                      "Access is limited to specific views controlled by the project settings"));
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Overridden paint event

   Draws the background element

   \param[in,out]  opc_Event  Pointer to paint event
*/
//----------------------------------------------------------------------------------------------------------------------
void C_NagServiceModeInfo::paintEvent(QPaintEvent * const opc_Event)
{
   QPainter c_Painter(this);

   //draw background
   C_OgeWiUtil::h_DrawBackground(this, &c_Painter);

   QWidget::paintEvent(opc_Event);
}
