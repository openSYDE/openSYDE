//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Progress bar part of dashboard progress bar item (implementation)

   Progress bar part of dashboard progress bar item

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.hpp"

#include <QPainter>
#include "stwtypes.hpp"
#include "constants.hpp"
#include "C_OgeDashboardProgressBar.hpp"

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
C_OgeDashboardProgressBar::C_OgeDashboardProgressBar(QWidget * const opc_Parent) :
   QProgressBar(opc_Parent),
   me_Style(C_PuiSvDbWidgetBase::eOPENSYDE),
   ms32_Offset(1)
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
         this->ms32_Offset = 1;
         break;
      case C_PuiSvDbWidgetBase::eFLAT:
         this->mc_BackgroundColor = mc_STYLE_GUIDE_COLOR_10;
         this->mc_ProgressColor = mc_STYLE_GUIDE_COLOR_26;
         this->ms32_Offset = 1;
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
         this->ms32_Offset = 1;
         break;
      case C_PuiSvDbWidgetBase::eFLAT:
         this->mc_BackgroundColor = mc_STYLE_GUIDE_COLOR_32;
         this->mc_ProgressColor = mc_STYLE_GUIDE_COLOR_26;
         this->ms32_Offset = 1;
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
      const int32_t s32_ValueToMinimumDistance = this->value() - this->minimum();
      const float32_t f32_ValueToMinimumDistance = static_cast<float32_t>(s32_ValueToMinimumDistance);
      const int32_t s32_MinimumToMaximumDistance = this->maximum() - this->minimum();
      const float32_t f32_MinimumToMaximumDistance = static_cast<float32_t>(s32_MinimumToMaximumDistance);

      Q_UNUSED(opc_Event)
      if (this->orientation() == Qt::Horizontal)
      {
         const int32_t s32_SegmentCount = std::max(this->rect().width() / 50, 1);
         const float32_t f32_RectWidth = static_cast<float32_t>(this->rect().width());
         const float32_t f32_ProgressHorizontal = f32_RectWidth *
                                                  (f32_ValueToMinimumDistance / f32_MinimumToMaximumDistance);
         const int32_t s32_ProgressHorizontal = static_cast<int32_t>(f32_ProgressHorizontal);
         QPainter c_Painter(this);

         //Paint segments
         if (s32_SegmentCount > 1)
         {
            const float32_t f32_SegmentWidth = static_cast<float32_t>(this->rect().width()) /
                                               static_cast<float32_t>(s32_SegmentCount);

            //Each segment
            for (int32_t s32_Segment = 0; s32_Segment < s32_SegmentCount; ++s32_Segment)
            {
               const float32_t f32_Segment = static_cast<float32_t>(s32_Segment);
               const int32_t s32_NextSegment = s32_Segment + 1;
               const float32_t f32_NextSegment = static_cast<float32_t>(s32_NextSegment);
               const float32_t f32_StartHorizontal = f32_Segment * f32_SegmentWidth;
               const float32_t f32_EndHorizontal = f32_NextSegment * f32_SegmentWidth;
               const int32_t s32_StartHorizontal = static_cast<int32_t>(f32_StartHorizontal);
               const int32_t s32_EndHorizontal = static_cast<int32_t>(f32_EndHorizontal);
               const QRect c_Rect =
                  QRect(QPoint(s32_StartHorizontal + this->ms32_Offset, this->rect().topLeft().y()),
                        QSize((s32_EndHorizontal - s32_StartHorizontal) - (2 * this->ms32_Offset),
                              this->rect().height()));
               const QRect c_Rect2 =
                  QRect(QPoint(s32_StartHorizontal + this->ms32_Offset,
                               this->rect().topLeft().y() + (c_Rect.height() / 10)),
                        QSize((s32_EndHorizontal - s32_StartHorizontal) - (2 * this->ms32_Offset),
                              (this->rect().height() * 8) / 10));

               //Has progress reached this segment
               if (s32_StartHorizontal < s32_ProgressHorizontal)
               {
                  //Has progress surpassed this segment
                  if (s32_ProgressHorizontal < s32_EndHorizontal)
                  {
                     const int32_t s32_SegmentProgressWidth = (s32_ProgressHorizontal - s32_StartHorizontal) -
                                                              this->ms32_Offset;
                     //Partial fill

                     //Background
                     c_Painter.setPen(Qt::NoPen);
                     c_Painter.setBrush(this->mc_BackgroundColor);
                     if (this->me_Style == C_PuiSvDbWidgetBase::eFLAT)
                     {
                        c_Painter.drawRect(QRect(c_Rect.topLeft() +
                                                 QPoint(s32_SegmentProgressWidth, c_Rect.height() / 10),
                                                 QSize(c_Rect.width() - s32_SegmentProgressWidth,
                                                       (c_Rect.height() * 8) / 10)));
                     }
                     else
                     {
                        c_Painter.drawRect(QRect(c_Rect.topLeft() + QPoint(s32_SegmentProgressWidth, 0),
                                                 QSize(c_Rect.width() - s32_SegmentProgressWidth, c_Rect.height())));
                     }

                     //Progress
                     c_Painter.setPen(Qt::NoPen);
                     c_Painter.setBrush(this->mc_ProgressColor);
                     c_Painter.drawRect(QRect(c_Rect.topLeft(), QSize(s32_SegmentProgressWidth, c_Rect.height())));
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
            c_Painter.drawRect(QRect(this->rect().topLeft(), QSize(s32_ProgressHorizontal, this->rect().height())));
         }
      }
      else
      {
         const int32_t s32_SegmentCount = std::max(this->rect().height() / 12, 1);
         const float32_t f32_RectHeight = static_cast<float32_t>(this->rect().height());
         const float32_t f32_ProgressVertical = f32_RectHeight *
                                                (f32_ValueToMinimumDistance / f32_MinimumToMaximumDistance);
         const int32_t s32_ProgressVertical = static_cast<int32_t>(f32_ProgressVertical);
         QPainter c_Painter(this);

         //Paint segments
         if (s32_SegmentCount > 1)
         {
            const float32_t f32_SegmentHeight = static_cast<float32_t>(this->rect().height()) /
                                                static_cast<float32_t>(s32_SegmentCount);

            //Each segment
            for (int32_t s32_Segment = 0; s32_Segment < s32_SegmentCount; ++s32_Segment)
            {
               const float32_t f32_Segment = static_cast<float32_t>(s32_Segment);
               const int32_t s32_NextSegment = s32_Segment + 1;
               const float32_t f32_NextSegment = static_cast<float32_t>(s32_NextSegment);
               const float32_t f32_StartVertical = f32_Segment * f32_SegmentHeight;
               const float32_t f32_EndVertical = f32_NextSegment * f32_SegmentHeight;
               const int32_t s32_StartVertical = static_cast<int32_t>(f32_StartVertical);
               const int32_t s32_EndVertical = static_cast<int32_t>(f32_EndVertical);
               const QRect c_Rect =
                  QRect(QPoint(this->rect().topLeft().x(), s32_StartVertical + this->ms32_Offset),
                        QSize(this->rect().width(), (s32_EndVertical - s32_StartVertical) - (2 * this->ms32_Offset)));
               const QRect c_Rect2 =
                  QRect(QPoint(
                           this->rect().topLeft().x() + (c_Rect.width() / 10), s32_StartVertical + this->ms32_Offset),
                        QSize((this->rect().width() * 8) / 10,
                              (s32_EndVertical - s32_StartVertical) - (2 * this->ms32_Offset)));

               //Has progress reached this segment
               if (s32_EndVertical > (this->rect().height() - s32_ProgressVertical))
               {
                  //Has progress surpassed this segment
                  if (s32_StartVertical < (this->rect().height() - s32_ProgressVertical))
                  {
                     const int32_t s32_SegmentProgressHeight = s32_EndVertical -
                                                               (this->rect().height() - s32_ProgressVertical);
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
                                                               QPoint(0, c_Rect.height() - s32_SegmentProgressHeight)),
                                           QSize(c_Rect.width(), s32_SegmentProgressHeight)));
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
            c_Painter.drawRect(QRect(this->rect().bottomLeft() - QPoint(0, s32_ProgressVertical),
                                     QSize(this->rect().width(), s32_ProgressVertical)));
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
         static_cast<QString>("stw--opensyde_gui_elements--C_OgeDashboardProgressBar[Style=\"SKEUOMORPH_DARK\"],"
                              "stw--opensyde_gui_elements--C_OgeDashboardProgressBar[Style=\"SKEUOMORPH_DARK\"]::chunk,"
                              "stw--opensyde_gui_elements--C_OgeDashboardProgressBar[Style=\"SKEUOMORPH_BRIGHT\"],"
                              "stw--opensyde_gui_elements--C_OgeDashboardProgressBar[Style=\"SKEUOMORPH_BRIGHT\"]::chunk"
                              "{border-radius:%1px;}").arg((this->size().height() * 3) / 10);
      this->setStyleSheet(c_Border);
   }
   else
   {
      const QString c_Border =
         static_cast<QString>("stw--opensyde_gui_elements--C_OgeDashboardProgressBar[Style=\"SKEUOMORPH_DARK\"],"
                              "stw--opensyde_gui_elements--C_OgeDashboardProgressBar[Style=\"SKEUOMORPH_DARK\"]::chunk,"
                              "stw--opensyde_gui_elements--C_OgeDashboardProgressBar[Style=\"SKEUOMORPH_BRIGHT\"],"
                              "stw--opensyde_gui_elements--C_OgeDashboardProgressBar[Style=\"SKEUOMORPH_BRIGHT\"]::chunk"
                              "{border-radius:%1px;}").arg((this->size().width() * 25) / 1000);
      this->setStyleSheet(c_Border);
   }
}
