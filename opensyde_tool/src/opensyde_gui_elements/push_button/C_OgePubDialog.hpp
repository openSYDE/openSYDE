//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Standard dialog button (header)

   Standard dialog button (note: main module description should be in .cpp file)

   \copyright   Copyright 2016 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_OGEPUBDIALOG_H
#define C_OGEPUBDIALOG_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */

#include <QPushButton>

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_gui_elements
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_OgePubDialog :
   public QPushButton
{
   Q_OBJECT

public:
   C_OgePubDialog(QWidget * const opc_Parent = NULL);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
