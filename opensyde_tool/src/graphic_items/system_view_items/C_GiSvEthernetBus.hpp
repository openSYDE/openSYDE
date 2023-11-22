//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       System view ethernet bus (header)

   See cpp file for detailed description

   \copyright   Copyright 2018 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_GISVETHERNETBUS_HPP
#define C_GISVETHERNETBUS_HPP

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "C_GiLiEthernetBus.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_gui
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_GiSvEthernetBus :
   public C_GiLiEthernetBus
{
public:
   C_GiSvEthernetBus(const uint32_t ou32_ViewIndex, const int32_t & ors32_Index, const uint64_t & oru64_Id,
                     C_GiTextElementBus * const opc_TextElementName,
                     const std::vector<QPointF> * const opc_Points = NULL, QGraphicsItem * const opc_Parent = NULL);
   void CheckBusForChanges(void) override;

private:
   uint32_t mu32_ViewIndex;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
