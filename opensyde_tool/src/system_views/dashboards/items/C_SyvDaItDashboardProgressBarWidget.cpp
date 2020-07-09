//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Widget base for dashboard progress bar widget

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.h"

#include "constants.h"

#include <cmath>
#include <QPainter>
#include "stwtypes.h"
#include "C_SyvDaItUtil.h"
#include "C_GiBiLineBounding.h"
#include "C_GiBiConnectableItem.h"
#include "C_SyvDaItDashboardProgressBarWidget.h"
#include "ui_C_SyvDaItDashboardProgressBarWidget.h"
#include <QTime>

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw_types;
using namespace stw_opensyde_gui;
using namespace stw_opensyde_gui_logic;
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
C_SyvDaItDashboardProgressBarWidget::C_SyvDaItDashboardProgressBarWidget(QWidget * const opc_Parent) :
   QWidget(opc_Parent),
   mpc_Ui(new Ui::C_SyvDaItDashboardProgressBarWidget),
   me_Type(C_PuiSvDbProgressBar::eTYPE_1),
   me_Alignment(C_PuiSvDbProgressBar::eTOP),
   mc_MinValue("0"),
   mc_MaxValue("100"),
   mq_ShowMinMax(true),
   msn_Transparence(255)
{
   mpc_Ui->setupUi(this);

   this->SetDisplayStyle(C_PuiSvDbWidgetBase::eOPENSYDE, C_PuiSvDbProgressBar::eTYPE_1, C_PuiSvDbProgressBar::eTOP,
                         false);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default destructor

   Clean up.
*/
//----------------------------------------------------------------------------------------------------------------------
C_SyvDaItDashboardProgressBarWidget::~C_SyvDaItDashboardProgressBarWidget(void)
{
   delete mpc_Ui;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Apply style

   \param[in] oe_Style     New style type
   \param[in] oe_Type      New item type
   \param[in] oe_Alignment New item alignment
   \param[in] oq_DarkMode  Flag if dark mode is active
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaItDashboardProgressBarWidget::SetDisplayStyle(const C_PuiSvDbWidgetBase::E_Style oe_Style,
                                                          const C_PuiSvDbProgressBar::E_Type oe_Type,
                                                          const C_PuiSvDbProgressBar::E_Alignment oe_Alignment,
                                                          const bool oq_DarkMode)
{
   this->me_Style = oe_Style;
   this->me_Type = oe_Type;
   this->me_Alignment = oe_Alignment;
   this->mq_DarkMode = oq_DarkMode;

   this->mpc_Ui->pc_ProgressBar->SetDisplayStyle(oe_Style, oq_DarkMode);

   switch (this->me_Type)
   {
   case C_PuiSvDbProgressBar::eTYPE_1:
      this->mpc_Ui->pc_ProgressBar->setOrientation(Qt::Horizontal);
      break;
   case C_PuiSvDbProgressBar::eTYPE_2:
      this->mpc_Ui->pc_ProgressBar->setOrientation(Qt::Vertical);
      break;
   case C_PuiSvDbProgressBar::eTYPE_3:
      this->mpc_Ui->pc_ProgressBar->setOrientation(Qt::Vertical);

      break;
   default:
      break;
   }

   SetBarMargins();
   this->update();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Overwritten paint event slot

   Here: manual markers

   \param[in,out] opc_Event Event identification and information
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaItDashboardProgressBarWidget::paintEvent(QPaintEvent * const opc_Event)
{
   // Karsten Anfang
   // Variables

   QPainter c_Painter(this);
   QFont c_TextFont;
   QRect c_MinRect;
   QRect c_MaxRect;
   QRect c_ValueRect;
   QPointF ac_ArrowPoints[3];
   QPen c_ArrowPen;
   QPen c_MinMaxPen;
   QPen c_ValuePen;

   float32 f32_ArrowSizeH = 3.0F; // Factor for sizing the arrow horizontal
   float32 f32_ArrowSizeV = 5.0F; // Factor for sizing the arrow vertical
   float32 f32_Value;
   sintn sn_ActualPixelSize;
   float32 f32_TemporaryHelpingVariable1;
   float32 f32_TemporaryHelpingVariable2;

   QFontMetrics c_TextFontMetric(c_TextFont);

   Q_UNUSED(opc_Event)
   SetLabelColor(c_Painter, c_TextFont, c_ArrowPen, c_MinMaxPen, c_ValuePen); // Setting all Colors
   c_ArrowPen.setJoinStyle(Qt::RoundJoin);                                    // To draw the arrow with round edges
   // Value for setting the value text to the right position
   f32_Value = static_cast<float32>(this->mpc_Ui->pc_ProgressBar->value()) -
               static_cast<float32>(this->mpc_Ui->pc_ProgressBar->minimum());

   switch (this->me_Type)
   {
   case stw_opensyde_gui_logic::C_PuiSvDbProgressBar::eTYPE_1:

      // Setting up the Rects for Min and Max ---------------------------------------------------------------------
      // Normal if this->rect is high enought -----------------------------------
      c_MinRect.setLeft(this->rect().left());
      f32_TemporaryHelpingVariable1 = static_cast<float32>(this->rect().top()) + mf32_BarMarginTop;
      c_MinRect.setTop(static_cast<sintn>(f32_TemporaryHelpingVariable1));
      f32_TemporaryHelpingVariable1 = static_cast<float32>(this->rect().bottom()) - mf32_BarMarginBottom;
      c_MinRect.setBottom(static_cast<sintn>(f32_TemporaryHelpingVariable1));
      f32_TemporaryHelpingVariable1 = (static_cast<float32>(this->rect().left()) + mf32_BarMarginLeft) -
                                      (static_cast<float32>(this->rect().width()) * 0.01F);
      c_MinRect.setRight(static_cast<sintn>(f32_TemporaryHelpingVariable1));
      f32_TemporaryHelpingVariable1 = (static_cast<float32>(this->rect().right()) - mf32_BarMarginRight) +
                                      (static_cast<float32>(this->rect().width()) * 0.01F);
      c_MaxRect.setLeft(static_cast<sintn>(f32_TemporaryHelpingVariable1));
      f32_TemporaryHelpingVariable1 = static_cast<float32>(this->rect().top()) + mf32_BarMarginTop;
      c_MaxRect.setTop(static_cast<sintn>(f32_TemporaryHelpingVariable1));
      f32_TemporaryHelpingVariable1 = static_cast<float32>(this->rect().bottom()) - mf32_BarMarginBottom;
      c_MaxRect.setBottom(static_cast<sintn>(f32_TemporaryHelpingVariable1));
      c_MaxRect.setRight(this->rect().right());
      //-------------------------------------------------------------------------
      // Special aligment and text size if this->rect gets to small -------------
      // Text --------------------------------------
      if ((static_cast<float32>(this->rect().height()) * 0.4F) < 15.0F)
      {
         c_TextFont.setPixelSize(15);
      }
      else
      {
         f32_TemporaryHelpingVariable1 = static_cast<float32>(c_MinRect.height()) * 0.95F;
         c_TextFont.setPixelSize(static_cast<sintn>(f32_TemporaryHelpingVariable1)); // Nomal text Size
      }
      c_Painter.setFont(c_TextFont);
      c_TextFontMetric = QFontMetrics(c_TextFont);
      c_Painter.setPen(c_MinMaxPen);
      // -------------------------------------------
      // Min and Max rect size and draw Text ---------------------
      if (static_cast<float32>(c_MinRect.height()) < 15.0F)
      {
         if (this->me_Alignment == stw_opensyde_gui_logic::C_PuiSvDbProgressBar::eTOP)
         {
            f32_TemporaryHelpingVariable1 = (static_cast<float32>(this->rect().bottom()) - mf32_BarMarginBottom) -
                                            17.0F;
            c_MinRect.setTop(static_cast<sintn>(f32_TemporaryHelpingVariable1));
            f32_TemporaryHelpingVariable1 = static_cast<float32>(this->rect().bottom()) + 3.0F;
            c_MinRect.setBottom(static_cast<sintn>(f32_TemporaryHelpingVariable1));
            if (mq_ShowMinMax)
            {
               c_Painter.drawText(c_MinRect, static_cast<sintn>(Qt::AlignRight | Qt::AlignBottom),
                                  this->mc_MinValue + this->mc_Unit);
            }
         }
         else
         {
            f32_TemporaryHelpingVariable1 = static_cast<float32>(this->rect().top()) - 3.0F;
            c_MinRect.setTop(static_cast<sintn>(f32_TemporaryHelpingVariable1));
            f32_TemporaryHelpingVariable1 = static_cast<float32>(this->rect().top()) + mf32_BarMarginTop + 17.0F;
            c_MinRect.setBottom(static_cast<sintn>(f32_TemporaryHelpingVariable1));
            if (mq_ShowMinMax)
            {
               c_Painter.drawText(c_MinRect, static_cast<sintn>(Qt::AlignRight | Qt::AlignTop),
                                  this->mc_MinValue + this->mc_Unit);
            }
         }
      }
      else
      {
         if (mq_ShowMinMax)
         {
            c_Painter.drawText(c_MinRect, static_cast<sintn>(Qt::AlignRight | Qt::AlignVCenter),
                               this->mc_MinValue + this->mc_Unit);
         }
      }
      if (static_cast<float32>(c_MaxRect.height()) < 15.0F)
      {
         if (this->me_Alignment == stw_opensyde_gui_logic::C_PuiSvDbProgressBar::eTOP)
         {
            f32_TemporaryHelpingVariable1 = static_cast<float32>(this->rect().bottom()) + 3.0F;
            c_MaxRect.setBottom(static_cast<sintn>(f32_TemporaryHelpingVariable1));
            f32_TemporaryHelpingVariable1 = (static_cast<float32>(this->rect().bottom()) - mf32_BarMarginBottom) -
                                            17.0F;
            c_MaxRect.setTop(static_cast<sintn>(f32_TemporaryHelpingVariable1));
            if (mq_ShowMinMax)
            {
               c_Painter.drawText(c_MaxRect, static_cast<sintn>(Qt::AlignLeft  | Qt::AlignBottom),
                                  this->mc_MaxValue + this->mc_Unit);
            }
         }
         else
         {
            f32_TemporaryHelpingVariable1 = static_cast<float32>(this->rect().top()) - 3.0F;
            c_MaxRect.setTop(static_cast<sintn>(f32_TemporaryHelpingVariable1));
            f32_TemporaryHelpingVariable1 = static_cast<float32>(this->rect().top()) + mf32_BarMarginTop + 17.0F;
            c_MaxRect.setBottom(static_cast<sintn>(f32_TemporaryHelpingVariable1));
            if (mq_ShowMinMax)
            {
               c_Painter.drawText(c_MaxRect, static_cast<sintn>(Qt::AlignLeft  | Qt::AlignTop),
                                  this->mc_MaxValue + this->mc_Unit);
            }
         }
      }
      else
      {
         if (mq_ShowMinMax)
         {
            c_Painter.drawText(c_MaxRect, static_cast<sintn>(Qt::AlignLeft  | Qt::AlignVCenter),
                               this->mc_MaxValue + this->mc_Unit);
         }
      }
      //--------------------------------------------
      //----------------------------------------------------------------------------------------------------------

      // Value Rect -----------------------------------------------------------------------------------------------
      // Left and Right depending on the actual value ----------------------------------
      f32_TemporaryHelpingVariable1 = ((static_cast<float32>(this->rect().left()) + mf32_BarMarginLeft) +
                                       ((((static_cast<float32>(this->rect().width()) - mf32_BarMarginLeft) -
                                          mf32_BarMarginRight) * f32_Value) /
                                        (static_cast<float32>(this->mpc_Ui->pc_ProgressBar->maximum()) -
                                         static_cast<float32>(this->mpc_Ui->pc_ProgressBar->minimum())))) -
                                      (static_cast<float32>(this->rect().width()));
      c_ValueRect.setLeft(static_cast<sintn>(f32_TemporaryHelpingVariable1));
      f32_TemporaryHelpingVariable1 = ((static_cast<float32>(this->rect().left()) + mf32_BarMarginLeft) +
                                       ((((static_cast<float32>(this->rect().width()) - mf32_BarMarginLeft) -
                                          mf32_BarMarginRight) * f32_Value) /
                                        (static_cast<float32>(this->mpc_Ui->pc_ProgressBar->maximum()) -
                                         static_cast<float32>(this->mpc_Ui->pc_ProgressBar->minimum())))) +
                                      (static_cast<float32>(this->rect().width()));
      c_ValueRect.setRight(static_cast<sintn>(f32_TemporaryHelpingVariable1));
      // --------------------------------------------------------------------------------

      c_Painter.setPen(c_ValuePen);
      switch (this->me_Alignment)
      {
      case stw_opensyde_gui_logic::C_PuiSvDbProgressBar::eTOP:
         c_ValueRect.setTop(this->rect().top());
         f32_TemporaryHelpingVariable1 = (static_cast<float32>(this->rect().top()) + mf32_BarMarginTop) -
                                         (static_cast<float32>(this->rect().height()) * 0.07F);
         c_ValueRect.setBottom(static_cast<sintn>(f32_TemporaryHelpingVariable1));
         // TopLeft
         f32_TemporaryHelpingVariable1 =
            (static_cast<float32>(c_ValueRect.left()) -
             (static_cast<float32>(c_TextFontMetric.width("8")) / f32_ArrowSizeH)) +
            static_cast<float32>(this->rect().width());
         f32_TemporaryHelpingVariable2 = ((static_cast<float32>(this->rect().top()) + mf32_BarMarginTop) -
                                          (static_cast<float32>(c_TextFontMetric.width("8")) / f32_ArrowSizeH)) -
                                         (static_cast<float32>(this->rect().height()) * 0.05F);
         ac_ArrowPoints[0] =
            QPointF(static_cast<float64>(f32_TemporaryHelpingVariable1),
                    static_cast<float64>(f32_TemporaryHelpingVariable2));
         // TopRight
         f32_TemporaryHelpingVariable1 =
            (static_cast<float32>(c_ValueRect.left()) +
             (static_cast<float32>(c_TextFontMetric.width("8")) / f32_ArrowSizeH)) +
            static_cast<float32>(this->rect().width());
         f32_TemporaryHelpingVariable2 = ((static_cast<float32>(this->rect().top()) + mf32_BarMarginTop) -
                                          (static_cast<float32>(c_TextFontMetric.width("8")) / f32_ArrowSizeH)) -
                                         (static_cast<float32>(this->rect().height()) * 0.05F);
         ac_ArrowPoints[1] =
            QPointF(static_cast<float64>(f32_TemporaryHelpingVariable1),
                    static_cast<float64>(f32_TemporaryHelpingVariable2));
         // Bottom
         f32_TemporaryHelpingVariable1 = static_cast<float32>(c_ValueRect.left()) +
                                         static_cast<float32>(this->rect().width());
         f32_TemporaryHelpingVariable2 = (static_cast<float32>(this->rect().top()) + mf32_BarMarginTop) -
                                         (static_cast<float32>(this->rect().height()) * 0.05F);
         ac_ArrowPoints[2] =
            QPointF(static_cast<float64>(f32_TemporaryHelpingVariable1),
                    static_cast<float64>(f32_TemporaryHelpingVariable2));
         break;
      case stw_opensyde_gui_logic::C_PuiSvDbProgressBar::eBOTTOM:
         f32_TemporaryHelpingVariable1 = (static_cast<float32>(this->rect().bottom()) - mf32_BarMarginBottom) +
                                         (static_cast<float32>(this->rect().height()) * 0.05F);
         c_ValueRect.setTop(static_cast<sintn>(f32_TemporaryHelpingVariable1));
         c_ValueRect.setBottom(this->rect().bottom());
         // BottomLeft
         //x position
         f32_TemporaryHelpingVariable1 = (static_cast<float32>(c_ValueRect.left()) -
                                          (static_cast<float32>(c_TextFontMetric.width("8")) / f32_ArrowSizeH)) +
                                         static_cast<float32>(this->rect().width());
         //y position
         f32_TemporaryHelpingVariable2 = ((static_cast<float32>(this->rect().bottom()) - mf32_BarMarginBottom) +
                                          (static_cast<float32>(this->rect().height()) * 0.05F)) +
                                         (static_cast<float32>(c_TextFontMetric.width("8")) / f32_ArrowSizeH);
         ac_ArrowPoints[0] =
            QPointF(static_cast<float64>(f32_TemporaryHelpingVariable1),
                    static_cast<float64>(f32_TemporaryHelpingVariable2));
         // BottomRight
         //x position
         f32_TemporaryHelpingVariable1 = (static_cast<float32>(c_ValueRect.left()) +
                                          (static_cast<float32>(c_TextFontMetric.width("8")) / f32_ArrowSizeH)) +
                                         static_cast<float32>(this->rect().width());
         //y position
         f32_TemporaryHelpingVariable2 = ((static_cast<float32>(this->rect().bottom()) - mf32_BarMarginBottom) +
                                          (static_cast<float32>(this->rect().height()) * 0.05F)) +
                                         (static_cast<float32>(c_TextFontMetric.width("8")) / f32_ArrowSizeH);
         ac_ArrowPoints[1] =
            QPointF(static_cast<float64>(f32_TemporaryHelpingVariable1),
                    static_cast<float64>(f32_TemporaryHelpingVariable2));
         // Top
         //x position
         f32_TemporaryHelpingVariable1 = static_cast<float32>(c_ValueRect.left()) +
                                         static_cast<float32>(this->rect().width());
         //y position
         f32_TemporaryHelpingVariable2 = (static_cast<float32>(this->rect().bottom()) - mf32_BarMarginBottom) +
                                         (static_cast<float32>(this->rect().height()) * 0.05F);
         ac_ArrowPoints[2] =
            QPointF(static_cast<float64>(f32_TemporaryHelpingVariable1),
                    static_cast<float64>(f32_TemporaryHelpingVariable2));
         break;
      default:
         break;
      }

      // Draw arrow
      mh_DrawArrow(c_Painter, ac_ArrowPoints[0], ac_ArrowPoints[1], ac_ArrowPoints[2], c_ArrowPen);
      //ME: Adapt left position if necessary
      f32_TemporaryHelpingVariable2 = static_cast<float32>(c_TextFontMetric.width(mc_Value + this->mc_Unit));
      if (c_ValueRect.left() < (-this->rect().width() + static_cast<sintn>(f32_TemporaryHelpingVariable2)))
      {
         c_ValueRect.setLeft(-this->rect().width() + static_cast<sintn>(f32_TemporaryHelpingVariable2));
      }

      // Draw value
      c_Painter.drawText(c_ValueRect.adjusted(static_cast<sintn>(f32_TemporaryHelpingVariable2), 0,
                                              -static_cast<sintn>(f32_TemporaryHelpingVariable2),
                                              0), static_cast<sintn>(Qt::AlignVCenter  | Qt::AlignHCenter),
                         mc_Value + this->mc_Unit);
      // ---------------------------------------------------------------------------------------------------------
      break;

   //--
   //--
   //--
   //--

   case stw_opensyde_gui_logic::C_PuiSvDbProgressBar::eTYPE_2:
      // Setting up the Rects for Min and Max ---------------------------------------------------------------------
      // Normal if this->rect is high enought -----------------------------------
      f32_TemporaryHelpingVariable1 = static_cast<float32>(this->rect().left()) + mf32_BarMarginLeft;
      c_MaxRect.setLeft(static_cast<sintn>(f32_TemporaryHelpingVariable1));
      c_MaxRect.setTop(this->rect().top());
      f32_TemporaryHelpingVariable1 = (static_cast<float32>(this->rect().top()) + mf32_BarMarginTop) - 2.0F;
      //(static_cast<float32>(this->rect().height()) * 0.01F);
      c_MaxRect.setBottom(static_cast<sintn>(f32_TemporaryHelpingVariable1));
      f32_TemporaryHelpingVariable1 = static_cast<float32>(this->rect().right()) - mf32_BarMarginRight;
      c_MaxRect.setRight(static_cast<sintn>(f32_TemporaryHelpingVariable1));

      f32_TemporaryHelpingVariable1 = static_cast<float32>(this->rect().left()) + mf32_BarMarginLeft;
      c_MinRect.setLeft(static_cast<sintn>(f32_TemporaryHelpingVariable1));
      f32_TemporaryHelpingVariable1 = (static_cast<float32>(this->rect().bottom()) - mf32_BarMarginBottom) + 2.0F;
      //(static_cast<float32>(this->rect().height()) * 0.01F);
      c_MinRect.setTop(static_cast<sintn>(f32_TemporaryHelpingVariable1));
      c_MinRect.setBottom(this->rect().bottom());
      f32_TemporaryHelpingVariable1 = static_cast<float32>(this->rect().right()) - mf32_BarMarginRight;
      c_MinRect.setRight(static_cast<sintn>(f32_TemporaryHelpingVariable1));

      if ((static_cast<float32>(this->rect().height()) * 0.4F) < 15.0F)
      {
         c_TextFont.setPixelSize(15);
      }
      else
      {
         f32_TemporaryHelpingVariable1 = static_cast<float32>(c_MinRect.height()) * 0.9F;
         c_TextFont.setPixelSize(static_cast<sintn>(f32_TemporaryHelpingVariable1));
      }
      c_Painter.setFont(c_TextFont);
      c_TextFontMetric = QFontMetrics(c_TextFont);
      c_Painter.setPen(c_MinMaxPen);

      if ((c_TextFontMetric.width(mc_MaxValue) + c_TextFontMetric.width(mc_Unit)) > c_MaxRect.width())
      {
         if (me_Alignment == stw_opensyde_gui_logic::C_PuiSvDbProgressBar::eLEFT)
         {
            c_MaxRect.setLeft(this->rect().left());
            if (mq_ShowMinMax)
            {
               c_Painter.drawText(c_MaxRect, static_cast<sintn>(Qt::AlignBottom | Qt::AlignRight),
                                  this->mc_MaxValue + this->mc_Unit);
            }
         }
         else
         {
            c_MaxRect.setRight(this->rect().right());
            if (mq_ShowMinMax)
            {
               c_Painter.drawText(c_MaxRect, static_cast<sintn>(Qt::AlignBottom | Qt::AlignLeft),
                                  this->mc_MaxValue + this->mc_Unit);
            }
         }
      }
      else
      {
         if (mq_ShowMinMax)
         {
            c_Painter.drawText(c_MaxRect, static_cast<sintn>(Qt::AlignBottom | Qt::AlignHCenter),
                               this->mc_MaxValue + this->mc_Unit);
         }
      }

      if ((c_TextFontMetric.width(mc_MinValue) + c_TextFontMetric.width(mc_Unit)) > c_MinRect.width())
      {
         if (me_Alignment == stw_opensyde_gui_logic::C_PuiSvDbProgressBar::eLEFT)
         {
            c_MinRect.setLeft(this->rect().left());
            if (mq_ShowMinMax)
            {
               c_Painter.drawText(c_MinRect, static_cast<sintn>(Qt::AlignTop  | Qt::AlignRight),
                                  this->mc_MinValue + this->mc_Unit);
            }
         }
         else
         {
            c_MinRect.setRight(this->rect().right());
            if (mq_ShowMinMax)
            {
               c_Painter.drawText(c_MinRect, static_cast<sintn>(Qt::AlignTop  | Qt::AlignLeft),
                                  this->mc_MinValue + this->mc_Unit);
            }
         }
      }
      else
      {
         if (mq_ShowMinMax)
         {
            c_Painter.drawText(c_MinRect, static_cast<sintn>(Qt::AlignTop  | Qt::AlignHCenter),
                               this->mc_MinValue + this->mc_Unit);
         }
      }

      // Value Rect -----------------------------------------------------------------------------------------------
      f32_TemporaryHelpingVariable1 = ((static_cast<float32>(this->rect().bottom()) - mf32_BarMarginBottom) -
                                       ((((static_cast<float32>(this->rect().height()) - mf32_BarMarginTop) -
                                          mf32_BarMarginBottom) * f32_Value) /
                                        (static_cast<float32>(this->mpc_Ui->pc_ProgressBar->maximum()) -
                                         static_cast<float32>(this->mpc_Ui->pc_ProgressBar->minimum())))) -
                                      (((static_cast<float32>(this->rect().height()) - mf32_BarMarginTop) -
                                        mf32_BarMarginBottom) * (2.0F / 3.0F));

      c_ValueRect.setTop(static_cast<sintn>(f32_TemporaryHelpingVariable1));
      f32_TemporaryHelpingVariable1 = ((static_cast<float32>(this->rect().bottom()) - mf32_BarMarginBottom) -
                                       ((((static_cast<float32>(this->rect().height()) - mf32_BarMarginTop) -
                                          mf32_BarMarginBottom) * f32_Value) /
                                        (static_cast<float32>(this->mpc_Ui->pc_ProgressBar->maximum()) -
                                         static_cast<float32>(this->mpc_Ui->pc_ProgressBar->minimum())))) +
                                      (((static_cast<float32>(this->rect().height()) - mf32_BarMarginTop) -
                                        mf32_BarMarginBottom) * (2.0F / 3.0F));
      c_ValueRect.setBottom(static_cast<sintn>(f32_TemporaryHelpingVariable1));

      c_Painter.setPen(c_ValuePen);
      switch (this->me_Alignment)
      {
      case stw_opensyde_gui_logic::C_PuiSvDbProgressBar::eLEFT:
         c_ValueRect.setLeft(this->rect().left());
         //         f32_TemporaryHelpingVariable1 = (static_cast<float32>(this->rect().left()) + mf32_BarMarginLeft) -
         //                                         (static_cast<float32>(this->rect().width()) * 0.07F);
         //         c_ValueRect.setRight(static_cast<sintn>(f32_TemporaryHelpingVariable1));
         if ((static_cast<float32>(this->rect().height()) * 0.2F) < 15.0F)
         {
            c_ValueRect.setTop(c_ValueRect.top() - this->rect().height());
            c_ValueRect.setBottom(c_ValueRect.bottom() + this->rect().height());
            c_TextFont.setPixelSize(15);
         }
         else
         {
            f32_TemporaryHelpingVariable1 = static_cast<float32>(c_MinRect.height()) * 0.9F;
            c_TextFont.setPixelSize(static_cast<sintn>(f32_TemporaryHelpingVariable1));
         }
         f32_TemporaryHelpingVariable1 = (((static_cast<float32>(this->rect().left()) + mf32_BarMarginLeft) -
                                           (static_cast<float32>(c_TextFont.pixelSize()) / f32_ArrowSizeV)) -
                                          (static_cast<float32>(this->rect().width()) * 0.005F)) - 5.0F;
         c_ValueRect.setRight(static_cast<sintn>(f32_TemporaryHelpingVariable1));
         c_Painter.setFont(c_TextFont);
         c_Painter.drawText(c_ValueRect, Qt::AlignVCenter | Qt::AlignRight, mc_Value + this->mc_Unit);
         // TopLeft
         f32_TemporaryHelpingVariable1 = ((static_cast<float32>(this->rect().left()) + mf32_BarMarginLeft) -
                                          (static_cast<float32>(c_TextFont.pixelSize()) / f32_ArrowSizeV)) -
                                         (static_cast<float32>(this->rect().width()) * 0.005F);
         f32_TemporaryHelpingVariable2 = ((static_cast<float32>(this->rect().bottom()) - mf32_BarMarginBottom) -
                                          ((((static_cast<float32>(this->rect().height()) - mf32_BarMarginTop) -
                                             mf32_BarMarginBottom) *
                                            f32_Value) /
                                           (static_cast<float32>(this->mpc_Ui->pc_ProgressBar->maximum()) -
                                            static_cast<float32>(this->mpc_Ui->pc_ProgressBar->minimum())))) -
                                         (static_cast<float32>(c_TextFont.pixelSize()) /
                                          f32_ArrowSizeV);
         ac_ArrowPoints[0] =
            QPointF(static_cast<float64>(f32_TemporaryHelpingVariable1),
                    static_cast<float64>(f32_TemporaryHelpingVariable2));
         // BottomLeft
         // x position same as TopLeft
         f32_TemporaryHelpingVariable2 = ((static_cast<float32>(this->rect().bottom()) - mf32_BarMarginBottom) -
                                          ((((static_cast<float32>(this->rect().height()) - mf32_BarMarginTop) -
                                             mf32_BarMarginBottom) *
                                            f32_Value) /
                                           (static_cast<float32>(this->mpc_Ui->pc_ProgressBar->maximum()) -
                                            static_cast<float32>(this->mpc_Ui->pc_ProgressBar->minimum())))) +
                                         (static_cast<float32>(c_TextFont.pixelSize()) /
                                          f32_ArrowSizeV);
         ac_ArrowPoints[1] =
            QPointF(static_cast<float64>(f32_TemporaryHelpingVariable1),
                    static_cast<float64>(f32_TemporaryHelpingVariable2));
         // Right
         f32_TemporaryHelpingVariable1 = (static_cast<float32>(this->rect().left()) + mf32_BarMarginLeft) -
                                         (static_cast<float32>(this->rect().width()) * 0.005F);
         f32_TemporaryHelpingVariable2 = (static_cast<float32>(this->rect().bottom()) - mf32_BarMarginBottom) -
                                         ((((static_cast<float32>(this->rect().height()) - mf32_BarMarginTop) -
                                            mf32_BarMarginBottom) *
                                           f32_Value) /
                                          (static_cast<float32>(this->mpc_Ui->pc_ProgressBar->maximum()) -
                                           static_cast<float32>(this->mpc_Ui->pc_ProgressBar->minimum())));
         ac_ArrowPoints[2] =
            QPointF(static_cast<float64>(f32_TemporaryHelpingVariable1),
                    static_cast<float64>(f32_TemporaryHelpingVariable2));
         break;
      case stw_opensyde_gui_logic::C_PuiSvDbProgressBar::eRIGHT:
         c_ValueRect.setRight(this->rect().right());
         //         f32_TemporaryHelpingVariable1 = (static_cast<float32>(this->rect().right()) - mf32_BarMarginRight) +
         //                                         (static_cast<float32>(this->rect().width()) * 0.05F);
         //         c_ValueRect.setLeft(static_cast<sintn>(f32_TemporaryHelpingVariable1));
         if ((static_cast<float32>(this->rect().height()) * 0.2F) < 15.0F)
         {
            c_ValueRect.setTop(c_ValueRect.top() - this->rect().height());
            c_ValueRect.setBottom(c_ValueRect.bottom() + this->rect().height());
            c_TextFont.setPixelSize(15);
         }
         else
         {
            f32_TemporaryHelpingVariable1 = static_cast<float32>(c_MinRect.height()) * 0.9F;
            c_TextFont.setPixelSize(static_cast<sintn>(f32_TemporaryHelpingVariable1));
         }
         f32_TemporaryHelpingVariable1 = (static_cast<float32>(this->rect().right()) - mf32_BarMarginRight) +
                                         (static_cast<float32>(c_TextFont.pixelSize()) / f32_ArrowSizeV) +
                                         (static_cast<float32>(this->rect().width()) * 0.01F) + 1.5F;
         c_ValueRect.setLeft(static_cast<sintn>(f32_TemporaryHelpingVariable1));
         c_Painter.setFont(c_TextFont);
         c_Painter.drawText(c_ValueRect, Qt::AlignVCenter | Qt::AlignLeft, mc_Value + this->mc_Unit);
         // TopRight
         f32_TemporaryHelpingVariable1 = (static_cast<float32>(this->rect().right()) - mf32_BarMarginRight) +
                                         (static_cast<float32>(c_TextFont.pixelSize()) / f32_ArrowSizeV) +
                                         (static_cast<float32>(this->rect().width()) * 0.01F);
         f32_TemporaryHelpingVariable2 = ((static_cast<float32>(this->rect().bottom()) - mf32_BarMarginBottom) -
                                          ((((static_cast<float32>(this->rect().height()) - mf32_BarMarginTop) -
                                             mf32_BarMarginBottom) * f32_Value) /
                                           (static_cast<float32>(this->mpc_Ui->pc_ProgressBar->maximum()) -
                                            static_cast<float32>(this->mpc_Ui->pc_ProgressBar->minimum())))) -
                                         (static_cast<float32>(c_TextFont.pixelSize()) /
                                          f32_ArrowSizeV);
         ac_ArrowPoints[0] =
            QPointF(static_cast<float64>(f32_TemporaryHelpingVariable1),
                    static_cast<float64>(f32_TemporaryHelpingVariable2));
         // BottomRight
         f32_TemporaryHelpingVariable1 = (static_cast<float32>(this->rect().right()) - mf32_BarMarginRight) +
                                         (static_cast<float32>(c_TextFont.pixelSize()) / f32_ArrowSizeV) +
                                         (static_cast<float32>(this->rect().width()) * 0.01F);
         f32_TemporaryHelpingVariable2 = ((static_cast<float32>(this->rect().bottom()) - mf32_BarMarginBottom) -
                                          (((static_cast<float32>(this->rect().height()) - mf32_BarMarginTop) -
                                            mf32_BarMarginBottom) * (f32_Value /
                                                                     (
                                                                        static_cast<float32>(this->mpc_Ui->
                                                                                             pc_ProgressBar
                                                                                             ->maximum()) -
                                                                        static_cast
                                                                        <float32>(this->mpc_Ui->pc_ProgressBar->minimum())))))
                                         +
                                         (static_cast<float32>(c_TextFont.pixelSize()) / f32_ArrowSizeV);
         ac_ArrowPoints[1] =
            QPointF(static_cast<float64>(f32_TemporaryHelpingVariable1),
                    static_cast<float64>(f32_TemporaryHelpingVariable2));
         // Left
         f32_TemporaryHelpingVariable1 = (static_cast<float32>(this->rect().right()) - mf32_BarMarginRight) +
                                         (static_cast<float32>(this->rect().width()) * 0.01F);
         f32_TemporaryHelpingVariable2 = (static_cast<float32>(this->rect().bottom()) - mf32_BarMarginBottom) -
                                         (((static_cast<float32>(this->rect().height()) - mf32_BarMarginTop) -
                                           mf32_BarMarginBottom) * (f32_Value /
                                                                    (
                                                                       static_cast<float32>(this->mpc_Ui->pc_ProgressBar
                                                                                            ->maximum()) -
                                                                       static_cast
                                                                       <float32>(this->mpc_Ui->pc_ProgressBar->minimum()))));
         ac_ArrowPoints[2] =
            QPointF(static_cast<float64>(f32_TemporaryHelpingVariable1),
                    static_cast<float64>(f32_TemporaryHelpingVariable2));
         break;
      default:
         break;
      }
      // Draw arrow
      mh_DrawArrow(c_Painter, ac_ArrowPoints[0], ac_ArrowPoints[1], ac_ArrowPoints[2], c_ArrowPen);
      break;

   //--
   //--
   //--
   //--
   case stw_opensyde_gui_logic::C_PuiSvDbProgressBar::eTYPE_3:
      // Setting up the Rects for Min and Max ---------------------------------------------------------------------
      // Normal if this->rect is high enought -----------------------------------
      f32_TemporaryHelpingVariable1 = static_cast<float32>(this->rect().left()) + mf32_BarMarginLeft;
      c_MaxRect.setLeft(static_cast<sintn>(f32_TemporaryHelpingVariable1));
      c_MaxRect.setTop(this->rect().top());
      f32_TemporaryHelpingVariable1 = static_cast<float32>(this->rect().top()) +
                                      (mf32_BarMarginTop - (static_cast<float32>(this->rect().height()) * 0.01F));
      c_MaxRect.setBottom(static_cast<sintn>(f32_TemporaryHelpingVariable1));
      f32_TemporaryHelpingVariable1 = static_cast<float32>(this->rect().right()) - mf32_BarMarginRight;
      c_MaxRect.setRight(static_cast<sintn>(f32_TemporaryHelpingVariable1));

      f32_TemporaryHelpingVariable1 = static_cast<float32>(this->rect().left()) + mf32_BarMarginLeft;
      c_MinRect.setLeft(static_cast<sintn>(f32_TemporaryHelpingVariable1));
      f32_TemporaryHelpingVariable1 = (static_cast<float32>(this->rect().bottom()) - mf32_BarMarginBottom) +
                                      (static_cast<float32>(this->rect().height()) * 0.01F);
      c_MinRect.setTop(static_cast<sintn>(f32_TemporaryHelpingVariable1));
      c_MinRect.setBottom(this->rect().bottom());
      f32_TemporaryHelpingVariable1 = static_cast<float32>(this->rect().right()) - mf32_BarMarginRight;
      c_MinRect.setRight(static_cast<sintn>(f32_TemporaryHelpingVariable1));

      if (this->mq_ShowMinMax == true)
      {
         f32_TemporaryHelpingVariable1 = static_cast<float32>(c_MinRect.height()) * 0.95F;
         c_TextFont.setPixelSize(static_cast<sintn>(f32_TemporaryHelpingVariable1));
      }
      c_Painter.setFont(c_TextFont);
      c_TextFontMetric = QFontMetrics(c_TextFont);
      c_Painter.setPen(c_MinMaxPen);

      if ((c_TextFontMetric.width(mc_MaxValue) + c_TextFontMetric.width(mc_Unit)) > c_MaxRect.width())
      {
         if (me_Alignment == stw_opensyde_gui_logic::C_PuiSvDbProgressBar::eLEFT)
         {
            c_MaxRect.setLeft(this->rect().left());
            if (mq_ShowMinMax)
            {
               c_Painter.drawText(c_MaxRect, static_cast<sintn>(Qt::AlignVCenter | Qt::AlignRight),
                                  this->mc_MaxValue + this->mc_Unit);
            }
         }
         else
         {
            c_MaxRect.setRight(this->rect().right());
            if (mq_ShowMinMax)
            {
               c_Painter.drawText(c_MaxRect, static_cast<sintn>(Qt::AlignVCenter | Qt::AlignLeft),
                                  this->mc_MaxValue + this->mc_Unit);
            }
         }
      }
      else
      {
         if (mq_ShowMinMax)
         {
            c_Painter.drawText(c_MaxRect, static_cast<sintn>(Qt::AlignVCenter | Qt::AlignHCenter),
                               this->mc_MaxValue + this->mc_Unit);
         }
      }

      if ((c_TextFontMetric.width(mc_MinValue) + c_TextFontMetric.width(mc_Unit)) > c_MinRect.width())
      {
         if (me_Alignment == stw_opensyde_gui_logic::C_PuiSvDbProgressBar::eLEFT)
         {
            c_MinRect.setLeft(this->rect().left());
            if (mq_ShowMinMax)
            {
               c_Painter.drawText(c_MinRect, static_cast<sintn>(Qt::AlignVCenter  | Qt::AlignRight),
                                  this->mc_MinValue + this->mc_Unit);
            }
         }
         else
         {
            c_MinRect.setRight(this->rect().right());
            if (mq_ShowMinMax)
            {
               c_Painter.drawText(c_MinRect, static_cast<sintn>(Qt::AlignVCenter  | Qt::AlignLeft),
                                  this->mc_MinValue + this->mc_Unit);
            }
         }
      }
      else
      {
         if (mq_ShowMinMax)
         {
            c_Painter.drawText(c_MinRect, static_cast<sintn>(Qt::AlignVCenter  | Qt::AlignHCenter),
                               this->mc_MinValue + this->mc_Unit);
         }
      }

      // Value Rect -----------------------------------------------------------------------------------------------
      f32_TemporaryHelpingVariable1 = static_cast<float32>(this->rect().top()) + mf32_BarMarginTop;
      c_ValueRect.setTop(static_cast<sintn>(f32_TemporaryHelpingVariable1));
      f32_TemporaryHelpingVariable1 = static_cast<float32>(this->rect().bottom()) - mf32_BarMarginBottom;
      c_ValueRect.setBottom(static_cast<sintn>(f32_TemporaryHelpingVariable1));
      if (static_cast<float32>(c_ValueRect.height()) < 16.0F)
      {
         c_ValueRect.setTop(c_ValueRect.top() - this->rect().height());
         c_ValueRect.setBottom(c_ValueRect.bottom() + this->rect().height());
         c_TextFont.setPixelSize(15);
      }
      else
      {
         c_TextFont.setPixelSize(c_ValueRect.height());
      }
      c_Painter.setFont(c_TextFont);
      c_TextFontMetric = QFontMetrics(c_TextFont);
      sn_ActualPixelSize = c_TextFont.pixelSize();

      c_Painter.setPen(c_ValuePen);
      switch (this->me_Alignment)
      {
      case stw_opensyde_gui_logic::C_PuiSvDbProgressBar::eLEFT:
         f32_TemporaryHelpingVariable1 = static_cast<float32>(sn_ActualPixelSize) / 3.0F;
         c_TextFont.setPixelSize(static_cast<sintn>(f32_TemporaryHelpingVariable1));
         c_Painter.setFont(c_TextFont);
         c_TextFontMetric = QFontMetrics(c_TextFont);
         f32_TemporaryHelpingVariable1 = (static_cast<float32>(this->rect().left()) + mf32_BarMarginLeft) - 3.0F;
         //                                         (static_cast<float32>(this->rect().width()) * 0.01F);
         c_ValueRect.setRight(static_cast<sintn>(f32_TemporaryHelpingVariable1));
         c_ValueRect.setLeft(c_ValueRect.right() - c_TextFontMetric.width(mc_Unit));
         c_Painter.drawText(c_ValueRect, Qt::AlignBottom | Qt::AlignLeft, this->mc_Unit);
         c_ValueRect.setRight(c_ValueRect.left());
         c_ValueRect.setLeft(this->rect().left());
         c_TextFont.setPixelSize(sn_ActualPixelSize);
         c_Painter.setFont(c_TextFont);
         c_Painter.drawText(c_ValueRect, Qt::AlignVCenter | Qt::AlignRight, this->mc_Value);
         break;
      case stw_opensyde_gui_logic::C_PuiSvDbProgressBar::eRIGHT:
         f32_TemporaryHelpingVariable1 = (static_cast<float32>(this->rect().right()) - mf32_BarMarginRight) + 3.0F;
         //                                         (static_cast<float32>(this->rect().width()) * 0.01F);
         c_ValueRect.setLeft(static_cast<sintn>(f32_TemporaryHelpingVariable1));
         c_ValueRect.setRight(c_ValueRect.left() + c_TextFontMetric.width(mc_Value));
         c_Painter.drawText(c_ValueRect, Qt::AlignVCenter | Qt::AlignRight, this->mc_Value);
         c_ValueRect.setLeft(c_ValueRect.right());
         c_ValueRect.setRight(this->rect().right());
         f32_TemporaryHelpingVariable1 = static_cast<float32>(sn_ActualPixelSize) / 3.0F;
         c_TextFont.setPixelSize(static_cast<sintn>(f32_TemporaryHelpingVariable1));
         c_Painter.setFont(c_TextFont);
         c_Painter.drawText(c_ValueRect, Qt::AlignBottom | Qt::AlignLeft, this->mc_Unit);
         break;
      default:
         break;
      }
      break;

   default:
      break;
   }
   // Karsten Ende
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Adjust font to current size
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaItDashboardProgressBarWidget::AdjustFontToSize(void)
{
   SetBarMargins();

   this->update();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set value

   \param[in] osn_Value New value
   \param[in] orc_Value New value string
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaItDashboardProgressBarWidget::SetValue(const sintn osn_Value, const QString & orc_Value)
{
   const sintn sn_Min = this->mpc_Ui->pc_ProgressBar->minimum();

   // Karsten Anfang
   if (osn_Value < sn_Min)
   // Necessary becausde osn_Value = -7905 when it should be 0
   // setValue() doesn't take negative values
   {
      // Setting the shown value
      this->mpc_Ui->pc_ProgressBar->setValue(0);
   }
   else
   {
      // Setting the value
      this->mpc_Ui->pc_ProgressBar->setValue(osn_Value);
   }
   // Karsten Ende

   this->mc_Value = orc_Value;
   this->update();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set min

   \param[in] osn_Value New min value
   \param[in] orc_Value New min string
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaItDashboardProgressBarWidget::SetMin(const sintn osn_Value, const QString & orc_Value)
{
   this->mc_MinValue = orc_Value;
   this->mpc_Ui->pc_ProgressBar->setMinimum(osn_Value);
   SetBarMargins();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set max

   \param[in] osn_Value New max value
   \param[in] orc_Value New max string
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaItDashboardProgressBarWidget::SetMax(const sintn osn_Value, const QString & orc_Value)
{
   this->mc_MaxValue = orc_Value;
   this->mpc_Ui->pc_ProgressBar->setMaximum(osn_Value);
   SetBarMargins();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Sets the flag for showing minimum and maximum

   \param[in]     oq_Value       Flag for showing minimum and maximum
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaItDashboardProgressBarWidget::SetShowMinMax(const bool oq_Value)
{
   this->mq_ShowMinMax = oq_Value;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set unit

   \param[in] orc_Value New unit
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaItDashboardProgressBarWidget::SetUnit(const QString & orc_Value)
{
   mc_Unit = orc_Value;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Overwritten resize event slot

   Here: Handle resize specific update

   \param[in,out] opc_Event Event identification and information
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaItDashboardProgressBarWidget::resizeEvent(QResizeEvent * const opc_Event)
{
   QWidget::resizeEvent(opc_Event);

   AdjustFontToSize();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Arrow drawing function

   \param[in,out] orc_Painter Current painter
   \param[in]     orc_P1      Arrow point 1
   \param[in]     orc_P2      Arrow point 2
   \param[in]     orc_P3      Arrow point 3
   \param[in]     orc_Pen     Arrow pen
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaItDashboardProgressBarWidget::mh_DrawArrow(QPainter & orc_Painter, const QPointF & orc_P1,
                                                       const QPointF & orc_P2, const QPointF & orc_P3,
                                                       const QPen & orc_Pen)
{
   QPointF c_P2;
   QPointF c_P3;

   if (C_GiBiLineBounding::h_CrossProduct(orc_P1 - orc_P2, orc_P1 - orc_P3) > 0.0)
   {
      c_P2 = orc_P2;
      c_P3 = orc_P3;
   }
   else
   {
      c_P2 = orc_P3;
      c_P3 = orc_P2;
   }
   {
      QPainterPath c_Path;
      const float64 f64_HalfWidth = orc_Pen.widthF() / 2.0;
      //Rects
      const QPolygonF c_Poly1To3 = mh_GetOuterRect(orc_P1, c_P3, c_P2, f64_HalfWidth);
      const QPolygonF c_Poly3To2 = mh_GetOuterRect(c_P3, c_P2, orc_P1, f64_HalfWidth);
      const QPolygonF c_Poly2To1 = mh_GetOuterRect(c_P2, orc_P1, c_P3, f64_HalfWidth);

      c_Path.setFillRule(Qt::WindingFill);

      //P1 to P3
      c_Path.moveTo(c_Poly1To3.at(3));
      c_Path.lineTo(c_Poly1To3.at(2));
      //Circle P3
      mh_SetArc(c_Path, c_P3, orc_P1, c_P2, f64_HalfWidth);
      //P3 to P2
      c_Path.lineTo(c_Poly3To2.at(2));
      //Circle P2
      mh_SetArc(c_Path, c_P2, orc_P1, c_P3, f64_HalfWidth);
      //P2 to P1
      c_Path.lineTo(c_Poly2To1.at(2));
      //Circle P1
      mh_SetArc(c_Path, orc_P1, c_P3, c_P2, f64_HalfWidth);
      //Close
      c_Path.closeSubpath();

      c_Path.setFillRule(Qt::WindingFill);

      orc_Painter.save();
      orc_Painter.setPen(Qt::NoPen);
      orc_Painter.setRenderHint(QPainter::Antialiasing, true);
      orc_Painter.fillPath(c_Path, orc_Painter.brush());
      orc_Painter.setRenderHint(QPainter::Antialiasing, false);
      orc_Painter.restore();
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get rectangle at border of current line

   \param[in]     orc_P1        First point
   \param[in]     orc_P2        Second point
   \param[in]     orc_POpposite Point opposite of rectangle
   \param[in]     of64_Width    Rectangle width

   \return
   Rectangle at border of current line (output: P1,P2,P4,P3 -> look at diagram)
*/
//----------------------------------------------------------------------------------------------------------------------
QPolygonF C_SyvDaItDashboardProgressBarWidget::mh_GetOuterRect(const QPointF & orc_P1, const QPointF & orc_P2,
                                                               const QPointF & orc_POpposite, const float64 of64_Width)
{
   QPolygonF c_Retval;
   /*
   //                                   P4/ \.
   //                                    /    \.
   //                                   /       \.
   //                                  /          \.
   //                                 /        90° /\P2
   //                                /            /  \
   //                               /            /    \
   //                              /            /      \
   //                             /            /        \
   //                            /            /          \
   //                           /            /            \
   //                          /            /              \
   //                         /            /                \
   //                        /            /                  \
   //                       /            /                    \
   //                      /            /                      \
   //                     /            /                        \
   //                    /            /                          \
   //                   /            /                            \
   //                  /            /                              \
   //                 /            /                                \
   //                /            /                                  \
   //               /            /                                    \
   //              /            /                                      \
   //             /            /                                        \
   //            /            /                                          \
   //           /            /                                            \
   //          /            /                                              \
   //         /            /                                                \
   //        /            /                                                  \
   //     P3/            /                                                    \
   //       \.          /                                                      \
   //         \.       /                                                        \
   //           \.90° /                                                          \
   //             \. /P1                                                          \
   //               \--------------------------------------------------------------
   */
   const QPointF c_P1ToP2 = orc_P2 - orc_P1;
   const QPointF c_Perpendicular = C_GiBiLineBounding::h_GetPerpendicular(c_P1ToP2);
   const float64 f64_LengthP1Perpendicular = std::sqrt(
      (c_Perpendicular.x() * c_Perpendicular.x()) + (c_Perpendicular.y() * c_Perpendicular.y()));
   const QPointF c_PerpendicularToNewPoints = (c_Perpendicular / f64_LengthP1Perpendicular) * of64_Width;
   const QPointF c_P3Plus = orc_POpposite + c_PerpendicularToNewPoints;
   const QPointF c_P3Minus = orc_POpposite - c_PerpendicularToNewPoints;
   float64 f64_Dist1;
   float64 f64_Dist2;

   C_GiBiConnectableItem::h_DistToPoint(orc_P1, c_P3Plus, f64_Dist1);
   C_GiBiConnectableItem::h_DistToPoint(orc_P1, c_P3Minus, f64_Dist2);

   c_Retval.push_back(orc_P1);
   c_Retval.push_back(orc_P2);
   if (f64_Dist1 < f64_Dist2)
   {
      const QPointF c_P4 = orc_P2 + c_PerpendicularToNewPoints;
      const QPointF c_P3 = orc_P1 + c_PerpendicularToNewPoints;
      c_Retval.push_back(c_P4);
      c_Retval.push_back(c_P3);
   }
   else
   {
      const QPointF c_P4 = orc_P2 - c_PerpendicularToNewPoints;
      const QPointF c_P3 = orc_P1 - c_PerpendicularToNewPoints;
      c_Retval.push_back(c_P4);
      c_Retval.push_back(c_P3);
   }
   return c_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get circle around current point

   \param[in] orc_P1     Current point
   \param[in] of64_Width Circle radius

   \return
   Rectangle for circle
*/
//----------------------------------------------------------------------------------------------------------------------
QRectF C_SyvDaItDashboardProgressBarWidget::mh_GetCircle(const QPointF & orc_P1, const float64 of64_Width)
{
   return QRectF(orc_P1.x() - of64_Width, orc_P1.y() - of64_Width, of64_Width * 2.0, of64_Width * 2.0);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Apply arc

   \param[in,out] orc_Path    Current path
   \param[in]     orc_PCenter Arc center
   \param[in]     orc_P2      Other triangle point 1
   \param[in]     orc_P3      Other triangle point 2
   \param[in]     of64_Radius Circle radius
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaItDashboardProgressBarWidget::mh_SetArc(QPainterPath & orc_Path, const QPointF & orc_PCenter,
                                                    const QPointF & orc_P2, const QPointF & orc_P3,
                                                    const float64 of64_Radius)
{
   const float64 f64_Full360 = 2.0 * M_PI;
   QPointF c_P1;
   QPointF c_P2;

   if (C_GiBiLineBounding::h_CrossProduct(orc_PCenter - orc_P2, orc_PCenter - orc_P3) > 0.0)
   {
      c_P1 = orc_P2;
      c_P2 = orc_P3;
   }
   else
   {
      c_P1 = orc_P3;
      c_P2 = orc_P2;
   }
   {
      /*
      //                       \.   sweep      ./
      //                         \.----------./\
      //                           \.      ./   \Start angle
      //                             \.  ./      \
      //                           90° /\ 90° ------------
      //                              /^ \
      //                             / |  \
      //                            /  |   \
      //                           /PCenter \
      //                          /          \
      //                         /            \
      //                        /              \
      //                       /                \
      //                      /                  \
      //                     /                    \
      //                    /                      \
      //                   /                        \
      //                  /                          \
      //                 /                            \
      //                /                              \
      //               /                                \
      //              /                                  \
      //             /                                    \
      //            /                                      \
      //           /                                        \
      //          /                                          \
      //         /Alpha                                       \Beta
      //        /-----------------------                       \----------------------------------
      //       /                                                \
      //      /                                                  \
      //     /                                                    \
      //    /                                                      \
      //   /                                                        \
      //  /                                                          \
      // /                                                            \
      // --------------------------------------------------------------
      // P2                                                           P1
      */
      const float64 f64_Beta = mh_GetArc(orc_PCenter - c_P1, QPointF(1.0, 0.0));
      const float64 f64_Alpha = mh_GetArc(orc_PCenter - c_P2, QPointF(1.0, 0.0));
      const float64 f64_Start = f64_Beta - (f64_Full360 / 4.0);
      const float64 f64_Sweep =  ((f64_Full360 / 2.0) + f64_Alpha) - f64_Beta;

      orc_Path.arcTo(mh_GetCircle(orc_PCenter,
                                  of64_Radius), f64_Start * (360.0 / f64_Full360), f64_Sweep * (360.0 / f64_Full360));
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get angle between two vectors

   \param[in] orc_V1 First vector
   \param[in] orc_V2 Second vector

   \return
   Angle between vectors
*/
//----------------------------------------------------------------------------------------------------------------------
float64 C_SyvDaItDashboardProgressBarWidget::mh_GetArc(const QPointF & orc_V1, const QPointF & orc_V2)
{
   const float64 f64_DotProduct = (orc_V1.x() * orc_V2.x()) + (orc_V1.y() * orc_V2.y());
   const float64 f64_Determinant = (orc_V1.x() * orc_V2.y()) - (orc_V1.y() * orc_V2.x());
   const float64 f64_Angle = std::atan2(f64_Determinant, f64_DotProduct);

   return f64_Angle;
}

//----------------------------------------------------------------------------------------------------------------------
// Karsten Anfang
void C_SyvDaItDashboardProgressBarWidget::SetLabelColor(QPainter & orc_Painter, QFont & orc_TextFont,
                                                        QPen & orc_ArrowPen, QPen & orc_MinMaxPen,
                                                        QPen & orc_ValuePen) const
{
   QColor c_HelpingColorVariable;

   orc_TextFont.setFamily("Segoe UI");
   if (mq_DarkMode)
   {
      c_HelpingColorVariable = mc_STYLE_GUIDE_COLOR_0;
      c_HelpingColorVariable.setAlpha(msn_Transparence); // Color for Value hast to get more transparent some times
      orc_MinMaxPen.setColor(mc_STYLE_GUIDE_COLOR_0);
      orc_ValuePen.setColor(c_HelpingColorVariable);
      orc_Painter.setBrush(QBrush(c_HelpingColorVariable)); // Arrow inside
      orc_ArrowPen.setColor(c_HelpingColorVariable);        // Arrow Rim
   }
   else
   {
      switch (me_Style)
      {
      case stw_opensyde_gui_logic::C_PuiSvDbWidgetBase::eOPENSYDE_2:
         c_HelpingColorVariable = mc_STYLE_GUIDE_COLOR_6;
         c_HelpingColorVariable.setAlpha(msn_Transparence);
         orc_MinMaxPen.setColor(mc_STYLE_GUIDE_COLOR_6);
         orc_ValuePen.setColor(c_HelpingColorVariable);
         orc_Painter.setBrush(QBrush(c_HelpingColorVariable));
         orc_ArrowPen.setColor(c_HelpingColorVariable);
         break;
      default:
         c_HelpingColorVariable = mc_STYLE_GUIDE_COLOR_34;
         c_HelpingColorVariable.setAlpha(msn_Transparence);
         orc_MinMaxPen.setColor(mc_STYLE_GUIDE_COLOR_34);
         orc_ValuePen.setColor(c_HelpingColorVariable);
         orc_Painter.setBrush(QBrush(c_HelpingColorVariable));
         orc_ArrowPen.setColor(c_HelpingColorVariable);
         break;
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------

void C_SyvDaItDashboardProgressBarWidget::SetBarMargins()
{
   QFont c_TextFont;
   QFontMetrics c_TextFontMetric(c_TextFont);
   float32 f32_TemporaryHelpingVariable1;

   switch (this->me_Type)
   {
   case stw_opensyde_gui_logic::C_PuiSvDbProgressBar::eTYPE_1:
      // This ensures, that the values can always be displayd correctly
      // ----------------------------------------------------------------------------------------------------------
      if ((static_cast<float32>(this->rect().height()) * 0.4F) < 15.0F) //0.4F for same size with Bar later on
      {
         // 15.0F good readable size (defined by try and error)
         c_TextFont.setPixelSize(15);
      }
      else
      {
         f32_TemporaryHelpingVariable1 = static_cast<float32>(this->rect().height()) * 0.4F;
         c_TextFont.setPixelSize(static_cast<sintn>(f32_TemporaryHelpingVariable1));
      }
      c_TextFontMetric = QFontMetrics(c_TextFont);
      // Variables to marge the ProgressBar dependent on the size of the numbers
      // Margin for Top Aligment of the Text -------------------------------------------------------
      if (this->me_Alignment == stw_opensyde_gui_logic::C_PuiSvDbProgressBar::eTOP)
      {
         if ((static_cast<float32>(this->rect().height()) * 0.4F) < 15.0F)
         {
            mf32_BarMarginTop = 20.0F;
         }
         else
         {
            mf32_BarMarginTop = static_cast<float32>(this->rect().height()) * 0.6F;
         }
         mf32_BarMarginBottom = 0.0F;
      } //------------------------------------------------------------------------------------------
        // Margin vor Bottom Aligment of the Text ----------------------------------------------------
      else
      {
         mf32_BarMarginTop = 0.0F;
         if ((static_cast<float32>(this->rect().height()) * 0.4F) < 15.0F)
         {
            mf32_BarMarginBottom = 20.0F;
         }
         else
         {
            mf32_BarMarginBottom = static_cast<float32>(this->rect().height()) * 0.6F;
         }
         mf32_BarMarginTop = 0.0F;
      } //-----------------------------------------------------------------------------------------
      mf32_BarMarginLeft = static_cast<float32>(c_TextFontMetric.width(mc_MinValue)) +
                           static_cast<float32>(c_TextFontMetric.width(mc_Unit)) +
                           (0.01F * static_cast<float32>(this->rect().width()));
      mf32_BarMarginRight = static_cast<float32>(c_TextFontMetric.width(mc_MaxValue)) +
                            static_cast<float32>(c_TextFontMetric.width(mc_Unit)) +
                            (0.01F * static_cast<float32>(this->rect().width()));
      // ----------------------------------------------------------------------------------------------------------
      this->mpc_Ui->horizontalLayout->setContentsMargins(static_cast<sintn>(mf32_BarMarginLeft),
                                                         static_cast<sintn>(mf32_BarMarginTop),
                                                         static_cast<sintn>(mf32_BarMarginRight),
                                                         static_cast<sintn>(mf32_BarMarginBottom));
      // ---------------------------------------------------------------------------------------------------------
      break;

   //--
   //--
   //--
   //--

   case stw_opensyde_gui_logic::C_PuiSvDbProgressBar::eTYPE_2:
      // Variables to marge the ProgressBar dependent on the size of the rect
      if ((static_cast<float32>(this->rect().height()) * 0.2F) < 15.0F)
      {
         c_TextFont.setPixelSize(16);
      }
      else
      {
         f32_TemporaryHelpingVariable1 = static_cast<float32>(this->rect().height()) * 0.3F * 0.9F;
         c_TextFont.setPixelSize(static_cast<sintn>(f32_TemporaryHelpingVariable1));
      }
      c_TextFontMetric = QFontMetrics(c_TextFont);
      // This ensures, that the values can always be displayd correctly
      // ----------------------------------------------------------------------------------------------------------
      // Setting the margins depending on the widest min or max
      if (this->me_Alignment == stw_opensyde_gui_logic::C_PuiSvDbProgressBar::eLEFT)
      {
         if (c_TextFontMetric.width(mc_MaxValue) > c_TextFontMetric.width(mc_MinValue))
         {
            mf32_BarMarginLeft = (static_cast<float32>(c_TextFontMetric.width(mc_MaxValue)) +
                                  (static_cast<float32>(c_TextFontMetric.width(mc_Unit)) / 1.5F)) + 1.0F;
            //                                 (static_cast<float32>(this->rect().width()) * 0.01F);
         }
         else
         {
            mf32_BarMarginLeft = (static_cast<float32>(c_TextFontMetric.width(mc_MinValue)) +
                                  (static_cast<float32>(c_TextFontMetric.width(mc_Unit)) / 1.5F)) + 1.0F;
            //                                 (static_cast<float32>(this->rect().width()) * 0.01F);
         }
         mf32_BarMarginRight = 0.0F;
      }
      else
      {
         mf32_BarMarginLeft = 0.0F;
         if (c_TextFontMetric.width(mc_MaxValue) > c_TextFontMetric.width(mc_MinValue))
         {
            mf32_BarMarginRight = (static_cast<float32>(c_TextFontMetric.width(mc_MaxValue)) +
                                   (static_cast<float32>(c_TextFontMetric.width(mc_Unit)) / 1.5F)) + 1.0F;
            //                                  (static_cast<float32>(this->rect().width()) * 0.01F);
         }
         else
         {
            mf32_BarMarginRight = (static_cast<float32>(c_TextFontMetric.width(mc_MinValue)) +
                                   (static_cast<float32>(c_TextFontMetric.width(mc_Unit)) / 1.5F)) + 1.0F;
            //                                  (static_cast<float32>(this->rect().width()) * 0.01F);
         }
      }

      if ((static_cast<float32>(this->rect().height()) * 0.2F) < 15.0F)
      {
         mf32_BarMarginBottom = 16.0F;
         mf32_BarMarginTop = 16.0F;
      }
      else
      {
         mf32_BarMarginBottom = 0.2F * static_cast<float32>(this->rect().height());
         mf32_BarMarginTop = 0.2F * static_cast<float32>(this->rect().height());
      }
      // ----------------------------------------------------------------------------------------------------------
      this->mpc_Ui->horizontalLayout->setContentsMargins(static_cast<sintn>(mf32_BarMarginLeft),
                                                         static_cast<sintn>(mf32_BarMarginTop),
                                                         static_cast<sintn>(mf32_BarMarginRight),
                                                         static_cast<sintn>(mf32_BarMarginBottom));
      break;

   //--
   //--
   //--
   //--
   case stw_opensyde_gui_logic::C_PuiSvDbProgressBar::eTYPE_3:
      // Variables to marge the ProgressBar dependent on the size of the rect
      if ((static_cast<float32>(this->rect().height()) * 0.6F) < 15.0F)
      {
         c_TextFont.setPixelSize(16);
      }
      else
      {
         if (this->mq_ShowMinMax == false)
         {
            f32_TemporaryHelpingVariable1 = static_cast<float32>(this->rect().height());
         }
         else
         {
            f32_TemporaryHelpingVariable1 = static_cast<float32>(this->rect().height()) * 0.6F;
         }
         c_TextFont.setPixelSize(static_cast<sintn>(f32_TemporaryHelpingVariable1));
      }
      //      f32_TemporaryHelpingVariable1 = static_cast<float32>(this->rect().height()) * 0.6F;
      //      c_TextFont.setPixelSize(static_cast<sintn>(f32_TemporaryHelpingVariable1));
      c_TextFontMetric = QFontMetrics(c_TextFont); // = c_Painter.fontMetrics();
      // This ensures, that the values can always be displayd correctly
      // ----------------------------------------------------------------------------------------------------------
      if (this->me_Alignment == stw_opensyde_gui_logic::C_PuiSvDbProgressBar::eLEFT)
      {
         if (c_TextFontMetric.width(mc_MaxValue) > c_TextFontMetric.width(mc_MinValue))
         {
            mf32_BarMarginLeft = static_cast<float32>(c_TextFontMetric.width(mc_MaxValue)) +
                                 (static_cast<float32>(this->rect().width()) * 0.01F);
            if (this->mq_ShowMinMax == false)
            {
               f32_TemporaryHelpingVariable1 = static_cast<float32>(this->rect().height()) * 0.33F;
            }
            else
            {
               f32_TemporaryHelpingVariable1 = static_cast<float32>(this->rect().height()) * 0.2F;
            }
            //            f32_TemporaryHelpingVariable1 = static_cast<float32>(this->rect().height()) * 0.2F;
            c_TextFont.setPixelSize(static_cast<sintn>(f32_TemporaryHelpingVariable1));
            c_TextFontMetric = QFontMetrics(c_TextFont);
            mf32_BarMarginLeft += static_cast<float32>(c_TextFontMetric.width(mc_Unit));
         }
         else
         {
            mf32_BarMarginLeft = static_cast<float32>(c_TextFontMetric.width(mc_MinValue)) +
                                 (static_cast<float32>(this->rect().width()) * 0.01F);
            if (this->mq_ShowMinMax == false)
            {
               f32_TemporaryHelpingVariable1 = static_cast<float32>(this->rect().height()) * 0.33F;
            }
            else
            {
               f32_TemporaryHelpingVariable1 = static_cast<float32>(this->rect().height()) * 0.2F;
            }
            //            f32_TemporaryHelpingVariable1 = static_cast<float32>(this->rect().height()) * 0.2F;
            c_TextFont.setPixelSize(static_cast<sintn>(f32_TemporaryHelpingVariable1));
            c_TextFontMetric = QFontMetrics(c_TextFont);
            mf32_BarMarginLeft += static_cast<float32>(c_TextFontMetric.width(mc_Unit));
         }
         mf32_BarMarginRight = 0.0F;
      }
      else
      {
         mf32_BarMarginLeft = 0.0F;
         if (c_TextFontMetric.width(mc_MaxValue) > c_TextFontMetric.width(mc_MinValue))
         {
            mf32_BarMarginRight = static_cast<float32>(c_TextFontMetric.width(mc_MaxValue)) +
                                  (static_cast<float32>(this->rect().width()) * 0.01F);
            if (this->mq_ShowMinMax == false)
            {
               f32_TemporaryHelpingVariable1 = static_cast<float32>(this->rect().height()) * 0.33F;
            }
            else
            {
               f32_TemporaryHelpingVariable1 = static_cast<float32>(this->rect().height()) * 0.2F;
            }
            //            f32_TemporaryHelpingVariable1 = static_cast<float32>(this->rect().height()) * 0.2F;
            c_TextFont.setPixelSize(static_cast<sintn>(f32_TemporaryHelpingVariable1));
            c_TextFontMetric = QFontMetrics(c_TextFont);
            mf32_BarMarginRight += static_cast<float32>(c_TextFontMetric.width(mc_Unit));
         }
         else
         {
            mf32_BarMarginRight = static_cast<float32>(c_TextFontMetric.width(mc_MinValue)) +
                                  (static_cast<float32>(this->rect().width()) * 0.01F);
            if (this->mq_ShowMinMax == false)
            {
               f32_TemporaryHelpingVariable1 = static_cast<float32>(this->rect().height()) * 0.33F;
            }
            else
            {
               f32_TemporaryHelpingVariable1 = static_cast<float32>(this->rect().height()) * 0.2F;
            }
            //            f32_TemporaryHelpingVariable1 = static_cast<float32>(this->rect().height()) * 0.2F;
            c_TextFont.setPixelSize(static_cast<sintn>(f32_TemporaryHelpingVariable1));
            c_TextFontMetric = QFontMetrics(c_TextFont);
            mf32_BarMarginRight += static_cast<float32>(c_TextFontMetric.width(mc_Unit));
         }
      }

      if ((static_cast<float32>(this->rect().height()) * 0.2F) < 15.0F)
      {
         mf32_BarMarginBottom = 16.0F;
         mf32_BarMarginTop = 16.0F;
      }
      else
      {
         mf32_BarMarginBottom = 0.2F * static_cast<float32>(this->rect().height());
         mf32_BarMarginTop = 0.2F * static_cast<float32>(this->rect().height());
      }
      //-------------------------
      if (this->mq_ShowMinMax == false)
      {
         mf32_BarMarginBottom = 0.0F;
         mf32_BarMarginTop = 0.0F;
      }
      // ----------------------------------------------------------------------------------------------------------
      this->mpc_Ui->horizontalLayout->setContentsMargins(static_cast<sintn>(mf32_BarMarginLeft),
                                                         static_cast<sintn>(mf32_BarMarginTop),
                                                         static_cast<sintn>(mf32_BarMarginRight),
                                                         static_cast<sintn>(mf32_BarMarginBottom));
      break;

   default:
      break;
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Sets of the color transparence value configured by the actual timeout state

   \param[in]     osn_Value                           Value for transparence (0..255)
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaItDashboardProgressBarWidget::SetTransparence(const sintn osn_Value)
{
   this->msn_Transparence = osn_Value;
}

//----------------------------------------------------------------------------------------------------------------------

stw_opensyde_gui_logic::C_PuiSvDbProgressBar::E_Type C_SyvDaItDashboardProgressBarWidget::GetType() const
{
   return this->me_Type;
}

//----------------------------------------------------------------------------------------------------------------------
// Karsten Ende
