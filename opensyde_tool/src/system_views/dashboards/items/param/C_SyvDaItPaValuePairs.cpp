//-----------------------------------------------------------------------------
/*!
   \internal
   \file
   \brief       Utility class to model value pairs for parametrization write process (implementation)

   Utility class to model value pairs for parametrization write process

   \implementation
   project     openSYDE
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     29.10.2018  STW/M.Echtler
   \endimplementation
*/
//-----------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------- */
#include "precomp_headers.h"

#include "C_SyvDaItPaValuePairs.h"

/* -- Used Namespaces ------------------------------------------------------ */
using namespace stw_opensyde_gui_logic;

/* -- Module Global Constants ---------------------------------------------- */

/* -- Types ---------------------------------------------------------------- */

/* -- Global Variables ----------------------------------------------------- */

/* -- Module Global Variables ---------------------------------------------- */

/* -- Module Global Function Prototypes ------------------------------------ */

/* -- Implementation ------------------------------------------------------- */

//-----------------------------------------------------------------------------
/*!
   \brief   Default constructor

   Warning: this is necessary for container interface but not be called explicitly

   \created     29.10.2018  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
C_SyvDaItPaValuePairs::C_SyvDaItPaValuePairs(void) :
   q_Changed(false)
{
}

//-----------------------------------------------------------------------------
/*!
   \brief   Default constructor

   \param[in] orc_Expected Expected value
   \param[in] oq_Changed   Flag if expected value was marked as changed

   \created     29.10.2018  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
C_SyvDaItPaValuePairs::C_SyvDaItPaValuePairs(const stw_opensyde_core::C_OSCNodeDataPoolContent & orc_Expected,
                                             const bool oq_Changed) :
   q_Changed(oq_Changed),
   c_Expected(orc_Expected),
   c_Actual(orc_Expected)
{
}
