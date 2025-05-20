//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       System update package definition
   \copyright   Copyright 2024 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_OSCSUPDEFINITION_HPP
#define C_OSCSUPDEFINITION_HPP

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "C_OscSupNodeDefinition.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_core
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_OscSupDefinition
{
public:
   C_OscSupDefinition(void);

   //lint -sem(stw::opensyde_core::C_OscSupDefinition::Initialize,initializer)
   void Initialize(void);

   std::vector<C_OscSupNodeDefinition> c_Nodes;
   uint32_t u32_ActiveBusIndex;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
