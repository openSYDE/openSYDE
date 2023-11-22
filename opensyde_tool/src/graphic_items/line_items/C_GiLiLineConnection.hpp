//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Helper line for C_GiLiLine (header)

   Line for bus connection (note: main module description should be in .cpp file)

   \copyright   Copyright 2016 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_GILILINECONNECTION_HPP
#define C_GILILINECONNECTION_HPP

/* -- Includes ------------------------------------------------------------------------------------------------------ */

#include "C_GiLiSimpleLine.hpp"
#include "C_GiBiCustomMouseItem.hpp"
#include "C_GiBiConnectableItem.hpp"
#include "stwtypes.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_gui
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_GiLiLineConnection :
   public C_GiBiConnectableItem,
   public C_GiLiSimpleLine,
   public C_GiBiCustomMouseItem

{
   Q_OBJECT

public:
   C_GiLiLineConnection(const int32_t & ors32_LineIndex = 0, QGraphicsItem * const opc_Parent = NULL);
   ~C_GiLiLineConnection() override;
   void AdaptLine(const QLineF & orc_Line);
   void SetLineIndex(const int32_t & ors32_LineIndex);
   int32_t GetLineIndex(void) const;

   void RestoreDefaultCursor(void) override;
   void SetTemporaryCursor(const QCursor & orc_TemporaryCursor) override;
   void SetDefaultCursor(const QCursor & orc_Value) override;

   void paint(QPainter * const opc_Painter, const QStyleOptionGraphicsItem * const opc_Option,
              QWidget * const opc_Widget = NULL) override;

   QPainterPath shape(void) const override;
   void FindClosestPoint(const QPointF & orc_ScenePoint, QPointF & orc_Closest) const override;
   virtual void FindClosestPointRelative(const QPointF & orc_ScenePoint, float64_t & orf64_Relative) const;

   void SetInteractionWidth(const float64_t & orf64_Value);

protected:
   QVariant itemChange(const GraphicsItemChange oe_Change, const QVariant & orc_Value) override;

private:
   int32_t ms32_LineIndex; ///< Index of this line in the segmented line
   float64_t mf64_InteractionWidth;

   //Avoid call
   C_GiLiLineConnection(const C_GiLiLineConnection &);
   C_GiLiLineConnection & operator =(const C_GiLiLineConnection &) &; //lint !e1511 //we want to hide the base func.
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
