//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       System definition lists back navigation
   \copyright   Copyright 2020 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_OGEPUBBACKNAVIGATION_HPP
#define C_OGEPUBBACKNAVIGATION_HPP

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "C_OgePubSvgIconWithTextBase.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_gui_elements
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
}
} //end of namespace

#endif
