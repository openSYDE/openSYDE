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
#include "C_GiTextElementBus.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_gui
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_GiSvTextElementBus :
   public C_GiTextElementBus
{
public:
   C_GiSvTextElementBus(const int32_t & ors32_Index, const uint64_t & oru64_Id,
                        QGraphicsItem * const opc_Parent = NULL);
   void GenerateHint(void) override;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
