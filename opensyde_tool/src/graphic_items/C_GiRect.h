//-----------------------------------------------------------------------------
/*!
   \file
   \brief       A sizeable variant of QGraphicsRectItem (header)

   \implementation
   project     openSYDE
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     27.10.2016  STW/B.Bayer
   \endimplementation
*/
//-----------------------------------------------------------------------------
#ifndef C_GIRECT_H
#define C_GIRECT_H

/* -- Includes ------------------------------------------------------------- */

#include <QGraphicsRectItem>

#include "C_GiBiSizeableItem.h"

/* -- Namespace ------------------------------------------------------------ */
namespace stw_opensyde_gui
{
/* -- Global Constants ----------------------------------------------------- */

/* -- Types ---------------------------------------------------------------- */

class C_GiRect :
   public QGraphicsRectItem,
   public C_GiBiSizeableItem
{
public:
   C_GiRect(const QRectF & orc_Rect, QGraphicsItem * const opc_Parent = NULL);
   virtual ~C_GiRect(void);

   virtual void Redraw(void) override;
};

/* -- Extern Global Variables ---------------------------------------------- */
} //end of namespace

#endif
