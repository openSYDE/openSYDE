//-----------------------------------------------------------------------------
/*!
   \file
   \brief       Custom interaction point (header)

   \implementation
   project     openSYDE
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     16.09.2016  STW/B.Bayer
   \endimplementation
*/
//-----------------------------------------------------------------------------
#ifndef C_GILIPOINTINTERACTION_H
#define C_GILIPOINTINTERACTION_H

/* -- Includes ------------------------------------------------------------- */
#include <QGraphicsEllipseItem>

#include "stwtypes.h"

/* -- Namespace ------------------------------------------------------------ */
namespace stw_opensyde_gui
{
/* -- Global Constants ----------------------------------------------------- */

/* -- Types ---------------------------------------------------------------- */

class C_GiPointInteraction :
   public QGraphicsEllipseItem
{
public:
   C_GiPointInteraction(QGraphicsItem * const opc_Parent = NULL);
   ~C_GiPointInteraction();

   virtual stw_types::sintn type() const override;
   bool IsPointInside(const QPointF & orc_Pos) const;
   void UpdateTransform(const QTransform & orc_Transform);

   // The naming of the Qt parameters can't be changed and are not compliant with the naming conventions
   //lint -save -e1960
   virtual void paint(QPainter * const opc_Painter, const QStyleOptionGraphicsItem * const opc_Option,
                      QWidget * const opc_Widget) override;
   //lint -restore

private:
   //Avoid call
   C_GiPointInteraction(const C_GiPointInteraction &);
   C_GiPointInteraction & operator =(const C_GiPointInteraction &);
};

/* -- Extern Global Variables ---------------------------------------------- */
} //end of namespace

#endif
