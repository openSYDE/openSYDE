//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Sorting helper for scene z order
   \copyright   Copyright 2019 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_SEBUNOZETORDERSORTHELPER_H
#define C_SEBUNOZETORDERSORTHELPER_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <QGraphicsItem>

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_gui_logic
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_SebUnoZetOrderSortHelper
{
public:
   C_SebUnoZetOrderSortHelper(void);

   bool operator ()(const QGraphicsItem * const opc_Item1, const QGraphicsItem * const opc_Item2) const;

   static void h_SortZetValues(QList<QGraphicsItem *> & orc_ZetValues);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
