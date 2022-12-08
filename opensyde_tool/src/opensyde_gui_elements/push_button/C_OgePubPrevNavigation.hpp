//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       System definition lists previous navigation.
   \copyright   Copyright 2020 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_OGEPUBPREVNAVIGATION_H
#define C_OGEPUBPREVNAVIGATION_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "C_OgePubSvgIconWithTextBase.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_gui_elements
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_OgePubPrevNavigation :
   public C_OgePubSvgIconWithTextBase
{
   Q_OBJECT

public:
   explicit C_OgePubPrevNavigation(QWidget * const opc_Parent = NULL);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
