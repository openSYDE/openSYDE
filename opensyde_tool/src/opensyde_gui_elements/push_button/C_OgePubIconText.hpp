//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Button in material design without borders and with icon

   \copyright   Copyright 2016 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

#ifndef C_OGEPUBICONTEXT_HPP
#define C_OGEPUBICONTEXT_HPP

/* -- Includes ------------------------------------------------------------------------------------------------------ */

#include "precomp_headers.hpp"
#include "C_OgePubStandard.hpp"
#include "stwtypes.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */

namespace stw
{
namespace opensyde_gui_elements
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_OgePubIconText :
   public C_OgePubStandard
{
   Q_OBJECT

public:
   explicit C_OgePubIconText(QWidget * const opc_Parent = NULL);

   void SetIconSize(const int32_t os32_IconSize);

protected:
   void paintEvent(QPaintEvent * const opc_Event) override;

   int32_t ms32_IconSize;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
