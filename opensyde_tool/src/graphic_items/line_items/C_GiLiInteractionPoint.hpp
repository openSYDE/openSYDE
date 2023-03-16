//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Interactable point for line (header)

   \copyright   Copyright 2016 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_GILIINTERACTIONPOINT_HPP
#define C_GILIINTERACTIONPOINT_HPP

/* -- Includes ------------------------------------------------------------------------------------------------------ */

#include <QObject>
#include <QGraphicsSceneMouseEvent>
#include <QFocusEvent>

#include "C_GiPointInteraction.hpp"
#include "C_GiBiCustomMouseItem.hpp"
#include "stwtypes.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_gui
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
   C_GiLiInteractionPoint(const int32_t & ors32_PointIndex = 0, QGraphicsItem * const opc_Parent = NULL);
   ~C_GiLiInteractionPoint() override;
   int32_t type() const override;

   void SetPointIndex(const int32_t & ors32_PointIndex);
   int32_t GetPointIndex() const;

   void RestoreDefaultCursor(void) override;
   void SetTemporaryCursor(const QCursor & orc_TemporaryCursor) override;
   void SetDefaultCursor(const QCursor & orc_Value) override;

   //The signals keyword is necessary for Qt signal slot functionality
   //lint -save -e1736

Q_SIGNALS:
   //lint -restore
   void SigPointChanged(const int32_t & ors32_PointIndex);

protected:
   QVariant itemChange(const GraphicsItemChange oe_Change, const QVariant & orc_Value) override;

private:
   int32_t ms32_PointIndex;

   //Avoid call
   C_GiLiInteractionPoint(const C_GiLiInteractionPoint &);
   C_GiLiInteractionPoint & operator =(const C_GiLiInteractionPoint &) &; //lint !e1511 //we want to hide the base func.
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
