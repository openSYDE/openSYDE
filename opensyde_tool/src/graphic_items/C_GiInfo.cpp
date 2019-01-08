//-----------------------------------------------------------------------------
/*!
   \internal
   \file
   \brief       short description (implementation)

   detailed description

   \implementation
   project     openSYDE
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     13.02.2018  STW/M.Echtler
   \endimplementation
*/
//-----------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------- */
#include "precomp_headers.h"

#include <QPainter>
#include <QStyleOptionGraphicsItem>
#include "C_GiInfo.h"
#include "C_Uti.h"

/* -- Used Namespaces ------------------------------------------------------ */
using namespace stw_types;
using namespace stw_opensyde_gui;
using namespace stw_opensyde_gui_logic;

/* -- Module Global Constants ---------------------------------------------- */

/* -- Types ---------------------------------------------------------------- */

/* -- Global Variables ----------------------------------------------------- */

/* -- Module Global Variables ---------------------------------------------- */

/* -- Module Global Function Prototypes ------------------------------------ */

/* -- Implementation ------------------------------------------------------- */

//-----------------------------------------------------------------------------
/*!
   \brief   Default constructor

   Set up GUI with all elements.

   \param[in]     orc_Rect   Rectangle
   \param[in,out] opc_Parent Optional pointer to parent

   \created     13.02.2018  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
C_GiInfo::C_GiInfo(const QRectF & orc_Rect, QGraphicsItem * const opc_Parent) :
   QGraphicsRectItem(orc_Rect, opc_Parent),
   mc_Text(""),
   mc_BackgroundColor(Qt::transparent),
   mc_TextColor(Qt::transparent),
   ms32_IconSize(0),
   ms32_TextAlignment(static_cast<sint32>(Qt::AlignCenter)),
   mpc_SvgRenderer(NULL)
{
}

//-----------------------------------------------------------------------------
/*!
   \brief   Default destructor

   Clean up.

   \created     15.02.2018  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
C_GiInfo::~C_GiInfo(void)
{
   delete mpc_SvgRenderer;
   mpc_SvgRenderer = NULL;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Set text size

   \param[in] orc_Value    New text size

   \created     13.02.2018  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_GiInfo::SetTextFont(const QFont & orc_Value)
{
   this->mc_TextFont = orc_Value;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Set text

   \param[in] orc_Value New text

   \created     13.02.2018  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_GiInfo::SetText(const QString & orc_Value)
{
   this->mc_Text = orc_Value;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Set pixmap

   \param[in] orc_Value New pixmap

   \created     13.02.2018  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_GiInfo::SetSvg(const QString & orc_Value)
{
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

//-----------------------------------------------------------------------------
/*!
   \brief   Set color

   \param[in] orc_Value New color

   \created     19.02.2018  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_GiInfo::SetBackgroundColor(const QColor & orc_Value)
{
   this->mc_BackgroundColor = orc_Value;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Set color

   \param[in] orc_Value    New color
   \param[in] oc_Alignment New text alignment

   \created     13.02.2018  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_GiInfo::SetTextColor(const QColor & orc_Value, const sint32 os32_Alignment)
{
   this->mc_TextColor = orc_Value;
   this->ms32_TextAlignment = os32_Alignment;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Set icon size

   \param[in] os32_IconSize New icon size

   \created     13.02.2018  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_GiInfo::SetIconSize(const sint32 os32_IconSize)
{
   this->ms32_IconSize = os32_IconSize;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Overwritten paint event slot

   Here: Custom paint operation

   \param[in,out] opc_Event Event identification and information

   \created     13.02.2018  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_GiInfo::paint(QPainter * const opc_Painter, const QStyleOptionGraphicsItem * const opc_Option,
                     QWidget * const opc_Widget)
{
   const QFontMetrics c_Metrics(this->mc_TextFont);
   const sint32 s32_TextOffset = static_cast<sint32>(opc_Option->rect.height() - c_Metrics.height()) / 2L;
   sint32 s32_TextX = opc_Option->rect.topLeft().x() + s32_TextOffset;
   const sint32 s32_TextY = opc_Option->rect.topLeft().y() + s32_TextOffset;
   sint32 s32_TextWidth = opc_Option->rect.width() - (2L * s32_TextOffset);
   const sint32 s32_TextHeight = opc_Option->rect.height() - (2L * s32_TextOffset);

   //Background
   if (this->mc_BackgroundColor.alpha() > 0)
   {
      opc_Painter->save();
      opc_Painter->setBrush(QBrush(this->mc_BackgroundColor));
      opc_Painter->setPen(Qt::NoPen);
      opc_Painter->drawRoundedRect(opc_Option->rect,
                                   std::min(static_cast<float64>(opc_Option->rect.width()) / 2.0,
                                            static_cast<float64>(opc_Option->rect.height()) / 2.0),
                                   static_cast<float64>(opc_Option->rect.height()) / 2.0);
      opc_Painter->restore();
   }

   //Icon
   if (this->mpc_SvgRenderer != NULL)
   {
      const sint32 s32_IconOffset = (opc_Option->rect.height() - this->ms32_IconSize) / 2;
      const sint32 s32_IconRectX = opc_Option->rect.topLeft().x() + s32_IconOffset;
      const sint32 s32_IconRectY = opc_Option->rect.topLeft().y() + s32_IconOffset;
      const QRectF c_IconRect(static_cast<float64>(s32_IconRectX),
                              static_cast<float64>(s32_IconRectY),
                              static_cast<float64>(this->ms32_IconSize),
                              static_cast<float64>(this->ms32_IconSize));
      s32_TextX += this->ms32_IconSize + s32_IconOffset;
      s32_TextWidth -= this->ms32_IconSize;
      this->mpc_SvgRenderer->render(opc_Painter, c_IconRect);
   }
   Q_UNUSED(opc_Widget)

   //Text
   opc_Painter->save();
   opc_Painter->setFont(this->mc_TextFont);
   opc_Painter->setPen(QPen(this->mc_TextColor, this->mc_TextFont.pointSizeF()));
   opc_Painter->drawText(QRect(QPoint(s32_TextX, s32_TextY), QSize(s32_TextWidth,
                                                                   s32_TextHeight)), ms32_TextAlignment,
                         C_Uti::h_AdaptStringToSize(this->mc_Text, c_Metrics, static_cast<float64>(s32_TextWidth)));
   opc_Painter->restore();
}

//-----------------------------------------------------------------------------
/*!
   \brief   Trigger update

   Probably only necessary for animations

   \created     15.02.2018  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_GiInfo::m_Update(void)
{
   this->update();
}
