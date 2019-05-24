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
#include "C_GiNode.h"
#include "C_GiBiCustomToolTip.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw_opensyde_gui
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
   virtual ~C_GiArrowCursorButton();
   void DetachNode(void);
   void AttachToNode(const C_GiNode * const opc_Node);
   virtual stw_types::sintn type() const override;
   void UpdateTransform(const QTransform & orc_Transform) const;
   virtual void GenerateHint(void);

   //The signals keyword is necessary for Qt signal slot functionality
   //lint -save -e1736

Q_SIGNALS:
   //lint -restore
   void StartConnector(const QPointF & orc_LineStart, const QPointF & orc_SceneTriggerPos);

protected:
   // The naming of the Qt parameters can't be changed and are not compliant with the naming conventions
   //lint -save -e1960
   virtual void mousePressEvent(QGraphicsSceneMouseEvent * const opc_Event) override;
   //lint -restore

private:
   const C_GiNode * mpc_ConnectedNode; ///< Node this arrow cursor is currently attached to

   //Avoid call
   C_GiArrowCursorButton(const C_GiArrowCursorButton &);
   C_GiArrowCursorButton & operator =(const C_GiArrowCursorButton &);

   void m_OnNodeChange(void);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif
