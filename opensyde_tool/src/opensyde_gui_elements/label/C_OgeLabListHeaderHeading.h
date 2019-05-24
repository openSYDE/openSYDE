//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Label for list header heading (header)

   See cpp file for detailed description

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_OGELABLISTHEADERHEADING_H
#define C_OGELABLISTHEADERHEADING_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */

#include <QLabel>

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw_opensyde_gui_elements
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_OgeLabListHeaderHeading :
   public QLabel
{
   Q_OBJECT

public:
   C_OgeLabListHeaderHeading(QWidget * const opc_Parent = NULL);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif
