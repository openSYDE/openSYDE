//-----------------------------------------------------------------------------
/*!
   \file
   \brief       A collection of useful functions for graphics items (header)

   \implementation
   project     openSYDE
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     11.08.2016  STW/B.Bayer
   \endimplementation
*/
//-----------------------------------------------------------------------------
#ifndef C_GILICUSTOMFUNCTIONS_H
#define C_GILICUSTOMFUNCTIONS_H

/* -- Includes ------------------------------------------------------------- */
#include <QVariant>
#include <QGraphicsItem>

#include "stwtypes.h"

/* -- Namespace ------------------------------------------------------------ */
namespace stw_opensyde_gui
{
/* -- Global Constants ----------------------------------------------------- */

/* -- Types ---------------------------------------------------------------- */

class C_GiCustomFunctions
{
public:
   static QVariant h_ItemChange(const QGraphicsItem::GraphicsItemChange oe_Change, const QVariant & orc_Value,
                                const QGraphicsItem * const opc_GraphicsItem, bool * const opq_Changed = NULL);
   static void h_AdaptMouseRangePos(QPointF & orc_Pos, const QPointF & orc_Offset = QPointF(0.0, 0.0));

   static const stw_types::float64 hf64_SceneMinBorderSize;
};

/* -- Extern Global Variables ---------------------------------------------- */
} //end of namespace

#endif
