//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Button with  text and SVG item, drawn manually (header)

   See cpp file for detailed description

   \copyright   Copyright 2018 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_OGEPUBSVGICONWITHTEXT_HPP
#define C_OGEPUBSVGICONWITHTEXT_HPP

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "C_OgePubSvgIconWithTextBase.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_gui_elements
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_OgePubSvgIconWithText :
   public C_OgePubSvgIconWithTextBase
{
public:
   C_OgePubSvgIconWithText(QWidget * const opc_Parent = NULL);

   void HideToolTip(void);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
