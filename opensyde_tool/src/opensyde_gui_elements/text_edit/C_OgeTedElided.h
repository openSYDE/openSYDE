//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Text edit with automatic eliding (header)

   See cpp file for detailed description

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_OGETEDELIDED_H
#define C_OGETEDELIDED_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "C_OgeTedContextMenuBase.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw_opensyde_gui_elements
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_OgeTedElided :
   public C_OgeTedContextMenuBase
{
public:
   C_OgeTedElided(QWidget * const opc_Parent = NULL);

protected:
   virtual void paintEvent(QPaintEvent * const opc_Event) override;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif
