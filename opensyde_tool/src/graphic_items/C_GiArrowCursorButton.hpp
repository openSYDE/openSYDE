//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       A button displaying an arrow cursor (header)

   \copyright   Copyright 2016 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_GILIARROWCURSORBUTTON_H
#define C_GILIARROWCURSORBUTTON_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */

#include <QGraphicsSvgItem>
#include <QGraphicsSceneMouseEvent>
#include "C_GiNode.hpp"
#include "C_GiBiCustomToolTip.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_gui
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_GiArrowCursorButton :
   public QGraphicsSvgItem,
   public C_GiBiCustomToolTip
{
   Q_OBJECT

public:
   C_GiArrowCursorButton(QGraphicsItem * const opc_Parent = NULL);
   ~C_GiArrowCursorButton() override;
   void DetachNode(void);
   void AttachToNode(const C_GiNode * const opc_Node);
   int32_t type() const override;
   void GenerateHint(void) override;

   //The signals keyword is necessary for Qt signal slot functionality
   //lint -save -e1736

Q_SIGNALS:
   //lint -restore
   void SigStartConnector(const QPointF & orc_LineStart, const QPointF & orc_SceneTriggerPos);

protected:
   void mousePressEvent(QGraphicsSceneMouseEvent * const opc_Event) override;

private:
   const C_GiNode * mpc_ConnectedNode; ///< Node this arrow cursor is currently attached to

   //Avoid call
   C_GiArrowCursorButton(const C_GiArrowCursorButton &);
   C_GiArrowCursorButton & operator =(const C_GiArrowCursorButton &) &;

   void m_OnNodeChange(void);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
