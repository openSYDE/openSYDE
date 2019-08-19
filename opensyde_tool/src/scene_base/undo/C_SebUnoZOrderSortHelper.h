//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Sorting helper for scene z order
   \copyright   Copyright 2019 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_SEBUNOZORDERSORTHELPER_H
#define C_SEBUNOZORDERSORTHELPER_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <QGraphicsItem>

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw_opensyde_gui_logic
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_SebUnoZOrderSortHelper
{
public:
   C_SebUnoZOrderSortHelper(void);

   bool operator ()(const QGraphicsItem * const opc_Item1, const QGraphicsItem * const opc_Item2) const;

   static void h_SortZValues(QList<QGraphicsItem *> & orc_ZValues);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif
