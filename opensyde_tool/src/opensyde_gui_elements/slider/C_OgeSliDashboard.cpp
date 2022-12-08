//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Slider part of dashboard slider item (implementation)

   Slider part of dashboard slider item.
   This class does not contain any functionality,
   but needs to exist, to have a unique group,
   to apply a specific stylesheet for.

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.hpp"

#include <QPainter>

#include "stwtypes.hpp"
#include "C_OgeWiUtil.hpp"
#include "C_OgeSliDashboard.hpp"
#include "TglUtils.hpp"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
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
C_OgeSliDashboard::C_OgeSliDashboard(QWidget * const opc_Parent) :
   C_OgeSliToolTipBase(opc_Parent),
   mpc_SvgRenderer(NULL),
   mq_IsDark(false),
   me_Style(C_PuiSvDbSlider::eOPENSYDE),
   me_Type(C_PuiSvDbSlider::eTYPE_SMALL_COLOR_1)
{
   C_OgeWiUtil::h_ApplyStylesheetProperty(this, "Type", 1);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default destructor

   Clean up.
*/
//----------------------------------------------------------------------------------------------------------------------
C_OgeSliDashboard::~C_OgeSliDashboard(void)
{
   delete this->mpc_SvgRenderer;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set current style

   \param[in]  oe_Style    Widget style
   \param[in]  oq_IsDark   Dark mode active flag
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OgeSliDashboard::SetCurrentStyle(const C_PuiSvDbWidgetBase::E_Style oe_Style, const bool oq_IsDark)
{
   this->me_Style = oe_Style;
   this->mq_IsDark = oq_IsDark;
   m_ReinitStyle();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Apply style

   \param[in]  oe_Type  New item type
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OgeSliDashboard::SetDisplayStyle(const C_PuiSvDbSlider::E_Type oe_Type)
{
   this->me_Type = oe_Type;

   switch (this->me_Type)
   {
   case C_PuiSvDbSlider::eTYPE_SMALL_COLOR_1:
   case C_PuiSvDbSlider::eTYPE_SMALL_COLOR_2:
      C_OgeWiUtil::h_ApplyStylesheetProperty(this, "Type", 1);
      break;
   case C_PuiSvDbSlider::eTYPE_BIG_COLOR_1:
   case C_PuiSvDbSlider::eTYPE_BIG_COLOR_2:
      C_OgeWiUtil::h_ApplyStylesheetProperty(this, "Type", 2);
      break;
   default:
      break;
   }

   HandleResize();
   m_ReinitStyle();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Update style for resized element
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OgeSliDashboard::HandleResize(void)
{
   QString c_Style = "";
   int32_t s32_GrooveHeight;
   int32_t s32_HandleMargin;
   int32_t s32_AvailableHeight;
   int32_t s32_Offset = 0;

   switch (me_Type)
   {
   case C_PuiSvDbSlider::eTYPE_SMALL_COLOR_1:
   case C_PuiSvDbSlider::eTYPE_SMALL_COLOR_2:
      s32_GrooveHeight = std::max(this->height() / 10, 1);
      break;
   case C_PuiSvDbSlider::eTYPE_BIG_COLOR_1:
   case C_PuiSvDbSlider::eTYPE_BIG_COLOR_2:
      s32_GrooveHeight = std::max(this->height() / 5, 1);
      break;
   default:
      tgl_assert(false);
      s32_GrooveHeight = 0;
      break;
   }
   s32_AvailableHeight = this->height() - s32_GrooveHeight;
   if ((s32_AvailableHeight % 2) == 0)
   {
      //Adapt to uneven result (even result should not be bigger than uneven result)
      s32_Offset = 1;
   }
   s32_HandleMargin = (s32_AvailableHeight + s32_Offset) / 2;
   c_Style = static_cast<QString>("stw--opensyde_gui_elements--C_OgeSliDashboard::groove {\n"
                                  " height:%1px;\n"
                                  "}\n"
                                  "stw--opensyde_gui_elements--C_OgeSliDashboard::handle {\n"
                                  " height:%2px;\n"
                                  " width:%2px;\n"
                                  " margin-top: -%3px;"
                                  " margin-bottom: -%3px;\n"
                                  "}\n").arg(QString::number(s32_GrooveHeight),            //Bar height
                                             QString::number(this->height() - s32_Offset), //Handle size
                                             QString::number(s32_HandleMargin));           //Handle offset, negative to
                                                                                           // be outside
                                                                                           // of groove/bar
   this->setStyleSheet(c_Style);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Overwritten resize event slot

   Here: Update style

   \param[in,out]  opc_Event  Event identification and information
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OgeSliDashboard::resizeEvent(QResizeEvent * const opc_Event)
{
   QSlider::resizeEvent(opc_Event);

   HandleResize();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Overwritten paint event slot

   Here: paint handle manually

   \param[in,out]  opc_Event  Event identification and information
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OgeSliDashboard::paintEvent(QPaintEvent * const opc_Event)
{
   C_OgeSliToolTipBase::paintEvent(opc_Event);
   m_DrawHandle(opc_Event->rect());
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set SVG path

   \param[in]  orc_Path    SVG path
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OgeSliDashboard::m_SetSvg(const QString & orc_Path)
{
   delete this->mpc_SvgRenderer;
   if (orc_Path.compare("") == 0)
   {
      this->mpc_SvgRenderer = NULL;
   }
   else
   {
      this->mpc_SvgRenderer = new QSvgRenderer(orc_Path);
   }
   this->update();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Manually draw handle

   \param[in]  orc_Rect    Rectangle to draw in
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OgeSliDashboard::m_DrawHandle(const QRect & orc_Rect)
{
   if (this->mpc_SvgRenderer != NULL)
   {
      QPainter c_Painter(this);
      c_Painter.setClipRect(orc_Rect);
      const float64_t f64_SliderSize = static_cast<float64_t>(std::min(this->rect().width(), this->rect().height()));
      const float64_t f64_SliderPosVertical = (static_cast<float64_t>(this->rect().height()) - f64_SliderSize) / 2.0;
      const float64_t f64_Min = static_cast<float64_t>(this->minimum());
      const float64_t f64_Val = static_cast<float64_t>(this->value());
      const float64_t f64_Max = static_cast<float64_t>(this->maximum());
      const float64_t f64_Progress = (f64_Val - f64_Min) / (f64_Max - f64_Min);
      const float64_t f64_HorizontalMin = static_cast<float64_t>(this->rect().x());
      //Don't use exact width but consider all possible slider center positions instead 2* half slider offset
      const float64_t f64_Width = static_cast<float64_t>(this->rect().width()) - f64_SliderSize;
      //X is top left corner so don't add half slider offset for correct progress information
      const float64_t f64_HorizontalMidProgress = f64_HorizontalMin + (f64_Width * f64_Progress);
      // Qt issue in QStyle::sliderPositionFromValue (https://bugreports.qt.io/browse/QTBUG-29764)
      // This function is actually used to handle the slider interaction,
      // but it is buggy or max values >= 0 in this case:
      // QStyle::sliderPositionFromValue(std::numeric_limits<int>::min(), max, 0, 100,false);
      //const float64_t f64_HorizontalMidProgress = f64_HorizontalMin + QStyle::sliderPositionFromValue(
      // this->minimum(),this->maximum(),this->value(),f64_Width,false);
      const QRectF c_SliderRect(QPointF(f64_HorizontalMidProgress, f64_SliderPosVertical),
                                QSizeF(f64_SliderSize, f64_SliderSize));

      this->mpc_SvgRenderer->render(&c_Painter, c_SliderRect);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set appropriate handle for current style
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OgeSliDashboard::m_ReinitStyle(void)
{
   switch (this->me_Type)
   {
   case C_PuiSvDbSlider::eTYPE_SMALL_COLOR_1:
   case C_PuiSvDbSlider::eTYPE_BIG_COLOR_1:
      switch (this->me_Style)
      {
      case C_PuiSvDbSlider::eOPENSYDE:
         if (this->mq_IsDark == false)
         {
            this->m_SetSvg("://images/system_views/dashboards/icons/SliderHandleOpenSYDEBright1.svg");
         }
         else
         {
            this->m_SetSvg("://images/system_views/dashboards/icons/SliderHandleOpenSYDEDark1.svg");
         }
         break;
      case C_PuiSvDbSlider::eOPENSYDE_2:
         this->m_SetSvg("://images/system_views/dashboards/icons/SliderHandleOpenSYDE2Bright1.svg");
         break;
      case C_PuiSvDbSlider::eFLAT:
         this->m_SetSvg("://images/system_views/dashboards/icons/SliderHandleFlat1.svg");
         break;
      case C_PuiSvDbSlider::eSKEUOMORPH:
         if (this->mq_IsDark == false)
         {
            this->m_SetSvg("://images/system_views/dashboards/icons/SliderHandleSkeuomorphBright1.svg");
         }
         else
         {
            this->m_SetSvg("://images/system_views/dashboards/icons/SliderHandleSkeuomorphDark1.svg");
         }
         break;
      default:
         tgl_assert(false);
         break;
      }
      break;
   case C_PuiSvDbSlider::eTYPE_BIG_COLOR_2:
   case C_PuiSvDbSlider::eTYPE_SMALL_COLOR_2:
      switch (this->me_Style)
      {
      case C_PuiSvDbSlider::eOPENSYDE:
         if (this->mq_IsDark == false)
         {
            this->m_SetSvg("://images/system_views/dashboards/icons/SliderHandleOpenSYDEBright2.svg");
         }
         else
         {
            this->m_SetSvg("://images/system_views/dashboards/icons/SliderHandleOpenSYDEDark2.svg");
         }
         break;
      case C_PuiSvDbSlider::eOPENSYDE_2:
         if (this->mq_IsDark == false)
         {
            this->m_SetSvg("://images/system_views/dashboards/icons/SliderHandleOpenSYDE2Bright2.svg");
         }
         else
         {
            this->m_SetSvg("://images/system_views/dashboards/icons/SliderHandleOpenSYDE2Dark2.svg");
         }
         break;
      case C_PuiSvDbSlider::eFLAT:
         this->m_SetSvg("://images/system_views/dashboards/icons/SliderHandleFlat2.svg");
         break;
      case C_PuiSvDbSlider::eSKEUOMORPH:
         if (this->mq_IsDark == false)
         {
            this->m_SetSvg("://images/system_views/dashboards/icons/SliderHandleSkeuomorphBright2.svg");
         }
         else
         {
            this->m_SetSvg("://images/system_views/dashboards/icons/SliderHandleSkeuomorphDark2.svg");
         }
         break;
      default:
         tgl_assert(false);
         break;
      }
      break;
   default:
      tgl_assert(false);
      break;
   }
}
