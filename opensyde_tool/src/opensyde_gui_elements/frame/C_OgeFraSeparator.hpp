//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Custom draw frame (header)

   Custom draw frame (note: main module description should be in .cpp file)

   \copyright   Copyright 2016 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_OGEFRASEPARATOR_HPP
#define C_OGEFRASEPARATOR_HPP

/* -- Includes ------------------------------------------------------------------------------------------------------ */

#include <QFrame>

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_gui_elements
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_OgeFraSeparator :
   public QFrame
{
   Q_OBJECT

public:
   C_OgeFraSeparator(QWidget * const opc_Parent = NULL);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
