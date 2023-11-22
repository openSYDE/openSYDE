//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       UI text element bus data: stores UI information (header)

   See cpp file for detailed description

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_PUISDTEXTELEMENTBUS_HPP
#define C_PUISDTEXTELEMENTBUS_HPP

/* -- Includes ------------------------------------------------------------------------------------------------------ */

#include "stwtypes.hpp"

#include "C_PuiBsTextElement.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_gui_logic
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_PuiSdTextElementBus :
   public C_PuiBsTextElement
{
public:
   C_PuiSdTextElementBus();
   ~C_PuiSdTextElementBus() override;

   void CalcHash(uint32_t & oru32_HashValue) const override;

   uint32_t u32_BusIndex;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
