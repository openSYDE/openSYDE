//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       For rendering SVG Images

   \copyright   Copyright 2018 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <QPainter>
#include "C_GiSvgGraphicsItem.h"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw_opensyde_gui;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

/* -- Global Variables ---------------------------------------------------------------------------------------------- */

/* -- Module Global Variables --------------------------------------------------------------------------------------- */

/* -- Module Global Function Prototypes ----------------------------------------------------------------------------- */

/* -- Implementation ------------------------------------------------------------------------------------------------ */

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default constructor

   Set up GUI with all elements.

   \param[in]     orc_ImagePath  File path to svg
   \param[in]     of64_Width     Picture width
   \param[in]     of64_Height    Picture height
   \param[in,out] opc_Parent     Optional pointer to parent
*/
//----------------------------------------------------------------------------------------------------------------------
C_GiSvgGraphicsItem::C_GiSvgGraphicsItem(const QString & orc_ImagePath, const stw_types::float64 of64_Width,
                                         const stw_types::float64 of64_Height, QGraphicsItem * const opc_Parent) :
   QGraphicsRectItem(0.0, 0.0, of64_Width, of64_Height, opc_Parent),
   C_GiBiSizeableItem(of64_Width, of64_Height),
   mc_ImagePath(orc_ImagePath)
{
   if (orc_ImagePath.compare("") == 0)
   {
      this->mpc_SvgRenderer = NULL;
   }
   else
   {
      this->mpc_SvgRenderer = new QSvgRenderer(this->mc_ImagePath);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default destructor

   Clean up.
*/
//----------------------------------------------------------------------------------------------------------------------
C_GiSvgGraphicsItem::~C_GiSvgGraphicsItem(void)
{
   delete this->mpc_SvgRenderer;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set new SVG

   \param[in] orc_ImagePath File path to svg
*/
//----------------------------------------------------------------------------------------------------------------------
void C_GiSvgGraphicsItem::SetSvg(const QString & orc_ImagePath)
{
   delete this->mpc_SvgRenderer;
   if (orc_ImagePath.compare("") == 0)
   {
      this->mpc_SvgRenderer = NULL;
   }
   else
   {
      this->mpc_SvgRenderer = new QSvgRenderer(orc_ImagePath);
   }
   this->update();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Apply new size

   \param[in] of64_Width  New width
   \param[in] of64_Height New height

   \return
   possible return value(s) and description
*/
//----------------------------------------------------------------------------------------------------------------------
void C_GiSvgGraphicsItem::SetNewSize(const stw_types::float64 of64_Width, const stw_types::float64 of64_Height)
{
   this->f64_Width = of64_Width;
   this->f64_Height = of64_Height;
   this->setRect(QRectF(this->pos(), QSizeF(of64_Width, of64_Height)));
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Updates the drawing
*/
//----------------------------------------------------------------------------------------------------------------------
void C_GiSvgGraphicsItem::Redraw(void)
{
   this->setRect(this->GetSizeRect());
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Overridden paint event

   Here: Custom paint operation

   \param[in]     opc_Painter    Pointer to Painter
   \param[in]     opc_Option     Style option
   \param[in]     opc_Widget     Optional widget to paint on
*/
//----------------------------------------------------------------------------------------------------------------------
void C_GiSvgGraphicsItem::paint(QPainter * const opc_Painter, const QStyleOptionGraphicsItem * const opc_Option,
                                QWidget * const opc_Widget)
{
   if (this->mpc_SvgRenderer != NULL)
   {
      //Icon
      this->mpc_SvgRenderer->render(opc_Painter, this->rect());
   }
   Q_UNUSED(opc_Option)
   Q_UNUSED(opc_Widget)
}
