//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Data logger job additional trigger properties
   \copyright   Copyright 2025 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_OSCDATALOGGERJOBADDITIONALTRIGGERPROPERTIES_HPP
#define C_OSCDATALOGGERJOBADDITIONALTRIGGERPROPERTIES_HPP

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "C_OscNodeDataPoolContent.hpp"
#include "C_OscNodeDataPoolListElementOptArrayId.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_core
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_OscDataLoggerJobAdditionalTriggerProperties
{
public:
   C_OscDataLoggerJobAdditionalTriggerProperties();

   void CalcHash(uint32_t & oru32_HashValue) const;

   bool q_Enable;                                      ///< flag to enable additional trigger
   C_OscNodeDataPoolListElementOptArrayId c_ElementId; ///< Selected element for additional trigger
   C_OscNodeDataPoolContent c_Threshold;               ///< Selected threshold for additional trigger
   stw::scl::C_SclString c_Operation;                  ///< Which operator to use for additional trigger threshold
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
