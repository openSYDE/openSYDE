//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Widget for showing the progress and the state of the update process

   \copyright   Copyright 2018 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#include <QFileInfo>
#include <QFontMetrics>
#include <QWinTaskbarButton>
#include "stwerrors.h"
#include "constants.h"
#include "C_OgeWiUtil.h"
#include "C_GtGetText.h"
#include "C_OsyHexFile.h"
#include "C_PuiProject.h"
#include "C_PuiSvHandler.h"
#include "C_SyvUpSummaryWidget.h"
#include "ui_C_SyvUpSummaryWidget.h"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw_types;
using namespace stw_errors;
using namespace stw_opensyde_gui;
using namespace stw_opensyde_core;
using namespace stw_opensyde_gui_logic;
using namespace stw_opensyde_gui_elements;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */
//System update layout state thresholds
const stw_types::sint32 C_SyvUpSummaryWidget::mhs32_LAYOUT_THRESHOLD_1 = 120;
const stw_types::sint32 C_SyvUpSummaryWidget::mhs32_LAYOUT_THRESHOLD_2 = 160; // Keep synced with update package widget
const stw_types::sint32 C_SyvUpSummaryWidget::mhs32_LAYOUT_THRESHOLD_3 = 250;
const stw_types::sint32 C_SyvUpSummaryWidget::mhs32_LAYOUT_THRESHOLD_4 = 300;

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
C_SyvUpSummaryWidget::C_SyvUpSummaryWidget(QWidget * const opc_Parent) :
   QWidget(opc_Parent),
   mpc_Ui(new Ui::C_SyvUpSummaryWidget)
{
   mpc_Ui->setupUi(this);

   //Deactivate debug string
   this->mpc_Ui->pc_LabelHeadingIcon->setText("");

   //Progress bar design
   this->mpc_Ui->pc_ProgressBarBig->SetColorFree(mc_STYLE_GUIDE_COLOR_11);
   this->mpc_Ui->pc_ProgressBarBig->SetColorTooMuch(mc_STYLE_GUIDE_COLOR_13);
   this->mpc_Ui->pc_ProgressBarBig->SetSpacingWidth(0);
   this->mpc_Ui->pc_ProgressBarSmall->SetColorTooMuch(mc_STYLE_GUIDE_COLOR_13);
   this->mpc_Ui->pc_ProgressBarSmall->SetSpacingWidth(0);

   //Label style
   this->mpc_Ui->pc_LabelPercentBig->SetForegroundColor(1);
   this->mpc_Ui->pc_LabelPercentBig->SetFontPixel(16);
   this->mpc_Ui->pc_LabelPercentSmall->SetForegroundColor(1);
   this->mpc_Ui->pc_LabelPercentSmall->SetFontPixel(16);

   //Button icon
   this->mpc_Ui->pc_PushButton->SetSvg("://images/system_views/IconDoubleArrowDown.svg");

   //Button connect
   connect(this->mpc_Ui->pc_PushButton, &C_OgePubSvgIconOnly::clicked,
           this, &C_SyvUpSummaryWidget::SigHideBigSummaryWidget);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   default destructor

   Clean up.
*/
//----------------------------------------------------------------------------------------------------------------------
C_SyvUpSummaryWidget::~C_SyvUpSummaryWidget()
{
   delete mpc_Ui;
   //lint -e{1740}  no memory leak because the ownership of these objects was never transfered to this class
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set heading text and style

   \param[in]  orc_Icon    Icon for text decoration
   \param[in]  orc_Text    Heading text
   \param[in]  oq_Failure  Flag if failed (for red coloring)
   \param[in]  oq_Success  Flag if succeeded (for green coloring)
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvUpSummaryWidget::SetHeading(const QString & orc_Icon, const QString & orc_Text, const bool oq_Failure,
                                      const bool oq_Success) const
{
   if (orc_Icon.compare("") != 0)
   {
      this->mpc_Ui->pc_LabelHeadingIcon->SetSvg(orc_Icon);
      // do not show gif AND icon (when user resizes after success this state stays but is okay)
      this->mpc_Ui->pc_LabelHeadingIcon->setVisible(((oq_Success == true) &&
                                                     (this->mpc_Ui->pc_WidgetCircle->isVisible() == false)) ||
                                                    (oq_Success == false));
   }
   else
   {
      this->mpc_Ui->pc_LabelHeadingIcon->setVisible(false);
   }
   this->mpc_Ui->pc_LabelHeading->setText(orc_Text);

   if (oq_Failure == true)
   {
      this->mpc_Ui->pc_WidgetProgress->SetActiveColor(mc_STYLE_GUIDE_COLOR_24);
      this->mpc_Ui->pc_ProgressBarBig->SetColorReserved(mc_STYLE_GUIDE_COLOR_24);
      this->mpc_Ui->pc_ProgressBarSmall->SetColorReserved(mc_STYLE_GUIDE_COLOR_24);
   }
   else
   {
      this->mpc_Ui->pc_WidgetProgress->SetActiveColor(mc_STYLE_GUIDE_COLOR_13);
      this->mpc_Ui->pc_ProgressBarBig->SetColorReserved(mc_STYLE_GUIDE_COLOR_13);
      this->mpc_Ui->pc_ProgressBarSmall->SetColorReserved(mc_STYLE_GUIDE_COLOR_13);
   }

   //Style label
   C_OgeWiUtil::h_ApplyStylesheetProperty(this->mpc_Ui->pc_LabelHeading, "Success", oq_Success);
   C_OgeWiUtil::h_ApplyStylesheetProperty(this->mpc_Ui->pc_LabelHeading, "Error", oq_Failure);
   C_OgeWiUtil::h_ApplyStylesheetProperty(this->mpc_Ui->pc_LabelElapsed, "Error", oq_Failure);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Update progress visualization

   \param[in]   ou16_Progress100  progress as number between 0 and 100
   \param[in]   oq_Finished       Flag if already finished
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvUpSummaryWidget::SetProgress(const uint16 ou16_Progress100, const bool oq_Finished) const
{
   this->mpc_Ui->pc_WidgetProgress->SetProgress100(ou16_Progress100, oq_Finished);
   this->mpc_Ui->pc_ProgressBarBig->SetProgress(static_cast<sint32>(ou16_Progress100), oq_Finished);
   this->mpc_Ui->pc_LabelPercentBig->setText(QString::number(ou16_Progress100) + "%");
   this->mpc_Ui->pc_ProgressBarSmall->SetProgress(static_cast<sint32>(ou16_Progress100), oq_Finished);
   this->mpc_Ui->pc_LabelPercentSmall->setText(QString::number(ou16_Progress100) + "%");
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Update elapsed time label.

   \param[in]       orc_Time   Time string
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvUpSummaryWidget::SetElapsedTime(const QString & orc_Time) const
{
   this->mpc_Ui->pc_LabelElapsed->setText(orc_Time);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Update data transfer label.

   \param[in]       orc_DataTransfer   Data transfer string
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvUpSummaryWidget::SetDataTransfer(const QString & orc_DataTransfer) const
{
   this->mpc_Ui->pc_LabelDataTransfer->setText(orc_DataTransfer);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Overwritten resize event slot

   Here: update layout state

   \param[in,out] opc_Event Event identification and information
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvUpSummaryWidget::resizeEvent(QResizeEvent * const opc_Event)
{
   QWidget::resizeEvent(opc_Event);

   if (this->height() < mhs32_LAYOUT_THRESHOLD_1)
   {
      // show small progress bar
      this->mpc_Ui->pc_WidgetCircle->setVisible(false);
      this->mpc_Ui->pc_WidgetBarBig->setVisible(false);
      this->mpc_Ui->pc_WidgetBarSmall->setVisible(true);
   }
   else if ((this->height() >= mhs32_LAYOUT_THRESHOLD_1) && (this->height() < mhs32_LAYOUT_THRESHOLD_2))
   {
      // show big progress bar
      this->mpc_Ui->pc_WidgetCircle->setVisible(false);
      this->mpc_Ui->pc_WidgetBarBig->setVisible(true);
      this->mpc_Ui->pc_WidgetBarSmall->setVisible(false);
   }
   else if ((this->height() >= mhs32_LAYOUT_THRESHOLD_2) && (this->height() < mhs32_LAYOUT_THRESHOLD_3))
   {
      // show circle only
      this->mpc_Ui->pc_WidgetCircle->setVisible(true);
      this->mpc_Ui->pc_WidgetBarBig->setVisible(false);
      this->mpc_Ui->pc_WidgetBarSmall->setVisible(false);
      this->mpc_Ui->pc_WidgetElapsed->setVisible(false);
      this->mpc_Ui->pc_WidgetDataTransfer->setVisible(false);
   }
   else if ((this->height() >= mhs32_LAYOUT_THRESHOLD_3) && (this->height() < mhs32_LAYOUT_THRESHOLD_4))
   {
      // show circle and time
      this->mpc_Ui->pc_WidgetCircle->setVisible(true);
      this->mpc_Ui->pc_WidgetBarBig->setVisible(false);
      this->mpc_Ui->pc_WidgetBarSmall->setVisible(false);
      this->mpc_Ui->pc_WidgetElapsed->setVisible(true);
      this->mpc_Ui->pc_WidgetDataTransfer->setVisible(false);
   }
   else // i.e. >= mhs32_LAYOUT_THRESHOLD_4
   {
      // show circle, time and label
      this->mpc_Ui->pc_WidgetCircle->setVisible(true);
      this->mpc_Ui->pc_WidgetBarBig->setVisible(false);
      this->mpc_Ui->pc_WidgetBarSmall->setVisible(false);
      this->mpc_Ui->pc_WidgetElapsed->setVisible(true);
      this->mpc_Ui->pc_WidgetDataTransfer->setVisible(true);
   }
}
