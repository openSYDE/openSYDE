//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Utility functions for any delegate
   \copyright   Copyright 2019 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_TBLTREDELEGATEUTIL_H
#define C_TBLTREDELEGATEUTIL_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <QPainter>
#include <QModelIndex>
#include <QStyleOptionViewItem>

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_gui
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_TblTreDelegateUtil
{
public:
   static bool h_PaintIcon(QPainter * const opc_Painter, const QStyleOptionViewItem & orc_Option,
                           const QModelIndex & orc_Index);

private:
   C_TblTreDelegateUtil(void);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
