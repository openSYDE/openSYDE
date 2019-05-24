//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Widget for dashboard interactions (connect and dark mode)

   Widget for dashboard interactions (connect and dark mode)

   \copyright   Copyright 2019 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.h"

#include "constants.h"
#include "C_GtGetText.h"
#include "C_SyvDaDashboardInteraction.h"
#include "ui_C_SyvDaDashboardInteraction.h"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw_opensyde_gui;
using namespace stw_opensyde_gui_logic;

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
C_SyvDaDashboardInteraction::C_SyvDaDashboardInteraction(QWidget * const opc_Parent) :
   QWidget(opc_Parent),
   mpc_Movie(new QMovie("://images/system_views/UpdateAnimationInProgress.gif", QByteArray(), this)),
   mpc_Ui(new Ui::C_SyvDaDashboardInteraction)
{
   this->mpc_Ui->setupUi(this);

   //Remove debug label
   this->mpc_Ui->pc_LabelConnectStatus->setText("");

   //Style label
   this->mpc_Ui->pc_LabelConnectText->SetBackgroundColor(-1);
   this->mpc_Ui->pc_LabelConnectText->SetForegroundColor(4);
   this->mpc_Ui->pc_LabelConnectText->SetFontPixel(16, false, false);

   //Apply GIF
   this->mpc_Movie->setScaledSize(QSize(20, 20));
   this->mpc_Ui->pc_LabelConnectStatus->setMovie(this->mpc_Movie);
   this->mpc_Ui->pc_LabelConnectStatus->setVisible(false);

   InitStaticNames();

   connect(this->mpc_Ui->pc_PushButtonDarkMode, &QPushButton::clicked, this,
           &C_SyvDaDashboardInteraction::SigPushButtonDarkModePressed);
   connect(this->mpc_Ui->pc_PushButtonConnect, &QPushButton::clicked, this,
           &C_SyvDaDashboardInteraction::SigPushButtonConnectPressed);
   connect(this->mpc_Movie, &QMovie::finished, this, &C_SyvDaDashboardInteraction::m_RestartMovie);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default destructor
*/
//----------------------------------------------------------------------------------------------------------------------
C_SyvDaDashboardInteraction::~C_SyvDaDashboardInteraction(void)
{
   delete this->mpc_Ui;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Initialize all displayed static names
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaDashboardInteraction::InitStaticNames(void) const
{
   this->mpc_Ui->pc_LabelConnectText->setText(C_GtGetText::h_GetText("Online"));
   this->mpc_Ui->pc_PushButtonDarkMode->SetToolTipInformation(C_GtGetText::h_GetText("Switch Mode"),
                                                              C_GtGetText::h_GetText(
                                                                 "Switch between dark and bright mode."));
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Set push button connect SVG icon path

   \param[in] orc_IconPath     Push button connect SVG icon path
   \param[in] oq_ShowAnimation Flag to request show of animation
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaDashboardInteraction::SetPushButtonConnectSvg(const QString & orc_IconPath, const bool oq_ShowAnimation)
{
   this->mpc_Ui->pc_PushButtonConnect->SetSvg(orc_IconPath);
   this->mpc_Ui->pc_PushButtonConnect->update();
   this->mpc_Ui->pc_LabelConnectStatus->setVisible(oq_ShowAnimation);
   //Start animation only while it is displayed for performance reasons
   if (oq_ShowAnimation)
   {
      this->mpc_Movie->start();
   }
   else
   {
      this->mpc_Movie->stop();
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Set push button dark mode SVG icon path

   \param[in] orc_IconPath Push button dark mode SVG icon path
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaDashboardInteraction::SetPushButtonDarkSvg(const QString & orc_IconPath) const
{
   this->mpc_Ui->pc_PushButtonDarkMode->SetSvg(orc_IconPath);
   this->mpc_Ui->pc_PushButtonDarkMode->update();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Restart animation
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaDashboardInteraction::m_RestartMovie(void)
{
   this->mpc_Movie->start();
}
