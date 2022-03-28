//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Standalone HALC domain config

   Standalone HALC domain config

   \copyright   Copyright 2020 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.h"

#include "C_OSCHalcConfigStandaloneDomain.h"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw_opensyde_core;

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
C_OSCHalcConfigStandaloneDomain::C_OSCHalcConfigStandaloneDomain(void) :
   C_OSCHalcConfigDomain()
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Constructor to use data of an instance of type C_OSCHalcConfigDomain

   \param[in]   orc_ConfigDomain          Project HALC domain configuration
   \param[in]   orc_StandaloneChannels    Standalone information of all domain channels
*/
//----------------------------------------------------------------------------------------------------------------------
C_OSCHalcConfigStandaloneDomain::C_OSCHalcConfigStandaloneDomain(const C_OSCHalcConfigDomain & orc_ConfigDomain,
                                                                 const std::vector<C_OSCHalcConfigStandaloneChannel> & orc_StandaloneChannels)
   :
   C_OSCHalcConfigDomain(orc_ConfigDomain),
   c_StandaloneChannels(orc_StandaloneChannels)
{
}
