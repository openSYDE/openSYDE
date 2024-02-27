//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Widget for update properties

   Widget for update properties

   \copyright   Copyright 2023 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.hpp"

#include <QFileDialog>
#include "C_GtGetText.hpp"
#include "TglUtils.hpp"
#include "C_Uti.hpp"
#include "C_OgeWiUtil.hpp"
#include "TglFile.hpp"
#include "C_UsHandler.hpp"
#include "C_OgeWiCustomMessage.hpp"

#include "C_FlaUpProperties.hpp"
#include "ui_C_FlaUpProperties.h"

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
C_FlaUpProperties::C_FlaUpProperties(QWidget * const opc_Parent) :
   QWidget(opc_Parent),
   mpc_Ui(new Ui::C_FlaUpProperties)
{
   this->mpc_Ui->setupUi(this);
   this->mpc_ScrollLayout = new QVBoxLayout(this->mpc_Ui->pc_ScrollAreaWidget);
   this->mpc_ScrollLayout->setContentsMargins(0, 0, 0, 0);
   this->mpc_ListWidget = new C_FlaUpListWidget(this->mpc_Ui->pc_ScrollAreaWidget);
   this->mpc_ScrollLayout->addWidget(this->mpc_ListWidget);

   this->mpc_Ui->pc_WiTitle->SetTitle(C_GtGetText::h_GetText("Update"));
   this->mpc_Ui->pc_WiTitle->SetIconType(C_CamOgeWiSectionHeader::E_ButtonType::eNOBUTTON);
   this->mpc_Ui->pc_LabelTitle->SetForegroundColor(4);
   this->mpc_Ui->pc_LabelTitle->SetFontPixel(12, true, false);
   this->mpc_Ui->pc_LabelTitle->setText(
      static_cast<QString>(C_GtGetText::h_GetText("<b>Hex-Files (%1)<\b>")).arg(QString::number(this->mpc_ListWidget->
                                                                                                count())));
   if (this->mpc_ListWidget->count() <= 0)
   {
      this->mpc_Ui->pc_GroupBoxNoMessages->setVisible(true);
      this->mpc_Ui->pc_Seperator->setVisible(false);
   }
   else
   {
      this->mpc_Ui->pc_GroupBoxNoMessages->setVisible(false);
      this->mpc_Ui->pc_Seperator->setVisible(true);
   }
   this->mpc_Ui->pc_AddButton->SetSvg("://images/IconAddEnabled.svg", "://images/IconAddDisabled.svg",
                                      "://images/IconAddEnabledHover.svg", "", "",
                                      "",
                                      "://images/IconAddEnabledPressed.svg");
   this->mpc_Ui->pc_AddButton->SetToolTipInformation(C_GtGetText::h_GetText("Add file"),
                                                     C_GtGetText::h_GetText(
                                                        "Add HEX file (.*hex)"));
   this->mpc_Ui->pc_FileIconButton->SetSvg("://images/IconApp.svg");

   stw::opensyde_gui_logic::C_OgeWiUtil::h_ApplyStylesheetProperty(this->mpc_Ui->pc_Seperator,
                                                                   "HasColor9Background", true);
   this->mpc_Ui->pc_ScrollAreaWidget->SetBackgroundColor(0);
   this->mpc_Ui->pc_LabelNoMessages->SetForegroundColor(8);
   this->mpc_Ui->pc_LabelNoMessages->SetFontPixel(12, false, false);
   this->mpc_Ui->pc_LabelNoMessages->setText("No files, add any via the '+' button.");

   connect(this->mpc_Ui->pc_AddButton, &QPushButton::clicked, this, [this] ()
   {
      this->mpc_ListWidget->AddFileAction(false, this->mpc_ListWidget->count());
   }
           );
   connect(this->mpc_ListWidget, &C_FlaUpListWidget::SigUpdateFileCounter, this,
           &C_FlaUpProperties::m_UpdateLabelTitleWithFileCounter);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default destructor
*/
//----------------------------------------------------------------------------------------------------------------------
C_FlaUpProperties::~C_FlaUpProperties()
{
   delete this->mpc_ScrollLayout;
   delete this->mpc_ListWidget;
   delete this->mpc_Ui;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Save user settings
*/
//----------------------------------------------------------------------------------------------------------------------
void C_FlaUpProperties::SaveUserSettings() const
{
   if (this->mpc_ListWidget != NULL)
   {
      this->mpc_ListWidget->SetLastKnownHexFilePaths();
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get list of all HEX file paths

   \return
   HEX file paths
*/
//----------------------------------------------------------------------------------------------------------------------
QStringList C_FlaUpProperties::GetHexFilePaths() const
{
   QStringList c_HexFilePaths;

   if (this->mpc_ListWidget != NULL)
   {
      c_HexFilePaths =  this->mpc_ListWidget->GetHexFilePaths();
   }
   return c_HexFilePaths;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get list of all devices in all HEX files

   \return
   Device names
*/
//----------------------------------------------------------------------------------------------------------------------
QStringList C_FlaUpProperties::GetHexFileDeviceNames() const
{
   QStringList c_HexFilePaths;

   if (this->mpc_ListWidget != NULL)
   {
      c_HexFilePaths = this->mpc_ListWidget->GetHexFileDeviceNames();
   }
   return c_HexFilePaths;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get information if all files are valid and existing

   \retval   True    all files are existing and valid
   \retval   False   at least one file is missing or invalid
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_FlaUpProperties::AreAllFilesValid(void) const
{
   bool q_Result = false;

   if (this->mpc_ListWidget != NULL)
   {
      q_Result = this->mpc_ListWidget->AreAllFilesValid();
   }
   return q_Result;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Enable/disable settings widgets

   \param[in]  oq_Enabled  true: enable, false: disable
*/
//----------------------------------------------------------------------------------------------------------------------
void C_FlaUpProperties::EnableSettings(const bool oq_Enabled)
{
   this->mpc_Ui->pc_AddButton->setDisabled(!oq_Enabled);
   this->mpc_ListWidget->EnableSettings(oq_Enabled);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Update Title with Counter value
*/
//----------------------------------------------------------------------------------------------------------------------
void C_FlaUpProperties::m_UpdateLabelTitleWithFileCounter() const
{
   if (this->mpc_ListWidget != NULL)
   {
      this->mpc_Ui->pc_LabelTitle->setText(
         static_cast<QString>(C_GtGetText::h_GetText("<b>Hex-Files (%1)<\b>")).arg(this->mpc_ListWidget->count()));
   }

   if (this->mpc_ListWidget->count() <= 0)
   {
      this->mpc_Ui->pc_GroupBoxNoMessages->setVisible(true);
      this->mpc_Ui->pc_Seperator->setVisible(false);
   }
   else
   {
      this->mpc_Ui->pc_GroupBoxNoMessages->setVisible(false);
      this->mpc_Ui->pc_Seperator->setVisible(true);
   }
}
