//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       System view CAN bus (header)

   See cpp file for detailed description

   \copyright   Copyright 2018 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_GISVCANBUS_H
#define C_GISVCANBUS_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "C_GiLiCanBus.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_gui
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_GiSvCanBus :
   public C_GiLiCanBus
{
public:
   C_GiSvCanBus(const uint32_t ou32_ViewIndex, const int32_t & ors32_Index, const uint64_t & oru64_Id,
                C_GiTextElementBus * const opc_TextElementName, const std::vector<QPointF> * const opc_Points = NULL,
                QGraphicsItem * const opc_Parent = NULL);
   void CheckBusForChanges(void) override;

private:
   uint32_t mu32_ViewIndex;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
