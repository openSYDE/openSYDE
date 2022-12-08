//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Utility class to model value pairs for parametrization write process (implementation)

   Utility class to model value pairs for parametrization write process

   \copyright   Copyright 2018 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.hpp"

#include "C_SyvDaItPaValuePairs.hpp"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw::opensyde_gui_logic;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

/* -- Global Variables ---------------------------------------------------------------------------------------------- */

/* -- Module Global Variables --------------------------------------------------------------------------------------- */

/* -- Module Global Function Prototypes ----------------------------------------------------------------------------- */

/* -- Implementation ------------------------------------------------------------------------------------------------ */

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default constructor

   Warning: this is necessary for container interface but not be called explicitly
*/
//----------------------------------------------------------------------------------------------------------------------
C_SyvDaItPaValuePairs::C_SyvDaItPaValuePairs(void) :
   q_Changed(false)
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default constructor

   \param[in] orc_Expected Expected value
   \param[in] oq_Changed   Flag if expected value was marked as changed
*/
//----------------------------------------------------------------------------------------------------------------------
C_SyvDaItPaValuePairs::C_SyvDaItPaValuePairs(const stw::opensyde_core::C_OscNodeDataPoolContent & orc_Expected,
                                             const bool oq_Changed) :
   q_Changed(oq_Changed),
   c_Expected(orc_Expected),
   c_Actual(orc_Expected)
{
}
