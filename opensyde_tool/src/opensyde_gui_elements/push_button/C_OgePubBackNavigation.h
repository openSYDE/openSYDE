//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       System definition lists back navigation
   \copyright   Copyright 2020 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_OGEPUBBACKNAVIGATION_H
#define C_OGEPUBBACKNAVIGATION_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "C_OgePubSvgIconWithTextBase.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw_opensyde_gui_elements
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_OgePubBackNavigation :
   public C_OgePubSvgIconWithTextBase
{
public:
   C_OgePubBackNavigation(QWidget * const opc_Parent = NULL);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif
