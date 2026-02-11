//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Data logger job additional trigger expert mode properties
   \copyright   Copyright 2025 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_OSCDATALOGGERJOBADDITIONALTRIGGEREXPERTMODE_H
#define C_OSCDATALOGGERJOBADDITIONALTRIGGEREXPERTMODE_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "stwtypes.hpp"
#include "C_SclString.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_core
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_OscDataLoggerJobAdditionalTriggerExpertMode
{
public:
   C_OscDataLoggerJobAdditionalTriggerExpertMode();

   void CalcHash(uint32_t & oru32_HashValue) const;
   static void h_ReplaceUiVarNames(stw::scl::C_SclString & orc_Expression);

   bool q_Enable;                                ///< flag to enable expert mode
   stw::scl::C_SclString c_TriggerConfiguration; ///< trigger configuration, only valid if enabled flag is set
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
