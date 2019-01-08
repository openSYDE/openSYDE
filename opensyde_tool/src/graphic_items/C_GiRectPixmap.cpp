//-----------------------------------------------------------------------------
/*!
   \internal
   \file
   \brief       Graphics item for showing an optional icon and a background (implementation)

   \implementation
   project     openSYDE
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     26.06.2017  STW/B.Bayer
   \endimplementation
*/
//-----------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------- */
#include "precomp_headers.h"

#include <QPen>
#include <QPainter>

#include "stwtypes.h"
#include "C_GiRectPixmap.h"

/* -- Used Namespaces ------------------------------------------------------ */
using namespace stw_types;
using namespace stw_opensyde_gui;

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

   \param[in]     orc_Rect     Initial size of the item
   \param[in,out] opc_Parent   Optional pointer to parent

   \created     26.06.2017  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
C_GiRectPixmap::C_GiRectPixmap(const QRectF & orc_Rect, QGraphicsItem * const opc_Parent) :
   QGraphicsRectItem(orc_Rect, opc_Parent),
   mq_DrawPixmap(false),
   mq_DrawSvg(false),
   mpc_SvgRenderer(NULL)
{
   this->setPen(Qt::NoPen);
}

//-----------------------------------------------------------------------------
/*!
   \brief   Default destructor

   \created     26.06.2017  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
C_GiRectPixmap::~C_GiRectPixmap()
{
   delete mpc_SvgRenderer;
   mpc_SvgRenderer = NULL;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Sets the new background color

   \param[in]     orc_Color      New background color

   \created     26.06.2017  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
void C_GiRectPixmap::SetBackgroundColor(const QColor & orc_Color)
{
   this->setBrush(orc_Color);
}

//-----------------------------------------------------------------------------
/*!
   \brief   Sets the icon as pixmap

   \param[in]     orc_Pixmap      New Icon

   \created     26.06.2017  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
void C_GiRectPixmap::SetPixmap(const QPixmap & orc_Pixmap)
{
   this->mc_Pixmap = orc_Pixmap;
   this->mc_PixmapScaled = this->mc_Pixmap.scaledToWidth(
      static_cast<sintn>(this->rect().width()), Qt::SmoothTransformation);
   this->mq_DrawPixmap = true;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Set svg

   \param[in] orc_Value New svg

   \created     13.02.2018  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_GiRectPixmap::SetSvg(const QString & orc_Value)
{
   delete (this->mpc_SvgRenderer);
   if (orc_Value.compare("") != 0)
   {
      this->mpc_SvgRenderer = new QSvgRenderer(orc_Value);
      this->mq_DrawSvg = true;
      //Probably only necessary for animations
      //connect(this->mpc_SvgRenderer, &QSvgRenderer::repaintNeeded, this, &C_GiInfo::m_Update);
   }
   else
   {
      this->mpc_SvgRenderer = NULL;
   }
}

//-----------------------------------------------------------------------------
/*!
   \brief   Sets the state of the drawing of the pixmap

   \param[in]     oq_Active      State of the drawing

   \created     26.06.2017  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
void C_GiRectPixmap::SetDrawPixmap(const bool oq_Active)
{
   this->mq_DrawPixmap = oq_Active;
   this->mq_DrawSvg = oq_Active;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Sets a new size

   \param[in]     orc_Size       New size of the rectangle

   \created     26.06.2017  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
void C_GiRectPixmap::SetNewSize(const QSizeF & orc_Size)
{
   this->SetNewRect(QRectF(this->rect().topLeft(), orc_Size));
}

//-----------------------------------------------------------------------------
/*!
   \brief   Sets a new rectangle

   \param[in]     orc_Rect       New rectangle

   \created     26.06.2017  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
void C_GiRectPixmap::SetNewRect(const QRectF & orc_Rect)
{
   this->setRect(orc_Rect);
   if (this->mq_DrawSvg == false)
   {
      this->mc_PixmapScaled =
         this->mc_Pixmap.scaledToWidth(static_cast<sintn>(orc_Rect.width()), Qt::FastTransformation);
   }
}

//-----------------------------------------------------------------------------
/*!
   \brief   Painting the checkbox

   \param[in,out] opc_Painter Painter
   \param[in,out] opc_Option  Option
   \param[in,out] opc_Widget  Widget

   \created     23.06.2017  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
void C_GiRectPixmap::paint(QPainter * const opc_Painter, const QStyleOptionGraphicsItem * const opc_Option,
                           QWidget * const opc_Widget)
{
   QGraphicsRectItem::paint(opc_Painter, opc_Option, opc_Widget);

   //Icon
   if (this->mq_DrawSvg == true)
   {
      if (this->mpc_SvgRenderer != NULL)
      {
         this->mpc_SvgRenderer->render(opc_Painter, this->rect());
      }
   }
   else
   {
      // Pixmap
      if (this->mq_DrawPixmap == true)
      {
         opc_Painter->drawPixmap(this->rect().topLeft(), this->mc_PixmapScaled);
      }
   }
}
