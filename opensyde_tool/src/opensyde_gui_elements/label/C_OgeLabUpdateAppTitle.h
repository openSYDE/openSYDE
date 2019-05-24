//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Label for update application title (header)

   See cpp file for detailed description

   \copyright   Copyright 2018 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_OGELABUPDATEAPPTITLE_H
#define C_OGELABUPDATEAPPTITLE_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <QLabel>

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw_opensyde_gui_elements
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_OgeLabUpdateAppTitle :
   public QLabel
{
   Q_OBJECT

public:
   C_OgeLabUpdateAppTitle(QWidget * const opc_Parent = NULL);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif
