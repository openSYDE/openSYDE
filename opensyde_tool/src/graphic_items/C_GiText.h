//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Sizeable variant of QGraphicsTextItem (header)

   \copyright   Copyright 2016 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_GITEXT_H
#define C_GITEXT_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */

#include <QGraphicsTextItem>

#include "C_GiBiSizeableItem.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw_opensyde_gui
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_GiText :
   public QGraphicsTextItem,
   public C_GiBiSizeableItem
{
   Q_OBJECT

public:
   C_GiText(const QRectF & orc_Rect, const bool oq_Editable, QGraphicsItem * const opc_Parent = NULL);
   virtual ~C_GiText(void);

   virtual stw_types::sintn type(void) const override;

   virtual void Redraw(void) override;

   virtual QRectF boundingRect(void) const override;

   void SetEditable(const bool oq_Editable);
   void SetText(const QString & orc_Text);
   bool IsEditModeActive(void) const;
   void AutoAdaptSize(void);
   void SetTextInteraction(const bool oq_On, const bool oq_SelectAll = false);

   //The signals keyword is necessary for Qt signal slot functionality
   //lint -save -e1736

Q_SIGNALS:
   //lint -restore
   void SigChangedSize(void);
   void SigTextInteractionModeStateChanged(const bool & orq_On);

protected:
   virtual void keyPressEvent(QKeyEvent * const opc_Event) override;
   virtual void mouseDoubleClickEvent(QGraphicsSceneMouseEvent * const opc_Event) override;
   virtual QVariant itemChange(const GraphicsItemChange oe_Change, const QVariant & orc_Value) override;

private:
   //Avoid call
   C_GiText(const C_GiText &);
   C_GiText & operator =(const C_GiText &); //lint !e1511 //we want to hide the base func.

   bool mq_Editable;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif
