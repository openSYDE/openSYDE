//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Text browser field for message details (header)

   See cpp file for detailed description

   \copyright   Copyright 2018 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_OGETEBMESSAGEDETAILS_H
#define C_OGETEBMESSAGEDETAILS_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "C_OgeTebContextMenuBase.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_gui_elements
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_OgeTebMessageDetails :
   public C_OgeTebContextMenuBase
{
   Q_OBJECT

public:
   C_OgeTebMessageDetails(QWidget * const opc_Parent = NULL);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
