//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Graphics item for showing an optional icon and a background (implementation)

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.hpp"

#include <QPen>
#include <QPainter>

#include "stwtypes.hpp"
#include "C_GiRectPixmap.hpp"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw::opensyde_gui;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

/* -- Global Variables ---------------------------------------------------------------------------------------------- */

/* -- Module Global Variables --------------------------------------------------------------------------------------- */

/* -- Module Global Function Prototypes ----------------------------------------------------------------------------- */

/* -- Implementation ------------------------------------------------------------------------------------------------ */

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default constructor

   Set up GUI with all elements.

   \param[in]     orc_Rect     Initial size of the item
   \param[in,out] opc_Parent   Optional pointer to parent
*/
//----------------------------------------------------------------------------------------------------------------------
C_GiRectPixmap::C_GiRectPixmap(const QRectF & orc_Rect, QGraphicsItem * const opc_Parent) :
   QGraphicsRectItem(orc_Rect, opc_Parent),
   mq_DrawPixmap(false),
   mq_DrawSvg(false),
   mpc_SvgRenderer(NULL)
{
   this->setPen(Qt::NoPen);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default destructor
*/
//----------------------------------------------------------------------------------------------------------------------
C_GiRectPixmap::~C_GiRectPixmap()
{
   delete mpc_SvgRenderer;
   mpc_SvgRenderer = NULL;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Sets the new background color

   \param[in]     orc_Color      New background color
*/
//----------------------------------------------------------------------------------------------------------------------
void C_GiRectPixmap::SetBackgroundColor(const QColor & orc_Color)
{
   this->setBrush(orc_Color);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set svg

   \param[in] orc_Value New svg
*/
//----------------------------------------------------------------------------------------------------------------------
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

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Sets the state of the drawing of the pixmap

   \param[in]     oq_Active      State of the drawing
*/
//----------------------------------------------------------------------------------------------------------------------
void C_GiRectPixmap::SetDrawPixmap(const bool oq_Active)
{
   this->mq_DrawPixmap = oq_Active;
   this->mq_DrawSvg = oq_Active;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Sets a new size

   \param[in]     orc_Size       New size of the rectangle
*/
//----------------------------------------------------------------------------------------------------------------------
void C_GiRectPixmap::SetNewSize(const QSizeF & orc_Size)
{
   this->SetNewRect(QRectF(this->rect().topLeft(), orc_Size));
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Sets a new rectangle

   \param[in]     orc_Rect       New rectangle
*/
//----------------------------------------------------------------------------------------------------------------------
void C_GiRectPixmap::SetNewRect(const QRectF & orc_Rect)
{
   this->setRect(orc_Rect);
   if (this->mq_DrawSvg == false)
   {
      this->mc_PixmapScaled =
         this->mc_Pixmap.scaledToWidth(static_cast<int32_t>(orc_Rect.width()), Qt::FastTransformation);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Painting the checkbox

   \param[in,out] opc_Painter Painter
   \param[in,out] opc_Option  Option
   \param[in,out] opc_Widget  Widget
*/
//----------------------------------------------------------------------------------------------------------------------
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
