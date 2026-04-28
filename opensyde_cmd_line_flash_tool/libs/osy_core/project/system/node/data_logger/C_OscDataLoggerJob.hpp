//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Data logger job information
   \copyright   Copyright 2024 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_OSCDATALOGGERJOB_HPP
#define C_OSCDATALOGGERJOB_HPP

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <vector>
#include "C_OscDataLoggerJobProperties.hpp"
#include "C_OscDataLoggerDataElementReference.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_core
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_OscDataLoggerJob
{
public:
   C_OscDataLoggerJob();

   void CalcHash(uint32_t & oru32_HashValue) const;

   static bool hq_AllowDataloggerFeature;                                     ///< Flag to enable/disable data logger
                                                                              // feature
   bool q_IsEnabled;                                                          ///< Flag to enable or disable logging
                                                                              // configuration
   C_OscDataLoggerJobProperties c_Properties;                                 ///< all configured properties
   std::vector<C_OscDataLoggerDataElementReference> c_ConfiguredDataElements; ///< selected data elements to log
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
