//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Progress bar part of dashboard progress bar item (implementation)

   Progress bar part of dashboard progress bar item

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.h"

#include <QPainter>
#include "stwtypes.h"
#include "constants.h"
#include "C_OgeDashboardProgressBar.h"

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
C_OgeDashboardProgressBar::C_OgeDashboardProgressBar(QWidget * const opc_Parent) :
   QProgressBar(opc_Parent),
   me_Style(C_PuiSvDbWidgetBase::eOPENSYDE),
   msn_Offset(1)
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Apply style

   \param[in] oe_Style    New style type
   \param[in] oq_DarkMode Flag if dark mode is active
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OgeDashboardProgressBar::SetDisplayStyle(const C_PuiSvDbWidgetBase::E_Style oe_Style, const bool oq_DarkMode)
{
   this->me_Style = oe_Style;
   if (oq_DarkMode == false)
   {
      switch (oe_Style)
      {
      case C_PuiSvDbWidgetBase::eOPENSYDE:
         this->mc_BackgroundColor = mc_STYLE_GUIDE_COLOR_10;
         this->mc_ProgressColor = mc_STYLE_GUIDE_COLOR_7;
         this->msn_Offset = 1;
         break;
      case C_PuiSvDbWidgetBase::eFLAT:
         this->mc_BackgroundColor = mc_STYLE_GUIDE_COLOR_10;
         this->mc_ProgressColor = mc_STYLE_GUIDE_COLOR_26;
         this->msn_Offset = 1;
         break;
      case C_PuiSvDbWidgetBase::eSKEUOMORPH:
      case C_PuiSvDbWidgetBase::eOPENSYDE_2:
      default:
         break;
      }
   }
   else
   {
      switch (oe_Style)
      {
      case C_PuiSvDbWidgetBase::eOPENSYDE:
         this->mc_BackgroundColor = mc_STYLE_GUIDE_COLOR_36;
         this->mc_ProgressColor = mc_STYLE_GUIDE_COLOR_2;
         this->msn_Offset = 1;
         break;
      case C_PuiSvDbWidgetBase::eFLAT:
         this->mc_BackgroundColor = mc_STYLE_GUIDE_COLOR_32;
         this->mc_ProgressColor = mc_STYLE_GUIDE_COLOR_26;
         this->msn_Offset = 1;
         break;
      case C_PuiSvDbWidgetBase::eSKEUOMORPH:
      case C_PuiSvDbWidgetBase::eOPENSYDE_2:
      default:
         break;
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Overwritten paint event slot

   Here: manual markers

   \param[in,out] opc_Event Event identification and information
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OgeDashboardProgressBar::paintEvent(QPaintEvent * const opc_Event)
{
   if ((this->me_Style == C_PuiSvDbWidgetBase::eOPENSYDE) || (this->me_Style == C_PuiSvDbWidgetBase::eFLAT))
   {
      const sintn sn_ValueToMinimumDistance = this->value() - this->minimum();
      const float32 f32_ValueToMinimumDistance = static_cast<float32>(sn_ValueToMinimumDistance);
      const sintn sn_MinimumToMaximumDistance = this->maximum() - this->minimum();
      const float32 f32_MinimumToMaximumDistance = static_cast<float32>(sn_MinimumToMaximumDistance);

      Q_UNUSED(opc_Event)
      if (this->orientation() == Qt::Horizontal)
      {
         const sintn sn_SegmentCount = std::max(this->rect().width() / 50, 1);
         const float32 f32_RectWidth = static_cast<float32>(this->rect().width());
         const float32 f32_ProgressX = f32_RectWidth * (f32_ValueToMinimumDistance / f32_MinimumToMaximumDistance);
         const sintn sn_ProgressX = static_cast<sintn>(f32_ProgressX);
         QPainter c_Painter(this);

         //Paint segments
         if (sn_SegmentCount > 1)
         {
            const float32 f32_SegmentWidth = static_cast<float32>(this->rect().width()) /
                                             static_cast<float32>(sn_SegmentCount);

            //Each segment
            for (sintn sn_Segment = 0; sn_Segment < sn_SegmentCount; ++sn_Segment)
            {
               const float32 f32_Segment = static_cast<float32>(sn_Segment);
               const sintn sn_NextSegment = sn_Segment + 1;
               const float32 f32_NextSegment = static_cast<float32>(sn_NextSegment);
               const float32 f32_StartX = f32_Segment * f32_SegmentWidth;
               const float32 f32_EndX = f32_NextSegment * f32_SegmentWidth;
               const sintn sn_StartX = static_cast<sintn>(f32_StartX);
               const sintn sn_EndX = static_cast<sintn>(f32_EndX);
               const QRect c_Rect =
                  QRect(QPoint(sn_StartX + this->msn_Offset, this->rect().topLeft().y()),
                        QSize((sn_EndX - sn_StartX) - (2 * this->msn_Offset), this->rect().height()));
               const QRect c_Rect2 =
                  QRect(QPoint(sn_StartX + this->msn_Offset, this->rect().topLeft().y() + (c_Rect.height() / 10)),
                        QSize((sn_EndX - sn_StartX) - (2 * this->msn_Offset), (this->rect().height() * 8) / 10));

               //Has progress reached this segment
               if (sn_StartX < sn_ProgressX)
               {
                  //Has progress surpassed this segment
                  if (sn_ProgressX < sn_EndX)
                  {
                     const sintn sn_SegmentProgressWidth = (sn_ProgressX - sn_StartX) - this->msn_Offset;
                     //Partial fill

                     //Background
                     c_Painter.setPen(Qt::NoPen);
                     c_Painter.setBrush(this->mc_BackgroundColor);
                     if (this->me_Style == C_PuiSvDbWidgetBase::eFLAT)
                     {
                        c_Painter.drawRect(QRect(c_Rect.topLeft() +
                                                 QPoint(sn_SegmentProgressWidth, c_Rect.height() / 10),
                                                 QSize(c_Rect.width() - sn_SegmentProgressWidth,
                                                       (c_Rect.height() * 8) / 10)));
                     }
                     else
                     {
                        c_Painter.drawRect(QRect(c_Rect.topLeft() + QPoint(sn_SegmentProgressWidth, 0),
                                                 QSize(c_Rect.width() - sn_SegmentProgressWidth, c_Rect.height())));
                     }

                     //Progress
                     c_Painter.setPen(Qt::NoPen);
                     c_Painter.setBrush(this->mc_ProgressColor);
                     c_Painter.drawRect(QRect(c_Rect.topLeft(), QSize(sn_SegmentProgressWidth, c_Rect.height())));
                  }
                  else
                  {
                     //Complete fill
                     c_Painter.setPen(Qt::NoPen);
                     c_Painter.setBrush(this->mc_ProgressColor);
                     c_Painter.drawRect(c_Rect);
                  }
               }
               else
               {
                  //Empty fill
                  c_Painter.setPen(Qt::NoPen);
                  c_Painter.setBrush(this->mc_BackgroundColor);
                  if (this->me_Style == C_PuiSvDbWidgetBase::eFLAT)
                  {
                     c_Painter.drawRect(c_Rect2);
                  }
                  else
                  {
                     c_Painter.drawRect(c_Rect);
                  }
               }
            }
         }
         else
         {
            //Paint all
            c_Painter.setPen(Qt::NoPen);
            c_Painter.setBrush(this->mc_BackgroundColor);
            c_Painter.drawRect(this->rect());

            //Paint progress
            c_Painter.setPen(Qt::NoPen);
            c_Painter.setBrush(this->mc_ProgressColor);
            c_Painter.drawRect(QRect(this->rect().topLeft(), QSize(sn_ProgressX, this->rect().height())));
         }
      }
      else
      {
         const sintn sn_SegmentCount = std::max(this->rect().height() / 12, 1);
         const float32 f32_RectHeight = static_cast<float32>(this->rect().height());
         const float32 f32_ProgressY = f32_RectHeight * (f32_ValueToMinimumDistance / f32_MinimumToMaximumDistance);
         const sintn sn_ProgressY = static_cast<sintn>(f32_ProgressY);
         QPainter c_Painter(this);

         //Paint segments
         if (sn_SegmentCount > 1)
         {
            const float32 f32_SegmentHeight = static_cast<float32>(this->rect().height()) /
                                              static_cast<float32>(sn_SegmentCount);

            //Each segment
            for (sintn sn_Segment = 0; sn_Segment < sn_SegmentCount; ++sn_Segment)
            {
               const float32 f32_Segment = static_cast<float32>(sn_Segment);
               const sintn sn_NextSegment = sn_Segment + 1;
               const float32 f32_NextSegment = static_cast<float32>(sn_NextSegment);
               const float32 f32_StartY = f32_Segment * f32_SegmentHeight;
               const float32 f32_EndY = f32_NextSegment * f32_SegmentHeight;
               const sintn sn_StartY = static_cast<sintn>(f32_StartY);
               const sintn sn_EndY = static_cast<sintn>(f32_EndY);
               const QRect c_Rect =
                  QRect(QPoint(this->rect().topLeft().x(), sn_StartY + this->msn_Offset),
                        QSize(this->rect().width(), (sn_EndY - sn_StartY) - (2 * this->msn_Offset)));
               const QRect c_Rect2 =
                  QRect(QPoint(this->rect().topLeft().x() + (c_Rect.width() / 10), sn_StartY + this->msn_Offset),
                        QSize((this->rect().width() * 8) / 10, (sn_EndY - sn_StartY) - (2 * this->msn_Offset)));

               //Has progress reached this segment
               if (sn_EndY > (this->rect().height() - sn_ProgressY))
               {
                  //Has progress surpassed this segment
                  if (sn_StartY < (this->rect().height() - sn_ProgressY))
                  {
                     const sintn sn_SegmentProgressHeight = sn_EndY - (this->rect().height() - sn_ProgressY);
                     //Partial fill

                     //Background
                     c_Painter.setPen(Qt::NoPen);
                     c_Painter.setBrush(this->mc_BackgroundColor);
                     if (this->me_Style == C_PuiSvDbWidgetBase::eFLAT)
                     {
                        c_Painter.drawRect(c_Rect2);
                     }
                     else
                     {
                        c_Painter.drawRect(c_Rect);
                     }

                     //Progress
                     c_Painter.setPen(Qt::NoPen);
                     c_Painter.setBrush(this->mc_ProgressColor);
                     c_Painter.drawRect(QRect(
                                           static_cast<QPoint>(c_Rect.topLeft() +
                                                               QPoint(0, c_Rect.height() - sn_SegmentProgressHeight)),
                                           QSize(c_Rect.width(), sn_SegmentProgressHeight)));
                  }
                  else
                  {
                     //Complete fill
                     c_Painter.setPen(Qt::NoPen);
                     c_Painter.setBrush(this->mc_ProgressColor);
                     c_Painter.drawRect(c_Rect);
                  }
               }
               else
               {
                  //Empty fill
                  c_Painter.setPen(Qt::NoPen);
                  c_Painter.setBrush(this->mc_BackgroundColor);
                  if (this->me_Style == C_PuiSvDbWidgetBase::eFLAT)
                  {
                     c_Painter.drawRect(c_Rect2);
                  }
                  else
                  {
                     c_Painter.drawRect(c_Rect);
                  }
               }
            }
         }
         else
         {
            //Paint all
            c_Painter.setPen(Qt::NoPen);
            c_Painter.setBrush(this->mc_BackgroundColor);
            c_Painter.drawRect(this->rect());

            //Paint progress
            c_Painter.setPen(Qt::NoPen);
            c_Painter.setBrush(this->mc_ProgressColor);
            c_Painter.drawRect(QRect(this->rect().bottomLeft() - QPoint(0, sn_ProgressY),
                                     QSize(this->rect().width(), sn_ProgressY)));
         }
      }
   }
   else
   {
      QProgressBar::paintEvent(opc_Event);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Overwritten show event slot

   \param[in,out] opc_Event Event identification and information
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OgeDashboardProgressBar::resizeEvent(QResizeEvent * const opc_Event)
{
   QProgressBar::resizeEvent(opc_Event);
   if (this->orientation() == Qt::Horizontal)
   {
      const QString c_Border =
         static_cast<QString>("stw_opensyde_gui_elements--C_OgeDashboardProgressBar[Style=\"SKEUOMORPH_DARK\"],"
                              "stw_opensyde_gui_elements--C_OgeDashboardProgressBar[Style=\"SKEUOMORPH_DARK\"]::chunk,"
                              "stw_opensyde_gui_elements--C_OgeDashboardProgressBar[Style=\"SKEUOMORPH_BRIGHT\"],"
                              "stw_opensyde_gui_elements--C_OgeDashboardProgressBar[Style=\"SKEUOMORPH_BRIGHT\"]::chunk"
                              "{border-radius:%1px;}").arg((this->size().height() * 3) / 10);
      this->setStyleSheet(c_Border);
   }
   else
   {
      const QString c_Border =
         static_cast<QString>("stw_opensyde_gui_elements--C_OgeDashboardProgressBar[Style=\"SKEUOMORPH_DARK\"],"
                              "stw_opensyde_gui_elements--C_OgeDashboardProgressBar[Style=\"SKEUOMORPH_DARK\"]::chunk,"
                              "stw_opensyde_gui_elements--C_OgeDashboardProgressBar[Style=\"SKEUOMORPH_BRIGHT\"],"
                              "stw_opensyde_gui_elements--C_OgeDashboardProgressBar[Style=\"SKEUOMORPH_BRIGHT\"]::chunk"
                              "{border-radius:%1px;}").arg((this->size().width() * 25) / 1000);
      this->setStyleSheet(c_Border);
   }
}
