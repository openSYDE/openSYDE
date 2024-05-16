//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Represents the buttom update widget.

   Display information about update progress.

   \copyright   Copyright 2024 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.hpp"

#include "C_FlaBottomBar.hpp"
#include "C_OgeWiUtil.hpp"
#include "constants.hpp"
#include "ui_C_FlaBottomBar.h"
#include "C_FlaUpListItemWidget.hpp"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw::opensyde_gui;
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
C_FlaBottomBar::C_FlaBottomBar(QWidget * const opc_Parent) :
   QWidget(opc_Parent),
   mpc_Ui(new Ui::C_FlaBottomBar)
{
   const QSize c_DEFAULT_SIZE = QSize(20, 20);
   const int32_t os32_FOREGROUND_COLOR_VALUE = 5;
   const int32_t os32_FONT_PIXEL_VALUE = 14;
   const QString c_ProgressIcon = ":/images/UpdateSummaryOk.svg";

   this->mpc_Ui->setupUi(this);

   // define data transfer label (right side corner) with color and pixel font. In C_FlaUpProperties constructor the
   // text is set
   this->mpc_Ui->pc_LabelDataTransfer->SetForegroundColor(os32_FOREGROUND_COLOR_VALUE);
   this->mpc_Ui->pc_LabelDataTransfer->SetFontPixel(os32_FONT_PIXEL_VALUE);

   // define percentage label with text, color, and pixel font
   this->mpc_Ui->pc_LabelPercent->SetForegroundColor(os32_FOREGROUND_COLOR_VALUE);
   this->mpc_Ui->pc_LabelPercent->SetFontPixel(os32_FONT_PIXEL_VALUE);
   this->mpc_Ui->pc_LabelPercent->setText("0%");

   // define percentage label with text, color, and pixel font
   this->mpc_Ui->pc_LabelTime->SetForegroundColor(os32_FOREGROUND_COLOR_VALUE);
   this->mpc_Ui->pc_LabelTime->SetFontPixel(os32_FONT_PIXEL_VALUE);

   // define title of the progress bar (left side corner) with text, color, and pixel font
   this->mpc_Ui->pc_LabelTitle->SetForegroundColor(os32_FOREGROUND_COLOR_VALUE);
   this->mpc_Ui->pc_LabelTitle->SetFontPixel(os32_FONT_PIXEL_VALUE);
   this->mpc_Ui->pc_LabelTitle->setText("---");

   // define progress bar with colors and spacing
   this->mpc_Ui->pc_ProgressBar->SetSpacingWidth(0);
   this->mpc_Ui->pc_ProgressBar->SetColorReserved(mc_STYLE_GUIDE_COLOR_13);
   this->mpc_Ui->pc_ProgressBar->SetColorFree(mc_STYLE_GUIDE_COLOR_11);
   this->mpc_Ui->pc_ProgressBar->SetColorTooMuch(mc_STYLE_GUIDE_COLOR_18);

   // define progress icon by making the icon visible and define the icon with a default size
   this->mpc_Ui->pc_ProgressIcon->setPixmap(QIcon(c_ProgressIcon).pixmap(c_DEFAULT_SIZE));
   this->mpc_Ui->pc_ProgressIcon->setVisible(true);

   m_SetSizePolicy();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default destructor
*/
//----------------------------------------------------------------------------------------------------------------------
//lint -e{1540}  no memory leak because of the parent of mpc_Movie and the Qt memory management
C_FlaBottomBar::~C_FlaBottomBar()
{
   delete this->mpc_Ui;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Show progress bar text

   Change the text and style it if necessary

   \param[in]  orc_Text             Heading text
   \param[in]  oru8_ProgressState   State of the progress (hu32_STATE_DEFAULT - black,
                                                           hu32_STATE_FINISHED - green,
                                                           hu32_STATE_ERROR - red)
*/
//----------------------------------------------------------------------------------------------------------------------
void C_FlaBottomBar::SetHeadingText(const QString & orc_Text, const uint8_t & oru8_ProgressState) const
{
   this->mpc_Ui->pc_LabelTitle->setText(orc_Text);
   this->m_StyleHeadingText(oru8_ProgressState);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Set Heading Icon

   \param[in]  oru8_ProgressState     Progress state

*/
//----------------------------------------------------------------------------------------------------------------------
void C_FlaBottomBar::SetHeadingIcon(const uint8_t & oru8_ProgressState)
{
   const QSize c_SIZE = this->mpc_Ui->pc_ProgressIcon->size();
   const bool q_IS_VISIBLE =
      ((oru8_ProgressState == C_FlaUpListItemWidget::hu32_STATE_ERROR) ||
       (oru8_ProgressState == C_FlaUpListItemWidget::hu32_STATE_FINISHED)) ? true : false;

   this->mpc_Ui->pc_ProgressIcon->setVisible(q_IS_VISIBLE);

   switch (oru8_ProgressState)
   {
   case C_FlaUpListItemWidget::hu32_STATE_ERROR:
      this->mpc_Ui->pc_ProgressIcon->setPixmap(QIcon(":/images/UpdateSummaryFailure.svg").pixmap(c_SIZE));
      break;
   case C_FlaUpListItemWidget::hu32_STATE_FINISHED:
      this->mpc_Ui->pc_ProgressIcon->setPixmap(QIcon(":/images/UpdateSummaryOk.svg").pixmap(c_SIZE));
      break;
   default:
      break;
   }

   this->repaint();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief    Style heading labels

   \param[in]  u8_ProgressState     State of the progress (hu32_STATE_DEFAULT - black,
                                                           hu32_STATE_FINISHED - green,
                                                           hu32_STATE_ERROR - red)
*/
//----------------------------------------------------------------------------------------------------------------------
void C_FlaBottomBar::m_StyleHeadingText(const uint8_t & oru8_ProgressState) const
{
   int32_t s32_Textcolor = 5; //mc_STYLE_GUIDE_COLOR_5 - default

   if (oru8_ProgressState == static_cast<uint8_t>(C_FlaUpListItemWidget::hu32_STATE_FINISHED))
   {
      s32_Textcolor = 21; //mc_STYLE_GUIDE_COLOR_13 - green
   }
   else if (oru8_ProgressState == static_cast<uint8_t>(C_FlaUpListItemWidget::hu32_STATE_ERROR))
   {
      s32_Textcolor = 24; // mc_STYLE_GUIDE_COLOR_24 - red
   }
   else
   {
   }

   this->mpc_Ui->pc_LabelTitle->SetForegroundColor(s32_Textcolor);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Update progress visualization

   \param[in]   ou16_Progress100  Progress as number between 0 and 100
   \param[in]   oq_Finished       Flag if already finished
*/
//----------------------------------------------------------------------------------------------------------------------
void C_FlaBottomBar::SetProgress(const uint16_t ou16_Progress100, const bool oq_Finished) const
{
   this->mpc_Ui->pc_ProgressBar->SetProgress(static_cast<int32_t>(ou16_Progress100), oq_Finished);
   this->mpc_Ui->pc_LabelPercent->setText(QString::number(ou16_Progress100) + "%");
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Set progress bar color

   \param[in]  orq_Success    If oq_Success bar is green else red
*/
//----------------------------------------------------------------------------------------------------------------------
void C_FlaBottomBar::SetProgressBarColor(const bool & orq_Success) const
{
   if (orq_Success)
   {
      this->mpc_Ui->pc_ProgressBar->SetColorReserved(mc_STYLE_GUIDE_COLOR_13);
   }
   else
   {
      this->mpc_Ui->pc_ProgressBar->SetColorReserved(mc_STYLE_GUIDE_COLOR_24);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Update elapsed time label.

   \param[in]       orc_Time   Time string
*/
//----------------------------------------------------------------------------------------------------------------------
void C_FlaBottomBar::SetElapsedTime(const QString & orc_Time) const
{
   this->mpc_Ui->pc_LabelTime->setText(orc_Time);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Update data transfer label

   \param[in]       orc_DataTransfer   Data transfer string
*/
//----------------------------------------------------------------------------------------------------------------------
void C_FlaBottomBar::SetDataTransfer(const QString & orc_DataTransfer) const
{
   const QString c_DefaultText = "Data Transfer: ";
   const QString c_Text = c_DefaultText + orc_DataTransfer;

   this->mpc_Ui->pc_LabelDataTransfer->setText(c_Text);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Set icon size policy
*/
//----------------------------------------------------------------------------------------------------------------------
void C_FlaBottomBar::m_SetSizePolicy(void)
{
   QSizePolicy c_SizePolicy = this->mpc_Ui->pc_ProgressIcon->sizePolicy();

   const bool q_RETAIN_WIDGET_SIZE_WHEN_HIDDEN = false;

   c_SizePolicy.setRetainSizeWhenHidden(q_RETAIN_WIDGET_SIZE_WHEN_HIDDEN);

   this->mpc_Ui->pc_ProgressIcon->setSizePolicy(c_SizePolicy);
}
