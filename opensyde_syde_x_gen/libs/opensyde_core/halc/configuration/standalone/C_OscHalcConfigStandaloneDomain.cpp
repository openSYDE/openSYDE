//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Standalone HALC domain config

   Standalone HALC domain config

   \copyright   Copyright 2020 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.hpp"

#include "C_OscHalcConfigStandaloneDomain.hpp"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw::opensyde_core;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

/* -- Global Variables ---------------------------------------------------------------------------------------------- */

/* -- Module Global Variables --------------------------------------------------------------------------------------- */

/* -- Module Global Function Prototypes ----------------------------------------------------------------------------- */

/* -- Implementation ------------------------------------------------------------------------------------------------ */

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Default constructor
*/
//----------------------------------------------------------------------------------------------------------------------
C_OscHalcConfigStandaloneDomain::C_OscHalcConfigStandaloneDomain(void) :
   C_OscHalcConfigDomain()
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Constructor to use data of an instance of type C_OscHalcConfigDomain

   \param[in]   orc_ConfigDomain          Project HALC domain configuration
   \param[in]   orc_StandaloneChannels    Standalone information of all domain channels
*/
//----------------------------------------------------------------------------------------------------------------------
C_OscHalcConfigStandaloneDomain::C_OscHalcConfigStandaloneDomain(const C_OscHalcConfigDomain & orc_ConfigDomain,
                                                                 const std::vector<C_OscHalcConfigStandaloneChannel> & orc_StandaloneChannels)
   :
   C_OscHalcConfigDomain(orc_ConfigDomain),
   c_StandaloneChannels(orc_StandaloneChannels)
{
}
