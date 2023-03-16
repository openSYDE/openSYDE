//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       short description (implementation)

   detailed description

   \copyright   Copyright 2018 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.hpp"

#include <QPainter>
#include <QStyleOptionGraphicsItem>
#include "C_GiInfo.hpp"
#include "C_Uti.hpp"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw::opensyde_gui;
using namespace stw::opensyde_gui_logic;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

/* -- Global Variables ---------------------------------------------------------------------------------------------- */

/* -- Module Global Variables --------------------------------------------------------------------------------------- */

/* -- Module Global Function Prototypes ----------------------------------------------------------------------------- */

/* -- Implementation ------------------------------------------------------------------------------------------------ */

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default constructor

   Set up GUI with all elements.

   \param[in]     orc_Rect   Rectangle
   \param[in,out] opc_Parent Optional pointer to parent
*/
//----------------------------------------------------------------------------------------------------------------------
C_GiInfo::C_GiInfo(const QRectF & orc_Rect, QGraphicsItem * const opc_Parent) :
   QGraphicsRectItem(orc_Rect, opc_Parent),
   mc_Text(""),
   mc_BackgroundColor(Qt::transparent),
   mc_TextColor(Qt::transparent),
   ms32_IconSize(0),
   ms32_TextAlignment(static_cast<int32_t>(Qt::AlignCenter)),
   mpc_SvgRenderer(NULL),
   mpc_Movie(NULL)
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default destructor

   Clean up.
*/
//----------------------------------------------------------------------------------------------------------------------
C_GiInfo::~C_GiInfo(void)
{
   delete mpc_SvgRenderer;
   mpc_SvgRenderer = NULL;
   delete mpc_Movie;
   mpc_Movie = NULL;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set text size

   \param[in] orc_Value    New text size
*/
//----------------------------------------------------------------------------------------------------------------------
void C_GiInfo::SetTextFont(const QFont & orc_Value)
{
   this->mc_TextFont = orc_Value;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set text

   \param[in] orc_Value New text
*/
//----------------------------------------------------------------------------------------------------------------------
void C_GiInfo::SetText(const QString & orc_Value)
{
   this->mc_Text = orc_Value;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Set GIF

   \param[in] orc_Value GIF path
*/
//----------------------------------------------------------------------------------------------------------------------
void C_GiInfo::SetGif(const QString & orc_Value)
{
   delete (this->mpc_SvgRenderer);
   this->mpc_SvgRenderer = NULL;
   delete (this->mpc_Movie);
   if (orc_Value.compare("") != 0)
   {
      this->mpc_Movie = new QMovie(orc_Value);
      connect(this->mpc_Movie, &QMovie::frameChanged, this, &C_GiInfo::m_Update);
      connect(this->mpc_Movie, &QMovie::finished, this, &C_GiInfo::m_RestartGif);
      this->mpc_Movie->setScaledSize(QSize(this->ms32_IconSize, this->ms32_IconSize));
      this->mpc_Movie->start();
   }
   else
   {
      this->mpc_Movie = NULL;
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set SVG

   \param[in] orc_Value New SVG
*/
//----------------------------------------------------------------------------------------------------------------------
void C_GiInfo::SetSvg(const QString & orc_Value)
{
   delete (this->mpc_Movie);
   this->mpc_Movie = NULL;
   delete (this->mpc_SvgRenderer);
   if (orc_Value.compare("") != 0)
   {
      this->mpc_SvgRenderer = new QSvgRenderer(orc_Value);
      //Probably only necessary for animations
      connect(this->mpc_SvgRenderer, &QSvgRenderer::repaintNeeded, this, &C_GiInfo::m_Update);
   }
   else
   {
      this->mpc_SvgRenderer = NULL;
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set color

   \param[in] orc_Value New color
*/
//----------------------------------------------------------------------------------------------------------------------
void C_GiInfo::SetBackgroundColor(const QColor & orc_Value)
{
   this->mc_BackgroundColor = orc_Value;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set color and alignment

   \param[in] orc_Value       New color
   \param[in] os32_Alignment  New text alignment
*/
//----------------------------------------------------------------------------------------------------------------------
void C_GiInfo::SetTextColor(const QColor & orc_Value, const int32_t os32_Alignment)
{
   this->mc_TextColor = orc_Value;
   this->ms32_TextAlignment = os32_Alignment;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set icon size

   \param[in] os32_IconSize New icon size
*/
//----------------------------------------------------------------------------------------------------------------------
void C_GiInfo::SetIconSize(const int32_t os32_IconSize)
{
   this->ms32_IconSize = os32_IconSize;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Signal for update of current scaling

   \param[in] orc_Transform Current scaling
*/
//----------------------------------------------------------------------------------------------------------------------
void C_GiInfo::UpdateTransform(const QTransform & orc_Transform)
{
   this->mc_Transform = orc_Transform;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Overridden paint event

   Here: Custom paint operation

   \param[in]     opc_Painter    Pointer to Painter
   \param[in]     opc_Option     Style option
   \param[in]     opc_Widget     Optional widget to paint on
*/
//----------------------------------------------------------------------------------------------------------------------
void C_GiInfo::paint(QPainter * const opc_Painter, const QStyleOptionGraphicsItem * const opc_Option,
                     QWidget * const opc_Widget)
{
   const QFontMetrics c_Metrics(this->mc_TextFont);
   const int32_t s32_TextOffset = static_cast<int32_t>(opc_Option->rect.height() - c_Metrics.height()) / 2L;
   int32_t s32_TextHorizontal = opc_Option->rect.topLeft().x() + s32_TextOffset;
   const int32_t s32_TextVertical = opc_Option->rect.topLeft().y() + s32_TextOffset;
   int32_t s32_TextWidth = static_cast<int32_t>(opc_Option->rect.width()) - (2L * s32_TextOffset);
   const int32_t s32_TextHeight = static_cast<int32_t>(opc_Option->rect.height()) - (2L * s32_TextOffset);
   const int32_t s32_IconOffset = (static_cast<int32_t>(opc_Option->rect.height()) - this->ms32_IconSize) / 2;
   const int32_t s32_IconRectHorizontal = static_cast<int32_t>(opc_Option->rect.topLeft().x()) + s32_IconOffset;
   const int32_t s32_IconRectVertical = static_cast<int32_t>(opc_Option->rect.topLeft().y()) + s32_IconOffset;
   const QRectF c_IconRect(static_cast<float64_t>(s32_IconRectHorizontal),
                           static_cast<float64_t>(s32_IconRectVertical),
                           static_cast<float64_t>(this->ms32_IconSize),
                           static_cast<float64_t>(this->ms32_IconSize));

   //Background
   if (this->mc_BackgroundColor.alpha() > 0)
   {
      opc_Painter->save();
      opc_Painter->setBrush(static_cast<QBrush>(this->mc_BackgroundColor));
      opc_Painter->setPen(Qt::NoPen);
      opc_Painter->drawRoundedRect(opc_Option->rect,
                                   std::min(static_cast<float64_t>(opc_Option->rect.width()) / 2.0,
                                            static_cast<float64_t>(opc_Option->rect.height()) / 2.0),
                                   static_cast<float64_t>(opc_Option->rect.height()) / 2.0);
      opc_Painter->restore();
   }

   //Icon
   if (this->mpc_SvgRenderer != NULL)
   {
      s32_TextHorizontal += this->ms32_IconSize + s32_IconOffset;
      s32_TextWidth -= this->ms32_IconSize;
      this->mpc_SvgRenderer->render(opc_Painter, c_IconRect);
   }

   //Gif
   if (this->mpc_Movie != NULL)
   {
      const QRectF c_TransformedRect = this->mc_Transform.mapRect(c_IconRect);
      this->mpc_Movie->setScaledSize(static_cast<QSize>(c_TransformedRect.size().toSize()));
      {
         const QImage c_Image = this->mpc_Movie->currentImage();
         opc_Painter->drawImage(c_IconRect, c_Image);
      }
   }

   Q_UNUSED(opc_Widget)

   //Text
   opc_Painter->save();
   opc_Painter->setFont(this->mc_TextFont);
   opc_Painter->setPen(QPen(this->mc_TextColor, this->mc_TextFont.pointSizeF()));
   opc_Painter->drawText(QRect(QPoint(s32_TextHorizontal, s32_TextVertical), QSize(s32_TextWidth,
                                                                                   s32_TextHeight)), ms32_TextAlignment,
                         C_Uti::h_AdaptStringToSize(this->mc_Text, c_Metrics, static_cast<float64_t>(s32_TextWidth)));
   opc_Painter->restore();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Trigger update

   Probably only necessary for animations
*/
//----------------------------------------------------------------------------------------------------------------------
void C_GiInfo::m_Update(void)
{
   this->update();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  If there is any GIF, restart it
*/
//----------------------------------------------------------------------------------------------------------------------
void C_GiInfo::m_RestartGif()
{
   if (this->mpc_Movie != NULL)
   {
      this->mpc_Movie->start();
   }
}
