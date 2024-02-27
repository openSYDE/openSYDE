//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Title bar widget for SYDEflash

   Title bar widget for SYDEflash

   \copyright   Copyright 2023 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.hpp"

#include "C_GtGetText.hpp"
#include "C_HeHandler.hpp"
#include "C_OgeWiUtil.hpp"
#include "C_NagAboutDialog.hpp"
#include "C_OgePopUpDialog.hpp"
#include "C_FlaConNodeConfigPopup.hpp"

#include "C_FlaTitleBarWidget.hpp"
#include "ui_C_FlaTitleBarWidget.h"

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
C_FlaTitleBarWidget::C_FlaTitleBarWidget(QWidget * const opc_Parent) :
   C_OgeWiOnlyBackground(opc_Parent),
   mpc_Ui(new Ui::C_FlaTitleBarWidget)
{
   QPixmap c_ImgLogo;

   this->mpc_Ui->setupUi(this);
   this->SetBackgroundColor(10);

   C_OgeWiUtil::h_ApplyStylesheetProperty(this->mpc_Ui->pc_Frame, "HasColor8Background", true);

   // initialize static names
   this->InitStaticNames();

   //load STW logo
   c_ImgLogo.load("://images/STW_Logo_Dark.png");
   c_ImgLogo = c_ImgLogo.scaled((c_ImgLogo.width() / 18), (c_ImgLogo.height() / 18),
                                Qt::KeepAspectRatio,
                                Qt::SmoothTransformation);
   this->mpc_Ui->pc_LogoLabel->setPixmap(c_ImgLogo);

   // button icons
   this->mpc_Ui->pc_PushButtonUpdate->setIconSize(QSize(22, 22));
   this->mpc_Ui->pc_PushButtonSearch->setIconSize(QSize(24, 24));
   this->mpc_Ui->pc_PushButtonConfigure->setIconSize(QSize(24, 24));
   this->mpc_Ui->pc_PushButtonHelp->setIconSize(QSize(24, 24));
   this->mpc_Ui->pc_PushButtonUpdate->SetSvg("://images/IconUpdate.svg", "://images/IconUpdateDisabled.svg");
   this->mpc_Ui->pc_PushButtonSearch->SetSvg("://images/IconSearch.svg", "://images/IconSearchDisable.svg");
   this->mpc_Ui->pc_PushButtonConfigure->SetSvg("://images/IconConfigure.svg", "://images/IconConfigureDisabled.svg");
   this->mpc_Ui->pc_PushButtonHelp->SetSvg("://images/IconHelp.svg");
   this->mpc_Ui->pc_PushButtonHelp->setText("");
   this->mpc_Ui->pc_PushButtonHelp->SetIconOnly(true);
   connect(this->mpc_Ui->pc_PushButtonUpdate, &QPushButton::clicked, this, &C_FlaTitleBarWidget::SigUpdateNode);
   connect(this->mpc_Ui->pc_PushButtonSearch, &QPushButton::clicked, this, &C_FlaTitleBarWidget::SigSearchNode);
   connect(this->mpc_Ui->pc_PushButtonConfigure, &QPushButton::clicked, this, &C_FlaTitleBarWidget::SigConfigureNode);
   connect(this->mpc_Ui->pc_PushButtonAbout, &QPushButton::clicked, this, &C_FlaTitleBarWidget::m_ShowAbout);
   connect(this->mpc_Ui->pc_PushButtonHelp, &QPushButton::clicked, this, &C_FlaTitleBarWidget::m_TriggerHelp);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default destructor
*/
//----------------------------------------------------------------------------------------------------------------------
C_FlaTitleBarWidget::~C_FlaTitleBarWidget()
{
   delete this->mpc_Ui;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Initialize all displayed static names
*/
//----------------------------------------------------------------------------------------------------------------------
void C_FlaTitleBarWidget::InitStaticNames(void) const
{
   // initialize button texts
   this->mpc_Ui->pc_PushButtonAbout->setText(C_GtGetText::h_GetText("About")); // no icon therefore no icon-only mode
   this->m_SetButtonsText(false);

   //tooltips
   this->mpc_Ui->pc_PushButtonHelp->SetToolTipInformation(C_GtGetText::h_GetText("Help"),
                                                          C_GtGetText::h_GetText("Open user manual for help."));
   this->mpc_Ui->pc_PushButtonAbout->SetToolTipInformation(C_GtGetText::h_GetText("About"),
                                                           C_GtGetText::h_GetText(
                                                              "Show information about SYDEflash."));

   this->mpc_Ui->pc_PushButtonUpdate->SetToolTipInformation(C_GtGetText::h_GetText("Update Node"),
                                                            C_GtGetText::h_GetText(
                                                               "Flash selected HEX files on connected node."));
   this->mpc_Ui->pc_PushButtonSearch->SetToolTipInformation(C_GtGetText::h_GetText("Search Node"),
                                                            C_GtGetText::h_GetText(
                                                               "Scan network for connected openSYDE nodes."));
   this->mpc_Ui->pc_PushButtonConfigure->SetToolTipInformation(C_GtGetText::h_GetText("Configure Node"),
                                                               C_GtGetText::h_GetText("Apply new node ID and bitrate "
                                                                                      "to connected node."));
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Enable/disable action buttons while update

   \param[in]  oq_Enabled  true: enable, false: disable
*/
//----------------------------------------------------------------------------------------------------------------------
void C_FlaTitleBarWidget::EnableActions(const bool oq_Enabled)
{
   this->mpc_Ui->pc_PushButtonUpdate->setEnabled(oq_Enabled);
   this->mpc_Ui->pc_PushButtonSearch->setEnabled(oq_Enabled);
   this->mpc_Ui->pc_PushButtonConfigure->setEnabled(oq_Enabled);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Overwritten resize event slot

   Here: remove or add button text depending on size

   \param[in,out]  opc_Event  Event identification and information
*/
//----------------------------------------------------------------------------------------------------------------------
void C_FlaTitleBarWidget::resizeEvent(QResizeEvent * const opc_Event)
{
   C_OgeWiOnlyBackground::resizeEvent(opc_Event);
   this->m_SetButtonsText(this->width() < 910);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Show about screen
*/
//----------------------------------------------------------------------------------------------------------------------
void C_FlaTitleBarWidget::m_ShowAbout(void)
{
   const QPointer<C_OgePopUpDialog> c_New = new C_OgePopUpDialog(this, this);

   new C_NagAboutDialog(*c_New, "SYDEflash", ":/images/SYDEflash_logo.png", 20);

   //Resize
   const QSize c_SIZE(650, 500);
   c_New->SetSize(c_SIZE);

   c_New->exec();

   if (c_New != NULL)
   {
      c_New->HideOverlay();
   }
} //lint !e429  no memory leak because of the parent of pc_Dialog and the Qt memory management

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Trigger help
*/
//----------------------------------------------------------------------------------------------------------------------
void C_FlaTitleBarWidget::m_TriggerHelp(void)
{
   stw::opensyde_gui_logic::C_HeHandler::h_GetInstance().CallSpecificHelpPage(this->metaObject()->className());
}
/*! \brief  Set buttons text or remove them if only icon should be visible (in small width mode)

   \param[in]  oq_IconOnly    Flag to indicate icon only vs icon with text
*/
//----------------------------------------------------------------------------------------------------------------------
void C_FlaTitleBarWidget::m_SetButtonsText(const bool oq_IconOnly) const
{
   this->mpc_Ui->pc_PushButtonUpdate->SetIconOnly(oq_IconOnly);
   this->mpc_Ui->pc_PushButtonSearch->SetIconOnly(oq_IconOnly);
   this->mpc_Ui->pc_PushButtonConfigure->SetIconOnly(oq_IconOnly);

   if (oq_IconOnly == true)
   {
      const uint32_t u32_SMALL_MAXIMUM_SIZE = 34;
      this->mpc_Ui->pc_PushButtonUpdate->setText(C_GtGetText::h_GetText(""));
      this->mpc_Ui->pc_PushButtonSearch->setText(C_GtGetText::h_GetText(""));
      this->mpc_Ui->pc_PushButtonConfigure->setText(C_GtGetText::h_GetText(""));

      this->mpc_Ui->pc_PushButtonUpdate->setMaximumWidth(u32_SMALL_MAXIMUM_SIZE);
      this->mpc_Ui->pc_PushButtonSearch->setMaximumWidth(u32_SMALL_MAXIMUM_SIZE);
      this->mpc_Ui->pc_PushButtonConfigure->setMaximumWidth(u32_SMALL_MAXIMUM_SIZE);
   }
   else
   {
      this->mpc_Ui->pc_PushButtonUpdate->setText(C_GtGetText::h_GetText("Update Node"));
      this->mpc_Ui->pc_PushButtonSearch->setText(C_GtGetText::h_GetText("Search Node"));
      this->mpc_Ui->pc_PushButtonConfigure->setText(C_GtGetText::h_GetText("Configure Node"));

      // we cannot change Qt constant but it is still better than using the hard coded magic number 16777215
      this->mpc_Ui->pc_PushButtonUpdate->setMaximumWidth(QWIDGETSIZE_MAX);    //lint !e893 !e9130 !e9136
      this->mpc_Ui->pc_PushButtonSearch->setMaximumWidth(QWIDGETSIZE_MAX);    //lint !e893 !e9130 !e9136
      this->mpc_Ui->pc_PushButtonConfigure->setMaximumWidth(QWIDGETSIZE_MAX); //lint !e893 !e9130 !e9136
   }
}
