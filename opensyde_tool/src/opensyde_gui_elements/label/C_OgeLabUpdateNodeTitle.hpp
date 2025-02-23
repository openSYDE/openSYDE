//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Label for node name as title in update package (header)

   See cpp file for detailed description

   \copyright   Copyright 2018 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_OGELABUPDATENODETITLE_HPP
#define C_OGELABUPDATENODETITLE_HPP

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <QLabel>

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_gui_elements
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_OgeLabUpdateNodeTitle :
   public QLabel
{
   Q_OBJECT

public:
   C_OgeLabUpdateNodeTitle(QWidget * const opc_Parent = NULL);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
