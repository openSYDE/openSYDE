//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       QCustomplot adaption

   Extended functionality for QCustomPlot

   \copyright   Copyright 2021 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.hpp"

#include "stwtypes.hpp"
#include "constants.hpp"
#include "TglUtils.hpp"

#include "C_SyvDaChaPlot.hpp"

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

   \param[in]      oq_RightTriangle           Flag if triangle is a left or right orientated triangle
   \param[in,out]  opc_ParentPlot             Pointer to parent plot
*/
//----------------------------------------------------------------------------------------------------------------------
C_SyvDaChaPlotTriangleItem::C_SyvDaChaPlotTriangleItem(const bool oq_RightTriangle,
                                                       QCustomPlot * const opc_ParentPlot) :
   QCPAbstractItem(opc_ParentPlot),
   pc_Position(createPosition(QLatin1String("position")))
{
   this->pc_Position->setCoords(0.0, 0.0);
   const float64_t f64_WIDTH = 8.0;
   const float64_t f64_Y = 0.0;
   const float64_t f64_X = 0.0;
   // Factor for adjusting the "length" of the triangle
   const float64_t f64_FACTOR_MID_POINT = 0.6;

   if (oq_RightTriangle == true)
   {
      // Lines to form the right orientated triangle
      this->mac_LineArray[0] = QPointF(f64_X, f64_Y - f64_WIDTH);
      this->mac_LineArray[1] = QPointF(f64_X, f64_Y + f64_WIDTH);
      this->mac_LineArray[2] = QPointF(f64_X + (f64_FACTOR_MID_POINT * f64_WIDTH), f64_Y);
   }
   else
   {
      // Lines to form the left orientated triangle
      this->mac_LineArray[0] = QPointF(f64_X, f64_Y - f64_WIDTH);
      this->mac_LineArray[1] = QPointF(f64_X, f64_Y + f64_WIDTH);
      this->mac_LineArray[2] = QPointF(f64_X - (f64_FACTOR_MID_POINT * f64_WIDTH), f64_Y);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default destructor
*/
//----------------------------------------------------------------------------------------------------------------------
C_SyvDaChaPlotTriangleItem::~C_SyvDaChaPlotTriangleItem(void)
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Sets the colors of the cursor tag and its elements

   \param[in]       orc_Color     Color of triangle
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaChaPlotTriangleItem::UpdateColor(const QColor & orc_Color)
{
   this->mc_Color = orc_Color;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Part of the interface QCPAbstractItem as pure virtual function

   \param[in]      orc_Pos             Position for test
   \param[in]      oq_OnlySelectable   Flag if item is only selectable
   \param[in]      opc_Details         Optional pointer to extra details

   \return
   Type of return values, e.g. STW error codes

   \retval   >= 0   Shortest pixel distance of orc_Pos to the object
   \retval   -1.0   Object is either invisible or the distance couldn't be determined or is not selectable
*/
//----------------------------------------------------------------------------------------------------------------------
//lint -e{952,8001,8011,9272}  //name of function and parameters dictated by base class
float64_t C_SyvDaChaPlotTriangleItem::selectTest(const QPointF & orc_Pos, bool oq_OnlySelectable,
                                                 QVariant * const opc_Details) const
{
   Q_UNUSED(orc_Pos)
   Q_UNUSED(oq_OnlySelectable)
   Q_UNUSED(opc_Details)

   // Not necessary in this case
   return -1.0;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Overridden draw event

   Here: Custom paint operation

   \param[in]     opc_Painter    Pointer to Painter
*/
//----------------------------------------------------------------------------------------------------------------------
//lint -e{8001,8011,9272}  //name of function and parameters dictated by base class
void C_SyvDaChaPlotTriangleItem::draw(QCPPainter * const opc_Painter)
{
   const QPointF c_Pos(this->pc_Position->pixelPosition());
   QPointF ac_LineArray[3];
   int32_t s32_LineCounter;
   QPen c_Pen = opc_Painter->pen();
   QBrush c_Brush = opc_Painter->brush();

   c_Pen.setColor(this->mc_Color);
   opc_Painter->setPen(c_Pen);
   c_Brush.setColor(this->mc_Color);
   c_Brush.setStyle(Qt::SolidPattern);
   opc_Painter->setBrush(c_Brush);

   // Adapt the lines for the triangle to current coordinates
   for (s32_LineCounter = 0; s32_LineCounter < 3; ++s32_LineCounter)
   {
      ac_LineArray[s32_LineCounter] = this->mac_LineArray[s32_LineCounter];
      ac_LineArray[s32_LineCounter].setX(ac_LineArray[s32_LineCounter].x() + c_Pos.x());
      ac_LineArray[s32_LineCounter].setY(ac_LineArray[s32_LineCounter].y() + c_Pos.y());
   }

   opc_Painter->drawPolygon(ac_LineArray, 3);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default constructor

   Set up arrow for following an line item at the bottom x axis.

   Oriented at the example project axis-tags-example of QCustomplot

   \param[in,out]  opc_ParentAxis             Pointer to parent axis
   \param[in]      orc_LabelText              String for label of tag
*/
//----------------------------------------------------------------------------------------------------------------------
C_SyvDaChaPlotCursorTag::C_SyvDaChaPlotCursorTag(QCPAxis * const opc_ParentAxis, const QString & orc_LabelText) :
   QObject(opc_ParentAxis),
   mpc_Axis(opc_ParentAxis)
{
   // The dummy tracer serves here as an invisible anchor which always sticks to the bottom side of
   // the axis rect
   this->mpc_Tracer = new QCPItemTracer(this->mpc_Axis->parentPlot());
   this->mpc_Tracer->setVisible(false);
   this->mpc_Tracer->position->setTypeX(QCPItemPosition::ptPlotCoords);
   this->mpc_Tracer->position->setTypeY(QCPItemPosition::ptAxisRectRatio);
   this->mpc_Tracer->position->setAxisRect(this->mpc_Axis->axisRect());
   this->mpc_Tracer->position->setAxes(this->mpc_Axis, NULL);
   this->mpc_Tracer->position->setCoords(0.0, 1.0);

   // the arrow end (head) is set to move along with the dummy tracer by setting it as its parent
   // anchor. Its coordinate system (setCoords) is thus pixels, and this is how the needed horizontal
   // offset for the tag of the second y axis is achieved. This horizontal offset gets dynamically
   // updated in AxisTag::updatePosition. the arrow "start" is simply set to have the "end" as parent
   // anchor. It is given a horizontal offset to the right, which results in a 15 pixel long arrow.
   this->mpc_Marker = new QCPItemLine(this->mpc_Axis->parentPlot());
   this->mpc_Marker->setSelectable(false);
   this->mpc_Marker->setLayer("overlay");
   this->mpc_Marker->setClipToAxisRect(false);
   this->mpc_Marker->setHead(QCPLineEnding::esNone);
   this->mpc_Marker->end->setParentAnchor(this->mpc_Tracer->position);
   this->mpc_Marker->start->setParentAnchor(this->mpc_Marker->end);
   this->mpc_Marker->start->setCoords(0.0, 15.0);

   // The text label is anchored at the arrow start (tail) and has its "position" aligned at the
   // left, and vertically centered to the text label box.
   this->mpc_Label = new QCPItemText(this->mpc_Axis->parentPlot());
   this->mpc_Label->setSelectable(false);
   this->mpc_Label->setLayer("overlay");
   this->mpc_Label->setClipToAxisRect(false);
   this->mpc_Label->setPadding(QMargins(3, 0, 3, 3));
   this->mpc_Label->setPositionAlignment(Qt::AlignHCenter | Qt::AlignTop);
   this->mpc_Label->position->setParentAnchor(this->mpc_Marker->start);
   this->mpc_Label->setText(orc_LabelText);

   // The both triangles left and right next to the label
   this->mpc_TriangleLeft = new C_SyvDaChaPlotTriangleItem(false, this->mpc_Axis->parentPlot());
   this->mpc_TriangleLeft->setLayer("overlay");
   this->mpc_TriangleLeft->setClipToAxisRect(false);
   this->mpc_TriangleLeft->pc_Position->setParentAnchor(this->mpc_Marker->start);
   this->mpc_TriangleLeft->pc_Position->setCoords(-11.0, 8.0);
   this->mpc_TriangleRight = new C_SyvDaChaPlotTriangleItem(true, this->mpc_Axis->parentPlot());
   this->mpc_TriangleRight->setLayer("overlay");
   this->mpc_TriangleRight->setClipToAxisRect(false);
   this->mpc_TriangleRight->pc_Position->setParentAnchor(this->mpc_Marker->start);
   this->mpc_TriangleRight->pc_Position->setCoords(11.0, 8.0);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default destructor
*/
//----------------------------------------------------------------------------------------------------------------------
//lint -e{1540}  no memory leak because of the parent of mpc_Tracer and the Qt memory management
C_SyvDaChaPlotCursorTag::~C_SyvDaChaPlotCursorTag(void)
{
   if (this->mpc_Tracer != NULL)
   {
      this->mpc_Tracer->parentPlot()->removeItem(this->mpc_Tracer);
   }
   if (this->mpc_Marker != NULL)
   {
      this->mpc_Marker->parentPlot()->removeItem(this->mpc_Marker);
   }
   if (this->mpc_Label != NULL)
   {
      this->mpc_Label->parentPlot()->removeItem(this->mpc_Label);
   }
   if (this->mpc_TriangleLeft != NULL)
   {
      this->mpc_TriangleLeft->parentPlot()->removeItem(this->mpc_TriangleLeft);
   }
   if (this->mpc_TriangleRight != NULL)
   {
      this->mpc_TriangleRight->parentPlot()->removeItem(this->mpc_TriangleRight);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Sets the colors of the cursor tag and its elements

   \param[in]       orc_LabelColor     Color of the font of the label
   \param[in]       orc_LineColor      Color of the lines and filled areas
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaChaPlotCursorTag::UpdateColors(const QColor & orc_LabelColor, const QColor & orc_LineColor)
{
   QBrush c_Brush;

   QPen c_Pen = this->mpc_Marker->pen();

   c_Pen.setColor(orc_LineColor);
   this->mpc_Marker->setPen(c_Pen);

   // Color of font
   //lint -e{1938}  static const is guaranteed preinitialized before main
   this->mpc_Label->setColor(orc_LabelColor);

   // Color of lines and filled areas
   c_Brush = this->mpc_Label->brush();
   c_Brush.setStyle(Qt::SolidPattern);
   c_Brush.setColor(orc_LineColor);
   this->mpc_Label->setBrush(c_Brush);
   c_Pen = this->mpc_Label->pen();
   c_Pen.setStyle(Qt::SolidLine);
   c_Pen.setColor(orc_LineColor);
   this->mpc_Label->setPen(c_Pen);

   this->mpc_TriangleLeft->UpdateColor(orc_LineColor);
   this->mpc_TriangleRight->UpdateColor(orc_LineColor);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Updates the position of the cursor tag

   \param[in]       of64_PosHorizontal    New position on X axis
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaChaPlotCursorTag::UpdatePosition(const float64_t of64_PosHorizontal)
{
   // since both the arrow and the text label are chained to the dummy tracer (via anchor
   // parent-child relationships) it is sufficient to update the dummy tracer coordinates. The
   // Horizontal coordinate type was set to ptAxisRectRatio so to keep it aligned at the right side
   // of the axis rect, it is always kept at 1. The vertical coordinate type was set to
   // ptPlotCoordinates of the passed parent axis, so the vertical coordinate is set to the new
   // value.
   this->mpc_Tracer->position->setCoords(of64_PosHorizontal, 1.0);

   // We want the arrow head to be at the same horizontal position as the axis backbone, even if
   // the axis has a certain offset from the axis rect border (like the added second y axis). Thus we
   // set the horizontal pixel position of the arrow end (head) to the axis offset (the pixel
   // distance to the axis rect border). This works because the parent anchor of the arrow end is
   // the dummy tracer, which, as described earlier, is tied to the right axis rect border.
   this->mpc_Marker->end->setCoords(0.0, static_cast<float64_t>(this->mpc_Axis->offset()));
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Sets the visibility of all items which are not in the plot rectangle

   \param[in]       oq_Visible     Flag if items should be visible or not
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaChaPlotCursorTag::SetVisible(const bool oq_Visible)
{
   this->mpc_Label->setVisible(oq_Visible);
   this->mpc_Marker->setVisible(oq_Visible);
   this->mpc_TriangleLeft->setVisible(oq_Visible);
   this->mpc_TriangleRight->setVisible(oq_Visible);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Checks if a specific item is a sub item

   \param[in]       opc_Item     Pointer of the item for checking

   \retval   true    Element is a sub item
   \retval   false   Element is not a sub item
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_SyvDaChaPlotCursorTag::ContainsItem(const QCPAbstractItem * const opc_Item) const
{
   bool q_Return = false;

   if ((this->mpc_Label == opc_Item) ||
       (this->mpc_Marker == opc_Item))
   {
      q_Return = true;
   }

   return q_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default constructor

   \param[in,out]  opc_ParentPlot             Pointer to parent plot
   \param[in]      of64_InitHorizontalPos              Initial position of line and tag
   \param[in]      orc_LabelText              String for label of tag
*/
//----------------------------------------------------------------------------------------------------------------------
C_SyvDaChaPlotCursorItem::C_SyvDaChaPlotCursorItem(QCustomPlot * const opc_ParentPlot,
                                                   const float64_t of64_InitHorizontalPos,
                                                   const QString & orc_LabelText) :
   QCPItemLine(opc_ParentPlot),
   ms32_WidthLineSelected(1),
   ms32_WidthLineDefault(1),
   mq_ItemsNextToPlotVisible(true)
{
   QPen c_Pen;

   this->start->setTypeX(QCPItemPosition::ptPlotCoords);
   this->start->setTypeY(QCPItemPosition::ptAxisRectRatio);
   this->start->setCoords(of64_InitHorizontalPos, 0.0);
   this->end->setTypeX(QCPItemPosition::ptPlotCoords);
   this->end->setTypeY(QCPItemPosition::ptAxisRectRatio);
   this->end->setCoords(of64_InitHorizontalPos, 1.0);

   // Configure the style of the line
   c_Pen = this->pen();
   c_Pen.setWidth(ms32_WidthLineDefault);
   this->setPen(c_Pen);
   c_Pen.setWidth(ms32_WidthLineSelected);
   this->setSelectedPen(c_Pen);

   // Create the tag for the cursor
   this->mpc_CursorTag = new C_SyvDaChaPlotCursorTag(opc_ParentPlot->xAxis, orc_LabelText);
   this->mpc_CursorTag->UpdatePosition(of64_InitHorizontalPos);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default destructor
*/
//----------------------------------------------------------------------------------------------------------------------
C_SyvDaChaPlotCursorItem::~C_SyvDaChaPlotCursorItem(void)
{
   this->mpc_CursorTag->setParent(NULL);
   delete this->mpc_CursorTag;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Sets the colors of the cursor item and its elements

   \param[in]       orc_LabelColor     Color of the font of the label
   \param[in]       orc_LineColor      Color of the lines and filled areas
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaChaPlotCursorItem::UpdateColors(const QColor & orc_LabelColor, const QColor & orc_LineColor)
{
   QPen c_Pen = this->pen();

   c_Pen.setColor(orc_LineColor);
   this->setPen(c_Pen);

   c_Pen = this->selectedPen();
   c_Pen.setColor(orc_LineColor);
   this->setSelectedPen(c_Pen);

   this->mpc_CursorTag->UpdateColors(orc_LabelColor, orc_LineColor);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Updates the position of the cursor and its components

   \param[in]       of64_PosHorizontal    New position on X axis
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaChaPlotCursorItem::UpdatePosition(const float64_t of64_PosHorizontal)
{
   this->start->setCoords(of64_PosHorizontal, 0.0);
   this->end->setCoords(of64_PosHorizontal, 1.0);

   this->mpc_CursorTag->UpdatePosition(of64_PosHorizontal);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Checks if a specific item is a sub item

   \param[in]       opc_Item     Pointer of the item for checking

   \retval   true    Element is a sub item
   \retval   false   Element is not a sub item
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_SyvDaChaPlotCursorItem::ContainsItem(const QCPAbstractItem * const opc_Item) const
{
   return this->mpc_CursorTag->ContainsItem(opc_Item);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Sets the visibility of all items which are not in the plot rectangle

   \param[in]       oq_Visible     Flag if items should be visible or not
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaChaPlotCursorItem::SetItemsNextToPlotVisible(const bool oq_Visible)
{
   if (oq_Visible != this->mq_ItemsNextToPlotVisible)
   {
      this->mq_ItemsNextToPlotVisible = oq_Visible;

      this->mpc_CursorTag->SetVisible(oq_Visible);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default constructor

   Set up GUI with all elements.

   \param[in,out]  opc_Parent             Optional pointer to parent
*/
//----------------------------------------------------------------------------------------------------------------------
C_SyvDaChaPlot::C_SyvDaChaPlot(QWidget * const opc_Parent) :
   QCustomPlot(opc_Parent),
   mq_DragMouseCursorAdapted(false),
   mpc_ClickedCursor(NULL),
   mc_LastItemLineEvent(QEvent::None, QPointF(), QPointF(), QPointF(), Qt::NoButton, Qt::NoButton, Qt::NoModifier)
{
   QPen c_Pen = this->selectionRect()->pen();
   QBrush c_Brush = this->selectionRect()->brush();
   //lint -e{1938}  static const is guaranteed preinitialized before main
   QColor c_BrushColor = mc_STYLE_GUIDE_COLOR_21;

   c_Pen.setStyle(Qt::SolidLine);
   //lint -e{1938}  static const is guaranteed preinitialized before main
   c_Pen.setColor(mc_STYLE_GUIDE_COLOR_21);

   c_Brush.setStyle(Qt::SolidPattern);
   c_BrushColor.setAlpha(51);
   c_Brush.setColor(c_BrushColor);

   this->selectionRect()->setPen(c_Pen);
   this->selectionRect()->setBrush(c_Brush);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default destructor
*/
//----------------------------------------------------------------------------------------------------------------------
//lint -e{1540}  no memory leak because of the parent of mpc_ClickedCursor and the Qt memory management
C_SyvDaChaPlot::~C_SyvDaChaPlot(void)
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Registers a cursor item

   \param[in]       opc_CursorItem     Pointer to a cursor item
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaChaPlot::RegisterCursorItem(C_SyvDaChaPlotCursorItem * const opc_CursorItem)
{
   this->mc_RegisteredCursorItems.append(opc_CursorItem);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Removes a cursor item from the registration and from the plot

   \param[in]       opc_CursorItem     Pointer to a cursor item
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaChaPlot::RemoveCursorItem(C_SyvDaChaPlotCursorItem * const opc_CursorItem)
{
   tgl_assert(this->mc_RegisteredCursorItems.removeOne(opc_CursorItem) == true);
   this->removeItem(opc_CursorItem);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Overwritten mouse press event slot

   Here handling of:
   * Mouse cursor adaption depending on action
   * Detecting and activating moving of an line item as measurement cursor

   \param[in,out] opc_Event Event identification and information
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaChaPlot::mousePressEvent(QMouseEvent * const opc_Event)
{
   if (opc_Event->button() == Qt::LeftButton)
   {
      // Drag mode
      // Check mouse position
      const QList<QCPLayerable *> c_ClickedItems = this->layerableListAt(opc_Event->pos(), false);
      const QCP::SelectionRectMode e_SelectionMode = this->selectionRectMode();

      if (c_ClickedItems.isEmpty() == false)
      {
         bool q_CursorItemClicked = false;
         QCPLayerable * const pc_ItemClicked = c_ClickedItems.first();

         // Check for a cursor item
         if (this->mc_RegisteredCursorItems.size() > 0)
         {
            C_SyvDaChaPlotCursorItem * const pc_Line = dynamic_cast<C_SyvDaChaPlotCursorItem *>(pc_ItemClicked);
            if (pc_Line != NULL)
            {
               // The concrete cursor item found, no further searching necessary
               this->mpc_ClickedCursor = pc_Line;
               q_CursorItemClicked = true;
            }
            else
            {
               QCPAbstractItem * const pc_AbstractItem = dynamic_cast<QCPAbstractItem *>(pc_ItemClicked);

               if (pc_AbstractItem != NULL)
               {
                  // Searching for sub items
                  int32_t s32_Counter;

                  for (s32_Counter = 0UL; s32_Counter < this->mc_RegisteredCursorItems.size(); ++s32_Counter)
                  {
                     C_SyvDaChaPlotCursorItem * const pc_CursorItem = this->mc_RegisteredCursorItems.at(s32_Counter);
                     if (pc_CursorItem->ContainsItem(pc_AbstractItem) == true)
                     {
                        this->mpc_ClickedCursor = pc_CursorItem;
                        q_CursorItemClicked = true;
                        break;
                     }
                  }
               }
            }

            if (q_CursorItemClicked == true)
            {
               this->mpc_ClickedCursor->setSelected(true);
               Q_EMIT (this->SigCursorItemClicked(this->mpc_ClickedCursor));
               this->mc_LastItemLineEvent = *opc_Event;
               QApplication::setOverrideCursor(Qt::SplitHCursor);
            }
         }

         // Check for an axis
         if ((q_CursorItemClicked == false) &&
             (e_SelectionMode == QCP::srmNone))
         {
            QCPAxis * const pc_Axis = dynamic_cast<QCPAxis *>(pc_ItemClicked);

            if (pc_Axis != NULL)
            {
               // ... and its orientation
               if (pc_Axis->orientation() == Qt::Horizontal)
               {
                  QApplication::setOverrideCursor(Qt::SplitHCursor);
               }
               else
               {
                  QApplication::setOverrideCursor(Qt::SplitVCursor);
               }
            }
            else
            {
               QApplication::setOverrideCursor(Qt::SizeAllCursor);
            }
         }
      }
      else if (e_SelectionMode == QCP::srmNone)
      {
         QApplication::setOverrideCursor(Qt::SizeAllCursor);
      }
      else
      {
         // Nothing to do
      }

      this->mq_DragMouseCursorAdapted = true;
   }

   QCustomPlot::mousePressEvent(opc_Event);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Overwritten mouse move event slot

   Here moving of an line item as measurement cursor

   \param[in,out] opc_Event Event identification and information
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaChaPlot::mouseMoveEvent(QMouseEvent * const opc_Event)
{
   if (this->mpc_ClickedCursor != NULL)
   {
      const QPointF c_Delta = opc_Event->pos() - this->mc_LastItemLineEvent.pos();
      // Get the position in pixel coordinates
      const float64_t f64_CurrPosHorizontal = this->xAxis->coordToPixel(this->mpc_ClickedCursor->start->coords().x());
      // Calculate the difference in pixels
      const float64_t f64_NewHorizontal = f64_CurrPosHorizontal + c_Delta.x();
      // Convert to x axis coordinate
      const float64_t f64_NewAxisX = this->xAxis->pixelToCoord(f64_NewHorizontal);

      // Adapt the x position of the cursor
      this->mpc_ClickedCursor->UpdatePosition(f64_NewAxisX);

      // Store the event
      this->mc_LastItemLineEvent = *opc_Event;

      // Redrawing is necessary
      this->replot();

      // Inform about the new position
      Q_EMIT (this->SigCursorItemMovedOnHorizontalAxis(this->mpc_ClickedCursor, f64_NewAxisX));

      opc_Event->accept();
   }
   else
   {
      // Call the original function only if no cursor movement is active
      QCustomPlot::mouseMoveEvent(opc_Event);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Overwritten mouse release event slot

   Here restoring mouse cursors and used flags

   \param[in,out] opc_Event Event identification and information
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaChaPlot::mouseReleaseEvent(QMouseEvent * const opc_Event)
{
   if (this->mpc_ClickedCursor != NULL)
   {
      Q_EMIT (this->SigCursorItemReleased(this->mpc_ClickedCursor));
      this->mpc_ClickedCursor = NULL;
      QApplication::restoreOverrideCursor();
   }
   else if (this->mq_DragMouseCursorAdapted == true)
   {
      this->mq_DragMouseCursorAdapted = false;
      QApplication::restoreOverrideCursor();
   }
   else
   {
      // Nothing to do
   }

   QCustomPlot::mouseReleaseEvent(opc_Event);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Overwritten wheel event

   Changes the zoom orientation configuration in case of a wheel event on an axis
   If the zoom orientation configuration of the hovered axis orientation does not match,
   the zoom orientation will be changed for this event and will be reseted at the end.

   \param[in,out]  opc_Event  Event identification and information
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaChaPlot::wheelEvent(QWheelEvent * const opc_Event)
{
   QCPAxisRect * const pc_AxisRect = this->axisRect();

   Qt::Orientations c_Orientations = Qt::Horizontal | Qt::Vertical;
   bool q_ConfigChanged = false;

   if (pc_AxisRect != NULL)
   {
      if (pc_AxisRect->rangeZoom() != c_Orientations)
      {
         // Save the previous orientation configuration
         c_Orientations = pc_AxisRect->rangeZoom();

         // Check mouse position
         QList<QCPLayerable *> c_ClickedItems = this->layerableListAt(opc_Event->position(), false);
         if (c_ClickedItems.isEmpty() == false)
         {
            QCPLayerable * const pc_ItemClicked = c_ClickedItems.first();
            QCPAxis * const pc_Axis = dynamic_cast<QCPAxis *>(pc_ItemClicked);

            // Check for an axis
            if (pc_Axis != NULL)
            {
               // Check if the current configuration contains the needed orientation of the axis
               if (c_Orientations.testFlag(pc_Axis->orientation()) == false)
               {
                  // Adapt it...
                  q_ConfigChanged = true;
                  if (pc_Axis->orientation() == Qt::Horizontal)
                  {
                     pc_AxisRect->setRangeZoom(Qt::Horizontal);
                  }
                  else
                  {
                     pc_AxisRect->setRangeZoom(Qt::Vertical);
                  }
               }
            }
         }
      }
   }

   QCustomPlot::wheelEvent(opc_Event);

   if (q_ConfigChanged == true)
   {
      // Revert the configuration to the previous state
      if (pc_AxisRect != NULL)
      {
         pc_AxisRect->setRangeZoom(c_Orientations);
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Overridden key press release event slot

   Here: Cancel zoom process with rubberband

   \param[in,out]  opc_KeyEvent  Key event identification and information
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaChaPlot::keyPressEvent(QKeyEvent * const opc_KeyEvent)
{
   QCustomPlot::keyPressEvent(opc_KeyEvent);

   if (opc_KeyEvent->key() == static_cast<int32_t>(Qt::Key_Escape))
   {
      // Abort the zooming by using the rubberband with pressing escape
      if (this->selectionRect()->isActive() == true)
      {
         this->selectionRect()->cancel();
         this->replot();
         // The escape key press shall not be used by parent widgets to avoid a jump back to the main screen
         opc_KeyEvent->accept();
      }
   }
}
