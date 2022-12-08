//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Informational Line / Arrow for user (implementation)

   Informational Line / Arrow for user

   \copyright   Copyright 2016 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.hpp"

#include <QPen>
#include <QGraphicsView>

#include "C_GiBiArrow.hpp"
#include "C_GiBiLineBounding.hpp"
#include "C_OgePopUpDialog.hpp"
#include "C_GtGetText.hpp"
#include "C_GiSyBaseWidget.hpp"
#include "C_GiSyLineWidget.hpp"
#include "gitypes.hpp"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw::opensyde_gui;
using namespace stw::opensyde_gui_logic;
using namespace stw::opensyde_gui_elements;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

/* -- Global Variables ---------------------------------------------------------------------------------------------- */

/* -- Module Global Variables --------------------------------------------------------------------------------------- */
const float64_t C_GiBiArrow::mhf64_SHAPE_OFFSET_FACTOR = 4.0;

/* -- Module Global Function Prototypes ----------------------------------------------------------------------------- */

/* -- Implementation ------------------------------------------------------------------------------------------------ */

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Default constructor

   Set up GUI with all elements.

   \param[in]     oru64_Id   Unique ID
   \param[in]     opc_Points Points for line
   \param[in,out] opc_Parent Optional pointer to parent
*/
//----------------------------------------------------------------------------------------------------------------------
C_GiBiArrow::C_GiBiArrow(const uint64_t & oru64_Id, const std::vector<QPointF> * const opc_Points,
                         QGraphicsItem * const opc_Parent) :
   C_GiLiLineGroup(opc_Points, false, opc_Parent),
   C_GiUnique(oru64_Id),
   me_StartArrowHeadType(C_PuiBsLineArrow::E_ArrowHeadType::eNONE),
   me_EndArrowHeadType(C_PuiBsLineArrow::E_ArrowHeadType::eNONE),
   me_LineType(C_PuiBsLineArrow::E_LineType::eSOLID),
   mpc_ArrowHeadStart(NULL),
   mpc_ArrowHeadEnd(NULL),
   //lint -e{1938}  static const is guaranteed preinitialized before main
   mf64_ArrowLength(mhf64_SHAPE_OFFSET_FACTOR),
   //lint -e{1938}  static const is guaranteed preinitialized before main
   mf64_ArrowHeight(mhf64_SHAPE_OFFSET_FACTOR)
{
   this->setFlag(ItemIsSelectable);
   this->setFlag(ItemIsMovable);

   mpc_ArrowHeadStart = new QGraphicsPathItem(this);
   mpc_ArrowHeadStart->setBrush(Qt::black);
   mpc_ArrowHeadStart->setCursor(Qt::SizeAllCursor);
   this->addToGroup(mpc_ArrowHeadStart);

   mpc_ArrowHeadEnd = new QGraphicsPathItem(this);
   mpc_ArrowHeadEnd->setBrush(Qt::black);
   mpc_ArrowHeadEnd->setCursor(Qt::SizeAllCursor);
   this->addToGroup(mpc_ArrowHeadEnd);

   //Default
   this->mpc_LinePath->SetLineStyle(me_LineType);
   this->C_GiBiArrow::SetWidth(1);
   this->SetColor(Qt::black);
   //Z value
   if (opc_Points != NULL)
   {
      this->setZValue(mf64_ZORDER_INIT_LINE_ARROW);
   }

   connect(this, &C_GiBiArrow::SigChangedGraphic, this, &C_GiBiArrow::m_GenerateArrows);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Default destructor
*/
//----------------------------------------------------------------------------------------------------------------------
//lint -e{1540}  no memory leak because of the parent of mpc_ArrowHeadStart and -End and the Qt memory management
C_GiBiArrow::~C_GiBiArrow(void)
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Shape including arrow head(s)

   \return
   shape of arrow
*/
//----------------------------------------------------------------------------------------------------------------------
QPainterPath C_GiBiArrow::shape() const
{
   const float64_t f64_AdaptedWidth = static_cast<float64_t>(this->GetWidth()) * mhf64_SHAPE_OFFSET_FACTOR;
   float64_t f64_InteractioWidth = 0.0;

   //Copy points for interface
   QVector<QPointF> c_Points;
   c_Points.reserve(this->mc_Points.size());
   for (int32_t s32_ItPoint = 0; s32_ItPoint < this->mc_Points.size(); ++s32_ItPoint)
   {
      c_Points.push_back(this->mc_Points[s32_ItPoint]->pos());
      f64_InteractioWidth = this->mc_Points[s32_ItPoint]->sceneBoundingRect().width();
   }

   C_GiBiLineBounding c_LineBounding(c_Points, f64_AdaptedWidth, f64_InteractioWidth);

   return c_LineBounding.GetShape();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Custom set width

   \param[in] ors32_Width New width
*/
//----------------------------------------------------------------------------------------------------------------------
void C_GiBiArrow::SetWidth(const int32_t & ors32_Width)
{
   C_GiLiLineGroup::SetWidth(ors32_Width);
   mf64_ArrowLength = mhf64_SHAPE_OFFSET_FACTOR * static_cast<float64_t>(ors32_Width);
   mf64_ArrowHeight = mhf64_SHAPE_OFFSET_FACTOR * static_cast<float64_t>(ors32_Width);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Returns the type of this item

   \return  ID
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_GiBiArrow::type() const
{
   return ms32_GRAPHICS_ITEM_LINE_ARROW;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Open style dialog

   \param[in] oq_DarkMode Optional dark mode flag

   \retval true   style dialog was accepted
   \retval false  style dialog was rejected
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_GiBiArrow::OpenStyleDialog(const bool oq_DarkMode)
{
   bool q_Retval;

   QGraphicsView * const pc_View = this->scene()->views().at(0);

   const QPointer<C_OgePopUpDialog> c_New = new C_OgePopUpDialog(pc_View, pc_View);
   C_GiSyBaseWidget * const pc_Dialog = new C_GiSyBaseWidget(*c_New, C_GtGetText::h_GetText("Line/Arrow"), oq_DarkMode);
   C_GiSyLineWidget * const pc_SettingsWidget = new C_GiSyLineWidget(C_GiSyLineWidget::E_Type::eLINE, *pc_Dialog);

   pc_SettingsWidget->SetLineColor(this->GetColor());
   pc_SettingsWidget->SetLineWidth(this->GetWidth());
   pc_SettingsWidget->SetLineStyle(this->me_LineType);
   pc_SettingsWidget->SetStartArrow(this->me_StartArrowHeadType);
   pc_SettingsWidget->SetEndArrow(this->me_EndArrowHeadType);

   if (c_New->exec() == static_cast<int32_t>(QDialog::Accepted))
   {
      this->ApplyStyle(pc_SettingsWidget->GetLineColor(),
                       pc_SettingsWidget->GetLineWidth(),
                       pc_SettingsWidget->GetLineStyle(),
                       pc_SettingsWidget->GetStartArrow(),
                       pc_SettingsWidget->GetEndArrow());
      q_Retval = true;
   }
   else
   {
      q_Retval = false;
   }
   if (c_New != NULL)
   {
      c_New->HideOverlay();
      c_New->deleteLater();
   }
   return q_Retval; //lint !e429  //no memory leak because of the parent of pc_Dialog and the Qt memory management
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Copy the style of the provided element

   Warning: Only expected to work if the provided item is of the same type as this element

   \param[in] opc_GuidelineItem Detailed input parameter description
*/
//----------------------------------------------------------------------------------------------------------------------
void C_GiBiArrow::CopyStyle(const QGraphicsItem * const opc_GuidelineItem)
{
   const C_GiBiArrow * const pc_Item = dynamic_cast<const C_GiBiArrow * const>(opc_GuidelineItem);

   if (pc_Item != NULL)
   {
      this->ApplyStyle(pc_Item->GetColor(), pc_Item->GetWidth(), pc_Item->GetLineType(),
                       pc_Item->GetStartArrowHeadType(), pc_Item->GetEndArrowHeadType());
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Apply style

   \param[in] orc_LineColor line color
   \param[in] os32_Width     line width
   \param[in] oe_LineType   line type
   \param[in] oe_StartArrow start arrow head type
   \param[in] oe_EndArrow   end arrow head type
*/
//----------------------------------------------------------------------------------------------------------------------
void C_GiBiArrow::ApplyStyle(const QColor & orc_LineColor, const int32_t os32_Width,
                             const stw::opensyde_gui_logic::C_PuiBsLineArrow::E_LineType oe_LineType,
                             const stw::opensyde_gui_logic::C_PuiBsLineArrow::E_ArrowHeadType oe_StartArrow,
                             const stw::opensyde_gui_logic::C_PuiBsLineArrow::E_ArrowHeadType oe_EndArrow)
{
   this->SetColor(orc_LineColor);
   this->SetWidth(os32_Width);
   this->SetLineType(oe_LineType);
   this->SetStartArrowHeadType(oe_StartArrow);
   this->SetEndArrowHeadType(oe_EndArrow);
   this->TriggerSigChangedGraphic();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get start arrow head type

   \return
   Current value
*/
//----------------------------------------------------------------------------------------------------------------------
stw::opensyde_gui_logic::C_PuiBsLineArrow::E_ArrowHeadType C_GiBiArrow::GetStartArrowHeadType() const
{
   return this->me_StartArrowHeadType;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get end arrow head type

   \return
   Current value
*/
//----------------------------------------------------------------------------------------------------------------------
stw::opensyde_gui_logic::C_PuiBsLineArrow::E_ArrowHeadType C_GiBiArrow::GetEndArrowHeadType() const
{
   return this->me_EndArrowHeadType;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get line type

   \return
   Current value
*/
//----------------------------------------------------------------------------------------------------------------------
stw::opensyde_gui_logic::C_PuiBsLineArrow::E_LineType C_GiBiArrow::GetLineType() const
{
   return this->me_LineType;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Set start arrow head type

   \param[in] ore_New New value
*/
//----------------------------------------------------------------------------------------------------------------------
void C_GiBiArrow::SetStartArrowHeadType(const stw::opensyde_gui_logic::C_PuiBsLineArrow::E_ArrowHeadType & ore_New)
{
   this->me_StartArrowHeadType = ore_New;
   m_GenerateArrows();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Set end arrow head type

   \param[in] ore_New New value
*/
//----------------------------------------------------------------------------------------------------------------------
void C_GiBiArrow::SetEndArrowHeadType(const stw::opensyde_gui_logic::C_PuiBsLineArrow::E_ArrowHeadType & ore_New)
{
   this->me_EndArrowHeadType = ore_New;
   m_GenerateArrows();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Set line type

   \param[in] ore_New New value
*/
//----------------------------------------------------------------------------------------------------------------------
void C_GiBiArrow::SetLineType(const stw::opensyde_gui_logic::C_PuiBsLineArrow::E_LineType & ore_New)
{
   this->me_LineType = ore_New;
   this->mpc_LinePath->SetLineStyle(me_LineType);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Check where interaction point placement is for the specified type

   \param[in] ore_Type Arrow head type

   \return
   true:  interaction point not at end
   false: interaction point at end
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_GiBiArrow::h_HasOffsetInteractionPoint(const C_PuiBsLineArrow::E_ArrowHeadType & ore_Type)
{
   return (ore_Type == C_PuiBsLineArrow::E_ArrowHeadType::eRECT) ||
          (ore_Type == C_PuiBsLineArrow::E_ArrowHeadType::eCIRCLE);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Overwritten mouse press event slot

   \param[in,out] opc_Event Event identification and information
*/
//----------------------------------------------------------------------------------------------------------------------
void C_GiBiArrow::mousePressEvent(QGraphicsSceneMouseEvent * const opc_Event)
{
   if (opc_Event->button() == Qt::LeftButton)
   {
      if (this->me_ActiveResizeMode != eALL)
      {
         if (this->mpc_ArrowHeadStart->isUnderMouse() == true)
         {
            this->me_ActiveResizeMode = eLINE;
            this->ms32_ActiveItemIndex = 0;
         }
         if (this->mpc_ArrowHeadEnd->isUnderMouse() == true)
         {
            this->me_ActiveResizeMode = eLINE;
            this->ms32_ActiveItemIndex = this->mc_Points.size() - 2;
         }
      }
   }
   C_GiLiLineGroup::mousePressEvent(opc_Event);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Generate polygon for arrows
*/
//----------------------------------------------------------------------------------------------------------------------
void C_GiBiArrow::m_GenerateArrows()
{
   const QVector<C_GiLiLineConnection *> c_Lines = this->GetLines();

   if (c_Lines.size() > 0)
   {
      m_GenerateArrow(this->mpc_ArrowHeadStart, this->me_StartArrowHeadType, c_Lines[0], true);
      m_GenerateArrow(this->mpc_ArrowHeadEnd, this->me_EndArrowHeadType, c_Lines[c_Lines.size() - 1], false);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Load from internal data

   \param[in] orc_Data Line data
*/
//----------------------------------------------------------------------------------------------------------------------
void C_GiBiArrow::m_LoadFromData(const C_PuiBsLineArrow & orc_Data)
{
   this->m_LoadBasicData(orc_Data);
   this->SetLineType(orc_Data.e_LineType);
   this->SetStartArrowHeadType(orc_Data.e_StartArrowHeadType);
   this->SetEndArrowHeadType(orc_Data.e_EndArrowHeadType);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Update internal data

   \param[in] orc_Data line data
*/
//----------------------------------------------------------------------------------------------------------------------
void C_GiBiArrow::m_UpdateData(C_PuiBsLineArrow & orc_Data) const
{
   this->m_UpdateBasicData(orc_Data);
   orc_Data.e_EndArrowHeadType = this->me_EndArrowHeadType;
   orc_Data.e_StartArrowHeadType = this->me_StartArrowHeadType;
   orc_Data.e_LineType = this->me_LineType;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Generate polygon for arrow

   \param[in,out] opc_ArrowItem     Arrow item to fill
   \param[in]     ore_ArrowHeadType Arrow head type to generate
   \param[in]     opc_Conn          Connect to align arrow to
   \param[in]     orq_Start         Flag if line start or end
*/
//----------------------------------------------------------------------------------------------------------------------
void C_GiBiArrow::m_GenerateArrow(QGraphicsPathItem * const opc_ArrowItem,
                                  const C_PuiBsLineArrow::E_ArrowHeadType & ore_ArrowHeadType,
                                  const C_GiLiLineConnection * const opc_Conn, const bool & orq_Start)
{
   if (opc_ArrowItem != NULL)
   {
      const QColor c_Color = this->GetColor();
      QPainterPath c_Path;
      QPolygonF c_Poly;
      QRectF c_Rect;
      QPen c_Pen;
      int32_t s32_Width;
      const QPointF c_LineStart = this->mapFromScene(opc_Conn->mapToScene(opc_Conn->GetP1()));
      const QPointF c_LineEnd = this->mapFromScene(opc_Conn->mapToScene(opc_Conn->GetP2()));
      const QPointF c_LineDiff = c_LineEnd - c_LineStart;
      const QPointF c_AdaptedLineDiff = C_GiBiLineBounding::h_AdaptVecToWidth(c_LineDiff, this->mf64_ArrowLength);
      const QPointF c_ArrowWingVector =
         C_GiBiLineBounding::h_AdaptVecToWidth(C_GiBiLineBounding::h_GetPerpendicular(c_LineDiff),
                                               this->mf64_ArrowHeight / 2.0);
      const QPointF c_HalfAdaptedLineDiff =
         C_GiBiLineBounding::h_AdaptVecToWidth(c_LineDiff, this->mf64_ArrowLength / 2.0);
      QPointF c_WidthAdaptedLineDiff;
      QPointF c_WidthAdaptedWingVector;
      //Important arrow bounding points
      QPointF c_ArrowStart;
      QPointF c_ArrowEnd;
      QPointF c_ArrowLeft;
      QPointF c_ArrowRight;
      if (orq_Start == true)
      {
         c_ArrowStart = c_LineStart + c_AdaptedLineDiff;
         c_ArrowEnd = c_LineStart;
         this->mpc_LinePath->SetOffsetStart(0.0);
      }
      else
      {
         c_ArrowStart = c_LineEnd - c_AdaptedLineDiff;
         c_ArrowEnd = c_LineEnd;
         this->mpc_LinePath->SetOffsetEnd(0.0);
      }
      c_ArrowLeft = c_ArrowStart + c_ArrowWingVector;
      c_ArrowRight = c_ArrowStart - c_ArrowWingVector;
      switch (ore_ArrowHeadType)
      {
      case C_PuiBsLineArrow::E_ArrowHeadType::eNORMAL:
         if (orq_Start == true)
         {
            this->mpc_LinePath->SetOffsetStart(this->mf64_ArrowLength / 2.0);
         }
         else
         {
            this->mpc_LinePath->SetOffsetEnd(this->mf64_ArrowLength / 2.0);
         }
         c_Poly.append(c_ArrowLeft);
         c_Poly.append(c_ArrowRight);
         c_Poly.append(c_ArrowEnd);
         //Repeat start to finish polygon
         c_Poly.append(c_ArrowLeft);
         c_Path.addPolygon(c_Poly);
         //Set pen style
         c_Pen = opc_ArrowItem->pen();
         c_Pen.setJoinStyle(Qt::MiterJoin);
         c_Pen.setCapStyle(Qt::FlatCap);
         c_Pen.setColor(Qt::transparent);
         opc_ArrowItem->setPen(c_Pen);
         opc_ArrowItem->setBrush(c_Color);
         break;
      case C_PuiBsLineArrow::E_ArrowHeadType::eRECT:
         c_Poly.append(c_ArrowEnd + c_ArrowWingVector);
         c_Poly.append(c_ArrowEnd - c_HalfAdaptedLineDiff);
         c_Poly.append(c_ArrowEnd - c_ArrowWingVector);
         c_Poly.append(c_ArrowEnd + c_HalfAdaptedLineDiff);
         //Repeat start to finish polygon
         c_Poly.append(c_ArrowEnd + c_ArrowWingVector);
         c_Path.addPolygon(c_Poly);
         //Set pen style
         c_Pen = opc_ArrowItem->pen();
         c_Pen.setJoinStyle(Qt::MiterJoin);
         c_Pen.setCapStyle(Qt::FlatCap);
         c_Pen.setColor(Qt::transparent);
         opc_ArrowItem->setPen(c_Pen);
         opc_ArrowItem->setBrush(c_Color);
         break;
      case C_PuiBsLineArrow::E_ArrowHeadType::eHALF:
         if (orq_Start == true)
         {
            this->mpc_LinePath->SetOffsetStart(this->mf64_ArrowLength / 2.0);
         }
         else
         {
            this->mpc_LinePath->SetOffsetEnd(this->mf64_ArrowLength / 2.0);
         }
         c_Poly.append(c_ArrowLeft);
         if (orq_Start == true)
         {
            c_Poly.append(c_ArrowStart - c_HalfAdaptedLineDiff);
         }
         else
         {
            c_Poly.append(c_ArrowStart + c_HalfAdaptedLineDiff);
         }
         c_Poly.append(c_ArrowRight);
         c_Poly.append(c_ArrowEnd);
         //Repeat start to finish polygon
         c_Poly.append(c_ArrowLeft);
         c_Path.addPolygon(c_Poly);
         //Set pen style
         c_Pen = opc_ArrowItem->pen();
         c_Pen.setJoinStyle(Qt::MiterJoin);
         c_Pen.setCapStyle(Qt::FlatCap);
         c_Pen.setColor(Qt::transparent);
         opc_ArrowItem->setPen(c_Pen);
         opc_ArrowItem->setBrush(c_Color);
         break;
      case C_PuiBsLineArrow::E_ArrowHeadType::eCIRCLE:
         //Center of rect
         c_Rect.setX(c_ArrowEnd.x() - (this->mf64_ArrowLength / 2.0));
         c_Rect.setY(c_ArrowEnd.y() - (this->mf64_ArrowHeight / 2.0));
         c_Rect.setHeight(this->mf64_ArrowHeight);
         c_Rect.setWidth(this->mf64_ArrowLength);
         c_Path.addEllipse(c_Rect);
         //Set pen style
         c_Pen = opc_ArrowItem->pen();
         c_Pen.setColor(Qt::transparent);
         opc_ArrowItem->setPen(c_Pen);
         opc_ArrowItem->setBrush(c_Color);
         break;
      case C_PuiBsLineArrow::E_ArrowHeadType::eLINE:
         s32_Width = this->GetWidth();
         //Offsets to include pen width and still half a 45 degree angle
         c_WidthAdaptedLineDiff = C_GiBiLineBounding::h_AdaptVecToWidth(c_LineDiff, static_cast<float64_t>(s32_Width));
         c_WidthAdaptedWingVector =
            C_GiBiLineBounding::h_AdaptVecToWidth(c_ArrowWingVector, static_cast<float64_t>(s32_Width) / 2.0);
         if (orq_Start == true)
         {
            this->mpc_LinePath->SetOffsetStart(static_cast<float64_t>(s32_Width));
         }
         else
         {
            this->mpc_LinePath->SetOffsetEnd(static_cast<float64_t>(s32_Width));
         }
         //Adapt for pen width
         c_Poly.append(c_ArrowLeft - c_WidthAdaptedWingVector);
         if (orq_Start == true)
         {
            c_ArrowEnd = c_ArrowEnd + c_WidthAdaptedLineDiff;
         }
         else
         {
            c_ArrowEnd = c_ArrowEnd - c_WidthAdaptedLineDiff;
         }
         c_Poly.append(c_ArrowEnd);
         c_Poly.append(c_ArrowRight + c_WidthAdaptedWingVector);
         //No repeat start to explicitly not finish polygon
         c_Path.addPolygon(c_Poly);
         //Deactivate brush
         opc_ArrowItem->setBrush(Qt::NoBrush);
         //Set pen to width
         c_Pen = opc_ArrowItem->pen();
         c_Pen.setWidth(s32_Width);
         c_Pen.setJoinStyle(Qt::MiterJoin);
         c_Pen.setCapStyle(Qt::RoundCap);
         c_Pen.setColor(c_Color);
         opc_ArrowItem->setPen(c_Pen);
         break;
      case C_PuiBsLineArrow::E_ArrowHeadType::eNONE:
      default:
         //No arrow
         if (orq_Start == true)
         {
            this->mpc_LinePath->SetOffsetStart(0.0);
         }
         else
         {
            this->mpc_LinePath->SetOffsetEnd(0.0);
         }
         break;
      }
      opc_ArrowItem->setPath(c_Path);
   }
   //lint -e{429,593}  no memory leak because of the parent of opc_ArrowItem and the Qt memory management
}
