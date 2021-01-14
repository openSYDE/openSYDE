//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Interactable point for line (header)

   \copyright   Copyright 2016 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_GILIINTERACTIONPOINT_H
#define C_GILIINTERACTIONPOINT_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */

#include <QObject>
#include <QGraphicsSceneMouseEvent>
#include <QFocusEvent>

#include "C_GiPointInteraction.h"
#include "C_GiBiCustomMouseItem.h"
#include "stwtypes.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw_opensyde_gui
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_GiLiInteractionPoint :
   public QObject,
   public C_GiPointInteraction,
   public C_GiBiCustomMouseItem
{
   Q_OBJECT

public:
   C_GiLiInteractionPoint(const stw_types::sint32 & ors32_PointIndex = 0, QGraphicsItem * const opc_Parent = NULL);
   virtual ~C_GiLiInteractionPoint();
   virtual stw_types::sintn type() const override;

   void SetPointIndex(const stw_types::sint32 & ors32_PointIndex);
   stw_types::sint32 GetPointIndex() const;

   virtual void RestoreDefaultCursor(void);
   virtual void SetTemporaryCursor(const QCursor & orc_TemporaryCursor) override;
   virtual void SetDefaultCursor(const QCursor & orc_Value) override;

   //The signals keyword is necessary for Qt signal slot functionality
   //lint -save -e1736

Q_SIGNALS:
   //lint -restore
   void PointChanged(const stw_types::sint32 & ors32_PointIndex);

protected:
   virtual QVariant itemChange(const GraphicsItemChange oe_Change, const QVariant & orc_Value) override;

private:
   stw_types::sint32 ms32_PointIndex;

   //Avoid call
   C_GiLiInteractionPoint(const C_GiLiInteractionPoint &);
   C_GiLiInteractionPoint & operator =(const C_GiLiInteractionPoint &); //lint !e1511 //we want to hide the base func.
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif
