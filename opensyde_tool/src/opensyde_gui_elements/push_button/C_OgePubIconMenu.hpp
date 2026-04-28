//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Button with icon only that can be used with a menu.
   \copyright   Copyright 2026 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_OGEPUBICONMENU_HPP
#define C_OGEPUBICONMENU_HPP

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "C_OgePubSvgIconWithTextBase.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_gui_elements
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_OgePubIconMenu :
   public C_OgePubSvgIconWithTextBase // base class C_OgePubSvgIconOnly does not work as it stretches image to fit
{
   Q_OBJECT

public:
   explicit C_OgePubIconMenu(QWidget * const opc_Parent = NULL);

protected:
   void paintEvent(QPaintEvent * const opc_Event) override;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} // namespace opensyde_gui_elements
} // namespace stw

#endif
