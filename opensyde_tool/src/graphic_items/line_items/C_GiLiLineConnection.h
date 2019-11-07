//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Helper line for C_GiLiLine (header)

   Line for bus connection (note: main module description should be in .cpp file)

   \copyright   Copyright 2016 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_GILILINECONNECTION_H
#define C_GILILINECONNECTION_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */

#include "C_GiLiSimpleLine.h"
#include "C_GiBiCustomMouseItem.h"
#include "C_GiBiConnectableItem.h"
#include "stwtypes.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw_opensyde_gui
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
   C_GiLiLineConnection(const stw_types::sint32 & ors32_LineIndex, QGraphicsItem * const opc_Parent = NULL);
   virtual ~C_GiLiLineConnection();
   void AdaptLine(const QLineF & orc_Line);
   void SetLineIndex(const stw_types::sint32 & ors32_LineIndex);
   stw_types::sint32 GetLineIndex(void) const;

   virtual void RestoreDefaultCursor(void);
   virtual void SetTemporaryCursor(const QCursor & orc_TemporaryCursor) override;
   virtual void SetDefaultCursor(const QCursor & orc_Value) override;

   // The naming of the Qt parameters can't be changed and are not compliant with the naming conventions,
   // and default parameters are identical.
   //lint -save -e1960 -e1735
   virtual void paint(QPainter * const opc_Painter, const QStyleOptionGraphicsItem * const opc_Option,
                      QWidget * const opc_Widget = NULL) override;
   //lint -restore
   virtual QPainterPath shape(void) const;
   virtual void FindClosestPoint(const QPointF & orc_ScenePoint, QPointF & orc_Closest) const;
   virtual void FindClosestPointRelative(const QPointF & orc_ScenePoint, stw_types::float64 & orf64_Relative) const;

   void SetInteractionWidth(const stw_types::float64 & orf64_Value);

protected:
   // The naming of the Qt parameters can't be changed and are not compliant with the naming conventions
   //lint -save -e1960
   virtual QVariant itemChange(const GraphicsItemChange oe_Change, const QVariant & orc_Value) override;
   //lint -restore

private:
   stw_types::sint32 ms32_LineIndex; ///< Index of this line in the segmented line
   stw_types::float64 mf64_InteractionWidth;

   //Avoid call
   C_GiLiLineConnection(const C_GiLiLineConnection &);
   C_GiLiLineConnection & operator =(const C_GiLiLineConnection &);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif
