//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Label for user nodes not existing (header)

   See cpp file for detailed description

   \copyright   Copyright 2020 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_OGELABTOPOLOGYTOOLBOXUSERNODES_H
#define C_OGELABTOPOLOGYTOOLBOXUSERNODES_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <QLabel>

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw_opensyde_gui_elements
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_OgeLabTopologyToolboxUserNodes :
   public QLabel
{
   Q_OBJECT

public:
   C_OgeLabTopologyToolboxUserNodes(QWidget * const opc_Parent = NULL);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif
