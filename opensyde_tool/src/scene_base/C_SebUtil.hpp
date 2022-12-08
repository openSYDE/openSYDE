//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Common scene utility functions (header)

   See cpp file for detailed description

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_SEBUTIL_H
#define C_SEBUTIL_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */

#include <QObject>
#include <QGraphicsItem>

#include "C_PuiBsLineBase.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_gui_logic
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */
enum E_Alignment
{
   eAL_LEFT,
   eAL_HORIZONTAL_CENTER,
   eAL_RIGHT,
   eAL_TOP,
   eAL_VERTICAL_CENTER,
   eAL_BOTTOM
};

class C_SebUtil :
   public QObject // for using signal functionality
{
   Q_OBJECT

public:
   static QGraphicsItem * h_GetHighestParent(QGraphicsItem * const opc_Item);
   static const QGraphicsItem * h_GetHighestParent(const QGraphicsItem * const opc_Item);
   static bool h_CheckFilePathForImage(QString & orc_FilePath);
   static void h_AddLineOffset(C_PuiBsLineBase & orc_LineBase, const QPointF & orc_Offset);

private:
   C_SebUtil();
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
