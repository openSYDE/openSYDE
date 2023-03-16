//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Fix position widget in material design

   The widget offers the possibility to show different widgets with a fix position
   with the same design and layout.

   \copyright   Copyright 2019 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.hpp"

#include <QGraphicsDropShadowEffect>

#include "C_OgeWiFixPosition.hpp"
#include "ui_C_OgeWiFixPosition.h"
#include "C_OgeWiUtil.hpp"
#include "constants.hpp"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw::opensyde_gui;
using namespace stw::opensyde_gui_elements;
using namespace stw::opensyde_gui_logic;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */
static const int32_t ms32_SHADOW_WIDTH = 8;

/* -- Types --------------------------------------------------------------------------------------------------------- */

/* -- Global Variables ---------------------------------------------------------------------------------------------- */

/* -- Module Global Variables --------------------------------------------------------------------------------------- */

/* -- Module Global Function Prototypes ----------------------------------------------------------------------------- */

/* -- Implementation ------------------------------------------------------------------------------------------------ */

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default constructor

   Set up GUI with all elements.

   \param[in,out] opc_Parent   Optional pointer to parent
   \param[in,out] oc_Title     Title of fix position widget
   \param[in,out] oc_IconPath  Icon of fix position widget
   \param[in,out] orc_Geometry Position of fix position widget
*/
//----------------------------------------------------------------------------------------------------------------------
C_OgeWiFixPosition::C_OgeWiFixPosition(const QString oc_Title, const QString oc_IconPath, const QRect & orc_Geometry,
                                       QWidget * const opc_Parent) :
   QWidget(opc_Parent),
   mpc_Ui(new Ui::C_OgeWiFixPosition)
{
   QImage c_Icon;

   this->mpc_Ui->setupUi(this);

   this->mpc_Ui->pc_LabelTitle->setText(oc_Title);
   this->mpc_Ui->pc_LabelTitle->SetForegroundColor(3);
   this->mpc_Ui->pc_LabelTitle->SetFontPixel(13, false, true);
   this->mpc_Ui->pc_PushButtonMax->setIcon(QIcon(":images/IconDoubleArrowLeft.svg"));
   c_Icon.load(oc_IconPath);
   this->mpc_Ui->pc_LabelIcon->setPixmap(QPixmap::fromImage(c_Icon));
   this->setGeometry(orc_Geometry);
   this->mpc_Ui->pc_GroupBox->setMaximumSize(orc_Geometry.width() - ms32_SHADOW_WIDTH,
                                             orc_Geometry.height() - ms32_SHADOW_WIDTH);

   // Activate shadow
   this->m_ApplyShadow();

   connect(this->mpc_Ui->pc_PushButtonMax, &QPushButton::clicked, this, &C_OgeWiFixPosition::SigWiFixPosMaxBtnClicked);
   connect(this->mpc_Ui->pc_LabelTitle, &C_OgeLabDoubleClick::SigDoubleClicked,
           this, &C_OgeWiFixPosition::SigWiFixPosMaxBtnClicked);
   connect(this->mpc_Ui->pc_LabelIcon, &C_OgeLabDoubleClick::SigDoubleClicked,
           this, &C_OgeWiFixPosition::SigWiFixPosMaxBtnClicked);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default destructor
*/
//----------------------------------------------------------------------------------------------------------------------
C_OgeWiFixPosition::~C_OgeWiFixPosition()
{
   delete this->mpc_Ui;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Apply current dark mode setting

   \param[in] oq_Active Dark mode active
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OgeWiFixPosition::m_ApplyShadow()
{
   QColor c_Color;
   QGraphicsDropShadowEffect * pc_Shadow;

   C_OgeWiUtil::h_ApplyStylesheetPropertyToItselfAndAllChildren(this, "DarkMode", false);

   pc_Shadow = new QGraphicsDropShadowEffect(this->mpc_Ui->pc_GroupBox);
   pc_Shadow->setBlurRadius(10.0);
   pc_Shadow->setOffset(0.0);
   c_Color = mc_STYLE_GUIDE_COLOR_33;
   c_Color.setAlpha(128);
   pc_Shadow->setColor(c_Color);
   this->mpc_Ui->pc_GroupBox->setGraphicsEffect(pc_Shadow);
} //lint !e429  //no memory leak because of the parent of pc_Shadow and the Qt memory management
