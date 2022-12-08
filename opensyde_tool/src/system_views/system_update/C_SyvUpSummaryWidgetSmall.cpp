//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Small summary widget to show below update package.

   Display information about update progress in small widget (no GIF etc.).

   \copyright   Copyright 2019 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.hpp"

#include "C_SyvUpSummaryWidgetSmall.hpp"
#include "ui_C_SyvUpSummaryWidgetSmall.h"
#include "C_GtGetText.hpp"
#include "constants.hpp"

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

   \param[in,out] opc_Parent Optional pointer to parent
*/
//----------------------------------------------------------------------------------------------------------------------
C_SyvUpSummaryWidgetSmall::C_SyvUpSummaryWidgetSmall(QWidget * const opc_Parent) :
   QWidget(opc_Parent),
   mpc_Ui(new Ui::C_SyvUpSummaryWidgetSmall)
{
   this->mpc_Ui->setupUi(this);

   //Labels style
   this->mpc_Ui->pc_LabelDataTransfer->SetForegroundColor(5);
   this->mpc_Ui->pc_LabelDataTransfer->SetFontPixel(14);
   this->mpc_Ui->pc_LabelEstimated->SetForegroundColor(5);
   this->mpc_Ui->pc_LabelEstimated->SetFontPixel(14);
   this->mpc_Ui->pc_LabelPercent->SetForegroundColor(5);
   this->mpc_Ui->pc_LabelPercent->SetFontPixel(14);
   this->mpc_Ui->pc_LabelTime->SetForegroundColor(5);
   this->mpc_Ui->pc_LabelTime->SetFontPixel(14);
   this->mpc_Ui->pc_LabelTitle->SetForegroundColor(5);
   this->mpc_Ui->pc_LabelTitle->SetFontPixel(14);

   // progress bar
   this->mpc_Ui->pc_ProgressBar->SetSpacingWidth(0);
   this->mpc_Ui->pc_ProgressBar->SetColorFree(mc_STYLE_GUIDE_COLOR_11);
   this->mpc_Ui->pc_ProgressBar->SetColorTooMuch(mc_STYLE_GUIDE_COLOR_13);

   //Button icon
   this->mpc_Ui->pc_PushButton->SetSvg("://images/system_views/IconDoubleArrowUp.svg");

   //Button connect
   connect(this->mpc_Ui->pc_PushButton, &C_OgePubSvgIconOnly::clicked,
           this, &C_SyvUpSummaryWidgetSmall::SigHideSmallSummaryWidget);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default destructor
*/
//----------------------------------------------------------------------------------------------------------------------
C_SyvUpSummaryWidgetSmall::~C_SyvUpSummaryWidgetSmall()
{
   delete this->mpc_Ui;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set heading text and style

   \param[in]  orc_Icon    Icon for text decoration
   \param[in]  orc_Text    Heading text
   \param[in]  oq_Failure  Flag if failed (for red coloring)
   \param[in]  oq_Success  Flag if succeeded (for green coloring)
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvUpSummaryWidgetSmall::SetHeading(const QString & orc_Icon, const QString & orc_Text, const bool oq_Failure,
                                           const bool oq_Success) const
{
   if (orc_Icon.compare("") != 0)
   {
      this->mpc_Ui->pc_LabelTitleIcon->setVisible(true);
      this->mpc_Ui->pc_LabelTitleIcon->SetSvg(orc_Icon);
   }
   else
   {
      this->mpc_Ui->pc_LabelTitleIcon->setVisible(false);
   }
   this->mpc_Ui->pc_LabelTitle->setText(orc_Text);

   if (oq_Failure == true)
   {
      this->mpc_Ui->pc_ProgressBar->SetColorReserved(mc_STYLE_GUIDE_COLOR_24);
   }
   else
   {
      this->mpc_Ui->pc_ProgressBar->SetColorReserved(mc_STYLE_GUIDE_COLOR_13);
   }

   //Style labels
   if (oq_Success == true)
   {
      this->mpc_Ui->pc_LabelTitle->SetForegroundColor(21);
   }
   else if (oq_Failure == true)
   {
      this->mpc_Ui->pc_LabelTitle->SetForegroundColor(24);
      this->mpc_Ui->pc_LabelTime->SetForegroundColor(24);
   }
   else
   {
      this->mpc_Ui->pc_LabelTitle->SetForegroundColor(5);
      this->mpc_Ui->pc_LabelTime->SetForegroundColor(5);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Update progress visualization

   \param[in]   ou16_Progress100  progress as number between 0 and 100
   \param[in]   oq_Finished       Flag if already finished
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvUpSummaryWidgetSmall::SetProgress(const uint16_t ou16_Progress100, const bool oq_Finished) const
{
   this->mpc_Ui->pc_ProgressBar->SetProgress(static_cast<int32_t>(ou16_Progress100), oq_Finished);
   this->mpc_Ui->pc_LabelPercent->setText(QString::number(ou16_Progress100) + "%");
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Update elapsed time label.

   \param[in]       orc_Time   Time string
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvUpSummaryWidgetSmall::SetElapsedTime(const QString & orc_Time) const
{
   this->mpc_Ui->pc_LabelTime->setText(orc_Time);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Update data transfer label.

   \param[in]       orc_DataTransfer   Data transfer string
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvUpSummaryWidgetSmall::SetDataTransfer(const QString & orc_DataTransfer) const
{
   this->mpc_Ui->pc_LabelDataTransfer->setText(orc_DataTransfer);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Update estimated time label.

   \param[in]       orc_DataTransfer   Data transfer string
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvUpSummaryWidgetSmall::SetEstimatedTime(const QString & orc_DataTransfer) const
{
   this->mpc_Ui->pc_LabelEstimated->setText(orc_DataTransfer);

   // hide label and separator if string is empty
   this->mpc_Ui->pc_LabelEstimated->setVisible(orc_DataTransfer != "");
   this->mpc_Ui->pc_SeparatorRight->setVisible(orc_DataTransfer != "");
}
