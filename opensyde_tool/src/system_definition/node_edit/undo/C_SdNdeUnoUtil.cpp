//-----------------------------------------------------------------------------
/*!
   \internal
   \file
   \brief       Storage for undo command utility functions (implementation)

   Storage for undo command utility functions

   \implementation
   project     openSYDE
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     13.02.2017  STW/M.Echtler
   \endimplementation
*/
//-----------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------- */
#include "precomp_headers.h"

#include "stwtypes.h"
#include "C_SdNdeUnoUtil.h"

/* -- Used Namespaces ------------------------------------------------------ */
using namespace stw_types;
using namespace stw_opensyde_gui_logic;

/* -- Module Global Constants ---------------------------------------------- */

/* -- Types ---------------------------------------------------------------- */

/* -- Global Variables ----------------------------------------------------- */

/* -- Module Global Variables ---------------------------------------------- */

/* -- Module Global Function Prototypes ------------------------------------ */

/* -- Implementation ------------------------------------------------------- */

//-----------------------------------------------------------------------------
/*!
   \brief   Adapt target indices to be at correct position after deletion of source indices

   \param[in]     orc_Source Source indices
   \param[in,out] orc_Target Target indices

   \created     29.05.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_SdNdeUnoUtil::h_AdaptTargetToDeletedSource(const std::vector<uint32> & orc_Source,
                                                  std::vector<uint32> & orc_Target)
{
   for (uint32 u32_ItTarget = 0; u32_ItTarget < orc_Target.size(); ++u32_ItTarget)
   {
      uint32 u32_Adaption = 0;
      for (uint32 u32_ItSource = 0; u32_ItSource < orc_Source.size(); ++u32_ItSource)
      {
         if (orc_Source[u32_ItSource] <= orc_Target[u32_ItTarget])
         {
            ++u32_Adaption;
         }
      }
      orc_Target[u32_ItTarget] -= u32_Adaption;
   }
}

//-----------------------------------------------------------------------------
/*!
   \brief   Default constructor

   \created     13.02.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
C_SdNdeUnoUtil::C_SdNdeUnoUtil()
{
}
