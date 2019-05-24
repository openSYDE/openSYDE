//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Custom draw frame (header)

   Custom draw frame (note: main module description should be in .cpp file)

   \copyright   Copyright 2016 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_OGEFRAME_H
#define C_OGEFRAME_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */

#include <QFrame>

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw_opensyde_gui_elements
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_OgeFrame :
   public QFrame
{
   Q_OBJECT

public:
   C_OgeFrame(QWidget * const opc_Parent = NULL);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif
