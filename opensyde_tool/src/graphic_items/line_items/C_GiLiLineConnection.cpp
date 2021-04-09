//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Helper line for C_GiLiLine (implementation)

   This line will not be drawn. It is only a helper class.

   \copyright   Copyright 2016 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.h"

#include <QPen>
#include <QCursor>
#include "C_GiLiLineConnection.h"
#include "C_GiBiLineBounding.h"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */

using namespace stw_opensyde_gui;
using namespace stw_types;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

/* -- Global Variables ---------------------------------------------------------------------------------------------- */

/* -- Module Global Variables --------------------------------------------------------------------------------------- */

/* -- Module Global Function Prototypes ----------------------------------------------------------------------------- */

/* -- Implementation ------------------------------------------------------------------------------------------------ */

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Default constructor

   Set up GUI with all elements.

   \param[in]     ors32_LineIndex Index of line
   \param[in,out] opc_Parent      Optional pointer to parent
*/
//----------------------------------------------------------------------------------------------------------------------
C_GiLiLineConnection::C_GiLiLineConnection(const sint32 & ors32_LineIndex, QGraphicsItem * const opc_Parent) :
   C_GiLiSimpleLine(opc_Parent),
   ms32_LineIndex(ors32_LineIndex),
   mf64_InteractionWidth(0.0)
{
   this->setFlag(ItemIsMovable);
   this->setFlag(ItemSendsGeometryChanges);

   this->mc_DefaultCursor = static_cast<QCursor>(Qt::SizeAllCursor);
   this->setCursor(this->mc_DefaultCursor);

   this->setLine(QLineF(0.0, 0.0, 0.0, 0.0));
   this->setPen(QPen(QColor(115, 115, 185), 2., Qt::SolidLine, Qt::SquareCap));
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Default destructor

   Clean up.
*/
//----------------------------------------------------------------------------------------------------------------------
C_GiLiLineConnection::~C_GiLiLineConnection()
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Overwritten set line

   Here: Emit new line coordinates

   \param[in] orc_Line New line
*/
//----------------------------------------------------------------------------------------------------------------------
void C_GiLiLineConnection::AdaptLine(const QLineF & orc_Line)
{
   C_GiLiSimpleLine::setLine(orc_Line);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Set line index

   \param[in] ors32_LineIndex Index of line
*/
//----------------------------------------------------------------------------------------------------------------------
void C_GiLiLineConnection::SetLineIndex(const stw_types::sint32 & ors32_LineIndex)
{
   this->ms32_LineIndex = ors32_LineIndex;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get line index

   \return Index of line
*/
//----------------------------------------------------------------------------------------------------------------------
stw_types::sint32 C_GiLiLineConnection::GetLineIndex(void) const
{
   return this->ms32_LineIndex;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Restore default mouse cursor
*/
//----------------------------------------------------------------------------------------------------------------------
void C_GiLiLineConnection::RestoreDefaultCursor(void)
{
   this->setCursor(this->mc_DefaultCursor);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Change mouse cursor temporarily

   \param[in] orc_TemporaryCursor New mouse cursor
*/
//----------------------------------------------------------------------------------------------------------------------
void C_GiLiLineConnection::SetTemporaryCursor(const QCursor & orc_TemporaryCursor)
{
   this->setCursor(orc_TemporaryCursor);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Set default mouse cursor

   \param[in] orc_Value New default mouse cursor
*/
//----------------------------------------------------------------------------------------------------------------------
void C_GiLiLineConnection::SetDefaultCursor(const QCursor & orc_Value)
{
   this->setCursor(orc_Value);
   this->mc_DefaultCursor = orc_Value;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Overwritten paint event slot

   Here: Avoid drawing the line. This is only a helper class

   \param[in,out] opc_Painter Painter
   \param[in,out] opc_Option  Option
   \param[in,out] opc_Widget  Widget
*/
//----------------------------------------------------------------------------------------------------------------------
//lint -e{9175}  //intentionally no functionality in default implementation
void C_GiLiLineConnection::paint(QPainter * const opc_Painter, const QStyleOptionGraphicsItem * const opc_Option,
                                 QWidget * const opc_Widget)
{
   Q_UNUSED(opc_Painter)
   Q_UNUSED(opc_Option)
   Q_UNUSED(opc_Widget)

   // no need of drawing this line. It is only a helper class
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Return more accurate shape

   \return  The painter path as polygon
*/
//----------------------------------------------------------------------------------------------------------------------
QPainterPath C_GiLiLineConnection::shape() const
{
   stw_opensyde_gui_logic::C_GiBiLineBounding c_LineBounding(this->line(), static_cast<float64>(this->GetWidth()),
                                                             this->mf64_InteractionWidth);
   return c_LineBounding.GetShape();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Find closest point in shape to scene position

   \param[in]  orc_ScenePoint Scene position
   \param[out] orc_Closest    Closest point in shape
*/
//----------------------------------------------------------------------------------------------------------------------
void C_GiLiLineConnection::FindClosestPoint(const QPointF & orc_ScenePoint, QPointF & orc_Closest) const
{
   C_GiBiConnectableItem::h_DistToLine(this->mapToScene(this->line().p1()), this->mapToScene(this->line().p2()),
                                       orc_ScenePoint, NULL, &orc_Closest, NULL);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Find closest point in shape to scene position

   \param[in]  orc_ScenePoint Scene position
   \param[out] orf64_Relative Relative position
*/
//----------------------------------------------------------------------------------------------------------------------
void C_GiLiLineConnection::FindClosestPointRelative(const QPointF & orc_ScenePoint, float64 & orf64_Relative) const
{
   C_GiBiConnectableItem::h_DistToLine(this->mapToScene(this->line().p1()), this->mapToScene(this->line().p2()),
                                       orc_ScenePoint, NULL, NULL, &orf64_Relative);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Overwritten itemChange event slot

   Here: Signal position change

   \param[in] oe_Change Indicator what changed
   \param[in] orc_Value Value corresponding to change

   \return
   new value
*/
//----------------------------------------------------------------------------------------------------------------------
QVariant C_GiLiLineConnection::itemChange(const GraphicsItemChange oe_Change, const QVariant & orc_Value)
{
   return C_GiLiSimpleLine::itemChange(oe_Change, orc_Value);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Set interaction point width

   \param[in] orf64_Value New value
*/
//----------------------------------------------------------------------------------------------------------------------
void C_GiLiLineConnection::SetInteractionWidth(const stw_types::float64 & orf64_Value)
{
   this->prepareGeometryChange();
   this->mf64_InteractionWidth = orf64_Value;
}
