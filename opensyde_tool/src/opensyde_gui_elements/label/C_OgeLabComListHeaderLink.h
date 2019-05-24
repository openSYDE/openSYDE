//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Label for a link in com list header (header)

   See cpp file for detailed description

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_OGELABCOMLISTHEADERLINK_H
#define C_OGELABCOMLISTHEADERLINK_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */

#include <QLabel>

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw_opensyde_gui_elements
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_OgeLabComListHeaderLink :
   public QLabel
{
   Q_OBJECT

public:
   C_OgeLabComListHeaderLink(QWidget * const opc_Parent = NULL);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif
