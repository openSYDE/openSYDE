//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Text element for system view bus (header)

   See cpp file for detailed description

   \copyright   Copyright 2018 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_GISVTEXTELEMENTBUS_H
#define C_GISVTEXTELEMENTBUS_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "C_GiTextElementBus.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw_opensyde_gui
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_GiSvTextElementBus :
   public C_GiTextElementBus
{
public:
   C_GiSvTextElementBus(const stw_types::sint32 & ors32_Index, const stw_types::uint64 & oru64_ID,
                        QGraphicsItem * const opc_Parent = NULL);
   virtual void GenerateHint(void) override;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif
