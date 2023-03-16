//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Smooth scroll area (header)

   Smooth scroll area (note: main module description should be in .cpp file)

   \copyright   Copyright 2016 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_OGESMOOTHSCROLLAREA_H
#define C_OGESMOOTHSCROLLAREA_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */

#include <QScrollArea>

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_gui_elements
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_OgeSmoothScrollArea :
   public QScrollArea
{
   Q_OBJECT

public:
   C_OgeSmoothScrollArea(QWidget * const opc_Parent = NULL);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
