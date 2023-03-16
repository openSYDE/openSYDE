//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Storage for undo command utility functions (implementation)

   Storage for undo command utility functions

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.hpp"

#include "stwtypes.hpp"
#include "C_SdNdeUnoUtil.hpp"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw::opensyde_gui_logic;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

/* -- Global Variables ---------------------------------------------------------------------------------------------- */

/* -- Module Global Variables --------------------------------------------------------------------------------------- */

/* -- Module Global Function Prototypes ----------------------------------------------------------------------------- */

/* -- Implementation ------------------------------------------------------------------------------------------------ */

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Adapt target indices to be at correct position after deletion of source indices

   \param[in]     orc_Source Source indices
   \param[in,out] orc_Target Target indices
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeUnoUtil::h_AdaptTargetToDeletedSource(const std::vector<uint32_t> & orc_Source,
                                                  std::vector<uint32_t> & orc_Target)
{
   for (uint32_t u32_ItTarget = 0; u32_ItTarget < orc_Target.size(); ++u32_ItTarget)
   {
      uint32_t u32_Adaption = 0;
      for (uint32_t u32_ItSource = 0; u32_ItSource < orc_Source.size(); ++u32_ItSource)
      {
         if (orc_Source[u32_ItSource] <= orc_Target[u32_ItTarget])
         {
            ++u32_Adaption;
         }
      }
      orc_Target[u32_ItTarget] -= u32_Adaption;
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default constructor
*/
//----------------------------------------------------------------------------------------------------------------------
C_SdNdeUnoUtil::C_SdNdeUnoUtil()
{
}
